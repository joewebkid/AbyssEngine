// Station records are read by the intact production reader in this process.
// Catalog expectations are independently decoded from the same original files.
#include "gles_atlas_file_service.h"
#include "station_binary_gate_under_test.inc"

static void freeFixtureStations(Array<Station *> *stations) {
    for (auto *station : *stations) {
        if (!station) continue;
        // Fixture cleanup only: these newly read records have no stock/agents.
        assert(!station->items && !station->ships && !station->agents);
        station->name.~String();
        ::operator delete(station);
    }
    delete stations;
}

static void checkRecords(Array<Station *> *stations, const std::vector<int> &ids) {
    assert(stations && stations->size() == ids.size());
    for (unsigned i = 0; i < ids.size(); ++i) {
        const auto &expected = expectedStations[ids[i]];
        Station *actual = (*stations)[i];
        assert(actual && actual->index == ids[i]);
        assert(actual->systemIndex == expected.system && actual->techLevel == expected.tech);
        assert(actual->textureIndex == expected.texture && !actual->planet && !actual->visited);
        assert(actual->name.length == expected.name.size());
        for (unsigned j = 0; j < expected.name.size(); ++j)
            assert(actual->name.data[j] == expected.name[j]);
        assert(!actual->items && !actual->ships && !actual->agents && !actual->attackedFriends);
    }
}

static void checkCatalogReaders() {
    FileRead reader;
    unsigned records = 0;
    for (unsigned i = 0; i < 34; ++i) {
        SolarSystem *system = fixtureSystem(i);
        auto expected = expectedSystems[i].stations;
        std::sort(expected.begin(), expected.end());
        int oldOpened = opened, oldClosed = closed;
        auto *stations = reader.loadStationsBinary(system);
        checkRecords(stations, expected);
        assert(opened == oldOpened + 1 && closed == oldClosed + 1);
        assert(cursor == expectedStations[expected.back()].end);
        records += stations->size();
        freeFixtureStations(stations);
        delete system;
    }
    assert(records == 135);
    // Original scans in file order, retains duplicates, scans all 135 records
    // for zero/missing selections. Do not impose a new sorting/error policy.
    for (const auto &ids : {std::vector<int>{9, 5}, {7, 7}, {}, {135}}) {
        SolarSystem *system = fixtureSystem(1);
        ArraySetLength(unsigned(ids.size()), *system->stationIds);
        for (unsigned i = 0; i < ids.size(); ++i) (*system->stationIds)[i] = ids[i];
        auto *stations = reader.loadStationsBinary(system);
        assert(stations && stations->size() == ids.size());
        if (ids.empty() || ids[0] == 135) {
            assert(cursor == 3408);
            if (!ids.empty()) assert((*stations)[0] == nullptr);
        } else {
            auto sorted = ids;
            std::sort(sorted.begin(), sorted.end());
            checkRecords(stations, sorted);
            assert(cursor == expectedStations[sorted.back()].end);
        }
        freeFixtureStations(stations);
        delete system;
    }
    missingStationFile = true;
    auto *system = fixtureSystem(1);
    int oldOpened = opened;
    assert(reader.loadStationsBinary(system) == nullptr && opened == oldOpened);
    // Original leaks its newly allocated empty Array on this failure; preserved.
    missingStationFile = false;
    delete system;
    std::printf("Station reader: 34 catalogs, 135 full records/UTF-16 names, early-close offsets, "
                "unsorted/duplicate/empty/missing IDs and absent file: OK\n");
}

int main(int argc, char **argv) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    AtlasEglContext context;
    int result = stationEntryMain(argc, argv);
    assert(systemTextureBoundaryCalls == 1);
    checkCatalogReaders();
    return result;
}
