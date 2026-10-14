#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

template<class T> using Array = std::vector<T>;

struct Vector { float x, y, z; };
using Vec3 = Vector;
Vector operator*(const Vector &value, float factor) {
    return {value.x * factor, value.y * factor, value.z * factor};
}
Vector operator+(const Vector &a, const Vector &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
struct Matrix {
    Vector right{1.0f, 0.0f, 0.0f};
    Vector up{0.0f, 1.0f, 0.0f};
    Vector position{};
    Vector dir{0.0f, 0.0f, 1.0f};
};
namespace AbyssEngine {
struct Mesh { float shaderAnimValue1 = 0.0f; };
struct Transform {
    int updates = 0;
    int lastDt = -1;
    void Update(int dt, bool updateBounds) {
        assert(!updateBounds);
        ++updates;
        lastDt = dt;
    }
};
}
static const Matrix *expectedFirstMatrix = nullptr;
static const Matrix *expectedSecondMatrix = nullptr;
static const Matrix *expectedDockRotationMatrix = nullptr;
static Matrix combinedMatrix{};
Matrix operator*(const Matrix &first, const Matrix &second) {
    assert(&first == expectedFirstMatrix && &second == expectedSecondMatrix);
    return combinedMatrix;
}
namespace AbyssEngine { namespace AEMath {
Vector MatrixRotateVector(const Matrix &matrix, const Vector &value) {
    assert(&matrix == expectedDockRotationMatrix);
    return value;
}
float VectorLength(const Vector &value) {
    return std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
}
Vector VectorCross(const Vector &a, const Vector &b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
Matrix MatrixSetRotation(Matrix &matrix, const Vector &right, const Vector &up, const Vector &dir) {
    matrix.right = right; matrix.up = up; matrix.dir = dir; return matrix;
}
Matrix MatrixSetTranslation(Matrix &matrix, const Vector &position) {
    matrix.position = position; return matrix;
}
Vector MatrixGetUp(const Matrix &matrix) { return matrix.up; }
Vector MatrixGetPosition(const Matrix &matrix) { return matrix.position; }
Vector MatrixGetDir(const Matrix &matrix) { return matrix.dir; }
Vector VectorNormalize(const Vector &value) {
    float length = std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
    return length == 0.0f ? Vector{} : Vector{value.x / length, value.y / length, value.z / length};
}
} }
namespace AbyssEngine {
struct EaseInOutMatrix {
    static int creations, destructions;
    Matrix min, max;
    int duration;
    int elapsed = 0;
    int increaseCalls = 0;
    EaseInOutMatrix(Matrix from, Matrix to, int milliseconds)
        : min(from), max(to), duration(milliseconds) { ++creations; }
    ~EaseInOutMatrix() { ++destructions; }
    void Increase(float dt) { elapsed += static_cast<int>(dt); ++increaseCalls; }
    Matrix GetValue() { return min; }
};
int EaseInOutMatrix::creations = 0;
int EaseInOutMatrix::destructions = 0;
}
struct AEGeometry {
    static int destructions;
    unsigned int transform = 17;
    unsigned int meshId = 29;
    Matrix matrix{};
    Vector position{};
    Vector scaling{1.0f, 1.0f, 1.0f};
    int rotations = 0;
    int matrixWrites = 0;
    ~AEGeometry() { ++destructions; }
    Matrix &getMatrix() { return matrix; }
    Vector getPosition() { return position; }
    Vector getScaling() { return scaling; }
    void rotate(float x, float y, float z) { assert(x == 0.03f && y == 0.03f && z == 0.03f); ++rotations; }
    void setMatrix(const Matrix &value) { matrix = value; position = value.position; ++matrixWrites; }
    void translate(const Vector &value) { position = position + value; }
    void setScaling(float x, float y, float z) { scaling = {x, y, z}; }
};
int AEGeometry::destructions = 0;
struct PaintCanvas {
    Matrix *local = nullptr;
    Matrix *otherLocal = nullptr;
    int reads = 0;
    Vector screenResult{};
    Vector lastWorldPosition{};
    int projections = 0;
    AbyssEngine::Mesh *emergencyMesh = nullptr;
    AbyssEngine::Transform *emergencyTransform = nullptr;
    AbyssEngine::Mesh *MeshGetPointer(unsigned int id) {
        assert(id == 29 && emergencyMesh != nullptr);
        return emergencyMesh;
    }
    void *TransformGetTransform(unsigned int id) {
        assert(id == 17 && emergencyTransform != nullptr);
        return emergencyTransform;
    }
    void *TransformGetLocal(unsigned int id) {
        assert(id == 17 || id == 18);
        ++reads;
        return id == 17 ? local : otherLocal;
    }
    int GetScreenPosition(const Vector &world, Vector &screen) {
        ++projections;
        lastWorldPosition = world;
        screen = screenResult;
        return 1;
    }
};
struct ParticleDefinition {
    int count = 0;
    uint32_t color0 = 0;
    float lifeBase = 0.0f;
};
struct ParticleSettingsRef {
    struct Table { ParticleDefinition sets[48]; };
    static Table cur;
    static Table init;
};
ParticleSettingsRef::Table ParticleSettingsRef::cur{};
ParticleSettingsRef::Table ParticleSettingsRef::init{};
struct ParticleSettings {
    enum ParticleSet { ParticleSet_4 = 4, ParticleSet_6 = 6 };
    static int interpolations;
    static float blend;
    static void Interpolate(ParticleSet a, ParticleSet b, float t, ParticleSet out) {
        assert(a == ParticleSet_4 && b == ParticleSet_6 && out == ParticleSet_4);
        ++interpolations;
        blend = t;
    }
};
struct ParticleSystemManager {
    int emitDisables = 0, renderDisables = 0, manualEmits = 0;
    int lastHandle = -1, lastSet = -1;
    Vector lastPosition{}, lastVelocity{};
    void enableSystemEmit(int handle, bool value) { assert(!value); lastHandle = handle; ++emitDisables; }
    void enableSystemRender(int handle, bool value) { assert(!value); lastHandle = handle; ++renderDisables; }
    void emitManual(int handle, const Vector &position, int set, const Vector &velocity, float lifetime) {
        assert(lifetime == -1.0f);
        lastHandle = handle; lastSet = set; lastPosition = position; lastVelocity = velocity;
        ++manualEmits;
    }
};
struct KIPlayer;
struct SpacePoint {
    Vector position{};
    Vector direction{};
    int takes = 0, releases = 0;
    void take() { ++takes; }
    void giveFree() { ++releases; }
};
struct Level {
    ParticleSystemManager *field_74 = nullptr;
    int field_3c = 0;
    std::vector<AEGeometry *> *field_a4 = nullptr;
    std::vector<KIPlayer *> *enemies = nullptr;
    int radioType = -1, radioParam = -1;
    int deliveredOre = 0;
    std::vector<int> dockingTargets;
    std::vector<KIPlayer *> *getEnemies() { return enemies; }
    void createRadioMessage(int type, int param) { radioType = type; radioParam = param; }
    void incNumDeliveredOre(int amount) { deliveredOre += amount; }
    int getNumDeliveredOre() { return deliveredOre; }
    int getNumDockingTargets() { return static_cast<int>(dockingTargets.size()); }
    int getDockingTarget(int index) { return dockingTargets[index]; }
};
struct Explosion {
    int starts = 0, updates = 0, lastDt = -1;
    Vector lastPosition{}, lastDirection{};
    void start(const Vector &position, const Vector &direction) {
        lastPosition = position; lastDirection = direction; ++starts;
    }
    void update(int dt, const Vector &position) {
        lastDt = dt; lastPosition = position; ++updates;
    }
};
int ParticleSettings::interpolations = 0;
float ParticleSettings::blend = -1.0f;
struct CargoArray {
    static int destructions;
    ~CargoArray() { ++destructions; }
};
int CargoArray::destructions = 0;
struct KIPlayer {
    int state = 0;
    unsigned char _pad_0x04[0x6c]{};
    unsigned char field_0x70 = 0;
    unsigned char _pad_0x71[0x1b]{};
    unsigned char field_0x8c = 0;
    CargoArray *cargo = nullptr;
    SpacePoint *nearestNav = nullptr;
    SpacePoint *nearestDock = nullptr;
    Vector lastDockQuery{};
    int dockQueries = 0;
    AEGeometry *geometry = nullptr;
    Vector position{};
    bool isDying() { return state == 3; }
    bool isDead() { return state == 4; }
    void *getNearestNavigationPoint(Vector, SpacePoint *) { return nearestNav; }
    SpacePoint *getNearestDockingPoint(const Vector &point) {
        lastDockQuery = point; ++dockQueries; return nearestDock;
    }
    Vector getPosition() { return position; }
};
static_assert(offsetof(KIPlayer, field_0x70) == 0x70, "KIPlayer mock docking flag offset");
static_assert(offsetof(KIPlayer, field_0x8c) == 0x8c, "KIPlayer mock target flag offset");
struct PlayerFixedObject : KIPlayer {
    int dockingType = -1;
    int getDockingType() { return dockingType; }
    void setDockingType(int value) { dockingType = value; }
};
struct PlayerAsteroid : KIPlayer {
    float scaling = 1.0f;
    bool rotationEnabled = false;
    float getScaling() { return scaling; }
    void setRotationEnabled(bool value) { rotationEnabled = value; }
};
struct Radar {
    int unlocks = 0;
    void *dockTargetPtr = nullptr;
    void *dockNavPtr = nullptr;
    void unlockAsteroid() { ++unlocks; }
};
struct TargetFollowCamera {
    bool active = false;
    bool lookAt = false, targetsUp = false;
    bool fastForward = false;
    int fastForwardQueries = 0;
    int handlingSets = 0;
    int handlingResets = 0;
    float lastHandling = 0.0f;
    void setActive(bool value) { active = value; }
    void setLookAtCam(bool value) { lookAt = value; }
    void useTargetsUpVector(bool value) { targetsUp = value; }
    bool isInFastForwardMode() { ++fastForwardQueries; return fastForward; }
    void setShipHandling(float value) { ++handlingSets; lastHandling = value; }
    void resetShipHandling() { ++handlingResets; }
};
struct LevelScript {
    Level *m_pLevel = nullptr;
    int cameraResets = 0;
    void resetCamera(Level *level) { assert(level == m_pLevel); ++cameraResets; }
};
struct Player {
    Matrix transformMatrix{};
    int resetCount = 0;
    float flShake = 0.0f;
    int stopShots = 0;
    int hitpoints = 10;
    int maxHitpoints = 10;
    int armorHP = 0;
    int maxArmorHP = 10;
    int shieldHP = 0;
    int maxShieldHP = 10;
    int shieldRecoveries = 0;
    int hullRepairs = 0;
    int armorRepairs = 0;
    float lastShieldRecovery = 0.0f;
    int updates = 0;
    int lastUpdateDt = 0;
    bool lastDoSound = false;
    bool vulnerable = true;
    int vulnerabilityWrites = 0;
    void setVulnerable(bool value) { vulnerable = value; ++vulnerabilityWrites; }
    unsigned char shieldHit = 0;
    unsigned char armorHit = 0;
    unsigned char hullHit = 0;
    Vector hitVector{3.0f, 4.0f, 0.0f};
    int hitVectorReads = 0;
    int getHitpoints() { return hitpoints; }
    int getMaxHitpoints() { return maxHitpoints; }
    int getArmorHP() { return armorHP; }
    int getMaxArmorHP() { return maxArmorHP; }
    int getShieldHP() { return shieldHP; }
    int getMaxShieldHP() { return maxShieldHP; }
    int getCombinedHP() { return hitpoints + armorHP + shieldHP; }
    Vector getHitVector() { ++hitVectorReads; return hitVector; }
    void setShieldHP(int value) { shieldHP = value > maxShieldHP ? maxShieldHP : value; }
    void regenerateShield(float amount) { ++shieldRecoveries; lastShieldRecovery = amount; }
    void regenerateHull() { ++hullRepairs; ++hitpoints; }
    void regenerateArmor() { ++armorRepairs; armorHP += 2; }
    void update(int dt, bool doSound) { ++updates; lastUpdateDt = dt; lastDoSound = doSound; }
    Vector getPosition() { return {7.0f, 0.0f, 0.0f}; }
    void resetGunDelay(int value) { assert(value == 0); ++resetCount; }
    void stopShooting(int value) { assert(value == 0); ++stopShots; }
};
struct PlayerEgo;
struct Hud {
    int event = -1;
    int amount = -1;
    int events = 0;
    int hits = 0;
    int hackingActiveChanges = 0;
    bool hackingActive = true;
    void hudEvent(int id, PlayerEgo *, int value) { event = id; amount = value; ++events; }
    void playerHit() { ++hits; }
    void setHackingGameActive(bool value) { hackingActive = value; ++hackingActiveChanges; }
};
struct Radio {};
struct Route {
    int current = 0;
    int next = 0;
    unsigned char loop = 0;
    int updates = 0;
    Vector lastPosition{};
    int getCurrent() { return current; }
    void update(const Vector &position) { ++updates; lastPosition = position; current = next; }
};
struct Item {
    int amount = 2;
    int index = 0;
    int getAttribute(int id) { assert(id == 59); return amount; }
    int getAmount() { return amount; }
    int getIndex() { return index; }
};
struct Ship {
    int freeSpace = 0;
    bool volatileGoods = false;
    Item *equipment = nullptr;
    Item *boostEquipment = nullptr;
    Item *cargoItem = nullptr;
    int maxPassengers = 0;
    int cargo = 0;
    int removals = 0;
    int lastRemovedIndex = -1;
    int lastRemovedAmount = 0;
    int shieldRegen = 0;
    int maxShieldHP = 10;
    int repairType = -1;
    int index = 0;
    float handling = 1.0f;
    int getFreeSpace() { return freeSpace; }
    bool hasVolatileGoods() { return volatileGoods; }
    int getShieldRegen() { return shieldRegen; }
    int getMaxShieldHP() { return maxShieldHP; }
    int getIndex() { return index; }
    float getHandling() { return handling; }
    int getRepairType() { return repairType; }
    int getMaxPassengers() { return maxPassengers; }
    Item *getCargo(int index) { assert(index == 202); return cargoItem; }
    Item *getFirstEquipmentOfSort(int sort) {
        assert(sort == 43 || sort == 14);
        return sort == 43 ? equipment : boostEquipment;
    }
    bool hasCargo(int index, int amount) { assert(index == 202); return cargo >= amount; }
    int removeCargo(int index, int amount) {
        assert(index == 202 && cargo >= amount);
        lastRemovedIndex = index;
        lastRemovedAmount = amount;
        cargo -= amount;
        ++removals;
        return cargo;
    }
};
struct Station {
    bool hiddenBlueprint = false;
    unsigned int hiddenBlueprintIndex = 0;
    bool stationHasHiddenBlueprint(bool ignoreFound) { assert(!ignoreFound); return hiddenBlueprint; }
    unsigned int getHiddenBlueprintIndex() { return hiddenBlueprintIndex; }
};
struct Mission {
    bool empty = true;
    int type = 0;
    int statusValue = 0;
    int productionGoodIndex = 202;
    int productionGoodAmount = 0;
    bool isEmpty() { return empty; }
    int getType() { return type; }
    int getStatusValue() { return statusValue; }
    void setStatusValue(int value) { statusValue = value; }
    int getProductionGoodIndex() { return productionGoodIndex; }
    int getProductionGoodAmount() { return productionGoodAmount; }
};
struct Status {
    Ship *ship = nullptr;
    Station *station = nullptr;
    std::vector<unsigned char> *field_58 = nullptr;
    int unlockedBlueprint = -1;
    int field_178 = 0;
    int field_13c = 0;
    int currentCampaignMission = 0;
    Mission *mission = nullptr;
    Mission *campaignMission = nullptr;
    Ship *getShip() { return ship; }
    Station *getStation() { return station; }
    void unlockBluePrint(int index) { unlockedBlueprint = index; }
    Mission *getMission() { return mission; }
    intptr_t getCampaignMission() { return reinterpret_cast<intptr_t>(campaignMission); }
    int getCurrentCampaignMission() { return currentCampaignMission; }
};
static Status status;
Status *PE_status() { return &status; }
struct PlayerEgo;
void PE_upd_docksFinishDelivery(PlayerEgo *, Radio *);
static int destroyedMiningGames = 0;
struct MiningGame { ~MiningGame() { ++destroyedMiningGames; } };
struct HackingGame {
    static int destructions;
    bool won = false;
    int updates = 0, lastDt = -1;
    int constructorType = -1, constructorMode = -1, constructorDockIndex = -1;
    HackingGame() = default;
    HackingGame(int type, int mode, int rewardItem, int rewardAmount, int dockingIndex)
        : constructorType(type), constructorMode(mode), constructorDockIndex(dockingIndex) {
        assert(rewardItem == -1 && rewardAmount == -1);
    }
    ~HackingGame() { ++destructions; }
    int update(int dt) { ++updates; lastDt = dt; return 0; }
    int gameWon() { return won; }
};
int HackingGame::destructions = 0;
struct FModSound {
    int played[16]{};
    int playCount = 0;
    int stopped = 0;
    int stopCount = 0;
    int updated[8]{};
    int updateCount = 0;
    float parameter = -1.0f;
    void play(int id, Vector *, Vector *, float) { played[playCount++] = id; }
    void stop(int id) { stopped = id; ++stopCount; }
    void setParamValue(int param, int id, float value) {
        assert(param == 0 && id == 2257);
        parameter = value;
    }
    void updateEvent3DAttributes(int id, Vector *pos, Vector *vel, bool restart) {
        assert(pos->x == 7.0f && (vel == nullptr || vel->x == 0.0f) && !restart);
        updated[updateCount++] = id;
    }
};
struct GlobalsService {
    int registeredSound = -1;
    int registrations = 0;
    void addSoundResourceToList(int id) { registeredSound = id; ++registrations; }
};
struct Engine {
    int postEffectCalls = 0;
    uint32_t lastEffect = 0;
    void SetPostEffect(uint32_t effect, bool enable) {
        assert(!enable); lastEffect = effect; ++postEffectCalls;
    }
};
struct ApplicationManager {
    Engine *engine = nullptr;
    void *GetEngine() { return engine; }
};
namespace Globals {
    FModSound *sound = nullptr;
    GlobalsService *gGlobals = nullptr;
    PaintCanvas *Canvas = nullptr;
    ApplicationManager *appManager = nullptr;
    Status *status = nullptr;
    int mouseCursorActivated = 0;
    int mouseDeltaX = 0;
    int mouseDeltaY = 0;
    int w = 800;
    int h = 600;
    unsigned char options[100]{};
}
static const float g_PE_astApproach = 2500.0f;
static const float g_PE_upd_handlingBias = 2.7f;
struct PlayerEgo {
    static Vector vec_up;
    static Vector crosshairPos;
    static Vector crosshairShootPos;
    union { int asteroidTarget; void *asteroidTargetPtr; };
    union { int targetFollowCamera; void *targetFollowCameraPtr; };
    union { int miningGame; void *miningGamePtr; };
    Player *player = nullptr;
    int dockingState = 0;
    int miningSettleTimer = 0;
    int dockingPointIndex = 0;
    unsigned char switchToStandardCam = 0;
    unsigned char field_0xb2 = 0;
    unsigned char switchToFreeLookCam = 0;
    int dockScaling = 0;
    union {
        struct { int dockApproachDist, dockApproachThreshold, field_0x1d0; };
        Vector dockApproachVector{};
    };
    int dockedFlag = 0;
    unsigned char aboutToReachAutoTargetFlag = 0;
    SpacePoint *spacePoint = nullptr;
    void *easeMatrix = nullptr;
    void *dockStation = nullptr;
    LevelScript *levelScript = nullptr;
    int engineSoundStops = 0;
    float waypointX = 0.0f, waypointY = 0.0f;
    uint32_t gunBaseGeo = 0;
    int pitchAccumDir = 0;
    Vector lastMovePosition{};
    float lastMoveSpeed = 0.0f;
    int moveCalls = 0;
    bool lastMoveSteer = false;
    int approachCount = 0;
    int dockPointStops = 0;
    unsigned char field_0x145 = 0;
    int lastFreeSpace = 0;
    int dockTransferTimer = 0;
    int dockTotalAmount = 0;
    int dockTransferedAmount = 0;
    unsigned char volatileGoods = 0;
    unsigned char shieldRegenActive = 0;
    unsigned char shieldRegenEquipped = 0;
    void *gunMuzzleRoot = nullptr;
    int field_0x1c = -1;
    int engineSoundId = -1;
    float handling = 0.0f;
    unsigned int hudHitDirectionFlags = 0;
    unsigned char shieldHitFlash = 0;
    unsigned char cursorStateByte = 0;
    unsigned char cursorStateDirty = 0;
    unsigned char computerControlled = 0;
    unsigned char freeze = 0;
    int lastHP = 0;
    int hitCameraCalls = 0;
    AEGeometry *geometry = nullptr;
    AEGeometry *field_0x4 = nullptr;
    void *field_0x2c = nullptr;
    void *turretGeometry = nullptr;
    unsigned char turretActive = 0;
    unsigned char autoPilot = 0;
    Vector crosshairPosPrevious{};
    Vector crosshairShootPosPrevious{};
    Level *level = nullptr;
    void *explosion = nullptr;
    void *explosion2 = nullptr;
    int rocketControlGun = 0, rocketBanking = 0;
    int explosionTimer = 0, shakeIntensity = 0, currentSystem = 0;
    unsigned char turretMode = 0;
    int rightCalls = 0, leftCalls = 0, downCalls = 0, upCalls = 0;
    float lastRight = 0.0f, lastLeft = 0.0f, lastDown = 0.0f, lastUp = 0.0f;
    unsigned char boostingFlag = 0;
    uintptr_t field_0xac = 0;
    int emergencySystemTimer = 0;
    int emergencyVal1 = 0;
    Vector emergencyVec{};
    float emergencyVal2 = 0.0f;
    int cloakRechargeTimer = 0;
    void *hackingGame = nullptr;
    int boostTimer = 0;
    int boostSpeed = 0;
    int field_0xcc = 0;
    int field_0xd0 = 0;
    unsigned char field_0x146 = 0;
    float speedValue = 0.0f;
    float boostPercentage = 0.0f;
    int boostPercentReads = 0;
    Route *route = nullptr;
    Vector dockOffsetVec{};
    int64_t shieldRecoveryTimer = 0;
    int64_t hullRepairTimer = 0;
    int64_t armorRepairTimer = 0;
    float shieldRecoveryAmount = 2.5f;
    Vector position{7.0f, 0.0f, 0.0f};
    bool exhaustVisible = false;
    void setExhaustVisible(bool value) { exhaustVisible = value; }
    void dockToAsteroid(KIPlayer *kip, Radar *radar);
    void updateDocking(Radar *radar, Hud *hud, Radio *radio);
    void updateDockFinish(Hud *hud);
    void initializeDockTransfer(Hud *hud);
    int dockStateGate();
    void updateCargoState();
    void updateBoostTimer(int dt);
    void updateBoost(int dt);
    void updateSubsystems(int dt, Hud *hud);
    void updateShield(int dt, Hud *hud);
    void updateRecovery(int dt);
    void updateEmergency(int dt);
    void updatePlayerSound(int dt);
    void updateVisualState();
    void updateRoute(Hud *hud);
    void updateHitState(Hud *hud);
    void updateCrosshair(int dt, bool arg6, int arg7);
    void updateTail(Vector cameraPosition, Vector cameraDirection);
    void initializeEmergency();
    float right(int dt, float delta) { assert(dt == 16); ++rightCalls; lastRight = delta; return 0.0f; }
    float left(int dt, float delta) { assert(dt == 16); ++leftCalls; lastLeft = delta; return 0.0f; }
    float down(int dt, float delta) { assert(dt == 16); ++downCalls; lastDown = delta; return 0.0f; }
    float up(int dt, float delta) { assert(dt == 16); ++upCalls; lastUp = delta; return 0.0f; }
    void initializeRecoveryAmount(Ship *ship);
    void initializeEngineSound(Ship *ship);
    bool isDead() { return player->hitpoints < 1; }
    float getBoostPercentage() { ++boostPercentReads; return boostPercentage; }
    void hitCamera() { ++hitCameraCalls; }
    Vector getPosition() { return position; }
    int approachAsteroid(Hud *, int, Radar *) { ++approachCount; return 0; }
    int approachDockingPoint(Hud *, int, Radar *);
    void moveToPosition(Vector target, bool steer, float speed) {
        lastMovePosition = target; lastMoveSteer = steer; lastMoveSpeed = speed; ++moveCalls;
    }
    void StopEngineSound() { ++engineSoundStops; }
    int setTurretMode(bool enable) { turretActive = enable; return 0; }
    void dockToDockingPoint(KIPlayer *, Radar *) { dockedFlag = 0; ++dockPointStops; }
};
Vector PlayerEgo::vec_up{};
Vector PlayerEgo::crosshairPos{};
Vector PlayerEgo::crosshairShootPos{};
int dockRadius[1] = {200};
int *g_PE_adp_dockRadius = dockRadius;

// PRODUCTION_METHOD
// PRODUCTION_UPDATE_DOCKING
// PRODUCTION_UPDATE_DOCK_FINISH
// PRODUCTION_DOCK_TRANSFER_INIT
// PRODUCTION_DOCK_STATE_GATE
// PRODUCTION_APPROACH_DOCKING_POINT
// PRODUCTION_UPDATE_CARGO
// PRODUCTION_UPDATE_BOOST_TIMER
// PRODUCTION_UPDATE_BOOST
// PRODUCTION_UPDATE_SUBSYSTEMS
// PRODUCTION_UPDATE_SHIELD
// PRODUCTION_UPDATE_RECOVERY
// PRODUCTION_UPDATE_EMERGENCY
// PRODUCTION_UPDATE_PLAYER_SOUND
// PRODUCTION_UPDATE_VISUAL_STATE
// PRODUCTION_UPDATE_ROUTE
// PRODUCTION_UPDATE_HIT_STATE
// PRODUCTION_UPDATE_CROSSHAIR
// PRODUCTION_UPDATE_TAIL
// PRODUCTION_RECOVERY_AMOUNT
// PRODUCTION_EMERGENCY_CONSTRUCTOR
// PRODUCTION_ENGINE_SOUND_SELECTION

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
    ego.dockApproachDist = 1;
    ego.dockApproachThreshold = 2;
    ego.field_0x1d0 = 3;
    ego.dockToAsteroid(nullptr, &radar);
    assert(ego.dockingState == 0 && ego.asteroidTargetPtr == nullptr);
    assert(ego.field_0x145 == 0 && asteroid.rotationEnabled);
    assert(ego.dockApproachDist == 0 && ego.dockApproachThreshold == 0 && ego.field_0x1d0 == 0);
    assert(camera.active && player.resetCount == 1 && radar.unlocks == 1);
    assert(ego.miningGamePtr == nullptr && destroyedMiningGames == 1);
    assert(ego.exhaustVisible && ego.dockingPointIndex == 0);

    Hud hud;
    Radio radio;
    asteroid.state = 0;
    ego.dockToAsteroid(&asteroid, &radar);
    ego.updateDocking(&radar, &hud, &radio);
    assert(ego.dockingState == 1 && ego.approachCount == 1);
    assert(radar.unlocks == 1);

    asteroid.state = 3;
    ego.updateDocking(&radar, &hud, &radio);
    assert(ego.dockingState == 0 && radar.unlocks == 2);
    assert(ego.asteroidTargetPtr == nullptr);

    asteroid.state = 4;
    ego.dockToAsteroid(&asteroid, &radar);
    ego.updateDocking(&radar, &hud, &radio);
    assert(ego.dockingState == 0 && radar.unlocks == 3);

    Ship ship;
    status.ship = &ship;
    GlobalsService globals;
    Globals::gGlobals = &globals;
    ship.index = 42;
    ego.initializeEngineSound(&ship);
    assert(ego.field_0x1c == 1104 && globals.registeredSound == 1104);
    ship.index = 43;
    ego.initializeEngineSound(&ship);
    assert(ego.field_0x1c == 1106);
    ship.index = 40;
    ego.initializeEngineSound(&ship);
    assert(ego.field_0x1c == 1107);
    ship.index = 0;
    ship.handling = 0.94f;
    ego.initializeEngineSound(&ship);
    assert(ego.field_0x1c == 42);
    ship.handling = 0.95f;
    ego.initializeEngineSound(&ship);
    assert(ego.field_0x1c == 43);
    ship.handling = 1.15f;
    ego.initializeEngineSound(&ship);
    assert(ego.field_0x1c == 44);
    ship.handling = 1.4f;
    ego.initializeEngineSound(&ship);
    assert(ego.field_0x1c == 45 && globals.registrations == 7);
    ego.lastFreeSpace = 10;
    ship.freeSpace = 5;
    ship.volatileGoods = true;
    ego.updateCargoState();
    assert(ego.lastFreeSpace == 5 && ego.volatileGoods == 1);
    ship.freeSpace = 10;
    ship.volatileGoods = false;
    ego.updateCargoState();
    assert(ego.lastFreeSpace == 5 && ego.volatileGoods == 1);
    ship.freeSpace = 5;
    ego.updateCargoState();
    assert(ego.lastFreeSpace == 5 && ego.volatileGoods == 1);
    ship.freeSpace = 4;
    ego.updateCargoState();
    assert(ego.lastFreeSpace == 4 && ego.volatileGoods == 0);

    PlayerEgo boostEgo{};
    Player boostPlayer;
    Level boostLevel;
    Engine engine;
    ApplicationManager appManager{&engine};
    Globals::appManager = &appManager;
    boostEgo.player = &boostPlayer;
    boostEgo.level = &boostLevel;
    boostEgo.boostTimer = -100;
    boostEgo.updateBoostTimer(30);
    assert(boostEgo.boostTimer == -70);
    boostEgo.updateBoostTimer(30);
    assert(boostEgo.boostTimer == 30);
    boostEgo.updateBoost(30);
    assert(boostPlayer.flShake == 0.0f && engine.postEffectCalls == 0);

    AEGeometry nozzle1, nozzle2;
    nozzle1.scaling.x = 0.5f;
    nozzle2.scaling.x = 2.0f;
    std::vector<AEGeometry *> nozzles{&nozzle1, &nozzle2};
    boostLevel.field_a4 = &nozzles;
    boostEgo.boostTimer = 0;
    boostEgo.field_0xcc = 100;
    boostEgo.field_0xd0 = 30;
    boostEgo.field_0x146 = 1;
    boostEgo.boostingFlag = 1;
    boostEgo.volatileGoods = 1;
    boostEgo.boostPercentage = 0.25f;
    boostEgo.updateBoostTimer(10);
    boostEgo.updateBoost(10);
    assert(std::fabs(boostPlayer.flShake - 0.0013f) < 0.000001f);
    assert(ParticleSettingsRef::cur.sets[29].lifeBase == 140.625f);
    assert(ParticleSettingsRef::cur.sets[30].lifeBase == 562.5f);
    assert(boostEgo.boostPercentReads == 2 && engine.postEffectCalls == 0);
    Item boostEquipment;
    boostEquipment.index = 195;
    ship.boostEquipment = &boostEquipment;
    boostEgo.updateBoostTimer(90);
    boostEgo.updateBoost(90);
    assert(std::fabs(boostPlayer.flShake - 0.0166f) < 0.000001f);
    assert(engine.postEffectCalls == 0 && boostEgo.boostingFlag == 1);
    boostEgo.updateBoostTimer(1);
    boostEgo.updateBoost(1);
    assert(engine.postEffectCalls == 1 && engine.lastEffect == 0x1400002u);
    assert(boostEgo.boostingFlag == 0 && boostEgo.speedValue == 2.0f);
    assert(boostEgo.boostTimer == -30 && boostEgo.boostSpeed == 0);
    assert(boostEgo.field_0xcc == 100 && boostEgo.field_0xd0 == 30);
    boostEgo.updateBoostTimer(10);
    assert(boostEgo.boostTimer == -20);
    boostEgo.updateBoostTimer(7);
    assert(boostEgo.boostTimer == 7);
    boostEgo.field_0x146 = 0;
    boostEgo.boostingFlag = 1;
    boostEgo.boostTimer = 101;
    boostEgo.updateBoost(0);
    assert(engine.postEffectCalls == 2 && boostEgo.boostSpeed == 2);
    assert(boostEgo.field_0xcc == 0 && boostEgo.field_0xd0 == 0);

    PlayerEgo subsystemEgo{};
    Level subsystemLevel;
    subsystemEgo.level = &subsystemLevel;
    subsystemEgo.cloakRechargeTimer = 5;
    int eventsBefore = hud.events;
    subsystemEgo.updateSubsystems(4, &hud);
    assert(subsystemEgo.cloakRechargeTimer == 1 && hud.events == eventsBefore);
    subsystemEgo.updateSubsystems(1, &hud);
    assert(subsystemEgo.cloakRechargeTimer == 0 && hud.event == 46);
    assert(hud.events == eventsBefore + 1);
    subsystemEgo.updateSubsystems(1, &hud);
    assert(hud.events == eventsBefore + 1);
    subsystemEgo.emergencySystemTimer = 3;
    subsystemEgo.updateSubsystems(3, &hud);
    assert(subsystemEgo.emergencySystemTimer == 3 && hud.events == eventsBefore + 1);

    Station station;
    status.station = &station;
    std::vector<unsigned char> blueprintFlags(5, 0);
    status.field_58 = &blueprintFlags;
    HackingGame *hacking = new HackingGame;
    subsystemEgo.hackingGame = hacking;
    subsystemEgo.shakeIntensity = 16;
    subsystemEgo.updateSubsystems(4, &hud);
    assert(hacking->updates == 1 && hacking->lastDt == 16);
    assert(hud.hackingActiveChanges == 0 && HackingGame::destructions == 0);
    hacking->won = true;
    subsystemEgo.updateSubsystems(4, &hud);
    assert(hud.hackingActiveChanges == 1 && !hud.hackingActive);
    assert(subsystemEgo.hackingGame == hacking && HackingGame::destructions == 0);
    assert(status.unlockedBlueprint == -1 && subsystemLevel.radioType == -1);

    station.hiddenBlueprint = true;
    station.hiddenBlueprintIndex = 2;
    PlayerFixedObject ordinaryEnemy, hiddenEnemy, laterEnemy;
    hiddenEnemy.field_0x70 = 1;
    hiddenEnemy.cargo = new CargoArray;
    laterEnemy.field_0x70 = 1;
    std::vector<KIPlayer *> enemies{&ordinaryEnemy, &hiddenEnemy, &laterEnemy};
    subsystemLevel.enemies = &enemies;
    subsystemEgo.updateSubsystems(4, &hud);
    assert(hud.hackingActiveChanges == 2 && subsystemEgo.hackingGame == nullptr);
    assert(HackingGame::destructions == 1 && status.unlockedBlueprint == 223);
    assert(hiddenEnemy.field_0x70 == 0 && hiddenEnemy.dockingType == 0);
    assert(hiddenEnemy.cargo == nullptr && CargoArray::destructions == 1);
    assert(laterEnemy.field_0x70 == 1 && laterEnemy.dockingType == -1);
    assert(subsystemLevel.radioType == 23 && subsystemLevel.radioParam == 0);
    assert(blueprintFlags[2] == 1);
    station.hiddenBlueprintIndex = 4;
    subsystemLevel.enemies = nullptr;
    HackingGame *secondHacking = new HackingGame;
    secondHacking->won = true;
    subsystemEgo.hackingGame = secondHacking;
    subsystemEgo.updateSubsystems(4, &hud);
    assert(status.unlockedBlueprint == 227 && blueprintFlags[4] == 1);
    assert(subsystemLevel.radioType == 25 && HackingGame::destructions == 2);

    FModSound sound;
    Globals::sound = &sound;
    Item equipment;
    ego.updateShield(0, &hud);
    assert(!ego.shieldRegenActive && sound.playCount == 0);
    ego.shieldRegenEquipped = 1;
    ship.cargo = 2;
    ego.updateShield(0, &hud);
    assert(!ego.shieldRegenActive && ship.cargo == 2);
    ship.equipment = &equipment;
    player.shieldHP = 1;
    ego.updateShield(0, &hud);
    assert(!ego.shieldRegenActive && sound.playCount == 0);
    player.shieldHP = 0;
    ship.cargo = 0;
    ego.updateShield(0, &hud);
    assert(!ego.shieldRegenActive && sound.playCount == 0);
    ship.cargo = 2;
    ego.updateShield(0, &hud);
    assert(ego.shieldRegenActive && player.shieldHP == 1);
    assert(ship.cargo == 0 && ship.removals == 1 && hud.event == 47 && hud.amount == 2);
    assert(sound.playCount == 2 && sound.played[0] == 0x8d2 && sound.played[1] == 0x8d1);
    assert(sound.updateCount == 2 && sound.updated[0] == 0x8d2 && sound.updated[1] == 0x8d1);
    assert(sound.parameter == 0.1f);
    ego.shieldRegenEquipped = 0;
    ego.updateShield(20, &hud);
    assert(ego.shieldRegenActive && player.shieldHP == 4 && ship.removals == 1);
    player.shieldHP = player.maxShieldHP;
    ego.updateShield(20, &hud);
    assert(!ego.shieldRegenActive && sound.played[2] == 0x8d3 && sound.stopped == 2257);
    assert(sound.updated[3] == 0x8d3 && ship.removals == 1);

    ship.maxShieldHP = 200;
    ship.shieldRegen = 25;
    ego.initializeRecoveryAmount(&ship);
    assert(ego.shieldRecoveryAmount == 800.0f);
    ego.shieldRecoveryAmount = 2.5f;
    ship.shieldRegen = 0;
    ego.updateRecovery(200);
    assert(ego.shieldRecoveryTimer == 0 && ego.hullRepairTimer == 0 && ego.armorRepairTimer == 0);
    ship.shieldRegen = 1;
    ego.updateRecovery(100);
    assert(ego.shieldRecoveryTimer == 100 && player.shieldRecoveries == 0);
    ego.updateRecovery(1);
    assert(ego.shieldRecoveryTimer == 0 && player.shieldRecoveries == 1);
    assert(player.lastShieldRecovery == 2.5f);
    player.hitpoints = 0;
    ego.updateRecovery(200);
    assert(ego.shieldRecoveryTimer == 0 && player.shieldRecoveries == 1);
    ship.repairType = 0;
    ego.updateRecovery(1001);
    assert(ego.hullRepairTimer == 0 && ego.armorRepairTimer == 0);
    player.hitpoints = 9;
    ship.shieldRegen = 0;
    ship.repairType = 0;
    ego.updateRecovery(600);
    assert(ego.hullRepairTimer == 600 && ego.armorRepairTimer == 600 && player.hullRepairs == 0);
    ego.updateRecovery(1);
    assert(ego.hullRepairTimer == 0 && player.hullRepairs == 1 && player.hitpoints == 10);
    ego.updateRecovery(399);
    assert(ego.armorRepairTimer == 1000 && player.armorRepairs == 0);
    ego.updateRecovery(1);
    assert(ego.armorRepairTimer == 0 && player.armorRepairs == 1);
    ship.repairType = 1;
    ego.hullRepairTimer = 0;
    ego.armorRepairTimer = 0;
    player.hitpoints = 8;
    ego.updateRecovery(420);
    assert(ego.hullRepairTimer == 420 && player.hullRepairs == 1);
    ego.updateRecovery(1);
    assert(ego.hullRepairTimer == 0 && player.hullRepairs == 2 && player.hitpoints == 9);
    ego.updateRecovery(279);
    assert(ego.armorRepairTimer == 700 && player.armorRepairs == 1);
    ego.updateRecovery(1);
    assert(ego.armorRepairTimer == 0 && player.armorRepairs == 1);
    player.hitpoints = 10;
    ego.updateRecovery(701);
    assert(ego.armorRepairTimer == 0 && player.armorRepairs == 2);

    ego.gunMuzzleRoot = reinterpret_cast<void *>(static_cast<uintptr_t>(0x1234));
    ego.engineSoundId = 99;
    ego.field_0x1c = -1;
    Globals::options[0x0f] = 1;
    ego.updatePlayerSound(16);
    assert(player.updates == 1 && player.lastUpdateDt == 16 && !player.lastDoSound);
    assert(sound.updated[4] == 99);
    ego.field_0x1c = 5;
    ego.updatePlayerSound(20);
    assert(player.updates == 2 && player.lastDoSound && sound.updated[5] == 99);
    player.hitpoints = 0;
    ego.updatePlayerSound(24);
    assert(player.updates == 3 && !player.lastDoSound && sound.updated[6] == 99);
    player.hitpoints = 10;
    Globals::options[0x0f] = 0;
    ego.updatePlayerSound(28);
    assert(sound.updateCount == 7);
    Globals::options[0x0f] = 1;
    ego.gunMuzzleRoot = reinterpret_cast<void *>(static_cast<uintptr_t>(0x100));
    ego.updatePlayerSound(32);
    assert(sound.updateCount == 7);
    ego.gunMuzzleRoot = reinterpret_cast<void *>(static_cast<uintptr_t>(0x1234));
    ego.engineSoundId = -1;
    ego.updatePlayerSound(36);
    assert(sound.updateCount == 7);

    Matrix matrix{};
    matrix.up = {1.0f, 2.0f, 3.0f};
    PaintCanvas canvas;
    canvas.local = &matrix;
    Globals::Canvas = &canvas;
    Globals::status = &status;
    Player emergencyPlayer;
    emergencyPlayer.transformMatrix.position = {10.0f, 20.0f, 30.0f};
    PlayerEgo emergencyEgo{};
    emergencyEgo.player = &emergencyPlayer;
    emergencyEgo.field_0xac = 1;
    emergencyEgo.emergencySystemTimer = 123;
    emergencyEgo.emergencyVal1 = 456;
    emergencyEgo.emergencyVec = {4.0f, 5.0f, 6.0f};
    emergencyEgo.initializeEmergency();
    assert(emergencyEgo.field_0xac == 0 && emergencyEgo.emergencySystemTimer == 0);
    assert(emergencyEgo.emergencyVal1 == 0);
    assert(emergencyEgo.emergencyVec.x == 0.0f && emergencyEgo.emergencyVec.y == 0.0f);
    assert(emergencyEgo.emergencyVec.z == 0.0f);
    emergencyEgo.emergencyVal1 = 1000;
    emergencyEgo.emergencyVal2 = 2.0f;
    emergencyEgo.emergencyVec = {1.0f, 2.0f, 3.0f};
    emergencyEgo.emergencySystemTimer = 1000;
    emergencyEgo.updateEmergency(25);
    assert(emergencyEgo.emergencySystemTimer == 1000 && emergencyPlayer.vulnerabilityWrites == 0);
    AbyssEngine::Mesh emergencyMesh;
    AbyssEngine::Transform emergencyTransform;
    canvas.emergencyMesh = &emergencyMesh;
    canvas.emergencyTransform = &emergencyTransform;
    expectedDockRotationMatrix = &emergencyPlayer.transformMatrix;
    const int geometryDestructions = AEGeometry::destructions;
    ((void *&) emergencyEgo.field_0xac) = new AEGeometry;
    emergencyEgo.emergencySystemTimer = 0;
    emergencyEgo.updateEmergency(25);
    assert(emergencyTransform.updates == 0 && AEGeometry::destructions == geometryDestructions);
    emergencyEgo.emergencySystemTimer = 1000;
    status.field_13c = 9;
    emergencyEgo.updateEmergency(25);
    AEGeometry *emergencyGeometry = (AEGeometry *)(void *&) emergencyEgo.field_0xac;
    assert(emergencyEgo.emergencySystemTimer == 975 && !emergencyPlayer.vulnerable);
    assert(emergencyGeometry->matrixWrites == 1 && emergencyGeometry->position.x == 11.0f);
    assert(emergencyGeometry->position.y == 22.0f && emergencyGeometry->position.z == 33.0f);
    assert(emergencyGeometry->scaling.x == 1.0f && emergencyMesh.shaderAnimValue1 == 0.025f);
    assert(emergencyTransform.updates == 1 && emergencyTransform.lastDt == 25);
    emergencyEgo.updateEmergency(100);
    assert(emergencyEgo.emergencySystemTimer == 875 && emergencyGeometry->scaling.x == 2.0f);
    emergencyEgo.emergencySystemTimer = 40;
    emergencyEgo.updateEmergency(20);
    assert(emergencyEgo.emergencySystemTimer == 20 && emergencyGeometry->scaling.x == 0.8f);
    const int stopsBeforeEmergency = sound.stopCount;
    emergencyEgo.updateEmergency(30);
    assert(emergencyEgo.emergencySystemTimer == -10 && emergencyEgo.field_0xac == 0);
    assert(AEGeometry::destructions == geometryDestructions + 1);
    assert(emergencyPlayer.vulnerable && emergencyPlayer.vulnerabilityWrites == 5);
    assert(sound.stopCount == stopsBeforeEmergency + 1 && sound.stopped == 1115);
    assert(status.field_13c == 0 && emergencyTransform.updates == 4);
    expectedDockRotationMatrix = nullptr;
    AEGeometry geometry;
    ego.geometry = &geometry;
    ParticleSettingsRef::cur.sets[4].count = 5;
    ParticleSettingsRef::cur.sets[4].color0 = 0x12345678;
    ParticleSettingsRef::cur.sets[4].lifeBase = 1.0f;
    ParticleSettingsRef::init.sets[4].count = 20;
    ParticleSettingsRef::init.sets[4].color0 = 0xaabbccdd;
    ParticleSettingsRef::init.sets[4].lifeBase = 9.0f;
    ego.updateVisualState();
    assert(canvas.reads == 1 && PlayerEgo::vec_up.x == 1.0f);
    assert(PlayerEgo::vec_up.y == 2.0f && PlayerEgo::vec_up.z == 3.0f);
    assert(ParticleSettingsRef::cur.sets[4].count == 20);
    assert(ParticleSettingsRef::cur.sets[4].lifeBase == 9.0f);
    assert(ParticleSettingsRef::cur.sets[4].color0 == 0x12345678);
    assert(ParticleSettings::interpolations == 0 && ego.boostPercentReads == 0);
    matrix.up = {-1.0f, 0.0f, 1.0f};
    ego.boostingFlag = 1;
    ego.boostPercentage = 0.25f;
    ego.updateVisualState();
    assert(canvas.reads == 2 && PlayerEgo::vec_up.x == -1.0f);
    assert(ParticleSettings::interpolations == 1 && ParticleSettings::blend == 0.25f);
    assert(ego.boostPercentReads == 1);
    assert(ParticleSettingsRef::cur.sets[4].color0 == 0x12345678);

    const int oldEvents = hud.events;
    ego.updateRoute(&hud);
    assert(hud.events == oldEvents);
    Route route;
    ego.route = &route;
    ego.dockOffsetVec = {7.0f, 8.0f, 9.0f};
    ego.updateRoute(&hud);
    assert(route.updates == 1 && route.lastPosition.x == 7.0f);
    assert(route.lastPosition.y == 8.0f && route.lastPosition.z == 9.0f);
    assert(hud.events == oldEvents);
    route.next = 1;
    ego.updateRoute(&hud);
    assert(hud.events == oldEvents + 1 && hud.event == 23 && hud.amount == 0);
    route.next = 0;
    ego.updateRoute(&hud);
    assert(hud.events == oldEvents + 2 && hud.event == 24);
    route.current = 1;
    route.loop = 1;
    ego.updateRoute(&hud);
    assert(hud.events == oldEvents + 3 && hud.event == 23);

    player.hitpoints = 10;
    player.armorHP = 0;
    player.shieldHP = 0;
    ego.lastHP = player.getCombinedHP();
    ego.hudHitDirectionFlags = 0xff;
    ego.shieldHitFlash = 1;
    ego.updateHitState(&hud);
    assert(ego.hudHitDirectionFlags == 0 && ego.shieldHitFlash == 0);
    assert(ego.cursorStateDirty == 0 && hud.hits == 0 && canvas.projections == 0);
    assert(camera.fastForwardQueries == 0 && ego.cursorStateByte == 0);
    Globals::mouseCursorActivated = 1;
    camera.fastForward = true;
    ego.updateHitState(&hud);
    assert(ego.cursorStateDirty == 1 && ego.cursorStateByte == 1);
    assert(camera.fastForwardQueries == 1 && camera.handlingSets == 0);
    camera.fastForward = false;
    ego.handling = 1.25f;
    ego.updateHitState(&hud);
    assert(ego.cursorStateDirty == 0 && camera.handlingSets == 1);
    assert(camera.lastHandling == 1.25f && camera.handlingResets == 0);
    Globals::mouseCursorActivated = 0;
    ego.updateHitState(&hud);
    assert(ego.cursorStateDirty == 0 && ego.cursorStateByte == 0);
    assert(camera.handlingSets == 1 && camera.handlingResets == 1);
    Globals::mouseCursorActivated = 2;
    ego.updateHitState(&hud);
    assert(ego.cursorStateByte == 1 && camera.handlingSets == 2);
    Globals::mouseCursorActivated = 0;
    ego.updateHitState(&hud);
    assert(ego.cursorStateByte == 0 && camera.handlingResets == 2);

    ego.lastHP = 11;
    player.shieldHit = 1;
    player.armorHit = 1;
    player.hullHit = 1;
    ego.computerControlled = 1;
    const int soundsBeforeHit = sound.playCount;
    ego.updateHitState(&hud);
    assert(hud.hits == 0 && ego.lastHP == 11 && sound.playCount == soundsBeforeHit);
    assert(canvas.projections == 0 && player.hitVectorReads == 0);
    ego.computerControlled = 0;
    ego.freeze = 1;
    ego.updateHitState(&hud);
    assert(hud.hits == 0 && player.shieldHit == 1);
    ego.freeze = 0;
    ego.updateHitState(&hud);
    assert(hud.hits == 1 && ego.hitCameraCalls == 1 && ego.lastHP == 10);
    assert(ego.shieldHitFlash == 1 && sound.played[soundsBeforeHit] == 25);
    assert(player.shieldHit == 0 && player.armorHit == 1 && player.hullHit == 1);
    assert(canvas.projections == 1 && player.hitVectorReads == 1);
    assert(std::fabs(canvas.lastWorldPosition.x - 3007.0f) < 0.01f);
    assert(std::fabs(canvas.lastWorldPosition.y - 4008.0f) < 0.01f);
    assert(canvas.lastWorldPosition.z == 9.0f && ego.hudHitDirectionFlags == 0x21u);
    player.hitpoints = 9;
    ego.updateHitState(&hud);
    assert(hud.hits == 2 && ego.lastHP == 9 && sound.played[soundsBeforeHit + 1] == 23);
    assert(player.armorHit == 0 && player.hullHit == 1 && ego.shieldHitFlash == 0);
    player.hitpoints = 8;
    ego.updateHitState(&hud);
    assert(hud.hits == 3 && ego.lastHP == 8 && sound.played[soundsBeforeHit + 2] == 24);
    assert(player.hullHit == 0);
    player.hitpoints = 7;
    ego.updateHitState(&hud);
    assert(hud.hits == 4 && ego.hitCameraCalls == 4 && sound.playCount == soundsBeforeHit + 3);
    player.hitpoints = 10;
    ego.updateHitState(&hud);
    assert(hud.hits == 4 && ego.lastHP == 7 && canvas.projections == 4);

    auto checkHitFlags = [&](float x, float z, unsigned int expected) {
        canvas.screenResult = {x, 0.0f, z};
        ego.lastHP = player.getCombinedHP() + 1;
        int previousProjections = canvas.projections;
        ego.updateHitState(&hud);
        assert(canvas.projections == previousProjections + 1);
        assert(ego.hudHitDirectionFlags == expected);
    };
    checkHitFlags(-60000.0f, 1.0f, 0x01u);
    checkHitFlags(-20000.0f, 1.0f, 0x21u);
    checkHitFlags(400.0f, 1.0f, 0x20u);
    checkHitFlags(900.0f, 1.0f, 0x22u);
    checkHitFlags(50000.0f, 1.0f, 0x22u);
    checkHitFlags(60000.0f, 1.0f, 0x02u);
    checkHitFlags(400.0f, -1.0f, 0x10u);
    checkHitFlags(-60000.0f, -1.0f, 0x11u);

    Globals::mouseCursorActivated = 0;
    player.transformMatrix.position = {10.0f, 20.0f, 30.0f};
    player.transformMatrix.dir = {0.0f, 0.0f, 2.0f};
    canvas.screenResult = {320.0f, 240.0f, 5.0f};
    int previousProjections = canvas.projections;
    ego.updateCrosshair(16, false, 1);
    assert(canvas.projections == previousProjections + 1);
    assert(canvas.lastWorldPosition.x == 10.0f && canvas.lastWorldPosition.y == 20.0f);
    assert(canvas.lastWorldPosition.z == 22030.0f);
    assert(PlayerEgo::crosshairPos.x == 320.0f && ego.crosshairPosPrevious.z == 5.0f);

    canvas.screenResult = {100.0f, 140.0f, -5.0f};
    ego.updateCrosshair(16, false, 1);
    assert(PlayerEgo::crosshairPos.x == 276.0f && PlayerEgo::crosshairPos.y == 220.0f);
    assert(PlayerEgo::crosshairPos.z == 3.0f && ego.crosshairPosPrevious.x == 276.0f);

    Globals::mouseCursorActivated = 1;
    previousProjections = canvas.projections;
    Globals::mouseDeltaX = 100;
    Globals::mouseDeltaY = -90;
    canvas.screenResult = {500.0f, 320.0f, 7.0f};
    ego.updateCrosshair(16, false, 1);
    assert(canvas.projections == previousProjections + 1);
    assert(PlayerEgo::crosshairPos.x == 500.0f && PlayerEgo::crosshairPos.y == 210.0f);
    assert(ego.crosshairPosPrevious.x == 500.0f);
    assert(PlayerEgo::crosshairShootPos.x == 500.0f && ego.crosshairShootPosPrevious.z == 7.0f);
    assert(ego.rightCalls == 1 && ego.upCalls == 1);
    assert(std::fabs(ego.lastRight - 100.0f / 280.0f) < 0.00001f);
    assert(std::fabs(ego.lastUp - 90.0f / 210.0f) < 0.00001f);

    Globals::mouseDeltaX = 999;
    Globals::mouseDeltaY = -999;
    canvas.screenResult = {100.0f, 120.0f, -3.0f};
    ego.updateCrosshair(16, false, 1);
    assert(Globals::mouseDeltaX == 280 && Globals::mouseDeltaY == -210);
    assert(PlayerEgo::crosshairPos.x == 680.0f && PlayerEgo::crosshairPos.y == 90.0f);
    assert(PlayerEgo::crosshairShootPos.x == 420.0f && PlayerEgo::crosshairShootPos.z == 5.0f);
    assert(ego.rightCalls == 2 && ego.upCalls == 2 && ego.lastRight == 1.0f && ego.lastUp == 1.0f);

    Globals::mouseDeltaX = -71;
    Globals::mouseDeltaY = 42;
    canvas.screenResult = {400.0f, 300.0f, 1.0f};
    ego.updateCrosshair(16, false, 1);
    assert(PlayerEgo::crosshairPos.x == 329.0f && PlayerEgo::crosshairPos.y == 342.0f);
    assert(ego.leftCalls == 1 && ego.downCalls == 1);
    assert(std::fabs(ego.lastLeft - 71.0f / 280.0f) < 0.00001f);
    assert(std::fabs(ego.lastDown - 42.0f / 210.0f) < 0.00001f);

    previousProjections = canvas.projections;
    ego.updateCrosshair(16, true, 1);
    assert(canvas.projections == previousProjections + 1);
    ego.updateCrosshair(16, false, 3);
    assert(canvas.projections == previousProjections + 2);
    ego.autoPilot = 1;
    ego.updateCrosshair(16, false, 0);
    assert(canvas.projections == previousProjections + 3);
    ego.updateCrosshair(16, false, 1);
    assert(canvas.projections == previousProjections + 4);

    AEGeometry cameraGeometry;
    AEGeometry turretGeometry;
    expectedFirstMatrix = &cameraGeometry.matrix;
    expectedSecondMatrix = &turretGeometry.matrix;
    combinedMatrix.position = {40.0f, 50.0f, 60.0f};
    combinedMatrix.dir = {1.0f, 0.0f, 0.0f};
    ego.field_0x2c = &cameraGeometry;
    ego.turretGeometry = &turretGeometry;
    ego.turretActive = 1;
    Globals::mouseCursorActivated = 0;
    ego.updateCrosshair(16, false, 1);
    assert(canvas.lastWorldPosition.x == 22040.0f);
    assert(canvas.lastWorldPosition.y == 50.0f && canvas.lastWorldPosition.z == 60.0f);
    assert(ego.crosshairPosPrevious.x == 400.0f);

    AEGeometry bodyGeometry;
    AEGeometry shipGeometry;
    bodyGeometry.transform = 18;
    shipGeometry.position = {1.0f, 2.0f, 3.0f};
    ego.field_0x4 = &bodyGeometry;
    ego.geometry = &shipGeometry;
    ParticleSystemManager particles;
    Level level;
    level.field_74 = &particles;
    level.field_3c = 77;
    ego.level = &level;
    ego.currentSystem = 9;
    ego.shakeIntensity = 20;
    ego.explosionTimer = 2990;
    Explosion primaryExplosion, secondaryExplosion;
    ego.explosion = &primaryExplosion;
    ego.explosion2 = &secondaryExplosion;
    ego.updateTail({4.0f, 5.0f, 6.0f}, {0.0f, 0.0f, 1.0f});
    assert(player.stopShots == 0 && primaryExplosion.starts == 0 && ego.explosionTimer == 2990);

    player.hitpoints = 0;
    ego.explosion = nullptr;
    ego.updateTail({4.0f, 5.0f, 6.0f}, {0.0f, 0.0f, 1.0f});
    assert(player.stopShots == 0 && ego.explosionTimer == 2990);
    ego.explosion = &primaryExplosion;
    ego.rocketControlGun = 1;
    ego.rocketBanking = 1;
    ego.dockedFlag = 0;
    int priorStops = sound.stopCount;
    ego.updateTail({4.0f, 5.0f, 6.0f}, {0.0f, 0.0f, 1.0f});
    assert(player.stopShots == 1 && sound.stopCount == priorStops + 2 && sound.stopped == 0);
    assert(bodyGeometry.rotations == 1 && primaryExplosion.starts == 1);
    assert(primaryExplosion.lastPosition.x == 1.0f && primaryExplosion.lastDirection.x == 0.0f);
    assert(particles.emitDisables == 1 && particles.renderDisables == 1 && particles.manualEmits == 1);
    assert(particles.lastHandle == 77 && particles.lastSet == 0);
    assert(particles.lastPosition.x == 4.0f && particles.lastVelocity.z == 1.0f);
    assert(secondaryExplosion.updates == 1 && secondaryExplosion.lastDt == 20 && ego.explosionTimer == 3010);

    ego.dockedFlag = 1;
    ego.dockingPointIndex = 1;
    ego.explosionTimer = 3000;
    ego.updateTail({4.0f, 5.0f, 6.0f}, {0.0f, 0.0f, 1.0f});
    assert(primaryExplosion.starts == 1 && primaryExplosion.updates == 0);
    assert(bodyGeometry.rotations == 1 && ego.explosionTimer == 3020);
    ego.explosionTimer = 3001;
    ego.updateTail({4.0f, 5.0f, 6.0f}, {0.0f, 0.0f, 1.0f});
    assert(primaryExplosion.updates == 1 && primaryExplosion.lastDt == 20);
    assert(primaryExplosion.lastPosition.z == 3.0f && ego.explosionTimer == 3021);

    player.hitpoints = 10;
    ego.turretMode = 1;
    Matrix shipLocal{}, bodyLocal{};
    canvas.local = &shipLocal;
    canvas.otherLocal = &bodyLocal;
    expectedFirstMatrix = &shipLocal;
    expectedSecondMatrix = &bodyLocal;
    combinedMatrix.position = {9.0f, 8.0f, 7.0f};
    ego.updateTail({4.0f, 5.0f, 6.0f}, {0.0f, 0.0f, 1.0f});
    assert(cameraGeometry.matrixWrites == 1 && cameraGeometry.matrix.position.x == 9.0f);

    ego.asteroidTargetPtr = &asteroid;
    ego.dockedFlag = 1;
    ego.updateDocking(&radar, &hud, &radio);
    assert(ego.dockedFlag == 0 && ego.dockPointStops == 1);

    PlayerEgo dockEgo{};
    Level dockLevel;
    PlayerFixedObject dockTarget;
    Mission mission, campaign;
    status.mission = &mission;
    status.campaignMission = &campaign;
    dockEgo.level = &dockLevel;
    dockEgo.asteroidTargetPtr = &dockTarget;
    dockEgo.shakeIntensity = 1000;
    dockEgo.dockingPointIndex = 1;
    assert(dockEgo.dockStateGate() == 1);
    dockTarget.state = 3;
    assert(dockEgo.dockStateGate() == 0);
    dockTarget.state = 0;
    dockEgo.dockingPointIndex = 0;
    assert(dockEgo.dockStateGate() == -1);

    PlayerEgo approachEgo{};
    TargetFollowCamera approachCamera;
    LevelScript approachScript;
    SpacePoint dockNav;
    SpacePoint dockSpot;
    AEGeometry dockGeometry;
    AEGeometry approachShipGeometry;
    AEGeometry approachBodyGeometry;
    Radar approachRadar;
    approachScript.m_pLevel = &dockLevel;
    approachEgo.player = &player;
    approachEgo.level = &dockLevel;
    approachEgo.levelScript = &approachScript;
    approachEgo.targetFollowCameraPtr = &approachCamera;
    approachEgo.asteroidTargetPtr = &dockTarget;
    approachEgo.geometry = &approachShipGeometry;
    approachEgo.field_0x4 = &approachBodyGeometry;
    approachEgo.position = {0.0f, 0.0f, 0.0f};
    approachEgo.shakeIntensity = 16;
    approachEgo.dockScaling = 1400;
    approachEgo.dockApproachThreshold = 2000;
    dockTarget.nearestNav = &dockNav;
    dockTarget.nearestDock = &dockSpot;
    dockTarget.geometry = &dockGeometry;
    expectedDockRotationMatrix = &dockGeometry.matrix;
    dockNav.position = {400.0f, 5.0f, 6.0f};
    dockSpot.position = {390.0f, 5.0f, 6.0f};
    dockSpot.direction = {0.0f, 0.0f, 1.0f};
    approachShipGeometry.matrix.position = {3.0f, 4.0f, 5.0f};
    dockTarget.field_0x70 = 1;
    dockTarget.field_0x8c = 1;
    ship.handling = 1.0f;
    dockTarget.position = {19600.0f, 0.0f, 0.0f};
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 0);
    assert(approachEgo.moveCalls == 1 && approachEgo.lastMoveSteer);
    assert(std::fabs(approachEgo.lastMoveSpeed - 3.7f) < 0.0001f);
    assert(approachEgo.lastMovePosition.x == 20000.0f && approachEgo.lastMovePosition.y == 5.0f);
    float waypointWorldY, waypointZ;
    std::memcpy(&waypointWorldY, &approachEgo.gunBaseGeo, sizeof(float));
    std::memcpy(&waypointZ, &approachEgo.pitchAccumDir, sizeof(float));
    assert(approachEgo.waypointX == 0.0f && approachEgo.waypointY == 20000.0f);
    assert(waypointWorldY == 5.0f && waypointZ == 6.0f);
    assert(approachEgo.aboutToReachAutoTargetFlag == 0);
    ship.handling = 2.0f;
    dockTarget.position = {19599.0f, 0.0f, 0.0f};
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 0);
    assert(approachEgo.moveCalls == 2 && approachEgo.lastMoveSpeed == 4.0f);
    assert(approachEgo.aboutToReachAutoTargetFlag == 1);
    dockTarget.position = {1000.0f, 0.0f, 0.0f};
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 0);
    assert(approachEgo.dockingPointIndex == 0 && approachEgo.moveCalls == 3);
    assert(approachEgo.spacePoint == &dockNav && dockNav.takes == 1);
    approachEgo.easeMatrix = new AbyssEngine::EaseInOutMatrix(Matrix{}, Matrix{}, 100);
    int oldEaseCreations = AbyssEngine::EaseInOutMatrix::creations;
    int oldEaseDestructions = AbyssEngine::EaseInOutMatrix::destructions;
    int oldDockSoundPlays = sound.playCount;
    int oldDockSoundUpdates = sound.updateCount;
    dockTarget.position = {999.0f, 0.0f, 0.0f};
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 0);
    assert(approachEgo.dockingPointIndex == 2 && approachCamera.lookAt);
    assert(dockTarget.field_0x8c == 0 && approachEgo.moveCalls == 3);
    assert(dockTarget.dockQueries == 1 && dockTarget.lastDockQuery.x == 1399.0f);
    assert(dockTarget.lastDockQuery.y == 5.0f && dockTarget.lastDockQuery.z == 6.0f);
    auto *dockEase = static_cast<AbyssEngine::EaseInOutMatrix *>(approachEgo.easeMatrix);
    assert(dockEase != nullptr && dockEase->duration == 2000);
    assert(dockEase->min.position.x == 3.0f && dockEase->min.position.y == 4.0f);
    assert(dockEase->max.dir.z == 1.0f && dockEase->max.up.x == 1.0f && dockEase->max.right.y == -1.0f);
    assert(AbyssEngine::EaseInOutMatrix::creations == oldEaseCreations + 1);
    assert(AbyssEngine::EaseInOutMatrix::destructions == oldEaseDestructions + 1);
    assert(sound.playCount == oldDockSoundPlays + 1 && sound.played[oldDockSoundPlays] == 0x8de);
    assert(sound.updateCount == oldDockSoundUpdates + 1 && sound.updated[oldDockSoundUpdates] == 0x8de);
    assert(approachEgo.engineSoundStops == 1);
    ship.handling = 1.4f;

    dockTarget.position = {-190.0f, 0.0f, 0.0f};
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 0);
    assert(approachEgo.aboutToReachAutoTargetFlag == 1 && dockEase->increaseCalls == 1);
    assert(dockEase->elapsed == 16 && approachBodyGeometry.matrixWrites == 1);
    assert(approachShipGeometry.matrixWrites == 1 && dockTarget.dockQueries == 2);
    assert(std::fabs(approachShipGeometry.matrix.position.x - 1.92f) < 0.0001f);
    assert(std::fabs(approachEgo.dockOffsetVec.x - 1.92f) < 0.0001f);
    assert(std::fabs(player.transformMatrix.position.x - 1.92f) < 0.0001f);
    dockTarget.position = {-191.0f, 0.0f, 0.0f};
    approachEgo.field_0xb2 = 1;
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 1);
    assert(approachEgo.dockingPointIndex == 1 && approachEgo.field_0xb2 == 0);
    assert(approachEgo.switchToFreeLookCam == 0 && dockEase->increaseCalls == 1);
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 1);

    approachEgo.dockingPointIndex = 3;
    approachEgo.dockedFlag = 1;
    approachEgo.field_0x145 = 1;
    approachEgo.dockApproachDist = 1;
    approachEgo.dockApproachThreshold = 2;
    approachEgo.field_0x1d0 = 3;
    approachEgo.dockStation = &dockLevel;
    approachRadar.dockTargetPtr = &dockTarget;
    approachRadar.dockNavPtr = &dockNav;
    dockTarget.position = {-200.0f, 0.0f, 0.0f};
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 0);
    assert(approachEgo.dockedFlag == 1 && dockEase->increaseCalls == 2);
    assert(dockEase->elapsed == 32 && approachBodyGeometry.matrixWrites == 2);
    assert(std::fabs(approachShipGeometry.matrix.position.x - 3.84f) < 0.0001f);
    approachEgo.hackingGame = new HackingGame;
    int oldHackingDestructions = HackingGame::destructions;
    int oldGunResets = player.resetCount;
    dockTarget.position = {-201.0f, 0.0f, 0.0f};
    assert(approachEgo.approachDockingPoint(&hud, 0, &approachRadar) == 0);
    assert(approachEgo.dockedFlag == 0 && approachEgo.field_0x145 == 0);
    assert(approachEgo.asteroidTarget == 0 && approachEgo.dockStation == &dockLevel);
    assert(approachEgo.dockingPointIndex == 0 && approachEgo.switchToStandardCam == 1);
    assert(approachEgo.dockApproachDist == 0 && approachEgo.dockApproachThreshold == 0 && approachEgo.field_0x1d0 == 0);
    assert(approachRadar.dockTargetPtr == nullptr && approachRadar.dockNavPtr == nullptr);
    assert(approachCamera.active && !approachCamera.lookAt && !approachCamera.targetsUp);
    assert(approachScript.cameraResets == 1 && player.resetCount == oldGunResets + 1);
    assert(approachEgo.spacePoint == nullptr && dockNav.releases == 1);
    assert(approachEgo.hackingGame == nullptr && HackingGame::destructions == oldHackingDestructions + 1);
    assert(!hud.hackingActive && dockEase->increaseCalls == 2);
    delete static_cast<AbyssEngine::EaseInOutMatrix *>(approachEgo.easeMatrix);
    approachEgo.easeMatrix = nullptr;

    dockTarget.dockingType = 1;
    mission.empty = false;
    mission.type = 15;
    mission.productionGoodAmount = 2;
    Item dockCargo;
    dockCargo.amount = 5;
    ship.cargoItem = &dockCargo;
    ship.cargo = 10;
    dockEgo.dockTransferTimer = 17;
    dockEgo.dockTransferedAmount = 7;
    dockEgo.initializeDockTransfer(&hud);
    assert(dockEgo.dockTotalAmount == 2 && dockEgo.dockTransferedAmount == 0);
    assert(dockEgo.dockTransferTimer == 0 && hud.event == 41);
    assert(dockEgo.dockingPointIndex == 1 && dockEgo.switchToFreeLookCam == 1);
    dockEgo.updateDockFinish(&hud);
    assert(dockEgo.dockTransferedAmount == 0 && dockEgo.dockTransferTimer == 1000);
    dockEgo.updateDockFinish(&hud);
    assert(dockEgo.dockTransferedAmount == 1 && dockLevel.deliveredOre == 1);
    assert(ship.lastRemovedIndex == 202 && ship.lastRemovedAmount == 1);
    int dockEvents = hud.events;
    dockEgo.updateDockFinish(&hud);
    assert(dockEgo.dockTransferedAmount == 2 && dockLevel.deliveredOre == 2);
    assert(hud.events == dockEvents + 1 && hud.event == 42);
    dockEgo.updateDockFinish(&hud);
    assert(dockLevel.deliveredOre == 2 && dockEgo.dockTransferTimer == 4000);

    mission.type = 184;
    mission.statusValue = 1;
    status.field_178 = 2;
    dockEgo.initializeDockTransfer(&hud);
    assert(dockEgo.dockTotalAmount == 2 && hud.event == 37);
    dockEgo.dockTransferTimer = 3000;
    dockEgo.updateDockFinish(&hud);
    assert(dockEgo.dockTransferedAmount == 2 && hud.event == 38);
    assert(status.field_178 == 0 && mission.statusValue == 0);

    mission.type = 168;
    mission.statusValue = 2;
    dockTarget.dockingType = 2;
    status.field_178 = 0;
    ship.maxPassengers = 0;
    dockEgo.initializeDockTransfer(&hud);
    assert(dockEgo.dockTotalAmount == 2 && hud.event == 35);
    dockEgo.dockTransferTimer = 3000;
    dockEgo.updateDockFinish(&hud);
    assert(dockEgo.dockTransferedAmount == 2 && hud.event == 36);
    assert(status.field_178 == 2);

    mission.empty = true;
    campaign.empty = false;
    campaign.type = 167;
    campaign.statusValue = 5;
    campaign.productionGoodAmount = 7;
    dockTarget.dockingType = 1;
    ship.cargo = 10;
    dockEgo.initializeDockTransfer(&hud);
    assert(dockEgo.dockTotalAmount == 2 && hud.event == 41);
    dockEgo.dockTransferTimer = 2000;
    dockEgo.updateDockFinish(&hud);
    assert(dockEgo.dockTransferedAmount == 2 && hud.event == 42);
    assert(ship.lastRemovedAmount == 2 && campaign.statusValue == 6);

    campaign.empty = true;
    dockTarget.dockingType = 3;
    dockEgo.dockTransferTimer = 0;
    dockLevel.dockingTargets = {41, dockEgo.asteroidTarget, 42, dockEgo.asteroidTarget};
    status.currentCampaignMission = 91;
    int hackedEvents = hud.hackingActiveChanges;
    dockEgo.updateDockFinish(&hud);
    HackingGame *dockHacking = static_cast<HackingGame *>(dockEgo.hackingGame);
    assert(dockHacking != nullptr && dockHacking->constructorType == 0);
    assert(dockHacking->constructorMode == 1 && dockHacking->constructorDockIndex == 3);
    assert(hud.hackingActiveChanges == hackedEvents + 1 && hud.hackingActive);
    assert(dockEgo.dockTransferTimer == 1000);
    delete dockHacking;
    dockEgo.hackingGame = nullptr;
    status.currentCampaignMission = 90;
    dockLevel.dockingTargets = {41, 42};
    dockEgo.updateDockFinish(&hud);
    dockHacking = static_cast<HackingGame *>(dockEgo.hackingGame);
    assert(dockHacking->constructorMode == 4 && dockHacking->constructorDockIndex == 0);
    delete dockHacking;
    dockEgo.hackingGame = nullptr;
    std::puts("PlayerEgo docking/cargo/shield/repair/sound/visual/route/hit/tail smoke: OK");
}
