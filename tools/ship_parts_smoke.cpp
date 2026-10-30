#include "engine/core/Array.h"

#include <cassert>
#include <cstdint>
#include <cstring>

struct String {
    static int live;
    explicit String(const char *path) { assert(std::strcmp(path, "data/bin/shipparts.bin") == 0); ++live; }
    ~String() { --live; }
};
int String::live = 0;

static bool fileExists;
static int opened;
static int closed;
static int cursor;
static int32_t record[12];

class AEFile {
public:
    static uint32_t FileExist(const String &) { return fileExists; }
    static uint32_t OpenRead(const char *path, uint32_t *handle) {
        assert(std::strcmp(path, "data/bin/shipparts.bin") == 0 && String::live == 0);
        *handle = 1;
        ++opened;
        return 1;
    }
    static void Close(uint32_t handle) { assert(handle == 1); ++closed; }
    static uint32_t Read(int8_t &value, uint32_t handle) {
        assert(handle == 1 && cursor < 2);
        value = static_cast<int8_t>(record[cursor++]);
        return 1;
    }
    static uint32_t ReadSwitched(int16_t &value, uint32_t handle) {
        assert(handle == 1 && cursor < 12);
        value = static_cast<int16_t>(record[cursor++]);
        return 1;
    }
    static uint32_t ReadSwitched(int32_t &value, uint32_t handle) {
        assert(handle == 1 && cursor < 12);
        value = record[cursor++];
        return 1;
    }
};

class FileRead {
public:
    Array<int32_t> *loadShipParts(int32_t id);
};

#include "ship_parts_under_test.inc"

static void checkRecord(int32_t group, int32_t id) {
    record[0] = group;
    record[1] = 1;
    for (int i = 0; i < 10; ++i)
        record[i + 2] = i + 10;
    cursor = opened = closed = 0;
    FileRead loader;
    Array<int32_t> *parts = loader.loadShipParts(id);
    assert(parts != nullptr && parts->size() == 10);
    for (int i = 0; i < 10; ++i)
        assert(parts->data()[i] == i + 10);
    assert(cursor == 12 && opened == 1 && closed == 0 && String::live == 0);
    delete parts;
}

int main() {
    FileRead loader;
    fileExists = false;
    assert(loader.loadShipParts(0) == nullptr);
    assert(opened == 0 && String::live == 0);
    fileExists = true;
    checkRecord(1, 0);
    checkRecord(-1, -2);
}
