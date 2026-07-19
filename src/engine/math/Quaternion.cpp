#include "engine/math/Quaternion.h"
#include <arm_neon.h>
#include <cmath>

namespace AbyssEngine {
    Quaternion::Quaternion() {
    }

    Quaternion::Quaternion(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {
    }

    Quaternion::Quaternion(Quaternion *other) {
        (void) other;
    }

    Quaternion::Quaternion(AEMath::Vector angles) {
        Set(angles);
    }

    Quaternion::Quaternion(const AEMath::Matrix &matrix) {
        Set(matrix);
    }

    float Quaternion::Dot(const Quaternion &a, const Quaternion &b) {
        return a.y * b.y + a.x * b.x + a.z * b.z + a.w * b.w;
    }

    float Quaternion::Length() const {
        return sqrtf(x * x + y * y + z * z + w * w);
    }

    void Quaternion::Set(AEMath::Vector angles) {
        Set(angles.x, angles.y, angles.z);
    }

    void Quaternion::Set(float x_angle, float y_angle, float z_angle) {
        float sinZ = sinf(z_angle * 0.5f);
        float sinY = sinf(y_angle * 0.5f);
        float sinX = sinf(x_angle * 0.5f);
        float cosZ = cosf(z_angle * 0.5f);
        float cosY = cosf(y_angle * 0.5f);
        float cosX = cosf(x_angle * 0.5f);
        x = sinX * cosZ * cosY - sinZ * sinY * cosX;
        y = -(sinY * cosZ * cosX) + -(sinX * sinZ * cosY);
        z = sinZ * cosY * cosX - sinX * sinY * cosZ;
        w = cosZ * cosY * cosX + sinZ * sinY * sinX;
    }

    void Quaternion::Set(const AEMath::Matrix &matrix) {
        const float one = 1.0f;
        const float epsilon = 0.0000001f;

        float m00 = matrix.m[0];
        float m11 = matrix.m[5];
        float m22 = matrix.m[10];
        float trace = ((m00 + m11) + m22) + one;

        if (trace > epsilon) {
            float scale = sqrtf(trace);
            scale = (one / scale) * 0.5f;

            w = 0.25f / scale;
            x = scale * (matrix.m[6] - matrix.m[9]);
            y = scale * (matrix.m[8] - matrix.m[2]);
            z = scale * (matrix.m[1] - matrix.m[4]);
        } else if (m00 > m11 && m00 > m22) {
            float scale = sqrtf(((m00 + one) - m11) - m22);
            scale = scale + scale;

            float w_value = (matrix.m[9] - matrix.m[6]) / scale;
            x = scale * 0.25f;
            w = w_value;
            y = (matrix.m[4] + matrix.m[1]) / scale;
            z = (matrix.m[8] + matrix.m[2]) / scale;
        } else if (m11 > m22) {
            float scale = sqrtf(((m11 + one) - m00) - m22);
            scale = scale + scale;

            w = (matrix.m[8] - matrix.m[2]) / scale;
            x = (matrix.m[4] + matrix.m[1]) / scale;
            y = scale * 0.25f;
            z = (matrix.m[9] + matrix.m[6]) / scale;
        } else {
            float scale = sqrtf(((m22 + one) - m00) - m11);
            scale = scale + scale;

            w = (matrix.m[4] - matrix.m[1]) / scale;
            x = (matrix.m[8] + matrix.m[2]) / scale;
            y = (matrix.m[9] + matrix.m[6]) / scale;
            z = scale * 0.25f;
        }
    }

    Quaternion Quaternion::Normalized() {
        float length = Length();
        x /= length;
        y /= length;
        z /= length;
        w /= length;
        return *this;
    }

    Quaternion Quaternion::Inverse() const {
        float inv = 1.0f / (x * x + y * y + z * z + w * w);
        Quaternion result;
        result.x = -(inv * x);
        result.y = -(inv * y);
        result.z = -(inv * z);
        result.w = w * inv;
        return result;
    }

    void Quaternion::Lerp(const Quaternion &a, const Quaternion &b, float t) {
        float32x4_t av = vld1q_f32(&a.x);
        float32x4_t bv = vld1q_f32(&b.x);
        float32x4_t delta = vsubq_f32(bv, av);
        float32x4_t result = vmlaq_f32(av, vdupq_n_f32(t), delta);

        vst1q_f32(&x, result);
        *this = Normalized();
    }

    void Quaternion::Lerp(const float *a, const float *b, float t) {
        float32x4_t av = vld1q_f32(a);
        float32x4_t bv = vld1q_f32(b);

        float dot = vgetq_lane_f32(av, 0) * vgetq_lane_f32(bv, 0);
        dot += vgetq_lane_f32(av, 1) * vgetq_lane_f32(bv, 1);
        dot += vgetq_lane_f32(av, 2) * vgetq_lane_f32(bv, 2);
        dot += vgetq_lane_f32(av, 3) * vgetq_lane_f32(bv, 3);

        float32x4_t delta = (dot < 0.0f)
                                ? vsubq_f32(vnegq_f32(bv), av)
                                : vsubq_f32(bv, av);

        float32x4_t result = vmlaq_f32(av, vdupq_n_f32(t), delta);
        vst1q_f32(&x, result);
        *this = Normalized();
    }

    void Quaternion::Convert(AEMath::Matrix &matrix) {
        float qx = x;
        float qy = y;
        float qz = z;
        float qw = w;
        double xx = qx * qx;
        double yy = qy * qy;
        double zz = qz * qz;
        double ww = qw * qw;
        double inv = 1.0 / (xx + yy + zz + ww);
        double xy_plus = (double) (qx * qy) + (double) (qw * qz);
        double xy_minus = (double) (qx * qy) - (double) (qw * qz);
        double xz_minus = (double) (qx * qz) - (double) (qw * qy);
        double xz_plus = (double) (qw * qy) + (double) (qx * qz);
        double yz_plus = (double) (qw * qx) + (double) (qy * qz);
        double yz_minus = (double) (qy * qz) - (double) (qw * qx);
        matrix.m[0] = (((xx - yy) - zz) + ww) * inv;
        matrix.m[1] = (xy_plus + xy_plus) * inv;
        matrix.m[2] = (xz_minus + xz_minus) * inv;
        matrix.m[4] = (xy_minus + xy_minus) * inv;
        matrix.m[5] = (((yy - xx) - zz) + ww) * inv;
        matrix.m[6] = (yz_plus + yz_plus) * inv;
        matrix.m[8] = (xz_plus + xz_plus) * inv;
        matrix.m[9] = (yz_minus + yz_minus) * inv;
        matrix.m[10] = ((-xx - yy) + zz + ww) * inv;
    }
}
