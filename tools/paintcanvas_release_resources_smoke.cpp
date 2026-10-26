#include <cassert>
#include <cstdint>

using GLuint = unsigned;
static int deletedTextures = 0;
static GLuint deletedTextureId = 0;
extern "C" void glDeleteTextures(int count, const GLuint *ids) {
    deletedTextures += count;
    deletedTextureId = ids[0];
}

struct PCResourceView {
    unsigned short id;
    unsigned short pad;
    int type;
    int handle;
    void *payload;
};

struct ArrayHeader { unsigned count; void *data; unsigned capacity; };
static void PCArrayRemoveAll(void *header) {
    static_cast<ArrayHeader *>(header)->count = 0;
}

template<class T> struct Array {
    unsigned count = 0;
    T *data_ = nullptr;
    unsigned capacity = 0;
};

namespace AbyssEngine {
int CubeMapSetIndex = 7;
static int stringDestructors = 0;
static int materialDestructors = 0;
static int transformDestructors = 0;
static int fontReleases = 0;
static int imageReleases = 0;
static int meshReleases = 0;
static int spriteReleases = 0;

struct Engine {
    static int ImageCount;
    static int vboSize;
    unsigned field_0x70 = 100;
};
int Engine::ImageCount = 5;
int Engine::vboSize = 200;

struct String { ~String() { ++stringDestructors; } };
struct AELoadedTexture {
    std::uint32_t glId;
    String name;
    unsigned byteSize;
};
using Resource = ::PCResourceView;
struct ImageFont {};
struct Image2D {};
struct Mesh { int vboByteSize; };
struct Transform { ~Transform() { ++transformDestructors; } };
struct Camera {};
struct Material { ~Material() { ++materialDestructors; } };
struct SpriteSystem {};

void ImageFontRelease(Engine *, ImageFont **slot) {
    ++fontReleases;
    delete *slot;
    *slot = nullptr;
}
void Image2DRelease(Engine *, Image2D **slot) {
    ++imageReleases;
    delete *slot;
    *slot = nullptr;
}
void MeshRelease(Engine *, Mesh **slot) {
    ++meshReleases;
    delete *slot;
    *slot = nullptr;
}
void SpriteSystemRelease(Engine *, SpriteSystem **slot) {
    ++spriteReleases;
    delete *slot;
    *slot = nullptr;
}

struct PaintCanvas {
    Engine *engine;
    Array<Resource *> resources;
    Array<AELoadedTexture *> cubeTextures;
    Array<ImageFont *> fonts;
    Array<Image2D *> images;
    unsigned meshCount = 0;
    char **meshes = nullptr;
    unsigned meshCapacity = 0;
    unsigned transformCount = 0;
    char **transforms = nullptr;
    unsigned transformCapacity = 0;
    Array<Camera *> cameras;
    unsigned currentCamera = 3;
    Array<Material *> materials;
    Array<SpriteSystem *> spriteSystems;
    unsigned field_0x1cc = 1;

    explicit PaintCanvas(Engine *value) : engine(value) {}
    void ReleaseAllResources();
};
}

using namespace AbyssEngine;

// PRODUCTION_METHOD

int main() {
    Engine engine;
    PaintCanvas canvas(&engine);
    Resource record{1, 0, 0, 2, nullptr};
    Resource *resourceSlots[] = {&record};
    canvas.resources = {1, resourceSlots, 1};

    AELoadedTexture *textureSlots[] = {
        new AELoadedTexture{42, {}, 64},
        new AELoadedTexture{UINT32_MAX, {}, 13}
    };
    canvas.cubeTextures = {2, textureSlots, 2};
    ImageFont *fontSlots[] = {new ImageFont, nullptr};
    canvas.fonts = {2, fontSlots, 2};
    Image2D *imageSlots[] = {new Image2D, nullptr};
    canvas.images = {2, imageSlots, 2};
    char *meshSlots[] = {reinterpret_cast<char *>(new Mesh{27}), nullptr};
    canvas.meshCount = 2;
    canvas.meshes = meshSlots;
    canvas.meshCapacity = 2;
    char *transformSlots[] = {reinterpret_cast<char *>(new Transform)};
    canvas.transformCount = 1;
    canvas.transforms = transformSlots;
    canvas.transformCapacity = 1;
    Camera *cameraSlots[] = {new Camera};
    canvas.cameras = {1, cameraSlots, 1};
    Material *materialSlots[] = {new Material};
    canvas.materials = {1, materialSlots, 1};
    SpriteSystem *spriteSlots[] = {new SpriteSystem};
    canvas.spriteSystems = {1, spriteSlots, 1};

    canvas.ReleaseAllResources();
    assert(record.handle == -1 && CubeMapSetIndex == 0);
    assert(deletedTextures == 1 && deletedTextureId == 42);
    assert(Engine::ImageCount == 4 && engine.field_0x70 == 36);
    assert(stringDestructors == 2 && textureSlots[0] == nullptr && textureSlots[1] == nullptr);
    assert(fontReleases == 1 && imageReleases == 1);
    assert(meshReleases == 1 && Engine::vboSize == 173);
    assert(transformDestructors == 1 && materialDestructors == 1);
    assert(spriteReleases == 1);
    assert(canvas.cubeTextures.count == 0 && canvas.fonts.count == 0);
    assert(canvas.images.count == 0 && canvas.meshCount == 0);
    assert(canvas.transformCount == 0 && canvas.cameras.count == 0);
    assert(canvas.materials.count == 0 && canvas.spriteSystems.count == 0);
    assert(meshSlots[0] == nullptr && transformSlots[0] == nullptr);
    assert(cameraSlots[0] == nullptr && materialSlots[0] == nullptr);
    assert(spriteSlots[0] == nullptr && canvas.currentCamera == UINT32_MAX);
    assert(canvas.field_0x1cc == 0);
}
