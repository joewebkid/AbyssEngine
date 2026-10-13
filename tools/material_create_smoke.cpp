#include "engine/render/Material.h"
#include "engine/render/RenderEnums.h"
#include <cassert>
#include <cstdint>
#include <cstdio>

namespace AbyssEngine {
struct PCResourceView {
    uint16_t id;
    uint8_t pad02[2];
    int type;
    int handle;
    void *payload;
};
struct PCMaterialInfoView {
    uint16_t textureIds[8];
    uint32_t flags0, flags1, flags2;
    float vec[3];
};
struct PaintCanvas {
    Array<Material *> materials;
    PCResourceView *resources[3] = {};
    int textureCreates = 0;

    void *FindResource(unsigned short id) {
        for (PCResourceView *res : resources)
            if (res && res->id == id) return res;
        return nullptr;
    }
    void TextureCreate(unsigned short id, unsigned &out, bool flag) {
        assert(id == 11 && flag);
        PCResourceView *res = (PCResourceView *) FindResource(id);
        res->handle = 42;
        out = 42;
        ++textureCreates;
    }
    void MaterialCreate(unsigned &out, BlendMode mode, unsigned textures, unsigned short p4);
    void MaterialCreate(unsigned short resId, unsigned &out);
};

// PRODUCTION_SIMPLE
// PRODUCTION_RESOURCE
}

int main() {
    using namespace AbyssEngine;
    PaintCanvas canvas;
    unsigned out = 77;
    canvas.MaterialCreate(out, (BlendMode) 3, 123, 0);
    assert(out == 0 && canvas.materials.size_ == 1);
    assert(canvas.materials[0]->textures[0] == 123);
    assert(canvas.materials[0]->materialMode == 3);
    assert(canvas.materials[0]->ambientColor.x == -10.0f);

    PCMaterialInfoView info = {};
    for (auto &id : info.textureIds) id = 0xffff;
    info.textureIds[0] = 10;
    info.textureIds[1] = 11;
    info.textureIds[3] = 12;
    info.flags0 = 7;
    info.vec[0] = 0.25f; info.vec[1] = 0.5f; info.vec[2] = 0.75f;
    PCResourceView mainRes = {100, {}, 0, -1, &info};
    PCResourceView firstTex = {10, {}, 0, 5, nullptr};
    PCResourceView secondTex = {11, {}, 0, -1, nullptr};
    canvas.resources[0] = &mainRes;
    canvas.resources[1] = &firstTex;
    canvas.resources[2] = &secondTex;
    canvas.MaterialCreate((unsigned short) 100, out);
    assert(out == 1 && mainRes.handle == 1 && canvas.materials.size_ == 2);
    assert(canvas.textureCreates == 1 && secondTex.handle == 42);
    Material *mat = canvas.materials[1];
    assert(mat->textures[0] == 5 && mat->textures[1] == 42);
    assert(mat->textures[2] == -1 && mat->textures[3] == -1);
    assert(mat->materialMode == 7 && mat->addData == nullptr && mat->addDataSize == 0);
    assert(mat->ambientColor.x == 0.25f && mat->ambientColor.y == 0.5f);
    assert(mat->ambientColor.z == 0.75f);

    canvas.MaterialCreate((unsigned short) 100, out);
    assert(out == 1 && canvas.materials.size_ == 2 && canvas.textureCreates == 1);
    out = 77;
    canvas.MaterialCreate((unsigned short) 999, out);
    assert(out == 77 && canvas.materials.size_ == 2);
    delete canvas.materials[0];
    delete canvas.materials[1];
    std::puts("MaterialCreate host smoke: OK");
}
