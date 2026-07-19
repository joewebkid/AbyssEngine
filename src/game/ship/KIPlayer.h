#ifndef GOF2_KIPLAYER_H
#define GOF2_KIPLAYER_H
#include <cstddef>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Player.h"
#include "engine/core/AbyssEngine.h"

#include "engine/math/Vector.h"
#include "engine/render/AEGeometry.h"
#include "game/weapons/Gun.h"

class Player;


class AEGeometry;
class Gun;
class Hud;
class Level;
class Route;
class SpacePoint;


class KIPlayer {
public:


    // Byte-faithful to android_2.0.16_libgof2hdaa.so KIPlayer (Ghidra, size 292,
    // alignment 1). Laid out with #pragma pack(1) below so every field lands at
    // its true offset (named field_0xNN == byte offset NN). Verified in the
    // 32-bit MATCH build with offsetof probes.
    Player *player;                  // 0x04
    AEGeometry *geometry;            // 0x08
    AEGeometry *parentGeometry;      // 0x0c
    int field_0x10;                  // 0x10
    int field_0x14;                  // 0x14
    String name;                     // 0x18 (12 bytes -> ends 0x24)
    uint8_t field_0x24;              // 0x24

    union {
        uint8_t garbledWingmanFlag;
        uint8_t field_0x25;          // 0x25
    };

    uint8_t _pad_0x26[2];            // 0x26..0x27
    int shipGroup;                   // 0x28

    union {
        int autoPilotState;
        int field_0x2c;              // 0x2c
    };

    int field_0x30;                  // 0x30
    int field_0x34;                  // 0x34
    int field_0x38;                  // 0x38

    union {
        int stealFlag;               // 0x3c

        struct {
            char stealFlagByte;          // 0x3c
            uint8_t countsAsEnemyExcludeFlag; // 0x3d
            uint8_t field_0x3e;          // 0x3e
            char field_0x3f_b;           // 0x3f
        };
    };

    union {
        int field_0x40;                  // 0x40 (4-byte view)

        struct {
            char field_0x3f;             // 0x40 (legacy name)
            char deadFlag;               // 0x41
            char field_0x42;             // 0x42 (Ghidra char @0x42)
            uint8_t reviveLockFlag;      // 0x43
        };

        struct {
            char field_0x40b;            // 0x40
            char field_0x41b;            // 0x41
            char field_0x42b;            // 0x42
            char field_0x43b;            // 0x43
        };
    };

    int field_0x44;                  // 0x44
    int field_0x48;                  // 0x48
    int hasCargo;                    // 0x4c
    Array<int> *cargo;               // 0x50
    Level *level;                    // 0x54
    float posX;                      // 0x58
    float posY;                      // 0x5c
    float posZ;                      // 0x60
    uint32_t field_0x64;             // 0x64
    uint8_t diedWithMissionCrate;    // 0x68

    union {
        uint8_t lostMissionCrateToEgo;
        uint8_t inactiveFlag;
        uint8_t proximityAlarmFlag;  // 0x69
    };

    uint8_t field_0x6a;              // 0x6a
    uint8_t _pad_0x6b;               // 0x6b

    union {
        Route *route;                // 0x6c

        struct {
            uint8_t noTargetFlag;        // 0x6c
            uint8_t routeByte1;          // 0x6d
            uint8_t routeByte2;          // 0x6e
            uint8_t routeByte3;          // 0x6f
        };
    };

    uint16_t field_0x70;             // 0x70 (2 bytes)
    uint8_t field_0x72;              // 0x72
    uint8_t field_0x73;              // 0x73
    uint8_t field_0x74;              // 0x74
    uint8_t field_0x75;              // 0x75
    uint8_t field_0x76;              // 0x76
    uint8_t _pad_0x77;               // 0x77
    AEGeometry *crateGeometry;       // 0x78
    int shipGroupFlag;               // 0x7c
    int field_0x80;                  // 0x80
    int field_0x84;                  // 0x84
    int state;                       // 0x88
    int field_0x8c;                  // 0x8c
    int field_0x90;                  // 0x90
    int field_0x94;                  // 0x94
    int field_0x98;                  // 0x98
    int field_0x9c;                  // 0x9c
    int field_0xa0;                  // 0xa0
    int field_0xa4;                  // 0xa4
    float rotationSpeed;             // 0xa8
    int type;                        // 0xac
    uint8_t _pad_0xb0;               // 0xb0
    uint8_t sleepFlag;               // 0xb1
    uint8_t initActiveFlag;          // 0xb2
    uint8_t _pad_0xb3;               // 0xb3
    Route *initialRoute;             // 0xb4
    int field_0xb8;                  // 0xb8
    int field_0xbc;                  // 0xbc
    uint8_t field_0xc0;              // 0xc0
    uint8_t _pad_0xc1[3];            // 0xc1..0xc3
    Array<uint32_t> *spacePointIds;  // 0xc4
    int field_0xc8;                  // 0xc8
    Array<SpacePoint *> *spacePoints;// 0xcc
    int carriesMissionCrate;         // 0xd0
    uint32_t jumpSphere;             // 0xd4
    int field_0xd8;                  // 0xd8
    uint8_t wingmanFlag;             // 0xdc
    uint8_t _pad_0xdd[3];            // 0xdd..0xdf
    int wingmanCommand;              // 0xe0
    int field_0xe4;                  // 0xe4
    KIPlayer *wingmanTarget;         // 0xe8
    uint8_t jumperFlag;              // 0xec
    uint8_t _pad_0xed[7];            // 0xed..0xf3
    uint8_t jumpDone;                // 0xf4
    uint8_t visibleFlag;             // 0xf5
    uint8_t _pad_0xf6[2];            // 0xf6..0xf7
    int engineSoundEvent;            // 0xf8
    uint32_t field_0xfc;             // 0xfc
    uint8_t field_0x100;             // 0x100
    uint8_t field_0x101;             // 0x101
    uint8_t _pad_0x102[2];           // 0x102..0x103
    int field_0x104;                 // 0x104
    int field_0x108;                 // 0x108
    int field_0x10c;                 // 0x10c
    int field_0x110;                 // 0x110
    int field_0x114;                 // 0x114
    int field_0x118;                 // 0x118
    int field_0x11c;                 // 0x11c
    int field_0x120;                 // 0x120

    KIPlayer(int faction, int group, Player *player, AEGeometry *geom,
             float x, float y, float z, bool active);

    virtual ~KIPlayer();

    virtual void PauseEngineSound();

    virtual void PlayEngineSound();

    virtual void ResumeEngineSound();

    virtual void StopEngineSound();

    virtual void awake();

    virtual void captureCrate(Hud *hud);

    virtual int cargoAvailable();

    virtual void createCrate(int type);

    SpacePoint *getNearestDockingPoint(const Vector &dir);

    SpacePoint *getNearestNavigationPoint(const Vector &dir, SpacePoint *target);

    virtual Vector getPosition();

    int getSpeed();

    int getType();

    bool isDead();

    bool isDocked();

    bool isDying();

    uint8_t isEnemy();

    uint8_t isJumper();

    uint8_t isVisible();

    uint8_t isWingMan();

    void jump();

    virtual int collide(float x, float y, float z);

    virtual int outerCollide(const Vector &v);

    virtual int outerCollide(float x, float y, float z);

    virtual void render();

    virtual void update(int dt);

    void reset();

    void setActive(bool active);

    void setDead();

    void setEnemies(Array<Player *> *enemies);

    void setInitActive(bool active);

    void setJumpSphere(uint32_t sphere);

    void setJumper(bool b);

    virtual void setPosition(float x, float y, float z);

    void setPosition(const Vector &v);

    void setRotationSpeed(float speed);

    void setRoute(Route *route);

    virtual void setShipGroup(AEGeometry *geom, int group, bool flag);

    void setSpacePoints(Array<SpacePoint *> *pts);

    void setState(int state);

    void setToSleep();

    void setVisible(bool visible);

    void setWingman(bool b, int cmd);

    virtual void setWingmanCommand(int cmd, KIPlayer *target);

    virtual void setSpeed(float v);

    virtual void revive();

    virtual void push(int dt);

    virtual void translate(const Vector &v);

    virtual void initPush(const Vector &target, int radius);

    Route *getRoute();

    Array<SpacePoint *> *getSpacePoints();

    virtual void setLevel(Level *lvl);

    void addGun(Gun *gun, int slot);

    void addGun(Array<Gun *> *guns, int slot);

    int levelCollision(Vector *out, long long flags);

    void enableExplosion();

    void setInitialRotation(Vector rotation);

    virtual Vector getProjectionVector(const Vector &v);

    Vector getCollisionNormal(const Vector &position);

    virtual Vector projectCollisionOnSurface(const Vector &position);
};

#if __SIZEOF_POINTER__ == 4
// Byte-faithful to android_2.0.16_libgof2hdaa.so (Ghidra: size 292, alignment 1).
static_assert(sizeof(KIPlayer) == 292, "KIPlayer must be 292 bytes (binary layout)");
static_assert(offsetof(KIPlayer, posX) == 0x58, "KIPlayer::posX must be at 0x58");
static_assert(offsetof(KIPlayer, route) == 0x6c, "KIPlayer::route must be at 0x6c");
static_assert(offsetof(KIPlayer, field_0x74) == 0x74, "KIPlayer::field_0x74 must be at 0x74");
static_assert(offsetof(KIPlayer, field_0x76) == 0x76, "KIPlayer::field_0x76 must be at 0x76");
#endif

#endif
