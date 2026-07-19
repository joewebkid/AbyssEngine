#ifndef GOF2_QUATERNION_H
#define GOF2_QUATERNION_H
#include "Matrix.h"
#include "Vector.h"

#include "engine/math/AEMath.h"


namespace AbyssEngine {
    class Quaternion {
    public:
        float x, y, z, w;

        Quaternion();

        Quaternion(float x_, float y_, float z_, float w_);

        Quaternion(AEMath::Vector angles);

        Quaternion(Quaternion *other);

        Quaternion(const AEMath::Matrix &matrix);

        ~Quaternion();

        void Set(AEMath::Vector angles);

        void Set(const AEMath::Matrix &matrix);

        void Set(float xa, float ya, float za);

        Quaternion Inverse() const;

        static float Dot(const Quaternion &a, const Quaternion &b);

        void Lerp(const Quaternion &a, const Quaternion &b, float t);

        void Lerp(const float *a, const float *b, float t);

        Quaternion Normalized();

        float Length() const;

        void Convert(AEMath::Matrix &matrix);

        operator float *();

        operator const float *() const;

        float &operator[](int i);

        float operator[](int i) const;
    };

    static_assert(sizeof(Quaternion) == 0x10, "Quaternion layout");
    static_assert(__builtin_offsetof(Quaternion, x) == 0x0, "Quaternion::x offset");
    static_assert(__builtin_offsetof(Quaternion, y) == 0x4, "Quaternion::y offset");
    static_assert(__builtin_offsetof(Quaternion, z) == 0x8, "Quaternion::z offset");
    static_assert(__builtin_offsetof(Quaternion, w) == 0xc, "Quaternion::w offset");
}

#endif
