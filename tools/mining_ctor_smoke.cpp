#include "game/menu/MiningGame.h"
#include "game/core/Globals.h"
#include "game/ui/Layout.h"
#include "game/mission/Status.h"
#include "game/mission/Item.h"
#include "game/ship/Ship.h"
#include "engine/render/PaintCanvas.h"
#include <cassert>
#include <cmath>
#include <cstdio>

static Item *fixtureEquipment = nullptr;
static Ship *expectedShip = nullptr;
static int createdCount = 0;
static int lastResource = 0;
static unsigned int drillInput = 0;

Layout *Globals::layout = nullptr;
Status *Globals::status = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
int Globals::w = 800;
int Globals::h = 600;

AbyssEngine::String::String() : data(nullptr), length(0) {}
AbyssEngine::String::~String() = default;
AbyssEngine::AEMath::Matrix::Matrix() = default;
Status::Status() = default;
Status::~Status() = default;
Ship::Ship(int, int, int, int, int, int, int, int, float) {}
Ship::~Ship() = default;
Item::Item(Array<int> *, Array<int> *, Array<int> *) {}
Item::~Item() = default;
Layout::Layout() : centerYBase(7), marqueeWidth(53), progressBarYBase(11),
                   animSpeedScale(2.0f), field_0xdc(4.0f) {}
Layout::~Layout() = default;
PaintCanvas::PaintCanvas(Engine *) {}
PaintCanvas::~PaintCanvas() = default;
void PaintCanvas::Image2DCreate(unsigned short resource, unsigned int &out) {
    if (createdCount == 0) drillInput = out;
    ++createdCount;
    lastResource = resource;
    out = resource + 1000;
}
int PaintCanvas::GetImage2DWidth(unsigned int) { return 64; }
int PaintCanvas::GetImage2DHeight(unsigned int) { return 40; }
Ship *Status::getShip() { assert(this == Globals::status); return ship; }
int Status::getCurrentCampaignMission() { return currentCampaignMission; }
Item *Ship::getFirstEquipmentOfSort(int sort) {
    assert(this == expectedShip && sort == 19);
    return fixtureEquipment;
}
int Item::getAttribute(int attribute) {
    assert(this == fixtureEquipment);
    assert(attribute == 32 || attribute == 33);
    return attribute == 32 ? 80 : 25;
}
Sprite::Sprite(uint32_t id, int width, int height)
    : image(id), frameWidth(width), frameHeight(height) {}
Sprite::~Sprite() = default;
void Sprite::defineReferencePixel(int x, int y) { refPixelX = x; refPixelY = y; }
MarqueeImage::MarqueeImage(uint16_t id, int width, int xValue, int yValue, float value)
    : image(id), visibleWidth(width), x(xValue), y(yValue), speed(value) {}
MarqueeImage::~MarqueeImage() = default;
void MarqueeImage::setSpeed(float value) { speed = value; }

int main() {
    Layout layout;
    Status status;
    Ship ship(0, 0, 0, 0, 0, 0, 0, 0, 0.0f);
    Item item(nullptr, nullptr, nullptr);
    PaintCanvas canvas(nullptr);
    expectedShip = &ship;
    fixtureEquipment = &item;
    status.ship = &ship;
    status.currentCampaignMission = 5;
    Globals::layout = &layout;
    Globals::status = &status;
    Globals::Canvas = &canvas;

    MiningGame *regular = new MiningGame(3, 0, nullptr);
    assert(drillInput == static_cast<unsigned int>(-1));
    assert(createdCount == 13 && lastResource == 0x4ed);
    assert(regular->centerX == 400 && regular->centerY == 307);
    assert(regular->progressBarX == 368 && regular->progressBarY == 11);
    assert(regular->marqueeWidth == 53);
    assert(std::fabs(regular->controlDivisor - 1.5f) < 0.001f);
    assert(std::fabs(regular->oreRate - 0.25f) < 0.001f);
    assert(regular->drillSprite->refPixelX == 20);
    assert(regular->oreMarquee->speed == 10.0f);
    assert(regular->campaignFlag == 1 && regular->isCoreLayer == 0);

    createdCount = 0;
    MiningGame *core = new MiningGame(7, 0xa4, nullptr);
    assert(createdCount == 14 && lastResource == 0x522);
    assert(core->isCoreLayer == 1 && core->coreImageId == 0x522 + 1000);
    std::puts("MiningGame constructor smoke: OK");
}
