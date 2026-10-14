#ifndef GOF2_PLAYEREGO_H
#define GOF2_PLAYEREGO_H
#include <cstdint>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "TargetFollowCamera.h"
#include "engine/math/Vector.h"
#include "engine/math/Matrix.h"
#include "engine/render/AEGeometry.h"
#include "game/weapons/Radar.h"
#include "game/weapons/RepairBeam.h"

#include "engine/math/AEMath.h"



#include "game/ship/ExplosionEmitterHolder.h"

#include "game/ship/MiningInputFlags.h"
class Player;
class Radar;


class AEGeometry;
class Gun;
class Hud;
class KIPlayer;
class Level;
class LevelScript;
class Radio;
class RepairBeam;
class Route;
class SpacePoint;
class TargetFollowCamera;


typedef AbyssEngine::AEMath::Vector Vec3;



class PlayerEgo {
public:
    void *player;

    // Bytes 0x4..0x40 hold the ship transform Matrix (sizeof 0x3c) in the
    // binary; the same storage is also accessed as the named pointer fields
    // below. Expose both via an anonymous union without reordering.
    //
    // The outer union additionally exposes `rocketReturnMatrix`, a Matrix that
    // begins at byte 0x10 (0xC into this storage region) and runs to 0x4c.
    // The original constructor also constructs a separate Matrix at 0x40;
    // maneuver parameters and the camera pointer follow that Matrix.
    union {
        struct {
            union {
                float transform[15];

                struct {
                    AEGeometry *field_0x4;
                    AEGeometry *geometry;
                    Level *level;
                    LevelScript *levelScript;
                    void *field_0x14;
                    Radio *radioRef;
                    int field_0x1c; // Engine sound resource; distinct from engineSoundId at +0xb4.
                    union {
                        int field_0x20;
                        unsigned int hudHitDirectionFlags;
                    };
                    uint8_t freeze;
                    uint8_t inWormhole;
                    void *turretGeometry;
                    void *field_0x2c;
                    void *field_0x30;
                    void *gunYawGeo;
                    void *gunMuzzleRoot;
                    void *gunExtraGeo;
                };
            };
            float matrix_0x40_storage[15];
            float maneuverParam;
            float field_0x80;
            uint8_t cursorStateDirty;
            char _unknown_0x85[3];
            int targetFollowCamera;
        };

        struct {
            char _rocketReturnMatrixPad[0xC];
            float rocketReturnMatrixStorage[15];
        };
    };
    void *explosion;
    void *explosion2;
    AbyssEngine::AEMath::Vector crosshairPosPrevious;
    AbyssEngine::AEMath::Vector crosshairShootPosPrevious;
    int field_0xac;
    union {
        int field_0xb0;
        struct {
            uint8_t switchToStandardCam;
            uint8_t switchToFreeLookCam;
            uint8_t field_0xb2;
            uint8_t field_0xb3;
        };
    };
    int engineSoundId;
    union {
        int speed; // Legacy bit view used by unreconstructed methods.
        float speedValue;
    };
    float thrust;
    char _unknown_0xc0[4];
    uint8_t freeLookMode;
    int boostSpeed;
    int field_0xcc;
    int field_0xd0;
    void *boostSoundId;
    void *rollGeometry;
    float rotX;
    float rotY;
    float rotZ;
    float waypointX;
    float waypointY;
    void *gunBaseGeo;
    int pitchAccumDir;
    int yawAccumDir;
    Route *route;
    float yawAccumulator;
    float pitchAccumulator;
    float yawRate;
    int currentSecondaryWeaponIndex;
    float pitchRate;
    char _unknown_0x114[4];
    int64_t shieldRecoveryTimer;
    int64_t hullRepairTimer;
    int64_t armorRepairTimer;
    int lastHP;
    int shakeIntensity;
    int boostTimer;
    uint8_t boostingFlag;
    char _unknown_0x13d[7];
    uint8_t collide;
    uint8_t field_0x145;
    uint8_t field_0x146;
    AbyssEngine::AEMath::Vector dockOffsetVec;
    float handling;
    uint8_t autoPilot;
    int autoPilotTarget;
    uint8_t goingToWaypointFlag;
    AbyssEngine::AEMath::Vector headingVec;
    uint8_t turretMode;
    uint32_t turretCamera;
    void *dockCameraNode;
    void *dockCameraLeaf;
    uint8_t autoTurretEquipped;
    int autoTurretTimer;
    int autoTurretFireTimer;
    void *autoTurretTarget;
    void *autoTurretPrevTarget;
    int rocketControlGun;
    int rocketBanking;
    void *dockCameraMid;
    uint8_t turretActive;
    uint8_t field_0x1a1;
    float lookPitch;
    float lookYaw;
    uint8_t cloaked;
    uint8_t chargingCloak;
    void *cloak;
    void *tractorBeam;
    Array<RepairBeam *> *repairBeams;
    int asteroidTarget;
    int dockingState;
    int dockingPointIndex;

    union {
        int dockApproachDist;
        uint8_t resumeFlag;
    };

    int dockApproachThreshold;
    int field_0x1d0;
    char _unknown_0x1d4[4];
    int dockScaling;
    int miningSettleTimer;
    void *dockStation;
    int miningGame;
    int hackingGame;
    short docked;
    uint8_t dockedToStream;
    uint8_t dockingToPlanet;
    int planetDockTimer;
    uint8_t computerControlled;
    int turretPitch;
    int driveCharge;
    int driveChargeMax;
    uint8_t chargingDrive;
    int cloakCharge;
    int cloakRechargeTimer;
    int cloakChargeMax;
    int cloakDischargeMax;
    float shieldRecoveryAmount;
    char _unknown_0x21c[4];
    int hud;
    AbyssEngine::AEMath::Vector turretOffsetVec;
    char _unknown_0x230[4];
    uint8_t collidesWithStationFlag;
    uint8_t hardCoreMode;
    unsigned int dockArrowImage;
    unsigned int radarBlipImage1;
    unsigned int radarBlipImage2;
    unsigned int radarBlipImage3;
    unsigned int radarBlipImage4;
    int crosshairBlinkTimer;
    int lastFreeSpace;
    float agility;
    float pitchRamp;
    float yawRamp;
    char _unknown_0x260[8];
    double rollAccum;
    double yawAccumD;
    double pitchAccumD;
    float yawAccumF;
    int field_0x2a4;
    char _unknown_0x288[32];
    uint8_t field_0x2a8;
    uint8_t rollDirection;
    AbyssEngine::AEMath::Matrix rollMatrix;
    float rotateX;
    float rotateY;
    float rotateZ;
    uint8_t rolling;
    uint8_t field_0x2f5;
    int explosionTimer;
    int currentSystem;
    int smokeSystem;
    int explosionSmoke;
    uint8_t field_0x309;
    uint8_t requestedVisible;
    uint8_t shieldHitFlash;
    int emergencySystemTimer;
    int emergencyVal1;
    AbyssEngine::AEMath::Vector emergencyVec;
    float emergencyVal2;
    uint8_t autoLevel;
    int hitShakeTimer;
    uint8_t hitShake;
    uint8_t field_0x32d;
    uint8_t visible;
    uint8_t exhaustVisible;
    uint8_t aboutToReachAutoTargetFlag;
    int maneuverType;
    AbyssEngine::AEMath::Vector strafeTargetVec;
    AbyssEngine::AEMath::Vector facingVec;
    void *navPoint;
    uint8_t autoTurretEnabled;
    uint8_t dockedFlag;
    void *easeMatrix;
    int dockTransferTimer;
    int dockTotalAmount;
    int dockTransferedAmount;
    int cloakRechargeMax;
    SpacePoint *spacePoint;
    uint8_t throttleStarted;
    int throttle;
    uint8_t cursorStateByte;
    char _unknown_0x379[3];
    float strafeAccel;
    float strafeFactor;
    int field_0x384;
    int cloakMaterial1;
    int cloakMaterial2;
    int cloakMaterial3;
    void *field_0x394;
    uint8_t volatileGoods;
    uint8_t shieldRegenActive;
    uint8_t shieldRegenEquipped;
    uint8_t lostMiningGameFlag;

    PlayerEgo(Player *player);

    ~PlayerEgo() noexcept(false);

    Vec3 GetDirVector();

    Vec3 GetUpVector();

    void PauseEngineSound();

    void PlayEngineSound();

    void ResumeEngineSound();

    void StopEngineSound();

    unsigned char aboutToReachAutoTarget();

    void addGun(Gun *gun, int x);

    void addGun(Array<Gun *> *arr, int x);

    void addNukeVolatileForce(float v);

    void alignToHorizon();

    void approachAsteroid(Hud *hud, int hud2, Radar *radar);

    int approachDockingPoint(Hud *hud, int /*hud2*/, Radar *radar);

    unsigned char autoTurretIsEnabled();

    void boost();

    unsigned char boosting();

    void calcCollision(Array<KIPlayer *> *candidates);

    void changeThrust(float v);

    void checkForTurret();

    unsigned char collidesWithStation();

    void deleteHackingGame();

    void dockToAsteroid(KIPlayer *kip, Radar *radar);

    void dockToDockingPoint(KIPlayer *kip, Radar *radar);

    void dockToPlanet();

    void dockToStream(bool param);

    float down(int frameTime, float delta);

    void draw(bool allowHud);

    void drawThrottle();

    bool driveReady();

    bool emergencySystemActive();

    void endExplosion();

    void explode();

    bool explosionEnded();

    void forceBoost();

    int getAutoPilotTarget();

    float getBoostPercentage();

    float getBoostRate();

    int getBoostSpeed();

    float getCloakRate();

    float getCloakRechargeRate();

    float getCloakingPercentage();

    int getCurrentMiningAmount();

    int getCurrentSecondaryWeaponIndex();

    int getDockTotalAmount();

    int getDockTransferedAmount();

    float getDriveChargeRate();

    int getHUD();

    int getHackingGameDockIndex();

    float getHandling();

    int getHitpoints();

    int getHullDamageRate();

    Vec3 getPosition();

    int getRocketBanking();

    Route *getRoute();

    int getShieldDamageRate();

    float getSpeed();

    int getTargetFollowCamera();

    Vec3 getTurretPosition();

    AbyssEngine::AEMath::Matrix &rocketReturnMatrix() {
        return *reinterpret_cast<AbyssEngine::AEMath::Matrix *>(rocketReturnMatrixStorage);
    }

    const AbyssEngine::AEMath::Matrix &rocketReturnMatrix() const {
        return *reinterpret_cast<const AbyssEngine::AEMath::Matrix *>(rocketReturnMatrixStorage);
    }

    float getThrust();

    float getVolatileForce();

    bool goingToPlanet();

    int goingToStation();

    int goingToStream();

    unsigned char goingToWaypoint();

    bool goingToWormhole();

    void hackingRotateLCW();

    void hackingRotateRCW();

    void hackingShuffle();

    int hackingWon();

    void handleAutoTurret(int dt);

    void handleShip(int dt);

    void handleTurretView(int dt);

    unsigned char hasAutoTurret();

    bool hasCloak();

    unsigned char hasVolatileGoods();

    void hideShipForFirstPersonCameraView(bool param);

    void hitCamera();

    void initManeuver(int type);

    unsigned char isAutoPilot();

    int isBoostRefreshed();

    unsigned char isChargingCloak();

    unsigned char isChargingDrive();

    unsigned char isCloaked();

    bool isDead();

    bool isDockedToAsteroid();

    bool isDockedToDockingPoint();

    bool isDockedToMiningPlant();

    bool isDockedToPlanet();

    unsigned char isDockedToStream();

    bool isDockingToAsteroid();

    bool isDockingToDockingPoint();

    unsigned char isDockingToPlanet();

    unsigned char isDockingToStream();

    bool isHacking();

    bool isInDockingProcedure();

    unsigned char isInFreeLookMode();

    bool isInRocketControl();

    unsigned char isInTurretMode();

    bool isInWormhole();

    bool isLandingOrTakingOff();

    bool isMining();

    bool isRechargingCloak();

    void killLiberator();

    int levelCollision();

    float left(int frameTime, float delta);

    unsigned char lostMiningGame();

    void moveToPosition(AbyssEngine::AEMath::Vector target, bool steer, float speed);

    void pitchAllPrimaryGuns(float);

    bool readyForCloak();

    bool readyToBoost();

    void refillGunDelay();

    void removeRoute();

    void render(bool allowHud);

    void resetChargingDrive();

    void resetGunDelay();

    void resetLastHP();

    void resetMovement();

    void revive();

    float right(int frameTime, float delta);

    void roll(int amount);

    void rotate(float rx, float ry, float rz);

    void setActive(bool);

    void setAutoPilot(KIPlayer *kip);

    void setAutoTurret(bool on);

    void setCollide(bool v);

    void setComputerControlled(bool v);

    void setCurrentSecondaryWeaponIndex(int idx);

    void setDockingCamera();

    void setDockingState(int s);

    void setExhaustVisible(bool param);

    void setFreeLookMode(bool v);

    void setFreeze(bool v);

    void setLevel(Level *level);

    void setPosition(AbyssEngine::AEMath::Vector v);

    void setPosition(float x, float y, float z);

    void setRocketControl(Gun *gun, AEGeometry *geo);

    void setRotation(float rx, float ry, float rz);

    void setRoute(Route *v);

    void setShip(int race, int group);

    void setSpeed(float v);

    void setTargetFollowCamera(TargetFollowCamera *cam);

    void setThrust(float v);

    int setTurretMode(bool enable);

    void setTurretPosition(AbyssEngine::AEMath::Vector v);

    void setVisible(bool v);

    void shake(int amount);

    void shoot(int weapon, int type);

    int shouldSwitchToFreeLookCam();

    int shouldSwitchToStandardCam();

    void startJumpDrive();

    void startSmokeEmission();

    void stopBoost();

    void stopMining();

    void stopPlanetDock();

    void stopShooting(int param);

    void strafe(int /*dir*/, bool positive);

    void throttleChanged();

    int toggleCloaking();

    int tryToStartEmergencySystem();

    void turnHorizontal(int a, float v);

    void turnVertical(int a, float v);

    float up(int frameTime, float delta);

    void update(int dt, Radar *radar, Hud *hud, Radio *radio, LevelScript *script, int arg5, bool arg6, int arg7);

    int updateManeuver();

    // Static data members present in the original binary (defined for symbol parity).
    static AbyssEngine::AEMath::Vector crosshairPos;
    static AbyssEngine::AEMath::Vector crosshairShootPos;
    static AbyssEngine::AEMath::Vector vec_up;
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PlayerEgo, transform) == 0x4, "PlayerEgo::transform offset");
static_assert(__builtin_offsetof(PlayerEgo, rocketReturnMatrixStorage) == 0x10, "PlayerEgo::rocketReturnMatrix offset");
static_assert(__builtin_offsetof(PlayerEgo, field_0x14) == 0x14, "PlayerEgo::field_0x14 offset");
static_assert(__builtin_offsetof(PlayerEgo, gunMuzzleRoot) == 0x38, "PlayerEgo::gunMuzzleRoot offset");
static_assert(__builtin_offsetof(PlayerEgo, hudHitDirectionFlags) == 0x20,
              "PlayerEgo::hudHitDirectionFlags offset");
static_assert(__builtin_offsetof(PlayerEgo, matrix_0x40_storage) == 0x40, "PlayerEgo::matrix_0x40 offset");
static_assert(__builtin_offsetof(PlayerEgo, maneuverParam) == 0x7c, "PlayerEgo::maneuverParam offset");
static_assert(__builtin_offsetof(PlayerEgo, field_0x80) == 0x80, "PlayerEgo::field_0x80 offset");
static_assert(__builtin_offsetof(PlayerEgo, targetFollowCamera) == 0x88, "PlayerEgo::targetFollowCamera offset");
static_assert(__builtin_offsetof(PlayerEgo, crosshairPosPrevious) == 0x94,
              "PlayerEgo::crosshairPosPrevious offset");
static_assert(__builtin_offsetof(PlayerEgo, crosshairShootPosPrevious) == 0xa0,
              "PlayerEgo::crosshairShootPosPrevious offset");
static_assert(__builtin_offsetof(PlayerEgo, switchToStandardCam) == 0xb0, "PlayerEgo::standardCameraFlag offset");
static_assert(__builtin_offsetof(PlayerEgo, switchToFreeLookCam) == 0xb1, "PlayerEgo::freeLookCameraFlag offset");
static_assert(__builtin_offsetof(PlayerEgo, speedValue) == 0xb8, "PlayerEgo::speed offset");
static_assert(__builtin_offsetof(PlayerEgo, thrust) == 0xbc, "PlayerEgo::thrust offset");
static_assert(__builtin_offsetof(PlayerEgo, freeLookMode) == 0xc4, "PlayerEgo::freeLookMode offset");
static_assert(__builtin_offsetof(PlayerEgo, boostSpeed) == 0xc8, "PlayerEgo::boostSpeed offset");
static_assert(__builtin_offsetof(PlayerEgo, field_0xcc) == 0xcc, "PlayerEgo::boostTime offset");
static_assert(__builtin_offsetof(PlayerEgo, field_0xd0) == 0xd0, "PlayerEgo::boostDelay offset");
static_assert(__builtin_offsetof(PlayerEgo, cursorStateDirty) == 0x84, "PlayerEgo::cursorStateDirty offset");
static_assert(__builtin_offsetof(PlayerEgo, route) == 0xfc, "PlayerEgo::route offset");
static_assert(__builtin_offsetof(PlayerEgo, currentSecondaryWeaponIndex) == 0x10c,
              "PlayerEgo::currentSecondaryWeaponIndex offset");
static_assert(__builtin_offsetof(PlayerEgo, shieldRecoveryTimer) == 0x118, "PlayerEgo::shieldRecoveryTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, hullRepairTimer) == 0x120, "PlayerEgo::hullRepairTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, armorRepairTimer) == 0x128, "PlayerEgo::armorRepairTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, lastHP) == 0x130, "PlayerEgo::lastHP offset");
static_assert(__builtin_offsetof(PlayerEgo, shakeIntensity) == 0x134, "PlayerEgo::frameTime offset");
static_assert(__builtin_offsetof(PlayerEgo, boostTimer) == 0x138, "PlayerEgo::boostTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, boostingFlag) == 0x13c, "PlayerEgo::boostingFlag offset");
static_assert(__builtin_offsetof(PlayerEgo, collide) == 0x144, "PlayerEgo::collide offset");
static_assert(__builtin_offsetof(PlayerEgo, field_0x145) == 0x145, "PlayerEgo::field_0x145 offset");
static_assert(__builtin_offsetof(PlayerEgo, dockOffsetVec) == 0x148, "PlayerEgo::positionCache offset");
static_assert(__builtin_offsetof(PlayerEgo, handling) == 0x154, "PlayerEgo::handling offset");
static_assert(__builtin_offsetof(PlayerEgo, autoPilotTarget) == 0x15c, "PlayerEgo::autoPilotTarget offset");
static_assert(__builtin_offsetof(PlayerEgo, asteroidTarget) == 0x1bc, "PlayerEgo::asteroidTarget offset");
static_assert(__builtin_offsetof(PlayerEgo, dockingState) == 0x1c0, "PlayerEgo::dockingState offset");
static_assert(__builtin_offsetof(PlayerEgo, dockingPointIndex) == 0x1c4, "PlayerEgo::dockingPointIndex offset");
static_assert(__builtin_offsetof(PlayerEgo, dockScaling) == 0x1d8, "PlayerEgo::dockScaling offset");
static_assert(__builtin_offsetof(PlayerEgo, miningSettleTimer) == 0x1dc, "PlayerEgo::miningSettleTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, dockStation) == 0x1e0, "PlayerEgo::dockStation offset");
static_assert(__builtin_offsetof(PlayerEgo, miningGame) == 0x1e4, "PlayerEgo::miningGame offset");
static_assert(__builtin_offsetof(PlayerEgo, hackingGame) == 0x1e8, "PlayerEgo::hackingGame offset");
static_assert(__builtin_offsetof(PlayerEgo, turretActive) == 0x1a0, "PlayerEgo::turretActive offset");
static_assert(__builtin_offsetof(PlayerEgo, cloak) == 0x1b0, "PlayerEgo::cloak offset");
static_assert(__builtin_offsetof(PlayerEgo, shieldRecoveryAmount) == 0x218, "PlayerEgo::shieldRecoveryAmount offset");
static_assert(__builtin_offsetof(PlayerEgo, hud) == 0x220, "PlayerEgo::hud offset");
static_assert(__builtin_offsetof(PlayerEgo, turretOffsetVec) == 0x224, "PlayerEgo::turretOffsetVec offset");
static_assert(__builtin_offsetof(PlayerEgo, collidesWithStationFlag) == 0x234, "PlayerEgo::stationCollision offset");
static_assert(__builtin_offsetof(PlayerEgo, hardCoreMode) == 0x235, "PlayerEgo::hardCoreMode offset");
static_assert(__builtin_offsetof(PlayerEgo, dockArrowImage) == 0x238, "PlayerEgo::throttleImage offset");
static_assert(__builtin_offsetof(PlayerEgo, radarBlipImage4) == 0x248, "PlayerEgo::plasmaLockImage offset");
static_assert(__builtin_offsetof(PlayerEgo, crosshairBlinkTimer) == 0x24c, "PlayerEgo::blinkTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, lastFreeSpace) == 0x250, "PlayerEgo::lastFreeSpace offset");
static_assert(__builtin_offsetof(PlayerEgo, agility) == 0x254, "PlayerEgo::agility offset");
static_assert(__builtin_offsetof(PlayerEgo, pitchRamp) == 0x258, "PlayerEgo::pitchRamp offset");
static_assert(__builtin_offsetof(PlayerEgo, yawRamp) == 0x25c, "PlayerEgo::yawRamp offset");
static_assert(__builtin_offsetof(PlayerEgo, rollAccum) == 0x268, "PlayerEgo::rollAccum offset");
static_assert(__builtin_offsetof(PlayerEgo, rollDirection) == 0x2a9, "PlayerEgo::rollDirection offset");
static_assert(__builtin_offsetof(PlayerEgo, rollMatrix) == 0x2ac, "PlayerEgo::rollMatrix offset");
static_assert(__builtin_offsetof(PlayerEgo, rotateX) == 0x2e8, "PlayerEgo::rotateX offset");
static_assert(__builtin_offsetof(PlayerEgo, explosionTimer) == 0x2f8, "PlayerEgo::explosionTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, field_0x309) == 0x308, "PlayerEgo::renderVisibility offset");
static_assert(__builtin_offsetof(PlayerEgo, requestedVisible) == 0x309, "PlayerEgo::requestedVisibility offset");
static_assert(__builtin_offsetof(PlayerEgo, shieldHitFlash) == 0x30a, "PlayerEgo::shieldHitFlash offset");
static_assert(__builtin_offsetof(PlayerEgo, autoLevel) == 0x324, "PlayerEgo::autoLevel offset");
static_assert(__builtin_offsetof(PlayerEgo, visible) == 0x32e, "PlayerEgo::visible offset");
static_assert(__builtin_offsetof(PlayerEgo, exhaustVisible) == 0x32f, "PlayerEgo::exhaustVisible offset");
static_assert(__builtin_offsetof(PlayerEgo, spacePoint) == 0x36c, "PlayerEgo::spacePoint offset");
static_assert(__builtin_offsetof(PlayerEgo, cursorStateByte) == 0x378, "PlayerEgo::cursorStateByte offset");
static_assert(__builtin_offsetof(PlayerEgo, strafeAccel) == 0x37c, "PlayerEgo::strafeAccel offset");
static_assert(__builtin_offsetof(PlayerEgo, strafeFactor) == 0x380, "PlayerEgo::strafeFactor offset");
static_assert(__builtin_offsetof(PlayerEgo, cloakMaterial1) == 0x388, "PlayerEgo::cloakMaterial1 offset");
static_assert(__builtin_offsetof(PlayerEgo, cloakMaterial2) == 0x38c, "PlayerEgo::cloakMaterial2 offset");
static_assert(__builtin_offsetof(PlayerEgo, cloakMaterial3) == 0x390, "PlayerEgo::cloakMaterial3 offset");
static_assert(__builtin_offsetof(PlayerEgo, dockTransferTimer) == 0x35c, "PlayerEgo::dockTransferTimer offset");
static_assert(__builtin_offsetof(PlayerEgo, dockTotalAmount) == 0x360, "PlayerEgo::dockTotalAmount offset");
static_assert(__builtin_offsetof(PlayerEgo, field_0x394) == 0x394, "PlayerEgo::materialList offset");
static_assert(__builtin_offsetof(PlayerEgo, volatileGoods) == 0x398, "PlayerEgo::volatileGoods offset");
static_assert(__builtin_offsetof(PlayerEgo, shieldRegenActive) == 0x399, "PlayerEgo::shieldRegenActive offset");
static_assert(__builtin_offsetof(PlayerEgo, shieldRegenEquipped) == 0x39a, "PlayerEgo::shieldRegenEquipped offset");
static_assert(__builtin_offsetof(PlayerEgo, lostMiningGameFlag) == 0x39b, "PlayerEgo::lostMiningGameFlag offset");
static_assert(sizeof(PlayerEgo) == 0x3a0, "PlayerEgo ARM32 allocation size");
#endif

#endif
