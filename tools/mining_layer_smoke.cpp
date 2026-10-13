#include "game/menu/MiningGame.h"
#include "game/core/Globals.h"
#include "game/ui/Layout.h"
#include <cassert>
#include <cstdio>

extern "C" int LAYER_DIAMETERS[49];

Layout *Globals::layout = nullptr;
void *Globals::globals = nullptr;
Globals::Globals() = default;
Globals::~Globals() = default;
Layout::Layout() { ringRadiusScale = 1.0f; }
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
    std::puts("MiningGame layer smoke: OK");
}
