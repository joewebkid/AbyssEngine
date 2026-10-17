// Host boundary for the production AEFile -> FileRead weapon-position path.
#include "engine/file/AEFile.h"
#include "engine/file/FileRead.h"
#include "engine/core/GameText.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include "game/core/Globals.h"
#include "game/world/Standing.h"
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

// These player-status branches are unreachable while loading catalog templates.
Status *Globals::status = nullptr;
Standing *Status::getStanding() { std::abort(); }
void Standing::setPlayerSignatureRace(int) { std::abort(); }

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

std::filesystem::path resource(const String &name) {
    const std::string path = narrow(name);
    if (path == "data/bin/weapons_hd.bin") return assetDirectory / "weapons_hd.bin";
    if (path == "data/bin/items.bin") return assetDirectory / "items.bin";
    if (path == "data/bin/ships.bin") return assetDirectory / "ships.bin";
    return {};
}

// This fixture mounts only three evidenced paths; it is not a game host.
class HostFile final : public FileInterface {
public:
    explicit HostFile(std::FILE *stream = nullptr) : stream_(stream) { enabled = 1; }
    ~HostFile() override { Close(); }

    void *OpenRead(String name, int, bool windowed, int, int, unsigned offset) override {
        assert(!windowed && offset == 0);
        const auto path = resource(name);
        if (path.empty()) return nullptr;
        std::FILE *stream = std::fopen(path.string().c_str(), "rb");
        if (!stream) return nullptr;
        ++opens;
        return new HostFile(stream);
    }
    uint32_t FileExist(String name) override {
        const auto path = resource(name);
        return !path.empty() && std::filesystem::is_regular_file(path);
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
    assert(std::filesystem::is_regular_file(realAssetDirectory / "items.bin"));
    assert(std::filesystem::is_regular_file(realAssetDirectory / "ships.bin"));
    HostFile interface;
    AEFile::SetInterface(&interface);
    FileRead reader;

    assetDirectory = realAssetDirectory / "missing";
    assert(reader.loadWeaponPositions(63) == nullptr);
    assert(reader.loadItemsBinary() == nullptr);
    assert(reader.loadShipsBinary() == nullptr);
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

    auto *items = reader.loadItemsBinary();
    assert(items && items->size() == 233);
    assert((*items)[0]->attributes->size() >= 18);
    assert((*items)[0]->attributes->data()[1] == 0);
    assert((*items)[0]->attributes->data()[15] == 400);
    assert((*items)[0]->index == 0 && (*items)[0]->minPrice == 400);
    assert((*items)[232]->attributes->data()[1] == 232);
    assert((*items)[232]->attributes->data()[15] == 1550000);
    assert((*items)[232]->index == 232 && (*items)[232]->type == 1);
    assert((*items)[232]->ingredients->size() == 2);
    for (unsigned i = 0; i < items->size(); ++i) {
        Item *item = (*items)[i];
        assert(item && item->index == static_cast<int>(i));
        assert(item->attributes && item->attributes->size() >= 18);
        delete item->ingredients;
        delete item->quantities;
        delete item->attributes;
        delete item;
    }
    delete items;

    auto *ships = reader.loadShipsBinary();
    assert(ships && ships->size() == 64);
    assert((*ships)[5]->index == 5 && (*ships)[5]->baseHP == 150);
    assert((*ships)[5]->value == 30900 && (*ships)[5]->price == 30900);
    assert((*ships)[5]->slots[0] == 2 && (*ships)[5]->slots[3] == 4);
    assert((*ships)[5]->equipment->size() == 7);
    assert((*ships)[30]->index == 30 && (*ships)[30]->value == 70560);
    assert((*ships)[30]->slots[1] == 2 && std::fabs((*ships)[30]->handling - 1.28f) < 0.00001f);
    for (unsigned i = 0; i < ships->size(); ++i) {
        Ship *ship = (*ships)[i];
        assert(ship && ship->index == static_cast<int>(i));
        delete ship;
    }
    delete ships;

    AEFile::Release();
    assert(opens == 4 && closes == 4);
    std::puts("production AEFile/FileRead core assets: OK");
}
