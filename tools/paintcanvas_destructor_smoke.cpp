#include <cassert>
#include <cstdint>
#include <new>

struct PCResourceView {
    unsigned short id;
    unsigned short pad;
    int type;
    int handle;
    void *payload;
};

struct PCArrayHeader {
    std::uint32_t count;
    void *data;
    std::uint32_t capacity;
};

template<class T> struct Array {
    unsigned count = 0;
    T *data_ = nullptr;
    unsigned capacity = 0;
    ~Array() { ::operator delete[](data_); }
};

namespace AbyssEngine {
static int releaseCalls = 0;
static int textureDtors = 0;
static int resourceMeshDtors = 0;
static int transformDtors = 0;
static int stringDtors = 0;
static int meshDtors = 0;
static int meshReleases = 0;

struct Engine {};
struct Mesh { ~Mesh() { ++meshDtors; } };
struct ResourceTexture { ~ResourceTexture() { ++textureDtors; } };
struct ResourceMesh { ~ResourceMesh() { ++resourceMeshDtors; } };
struct ResourceTransform { ~ResourceTransform() { ++transformDtors; } };
struct String { ~String() { ++stringDtors; } };
struct AELoadedTexture { unsigned glId; String name; };
struct SpriteSystem {};
struct Material {};
struct Camera {};
struct Image2D {};
struct ImageFont {};

void MeshRelease(Engine *, Mesh **slot) {
    ++meshReleases;
    delete *slot;
    *slot = nullptr;
}

struct PaintCanvas {
    Engine *engine = nullptr;
    Array<PCResourceView *> resources;
    Array<AELoadedTexture *> cubeTextures;
    Array<ImageFont *> fonts;
    Array<Image2D *> images;
    unsigned meshCount = 0;
    char **meshes = nullptr;
    char *mask2dImage = nullptr;
    unsigned transformCount = 0;
    char **transforms = nullptr;
    unsigned transforms_cap = 0;
    Array<Camera *> cameras;
    Array<Material *> materials;
    Array<SpriteSystem *> spriteSystems;
    unsigned glowMeshes_count = 0;
    char *glowMeshes_data = nullptr;
    unsigned glowMeshes_cap = 0;
    unsigned glowMatA_count = 0;
    char *glowMatA_data = nullptr;
    unsigned glowMatA_cap = 0;
    unsigned glowMatB_count = 0;
    char *glowMatB_data = nullptr;
    unsigned glowMatB_cap = 0;
    unsigned glowUints_count = 0;
    char *glowUints_data = nullptr;
    unsigned glowUints_cap = 0;
    unsigned glowMatC_count = 0;
    char *glowMatC_data = nullptr;
    unsigned glowMatC_cap = 0;
    Mesh *quad2dMesh = nullptr;
    Mesh *lineMesh = nullptr;

    void ReleaseAllResources() { ++releaseCalls; }
    ~PaintCanvas();
};
}

using namespace AbyssEngine;

// PRODUCTION_METHOD

int main() {
    Engine engine;
    auto *canvas = new PaintCanvas;
    canvas->engine = &engine;
    canvas->resources.count = 5;
    canvas->resources.capacity = 5;
    canvas->resources.data_ = static_cast<PCResourceView **>(::operator new[](5 * sizeof(PCResourceView *)));
    canvas->resources.data_[0] = new PCResourceView{1, 0, 1, -1, ::operator new(8)};
    canvas->resources.data_[1] = new PCResourceView{2, 0, 2, -1, new ResourceTexture};
    canvas->resources.data_[2] = new PCResourceView{3, 0, 4, -1, new ResourceMesh};
    canvas->resources.data_[3] = new PCResourceView{4, 0, 5, -1, new ResourceTransform};
    canvas->resources.data_[4] = new PCResourceView{5, 0, 6, -1, ::operator new(8)};

    auto *glowPointee = new Mesh;
    canvas->glowMeshes_count = canvas->glowMeshes_cap = 1;
    canvas->glowMeshes_data = static_cast<char *>(::operator new[](sizeof(Mesh *)));
    *reinterpret_cast<Mesh **>(canvas->glowMeshes_data) = glowPointee;
    canvas->glowUints_count = canvas->glowUints_cap = 1;
    canvas->glowUints_data = static_cast<char *>(::operator new[](sizeof(unsigned)));
    *reinterpret_cast<unsigned *>(canvas->glowUints_data) = 0x12345678;
    canvas->glowMatA_count = canvas->glowMatA_cap = 1;
    canvas->glowMatA_data = static_cast<char *>(::operator new[](64));
    canvas->glowMatB_count = canvas->glowMatB_cap = 1;
    canvas->glowMatB_data = static_cast<char *>(::operator new[](64));
    canvas->glowMatC_count = canvas->glowMatC_cap = 1;
    canvas->glowMatC_data = static_cast<char *>(::operator new[](64));

    canvas->quad2dMesh = new Mesh;
    canvas->lineMesh = new Mesh;
    canvas->cubeTextures.count = canvas->cubeTextures.capacity = 2;
    canvas->cubeTextures.data_ = static_cast<AELoadedTexture **>(::operator new[](2 * sizeof(AELoadedTexture *)));
    canvas->cubeTextures.data_[0] = new AELoadedTexture{1, {}};
    canvas->cubeTextures.data_[1] = new AELoadedTexture{2, {}};
    delete canvas;

    assert(releaseCalls == 1);
    assert(textureDtors == 1 && resourceMeshDtors == 1 && transformDtors == 1);
    assert(stringDtors == 2 && meshReleases == 2 && meshDtors == 2);
    delete glowPointee;
    assert(meshDtors == 3);
}
