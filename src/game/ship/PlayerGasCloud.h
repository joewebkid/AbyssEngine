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
    // The Android constructor places center at +0x128, four bytes after
    // KIPlayer's known +0x124 end. No access identifies this slot yet.
    uint8_t _unknown_0x124[4];
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
    int sparkMeshId;
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

#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(PlayerGasCloud, center) == 0x128, "PlayerGasCloud.center ARM offset");
static_assert(offsetof(PlayerGasCloud, modelGeometry) == 0x134, "PlayerGasCloud.modelGeometry ARM offset");
static_assert(offsetof(PlayerGasCloud, itemId) == 0x160, "PlayerGasCloud.itemId ARM offset");
static_assert(offsetof(PlayerGasCloud, sparkMeshId) == 0x164, "PlayerGasCloud.sparkMeshId ARM offset");
static_assert(offsetof(PlayerGasCloud, cloudMeshId) == 0x168, "PlayerGasCloud.cloudMeshId ARM offset");
static_assert(sizeof(PlayerGasCloud) == 0x16c, "PlayerGasCloud ARM size");
#endif
#endif
