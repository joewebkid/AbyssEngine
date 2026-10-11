#ifndef GOF2_PLAYERASTEROID_H
#define GOF2_PLAYERASTEROID_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/math/Matrix.h"
#include "game/mission/Explosion.h"

#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"
class AEGeometry;
class Explosion;


class PlayerAsteroid : public KIPlayer {
public:



    uint8_t asteroidFlag;
    int asteroidIndex;
    Explosion *explosion;
    int field_0x130;
    int lastDelta;
    float scaling;
    uint8_t minable;
    Vector spin;
    uint8_t rotationEnabled;
    int quality;
    int field_0x154;
    int lastHitpoints;
    float hitFlashAmount;
    float hitFlashTimer;
    int field_0x164;
    int field_0x168;
    int field_0x16c;

    PlayerAsteroid(int playerId, AEGeometry *geometry, int explosionType, int asteroidIndex,
                   const Vector &position, float scaling, int quality);

    ~PlayerAsteroid();

    void setAsteroidIndex(int asteroidIndex);

    void translate(const Vector &delta) override;

    void render() override;

    void setPosition(const Vector &position);

    int outerCollide(float x, float y, float z) override;

    int outerCollide(Vector point);

    Vector getPosition();

    void setRotationEnabled(bool enabled);

    int getQualityFrameIndex();

    int getQuality();

    float getScaling();

    uint8_t isMinable();

    String getQualityString();

    void update(int delta) override;

    Vector getProjectionVector(const Vector &value) override;

    void setAsteroidCenter(Vector center);

    int collide(float x, float y, float z) override;

    void push(int delta) override;

    void initPush(const Vector &target, int duration) override;

private:
    int &dropsLoot() { return this->hasCargo; }

    Array<int> *&loot() { return this->cargo; }

    AEGeometry *&secondaryGeometry() { return this->crateGeometry; }

    int &pushTimer() { return this->field_0x104; }
    int &pushDuration() { return *reinterpret_cast<int *>(reinterpret_cast<char *>(&this->field_0x104) + 4); }
    Vector &pushDirection() { return *reinterpret_cast<Vector *>(&this->field_0x10c); }
    Vector &pushSpin() { return *reinterpret_cast<Vector *>(&this->field_0x118); }

    // Static data members present in the original binary (defined for symbol parity).
    static AbyssEngine::AEMath::Vector tmp_vector2;
    static AbyssEngine::AEMath::Vector asteroidCenter;
    static float asteroidDistance;
    static AbyssEngine::AEMath::Vector pos;
    static float emitTime;
};

#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(PlayerAsteroid, asteroidFlag) == 0x124, "PlayerAsteroid.asteroidFlag");
static_assert(offsetof(PlayerAsteroid, asteroidIndex) == 0x128, "PlayerAsteroid.asteroidIndex");
static_assert(offsetof(PlayerAsteroid, explosion) == 0x12c, "PlayerAsteroid.explosion");
static_assert(offsetof(PlayerAsteroid, lastDelta) == 0x134, "PlayerAsteroid.lastDelta");
static_assert(offsetof(PlayerAsteroid, scaling) == 0x138, "PlayerAsteroid.scaling");
static_assert(offsetof(PlayerAsteroid, minable) == 0x13c, "PlayerAsteroid.minable");
static_assert(offsetof(PlayerAsteroid, spin) == 0x140, "PlayerAsteroid.spin");
static_assert(offsetof(PlayerAsteroid, rotationEnabled) == 0x14c, "PlayerAsteroid.rotationEnabled");
static_assert(offsetof(PlayerAsteroid, quality) == 0x150, "PlayerAsteroid.quality");
static_assert(offsetof(PlayerAsteroid, lastHitpoints) == 0x158, "PlayerAsteroid.lastHitpoints");
static_assert(offsetof(PlayerAsteroid, hitFlashAmount) == 0x15c, "PlayerAsteroid.hitFlashAmount");
static_assert(offsetof(PlayerAsteroid, hitFlashTimer) == 0x160, "PlayerAsteroid.hitFlashTimer");
static_assert(sizeof(PlayerAsteroid) == 0x170, "PlayerAsteroid ARM size");
#endif
#endif
