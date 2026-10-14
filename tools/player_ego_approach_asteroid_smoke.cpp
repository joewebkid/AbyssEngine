#include <cassert>
#include <cstdio>

struct Vector {};
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
    Player *player = nullptr;
    int state = 0;
    int asteroidIndex = 154;
    int getQuality() { return 7; }
    bool isDying() { return state == 3; }
    bool isDead() { return state == 4; }
};
struct Status { int field_124 = 1; };
struct FModSound {
    int played = 0;
    int paused = -1;
    void play(int kind, Vector *, Vector *, float pitch) {
        assert(kind == 1 && pitch == 0);
        ++played;
    }
    int pause(int event) { paused = event; return 0; }
};
struct Globals {
    static unsigned char hints[59];
    static Status *status;
    static FModSound *sound;
};
unsigned char Globals::hints[59]{};
Status *Globals::status = nullptr;
FModSound *Globals::sound = nullptr;

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
static int approachSteps = 0;
struct PlayerEgo;
int PE_aa_approachStep(PlayerEgo *, int, void *) { ++approachSteps; return 0; }

struct PlayerEgo {
    union { int asteroidTarget; void *asteroidTargetPtr; };
    union { int miningGame; void *miningGamePtr; };
    int dockingPointIndex = 0;
    int miningSettleTimer = -1100;
    int shakeIntensity = 16;
    int field_0x1c = 42;
    unsigned char field_0x2f5 = 1;
    unsigned char lostMiningGameFlag = 0;
    int stopCount = 0;
    void stopMining() { ++stopCount; delete (MiningGame *) miningGamePtr; miningGamePtr = nullptr; }
    void approachAsteroid(Hud *hud, int hud2, Radar *radar);
};

// PRODUCTION_METHOD

int main() {
    Player player;
    PlayerAsteroid asteroid;
    asteroid.player = &player;
    PlayerEgo ego{};
    ego.asteroidTargetPtr = &asteroid;
    Hud hud;
    Status status;
    FModSound sound;
    Radar radar;
    Globals::status = &status;
    Globals::sound = &sound;

    asteroid.state = 3;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(approachSteps == 0 && ego.miningGamePtr == nullptr);

    asteroid.state = 0;
    ego.approachAsteroid(&hud, 0, &radar);
    assert(approachSteps == 1 && ego.dockingPointIndex == 0);

    ego.dockingPointIndex = 1;
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
