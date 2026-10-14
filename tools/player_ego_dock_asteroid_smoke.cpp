#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

struct Vector { float x, y, z; };
Vector operator*(const Vector &value, float factor) {
    return {value.x * factor, value.y * factor, value.z * factor};
}
Vector operator+(const Vector &a, const Vector &b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
struct Matrix { Vector up; };
namespace AbyssEngine { namespace AEMath {
Vector MatrixGetUp(const Matrix &matrix) { return matrix.up; }
Vector VectorNormalize(const Vector &value) {
    float length = std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
    return length == 0.0f ? Vector{} : Vector{value.x / length, value.y / length, value.z / length};
}
} }
struct AEGeometry { unsigned int transform = 17; };
struct PaintCanvas {
    Matrix *local = nullptr;
    int reads = 0;
    Vector screenResult{};
    Vector lastWorldPosition{};
    int projections = 0;
    void *TransformGetLocal(unsigned int id) { assert(id == 17); ++reads; return local; }
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
    struct Table { ParticleDefinition sets[7]; };
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
int ParticleSettings::interpolations = 0;
float ParticleSettings::blend = -1.0f;
struct KIPlayer {
    int state = 0;
    bool isDying() { return state == 3; }
    bool isDead() { return state == 4; }
};
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
    bool fastForward = false;
    int fastForwardQueries = 0;
    int handlingSets = 0;
    int handlingResets = 0;
    float lastHandling = 0.0f;
    void setActive(bool value) { active = value; }
    bool isInFastForwardMode() { ++fastForwardQueries; return fastForward; }
    void setShipHandling(float value) { ++handlingSets; lastHandling = value; }
    void resetShipHandling() { ++handlingResets; }
};
struct Player {
    int resetCount = 0;
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
};
struct PlayerEgo;
struct Hud {
    int event = -1;
    int amount = -1;
    int events = 0;
    int hits = 0;
    void hudEvent(int id, PlayerEgo *, int value) { event = id; amount = value; ++events; }
    void playerHit() { ++hits; }
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
    int getAttribute(int id) { assert(id == 59); return amount; }
};
struct Ship {
    int freeSpace = 0;
    bool volatileGoods = false;
    Item *equipment = nullptr;
    int cargo = 0;
    int removals = 0;
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
    Item *getFirstEquipmentOfSort(int sort) { assert(sort == 43); return equipment; }
    bool hasCargo(int index, int amount) { assert(index == 202); return cargo >= amount; }
    int removeCargo(int index, int amount) {
        assert(index == 202 && cargo >= amount);
        cargo -= amount;
        ++removals;
        return cargo;
    }
};
struct Status {
    Ship *ship = nullptr;
    Ship *getShip() { return ship; }
};
static Status status;
Status *PE_status() { return &status; }
struct PlayerEgo;
void PE_upd_docksFinishDelivery(PlayerEgo *, Radio *);
static int destroyedMiningGames = 0;
struct MiningGame { ~MiningGame() { ++destroyedMiningGames; } };
struct FModSound {
    int played[16]{};
    int playCount = 0;
    int stopped = 0;
    int updated[8]{};
    int updateCount = 0;
    float parameter = -1.0f;
    void play(int id, Vector *, Vector *, float) { played[playCount++] = id; }
    void stop(int id) { stopped = id; }
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
namespace Globals {
    FModSound *sound = nullptr;
    GlobalsService *gGlobals = nullptr;
    PaintCanvas *Canvas = nullptr;
    int mouseCursorActivated = 0;
    int w = 800;
    unsigned char options[100]{};
}
static const float g_PE_astApproach = 2500.0f;
void Vec_assign(void *dst, const void *src) { std::memcpy(dst, src, 3 * sizeof(int)); }

struct PlayerEgo {
    static Vector vec_up;
    union { int asteroidTarget; void *asteroidTargetPtr; };
    union { int targetFollowCamera; void *targetFollowCameraPtr; };
    union { int miningGame; void *miningGamePtr; };
    Player *player = nullptr;
    int dockingState = 0;
    int miningSettleTimer = 0;
    int dockingPointIndex = 0;
    int dockScaling = 0;
    int dockApproachDist[3]{};
    int dockedFlag = 0;
    int approachCount = 0;
    int dockPointStops = 0;
    unsigned char field_0x145 = 0;
    int lastFreeSpace = 0;
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
    unsigned char boostingFlag = 0;
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
    void updateCargoState();
    void updateShield(int dt, Hud *hud);
    void updateRecovery(int dt);
    void updatePlayerSound(int dt);
    void updateVisualState();
    void updateRoute(Hud *hud);
    void updateHitState(Hud *hud);
    void initializeRecoveryAmount(Ship *ship);
    void initializeEngineSound(Ship *ship);
    bool isDead() { return player->hitpoints < 1; }
    float getBoostPercentage() { ++boostPercentReads; return boostPercentage; }
    void hitCamera() { ++hitCameraCalls; }
    Vector getPosition() { return position; }
    int approachAsteroid(Hud *, int, Radar *) { ++approachCount; return 0; }
    int approachDockingPoint(Hud *, int, Radar *) { return 0; }
    void dockToDockingPoint(KIPlayer *, Radar *) { dockedFlag = 0; ++dockPointStops; }
};
Vector PlayerEgo::vec_up{};

// PRODUCTION_METHOD
// PRODUCTION_UPDATE_DOCKING
// PRODUCTION_UPDATE_CARGO
// PRODUCTION_UPDATE_SHIELD
// PRODUCTION_UPDATE_RECOVERY
// PRODUCTION_UPDATE_PLAYER_SOUND
// PRODUCTION_UPDATE_VISUAL_STATE
// PRODUCTION_UPDATE_ROUTE
// PRODUCTION_UPDATE_HIT_STATE
// PRODUCTION_RECOVERY_AMOUNT
// PRODUCTION_ENGINE_SOUND_SELECTION

void PE_upd_docksFinishDelivery(PlayerEgo *, Radio *) { assert(false); }

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

    Matrix matrix{{1.0f, 2.0f, 3.0f}};
    PaintCanvas canvas;
    canvas.local = &matrix;
    Globals::Canvas = &canvas;
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

    ego.asteroidTargetPtr = &asteroid;
    ego.dockedFlag = 1;
    ego.updateDocking(&radar, &hud, &radio);
    assert(ego.dockedFlag == 0 && ego.dockPointStops == 1);
    std::puts("PlayerEgo docking/cargo/shield/repair/sound/visual/route/hit smoke: OK");
}
