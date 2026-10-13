#include "engine/render/Material.h"

namespace AbyssEngine {
    Material::Material() {
        this->blendMode = 0;

        this->ambientColor.x = -10.0f;
        this->ambientColor.y = 0;
        this->ambientColor.z = 0;

        for (int i = 0; i != 8; ++i)
            this->textures[i] = -1;
        this->textures[0] = 0;

        this->addData = 0;
        this->addDataSize = 0;
    }

    Material::Material(Material *other) {
        this->ambientColor.x = 0;
        this->ambientColor.y = 0;
        this->ambientColor.z = 0;

        if (other == 0) {
            this->blendMode = 0;
            for (int i = 0; i != 8; ++i)
                this->textures[i] = -1;
        } else {
            for (int i = 0; i != 8; ++i)
                this->textures[i] = other->textures[i];
            this->blendMode = other->blendMode;
            uint32_t size = other->addDataSize;
            this->addDataSize = size;
            this->addData = size == 0 ? 0 : new uint8_t[size];
            this->ambientColor = other->ambientColor;
        }
    }

    Material::~Material() {
        // Inline Array members are destroyed automatically in reverse
        // declaration order (arr_5c, arr_50, meshes, arr_38, arr_2c),
        // matching the original Material::~Material.
    }
}
