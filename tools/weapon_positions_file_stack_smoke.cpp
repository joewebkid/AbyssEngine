// Host boundary for the production AEFile -> FileRead weapon-position path.
#include "engine/file/AEFile.h"
#include "engine/file/FileRead.h"
#include "engine/core/GameText.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include "game/core/Globals.h"
#include "game/world/Standing.h"
#include "game/world/Station.h"
#include "game/world/SolarSystem.h"
#include "host/ReadOnlyAssetFileInterface.h"
#include <cassert>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

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
// Catalog stations have no owned item/ship/agent lists in this fixture.
Station::~Station() {}

namespace {
std::string narrow(const String &name) {
    char *text = name.GetAEChar();
    std::string result(text);
    delete[] text;
    return result;
}

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
    assert(std::filesystem::is_regular_file(realAssetDirectory / "stations.bin"));
    assert(std::filesystem::is_regular_file(realAssetDirectory / "systems.bin"));
    ReadOnlyAssetFileInterface interface;
    AEFile::SetInterface(&interface);
    FileRead reader;

    assert(reader.loadWeaponPositions(63) == nullptr);
    assert(reader.loadItemsBinary() == nullptr);
    assert(reader.loadShipsBinary() == nullptr);
    assert(reader.loadSystemsBinary() == nullptr);
    assert(interface.opens() == 0 && interface.closes() == 0);
    assert(!interface.addMount("data/bin/../", realAssetDirectory));
    assert(interface.addMount("data/bin/", realAssetDirectory));
    assert(!interface.addMount("data/bin/", realAssetDirectory));
    assert(!interface.FileExist(String("data/bin/../stations.bin")));
    assert(!interface.FileExist(String("data/bin_evil/stations.bin")));
    assert(!interface.FileExist(String("data/bin/not_present.bin")));
    assert(interface.FileExist(String("data/bin/stations.bin")));
    assert(interface.FileExist(String("data/bin/systems.bin")));
    assert(interface.OpenWrite(String("data/bin/stations.bin"), 0, false, 0) == nullptr);

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

    auto *systems = reader.loadSystemsBinary();
    assert(systems && systems->size() == 34);
    for (unsigned i = 0; i < systems->size(); ++i) {
        assert((*systems)[i] && (*systems)[i]->systemId == static_cast<int>(i));
    }
    assert(narrow((*systems)[0]->name) == "Suteo");
    assert((*systems)[0]->mapX == 22 && (*systems)[0]->mapY == 55);
    assert((*systems)[0]->mapZ == 55 && (*systems)[0]->jumpgateStationId == 0);
    assert((*systems)[0]->stationIds->size() == 5 && (*(*systems)[0]->stationIds)[4] == 4);
    assert((*systems)[0]->linkedSystemIds->size() == 2);
    assert((*(*systems)[0]->linkedSystemIds)[0] == 2 && (*(*systems)[0]->linkedSystemIds)[1] == 8);
    assert(narrow((*systems)[18]->name) == "Pescal Inartu");
    assert((*systems)[18]->jumpgateStationId == 90 && (*systems)[18]->visible);
    assert((*systems)[18]->linkedSystemIds->size() == 2);
    assert((*(*systems)[18]->linkedSystemIds)[0] == 1 && (*(*systems)[18]->linkedSystemIds)[1] == 8);
    assert((*systems)[18]->stationIsInSystem(91));
    assert(narrow((*systems)[33]->name) == "Alda");
    assert((*systems)[33]->stationIds->size() == 2);
    assert((*(*systems)[33]->stationIds)[0] == 133);

    int16_t ids[135];
    for (int16_t i = 0; i < 135; ++i) ids[i] = i;
    auto *stations = reader.loadStationsBinary(ids, 135);
    assert(stations && stations->size() == 135);
    for (unsigned i = 0; i < stations->size(); ++i) {
        assert((*stations)[i] && (*stations)[i]->index == static_cast<int>(i));
        const int system = (*stations)[i]->systemIndex;
        assert(system >= 0 && system < static_cast<int>(systems->size()));
        assert((*systems)[system]->stationIsInSystem(static_cast<int>(i)));
    }
    assert((*stations)[0]->name.data[3] == 0xe9); // Nehébru, UTF-8 decoded.
    assert((*stations)[70]->systemIndex == 14 && narrow((*stations)[70]->name) == "Dis");
    assert((*stations)[91]->systemIndex == 18 && narrow((*stations)[91]->name) == "Dima");
    for (SolarSystem *system : *systems) {
        auto *local = reader.loadStationsBinary(system);
        assert(local && local->size() == system->stationIds->size());
        std::vector<int> fileOrder;
        for (int id : *system->stationIds) fileOrder.push_back(id);
        std::sort(fileOrder.begin(), fileOrder.end());
        if (system->systemId == 20) {
            assert((*system->stationIds)[0] == 62 && (*system->stationIds)[1] == 59);
            assert(fileOrder[0] == 58 && fileOrder[2] == 62);
        }
        for (unsigned i = 0; i < local->size(); ++i) {
            assert((*local)[i] && (*local)[i]->index == fileOrder[i]);
            delete (*local)[i];
        }
        delete local;
    }
    for (Station *station : *stations) delete station;
    delete stations;

    for (SolarSystem *system : *systems) delete system;
    delete systems;

    Station *dlc1 = reader.loadStation(91);
    Station *dlc2 = reader.loadStation(70);
    assert(dlc1 && dlc1->index == 91 && narrow(dlc1->name) == "Dima");
    assert(dlc2 && dlc2->index == 70 && narrow(dlc2->name) == "Dis");
    delete dlc1;
    delete dlc2;

    AEFile::Release();
    assert(interface.opens() == 42 && interface.closes() == 42);
    std::puts("production AEFile/FileRead five core assets: OK");
}
