#include "engine/core/AERandom.h"
AbyssEngine::AERandom *AERandom::gRandom = nullptr;
#include <ctime>

namespace AbyssEngine {
    AERandom::AERandom() {
        reset();
    }

    AERandom::AERandom(long long seed) {
        setSeed(seed);
    }

    AERandom::~AERandom() {
    }

    void AERandom::setSeed(long long seed) {
        seedLow = 0xdeece66dU ^ static_cast<uint32_t>(seed);
        seedHigh = (static_cast<uint32_t>(seed >> 32) & 0xffffU) ^ 5U;
    }

    void AERandom::reset() {
        long long now = time(0);
        seedLow = 0xdeece66dU ^ static_cast<uint32_t>(now);
        seedHigh = (static_cast<uint32_t>(now >> 31) & 0xffffU) ^ 5U;
    }

    uint32_t AERandom::next(int bits) {
        uint64_t seed = (static_cast<uint64_t>(seedHigh) << 32) | seedLow;
        seed = seed * 0x5deece66dULL + 0xbULL;
        uint32_t newLo = static_cast<uint32_t>(seed);
        uint32_t newHi = static_cast<uint32_t>(seed >> 32) & 0xffffU;

        seedLow = newLo;
        seedHigh = newHi;

        return static_cast<uint32_t>(((static_cast<uint64_t>(newHi) << 32) | newLo) >> (48 - bits));
    }

    int AERandom::nextInt() {
        return static_cast<int>(next(31));
    }

    int AERandom::nextInt(int bound) {
        if ((bound & -bound) == bound) {
            return static_cast<int>((static_cast<int64_t>(bound) * next(31)) >> 31);
        }

        int bits;
        int value;
        do {
            bits = static_cast<int>(next(31));
            value = bits % bound;
        } while (bits - value + (bound - 1) < 0);
        return value;
    }
}
