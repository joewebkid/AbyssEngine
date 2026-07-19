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
    int lastDelta;
    float scaling;
    uint8_t minable;
    Vector spin;
    uint8_t rotationEnabled;
    int quality;
    int lastHitpoints;
    int hitFlashActive;
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
#endif
