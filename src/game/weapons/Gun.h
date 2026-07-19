#ifndef GOF2_GUN_H
#define GOF2_GUN_H
#include <cstddef>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/render/Sparks.h"

#include "game/ItemSort.h"

#include "engine/math/Vector.h"

#include "engine/math/Matrix.h"


class Player;

class Level;
class Sparks;


class Gun {
public:
    int lastHitKIPlayer;
    Player *owner;
    unsigned count;
    char *positions;
    int positionsCapacity;
    int directionCount;
    char *velocities;
    int velocitiesCapacity;
    int field_0x20;
    char *upVectors;
    int upVectorsCapacity;
    int field_0x2c;
    char *hitPositions;
    int hitPositionsCapacity;
    int level;
    int *lifetimes;
    uint8_t *hitFlags;
    int initialLifetime;
    int fireDelay;
    uint8_t active;
    uint8_t hitSmall;

    union {
        float pitchRate;

        float field_0x50;
    };

    uint8_t field_0x54;
    int itemIndex;
    ItemSort weaponType;
    int damage;
    int empDamage;
    uint8_t useCustomRadius;
    int timer;
    int delayTimer;
    int ammoCount;
    int field_0x78;
    Vector offset;
    uint8_t ignited;
    char field_0x89;
    int32_t field_0x8c;
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int fireIndex;
    uint8_t _pad_0xa0[4];

    union {
        int field_0xa4;
        struct {
            uint8_t field_0xa4_b0;
            uint8_t field_0xa5;
            uint8_t field_0xa6;
            uint8_t field_0xa7;
        };
    };

    uint8_t field_0xa8;
    uint8_t delayActive;
    Array<int> *wobbleOffsets;
    float field_0xb0;
    Array<Player *> *enemies;
    Sparks *impact;
    Player *target;
    Vector basePos;
    int field_0xcc;
    int field_0xd0;
    int field_0xd4;
    Vector targetDir;
    Vector velocity;
    uint8_t playerGun;
    int slotIndex;
    uint8_t levelCollision;
    uint8_t friendGun;
    float errorMagnitudePercentage;
    int magnitude;
    int customRadius;
    uint8_t homing;
    int *geometries;
    uint8_t *randomFlags;

    Gun(int kind, int p2, int count, int p4, int p5, int p6, float p7, Vector dir, Vector vel);

    ~Gun() noexcept(false);

    void calcCharacterCollision();

    void calcLevelCollision();

    void *getEnemies();

    int getMagnitude();

    void ignite();

    uint8_t isPlayerGun();

    void removeAllEnemies();

    void render();

    void setEnemies(Array<Player *> *enemies);

    void setEnemy(Player *enemy);

    void setErrorMagnitudePercentage(int v);

    void setFriendGun(bool v);

    void setLevel(Level *lvl);

    void setImpact(Sparks *impact);

    void setIndex(int index);

    void setLevelCollision(bool v);

    void setMagnitude(int v);

    void setOffset(Vector *v);

    void setOffset(int a, int b);

    void setPlayerGun(bool v);

    void shoot(Matrix m, int n, bool b);

    void shootAt(Matrix m, int n, Player *p, bool b);

    void translate(const Vector &v);

    void update(int dt);
};

#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(Gun, field_0x20) == 0x20, "Gun::field_0x20 offset");
static_assert(offsetof(Gun, field_0x2c) == 0x2c, "Gun::field_0x2c offset");
static_assert(offsetof(Gun, level) == 0x38, "Gun::level offset");
static_assert(offsetof(Gun, lifetimes) == 0x3c, "Gun::lifetimes offset");
static_assert(offsetof(Gun, hitFlags) == 0x40, "Gun::hitFlags offset");
static_assert(offsetof(Gun, fireDelay) == 0x48, "Gun::fireDelay offset");
static_assert(offsetof(Gun, active) == 0x4c, "Gun::active offset");
static_assert(offsetof(Gun, field_0x54) == 0x54, "Gun::field_0x54 offset");
static_assert(offsetof(Gun, itemIndex) == 0x58, "Gun::itemIndex offset");
static_assert(offsetof(Gun, weaponType) == 0x5c, "Gun::weaponType offset");
static_assert(offsetof(Gun, field_0x78) == 0x78, "Gun::field_0x78 offset");
static_assert(offsetof(Gun, offset) == 0x7c, "Gun::offset offset");
static_assert(offsetof(Gun, field_0xa4) == 0xa4, "Gun::field_0xa4 offset");
static_assert(offsetof(Gun, field_0xa5) == 0xa5, "Gun::field_0xa5 offset");
static_assert(offsetof(Gun, field_0xa8) == 0xa8, "Gun::field_0xa8 offset");
static_assert(offsetof(Gun, delayActive) == 0xa9, "Gun::delayActive offset");
static_assert(offsetof(Gun, field_0xb0) == 0xb0, "Gun::field_0xb0 offset");
static_assert(offsetof(Gun, targetDir) == 0xd8, "Gun::targetDir offset");
#endif
#endif
