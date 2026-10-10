#include "engine/file/FileRead.h"
#include "game/world/Station.h"

#include <cassert>
#include <cstdio>
#include <cstring>

namespace {
unsigned recordIndex;
unsigned fieldIndex;
unsigned closeCount;
unsigned constructedStations;
unsigned destroyedStations;
}

// The file boundary is synthetic; the loader body comes from FileRead.cpp.
uint32_t AEFile::FileExist(const String &) { return 1; }
uint32_t AEFile::OpenRead(const char *, uint32_t *handle) {
    recordIndex = fieldIndex = closeCount = 0;
    *handle = 1;
    return 1;
}
void AEFile::Close(uint32_t handle) {
    assert(handle == 1);
    ++closeCount;
}
void AEFile::ReadSwitched(String &value, uint32_t handle) {
    assert(handle == 1 && recordIndex < 135);
    value.Set("station");
    fieldIndex = 0;
}
uint32_t AEFile::ReadSwitched(int32_t &value, uint32_t handle) {
    assert(handle == 1 && fieldIndex < 4);
    value = fieldIndex++ == 0 ? static_cast<int32_t>(recordIndex) : 42;
    if (fieldIndex == 4) ++recordIndex;
    return 1;
}

namespace AbyssEngine {
String::String() : data(nullptr), length(0) {}
String::String(const char *text, bool) : String() { Set(text); }
String::~String() { delete[] data; }
void String::Set(const char *text) {
    delete[] data;
    length = static_cast<int>(std::strlen(text));
    data = new unsigned short[length + 1];
    for (int i = 0; i < length; ++i) data[i] = static_cast<unsigned char>(text[i]);
    data[length] = 0;
}
void String::Set(const unsigned short *text) {
    unsigned lengthNew = 0;
    while (text[lengthNew] != 0) ++lengthNew;
    auto *copy = new unsigned short[lengthNew + 1];
    std::memcpy(copy, text, (lengthNew + 1) * sizeof(*copy));
    delete[] data;
    data = copy;
    length = static_cast<int>(lengthNew);
}
void String::ConvertFromUTF8() {}
}

Station::Station(String stationName, int stationId, int systemId, int tech, int texture)
    : name(stationName), index(stationId), systemIndex(systemId), planet(0),
      textureIndex(texture), visited(0), techLevel(tech), attackedFriends(0),
      items(nullptr), ships(nullptr), agents(nullptr) { ++constructedStations; }
Station::~Station() { ++destroyedStations; }

SolarSystem::SolarSystem(int, String displayName, int, bool, int, int, int, int,
                         int, int, int *, Array<int> *stations, Array<int> *, Array<int> *)
    : name(displayName), stationIds(stations) {}
SolarSystem::~SolarSystem() {}
uint32_t *SolarSystem::getStations() { return reinterpret_cast<uint32_t *>(stationIds); }
FileRead::FileRead() {}
FileRead::~FileRead() {}

#include "station_loader_under_test.inc"

static void check(const int *ids, unsigned count, const int *expected, unsigned expectedCount,
                  unsigned recordsRead) {
    Array<int> stationIds;
    ArraySetLength(count, stationIds);
    for (unsigned i = 0; i < count; ++i) stationIds[i] = ids[i];
    SolarSystem system(0, String("system"), 0, false, 0, 0, 0, 0, 0, 0,
                       nullptr, &stationIds, nullptr, nullptr);
    FileRead reader;
    constructedStations = destroyedStations = 0;
    Array<Station *> *stations = reader.loadStationsBinary(&system);
    assert(stations != nullptr && stations->size() == count);
    assert(recordIndex == recordsRead && closeCount == 1);
    assert(constructedStations == expectedCount);
    for (unsigned i = 0; i < expectedCount; ++i) {
        assert((*stations)[i] != nullptr);
        assert((*stations)[i]->index == expected[i]);
        assert((*stations)[i]->systemIndex == 42);
    }
    for (unsigned i = expectedCount; i < count; ++i) assert((*stations)[i] == nullptr);
    ArrayReleaseClasses(*stations);
    delete stations;
    assert(destroyedStations == constructedStations);
}

int main() {
    const int unordered[] = {5, 1, 7};
    const int fileOrder[] = {1, 5, 7};
    check(unordered, 3, fileOrder, 3, 8);
    const int duplicates[] = {2, 2};
    check(duplicates, 2, duplicates, 2, 3);
    const int missing[] = {200};
    check(missing, 1, nullptr, 0, 135);
    check(nullptr, 0, nullptr, 0, 135);
    std::puts("station loader smoke: OK");
}
