#ifndef GOF2_IMAGEFONT_H
#define GOF2_IMAGEFONT_H
#include <cstdint>

namespace AbyssEngine {
    class Mesh;

    // Engine-internal resource struct. Layout recovered from the original
    // ARM disassembly (offsets verified against the 32-bit match build).
    struct ImageFont {        // size 0x14
        uint16_t glyphCount;  // 0x00
        uint16_t pad0x2;      // 0x02
        uint16_t *codes;      // 0x04: glyphCount unicode code points
        void *field_0x8;      // 0x08
        Mesh **glyphMeshes;   // 0x0c: glyphCount Mesh* entries
        int16_t spacing;      // 0x10
        int16_t yOffset;      // 0x12
    };
}

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(sizeof(AbyssEngine::ImageFont) == 0x14, "ImageFont layout");
static_assert(offsetof(AbyssEngine::ImageFont, codes) == 0x4, "ImageFont.codes");
static_assert(offsetof(AbyssEngine::ImageFont, glyphMeshes) == 0xc, "ImageFont.glyphMeshes");
static_assert(offsetof(AbyssEngine::ImageFont, spacing) == 0x10, "ImageFont.spacing");
static_assert(offsetof(AbyssEngine::ImageFont, yOffset) == 0x12, "ImageFont.yOffset");
#endif

#endif
