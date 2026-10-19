#include <algorithm>
#include <type_traits>
#include "station_binary_oracle.inc"
static unsigned systemTextureBoundaryCalls;
static Array<uint32_t> *starTextures;
static Array<int> *starIndices;
static Array<Station *> *loadedStationRecords;

// Test observation only: retain the real template implementation and arguments.
// Applied only around the intact StarSystem constructor and station reader.
template<class T> static void stationArraySetLength(unsigned n, Array<T> &array) {
    ArraySetLength<T>(n, array);
    if constexpr (std::is_same<T, uint32_t>::value) starTextures = &array;
    if constexpr (std::is_same<T, int>::value) starIndices = &array;
    if constexpr (std::is_same<T, Station *>::value) loadedStationRecords = &array;
}

static void checkRecords(Array<Station *> *, const std::vector<int> &);
static void stationTextureBoundary(unsigned short id, unsigned &out, bool flag) {
    if (id != 0x2731) return; // Original Pan system's texture table entry 2.
    assert(!flag && systemTextureBoundaryCalls++ == 0);
    assert(starTextures && starTextures->size() == 6 && &out == starTextures->data());
    assert(starIndices && starIndices->size() == 5);
    for (auto value : *starTextures) assert(value == 0);
    for (auto value : *starIndices) assert(value == 0);
    checkRecords(loadedStationRecords, {5, 6, 7, 8, 9});
    assert(cursor == 247 && input.size() == 3408 && opened == closed);
    auto *resource = static_cast<Resource *>(PaintCanvas::gCanvas->FindResource(id));
    assert(resource && resource->kind == 2 && resource->handle == -1);
    auto *payload = static_cast<PCTexInfoView *>(resource->payload);
    assert(std::strcmp(payload->path, "data/assets/main/3d/textures/low/etc/suns/sun_002.aei") == 0);
    std::puts("Real station read -> StarSystem arrays 6/5 -> sun resource 10033: OK; "
              "stopped before TextureCreate, no sun handle or completed StarSystem/Level stage 0");
    throw FlareImageBoundary{};
}

#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(SolarSystem) == 0x48 && offsetof(SolarSystem, stationIds) == 0x38,
              "original SolarSystem station array ABI");
static_assert(sizeof(Station) == 0x34 && offsetof(Station, index) == 0xc,
              "original station record ABI");
static_assert(sizeof(Array<Station *>) == 0xc && offsetof(Array<Station *>, data_) == 4,
              "original flat Array ABI");
static_assert(offsetof(StarSystem, texturesArray) == 0x14 &&
              offsetof(StarSystem, stationIdxArray) == 0x24, "original StarSystem arrays ABI");
#endif

static SolarSystem *fixtureSystem(unsigned index) {
    const auto &s = expectedSystems[index];
    auto *ids = new Array<int>();
    ArraySetLength(unsigned(s.stations.size()), *ids);
    for (unsigned i = 0; i < ids->size(); ++i) (*ids)[i] = s.stations[i];
    int rgb[] = {s.rgb[0], s.rgb[1], s.rgb[2]};
    return new SolarSystem(index, String(s.name), s.fields[0], s.fields[1] == 1,
        s.fields[2], s.fields[3], s.fields[4], s.fields[5], s.fields[6], s.fields[7],
        rgb, ids, nullptr, nullptr);
}
