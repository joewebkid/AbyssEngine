#ifndef GOF2_BOUNDINGSPHERE_H
#define GOF2_BOUNDINGSPHERE_H

#include <cstdint>
#include "Vector.h"
#include "engine/math/BoundingVolume.h"

#include "engine/math/AEMath.h"


class BoundingSphere : public BoundingVolume {
public:
    // Android allocates 0x48 bytes and its constructor stores radius at +0x38.
    uint8_t _opaque_0x20[0x18];
    float radius;
    uint8_t _opaque_0x3c[0x0c];

    BoundingSphere(float x, float y, float z, float ex, float ey, float ez, float radius);

    ~BoundingSphere();

    int collide(float x, float y, float z) override;

    int outerCollide(float x, float y, float z) override;

    void update(float x, float y, float z) override;

    AbyssEngine::AEMath::Vector projectCollisionOnSurface(const AbyssEngine::AEMath::Vector &position) override;

    AbyssEngine::AEMath::Vector getCollisionNormal(const AbyssEngine::AEMath::Vector &position) override;
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(BoundingSphere, radius) == 0x38, "BoundingSphere ARM radius offset");
static_assert(sizeof(BoundingSphere) == 0x48, "BoundingSphere ARM allocation size");
#endif

#endif
