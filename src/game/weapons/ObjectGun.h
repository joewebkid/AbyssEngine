#ifndef GOF2_OBJECTGUN_H
#define GOF2_OBJECTGUN_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Gun.h"

#include "game/weapons/AbstractGun.h"

#include "engine/math/Vector.h"

#include "engine/math/Matrix.h"


class Player;

class AEGeometry;
class Explosion;
class Gun;
class Level;


class ObjectGun : public AbstractGun {
public:
    int unusedSlot;
    Gun *gun;
    Level *level;
    uint32_t transform;
    int secondaryTransform;
    AEGeometry *geometry;
    uint8_t hasGeometry;
    uint8_t wasFiring;
    float rollAngle;
    uint8_t useEgoOrientation;
    int meshId;
    Array<Explosion *> *explosions;
    uint8_t *explosionReady;
    int deltaTime;
    int field_0x38;
    float scaleX;
    float scaleY;
    float scaleZ;
    float spinAngle;
    uint8_t visible;
    Vector dir;
    Vector up;
    Vector side;
    Matrix orientation;

    ObjectGun(int unused, Gun *gun, int mesh, uint32_t param, Level *level);

    virtual ~ObjectGun();

    void setScaling(int x, int y, int z);

    void replaceGun(unsigned int mesh, int unused);

    void setEnemies(Array<Player *> *enemies);

    void setEnemy(Player *enemy);

    void translate(const Vector &v);

    void update(int dt);

    void render();
};
#endif
