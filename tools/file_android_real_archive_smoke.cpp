#include "game/core/String.h"
#include "engine/core/GameText.h"
#include <archive.h>
#include <archive_entry.h>
#include <openssl/sha.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
}

struct zip { std::string path; };
struct zip_file { archive *reader; };
static std::vector<zip_file *> archiveHandles;

zip *zip_open(const char *path, int flags, int *error) {
    assert(flags == 0 && error == nullptr);
    return std::filesystem::is_regular_file(path) ? new zip{path} : nullptr;
}
zip_file *zip_fopen(zip *source, const char *name, int flags) {
    assert(flags == 0);
    if (!source) return nullptr;
    archive *reader = archive_read_new();
    archive_read_support_format_zip(reader);
    if (archive_read_open_filename(reader, source->path.c_str(), 10240) != ARCHIVE_OK) {
        archive_read_free(reader);
        return nullptr;
    }
    archive_entry *entry;
    while (archive_read_next_header(reader, &entry) == ARCHIVE_OK) {
        if (std::strcmp(archive_entry_pathname(entry), name) == 0) {
            auto *file = new zip_file{reader};
            archiveHandles.push_back(file);
            return file;
        }
        archive_read_data_skip(reader);
    }
    archive_read_free(reader);
    return nullptr;
}
long long zip_fread(zip_file *file, void *buffer, unsigned n) {
    unsigned total = 0;
    while (total < n) {
        const auto got = archive_read_data(file->reader,
                                           static_cast<char *>(buffer) + total, n - total);
        if (got <= 0) return got < 0 ? -1 : total;
        total += static_cast<unsigned>(got);
    }
    return total;
}
int zip_fclose(zip_file *file) {
    if (file->reader) archive_read_free(file->reader);
    file->reader = nullptr;
    return 0;
}

class FileInterfaceAndroid {
public:
    int enabled = 1;
    FILE *file = nullptr;
    zip_file *zipFile = nullptr;
    void *jniStream = nullptr;
    bool modeFlag = false;
    int zipReadPos = 0;
    int zipReadLen = 0;
    bool zipAppend = false;
    void *appRootDir = nullptr;
    static int fileCounter;
    static void **gZipMain;
    static void **gZipPatch;
    FileInterfaceAndroid() = default;
    FileInterfaceAndroid(FILE *stream, bool append);
    FileInterfaceAndroid(zip_file *stream, bool append, int offset, int, int);
    ~FileInterfaceAndroid() {
        if (file) std::fclose(file);
        if (zipFile) zip_fclose(zipFile);
    }
    uint32_t Seek(uint32_t n);
    uint32_t FileExist(String name);
    void *OpenRead(String name, int p2, bool p3, int p4, int p5, unsigned p6);
};
int FileInterfaceAndroid::fileCounter = 0;

#include "file_android_readexist_under_test.inc"

static std::vector<unsigned char> readBytes(FileInterfaceAndroid *file, size_t size) {
    assert(file && file->zipFile);
    std::vector<unsigned char> bytes(size);
    assert(zip_fread(file->zipFile, bytes.data(), size) == static_cast<long long>(size));
    delete file;
    return bytes;
}
static void checkSha256(const std::vector<unsigned char> &bytes, const char *expected) {
    uint64_t context[0x70 / sizeof(uint64_t)];
    unsigned char digest[32];
    SHA256_Init(context);
    SHA256_Update(context, bytes.data(), bytes.size());
    SHA256_Final(digest, context);
    char hex[65];
    for (int i = 0; i < 32; ++i) std::sprintf(hex + 2 * i, "%02x", digest[i]);
    assert(std::strcmp(hex, expected) == 0);
}

int main(int argc, char **argv) {
    assert(argc == 3);
    assert(loadAPKAndZip(argv[1], argv[2]));
    FileInterfaceAndroid reader;
    assert(reader.FileExist(String("data/bin/items.bin")));
    auto *items = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/items.bin"), 18, false, 0, 0, 0));
    checkSha256(readBytes(items, 27628),
                "639b1da183594b9ca024a528d95082152591e49c9a685b6e42419bff931d7f4c");

    assert(reader.FileExist(String("data/bin/stations.bin")));
    auto *stations = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/stations.bin"), 21, false, 0, 0, 0));
    checkSha256(readBytes(stations, 3408),
                "761e84019f4c0ae6f4c8e160252be56add2203d1325af2571c94fa7e45c7baa3");

    auto *offsetItems = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/items.bin"), 18, true, 0, 0, 4));
    assert(offsetItems && offsetItems->zipFile && offsetItems->zipAppend);
    auto suffix = readBytes(offsetItems, 27624);
    auto *fullItems = static_cast<FileInterfaceAndroid *>(
        reader.OpenRead(String("data/bin/items.bin"), 18, false, 0, 0, 0));
    auto full = readBytes(fullItems, 27628);
    assert(std::memcmp(suffix.data(), full.data() + 4, suffix.size()) == 0);

    delete static_cast<zip *>(*FileInterfaceAndroid::gZipMain);
    delete static_cast<zip *>(*FileInterfaceAndroid::gZipPatch);
    for (auto *handle : archiveHandles) {
        zip_fclose(handle);
        delete handle;
    }
    std::puts("Original APK/OBB archive selection and offset: OK");
}
