#ifndef GOF2_TRANSFORM_H
#define GOF2_TRANSFORM_H
#include "engine/core/Array.h"
#include "engine/math/BSphere.h"
#include "../core/AEString.h"
#include "Quaternion.h"
#include "engine/core/KeyFrame.h"

#include "engine/math/AEMath.h"
#include "engine/render/Camera.h"

#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"
namespace AbyssEngine { 
    class Camera;
    class KeyFrame;
 }


using uint = uint32_t;
using longlong = int64_t;
using ulonglong = uint64_t;

namespace AbyssEngine {
    class Mesh;

    class Transform;

    enum AnimationMode {
        AnimationMode_0 = 0
    };

    int CameraIsSphereinViewFrustum(const AEMath::Vector &center, float radius, Camera *camera);

    class Transform {
    public:
        // Layout matches the binary (Ghidra-validated via Transform::Transform ctor @0x846a0
        // and MatrixGetGL). AEMath::Matrix is 0x3c, Quaternion 0x10, Vector/Array<T> 0xc.
        AEMath::Matrix worldMatrix;          // 0x00
        Array<Mesh *> meshes;                // 0x3c
        int id;                              // 0x48
        Array<Transform *> children;         // 0x4c

        union {
            int flags;                       // 0x58
            int renderMode;
        };

        AEMath::Matrix rotationMatrix;       // 0x5c
        AEMath::Matrix localMatrix;          // 0x98
        AEMath::Vector boundingCenter;       // 0xd4
        float boundingRadius;                // 0xe0
        float boundingRadius2;               // 0xe4
        int animationStart;                  // 0xe8
        bool visible;                        // 0xec
        bool animating;                      // 0xed
        float animationSpeed;                // 0xf0
        int color;                           // 0xf4 (packed RGBA from alpha)
        longlong animationLength;            // 0xf8
        longlong rangeStart;                 // 0x100
        longlong rangeEnd;                   // 0x108
        longlong currentTime;                // 0x110
        int currentKeyFrameIndex;            // 0x118
        Array<KeyFrame *> keyFrames;         // 0x11c
        Quaternion rotation;                 // 0x128
        AEMath::Vector translation;          // 0x138
        AEMath::Vector scale;                // 0x144
        Quaternion localRotation;            // 0x150
        AEMath::Vector localTranslation;     // 0x160
        AEMath::Vector localScale;           // 0x16c
        float alpha;                         // 0x178
        bool vfcEnabled;                     // 0x17c
        bool keyFramesShared;                // 0x17d

        AEMath::BSphere &bounds() { return *(AEMath::BSphere *) &boundingCenter; }

        Transform();

        Transform(Transform *other);

        ~Transform();

        void SetCurrentAnimationTime(longlong value);

        void SetAnimationRangeInKeyFrames(int first, int last);

        void SetVisible(bool value);

        void SetVFCFlag(bool value);

        void PauseAnimationWithKeyFrame(int index);

        void PauseAnimationWithTimeStamp(longlong time);

        void SetAnimationSpeed(float value);

        void CollectAnimationData();

        void InitAnimationRangeInTime();

        int IsRunning();

        void UpdateKeyFrames(KeyFrame *keyFrame, int index);

        void SetAnimationLength(longlong value);

        void SetAnimationRangeInTime(longlong start, longlong end);

        void InsertKeyFrame(KeyFrame *keyFrame, int index);

        void InsertKeyFrame(unsigned int channel, float value);

        void InsertKeyFrame(const float *values, longlong flags, int time);

        void InsertKeyFrame_old(const float *values, longlong flags, int time);

        void Update(longlong time, bool updateBounds);

        void InternUpdate(longlong time, bool updateBounds);

        int DebugOut(int value);

        void SetAnimationState(AnimationMode, void *);

        void AddKeyFrame(const AEMath::Vector &a, const AEMath::Vector &b,
                         const AEMath::Vector &c, const AEMath::Vector &d,
                         const AEMath::Vector &e, const AEMath::Vector &f,
                         int time);

        int InCameraVF(AEMath::Matrix *matrix, Camera *camera);
    };

#ifdef GOF2_MATCH  // 32-bit layout offsets only hold in the match build (native is 64-bit: 8-byte pointers)
    static_assert(__builtin_offsetof(Transform, meshes) == 0x3c, "Transform::meshes");
    static_assert(__builtin_offsetof(Transform, id) == 0x48, "Transform::id");
    static_assert(__builtin_offsetof(Transform, children) == 0x4c, "Transform::children");
    static_assert(__builtin_offsetof(Transform, flags) == 0x58, "Transform::flags");
    static_assert(__builtin_offsetof(Transform, rotationMatrix) == 0x5c, "Transform::rotationMatrix");
    static_assert(__builtin_offsetof(Transform, localMatrix) == 0x98, "Transform::localMatrix");
    static_assert(__builtin_offsetof(Transform, boundingCenter) == 0xd4, "Transform::boundingCenter");
    static_assert(__builtin_offsetof(Transform, animationLength) == 0xf8, "Transform::animationLength");
    static_assert(__builtin_offsetof(Transform, rangeStart) == 0x100, "Transform::rangeStart");
    static_assert(__builtin_offsetof(Transform, currentKeyFrameIndex) == 0x118, "Transform::currentKeyFrameIndex");
    static_assert(__builtin_offsetof(Transform, keyFrames) == 0x11c, "Transform::keyFrames");
    static_assert(__builtin_offsetof(Transform, rotation) == 0x128, "Transform::rotation");
    static_assert(__builtin_offsetof(Transform, alpha) == 0x178, "Transform::alpha");
    static_assert(sizeof(Transform) == 0x17e, "Transform size");
#endif
}
#endif
