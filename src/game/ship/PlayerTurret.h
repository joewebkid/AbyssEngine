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


    bool turretEnabled;
    bool isSentryGun;
    Vector &cachedPosition() { return reinterpret_cast<Vector &>(this->posX); }
    Vector &hostWorldOffset() { return reinterpret_cast<Vector &>(this->field_0x90); }
    Vector &aimPoint() { return reinterpret_cast<Vector &>(this->field_0x9c); }
    int &reviveFlag() { return reinterpret_cast<int &>(this->rotationSpeed); }
    int &frameDelta() { return reinterpret_cast<int &>(this->type); }

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

    void setPosition(const Vector &position);

    void reset();

    void setLevel(Level *level);

    KIPlayer *getHost();

    void setScaling(float scale);

    void handleRotation(int delta, AEGeometry *mainGeometry, AEGeometry *turretGeometry);

    void update(int delta) override;

    void pickEnemy();
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PlayerTurret, isSentryGun) == 0x125,
              "PlayerTurret::isSentryGun offset");
static_assert(__builtin_offsetof(PlayerTurret, spawnInvulnTimer) == 0x128,
              "PlayerTurret::spawnInvulnTimer offset");
static_assert(__builtin_offsetof(PlayerTurret, explosion) == 0x13c,
              "PlayerTurret::explosion offset");
static_assert(__builtin_offsetof(PlayerTurret, turretHost) == 0x154,
              "PlayerTurret::turretHost offset");
static_assert(__builtin_offsetof(PlayerTurret, turretRange) == 0x164,
              "PlayerTurret::turretRange offset");
static_assert(sizeof(PlayerTurret) == 0x168, "PlayerTurret size");
#endif

#endif
