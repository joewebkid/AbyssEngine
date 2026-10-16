#include "engine/core/AERandom.h"
#include <cassert>
#include <climits>
#include <cstdint>

static int armNextInt(int bound, uint64_t &state, int &draws) {
    const uint64_t mask = 0xffffffffffffULL;
    auto nextBits = [&]() {
        state = (state * 0x5deece66dULL + 11ULL) & mask;
        ++draws;
        return int(state >> 17);
    };
    uint32_t unsignedBound = uint32_t(bound);
    if ((unsignedBound & (0u - unsignedBound)) == unsignedBound) {
        int64_t product = int64_t(bound) * nextBits();
        return int32_t(uint32_t(product >> 31));
    }
    for (;;) {
        int bits = nextBits();
        int value = bits % bound;
        uint32_t check = uint32_t(bits) - uint32_t(value) + unsignedBound - 1u;
        if ((check & 0x80000000u) == 0) {
            return value;
        }
    }
}

int main() {
    const int bounds[]{0, 1, 2, 3, 7, 8, 40000, 0x60000000, INT_MAX, INT_MIN};
    const long long seeds[]{-1, 0, 1, 0x12345678};
    int rejectionDraws = 0;
    for (long long seed : seeds) {
        for (int bound : bounds) {
            AERandom actual(seed);
            uint64_t state = (uint64_t(actual.seedHigh) << 32) | actual.seedLow;
            int draws = 0;
            for (int i = 0; i < 64; ++i) {
                int expected = armNextInt(bound, state, draws);
                assert(actual.nextInt(bound) == expected);
                assert(actual.seedLow == uint32_t(state));
                assert(actual.seedHigh == uint32_t(state >> 32));
            }
            if (bound == 0x60000000) {
                rejectionDraws += draws - 64;
            }
        }
    }
    assert(rejectionDraws > 0);
}
