// Synthetic raw PAK exercises production AEFile registration and fallback.
#include "engine/file/AEFile.h"
#include "engine/core/GameText.h"
#include "host/ReadOnlyAssetFileInterface.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <utility>

int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
String operator+(const String &a, const String &b) {
    String result(a);
    result += b;
    return result;
}
}

static void writeEntry(std::FILE *file, const char *name, const char *data) {
    const uint8_t nameSize = static_cast<uint8_t>(std::strlen(name));
    const uint32_t packedSize = static_cast<uint32_t>(std::strlen(data));
    const uint32_t rawMarker = 0xffffffffu;
    assert(nameSize < 128);
    assert(std::fwrite(&nameSize, 1, 1, file) == 1);
    assert(std::fwrite(name, 1, nameSize, file) == nameSize);
    assert(std::fwrite(&packedSize, 4, 1, file) == 1);
    assert(std::fwrite(&rawMarker, 4, 1, file) == 1);
    assert(std::fwrite(data, 1, packedSize, file) == packedSize);
}

int main(int argc, char **argv) {
    assert(argc == 2);
    const auto root = std::filesystem::path(argv[1]);
    const auto pak = root / "fixture.pak";
    std::FILE *file = std::fopen(pak.string().c_str(), "wb");
    assert(file);
    writeEntry(file, "/assets/gamma.bin", "GAMMA");
    writeEntry(file, "/assets/alpha.bin", "ALPHA");
    writeEntry(file, "/assets/beta.bin", "BETA");
    assert(std::fclose(file) == 0);

    ReadOnlyAssetFileInterface interface;
    assert(interface.addMount("pak/", root));
    assert(interface.OpenRead(String("pak/fixture.pak"), 0, false, 0, 0, 1000) == nullptr);
    AEFile::SetInterface(&interface);
    assert(AEFile::crc32_ccitt(String("/assets/gamma.bin")) >
           AEFile::crc32_ccitt(String("/assets/alpha.bin")));
    assert(AEFile::crc32_ccitt(String("/assets/alpha.bin")) >
           AEFile::crc32_ccitt(String("/assets/beta.bin")));
    AEFile::RegisterPakFile(String("pak/fixture.pak"));
    assert(interface.opens() == 1 && interface.closes() == 1);
    assert(AEFile::FileExist(String("pak/fixture.pak")) == 1);
    assert(AEFile::FileExist(String("assets/missing.bin")) == 0);

    auto *entry = static_cast<AEPakFile *>(AEFile::findPakFile(String("/assets/alpha.bin")));
    assert(entry != nullptr && entry->sizeLimit == 5 && entry->rawSize == 0xffffffffu);
    const unsigned closeCalls = interface.closeCalls();
    assert(entry->Release() == 1);
    assert(interface.closeCalls() == closeCalls + 2);
    delete entry;
    assert(interface.closeCalls() == closeCalls + 2);
    assert(interface.opens() == 2 && interface.closes() == 2);

    for (const auto &[name, expected] : {
             std::pair{"assets/gamma.bin", "GAMMA"},
             std::pair{"assets/alpha.bin", "ALPHA"},
             std::pair{"assets/beta.bin", "BETA"}}) {
        uint32_t handle = 99;
        String path(name);
        assert(AEFile::OpenRead(path, &handle) == 1 && handle == 1);
        assert(*path.index(0) == '/');
        const uint32_t size = static_cast<uint32_t>(std::strlen(expected));
        assert(AEFile::GetFileSize(handle) == size);
        char buffer[6] = {};
        assert(AEFile::Read(size + 1, buffer, handle) == 1);
        assert(std::strcmp(buffer, expected) == 0);
        assert(AEFile::Read(1, buffer, handle) == 0);
        AEFile::Close(handle);
        AEFile::Close(handle);
        assert(AEFile::GetFileSize(handle) == 0);
        assert(interface.opens() == interface.closes());
    }
    uint32_t missing = 99;
    assert(AEFile::OpenRead("assets/missing.bin", &missing) == 0 && missing == 99);

    // The unnamed handle uses slot zero and releases its previous file on reuse.
    for (const auto &[name, expected] : {
             std::pair{"assets/alpha.bin", "ALPHA"},
             std::pair{"assets/beta.bin", "BETA"}}) {
        String path(name);
        assert(AEFile::OpenRead(path, nullptr) == 1);
        char buffer[6] = {};
        assert(AEFile::Read(5, buffer, 0) == 1);
        assert(std::strcmp(buffer, expected) == 0);
    }
    AEFile::Close(0);
    AEFile::Close(0);
    assert(AEFile::Read(1, nullptr, 0) == 0);
    assert(interface.opens() == interface.closes());

    String outstanding("assets/alpha.bin");
    uint32_t outstandingHandle = 99;
    assert(AEFile::OpenRead(outstanding, &outstandingHandle) == 1);
    assert(interface.opens() == interface.closes() + 1);
    AEFile::Release();
    AEFile::Release();
    assert(interface.opens() == 8 && interface.closes() == 8);
    std::puts("AEFile raw PAK registration, fallback and lifetimes: OK");
}
