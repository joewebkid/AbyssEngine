#ifndef GOF2_VECTOR_H
#define GOF2_VECTOR_H

namespace AbyssEngine {
    namespace AEMath {
        struct Vector {
            float x, y, z;

            operator float *();

            operator const float *() const;

            float &operator[](int i);

            float operator[](int i) const;

            Vector &operator=(const Vector &o);

            Vector &operator+=(const Vector &o);

            Vector &operator-=(const Vector &o);

            Vector &operator*=(float s);

            Vector &operator*=(const Vector &o);

            Vector &operator/=(float s);

            Vector &operator/=(const Vector &o);
        };
    }

    using AEMath::Vector;

    static_assert(sizeof(AEMath::Vector) == 0x0c, "AEMath::Vector size");
    static_assert(__builtin_offsetof(AEMath::Vector, x) == 0x00, "AEMath::Vector::x offset");
    static_assert(__builtin_offsetof(AEMath::Vector, y) == 0x04, "AEMath::Vector::y offset");
    static_assert(__builtin_offsetof(AEMath::Vector, z) == 0x08, "AEMath::Vector::z offset");
}

using ::AbyssEngine::Vector;

#endif
