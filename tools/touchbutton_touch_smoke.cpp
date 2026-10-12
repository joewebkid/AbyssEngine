// Fixture constructors/services surround unchanged TouchButton touch methods.
#include "game/ui/TouchButton.h"
#include "game/ui/Layout.h"
#include "game/core/Globals.h"
#include "engine/audio/FModSound.h"
#include "engine/render/PaintCanvas.h"
#include <cassert>
#include <cstdio>
#include <vector>

Layout *Globals::layout = nullptr;
FModSound *Globals::sound = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
unsigned int Globals::font = 5;

AbyssEngine::String::String() : data(nullptr), length(0) {}
AbyssEngine::String::~String() = default;
Layout::Layout() { field_0x38 = 0; drawColor = -1; }
Layout::~Layout() = default;
FModSound::FModSound() = default;
FModSound::~FModSound() = default;
AbyssEngine::AEMath::Matrix::Matrix() = default;
PaintCanvas::PaintCanvas(Engine *value) { engine = value; }
PaintCanvas::~PaintCanvas() = default;

struct ImageCall { unsigned int id; int x, y; };
struct RegionCall { unsigned int id; int width, height, x, y; };
struct TextCall { unsigned int font; int x, y; };
static std::vector<ImageCall> images;
static std::vector<RegionCall> regions;
static std::vector<TextCall> texts;
static std::vector<unsigned int> colors;
static unsigned int currentColor = 0x12345678;
static short currentSpacing = 2;

unsigned int PaintCanvas::GetColor() { return currentColor; }
void PaintCanvas::SetColor(unsigned int color) { currentColor = color; colors.push_back(color); }
void PaintCanvas::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    SetColor((unsigned(r) << 24) | (unsigned(g) << 16) | (unsigned(b) << 8) | a);
}
int PaintCanvas::FontGetSpacing(unsigned int) { return currentSpacing; }
void PaintCanvas::FontSetSpacing(unsigned int, short spacing) { currentSpacing = spacing; }
int PaintCanvas::GetImage2DWidth(unsigned int) { return 10; }
int PaintCanvas::GetImage2DHeight(unsigned int) { return 20; }
int PaintCanvas::GetTextWidth(unsigned int, const String &) { return 8; }
int PaintCanvas::GetTextHeight(unsigned int) { return 8; }
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y) { ::images.push_back({id, x, y}); }
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y, unsigned char, unsigned char) {
    ::images.push_back({id, x, y});
}
void PaintCanvas::DrawString(unsigned int font, const String &, int x, int y, bool) {
    texts.push_back({font, x, y});
}
void PaintCanvas::DrawRegion2D(unsigned int id, int, int, int width, int height, float unused,
                               int, int, int x, int y) {
    assert(unused == 0.0f);
    regions.push_back({id, width, height, x, y});
}

static bool imageAt(unsigned int id, int x, int y) {
    for (const ImageCall &call : images)
        if (call.id == id && call.x == x && call.y == y) return true;
    return false;
}
static bool sawColor(unsigned int color) {
    for (unsigned int seen : colors) if (seen == color) return true;
    return false;
}
static void clearDraws() {
    images.clear(); regions.clear(); texts.clear(); colors.clear();
    currentColor = 0x12345678;
    currentSpacing = 2;
}

static int sounds[8], soundCount;
void FModSound::play(int id, Vector *pos, Vector *vel, float pitch) {
    assert(pos == nullptr && vel == nullptr && pitch == 0.0f);
    assert(soundCount < 8);
    sounds[soundCount++] = id;
}

// This constructor only establishes object lifetime and touch geometry.
TouchButton::TouchButton(unsigned int buttonKind, int px, int py, int buttonWidth, unsigned char)
    : kind(static_cast<int>(buttonKind)), x(px), y(py), layoutHeight(20), width(buttonWidth),
      touchMargin(0), touched(0), visible(1), halfTransparent(0) {
    fontId = 9; fontSpacing = 3; subId = -1; adornImage = -1;
    imgFrameL = 10; imgFrameM = 11; imgFrameR = 12;
    imgFrameTL = 13; imgFrameT = 14; imgFrameTR = 15;
    imgFrameBL = 16; imgFrameB = 17; imgFrameBR = 18;
    height = 20; leftWidth = 10; midStretch = 20; rightWidth = 10;
    textOffsetX = 3; textOffsetY = 4; textColor = 0x11223344;
    pressProgress = 0.0f; progressHighlight = 1; alwaysPressed = 0;
    iconSmall = -1; iconOverlay = -1;
}
TouchButton::~TouchButton() = default;

int main() {
    Layout layout;
    FModSound sound;
    PaintCanvas canvas(nullptr);
    Globals::layout = &layout;
    Globals::sound = &sound;
    Globals::Canvas = &canvas;

    layout.field_0x38 = 4;
    TouchButton inset(3, 100, 50, 40, 0);
    assert(!inset.touchedInside(102, 60));
    assert(inset.touchedInside(103, 49));
    assert(inset.touchedInside(136, 71));
    assert(!inset.touchedInside(137, 60));
    assert(!inset.touchedInside(103, 72));

    assert(inset.OnTouchBegin(103, 60) && inset.touched && sounds[0] == 0x7c);
    assert(inset.OnTouchMove(136, 60) && inset.touched);
    assert(inset.OnTouchEnd(136, 60) == 1 && !inset.touched);
    assert(soundCount == 2 && sounds[1] == 0x7b);

    assert(inset.OnTouchBegin(103, 60));
    assert(!inset.OnTouchMove(137, 60) && !inset.touched);
    assert(!inset.OnTouchEnd(103, 60) && soundCount == 3);
    inset.halfTransparent = 1;
    assert(!inset.OnTouchBegin(103, 60) && soundCount == 3);
    inset.halfTransparent = 0;
    inset.visible = 0;
    assert(!inset.OnTouchBegin(103, 60) && soundCount == 3);

    TouchButton ordinary(2, 100, 50, 40, 0);
    ordinary.touchMargin = 3;
    assert(ordinary.touchedInside(96, 46));
    assert(ordinary.touchedInside(143, 74));
    assert(!ordinary.touchedInside(95, 60));
    assert(!ordinary.touchedInside(144, 60));
    assert(!ordinary.touchedInside(100, 75));

    clearDraws();
    layout.drawBGPattern(21, 4, 5, 17, 23);
    assert(imageAt(21, 4, 5) && regions.size() == 3);
    assert(regions[0].width == 7 && regions[0].height == 20 && regions[0].x == 14 && regions[0].y == 5);
    assert(regions[1].width == 10 && regions[1].height == 3 && regions[1].x == 4 && regions[1].y == 25);
    assert(regions[2].width == 7 && regions[2].height == 3 && regions[2].x == 14 && regions[2].y == 25);

    clearDraws();
    ordinary.draw();
    assert(imageAt(11, 110, 50) && imageAt(11, 120, 50));
    assert(imageAt(12, 130, 50) && imageAt(10, 100, 50));
    assert(texts.size() == 1 && texts[0].font == 9 && texts[0].x == 103 && texts[0].y == 54);
    assert(regions.empty() && currentColor == 0x12345678 && currentSpacing == 2);

    clearDraws();
    ordinary.pressProgress = 0.5f;
    ordinary.draw();
    assert(regions.size() == 1 && regions[0].id == 13);
    assert(regions[0].width == 10 && regions[0].height == 20);
    assert(regions[0].x == 100 && regions[0].y == 50);
    assert(imageAt(14, 110, 50) && sawColor(0xffffff80));

    clearDraws();
    ordinary.pressProgress = 0.9f;
    ordinary.draw();
    assert(regions.size() == 2 && regions[1].id == 15);
    assert(regions[1].width == 6 && regions[1].x == 130 && regions[1].y == 50);

    clearDraws();
    TouchButton overlay(16, 100, 50, 40, 0);
    overlay.touched = 1;
    overlay.draw();
    assert(images.size() == 2 && imageAt(10, 100, 50) && imageAt(13, 100, 50));

    clearDraws();
    TouchButton medal(4, 100, 50, 40, 0);
    medal.iconImage = 31;
    medal.iconSmall = 32;
    medal.iconOverlay = 33;
    medal.iconTexId = 0xaabbccdd;
    medal.touched = 1;
    medal.draw();
    assert(imageAt(31, 100, 50) && imageAt(32, 120, 59) && imageAt(33, 100, 50));
    assert(sawColor(0xaabbccdd) && currentColor == 0x12345678);

    std::puts("TouchButton/Layout host smoke: OK");
}
