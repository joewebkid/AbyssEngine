#include "engine/math/BSphere.h"
#include <cmath>

#include "Matrix.h"
#include "Vector.h"

namespace AbyssEngine {
    namespace AEMath {
        Vector MatrixTransformVector(const Matrix &matrix, const Vector &vector);

        Vector MatrixRotateVector(const Matrix &matrix, const Vector &vector);

        BSphere &BSphere::operator=(const BSphere &other) {
            this->center.x = other.center.x;
            this->center.y = other.center.y;
            this->center.z = other.center.z;
            this->radius = other.radius;
            this->radius2 = other.radius2;
            return *this;
        }

        void BSphere::Merge(const BSphere &other) {
            if (this->radius2 < other.radius2)
                this->radius2 = other.radius2;

            if (other.radius == 0.0f)
                return;

            if (this->radius == 0.0f) {
                *this = other;
                return;
            }

            float dx = other.center.x - this->center.x;
            float dy = other.center.y - this->center.y;
            float dz = other.center.z - this->center.z;
            float dist = std::sqrt(dx * dx + dy * dy + dz * dz);

            float r1 = this->radius;
            float r2 = other.radius;

            if (dist == 0.0f) {
                if (r2 < r1)
                    r2 = r1;
                this->radius = r2;
                return;
            }

            if (r1 > dist + r2)
                return;

            if (dist - r2 < -r1) {
                this->center.x = other.center.x;
                this->center.y = other.center.y;
                this->center.z = other.center.z;
                this->radius = r2;
                return;
            }

            float t = (((dist + r2) - r1) * 0.5f) / dist;
            this->center.x = this->center.x + dx * t;
            this->center.y = this->center.y + dy * t;
            this->center.z = this->center.z + dz * t;
            this->radius = (dist + r1 + r2) * 0.5f;
        }

        void BSphere::Merge(const Transform &t) {
            const Matrix &worldMatrix = *reinterpret_cast<const Matrix *>(&t);
            float boundingRadius = *reinterpret_cast<const float *>(
                reinterpret_cast<const char *>(&t) + 0xe0);

            Vector origin = {0.0f, 0.0f, 0.0f};
            Vector center = MatrixTransformVector(worldMatrix, origin);

            Vector axisRadius = {boundingRadius, boundingRadius, boundingRadius};
            Vector rotated = MatrixRotateVector(worldMatrix, axisRadius);

            float ax = std::fabs(rotated.x);
            float ay = std::fabs(rotated.y);
            float az = std::fabs(rotated.z);
            if (ax > ay) ay = ax;
            if (ay > az) az = ay;

            BSphere world;
            world.center.x = center.x;
            world.center.y = center.y;
            world.center.z = center.z;
            world.radius = az;
            world.radius2 = 1.0f;
            Merge(world);
        }
    }
}
