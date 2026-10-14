#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdint>

namespace AbyssEngine { namespace AEMath {
struct Vector {
    float x, y, z;
    Vector &operator-=(const Vector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector &operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
};
Vector operator+(const Vector &a, const Vector &b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
Vector VectorNormalize(const Vector &v) {
    float n = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return {v.x/n, v.y/n, v.z/n};
}
float VectorDot(const Vector &a, const Vector &b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
float ACosf(float v) { return std::acos(v); }
struct Matrix {
    float m[15]{};
    Matrix() { m[0] = m[5] = m[10] = 1.0f; }
};
Vector MatrixRotateVector(const Matrix &m, const Vector &v) {
    return {m.m[0]*v.x + m.m[1]*v.y + m.m[2]*v.z,
            m.m[4]*v.x + m.m[5]*v.y + m.m[6]*v.z,
            m.m[8]*v.x + m.m[9]*v.y + m.m[10]*v.z};
}
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::Matrix;

struct AEGeometry {
    Vector pos{0.0f, 0.0f, 0.0f};
    Vector dir{0.0f, 0.0f, 1.0f};
    Matrix matrix{};
    float lastForward = 0.0f;
    int setDirectionCount = 0, setMatrixCount = 0;
    Vector getPosition() { return pos; }
    Vector getDirection() { return dir; }
    Vector getRightVector() { return {1.0f, 0.0f, 0.0f}; }
    Matrix &getMatrix() { return matrix; }
    void setDirection(const Vector &d, const Vector &up) {
        assert(up.x == 0.0f && up.y == 1.0f && up.z == 0.0f);
        dir = d;
        ++setDirectionCount;
    }
    void moveForward(float d) {
        lastForward = d;
        pos.z += d;
        matrix.m[11] = pos.z;
    }
    void setMatrix(const Matrix &m) {
        matrix = m;
        pos = {m.m[3], m.m[7], m.m[11]};
        ++setMatrixCount;
    }
};
struct Player { Matrix transformMatrix{}; };
struct TargetFollowCamera {
    int translations = 0;
    Vector last{};
    void translateNoUpdate(float x, float y, float z) {
        ++translations;
        last = {x, y, z};
    }
};
struct KIPlayer { unsigned char field_0x72 = 0; int autoPilotState = 1; };
namespace AbyssEngine {
struct Engine { int autoPilotEngaged = 1; };
struct ApplicationManager {
    static ApplicationManager *gAppManager;
    Engine engine{};
    void *GetEngine() { return &engine; }
};
ApplicationManager *ApplicationManager::gAppManager = nullptr;
}
using AbyssEngine::ApplicationManager;
static const float g_PE_mtp_strafeEps = 0.01f;
static const float g_PE_mtp_strafeReset = 0.1f;
static const float g_PE_mtp_strafeK = 0.7f;

struct PlayerEgo {
    void *player = nullptr;
    AEGeometry *geometry = nullptr;
    void *field_0x14 = nullptr;
    float rotY = 0.0f, rotZ = 0.0f, waypointX = 0.0f;
    float waypointY = 0.0f, gunBaseGeo = 0.0f, pitchAccumDir = 0.0f;
    Vector headingVec{};
    Vector dockOffsetVec{};
    int shakeIntensity = 10;
    float thrust = 2.0f, speedValue = 3.0f;
    float strafeAccel = 0.0f, strafeFactor = 0.0f;
    float steerLimit = 1.0f, steerScale = 15.139357f;
    float steerAngleHistory[5]{};
    int steerHistoryIndex = 0;
    unsigned char steerHistoryFull = 0;
    void *targetFollowCamera = nullptr;
    unsigned char goingToWaypointFlag = 0, autoPilot = 0;
    int autoPilotTarget = 0;
    float moveToPosition(Vector target, bool steer, float speed);
    void setAutoPilot(KIPlayer *kip);
    int rollCalls = 0, lastRoll = 0;
    void roll(int dt) { ++rollCalls; lastRoll = dt; }
};

// PRODUCTION_MOVE
// PRODUCTION_AUTOPILOT

static bool near(float a, float b) { return std::fabs(a - b) < 0.00001f; }

int main() {
    Player player;
    AEGeometry geometry;
    TargetFollowCamera camera;
    AbyssEngine::ApplicationManager app;
    AbyssEngine::ApplicationManager::gAppManager = &app;
    PlayerEgo ego;
    ego.player = &player;
    ego.geometry = &geometry;
    ego.targetFollowCamera = &camera;
    ego.strafeAccel = 0.5f;
    float turn = ego.moveToPosition({100.0f, 0.0f, 100.0f}, false, 409.6f);
    assert(turn == 0.0f && ego.steerHistoryIndex == 0);
    assert(geometry.setDirectionCount == 1 && ego.headingVec.x > 0.3f);
    assert(near(geometry.lastForward, 60.0f) && ego.rollCalls == 1 && ego.lastRoll == 10);
    assert(geometry.setMatrixCount == 1 && camera.translations == 1);
    assert(near(camera.last.x, 5.0f) && near(ego.strafeAccel, 0.35f));
    assert(near(player.transformMatrix.m[3], 5.0f));
    assert(near(ego.dockOffsetVec.x, geometry.pos.x));

    ego.strafeAccel = 0.005f;
    turn = ego.moveToPosition({100.0f, 0.0f, 100.0f}, true, 409.6f);
    assert(std::isfinite(turn) && turn > 0.0f && turn <= ego.steerLimit);
    assert(ego.steerHistoryIndex == 1);
    assert(ego.steerAngleHistory[0] != 0.0f);
    assert(camera.translations == 1 && near(ego.strafeFactor, 0.1f));
    assert(ego.steerHistoryFull == 0);

    ego.steerHistoryIndex = 4;
    ego.steerHistoryFull = 0;
    ego.moveToPosition({100.0f, 0.0f, 100.0f}, true, 409.6f);
    assert(ego.steerHistoryIndex == 0 && ego.steerHistoryFull == 1);
    ego.moveToPosition({100.0f, 0.0f, 100.0f}, true, 409.6f);
    assert(ego.steerHistoryIndex == 1 && ego.steerHistoryFull == 1);

    ego.strafeAccel = std::nanf("");
    ego.moveToPosition({100.0f, 0.0f, 100.0f}, false, 0.0f);
    assert(camera.translations == 1 && near(ego.strafeFactor, 0.1f));

    KIPlayer autopilot;
    autopilot.field_0x72 = 1;
    ego.field_0x14 = &autopilot;
    ego.setAutoPilot(&autopilot);
    assert(ego.autoPilot && ego.goingToWaypointFlag && app.engine.autoPilotEngaged == 0);
    ego.setAutoPilot(nullptr);
    assert(!ego.autoPilot && ego.steerHistoryIndex == 0 && ego.steerHistoryFull == 0);
    assert(near(ego.steerLimit, 1.0f));
    assert(autopilot.autoPilotState == 0);
    std::puts("PlayerEgo moveToPosition smoke: OK");
}
