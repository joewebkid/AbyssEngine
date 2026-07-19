#ifndef GOF2_KEYCODE_H
#define GOF2_KEYCODE_H
#include "game/core/String.h"

#include "engine/core/AEString.h"


namespace AbyssEngine {
    class KeyCode {
    public:
        int code;
        String name;

        KeyCode &operator=(const KeyCode &other);
    };
}

#endif
