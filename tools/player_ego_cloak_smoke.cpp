#include <cassert>
#include <cmath>
#include <cstdint>
#include <utility>
#include <vector>

static std::vector<int> transitionOrder;

namespace AbyssEngine {
namespace AEMath { struct Vector {}; }
struct Material { int pad[8] = {}; int materialMode = 0; };
struct Mesh { unsigned id = 0; };
struct Transform {
    bool visible = true;
    void SetVisible(bool value) { visible = value; }
};
}
using Vector = AbyssEngine::AEMath::Vector;

struct FModSound {
    int played = 0;
    void play(int id, Vector *, Vector *, float volume) {
        assert(id == 30 && volume == 0.0f);
        transitionOrder.push_back(300);
        played++;
    }
};
struct PaintCanvas {
    AbyssEngine::Material materials[32];
    AbyssEngine::Mesh meshes[256];
    AbyssEngine::Transform transforms[256];
    std::vector<std::pair<unsigned, unsigned>> materialChanges;
    std::vector<std::pair<unsigned, unsigned>> shaderChanges;
    std::vector<float> shaderValues;
    std::vector<std::pair<unsigned, unsigned>> colorChanges;
    std::vector<std::pair<unsigned, unsigned>> resourceChanges;
    unsigned createdResource = 0;
    void *MaterialGetMaterial(unsigned id) {
        assert(id < 32);
        return reinterpret_cast<unsigned *>(&materials[id]);
    }
    void MeshChangeMaterial(unsigned mesh, unsigned short material) {
        materialChanges.emplace_back(mesh, material);
    }
    AbyssEngine::Mesh *MeshGetPointer(unsigned mesh) {
        assert(mesh < 256);
        meshes[mesh].id = mesh;
        return &meshes[mesh];
    }
    void MeshChangeShaderAnimValue(AbyssEngine::Mesh *mesh, float value, unsigned channel) {
        shaderChanges.emplace_back(mesh->id, channel);
        shaderValues.push_back(value);
    }
    void TransformSetColor(unsigned transform, unsigned color) {
        colorChanges.emplace_back(transform, color);
    }
    AbyssEngine::Transform *TransformGetTransform(unsigned transform) {
        assert(transform < 256);
        return &transforms[transform];
    }
    void MaterialCreate(unsigned short resource, unsigned &out) {
        assert(out == 0);
        createdResource = resource;
        out = 999;
    }
    void MeshChangeResourceMaterial(unsigned mesh, unsigned short resource) {
        resourceChanges.emplace_back(mesh, resource);
    }
};
struct Globals {
    static FModSound *sound;
    static PaintCanvas *Canvas;
};
FModSound *Globals::sound = nullptr;
PaintCanvas *Globals::Canvas = nullptr;

struct Item {
    int index = 0, amount = 0, attribute = 0, lastAttribute = -1;
    int getAttribute(int id) { lastAttribute = id; return attribute; }
    int getAmount() { return amount; }
    int getIndex() { return index; }
};
struct Ship {
    Item *cargo = nullptr, *sort8 = nullptr, *sort35 = nullptr;
    int removedIndex = -1, removedAmount = -1;
    Item *getCargo(int id) { assert(id == 122); return cargo; }
    void removeCargo(int id, int amount) { removedIndex = id; removedAmount = amount; }
    Item *getFirstEquipmentOfSort(int sort) {
        assert(sort == 8 || sort == 35);
        return sort == 8 ? sort8 : sort35;
    }
};
struct Status { Ship *ship; int64_t field_c0_q = 0; Ship *getShip() { return ship; } };
Status *currentStatus = nullptr;
Status *PE_status() { return currentStatus; }
struct Hud {
    std::vector<std::pair<int, int>> events;
    void hudEvent(int id, void *, int arg) {
        events.emplace_back(id, arg);
        transitionOrder.push_back(id);
    }
};
struct Player { uint8_t field_5e = 0; };
struct AEGeometry {
    unsigned meshId;
    unsigned transform = 0;
    unsigned parentTransform = static_cast<unsigned>(-1);
    unsigned childTransform = static_cast<unsigned>(-1);
};

struct PlayerEgo {
    uint8_t chargingCloak = 0, cloaked = 0;
    int cloakRechargeTimer = 0, cloakRechargeMax = 0, cloakCharge = 0;
    int cloakDischargeMax = 0, cloakChargeMax = 0;
    void *cloak = nullptr, *player = nullptr, *hud = nullptr;
    AEGeometry *field_0x4 = nullptr;
    void *rollGeometry = nullptr, *turretGeometry = nullptr;
    int cloakMaterial1 = 0, cloakMaterial2 = 0, cloakMaterial3 = 0;
    int originalMaterialMode = 0;
    uint8_t turretMode = 0;
    uint8_t cloakChildHidden = 0;
    int toggleCloaking();
    float getCloakingPercentage();
    void updateCloakCharge(int dt, Hud *hud);
};

// PRODUCTION_PERCENTAGE
// PRODUCTION_METHOD

void PlayerEgo::updateCloakCharge(int dt, Hud *hud) {
// PRODUCTION_UPDATE_FRAGMENT
}

int main() {
    FModSound sound;
    PaintCanvas canvas;
    Globals::sound = &sound;
    Globals::Canvas = &canvas;
    Ship ship;
    Status status{&ship};
    currentStatus = &status;
    Item cloak;
    cloak.attribute = 4;
    Item cargo;
    cargo.amount = 3;
    ship.cargo = &cargo;
    Hud hud;
    Player player;
    AEGeometry hull{40}, base{50}, yaw{60};
    hull.parentTransform = 70;
    hull.childTransform = 71;
    base.transform = 72;
    yaw.transform = 73;
    PlayerEgo ego;
    ego.cloak = &cloak;
    ego.player = &player;
    ego.hud = &hud;
    ego.field_0x4 = &hull;
    ego.rollGeometry = &base;
    ego.turretGeometry = &yaw;
    ego.cloakMaterial1 = 11;
    ego.cloakMaterial2 = 12;
    ego.cloakMaterial3 = 13;
    ego.cloakDischargeMax = 8;
    ego.cloakChargeMax = 6000;
    ego.cloakRechargeMax = 2500;
    ego.originalMaterialMode = 7;

    assert(ego.toggleCloaking() == 0);
    assert(cloak.lastAttribute == 38 && ship.removedIndex == -1);
    cargo.amount = 10;
    assert(ego.toggleCloaking() == 1);
    assert(ship.removedIndex == 122 && ship.removedAmount == 4);
    assert(ego.chargingCloak == 1);
    assert((hud.events == std::vector<std::pair<int, int>>{{30, 4}, {28, 0}}));

    assert(ego.toggleCloaking() == 0 && sound.played == 0);
    ego.updateCloakCharge(8, &hud);
    assert(ego.cloakCharge == 8 && ego.chargingCloak == 1 && sound.played == 0);
    ego.updateCloakCharge(1, &hud);
    assert((hud.events == std::vector<std::pair<int, int>>{{30, 4}, {28, 0}, {29, 0}}));
    assert((transitionOrder == std::vector<int>{30, 28, 29, 300}));
    assert(sound.played == 1 && player.field_5e == 1);
    assert(ego.cloakCharge == 0 && ego.cloaked == 1 && ego.chargingCloak == 0);
    assert(canvas.materials[11].materialMode == 14);
    assert((canvas.materialChanges == std::vector<std::pair<unsigned, unsigned>>{{40, 11}}));
    assert((canvas.shaderChanges == std::vector<std::pair<unsigned, unsigned>>{{40, 1}, {40, 2}}));

    ego.updateCloakCharge(500, &hud);
    assert(ego.cloakCharge == 500 && status.field_c0_q == 500);
    assert(ego.cloakChildHidden == 0 && canvas.transforms[71].visible);
    assert(canvas.shaderChanges.back() == std::make_pair(40u, 2u));
    assert(std::fabs(canvas.shaderValues.back() - 0.5f) < 0.0001f);
    assert(canvas.colorChanges.back() == std::make_pair(71u, 0xffffffc0u));
    ego.updateCloakCharge(1, &hud);
    assert(ego.cloakChildHidden == 1 && !canvas.transforms[71].visible);
    ego.updateCloakCharge(4999, &hud);
    assert(ego.cloakCharge == 5500 && ego.cloakChildHidden == 1);
    ego.updateCloakCharge(1, &hud);
    assert(ego.cloakCharge == 5501 && ego.cloakChildHidden == 0);
    assert(canvas.transforms[71].visible);
    ego.updateCloakCharge(500, &hud);
    assert(ego.cloaked == 0 && ego.cloakCharge == 0);
    assert(ego.cloakRechargeTimer == 2500 && status.field_c0_q == 6001);
    assert(player.field_5e == 0 && sound.played == 2);
    assert(canvas.materials[11].materialMode == 7);
    assert(hud.events.size() == 3);

    canvas.materialChanges.clear();
    canvas.shaderChanges.clear();
    canvas.shaderValues.clear();
    ego.cloakCharge = 8;
    ego.chargingCloak = 1;
    ego.turretMode = 1;
    Item equipment8;
    equipment8.index = 224;
    ship.sort8 = &equipment8;
    Item equipment35;
    equipment35.index = 199;
    ship.sort35 = &equipment35;
    assert(ego.toggleCloaking() == 1);
    assert(canvas.materials[12].materialMode == 14 && canvas.materials[13].materialMode == 14);
    assert((canvas.materialChanges == std::vector<std::pair<unsigned, unsigned>>{{40, 11}, {50, 12}, {60, 13}}));
    assert((canvas.shaderChanges == std::vector<std::pair<unsigned, unsigned>>{{40, 1}, {40, 2}, {50, 1}, {50, 2}, {60, 1}, {60, 2}}));
    assert(canvas.createdResource == 0x7167);
    assert((canvas.resourceChanges == std::vector<std::pair<unsigned, unsigned>>{{50, 0x7167}, {60, 0x7167}}));
    ego.updateCloakCharge(6001, &hud);
    assert(ego.cloaked == 0 && ego.cloakRechargeTimer == 2500);
    assert(status.field_c0_q == 12002 && sound.played == 4);
    assert(canvas.materials[11].materialMode == 7);
    assert(canvas.materials[12].materialMode == 7 && canvas.materials[13].materialMode == 7);
    assert((canvas.resourceChanges == std::vector<std::pair<unsigned, unsigned>>{
        {50, 0x7167}, {60, 0x7167}, {50, 29029}, {60, 29029}}));
    assert(canvas.colorChanges[canvas.colorChanges.size() - 1].first == 73);
}
