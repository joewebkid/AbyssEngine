#include <cassert>
#include <cstdint>

struct Vector { float x, y, z; };
struct Matrix {
    int value = 0;
    Matrix &operator*=(const Matrix &other) {
        value = value * 10 + other.value;
        return *this;
    }
};
struct AEGeometry {
    Vector position{1.25f, -2.5f, 3.75f};
    Matrix matrix{2};
    int matrixCalls = 0;
    Vector getPosition() { return position; }
    Matrix &getMatrix() { ++matrixCalls; return matrix; }
};
struct Explosion { bool playing = false; bool isPlaying() { return playing; } };
struct Player {
    bool active = true;
    Matrix transformMatrix{};
    union {
        uint16_t enemyFlags = 0x707;
        struct { uint8_t enemyFlagsLo, carriesFriendCargoFlag; };
    };
    int damageDoneByPlayer = 0;
    int maxHitpoints = 100;
    bool gunAvailableResult = false;
    bool alwaysEnemy = false;
    bool alwaysFriend = false;
    bool turnedEnemyFlag = false;
    int gunChecks = 0;
    int getMaxHitpoints() { return maxHitpoints; }
    void setActive(bool value) { active = value; }
    bool gunAvailable(int slot) { assert(slot == 1); ++gunChecks; return gunAvailableResult; }
    bool isAlwaysEnemy() { return alwaysEnemy; }
    void setAlwaysEnemy(bool value) {
        alwaysEnemy = value;
        enemyFlags = 1;
        turnedEnemyFlag = value;
    }
    bool turnedEnemy() { return turnedEnemyFlag; }
    bool isAlwaysFriend() { return alwaysFriend; }
};
struct Standing {
    bool enemy = false;
    bool friendly = false;
    int enemyCalls = 0;
    int friendCalls = 0;
    bool isEnemy(int) { ++enemyCalls; return enemy; }
    bool isFriend(int) { ++friendCalls; return friendly; }
};
struct Status {
    Standing *standing;
    bool blackMarket = false;
    uint8_t field_110 = 0;
    Standing *getStanding() { return standing; }
    bool inBlackMarketSystem() { return blackMarket; }
};
struct Globals { static Status *status; };
Status *Globals::status = nullptr;

struct KIPlayer;
struct Radar { KIPlayer *lockedEnemy = nullptr; };
struct PlayerEgo { void *field_0x14 = nullptr; };
struct Level {
    PlayerEgo *ego = nullptr;
    int getPlayerCalls = 0;
    int uncovered = -1;
    PlayerEgo *getPlayer() { ++getPlayerCalls; return ego; }
    void uncoverWanted(int index) { uncovered = index; }
};
struct Route {
    int cloneCalls = 0;
    Route *clone() { ++cloneCalls; return this; }
};
struct Trail {
    int calls = 0;
    Vector current{}, previous{};
    void update(const Vector &a, const Vector &b) {
        ++calls;
        current = a;
        previous = b;
    }
};
struct KIPlayer {
    Player *player = nullptr;
    AEGeometry *geometry = nullptr;
    Explosion *explosion = nullptr;
    int state = 0;
    uint8_t crateCaptured = 1;
    int deathTimer = 0;
    int field_0x1c0 = 0;
    int maneuverTimer = 0;
    int shipGroup = 0;
    int deltaTime = 0;
    int deltaTimeHi = 0;
    Vector renderPosition{};
    uint8_t field_0x43 = 0;
    char field_0x42 = 0;
    int gunSwitchTimer = 0;
    int field_0x140 = 0;
    int field_0x48 = 9;
    Level *level = nullptr;
    AEGeometry *subGeometry = nullptr;
    Route *routeClone = nullptr;
    uint8_t wingmanFlag = 0;
    bool wingman = false;
    void setActive(bool value) { player->setActive(value); }
    bool isWingMan() { return wingman; }
};
struct PlayerFighter : KIPlayer {
    Route *route = nullptr;
    Trail *trail = nullptr;
    Vector workingPosition{};
    int field_0x1c8 = 0;
    void update(int dt);
};
int bodyCalls = 0;
void PF_update_body(PlayerFighter *, int) { ++bodyCalls; }

// PRODUCTION_METHOD

int main() {
    AEGeometry geometry;
    Explosion explosion;
    Player player;
    Standing standing;
    Status status{&standing};
    Globals::status = &status;
    PlayerFighter fighter;
    fighter.geometry = &geometry;
    fighter.explosion = &explosion;
    fighter.player = &player;
    Route route;
    fighter.route = &route;

    fighter.state = 4;
    fighter.crateCaptured = 0;
    fighter.update(7);
    assert(!player.active && bodyCalls == 0 && fighter.deltaTime == 0);

    player.active = true;
    fighter.crateCaptured = 1;
    fighter.deathTimer = 60000;
    fighter.shipGroup = 8;
    fighter.update(7);
    assert(player.active && bodyCalls == 1);
    assert(fighter.deltaTime == 7 && fighter.deltaTimeHi == 0);
    assert(fighter.renderPosition.x == 1.25f && fighter.renderPosition.y == -2.5f);
    assert(fighter.renderPosition.z == 3.75f);
    assert(fighter.field_0x1c0 == 7 && fighter.maneuverTimer == 7);
    assert(player.enemyFlagsLo == 1 && player.carriesFriendCargoFlag == 0);
    assert(standing.enemyCalls == 0 && standing.friendCalls == 0);
    assert(fighter.routeClone == &route && route.cloneCalls == 1);
    assert(fighter.field_0x1c8 == 7 && player.transformMatrix.value == 2);

    fighter.shipGroup = 2;
    fighter.wingman = true;
    fighter.update(-3);
    assert(fighter.deltaTime == -3 && fighter.deltaTimeHi == -1);
    assert(player.enemyFlagsLo == 0 && player.carriesFriendCargoFlag == 1);

    fighter.wingman = false;
    standing.enemy = true;
    standing.friendly = true;
    AEGeometry secondary;
    secondary.matrix.value = 3;
    fighter.subGeometry = &secondary;
    Trail trail;
    fighter.trail = &trail;
    fighter.workingPosition = {9.0f, 8.0f, 7.0f};
    fighter.field_0x1c8 = 200;
    fighter.update(1);
    assert(player.enemyFlagsLo == 1 && player.carriesFriendCargoFlag == 1);
    assert(standing.enemyCalls == 1 && standing.friendCalls == 1);
    assert(trail.calls == 1 && trail.current.x == 1.25f && trail.previous.x == 9.0f);
    assert(fighter.workingPosition.y == -2.5f && fighter.field_0x1c8 == 0);
    assert(player.transformMatrix.value == 23 && secondary.matrixCalls == 1);
    assert(route.cloneCalls == 1);

    fighter.field_0x43 = 1;
    player.enemyFlagsLo = 7;
    player.carriesFriendCargoFlag = 7;
    player.damageDoneByPlayer = 5;
    fighter.update(1);
    assert(player.enemyFlagsLo == 7 && player.carriesFriendCargoFlag == 7);
    player.damageDoneByPlayer = 6;
    fighter.update(1);
    assert(player.enemyFlagsLo == 1 && player.carriesFriendCargoFlag == 7);
    assert(standing.enemyCalls == 1 && standing.friendCalls == 1);

    Radar radar{&fighter};
    PlayerEgo ego{&radar};
    Level level{&ego};
    fighter.level = &level;
    fighter.field_0x42 = 1;
    fighter.field_0x43 = 0;
    fighter.shipGroup = 2;
    fighter.gunSwitchTimer = 19999;
    player.gunAvailableResult = true;
    fighter.update(1);
    assert(fighter.gunSwitchTimer == 20000 && fighter.field_0x140 == 0);
    assert(player.gunChecks == 0 && player.alwaysEnemy);
    assert(level.uncovered == 9 && level.getPlayerCalls == 2);
    fighter.update(1);
    assert(fighter.gunSwitchTimer == 0 && fighter.field_0x140 == 1);
    assert(player.gunChecks == 1 && level.uncovered == 9);

    player.gunAvailableResult = false;
    fighter.gunSwitchTimer = 20000;
    fighter.update(1);
    assert(fighter.gunSwitchTimer == 0 && fighter.field_0x140 == 1);
    assert(player.gunChecks == 2);

    player.alwaysEnemy = false;
    player.turnedEnemyFlag = false;
    fighter.field_0x43 = 1;
    level.uncovered = -1;
    fighter.update(1);
    assert(level.uncovered == -1 && level.getPlayerCalls == 4);

    fighter.field_0x42 = 0;
    player.turnedEnemyFlag = false;
    player.alwaysFriend = false;
    status.blackMarket = true;
    status.field_110 = 1;
    fighter.shipGroup = 8;
    fighter.update(1);
    assert(player.enemyFlags == 0);
    player.turnedEnemyFlag = true;
    fighter.update(1);
    assert(player.enemyFlags == 1);
    player.alwaysFriend = true;
    fighter.update(1);
    assert(player.enemyFlags == 0x100);

    fighter.wingmanFlag = 1;
    fighter.routeClone = nullptr;
    fighter.trail = nullptr;
    fighter.field_0x1c8 = 200;
    fighter.update(1);
    assert(fighter.routeClone == nullptr && route.cloneCalls == 1);
    assert(fighter.field_0x1c8 == 0 && trail.calls == 1);

    fighter.deathTimer = 60001;
    fighter.update(1);
    assert(!player.active && bodyCalls == 13);
}
