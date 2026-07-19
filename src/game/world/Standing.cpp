#include "game/world/Standing.h"
#include "game/mission/Status.h"
#include "game/world/SolarSystem.h"

Standing::Standing() {
    int *p = new int[2];
    p[0] = 0x1e;
    p[1] = 0;
    this->standings = p;
    this->currentRace = -1;
}

Standing::~Standing() {
    delete[] this->standings;
    this->standings = nullptr;
}

int *Standing::getStandings() {
    return this->standings;
}

void Standing::setStandings(int *arr) {
    this->standings = arr;
}

void Standing::applyPoints(int race, int delta) {
    int *p = this->standings;
    int v = delta + p[race];
    p[race] = v;
    if (v > 100) {
        p[race] = 100;
    } else if (v < -100) {
        p[race] = -100;
    }
}

float Standing::getStandingRate(int race) {
    return (float) this->getStanding(race) / 100.0f;
}

bool Standing::isEnemyWithAnyone() {
    int a = this->standings[0];
    int b = this->standings[1];
    return ((unsigned) (b + 0x46) > 0x8c) || ((unsigned) (a + 0x46) > 0x8c);
}

static const uint32_t Standing_enemyRaceTable[4] = {1, 0, 3, 2};

uint32_t Standing::getEnemyRace(int idx) {
    if ((unsigned) idx < 4)
        return Standing_enemyRaceTable[idx];
    return 8;
}

unsigned Standing::isNeutral(int race) {
    if (this->isEnemy(race)) return 0;
    return this->isFriend(race) ? 0 : 1;
}

int Standing::getStanding(int race) {
    int cr = this->currentRace;
    if (cr >= 0) {
        if (race == 0) {
            if (cr == 0) return 100;
            if (cr == 1) return -100;
            return 0x46;
        }
        if (race == 1) {
            if (cr == 2) return 100;
            if (cr == 3) return -100;
            return 0x46;
        }
    }
    return this->standings[race];
}

void Standing::applyMissionCompleted(int race) {
    this->applyPoints(race, -5);
}

void Standing::setStanding(int race, int value) {
    this->standings[race] = value;
}

void Standing::setPlayerSignatureRace(int race) {
    this->currentRace = race;
}

void Standing::applyStealCargo(int race) {
    this->applyPoints(race, 2);
}

bool Standing::isEnemy(int race) {
    int cr = this->currentRace;
    if (cr >= 0) {
        if (race != 1) {
            if (race == 3) {
                cr = cr - 2;
            } else if (race == 2) {
                cr = cr - 3;
            } else {
                if (race != 0) return false;
                cr = cr - 1;
            }
        }
        return cr == 0;
    }
    if (race == 1 || race == 0) {
        return this->standings[0] < -0x46;
    }
    if (race == 2) {
        return this->standings[1] < -0x46;
    }
    if (race == 3) {
        return 0x46 < this->standings[1];
    }
    return false;
}

void Standing::applyDelict(int kind, int severity) {
    int hc = Status::gStatus->hardCoreMode();
    int delta = severity << hc;
    switch (kind) {
        case 0:
            this->applyPoints(0, -delta);
            return;
        case 1:
            this->applyPoints(0, delta);
            return;
        case 2:
            this->applyPoints(1, -delta);
            return;
        case 3:
            this->applyPoints(1, delta);
            return;
        default:
            return;
    }
}

static const int g_apk_raceTable[4] = {1, 0, 3, 2};

void Standing::applyKill(int kind) {
    Status *status = Status::gStatus;
    unsigned sysRace;
    if (status->inAlienOrbit() != 0) {
        sysRace = 9;
    } else {
        sysRace = ((SolarSystem *) (long) status->getSystem())->getRace();
    }
    int delta;
    if (kind == 8) {
        if (this->currentRace >= 0) return;
        delta = 1;
        if (sysRace < 4) {
            kind = g_apk_raceTable[sysRace];
        } else {
            kind = 8;
        }
    } else {
        delta = 5;
    }
    this->applyPoints(kind, delta);
}

bool Standing::isFriend(int race) {
    int cr = this->currentRace;
    if (cr >= 0) {
        if (race == 1) {
            cr = cr - 1;
        } else if (race == 3) {
            cr = cr - 3;
        } else if (race == 2) {
            cr = cr - 2;
        } else if (race != 0) {
            return false;
        }
        return cr == 0;
    }
    if (race == 1 || race == 0) {
        return 0x46 < this->standings[0];
    }
    if (race == 2) {
        return 0x46 < this->standings[1];
    }
    if (race == 3) {
        return this->standings[1] < -0x46;
    }
    return false;
}

float Standing::getMissionBonus(int race) {
    float s0;
    switch (race) {
        case 0:
            s0 = (float) this->standings[0];
            break;
        case 1:
            s0 = (float) (-this->standings[0]);
            break;
        case 2:
            s0 = (float) this->standings[1];
            break;
        case 3:
            s0 = (float) (-this->standings[1]);
            break;
        default:
            return 0.0f;
    }
    float r = s0 / 100.0f;
    return r > 0.0f ? r : 0.0f;
}

void Standing::applyDisable(int race) {
    this->applyPoints(race, 2);
}

void Standing::rehabilitate(int race) {
    if (race == 0) {
        this->standings[0] = -35;
    } else if (race == 1) {
        this->standings[0] = 35;
    } else if (race == 2) {
        this->standings[1] = -35;
    } else if (race == 3) {
        this->standings[1] = 35;
    }
}
