// Host boundary for the production AEFile -> FileRead weapon-position path.
#include "engine/file/AEFile.h"
#include "engine/file/FileRead.h"
#include "engine/core/GameText.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

// Unused String/locale services are host boundaries; the concatenation is
// the same five-line body as AbyssEngine.cpp's String operator+.
namespace AbyssEngine {
String operator+(const String &a, const String &b) {
    String result(a);
    result += b;
    return result;
}
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
}
int GameText::getLanguage() { return 0; }
void String_printImpl(const char *) { std::abort(); }

namespace {
std::filesystem::path assetDirectory;
int opens = 0;
int closes = 0;

std::string narrow(const String &name) {
    char *text = name.GetAEChar();
    std::string result(text);
    delete[] text;
    return result;
}

// This fixture mounts only the one evidenced path; it is not a game host.
class HostFile final : public FileInterface {
public:
    explicit HostFile(std::FILE *stream = nullptr) : stream_(stream) { enabled = 1; }
    ~HostFile() override { Close(); }

    void *OpenRead(String name, int, bool windowed, int, int, unsigned offset) override {
        assert(!windowed && offset == 0);
        if (narrow(name) != "data/bin/weapons_hd.bin") return nullptr;
        std::FILE *stream = std::fopen((assetDirectory / "weapons_hd.bin").string().c_str(), "rb");
        if (!stream) return nullptr;
        ++opens;
        return new HostFile(stream);
    }
    uint32_t FileExist(String name) override {
        return narrow(name) == "data/bin/weapons_hd.bin" &&
               std::filesystem::is_regular_file(assetDirectory / "weapons_hd.bin");
    }
    uint32_t Read(uint32_t bytes, void *buffer) override {
        assert(stream_);
        return static_cast<uint32_t>(std::fread(buffer, 1, bytes, stream_));
    }
    uint32_t GetFileSize() override {
        assert(stream_);
        long position = std::ftell(stream_);
        std::fseek(stream_, 0, SEEK_END);
        long size = std::ftell(stream_);
        std::fseek(stream_, position, SEEK_SET);
        return static_cast<uint32_t>(size);
    }
    void Close() override {
        if (stream_) { std::fclose(stream_); stream_ = nullptr; ++closes; }
    }
    void *OpenWrite(String, int, bool, unsigned) override { return nullptr; }
    void *OpenAppend(String, int, bool, unsigned) override { return nullptr; }
    uint32_t Write(uint32_t, const void *) override { return 0; }
    uint32_t Seek(uint32_t) override { return 0; }
    uint32_t FileDelete(String) override { return 0; }
    uint32_t GetDeviceFreeSpace() override { return 0; }
    const char *GetAppRootDir() override { return ""; }
    void SetAppRootDir(void *) override {}
    void SetZipDirectory(void *) override {}
    uint32_t FileEnumInit(char *, bool) override { return 0; }
    uint32_t FileGetNextEnum(String &) override { return 0; }
    String GetDirPreFix() override { return String(""); }
    char *Output(char *line) override { return line; }
    void SetSaveDirectory(String) override {}
    void ResetSaveDirectory() override {}

private:
    std::FILE *stream_;
};

void release(Array<Array<AbyssEngine::AEMath::Vector *> *> *positions) {
    for (auto *slot : *positions) {
        if (!slot) continue;
        ArrayReleaseClasses(*slot);
        delete slot;
    }
    delete positions;
}
}

int main(int argc, char **argv) {
    assert(argc == 2);
    const std::filesystem::path realAssetDirectory = argv[1];
    assert(std::filesystem::is_regular_file(realAssetDirectory / "weapons_hd.bin"));
    HostFile interface;
    AEFile::SetInterface(&interface);
    FileRead reader;

    assetDirectory = realAssetDirectory / "missing";
    assert(reader.loadWeaponPositions(63) == nullptr);
    assert(opens == 0 && closes == 0);
    assetDirectory = realAssetDirectory;

    auto *first = reader.loadWeaponPositions(63);
    assert(first && first->size() == 4);
    assert((*first)[0]->size() == 2 && (*first)[1]->size() == 4);
    assert((*first)[2] == nullptr && (*first)[3]->size() == 6);
    auto *point = (*(*first)[0])[0];
    assert(point->x == 488 && point->y == -56 && point->z == 519);
    release(first);

    auto *later = reader.loadWeaponPositions(1);
    assert(later && later->size() == 4);
    assert((*later)[2]->size() == 1 && (*later)[3]->size() == 6);
    point = (*(*later)[3])[1];
    assert(std::fabs(point->x - 0.8f) < 0.00001f);
    release(later);

    AEFile::Release();
    assert(opens == 2 && closes == 2);
    std::puts("production AEFile/FileRead weapon asset: OK");
}
