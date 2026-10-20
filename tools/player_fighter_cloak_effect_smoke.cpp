#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>

static std::vector<std::string> events;

namespace AbyssEngine {
struct Transform {
    bool visible = true;
    void SetVisible(bool value) { visible = value; events.push_back(value ? "visible" : "hidden"); }
};
struct Material { int materialMode = 5; };
struct Mesh { void *material = nullptr; };
}

struct AEGeometry { int childTransform = -1; unsigned meshId = 7; };
struct Player { bool empDisabledByte = false; };
struct AERandom {
    std::vector<int> values;
    std::vector<int> bounds;
    int nextInt(int bound) {
        bounds.push_back(bound);
        assert(!values.empty());
        int value = values.front();
        values.erase(values.begin());
        return value;
    }
};
struct PaintCanvas {
    AbyssEngine::Transform transform;
    AbyssEngine::Material material;
    AbyssEngine::Mesh mesh;
    float alpha = -1.0f;
    int transformId = -1;
    void *TransformGetTransform(unsigned id) {
        transformId = static_cast<int>(id);
        events.push_back("transform");
        return &transform;
    }
    void MeshCloneMaterial(unsigned id, unsigned &out) {
        assert(id == 7);
        events.push_back("clone");
        out = 9;
    }
    AbyssEngine::Mesh *MeshGetPointer(unsigned id) {
        assert(id == 7);
        events.push_back("mesh");
        return &mesh;
    }
    void *MaterialGetMaterial(unsigned id) {
        assert(id == 9);
        events.push_back("material");
        return &material;
    }
    void MeshChangeMaterial(unsigned id, unsigned short materialId) {
        assert(id == 7 && materialId == 9);
        events.push_back("change-material");
    }
    void MeshChangeShaderAnimValue(AbyssEngine::Mesh *value, float opacity, unsigned mode) {
        assert(value == &mesh && mode == 1);
        alpha = opacity;
        events.push_back("shader");
    }
};
struct Globals {
    static PaintCanvas *Canvas;
    static AERandom *rnd;
};
PaintCanvas *Globals::Canvas = nullptr;
AERandom *Globals::rnd = nullptr;

struct PlayerFighter {
    AEGeometry *geometry = nullptr;
    AEGeometry *subGeometry = nullptr;
    Player *player = nullptr;
    int shipGroup = 10;
    bool dead = false;
    bool cloakingPossible = true;
    bool cloakActive = false;
    int cloakTimer = 0;
    int cloakDuration = 6000;
    int deltaTime = 0;
    int cloakCooldown = 0;
    int field_0x1e0 = 0;
    int field_0x13c = 0;
    int field_0x2d9 = 0;
    int exhaustHidden = 0;
    unsigned cloakMaterial = 0xffffffff;
    int cloakSavedMode = 0;
    bool isDead() { return dead; }
    void setExhaustVisible(bool visible);
    void cloak(int duration, bool flag);
    void handleCloaking();
};

// PRODUCTION_METHODS

static void expect(std::initializer_list<std::string> wanted) {
    if (events != std::vector<std::string>(wanted)) {
        std::fprintf(stderr, "actual:");
        for (const auto &event : events) std::fprintf(stderr, " %s", event.c_str());
        std::fprintf(stderr, "\n");
    }
    assert(events == std::vector<std::string>(wanted));
    events.clear();
}
static void near(float actual, float wanted) { assert(std::fabs(actual - wanted) < 0.0001f); }

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    canvas.mesh.material = &canvas.material;
    AERandom random;
    Globals::rnd = &random;
    AEGeometry body{4, 7}, sub{3, 7};
    Player player;
    PlayerFighter fighter;
    fighter.player = &player;

    fighter.setExhaustVisible(false);
    expect({});
    fighter.geometry = &body;
    fighter.setExhaustVisible(false);
    expect({"transform", "hidden"});
    assert(canvas.transformId == 4);
    fighter.subGeometry = &sub;
    fighter.setExhaustVisible(true);
    expect({"transform", "visible"});
    assert(canvas.transformId == 3);
    sub.childTransform = -1;
    fighter.setExhaustVisible(false);
    expect({});
    sub.childTransform = 3;

    fighter.shipGroup = 9;
    fighter.handleCloaking();
    fighter.shipGroup = 10;
    fighter.geometry = nullptr;
    fighter.handleCloaking();
    fighter.geometry = &body;
    fighter.dead = true;
    fighter.handleCloaking();
    fighter.dead = false;
    player.empDisabledByte = true;
    fighter.handleCloaking();
    player.empDisabledByte = false;
    fighter.cloakingPossible = false;
    fighter.handleCloaking();
    fighter.cloakingPossible = true;
    expect({});

    fighter.cloakActive = true;
    fighter.deltaTime = 1000;
    fighter.handleCloaking();
    expect({"clone", "mesh", "material", "change-material", "mesh", "shader"});
    assert(fighter.cloakMaterial == 9 && fighter.cloakSavedMode == 5);
    assert(canvas.material.materialMode == 14 && fighter.cloakTimer == 1000);
    near(canvas.alpha, 0.5f);

    fighter.handleCloaking();
    expect({"transform", "hidden", "mesh", "shader"});
    assert(fighter.cloakTimer == 2000 && fighter.exhaustHidden == 1);
    near(canvas.alpha, 1.0f);

    fighter.handleCloaking();
    expect({"transform", "hidden", "mesh", "shader"});
    assert(fighter.cloakTimer == 3000);
    near(canvas.alpha, 1.5f);
    fighter.deltaTime = 2000;
    fighter.handleCloaking();
    expect({"mesh", "shader"});
    assert(fighter.exhaustHidden == 0);
    near(canvas.alpha, 0.5f);

    fighter.handleCloaking();
    expect({"material", "transform", "visible"});
    assert(!fighter.cloakActive && fighter.cloakTimer == 0 && fighter.field_0x13c == 0);
    assert(canvas.material.materialMode == 5 && canvas.transform.visible);

    fighter.cloakActive = true;
    fighter.cloakTimer = 0;
    fighter.field_0x2d9 = 1;
    fighter.deltaTime = 0;
    fighter.handleCloaking();
    expect({"material", "change-material", "transform", "hidden", "mesh", "shader"});
    assert(fighter.field_0x2d9 == 0 && fighter.cloakTimer == 2000);
    near(canvas.alpha, 1.0f);

    fighter.cloakActive = false;
    fighter.field_0x1e0 = 1;
    random.values = {49, 123};
    fighter.handleCloaking();
    expect({});
    assert((random.bounds == std::vector<int>{100, 5000}));
    assert(fighter.cloakActive && fighter.cloakDuration == 9123);

    fighter.cloakActive = false;
    fighter.field_0x1e0 = 0;
    fighter.cloakCooldown = 7900;
    fighter.deltaTime = 101;
    random.values = {30};
    fighter.handleCloaking();
    expect({});
    assert(!fighter.cloakActive && fighter.cloakCooldown == 0);

    fighter.cloakCooldown = 7900;
    random.values = {29, 321};
    fighter.handleCloaking();
    expect({});
    assert(fighter.cloakCooldown == 0 && fighter.cloakDuration == 9321);
    assert((random.bounds == std::vector<int>{100, 5000, 100, 100, 5000}));
}
