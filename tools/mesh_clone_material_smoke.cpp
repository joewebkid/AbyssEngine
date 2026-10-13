#include "engine/render/Material.h"
#include "engine/render/Mesh.h"
#include <cassert>
#include <cstdio>

namespace AbyssEngine {
struct PaintCanvas {
    unsigned meshCount;
    Mesh **meshes;
    Array<Material *> materials;
    void MeshCloneMaterial(unsigned index, unsigned &out);
};

// PRODUCTION_METHOD
}

int main() {
    using namespace AbyssEngine;
    Material first, second;
    first.textures[1] = 12;
    second.textures[1] = 34;
    Mesh a, b;
    a.material = &first;
    b.material = &second;
    Mesh *meshes[] = {&a, &b};
    PaintCanvas canvas = {};
    canvas.meshCount = 2;
    canvas.meshes = meshes;

    unsigned out = 7;
    canvas.MeshCloneMaterial(2, out);
    assert(out == 0xffffffff && canvas.materials.size_ == 0);
    canvas.MeshCloneMaterial(0, out);
    assert(out == 0 && canvas.materials.size_ == 1);
    assert(canvas.materials[0] != &first && canvas.materials[0]->textures[1] == 12);
    assert(canvas.materials[0]->ambientColor.x == -10.0f);
    assert(canvas.materials[0]->arr_2c.size_ == 0);
    canvas.MeshCloneMaterial(1, out);
    assert(out == 1 && canvas.materials.size_ == 2);
    assert(canvas.materials[1] != &second && canvas.materials[1]->textures[1] == 34);
    assert(canvas.materials.capacity_ == 2);

    delete canvas.materials[0];
    delete canvas.materials[1];
    std::puts("MeshCloneMaterial host smoke: OK");
}
