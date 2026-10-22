#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>

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
Matrix operator*(Matrix lhs, const Matrix &rhs) { return lhs *= rhs; }
namespace AbyssEngine { namespace AEMath {
Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z) {
    (void)x; (void)y; (void)z;
    matrix.value = 7;
    return matrix;
}
} }
struct AEGeometry {
    int *destroyCounter = nullptr;
    Vector position{1.25f, -2.5f, 3.75f};
    Vector right{1.0f, 0.0f, 0.0f};
    Vector up{0.0f, 1.0f, 0.0f};
    Vector direction{0.0f, 0.0f, 1.0f};
    Matrix matrix{2};
    int matrixCalls = 0;
    int visibleCalls = 0;
    bool visible = false;
    int rightCalls = 0, upCalls = 0, directionCalls = 0;
    int setMatrixCalls = 0, translateCalls = 0, referenceCalls = 0;
    int rotateCalls = 0;
    float lastRotation = 0.0f;
    Vector lastTranslation{};
    ~AEGeometry() { if (destroyCounter) ++*destroyCounter; }
    Vector getPosition() { return position; }
    Vector getRightVector() { ++rightCalls; return right; }
    Vector getUpVector() { ++upCalls; return up; }
    Vector getDirection() { ++directionCalls; return direction; }
    Matrix &getMatrix() { ++matrixCalls; return matrix; }
    void setMatrix(const Matrix &value) { ++setMatrixCalls; matrix = value; }
    void translate(const Vector &value) {
        ++translateCalls; lastTranslation = value; position += value;
    }
    void updateReferenceMatrix() { ++referenceCalls; }
    void rotate(float x, float y, float z) {
        ++rotateCalls; assert(x == y && y == z); lastRotation = x;
    }
    void setVisible(bool value) { ++visibleCalls; visible = value; }
};
struct TargetFollowCamera;
struct Explosion {
    bool playing = false;
    bool endOnUpdate = false;
    int startCalls = 0;
    int updateCalls = 0, lastDt = 0;
    TargetFollowCamera *lastCamera = nullptr;
    Vector startPosition{}, startDirection{};
    bool isPlaying() { return playing; }
    void start(const Vector &position, const Vector &direction) {
        ++startCalls; startPosition = position; startDirection = direction;
    }
    void update(int dt, TargetFollowCamera *camera) {
        ++updateCalls; lastDt = dt; lastCamera = camera;
        if (endOnUpdate) playing = false;
    }
};
struct SpacePoint {
    bool free = false;
    int isFreeCalls = 0, giveFreeCalls = 0;
    bool isFree() { ++isFreeCalls; return free; }
    void giveFree() { ++giveFreeCalls; free = true; }
};
struct BoundingVolume {
    int calls = 0;
    Vector position{};
    virtual void update(float x, float y, float z) {
        ++calls;
        position = {x, y, z};
    }
};
struct ParticleSystemManager {
    int calls = 0;
    int lastHandle = -1;
    bool lastEnabled = false;
    int handles[64]{};
    bool enabled[64]{};
    int manualCalls = 0, manualHandle = -1, manualSet = -1;
    Vector manualPosition{}, manualVelocity{};
    float manualLifetime = 0.0f;
    void enableSystemEmit(int handle, bool enabled) {
        assert(calls < 64);
        handles[calls] = handle;
        this->enabled[calls] = enabled;
        ++calls;
        lastHandle = handle;
        lastEnabled = enabled;
    }
    void emitManual(int handle, const Vector &position, int particleSet,
                    const Vector &velocity, float lifetime) {
        ++manualCalls;
        manualHandle = handle;
        manualPosition = position;
        manualSet = particleSet;
        manualVelocity = velocity;
        manualLifetime = lifetime;
    }
};
struct String {
    const char *data = nullptr;
    int compareCalls = 0;
    int Compare(const String &other) {
        ++compareCalls;
        return std::strcmp(data, other.data);
    }
};
struct GameText {
    String missionName{"escort"};
    int getTextCalls = 0;
    String *getText(int key) {
        assert(key == 1663);
        ++getTextCalls;
        return &missionName;
    }
};
struct Mission {
    int type = 13;
    int targetStation = 7;
    int getTypeCalls = 0;
    int getType() { ++getTypeCalls; return type; }
    int getTargetStation() { return targetStation; }
};
struct Station {
    int index = 7;
    int getIndex() { return index; }
};
template<class T> struct Array {
    unsigned int count = 0;
    T *items = nullptr;
    unsigned int size() const { return count; }
    T &operator[](unsigned int i) { return items[i]; }
};
template<class T> void ArrayAdd(T value, Array<T> &array) {
    T *next = new T[array.count + 1];
    for (unsigned int i = 0; i < array.count; ++i) next[i] = array.items[i];
    delete[] array.items;
    array.items = next;
    next[array.count++] = value;
}
struct KIPlayer;
struct Player {
    bool active = true;
    bool dead = false;
    Vector position{};
    uint8_t field_69 = 0;
    uint8_t field_5e = 0;
    uint8_t destroyedByte = 0;
    KIPlayer *kiPlayer = nullptr;
    Array<Player *> *enemies = nullptr;
    Matrix transformMatrix{};
    union {
        uint16_t enemyFlags = 0x707;
        struct { uint8_t enemyFlagsLo, carriesFriendCargoFlag; };
    };
    int damageDoneByPlayer = 0;
    int maxHitpoints = 100;
    int hitpoints = 100;
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
    int bombForceCalls = 0;
    int getBombForceCalls = 0;
    float bombForce = 0.0f;
    int getMaxHitpoints() { return maxHitpoints; }
    int getHitpoints() { return hitpoints; }
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
    void setBombForce(float value) { ++bombForceCalls; bombForce = value; }
    float getBombForce() { ++getBombForceCalls; return bombForce; }
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
struct Wanted {
    int reward = 75;
    int board = 2;
    int terminateCalls = 0, activeCalls = 0, rewardCalls = 0;
    bool terminated = false, active = true;
    void setTerminated(bool value) { ++terminateCalls; terminated = value; }
    void setActive(bool value) { ++activeCalls; active = value; }
    int getReward() { ++rewardCalls; return reward; }
    int getBoard() { return board; }
};
struct Layout {
    int calls = 0, lastReward = -1;
    bool lastFlag = false;
    void showMissionRewardMessage(int reward, bool flag) {
        ++calls; lastReward = reward; lastFlag = flag;
    }
};
struct FModSound {
    int calls = 0, lastId = -1;
    Vector *lastPosition = nullptr;
    void play(int id, Vector *position, Vector *velocity, float pitch) {
        assert(velocity == nullptr && pitch == 0.0f);
        ++calls; lastId = id; lastPosition = position;
    }
};
struct Status {
    Standing *standing;
    int currentCampaignMission = 0;
    int campaignCalls = 0;
    Array<Wanted *> *wanted = nullptr;
    int credits = 0;
    int collectedCalls = 0, lastBoard = -1, pirateKills = 0;
    Mission *freelance = nullptr;
    Station *station = nullptr;
    uint8_t field_0xf0 = 0;
    uint8_t field_0xf1 = 0;
    bool blackMarket = false;
    uint8_t field_110 = 0;
    Standing *getStanding() { return standing; }
    int getCurrentCampaignMission() { ++campaignCalls; return currentCampaignMission; }
    Mission *getFreelanceMission() { return freelance; }
    Station *getStation() { return station; }
    int getCredits() { return credits; }
    void setCredits(int value) { credits = value; }
    void incCollectedBounties(int board) { ++collectedCalls; lastBoard = board; }
    void incPirateKills() { ++pirateKills; }
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
struct Globals { static Status *status; static void *rnd; static unsigned char options[100]; static GameText *gameText; static FModSound *sound; static Layout *layout; };
Status *Globals::status = nullptr;
void *Globals::rnd = nullptr;
unsigned char Globals::options[100]{};
GameText *Globals::gameText = nullptr;
FModSound *Globals::sound = nullptr;
Layout *Globals::layout = nullptr;

struct Radar { KIPlayer *lockedEnemy = nullptr; KIPlayer *field_0x1c = nullptr; };
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
    ParticleSystemManager *particleEmitBoolPtr = nullptr;
    ParticleSystemManager *particleRenderBoolPtr = nullptr;
    ParticleSystemManager *field_74 = nullptr;
    ParticleSystemManager *field_8c = nullptr;
    int field_3c = 55;
    int friendDeaths = 0, enemyDeaths = 0, wingmanDeaths = 0;
    int lastEnemyKind = -1;
    bool lastEnemyDestroyed = false;
    int getPlayerCalls = 0;
    int uncovered = -1;
    int pirateStationCalls = 0;
    bool lastPirateStationAction = false;
    PlayerEgo *getPlayer() { ++getPlayerCalls; return ego; }
    void uncoverWanted(int index) { uncovered = index; }
    void pirateStationAction(bool active) {
        ++pirateStationCalls; lastPirateStationAction = active;
    }
    void friendDied() { ++friendDeaths; }
    void wingmanDied(const String &) { ++wingmanDeaths; }
    void enemyDied(int kind, bool destroyed) {
        ++enemyDeaths; lastEnemyKind = kind; lastEnemyDestroyed = destroyed;
    }
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
    String name{};
    AEGeometry *geometry = nullptr;
    Explosion *explosion = nullptr;
    int state = 0;
    int field_0x80 = 0;
    int field_0x84 = 0;
    uint8_t crateCaptured = 1;
    int deathTimer = 0;
    int field_0x1c0 = 0;
    int maneuverTimer = 0;
    int shipGroup = 0;
    int kind = 19;
    Array<int> *cargo = nullptr;
    uint8_t isMissionCrate = 0;
    AEGeometry *crateGeometry = nullptr;
    uint8_t missionCrateCaptured = 0, missionCrateLost = 0;
    uint8_t finished = 0;
    int crateCalls = 0, lastCrateType = -1;
    uint8_t field_0x73 = 0;
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
    bool isDocked() { return state == 9; }
    int cargoAvailable() {
        if (!cargo) return 0;
        for (unsigned int i = 1; i < cargo->size(); i += 2)
            if ((*cargo)[i] > 0) return 1;
        return 0;
    }
    void createCrate(int type) { ++crateCalls; lastCrateType = type; }
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
    Array<BoundingVolume *> *boundingVolumes = nullptr;
    signed char field_0x1fc = 0;
    int field_0x130 = 0, field_0x134 = 0, field_0x138 = 0;
    int engineTrailSystem = 0, field_0x1f8 = 0;
    Vector deathDrift{}, deathDirection{};
    Vector postExplosionDirection{};
    SpacePoint *field_0x2c4 = nullptr;
    signed char field_0x13e = 0;
    float currentSpeed = 0.0f;
    int cloakingCalls = 0;
    void handleCloaking() { ++cloakingCalls; }
    int exhaustCalls = 0;
    bool exhaustVisible = true;
    void setExhaustVisible(bool visible) { ++exhaustCalls; exhaustVisible = visible; }
    Vector workingPosition{};
    int field_0x1c8 = 0;
    uint8_t field_0x12c = 0;
    uint8_t field_0x12e = 0;
    uint8_t field_0x12d = 0;
    signed char field_0x12f = 0;
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
    fighter.aiDisabled = 1;
    fighter.update(7);
    assert(player.active && bodyCalls == 0);
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
    assert(!player.active && bodyCalls == 0);

    player.active = true;
    fighter.state = 0;
    fighter.aiDisabled = 0;
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
    fighter.aiDisabled = 1;
    fighter.update(3);
    assert(player.stopSoundCalls == stopBefore + 1 && player.updateCalls == 3);

    fighter.state = 0;
    fighter.aiDisabled = 0;
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
    fighter.aiDisabled = 1;
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

    fighter.aiDisabled = 1;
    fighter.wingmanFlag = 0;
    fighter.field_0x128 = 0;
    fighter.state = 0;
    BoundingVolume firstVolume, secondVolume;
    BoundingVolume *volumes[] = {&firstVolume, &secondVolume};
    Array<BoundingVolume *> volumeArray{2, volumes};
    fighter.boundingVolumes = &volumeArray;
    fighter.update(1);
    assert(firstVolume.calls == 1 && secondVolume.calls == 1);
    assert(firstVolume.position.x == fighter.renderPosition.x &&
           firstVolume.position.y == fighter.renderPosition.y &&
           firstVolume.position.z == fighter.renderPosition.z);
    assert(secondVolume.position.x == fighter.renderPosition.x);

    ParticleSystemManager emission, rendering;
    level.particleEmitBoolPtr = &emission;
    level.particleRenderBoolPtr = &rendering;
    fighter.field_0x80 = 12;
    fighter.field_0x84 = 34;
    float quality = 1.0f;
    std::memcpy(Globals::options + 0x28, &quality, sizeof(quality));
    player.hitpoints = 30;
    fighter.update(1);
    assert(fighter.field_0x1fc == 1 && emission.calls == 1 && rendering.calls == 1);
    assert(emission.lastHandle == 12 && rendering.lastHandle == 34);
    assert(emission.lastEnabled && rendering.lastEnabled);
    fighter.update(1);
    assert(emission.calls == 1 && rendering.calls == 1);

    player.hitpoints = 33;
    fighter.update(1);
    assert(fighter.field_0x1fc == 0 && emission.calls == 2 && rendering.calls == 2);
    assert(!emission.lastEnabled && !rendering.lastEnabled);

    quality = 0.0f;
    std::memcpy(Globals::options + 0x28, &quality, sizeof(quality));
    player.hitpoints = 30;
    fighter.update(1);
    assert(fighter.field_0x1fc == 1 && emission.calls == 2 && rendering.calls == 2);
    player.hitpoints = 100;
    fighter.update(1);
    assert(fighter.field_0x1fc == 0 && emission.calls == 3 && rendering.calls == 3);

    quality = 1.0f;
    std::memcpy(Globals::options + 0x28, &quality, sizeof(quality));
    fighter.state = 9;
    player.hitpoints = 30;
    fighter.update(1);
    assert(fighter.field_0x1fc == 1 && emission.calls == 4 && rendering.calls == 4);
    assert(!emission.lastEnabled && !rendering.lastEnabled);

    Mission freelance;
    Station station;
    GameText gameText;
    status.freelance = &freelance;
    status.station = &station;
    Globals::gameText = &gameText;
    ParticleSystemManager deathEmission, deathSystems;
    level.field_74 = &deathEmission;
    level.field_8c = &deathSystems;
    FModSound sound;
    Layout layout;
    Globals::sound = &sound;
    Globals::layout = &layout;
    Globals::options[0x0f] = 1;
    fighter.engineTrailSystem = 41;
    fighter.field_0x130 = 42;
    fighter.field_0x134 = 43;
    fighter.field_0x138 = 44;
    fighter.field_0x73 = 9;
    fighter.field_0x43 = 1;
    player.enemyFlagsLo = 0;
    random.used = 0;
    random.values[0] = 0;
    random.values[1] = 100;
    random.values[2] = 100;
    random.values[3] = 101;
    fighter.state = 0;
    player.hitpoints = 0;
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 1);
    assert(gameText.getTextCalls == 0);
    assert(fighter.state == 3 && fighter.field_0x73 == 0 && fighter.field_0x1f8 == 1500);
    assert(level.friendDeaths == 1 && level.enemyDeaths == 0 && level.wingmanDeaths == 0);
    assert(deathEmission.manualCalls == 1 && deathEmission.manualHandle == level.field_3c);
    assert(deathEmission.manualPosition.x == fighter.renderPosition.x &&
           deathEmission.manualVelocity.x == 0.0f && deathEmission.manualLifetime == -1.0f);
    assert(sound.calls == 1 && sound.lastId == 20 && sound.lastPosition == &fighter.renderPosition);
    assert(fighter.deathDirection.z == geometry.direction.z && fighter.deathDrift.z == 0.05f);
    assert(deathEmission.handles[0] == 41 && deathEmission.enabled[0]);
    assert(deathSystems.calls == 3 && deathSystems.handles[0] == 42 &&
           deathSystems.handles[1] == 43 && deathSystems.handles[2] == 44);
    assert(!deathSystems.enabled[0] && !deathSystems.enabled[1] && !deathSystems.enabled[2]);
    assert(!fighter.crateCaptured && !fighter.exhaustVisible && fighter.exhaustCalls == 1);
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 1);
    assert(gameText.getTextCalls == 0);
    assert(level.friendDeaths == 1 && sound.calls == 1);

    status.field_0xf1 = 0;
    fighter.name.data = "escort";
    fighter.state = 0;
    random.used = 0;
    fighter.update(1);
    assert(status.field_0xf0 == 1 && status.field_0xf1 == 0);
    assert(gameText.getTextCalls == 1 && fighter.name.compareCalls == 1);

    fighter.name.data = "another";
    fighter.state = 0;
    random.used = 0;
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 1);

    status.field_0xf1 = 0;
    freelance.targetStation = 8;
    fighter.state = 0;
    random.used = 0;
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 0);
    freelance.targetStation = 7;
    freelance.type = 12;
    fighter.state = 0;
    random.used = 0;
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 0);
    freelance.type = 13;
    fighter.state = 3;
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 0);
    fighter.state = 0;
    player.hitpoints = 1;
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 0);
    player.hitpoints = 0;
    status.freelance = nullptr;
    random.used = 0;
    fighter.update(1);
    assert(status.field_0xf0 == 0 && status.field_0xf1 == 0);

    Wanted wanted;
    Wanted *wantedItems[] = {&wanted};
    Array<Wanted *> wantedArray{1, wantedItems};
    status.wanted = &wantedArray;
    status.credits = 100;
    fighter.field_0x42 = 1;
    fighter.field_0x48 = 0;
    fighter.shipGroup = 8;
    fighter.state = 0;
    status.blackMarket = false;
    status.field_110 = 0;
    player.alwaysFriend = false;
    player.turnedEnemyFlag = false;
    player.enemyFlagsLo = 1;
    player.destroyedByte = 0;
    random.used = 0;
    fighter.update(1);
    assert(fighter.state == 3 && wanted.terminated && !wanted.active);
    assert(wanted.terminateCalls == 1 && wanted.activeCalls == 1 && wanted.rewardCalls == 2);
    assert(layout.calls == 1 && layout.lastReward == 75 && layout.lastFlag);
    assert(status.credits == 175 && status.collectedCalls == 1 && status.lastBoard == 2);
    assert(player.enemyFlagsLo == 1);
    assert(fighter.shipGroup == 8);
    assert(status.pirateKills == 1);
    assert(level.enemyDeaths == 1);
    assert(level.lastEnemyKind == fighter.kind && !level.lastEnemyDestroyed);
    fighter.update(1);
    assert(status.credits == 175 && wanted.terminateCalls == 1 && level.enemyDeaths == 1);

    fighter.field_0x42 = 0;
    fighter.shipGroup = 9;
    fighter.state = 0;
    fighter.cargo = nullptr;
    Globals::options[0x0f] = 0;
    random.used = 0;
    fighter.update(1);
    assert(fighter.state == 3 && fighter.cargo && fighter.cargo->size() == 2);
    assert((*fighter.cargo)[0] == 131 && (*fighter.cargo)[1] == 1);
    assert(fighter.crateCaptured == 1 && sound.lastPosition == nullptr);
    assert(level.enemyDeaths == 2 && status.pirateKills == 1);
    delete[] fighter.cargo->items;
    delete fighter.cargo;
    fighter.cargo = nullptr;

    fighter.state = 0;
    fighter.shipGroup = 2;
    fighter.wingmanFlag = 1;
    fighter.routeClone = nullptr;
    player.enemyFlagsLo = 0;
    random.used = 0;
    fighter.update(1);
    assert(fighter.state == 3 && level.friendDeaths >= 2 && level.wingmanDeaths == 1);

    AEGeometry driftGeometry;
    Player driftPlayer;
    Explosion driftExplosion;
    PlayerFighter drifting;
    SpacePoint occupiedPoint;
    ParticleSystemManager driftTrail, driftEmit, driftRender;
    Level driftLevel;
    driftLevel.field_74 = &driftTrail;
    driftLevel.particleEmitBoolPtr = &driftEmit;
    driftLevel.particleRenderBoolPtr = &driftRender;
    drifting.geometry = &driftGeometry;
    drifting.player = &driftPlayer;
    drifting.explosion = &driftExplosion;
    drifting.level = &driftLevel;
    drifting.routeClone = &route;
    drifting.field_0x43 = 1;
    drifting.state = 3;
    drifting.shipGroup = 9;
    drifting.finished = 1;
    drifting.field_0x13e = 1;
    drifting.deathDrift = {0.05f, 0.05f, 0.05f};
    drifting.deathDirection = {0.0f, 0.0f, 1.0f};
    drifting.currentSpeed = 2.0f;
    drifting.field_0x1f8 = 5;
    drifting.engineTrailSystem = 41;
    drifting.field_0x80 = 12;
    drifting.field_0x84 = 34;
    drifting.field_0x2c4 = &occupiedPoint;
    drifting.update(2);
    assert(drifting.state == 3 && drifting.field_0x1f8 == 3);
    assert(!drifting.finished && drifting.field_0x13e == 0);
    assert(driftGeometry.matrix.value == 27 && driftGeometry.setMatrixCalls == 1);
    assert(driftGeometry.translateCalls == 1 && driftGeometry.lastTranslation.z == 4.0f);
    assert(driftGeometry.referenceCalls == 1 && driftExplosion.startCalls == 0);
    drifting.update(0);
    assert(driftGeometry.setMatrixCalls == 1 && drifting.field_0x1f8 == 3);

    random.used = 0;
    random.values[0] = 10;
    random.values[1] = 100;
    random.values[2] = 100;
    random.values[3] = 101;
    drifting.update(4);
    assert(drifting.state == 4 && drifting.field_0x1f8 == 0);
    assert(driftExplosion.startCalls == 1 && driftExplosion.startPosition.z == 7.75f);
    assert(driftExplosion.startDirection.x == 0.0f);
    assert(driftTrail.calls == 1 && driftTrail.handles[0] == 41 && !driftTrail.enabled[0]);
    assert(driftEmit.calls == 1 && driftRender.calls == 1);
    assert(driftPlayer.bombForceCalls == 1 && std::abs(driftPlayer.bombForce - 50.1f) < 0.001f);
    assert(random.used == 4 && random.bounds[0] == 50 && random.bounds[3] == 200);
    assert(drifting.postExplosionDirection.z == 1.0f);
    assert(occupiedPoint.giveFreeCalls == 1 && drifting.field_0x2c4 == nullptr);
    assert(drifting.crateCalls == 1 && drifting.lastCrateType == 4);

    AEGeometry disabledGeometry;
    Player disabledPlayer;
    PlayerFighter disabled;
    disabled.geometry = &disabledGeometry;
    disabled.player = &disabledPlayer;
    disabled.level = &driftLevel;
    disabled.routeClone = &route;
    disabled.field_0x43 = 1;
    disabled.state = 3;
    disabled.aiDisabled = 1;
    disabled.field_0x1f8 = 5;
    disabled.update(1);
    assert(disabledGeometry.referenceCalls == 1 && disabled.cloakingCalls == 1);
    assert(disabled.field_0x1f8 == 5 && disabled.state == 3);
    disabled.state = 0;
    disabled.update(1);
    assert(disabledGeometry.referenceCalls == 2 && disabled.cloakingCalls == 2);
    assert(disabledPlayer.updateCalls == 1 && disabled.state == 0);

    AEGeometry missionGeometry;
    Player missionPlayer;
    Explosion missionExplosion;
    PlayerFighter missionCrate;
    missionCrate.geometry = &missionGeometry;
    missionCrate.player = &missionPlayer;
    missionCrate.explosion = &missionExplosion;
    missionCrate.level = &driftLevel;
    missionCrate.routeClone = &route;
    missionCrate.field_0x43 = 1;
    missionCrate.state = 3;
    missionCrate.deathDrift = {0.05f, 0.05f, 0.05f};
    missionCrate.isMissionCrate = 1;
    missionCrate.field_0x1f8 = 0;
    SpacePoint freePoint;
    freePoint.free = true;
    missionCrate.field_0x2c4 = &freePoint;
    quality = 0.0f;
    std::memcpy(Globals::options + 0x28, &quality, sizeof(quality));
    random.used = 0;
    missionCrate.update(1);
    assert(missionCrate.state == 4 && !missionCrate.crateCaptured);
    assert(missionCrate.missionCrateLost && missionCrate.crateCalls == 0);
    assert(freePoint.isFreeCalls == 1 && freePoint.giveFreeCalls == 0 &&
           missionCrate.field_0x2c4 == &freePoint);
    assert(driftEmit.calls == 1 && driftRender.calls == 1);

    AEGeometry wreckedGeometry;
    Player wreckedPlayer;
    Explosion wreckedExplosion;
    PlayerFighter wrecked;
    Level wreckedLevel;
    PlayerEgo wreckedEgo;
    Radar wreckedRadar;
    TargetFollowCamera wreckedCamera;
    int deletedCrates = 0;
    AEGeometry *crateMesh = new AEGeometry;
    crateMesh->destroyCounter = &deletedCrates;
    wreckedRadar.field_0x1c = &wrecked;
    wreckedEgo.field_0x14 = &wreckedRadar;
    wreckedEgo.camera = &wreckedCamera;
    wreckedLevel.ego = &wreckedEgo;
    wrecked.geometry = &wreckedGeometry;
    wrecked.player = &wreckedPlayer;
    wrecked.explosion = &wreckedExplosion;
    wrecked.level = &wreckedLevel;
    wrecked.routeClone = &route;
    wrecked.field_0x43 = 1;
    wrecked.state = 4;
    wrecked.crateCaptured = 1;
    wrecked.crateGeometry = crateMesh;
    wrecked.postExplosionDirection = {0.0f, 0.0f, 1.0f};
    wrecked.deathDrift = {0.05f, 0.05f, 0.05f};
    wrecked.deathDirection = {0.0f, 0.0f, 1.0f};
    wrecked.deathTimer = 10;
    wrecked.field_0x73 = 3;
    wreckedPlayer.bombForce = 2.0f;
    wreckedExplosion.playing = true;
    wrecked.update(2);
    assert(wrecked.field_0x1f8 == 2 && wrecked.field_0x73 == 0);
    assert(wreckedExplosion.updateCalls == 1 && wreckedExplosion.lastDt == 2 &&
           wreckedExplosion.lastCamera == &wreckedCamera);
    assert(wrecked.deathTimer == 12 && wrecked.targetPos.z == 2.0f);
    assert(crateMesh->lastTranslation.z == 2.0f && wreckedGeometry.lastTranslation.z == 2.0f);
    assert(wreckedPlayer.getBombForceCalls == 1 &&
           std::abs(wreckedPlayer.bombForce - 1.96f) < 0.001f);
    assert(crateMesh->rotateCalls == 1 && crateMesh->lastRotation == 0.0f);
    assert(wreckedPlayer.transformMatrix.value == crateMesh->matrix.value);
    assert(deletedCrates == 0 && wreckedPlayer.active);

    wrecked.field_0x1f8 = 0;
    int referencesBefore = wreckedGeometry.referenceCalls;
    wrecked.update(0);
    assert(wreckedGeometry.referenceCalls == referencesBefore + 2);
    assert(wrecked.field_0x1f8 == 0 && wrecked.deathTimer == 12);
    assert(crateMesh->rotateCalls == 1 && wreckedExplosion.updateCalls == 2);

    wreckedPlayer.bombForce = 0.0f;
    wrecked.deathTimer = 60000;
    wrecked.isMissionCrate = 1;
    wreckedExplosion.playing = false;
    wrecked.update(1);
    assert(wrecked.finished && !wreckedPlayer.active && wreckedPlayer.setActiveCalls == 1);
    assert(wrecked.missionCrateLost && wrecked.deathTimer == 0);
    assert(wrecked.crateGeometry == nullptr && deletedCrates == 1);
    assert(wreckedRadar.field_0x1c == nullptr);
    assert(wreckedPlayer.getBombForceCalls == 2 && wreckedPlayer.bombForceCalls == 1);

    AEGeometry noCrateGeometry;
    Player noCratePlayer;
    Explosion noCrateExplosion;
    PlayerFighter noCrate;
    Level noCrateLevel;
    noCrate.geometry = &noCrateGeometry;
    noCrate.player = &noCratePlayer;
    noCrate.explosion = &noCrateExplosion;
    noCrate.level = &noCrateLevel;
    noCrate.routeClone = &route;
    noCrate.field_0x43 = 1;
    noCrate.state = 4;
    noCrate.crateCaptured = 0;
    noCrate.isMissionCrate = 1;
    noCrate.deathTimer = 100;
    noCrateExplosion.playing = true;
    noCrateExplosion.endOnUpdate = true;
    noCrate.update(1);
    assert(noCrateExplosion.updateCalls == 1 && noCrateExplosion.lastCamera == nullptr);
    assert(noCrate.finished && noCrate.missionCrateLost && noCratePlayer.active);
    assert(noCrate.deathTimer == 101);
    noCrate.update(1);
    assert(!noCratePlayer.active && noCrateExplosion.updateCalls == 1);

    AEGeometry lateGeometry;
    Player latePlayer;
    Explosion lateExplosion;
    PlayerFighter late;
    late.geometry = &lateGeometry;
    late.player = &latePlayer;
    late.explosion = &lateExplosion;
    late.level = &noCrateLevel;
    late.routeClone = &route;
    late.field_0x43 = 1;
    late.state = 4;
    late.crateCaptured = 0;
    late.deathTimer = 60000;
    lateExplosion.playing = true;
    lateExplosion.endOnUpdate = true;
    late.update(1);
    assert(late.finished && !latePlayer.active && late.deathTimer == 60001);

    AEGeometry dormantGeometry;
    AEGeometry dormantSubGeometry;
    Player dormantPlayer;
    Player dormantTarget;
    PlayerFighter dormant;
    Level dormantLevel;
    Route dormantRoute;
    dormant.geometry = &dormantGeometry;
    dormant.player = &dormantPlayer;
    dormant.level = &dormantLevel;
    dormant.routeClone = &dormantRoute;
    dormant.field_0x43 = 1;
    dormant.state = 5;
    dormant.field_0x128 = 0;
    dormantPlayer.enemyFlagsLo = 1;
    dormantGeometry.visible = true;
    status.currentCampaignMission = 2;
    int fallbackBefore = bodyCalls;
    dormant.update(1);
    assert(!dormantGeometry.visible && dormantGeometry.visibleCalls == 1);
    assert(status.campaignCalls == 1 && dormant.state == 5 && bodyCalls == fallbackBefore);

    status.currentCampaignMission = 1;
    dormantGeometry.visible = true;
    dormant.update(1);
    assert(dormantGeometry.visible && dormantGeometry.visibleCalls == 1);
    assert(status.campaignCalls == 2 && dormant.state == 5);

    dormant.subGeometry = &dormantSubGeometry;
    dormant.field_0x148 = &dormantTarget;
    dormant.field_0x128 = 100;
    dormant.field_0x12f = 1;
    dormantPlayer.enemyFlagsLo = 0;
    dormantPlayer.active = false;
    dormantTarget.field_5e = 1;
    dormant.resetVecB = {2.25f, -2.5f, 3.75f};
    dormant.update(1);
    assert(dormant.state == 5 && !dormantPlayer.active && dormantLevel.pirateStationCalls == 0);

    dormantTarget.field_5e = 0;
    dormant.resetVecB = {101.25f, -2.5f, 3.75f};
    dormant.update(1);
    assert(dormant.resetVecC.x == 100.0f && dormant.state == 5);
    assert(!dormantPlayer.active && dormantSubGeometry.visibleCalls == 0);

    dormant.resetVecB.x = std::nanf("");
    dormant.update(1);
    assert(dormant.state == 5 && !dormantPlayer.active);

    dormant.resetVecB.x = 101.0f;
    dormant.update(1);
    assert(dormant.resetVecC.x == 99.75f && dormant.state == 1);
    assert(dormantPlayer.active && dormantPlayer.setActiveCalls == 1);
    assert(dormantSubGeometry.visible && dormantSubGeometry.visibleCalls == 1);
    assert(dormantLevel.pirateStationCalls == 1 && dormantLevel.lastPirateStationAction);
    assert(bodyCalls == fallbackBefore);
}
