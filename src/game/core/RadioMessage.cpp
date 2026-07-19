#include "game/core/RadioMessage.h"
#include "game/core/Radio.h"
#include "game/core/Vector.h"
#include "game/mission/Objective.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "game/weapons/Radar.h"
#include "game/world/Level.h"
#include "game/world/LevelScript.h"
#include "game/world/Route.h"

RadioMessage::RadioMessage(int textID, int imageID, int conditionType, int conditionValue,
                           int targetCount) {
    this->targetIndices = nullptr;
    this->radio = nullptr;
    this->objective = nullptr;
    this->textID = textID;
    this->imageID = imageID;
    this->conditionType = conditionType;
    this->conditionValue = conditionValue;

    int *values = new int[targetCount];
    this->targetIndices = values;
    for (int i = 0; i < targetCount; ++i) {
        values[i] = conditionValue + i;
    }

    this->targetCount = targetCount;
    this->triggeredFlag = 0;
    this->over = 0;
    this->lastRouteIndex = 0;
}

RadioMessage::RadioMessage(int textID, int imageID, int conditionType, int conditionValue) {
    this->targetIndices = nullptr;
    this->radio = nullptr;
    this->objective = nullptr;
    this->textID = textID;
    this->imageID = imageID;
    this->conditionType = conditionType;
    this->conditionValue = conditionValue;

    int *values = new int[1];
    values[0] = conditionValue;
    this->targetCount = 1;
    this->targetIndices = values;

    this->triggeredFlag = 0;
    this->over = 0;
    this->lastRouteIndex = 0;
}

RadioMessage::RadioMessage(int textID, int imageID, Objective *objective) {
    this->targetIndices = nullptr;
    this->radio = nullptr;
    this->objective = objective;
    this->textID = textID;
    this->imageID = imageID;
    this->conditionType = 0xb;
    this->triggeredFlag = 0;
    this->over = 0;
    this->lastRouteIndex = 0;
}

RadioMessage::~RadioMessage() {
    delete[] this->targetIndices;
    this->targetIndices = nullptr;
}

void RadioMessage::finish() {
    this->over = 1;
}

uint8_t RadioMessage::isOver() {
    return this->over;
}

void RadioMessage::reset() {
    this->lastRouteIndex = 0;
    this->triggeredFlag = 0;
    this->over = 0;
}

void RadioMessage::trigger() {
    this->triggeredFlag = 1;
}

uint8_t RadioMessage::isTriggered() {
    return this->triggeredFlag;
}

void RadioMessage::setRadio(Radio *radio) {
    this->radio = radio;
}

int RadioMessage::getTextID() {
    return this->textID;
}

int RadioMessage::getImageID() {
    return this->imageID;
}

int RadioMessage::getSoundID() {
    return 0;
}

namespace {
    uint8_t enemyFriendFlag(Player *player) {
        return static_cast<uint8_t>(player->enemyFlags >> 8);
    }
}

int RadioMessage::triggered(int64_t time, PlayerEgo *ego, LevelScript *script) {
    if (this->triggeredFlag != 0) {
        return 0;
    }

    auto setResult = [this](int value) -> int {
        this->triggeredFlag = static_cast<uint8_t>(value);
        if (value == 0) {
            return 0;
        }
        this->radio->currentMessage = this;
        return 1;
    };
    auto triggerResult = [this]() -> int {
        this->triggeredFlag = 1;
        this->radio->currentMessage = this;
        return 1;
    };

    auto selectTarget = [this](Array<Player *> *list, int i) -> Player * {
        return list->data()[this->targetIndices[i]];
    };

    switch (this->conditionType) {
        case 0: {
            if (ego->getRoute() == 0) {
                break;
            }
            int current = reinterpret_cast<Route *>(static_cast<intptr_t>(ego->getRoute()))->getCurrent();
            int last = this->lastRouteIndex;
            int value = 0;
            if (current > last) {
                value = (last == this->conditionValue);
            }
            this->lastRouteIndex = current;
            this->triggeredFlag = static_cast<uint8_t>(value);
            if (value != 0) {
                this->radio->currentMessage = this;
                return 1;
            }
            return 0;
        }

        case 1: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                if (selectTarget(list, i)->isDead()) {
                    return triggerResult();
                }
            }
            break;
        }

        case 2: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                Player *player = selectTarget(list, i);
                if (enemyFriendFlag(player) != 0 && player->isDead()) {
                    return triggerResult();
                }
            }
            break;
        }

        case 3:
            return setResult(ego->level->getEnemiesLeft() < 1);

        case 4:
            return setResult(ego->level->getFriendsLeft() < 1);

        case 5:
            return setResult(time >= static_cast<int64_t>(this->conditionValue));

        case 6: {
            RadioMessage * message = this->radio->getMessage(this->conditionValue);
            return setResult(message->triggeredFlag);
        }

        case 8: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                Player *player = selectTarget(list, i);
                if (!player->isAsteroid() && player->isActive()) {
                    return triggerResult();
                }
            }
            break;
        }

        case 9: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                if (!selectTarget(list, i)->isDead()) {
                    break;
                }
                if (i + 1 >= this->targetCount) {
                    return triggerResult();
                }
            }
            break;
        }

        case 10: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                Player *player = selectTarget(list, i);
                if (!player->isAsteroid() && enemyFriendFlag(player) != 0 && player->isActive()) {
                    return triggerResult();
                }
            }
            break;
        }

        case 0x0b:
            return setResult(this->objective->achieved(static_cast<int>(time)));

        case 0x0c: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                Player *target = selectTarget(list, i);
                if (target->getHitpoints() < target->getMaxHitpoints() / 2) {
                    return triggerResult();
                }
            }
            break;
        }

        case 0x0e: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            KIPlayer *ki = list->data()[this->conditionValue]->getKIPlayer();
            return setResult(ki->lostMissionCrateToEgo);
        }

        case 0x0f: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (uint32_t i = 0; i < list->size(); ++i) {
                Player *player = list->data()[i];
                if (!player->isAsteroid() && player->isDead()) {
                    return triggerResult();
                }
            }
            break;
        }

        case 0x10: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (uint32_t i = 0; i < list->size(); ++i) {
                Player *player = list->data()[i];
                if (!player->isAsteroid() && player->isActive() && !player->isAlwaysFriend()) {
                    return triggerResult();
                }
            }
            break;
        }

        case 0x11: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (uint32_t i = 0; i < list->size(); ++i) {
                if (i == static_cast<uint32_t>(this->conditionValue)) {
                    continue;
                }
                Player *player = list->data()[i];
                if (player->isAsteroid()) {
                    continue;
                }
                if (player->isDead()) {
                    break;
                }
            }
            return triggerResult();
        }

        case 0x12: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            KIPlayer *ki = list->data()[this->conditionValue]->getKIPlayer();
            if (ki->lostMissionCrateToEgo != 0) {
                return triggerResult();
            }
            ki = list->data()[this->conditionValue]->getKIPlayer();
            return setResult(ki->field_0x6a);
        }

        case 0x13: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                Player *target = selectTarget(list, i);
                if (target->getHitpoints() < target->getMaxHitpoints() / 4) {
                    return triggerResult();
                }
            }
            break;
        }

        case 0x14: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            int dead = 0;
            for (uint32_t i = 0; i < list->size(); ++i) {
                Player *player = list->data()[i];
                if (!player->isAsteroid()) {
                    dead += player->isDead();
                }
                if (dead >= this->conditionValue) {
                    return triggerResult();
                }
            }
            break;
        }

        case 0x15: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            KIPlayer *ki = list->data()[this->conditionValue]->getKIPlayer();
            return setResult(ki->field_0x24);
        }

        case 0x16:
            return setResult(ego->level->field_1c >= this->conditionValue);

        case 0x17:
            return setResult(static_cast<Radar *>(ego->field_0x14)->stationLocked());

        case 0x18: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            Player *player = list->data()[this->conditionValue];
            if (!player->isActive()) {
                int value = (!player->isDead()) & (time > 0xea5fLL);
                return setResult(value);
            }
            break;
        }

        case 0x19: {
            if (ego->getRoute() == 0) {
                break;
            }
            int current = reinterpret_cast<Route *>(static_cast<intptr_t>(ego->getRoute()))->getCurrent();
            int last = this->lastRouteIndex;
            this->lastRouteIndex = current;
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            int active = 0;
            for (uint32_t i = 0; i < list->size(); ++i) {
                Player *player = list->data()[i];
                if (!player->isAsteroid()) {
                    active += player->isDead() ^ 1;
                }
            }
            int value = (active >= this->conditionValue) & (current > last) & (last == 0);
            return setResult(value);
        }

        case 0x1a: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            Player *player = list->data()[0];
            if (!player->isActive() || player->isDead()) {
                break;
            }
            Vector pos = player->getPosition();
            float dz = pos.z - static_cast<float>(this->conditionValue);
            if (dz > 0.0f) {
                return setResult(dz < 5000.0f);
            }
            return setResult(dz > -5000.0f);
        }

        case 0x1b:
            return setResult(script->getEvent() - this->conditionValue == 0);

        case 0x1c:
            return setResult(static_cast<Player *>(ego->player)->getArmorHP() < 1);

        case 0x1e: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            int dead = 0;
            for (int i = 2; i != 6; ++i) {
                Player *player = list->data()[i];
                if (!player->isAsteroid()) {
                    dead += player->isDead();
                }
            }
            return setResult(dead - this->conditionValue == 0);
        }

        case 0x1f: {
            Array<Player *> *list = static_cast<Player *>(ego->player)->getEnemies();
            for (int i = 0; i < this->targetCount; ++i) {
                Player *target = selectTarget(list, i);
                if (target->getHitpoints() < target->getMaxHitpoints() / 4 * 3) {
                    return triggerResult();
                }
            }
            break;
        }
    }

    this->triggeredFlag = 0;
    return 0;
}
