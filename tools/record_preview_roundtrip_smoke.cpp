#include "engine/core/GameText.h"
#include "engine/file/AEFile.h"
#include "game/mission/GameRecord.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

int GameText::getLanguage() { std::abort(); }
void String_printImpl(const char *) { std::abort(); }
namespace AbyssEngine {
unsigned short *computeFloatString(float, int, int *, int *, int) { std::abort(); }
}

struct PreviewStation {
    String name;
    String getName() { return name; }
};
struct PreviewShip { int index; int getIndex() { return index; } };
struct PreviewData {
    int64_t playTime64 = 0;
    int field_0x08 = 0;
    int field_0x40 = 0;
    int savedLevel = 0;
    int rankBits = 0;
    String previewSystemName;
    PreviewStation *station = nullptr;
    PreviewShip *ship = nullptr;
    PreviewStation *getStation() { return station; }
    PreviewShip *getShip() { return ship; }
    String &getPreviewSystemName() { return previewSystemName; }
};
struct PreviewWriter {
    String backupDir = String("preview/", false);
    void *recordStoreReadPreview(int slot);
    int recordStoreWritePreview(PreviewData *rec, int slot);
};

static std::vector<uint8_t> tape;
static size_t cursor = 0;
static bool exists = false;
static int deletes = 0;

static std::string pathText(const String &path) {
    std::string result;
    for (uint32_t i = 0; i < path.size(); ++i)
        result.push_back(static_cast<char>(path.data[i]));
    return result;
}

uint32_t AEFile::FileExist(const String &path) {
    assert(pathText(path) == "preview/4");
    return exists;
}
uint32_t AEFile::FileDelete(const String &path) {
    assert(pathText(path) == "preview/4" && exists);
    exists = false;
    tape.clear();
    ++deletes;
    return 1;
}
uint32_t AEFile::OpenWrite(String &path, uint32_t *handle) {
    assert(pathText(path) == "preview/4");
    tape.clear();
    cursor = 0;
    exists = true;
    *handle = 1;
    return 1;
}
uint32_t AEFile::OpenRead(String &path, uint32_t *handle) {
    assert(pathText(path) == "preview/4" && exists);
    cursor = 0;
    *handle = 1;
    return 1;
}
void AEFile::Close(uint32_t handle) { assert(handle == 1); }
uint32_t AEFile::Write(uint32_t bytes, void *buffer, uint32_t handle) {
    assert(handle == 1 && buffer && bytes);
    const auto *first = static_cast<const uint8_t *>(buffer);
    tape.insert(tape.end(), first, first + bytes);
    return bytes;
}
uint32_t AEFile::Read(uint32_t bytes, void *buffer, uint32_t handle) {
    assert(handle == 1 && buffer && bytes && cursor + bytes <= tape.size());
    std::memcpy(buffer, tape.data() + cursor, bytes);
    cursor += bytes;
    return bytes;
}

#include "record_preview_under_test.inc"

static void appendLE(std::vector<uint8_t> &bytes, uint64_t value, unsigned width) {
    for (unsigned i = 0; i < width; ++i) bytes.push_back((value >> (8 * i)) & 0xff);
}
static void appendWide(std::vector<uint8_t> &bytes, const uint16_t *text, unsigned length) {
    appendLE(bytes, length, 4);
    for (unsigned i = 0; i < length; ++i) appendLE(bytes, text[i], 2);
}
static void checkName(const String &actual, const uint16_t *expected, unsigned length) {
    assert(actual.size() == length);
    for (unsigned i = 0; i < length; ++i) assert(actual.data[i] == expected[i]);
}

int main() {
    const uint16_t stationName[] = {'K', 'a', 'l', 't', 'e', 'r', 0};
    const uint16_t systemName[] = {0x0410, 0x0440, 0x043a, 0x0430, 0x0434, 0x0438, 0x044f, 0};
    PreviewStation station{String(stationName, false)};
    PreviewShip ship{777};
    PreviewData source;
    source.playTime64 = 0x0102030405060708LL;
    source.field_0x08 = 12345;
    source.station = &station;
    source.previewSystemName = String(systemName, false);
    source.field_0x40 = 42;
    source.savedLevel = 9;
    source.rankBits = 0x40200000;
    source.ship = &ship;
    PreviewWriter writer;

    assert(writer.recordStoreReadPreview(4) == nullptr);
    assert(writer.recordStoreWritePreview(nullptr, 4) == 0 && !exists);
    assert(writer.recordStoreWritePreview(&source, 4) == 1);
    std::vector<uint8_t> expected;
    appendLE(expected, source.playTime64, 8);
    appendLE(expected, source.field_0x08, 4);
    appendWide(expected, stationName, 6);
    appendWide(expected, systemName, 7);
    appendLE(expected, source.field_0x40, 4);
    appendLE(expected, source.savedLevel, 4);
    appendLE(expected, source.rankBits, 4);
    appendLE(expected, ship.index, 4);
    assert(tape == expected);

    auto *read = static_cast<GameRecord *>(writer.recordStoreReadPreview(4));
    assert(read && cursor == tape.size());
    assert(read->playTime64 == source.playTime64 && read->field_0x08 == 12345);
    checkName(read->getPreviewStationName(), stationName, 6);
    checkName(read->getPreviewSystemName(), systemName, 7);
    assert(read->field_0x40 == 42 && read->savedLevel == 9);
    assert(read->rankBits == source.rankBits && read->shipId == 777);
    delete[] static_cast<uint8_t *>(read->data);
    delete read;

    assert(writer.recordStoreWritePreview(&source, 4) == 1);
    assert(deletes == 1 && tape == expected);
    std::puts("Record preview round trip: OK");
}
