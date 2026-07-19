#ifndef GOF2_CARGOBAY_H
#define GOF2_CARGOBAY_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ListItem.h"
#include "TouchButton.h"
#include "game/mission/Item.h"
#include "game/ship/PlayerEgo.h"
#include "game/ui/HudEventDisplay.h"

struct CargoBay {
    unsigned char pad_0x0[0x54];
    int cargoCurrent;
    int cargoMax;
};
#endif
