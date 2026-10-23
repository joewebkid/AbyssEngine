#include <cassert>
#include <cmath>
#include <cstdint>

struct Vector {
    float x, y, z;
};
Vector operator*(Vector v, float factor) { return {v.x * factor, v.y * factor, v.z * factor}; }
namespace AbyssEngine {
namespace AEMath {
Vector VectorNormalize(Vector v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / length, v.y / length, v.z / length};
}
}
struct Transform {
    int calls = 0, lastDt = 0;
    bool lastFlag = false;
    void Update(int dt, bool flag) { ++calls; lastDt = dt; lastFlag = flag; }
};
}
struct Canvas {
    AbyssEngine::Transform transforms[5];
    void *TransformGetTransform(unsigned id) { return &transforms[id]; }
};
struct Globals { static Canvas *Canvas; };
Canvas canvas;
Canvas *Globals::Canvas = &canvas;

struct AEGeometry {
    unsigned transform = 0;
    Vector position = {0, 0, 0}, direction = {0, 0, 1};
    Vector up = {0, 1, 0};
    float scale = 0;
    void setScaling(float x, float y, float z) { assert(x == y && y == z); scale = x; }
    void setDirection(Vector dir, Vector newUp) { direction = dir; up = newUp; }
    Vector getDirection() { return direction; }
    void setPosition(Vector newPosition) { position = newPosition; }
    Vector getPosition() { return position; }
    void translate(Vector delta) {
        position.x += delta.x; position.y += delta.y; position.z += delta.z;
    }
};
struct Item {};
struct Ship {
    int index = 0;
    bool hasEquipment = false;
    Item equipment;
    Item *getFirstEquipmentOfSort(int sort) { assert(sort == 35); return hasEquipment ? &equipment : nullptr; }
    int getIndex() { return index; }
};
struct Status {
    Ship ship;
    Ship *getShip() { return &ship; }
};
Status status;
Status *PE_status() { return &status; }
struct StarSystem {
    Vector getLightDirection() { return {3, 0, 4}; }
};
struct Level {
    StarSystem star;
    StarSystem *getStarSystem() { return &star; }
};

struct PlayerEgo {
    int throttleStarted = 0, throttle = 0;
    bool turretMode = false, autoTurretEquipped = false, turretActive = false;
    void *turretGeometry = nullptr, *field_0x30 = nullptr, *gunYawGeo = nullptr;
    uint32_t gunMuzzleRoot = 0;
    float gunEffectScale = 1.0f;
    AEGeometry *geometry, *field_0x4;
    Level *level;
    int shakeIntensity = 3;
    void updateTransforms(int dt) {
        // PRODUCTION_BLOCK
    }
};

int main() {
    AEGeometry body, root, turret, extra, effect;
    body.position = {10, 20, 30};
    root.transform = 0;
    turret.transform = 1;
    extra.transform = 2;
    effect.transform = 3;
    Level level;
    PlayerEgo ego;
    ego.geometry = &body;
    ego.field_0x4 = &root;
    ego.level = &level;

    ego.updateTransforms(1);
    assert(canvas.transforms[0].calls == 1);
    assert(canvas.transforms[1].calls == 0 && canvas.transforms[3].calls == 0);

    ego.throttleStarted = 1;
    ego.throttle = 1998;
    ego.updateTransforms(1);
    assert(ego.throttle == 1999 && ego.throttleStarted);
    ego.updateTransforms(1);
    assert(ego.throttle == 2000 && !ego.throttleStarted);

    ego.turretMode = true;
    ego.turretActive = true;
    ego.turretGeometry = &turret;
    ego.field_0x30 = &extra;
    status.ship.hasEquipment = true;
    ego.updateTransforms(5);
    assert(canvas.transforms[1].calls == 1 && canvas.transforms[1].lastFlag);
    assert(canvas.transforms[2].calls == 1 && !canvas.transforms[2].lastFlag);
    ego.autoTurretEquipped = true;
    ego.updateTransforms(5);
    assert(canvas.transforms[1].calls == 1 && canvas.transforms[2].calls == 1);
    ego.autoTurretEquipped = false;
    status.ship.hasEquipment = false;
    ego.updateTransforms(5);
    assert(canvas.transforms[1].calls == 1 && canvas.transforms[2].calls == 1);

    ego.gunYawGeo = &effect;
    ego.gunMuzzleRoot = 1;
    ego.gunEffectScale = 2.5f;
    status.ship.index = 8;
    body.direction = {0, 0, 2};
    ego.updateTransforms(7);
    assert(effect.scale == 2.5f);
    assert(std::fabs(effect.direction.x - 0.6f) < 0.0001f);
    assert(std::fabs(effect.direction.z - 0.8f) < 0.0001f);
    assert(effect.up.y == 1.0f && effect.position.z == 330.0f);
    assert(canvas.transforms[3].calls == 1 && canvas.transforms[3].lastDt == 3);
    assert(canvas.transforms[0].calls == 7 && canvas.transforms[0].lastDt == 7);

    status.ship.index = 7;
    ego.updateTransforms(2);
    assert(effect.position.z == 30.0f);
}
