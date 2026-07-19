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
