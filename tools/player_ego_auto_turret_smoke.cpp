#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>

namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
struct Matrix { Vector position{}, direction{0.0f, 0.0f, 1.0f}; };
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
Vector operator+(const Vector &a, const Vector &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
Vector operator*(const Vector &v, float scale) {
    return {v.x * scale, v.y * scale, v.z * scale};
}
Matrix operator*(const Matrix &a, const Matrix &) { return a; }
float VectorLength(const Vector &v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
Vector VectorNormalize(const Vector &v) {
    float length = VectorLength(v);
    return length == 0.0f ? Vector{} : Vector{v.x / length, v.y / length, v.z / length};
}
Vector MatrixGetDir(const Matrix &m) { return m.direction; }
Vector MatrixInverseTransformVector(const Matrix &m, const Vector &v) {
    return v - m.position;
}
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::Matrix;
template<class T> using Array = std::vector<T>;

namespace AbyssEngine {
struct Transform {
    int updates = 0;
    void Update(long long dt, bool bounds) { assert(dt > 0 && !bounds); ++updates; }
};
struct PaintCanvas {
    Transform turret, roll;
    void *TransformGetTransform(unsigned int id) {
        assert(id == 11 || id == 12);
        return id == 11 ? &turret : &roll;
    }
};
}
struct Globals { static AbyssEngine::PaintCanvas *Canvas; };
AbyssEngine::PaintCanvas *Globals::Canvas = nullptr;

struct AEGeometry {
    unsigned int transform = 0;
    Matrix matrix{};
    int yawRotations = 0, pitchRotations = 0;
    float lastYaw = 0.0f, lastPitch = 0.0f;
    Matrix &getMatrix() { return matrix; }
    void rotate(float x, float y, float z) {
        assert(z == 0.0f);
        if (x != 0.0f) { ++pitchRotations; lastPitch = x; }
        if (y != 0.0f) { ++yawRotations; lastYaw = y; }
    }
};
struct Player {
    bool active = true;
    int stops = 0;
    int shots = 0;
    Matrix transformMatrix{};
    bool isActive() { return active; }
    void stopShooting(int weapon) { assert(weapon == 2); ++stops; }
    bool shoot(int weapon, long long dt, bool flag, Matrix) {
        assert(weapon == 2 && dt == 3001 && !flag);
        ++shots;
        return true;
    }
};
struct KIPlayer {
    Player *player;
    explicit KIPlayer(Player *owner) : player(owner) {}
    AEGeometry *geometry = nullptr;
    bool dead = false, dying = false, enemy = true;
    unsigned char noTargetFlag = 0;
    Vector position{};
    int positionCalls = 0;
    bool isDead() { return dead; }
    bool isDying() { return dying; }
    bool isEnemy() { return enemy; }
    virtual Vector getPosition() { ++positionCalls; return position; }
};
struct Level {
    Array<KIPlayer *> enemies;
    Array<KIPlayer *> *getEnemies() { return &enemies; }
};
struct PlayerEgo {
    int autoTurretTimer = 0;
    void *autoTurretTarget = nullptr, *autoTurretPrevTarget = nullptr;
    void *level = nullptr, *player = nullptr;
    AEGeometry *geometry = nullptr;
    void *turretGeometry = nullptr, *rollGeometry = nullptr;
    Vector dockOffsetVec{};
    int autoTurretFireTimer = 0;
    float waypointY = 0.0f;
    uint32_t gunBaseGeo = 0;
    int pitchAccumDir = 0;
    int turretPitch = 100;
    int shakeIntensity = 16;
    float lookYaw = 0.0f;
    void handleAutoTurret(int dt);
};

// PRODUCTION_METHOD

int main() {
    Player self, other;
    AEGeometry ship, turret, roll;
    turret.transform = 11;
    roll.transform = 12;
    AbyssEngine::PaintCanvas canvas;
    Globals::Canvas = &canvas;
    Level level;
    KIPlayer dead{&other}, inactive{&other}, friendly{&other};
    KIPlayer blocked{&other}, far{&other}, nearest{&other};
    dead.dead = true;
    inactive.player->active = false;
    // Each candidate needs its own Player to test the active guard.
    Player active;
    friendly.player = &active;
    blocked.player = &active;
    far.player = &active;
    nearest.player = &active;
    friendly.enemy = false;
    blocked.noTargetFlag = 1;
    far.position = {0.0f, 0.0f, 50.0f};
    nearest.position = {0.0f, 0.0f, 25.0f};
    level.enemies = {&dead, &inactive, &friendly, &blocked, &far, &nearest};

    PlayerEgo ego;
    ego.level = &level;
    ego.player = &self;
    ego.geometry = &ship;
    ego.turretGeometry = &turret;
    ego.rollGeometry = &roll;
    ego.dockOffsetVec = {0.0f, 0.0f, 0.0f};
    ego.handleAutoTurret(3001);
    assert(ego.autoTurretTarget == &nearest && ego.autoTurretTimer == 0);
    assert(far.positionCalls == 1 && nearest.positionCalls == 1);
    assert(dead.positionCalls == 0 && inactive.positionCalls == 0);
    assert(friendly.positionCalls == 0 && blocked.positionCalls == 0);
    assert(self.stops == 1 && self.shots == 0);

    nearest.position = {0.0f, 0.0f, 60000.0f};
    far.position = {0.0f, 0.0f, 60001.0f};
    ego.handleAutoTurret(3001);
    assert(ego.autoTurretTarget == nullptr && self.stops == 2);

    nearest.position = {0.0f, 0.0f, 25.0f};
    nearest.geometry = &turret;
    ego.handleAutoTurret(3001);
    assert(ego.autoTurretTarget == &nearest && self.shots == 1);
    assert(ego.autoTurretFireTimer == 0 && self.stops == 2);
    assert(canvas.turret.updates == 1 && canvas.roll.updates == 1);
    Vector predicted{};
    std::memcpy(&predicted, &ego.waypointY, sizeof(predicted));
    assert(predicted.x == 0.0f && predicted.y == 0.0f && predicted.z == 1525.0f);

    nearest.position = {1000.0f, 0.0f, 25.0f};
    ego.handleAutoTurret(100);
    assert(roll.yawRotations == 1 && turret.yawRotations == 1);
    assert(roll.lastYaw > 0.0f && ego.autoTurretFireTimer == 100);

    nearest.position = {0.0f, 1000.0f, 25.0f};
    ego.handleAutoTurret(100);
    assert(turret.pitchRotations == 1 && turret.lastPitch < 0.0f);
    assert(ego.lookYaw == -16.0f && ego.autoTurretFireTimer == 200);

    nearest.position = {0.0f, -1000.0f, 25.0f};
    ego.handleAutoTurret(100);
    assert(turret.pitchRotations == 2 && turret.lastPitch > 0.0f);
    assert(ego.lookYaw == 0.0f && ego.autoTurretFireTimer == 300);

    nearest.position = {0.0f, 1000.0f, 25.0f};
    ego.lookYaw = -500.0f;
    ego.handleAutoTurret(100);
    assert(ego.autoTurretPrevTarget == &nearest && ego.autoTurretFireTimer == 400);

    nearest.position = {0.0f, -1000.0f, 25.0f};
    ego.lookYaw = 70.0f;
    ego.handleAutoTurret(101);
    assert(ego.autoTurretPrevTarget == &nearest && ego.autoTurretFireTimer == 501);
    assert(self.stops == 3 && self.shots == 1);
    std::puts("PlayerEgo auto-turret smoke: OK");
}
