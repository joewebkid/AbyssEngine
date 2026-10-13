#include <cassert>
#include <cmath>
#include <cstdio>

namespace AbyssEngine {
namespace AEMath {
struct Matrix {
    float m[15];
    Matrix(float p0, float p1, float p2, float p3, float p4,
           float p5, float p6, float p7, float p8, float p9,
           float p10, float p11, float p12, float p13, float p14)
        : m{p0, p1, p2, p3, p4, p5, p6, p7, p8, p9,
            p10, p11, p12, p13, p14} {}
    Matrix &operator*=(const Matrix &rhs) {
        float x = m[0], y = m[1], u = m[4], v = m[5];
        m[0] = x * rhs.m[0] + y * rhs.m[4];
        m[1] = x * rhs.m[1] + y * rhs.m[5];
        m[4] = u * rhs.m[0] + v * rhs.m[4];
        m[5] = u * rhs.m[1] + v * rhs.m[5];
        return *this;
    }
};
Matrix MatrixIdentity(Matrix &matrix) { return matrix; }
Matrix MatrixGetInverse(const Matrix &matrix) {
    Matrix result = matrix;
    result.m[1] = matrix.m[4];
    result.m[4] = matrix.m[1];
    result.m[3] = -matrix.m[3];
    return result;
}
float Sinf(float value) { return std::sin(value); }
float Cosf(float value) { return std::cos(value); }
}
using AEMath::Matrix;
struct Camera {
    Matrix projection;
};
static int frustumCalls = 0;
static bool frustumPass = true;
static bool hadRotation = false;
static float frustumRotation01 = 0.0f;
static Camera *lastCamera = nullptr;
struct Transform {
    bool visible = true;
    int InCameraVF(Matrix *extra, Camera *camera) {
        ++frustumCalls;
        hadRotation = extra != nullptr;
        if (extra) frustumRotation01 = extra->m[1];
        lastCamera = camera;
        return frustumPass;
    }
};
struct Engine {
    double gravity[2] = {0.0, 0.5};
    int gravityReads = 0;
    int eyeCalls = 0;
    float eyeX = 0.0f, eyeY = 0.0f, eyeZ = 0.0f;
    double *GetGravValue() { ++gravityReads; return gravity; }
    void SetEyePosition(float x, float y, float z) {
        ++eyeCalls;
        eyeX = x; eyeY = y; eyeZ = z;
    }
};
static int drawCalls = 0;
static Transform *lastTransform = nullptr;
static Matrix lastView(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1);
static Matrix lastWorld(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1);
struct PaintCanvas {
    unsigned transformCount;
    char **transforms;
    struct { unsigned count; Camera **data_; } cameras;
    unsigned currentCamera;
    unsigned char initialized;
    int gameOrientation;
    Engine *engine;
    int culledCount;
    void DrawTransform(unsigned index, const Matrix *viewMatrix);
    void DrawTransform(Transform *tf, const Matrix &view, Matrix &world) {
        ++drawCalls;
        lastTransform = tf;
        lastView = view;
        lastWorld = world;
    }
};

// PRODUCTION_METHOD
}

int main() {
    using namespace AbyssEngine;
    Matrix identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1);
    Camera camera{identity};
    camera.projection.m[3] = 3.0f;
    camera.projection.m[7] = 4.0f;
    camera.projection.m[11] = 5.0f;
    Camera *cameras[] = {&camera};
    Transform transform;
    char *transforms[] = {reinterpret_cast<char *>(&transform)};
    Engine engine;
    PaintCanvas canvas = {1, transforms, {0, cameras}, 0, 0, 1, &engine, 0};

    canvas.DrawTransform(1, nullptr);
    assert(drawCalls == 0);
    transform.visible = false;
    canvas.DrawTransform(0, nullptr);
    assert(drawCalls == 0);
    transform.visible = true;
    canvas.DrawTransform(0, nullptr);
    assert(drawCalls == 1 && frustumCalls == 0 && engine.eyeCalls == 0);
    assert(lastTransform == &transform && lastView.m[0] == 1.0f && lastWorld.m[3] == 0.0f);

    canvas.cameras.count = 1;
    canvas.DrawTransform(0, nullptr);
    assert(drawCalls == 2 && frustumCalls == 1 && !hadRotation && lastCamera == &camera);
    assert(lastWorld.m[3] == -3.0f && engine.eyeCalls == 1);
    assert(engine.eyeX == 3.0f && engine.eyeY == 4.0f && engine.eyeZ == 5.0f);
    Matrix supplied = identity;
    supplied.m[3] = 7.0f;
    canvas.DrawTransform(0, &supplied);
    assert(drawCalls == 3 && lastWorld.m[3] == -7.0f && engine.eyeX == 3.0f);
    frustumPass = false;
    canvas.DrawTransform(0, nullptr);
    assert(drawCalls == 3 && canvas.culledCount == 1 && engine.eyeCalls == 2);
    frustumPass = true;

    canvas.initialized = 1;
    canvas.DrawTransform(0, &identity);
    assert(drawCalls == 4 && hadRotation && engine.gravityReads == 1);
    assert(frustumRotation01 < -0.70f && frustumRotation01 > -0.72f);
    assert(lastWorld.m[1] > 0.70f && lastWorld.m[1] < 0.72f);
    canvas.gameOrientation = 0;
    canvas.DrawTransform(0, &identity);
    assert(drawCalls == 5 && frustumRotation01 > 0.70f && frustumRotation01 < 0.72f);
    assert(lastWorld.m[1] < -0.70f && lastWorld.m[1] > -0.72f);
    canvas.DrawTransform(0, nullptr);
    assert(drawCalls == 6 && lastWorld.m[3] == -3.0f);
    frustumPass = false;
    canvas.DrawTransform(0, nullptr);
    assert(drawCalls == 6 && canvas.culledCount == 2 && engine.eyeCalls == 5);
    std::puts("PaintCanvas DrawTransform smoke: OK");
}
