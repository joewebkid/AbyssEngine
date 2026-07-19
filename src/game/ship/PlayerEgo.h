#ifndef GOF2_PLAYEREGO_H
#define GOF2_PLAYEREGO_H
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
    // begins at byte 0x10 (0xC into this storage region) and runs 0x3c bytes to
    // 0x4c. During rocket-control hand-off the ship transform is copied into
    // that slot via a single bulk Matrix store, which overruns the inner union
    // into maneuverParam/field_0x80/targetFollowCamera. Modeling it as a named
    // member keeps the store as plain member access.
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
                    int field_0x1c;
                    int field_0x20;
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
            float maneuverParam;
            float field_0x80;
            int targetFollowCamera;
        };

        struct {
            char _rocketReturnMatrixPad[0xC];
            float rocketReturnMatrixStorage[15];
        };
    };
    void *explosion;
    void *explosion2;
    int field_0xac;
    int field_0xb0;
    uint8_t switchToStandardCam;
    uint8_t field_0xb2;
    int engineSoundId;
    int speed;
    float thrust;
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
    int route;
    int pitchAccumDir;
    int yawAccumDir;
    int currentSecondaryWeaponIndex;
    float yawAccumulator;
    float pitchAccumulator;
    float yawRate;
    float pitchRate;
    int lastHP;
    int shakeIntensity;
    int boostTimer;
    uint8_t boostingFlag;
    uint8_t collide;
    uint8_t field_0x145;
    uint8_t field_0x146;
    AbyssEngine::AEMath::Vector dockOffsetVec;
    int boostDelay;
    int handling;
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
    int hud;
    AbyssEngine::AEMath::Vector turretOffsetVec;
    uint8_t collidesWithStationFlag;
    uint8_t hardCoreMode;
    int hpGaugeImage;
    int dockArrowImage;
    int radarBlipImage1;
    int radarBlipImage2;
    int radarBlipImage3;
    int shakeAccum;
    float pitchRamp;
    float yawRamp;
    double rollAccum;
    double yawAccumD;
    double pitchAccumD;
    float yawAccumF;
    int field_0x2a4;
    uint8_t field_0x2a8;
    uint8_t rollDirection;
    AbyssEngine::AEMath::Matrix rollMatrix;
    void *field_0x2c0;
    uint8_t gunExtraVisible;
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
    int dockTotalAmount;
    int dockTransferedAmount;
    int cloakRechargeMax;
    int spacePoint;
    uint8_t throttleStarted;
    int throttle;
    float strafeAccel;
    void *strafeNavPoint;
    int cloakMaterial1;
    int cloakMaterial2;
    int cloakMaterial3;
    void *field_0x394;
    uint8_t volatileGoods;
    uint8_t lostMiningGameFlag;
    AbyssEngine::AEMath::Matrix turretHudMatrix;

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

    int getHandling();

    int getHitpoints();

    int getHullDamageRate();

    Vec3 getPosition();

    int getRocketBanking();

    int getRoute();

    int getShieldDamageRate();

    int getSpeed();

    int getTargetFollowCamera();

    Vec3 getTurretPosition();

    AbyssEngine::AEMath::Matrix &rocketReturnMatrix() {
        return *reinterpret_cast<AbyssEngine::AEMath::Matrix *>(rocketReturnMatrixStorage);
    }

    const AbyssEngine::AEMath::Matrix &rocketReturnMatrix() const {
        return *reinterpret_cast<const AbyssEngine::AEMath::Matrix *>(rocketReturnMatrixStorage);
    }

    int getThrust();

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

    int isDead();

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

    void setTurretMode(bool enable);

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

    void toggleCloaking();

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
static_assert(__builtin_offsetof(PlayerEgo, maneuverParam) == 0x40, "PlayerEgo::maneuverParam offset");
static_assert(__builtin_offsetof(PlayerEgo, targetFollowCamera) == 0x48, "PlayerEgo::targetFollowCamera offset");
#endif

#endif
