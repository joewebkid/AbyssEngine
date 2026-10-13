#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace AbyssEngine {
namespace AEMath {
struct Vector {
    float x, y, z;
    float &operator[](int i) { return (&x)[i]; }
};
struct Matrix {
    float m[15];
    Matrix() : m{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1} {}
    Matrix(float p0, float p1, float p2, float p3, float p4,
           float p5, float p6, float p7, float p8, float p9,
           float p10, float p11, float p12, float p13, float p14)
        : m{p0, p1, p2, p3, p4, p5, p6, p7, p8, p9,
            p10, p11, p12, p13, p14} {}
    Matrix &operator*=(const Matrix &rhs) {
        // The fixture camera matrix is identity, so its product is rhs.
        std::memcpy(m, rhs.m, sizeof(m));
        return *this;
    }
};
Matrix MatrixIdentity(Matrix &matrix) { matrix = Matrix(); return matrix; }
float Sinf(float value) { return std::sin(value); }
float Cosf(float value) { return std::cos(value); }
Vector MatrixTransformVector(const Matrix &m, const Vector &v) {
    return {v.x * m.m[0] + v.y * m.m[1] + v.z * m.m[2] + m.m[3],
            v.x * m.m[4] + v.y * m.m[5] + v.z * m.m[6] + m.m[7],
            v.x * m.m[8] + v.y * m.m[9] + v.z * m.m[10] + m.m[11]};
}
Vector MatrixInverseTransformVector(const Matrix &m, const Vector &v) {
    const float x = v.x - m.m[3], y = v.y - m.m[7], z = v.z - m.m[11];
    return {x * m.m[0] + y * m.m[4] + z * m.m[8],
            x * m.m[1] + y * m.m[5] + z * m.m[9],
            x * m.m[2] + y * m.m[6] + z * m.m[10]};
}
}
using AEMath::Matrix;
using AEMath::Vector;

struct Engine {
    double grav[2] = {0.0, 0.0};
    double *GetGravValue() { return grav; }
};
struct PCCameraView {
    unsigned param0;
    float param1, param2;
    float localMatrix[15];
    float factor48, factor4c;
};
struct PaintCanvas {
    Engine *engine;
    struct { unsigned count; PCCameraView **data_; } cameras;
    unsigned currentCamera;
    unsigned char initialized;
    int gameOrientation;
    int width, height;
    int GetWidth() { return width; }
    int GetHeight() { return height; }
    int GetScreenPosition(const Vector &worldPos, Vector &outVec);
    bool GetScreenPosition(Matrix &m, const Vector &worldPos, Vector &outVec);
};

// PRODUCTION_METHOD
// PRODUCTION_WRAPPER
}

int main() {
    using namespace AbyssEngine;
    Engine engine;
    Matrix identity;
    PCCameraView camera = {};
    std::memcpy(camera.localMatrix, identity.m, sizeof(identity.m));
    camera.param1 = 2.0f;
    camera.factor48 = camera.factor4c = 1.0f;
    PCCameraView *cameraSlots[] = {&camera};
    PaintCanvas canvas = {&engine, {1, cameraSlots}, 0, 0, 0, 100, 100};
    Vector result = {99, 99, 99};
    Matrix world;
    world.m[3] = 1.0f;
    assert(canvas.GetScreenPosition(world, {0, 0, 1}, result));
    assert(std::fabs(result.x) < 0.001f && std::fabs(result.y - 50.0f) < 0.001f);
    assert(canvas.GetScreenPosition(Vector{0, 0, 1}, result) == 1);
    assert(std::fabs(result.x - 50.0f) < 0.001f);
    camera.localMatrix[3] = 1.0f;
    assert(canvas.GetScreenPosition(world, {0, 0, 1}, result));
    assert(std::fabs(result.x - 50.0f) < 0.001f);
    camera.localMatrix[3] = 0.0f;

    canvas.cameras.count = 0;
    result = {99, 99, 99};
    assert(!canvas.GetScreenPosition(world, {0, 0, 1}, result));
    assert(result.x == 99 && result.z == 99);
    canvas.cameras.count = 1;
    camera.param1 = 0.5f;
    assert(!canvas.GetScreenPosition(world, {0, 0, 1}, result));
    assert(result.x == 1 && result.z == 1);
    camera.param1 = 2.0f;
    camera.factor4c = 0.0f;
    assert(!canvas.GetScreenPosition(world, {0, 0, 1}, result));
    camera.factor4c = 1.0f;
    assert(!canvas.GetScreenPosition(identity, {2, 0, 1}, result));
    assert(result.x < 0.0f);
    assert(canvas.GetScreenPosition(identity, {0, -2, 1}, result));
    assert(result.y < 0.0f); // Original clips only the upper Y bound.

    canvas.initialized = 1;
    canvas.gameOrientation = 1;
    engine.grav[1] = 0.5;
    assert(canvas.GetScreenPosition(identity, {1, 0, 1}, result));
    assert(result.x > 10.0f && result.x < 20.0f);
    assert(result.y > 10.0f && result.y < 20.0f);
    canvas.gameOrientation = 0;
    assert(canvas.GetScreenPosition(identity, {1, 0, 1}, result));
    assert(result.y > 80.0f && result.y < 90.0f);
    std::puts("GetScreenPosition world-matrix smoke: OK");
}
