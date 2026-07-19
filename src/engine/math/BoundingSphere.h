#ifndef GOF2_BOUNDINGSPHERE_H
#define GOF2_BOUNDINGSPHERE_H

#include "Vector.h"
#include "engine/math/BoundingVolume.h"

#include "engine/math/AEMath.h"


class BoundingSphere : public BoundingVolume {
public:
    float radius;

    BoundingSphere(float x, float y, float z, float ex, float ey, float ez, float radius);

    ~BoundingSphere();

    int collide(float x, float y, float z) override;

    int outerCollide(float x, float y, float z) override;

    void update(float x, float y, float z) override;

    AbyssEngine::AEMath::Vector projectCollisionOnSurface(const AbyssEngine::AEMath::Vector &position) override;

    AbyssEngine::AEMath::Vector getCollisionNormal(const AbyssEngine::AEMath::Vector &position);
};

#endif
