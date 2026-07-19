#ifndef GOF2_CAMERA_H
#define GOF2_CAMERA_H
#include <cstdint>

#include "engine/math/Matrix.h"

#include "engine/math/AEMath.h"


namespace AbyssEngine {
    class Camera {
    public:
        float position[3];        // 0x00: [0] fov, [1] nearClip, [2] farClip
        AEMath::Matrix projection; // 0x0c

        // Frustum parameters computed by CameraSetPerspective (offsets 0x48..0x5c).
        float frustumTanHalfFov;  // 0x48
        float field_0x4c;         // 0x4c
        float frustumAspect;      // 0x50
        float frustumInvCosX;     // 0x54
        float frustumInvCosY;     // 0x58
        float field_0x5c;         // 0x5c

        Camera(float fov, float aspect, float nearPlane, float farPlane, float param5);
    };
}

#endif
