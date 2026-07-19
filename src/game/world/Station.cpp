#include "game/world/Station.h"
#include "game/ship/Ship.h"
#include "game/world/Galaxy.h"
#include "game/mission/Mission.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/ship/Agent.h"
#include "game/core/String.h"

static const int kHiddenBlueprints[5] = {0, 0, 0, 0, 0};
static const int kPirateStations[4] = {0, 0, 0, 0};

Station::Station()
    : name("Station"),
      index(-1),
      systemIndex(-1),
      planet(0),
      textureIndex(0),
      visited(0),
      techLevel(0),
      attackedFriends(0),
      items(nullptr),
      ships(nullptr),
      agents(nullptr) {
}

Station::Station(String name, int index, int systemIndex, int techLevel, int textureIndex)
    : name(name),
      index(index),
      systemIndex(systemIndex),
      planet(0),
      textureIndex(textureIndex),
      visited(0),
      techLevel(techLevel),
      attackedFriends(0),
      items(nullptr),
      ships(nullptr),
      agents(nullptr) {
}

Station::~Station() {
    if (ships != nullptr) {
        ArrayReleaseClasses(*ships);
        delete ships;
        ships = nullptr;
    }
    if (items != nullptr) {
        ArrayReleaseClasses(*items);
        delete items;
        items = nullptr;
    }
    if (agents != nullptr) {
        Mission *campaign = reinterpret_cast<Mission *>(Status::gStatus->getCampaignMission());
        Mission *freelance = Status::gStatus->getFreelanceMission();
        Agent *campaignAgent = campaign != nullptr ? campaign->getAgent() : nullptr;
        Agent *freelanceAgent = freelance != nullptr ? freelance->getAgent() : nullptr;
        for (Agent *a: *agents) {
            if (a != nullptr && a != campaignAgent && a != freelanceAgent && !a->isStoryAgent())
                delete a;
        }
        delete agents;
        agents = nullptr;
    }
}

Station *Station::clone() {
    return new Station(name, index, systemIndex, techLevel, textureIndex);
}

int Station::getIndex() { return index; }
int Station::getSystem() { return systemIndex; }
int Station::getTecLevel() { return techLevel; }
int Station::getTextureIndex() { return textureIndex; }
bool Station::isPlanet() { return planet; }
String Station::getName() { return name; }

Array<Agent *> *Station::getAgents() { return agents; }
Array<Item *> *Station::getItems() { return items; }
Array<Ship *> *Station::getShips() { return ships; }

void Station::addItem(Item *item) {
    if (items == nullptr) {
        items = new Array<Item *>();
    } else {
        for (uint32_t i = 0; i < items->size(); i++) {
            if ((*items)[i]->equals(item)) {
                (*items)[i]->changeAmount(item->getAmount());
                return;
            }
        }
    }
    ArrayAdd(item, *items);
}

void Station::addShip(Ship *ship) {
    if (ships == nullptr) {
        ships = new Array<Ship *>();
    } else {
        for (uint32_t i = 0; i < ships->size(); i++) {
            if ((*ships)[i]->equals(ship))
                return;
        }
    }
    ArrayAdd(ship, *ships);
}

void Station::removeShip(Ship *ship) {
    if (ships == nullptr)
        return;
    ArrayRemove(ship, *ships);
}

void Station::removeShips() {
    if (ships != nullptr) {
        ArrayReleaseClasses(*ships);
        delete ships;
    }
    ships = nullptr;
}

bool Station::equals(Station *other) {
    return other != nullptr && index == other->index;
}

uint32_t Station::hasItem(int index) {
    if (items != nullptr) {
        for (uint32_t i = 0; i < items->size(); i++) {
            Item *it = (*items)[i];
            if (it != nullptr && it->getIndex() == index)
                return 1;
        }
    }
    return 0;
}

uint32_t Station::hasShip(int index) {
    if (ships != nullptr) {
        for (uint32_t i = 0; i < ships->size(); i++) {
            Ship *sh = (*ships)[i];
            if (sh != nullptr && sh->getIndex() == index)
                return 1;
        }
    }
    return 0;
}

void Station::setItems(Array<Item *> *items, bool deep) {
    if (this->items != nullptr)
        delete this->items;
    this->items = nullptr;
    if (items == nullptr || !deep) {
        this->items = items;
    } else {
        Array<Item *> *copy = new Array<Item *>();
        this->items = copy;
        ArraySetLength(items->size(), *copy);
        for (uint32_t i = 0; i < items->size(); i++)
            (*copy)[i] = (*items)[i]->clone();
    }
}

void Station::setShips(Array<Ship *> *ships, bool deep) {
    if (this->ships != nullptr) {
        ArrayReleaseClasses(*this->ships); ArrayRemoveAll(*(this->ships));
        delete this->ships;
    }
    this->ships = nullptr;
    if (ships == nullptr || !deep) {
        this->ships = ships;
    } else {
        Array<Ship *> *copy = new Array<Ship *>();
        this->ships = copy;
        ArraySetLength(ships->size(), *copy);
        for (uint32_t i = 0; i < ships->size(); i++)
            (*copy)[i] = (*ships)[i]->clone();
    }
}

void Station::setAgents(Array<Agent *> *agents) {
    if (this->agents == agents)
        return;
    if (this->agents != nullptr) {
        ArrayReleaseClasses(*this->agents); ArrayRemoveAll(*(this->agents));
        delete this->agents;
    }
    this->agents = agents;
}

void Station::setAttackedFriends(bool v) {
    attackedFriends = v;
}

uint8_t Station::hasAttackedFriends() {
    return attackedFriends;
}

bool Station::isAttackedByAliens() {
    return index == Status::gStatus->field_80;
}

uint8_t Station::isDiscovered() {
    return Galaxy::gGalaxy->getVisited()[index];
}

void Station::visit() {
    if (isDiscovered())
        return;
    visited = 1;
    Status::gStatus->visitStation();
    Galaxy::gGalaxy->visitStation(index);
}

uint32_t Station::getHiddenBlueprintIndex() {
    for (uint32_t i = 0; i < 5; i++) {
        if (kHiddenBlueprints[i] == index)
            return i;
    }
    return 0xffffffff;
}

uint32_t Station::getPirateStationIndex() {
    for (uint32_t i = 0; i < 4; i++) {
        if (kPirateStations[i] == index)
            return i;
    }
    return 0xffffffff;
}

uint32_t Station::stationHasHiddenBlueprint(bool ignoreFound) {
    for (uint32_t i = 0; i < 5; i++) {
        if (kHiddenBlueprints[i] == index) {
            if (ignoreFound)
                return 1;
            bool *flags = Status::gStatus->field_58->data_;
            if (flags[i] == 0)
                return 1;
        }
    }
    return 0;
}

uint32_t Station::stationHasPirateBase() {
    for (uint32_t i = 0; i < 4; i++) {
        if (kPirateStations[i] == index) {
            bool *flags = Status::gStatus->field_4c->data_;
            if (flags[i] == 0)
                return 1;
        }
    }
    return 0;
}
