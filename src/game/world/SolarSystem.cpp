#include "game/world/SolarSystem.h"
#include "game/world/Galaxy.h"
#include "game/world/Station.h"
#include "game/mission/Status.h"
#include "game/core/String.h"

SolarSystem::SolarSystem(int unk0, String displayName, int security,
                         bool isVisible, int factionId, int x, int y, int z,
                         int jumpgateId, int texture, int *starRGB,
                         Array<int> *stations, Array<int> *linkedSystems,
                         Array<int> *forbidden) {
    this->name = displayName;
    this->systemId = unk0;
    this->visible = isVisible;
    this->securityLevel = security;
    this->faction = factionId;
    this->mapX = x;
    this->mapY = y;
    this->mapZ = z;
    this->jumpgateStationId = jumpgateId;
    this->textureIndex = texture;
    this->starR = starRGB[0];
    this->starG = starRGB[1];
    this->starB = starRGB[2];
    this->stationIds = stations;
    this->forbiddenGoods = forbidden;
    this->linkedSystemIds = linkedSystems;
}

SolarSystem::~SolarSystem() {
    if (this->stationIds != nullptr) {
        ArrayRemoveAll(*(this->stationIds));
        delete this->stationIds;
    }
    this->stationIds = nullptr;
    if (this->forbiddenGoods != nullptr) {
        ArrayRemoveAll(*(this->forbiddenGoods));
        delete this->forbiddenGoods;
    }
    this->forbiddenGoods = nullptr;
    if (this->linkedSystemIds != nullptr) {
        ArrayRemoveAll(*(this->linkedSystemIds));
        delete this->linkedSystemIds;
    }
    this->linkedSystemIds = nullptr;
}

static Status ** gStatusOrbit = nullptr;

bool SolarSystem::currentOrbitHasWarpGate() {
    int orbit = this->jumpgateStationId;
    Station *st = (*gStatusOrbit)->getStation();
    return orbit == st->getIndex();
}

int SolarSystem::getIndex() { return systemId; }
int SolarSystem::getRace() { return (int) faction; }
int SolarSystem::getSecurityLevel() { return securityLevel; }
int SolarSystem::getTextureIndex() { return textureIndex; }
int SolarSystem::getX() { return mapX; }
int SolarSystem::getY() { return mapY; }
int SolarSystem::getZ() { return mapZ; }
int SolarSystem::getWarpGateIndex() { return jumpgateStationId; }
uint32_t *SolarSystem::getStations() { return (uint32_t *) stationIds; }
uint32_t *SolarSystem::getRoutes() { return (uint32_t *) linkedSystemIds; }
void *SolarSystem::getForbiddenGoods() { return forbiddenGoods; }

uint8_t SolarSystem::isVisible() {
    return this->visible;
}

int SolarSystem::stationIsInSystem(int idx) {
    Array<int> *arr = this->stationIds;
    uint32_t n = arr->size();
    for (uint32_t i = 0; i < n; i++) {
        if ((*arr)[i] == idx)
            return 1;
    }
    return 0;
}

int SolarSystem::systemIsInSystemRoutes(int sys) {
    if (this->systemId != sys) {
        Array<int> *arr = this->linkedSystemIds;
        if (arr == nullptr)
            return 0;
        uint32_t n = arr->size();
        for (uint32_t i = 0; i < n; i++) {
            if ((*arr)[i] == sys)
                return 1;
        }
        return 0;
    }
    return 1;
}

void SolarSystem::setVisible(bool v) {
    this->visible = v;
}

uint32_t SolarSystem::getStationEnumIndex(int idx) {
    Array<int> *arr = this->stationIds;
    for (uint32_t i = 0; i < arr->size(); i++) {
        if ((*arr)[i] == idx)
            return i;
    }
    return 0xffffffff;
}

String SolarSystem::getName() {
    return this->name;
}

static const int kPirateBaseStations[4] = {0, 0, 0, 0};

static void * gPirateBaseRoot = nullptr;

int SolarSystem::hasPirateBase() {
    char *base = *(char **) gPirateBaseRoot;
    for (uint32_t i = 0; i <= 3; i++) {
        if (stationIsInSystem(kPirateBaseStations[i]) != 0) {
            char *flags = *(char **) (*(char **) (base + 0x4c) + 4);
            if (flags[i] == 0)
                return 1;
        }
    }
    return 0;
}

static const int kAttackRaceTable[4] = {0, 0, 0, 0};

int SolarSystem::getAttackRace() {
    uint32_t r = this->faction;
    if (r < 4)
        return kAttackRaceTable[r];
    return 8;
}

uint32_t SolarSystem::hasNoOwner() {
    uint32_t x = this->systemId - 0x17;
    if (x < 0xb)
        return (0x60bU >> (x & 0x7ff)) & 1;
    return 0;
}

static const int kBlueprintStations[5] = {0, 0, 0, 0, 0};

static void * gBlueprintRoot = nullptr;

int SolarSystem::hasHiddenBlueprint() {
    char *base = *(char **) gBlueprintRoot;
    for (uint32_t i = 0; i <= 4; i++) {
        if (stationIsInSystem(kBlueprintStations[i]) != 0) {
            char *flags = *(char **) (*(char **) (base + 0x58) + 4);
            if (flags[i] == 0)
                return 1;
        }
    }
    return 0;
}

void SolarSystem::setCoords(int x, int y) {
    this->mapX = x;
    this->mapY = y;
}

int SolarSystem::getWarpGateEnumIndex() {
    return (int) getStationEnumIndex(this->jumpgateStationId);
}

static Galaxy ** gGalaxyDiscover = nullptr;

int SolarSystem::isFullyDiscovered() {
    Array<int> *arr = this->stationIds;
    uint32_t i = 0;
    Galaxy *gal = *gGalaxyDiscover;
    while (true) {
        if (i >= arr->size())
            return 1;
        char *visited = (char *) gal->getVisited();
        arr = this->stationIds;
        uint32_t flagIdx = (*arr)[i];
        i++;
        if (visited[flagIdx] == 0)
            return 0;
    }
}

int SolarSystem::stationIsInSystem(Station *station) {
    if (station == nullptr)
        return 0;
    return stationIsInSystem(station->getIndex());
}
