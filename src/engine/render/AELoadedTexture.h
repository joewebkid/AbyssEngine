#ifndef GOF2_AELOADEDTEXTURE_H
#define GOF2_AELOADEDTEXTURE_H
#include "engine/core/AEString.h"
#include <cstdint>

namespace AbyssEngine {
    // Runtime texture record stored in PaintCanvas::cubeTextures. The name is
    // retained so suspended GL textures can be recreated from their AEI file.
    struct AELoadedTexture {
        uint32_t glId;        // 0x00
        String name;          // 0x04
        float scale;          // 0x10
        uint8_t isCube;       // 0x14
        uint8_t valid;        // 0x15
        uint16_t pad0x16;     // 0x16
        uint32_t byteSize;    // 0x18
    };
}

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(offsetof(AbyssEngine::AELoadedTexture, name) == 0x04, "AELoadedTexture.name");
static_assert(offsetof(AbyssEngine::AELoadedTexture, scale) == 0x10, "AELoadedTexture.scale");
static_assert(offsetof(AbyssEngine::AELoadedTexture, isCube) == 0x14, "AELoadedTexture.isCube");
static_assert(offsetof(AbyssEngine::AELoadedTexture, valid) == 0x15, "AELoadedTexture.valid");
static_assert(offsetof(AbyssEngine::AELoadedTexture, byteSize) == 0x18, "AELoadedTexture.byteSize");
static_assert(sizeof(AbyssEngine::AELoadedTexture) == 0x1c, "AELoadedTexture size");
#endif

#endif
