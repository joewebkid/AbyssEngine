#ifndef GOF2_CHOICEWINDOWBUTTONPOSCACHE_H
#define GOF2_CHOICEWINDOWBUTTONPOSCACHE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ScrollTouchWindow.h"
#include "TouchButton.h"

struct ChoiceWindowButtonPosCache {
    void *field_0x0;
    int field_0x4;

    union {
        int field_0x8;
        int rightButtonPosX;
        int rightButtonPosY;
        int singleButtonPosX;
        int singleButtonPosY;
    };

    union {
        int field_0xc;
        int leftButtonPosX;
        int leftButtonPosY;
    };
};
#endif
