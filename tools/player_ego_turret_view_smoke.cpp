#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
struct Matrix {
    Vector position{0.0f, 0.0f, 0.0f};
    Vector direction{0.0f, 0.0f, 1.0f};
    Vector up{0.0f, 1.0f, 0.0f};
    Matrix &operator*=(const Matrix &rhs) {
        position.x += rhs.position.x;
        position.y += rhs.position.y;
        position.z += rhs.position.z;
        return *this;
    }
};
Matrix operator*(Matrix lhs, const Matrix &rhs) { return lhs *= rhs; }
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
Vector MatrixGetPosition(const Matrix &m) { return m.position; }
Vector MatrixGetDir(const Matrix &m) { return m.direction; }
Vector MatrixGetUp(const Matrix &m) { return m.up; }
static Vector lookEye{}, lookTarget{};
static int lookCalls = 0;
Matrix MatrixGetLookAt(const Vector &eye, const Vector &target, const Vector &up) {
    lookEye = eye;
    lookTarget = target;
    ++lookCalls;
    Matrix m;
    m.position = eye;
    m.up = up;
    return m;
}
}}
using AbyssEngine::AEMath::Matrix;
using AbyssEngine::AEMath::Vector;

namespace AbyssEngine {
struct AERandom {
    std::vector<int> values;
    std::vector<int> bounds;
    int nextInt(int bound) {
        bounds.push_back(bound);
        assert(bounds.size() <= values.size());
        return values[bounds.size() - 1];
    }
};
struct PaintCanvas {
    unsigned cameraId = 0;
    Matrix cameraMatrix{};
    Matrix shipLocal{}, bodyLocal{};
    void CameraSetLocal(unsigned id, const Matrix &m) {
        cameraId = id;
        cameraMatrix = m;
    }
    void *TransformGetLocal(unsigned id) {
        assert(id == 11 || id == 12);
        return id == 11 ? &shipLocal : &bodyLocal;
    }
};
}
struct Globals {
    static void *rnd;
    static AbyssEngine::PaintCanvas *Canvas;
};
void *Globals::rnd = nullptr;
AbyssEngine::PaintCanvas *Globals::Canvas = nullptr;

struct AEGeometry {
    Matrix matrix{};
    unsigned transform = 0;
    int moves = 0;
    float lastMove = 0.0f;
    Matrix &getMatrix() { return matrix; }
    void moveForward(float distance) { ++moves; lastMove = distance; }
};
struct Player { Matrix transformMatrix{}; };
struct PlayerEgo {
    Player *player = nullptr;
    AEGeometry *geometry = nullptr;
    AEGeometry *field_0x4 = nullptr;
    void *dockCameraNode = nullptr;
    void *dockCameraMid = nullptr;
    int autoPilotTarget = 0;
    unsigned char dockedToStream = 0, dockedFlag = 0;
    int dockingPointIndex = 0;
    float thrust = 2.0f, speedValue = 3.0f;
    unsigned char hitShake = 0, boostingFlag = 0;
    int hitShakeTimer = 0, boostSpeed = 0;
    unsigned turretCamera = 7;
    int pitchAccumDir = 1, yawAccumDir = 1, shakeIntensity = 17;
    float yawAccumulator = 1.0f, pitchAccumulator = 1.0f;
    int rolls = 0, lastRoll = 0;
    float getBoostPercentage() { return 0.5f; }
    void roll(int value) { ++rolls; lastRoll = value; }
    void handleTurretView(int dt);
};

// PRODUCTION_METHOD

static bool near(float a, float b) { return std::fabs(a - b) < 0.00001f; }

int main() {
    Player player;
    AEGeometry ship, body, node, mid;
    ship.transform = 11;
    body.transform = 12;
    ship.matrix.position = {1.0f, 2.0f, 3.0f};
    node.matrix.position = {4.0f, 5.0f, 6.0f};
    mid.matrix.position = {7.0f, 8.0f, 9.0f};
    AbyssEngine::PaintCanvas canvas;
    canvas.shipLocal.position = {10.0f, 20.0f, 30.0f};
    canvas.bodyLocal.position = {1.0f, 2.0f, 3.0f};
    AbyssEngine::AERandom random;
    Globals::Canvas = &canvas;
    Globals::rnd = &random;
    PlayerEgo ego;
    ego.player = &player;
    ego.geometry = &ship;
    ego.field_0x4 = &body;
    ego.dockCameraNode = &node;
    ego.dockCameraMid = &mid;

    ego.handleTurretView(10);
    assert(ship.moves == 1 && near(ship.lastMove, 60.0f));
    assert(canvas.cameraId == 7 && near(canvas.cameraMatrix.position.x, 12.0f));
    assert(near(player.transformMatrix.position.x, 11.0f));
    assert(ego.yawAccumulator == 0.0f && ego.pitchAccumulator == 0.0f);
    assert(ego.pitchAccumDir == 1 && ego.yawAccumDir == 1);
    assert(ego.rolls == 1 && ego.lastRoll == 17);

    ego.autoPilotTarget = 1;
    ego.handleTurretView(10);
    assert(ship.moves == 1);
    ego.dockedToStream = 1;
    ego.handleTurretView(10);
    assert(ship.moves == 2);
    ego.dockedFlag = 1;
    ego.dockingPointIndex = 1;
    ego.handleTurretView(10);
    assert(ship.moves == 2);
    ego.autoPilotTarget = 0;
    ego.handleTurretView(10);
    assert(ship.moves == 2);

    ego.hitShake = 1;
    ego.hitShakeTimer = 900;
    ego.boostingFlag = 1;
    ego.boostSpeed = 1;
    random.values = {7, 6, 5, 3, 2, 1};
    ego.handleTurretView(101);
    assert(ego.hitShake == 0 && ego.hitShakeTimer == 1001);
    assert(random.bounds == std::vector<int>({12, 12, 12, 4, 4, 4}));
    assert(AbyssEngine::AEMath::lookCalls == 1);
    assert(near(AbyssEngine::AEMath::lookEye.x, 12.001f));
    assert(near(AbyssEngine::AEMath::lookEye.z, 17.999f));
    assert(near(AbyssEngine::AEMath::lookTarget.x, 12.00075f));
    assert(near(AbyssEngine::AEMath::lookTarget.z, 16.99925f));
    assert(near(canvas.cameraMatrix.position.x, 12.001f));

    ego.boostSpeed = 99;
    random.values = {7, 6, 5, 3, 2, 1, 8, 8, 8};
    ego.handleTurretView(10);
    assert(random.bounds.size() == 9);
    assert(random.bounds[6] == 16 && random.bounds[8] == 16);
    std::puts("PlayerEgo turret-view smoke: OK");
}
