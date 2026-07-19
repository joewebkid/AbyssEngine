#ifndef GOF2_CHEATHANDLER_H
#define GOF2_CHEATHANDLER_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/core/CheatCode.h"

namespace AbyssEngine {
    class KeyCode;
 }


namespace AbyssEngine {
    typedef void (*CheatFunc)(int, void *);

    class CheatHandler {
    public:
        CheatFunc func;
        void *context;
        Array<CheatCode *> *codes;
        KeyCode *keys;

        CheatHandler(KeyCode *keys);

        ~CheatHandler();

        void AddCheatCode(const String &code, int value);

        void SetCheatFunc(CheatFunc func, void *context);

        void Update(uint16_t key);
    };
}

void OnCheatActivated(int code, void *data);

#endif
