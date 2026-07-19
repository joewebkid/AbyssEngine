#ifndef GOF2_ROCKETGUN_H
#define GOF2_ROCKETGUN_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
#include "game/weapons/ObjectGun.h"

#include "engine/math/AEMath.h"


class Radar;


class Gun;
class Level;


class RocketGun : public ObjectGun {
public:
    Radar *radar;
    int steerX;
    int steerY;
    int steerZ;
    uint8_t homing;
    int activeShotId;
    float turnRate;
    int particleSystem;
    int rocketKind;
    int fadeTimer;
    Array<AbyssEngine::AEMath::Matrix> *trailMatrices;
    Array<int> *trailSystems;
    Array<int> *trailTimers;
    int particleManager;

    RocketGun(int meshVariantId, Gun *gun, int mesh, int unused4,
              uint32_t flags, int rocketKind, bool homing, Level *level);

    ~RocketGun();

    int isRocketGun() override;

    void render();

    void translate(const AbyssEngine::AEMath::Vector &v);

    void setRadar(Radar *radar);

    void seekEnemy(int unused, int index);

    void update(int elapsed);
};
#endif
