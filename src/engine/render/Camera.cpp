#include "engine/render/Camera.h"

namespace AbyssEngine {
    float CameraSetPerspective(float fov, float aspect, float nearPlane, float farPlane, float param5,
                               Camera *cam);

    Camera::Camera(float fov, float aspect, float nearPlane, float farPlane, float param5) {
        CameraSetPerspective(fov, aspect, nearPlane, farPlane, param5, this);
    }
}
