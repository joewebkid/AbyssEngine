#ifndef GOF2_ABSTRACTGUN_H
#define GOF2_ABSTRACTGUN_H

#include "engine/core/Array.h"
#include "engine/math/Vector.h"

class Player;

class AbstractGun {
public:
    // Original exports RTTI and the is* helpers, but no AbstractGun destructor/vtable symbol.
    virtual ~AbstractGun() {}

    virtual void setEnemies(Array<Player *> *enemies) = 0;

    virtual void setEnemy(Player *enemy) = 0;

    virtual void update(int dt) = 0;

    virtual void render() = 0;

    virtual void translate(const AbyssEngine::AEMath::Vector &v) = 0;

    virtual void replaceGun(unsigned int mesh, int unused) = 0;

    virtual int isRocketGun();

    virtual int isBombGun();

    virtual int isMineGun();

    virtual int isAutoTurret();
};
#endif
