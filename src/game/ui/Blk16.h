#ifndef GOF2_BLK16_H
#define GOF2_BLK16_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/core/HangarList.h"
#include "game/mission/Item.h"
#include "game/ui/ChoiceWindow.h"
#include "game/ui/ListItemWindow.h"

// Android Layout+0x238 is copied verbatim into HangarWindow+0x100. The
// renderer and touch picker consume its fourth word as the gap between rows.
struct HangarRowLayoutMetrics {
    int field_0x00;
    int field_0x04;
    int field_0x08;
    int rowGap;
};

static_assert(sizeof(HangarRowLayoutMetrics) == 0x10, "Hangar row metrics ABI drift");
#endif
