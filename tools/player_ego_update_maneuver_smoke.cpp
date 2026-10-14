#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace AbyssEngine { namespace AEMath {
struct Vector {
    float x, y, z;
    Vector &operator*=(float value) { x *= value; y *= value; z *= value; return *this; }
};
Vector operator+(const Vector &a, const Vector &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
Vector operator*(const Vector &v, float value) {
    return {v.x * value, v.y * value, v.z * value};
}
struct Matrix {
    float m[15]{};
    Matrix() { m[0] = m[5] = m[10] = 1.0f; }
    template<class... Floats> explicit Matrix(Floats... values) : m{static_cast<float>(values)...} {
        static_assert(sizeof...(values) == 15);
    }
};
static int matrixMultiplies = 0;
Matrix operator*(const Matrix &a, const Matrix &) { ++matrixMultiplies; return a; }
static Vector lastRotation{};
Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z) {
    lastRotation = {x, y, z};
    return matrix;
}
float Sinf(float value) { return std::sin(value); }
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::Matrix;

struct AEGeometry {
    unsigned transform = 0;
    Vector position{10.0f, 20.0f, 30.0f};
    Matrix matrix{};
    int rightCalls = 0, upCalls = 0, directionCalls = 0;
    int forwardCalls = 0, translateCalls = 0, matrixSets = 0;
    float lastForward = 0.0f;
    Vector lastTranslate{};
    Vector getPosition() { return position; }
    Vector getRightVector() { ++rightCalls; return {1.0f, 0.0f, 0.0f}; }
    Vector getUpVector() { ++upCalls; return {0.0f, 1.0f, 0.0f}; }
    Vector getDirection() { ++directionCalls; return {0.0f, 0.0f, 1.0f}; }
    Matrix &getMatrix() { return matrix; }
    void setMatrix(const Matrix &value) { matrix = value; ++matrixSets; }
    void moveForward(float value) { ++forwardCalls; lastForward = value; }
    void translate(float x, float y, float z) {
        ++translateCalls; lastTranslate = {x, y, z};
    }
};
namespace AbyssEngine {
struct PaintCanvas {
    Matrix ship{}, body{};
    int gets = 0;
    void *TransformGetLocal(unsigned id) {
        ++gets;
        assert(id == 11 || id == 12);
        return id == 11 ? &ship : &body;
    }
};
}
struct Globals { static AbyssEngine::PaintCanvas *Canvas; };
AbyssEngine::PaintCanvas *Globals::Canvas = nullptr;
struct Player { Matrix transformMatrix{}; };
struct Level {};
struct LevelScript {
    int resets = 0;
    Level *lastLevel = nullptr;
    void resetCamera(Level *level) { ++resets; lastLevel = level; }
};
struct TargetFollowCamera {
    Vector targetOffset{1.0f, 2.0f, 3.0f};
    Vector lastNoUpdate{}, lastMove{};
    int noUpdateCalls = 0, moveCalls = 0;
    int lookCalls = 0, upCalls = 0;
    bool look = false, useUp = true;
    Vector *getTargetOffset() { return &targetOffset; }
    void setTargetOffset(const Vector &v) { targetOffset = v; }
    void translateNoUpdate(float x, float y, float z) {
        ++noUpdateCalls; lastNoUpdate = {x, y, z};
    }
    void translate(float x, float y, float z) {
        ++moveCalls; lastMove = {x, y, z};
    }
    void setLookAtCam(bool value) { ++lookCalls; look = value; }
    void useTargetsUpVector(bool value) { ++upCalls; useUp = value; }
};

struct PlayerEgo {
    void *player = nullptr;
    AEGeometry *field_0x4 = nullptr, *geometry = nullptr;
    Level *level = nullptr;
    LevelScript *levelScript = nullptr;
    void *targetFollowCamera = nullptr;
    int maneuverType = 0, maneuverTimer = 0, shakeIntensity = 100;
    float handling = 2.0f, thrust = 2.0f, speedValue = 3.0f;
    double pitchAccumD = 0.0;
    Vector strafeTargetVec{50.0f, 60.0f, 70.0f};
    Vector facingVec{1.0f, 0.0f, 0.0f};
    int moveCalls = 0;
    Vector lastTarget{};
    bool lastSteer = false;
    float lastSpeed = 0.0f;
    Vector getPosition() { return geometry->getPosition(); }
    float moveToPosition(Vector target, bool steer, float speed) {
        ++moveCalls; lastTarget = target; lastSteer = steer; lastSpeed = speed; return 0.0f;
    }
    int updateManeuver();
};

// PRODUCTION_METHOD

static bool near(float a, float b) { return std::fabs(a - b) < 0.001f; }

int main() {
    Player player;
    AEGeometry geometry, body;
    geometry.transform = 11;
    body.transform = 12;
    Level level;
    LevelScript script;
    TargetFollowCamera camera;
    AbyssEngine::PaintCanvas canvas;
    Globals::Canvas = &canvas;
    PlayerEgo ego;
    ego.player = &player;
    ego.geometry = &geometry;
    ego.field_0x4 = &body;
    ego.level = &level;
    ego.levelScript = &script;
    ego.targetFollowCamera = &camera;
    assert(ego.updateManeuver() == 0 && ego.maneuverTimer == 0);

    ego.maneuverType = 1;
    assert(ego.updateManeuver() == 1 && ego.maneuverTimer == 100);
    assert(near(geometry.lastForward, 600.0f));
    assert(geometry.translateCalls == 1 && geometry.lastTranslate.x > 0.0f);
    assert(camera.noUpdateCalls == 1 && camera.lastNoUpdate.x < 0.0f);
    assert(camera.targetOffset.x > 0.0f && near(camera.targetOffset.y, 2.0f));
    float sway = 0.0f;
    std::memcpy(&sway, reinterpret_cast<const char *>(&ego.pitchAccumD) + 4, sizeof(sway));
    assert(sway < 0.0f && near(camera.targetOffset.x, sway * -3.0f));
    assert(canvas.gets == 2 && AbyssEngine::AEMath::matrixMultiplies == 2);
    assert(near(AbyssEngine::AEMath::lastRotation.y, -0.0057596f));
    ego.maneuverTimer = 1190;
    ego.shakeIntensity = 10;
    assert(ego.updateManeuver() == 1 && ego.maneuverType == 0);
    assert(script.resets == 1 && script.lastLevel == &level);

    ego.maneuverType = 2;
    ego.maneuverTimer = 0;
    ego.shakeIntensity = 100;
    assert(ego.updateManeuver() == 1 && geometry.lastTranslate.x < 0.0f);
    assert(camera.lastNoUpdate.x > 0.0f);

    ego.maneuverType = 3;
    ego.maneuverTimer = 0;
    assert(ego.updateManeuver() == 1 && ego.maneuverTimer == 100);
    assert(ego.moveCalls == 1 && ego.lastSteer && near(ego.lastSpeed, 7.0f));
    assert(near(ego.lastTarget.x, 10.0f) && near(ego.lastTarget.y, 10020.0f));
    assert(near(ego.lastTarget.z, 10030.0f));
    assert(camera.look && !camera.useUp && camera.moveCalls == 1);
    assert(camera.lastMove.x > 0.0f && geometry.rightCalls >= 3);
    ego.maneuverTimer = 900;
    ego.updateManeuver();
    assert(near(ego.lastTarget.x, 50.0f) && near(ego.lastTarget.z, 70.0f));
    ego.maneuverTimer = 2900;
    ego.updateManeuver();
    assert(ego.maneuverType == 0 && !camera.look && camera.useUp);
    std::puts("PlayerEgo updateManeuver smoke: OK");
}
