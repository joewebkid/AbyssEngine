#include <cassert>
#include <cmath>
#include <cstdint>

struct Vector {
    float x, y, z;
    Vector &operator+=(const Vector &v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vector &operator-=(const Vector &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vector &operator*=(float n) { x *= n; y *= n; z *= n; return *this; }
};
Vector operator*(Vector v, float n) { return v *= n; }
namespace AbyssEngine { namespace AEMath {
Vector VectorNormalize(const Vector &v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return len == 0.0f ? Vector{0.0f, 1.0f, 0.0f} : Vector{v.x / len, v.y / len, v.z / len};
}
} }
struct Matrix {
    int value = 0;
    Matrix &operator*=(const Matrix &other) {
        value = value * 10 + other.value;
        return *this;
    }
};
struct AEGeometry {
    Vector position{1.25f, -2.5f, 3.75f};
    Vector right{1.0f, 0.0f, 0.0f};
    Vector up{0.0f, 1.0f, 0.0f};
    Vector direction{0.0f, 0.0f, 1.0f};
    Matrix matrix{2};
    int matrixCalls = 0;
    int rightCalls = 0, upCalls = 0, directionCalls = 0;
    Vector getPosition() { return position; }
    Vector getRightVector() { ++rightCalls; return right; }
    Vector getUpVector() { ++upCalls; return up; }
    Vector getDirection() { ++directionCalls; return direction; }
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
    int updateCalls = 0;
    int stopSoundCalls = 0;
    int enemiesCalls = 0;
    bool lastDoSound = false;
    int lastDt = 0;
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
    void update(int dt, bool doSound) {
        ++updateCalls;
        lastDt = dt;
        lastDoSound = doSound;
    }
    void StopEngineSound() { ++stopSoundCalls; }
    void *getEnemies() { ++enemiesCalls; return nullptr; }
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
struct PlayerEgo { void *field_0x14 = nullptr; AEGeometry *geometry = nullptr; };
struct Level {
    PlayerEgo *ego = nullptr;
    int getPlayerCalls = 0;
    int uncovered = -1;
    PlayerEgo *getPlayer() { ++getPlayerCalls; return ego; }
    void uncoverWanted(int index) { uncovered = index; }
};
struct Route {
    static int constructed, destroyed;
    int cloneCalls = 0;
    int points = 1;
    int lengthCalls = 0, setCoordsCalls = 0, loopCalls = 0;
    bool looping = false;
    Vector coords{};
    Route() = default;
    Route(int *v, int count) : points(count / 3), coords{float(v[0]), float(v[1]), float(v[2])} { ++constructed; }
    ~Route() { ++destroyed; }
    Route *clone() { ++cloneCalls; return this; }
    int length() { ++lengthCalls; return points; }
    void setNewCoords(Vector v) { ++setCoordsCalls; coords = v; }
    void setLoop(bool value) { ++loopCalls; looping = value; }
};
int Route::constructed = 0;
int Route::destroyed = 0;
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
    int wingmanCommand = 1;
    int field_0xe4 = 0;
    int fov = 0;
    Vector targetPos{};
    Vector objectPosition{};
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
    assert(player.stopSoundCalls == 1 && player.updateCalls == 0 && player.enemiesCalls == 0);
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

    player.active = true;
    fighter.state = 0;
    fighter.crateCaptured = 1;
    fighter.wingmanFlag = 0;
    fighter.update(17);
    assert(player.updateCalls == 1 && player.enemiesCalls == 1);
    assert(player.lastDt == 17 && player.lastDoSound);
    fighter.fov = -1;
    fighter.update(2);
    assert(player.updateCalls == 2 && !player.lastDoSound);
    fighter.fov = 0;
    fighter.state = 5;
    fighter.update(3);
    assert(player.updateCalls == 3 && !player.lastDoSound);
    int stopBefore = player.stopSoundCalls;
    fighter.state = 3;
    fighter.update(3);
    assert(player.stopSoundCalls == stopBefore + 1 && player.updateCalls == 3);

    fighter.state = 0;
    fighter.wingmanFlag = 1;
    fighter.routeClone = &route;
    fighter.field_0xe4 = 0;
    ego.geometry = &geometry;
    geometry.position = {4.0f, 5.0f, 6.0f};
    fighter.update(4);
    assert(fighter.targetPos.x == 4.0f && fighter.targetPos.y == 5.0f && fighter.targetPos.z == 6.0f);
    assert(player.updateCalls == 4 && player.enemiesCalls == 4);

    fighter.field_0xe4 = 1;
    geometry.position = {7.0f, 8.0f, 9.0f};
    fighter.update(5);
    assert(fighter.targetPos.x == 4007.0f && fighter.targetPos.y == 8.0f && fighter.targetPos.z == -2991.0f);
    assert(fighter.objectPosition.x == 4000.0f && fighter.objectPosition.y == 0.0f);
    assert(route.lengthCalls == 1 && route.setCoordsCalls == 1 && route.looping);
    assert(player.updateCalls == 5 && player.enemiesCalls == 5);

    fighter.wingmanCommand = 0;
    geometry.position = {10.0f, 20.0f, 30.0f};
    fighter.update(6);
    assert(fighter.targetPos.x == -3990.0f && fighter.targetPos.z == -2970.0f);
    fighter.wingmanCommand = 2;
    fighter.update(7);
    assert(fighter.targetPos.x == 10.0f && fighter.targetPos.y == 2020.0f && fighter.targetPos.z == -1970.0f);
    assert(fighter.objectPosition.y == 2000.0f && geometry.upCalls == 1);
    fighter.wingmanCommand = 3;
    fighter.update(8);
    assert(fighter.targetPos.x == 10.0f && fighter.targetPos.y == 20.0f && fighter.targetPos.z == 30.0f);
    assert(geometry.directionCalls == 3 && route.setCoordsCalls == 4 && route.loopCalls == 4);

    int firstCoords[6] = {0, 0, 0, 1, 1, 1};
    fighter.routeClone = new Route(firstCoords, 6);
    int destroyedBefore = Route::destroyed;
    int constructedBefore = Route::constructed;
    geometry.position = {10.9f, 20.9f, -30.9f};
    fighter.update(9);
    assert(Route::destroyed == destroyedBefore + 1 && Route::constructed == constructedBefore + 1);
    assert(fighter.routeClone->points == 1 && fighter.routeClone->coords.x == 10.0f);
    assert(fighter.routeClone->coords.y == 20.0f && fighter.routeClone->coords.z == -30.0f);
    assert(fighter.routeClone->looping);
    delete fighter.routeClone;
    fighter.routeClone = nullptr;
}
