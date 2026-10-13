#include "game/menu/MiningGame.h"
#include "game/core/Globals.h"
#include "game/ui/Layout.h"
#include "game/mission/Status.h"
#include "game/mission/Achievements.h"
#include "engine/core/AERandom.h"
#include "engine/audio/FModSound.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>

extern "C" int LAYER_DIAMETERS[49];
extern "C" float LAYER_SPEEDS[7];

static int played = -1;
static int stopped = -1;
static float soundParam = -1.0f;
static int spriteFrames = 0;
static int randomCalls = 0;
static int randomValues[5] = {5, 2, 0, 3, 1};
static bool medalOwned = true;
static int medalEvents = 0;
static int medalPercent = -1;

AERandom::AERandom() = default;
AERandom::~AERandom() = default;
int AERandom::nextInt(int) { return randomValues[randomCalls++]; }
FModSound::FModSound() = default;
FModSound::~FModSound() = default;
void FModSound::play(int id, Vector *, Vector *, float) { played = id; }
void FModSound::stop(int id) { stopped = id; }
void FModSound::setParamValue(int, int, float value) { soundParam = value; }
MarqueeImage::MarqueeImage(uint16_t, int, int, int, float value) : speed(value) {}
MarqueeImage::~MarqueeImage() = default;
void MarqueeImage::update(int) {}
void MarqueeImage::setSpeed(float value) { speed = value; }
Sprite::Sprite(uint32_t, int, int) {}
Sprite::~Sprite() = default;
void Sprite::nextFrame() { ++spriteFrames; }
Achievements::Achievements() = default;
Achievements::~Achievements() = default;
uint8_t Achievements::hasMedal(int id, int tier) {
    assert(id == 38 && tier == 1);
    return medalOwned;
}
int Achievements::getValue(int id, int tier) {
    assert(id == 38 && tier == 1);
    return 10;
}
void Hud::hudEventMedal(int id, int percent) {
    assert(id == 38);
    ++medalEvents;
    medalPercent = percent;
}

AbyssEngine::String::String() : data(nullptr), length(0) {}
AbyssEngine::String::~String() = default;
Status::Status() = default;
Status::~Status() = default;
Hud::Hud() = default;
Hud::~Hud() = default;

Layout *Globals::layout = nullptr;
void *Globals::globals = nullptr;
void *Globals::rnd = nullptr;
FModSound *Globals::sound = nullptr;
Status *Globals::status = nullptr;
void *Globals::achievements = nullptr;
Globals::Globals() = default;
Globals::~Globals() = default;
Layout::Layout() {
    ringRadiusScale = 1.0f;
    driftSpeedDivisor = 1.0f;
    animSpeedScale = 1.0f;
}
Layout::~Layout() = default;
MiningGame::MiningGame(int level, int system, Hud *hud)
    : layer(level), station(system), targetLayer(level), currentLayer(0), hud(hud) {
    posX = posY = 0.0f;
    centerX = centerY = 0;
}
MiningGame::~MiningGame() = default;

int main() {
    Globals globals;
    Layout layout;
    Globals::globals = &globals;
    Globals::layout = &layout;

    assert(LAYER_DIAMETERS[0] == 250 && LAYER_DIAMETERS[48] == 0);
    assert(LAYER_SPEEDS[0] == 5.0f && LAYER_SPEEDS[6] == 38.0f);
    MiningGame game(7, 0, nullptr);
    game.posX = 124.0f;
    assert(game.isInCurrentLayer());
    game.posX = 125.0f;
    assert(!game.isInCurrentLayer());
    game.posX = 3.0f;
    game.posY = 4.0f;
    assert(game.isInCurrentLayer());

    layout.ringRadiusScale = 1.5f;
    game.posX = 187.0f;
    game.posY = 0.0f;
    assert(game.isInCurrentLayer());
    game.posX = 188.0f;
    assert(!game.isInCurrentLayer());

    layout.ringRadiusScale = 1.0f;
    game.targetLayer = 3;
    game.currentLayer = 1;
    game.posX = 79.0f;
    assert(game.isInCurrentLayer());
    game.posX = 80.0f;
    assert(!game.isInCurrentLayer());

    game.targetLayer = 1;
    game.posX = 0.0f;
    assert(!game.isInCurrentLayer());

    AERandom random;
    FModSound sound;
    MarqueeImage left(0, 0, 0, 0, 0.0f);
    MarqueeImage right(0, 0, 0, 0, 0.0f);
    MarqueeImage ore(0, 0, 0, 0, 0.0f);
    Sprite drill(0, 0, 0);
    Globals::rnd = &random;
    Globals::sound = &sound;
    game.targetLayer = 7;
    game.currentLayer = 0;
    game.campaignFlag = 1;
    game.leftMarquee = &left;
    game.rightMarquee = &right;
    game.oreMarquee = &ore;
    game.drillSprite = &drill;
    game.posX = game.posY = game.inputX = game.inputY = 0.0f;
    game.driftX = game.driftY = 0.0f;
    game.driftTimer = game.layerTimer = game.lossTimer = game.promptPulseTimer = 0;
    game.animAccumulator = 0.0f;
    game.oreAmount = 3.2f;
    game.oreRate = 2.0f;
    game.textAlpha = 0.4f;
    assert(game.update(100) == 1);
    assert(std::fabs(game.textAlpha - 0.6f) < 0.001f);
    assert(soundParam == 0.0f);
    assert(spriteFrames == 1);

    game.oreAmount = 3.2f;
    game.oreRate = 20.0f;
    game.textAlpha = 0.4f;
    assert(game.update(100) == 1);
    assert(std::fabs(game.textAlpha - 0.2f) < 0.001f);

    int framesBeforeNan = spriteFrames;
    game.animAccumulator = std::numeric_limits<float>::quiet_NaN();
    game.oreRate = 0.0f;
    assert(game.update(0) == 1);
    assert(spriteFrames == framesBeforeNan);
    game.animAccumulator = 0.0f;

    game.driftTimer = 2501;
    game.controlDivisor = 2.0f;
    game.oreRate = 0.0f;
    game.update(0);
    assert(randomCalls == 5);
    assert(std::fabs(game.driftX - 0.35f) < 0.001f);
    assert(std::fabs(game.driftY + 0.4f) < 0.001f);

    game.posX = 124.0f;
    game.posY = 0.0f;
    game.inputX = 1.0f;
    played = stopped = -1;
    assert(game.update(2) == 1);
    assert(played == 3 && stopped == 1);
    assert(game.textAlpha == 1.0f);

    Achievements achievements;
    Globals::achievements = &achievements;
    game.posX = game.posY = game.inputX = game.inputY = 0.0f;
    game.driftX = game.driftY = 0.0f;
    game.currentLayer = 0;
    game.targetLayer = 7;
    game.layerTimer = 6001;
    assert(game.update(0) == 1);
    assert(game.currentLayer == 1 && ore.speed == 8.0f);

    game.currentLayer = 6;
    game.targetLayer = 7;
    game.layerTimer = 6001;
    assert(game.update(0) == 0);
    assert(game.gameWonFlag == 1 && game.gotCoreFlag == 1);

    Status status;
    Hud hud;
    Globals::status = &status;
    game.hud = &hud;
    medalOwned = false;
    status.field_124 = 2;
    status.field_128 = 0x12345600;
    game.currentLayer = 6;
    game.layerTimer = 6001;
    assert(game.update(0) == 0);
    assert(status.field_124 == 3 && status.field_128 == 0x12345600);
    assert(medalEvents == 1 && medalPercent == 30);

    status.field_124 = 9;
    game.currentLayer = 6;
    game.layerTimer = 6001;
    assert(game.update(0) == 0);
    assert(status.field_124 == 10 && status.field_128 == 0x12345601);
    assert(medalEvents == 2 && medalPercent == 100);

    game.currentLayer = 0;
    game.posX = 500.0f;
    game.posY = game.inputX = game.inputY = game.driftX = game.driftY = 0.0f;
    game.driftTimer = 0;
    game.lossTimer = 2499;
    game.oreAmount = 12.0f;
    status.field_124 = 5;
    assert(game.update(1) == 1);
    assert(game.lossTimer == 2500 && game.gameLostFlag == 0);
    assert(game.update(1) == 0);
    assert(game.lossTimer == 2500 && game.oreAmount == 0.0f);
    assert(game.gameLostFlag == 1 && status.field_124 == 0);
    std::puts("MiningGame layer smoke: OK");
}
