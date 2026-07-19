#ifndef GOF2_TOKENSTRUCT_H
#define GOF2_TOKENSTRUCT_H
#include "engine/core/Array.h"
#include "../core/AEString.h"

namespace AbyssEngine {
    class ConfigReader;

    typedef void (*ConfigTokenReadFunction)(ConfigReader *, void *);

    struct TokenStruct {
        String name;
        ConfigTokenReadFunction read;
        void *context;
    };
}

#endif
