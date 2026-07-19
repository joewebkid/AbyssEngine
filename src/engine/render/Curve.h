#ifndef GOF2_CURVE_H
#define GOF2_CURVE_H
#include <cstdint>

namespace AbyssEngine {
    // Engine-internal resource struct. Layout recovered from the original
    // ARM disassembly (offsets verified against the 32-bit match build).
    struct Curve {            // size 0x08
        uint16_t count;       // 0x00
        uint16_t pad0x2;      // 0x02
        void *entries;        // 0x04: count keyframe pointers
    };
}

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(sizeof(AbyssEngine::Curve) == 0x8, "Curve layout");
static_assert(offsetof(AbyssEngine::Curve, entries) == 0x4, "Curve.entries");
#endif

#endif
