#ifndef GOF2_PLAYERGASCLOUD_H
#define GOF2_PLAYERGASCLOUD_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"
class AEGeometry;
class ParticleSystemManager;


class PlayerGasCloud : public KIPlayer {
public:
    Vector center;
    AEGeometry *modelGeometry;
    Array<AEGeometry *> *sparkGeometries;
    Array<Vector *> *sparkVelocities;
    Array<float> *sparkLife;
    Array<float> *sparkLifeMin;
    Array<float> *sparkScale;
    Array<int> *sparkTimers;
    Array<bool> *sparkInSight;
    uint8_t exploded;
    int elapsedSinceExplosion;
    uint8_t settled;
    int itemId;
    uint16_t sparkMeshId;
    int cloudMeshId;

    PlayerGasCloud(int itemId, ParticleSystemManager *particles, AEGeometry *geometry,
                   const Vector &position);

    ~PlayerGasCloud();

    void translate(const Vector &v) override;

    bool isSparkAlive(int index);

    void setSparkInSight(int index, bool inSight);

    void setPosition(const Vector &position);

    void *getSparks();

    Vector getPosition();

    uint8_t hasExploded();

    void render() override;

    void explode(int itemIndex, Vector src, float radius);

    void update(int dt) override;
};
#endif
