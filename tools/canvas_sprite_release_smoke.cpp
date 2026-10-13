#include <cassert>
#include <cstdint>
#include <cstdio>

template <typename T> struct Array {
    unsigned count = 0;
    T *data_ = nullptr;
};

namespace AbyssEngine {
struct Engine {};
struct Mesh {};
struct SpriteSystem {
    uint16_t count = 0, pad = 0;
    float *posCpu = nullptr;
    int16_t *sizeCpu = nullptr;
    uint8_t sharedSize = 0;
    Mesh *mesh = nullptr;
};
static int meshReleases = 0;
static Engine *lastEngine = nullptr;
void MeshRelease(Engine *engine, Mesh **slot) {
    ++meshReleases;
    lastEngine = engine;
    *slot = nullptr;
}
void SpriteSystemRelease(Engine *engine, SpriteSystem **slot);
struct PaintCanvas {
    Engine *engine = nullptr;
    Array<SpriteSystem *> spriteSystems;
    void ReleaseSpriteSystemResource(unsigned index);
};

// PRODUCTION_CANVAS
// PRODUCTION_RELEASE
}

int main() {
    using namespace AbyssEngine;
    Engine engine;
    PaintCanvas canvas;
    canvas.engine = &engine;
    SpriteSystem *slots[2] = {nullptr, nullptr};
    canvas.spriteSystems = {2, slots};
    canvas.ReleaseSpriteSystemResource(2);
    canvas.ReleaseSpriteSystemResource(0);
    assert(meshReleases == 0 && slots[0] == nullptr);

    slots[0] = new SpriteSystem;
    slots[0]->posCpu = new float[6];
    slots[0]->sizeCpu = new int16_t[2];
    Mesh childMesh;
    slots[0]->mesh = &childMesh;
    SpriteSystem *other = new SpriteSystem;
    slots[1] = other;
    canvas.ReleaseSpriteSystemResource(0);
    assert(slots[0] == nullptr && slots[1] == other);
    assert(meshReleases == 1 && lastEngine == &engine);
    canvas.ReleaseSpriteSystemResource(0);
    assert(meshReleases == 1);
    delete other;
    std::puts("PaintCanvas sprite-system release smoke: OK");
}
