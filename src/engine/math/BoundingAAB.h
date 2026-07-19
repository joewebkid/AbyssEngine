#ifndef GOF2_BOUNDINGAAB_H
#define GOF2_BOUNDINGAAB_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/BoundingVolume.h"

#include "engine/math/Vector.h"


class BoundingAAB : public BoundingVolume {
public:
    float halfExtentX;
    float halfExtentY;
    float halfExtentZ;

    BoundingAAB(float x, float y, float z, float ex, float ey, float ez,
                float width, float height, float depth);

    ~BoundingAAB();

    int outerCollide(float x, float y, float z) override;

    Vector projectCollisionOnSurface(const Vector &point) override;

    Vector getCollisionNormal(const Vector &point);

    int collide(float x, float y, float z) override;

    void update(float x, float y, float z) override;
};
#endif
