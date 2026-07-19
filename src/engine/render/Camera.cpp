#include "engine/render/Camera.h"

#include <new>

namespace AbyssEngine {
    float CameraSetPerspective(float fov, float aspect, float nearPlane, float farPlane, float param5,
                               Camera *cam);

    Camera::Camera(float fov, float aspect, float nearPlane, float farPlane, float param5) {
        new(&this->projection) AEMath::Matrix();
        CameraSetPerspective(fov, aspect, nearPlane, farPlane, param5, this);
    }
}
