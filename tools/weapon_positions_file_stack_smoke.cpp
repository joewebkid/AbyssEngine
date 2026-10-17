// Host boundary for the production AEFile -> FileRead weapon-position path.
#include "engine/file/AEFile.h"
#include "engine/file/FileRead.h"
#include "engine/core/GameText.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/mission/Mission.h"
#include "game/mission/Achievements.h"
#include "game/ship/Ship.h"
#include "game/core/Globals.h"
#include "game/world/Standing.h"
#include "game/world/Station.h"
#include "game/world/SolarSystem.h"
#include "game/ship/Agent.h"
#include "game/world/Wanted.h"
#include "game/world/Galaxy.h"
#include "game/world/Level.h"
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

// The player-status pointer is the host boundary for negative station IDs.
Status *Globals::status = nullptr;
void *Globals::globals = nullptr;
void *Globals::galaxy = nullptr;
void *Globals::items = nullptr;
void *Globals::ships = nullptr;
void *Globals::achievements = nullptr;
unsigned char Globals::options[100] = {};
unsigned char Globals::hints[59] = {};
Globals::Globals() {}
Globals::~Globals() {}
void Globals::resetHints() { std::fill(std::begin(hints), std::end(hints), 0); }
int Level::energyCellsForNextJump = 0;
unsigned char Level::doInstantJump = 0;
void *Level::programmedStation = nullptr;
void *Mission::empty = nullptr;
Galaxy *Galaxy::gGalaxy = nullptr;
Status *Status::gStatus = nullptr;
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
    assert(std::filesystem::is_regular_file(realAssetDirectory / "agents.bin"));
    assert(std::filesystem::is_regular_file(realAssetDirectory / "wanted.bin"));
    ReadOnlyAssetFileInterface interface;
    AEFile::SetInterface(&interface);
    FileRead reader;

    assert(reader.loadWeaponPositions(63) == nullptr);
    assert(reader.loadItemsBinary() == nullptr);
    assert(reader.loadShipsBinary() == nullptr);
    assert(reader.loadSystemsBinary() == nullptr);
    assert(reader.loadAgents() == nullptr);
    assert(reader.loadWanted() == nullptr);
    assert(interface.opens() == 0 && interface.closes() == 0);
    assert(!interface.addMount("data/bin/../", realAssetDirectory));
    assert(interface.addMount("data/bin/", realAssetDirectory));
    assert(!interface.addMount("data/bin/", realAssetDirectory));
    assert(!interface.FileExist(String("data/bin/../stations.bin")));
    assert(!interface.FileExist(String("data/bin_evil/stations.bin")));
    assert(!interface.FileExist(String("data/bin/not_present.bin")));
    assert(interface.FileExist(String("data/bin/stations.bin")));
    assert(interface.FileExist(String("data/bin/systems.bin")));
    assert(interface.FileExist(String("data/bin/agents.bin")));
    assert(interface.FileExist(String("data/bin/wanted.bin")));
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
    auto *agents = reader.loadAgents();
    assert(agents && agents->size() == 27);
    assert(narrow((*agents)[0]->name) == "Geryan Zhamoum");
    assert((*agents)[0]->station == 3 && (*agents)[0]->system == 0);
    assert((*agents)[0]->sellBlueprintIndex == 11);
    assert((*agents)[0]->imageParts && (*agents)[0]->imageParts[0] == 2);
    assert(narrow((*agents)[26]->name) == "Umnar Zoboon");
    assert((*agents)[26]->station == 108 && (*agents)[26]->system == 26);
    for (unsigned i = 0; i < agents->size(); ++i) {
        Agent *agent = (*agents)[i];
        assert(agent && agent->station >= 0 && agent->station < 135);
        assert(agent->type == i);
        assert(agent->system >= 0 && agent->system < 34);
        assert((*stations)[agent->station]->systemIndex == agent->system);
        assert((*systems)[agent->system]->stationIsInSystem(agent->station));
        delete agent;
    }
    delete agents;

    auto *wanted = reader.loadWanted();
    assert(wanted && wanted->size() == 25);
    assert(narrow((*wanted)[0]->name) == "Pal Tyyrt");
    assert((*wanted)[0]->shipId == 9 && (*wanted)[0]->lootItemId == 103);
    assert((*wanted)[0]->reward == 50000 && (*wanted)[0]->imageParts);
    for (unsigned i = 0; i < wanted->size(); ++i) {
        Wanted *entry = (*wanted)[i];
        assert(entry && entry->index == static_cast<int>(i));
        assert(entry->shipId >= 0 && entry->shipId < 64);
        assert(entry->lootItemId >= 0 && entry->lootItemId < 233);
        delete entry;
    }
    delete wanted;

    {
        Galaxy galaxy;
        Galaxy::gGalaxy = &galaxy;
        Status status;
        Globals::status = &status;
        // Keep the two globals distinct to check the ARM-observed source.
        assert(galaxy.getStation(-1) == status.playerStation);
        Status::gStatus = &status;
        Station *directStation = galaxy.getStation(0);
        assert(directStation && directStation->index == 0);
        delete directStation;
        status.setStation((*stations)[91]);
        assert(status.system == galaxy.getSystem(18));
        assert((*status.field_b4)[18]);
        assert(status.planetNames->size() == status.system->stationIds->size());
        assert(narrow(*(*status.planetNames)[1]).empty());
        status.currentCampaignMission = 1;
        status.setStation((*stations)[58]);
        assert(status.system == galaxy.getSystem(20));
        assert((*status.field_b4)[20]);
        for (unsigned i = 0; i < status.system->stationIds->size(); ++i) {
            const int id = (*status.system->stationIds)[i];
            assert(narrow(*(*status.planetNames)[i]) == narrow((*stations)[id]->name));
            assert((*status.planetTextures)[i] == (*stations)[id]->textureIndex);
        }
        const unsigned opened = interface.opens();
        status.setStation((*stations)[58]);
        assert(interface.opens() == opened);
        ArrayReleaseClasses(*status.planetNames);
        delete status.planetNames;
        delete status.planetTextures;
        delete status.playerStation;
        delete status.missions;
        delete status.systemVisibilities;
        delete status.field_94;
        delete status.field_98;
        delete status.field_ac;
        delete status.field_b4;
        delete status.field_4c;
        delete status.field_50;
        delete status.field_90;
        delete status.field_54;
        delete status.field_58;
        Globals::status = nullptr;
        Status::gStatus = nullptr;
        Galaxy::gGalaxy = nullptr;
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

    assert(interface.opens() == 48 && interface.closes() == 48);
    {
        auto *startupItems = reader.loadItemsBinary();
        auto *startupShips = reader.loadShipsBinary();
        assert(startupItems && startupItems->size() == 233);
        assert(startupShips && startupShips->size() == 64);
        const unsigned blueprintCount = static_cast<unsigned>(std::count_if(
            startupItems->begin(), startupItems->end(),
            [](Item *item) { return item->getIngredients() != nullptr; }));
        Globals globals;
        Globals::globals = &globals;
        Globals::items = startupItems;
        Globals::ships = startupShips;
        Achievements achievements;
        Globals::achievements = &achievements;
        Mission emptyMission;
        Mission::empty = &emptyMission;
        Galaxy galaxy;
        Globals::galaxy = &galaxy;
        Galaxy::gGalaxy = &galaxy;
        Status status;
        Globals::status = &status;
        Status::gStatus = &status;
        Globals::options[0x35] = 1;
        Globals::options[0x36] = 1;
        std::fill(std::begin(Globals::hints), std::end(Globals::hints), 0xff);
        Level::energyCellsForNextJump = 7;
        Level::doInstantJump = 1;
        Level::programmedStation = &status;

        status.resetGame();
        assert(status.station && status.station->index == 78);
        assert(status.ship && status.ship->index == 10);
        assert(status.mission == (*status.missions)[0]);
        assert((*status.missions)[1] == &emptyMission);
        assert(status.mission->targetStation == 78);
        assert(status.agents && status.agents->size() == 27);
        assert(status.wanted && status.wanted->size() == 25);
        assert(status.bluePrints && status.bluePrints->size() == blueprintCount);
        BluePrint *blueprint = nullptr;
        for (BluePrint *candidate : *status.bluePrints)
            if (candidate->getIndex() != 210) { blueprint = candidate; break; }
        assert(blueprint);
        Item *templateItem = (*startupItems)[blueprint->getIndex()];
        Array<int> *ingredientIds = blueprint->getIngredientList();
        Array<int> *quantities = templateItem->getQuantities();
        assert(ingredientIds == templateItem->getIngredients());
        int ingredientsValue = 0;
        for (unsigned i = 0; i < ingredientIds->size(); ++i)
            ingredientsValue += (*quantities)[i] *
                (*startupItems)[(*ingredientIds)[i]]->getSinglePrice();
        assert(blueprint->getIngredientsValue() == ingredientsValue);
        assert(blueprint->getAutoCompletionPrice() ==
               static_cast<int>(static_cast<float>(blueprint->batchMultiplier *
                   templateItem->getMaxPrice()) * 1.25f));
        assert(status.field_114 == 3);
        assert((*status.systemVisibilities)[25]);
        assert(Level::energyCellsForNextJump == 0);
        assert(Level::doInstantJump == 0 && Level::programmedStation == nullptr);
        assert(std::all_of(std::begin(Globals::hints), std::end(Globals::hints),
                           [](unsigned char value) { return value == 0; }));
        const int equipmentIds[] = {2, 2, 0x36, 0x3b, 0x52, 0x49, 0x24};
        for (int id : equipmentIds) {
            int count = 0;
            for (Item *item : *status.ship->equipment)
                count += item && item->index == id;
            assert(count == (id == 2 ? 2 : 1));
        }
        for (Item *item : *status.ship->equipment)
            if (item && item->index == 0x24) assert(item->amount == 6);

        ArrayReleaseClasses(*status.planetNames);
        delete status.planetNames;
        delete status.planetTextures;
        delete status.station;
        delete status.playerStation;
        delete status.voidStation;
        ArrayReleaseClasses(*status.bluePrints);
        delete status.bluePrints;
        delete status.standing;
        delete status.missions;
        delete status.systemVisibilities;
        delete status.field_94;
        delete status.field_98;
        delete status.field_ac;
        delete status.field_b4;
        delete status.field_4c;
        delete status.field_50;
        delete status.field_90;
        delete status.field_54;
        delete status.field_58;
        delete status.field_0x3c;
        delete status.field_0x40;
        delete status.field_0x44;
        delete status.field_0x48;
        Globals::status = nullptr;
        Status::gStatus = nullptr;
        Globals::galaxy = nullptr;
        Galaxy::gGalaxy = nullptr;
        Globals::achievements = nullptr;
        Mission::empty = nullptr;
        Globals::items = nullptr;
        Globals::ships = nullptr;
        Globals::globals = nullptr;
        for (Ship *ship : *startupShips) delete ship;
        delete startupShips;
        for (Item *item : *startupItems) {
            delete item->ingredients;
            delete item->quantities;
            delete item->attributes;
            delete item;
        }
        delete startupItems;
    }

    AEFile::Release();
    assert(interface.opens() == 56 && interface.closes() == 56);
    std::puts("production AEFile/FileRead assets and Status startup path: OK");
}
