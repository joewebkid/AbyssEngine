#include "engine/math/BoundingAAB.h"
#include "engine/math/BoundingVolume.h"
#include "engine/math/AEMath.h"
#include <cmath>

int BoundingAAB::outerCollide(float x, float y, float z) {
    float centerX = this->centerX + this->extentsX;
    float extentX = this->halfExtentX;
    if (!(centerX - extentX < x)) {
        return 0;
    }
    if (!(x < centerX + extentX)) {
        return 0;
    }

    float centerY = this->centerY + this->extentsY;
    float extentY = this->halfExtentY;
    if (!(centerY - extentY < y)) {
        return 0;
    }
    if (!(y < centerY + extentY)) {
        return 0;
    }

    float centerZ = this->centerZ + this->extentsZ;
    float extentZ = this->halfExtentZ;
    if (!(centerZ - extentZ < z)) {
        return 0;
    }
    if (z < centerZ + extentZ) {
        return 1;
    }
    return 0;
}

Vector BoundingAAB::projectCollisionOnSurface(const Vector &point) {
    float distances[6];
    Vector offsets[6] = {};

    float centerX = this->centerX + this->extentsX;
    float extentX = this->halfExtentX;
    float highX = centerX + extentX;
    float lowX = centerX - extentX;
    offsets[0].x = point.x - highX;
    offsets[1].x = point.x - lowX;

    float centerY = this->centerY + this->extentsY;
    float extentY = this->halfExtentY;
    float highY = centerY + extentY;
    float lowY = centerY - extentY;
    offsets[2].y = point.y - highY;
    offsets[3].y = point.y - lowY;

    float centerZ = this->centerZ + this->extentsZ;
    float extentZ = this->halfExtentZ;
    float highZ = centerZ + extentZ;
    float lowZ = centerZ - extentZ;
    offsets[4].z = point.z - highZ;
    offsets[5].z = point.z - lowZ;

    distances[0] = std::fabs(offsets[0].x);
    distances[1] = std::fabs(offsets[1].x);
    distances[2] = std::fabs(offsets[2].y);
    distances[3] = std::fabs(offsets[3].y);
    distances[4] = std::fabs(offsets[4].z);
    distances[5] = std::fabs(offsets[5].z);

    float closest = distances[0];
    int closestIndex = 0;
    for (int i = 1; i != 6; i++) {
        if (distances[i] < closest) {
            closest = distances[i];
            closestIndex = i;
        }
    }

    return point - offsets[closestIndex];
}

BoundingAAB::BoundingAAB(float x, float y, float z, float ex, float ey, float ez,
                         float width, float height, float depth)
    : BoundingVolume(x, y, z, ex, ey, ez) {
    float halfWidth = width * 0.5f;
    float extentX = width * -0.5f;
    if (0.0f < halfWidth) {
        extentX = halfWidth;
    }

    float halfHeight = height * 0.5f;
    float extentY = height * -0.5f;
    if (0.0f < halfHeight) {
        extentY = halfHeight;
    }

    float halfDepth = depth * 0.5f;
    float extentZ = depth * -0.5f;
    if (0.0f < halfDepth) {
        extentZ = halfDepth;
    }

    this->halfExtentX = extentX;
    this->halfExtentY = extentY;
    this->halfExtentZ = extentZ;
}

BoundingAAB::~BoundingAAB() {
}

Vector BoundingAAB::getCollisionNormal(const Vector &) {
    Vector out;
    out.x = 0.0f;
    out.y = 0.0f;
    out.z = 0.0f;
    return out;
}

int BoundingAAB::collide(float x, float y, float z) {
    if (this->outerCollide(x, y, z) == 0) {
        return 0;
    }
    return BoundingVolume::collide(x, y, z);
}

void BoundingAAB::update(float x, float y, float z) {
    BoundingVolume::update(x, y, z);
}
