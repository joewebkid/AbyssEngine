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
    Vector center = Vector{centerX + extentsX, centerY + extentsY, centerZ + extentsZ};
    Vector delta = center - position;
    float length = VectorLength(delta);

    if (length >= radius) {
        return Vector{0.0f, 0.0f, 0.0f};
    }
    return center - delta * (radius / length);
}

int BoundingSphere::outerCollide(float x, float y, float z) {
    Vector delta = Vector{x, y, z}
                   - Vector{centerX + extentsX, centerY + extentsY, centerZ + extentsZ};
    float distanceSq = VectorDot(delta, delta);
    return distanceSq < radius * radius ? 1 : 0;
}

Vector BoundingSphere::getCollisionNormal(const Vector &position) {
    return VectorNormalize(
        Vector{centerX + extentsX, centerY + extentsY, centerZ + extentsZ} - position);
}

int BoundingSphere::collide(float x, float y, float z) {
    if (outerCollide(x, y, z)) {
        return 1;
    }
    return BoundingVolume::collide(x, y, z);
}
