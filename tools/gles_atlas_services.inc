// Included only by the real GLES gate, never by the CPU boundary gate.
#include "engine/render/LensFlare.h"
#include "engine/core/ApplicationManager.h"
#include "gles_atlas_context.h"
using namespace AbyssEngine;
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(Engine, appManager) == 0x30, "ARM Engine manager");
static_assert(offsetof(ApplicationManager, paintCanvas) == 0, "ARM manager canvas");
static_assert(offsetof(PaintCanvas, cubeTextures) == 0x10, "ARM managed textures");
static_assert(sizeof(Image) == 0x14 && sizeof(AELoadedTexture) == 0x1c, "ARM texture records");
#endif
struct FlareImageBoundary {};
static unsigned flareUploadCalls;
unsigned char Globals::iPad, Globals::iPadHD, Globals::retinaDisplay;
unsigned char Globals::iPadLarge, Globals::iPadAssetsWithLowerRes;
bool Engine::clampTextures;
float Engine::AnisotropyValue;
int Engine::ImageCount;

// Explicit fixture bootstrap. Production manager construction is outside this gate.
ApplicationManager::ApplicationManager(Engine *value) : paintCanvas(nullptr), engine(value) {}
ApplicationManager::~ApplicationManager() {}

unsigned int AELabelObject(unsigned int, unsigned int, const char *);
extern "C" void glTexEnvi(unsigned int, unsigned int, int);
#include "gles_atlas_compat_under_test.inc"
#include "lensflare_cpu_under_test.inc"
#include "lensflare_platform_under_test.inc"

uint32_t *SolarSystem::getStations() {
    // The next untouched world-loading boundary, after LensFlare has returned.
    throw FlareImageBoundary{};
}

int AbyssEngine::flareTextureBoundary(Engine *engine, const char *path, ImageCallback callback,
                                     void *user, unsigned int *out, float scale,
                                     AELoadedTexture *record, bool managed) {
    assert(!callback && !user && !record && managed && scale == 0.0f && *out == 0);
    assert(engine->boundTextures[0] == -1 && engine->boundTextures[1] == -1);
    ++flareUploadCalls;
    int result = FlareUpload::TextureCreateFromFileIntern(engine, path, callback, user, out, scale, record, managed);
    auto &textures = engine->appManager->paintCanvas->cubeTextures;
    assert(result == 1 && *out == 0 && textures.size() == 1);
    auto *texture = textures[0];
    assert(texture && texture->glId != 0 && texture->glId != 0xffffffffU);
    assert(texture->valid == 1 && texture->isCube == 0 && texture->scale == 0);
    char *name = texture->name.GetAEChar();
    assert(std::strcmp(name, path) == 0);
    delete[] name;
    unsigned side = std::strcmp(path, "data/textures/gof2_interface.aei") == 0 ? 1024 : 2048;
    assert(texture->byteSize == side * side * 4);
    assert(engine->textureByteCounter == int(texture->byteSize) && Engine::ImageCount == 1);
    // Independent AEI offset: 15-byte header followed by N eight-byte regions.
    assert(input.size() > 15 && input[8] == 1);
    size_t pixelOffset = 15 + 8 * (unsigned(input[13]) | unsigned(input[14]) << 8);
    assert(cursor == pixelOffset + texture->byteSize && cursor <= input.size());
    checkGpuPixels(texture->glId, side, input.data() + pixelOffset);
    return result;
}

static void checkGlBoundary(PaintCanvas &canvas) {
    assert(flareUploadCalls == 1 && canvas.cubeTextures.size() == 1 && canvas.images.size() == 3);
    auto *texture = static_cast<Resource *>(canvas.FindResource(10062));
    assert(texture && texture->handle == 0); // Managed index, not the GL name.
    const unsigned xy[][2] = {{71, 934}, {160, 902}, {285, 601}};
    for (unsigned i = 0; i < 3; ++i) {
        auto *resource = static_cast<Resource *>(canvas.FindResource(1288 + i));
        auto *region = canvas.images[i];
        assert(resource && resource->handle == int(i));
        assert(region->textureId == 0 && region->restoreFlag == 0);
        assert(region->atlasW == 1024 && region->atlasH == 1024);
        assert(region->offX == xy[i][0] && region->offY == xy[i][1]);
        assert(region->sizeX == 64 && region->sizeY == 64 && region->mesh);
        unsigned cached = 99;
        canvas.Image2DCreate(1288 + i, cached);
        assert(cached == i && flareUploadCalls == 1);
    }
    assert(glGetError() == GL_NO_ERROR);
    auto *loaded = canvas.cubeTextures[0];
    glDeleteTextures(1, &loaded->glId);
    assert(!glIsTexture(loaded->glId));
    delete loaded; // Test cleanup; full canvas destruction is a separate gate.
    canvas.cubeTextures[0] = nullptr;
}
