#include "engine/render/Image.h"

#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <new>
#include <vector>

namespace AbyssEngine {
class Engine;
void ImageRelease(Image **slot);
int ImageCreateFromFile(Engine *engine, const char *path, Image **out);
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
    static std::size_t cursor, limit;
    static unsigned opens, closes;
    static uint32_t OpenRead(const char *path, uint32_t *handle) {
        assert(path && *path && !bytes.empty());
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
std::size_t AEFile::cursor, AEFile::limit;
unsigned AEFile::opens, AEFile::closes;

#include "image_create_asset_under_test.inc"

int main(int argc, char **argv) {
    assert(argc > 1 && sizeof(AbyssEngine::Image) > 0x14);
    auto *engine = reinterpret_cast<AbyssEngine::Engine *>(uintptr_t(1));
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
}
