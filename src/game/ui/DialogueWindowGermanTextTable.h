#ifndef GOF2_DIALOGUEWINDOWGERMANTEXTTABLE_H
#define GOF2_DIALOGUEWINDOWGERMANTEXTTABLE_H
#include "ChoiceWindow.h"
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ScrollTouchWindow.h"
#include "TouchButton.h"
#include "engine/render/ImagePart.h"
#include "game/mission/Mission.h"
#include "game/world/Level.h"

struct DialogueWindowGermanTextTable {
    int maleRaceRow;
    int field_0x4[17];
    int femaleVariantBase;
    int field_0x4c[1];
};
#endif
