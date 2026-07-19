#ifndef GOF2_RADAR_H
#define GOF2_RADAR_H
#include <cstddef>
#include <cstdint>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/math/Matrix.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/AEMath.h"
#include "engine/math/Vector.h"


class Player;


class Hud;
class KIPlayer;
class Level;
class Route;
class Sprite;

#pragma pack(push, 4)
class Radar {
public:
    Level *level;

    union {
        KIPlayer *lockedEnemy;
        void *dockTargetPtr;
    };

    union {
        KIPlayer *dockNavPtr;
        KIPlayer *candidateEnemy;
        void *field_0x8;
    };

    KIPlayer *lockedAsteroid;
    union {
        int field_0x10;
        KIPlayer *candidateAsteroid;
    };
    union {
        int field_0x14;
        KIPlayer *lockedPlanetTarget;
    };
    union {
        int field_0x18;
        KIPlayer *candidatePlanetTarget;
    };
    KIPlayer *field_0x1c;
    int field_0x20;
    KIPlayer *lockedStation;
    union {
        int field_0x28;
        KIPlayer *candidateStation;
    };
    int field_0x2c;
    int field_0x30;
    Array<KIPlayer *> *players;
    KIPlayer *lockedGasCloud;
    union {
        int field_0x3c;
        KIPlayer *candidateGasCloud;
    };
    int planetDockIndex;
    int field_0x44;
    uint8_t enabled;
    uint8_t pad_0x49[3];
    int imageWidth;
    int imageHeight;
    uint8_t field_0x54;
    uint8_t pad_0x55[3];
    union {
        int field_0x58;
        int *radarSlots;
    };

    int imageIds_0x5c[37];
    Sprite *raceSprite;
    Sprite *blipSprite;
    Sprite *qualitySprite;
    int screenX;
    int screenY;
    volatile int centerX;
    volatile int centerY;
    volatile float weightX;
    volatile float weightY;
    int imageWidthSq;
    int imageHeightSq;
    uint8_t onScreen;
    union {
        uint8_t field_0x11d;
        uint8_t offscreenFlagRight;
    };
    union {
        uint8_t field_0x11e;
        uint8_t offscreenFlagLeftOrVertical;
    };
    union {
        uint8_t field_0x11f;
        uint8_t offscreenFlagBottom;
    };
    union {
        uint8_t field_0x120;
        uint8_t offscreenFlagTop;
    };
    uint8_t pad_0x121[3];
    int field_0x124;
    int field_0x128;
    int turretScopeHalfWidth;
    uint8_t plasmaInRange;
    uint8_t pad_0x131[3];
    union {
        int field_0x134;
        Array<KIPlayer *> *enemyTargets;
    };
    union {
        int field_0x138;
        Array<KIPlayer *> *landmarkTargets;
    };
    union {
        int field_0x13c;
        Array<KIPlayer *> *asteroidTargets;
    };
    union {
        int field_0x140;
        Array<KIPlayer *> *planetTargets;
    };
    union {
        int field_0x144;
        Array<KIPlayer *> *gasCloudTargets;
    };
    union {
        int field_0x148;
        int gasLockTimer;
    };
    int field_0x14c;
    union {
        int field_0x150;
        Route *playerRoute;
    };
    float radarPosX;
    float radarPosY;
    float radarPosZ;
    int field_0x160;
    int field_0x164;
    int field_0x168;
    int field_0x16c;
    int field_0x170;
    int field_0x174;
    int field_0x178;
    int field_0x17c;
    int field_0x180;
    int field_0x184;
    Array<AbyssEngine::String *> *labelStrings;
    AbyssEngine::String lockLabel;
    int field_0x198;
    union {
        int field_0x19c;
        int asteroidLockTimer;
    };
    int field_0x1a0;
    int field_0x1a4;
    uint8_t field_0x1a8;
    uint8_t field_0x1a9;
    uint8_t field_0x1aa;
    uint8_t scannerAvailable;
    uint8_t field_0x1ac;
    uint8_t field_0x1ad;
    uint8_t field_0x1ae;
    uint8_t field_0x1af;
    int field_0x1b0;
    int field_0x1b4;
    int field_0x1b8;
    int field_0x1bc;
    int field_0x1c0;
    int radarImage;
    int lockPanelWidth;
    int lockPanelHeight;
    AbyssEngine::AEMath::Matrix transform;
    int field_0x20c;
    int lockPanelX;
    int lockPanelY;
    union {
        int field_0x218;
        uint8_t field_0x218_byte;
    };
    int screenWidth;
    int halfScreenWidth;
    int screenHeight;
    int halfScreenHeight;
    int originX;
    int originY;
    int field_0x234;
    int field_0x238;
    float cameraPosX;
    float cameraPosY;
    float cameraPosZ;

    explicit Radar(Level *level);

    ~Radar();

    int getTurretScopeWidth();

    int hasScanner();

    int isPlasmaInRange();

    bool stationLocked();

    KIPlayer *getLockedEnemy();

    KIPlayer *getLockedAsteroid();

    KIPlayer *getLockedGasCloud();

    int unlockAsteroid();

    int getPlanetDockIndex();

    void update(KIPlayer *player);

    void update(AbyssEngine::AEMath::Vector value);

    int draw(Player *player, Hud *hud, int mode);

    AbyssEngine::AEMath::Vector elipsoidIntersect(int y, int x, AbyssEngine::AEMath::Vector value);

    int drawCurrentLock(Hud *hud);

    AbyssEngine::String calcDistance(float x, float y, float z, float originX, float originY, float originZ);

    // Static data members present in the original binary (defined for symbol parity).
    static unsigned char drawTarget;
};
#pragma pack(pop)

#if UINTPTR_MAX == 0xffffffffu
static_assert(offsetof(Radar, lockedEnemy) == 0x04, "Radar::lockedEnemy offset");
static_assert(offsetof(Radar, lockedStation) == 0x24, "Radar::lockedStation offset");
static_assert(offsetof(Radar, players) == 0x34, "Radar::players offset");
static_assert(offsetof(Radar, lockedGasCloud) == 0x38, "Radar::lockedGasCloud offset");
static_assert(offsetof(Radar, enabled) == 0x48, "Radar::enabled offset");
static_assert(offsetof(Radar, imageWidth) == 0x4c, "Radar::imageWidth offset");
static_assert(offsetof(Radar, imageIds_0x5c) == 0x5c, "Radar::imageIds_0x5c offset");
static_assert(offsetof(Radar, raceSprite) == 0xf0, "Radar::raceSprite offset");
static_assert(offsetof(Radar, screenX) == 0xfc, "Radar::screenX offset");
static_assert(offsetof(Radar, centerX) == 0x104, "Radar::centerX offset");
static_assert(offsetof(Radar, imageWidthSq) == 0x114, "Radar::imageWidthSq offset");
static_assert(offsetof(Radar, onScreen) == 0x11c, "Radar::onScreen offset");
static_assert(offsetof(Radar, turretScopeHalfWidth) == 0x12c, "Radar::turretScopeHalfWidth offset");
static_assert(offsetof(Radar, plasmaInRange) == 0x130, "Radar::plasmaInRange offset");
static_assert(offsetof(Radar, radarPosX) == 0x154, "Radar::radarPosX offset");
static_assert(offsetof(Radar, labelStrings) == 0x188, "Radar::labelStrings offset");
static_assert(offsetof(Radar, lockLabel) == 0x18c, "Radar::lockLabel offset");
static_assert(offsetof(Radar, scannerAvailable) == 0x1ab, "Radar::scannerAvailable offset");
static_assert(offsetof(Radar, radarImage) == 0x1c4, "Radar::radarImage offset");
static_assert(offsetof(Radar, transform) == 0x1d0, "Radar::transform offset");
static_assert(offsetof(Radar, screenWidth) == 0x21c, "Radar::screenWidth offset");
static_assert(offsetof(Radar, originY) == 0x230, "Radar::originY offset");
static_assert(offsetof(Radar, cameraPosX) == 0x23c, "Radar::cameraPosX offset");
static_assert(sizeof(Radar) == 0x248, "Radar size");
#endif

#endif
