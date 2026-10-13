#include "game/menu/MiningGame.h"
#include "game/core/Globals.h"
#include "game/ui/Layout.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include "engine/core/GameText.h"
#include "engine/render/PaintCanvas.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

struct ImageCall { unsigned id; int x, y; unsigned char align, place; };
struct RegionCall { unsigned id; int half; unsigned char align, place, frame; };
struct ColorCall { unsigned char r, g, b, a; };
struct DrawnText { std::string value; int x, y; };
static std::vector<ImageCall> imageCalls;
static std::vector<RegionCall> rings;
static std::vector<ColorCall> colors;
static std::vector<DrawnText> strings;
static int progressWidth = -1;
static int pulseCalls = 0;
static int freeSpace = 2;
static int drillX = 0, drillY = 0;
static int oreX = 0, oreY = 0;

Layout *Globals::layout = nullptr;
Status *Globals::status = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
GameText *Globals::gameText = nullptr;
unsigned int Globals::font = 77;
int Globals::w = 800;

namespace AbyssEngine {
String::String() : data(nullptr), length(0) {}
String::String(const char *value, bool) : data(nullptr), length(0) { Set(value); }
String::String(int value) : data(nullptr), length(0) { Set(std::to_string(value).c_str()); }
String::String(const String &value, bool) : data(nullptr), length(0) { Set(value.data); }
String::~String() { delete[] data; }
void String::Set(const char *value) {
    delete[] data;
    length = value ? int(std::strlen(value)) : 0;
    data = new unsigned short[length + 1];
    for (int i = 0; i < length; ++i) data[i] = static_cast<unsigned char>(value[i]);
    data[length] = 0;
}
void String::Set(const unsigned short *value) {
    delete[] data;
    length = 0;
    if (value) while (value[length]) ++length;
    data = new unsigned short[length + 1];
    for (int i = 0; i < length; ++i) data[i] = value[i];
    data[length] = 0;
}
String operator+(const String &left, const String &right) {
    String result;
    result.data = new unsigned short[left.length + right.length + 1];
    result.length = left.length + right.length;
    for (int i = 0; i < left.length; ++i) result.data[i] = left.data[i];
    for (int i = 0; i < right.length; ++i) result.data[left.length + i] = right.data[i];
    result.data[result.length] = 0;
    return result;
}
namespace AEMath { Matrix::Matrix() = default; }
}

static std::string narrow(const String &value) {
    std::string out;
    for (int i = 0; i < value.length; ++i) out.push_back(char(value.data[i]));
    return out;
}

Layout::Layout() : ringRadiusScale(1.0f), ringNearMidThreshold(100),
                   ringMidFarThreshold(200), hudInset(5), oreLabelYOffset(7),
                   oreTextYOffset(9), promptYOffset(12) {}
Layout::~Layout() = default;
float Layout::getPulseValue(float speed) { assert(speed == 10.0f); return ++pulseCalls == 1 ? 0.5f : 0.25f; }
Status::Status() = default;
Status::~Status() = default;
Ship::Ship(int, int, int, int, int, int, int, int, float) {}
Ship::~Ship() = default;
Ship *Status::getShip() { assert(this == Globals::status); return ship; }
int Status::getCurrentCampaignMission() { return currentCampaignMission; }
int Ship::getFreeSpace() { assert(this == Globals::status->ship); return freeSpace; }
GameText::GameText() = default;
GameText::~GameText() = default;
String *GameText::getText(int key) { static String prompt("PROMPT"); assert(key == 616); return &prompt; }

PaintCanvas::PaintCanvas(Engine *) {}
PaintCanvas::~PaintCanvas() = default;
void PaintCanvas::SetColor(unsigned int) {}
void PaintCanvas::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    colors.push_back({r, g, b, a});
}
void PaintCanvas::DrawImage2D(unsigned id, int x, int y) { imageCalls.push_back({id, x, y, 0, 0}); }
void PaintCanvas::DrawImage2D(unsigned id, int x, int y, unsigned char align, unsigned char place) {
    imageCalls.push_back({id, x, y, align, place});
}
void PaintCanvas::DrawImage2D(unsigned id, int, int, int w, int h,
                              unsigned char align, unsigned char place, unsigned char frame) {
    assert(w == h);
    rings.push_back({id, w, align, place, frame});
}
void PaintCanvas::DrawRegion2D(unsigned, int, int, int width, int, float, int, int, int, int) {
    progressWidth = width;
}
int PaintCanvas::GetTextWidth(unsigned font, const String &value) {
    assert(font == 77);
    return value.length * 10;
}
void PaintCanvas::DrawString(unsigned font, const String &value, int x, int y, bool) {
    assert(font == 77);
    strings.push_back({narrow(value), x, y});
}

MiningGame::MiningGame(int, int, Hud *) {}
MiningGame::~MiningGame() = default;
Sprite::Sprite(uint32_t, int, int) {}
Sprite::~Sprite() = default;
void Sprite::setRefPixelPosition(int x, int y) { drillX = x; drillY = y; }
void Sprite::draw(float x, float y) { assert(x == 1.0f && y == 1.0f); }
MarqueeImage::MarqueeImage(uint16_t, int, int, int, float) {}
MarqueeImage::~MarqueeImage() = default;
void MarqueeImage::draw() {}
void MarqueeImage::draw(int x, int y) { oreX = x; oreY = y; }

int main() {
    Layout layout;
    Status status;
    Ship ship(0, 0, 0, 0, 0, 0, 0, 0, 0.0f);
    GameText gameText;
    PaintCanvas canvas(nullptr);
    Sprite drill(0, 0, 0);
    MarqueeImage left(0, 0, 0, 0, 0), right(0, 0, 0, 0, 0), ore(0, 0, 0, 0, 0);
    Globals::layout = &layout;
    Globals::status = &status;
    Globals::Canvas = &canvas;
    Globals::gameText = &gameText;
    status.ship = &ship;
    status.currentCampaignMission = 4;

    MiningGame game(1, 0, nullptr);
    game.currentLayer = 0;
    game.targetLayer = 1;
    game.ringEvenNear = 101;
    game.ringEvenMid = 102;
    game.ringEvenFar = 103;
    game.isCoreLayer = 1;
    game.coreImageId = 201;
    game.oreIconImageId = 202;
    game.cornerImageId = 203;
    game.progressBarImageId = 204;
    game.progressLabelImageId = 205;
    game.oreLabelImageId = 206;
    game.posX = 123.4f;
    game.posY = 234.5f;
    game.centerX = 400;
    game.centerY = 300;
    game.progressBarX = 360;
    game.progressBarY = 11;
    game.progressBarWidth = 80;
    game.progressBarHeight = 10;
    game.lossTimer = 900;
    game.oreAmount = 3.8f;
    game.textAlpha = 0.5f;
    game.promptPulseTimer = 1500;
    game.oreIconOffsetX = 15;
    game.oreIconOffsetY = 10;
    game.oreImageHeight = 20;
    game.drillSprite = &drill;
    game.leftMarquee = &left;
    game.rightMarquee = &right;
    game.oreMarquee = &ore;
    game.render2D();

    assert(rings.size() == 4 && rings[0].id == 102 && rings[0].half == 125);
    assert(rings[0].align == 0x11 && rings[0].place == 0x22 && rings[0].frame == 0);
    assert(rings[3].place == 0x11 && rings[3].frame == 3);
    assert(imageCalls.size() >= 3 && imageCalls[0].id == 201 && imageCalls[0].align == 0x11 && imageCalls[0].place == 0x44);
    assert(imageCalls[1].id == 202 && imageCalls[1].align == 0x11 && imageCalls[1].place == 0x44);
    assert(progressWidth == 51 && drillX == 123 && drillY == 234);
    assert(oreX == 138 && oreY == 214);
    assert(colors.size() == 3 && colors[0].g == 127 && colors[0].b == 63);
    assert(colors[1].g == 42 && colors[1].a == 127 && colors[2].a == 129);
    assert(strings.size() == 2 && strings[0].value == "3t" && strings[1].value == "PROMPT");
    assert(strings[0].x == 133 && strings[0].y == 243);
    assert(strings[1].x == 370 && strings[1].y == 23);

    rings.clear(); imageCalls.clear(); colors.clear(); strings.clear();
    pulseCalls = 0;
    status.currentCampaignMission = 5;
    freeSpace = 10;
    game.isCoreLayer = 0;
    game.currentLayer = 1;
    game.targetLayer = 2;
    game.ringOddNear = 111;
    game.ringOddMid = 112;
    game.ringOddFar = 113;
    game.lossTimer = 0;
    game.render2D();
    assert(rings.size() == 4 && rings[0].id == 112 && rings[0].half == 60);
    assert(pulseCalls == 0 && progressWidth == 80);
    assert(colors.size() == 1 && colors[0].g == 255 && colors[0].a == 127);
    assert(strings.size() == 1 && strings[0].value == "3t");
    std::puts("MiningGame render smoke: OK");
}
