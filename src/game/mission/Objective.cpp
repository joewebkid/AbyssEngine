#include "game/mission/Objective.h"
#include "game/core/RadioMessage.h"
#include "game/world/Level.h"
#include "game/world/Route.h"
#include "game/world/Waypoint.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"

Objective::Objective(int type, int value, Level *level) {
    this->type = type;
    this->value = value;
    this->calcValue = 0;
    this->level = level;
    this->children = nullptr;
    this->achievedText = nullptr;
    this->storedValue = 0;
}

Objective::Objective(int type, int value, int calcValue, Level *level) {
    this->type = type;
    this->value = value;
    this->calcValue = calcValue;
    this->level = level;
    this->children = nullptr;
    this->achievedText = nullptr;
    this->storedValue = (type == 0xd) ? calcValue : 0;
}

Objective::~Objective() {
    if (this->children != nullptr) {
        ArrayReleaseClasses<Objective *>(*this->children);
        delete this->children;
        this->children = nullptr;
    }

    if (this->achievedText != nullptr) {
        delete this->achievedText;
        this->achievedText = nullptr;
    }
}

Objective *Objective::addObjective(Objective *objective) {
    if (this->children == nullptr)
        this->children = new Array<Objective *>();
    ArrayAdd<Objective *>(objective, *this->children);
    return this;
}

void Objective::setAchievedText(AbyssEngine::String *text) {
    this->achievedText = new AbyssEngine::String(*text);
}

AbyssEngine::String *Objective::getAchievedText() {
    return this->achievedText;
}

bool Objective::isSurvivalObjective() {
    return this->type == 3;
}

bool Objective::getCalcValue() {
    return this->type == 3;
}

unsigned int Objective::achieved(int value) {
    Array<KIPlayer *> *enemies = this->level->getEnemies();
    unsigned int result = 0;

    switch (this->type) {
        case 0:
            result = this->level->getEnemiesLeft() == 0;
            break;
        case 1:
            return (*this->level->getEnemies())[this->value]->isDead();
        case 2: {
            Waypoint *last =
                    ((Route *) (intptr_t) this->level->getPlayerRoute())->getLastWaypoint();
            result = last->state != 0;
            break;
        }
        case 3:
            result = this->value < value;
            break;
        case 4:
            return ((RadioMessage *) (*this->level->getMessages())[this->value])->isOver();
        case 5:
            result = this->level->getFriendsLeft() == 0;
            break;
        case 7: {
            result = 0;
            for (int i = 0; i < this->value; i++)
                result += (*enemies)[i]->isDead();
            result = (int) result == this->value;
            break;
        }
        case 8: {
            Array<KIPlayer *> *asteroids = this->level->getAsteroids();
            result = 0;
            for (uint32_t i = 0; i < asteroids->size(); i++)
                result += (*asteroids)[i]->isDead();
            result = (int) result > this->value;
            break;
        }
        case 9: {
            Array<KIPlayer *> *asteroids = this->level->getAsteroids();
            for (uint32_t i = 0; i < asteroids->size(); i++) {
                if ((int) i >= this->value)
                    return this->value < 1;
            }
        }
            [[fallthrough]];
        case 10: {
            Array<KIPlayer *> *asteroids = this->level->getAsteroids();
            for (uint32_t i = 0; i < asteroids->size(); i++) {
                if ((int) i >= this->value)
                    return 0;
            }
        }
            [[fallthrough]];
        case 11:
            result = (*this->level->getEnemies())[this->value]->lostMissionCrateToEgo != 0;
            break;
        case 12:
            result = (*this->level->getEnemies())[this->value]->diedWithMissionCrate != 0;
            break;
        case 15:
            return ((Player *) (*this->level->getEnemies())[this->value]->player)->isActive();
        case 16: {
            for (uint32_t i = 0; i < enemies->size(); i++) {
                if ((*enemies)[i]->lostMissionCrateToEgo == 0)
                    return 0;
            }
            return 1;
        }
        case 17: {
            for (uint32_t i = 0; i < enemies->size(); i++) {
                if ((*enemies)[i]->diedWithMissionCrate != 0)
                    return 1;
            }
            return 0;
        }
        case 18: {
            result = 0;
            for (int i = this->value; i < this->calcValue; i++)
                result += (*enemies)[i]->isDead();
            result = (int) result == this->calcValue - this->value;
            break;
        }
        case 19:
            return this->level->friendCargoWasStolen();
        case 20:
        case 21: {
            result = 0;
            for (int i = this->value; i < this->calcValue; i++) {
                KIPlayer *enemy = (*enemies)[i];
                if (enemy->isDead() != 0 && enemy->shipGroup == 8)
                    result++;
            }
            if ((int) result != this->calcValue - this->value)
                return 0;
            if (this->type != 0x14)
                return this->level->killCountB <= this->level->killCountA;
            result = this->level->killCountB > this->level->killCountA;
            break;
        }
        case 22: {
            Array<void *> *messages = this->level->getMessages();
            return ((RadioMessage *) (*messages)[messages->size() - 1])->isOver();
        }
        case 23:
            result = (uint8_t)(*this->level->getEnemies())[this->value]->field_0x24 != 0;
            break;
        case 25:
            result = *(float *) &(*this->level->getEnemies())[this->value]->field_0x64 == 0.0f;
            break;
        case 26: {
            for (int i = this->value; i < this->calcValue; i++) {
                if ((*enemies)[i]->isDead() != 0)
                    return 1;
            }
            return 0;
        }
        case 27:
            result = this->level->getEnemiesLeft() < this->value;
            break;
        case 28:
            result = this->level->getNumDeliveredOre() >= this->value;
            break;
        case 29:
            result = this->level->getNumDeliveredPassengers() >= this->value;
            break;
        case 30:
            return (*this->level->getEnemies())[this->value]->isDying();
        default:
            break;
    }
    return result;
}

