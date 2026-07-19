#ifndef GOF2_MATERIAL_H
#define GOF2_MATERIAL_H
#include <cstdint>

#include "engine/core/Array.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"


namespace AbyssEngine {
    class Mesh;

    class Material {
    public:
        int textures[8];

        union {
            int materialMode;
            int blendMode;
        };

        void *addData;
        uint32_t addDataSize;
        Array<AEMath::Matrix> arr_2c;
        Array<AEMath::Matrix> arr_38;
        Array<Mesh *> meshes;
        Array<uint32_t> arr_50;
        Array<AEMath::Matrix> arr_5c;
        AEMath::Vector ambientColor;

        Material();

        Material(Material *other);

        ~Material();
    };

#if __SIZEOF_POINTER__ == 4
    static_assert(sizeof(Array<AEMath::Matrix>) == 0x0c, "Array layout (Material)");
    static_assert(sizeof(Material) == 0x74, "Material size");
    static_assert(__builtin_offsetof(Material, textures) == 0x00, "Material::textures");
    static_assert(__builtin_offsetof(Material, materialMode) == 0x20, "Material::materialMode");
    static_assert(__builtin_offsetof(Material, addData) == 0x24, "Material::addData");
    static_assert(__builtin_offsetof(Material, addDataSize) == 0x28, "Material::addDataSize");
    static_assert(__builtin_offsetof(Material, arr_2c) == 0x2c, "Material::arr_2c");
    static_assert(__builtin_offsetof(Material, arr_38) == 0x38, "Material::arr_38");
    static_assert(__builtin_offsetof(Material, meshes) == 0x44, "Material::meshes");
    static_assert(__builtin_offsetof(Material, arr_50) == 0x50, "Material::arr_50");
    static_assert(__builtin_offsetof(Material, arr_5c) == 0x5c, "Material::arr_5c");
    static_assert(__builtin_offsetof(Material, ambientColor) == 0x68, "Material::ambientColor");
#endif
}

namespace AbyssEngine {
    class Material;
}
using ::AbyssEngine::Material;

#endif
