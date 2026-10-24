#include "engine/math/BoundingSphere.h"
#include "engine/math/AEMath.h"

BoundingSphere::BoundingSphere(
    float x, float y, float z, float ex, float ey, float ez, float radius)
    : BoundingVolume(x, y, z, ex, ey, ez) {
    this->radius = radius;
}

BoundingSphere::~BoundingSphere() {
}

void BoundingSphere::update(float x, float y, float z) {
    BoundingVolume::update(x, y, z);
}

Vector BoundingSphere::projectCollisionOnSurface(const Vector &position) {
    Vector center = Vector{centerX, centerY, centerZ}
                    + Vector{extentsX, extentsY, extentsZ};
    Vector delta = center - position;
    float length = VectorLength(delta);

    if (length >= radius) {
        return Vector{0.0f, 0.0f, 0.0f};
    }
    Vector projectedCenter = Vector{centerX, centerY, centerZ}
                             + Vector{extentsX, extentsY, extentsZ};
    return projectedCenter - (radius / length) * delta;
}

int BoundingSphere::outerCollide(float x, float y, float z) {
    Vector center = Vector{centerX, centerY, centerZ}
                    + Vector{extentsX, extentsY, extentsZ};
    Vector delta = Vector{x, y, z} - center;
    float distanceSq = VectorDot(delta, delta);
    return distanceSq < radius * radius ? 1 : 0;
}

Vector BoundingSphere::getCollisionNormal(const Vector &position) {
    Vector center = Vector{centerX, centerY, centerZ}
                    + Vector{extentsX, extentsY, extentsZ};
    return VectorNormalize(center - position);
}

int BoundingSphere::collide(float x, float y, float z) {
    if (outerCollide(x, y, z)) {
        return 1;
    }
    return BoundingVolume::collide(x, y, z);
}
