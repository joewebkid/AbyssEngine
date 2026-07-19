#ifndef GOF2_RESOURCEMATERIAL_H
#define GOF2_RESOURCEMATERIAL_H

#include <cstdint>
#include "engine/render/RenderEnums.h"

namespace AbyssEngine {

    class ResourceMaterial {
    public:
        ResourceMaterial(uint16_t texId, uint16_t texId2, BlendMode blend);

        ResourceMaterial(uint16_t texId, BlendMode blend);

        uint16_t texIndices[8];
        int blendMode;
        int field_14;
        int field_18;
        int field_1c;
        int field_20;
        int field_24;
    };
}

#endif
