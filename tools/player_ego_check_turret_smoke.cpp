#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>

namespace AbyssEngine {
enum AnimationMode { NONE, ONE, TWO };
namespace AEMath {
enum RotationOrder { ROTATION_ORDER_YXZ = 2 };
struct Vector { float x, y, z; };
}
struct Transform {
    float boundingRadius = 0;
    bool visible = false;
    int animationMode = -1;
    void SetVisible(bool value) { visible = value; }
    void SetAnimationState(AnimationMode mode, void *) { animationMode = mode; }
};
}
using Vec3 = AbyssEngine::AEMath::Vector;

struct PaintCanvas {
    AbyssEngine::Transform transforms[32];
    std::vector<unsigned> clonedMeshes;
    AbyssEngine::Transform *TransformGetTransform(unsigned handle) {
        assert(handle > 0 && handle < 32);
        return &transforms[handle];
    }
    void MeshCloneMaterial(unsigned mesh, uint32_t &out) {
        clonedMeshes.push_back(mesh);
        out = mesh + 1000;
    }
};
struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct AEGeometry {
    static std::vector<AEGeometry *> created;
    unsigned transform;
    unsigned meshId;
    bool deleted = false;
    std::vector<unsigned> children;
    AbyssEngine::AEMath::Vector position = {};
    AbyssEngine::AEMath::Vector translation = {};
    AbyssEngine::AEMath::Vector rotation = {};
    int order = -1;
    bool visible = false;
    AEGeometry(uint16_t mesh, PaintCanvas *, bool) : transform(created.size() + 1), meshId(mesh) {
        created.push_back(this);
    }
    explicit AEGeometry(PaintCanvas *) : AEGeometry(0, nullptr, false) {}
    void operator delete(void *) noexcept {} // Keep child records inspectable after production delete.
    ~AEGeometry() { deleted = true; }
    void setRotationOrder(AbyssEngine::AEMath::RotationOrder value) { order = value; }
    void addChild(unsigned handle) { children.push_back(handle); }
    void setPosition(const AbyssEngine::AEMath::Vector &value) { position = value; }
    void translate(float x, float y, float z) { translation = {x, y, z}; }
    void rotate(float x, float y, float z) { rotation = {x, y, z}; }
    void setVisible(bool value) { visible = value; }
};
std::vector<AEGeometry *> AEGeometry::created;

struct Gun {};
template <typename T> struct Array { T *data_; };
struct Player {
    int available = 0;
    int queriedSort = -1;
    int added = 0;
    int gunAvailable(int sort) { queriedSort = sort; return available; }
    void addGun(Gun *, int) { added++; }
    void addGun(Array<Gun *> *, int) { added++; }
};
struct Item {
    int index = 0;
    int attribute = 0;
    int queriedAttribute = -1;
    int getIndex() { return index; }
    int getAttribute(int attr) { queriedAttribute = attr; return attribute; }
};
struct Ship {
    Array<Item *> equipment;
    int queriedSort = -1;
    Array<Item *> *getEquipment(int sort) { queriedSort = sort; return &equipment; }
};
struct Status { Ship *ship; Ship *getShip() { return ship; } };
Status *currentStatus = nullptr;
Status *PE_status() { return currentStatus; }

struct PlayerEgo {
    void *player = nullptr;
    uint8_t turretMode = 0;
    uint8_t autoTurretEquipped = 0;
    uint8_t turretActive = 0;
    int turretPitch = 0;
    void *rollGeometry = nullptr;
    void *turretGeometry = nullptr;
    void *field_0x2c = nullptr;
    void *field_0x30 = nullptr;
    void *cloak = nullptr;
    int cloakMaterial2 = 0;
    int cloakMaterial3 = 0;
    AbyssEngine::AEMath::Vector turretOffsetVec = {3, 4, 5};
    float rotY = 0, rotZ = 0, waypointX = 0;
    AbyssEngine::AEMath::Vector lastPosition = {};
    int speed = 0;
    uint8_t freeze = 0, field_0x145 = 0, field_0x309 = 0;
    short docked = 0;
    AEGeometry *geometry = nullptr;
    bool autoPilotCleared = false;
    void checkForTurret();
    void addGun(Gun *, int);
    void addGun(Array<Gun *> *, int);
    void dockToStream(bool);
    void setPosition(float x, float y, float z) { lastPosition = {x, y, z}; }
    void setPosition(Vec3 v) { lastPosition = v; }
    void setAutoPilot(void *value) { assert(value == nullptr); autoPilotCleared = true; }
};
static const float g_PE_cft_transformVal = 1000000.0f;

// PRODUCTION_METHOD
// PRODUCTION_CALLERS
// PRODUCTION_DOCK

static void runCase(int index, bool available, bool cloak, bool active) {
    AEGeometry::created.clear();
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    Player player;
    player.available = available;
    Item item;
    item.index = index;
    item.attribute = 7;
    Item *items[] = {&item};
    Ship ship;
    ship.equipment.data_ = items;
    Status status = {&ship};
    currentStatus = &status;
    PlayerEgo ego;
    ego.player = &player;
    ego.turretActive = active;
    ego.cloak = cloak ? &ego : nullptr;
    Gun gun;
    if (index == 0xb5) {
        ego.addGun(&gun, 1);
    } else {
        Gun *guns[] = {&gun};
        Array<Gun *> arr = {guns};
        ego.addGun(&arr, 2);
    }
    assert(player.queriedSort == 2);
    assert(player.added == 1);
    if (!available) {
        assert(AEGeometry::created.empty());
        assert(ship.queriedSort == -1);
        return;
    }
    assert(ship.queriedSort == 2 && item.queriedAttribute == 17);
    assert(ego.turretPitch == 10 && ego.turretMode == 1);
    AEGeometry *base = static_cast<AEGeometry *>(ego.rollGeometry);
    AEGeometry *yaw = static_cast<AEGeometry *>(ego.turretGeometry);
    AEGeometry *root = static_cast<AEGeometry *>(ego.field_0x2c);
    assert(base && yaw && root && yaw->order == AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
    assert(root->children.size() == 2 && root->children[0] == base->transform
           && root->children[1] == yaw->transform);
    assert(base->position.x == 3 && yaw->position.z == 5);
    assert(canvas.transforms[root->transform].boundingRadius == 1000000.0f);
    assert(canvas.transforms[yaw->transform].animationMode == 2);
    assert(canvas.clonedMeshes.size() == (cloak ? 2u : 0u));
    if (cloak) {
        assert(canvas.clonedMeshes[0] == base->meshId && canvas.clonedMeshes[1] == yaw->meshId);
        assert(ego.cloakMaterial2 == int(base->meshId + 1000));
        assert(ego.cloakMaterial3 == int(yaw->meshId + 1000));
    }
    if (index == 0xb5) {
        assert(ego.autoTurretEquipped == 1);
        assert(base->meshId == 0x1a97 && yaw->meshId == 0x1a98);
        assert(yaw->translation.x == 0 && yaw->translation.y == 88 && yaw->translation.z == -11);
        assert(base->rotation.y == 3.1416f && yaw->rotation.y == 3.1416f);
        assert(ego.field_0x30 == nullptr);
    } else if (index == 0xb6) {
        assert(ego.autoTurretEquipped == 1);
        assert(yaw->translation.x == 34 && yaw->translation.y == 88
               && std::fabs(yaw->translation.z - 1.6f) < 0.0001f);
        assert(base->rotation.y == 3.1416f);
    } else if (index == 0xe0) {
        assert(base->meshId == 0x499a && yaw->meshId == 0x499b);
        assert(yaw->translation.y == 82);
        assert(base->children.size() == 1 && yaw->children.size() == 1);
        assert(AEGeometry::created[3]->deleted && AEGeometry::created[4]->deleted);
    } else if (index == 0xc8) {
        assert(ego.autoTurretEquipped == 0);
        assert(base->meshId == 0x496c && yaw->meshId == 0x496d);
        assert(yaw->translation.y == 75 && yaw->rotation.y == 0);
        assert(base->children.size() == 1 && yaw->children.size() == 3);
        assert(AEGeometry::created[3]->deleted && AEGeometry::created[4]->deleted
               && AEGeometry::created[5]->deleted);
        auto *extra = static_cast<AEGeometry *>(ego.field_0x30);
        assert(extra && extra == AEGeometry::created[6]);
        assert(canvas.transforms[extra->transform].visible == active);
    }
    ego.checkForTurret();
    assert(AEGeometry::created.size() == (index == 0xc8 ? 7u : index == 0xe0 ? 5u : 3u));
    ego.rotY = 12; ego.rotZ = 13; ego.waypointX = 14;
    ego.dockToStream(false);
    assert(ego.lastPosition.x == 12 && ego.lastPosition.y == 13 && ego.lastPosition.z == 14);
    assert(ego.rollGeometry == base && ego.speed == 0x40000000 && ego.docked == 0);
    assert(ego.autoPilotCleared);
}

int main() {
    runCase(0xb5, false, false, false);
    runCase(0xb5, true, true, false);
    runCase(0xb6, true, false, false);
    runCase(0xe0, true, false, false);
    runCase(0xc8, true, true, true);
    runCase(0xc8, true, false, false);
}
