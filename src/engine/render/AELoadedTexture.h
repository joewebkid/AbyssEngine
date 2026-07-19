#ifndef GOF2_AELOADEDTEXTURE_H
#define GOF2_AELOADEDTEXTURE_H
#include <cstdint>

namespace AbyssEngine {
    // Partial: fields touched by texture upload. Layout recovered from the
    // original ARM disassembly (32-bit match build).
    struct AELoadedTexture {
        uint32_t glId;        // 0x00
        uint32_t field_0x4;   // 0x04
        uint32_t field_0x8;   // 0x08
        uint32_t field_0xc;   // 0x0c
        uint32_t field_0x10;  // 0x10
        uint8_t isCube;       // 0x14
        uint8_t valid;        // 0x15
        uint16_t pad0x16;     // 0x16
        uint32_t byteSize;    // 0x18
    };
}

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(offsetof(AbyssEngine::AELoadedTexture, isCube) == 0x14, "AELoadedTexture.isCube");
static_assert(offsetof(AbyssEngine::AELoadedTexture, valid) == 0x15, "AELoadedTexture.valid");
static_assert(offsetof(AbyssEngine::AELoadedTexture, byteSize) == 0x18, "AELoadedTexture.byteSize");
#endif

#endif
