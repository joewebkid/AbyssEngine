#include <cstdint>
#include <cstdio>
#include <cstring>
#include <initializer_list>

unsigned classifyGunScatter(int);

static bool agreesWithNative(std::uint32_t bits) {
    static_assert(sizeof(int) == sizeof(bits), "Native weapon type is a 32-bit word");
    int kind;
    std::memcpy(&kind, &bits, sizeof(kind));
    return classifyGunScatter(kind) == static_cast<unsigned>(kind == 25);
}

int checkGunScatterClassification() {
    // Each leading-zero partition, its boundary, and wrapped signed inputs.
    if (!agreesWithNative(25) || !agreesWithNative(0) ||
        !agreesWithNative(0x80000000u) || !agreesWithNative(0x7fffffffu)) return 1;
    for (unsigned bit = 0; bit < 32; ++bit) {
        std::uint32_t difference = std::uint32_t(1) << bit;
        for (std::uint32_t value : {difference, difference - 1, 0u - difference}) {
            if (!agreesWithNative(value + 25u)) return 1;
        }
    }
    std::uint32_t sample = 0x12345678u;
    for (unsigned i = 0; i < 100000; ++i) {
        sample ^= sample << 13;
        sample ^= sample >> 17;
        sample ^= sample << 5;
        if (!agreesWithNative(sample)) return 1;
    }
    std::puts("Gun extracted scatter classifier boundary/random smoke: OK");
    return 0;
}
