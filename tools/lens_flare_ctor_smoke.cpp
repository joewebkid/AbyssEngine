#include "engine/render/LensFlare.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"

#include <cassert>

using AbyssEngine::Engine;
using AbyssEngine::PaintCanvas;

PaintCanvas *Globals::Canvas = nullptr;
static PaintCanvas *sizeCanvas;
static unsigned short loaded[3];
static int loadCount;

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
}
