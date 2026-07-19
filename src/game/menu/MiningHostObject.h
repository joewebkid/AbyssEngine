#ifndef GOF2_MININGHOSTOBJECT_H
#define GOF2_MININGHOSTOBJECT_H
#include <cstdint>
#include <cstddef>
#include "engine/render/MarqueeImage.h"
#include "engine/render/Sprite.h"
#include "game/ui/Hud.h"

struct MiningHostObject {
    uint8_t field_0x0[0x37];
    uint8_t miningActiveFlag;
    uint8_t field_0x38[0x124 - 0x38];
    int miningResultSlot;
};
#endif
