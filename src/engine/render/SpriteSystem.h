#ifndef GOF2_SPRITESYSTEM_H
#define GOF2_SPRITESYSTEM_H
#include <cstdint>

namespace AbyssEngine {
    class Mesh;

    // Engine-internal resource struct. Layout recovered from the original
    // ARM disassembly (offsets verified against the 32-bit match build).
    struct SpriteSystem {     // size 0x14
        uint16_t count;       // 0x00
        uint16_t pad0x2;      // 0x02
        float *posCpu;        // 0x04: count * vec3
        int16_t *sizeCpu;     // 0x08
        uint8_t sharedSize;   // 0x0c
        uint8_t pad0xd[3];    // 0x0d
        Mesh *mesh;           // 0x10
    };
}

#if __SIZEOF_POINTER__ == 4
#include <cstddef>
static_assert(sizeof(AbyssEngine::SpriteSystem) == 0x14, "SpriteSystem layout");
static_assert(offsetof(AbyssEngine::SpriteSystem, posCpu) == 0x4, "SpriteSystem.posCpu");
static_assert(offsetof(AbyssEngine::SpriteSystem, sizeCpu) == 0x8, "SpriteSystem.sizeCpu");
static_assert(offsetof(AbyssEngine::SpriteSystem, sharedSize) == 0xc, "SpriteSystem.sharedSize");
static_assert(offsetof(AbyssEngine::SpriteSystem, mesh) == 0x10, "SpriteSystem.mesh");
#endif

#endif
