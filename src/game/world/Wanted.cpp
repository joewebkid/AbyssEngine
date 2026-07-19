#include "game/world/Wanted.h"
#include "game/core/String.h"

int Wanted::getIndex() { return index; }
int Wanted::getBoard() { return board; }
int Wanted::getRace() { return race; }
int Wanted::isMale() { return male; }
int Wanted::getShip() { return shipId; }
int Wanted::getWeapon() { return weapon; }
int Wanted::getHitpoints() { return hitpoints; }
int Wanted::getLoot() { return lootItemId; }
int Wanted::getLootAmount() { return lootAmount; }
int Wanted::getReward() { return reward; }
int Wanted::getRequiredBounties() { return requiredBounties; }
int Wanted::getRequiredMission() { return requiredMission; }
int Wanted::getNumWingmen() { return numWingmen; }
int Wanted::getCurrentLocation() { return currentLocation; }
int Wanted::getTravelsTo() { return travelsTo; }
int Wanted::getLastSeen() { return lastSeen; }
int *Wanted::getImageParts() { return imageParts; }

void Wanted::setImageParts(int *parts) { imageParts = parts; }
void Wanted::setRequiredMission(int v) { requiredMission = v; }
void Wanted::setCurrentLocation(int v) { currentLocation = v; }
void Wanted::setTravelsTo(int v) { travelsTo = v; }
void Wanted::setLastSeen(int v) { lastSeen = v; }

uint8_t Wanted::isTerminated() { return terminated; }
void Wanted::setTerminated(bool v) { terminated = v; }
uint8_t Wanted::isActive() { return active; }
void Wanted::setActive(bool v) { active = v; }

String Wanted::getName() {
    return name;
}

Wanted::Wanted(int index, String name, int board, int race, bool male,
               int shipId, int weapon, int hitpoints, int lootItemId, int lootAmount,
               int reward, int requiredBounties, int requiredMission, int numWingmen) {
    this->index = index;
    this->name = name;
    this->board = board;
    this->race = race;
    this->male = male;
    this->terminated = 0;
    this->active = 0;
    this->imageParts = nullptr;
    this->currentLocation = -1;
    this->travelsTo = -1;
    this->lastSeen = -1;
    this->shipId = shipId;
    this->weapon = weapon;
    this->hitpoints = hitpoints;
    this->lootItemId = lootItemId;
    this->lootAmount = lootAmount;
    this->reward = reward;
    this->requiredBounties = requiredBounties;
    this->requiredMission = requiredMission;
    this->numWingmen = numWingmen;
}

Wanted::~Wanted() {
    delete[] imageParts;
    imageParts = nullptr;
}
