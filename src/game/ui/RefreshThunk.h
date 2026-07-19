#ifndef GOF2_REFRESHTHUNK_H
#define GOF2_REFRESHTHUNK_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ui/TouchButton.h"
#include "game/ui/ModuleTransitionThunk.h"

struct RefreshThunk {
    union {
        void (*refreshFn)();

        void *field_0x0;
    };
};
#endif
