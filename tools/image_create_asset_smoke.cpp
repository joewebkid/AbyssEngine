#include "engine/render/Image.h"
#include "engine/core/Array.h"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <new>
#include <string>
#include <vector>

namespace AbyssEngine {
class Engine { public: int boundTextures[2] = {0, 0}; };
class AELoadedTexture;
void ImageRelease(Image **slot);
int ImageCreateFromFile(Engine *engine, const char *path, Image **out);
int TextureCreateFromFile(Engine *, const char *, void (*)(Image *, void *),
                          void *, unsigned int *, bool, float);
int TextureCreateFromFileIntern(Engine *, const char *, void (*)(Image *, void *),
                                void *, unsigned int *, float, AELoadedTexture *, bool);
}

static bool captureAllocation;
static std::size_t imageAllocation;
void *operator new(std::size_t size) {
    if (captureAllocation) {
        imageAllocation = size;
        captureAllocation = false;
    }
    if (void *data = std::malloc(size)) return data;
    throw std::bad_alloc();
}
void operator delete(void *data) noexcept { std::free(data); }

struct AEFile {
    static std::vector<unsigned char> bytes;
    static std::string expectedPath;
    static std::size_t cursor, limit;
    static unsigned opens, closes;
    static uint32_t OpenRead(const char *path, uint32_t *handle) {
        assert(path && *path && !bytes.empty());
        if (!expectedPath.empty()) assert(expectedPath == path);
        *handle = 1;
        ++opens;
        return 1;
    }
    static uint32_t Read(uint32_t size, void *data, uint32_t handle) {
        assert(handle == 1);
        if (cursor + size > limit) return 0;
        std::memcpy(data, bytes.data() + cursor, size);
        cursor += size;
        return size;
    }
    static uint32_t Skip(uint32_t size, uint32_t handle) {
        assert(handle == 1);
        if (cursor + size > limit) return 0;
        cursor += size;
        return size;
    }
    static void Close(uint32_t handle) { assert(handle == 1); ++closes; }
};
std::vector<unsigned char> AEFile::bytes;
std::string AEFile::expectedPath;
std::size_t AEFile::cursor, AEFile::limit;
unsigned AEFile::opens, AEFile::closes;

#include "image_create_asset_under_test.inc"

struct PCResourceView {
    uint16_t id;
    uint8_t pad02[2];
    int type;
    int handle;
    void *payload;
};
struct PCTexInfoView { char *path; float value; };
namespace AbyssEngine {
class PaintCanvas {
public:
    Array<PCResourceView *> resources;
    Engine *engine = nullptr;
    void *FindResource(unsigned short id);
    void TextureCreate(unsigned short id, void (*callback)(Image *, void *),
                       void *userData, unsigned int &out, bool useCallbackLoader);
    void TextureCreate(unsigned short id, unsigned int &out, bool flag);
};
static unsigned int nextTexture = 100;
int TextureCreateFromFile(Engine *, const char *, void (*)(Image *, void *),
                          void *, unsigned int *, bool, float) { std::abort(); }
int TextureCreateFromFileIntern(Engine *engine, const char *path, void (*)(Image *, void *),
                                void *, unsigned int *out, float, AELoadedTexture *, bool) {
    Image *image = nullptr;
    if (ImageCreateFromFile(engine, path, &image) != 1) return -1;
    assert(image && image->format == 11);
    ImageRelease(&image);
    *out = nextTexture++;
    return 1;
}
}
using AbyssEngine::PaintCanvas;
static_assert(offsetof(PCResourceView, payload) == 16, "resource host payload");
static_assert(offsetof(PCTexInfoView, value) == 8, "texture info host scale");

#include "image_canvas_under_test.inc"

int main(int argc, char **argv) {
    assert(argc > 1 && sizeof(AbyssEngine::Image) > 0x14);
    AbyssEngine::Engine engineObject;
    auto *engine = &engineObject;
    for (int i = 1; i < argc; ++i) {
        std::ifstream file(argv[i], std::ios::binary);
        assert(file.good());
        AEFile::bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        assert(!AEFile::bytes.empty());

        AEFile::cursor = 0;
        AEFile::limit = AEFile::bytes.size();
        AEFile::opens = AEFile::closes = 0;
        AbyssEngine::Image *image = nullptr;
        captureAllocation = true;
        assert(AbyssEngine::ImageCreateFromFile(engine, argv[i], &image) == 1);
        assert(imageAllocation == sizeof(AbyssEngine::Image));
        assert(image && image->width == 1024 && image->height == 1024);
        assert(image->format == 11 && image->hasMipmaps == 1);
        assert(image->data && image->dataLen > 0);
        std::fprintf(stderr, "%s: parsed=%zu file=%zu data=%u\n", argv[i],
                     AEFile::cursor, AEFile::bytes.size(), image->dataLen);
        assert(AEFile::bytes.size() - AEFile::cursor == 2);
        const std::size_t parsedLength = AEFile::cursor;
        assert(AEFile::opens == 1 && AEFile::closes == 1);
        AbyssEngine::ImageRelease(&image);
        assert(image == nullptr);

        AEFile::cursor = 0;
        AEFile::limit = parsedLength - 1;
        AEFile::opens = AEFile::closes = 0;
        assert(AbyssEngine::ImageCreateFromFile(engine, argv[i], &image) == -1);
        assert(image == nullptr && AEFile::opens == 1 && AEFile::closes == 1);
    }
    if (argc == 3) {
        PaintCanvas canvas;
        canvas.engine = engine;
        PCTexInfoView infos[2] = {{argv[1], -1.0f}, {argv[2], -1.0f}};
        PCResourceView resources[2] = {
            {0x2767, {0, 0}, 2, -1, &infos[0]},
            {0x2753, {0, 0}, 2, -1, &infos[1]},
        };
        ArrayAdd(&resources[0], canvas.resources);
        ArrayAdd(&resources[1], canvas.resources);
        const unsigned short ids[] = {0x2767, 0x2753};
        for (int i = 0; i < 2; ++i) {
            std::ifstream file(argv[i + 1], std::ios::binary);
            assert(file.good());
            AEFile::bytes.assign(std::istreambuf_iterator<char>(file),
                                 std::istreambuf_iterator<char>());
            AEFile::cursor = 0;
            AEFile::limit = AEFile::bytes.size();
            AEFile::opens = AEFile::closes = 0;
            AEFile::expectedPath = argv[i + 1];
            unsigned int out = 0xffffffff;
            canvas.TextureCreate(ids[i], out, false);
            assert(out == unsigned(100 + i) && resources[i].handle == int(out));
            assert(AEFile::opens == 1 && AEFile::closes == 1);
            assert(engine->boundTextures[0] == -1 && engine->boundTextures[1] == -1);
            canvas.TextureCreate(ids[i], out, false);
            assert(out == unsigned(100 + i) && AEFile::opens == 1);
        }
    }
}
