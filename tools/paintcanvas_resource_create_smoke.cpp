#include <cassert>
#include <cstdint>

template<class T> struct Array {
    unsigned count = 0;
    T *data_ = nullptr;
    unsigned capacity = 0;
};

template<class T> void ArrayAdd(T value, Array<T> &array) {
    assert(array.count < array.capacity);
    array.data_[array.count++] = value;
}

namespace AbyssEngine {
struct Engine {};
struct Material {};
struct Mesh { void *material = nullptr; };
struct SpriteSystem { Mesh *mesh = nullptr; };

static Mesh createdMesh;
static SpriteSystem createdSprite{&createdMesh};
static int meshResult = 1, spriteResult = 1;
static int meshCalls = 0, spriteCalls = 0;

int MeshCreate(Engine *, unsigned short, unsigned short, signed char, Mesh **out) {
    ++meshCalls;
    *out = meshResult == 1 ? &createdMesh : nullptr;
    return meshResult;
}

int SpriteSystemCreate(Engine *, unsigned short, bool, SpriteSystem **out) {
    ++spriteCalls;
    *out = spriteResult == 1 ? &createdSprite : nullptr;
    return spriteResult;
}

struct PaintCanvas {
    Engine *engine;
    Array<Material *> materials;
    Array<SpriteSystem *> spriteSystems;
    unsigned meshCount = 0;
    Mesh **meshes = nullptr;
    unsigned meshCapacity = 0;

    explicit PaintCanvas(Engine *value) : engine(value) {}
    void MaterialCreate(unsigned short, unsigned &out) { out = 0; }
    void SpriteSystemCreate(unsigned short, bool, unsigned short, unsigned &);
    void SpriteSystemCreate(unsigned short, bool, unsigned &);
    void MeshCreate(unsigned short, unsigned short, signed char, unsigned short, unsigned &);
    void MeshCreate(unsigned short, unsigned short, signed char, unsigned &);
};
}

using namespace AbyssEngine;

// PRODUCTION_METHODS

int main() {
    Engine engine;
    Material material;
    Material *materialSlots[1] = {&material};
    SpriteSystem *spriteSlots[4] = {};
    Mesh *meshSlots[4] = {};
    PaintCanvas canvas(&engine);
    canvas.materials = {1, materialSlots, 1};
    canvas.spriteSystems = {1, spriteSlots, 4};
    canvas.meshes = meshSlots;
    canvas.meshCapacity = 4;

    unsigned id = 0xffffffff;
    canvas.SpriteSystemCreate(12, false, 31, id);
    assert(spriteCalls == 1 && id == 0 && spriteSlots[0] == &createdSprite);
    assert(createdMesh.material == &material);
#if INTPTR_MAX > INT32_MAX
    assert(reinterpret_cast<std::uintptr_t>(&material) > UINT32_MAX);
#endif

    canvas.SpriteSystemCreate(12, true, id);
    assert(spriteCalls == 2 && id == 1 && spriteSlots[1] == &createdSprite);

    canvas.MeshCreate(4, 2, 0x11, 31, id);
    assert(meshCalls == 1 && id == 0 && meshSlots[0] == &createdMesh);
    assert(createdMesh.material == &material);

    canvas.MeshCreate(4, 2, 0x11, id);
    assert(meshCalls == 2 && id == 1 && meshSlots[1] == &createdMesh);

    spriteResult = meshResult = -4;
    canvas.SpriteSystemCreate(12, false, id);
    assert(id == 0xffffffff && canvas.spriteSystems.count == 2);
    canvas.MeshCreate(4, 2, 0x11, id);
    assert(id == 0xffffffff && canvas.meshCount == 2);
}
