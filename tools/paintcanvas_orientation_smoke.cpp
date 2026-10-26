#include <cassert>
#include <cmath>
#include <cstring>

namespace AbyssEngine {
enum LandscapeMode : int { LandscapeMode_0, LandscapeMode_1, LandscapeMode_2, LandscapeMode_3 };

float i_fov, i_zNear, i_zFar;

namespace AEMath {
float Sinf(float value) { return std::sin(value); }
float Cosf(float value) { return std::cos(value); }
}

struct Engine {
    unsigned width = 1920, height = 1080;
    LandscapeMode lastOrientation = LandscapeMode_0;
    static float farPlane, nearPlane;
    unsigned GetDisplayWidth() { return width; }
    unsigned GetDisplayHeight() { return height; }
    void SetScreenOrientation(LandscapeMode orientation) { lastOrientation = orientation; }
};
float Engine::farPlane, Engine::nearPlane;

struct Matrix { float m[16] = {}; };
struct CameraArray { void **data_ = nullptr; };

struct PaintCanvas {
    int gameOrientation = -1;
    Engine *engine;
    Matrix projMatrix3d, projOrthoMatrix, worldViewMatrix;
    CameraArray cameras;
    unsigned currentCamera = -1;
    explicit PaintCanvas(Engine *value) : engine(value) {}
    void Initialize(bool landscape);
    void SetGameOrientation(LandscapeMode orientation);
    void SetProjectionMatrix3d(float fov, float nearPlane, float farPlane);
};
}

using namespace AbyssEngine;

// PRODUCTION_METHODS

static void near(float actual, float expected) {
    assert(std::fabs(actual - expected) < 1e-6f);
}

static void checkMatrix(const Matrix &matrix, const float expected[16]) {
    for (int i = 0; i < 16; ++i) near(matrix.m[i], expected[i]);
}

static void checkOrtho(const PaintCanvas &canvas, int orientation) {
    const float width = 1920.0f, height = 1080.0f;
    float expected[16] = {};
    expected[0] = 2.0f / ((orientation == 0 || orientation == 1) ? height : width);
    expected[5] = -2.0f / ((orientation == 0 || orientation == 1) ? width : height);
    expected[10] = -0.05f;
    expected[12] = -1.0f;
    expected[13] = 1.0f;
    expected[15] = 1.0f;
    checkMatrix(canvas.projOrthoMatrix, expected);
}

static void checkWorld(const PaintCanvas &canvas, int orientation) {
    float expected[16] = {};
    expected[10] = expected[15] = 1.0f;
    if (orientation == 0) {
        expected[1] = -1.0f; expected[4] = 1.0f; expected[13] = 1920.0f;
    } else if (orientation == 1) {
        expected[1] = 1.0f; expected[4] = -1.0f; expected[12] = 1080.0f;
    } else if (orientation == 2) {
        expected[0] = expected[5] = 1.0f;
    } else {
        expected[0] = expected[5] = -1.0f;
        expected[12] = 1920.0f; expected[13] = 1080.0f;
    }
    checkMatrix(canvas.worldViewMatrix, expected);
}

int main() {
    Engine engine;
    PaintCanvas portrait(&engine);
    portrait.Initialize(false);
    assert(portrait.gameOrientation == 0 && engine.lastOrientation == LandscapeMode_0);
    checkOrtho(portrait, 0);
    checkWorld(portrait, 0);
    near(portrait.projMatrix3d.m[11], -1.0f);
    near(Engine::nearPlane, 512.0f);
    near(Engine::farPlane, 65536.0f);
    near(i_fov, 16384.0f);

    PaintCanvas landscape(&engine);
    landscape.Initialize(true);
    assert(landscape.gameOrientation == 2 && engine.lastOrientation == LandscapeMode_2);
    checkOrtho(landscape, 2);

    for (int orientation = 0; orientation != 4; ++orientation) {
        PaintCanvas canvas(&engine);
        for (float &value : canvas.projOrthoMatrix.m) value = 7.0f;
        for (float &value : canvas.worldViewMatrix.m) value = 7.0f;
        canvas.SetGameOrientation(static_cast<LandscapeMode>(orientation));
        assert(canvas.gameOrientation == orientation && engine.lastOrientation == orientation);
        checkOrtho(canvas, orientation);
        checkWorld(canvas, orientation);
    }

    PaintCanvas cameraCanvas(&engine);
    float camera[] = {1.2f, 0.5f, 100.0f};
    void *cameraSlot = camera;
    cameraCanvas.cameras.data_ = &cameraSlot;
    cameraCanvas.currentCamera = 0;
    cameraCanvas.SetGameOrientation(LandscapeMode_3);
    near(i_fov, 1.2f);
    near(i_zNear, 0.5f);
    near(i_zFar, 100.0f);
    near(cameraCanvas.projMatrix3d.m[11], -1.0f);
}
