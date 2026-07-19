#ifndef GOF2_RESOURCETRANSFORM_H
#define GOF2_RESOURCETRANSFORM_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include <cstdint>

namespace AbyssEngine {
    class ResourceTransform {
    public:
        uint8_t header[0x40];
        uint8_t *dataA;
        uint32_t pad44;
        uint8_t *dataB;

        ~ResourceTransform();
    };
}
#endif
