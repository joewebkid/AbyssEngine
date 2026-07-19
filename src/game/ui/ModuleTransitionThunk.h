#ifndef GOF2_MODULETRANSITIONTHUNK_H
#define GOF2_MODULETRANSITIONTHUNK_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ui/TouchButton.h"
struct ModuleTransitionThunk {
    union {
        void (*transitionFn)(void *app, int mode);

        void (*transitionFn3)(void *app, int a, int b);

        void *field_0x0;
    };
};
#endif
