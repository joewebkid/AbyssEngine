#include "engine/core/AERandom.h"
AbyssEngine::AERandom *AERandom::gRandom = nullptr;
#include <ctime>

namespace AbyssEngine {
    AERandom::AERandom() {
        long long now = time(0);
        seedLow = 0xdeece66dU ^ static_cast<uint32_t>(now);
        seedHigh = (static_cast<uint32_t>(now >> 31) & 0xffffU) ^ 5U;
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
        uint64_t seed = (static_cast<uint64_t>(seedHigh) << 32) | seedLow;
        seed = seed * 0x5deece66dULL + 0xbULL;
        seedLow = static_cast<uint32_t>(seed);
        seedHigh = static_cast<uint32_t>(seed >> 32) & 0xffffU;
        return static_cast<int>((seedLow >> 16) | (seedHigh << 16));
    }

    int AERandom::nextInt(int bound) {
        if ((bound & -bound) == bound) {
            uint64_t seed = (static_cast<uint64_t>(seedHigh) << 32) | seedLow;
            seed = seed * 0x5deece66dULL + 0xbULL;
            seedLow = static_cast<uint32_t>(seed);
            seedHigh = static_cast<uint32_t>(seed >> 32) & 0xffffU;
            int bits = static_cast<int>((seedLow >> 17) | (seedHigh << 15));
            return static_cast<int>((static_cast<int64_t>(bound) * bits) >> 31);
        }

        uint64_t seed = (static_cast<uint64_t>(seedHigh) << 32) | seedLow;
        int bits;
        int value;
        do {
            seed = (seed * 0x5deece66dULL + 0xbULL) & 0xffffffffffffULL;
            bits = static_cast<int>(seed >> 17);
            value = bits % bound;
        } while ((static_cast<uint32_t>(bits - value) +
                  static_cast<uint32_t>(bound - 1)) & 0x80000000U);
        seedLow = static_cast<uint32_t>(seed);
        seedHigh = static_cast<uint32_t>(seed >> 32);
        return value;
    }
}
