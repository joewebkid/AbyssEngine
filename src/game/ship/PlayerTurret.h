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
    char field_0x3e;
    bool isSentryGun;
    Vector cachedPosition;
    Vector hostWorldOffset;
    Vector aimPoint;
    int reviveFlag;

    int frameDelta;
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

#endif
