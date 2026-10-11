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
    int lastElapsed;
    uint8_t raging;
    uint8_t hooked;
    uint8_t caught;
    uint8_t _pad_0x12b;
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

#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(PlayerCreature, lastElapsed) == 0x124, "PlayerCreature.lastElapsed");
static_assert(offsetof(PlayerCreature, raging) == 0x128, "PlayerCreature.raging");
static_assert(offsetof(PlayerCreature, hooked) == 0x129, "PlayerCreature.hooked");
static_assert(offsetof(PlayerCreature, caught) == 0x12a, "PlayerCreature.caught");
static_assert(offsetof(PlayerCreature, rageScale) == 0x12c, "PlayerCreature.rageScale");
static_assert(offsetof(PlayerCreature, maxEndurance) == 0x134, "PlayerCreature.maxEndurance");
static_assert(offsetof(PlayerCreature, rageMatrix) == 0x144, "PlayerCreature.rageMatrix");
static_assert(sizeof(PlayerCreature) == 0x180, "PlayerCreature ARM size");
#endif

#endif
