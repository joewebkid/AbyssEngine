#include "game/core/CheatCode.h"

namespace AbyssEngine {
    CheatCode::CheatCode() {
        keys = new Array<uint16_t>();
        value = 0;
        pos = 0;
    }

    CheatCode::~CheatCode() {
        delete keys;
        keys = nullptr;
    }

    bool CheatCode::Update(uint16_t key) {
        Array<uint16_t> *a = keys;
        uint32_t p = pos;
        uint16_t *d = a->data();
        uint32_t n = a->size();
        if (p >= n) {
            return false;
        }
        if (d[p] != key) {
            bool first = (d[0] == key);
            uint32_t one = (uint32_t) first;
            pos = (first && n != one);
            return n == one;
        }
        p = p + 1;
        uint32_t np = p;
        if (p == n) {
            np = 0;
        }
        pos = np;
        return p == n;
    }
}
