#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace AbyssEngine {
namespace AEMath {
struct Matrix {
    float m[15];
    Matrix() : m{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1} {}
    Matrix(float p0, float p1, float p2, float p3, float p4,
           float p5, float p6, float p7, float p8, float p9,
           float p10, float p11, float p12, float p13, float p14)
        : m{p0, p1, p2, p3, p4, p5, p6, p7, p8, p9,
            p10, p11, p12, p13, p14} {}
    Matrix &operator*=(const Matrix &rhs) {
        // The initialized-branch fixture camera has identity rotation.
        std::memcpy(m, rhs.m, sizeof(m));
        return *this;
    }
};
Matrix MatrixIdentity(Matrix &matrix) { matrix = Matrix(); return matrix; }
float Sinf(float value) { return std::sin(value); }
float Cosf(float value) { return std::cos(value); }
Matrix MatrixGetInverse(const Matrix &matrix) {
    Matrix result;
    result.m[0] = matrix.m[0]; result.m[1] = matrix.m[4];
    result.m[4] = matrix.m[1]; result.m[5] = matrix.m[5];
    result.m[3] = -matrix.m[3]; result.m[7] = -matrix.m[7];
    result.m[11] = -matrix.m[11];
    return result;
}
}
using AEMath::Matrix;

struct Engine {
    double grav[2] = {0.0, 0.0};
    double *GetGravValue() { return grav; }
};
struct SpriteSystem {};
struct PCCameraView {
    unsigned param0;
    float param1, param2;
    float localMatrix[15];
    float factor48, factor4c;
};
static unsigned drawCount = 0;
static Matrix lastView, lastWorld;
static SpriteSystem *lastSys = nullptr;
void SpriteSystemDraw(Engine *, const Matrix &view, const Matrix &world, SpriteSystem *sys) {
    ++drawCount;
    lastView = view;
    lastWorld = world;
    lastSys = sys;
}

struct PaintCanvas {
    Engine *engine;
    struct { unsigned count; PCCameraView **data_; } cameras;
    struct { unsigned count; SpriteSystem **data_; } spriteSystems;
    unsigned currentCamera;
    unsigned char initialized;
    int gameOrientation;
    void DrawSpriteSystem(unsigned index);
    void DrawSpriteSystem(unsigned index, Matrix mat);
    void DrawSpriteSystem(unsigned index, Matrix matA, Matrix matB);
};

// PRODUCTION_METHOD
// PRODUCTION_TWO_MATRICES
// PRODUCTION_ONE_MATRIX
}

int main() {
    using namespace AbyssEngine;
    Engine engine;
    SpriteSystem system;
    SpriteSystem *systems[] = {&system};
    Matrix identity;
    PCCameraView camera = {};
    std::memcpy(camera.localMatrix, identity.m, sizeof(identity.m));
    PCCameraView *cameras[] = {&camera};
    PaintCanvas canvas = {&engine, {0, cameras}, {1, systems}, 0, 0, 0};

    canvas.DrawSpriteSystem(1);
    assert(drawCount == 0);
    systems[0] = nullptr;
    canvas.DrawSpriteSystem(0);
    assert(drawCount == 0);
    systems[0] = &system;
    canvas.DrawSpriteSystem(0);
    assert(drawCount == 1 && lastSys == &system);
    assert(lastWorld.m[0] == 1 && lastWorld.m[3] == 0 && lastView.m[5] == 1);

    canvas.cameras.count = 1;
    camera.localMatrix[3] = 2.0f;
    canvas.DrawSpriteSystem(0);
    assert(drawCount == 2 && lastWorld.m[3] == -2.0f);

    camera.localMatrix[3] = 0.0f;
    canvas.initialized = 1;
    canvas.gameOrientation = 1;
    engine.grav[1] = 0.5;
    canvas.DrawSpriteSystem(0);
    assert(drawCount == 3 && lastWorld.m[1] > 0.7f && lastWorld.m[1] < 0.72f);
    canvas.gameOrientation = 0;
    canvas.DrawSpriteSystem(0);
    assert(drawCount == 4 && lastWorld.m[1] < -0.7f && lastWorld.m[1] > -0.72f);

    Matrix first, second;
    first.m[3] = 3.0f;
    second.m[3] = 2.0f;
    canvas.initialized = 0;
    canvas.DrawSpriteSystem(0, first, second);
    assert(drawCount == 5 && lastView.m[3] == 3.0f && lastWorld.m[3] == -2.0f);
    canvas.initialized = 1;
    engine.grav[1] = 0.5;
    canvas.DrawSpriteSystem(0, identity, identity);
    assert(drawCount == 6 && lastView.m[1] == 0.0f && lastWorld.m[1] == 0.0f);
    engine.grav[1] = 1.0;
    canvas.DrawSpriteSystem(0, identity, identity);
    assert(drawCount == 7 && lastView.m[1] > 0.83f && lastWorld.m[1] < -0.83f);
    canvas.gameOrientation = 1;
    canvas.DrawSpriteSystem(0, identity, identity);
    assert(drawCount == 8 && lastView.m[1] < -0.83f && lastWorld.m[1] > 0.83f);

    Matrix one;
    one.m[3] = 4.0f;
    canvas.initialized = 0;
    canvas.DrawSpriteSystem(1, one);
    assert(drawCount == 8);
    systems[0] = nullptr;
    canvas.DrawSpriteSystem(0, one);
    assert(drawCount == 8);
    systems[0] = &system;
    canvas.DrawSpriteSystem(0, one);
    assert(drawCount == 9 && lastView.m[0] == 1.0f && lastWorld.m[3] == -4.0f);
    canvas.initialized = 1;
    engine.grav[1] = 0.5;
    canvas.DrawSpriteSystem(0, identity);
    assert(drawCount == 10 && lastWorld.m[1] > 0.7f && lastWorld.m[1] < 0.72f);
    canvas.gameOrientation = 0;
    canvas.DrawSpriteSystem(0, identity);
    assert(drawCount == 11 && lastWorld.m[1] < -0.7f && lastWorld.m[1] > -0.72f);
    std::puts("PaintCanvas DrawSpriteSystem smoke: OK");
}
