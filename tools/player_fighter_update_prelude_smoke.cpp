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
Vector operator-(Vector a, const Vector &b) { return a -= b; }
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
    int visibleCalls = 0;
    bool visible = false;
    int rightCalls = 0, upCalls = 0, directionCalls = 0;
    Vector getPosition() { return position; }
    Vector getRightVector() { ++rightCalls; return right; }
    Vector getUpVector() { ++upCalls; return up; }
    Vector getDirection() { ++directionCalls; return direction; }
    Matrix &getMatrix() { ++matrixCalls; return matrix; }
    void setVisible(bool value) { ++visibleCalls; visible = value; }
};
struct Explosion { bool playing = false; bool isPlaying() { return playing; } };
template<class T> struct Array {
    unsigned int count = 0;
    T *items = nullptr;
    unsigned int size() const { return count; }
    T &operator[](unsigned int i) { return items[i]; }
};
struct KIPlayer;
struct Player {
    bool active = true;
    bool dead = false;
    Vector position{};
    uint8_t field_69 = 0;
    KIPlayer *kiPlayer = nullptr;
    Array<Player *> *enemies = nullptr;
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
    bool neverAttack = false;
    bool turnedEnemyFlag = false;
    int gunChecks = 0;
    int updateCalls = 0;
    int stopSoundCalls = 0;
    int enemiesCalls = 0;
    int activeChecks = 0;
    int deadChecks = 0;
    int positionCalls = 0;
    int getKIPlayerCalls = 0;
    int getEnemyCalls = 0;
    int neverAttackCalls = 0;
    int setActiveCalls = 0;
    bool lastDoSound = false;
    int lastDt = 0;
    int getMaxHitpoints() { return maxHitpoints; }
    void setActive(bool value) { ++setActiveCalls; active = value; }
    bool gunAvailable(int slot) { assert(slot == 1); ++gunChecks; return gunAvailableResult; }
    bool isAlwaysEnemy() { return alwaysEnemy; }
    void setAlwaysEnemy(bool value) {
        alwaysEnemy = value;
        enemyFlags = 1;
        turnedEnemyFlag = value;
    }
    bool turnedEnemy() { return turnedEnemyFlag; }
    bool isAlwaysFriend() { return alwaysFriend; }
    bool isActive() { ++activeChecks; return active; }
    bool isDead() { ++deadChecks; return dead; }
    Vector getPosition() { ++positionCalls; return position; }
    KIPlayer *getKIPlayer() { ++getKIPlayerCalls; return kiPlayer; }
    Player *getEnemy(int index) {
        ++getEnemyCalls;
        assert(enemies && index >= 0 && static_cast<unsigned int>(index) < enemies->size());
        return (*enemies)[static_cast<unsigned int>(index)];
    }
    bool doesNeverAttack() { ++neverAttackCalls; return neverAttack; }
    void update(int dt, bool doSound) {
        ++updateCalls;
        lastDt = dt;
        lastDoSound = doSound;
    }
    void StopEngineSound() { ++stopSoundCalls; }
    Array<Player *> *getEnemies() { ++enemiesCalls; return enemies; }
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
struct AERandom {
    int values[32]{};
    int used = 0;
    int bounds[32]{};
    int nextInt(int bound) {
        assert(used < 32);
        int value = values[used];
        bounds[used++] = bound;
        assert(value >= 0 && value < bound);
        return value;
    }
};
struct Globals { static Status *status; static void *rnd; };
Status *Globals::status = nullptr;
void *Globals::rnd = nullptr;

struct Radar { KIPlayer *lockedEnemy = nullptr; };
struct TargetFollowCamera {
    AEGeometry *target = nullptr;
    int targetCalls = 0;
    AEGeometry *getTarget() { ++targetCalls; return target; }
};
struct PlayerEgo {
    void *field_0x14 = nullptr;
    AEGeometry *geometry = nullptr;
    Player *player = nullptr;
    TargetFollowCamera *camera = nullptr;
    bool rocketControl = false;
    int rocketChecks = 0, cameraCalls = 0;
    bool isInRocketControl() { ++rocketChecks; return rocketControl; }
    TargetFollowCamera *getTargetFollowCamera() { ++cameraCalls; return camera; }
};
struct Level {
    PlayerEgo *ego = nullptr;
    int getPlayerCalls = 0;
    int uncovered = -1;
    PlayerEgo *getPlayer() { ++getPlayerCalls; return ego; }
    void uncoverWanted(int index) { uncovered = index; }
};
struct Waypoint { int x, y, z; };
struct Route {
    static int constructed, destroyed;
    int cloneCalls = 0;
    int points = 1;
    int lengthCalls = 0, setCoordsCalls = 0, loopCalls = 0;
    int updateCalls = 0, waypointCalls = 0, currentCalls = 0;
    int dockingTargetCalls = 0;
    bool looping = false;
    Vector coords{};
    Vector lastPosition{};
    Waypoint *waypoint = nullptr;
    KIPlayer *dockingTarget = nullptr;
    int current = 0;
    Route() = default;
    Route(int *v, int count) : points(count / 3), coords{float(v[0]), float(v[1]), float(v[2])} { ++constructed; }
    ~Route() { ++destroyed; }
    Route *clone() { ++cloneCalls; return this; }
    int length() { ++lengthCalls; return points; }
    void setNewCoords(Vector v) { ++setCoordsCalls; coords = v; }
    void setLoop(bool value) { ++loopCalls; looping = value; }
    void update(const Vector &v) { ++updateCalls; lastPosition = v; }
    Waypoint *getWaypoint() { ++waypointCalls; return waypoint; }
    int getCurrent() { ++currentCalls; return current; }
    KIPlayer *getDockingTarget() { ++dockingTargetCalls; return dockingTarget; }
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
    uint8_t jumperFlag = 0;
    int field_0xf0 = 0;
    int wingmanCommand = 1;
    int field_0xe4 = 0;
    KIPlayer *wingmanTarget = nullptr;
    int setWingmanCommandCalls = 0;
    int fov = 0;
    Vector targetPos{};
    Vector objectPosition{};
    Vector resetVecB{};
    Vector resetVecC{};
    int field_0x38 = 0;
    bool wingman = false;
    void setActive(bool value) { player->setActive(value); }
    bool isWingMan() { return wingman; }
    bool isDead() { return state == 4; }
    virtual void setWingmanCommand(int command, KIPlayer *target) {
        ++setWingmanCommandCalls;
        field_0xe4 = command;
        wingmanTarget = target;
    }
};
struct PlayerFighter : KIPlayer {
    Route *route = nullptr;
    Route *commandRoute = nullptr;
    Trail *trail = nullptr;
    Vector workingPosition{};
    int field_0x1c8 = 0;
    uint8_t field_0x12c = 0;
    uint8_t field_0x12e = 0;
    uint8_t field_0x12d = 0;
    int field_0x128 = 0;
    int field_0x1e4 = 0;
    Player *field_0x148 = nullptr;
    uint8_t aiDisabled = 0;
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
    Player egoBody;
    egoBody.position = {1000000.0f, 1000000.0f, 1000000.0f};
    ego.player = &egoBody;
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

    Player enemyA, enemyB;
    KIPlayer enemyBOwner;
    enemyBOwner.shipGroup = 3;
    enemyB.kiPlayer = &enemyBOwner;
    Player *enemyItems[2] = {&enemyA, &enemyB};
    Array<Player *> enemyList;
    enemyList.count = 2;
    enemyList.items = enemyItems;
    player.enemies = &enemyList;
    fighter.wingmanFlag = 1;
    fighter.field_0xe4 = 3;
    fighter.field_0x38 = 9;
    fighter.field_0x12e = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == 0 && fighter.field_0x12e == 1);
    assert(fighter.field_0x148 == &enemyA);
    assert(enemyA.activeChecks == 0);

    fighter.field_0x38 = 0;
    enemyA.active = false;
    fighter.update(1);
    assert(fighter.field_0x38 == 1 && fighter.field_0x12e == 0);
    assert(enemyA.activeChecks == 2 && enemyB.activeChecks == 1);
    fighter.field_0x38 = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == 0 && enemyB.activeChecks == 2);

    fighter.field_0x38 = 1;
    fighter.field_0x12e = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == 1 && fighter.field_0x12e == 1);
    assert(enemyB.activeChecks == 3);

    fighter.aiDisabled = 1;
    fighter.field_0x38 = 9;
    fighter.field_0x12e = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == 9 && fighter.field_0x12e == 1);
    fighter.aiDisabled = 0;

    player.enemies = nullptr;
    fighter.wingmanFlag = 0;
    fighter.routeClone = &route;
    int updateBefore = route.updateCalls;
    int waypointBefore = route.waypointCalls;
    geometry.position = {11.0f, 22.0f, 33.0f};
    fighter.update(1);
    assert(route.updateCalls == updateBefore + 1 && route.waypointCalls == waypointBefore + 1);
    assert(route.lastPosition.x == 11.0f && fighter.state == 0 && fighter.field_0x12c == 0);

    Waypoint waypoint{101, -202, 303};
    route.waypoint = &waypoint;
    waypointBefore = route.waypointCalls;
    fighter.update(1);
    assert(route.waypointCalls == waypointBefore + 4 && fighter.field_0x12c == 1);
    assert(fighter.resetVecB.x == 101.0f && fighter.resetVecB.y == -202.0f);
    assert(fighter.resetVecB.z == 303.0f);

    fighter.wingmanFlag = 1;
    fighter.routeClone = nullptr;
    fighter.state = 0;
    fighter.update(1);
    assert(fighter.state == 5);
    fighter.aiDisabled = 1;
    fighter.state = 0;
    fighter.update(1);
    assert(fighter.state == 0);

    fighter.aiDisabled = 0;
    player.enemies = &enemyList;
    fighter.field_0x128 = 100;
    fighter.field_0x12e = 0;
    fighter.field_0x148 = &enemyA;
    fighter.maneuverTimer = 0;
    fighter.shipGroup = 8;
    player.turnedEnemyFlag = false;
    enemyA.active = true;
    enemyB.active = true;
    enemyA.position = {300.0f, 22.0f, 33.0f};
    enemyB.position = {20.0f, 22.0f, 33.0f};
    fighter.update(1);
    assert(fighter.field_0x38 == 1 && fighter.field_0x12e == 1);
    assert(fighter.targetPos.x == 20.0f && fighter.field_0x148 == &enemyB);

    fighter.field_0x12e = 0;
    fighter.shipGroup = 2;
    player.alwaysFriend = false;
    player.turnedEnemyFlag = true;
    fighter.maneuverTimer = 0;
    fighter.update(1);
    assert(fighter.field_0x38 == 0 && fighter.field_0x12e == 1);
    assert(fighter.targetPos.x == 300.0f);
    fighter.shipGroup = 8;
    player.turnedEnemyFlag = false;

    AERandom random;
    Globals::rnd = &random;
    fighter.maneuverTimer = 5000;
    fighter.field_0x12d = 0;
    random.values[0] = 19;
    random.values[1] = 30;
    fighter.update(1);
    assert(fighter.maneuverTimer == 0 && fighter.field_0x12d == 1);
    assert(fighter.field_0x38 == 0 && fighter.field_0x12e == 1 && random.used == 2);
    assert(fighter.field_0x148 == &enemyA && fighter.resetVecB.x == 300.0f);
    assert(random.bounds[0] == 100 && random.bounds[1] == 100);

    random.used = 0;
    random.values[0] = 29;
    random.values[1] = 0;
    random.values[2] = 1;
    fighter.maneuverTimer = 5000;
    fighter.field_0x12d = 1;
    enemyA.active = false;
    fighter.update(1);
    assert(fighter.field_0x12d == 0 && fighter.field_0x12e == 1);
    assert(fighter.field_0x38 == 1 && fighter.targetPos.x == 20.0f);
    assert(random.used == 3 && random.bounds[0] == 100);
    assert(random.bounds[1] == 2 && random.bounds[2] == 2);

    random.used = 0;
    random.values[0] = 0;
    random.values[1] = 0;
    random.values[2] = 0;
    random.values[3] = 0;
    random.values[4] = 0;
    random.values[5] = 0;
    fighter.maneuverTimer = 5000;
    fighter.field_0x12d = 1;
    enemyB.active = false;
    fighter.update(1);
    assert(fighter.field_0x38 == 0 && fighter.field_0x12e == 0);
    assert(random.used == 6 && random.bounds[5] == 2);

    KIPlayer wingmanTarget;
    wingmanTarget.player = &enemyB;
    fighter.wingmanFlag = 1;
    fighter.field_0xe4 = 3;
    fighter.wingmanTarget = &wingmanTarget;
    fighter.field_0x12e = 0;
    fighter.maneuverTimer = 5000;
    enemyA.active = true;
    enemyB.active = true;
    random.used = 0;
    random.values[0] = 30;
    fighter.update(1);
    assert(fighter.field_0x38 == 1 && fighter.targetPos.x == 20.0f);
    assert(fighter.field_0x12e == 0 && fighter.setWingmanCommandCalls == 0);

    enemyB.alwaysFriend = true;
    fighter.maneuverTimer = 5000;
    random.used = 0;
    fighter.update(1);
    assert(fighter.field_0x38 == 0 && fighter.field_0xe4 == 3);
    enemyB.alwaysFriend = false;

    wingmanTarget.state = 4;
    fighter.maneuverTimer = 5000;
    random.used = 0;
    fighter.update(1);
    assert(fighter.setWingmanCommandCalls == 1 && fighter.field_0xe4 == 1);
    assert(fighter.wingmanTarget == nullptr && fighter.field_0x38 == 0);

    fighter.wingmanFlag = 0;
    fighter.maneuverTimer = 5000;
    fighter.field_0x12e = 1;
    enemyA.dead = true;
    random.used = 0;
    fighter.update(1);
    assert(fighter.field_0x12e == 0 && fighter.field_0x38 == -1);

    enemyA.dead = false;
    enemyA.position = {11.0f, 22.0f, 33.0f};
    fighter.field_0x43 = 0;
    fighter.shipGroup = 2;
    standing.enemy = false;
    player.alwaysFriend = false;
    player.turnedEnemyFlag = false;
    fighter.maneuverTimer = 0;
    fighter.wingmanFlag = 1;
    fighter.field_0xe4 = 3;
    fighter.update(1);
    assert(player.enemyFlagsLo == 0);
    assert(fighter.field_0x12e == 0 && fighter.field_0x38 == 1);

    fighter.wingmanFlag = 0;
    fighter.field_0x43 = 1;
    enemyB.active = true;
    enemyB.dead = false;
    enemyB.field_69 = 0;
    struct FactionCase { int target, own; bool selected; };
    const FactionCase factions[] = {
        {8, 2, true}, {8, 8, false}, {2, 8, true},
        {9, 2, true}, {9, 9, false}, {2, 9, true},
        {0, 1, true}, {1, 0, true}, {3, 2, true}, {2, 3, true},
        {10, 2, true}, {10, 10, false}, {2, 10, true}, {2, 2, false},
    };
    for (const FactionCase &test : factions) {
        fighter.shipGroup = test.own;
        enemyBOwner.shipGroup = test.target;
        fighter.field_0x38 = 1;
        fighter.field_0x12e = 1;
        fighter.field_0x12c = 1;
        fighter.maneuverTimer = 0;
        int calls = enemyB.getKIPlayerCalls;
        fighter.update(1);
        assert(fighter.field_0x38 == (test.selected ? 1 : -1));
        assert(fighter.field_0x12e == 1);
        assert(fighter.field_0x12c == (test.selected ? 0 : 1));
        if (!test.selected)
            assert(fighter.resetVecB.x == 101.0f && fighter.resetVecB.y == -202.0f);
        assert(enemyB.getKIPlayerCalls == calls + 2);
    }

    fighter.shipGroup = 1;
    enemyB.kiPlayer = nullptr;
    fighter.field_0x38 = 1;
    fighter.field_0x12e = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == -1);
    enemyB.field_69 = 1;
    fighter.field_0x38 = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == 1 && fighter.field_0x12e == 1);

    enemyB.kiPlayer = &enemyBOwner;
    enemyB.dead = true;
    fighter.field_0x38 = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == -1 && fighter.field_0x12e == 1);
    enemyB.dead = false;

    fighter.wingmanFlag = 1;
    fighter.field_0xe4 = 1;
    fighter.wingmanTarget = &enemyBOwner;
    enemyBOwner.player = &enemyB;
    enemyB.kiPlayer = &enemyBOwner;
    enemyB.field_69 = 0;
    enemyB.enemyFlagsLo = 0;
    fighter.field_0x38 = 1;
    fighter.field_0x12e = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == 1);
    fighter.wingmanTarget = nullptr;
    fighter.field_0x38 = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == -1);
    enemyB.enemyFlagsLo = 1;
    fighter.field_0x38 = 1;
    fighter.update(1);
    assert(fighter.field_0x38 == 1);

    fighter.wingmanFlag = 0;
    fighter.routeClone = &route;
    fighter.field_0x38 = -1;
    fighter.field_0x12e = 1;
    fighter.maneuverTimer = 0;
    route.waypoint = nullptr;
    int neverAttackBefore = player.neverAttackCalls;
    fighter.update(1);
    assert(fighter.field_0x38 == 0 && fighter.field_0x148 == &enemyA);
    assert(fighter.targetPos.x == enemyA.position.x && fighter.resetVecB.x == enemyA.position.x);
    assert(player.neverAttackCalls == neverAttackBefore);

    route.waypoint = &waypoint;
    player.active = false;
    fighter.field_0x38 = -1;
    fighter.update(1);
    assert(fighter.field_0x38 == 0 && fighter.field_0x148 == &enemyA);
    player.active = true;

    player.neverAttack = true;
    fighter.shipGroup = 2;
    enemyBOwner.shipGroup = 3;
    fighter.field_0x38 = 1;
    fighter.field_0x12e = 1;
    fighter.maneuverTimer = 0;
    neverAttackBefore = player.neverAttackCalls;
    fighter.update(1);
    assert(player.neverAttackCalls == neverAttackBefore + 1);
    assert(fighter.field_0x38 == 1 && fighter.field_0x12c == 1);
    assert(fighter.resetVecB.x == 101.0f && fighter.resetVecB.z == 303.0f);

    player.neverAttack = false;
    route.dockingTarget = &enemyBOwner;
    fighter.state = 1;
    fighter.field_0x38 = -1;
    fighter.field_0x12e = 1;
    int dockingCalls = route.dockingTargetCalls;
    fighter.update(1);
    assert(route.dockingTargetCalls == dockingCalls + 1);
    assert(fighter.state == 7 && fighter.field_0x12c == 1);

    fighter.wingmanFlag = 0;
    fighter.aiDisabled = 0;
    fighter.state = 0;
    fighter.routeClone = &route;
    route.waypoint = nullptr;
    player.enemies = nullptr;
    fighter.jumperFlag = 1;
    fighter.field_0x12c = 1;
    fighter.field_0xf0 = 19998;
    fighter.update(1);
    assert(fighter.state == 0 && fighter.field_0xf0 == 19999 && fighter.field_0x12c == 1);
    fighter.update(1);
    assert(fighter.state == 6 && fighter.field_0xf0 == 0 && fighter.field_0x12c == 0);

    fighter.state = 0;
    fighter.jumperFlag = 0;
    fighter.field_0x12c = 1;
    fighter.field_0xf0 = 10;
    fighter.update(1);
    assert(fighter.field_0xf0 == 0);
    fighter.jumperFlag = 1;
    fighter.field_0x12c = 0;
    fighter.field_0xf0 = 10;
    fighter.update(1);
    assert(fighter.field_0xf0 == 0);

    fighter.aiDisabled = 1;
    fighter.field_0x12c = 1;
    fighter.field_0xf0 = 17;
    fighter.update(1);
    assert(fighter.field_0xf0 == 17);

    fighter.wingmanFlag = 1;
    fighter.routeClone = nullptr;
    fighter.field_0xe4 = 2;
    fighter.field_0x1e4 = 2;
    auto *commandRoute = new Route();
    commandRoute->current = 2;
    fighter.commandRoute = commandRoute;
    fighter.field_0x12c = 0;
    fighter.update(1);
    assert(fighter.field_0x12c == 0 && fighter.commandRoute == commandRoute);
    commandRoute->waypoint = &waypoint;
    fighter.update(1);
    assert(commandRoute->updateCalls == 2 && commandRoute->currentCalls == 2);
    assert(fighter.commandRoute == commandRoute && fighter.field_0x12c == 1);
    assert(fighter.resetVecB.x == 101.0f && fighter.resetVecB.z == 303.0f);
    assert(fighter.resetVecC.x == 90.0f && fighter.resetVecC.z == 270.0f);

    int commandRouteDestroyedBefore = Route::destroyed;
    commandRoute->current = 3;
    fighter.update(1);
    assert(Route::destroyed == commandRouteDestroyedBefore + 1);
    assert(fighter.commandRoute == nullptr && fighter.field_0xe4 == 1);

    int commandsBefore = fighter.setWingmanCommandCalls;
    fighter.field_0xe4 = 2;
    fighter.update(1);
    assert(fighter.setWingmanCommandCalls == commandsBefore + 1 && fighter.field_0xe4 == 1);

    fighter.wingmanFlag = 0;
    fighter.state = 3;
    fighter.resetVecB = {10.0f, 20.0f, 30.0f};
    fighter.update(1);
    assert(fighter.resetVecC.x == -1.0f && fighter.resetVecC.y == -2.0f &&
           fighter.resetVecC.z == -3.0f);

    fighter.state = 5;
    fighter.aiDisabled = 1;
    fighter.field_0x42 = 0;
    fighter.field_0x43 = 1;
    fighter.field_0x128 = 1;
    fighter.subGeometry = nullptr;
    player.enemyFlagsLo = 1;
    player.active = false;
    egoBody.position = {20.0f, 30.0f, 40.0f};
    int playerLookups = level.getPlayerCalls;
    int activeCalls = player.setActiveCalls;
    fighter.update(1);
    assert(level.getPlayerCalls == playerLookups + 2 && ego.rocketChecks >= 1);
    assert(fighter.state == 1 && player.active && player.setActiveCalls == activeCalls + 1);
    assert(geometry.visible && fighter.targetPos.x == 20.0f);
    assert(fighter.resetVecC.x == 9.0f && fighter.resetVecC.y == 8.0f &&
           fighter.resetVecC.z == 7.0f);

    AEGeometry cameraGeometry;
    cameraGeometry.position = {30.0f, 40.0f, 50.0f};
    TargetFollowCamera camera{&cameraGeometry};
    ego.camera = &camera;
    ego.rocketControl = true;
    fighter.state = 5;
    fighter.subGeometry = &secondary;
    secondary.visible = false;
    playerLookups = level.getPlayerCalls;
    fighter.update(1);
    assert(level.getPlayerCalls == playerLookups + 2 && ego.cameraCalls == 1);
    assert(camera.targetCalls == 1 && fighter.targetPos.x == 30.0f);
    assert(fighter.resetVecC.x == 19.0f && secondary.visible && fighter.state == 1);

    ego.rocketControl = false;
    fighter.subGeometry = nullptr;
    fighter.state = 5;
    egoBody.position = {25011.0f, 22.0f, 33.0f};
    fighter.update(1);
    assert(fighter.state == 5 && fighter.resetVecC.x == 25000.0f);

    egoBody.position = {-24989.0f, 22.0f, 33.0f};
    fighter.update(1);
    assert(fighter.state == 5 && fighter.resetVecC.x == -25000.0f);

    egoBody.position = {11.0f, 25022.0f, 33.0f};
    fighter.update(1);
    assert(fighter.state == 5 && fighter.resetVecC.y == 25000.0f);

    egoBody.position = {11.0f, 22.0f, 25033.0f};
    fighter.update(1);
    assert(fighter.state == 5 && fighter.resetVecC.z == 25000.0f);

    egoBody.position = {11.0f, 22.0f, -49967.0f};
    fighter.update(1);
    assert(fighter.state == 1 && fighter.resetVecC.z == -50000.0f);

    player.damageDoneByPlayer = 0;
    player.enemyFlagsLo = 0;
    fighter.state = 5;
    int rocketChecks = ego.rocketChecks;
    fighter.update(1);
    assert(fighter.state == 5 && ego.rocketChecks == rocketChecks);

    player.enemyFlagsLo = 1;
    fighter.field_0x128 = 0;
    fighter.update(1);
    assert(fighter.state == 5 && ego.rocketChecks == rocketChecks);

    fighter.field_0x128 = 1;
    fighter.state = 0;
    fighter.wingmanFlag = 1;
    fighter.field_0xe4 = 1;
    egoBody.position = {20.0f, 30.0f, 40.0f};
    fighter.update(1);
    assert(fighter.state == 1 && fighter.targetPos.x == 20.0f);
}
