#include "engine/render/Material.h"
#include <cassert>
#include <cstdio>

int main() {
    AbyssEngine::Material material;
    assert(material.ambientColor.x == -10.0f);
    assert(material.ambientColor.y == 0 && material.ambientColor.z == 0);
    assert(material.blendMode == 0 && material.textures[0] == 0);
    for (int i = 1; i < 8; ++i) assert(material.textures[i] == -1);
    assert(material.addData == nullptr && material.addDataSize == 0);
    assert(material.arr_2c.size_ == 0 && material.arr_2c.capacity_ == 1);
    assert(material.arr_38.size_ == 0 && material.arr_38.capacity_ == 1);
    assert(material.meshes.size_ == 0 && material.meshes.capacity_ == 1);
    assert(material.arr_50.size_ == 0 && material.arr_50.capacity_ == 1);
    assert(material.arr_5c.size_ == 0 && material.arr_5c.capacity_ == 1);

    AbyssEngine::Matrix matrix;
    for (unsigned frame = 0; frame < 2; ++frame) {
        matrix.m[3] = float(frame + 1);
        ArrayAddCached(matrix, material.arr_2c);
        ArrayAddCached(matrix, material.arr_38);
        ArrayAddCached(static_cast<AbyssEngine::Mesh *>(nullptr), material.meshes);
        ArrayAddCached(0xffffffffu, material.arr_50);
        ArrayAddCached(matrix, material.arr_5c);
        assert(material.arr_2c.size_ == 1 && material.arr_2c[0].m[3] == float(frame + 1));
        assert(material.arr_5c.size_ == 1 && material.arr_5c[0].m[3] == float(frame + 1));
        material.arr_2c.size_ = material.arr_38.size_ = material.meshes.size_ = 0;
        material.arr_50.size_ = material.arr_5c.size_ = 0;
    }

    AbyssEngine::Material clone(&material);
    assert(clone.ambientColor.x == -10.0f && clone.blendMode == 0);
    assert(clone.arr_2c.size_ == 0 && clone.arr_5c.size_ == 0);
    AbyssEngine::Material empty(static_cast<AbyssEngine::Material *>(nullptr));
    assert(empty.blendMode == 0 && empty.textures[0] == -1);
    assert(empty.ambientColor.x == 0);
    std::puts("Material lifecycle host smoke: OK");
}
