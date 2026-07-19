#ifndef GOF2_PLAYERCREATURE_H
#define GOF2_PLAYERCREATURE_H

#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/Matrix.h"


class Player;


class AEGeometry;


class PlayerCreature : public KIPlayer {
public:
    AEGeometry *renderGeometry;
    int creatureType;
    int lastElapsed;
    uint16_t raging;
    uint8_t hooked;
    uint8_t caught;
    float rageScale;
    int rageTimer;
    int maxEndurance;
    int endurance;
    int lastHitpoints;
    int itemIndex;
    AbyssEngine::Matrix rageMatrix;

    PlayerCreature(int kind, int itemIndex, Player *player, AEGeometry *geometry,
                   float x, float y, float z);

    ~PlayerCreature();

    uint8_t isHooked();

    void calmDown();

    void unhook();

    void render() override;

    int getEndurance();

    int getWeight();

    uint8_t isCaught();

    void rage(int amount);

    int getMaxEndurance();

    int getItemIndex();

    void reset();

    void hook(int value);

    void update(int elapsed) override;

    int collide(float x, float y, float z) override;

    int outerCollide(float x, float y, float z) override;
};

#endif
