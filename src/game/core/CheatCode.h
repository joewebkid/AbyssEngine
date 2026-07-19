#ifndef GOF2_CHEATCODE_H
#define GOF2_CHEATCODE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
namespace AbyssEngine {
    class CheatCode {
    public:
        Array<uint16_t> *keys;
        int value;
        int pos;

        CheatCode();

        ~CheatCode();

        bool Update(uint16_t key);
    };
}

#endif
