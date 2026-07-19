#include "engine/math/AEMath.h"

#include <cmath>

namespace AbyssEngine {
    namespace AEMath {
        float Sqrtf(float value) { return ::sqrtf(value); }
        float Sinf(float value) { return ::sinf(value); }
        float Cosf(float value) { return ::cosf(value); }
        float ACosf(float value) { return ::acosf(value); }
        float ATanf(float value) { return ::atanf(value); }
        float Pow(float lhs, float rhs) { return ::powf(lhs, rhs); }

        float Absf(float value) {
            return value >= 0.0f ? value : -value;
        }

        float Max(float lhs, float rhs) {
            return lhs > rhs ? lhs : rhs;
        }

        float Min(float lhs, float rhs) {
            return lhs < rhs ? lhs : rhs;
        }

        float InvSqrt(float value) {
            union {
                float f;
                int i;
            } bits;

            bits.f = value;
            bits.i = 0x5f3759df - (bits.i >> 1);
            bits.f = bits.f * (1.5f - value * 0.5f * bits.f * bits.f);
            return bits.f;
        }

        Vector &Vector::operator=(const Vector &other) {
            x = other.x;
            y = other.y;
            z = other.z;
            return *this;
        }

        Vector &Vector::operator+=(const Vector &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }

        Vector &Vector::operator-=(const Vector &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }

        Vector &Vector::operator*=(float scale) {
            x *= scale;
            y *= scale;
            z *= scale;
            return *this;
        }

        Vector &Vector::operator*=(const Vector &other) {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }

        Vector &Vector::operator/=(float scale) {
            x /= scale;
            y /= scale;
            z /= scale;
            return *this;
        }

        Vector &Vector::operator/=(const Vector &other) {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        Vector operator+(const Vector &value) {
            return value;
        }

        Vector operator-(const Vector &value) {
            Vector result;
            result.x = -value.x;
            result.y = -value.y;
            result.z = -value.z;
            return result;
        }

        Vector operator+(const Vector &lhs, const Vector &rhs) {
            Vector result;
            result.x = lhs.x + rhs.x;
            result.y = lhs.y + rhs.y;
            result.z = lhs.z + rhs.z;
            return result;
        }

        Vector operator-(const Vector &lhs, const Vector &rhs) {
            Vector result;
            result.x = lhs.x - rhs.x;
            result.y = lhs.y - rhs.y;
            result.z = lhs.z - rhs.z;
            return result;
        }

        Vector operator*(const Vector &lhs, const Vector &rhs) {
            Vector result;
            result.x = lhs.x * rhs.x;
            result.y = lhs.y * rhs.y;
            result.z = lhs.z * rhs.z;
            return result;
        }

        Vector operator*(const Vector &lhs, float rhs) {
            Vector result;
            result.x = lhs.x * rhs;
            result.y = lhs.y * rhs;
            result.z = lhs.z * rhs;
            return result;
        }

        Vector operator*(float lhs, const Vector &rhs) {
            Vector result;
            result.x = rhs.x * lhs;
            result.y = rhs.y * lhs;
            result.z = rhs.z * lhs;
            return result;
        }

        Vector operator/(const Vector &lhs, const Vector &rhs) {
            Vector result;
            result.x = lhs.x / rhs.x;
            result.y = lhs.y / rhs.y;
            result.z = lhs.z / rhs.z;
            return result;
        }

        Vector operator/(const Vector &lhs, float rhs) {
            Vector result;
            result.x = lhs.x / rhs;
            result.y = lhs.y / rhs;
            result.z = lhs.z / rhs;
            return result;
        }

        Vector operator/(float lhs, const Vector &rhs) {
            Vector result;
            result.x = rhs.x / lhs;
            result.y = rhs.y / lhs;
            result.z = rhs.z / lhs;
            return result;
        }

        bool operator==(const Vector &lhs, const Vector &rhs) {
            return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
        }

        bool operator!=(const Vector &lhs, const Vector &rhs) {
            return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
        }

        bool operator<(const Vector &lhs, const Vector &rhs) {
            return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
        }

        bool operator>(const Vector &lhs, const Vector &rhs) {
            return lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z;
        }

        bool operator<=(const Vector &lhs, const Vector &rhs) {
            return lhs.x <= rhs.x && lhs.y <= rhs.y && lhs.z <= rhs.z;
        }

        bool operator>=(const Vector &lhs, const Vector &rhs) {
            return lhs.x >= rhs.x && lhs.y >= rhs.y && lhs.z >= rhs.z;
        }

        bool VectorIsEqual(const Vector &lhs, const Vector &rhs) {
            return lhs == rhs;
        }

        float VectorDot(const Vector &lhs, const Vector &rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
        }

        Vector VectorCross(const Vector &lhs, const Vector &rhs) {
            Vector result;
            result.x = lhs.y * rhs.z - lhs.z * rhs.y;
            result.y = lhs.z * rhs.x - lhs.x * rhs.z;
            result.z = lhs.x * rhs.y - lhs.y * rhs.x;
            return result;
        }

        Vector VectorNormalize(const Vector &value) {
            const float length = ::sqrtf(value.x * value.x + value.y * value.y + value.z * value.z);
            Vector result;
            if (length == 0.0f) {
                result.x = 0.0f;
                result.y = 1.0f;
                result.z = 0.0f;
            } else {
                result.x = value.x / length;
                result.y = value.y / length;
                result.z = value.z / length;
            }
            return result;
        }

        float VectorLength(const Vector &value) {
            return ::sqrtf(value.x * value.x + value.y * value.y + value.z * value.z);
        }

        Vector VectorLerp(const Vector &from, const Vector &to, float t) {
            Vector result;
            result.x = from.x + (to.x - from.x) * t;
            result.y = from.y + (to.y - from.y) * t;
            result.z = from.z + (to.z - from.z) * t;
            return result;
        }

        Matrix &Matrix::operator=(const Matrix &other) {
            for (int i = 0; i < 16; ++i) {
                m[i] = other.m[i];
            }
            return *this;
        }

        bool operator==(const Matrix &lhs, const Matrix &rhs) {
            for (int i = 0; i < 12; ++i) {
                if (lhs.m[i] != rhs.m[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const Matrix &lhs, const Matrix &rhs) {
            return !(lhs == rhs);
        }

        Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
            Matrix result;

            result.m[0] = lhs.m[0] * rhs.m[0] + lhs.m[1] * rhs.m[4] + lhs.m[2] * rhs.m[8];
            result.m[1] = lhs.m[0] * rhs.m[1] + lhs.m[1] * rhs.m[5] + lhs.m[2] * rhs.m[9];
            result.m[2] = lhs.m[0] * rhs.m[2] + lhs.m[1] * rhs.m[6] + lhs.m[2] * rhs.m[10];
            result.m[3] = lhs.m[3] + lhs.m[0] * rhs.m[3] + lhs.m[1] * rhs.m[7] + lhs.m[2] * rhs.m[11];

            result.m[4] = lhs.m[4] * rhs.m[0] + lhs.m[5] * rhs.m[4] + lhs.m[6] * rhs.m[8];
            result.m[5] = lhs.m[4] * rhs.m[1] + lhs.m[5] * rhs.m[5] + lhs.m[6] * rhs.m[9];
            result.m[6] = lhs.m[4] * rhs.m[2] + lhs.m[5] * rhs.m[6] + lhs.m[6] * rhs.m[10];
            result.m[7] = lhs.m[7] + lhs.m[4] * rhs.m[3] + lhs.m[5] * rhs.m[7] + lhs.m[6] * rhs.m[11];

            result.m[8] = lhs.m[8] * rhs.m[0] + lhs.m[9] * rhs.m[4] + lhs.m[10] * rhs.m[8];
            result.m[9] = lhs.m[8] * rhs.m[1] + lhs.m[9] * rhs.m[5] + lhs.m[10] * rhs.m[9];
            result.m[10] = lhs.m[8] * rhs.m[2] + lhs.m[9] * rhs.m[6] + lhs.m[10] * rhs.m[10];
            result.m[11] = lhs.m[11] + lhs.m[8] * rhs.m[3] + lhs.m[9] * rhs.m[7] + lhs.m[10] * rhs.m[11];

            result.m[12] = lhs.m[12] * rhs.m[12];
            result.m[13] = lhs.m[13] * rhs.m[13];
            result.m[14] = lhs.m[14] * rhs.m[14];
            result.m[15] = 1.0f;

            return result;
        }

        Vector operator*(const Matrix &matrix, const Vector &vector) {
            Vector result;
            result.x = vector.x * matrix.m[0] + vector.y * matrix.m[1] + vector.z * matrix.m[2] + matrix.m[3];
            result.y = vector.x * matrix.m[4] + vector.y * matrix.m[5] + vector.z * matrix.m[6] + matrix.m[7];
            result.z = vector.x * matrix.m[8] + vector.y * matrix.m[9] + vector.z * matrix.m[10] + matrix.m[11];
            return result;
        }

        Matrix &Matrix::operator*=(const Matrix &other) {
            *this = *this * other;
            return *this;
        }

        void MatrixMultiply(const Matrix &lhs, const Matrix &rhs) {
            lhs * rhs;
        }

        Matrix MatrixIdentity(Matrix &matrix) {
            for (int i = 0; i < 16; ++i) {
                matrix.m[i] = 0.0f;
            }
            matrix.m[0] = 1.0f;
            matrix.m[5] = 1.0f;
            matrix.m[10] = 1.0f;
            matrix.m[12] = 1.0f;
            matrix.m[13] = 1.0f;
            matrix.m[14] = 1.0f;
            return matrix;
        }

        bool MatrixIsEqual(const Matrix &lhs, const Matrix &rhs) {
            return lhs == rhs;
        }

        Vector MatrixGetRight(const Matrix &matrix) {
            Vector result;
            result.x = matrix.m[0];
            result.y = matrix.m[4];
            result.z = matrix.m[8];
            return result;
        }

        Vector MatrixGetUp(const Matrix &matrix) {
            Vector result;
            result.x = matrix.m[1];
            result.y = matrix.m[5];
            result.z = matrix.m[9];
            return result;
        }

        Vector MatrixGetDir(const Matrix &matrix) {
            Vector result;
            result.x = matrix.m[2];
            result.y = matrix.m[6];
            result.z = matrix.m[10];
            return result;
        }

        Vector MatrixGetPosition(const Matrix &matrix) {
            Vector result;
            result.x = matrix.m[3];
            result.y = matrix.m[7];
            result.z = matrix.m[11];
            return result;
        }

        Vector MatrixTransformVector(const Matrix &matrix, const Vector &vector) {
            Vector result;
            result.x = vector.x * matrix.m[0] + vector.y * matrix.m[1] + vector.z * matrix.m[2] + matrix.m[3];
            result.y = vector.x * matrix.m[4] + vector.y * matrix.m[5] + vector.z * matrix.m[6] + matrix.m[7];
            result.z = vector.x * matrix.m[8] + vector.y * matrix.m[9] + vector.z * matrix.m[10] + matrix.m[11];
            return result;
        }

        Vector MatrixRotateVector(const Matrix &matrix, const Vector &vector) {
            Vector result;
            result.x = vector.x * matrix.m[0] + vector.y * matrix.m[1] + vector.z * matrix.m[2];
            result.y = vector.x * matrix.m[4] + vector.y * matrix.m[5] + vector.z * matrix.m[6];
            result.z = vector.x * matrix.m[8] + vector.y * matrix.m[9] + vector.z * matrix.m[10];
            return result;
        }

        Vector MatrixInverseTransformVector(const Matrix &matrix, const Vector &vector) {
            const float tx = -(matrix.m[0] * matrix.m[3]) - matrix.m[4] * matrix.m[7] - matrix.m[8] * matrix.m[11];
            const float ty = -(matrix.m[1] * matrix.m[3]) - matrix.m[5] * matrix.m[7] - matrix.m[9] * matrix.m[11];
            const float tz = -(matrix.m[2] * matrix.m[3]) - matrix.m[6] * matrix.m[7] - matrix.m[10] * matrix.m[11];

            Vector result;
            result.x = vector.x * matrix.m[0] + vector.y * matrix.m[4] + vector.z * matrix.m[8] + tx;
            result.y = vector.x * matrix.m[1] + vector.y * matrix.m[5] + vector.z * matrix.m[9] + ty;
            result.z = vector.x * matrix.m[2] + vector.y * matrix.m[6] + vector.z * matrix.m[10] + tz;
            return result;
        }

        Vector MatrixInverseRotateVector(const Matrix &matrix, const Vector &vector) {
            Vector result;
            result.x = vector.x * matrix.m[0] + vector.y * matrix.m[4] + vector.z * matrix.m[8];
            result.y = vector.x * matrix.m[1] + vector.y * matrix.m[5] + vector.z * matrix.m[9];
            result.z = vector.x * matrix.m[2] + vector.y * matrix.m[6] + vector.z * matrix.m[10];
            return result;
        }

        Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z) {
            const float sx = ::sinf(x), sy = ::sinf(y), sz = ::sinf(z);
            const float cx = ::cosf(x), cy = ::cosf(y), cz = ::cosf(z);

            matrix.m[0] = cy * cz;
            matrix.m[1] = -(sz * cy);
            matrix.m[2] = sy;
            matrix.m[4] = sy * (sx * cz) + sz * cx;
            matrix.m[5] = cx * cz - (sx * sy) * sz;
            matrix.m[6] = -(sx * cy);
            matrix.m[8] = sx * sz - sy * (cx * cz);
            matrix.m[9] = sx * cz + sz * (sy * cx);
            matrix.m[10] = cx * cy;
            return matrix;
        }

        Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z, RotationOrder order) {
            const float sx = ::sinf(x), sy = ::sinf(y), sz = ::sinf(z);
            const float cx = ::cosf(x), cy = ::cosf(y), cz = ::cosf(z);

            switch (order) {
                case ROTATION_ORDER_XYZ:
                    matrix.m[0] = cy * cz;
                    matrix.m[1] = -(sz * cy);
                    matrix.m[2] = sy;
                    matrix.m[8] = sx * sz - sy * cx * cz;
                    matrix.m[9] = sx * cz + sz * sy * cx;
                    matrix.m[10] = cx * cy;
                    matrix.m[4] = sy * sx * cz + sz * cx;
                    matrix.m[5] = cx * cz - sx * sy * sz;
                    matrix.m[6] = -(sx * cy);
                    break;

                case ROTATION_ORDER_XZY:
                    matrix.m[0] = cy * cz;
                    matrix.m[1] = -sz;
                    matrix.m[2] = sy * cz;
                    matrix.m[4] = sx * sy + sz * cx * cy;
                    matrix.m[5] = cx * cz;
                    matrix.m[6] = sz * sy * cx - sx * cy;
                    matrix.m[8] = sz * sx * cy - sy * cx;
                    matrix.m[9] = sx * cz;
                    matrix.m[10] = cx * cy + sx * sy * sz;
                    break;

                case ROTATION_ORDER_YXZ:
                    matrix.m[4] = sz * cx;
                    matrix.m[5] = cx * cz;
                    matrix.m[6] = -sx;
                    matrix.m[0] = cy * cz + sx * sy * sz;
                    matrix.m[1] = sy * sx * cz - sz * cy;
                    matrix.m[2] = sy * cx;
                    matrix.m[8] = sz * sx * cy - sy * cz;
                    matrix.m[9] = sy * sz + sx * cy * cz;
                    matrix.m[10] = cx * cy;
                    break;

                case ROTATION_ORDER_YZX:
                    matrix.m[4] = sz;
                    matrix.m[5] = cx * cz;
                    matrix.m[6] = -(sx * cz);
                    matrix.m[8] = -(sy * cz);
                    matrix.m[9] = sx * cy + cx * sy * sz;
                    matrix.m[10] = cx * cy - sx * sy * sz;
                    matrix.m[0] = cy * cz;
                    matrix.m[1] = sx * sy - cx * sz * cy;
                    matrix.m[2] = sx * sz * cy + sy * cx;
                    break;

                case ROTATION_ORDER_ZXY:
                    matrix.m[8] = -(sy * cx);
                    matrix.m[9] = sx;
                    matrix.m[10] = cx * cy;
                    matrix.m[0] = cy * cz - sx * sy * sz;
                    matrix.m[1] = -(sz * cx);
                    matrix.m[2] = sy * cz + sx * sz * cy;
                    matrix.m[4] = sy * sx * cz + sz * cy;
                    matrix.m[5] = cx * cz;
                    matrix.m[6] = sy * sz - sx * cy * cz;
                    break;

                case ROTATION_ORDER_ZYX:
                    matrix.m[8] = -sy;
                    matrix.m[9] = sx * cy;
                    matrix.m[10] = cx * cy;
                    matrix.m[4] = sz * cy;
                    matrix.m[5] = cx * cz + sx * sy * sz;
                    matrix.m[6] = cx * sy * sz - sx * cz;
                    matrix.m[0] = cy * cz;
                    matrix.m[1] = sy * sx * cz - sz * cx;
                    matrix.m[2] = sy * cx * cz + sx * sz;
                    break;
            }

            return matrix;
        }

        Matrix MatrixSetRotation(Matrix &matrix, const Vector &dir) {
            Vector worldUp;
            worldUp.x = 0.0f;
            worldUp.y = 1.0f;
            worldUp.z = 0.0f;

            Vector right = VectorNormalize(VectorCross(worldUp, dir));
            Vector up = VectorNormalize(VectorCross(dir, right));
            return MatrixSetRotation(matrix, right, up, dir);
        }

        Matrix MatrixSetRotation(Matrix &matrix, const Vector &right, const Vector &up, const Vector &dir) {
            matrix.m[0] = right.x;
            matrix.m[4] = right.y;
            matrix.m[8] = right.z;
            matrix.m[1] = up.x;
            matrix.m[5] = up.y;
            matrix.m[9] = up.z;
            matrix.m[2] = dir.x;
            matrix.m[6] = dir.y;
            matrix.m[10] = dir.z;
            return matrix;
        }

        Matrix MatrixSetScaling(Matrix &matrix, float x, float y, float z) {
            matrix.m[0] *= x;
            matrix.m[4] *= x;
            matrix.m[8] *= x;
            matrix.m[1] *= y;
            matrix.m[5] *= y;
            matrix.m[9] *= y;
            matrix.m[2] *= z;
            matrix.m[6] *= z;
            matrix.m[10] *= z;
            matrix.m[12] = x;
            matrix.m[13] = y;
            matrix.m[14] = z;
            return matrix;
        }

        Matrix MatrixSetTranslation(Matrix &matrix, float x, float y, float z) {
            matrix.m[3] = x;
            matrix.m[7] = y;
            matrix.m[11] = z;
            return matrix;
        }

        Matrix MatrixSetTranslation(Matrix &matrix, const Vector &translation) {
            matrix.m[3] = translation.x;
            matrix.m[7] = translation.y;
            matrix.m[11] = translation.z;
            return matrix;
        }

        Matrix MatrixGetInverse(const Matrix &matrix) {
            Matrix result;

            result.m[0] = matrix.m[0];
            result.m[1] = matrix.m[4];
            result.m[2] = matrix.m[8];

            result.m[4] = matrix.m[1];
            result.m[5] = matrix.m[5];
            result.m[6] = matrix.m[9];

            result.m[8] = matrix.m[2];
            result.m[9] = matrix.m[6];
            result.m[10] = matrix.m[10];

            result.m[3] = -(matrix.m[3] * matrix.m[0]) - matrix.m[7] * matrix.m[4] - matrix.m[11] * matrix.m[8];
            result.m[7] = -(matrix.m[3] * matrix.m[1]) - matrix.m[7] * matrix.m[5] - matrix.m[11] * matrix.m[9];
            result.m[11] = -(matrix.m[3] * matrix.m[2]) - matrix.m[7] * matrix.m[6] - matrix.m[11] * matrix.m[10];

            return result;
        }

        Matrix MatrixGetLookAt(const Vector &position, const Vector &target, const Vector &up) {
            Vector dir = VectorNormalize(position - target);
            Vector right = VectorNormalize(VectorCross(up, dir));
            Vector newUp = VectorCross(dir, right);

            Matrix result;
            result.m[0] = right.x;
            result.m[1] = newUp.x;
            result.m[2] = dir.x;
            result.m[3] = position.x;
            result.m[4] = right.y;
            result.m[5] = newUp.y;
            result.m[6] = dir.y;
            result.m[7] = position.y;
            result.m[8] = right.z;
            result.m[9] = newUp.z;
            result.m[10] = dir.z;
            result.m[11] = position.z;
            result.m[12] = 1.0f;
            result.m[13] = 1.0f;
            result.m[14] = 1.0f;
            return result;
        }

        void MatrixGetGL(const Matrix &matrix, float *out) {
            out[0] = matrix.m[0];
            out[4] = matrix.m[1];
            out[8] = matrix.m[2];
            out[12] = matrix.m[3];
            out[1] = matrix.m[4];
            out[5] = matrix.m[5];
            out[9] = matrix.m[6];
            out[13] = matrix.m[7];
            out[2] = matrix.m[8];
            out[6] = matrix.m[9];
            out[10] = matrix.m[10];
            out[14] = matrix.m[11];
            out[3] = 0.0f;
            out[7] = 0.0f;
            out[11] = 0.0f;
            out[15] = 1.0f;
        }

        void MatrixDebugOut(const Matrix &matrix) {
            (void) matrix;
        }
    }
}
