#ifndef GOF2_HUDEVENTDISPLAY_H
#define GOF2_HUDEVENTDISPLAY_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ListItem.h"
#include "TouchButton.h"
#include "game/mission/Item.h"
#include "game/ship/PlayerEgo.h"

struct HudEventDisplay {
    unsigned char pad_0x0[0x1e0];
    float eventBannerDisplayScale;
    int eventBannerDisplayBase;
};
#endif
