#include "game/world/StarSystem.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"

#include <cassert>

using AbyssEngine::AEMath::Matrix;
using AbyssEngine::AEMath::Vector;
using AbyssEngine::Engine;
using AbyssEngine::PaintCanvas;

PaintCanvas *Globals::Canvas = nullptr;
PaintCanvas *PaintCanvas::gCanvas = nullptr;
static Matrix cameraLocal;
static bool projectedVisible;
static int cameraCalls, projectionCalls, updateCalls, flareCalls;
static Vector projectionInput;
static Vector flarePosition;
static int flareColor;

Matrix::Matrix() : m{} {}
Vector &Vector::operator=(const Vector &other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}
PaintCanvas::PaintCanvas(Engine *) {}
PaintCanvas::~PaintCanvas() = default;
unsigned int PaintCanvas::CameraGetCurrent() {
    assert(this == Globals::Canvas);
    ++cameraCalls;
    return 7;
}
void *PaintCanvas::CameraGetLocal(unsigned int index) {
    assert(this == Globals::Canvas && index == 7);
    return &cameraLocal;
}
int PaintCanvas::GetScreenPosition(const Vector &input, Vector &output) {
    assert(this == Globals::Canvas);
    ++projectionCalls;
    projectionInput = input;
    output = {123.0f, 456.0f, -7.0f};
    return projectedVisible;
}
LensFlare::LensFlare(PaintCanvas *) {}
LensFlare::~LensFlare() = default;
void LensFlare::update(int dt) {
    assert(dt == 0);
    ++updateCalls;
}
void LensFlare::render2D(float x, float y, float z, int color) {
    ++flareCalls;
    flarePosition = {x, y, z};
    flareColor = color;
}
StarSystem::StarSystem(int) {}
StarSystem::~StarSystem() = default;

int main() {
    PaintCanvas canvas(nullptr), wrongCanvas(nullptr);
    Globals::Canvas = &canvas;
    PaintCanvas::gCanvas = &wrongCanvas;
    cameraLocal.e3 = 10.0f;
    cameraLocal.e7 = 20.0f;
    cameraLocal.e11 = 30.0f;

    StarSystem star(0);
    star.lensFlare = nullptr;
    star.render2D();
    assert(cameraCalls == 0 && updateCalls == 0);

    LensFlare flare(&canvas);
    star.lensFlare = &flare;
    star.lightDirection = {1.0f, 2.0f, 3.0f};
    star.tintColor = 5;
    projectedVisible = false;
    star.render2D();
    assert(cameraCalls == 1 && projectionCalls == 1 &&
           updateCalls == 1 && flareCalls == 0);
    assert(projectionInput.x == 65546.0f && projectionInput.y == 131092.0f &&
           projectionInput.z == 196638.0f);

    projectedVisible = true;
    star.render2D();
    assert(cameraCalls == 2 && projectionCalls == 2 &&
           updateCalls == 2 && flareCalls == 1);
    assert(flarePosition.x == 123.0f && flarePosition.y == 456.0f &&
           flarePosition.z == -7.0f && flareColor == 5);
}
