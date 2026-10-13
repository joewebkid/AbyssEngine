#include <cassert>
#include <cmath>
#include <cstdio>

namespace AbyssEngine {
namespace AEMath {
struct Vector { float x, y, z; };
struct Matrix {
    float m[15];
    Matrix(float p0, float p1, float p2, float p3, float p4,
           float p5, float p6, float p7, float p8, float p9,
           float p10, float p11, float p12, float p13, float p14)
        : m{p0, p1, p2, p3, p4, p5, p6, p7, p8, p9,
            p10, p11, p12, p13, p14} {}
};
Matrix MatrixIdentity(Matrix &matrix) { return matrix; }
float Sinf(float angle) { return std::sin(angle); }
float Cosf(float angle) { return std::cos(angle); }
}
using AEMath::Vector;
using AEMath::Matrix;
struct Camera {};
struct Engine {
    double gravity[2] = {0.0, 0.5};
    int reads = 0;
    double *GetGravValue() { ++reads; return gravity; }
};
static int calls = 0;
static Camera *lastCamera = nullptr;
static const Vector *lastPoint = nullptr;
static float lastRadius = 0.0f;
static bool hadRotation = false;
static float rotation01 = 0.0f;
int CameraIsSphereinViewFrustum(const Vector &point, float radius,
                                Matrix *extra, Camera *camera) {
    ++calls;
    lastCamera = camera;
    lastPoint = &point;
    lastRadius = radius;
    hadRotation = extra != nullptr;
    if (extra) rotation01 = extra->m[1];
    return 7;
}
static int pointCalls = 0;
static bool pointHadRotation = false;
static float pointRotation01 = 0.0f;
int CameraIsPointinViewFrustum(const Vector &point, Matrix *extra, Camera *camera) {
    ++pointCalls;
    assert(&point == lastPoint && camera == lastCamera);
    pointHadRotation = extra != nullptr;
    if (extra) pointRotation01 = extra->m[1];
    return 9;
}
struct PaintCanvas {
    unsigned char initialized;
    struct { unsigned count; Camera **data_; } cameras;
    unsigned currentCamera;
    int gameOrientation;
    Engine *engine;
    int CameraIsSphereinViewFrustum(const Vector &point, float radius);
    int CameraIsPointinViewFrustum(const Vector &point);
};

// PRODUCTION_METHOD
// PRODUCTION_POINT_METHOD
}

int main() {
    using namespace AbyssEngine;
    Engine engine;
    Camera camera;
    Camera *cameras[] = {&camera};
    PaintCanvas canvas = {0, {1, cameras}, 0, 1, &engine};
    Vector point = {1.0f, 2.0f, 3.0f};
    assert(canvas.CameraIsSphereinViewFrustum(point, 0.0f) == 1 && calls == 0);
    canvas.currentCamera = 1;
    assert(canvas.CameraIsSphereinViewFrustum(point, 2.0f) == 1 && calls == 0);
    canvas.currentCamera = 0;
    assert(canvas.CameraIsSphereinViewFrustum(point, 2.0f) == 7);
    assert(calls == 1 && !hadRotation && engine.reads == 0);
    assert(lastCamera == &camera && lastPoint == &point && lastRadius == 2.0f);
    canvas.initialized = 1;
    assert(canvas.CameraIsSphereinViewFrustum(point, 2.0f) == 7);
    assert(calls == 2 && hadRotation && engine.reads == 1);
    assert(rotation01 < -0.70f && rotation01 > -0.72f);
    canvas.gameOrientation = 0;
    assert(canvas.CameraIsSphereinViewFrustum(point, 2.0f) == 7);
    assert(calls == 3 && rotation01 > 0.70f && rotation01 < 0.72f);
    canvas.currentCamera = 1;
    assert(canvas.CameraIsPointinViewFrustum(point) == 1 && pointCalls == 0);
    canvas.currentCamera = 0;
    canvas.initialized = 0;
    assert(canvas.CameraIsPointinViewFrustum(point) == 9);
    assert(pointCalls == 1 && !pointHadRotation && engine.reads == 2);
    canvas.initialized = 1;
    canvas.gameOrientation = 1;
    assert(canvas.CameraIsPointinViewFrustum(point) == 9);
    assert(pointCalls == 2 && pointHadRotation && engine.reads == 3);
    assert(pointRotation01 < -0.70f && pointRotation01 > -0.72f);
    canvas.gameOrientation = 0;
    assert(canvas.CameraIsPointinViewFrustum(point) == 9);
    assert(pointCalls == 3 && pointRotation01 > 0.70f && pointRotation01 < 0.72f);
    std::puts("PaintCanvas frustum wrappers smoke: OK");
}
