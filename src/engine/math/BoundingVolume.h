#ifndef GOF2_BOUNDINGVOLUME_H
#define GOF2_BOUNDINGVOLUME_H

#include "Vector.h"
#include "engine/core/Array.h"

#include "engine/math/AEMath.h"


class BoundingVolume {
public:
    Array<BoundingVolume *> *children;
    float centerX;
    float centerY;
    float centerZ;
    float extentsX;
    float extentsY;
    float extentsZ;

    BoundingVolume(float cx, float cy, float cz, float ex, float ey, float ez);

    virtual ~BoundingVolume();

    virtual int collide(float x, float y, float z);

    virtual int outerCollide(float x, float y, float z);

    virtual void update(float x, float y, float z);

    virtual AbyssEngine::AEMath::Vector projectCollisionOnSurface(const AbyssEngine::AEMath::Vector &point) = 0;

    AbyssEngine::AEMath::Vector getCollisionNormal(const AbyssEngine::AEMath::Vector &position);

    void setVolume(BoundingVolume *src);

    void setVolumes(Array<BoundingVolume *> *arr);

    void staticProjectCollisionOnSurface(const AbyssEngine::AEMath::Vector &v, Array<BoundingVolume *> *vols);

    AbyssEngine::AEMath::Vector getProjectionVector(const AbyssEngine::AEMath::Vector &v);
};

#endif
