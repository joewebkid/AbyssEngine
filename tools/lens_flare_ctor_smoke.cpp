#include "engine/render/LensFlare.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"

#include <cassert>
#include <cmath>
#include <limits>

using AbyssEngine::Engine;
using AbyssEngine::PaintCanvas;

PaintCanvas *Globals::Canvas = nullptr;
void *Globals::globals = nullptr;
static PaintCanvas *sizeCanvas;
static unsigned short loaded[3];
static int loadCount;
struct DrawCall { unsigned int image; int x, y, w, h; };
static DrawCall draws[7];
static int drawCount;
static unsigned char alphas[5];
static unsigned char rgb[5][3];
static int colorCount;
static unsigned int packedColors[2];
static int packedCount;
static int fillCount;

Globals::Globals() {}
Globals::~Globals() = default;
float Globals::sqrt(float value) { return std::sqrt(value); }

AbyssEngine::AEMath::Matrix::Matrix() : m{} {}
PaintCanvas::PaintCanvas(Engine *) {}
PaintCanvas::~PaintCanvas() = default;
void PaintCanvas::Image2DCreate(unsigned short id, unsigned int &out) {
    assert(this == Globals::Canvas && loadCount < 3);
    loaded[loadCount++] = id;
    out = id + 100;
}
int PaintCanvas::GetWidth() {
    assert(this == sizeCanvas);
    return 800;
}
int PaintCanvas::GetHeight() {
    assert(this == sizeCanvas);
    return 600;
}
int PaintCanvas::GetImage2DWidth(unsigned int id) {
    assert(this == Globals::Canvas);
    if (id == 1388) return 40;
    if (id == 1389) return 20;
    assert(id == 1390);
    return 10;
}
void PaintCanvas::SetColor(unsigned char r, unsigned char g, unsigned char b,
                           unsigned char alpha) {
    assert(this == Globals::Canvas && colorCount < 5);
    rgb[colorCount][0] = r;
    rgb[colorCount][1] = g;
    rgb[colorCount][2] = b;
    alphas[colorCount++] = alpha;
}
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y,
                              unsigned char align, unsigned char place) {
    assert(this == Globals::Canvas && align == 0x11 && place == 0x44);
    assert(drawCount < 7);
    draws[drawCount++] = {id, x, y, -1, -1};
}
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y, int w, int h,
                              unsigned char align, unsigned char place,
                              unsigned char flip) {
    assert(this == Globals::Canvas && align == 0x11 && place == 0x44 && flip == 0);
    assert(drawCount < 7);
    draws[drawCount++] = {id, x, y, w, h};
}
unsigned int PaintCanvas::GetColor() {
    assert(this == sizeCanvas);
    return 0x11223344;
}
void PaintCanvas::SetColor(unsigned int color) {
    assert(this == sizeCanvas && packedCount < 2);
    packedColors[packedCount++] = color;
}
void PaintCanvas::FillRectangle(int x, int y, int w, int h) {
    assert(this == sizeCanvas && x == 0 && y == 0 && w == 800 && h == 600);
    ++fillCount;
}

int main() {
    PaintCanvas resourceCanvas(nullptr);
    PaintCanvas inputCanvas(nullptr);
    Globals::Canvas = &resourceCanvas;
    sizeCanvas = &inputCanvas;
    LensFlare flare(&inputCanvas);
    assert(loadCount == 3);
    assert(loaded[0] == 1288 && loaded[1] == 1289 && loaded[2] == 1290);
    assert(flare.images[0] == 1388 && flare.images[1] == 1389 &&
           flare.images[2] == 1390);
    assert(flare.canvas == &inputCanvas && flare.width == 800 && flare.height == 600);

    Globals globals;
    Globals::globals = &globals;
    flare.render2D(400.0f, 300.0f, -1.0f, 0);
    assert(flare.intensity == 64.0f && drawCount == 7 && colorCount == 5);
    const unsigned int expectedImages[7] = {1388, 1388, 1389, 1388, 1389, 1390, 1390};
    const int expectedWidths[7] = {-1, 30, 10, 50, 10, 20, 5};
    for (int i = 0; i < 7; ++i) {
        assert(draws[i].image == expectedImages[i] &&
               draws[i].x == 400 && draws[i].y == 300 &&
               draws[i].w == expectedWidths[i] && draws[i].h == expectedWidths[i]);
    }
    assert(alphas[0] == 134 && alphas[1] == 104 &&
           alphas[2] == 134 && alphas[3] == 134 && alphas[4] == 134);
    assert(fillCount == 1 && packedCount == 2 &&
           packedColors[0] == 0xc8c8ff40 && packedColors[1] == 0x11223344);

    flare.render2D(-800.0f, 300.0f, -1.0f, 0);
    flare.render2D(400.0f, 300.0f, 1.0f, 0);
    flare.render2D(400.0f, 300.0f, std::numeric_limits<float>::quiet_NaN(), 0);
    assert(drawCount == 7 && fillCount == 1);

    drawCount = colorCount = packedCount = fillCount = 0;
    flare.render2D(300.0f, 250.0f, -1.0f, 5);
    assert(std::fabs(flare.intensity - 50.185f) < 0.01f);
    assert(drawCount == 7 && colorCount == 5 && fillCount == 1);
    const int expectedX[7] = {350, 375, 325, 387, 390, 475, 420};
    const int expectedY[7] = {275, 287, 262, 293, 295, 337, 310};
    for (int i = 0; i < 7; ++i) {
        assert(std::abs(draws[i].x - expectedX[i]) <= 1 &&
               std::abs(draws[i].y - expectedY[i]) <= 1);
    }
    for (int i = 0; i < 5; ++i)
        assert(rgb[i][0] == 255 && rgb[i][1] == 255 && rgb[i][2] == 255);
}
