// Bootstrap classes are host services; all exercised game bodies are source-extracted.
#include "engine/render/ResourceTexture.h"
#include "engine/render/ResourceMaterial.h"
#include "engine/render/ResourceMesh.h"
#include "engine/render/Image.h"
#include "engine/render/Image2D.h"
#include "engine/render/ImageFactoryPortraitData.h"
#include "game/core/String.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace AbyssEngine {
struct Resource;
struct Engine;
struct AELoadedTexture;
void ImageRelease(Image **);
struct PaintCanvas {
    Array<Resource *> resources;
    Array<Image2D *> images;
    Engine *engine;
    void SetResourceList(Resource *const *, unsigned int);
    void AddResource(Resource *);
    void *FindResource(unsigned short);
    void Image2DCreate(unsigned short, unsigned int &);
    void TextureCreate(unsigned short, void (*)(Image *, void *), void *, unsigned int &, bool);
    void TextureCreate(unsigned short, unsigned int &, bool);
};
struct ApplicationManager { PaintCanvas *paintCanvas; };
struct Engine { ApplicationManager *appManager; int boundTextures[2] = {9, 9}; };
int TextureCreateFromFile(Engine *, const char *, void (*)(Image *, void *), void *, unsigned int *, bool, float) {
    std::abort();
}
int TextureCreateFromFileIntern(Engine *, const char *, void (*)(Image *, void *), void *,
                                unsigned int *, float, AELoadedTexture *, bool);
int ImageCreateRegionFromFile(Engine *, const char *, unsigned short, Image2D *) { std::abort(); }
}
struct Globals {
    static unsigned char iPadLarge, iPadHD, retinaDisplay, iPad, n9;
    static AbyssEngine::PaintCanvas *Canvas;
};
unsigned char Globals::iPadLarge, Globals::iPadHD, Globals::retinaDisplay, Globals::iPad, Globals::n9;
AbyssEngine::PaintCanvas *Globals::Canvas;
void String::Reverse() { std::abort(); }
char *String_GetAEChar(String *s) { return s->GetAEChar(); }
void loadPortraits(AbyssEngine::Engine *);

// RESOURCE_REGISTRATION

struct PCResourceView {
    unsigned short id;
    int type, handle;
    void *payload;
};
struct PCTexInfoView { char *path; float value; };
static_assert(sizeof(PCResourceView) == sizeof(AbyssEngine::Resource));
static_assert(offsetof(PCResourceView, payload) == offsetof(AbyssEngine::Resource, payload));
static_assert(offsetof(PCResourceView, handle) == offsetof(AbyssEngine::Resource, unused));
using AbyssEngine::PaintCanvas;
using AbyssEngine::Image2D;

// loadImage must stop at upload before it allocates an ImagePart.
struct ImagePart { ImagePart(unsigned, int, int) { std::abort(); } };
struct ImageFactory { void *loadImage(int, int, int); };

struct AEFile {
    static inline std::filesystem::path root;
    static inline std::vector<char> bytes;
    static inline size_t cursor;
    static inline unsigned opens, closes, misses;
    static uint32_t OpenRead(const char *path, uint32_t *handle) {
        std::ifstream input(root / path, std::ios::binary);
        if (!input) { ++misses; return 0; }
        bytes.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
        cursor = 0;
        *handle = 1;
        ++opens;
        return 1;
    }
    static uint32_t Read(uint32_t size, void *out, uint32_t handle) {
        assert(handle == 1);
        if (size > bytes.size() - cursor) return 0;
        std::memcpy(out, bytes.data() + cursor, size);
        cursor += size;
        return size;
    }
    static uint32_t Skip(uint32_t size, uint32_t handle) {
        assert(handle == 1);
        if (size > bytes.size() - cursor) return 0;
        cursor += size;
        return size;
    }
    static void Close(uint32_t handle) { assert(handle == 1); ++closes; }
};

// PRODUCTION_BODIES

struct PortraitUploadBoundary {};
static std::string expectedUpload;
static unsigned uploadCalls;
int AbyssEngine::TextureCreateFromFileIntern(Engine *engine, const char *path,
    void (*callback)(Image *, void *), void *user, unsigned int *out, float scale,
    AELoadedTexture *record, bool managed) {
    assert(path == expectedUpload && !callback && !user && !record && managed);
    assert(scale == 0.0f && *out == 0);
    assert(engine->boundTextures[0] == -1 && engine->boundTextures[1] == -1);
    ++uploadCalls;
    throw PortraitUploadBoundary{};
}

int main(int argc, char **argv) {
    assert(argc == 2);
    AEFile::root = argv[1];
    const unsigned tailCounts[] = {234, 231, 264, 264, 235, 231, 271, 267,
                                   234, 231, 264, 264, 228, 231, 264, 267};
    unsigned cases = 0, parsed = 0, missing = 0;
    for (unsigned mask = 0; mask < 32; ++mask) {
        Globals::iPadLarge = (mask & 1) ? 0x80 : 0;
        Globals::iPadHD = (mask & 2) ? 0xff : 0;
        Globals::retinaDisplay = (mask & 4) ? 1 : 0;
        Globals::iPad = (mask & 8) ? 0xff : 0;
        Globals::n9 = (mask & 16) ? 0x80 : 0;
        const unsigned route = !!Globals::iPadHD | (!!Globals::iPad << 1) |
            (!!Globals::retinaDisplay << 2) | (!!Globals::iPadLarge << 3);
        const char *suffix = mask & 1 ? "_ipad_large" : mask & 2 ? "_ipad_1440" : mask & 28 ? "_ipad" : "";
        AbyssEngine::PaintCanvas canvas;
        AbyssEngine::ApplicationManager app{&canvas};
        AbyssEngine::Engine engine{&app};
        canvas.engine = &engine;
        Globals::Canvas = &canvas;
        registerPortraitResources(&engine);
        const unsigned start = 2488 + 152 + tailCounts[route];
        assert(canvas.resources.count == start + 152);
        ImageFactory factory;
        unsigned beforeInvalid = uploadCalls;
        assert(factory.loadImage(3, 0, 0) == nullptr && uploadCalls == beforeInvalid);
        for (unsigned i = 0; i < 152; ++i) {
            auto *texture = canvas.resources.data_[start + i];
            assert(texture->id == 10200 + i && texture->kind == 2 && texture->unused == -1);
            auto *payload = static_cast<AbyssEngine::ResourceTexture *>(texture->payload);
            assert(payload->value == 0.0f);
            // The post-table loop maps image 5000+i to texture 10200+i, region zero.
            auto *image = static_cast<AbyssEngine::Resource *>(canvas.FindResource(5000 + i));
            assert(image && image->kind == 3 && image->unused == -1);
            auto *info = static_cast<AbyssEngine::ResourceImage *>(image->payload);
            assert(info->lo == texture->id && info->hi == 0);
            int race, part, variant;
            assert(std::sscanf(payload->name, "data/textures/%d_%d_%d", &race, &part, &variant) == 3);
            // Rows 9/11/12 expose only column zero in the original consumer ID table;
            // filenames are not a universal (row,column,frame) descriptor.
            const int consumerPart = (race == 9 || race == 11 || race == 12) ? 0 : part;
            const int consumerVariant = int(image->id) - kImagePartIds[race][consumerPart];
            assert(kImagePartIds[race][consumerPart] >= 0 && consumerVariant >= 0);
            expectedUpload = "data/textures/" + std::to_string(race) + "_" +
                std::to_string(part) + "_" + std::to_string(variant) + suffix + ".aei";
            assert(expectedUpload == payload->name);
            const unsigned before = uploadCalls;
            try { factory.loadImage(race, consumerPart, consumerVariant); assert(false); }
            catch (const PortraitUploadBoundary &) {}
            assert(uploadCalls == before + 1 && canvas.images.count == 0);
            assert(texture->unused == -1 && image->unused == -1);
            // Separate CPU parser check: no handle injected and upload is never resumed.
            // Parse each of the four filename families once, including all six absences.
            if (mask == 0 || mask == 1 || mask == 2 || mask == 4) {
                AbyssEngine::Image *pixels = nullptr;
                int result = AbyssEngine::ImageCreateFromFile(&engine, payload->name, &pixels);
                bool absent = (mask == 2 && race == 12) ||
                    (mask == 4 && race == 4 && part == 2 && variant >= 3);
                if (absent) { assert(result == -1 && !pixels); ++missing; }
                else {
                    assert(result == 1 && pixels && pixels->data && pixels->dataLen);
                    assert(pixels->width && pixels->height);
                    AbyssEngine::ImageRelease(&pixels);
                    assert(!pixels);
                    ++parsed;
                }
            }
        }
        assert(canvas.images.count == 0);
        for (auto *resource : canvas.resources) {
            assert(resource->unused == -1);
            switch (resource->kind) {
            case 2: delete static_cast<AbyssEngine::ResourceTexture *>(resource->payload); break;
            case 1:
            case 3: delete static_cast<AbyssEngine::ResourceImage *>(resource->payload); break;
            case 4: delete static_cast<AbyssEngine::ResourceMesh *>(resource->payload); break;
            case 6: delete static_cast<AbyssEngine::ResourceMaterial *>(resource->payload); break;
            default: std::abort();
            }
            delete resource;
        }
        ++cases;
    }
    assert(cases == 32 && uploadCalls == 4864 && parsed == 602 && missing == 6);
    assert(AEFile::opens == 602 && AEFile::closes == 602 && AEFile::misses == 6);
    std::puts("Portrait runtime: table + complete platform tail + 152 portraits, 32 routes OK");
    std::puts("ImageFactory -> Image2DCreate -> TextureCreate: 4864 upload-entry stops, zero handles/images");
    std::puts("Separate original-file CPU parser: 602 successes, six confirmed missing variants; GPU untested");
}
