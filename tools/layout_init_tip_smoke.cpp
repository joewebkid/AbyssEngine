#include "game/ui/Layout.h"
#include "game/core/Globals.h"
#include "engine/core/AERandom.h"
#include "engine/core/GameText.h"
#include <cassert>
#include <cstdio>
#include <cstring>

static int releasedStrings = 0;
static int randomCalls = 0;
static int randomBound = -1;
static int textKey = -1;
static int lineCalls = 0;
static unsigned int lineFont = 0;
static int lineWidth = -1;
static Array<String *> *lineTarget = nullptr;

Layout *Globals::layout = nullptr;
void *Globals::rnd = nullptr;
GameText *Globals::gameText = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
unsigned int Globals::font = 0;

AbyssEngine::String::String() : data(nullptr), length(0) {}
AbyssEngine::String::String(const char *text, bool) : data(nullptr), length(0) { Set(text); }
AbyssEngine::String::~String() { delete[] data; ++releasedStrings; }
void AbyssEngine::String::Set(const char *text) {
    length = text ? int(std::strlen(text)) : 0;
    data = new unsigned short[length + 1];
    for (int i = 0; i < length; ++i) data[i] = static_cast<unsigned char>(text[i]);
    data[length] = 0;
}

AbyssEngine::AERandom::AERandom() : seedLow(0), seedHigh(0) {}
AbyssEngine::AERandom::~AERandom() = default;
int AbyssEngine::AERandom::nextInt(int bound) {
    ++randomCalls;
    randomBound = bound;
    return 4;
}

GameText::GameText() : textTable(nullptr), textCount(0) {}
GameText::~GameText() = default;
String *GameText::getText(int key) {
    static String tip("tip");
    textKey = key;
    return &tip;
}

AbyssEngine::AEMath::Matrix::Matrix() = default;
PaintCanvas::PaintCanvas(Engine *value) { engine = value; }
PaintCanvas::~PaintCanvas() = default;
void PaintCanvas::GetLineArray(unsigned int font, const String &text, int width,
                               Array<String *> *out) {
    ++lineCalls;
    lineFont = font;
    lineWidth = width;
    lineTarget = out;
    assert(text.length == 3);
    out->data_[0] = new String("line");
    out->size_ = 1;
}

Layout::Layout() { tipLines = nullptr; }
Layout::~Layout() = default;

int main() {
    Layout layout;
    GameText gameText;
    AERandom random;
    PaintCanvas canvas(nullptr);
    Globals::layout = &layout;
    Globals::gameText = &gameText;
    Globals::rnd = &random;
    Globals::Canvas = &canvas;
    Globals::font = 77;
    layout.field_0x78 = 320;
    layout.field_0x4c = 20;

    layout.initTip();
    assert(randomCalls == 1 && randomBound == 13 && textKey == 344);
    assert(lineCalls == 1 && lineFont == 77 && lineWidth == 280);
    assert(lineTarget == layout.tipLines && layout.tipLines->size() == 1);
    assert(releasedStrings == 0);

    layout.initTip();
    assert(randomCalls == 2 && lineCalls == 2);
    assert(lineTarget == layout.tipLines && layout.tipLines->size() == 1);
    assert(releasedStrings == 1);

    ArrayReleaseClasses(*layout.tipLines);
    delete layout.tipLines;
    layout.tipLines = nullptr;
    std::puts("Layout initTip host smoke: OK");
}
