#ifndef GOF2_IMAGE_H
#define GOF2_IMAGE_H
#include <cstdint>

namespace AbyssEngine {
    // Engine-internal resource struct. Layout recovered from the original
    // ARM disassembly (offsets verified against the 32-bit match build).
    struct Image {            // size 0x14
        uint16_t width;       // 0x00
        uint16_t height;      // 0x02
        uint32_t format;      // 0x04
        uint8_t hasMipmaps;   // 0x08
        uint8_t pad0x9[3];    // 0x09
        void *data;           // 0x0c
        uint32_t dataLen;     // 0x10
    };
}

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(offsetof(AbyssEngine::Image, format) == 0x4, "Image.format");
static_assert(offsetof(AbyssEngine::Image, hasMipmaps) == 0x8, "Image.hasMipmaps");
static_assert(offsetof(AbyssEngine::Image, data) == 0xc, "Image.data");
static_assert(offsetof(AbyssEngine::Image, dataLen) == 0x10, "Image.dataLen");
#endif

#endif
