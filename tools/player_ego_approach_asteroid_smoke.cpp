#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>

namespace AbyssEngine { namespace AEMath {
struct Vector {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    Vector &operator-=(const Vector &other) {
        x -= other.x; y -= other.y; z -= other.z; return *this;
    }
};
Vector operator-(Vector lhs, const Vector &rhs) { return lhs -= rhs; }
float VectorLength(const Vector &v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
Vector VectorNormalize(const Vector &v) {
    float length = VectorLength(v);
    return {v.x / length, v.y / length, v.z / length};
}
float VectorDot(const Vector &a, const Vector &b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
float ACosf(float v) { return std::acos(v); }
struct Matrix {
    float m[15]{};
    Matrix() { m[0] = m[5] = m[10] = 1.0f; }
    template<class... Floats> explicit Matrix(Floats... values) : m{static_cast<float>(values)...} {
        static_assert(sizeof...(values) == 15, "matrix size");
    }
    Matrix &operator*=(const Matrix &) { ++multiplies; return *this; }
    static int multiplies;
};
int Matrix::multiplies = 0;
static float lastRotationX = 0.0f;
Matrix MatrixSetRotation(Matrix &matrix, float x, float, float) {
    lastRotationX = x;
    return matrix;
}
}}
using Vector = AbyssEngine::AEMath::Vector;
using Matrix = AbyssEngine::AEMath::Matrix;
struct AEGeometry {
    Vector position{};
    Vector direction{0.0f, 0.0f, 1.0f};
    Vector up{0.0f, 1.0f, 0.0f};
    Matrix matrix{};
    unsigned transform = 11;
    int matrixSets = 0;
    Vector getPosition() { return position; }
    Vector getDirection() { return direction; }
    Vector getUpVector() { return up; }
    void setMatrix(const Matrix &value) { matrix = value; ++matrixSets; }
};
struct PaintCanvas {
    Matrix local{};
    void *TransformGetLocal(unsigned id) { assert(id == 11); return &local; }
};
struct TargetFollowCamera {
    bool active = true;
    void setActive(bool value) { active = value; }
};
struct PlayerEgo;
struct Hud {
    int events = 0;
    void hudEvent(int kind, PlayerEgo *, int value) {
        assert(kind == 8 && value == 0);
        ++events;
    }
};
struct Player { int radius = 12; };
struct PlayerAsteroid {
    unsigned char raw[0x3d]{};
    Player *player = nullptr;
    AEGeometry *geometry = nullptr;
    int state = 0;
    int asteroidIndex = 154;
    int rotationStops = 0;
    int getQuality() { return 7; }
    bool isDying() { return state == 3; }
    bool isDead() { return state == 4; }
    Vector getPosition() { return geometry->getPosition(); }
    void setRotationEnabled(bool enabled) { assert(!enabled); ++rotationStops; }
};
struct Ship { float handling = 2.0f; float getHandling() { return handling; } };
struct Status {
    int field_124 = 1;
    Ship ship;
    Ship *getShip() { return &ship; }
};
struct FModSound {
    int played = 0;
    int approachPlayed = 0;
    int paused = -1;
    void play(int kind, Vector *, Vector *, float pitch) {
        assert((kind == 1 || kind == 2) && pitch == 0);
        if (kind == 1) ++played;
        else ++approachPlayed;
    }
    int pause(int event) { paused = event; return 0; }
};
struct Globals {
    static unsigned char hints[59];
    static Status *status;
    static FModSound *sound;
    static PaintCanvas *Canvas;
};
unsigned char Globals::hints[59]{};
Status *Globals::status = nullptr;
FModSound *Globals::sound = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
Status *PE_status() { return Globals::status; }

struct MiningGame {
    static Hud *lastHud;
    int quality;
    int index;
    int result = 1;
    bool lost = false;
    int ore = 0;
    bool killTarget = false;
    PlayerAsteroid *target = nullptr;
    MiningGame(int q, int i, Hud *hud) : quality(q), index(i) { lastHud = hud; }
    int update(int) { if (killTarget) target->state = 3; return result; }
    bool gameLost() { return lost; }
    int getOreAmount() { return ore; }
};
Hud *MiningGame::lastHud = nullptr;

struct Radar {};
static const float g_PE_aa_settleEps = -1024.0f;
static const float g_PE_upd_handlingBias = 2.7f;

struct PlayerEgo {
    union { int asteroidTarget; void *asteroidTargetPtr; };
    union { int miningGame; void *miningGamePtr; };
    int dockingPointIndex = 0;
    int miningSettleTimer = 0;
    int shakeIntensity = 16;
    int field_0x1c = 42;
    int field_0xcc = 600, boostTimer = 0, dockScaling = 1000;
    float thrust = 0.0f;
    float waypointY = 0.0f, waypointZ = 0.0f, waypointW = 0.0f;
    float rotY = 0.0f, rotZ = 0.0f, waypointX = 0.0f;
    Vector dockApproachVector{};
    char _unknown_0x1d4[4]{};
    unsigned char turretActive = 1, rolling = 1, aboutToReachAutoTargetFlag = 0;
    unsigned char field_0x2f5 = 1;
    unsigned char lostMiningGameFlag = 0;
    AEGeometry *geometry = nullptr, *field_0x4 = nullptr;
    void *targetFollowCamera = nullptr;
    int turretStops = 0, exhaustStops = 0, moveCalls = 0;
    Vector lastMove{};
    float lastMoveSpeed = 0.0f;
    void setTurretMode(bool value) { assert(!value); ++turretStops; }
    void setExhaustVisible(bool value) { assert(!value); ++exhaustStops; }
    float moveToPosition(Vector point, bool steer, float speed) {
        assert(steer); ++moveCalls; lastMove = point; lastMoveSpeed = speed; return 0.0f;
    }
    Vector getPosition() { return geometry->getPosition(); }
    int stopCount = 0;
    void stopMining() { ++stopCount; delete (MiningGame *) miningGamePtr; miningGamePtr = nullptr; }
    void approachAsteroid(Hud *hud, int hud2, Radar *radar);
};

// PRODUCTION_METHOD

int main() {
    Player player;
    PlayerAsteroid asteroid;
    asteroid.player = &player;
    AEGeometry asteroidGeometry, egoGeometry, bodyGeometry;
    asteroidGeometry.position = {5000.0f, 0.0f, 0.0f};
    asteroid.geometry = &asteroidGeometry;
    PlayerEgo ego{};
    ego.asteroidTargetPtr = &asteroid;
    ego.geometry = &egoGeometry;
    ego.field_0x4 = &bodyGeometry;
    TargetFollowCamera camera;
    ego.targetFollowCamera = &camera;
    Hud hud;
    Status status;
    FModSound sound;
    PaintCanvas canvas;
    Radar radar;
    Globals::status = &status;
    Globals::sound = &sound;
    Globals::Canvas = &canvas;

    asteroid.state = 3;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego.moveCalls == 0 && ego.miningGamePtr == nullptr);

    asteroid.state = 0;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego.turretStops == 1 && ego.moveCalls == 1);
    assert(ego.lastMove.x == 5000.0f && ego.lastMoveSpeed == 4.0f);
    assert(ego.aboutToReachAutoTargetFlag == 1 && camera.active);
    status.ship.handling = 0.5f;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego.moveCalls == 2 && std::fabs(ego.lastMoveSpeed - 3.2f) < 0.001f);
    status.ship.handling = 2.0f;

    asteroidGeometry.position.x = 1500.0f;
    std::memset(&ego.miningSettleTimer, 0, sizeof(ego.miningSettleTimer));
    ego.field_0x2f5 = 0;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego.dockingPointIndex == 0 && ego.thrust == 1.0f);
    assert(sound.approachPlayed == 1 && ego.exhaustStops == 1 && !camera.active);
    assert(ego.boostTimer == 500 && ego.field_0x2f5 == 1);
    assert(bodyGeometry.matrixSets == 1 && Matrix::multiplies == 1);
    float settle = 0.0f;
    std::memcpy(&settle, &ego.miningSettleTimer, sizeof(settle));
    assert(std::fabs(settle + 5.6f) < 0.01f);
    assert(AbyssEngine::AEMath::lastRotationX < 0.0f);

    egoGeometry.direction = {0.0f, 1.0f, 0.0f};
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego._unknown_0x1d4[0] == 1 && ego.rolling == 0 && ego.field_0x2f5 == 0);
    asteroidGeometry.position.x = 100.0f;
    asteroid.raw[0x3c] = 1;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego.dockingPointIndex == 1 && asteroid.rotationStops == 1);

    float miningReady = -1100.0f;
    std::memcpy(&ego.miningSettleTimer, &miningReady, sizeof(miningReady));
    Globals::hints[0x37] = 1;
    ego.approachAsteroid(&hud, 0, &radar);
    MiningGame *game = (MiningGame *) ego.miningGamePtr;
    assert(game && game->quality == 7 && game->index == 154);
    assert(MiningGame::lastHud == &hud && Globals::hints[0x37] == 0);
    assert(player.radius == 0 && sound.played == 1 && sound.paused == 42);
    assert(ego.field_0x2f5 == 0 && ego.lostMiningGameFlag == 0);

    game->target = &asteroid;
    game->killTarget = true;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego.stopCount == 1 && ego.miningGamePtr == nullptr);
    assert(status.field_124 == 0 && hud.events == 1);

    asteroid.state = 0;
    ego.approachAsteroid(&hud, 0, &radar);
    game = (MiningGame *) ego.miningGamePtr;
    game->result = 0;
    game->lost = true;
    status.field_124 = 9;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(ego.stopCount == 2 && ego.lostMiningGameFlag == 1);
    assert(status.field_124 == 0 && hud.events == 2);
    std::puts("PlayerEgo approachAsteroid smoke: OK");
}
