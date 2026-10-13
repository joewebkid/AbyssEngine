// Fixture constructors/services surround unchanged TouchButton touch methods.
#include "game/ui/TouchButton.h"
#include "game/ui/Layout.h"
#include "game/ui/ChoiceWindow.h"
#include "game/core/Globals.h"
#include "engine/core/GameText.h"
#include "engine/audio/FModSound.h"
#include "engine/render/PaintCanvas.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

Layout *Globals::layout = nullptr;
FModSound *Globals::sound = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
GameText *Globals::gameText = nullptr;
unsigned int Globals::font = 5;
int Globals::w = 480;
int Globals::h = 320;
unsigned char Globals::iPad = 0;
unsigned char Globals::iPadHD = 0;
unsigned char Globals::iPadLarge = 0;
unsigned char Globals::n9 = 0;
unsigned char Globals::retinaDisplay = 0;
Status *Globals::status = nullptr;
int Globals::other_buttons_x[10] = {};
int Globals::other_buttons_y[10] = {};
static int formatLanguage = 0;
static int lastTextKey = -1;
int GameText::getLanguage() { return formatLanguage; }
GameText::GameText() = default;
GameText::~GameText() = default;
String *GameText::getText(int key) {
    lastTextKey = key;
    static String help("HELP"), reward("REWARD"), bonus("BONUS");
    if (key == 0x187) return &help;
    if (key == 216) return &reward;
    if (key == 3206) return &bonus;
    assert(false);
    return nullptr;
}

AbyssEngine::String::String() : data(nullptr), length(0) {}
AbyssEngine::String::String(const char *text, bool) : data(nullptr), length(0) { Set(text); }
AbyssEngine::String::String(const String &other, bool reverse) : data(nullptr), length(0) {
    assert(!reverse);
    Set(other.data);
}
AbyssEngine::String::String(int value) : data(nullptr), length(0) {
    Set(std::to_string(value).c_str());
}
AbyssEngine::String::~String() { delete[] data; }
void AbyssEngine::String::Set(const char *text) {
    const int size = text ? int(std::strlen(text)) : 0;
    auto *next = new unsigned short[size + 1];
    for (int i = 0; i < size; ++i) next[i] = (unsigned char) text[i];
    next[size] = 0;
    delete[] data;
    data = next;
    length = size;
}
void AbyssEngine::String::Set(const unsigned short *text) {
    int size = 0;
    if (text) while (text[size]) ++size;
    auto *next = new unsigned short[size + 1];
    for (int i = 0; i < size; ++i) next[i] = text[i];
    next[size] = 0;
    delete[] data;
    data = next;
    length = size;
}
namespace AbyssEngine {
String operator+(const String &left, const String &right) {
    String joined;
    joined.length = left.length + right.length;
    joined.data = new unsigned short[joined.length + 1];
    for (int i = 0; i < left.length; ++i) joined.data[i] = left.data[i];
    for (int i = 0; i < right.length; ++i) joined.data[left.length + i] = right.data[i];
    joined.data[joined.length] = 0;
    return joined;
}
}
Ship::Ship(int, int, int load, int, int, int, int, int, float)
    : baseLoad(load), currentLoad(0), cargoPlus(0) {}
Ship::~Ship() = default;
int Ship::getCurrentLoad() { return currentLoad; }
int Ship::getMaxLoad() { return baseLoad + cargoPlus; }
Status::Status() : ship(nullptr), credits(0) {}
Status::~Status() = default;
Ship *Status::getShip() { return ship; }
int Status::getCredits() { return credits; }
Layout::~Layout() = default;
static int choiceConstructs, choiceDraws;
static std::string choiceTitle, choiceMessage;
ChoiceWindow::ChoiceWindow() { ++choiceConstructs; }
ChoiceWindow::~ChoiceWindow() = default;
void ChoiceWindow::draw() { ++choiceDraws; }
void ChoiceWindow::set(String const &title, String const &message, bool hasButtons) {
    assert(!hasButtons);
    choiceTitle.clear(); choiceMessage.clear();
    for (int i = 0; i < title.length; ++i) choiceTitle.push_back(char(title.data[i]));
    for (int i = 0; i < message.length; ++i) choiceMessage.push_back(char(message.data[i]));
}
FModSound::FModSound() = default;
FModSound::~FModSound() = default;
AbyssEngine::AEMath::Matrix::Matrix() = default;
PaintCanvas::PaintCanvas(Engine *value) { engine = value; }
PaintCanvas::~PaintCanvas() = default;

struct ImageCall { unsigned int id; int x, y; unsigned char flagsA, flagsB; };
struct SizedImageCall { unsigned int id; int x, y, width, height; unsigned char align, anchor, flip; };
struct RegionCall { unsigned int id; int width, height, x, y; float angle; };
struct TextCall { unsigned int font; int x, y; std::string content; };
struct FillCall { int x, y, width, height; unsigned int color; };
struct OutlineCall { int x, y, width, height; unsigned int color; };
static std::vector<ImageCall> images;
static std::vector<SizedImageCall> sizedImages;
static std::vector<RegionCall> regions;
static std::vector<TextCall> texts;
static std::vector<FillCall> fills;
static std::vector<OutlineCall> outlines;
static std::vector<unsigned int> colors;
static std::vector<unsigned int> imageWidthQueries;
static std::vector<unsigned int> imageHeightQueries;
static unsigned int currentColor = 0x12345678;
static short currentSpacing = 2;

unsigned int PaintCanvas::GetColor() { return currentColor; }
void PaintCanvas::SetColor(unsigned int color) { currentColor = color; colors.push_back(color); }
void PaintCanvas::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    SetColor((unsigned(r) << 24) | (unsigned(g) << 16) | (unsigned(b) << 8) | a);
}
int PaintCanvas::FontGetSpacing(unsigned int) { return currentSpacing; }
void PaintCanvas::FontSetSpacing(unsigned int, short spacing) { currentSpacing = spacing; }
int PaintCanvas::GetImage2DWidth(unsigned int id) {
    imageWidthQueries.push_back(id);
    return 10;
}
int PaintCanvas::GetImage2DHeight(unsigned int id) {
    imageHeightQueries.push_back(id);
    return 20;
}
int PaintCanvas::GetTextWidth(unsigned int, const String &) { return 8; }
int PaintCanvas::GetTextHeight(unsigned int) { return 8; }
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y) { ::images.push_back({id, x, y, 0, 0}); }
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y, unsigned char flags) {
    ::images.push_back({id, x, y, flags, 0});
}
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y, unsigned char a, unsigned char b) {
    ::images.push_back({id, x, y, a, b});
}
void PaintCanvas::DrawImage2D(unsigned int id, int x, int y, int width, int height,
                              unsigned char align, unsigned char anchor, unsigned char flip) {
    sizedImages.push_back({id, x, y, width, height, align, anchor, flip});
}
void PaintCanvas::DrawString(unsigned int font, const String &value, int x, int y, bool) {
    std::string content;
    for (int i = 0; i < value.length; ++i) content.push_back(char(value.data[i]));
    texts.push_back({font, x, y, content});
}
void PaintCanvas::FillRectangle(int x, int y, int width, int height) {
    fills.push_back({x, y, width, height, currentColor});
}
void PaintCanvas::DrawRectangle(int x, int y, int width, int height) {
    outlines.push_back({x, y, width, height, currentColor});
}
void PaintCanvas::DrawRegion2D(unsigned int id, int, int, int width, int height, float angle,
                               int, int, int x, int y) {
    regions.push_back({id, width, height, x, y, angle});
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
    images.clear(); sizedImages.clear(); regions.clear(); texts.clear(); fills.clear(); outlines.clear(); colors.clear();
    imageWidthQueries.clear(); imageHeightQueries.clear();
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
    {
        Layout ordinary;
        assert(ordinary.field_0x4 == 15 && ordinary.field_0x58 == 243);
        assert(ordinary.rewardBoxWidth == 75 && ordinary.rewardBoxHeight == 196);
        assert(ordinary.field_0x238 == 50 && ordinary.helpWindowY == 76);
        assert(ordinary.field_0x70_rowHeight == 30 && ordinary.field_0x8c == 32);
        assert(ordinary.field_0x90 == 0x3f266666 && ordinary.field_0x94 == 10);
        assert(ordinary.field_0x98 == 420);
        assert(ordinary.field_0x9c == 50);
        assert(ordinary.field_0x12c == 15 && ordinary.field_0x130 == 40 &&
               ordinary.field_0x134 == 80 && ordinary.field_0x138 == 35);
        assert(ordinary.field_0x13c == 17 && ordinary.field_0x140 == 11 &&
               ordinary.field_0x144 == 13 && ordinary.field_0x148 == 2);
        assert(ordinary.centerYBase == 15 && ordinary.marqueeWidth == 20 &&
               ordinary.progressBarYBase == 37 && ordinary.field_0xdc == 10.0f);
        assert(ordinary.animSpeedScale == 1.0f && ordinary.driftSpeedDivisor == 20.0f &&
               ordinary.ringRadiusScale == 1.0f);
        assert(ordinary.ringNearMidThreshold == 40 && ordinary.ringMidFarThreshold == 200 &&
               ordinary.hudInset == 5 && ordinary.oreLabelYOffset == 2 && ordinary.oreTextYOffset == 3);
        Globals::retinaDisplay = 1;
        Layout retina;
        assert(retina.field_0x4 == 30 && retina.field_0x58 == 486);
        assert(retina.rewardBoxWidth == 150 && retina.rewardBoxHeight == 392);
        assert(retina.field_0x70_rowHeight == 60 && retina.field_0x8c == 64);
        assert(retina.field_0x90 == 0x3ea66666 && retina.field_0x94 == 20);
        assert(retina.field_0x98 == 840);
        assert(retina.field_0x9c == 100);
        assert(retina.field_0x12c == 30 && retina.field_0x130 == 80 &&
               retina.field_0x134 == 160 && retina.field_0x138 == 70);
        assert(retina.field_0x13c == 34 && retina.field_0x140 == 22 &&
               retina.field_0x144 == 26 && retina.field_0x148 == 4);
        assert(retina.centerYBase == 30 && retina.marqueeWidth == 40 &&
               retina.progressBarYBase == 74 && retina.field_0xdc == 20.0f);
        assert(retina.animSpeedScale == 2.0f && retina.driftSpeedDivisor == 10.0f &&
               retina.ringRadiusScale == 2.0f);
        assert(retina.ringNearMidThreshold == 80 && retina.ringMidFarThreshold == 400 &&
               retina.hudInset == 10 && retina.oreLabelYOffset == 4 && retina.oreTextYOffset == 5);
        Globals::retinaDisplay = 0;
        Globals::n9 = 1;
        Layout n9;
        assert(n9.field_0x4 == 22 && n9.field_0x58 == 364);
        assert(n9.rewardBoxWidth == 112 && n9.rewardBoxHeight == 352);
        assert(n9.field_0x70_rowHeight == 51 && n9.field_0x8c == 57);
        assert(n9.field_0x90 == 0x3ea66666 && n9.field_0x94 == 15);
        assert(n9.field_0x98 == 630);
        assert(n9.field_0x9c == 75);
        assert(n9.field_0x12c == 22 && n9.field_0x130 == 60 &&
               n9.field_0x134 == 120 && n9.field_0x138 == 52);
        assert(n9.field_0x13c == 25 && n9.field_0x140 == 19 &&
               n9.field_0x144 == 19 && n9.field_0x148 == 19);
        assert(n9.centerYBase == 22 && n9.marqueeWidth == 36 &&
               n9.progressBarYBase == 55 && n9.field_0xdc == 15.0f);
        assert(n9.animSpeedScale == 1.5f && n9.driftSpeedDivisor == 12.0f &&
               n9.ringRadiusScale == 1.5f);
        assert(n9.ringNearMidThreshold == 60 && n9.ringMidFarThreshold == 300 &&
               n9.hudInset == 0 && n9.oreLabelYOffset == 5 && n9.oreTextYOffset == 4);
        Globals::n9 = 0;
        Globals::iPadHD = 1;
        Layout ipadHd;
        assert(ipadHd.field_0x4 == 21 && ipadHd.field_0x58 == 341);
        assert(ipadHd.rewardBoxWidth == 140 && ipadHd.helpWindowX == 331);
        assert(ipadHd.field_0x70_rowHeight == 63 && ipadHd.field_0x8c == 45);
        assert(ipadHd.field_0x90 == 1055696996 && ipadHd.field_0x94 == 14);
        assert(ipadHd.field_0x98 == 590);
        assert(ipadHd.field_0x9c == 70);
        assert(ipadHd.field_0x12c == 21 && ipadHd.field_0x130 == 78 &&
               ipadHd.field_0x134 == 151 && ipadHd.field_0x138 == 66);
        assert(ipadHd.field_0x13c == 23 && ipadHd.field_0x140 == 15 &&
               ipadHd.field_0x144 == 18 && ipadHd.field_0x148 == 2);
        assert(ipadHd.centerYBase == 21 && ipadHd.marqueeWidth == 28 &&
               ipadHd.progressBarYBase == 52 && ipadHd.field_0xdc == 14.0625f);
        assert(ipadHd.animSpeedScale == 1.40625f && ipadHd.driftSpeedDivisor == 28.125f &&
               ipadHd.ringRadiusScale == 1.40625f);
        assert(ipadHd.ringNearMidThreshold == 56 && ipadHd.ringMidFarThreshold == 281 &&
               ipadHd.hudInset == 7 && ipadHd.oreLabelYOffset == 2 && ipadHd.oreTextYOffset == 5);
        Globals::iPadHD = 0;
        Globals::iPad = 1;
        Layout ipad;
        assert(ipad.rewardBoxWidth == 100 && ipad.field_0x2f4 == 80);
        assert(ipad.field_0x2f8 == 1128792064 && ipad.helpWindowY == 144);
        assert(ipad.field_0x70_rowHeight == 45 && ipad.field_0x90 == 0x3ea66666);
        assert(ipad.field_0x12c == 15 && ipad.field_0x130 == 56 &&
               ipad.field_0x134 == 108 && ipad.field_0x138 == 47);
        assert(ipad.field_0x13c == 17 && ipad.field_0x140 == 11 &&
               ipad.field_0x144 == 13 && ipad.field_0x148 == 2);
        assert(ipad.field_0xdc == 10.0f && ipad.driftSpeedDivisor == 20.0f &&
               ipad.ringRadiusScale == 1.0f);
        Globals::iPadLarge = 1;
        Layout ipadLarge;
        assert(ipadLarge.rewardBoxWidth == 200 && ipadLarge.field_0x2f4 == 160);
        assert(ipadLarge.field_0x2f8 == 1137180672 && ipadLarge.helpWindowY == 288);
        assert(ipadLarge.field_0x70_rowHeight == 90 && ipadLarge.field_0x90 == 0x3ea66666);
        assert(ipadLarge.field_0x12c == 15 && ipadLarge.field_0x130 == 112 &&
               ipadLarge.field_0x134 == 216 && ipadLarge.field_0x138 == 94);
        assert(ipadLarge.field_0x13c == 17 && ipadLarge.field_0x140 == 11 &&
               ipadLarge.field_0x144 == 13 && ipadLarge.field_0x148 == 2);
        assert(ipadLarge.field_0xdc == 10.0f && ipadLarge.driftSpeedDivisor == 20.0f &&
               ipadLarge.ringRadiusScale == 1.0f);
        Globals::iPadLarge = 0;
        Globals::iPad = 0;
    }
    Layout layout;
    layout.drawColor = -1;
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

    clearDraws();
    layout.fading = 0;
    layout.fillScreen = 0;
    assert(layout.drawFade() == 0 && fills.empty());

    layout.fading = 1;
    layout.fadeOut = 1;
    layout.fadeColor = 0x11223300;
    layout.fadeProgress = 25;
    layout.fadeDuration = 100;
    assert(layout.drawFade() == 1 && fills.size() == 1);
    assert(fills[0].x == 0 && fills[0].y == 0);
    assert(fills[0].width == 480 && fills[0].height == 320);
    assert(fills[0].color == 0x1122333f && currentColor == 0x12345678);

    clearDraws();
    layout.fadeOut = 0;
    layout.fillScreen = 1;
    assert(layout.drawFade() == 1 && fills.size() == 2);
    assert(fills[0].color == 0x112233bf && fills[1].color == 0xff);
    assert(fills[1].width == 480 && fills[1].height == 320);
    assert(currentColor == 0x12345678);

    clearDraws();
    layout.fading = 0;
    assert(layout.drawFade() == 0 && fills.size() == 1);
    assert(fills[0].color == 0xff && currentColor == 0x12345678);

    clearDraws();
    layout.fading = 1;
    layout.fillScreen = 0;
    layout.fadeOut = 1;
    layout.fadeProgress = 0;
    layout.fadeDuration = 0;
    assert(layout.drawFade() == 1 && fills.size() == 1);
    assert(fills[0].color == 0x112233ff && currentColor == 0x12345678);

    Ship ship(0, 0, 20, 0, 0, 0, 0, 0, 0.0f);
    Status status;
    ship.currentLoad = 25;
    status.ship = &ship;
    status.credits = 123;
    Globals::status = &status;
    layout.windowX = 4;
    layout.windowY = 5;
    layout.windowWidth = 80;
    layout.windowHeight = 40;
    layout.field_0x10 = 20;
    layout.field_0x74 = 15;
    layout.footerTextInset = 3;
    layout.footerImageLeft = 21;
    layout.footerImageRight = 22;
    layout.field_0x338 = 23;
    layout.drawColor = -1;
    TouchButton back(16, 1, 2, 10, 0);
    TouchButton secondary(16, 2, 3, 10, 0);
    back.imgFrameL = 31;
    secondary.imgFrameL = 32;
    layout.backButton = &back;
    layout.secondaryButton = &secondary;

    layout.field_0x340 = 26;
    layout.footerPatternImage = 24;
    back.visible = 0;
    clearDraws();
    layout.drawEmptyFooter(false);
    assert(!imageWidthQueries.empty() && imageWidthQueries[0] == 26);
    assert(colors[0] == layout.drawColor);
    assert(images.front().id == 21 && images.front().x == 0 && images.front().y == 320);
    assert(images.front().flagsA == 0x11 && images.front().flagsB == 0x21);
    assert(imageAt(24, 10, 300));
    assert(images.back().id == 21 && images.back().x == 470 && images.back().y == 300);
    assert(images.back().flagsA == 1 && !back.visible && !imageAt(31, 1, 2));

    clearDraws();
    layout.drawEmptyFooter(true);
    assert(back.visible && imageAt(31, 1, 2));

    clearDraws();
    layout.drawFooter();
    assert(back.visible && imageAt(31, 1, 2) && !imageAt(32, 2, 3));
    assert(imageAt(21, 4, 45) && imageAt(22, 14, 25));
    assert(images[0].flagsA == 0x11 && images[0].flagsB == 0x21);
    assert(sawColor(0xff2a00ff));
    assert(texts.size() == 2 && texts[0].content == "25 / 20t");
    assert(texts[0].font == 5 && texts[0].x == 40 && texts[0].y == 42);
    assert(texts[1].content == "123$" && texts[1].x == 66 && texts[1].y == 42);

    clearDraws();
    layout.drawFooterNoBackButton();
    assert(!back.visible && !imageAt(31, 1, 2) && !imageAt(32, 2, 3));
    assert(texts.size() == 2 && texts[0].content == "25 / 20t");

    clearDraws();
    ship.currentLoad = 15;
    layout.drawFooterStation();
    assert(!back.visible && !imageAt(31, 1, 2) && imageAt(32, 2, 3));
    assert(!sawColor(0xff2a00ff));
    assert(texts.size() == 2 && texts[0].content == "15 / 20t");
    assert(texts[1].content == "123$" && texts[1].x == 65 && texts[1].y == 42);

    TouchButton help(16, 0, 0, 40, 0);
    layout.helpButton = &help;
    layout.field_0x38 = 4;
    assert(layout.getHelpButtonOffset() == 36);
    Layout otherLayout;
    otherLayout.field_0x38 = 9;
    Globals::layout = &otherLayout;
    assert(layout.getHelpButtonOffset() == 31);
    Globals::layout = &layout;

    clearDraws();
    assert(layout.getFooterTransitionWidth() == 20);
    assert(imageWidthQueries.size() == 2);
    assert(imageWidthQueries[0] == 22 && imageWidthQueries[1] == 21);

    Globals::w = 17;
    Globals::h = 23;
    layout.bgPatternImage = 42;
    clearDraws();
    layout.drawBG();
    assert(imageAt(42, 0, 0) && regions.size() == 3);
    assert(regions[0].x == 10 && regions[0].y == 0);
    assert(regions[2].x == 10 && regions[2].y == 20);

    clearDraws();
    layout.drawMask();
    assert(fills.size() == 1 && fills[0].x == 0 && fills[0].y == 0);
    assert(fills[0].width == 17 && fills[0].height == 23);
    assert(fills[0].color == 0x80 && currentColor == 0x12345678);

    clearDraws();
    layout.drawMask(2, 3, 4, 5);
    assert(fills.size() == 1 && fills[0].x == 2 && fills[0].y == 3);
    assert(fills[0].width == 4 && fills[0].height == 5);
    assert(fills[0].color == 0x80 && currentColor == 0x12345678);
    Globals::w = 480;
    Globals::h = 320;

    layout.field_0x348 = 51;
    layout.field_0x34c = 52;
    layout.field_0x350 = 53;
    layout.field_0x354 = 54;
    layout.field_0x358 = 55;
    layout.field_0x35c = 56;
    layout.field_0x36c = 57;
    layout.field_0x370 = 58;
    layout.field_0x380 = 61;
    layout.field_0x384 = 62;
    layout.field_0x388 = 63;
    layout.field_0x38c = 64;
    layout.field_0x390 = 71;
    layout.field_0x394 = 72;
    layout.field_0x39c = 81;
    layout.field_0x3a0 = 82;
    layout.field_0x360 = 91;
    layout.field_0x364 = 92;
    layout.field_0x368 = 93;
    layout.headerIconImage = 73;
    layout.field_0x44 = 6;
    layout.field_0x1c = 12;
    layout.field_0x5c = 14;
    layout.field_0x8 = 8;
    layout.field_0x28 = 5;
    layout.headerTitleY = 7;
    layout.headerCapImage = 74;
    layout.headerPatternImage = 75;
    layout.textBaselineAdjust = 2;
    String boxText("BOX");

    help.imgFrameL = 97;
    layout.choiceWindowOpen = 0;
    clearDraws();
    layout.drawHeader(String(""), false);
    assert(imageWidthQueries[0] == 74 && imageHeightQueries[0] == 74);
    assert(imageAt(74, 4, 5) && imageAt(75, 14, 5));
    assert(sizedImages.size() == 1 && sizedImages[0].id == 74);
    assert(sizedImages[0].x == 84 && sizedImages[0].y == 5);
    assert(sizedImages[0].width == 10 && sizedImages[0].height == 20);
    assert(sizedImages[0].align == 0x11 && sizedImages[0].anchor == 0x12 && sizedImages[0].flip == 1);
    assert(texts.empty() && !imageAt(97, 0, 0) && !layout.helpButtonEnabled);

    clearDraws();
    layout.drawHeader(String("HEAD"), true);
    assert(imageAt(73, 4, 5) && imageAt(97, 0, 0));
    assert(texts.size() == 1 && texts[0].content == "HEAD" && texts[0].font == 5);
    assert(texts[0].x == 15 && texts[0].y == 12 && layout.helpButtonEnabled);

    layout.choiceWindowOpen = 1;
    clearDraws();
    layout.drawHeader(String(""), true);
    assert(layout.helpButtonEnabled && !imageAt(97, 0, 0));
    layout.choiceWindowOpen = 0;

    clearDraws();
    layout.drawWindow(boxText, 10, 20, 40, 50, true);
    assert(imageAt(42, 10, 28) && imageAt(73, 10, 20));
    assert(outlines.size() == 1 && outlines[0].x == 10 && outlines[0].y == 28);
    assert(outlines[0].width == 40 && outlines[0].height == 42);
    assert(outlines[0].color == 0x255d8dff && currentColor == 0x12345678);
    assert(texts.empty());

    clearDraws();
    layout.drawWindow(boxText, 10, 20, 40, 50, false);
    assert(!imageAt(42, 10, 28) && imageAt(73, 10, 20));
    assert(outlines.size() == 1 && outlines[0].y == 28 && outlines[0].height == 42);
    assert(currentColor == 0x12345678);

    clearDraws();
    layout.drawWindow(String("BOX"), false);
    assert(!imageAt(42, 0, 8) && imageAt(73, 0, 0));
    assert(outlines.size() == 1 && outlines[0].x == 0 && outlines[0].y == 8);
    assert(outlines[0].width == 480 && outlines[0].height == 304);
    assert(currentColor == 0x12345678);

    clearDraws();
    layout.drawWindow(String("BOX"), true);
    assert(imageAt(42, 0, 8) && outlines.size() == 1);
    assert(outlines[0].width == 480 && outlines[0].height == 304);

    clearDraws();
    layout.drawWindow(String("BOX"));
    assert(!imageAt(42, 0, 8) && imageAt(73, 0, 0));
    assert(outlines.size() == 1 && outlines[0].width == 480);
    assert(outlines[0].height == 304 && currentColor == 0x12345678);

    clearDraws();
    layout.drawBGBorder(61, 62, 10, 20, 47, 65, 2, 3);
    assert(images.size() == 8 && regions.size() == 8);
    assert(imageAt(61, 12, 22) && imageAt(61, 45, 63));
    assert(imageAt(62, 23, 23) && imageAt(62, 33, 86));
    assert(regions[0].width == 3 && regions[0].x == 43 && regions[0].y == 23);
    assert(regions[0].angle == 0.0f);
    assert(regions[1].width == 3 && regions[1].x == 46 && regions[1].y == 82);
    assert(regions[1].angle == 3.1416f);
    assert(regions[2].x == 13 && regions[2].y == 53 && regions[2].angle == -1.5708f);
    assert(regions[3].x == 60 && regions[3].y == 43 && regions[3].angle == 1.5708f);
    assert(regions[4].x == 13 && regions[4].y == 73);
    assert(regions[5].x == 60 && regions[5].y == 63);
    assert(regions[6].width == 1 && regions[6].x == 13 && regions[6].y == 64);
    assert(regions[7].width == 1 && regions[7].x == 56 && regions[7].y == 63);

    clearDraws();
    layout.drawBox(0, 10, 20, 40, 20, boxText, 0);
    assert(imageAt(51, 10, 20) && imageAt(51, 40, 20));
    assert(texts.size() == 1 && texts[0].font == 5 && texts[0].content == "BOX");
    assert(texts[0].x == 16 && texts[0].y == 25 && currentColor == 0x12345678);

    clearDraws();
    layout.drawBox(0, 10, 20, 40, 20, boxText, 2);
    assert(texts.size() == 1 && texts[0].x == 36);
    clearDraws();
    layout.drawBox(0, 10, 20, 40, 20, boxText, 4);
    assert(texts.size() == 1 && texts[0].x == 26);

    clearDraws();
    layout.drawBox(1, 10, 20, 40, 20, boxText, 0);
    assert(imageAt(53, 10, 20) && texts.size() == 1);
    assert(texts[0].x == 16 && texts[0].y == 26);
    clearDraws();
    layout.drawBox(6, 10, 20, 40, 20, boxText, 0);
    assert(imageAt(63, 10, 20) && texts.size() == 1);
    assert(texts[0].x == 16 && texts[0].y == 29);

    clearDraws();
    layout.drawBox(5, 10, 20, 40, 37, boxText, 0);
    assert(imageAt(61, 10, 20) && imageAt(61, 40, 37));
    assert(imageAt(62, 20, 20) && imageAt(62, 30, 57));
    clearDraws();
    layout.drawBox(8, 10, 20, 40, 37, boxText, 0);
    assert(imageAt(81, 10, 20) && imageAt(81, 40, 37));
    assert(imageAt(82, 20, 20) && imageAt(82, 30, 57));

    clearDraws();
    layout.drawBox(7, 10, 20, 40, 37, boxText, 0);
    assert(imageAt(71, -10, 8) && imageAt(71, 60, 57));
    assert(imageAt(72, 0, 8) && !regions.empty());
    assert(imageAt(73, 10, 20) && texts.size() == 1);
    assert(texts[0].x == 15 && texts[0].y == 23 && currentColor == 0x12345678);

    clearDraws();
    layout.drawBox(2, 4, 5, 17, 23, boxText);
    assert(imageAt(42, 4, 5) && regions.size() == 3);

    for (const auto &entry : {std::pair<int, unsigned>{3, 55}, {4, 57},
                              {9, 91}, {10, 93}}) {
        clearDraws();
        layout.drawBox(entry.first, 10, 20, 40, 20, String(""), 0);
        assert(imageAt(entry.second, 10, 20) && imageAt(entry.second, 40, 20));
        assert(texts.empty() && currentColor == 0x12345678);
    }

    clearDraws();
    layout.fadeProgress = 200;
    layout.fadeDuration = 100;
    assert(layout.drawFade() == 1 && fills.size() == 1);
    assert(fills[0].color == 0x112233ff && currentColor == 0x12345678);

    layout.footerButtonOffset = 25;
    layout.buttonInsetX = 5;
    layout.resetWindowDimensions();
    assert(layout.windowX == 0 && layout.windowY == 0);
    assert(layout.windowWidth == 480 && layout.windowHeight == 320);
    assert(help.x == 440 && help.y == 0);
    assert(back.x == 5 && back.y == 275);
    assert(secondary.x == 5 && secondary.y == 275);
    assert(Globals::other_buttons_x[0] == 5 && Globals::other_buttons_y[0] == 275);

    auto formatted = [&](int value) {
        String result = layout.formatNumber(value);
        std::string ascii;
        for (int i = 0; i < result.length; ++i) ascii.push_back(char(result.data[i]));
        return ascii;
    };
    formatLanguage = 0;
    assert(formatted(0) == "0" && formatted(123) == "123" && formatted(999) == "999");
    assert(formatted(1000) == "1,000" && formatted(123456) == "123,456");
    assert(formatted(999999) == "999,999" && formatted(1000000) == "1,000,000");
    assert(formatted(-1234567) == "-1,234,567");
    formatLanguage = 1;
    assert(formatted(1234567) == "1.234.567");
    formatLanguage = 10;
    assert(formatted(1234) == "1,234");
    formatLanguage = 11;
    assert(formatted(1234) == "1,234");
    formatLanguage = 12;
    assert(formatted(1234) == "1.234");
    formatLanguage = 0;
    assert(formatted(-2147483647 - 1) == "--2,147,483,648");

    String tagged = layout.tagString(String("SOL"));
    std::string taggedAscii;
    for (int i = 0; i < tagged.length; ++i) taggedAscii.push_back(char(tagged.data[i]));
    assert(taggedAscii == "<c:-8388353>SOL<c:>");

    GameText gameText;
    Globals::gameText = &gameText;
    layout.helpPressedFlag = 1;
    layout.choiceWindow = nullptr;
    int helpSoundStart = soundCount;
    layout.initHelpWindow(String("BODY"));
    assert(choiceConstructs == 1 && layout.choiceWindow != nullptr);
    assert(soundCount == helpSoundStart + 1 && sounds[soundCount - 1] == 0x7e);
    assert(choiceTitle == "HELP" && choiceMessage == "BODY");
    assert(layout.choiceWindowOpen == 1 && layout.helpPressedFlag == 0);
    layout.drawHelpWindow();
    assert(choiceDraws == 1);
    ChoiceWindow *firstChoice = layout.choiceWindow;
    layout.initHelpWindow(String("AGAIN"));
    assert(layout.choiceWindow == firstChoice && choiceConstructs == 1);
    assert(soundCount == helpSoundStart + 2 && sounds[soundCount - 1] == 0x7e);
    assert(choiceMessage == "AGAIN");
    delete layout.choiceWindow;
    layout.choiceWindow = nullptr;

    layout.rewardMessageActive = 0;
    int rewardSoundStart = soundCount;
    layout.showMissionRewardMessage(0, false);
    assert(!layout.rewardMessageActive && soundCount == rewardSoundStart);
    layout.showMissionRewardMessage(250, false);
    assert(layout.rewardMessageActive && !layout.rewardMessageFlag);
    assert(layout.rewardMessageTimer == 0 && layout.rewardCredits == 250);
    assert(soundCount == rewardSoundStart + 1 && sounds[soundCount - 1] == 0x24);

    layout.rewardBoxWidth = 30;
    layout.rewardBoxHeight = 20;
    layout.rewardBoxX = 150;
    layout.rewardBoxY = 12;
    layout.rewardBoxY2 = 36;
    layout.rewardIconImage = 177;
    layout.bgPatternImage = 178;
    layout.field_0x39c = 181;
    layout.field_0x3a0 = 182;
    layout.drawColor = 0x55667788;
    struct RewardFadeCase { int timer; unsigned color; };
    const RewardFadeCase rewardFadeCases[] = {
        {0, 0xffffff00u}, {1999, 0xfffffffeu}, {2000, 0xffffffffu},
        {5000, 0xffffffffu}, {5001, 0xfffffffeu}, {7000, 0xffffff00u},
    };
    for (const RewardFadeCase &fade : rewardFadeCases) {
        clearDraws();
        layout.rewardMessageTimer = fade.timer;
        layout.drawMissionRewardMessage(false);
        assert(colors.size() == 3 && colors[0] == 0xffffffffu && colors[1] == fade.color);
        assert(currentColor == 0x12345678 && layout.drawColor == 0x55667788u);
        assert(lastTextKey == 216 && images.size() == 1);
        assert(images[0].id == 177 && images[0].x == 240 && images[0].y == 150);
        assert(images[0].flagsA == 0x11 && images[0].flagsB == 0x14);
        assert(texts.size() == 2 && texts[0].content == "REWARD" && texts[1].content == "+ 250$");
        assert(texts[0].x == 236 && texts[0].y == 162 && texts[1].y == 186);
    }

    layout.showMissionRewardMessage(1234, true);
    assert(layout.rewardMessageFlag && layout.rewardMessageTimer == 0 && layout.rewardCredits == 1234);
    assert(soundCount == rewardSoundStart + 2 && sounds[soundCount - 1] == 0x24);
    layout.rewardMessageTimer = 2000;
    clearDraws();
    layout.drawMissionRewardMessage(true);
    assert(lastTextKey == 3206 && texts.size() == 2);
    assert(texts[0].content == "BONUS" && texts[1].content == "+ 1,234$");
    assert(imageAt(178, 230, 150) && imageAt(181, 230, 150));
    assert(currentColor == 0x12345678 && layout.drawColor == 0x55667788u);
    layout.rewardMessageActive = 0;
    clearDraws();
    layout.drawMissionRewardMessage(false);
    assert(images.empty() && texts.empty() && colors.empty());

    std::puts("TouchButton/Layout host smoke: OK");
}
