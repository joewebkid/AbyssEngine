#include <cassert>
#include <cstdint>
#include <cstdio>

struct Vec3 { float x = 0.0f, y = 0.0f, z = 0.0f; };
using Vector = Vec3;
struct Matrix {
    float m[15]{};
    Matrix() { m[0] = m[5] = m[10] = 1.0f; }
};
namespace AbyssEngine {
namespace AEMath {
Matrix MatrixSetTranslation(Matrix &matrix, const Vector &position) {
    matrix.m[3] = position.x;
    matrix.m[7] = position.y;
    matrix.m[11] = position.z;
    return matrix;
}
}
struct EaseInOutMatrix {
    static int destroyed;
    Matrix from, to;
    int duration;
    EaseInOutMatrix(Matrix start, Matrix end, int time)
        : from(start), to(end), duration(time) {}
    ~EaseInOutMatrix() { ++destroyed; }
};
int EaseInOutMatrix::destroyed = 0;
struct PaintCanvas {
    unsigned current = 0;
    void CameraSetCurrent(unsigned id) { current = id; }
};
}
struct Globals { static AbyssEngine::PaintCanvas *Canvas; };
AbyssEngine::PaintCanvas *Globals::Canvas = nullptr;
struct AEGeometry {
    Matrix matrix;
    Matrix &getMatrix() { return matrix; }
};
struct SpacePoint {
    Vector position;
    int releases = 0;
    void giveFree() { ++releases; }
};
struct KIPlayer {
    unsigned char flags[0x90]{};
    SpacePoint *nav = nullptr;
    int navCalls = 0;
    SpacePoint *getNearestNavigationPoint(const Vector &, SpacePoint *) {
        ++navCalls;
        return nav;
    }
};
struct Radar {};
struct TargetFollowCamera {
    unsigned id = 37;
    bool active = false, look = false, useUp = true;
    void setActive(bool value) { active = value; }
    void setLookAtCam(bool value) { look = value; }
    void useTargetsUpVector(bool value) { useUp = value; }
};
struct Level {};
struct LevelScript {
    Level *lastLevel = nullptr;
    void resetCamera(Level *value) { lastLevel = value; }
};
struct Player {
    int gunDelayResets = 0;
    void resetGunDelay(int value) { assert(value == 0); ++gunDelayResets; }
};
struct HackingGame {};
struct Hud { void setHackingGameActive(bool) {} };
struct PlayerEgo {
    bool dead = false;
    unsigned char dockedFlag = 0, field_0x145 = 0, field_0x1a1 = 0;
    void *asteroidTarget = nullptr, *easeMatrix = nullptr, *targetFollowCamera = nullptr;
    void *hackingGame = nullptr, *hud = nullptr, *player = nullptr;
    int dockScaling = 0, miningSettleTimer = 0, dockingPointIndex = 0;
    AEGeometry *geometry = nullptr;
    Level *level = nullptr;
    LevelScript *levelScript = nullptr;
    SpacePoint *spacePoint = nullptr;
    Vec3 dockApproachVector{1.0f, 2.0f, 3.0f};
    int turretCalls = 0, exhaustCalls = 0;
    bool isDead() { return dead; }
    Vector getPosition() { return {4.0f, 5.0f, 6.0f}; }
    void setTurretMode(int value) { assert(value == 0); ++turretCalls; }
    void setExhaustVisible(bool value) { assert(value); ++exhaustCalls; }
    void dockToDockingPoint(KIPlayer *, Radar *);
};
static int engineSounds = 0;
void PlayEngineSound_(PlayerEgo *) { ++engineSounds; }

// PRODUCTION_METHOD

int main() {
    AbyssEngine::PaintCanvas canvas;
    Globals::Canvas = &canvas;
    AEGeometry geometry;
    geometry.matrix.m[3] = 1.0f;
    geometry.matrix.m[7] = 2.0f;
    geometry.matrix.m[11] = 3.0f;
    SpacePoint nav{{10.0f, 20.0f, 30.0f}};
    KIPlayer kip;
    kip.nav = &nav;
    kip.flags[0x70] = 1;
    TargetFollowCamera camera;
    Level level;
    LevelScript script;
    Player player;
    PlayerEgo ego;
    ego.geometry = &geometry;
    ego.targetFollowCamera = &camera;
    ego.level = &level;
    ego.levelScript = &script;
    ego.player = &player;

    ego.dockToDockingPoint(&kip, nullptr);
    assert(ego.dockedFlag == 1 && ego.asteroidTarget == &kip);
    assert(ego.dockScaling == 1400 && ego.field_0x145 == 1);
    assert(kip.navCalls == 0);

    ego.spacePoint = &nav;
    ego.easeMatrix = new AbyssEngine::EaseInOutMatrix(Matrix{}, Matrix{}, 1);
    ego.dockToDockingPoint(&kip, nullptr);
    assert(kip.navCalls == 2 && kip.flags[0x8c] == 1);
    assert(ego.turretCalls == 1 && ego.dockingPointIndex == 3);
    assert(canvas.current == camera.id && script.lastLevel == &level);
    assert(camera.look && !camera.useUp && ego.exhaustCalls == 1);
    assert(AbyssEngine::EaseInOutMatrix::destroyed == 1);
    auto *ease = static_cast<AbyssEngine::EaseInOutMatrix *>(ego.easeMatrix);
    assert(ease && ease->duration == 3000);
    assert(ease->from.m[3] == 1.0f && ease->from.m[7] == 2.0f);
    assert(ease->to.m[3] == 10.0f && ease->to.m[7] == 20.0f);
    assert(ease->to.m[11] == 30.0f && ease->to.m[0] == 1.0f);
    delete ease;
    ego.easeMatrix = nullptr;

    kip.nav = nullptr;
    ego.dockToDockingPoint(&kip, nullptr);
    assert(ego.dockedFlag == 0 && ego.asteroidTarget == nullptr);
    assert(ego.dockApproachVector.x == 0.0f && ego.dockApproachVector.z == 0.0f);
    assert(camera.active && !camera.look && !camera.useUp);
    assert(player.gunDelayResets == 1 && nav.releases == 1 && ego.spacePoint == nullptr);
    assert(ego.dockingPointIndex == 0 && ego.exhaustCalls == 2);
    ego.dockedFlag = 1;
    ego.dockApproachVector = {1.0f, 2.0f, 3.0f};
    ego.dockToDockingPoint(nullptr, nullptr);
    assert(ego.dockedFlag == 0 && ego.dockApproachVector.y == 0.0f);
    assert(player.gunDelayResets == 2);
    ego.dead = true;
    ego.dockToDockingPoint(&kip, nullptr);
    assert(ego.dockedFlag == 0);
    std::puts("PlayerEgo dockToDockingPoint smoke: OK");
}
