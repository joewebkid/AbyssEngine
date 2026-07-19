#include "engine/math/BoundingVolume.h"
#include "engine/math/AEMath.h"

namespace AEMath = AbyssEngine::AEMath;
using Vector = AbyssEngine::AEMath::Vector;

BoundingVolume::~BoundingVolume() {
    if (children != nullptr) {
        for (BoundingVolume *child: *children) {
            delete child;
        }
        ArrayRemoveAll(*children);
        delete children;
    }
    children = nullptr;
}

int BoundingVolume::collide(float, float, float) {
    if (children != nullptr) {
        for (BoundingVolume *child: *children) {
            if (child->collide(centerX, centerY, centerZ) != 0) {
                return 1;
            }
        }
    }
    return 0;
}

int BoundingVolume::outerCollide(float, float, float) {
    return 0;
}

Vector BoundingVolume::getCollisionNormal(const Vector &position) {
    (void) position;
    Vector out;
    out.x = 0.0f;
    out.y = 0.0f;
    out.z = 0.0f;
    return out;
}

void BoundingVolume::setVolume(BoundingVolume *src) {
    children = new Array<BoundingVolume *>();
    ArrayAdd(src, *children);
}

void BoundingVolume::setVolumes(Array<BoundingVolume *> *arr) {
    children = arr;
}

BoundingVolume::BoundingVolume(float cx, float cy, float cz, float ex, float ey, float ez) {
    children = nullptr;
    centerX = cx;
    centerY = cy;
    centerZ = cz;
    extentsX = ex;
    extentsY = ey;
    extentsZ = ez;
}

void BoundingVolume::staticProjectCollisionOnSurface(const Vector &v, Array<BoundingVolume *> *vols) {
    centerX = v.x;
    centerY = v.y;
    centerZ = v.z;

    if (vols != nullptr) {
        for (int pass = 0; pass != 2; pass++) {
            for (BoundingVolume * bv


            :
            *vols
            )
            {
                if (bv->outerCollide(centerX, centerY, centerZ) != 0) {
                    Vector out = bv->projectCollisionOnSurface(Vector{centerX, centerY, centerZ});
                    centerX = out.x;
                    centerY = out.y;
                    centerZ = out.z;
                }
            }
        }
    }
}

void BoundingVolume::update(float x, float y, float z) {
    if (children != nullptr) {
        for (BoundingVolume * child


        :
        *children
        )
        {
            child->update(x, y, z);
        }
    }
    centerX = x;
    centerY = y;
    centerZ = z;
}

Vector BoundingVolume::getProjectionVector(const Vector &v) {
    Vector center;
    center.x = centerX;
    center.y = centerY;
    center.z = centerZ;
    return AEMath::VectorNormalize(v - center);
}
