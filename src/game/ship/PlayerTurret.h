#ifndef GOF2_PLAYERTURRET_H
#define GOF2_PLAYERTURRET_H

#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"

class Player;

class AEGeometry;
class Explosion;
class KIPlayer;
class Level;


class PlayerTurret : public KIPlayer {
public:


    int lastFrameDelta; // Android update stores delta at +0x124.
    uint8_t &turretEnabled() { return this->field_0x25; }
    char &isSentryGun() { return this->field_0x3f_b; }
    Vector &cachedPosition() { return this->position; }
    Vector &hostWorldOffset() { return this->targetPos; }
    Vector &aimPoint() { return reinterpret_cast<Vector &>(this->field_0x9c); }
    int &reviveFlag() { return this->field_0xd8; }
    int &frameDelta() { return this->lastFrameDelta; }

    int spawnInvulnTimer;
    int explosionTimer;
    int pickEnemyTimer;
    int rotationAccum;
    int particleSystemId;
    Explosion *explosion;
    AEGeometry *baseGeometry;
    AEGeometry *turretGeometry;
    AEGeometry *helperGeometry;
    Player *currentEnemy;
    Player *previousEnemy;
    KIPlayer *turretHost;
    Vector hostOffset;
    int turretRange;

    PlayerTurret(int mesh, Player *player, AEGeometry *geometry, float x, float y, float z);

    ~PlayerTurret();

    void setTurretRange(int range);

    void handleSentryGun(int delta);

    void setHost(KIPlayer *host, const Vector &offset);

    void render() override;

    int collide(float x, float y, float z) override;

    int outerCollide(float x, float y, float z) override;

    void handleTurret(int delta);

    void revive() override;

    void setPosition(const Vector &position) override;

    void reset();

    void setLevel(Level *level) override;

    KIPlayer *getHost();

    void setScaling(float scale);

    void handleRotation(int delta, AEGeometry *mainGeometry, AEGeometry *turretGeometry);

    void update(int delta) override;

    void pickEnemy();
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PlayerTurret, field_0x25) == 0x25, "PlayerTurret enabled byte");
static_assert(__builtin_offsetof(PlayerTurret, field_0x3f_b) == 0x3f, "PlayerTurret sentry byte");
static_assert(__builtin_offsetof(PlayerTurret, lastFrameDelta) == 0x124, "PlayerTurret frame delta");
static_assert(__builtin_offsetof(PlayerTurret, position) == 0x2c, "PlayerTurret cached position");
static_assert(__builtin_offsetof(PlayerTurret, targetPos) == 0x90, "PlayerTurret host world offset");
static_assert(__builtin_offsetof(PlayerTurret, field_0x9c) == 0x9c, "PlayerTurret aim point");
static_assert(__builtin_offsetof(PlayerTurret, field_0xd8) == 0xd8, "PlayerTurret revive flag");
static_assert(__builtin_offsetof(PlayerTurret, spawnInvulnTimer) == 0x128,
              "PlayerTurret::spawnInvulnTimer offset");
static_assert(__builtin_offsetof(PlayerTurret, explosion) == 0x13c,
              "PlayerTurret::explosion offset");
static_assert(__builtin_offsetof(PlayerTurret, explosionTimer) == 0x12c, "PlayerTurret explosion timer");
static_assert(__builtin_offsetof(PlayerTurret, pickEnemyTimer) == 0x130, "PlayerTurret pick timer");
static_assert(__builtin_offsetof(PlayerTurret, rotationAccum) == 0x134, "PlayerTurret pitch accumulation");
static_assert(__builtin_offsetof(PlayerTurret, particleSystemId) == 0x138, "PlayerTurret particle system");
static_assert(__builtin_offsetof(PlayerTurret, baseGeometry) == 0x140, "PlayerTurret base geometry");
static_assert(__builtin_offsetof(PlayerTurret, turretGeometry) == 0x144, "PlayerTurret turret geometry");
static_assert(__builtin_offsetof(PlayerTurret, helperGeometry) == 0x148, "PlayerTurret helper geometry");
static_assert(__builtin_offsetof(PlayerTurret, currentEnemy) == 0x14c, "PlayerTurret current enemy");
static_assert(__builtin_offsetof(PlayerTurret, previousEnemy) == 0x150, "PlayerTurret previous enemy");
static_assert(__builtin_offsetof(PlayerTurret, turretHost) == 0x154,
              "PlayerTurret::turretHost offset");
static_assert(__builtin_offsetof(PlayerTurret, turretRange) == 0x164,
              "PlayerTurret::turretRange offset");
static_assert(__builtin_offsetof(PlayerTurret, hostOffset) == 0x158, "PlayerTurret local host offset");
static_assert(sizeof(PlayerTurret) == 0x168, "PlayerTurret size");
#endif

#endif
