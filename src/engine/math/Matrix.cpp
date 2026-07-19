#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
#include "engine/math/Quaternion.h"

namespace AbyssEngine {
    namespace AEMath {
        Matrix::Matrix() {
            m[0] = 1.0f;
            m[1] = 0.0f;
            m[2] = 0.0f;
            m[3] = 0.0f;
            m[4] = 0.0f;
            m[5] = 1.0f;
            m[6] = 0.0f;
            m[7] = 0.0f;
            m[8] = 0.0f;
            m[9] = 0.0f;
            m[10] = 0.0f;
            m[11] = 0.0f;
            m[12] = 1.0f;
            m[13] = 0.0f;
            m[14] = 1.0f;
        }

        Matrix::operator float *() { return m; }
        Matrix::operator const float *() const { return m; }
    }
}

namespace AbyssEngine {
    namespace AEMath {
        Matrix &Matrix::operator*=(const Matrix &p) {
            float *t = this->m;
            const float *b = p.m;

            float a0 = t[0], a1 = t[1], a2 = t[2];
            t[0] = a1 * b[4] + a0 * b[0] + a2 * b[8];
            t[1] = a1 * b[5] + a0 * b[1] + a2 * b[9];
            t[2] = a1 * b[6] + a0 * b[2] + a2 * b[10];
            t[3] = t[3] + a1 * b[7] + a0 * b[3] + a2 * b[11];

            float c0 = t[4], c1 = t[5], c2 = t[6];
            t[4] = c0 * b[0] + b[4] * c1 + b[8] * c2;
            t[5] = c0 * b[1] + b[5] * c1 + b[9] * c2;
            t[6] = c0 * b[2] + b[6] * c1 + b[10] * c2;
            t[7] = t[7] + c0 * b[3] + b[7] * c1 + b[11] * c2;

            float e1 = t[9], e0 = t[8], e2 = t[10];
            t[8] = e1 * b[4] + b[0] * e0 + b[8] * e2;
            t[9] = e1 * b[5] + b[1] * e0 + b[9] * e2;
            t[10] = e1 * b[6] + b[2] * e0 + b[10] * e2;
            t[11] = t[11] + e1 * b[7] + b[3] * e0 + b[11] * e2;

            *(Vector *) &this->m[12] *= *(const Vector *) &p.m[12];
            return *this;
        }
    }
}

namespace AbyssEngine {
    namespace AEMath {
        Matrix &Matrix::operator=(const Matrix &other) {
            for (int i = 0; i < 12; ++i)
                this->m[i] = other.m[i];
            *(Vector *) &this->m[12] = *(const Vector *) &other.m[12];
            return *this;
        }
    }
}

namespace AbyssEngine {
    Quaternion::~Quaternion() {
    }

    Quaternion::operator float *() { return &x; }
    Quaternion::operator const float *() const { return &x; }
    float &Quaternion::operator[](int i) { return (&x)[i]; }
    float Quaternion::operator[](int i) const { return (&x)[i]; }
}
