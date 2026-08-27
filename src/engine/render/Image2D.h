#ifndef GOF2_IMAGE2D_H
#define GOF2_IMAGE2D_H
#include <cstdint>

namespace AbyssEngine {
    class Mesh;

    // Textured 2D region: a quad Mesh plus atlas metadata. Layout recovered
    // from the original ARM disassembly (32-bit match build).
    struct Image2D {
        Mesh *mesh;           // 0x00
        uint32_t field_0x4;   // 0x04
        uint16_t atlasW;      // 0x08
        uint16_t atlasH;      // 0x0a
        uint16_t offX;        // 0x0c
        uint16_t offY;        // 0x0e
        uint16_t sizeX;       // 0x10
        uint16_t sizeY;       // 0x12
    };
}

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(sizeof(AbyssEngine::Image2D) == 0x14, "Image2D size");
static_assert(offsetof(AbyssEngine::Image2D, field_0x4) == 0x4, "Image2D.field_0x4");
static_assert(offsetof(AbyssEngine::Image2D, atlasW) == 0x8, "Image2D.atlasW");
static_assert(offsetof(AbyssEngine::Image2D, offX) == 0xc, "Image2D.offX");
static_assert(offsetof(AbyssEngine::Image2D, sizeX) == 0x10, "Image2D.sizeX");
#endif

#endif
