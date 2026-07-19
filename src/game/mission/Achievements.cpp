#include "game/mission/Achievements.h"
Achievements *Achievements::gAchievements = nullptr;
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"

uint8_t Achievements::hasMedal(int index, int value) {
    return this->medals[index] == value;
}

int *Achievements::getMedals() {
    return this->medals;
}

int *Achievements::getNewMedals() {
    return this->newMedals;
}

Achievements::~Achievements() {
    delete[] this->medals;
    this->medals = nullptr;
    delete[] this->newMedals;
    this->newMedals = nullptr;
}

uint8_t Achievements::gotAllSupernovaMedals() {
    return this->gotAllSupernovaMedals_;
}

uint8_t Achievements::gotAllMedals() {
    return this->gotAllMedals_;
}

void Achievements::updateCredits(int value) {
    if (this->credits < value)
        this->credits = value;
}

void Achievements::setMedal(int index, int value) {
    this->medals[index] = value;
}

void Achievements::incPirateKills() {
    this->pirateKills += 1;
}

int Achievements::init() {
    for (int i = 0; i < 0x2d; ++i)
        this->medals[i] = 0;
    this->medals[0] = 1;
    for (int i = 0; i < 0x2d; ++i)
        this->newMedals[i] = 0;
    this->hasTurretAndWeapon = 0;
    this->credits = 0;
    this->kills = 0;
    this->catches = 0;
    this->pirateKills = 0;
    this->weaponCount = 0;
    return (int) (intptr_t) & this->kills;
}

uint8_t Achievements::isEliteMedal(int index) {
    return index > 0x23;
}

Achievements::Achievements() {
    this->medals = new int[45];
    this->newMedals = new int[45];
    this->gotAllSupernovaMedals_ = 0;
    this->gotAllMedals_ = 0;
}

void Achievements::resetNewMedals() {
    for (int i = 0; i < 0x2d; ++i)
        this->newMedals[i] = 0;
    this->hasTurretAndWeapon = 0;
    this->kills = 0;
    this->catches = 0;
    this->pirateKills = 0;
    this->weaponCount = 0;
}

int Achievements::getKills() {
    return this->kills;
}

int Achievements::getCatches() {
    return this->catches;
}

int Achievements::getPirateKills() {
    return this->pirateKills;
}

void Achievements::incCatches() {
    this->catches += 1;
}

void Achievements::incKills() {
    this->kills += 1;
}

static const int gAchievementValues[135] = {0};

int Achievements::getValue(int index, int sub) {
    const int *row = gAchievementValues + index * 3;
    return row[sub - 1];
}

void Achievements::resetPirateKills() {
    this->pirateKills = 0;
}

uint8_t Achievements::gotAllGoldMedals() {
    return this->gotAllGoldMedals_;
}

void Achievements::countMedals() {
    int total = 0;
    int golds = 0;
    this->medalCount = 0;
    for (int i = 0; i < 0x24; ++i) {
        int v = this->medals[i];
        if (v != 0) {
            total += 1;
            this->medalCount = total;
            if (v == 1)
                golds += 1;
        }
    }
    int supers = 0;
    for (int j = 0; j < 9; ++j) {
        if (this->medals[0x24 + j] != 0)
            supers += 1;
    }
    uint8_t allGold = (golds == 0x24);
    this->gotAllSupernovaMedals_ = (supers == 9) & allGold;
    this->gotAllGoldMedals_ = allGold;
    this->gotAllMedals_ = (total == 0x24);
}

static const int gCFN_req[135] = {0};

void Achievements::checkForNewMedal(PlayerEgo *ego) {
    (void) ego;
    initCheckEquipmentAndWeapons();

    for (unsigned m = 0; m < 0x2d; ++m) {
        int got = 0;
        if (this->medals[m] != 1) {
            for (unsigned tier = 0; tier < 3; ++tier) {
                int req = gCFN_req[m * 3 + tier];
                if (req < 0)
                    break;
                if (got > 0)
                    break;
            }
            int cur = this->medals[m];
            if (got < cur || cur == 0)
                this->newMedals[m] = got;
        } else {
            this->newMedals[m] = got;
        }
    }
}

void Achievements::applyNewMedals() {
    for (int i = 0; i < 0x2d; ++i) {
        int nv = this->newMedals[i];
        if (nv > 0) {
            int cur = this->medals[i];
            if (nv < cur || cur == 0)
                this->medals[i] = nv;
        }
    }
    countMedals();
    if (this->medalCount == 0x23) {
        this->newMedals[0x23] = 1;
        this->medals[0x23] = 1;
        countMedals();
    }
}

static Status *gAchStatus = nullptr;
static Status *const* gAchStatusHolder = &gAchStatus;

void Achievements::initCheckEquipmentAndWeapons() {
    Status *status = *gAchStatusHolder;
    uint8_t result;
    if (status->getCurrentCampaignMission() < 8) {
        result = 1;
    } else {
        Ship *ship = status->getShip();
        Array<Item *> *eq = ship->getEquipment();
        int weapons = 0;
        int turrets = 0;
        if (eq != nullptr) {
            for (unsigned i = 0; i < eq->size(); ++i) {
                Item *it = (*eq)[i];
                if (it != nullptr && it->getType() != 4) {
                    if (it->getType() == 0) {
                        this->weaponCount += 1;
                    } else if (it->getType() == 3) {
                        turrets += 1;
                        continue;
                    }
                    weapons += 1;
                }
            }
        }
        result = (turrets > 0) & (weapons > 0);
    }
    this->hasTurretAndWeapon = result;
}

void Achievements::setMedals(int *src, int count) {
    for (int i = 0; i < count; ++i) {
        unsigned v = (unsigned) src[i];
        this->medals[i] = (v < 4) ? (int) v : 0;
    }
    for (; count < 0x2d; ++count)
        this->medals[count] = 0;
}
