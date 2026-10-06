#ifndef GOF2_MATRIX_H
#define GOF2_MATRIX_H

namespace AbyssEngine {
    namespace AEMath {
        struct Matrix {
            union {
                float m[15];

                struct {
                    float e0, e1, e2, e3;
                    float m11_rightY, m12_upY, e6, e7;
                    float e8, e9, e10, e11;
                    float e12, e13, e14;
                };
            };

            Matrix();

            constexpr Matrix(float p0, float p1, float p2, float p3,
                             float p4, float p5, float p6, float p7,
                             float p8, float p9, float p10, float p11,
                             float p12, float p13, float p14)
                : m{p0, p1, p2, p3, p4, p5, p6, p7,
                    p8, p9, p10, p11, p12, p13, p14} {}


            operator float *();

            operator const float *() const;

            float &operator[](int i) { return m[i]; }
            float operator[](int i) const { return m[i]; }

            Matrix &operator=(const Matrix &o);

            Matrix &operator*=(const Matrix &o);
        };
    }

    using AEMath::Matrix;

    static_assert(__builtin_offsetof(AEMath::Matrix, m) == 0x00, "AEMath::Matrix::m offset");
    static_assert(__builtin_offsetof(AEMath::Matrix, m11_rightY) == 0x10, "AEMath::Matrix::m11_rightY offset");
    static_assert(__builtin_offsetof(AEMath::Matrix, m12_upY) == 0x14, "AEMath::Matrix::m12_upY offset");
    static_assert(sizeof(AEMath::Matrix) == 0x3c, "AEMath::Matrix size");
}

using ::AbyssEngine::Matrix;

#endif
