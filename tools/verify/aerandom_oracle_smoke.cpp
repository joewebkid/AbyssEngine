// Differential check against the 32-bit operations observed in the Android
// ARM AERandom routines at 0x71a54, 0x71aa4, and 0x71ad0.
#include "engine/core/AERandom.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>

struct ArmState {
    std::uint32_t lo;
    std::uint32_t hi;
};

static std::uint32_t armStep(ArmState &state) {
    const std::uint32_t oldLo = state.lo;
    const std::uint64_t product = std::uint64_t(oldLo) * 0xdeece66dU;
    const std::uint32_t productLo = std::uint32_t(product);
    state.lo = productLo + 11U;
    const std::uint32_t carry = state.lo < productLo;
    state.hi = (std::uint32_t(product >> 32) + oldLo * 5U +
                state.hi * 0xdeece66dU + carry) & 0xffffU;
    return (state.hi << 16) | (state.lo >> 16);
}

static void check(const char *what, std::uint32_t actual, std::uint32_t expected) {
    if (actual != expected) {
        std::fprintf(stderr, "%s: got %08x, expected %08x\n", what, actual, expected);
        std::abort();
    }
}

static void checkState(const AERandom &actual, const ArmState &expected) {
    check("seedLow", actual.seedLow, expected.lo);
    check("seedHigh", actual.seedHigh, expected.hi);
}

static int armBounded(ArmState &state, int bound, unsigned &rejections) {
    if ((bound & -bound) == bound) {
        const std::uint32_t bits = armStep(state) >> 1;
        return int((std::uint64_t(std::uint32_t(bound)) * bits) >> 31);
    }

    for (;;) {
        const std::int32_t bits = std::int32_t(armStep(state) >> 1);
        const int value = bits % bound;
        // ARM ADD/SUB wraps: BLT tests the sign of the wrapped result.
        const std::uint32_t test = std::uint32_t(bound - 1) -
                                   std::uint32_t(value) + std::uint32_t(bits);
        if ((test & 0x80000000U) == 0) return value;
        ++rejections;
    }
}

int main() {
    const std::int64_t seeds[] = {
        0, 1, -1, 0x123456789abcdef0LL,
        std::numeric_limits<std::int64_t>::min(),
        std::numeric_limits<std::int64_t>::max()
    };
    const int widths[] = {1, 2, 16, 17, 31, 32};
    const int bounds[] = {0, 1, 2, 3, 7, 1023, 1024, 10000,
                          1073741825, std::numeric_limits<int>::max()};
    unsigned rejections = 0;
    unsigned checks = 0;

    for (std::int64_t seed : seeds) {
        AERandom random(seed);
        ArmState expected{std::uint32_t(seed) ^ 0xdeece66dU,
                          (std::uint32_t(seed >> 32) & 0xffffU) ^ 5U};
        checkState(random, expected);
        for (unsigned round = 0; round < 128; ++round) {
            const std::uint32_t full = armStep(expected);
            check("nextInt()", std::uint32_t(random.nextInt()), full);
            checkState(random, expected);
            ++checks;

            const int width = widths[round % (sizeof(widths) / sizeof(widths[0]))];
            const std::uint32_t top = armStep(expected) >> (32 - width);
            check("next(bits)", random.next(width), top);
            checkState(random, expected);
            ++checks;

            const int bound = bounds[round % (sizeof(bounds) / sizeof(bounds[0]))];
            const int result = armBounded(expected, bound, rejections);
            check("nextInt(bound)", std::uint32_t(random.nextInt(bound)),
                  std::uint32_t(result));
            checkState(random, expected);
            ++checks;
        }

        random.setSeed(seed);
        expected = {std::uint32_t(seed) ^ 0xdeece66dU,
                    (std::uint32_t(seed >> 32) & 0xffffU) ^ 5U};
        checkState(random, expected);
    }

    if (rejections == 0) {
        std::fputs("rejection path was not exercised\n", stderr);
        return 1;
    }
    std::printf("AERandom ARM-oracle smoke: %u draws, %u rejections OK\n",
                checks, rejections);
}
