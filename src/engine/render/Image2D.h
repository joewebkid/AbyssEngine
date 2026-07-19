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

#endif
