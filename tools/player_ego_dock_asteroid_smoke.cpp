#include <cassert>
#include <cstdio>
#include <cstring>

struct KIPlayer {};
struct PlayerAsteroid : KIPlayer {
    float scaling = 1.0f;
    bool rotationEnabled = false;
    float getScaling() { return scaling; }
    void setRotationEnabled(bool value) { rotationEnabled = value; }
};
struct Radar {
    int unlocks = 0;
    void unlockAsteroid() { ++unlocks; }
};
struct TargetFollowCamera {
    bool active = false;
    void setActive(bool value) { active = value; }
};
struct Player {
    int resetCount = 0;
    void resetGunDelay(int value) { assert(value == 0); ++resetCount; }
};
static int destroyedMiningGames = 0;
struct MiningGame { ~MiningGame() { ++destroyedMiningGames; } };
static const float g_PE_astApproach = 2500.0f;
void Vec_assign(void *dst, const void *src) { std::memcpy(dst, src, 3 * sizeof(int)); }

struct PlayerEgo {
    union { int asteroidTarget; void *asteroidTargetPtr; };
    union { int targetFollowCamera; void *targetFollowCameraPtr; };
    union { int miningGame; void *miningGamePtr; };
    Player *player = nullptr;
    int dockingState = 0;
    int miningSettleTimer = 0;
    int dockingPointIndex = 0;
    int dockScaling = 0;
    int dockApproachDist[3]{};
    unsigned char field_0x145 = 0;
    bool exhaustVisible = false;
    void setExhaustVisible(bool value) { exhaustVisible = value; }
    void dockToAsteroid(KIPlayer *kip, Radar *radar);
};

// PRODUCTION_METHOD

int main() {
    PlayerEgo ego{};
    PlayerAsteroid asteroid;
    Radar radar;
    TargetFollowCamera camera;
    Player player;
    ego.player = &player;
    ego.targetFollowCameraPtr = &camera;
    ego.dockingPointIndex = 7;
    ego.dockScaling = 123;
    ego.dockToAsteroid(nullptr, &radar);
    assert(ego.dockingState == 0 && ego.asteroidTargetPtr == nullptr);
    assert(ego.dockScaling == 123 && radar.unlocks == 0);

    asteroid.scaling = 1.25f;
    ego.miningSettleTimer = 99;
    ego.dockToAsteroid(&asteroid, nullptr);
    assert(ego.dockingState == 1 && ego.asteroidTargetPtr == &asteroid);
    assert(ego.dockScaling == 3125 && ego.miningSettleTimer == 0);
    assert(ego.dockingPointIndex == 0 && ego.field_0x145 == 1);

    ego.miningGamePtr = new MiningGame;
    ego.dockApproachDist[0] = 1;
    ego.dockApproachDist[1] = 2;
    ego.dockApproachDist[2] = 3;
    ego.dockToAsteroid(nullptr, &radar);
    assert(ego.dockingState == 0 && ego.asteroidTargetPtr == nullptr);
    assert(ego.field_0x145 == 0 && asteroid.rotationEnabled);
    assert(ego.dockApproachDist[0] == 0 && ego.dockApproachDist[1] == 0 && ego.dockApproachDist[2] == 0);
    assert(camera.active && player.resetCount == 1 && radar.unlocks == 1);
    assert(ego.miningGamePtr == nullptr && destroyedMiningGames == 1);
    assert(ego.exhaustVisible && ego.dockingPointIndex == 0);
    std::puts("PlayerEgo dockToAsteroid smoke: OK");
}
