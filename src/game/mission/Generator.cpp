#include "game/mission/Generator.h"
#include "engine/render/ImageFactory.h"
#include "game/ship/Ship.h"
#include "engine/core/AERandom.h"
#include "game/world/Galaxy.h"
#include "game/mission/Item.h"
#include "game/mission/Achievements.h"
#include "game/ship/Agent.h"
#include "game/core/Globals.h"
#include "game/world/Station.h"
#include "game/mission/Mission.h"
#include "game/world/SolarSystem.h"
#include "game/world/Standing.h"
#include "game/world/Wanted.h"
#include "game/mission/Status.h"

Generator::Generator() {
}

Generator::~Generator() {
}

void Generator::computerTradeGoods(Station *station) {
    if (station->getIndex() != 0x6c) {
        Array<Item *> *items = station->getItems();
        if (items != nullptr) {
            for (uint32_t i = 0; i < items->size(); ++i) {
                int amount = items->data()[i]->getAmount();
                int take = static_cast<AERandom *>(Globals::rnd)->nextInt(3);
                if (take < amount) {
                    items->data()[i]->changeAmount(-take);
                }
            }
        }
    }
}

// Android 2.0.16 literal table at 0x1fb6f0.
static const volatile int g_Generator_stationBlockList[52] = {
    10, 22, 27, 29, 30, 48, 55, 56, 76, 79, 91, 98,
    1, 33, 47, 86, 58, 65, 66, 74, 81, 100, 101, 102,
    103, 104, 105, 106, 107, 108, 83, 85, 92, 109, 110, 11,
    113, 121, 126, 131, 38, 90, 91, 93, 94, 82, 40, 15,
    60, 95, 70, 80
};

int Generator::generateStationIndex(Array<SolarSystem *> *systems, int station) {
    uint32_t accepted = 0;
    int selected = 0;

    while ((accepted & 1) == 0) {

        int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(100);
        selected = station;
        if (roll >= 20) {
            roll = static_cast<AERandom *>(Globals::rnd)->nextInt(100);
            if (roll < 40) {
                int currentIndex = Globals::status->getSystem()->getIndex();
                Array<int> *stations =
                        (Array<int> *) systems->data()[currentIndex]->getStations();
                int pick = static_cast<AERandom *>(Globals::rnd)->nextInt(stations->size());
                selected = stations->data()[pick];
            } else {
                selected = static_cast<AERandom *>(Globals::rnd)->nextInt(135);
            }
        }

        if (Globals::status->getSystem()->getIndex() == 0xf) {
            Array<int> *stations =
                    (Array<int> *) Globals::status->getSystem()->getStations();
            int pick = static_cast<AERandom *>(Globals::rnd)->nextInt(
                reinterpret_cast<Array<int> *>(Globals::status->getSystem()->getStations())->size());
            selected = stations->data()[pick];
        }

        accepted = 1;
        for (uint32_t blocked = 0; blocked <= 0x33;) {
            int blockedStation = g_Generator_stationBlockList[blocked++];
            if (selected == blockedStation) {
                accepted = 0;
                break;
            }
        }

        uint32_t systemIndex = 0;
        while (systemIndex < systems->size()) {
            if (systems->data()[systemIndex]->stationIsInSystem(selected) != 0) {
                goto station_system_found;
            }
            ++systemIndex;
        }
        systemIndex = 0;
station_system_found:

        if ((uint32_t)(selected - 0x6d) < 5) {
            accepted = false;
        }

        Array<bool> *visibilities = Globals::status->getSystemVisibilities();
        if (visibilities != nullptr && visibilities->size() > systemIndex) {
            accepted = (*visibilities)[systemIndex];
        }

        if (systems->data()[systemIndex]->getRoutes() == nullptr) {
            if (systemIndex != (uint32_t) Globals::status->getSystem()->getIndex()) {
                accepted = false;
            }
        }
    }
    return selected;
}

// Android 2.0.16 literal table at 0x1fb590.
static const volatile int kaamoSpecialItems[9] = {
    200, 220, 208, 213, 216, 228, 229, 230, 231
};
// Android 2.0.16 literal table at 0x1fb7c0.
static const volatile int g_Generator_offerShipIds[6] = {
    55, 56, 57, 58, 59, 60
};
// Android 2.0.16 literal table at 0x1fb7d8.
static const volatile int g_Generator_enemyRaces[4] = {
    2, 3, 0, 1
};

Array<Agent *> *Generator::createAgents(Station *station) {

    Array<Agent *> *result = nullptr;
    if (Globals::status->inSupernovaSystem() == 0) {
        Array<Agent *> *existing = Globals::status->getAgents();
        int mission = Globals::status->getCurrentCampaignMission();
        bool keepExisting = mission > 0x10;
        if (!Globals::status->dlc1Won()) {
            keepExisting = (mission > 0x10) & (station->getIndex() != 0x6a);
        }

        uint32_t count = 0;
        for (uint32_t i = 0; i < existing->size(); ++i) {
            count += (existing->data()[i]->getStation() == station->getIndex()) & keepExisting;
        }

        if (station->getIndex() != 0x6c) {
            int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
            if ((int) (roll + count + 3) < 5) {
                count += 3 + static_cast<AERandom *>(Globals::rnd)->nextInt(2);
            } else {
                count = 5;
            }
        }

        result = new Array<Agent *>();
        ArraySetLength(count, *result);

        uint32_t out = 0;
        for (uint32_t i = 0; i < existing->size(); ++i) {
            if (!((existing->data()[i]->getStation() != station->getIndex()) | !keepExisting)) {
                result->data()[out++] = existing->data()[i];
                if (existing->data()[i]->getOffer() == 9) {
                    int index;
                    if (Globals::status->getCurrentCampaignMission() < 0x8e) {
                        index = static_cast<AERandom *>(Globals::rnd)->nextInt(7) + 2;
                    } else {
                        index = static_cast<AERandom *>(Globals::rnd)->nextInt(9);
                    }
                    int itemId = kaamoSpecialItems[index];
                    Item *item = static_cast<Array<Item *> *>(Globals::items)->data()[itemId];
                    int amount = ((uint32_t)(index - 3) < 2) ? 10 : 1;
                    existing->data()[i]->setSellItemData(itemId, amount,
                                           item->getSinglePrice() * amount);
                    existing->data()[i]->setOfferAccepted(false);
                } else if (existing->data()[i]->getOffer() == 10) {
                    Array<int> *choices = new Array<int>();
                    for (int j = 0; j != 6; ++j) {
                        int shipId = g_Generator_offerShipIds[j];
                        if (Globals::status->voidStation
                            ->hasShip(shipId) == 0) {
                            if (Globals::status->getShip()->getIndex() != shipId) {
                                ArrayAdd(shipId, *choices);
                            }
                        }
                    }
                    if (choices->size() != 0) {
                        int pick = static_cast<AERandom *>(Globals::rnd)->nextInt(choices->size());
                        int shipId = choices->data()[pick];
                        existing->data()[i]->setSellItemData(
                            shipId, 1,
                            static_cast<Array<Ship *> *>(Globals::ships)->data()[shipId]->getPrice());
                        existing->data()[i]->setOfferAccepted(false);
                    } else {
                        existing->data()[i]->setOfferAccepted(true);
                    }
                    delete choices;
                }
            }
        }

        Array<SolarSystem *> *systems =
                static_cast<Galaxy *>(Globals::galaxy)->getSystems();
        if (Globals::status->getCurrentCampaignMission() == 0x17 &&
            station->getIndex() == 10) {
            Agent *agent = new Agent(-1,
                static_cast<Globals *>(Globals::globals)->getRandomName(0, true), station->getIndex(),
                                     Globals::status->getSystem()->getIndex(), 0, 1, -1, -1, -1, -1);
            agent->setOffer(2);
            agent->setSellItemData(0x44, 1, 100000);
            agent->setImageParts(static_cast<ImageFactory *>(Globals::imageFactory)->createChar(1, 0));
            result->data()[out++] = agent;
        }

        bool hasWingman = false;
        for (uint32_t i = out; i < result->size(); ++i) {
            result->data()[i] = createAgent(station);
            if (result->data()[i]->getOffer() == 6) {
                if (hasWingman) {
                    result->data()[i]->setOffer(1);
                } else {
                    hasWingman = true;
                }
            } else if (result->data()[i]->getOffer() == 0) {
                Agent *agent = result->data()[i];
                agent->setMission(createMission(agent, systems));
            }
        }

        if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) < 0x23) {
            Standing *standing = reinterpret_cast<Standing *>(static_cast<intptr_t>(Globals::status->getStanding()));
            for (uint32_t raceIndex = 0; raceIndex < 4; ++raceIndex) {
                int race = g_Generator_enemyRaces[raceIndex];
                if (standing->isEnemy(race)) {
                    for (uint32_t i = 0; i < result->size(); ++i) {
                        Agent *agent = result->data()[i];
                        if (result->data()[i]->isGenericAgent() && result->data()[i]->getOffer() != 7) {
                            result->data()[i] = nullptr;
                            agent = new Agent(-1,
                                static_cast<Globals *>(Globals::globals)->getRandomName(race, true), station->getIndex(),
                                              Globals::status->getSystem()->getIndex(), race, 1, -1,
                                              -1, -1, -1);
                            result->data()[i] = agent;
                            result->data()[i]->setOffer(7);
                            result->data()[i]->setImageParts(
                                static_cast<ImageFactory *>(Globals::imageFactory)->createChar(1, race));
                            break;
                        }
                    }
                }
            }
        }

        if ((Globals::status->getCurrentCampaignMission() == 0x17 &&
             station->getIndex() == 10) ||
            static_cast<AERandom *>(Globals::rnd)->nextInt(100) == 1) {
            for (; out < result->size(); ++out) {
                Agent *agent = result->data()[out];
                if (!agent->isStoryAgent() && agent->getOffer() == 0 &&
                    agent->getMission() != nullptr) {
                    int reward = agent->getMission()->getReward();
                    if (reward < 50000) {
                        int newReward = 50000;
                        int boosted = agent->getMission()->getReward() * 10;
                        if (boosted < 50000) {
                            newReward = boosted;
                        }
                        agent->getMission()->setReward(newReward);
                        break;
                    }
                }
            }
        }
    }
    return result;
}


Mission *Generator::createMission(Agent *agent,
                                  Array<SolarSystem *> *systems) {

    int agentStation = agent->getStation();
    int targetStation = generateStationIndex(systems, agentStation);
    if (Globals::status->getSystem()->getIndex() == 0xf) {
        Array<int> *stations =
                (Array<int> *) Globals::status->getSystem()->getStations();
        targetStation = stations->data()[0] + static_cast<AERandom *>(Globals::rnd)->nextInt(4);
    }

    uint32_t race = agent->getRace();
    uint32_t type = 0;
    bool selected = false;
    int guard = 999;
    Array<uint8_t> *used = (Array<uint8_t> *) Globals::status->field_50;
    while (!selected) {
        selected = guard < 1;
        uint32_t pick = static_cast<AERandom *>(Globals::rnd)->nextInt(15);
        --guard;
        type = pick;
        if (pick == 8 || (pick == 10 && race >= 4)) {
            continue;
        }
        if (used != nullptr) {
            uint8_t *data = used->data();
            if (data[pick] != 0) {
                uint32_t sum = 0;
                uint32_t usedCount = used->size();
                for (uint32_t i = 0; i != usedCount; ++i) {
                    sum += data[i];
                }
                if (sum == 0xe) {
                    for (uint32_t i = 0; i < used->size(); ++i) {
                        data[i] = 0;
                    }
                }
                continue;
            }
            data[pick] = 1;
        }
        if (pick == 0xc) {
            selected = true;
            if (Globals::status->getSystem()->getIndex() != 0x19) {
                type = pick;
            } else {
                type = 0xc;
            }
        } else {
            selected = true;
        }
    }

    if (Globals::status->getCurrentCampaignMission() < 0x10 ||
        (type == 0xf && Globals::options[0x37] == 0)) {
        switch (static_cast<AERandom *>(Globals::rnd)->nextInt(5)) {
            case 1:
                type = 0;
                break;
            case 2:
                type = 7;
                break;
            case 3:
                type = 4;
                break;
            case 4:
                type = 0xc;
                targetStation = agent->getStation();
                break;
            default:
                type = 0xb;
                break;
        }
    } else if (type == 0xc) {
        targetStation = agent->getStation();
    }

    if (agent->getOffer() == 5) {
        targetStation = agent->getStation();
        type = 8;
    } else {
        if (type < 0xf && ((1U << type) & 0x4801U) != 0) {
            while (targetStation == Globals::status->getStation()->getIndex()) {
                targetStation = generateStationIndex(systems, agent->getStation());
            }
        }
        if (race < 4 && type == 0xd) {
            SolarSystem *agentSystem = systems->data()[agent->getSystem()];
            if (agentSystem->getRoutes() == nullptr) {
                type = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : 4;
            } else {
                bool ok = false;
                while (!ok) {
                    targetStation =
                            generateStationIndex(systems, agent->getStation());
                    for (uint32_t i = 0; i < systems->size(); ++i) {
                        if (systems->data()[i]->stationIsInSystem(targetStation) &&
                            (uint32_t) systems->data()[i]->getRace() == race) {
                            ok = true;
                            break;
                        }
                    }
                }
                type = 0xd;
            }
        }
    }

    AbyssEngine::String agentName = agent->getName();
    int difficulty = Globals::status->getCurrentCampaignMission() > 15
        ? static_cast<AERandom *>(Globals::rnd)->nextInt(9) + 1
        : static_cast<AERandom *>(Globals::rnd)->nextInt(2) + 1;

    int itemId = 0;
    int amount = 0;
    if (type == 8) {
        Array<Item *> *items = static_cast<Array<Item *> *>(Globals::items);
        for (;;) {
            itemId = static_cast<AERandom *>(Globals::rnd)->nextInt(items->size() - 0x61) + 0x61;
            if (items->data()[itemId]->getOccurence() == 0 ||
                items->data()[itemId]->getSinglePrice() == 0) {
                continue;
            }
            auto ingredients = items->data()[itemId]->getIngredients();
            if (((itemId - 0x61) & 0xfffffffe) == 0x78 || itemId == 0x75 ||
                ((itemId - 0x61) & 0xfffffffe) == 0x12 || itemId == 0x83 ||
                itemId == 0xa4 || itemId == 0xaf || ingredients != nullptr) {
                continue;
            }
            break;
        }
        amount = static_cast<AERandom *>(Globals::rnd)->nextInt(15) + 5;
        difficulty = items->data()[itemId]->getTecLevel();
    } else {
        switch (type) {
            case 0:
                itemId = static_cast<AERandom *>(Globals::rnd)->nextInt(7);
                amount = (int) (((float) difficulty / 10.0f) * 95.0f) + 5;
                break;
            case 2:
                amount = (int) ((float) static_cast<AERandom *>(Globals::rnd)->nextInt(4)) + 2;
                break;
            case 3:
            case 5:
                itemId = type == 3 ? 0x74 : 0x75;
                amount = (int) (((float) difficulty / 10.0f) * 8.0f) + 2;
                break;
            case 0xb:
                amount = (int) (((float) difficulty / 10.0f) * 18.0f) + 2;
                break;
            case 0xf:
                amount = (int)(float) static_cast<AERandom *>(Globals::rnd)->nextInt(90) + 30;
                for (uint32_t i = 0; i < systems->size(); ++i) {
                    if (systems->data()[i]->stationIsInSystem(targetStation)) {
                        int *prob =
                                (int *) static_cast<Galaxy *>(Globals::galaxy)
                                ->getAsteroidProbabilities(Globals::status->getStation());
                        itemId = prob[static_cast<AERandom *>(Globals::rnd)->nextInt(3) * 2];
                        break;
                    }
                }
                break;
            default:
                amount = 0;
                itemId = 0;
                break;
        }
    }

    if (difficulty > 10) {
        difficulty = 10;
    }

    SolarSystem *from =
            systems->data()[Globals::status->getStation()->getSystem()];
    SolarSystem *to = systems->data()
    [((Station *) (intptr_t) static_cast<Galaxy *>(Globals::galaxy)
        ->getStation(targetStation))->getSystem()];
    float distance = static_cast<Galaxy *>(Globals::galaxy)->distance(from, to);
    float reward = ((float) distance / 1200.0f + 1.0f) *
                   (float) ((int) (((float) difficulty / 10.0f) * 5500.0f) +
                            1500);
    if (type == 9) {
        reward *= 1.2f;
    } else if (type == 7) {
        reward *= 0.7f;
    } else if (type == 8) {
        reward = (float) (amount * static_cast<Array<Item *> *>(Globals::items)
                          ->data()[itemId]
                          ->getMaxPrice()) *
                 1.7f;
    } else if (type == 3 || type == 5) {
        reward += reward;
    } else if (type == 0xb) {
        reward = reward * 0.6f + ((reward * 0.6f) / 5.0f) * (float) amount;
    }

    int level = Globals::status->getLevel();
    reward += (float) (level * level * level * 10);

    int bonus = 0;
    if ((type | 4) != 0xc) {
        Standing *standing = reinterpret_cast<Standing *>(static_cast<intptr_t>(Globals::status->getStanding()));
        int rawBonus =
                (int) (reward * standing->getMissionBonus(agent->getRace()));
        bonus = rawBonus + rawBonus % 50;
        if (bonus % 50 != 0) {
            bonus = rawBonus - rawBonus % 50;
        }
    }

    float remainder = (float) ((int) reward % 50);
    float roundedReward = reward + remainder;
    if ((int) roundedReward % 50 != 0) {
        roundedReward = reward - remainder;
    }

    Mission *mission = new Mission(type, AbyssEngine::String(agentName, false),
        agent->getImageParts(), race, (int) roundedReward, targetStation, difficulty);

    int costs = (int) (roundedReward / 10.0f +
        (float) static_cast<AERandom *>(Globals::rnd)->nextInt((int) roundedReward / 10));
    if (type == 8) {
        costs = (int) ((float) costs * 0.5f);
    } else if (type == 6) {
        mission->setTargetName(static_cast<Globals *>(Globals::globals)->getRandomName(0, true));
    }
    int costRem = costs % 50;
    int roundedCosts = costs + costRem;
    if (roundedCosts % 50 != 0) {
        roundedCosts = costs - costRem;
    }
    mission->setCosts(roundedCosts);
    mission->setProductionGoods(itemId, amount);
    mission->setBonus(bonus);

    for (uint32_t i = 0; i < systems->size(); ++i) {
        if (systems->data()[i]->stationIsInSystem(targetStation)) {
            AbyssEngine::String systemName = systems->data()[i]->getName();
            mission->setTargetSystemName(systemName);
            break;
        }
    }

    return mission;
}


Agent *Generator::createAgent(Station *station) {

    SolarSystem *system = Globals::status->getSystem();
    int race = system->getRace();
    if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) < 20) {
        race = static_cast<AERandom *>(Globals::rnd)->nextInt(8);
    }

    int offer = -1;
    bool valid = false;
    do {
        offer = static_cast<AERandom *>(Globals::rnd)->nextInt(7);
        if (race == 1) {
            valid = false;
        }
        if (race != 1 || offer != 6) {
            valid = true;
            if ((uint32_t)(offer - 3) < 8) {
                valid = ((0x1cU >> (uint8_t)(offer - 3)) & 1) != 0;
            }
        }
    } while (!valid);

    if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) < 0x21) {
        offer = 0;
    } else if ((uint32_t)(offer - 5) < 2 &&
               Globals::status->getCurrentCampaignMission() < 0x10) {
        offer = 0;
    }

    int male = 1;
    if (race == 0 && offer != 6) {
        male = 0;
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) < 0x3c) {
            male = 1;
        }
    }

    Agent *agent = new Agent(-1,
        static_cast<Globals *>(Globals::globals)->getRandomName(race, male != 0), station->getIndex(), Globals::status->getSystem()->getIndex(),
                             race, male, -1, -1, -1, -1);
    agent->setOffer(offer);
    agent->setImageParts(static_cast<ImageFactory *>(Globals::imageFactory)->createChar(male, race));

    if (agent->getOffer() == 6) {
        uint32_t count = static_cast<AERandom *>(Globals::rnd)->nextInt(3);
        Array<AbyssEngine::String *> *friendNames =
                new Array<AbyssEngine::String *>();
        ArraySetLength(count, *friendNames);
        for (int i = 0; i < (int) count; ++i) {
            AbyssEngine::String *friendName = new AbyssEngine::String(
                static_cast<Globals *>(Globals::globals)->getRandomName(agent->getRace(), true));
            friendNames->data()[i] = friendName;
        }
        agent->setWingmanFriendNames(friendNames);
        int costRoll = static_cast<AERandom *>(Globals::rnd)->nextInt(1300);
        agent->setCosts((costRoll + 700) * (count + 1));
        if (Globals::status->hardCoreMode()) {
            agent->setCosts(agent->getCosts() * 7);
        }
    } else if (agent->getOffer() == 2) {
        Array<Item *> *items = static_cast<Array<Item *> *>(Globals::items);
        int selected;
        do {
            do {
                selected = static_cast<AERandom *>(Globals::rnd)->nextInt(items->size());
            } while ((uint32_t)(selected - 0xd9) < 2);
        } while (selected == 0x83 || selected == 0xa4 || selected == 0xaf ||
                 items->data()[selected]->getIngredients() != nullptr ||
                 items->data()[selected]->getSinglePrice() == 0 ||
                 items->data()[selected]->getOccurence() == 0);

        Item *item = items->data()[selected];
        int amount = static_cast<AERandom *>(Globals::rnd)->nextInt(15);
        if (item->getType() == 3 || item->getType() == 0 || item->getType() == 2) {
            amount = 1;
        } else {
            amount += 5;
        }
        float factor = (float) (static_cast<AERandom *>(Globals::rnd)->nextInt(120) + 0x28) / 100.0f;
        int price = static_cast<Array<Item *> *>(Globals::items)->data()[selected]->getSinglePrice();
        agent->setSellItemData(selected, amount,
                               amount * (int) (factor * (float) price));
    }

    return agent;
}

// Android 2.0.16 literal table at 0x1fb5dc.
static const volatile int g_Generator_shipRaces[64] = {
    3, 0, 8, 3, 2, 0, 3, 0, 9, 1, 0, 8,
    2, 0, 0, 0, 2, 0, 2, 3, 3, 2, 0, 8,
    8, 8, 0, 0, 0, 8, 3, 2, 8, 0, 0, 2,
    0, 0, 0, 1, 0, 1, 1, 2, 1, 3, 3, 3,
    3, 1, 1, 0, 8, 1, 1, 0, 3, 2, 0, 0,
    8, 1, 3, 1
};

Array<Ship *> *Generator::getShipBuyList(Station *station) {
    if ((station->getSystem() == 0xf &&
         Globals::status->getCurrentCampaignMission() < 0x10) ||
        station->getIndex() == 0x65 || station->getIndex() == 0x6c ||
        Globals::status->inSupernovaSystem() != 0) {
        return nullptr;
    }

    if (station->getIndex() == 100 && Globals::status->dlc1Won()) {
        Array<Ship *> *result = new Array<Ship *>();
        for (int i = 0; i != 0x40; ++i) {
            Ship *base = static_cast<Array<Ship *> *>(Globals::ships)->data()[i];
            if (base->hasJumpDriveIntegrated() && base->getIndex() != 0x25) {
                Ship *ship = base->makeShip(-1);
                ArrayAdd(ship, *result);
                Ship *added = result->data()[result->size() - 1];
                added->setRace(g_Generator_shipRaces[i]);
                result->data()[result->size() - 1]->adjustPrice();
            }
        }
        return result;
    }

    if (station->getIndex() == 0x6b) {
        Array<Ship *> *result = new Array<Ship *>();
        for (int i = 0; i != 0x32; ++i) {
            if (g_Generator_shipRaces[i] == 8) {
                Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[i]->makeShip(-1);
                ArrayAdd(ship, *result);
                Ship *added = result->data()[result->size() - 1];
                added->setRace(8);
                result->data()[result->size() - 1]->adjustPrice();
            }
        }
        if (Globals::options[0x37] != 0) {
            Array<Wanted *> *wanted = Globals::status->wanted;
            if (wanted != nullptr && wanted->size() != 0) {
                if (Globals::status->wanted->data()[6]->isTerminated()) {
                    Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[45]->makeShip(-1);
                    ArrayAdd(ship, *result);
                    result->data()[result->size() - 1]->setRace(8);
                    result->data()[result->size() - 1]->adjustPrice();
                }
                if (Globals::status->wanted->data()[12]->isTerminated()) {
                    Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[46]->makeShip(-1);
                    ArrayAdd(ship, *result);
                    result->data()[result->size() - 1]->setRace(8);
                    result->data()[result->size() - 1]->adjustPrice();
                }
                if (Globals::status->wanted->data()[18]->isTerminated()) {
                    Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[47]->makeShip(-1);
                    ArrayAdd(ship, *result);
                    result->data()[result->size() - 1]->setRace(8);
                    result->data()[result->size() - 1]->adjustPrice();
                }
                if (Globals::status->wanted->data()[24]->isTerminated()) {
                    Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[48]->makeShip(-1);
                    ArrayAdd(ship, *result);
                    result->data()[result->size() - 1]->setRace(8);
                    result->data()[result->size() - 1]->adjustPrice();
                }
            }
        }
        return result;
    }

    SolarSystem *system = Globals::status->getSystem();
    int race = system->getRace();
    bool gold = station->getIndex() == 10 &&
                static_cast<Achievements *>(Globals::achievements)->gotAllGoldMedals();
    int stationIndex = station->getIndex();
    int stationIndexAgain = station->getIndex();
    int count;
    if (gold) {
        count = 1;
    } else {
        count = static_cast<AERandom *>(Globals::rnd)->nextInt(6);
        if (stationIndex == 0x29) {
            ++count;
        }
        if (count == 0) {
            return nullptr;
        }
    }

    Array<Ship *> *result = new Array<Ship *>();
    ArraySetLength(count, *result);

    int forcedShip = gold ? 8 : 10;
    for (int i = 0; i < count; ++i) {
        unsigned first = i == 0;
        unsigned forced = gold | (first & (stationIndex == 0x29));
        int forcedIndex = forced ? forcedShip : 0;
        unsigned fixed = forced | ((stationIndexAgain == 0x4e) & first);
        int shipIndex = forcedIndex;
        bool unique = false;
        while (!unique) {
            shipIndex = forcedIndex;
            if (!fixed) {
                shipIndex =
                        static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);
            }
            if (count > 1 &&
                static_cast<AERandom *>(Globals::rnd)->nextInt(100) < 0x16) {
                int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
                shipIndex = forcedIndex;
                if (!fixed) {
                    int enemyRace = roll;
                    if (enemyRace == race || enemyRace == 4) {
                        enemyRace = 8;
                    }
                    shipIndex = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(enemyRace);
                }
            }
            unique = true;
            for (int j = 0; j < count; ++j) {
                if (result->data()[j] != 0 &&
                    result->data()[j]->getIndex() == shipIndex) {
                    unique = false;
                    break;
                }
            }
        }
        result->data()[i] = static_cast<Array<Ship *> *>(Globals::ships)->data()[shipIndex]->makeShip(-1);
        result->data()[i]->setRace(g_Generator_shipRaces[shipIndex]);
        result->data()[i]->adjustPrice();
    }

    bool terranBonus = false;
    int raceFlag = 0;
    if (race == 0) {
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(7) == 0) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x3e]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(3);
            result->data()[result->size() - 1]->adjustPrice();
        }
        terranBonus = true;
    } else if (race == 1) {
        raceFlag = 1;
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(5) == 0) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x3f]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            result->data()[result->size() - 1]->adjustPrice();
        }
    } else if (race == 2 &&
               static_cast<AERandom *>(Globals::rnd)->nextInt(8) == 0) {
        Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x3d]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(1);
        result->data()[result->size() - 1]->adjustPrice();
    }

    if (Globals::options[0x35] && ((Globals::status->dlc1Won() & raceFlag) == 1)) {
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x27]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            result->data()[result->size() - 1]->adjustPrice();
        }
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x29]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            result->data()[result->size() - 1]->adjustPrice();
        }
    }

    if (Globals::options[0x37] != 0) {
        if (raceFlag &&
            static_cast<AERandom *>(Globals::rnd)->nextInt(4) == 0) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x36]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            result->data()[result->size() - 1]->adjustPrice();
        }
        if (station->getIndex() == 0x78 &&
            Globals::status->getCurrentCampaignMission() > 0x9e &&
            (Globals::status->hardCoreMode() ||
             (!Globals::status->hardCoreMode() &&
              static_cast<Achievements *>(Globals::achievements)->gotAllSupernovaMedals()))) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x2c]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            result->data()[result->size() - 1]->adjustPrice();
        }
        if (station->getIndex() == 0x78 &&
            Globals::status->getCurrentCampaignMission() > 0x9e) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x31]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            result->data()[result->size() - 1]->adjustPrice();
        }
        if (terranBonus &&
            static_cast<AERandom *>(Globals::rnd)->nextInt(8) == 0) {
            Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x33]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(0);
            result->data()[result->size() - 1]->adjustPrice();
        }
    }

    if (station->getSystem() == 0x11 &&
        static_cast<AERandom *>(Globals::rnd)->nextInt(3) == 0) {
        Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x2a]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(1);
        result->data()[result->size() - 1]->adjustPrice();
    }
    if (station->getSystem() == 0x11 &&
        static_cast<AERandom *>(Globals::rnd)->nextInt(3) == 0) {
        Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x2b]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(2);
        result->data()[result->size() - 1]->adjustPrice();
    }
    if (station->getSystem() == 0x11 &&
        static_cast<AERandom *>(Globals::rnd)->nextInt(3) == 0) {
        Ship *ship = static_cast<Array<Ship *> *>(Globals::ships)->data()[0x34]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(0);
        result->data()[result->size() - 1]->adjustPrice();
    }

    return result;
}


// Android 2.0.16 literal table at 0x1fb5b4.
static const volatile int g_Generator_kaamoAllowed[10] = {
    101, 102, 103, 107, 108, 109, 114, 124, 0, 0
};


Array<Item *> *Generator::getItemBuyList(Station *station) {
    int stationIndex = station->getIndex();

    if (stationIndex == 0x4e &&
        Globals::status->getCurrentCampaignMission() < 7) {
        Array<Item *> *items = new Array<Item *>();
        ArraySetLength(3, *items);
        items->data()[0] = static_cast<Array<Item *> *>(Globals::items)->data()[0]->makeItem(1, 0);
        items->data()[1] = static_cast<Array<Item *> *>(Globals::items)->data()[0x16]->makeItem(1, 0);
        items->data()[2] = static_cast<Array<Item *> *>(Globals::items)->data()[0x37]->makeItem(1, 0);
        return items;
    }

    if (station->getIndex() == 0x6c ||
        Globals::status->inSupernovaSystem() != 0) {
        return nullptr;
    }

    uint32_t stationId = station->getIndex();
    Array<Item *> *result = new Array<Item *>();

    Array<Item *> *allItems = static_cast<Array<Item *> *>(Globals::items);
    Galaxy *galaxy = static_cast<Galaxy *>(Globals::galaxy);
    Array<SolarSystem *> *systems = galaxy->getSystems();
    int stationTec = station->getTecLevel();
    int minTec = stationTec >> 1; // Android 0x95506: arithmetic half-level shift.
    if (stationTec < 4) {
        minTec = 1;
    }
    if ((stationId | 2) == 0x6b) {
        minTec = 0;
    }

    if (Globals::status->getCurrentCampaignMission() == 0x8b &&
        station->getSystem() == 0x19) {
        ArrayAdd(allItems->data()[0xbe]->makeItem(), *result);
    }

    if (stationId == 0x7e) {
        Item *item = allItems->data()[0xd1]->makeItem();
        int amount = 1;
        if (Globals::status->getCurrentCampaignMission() != 0x75) {
            amount = static_cast<AERandom *>(Globals::rnd)->nextInt(10) + 1;
        }
        ArrayAdd(item->makeItem(amount), *result);
    }

    static_cast<AERandom *>(Globals::rnd)->reset();
    float campaignFactor = 1.5f;
    if ((float) (Globals::status->getCurrentCampaignMission() + 0x19) / 45.0f < 1.5f) {
        campaignFactor = (float) (Globals::status->getCurrentCampaignMission() + 0x19) / 45.0f;
    }

    for (uint32_t i = 0; i < allItems->size(); ++i) {
        Item *item = allItems->data()[i];
        bool systemGood = item->getIndex() >= 0x84 && item->getIndex() < 0x9a;
        bool forcedStation = false;

        if (station->getIndex() == item->getAttribute(0x3d)) {
            if ((item->getIndex() < 0xc4 || item->getIndex() > 0xc4 ||
                 Globals::status->getCurrentCampaignMission() > 0x8d) &&
                (item->getIndex() < 0xc6 || item->getIndex() > 0xc8 ||
                 Globals::status->getCurrentCampaignMission() > 0x8d)) {
                forcedStation = true;
            }
        }

        if (stationId == 0x6a) {
            bool allowed = false;
            for (uint32_t j = 0; j < 10; ++j) {
                if ((g_Generator_kaamoAllowed[j] == item->getIndex()) | systemGood) {
                    allowed = true;
                    break;
                }
            }
            if (!allowed) {
                continue;
            }
        }

        int occurrence = item->getOccurence();
        if (item->getIndex() == 0x7a && Globals::energyCellsProbChange != 0) {
            occurrence = (int) ((float) occurrence +
                                (float) (occurrence *
                                         Globals::energyCellsProbChange) *
                                0.01f);
        }
        if (item->getType() == 1 && Globals::secondaryWeaponsProbChange != 0) {
            occurrence =
                    (int) ((float) occurrence +
                           (float) (occurrence * Globals::secondaryWeaponsProbChange) * 0.01f);
        }

        int itemIndex = item->getIndex();
        int sort = item->getSort();
        int mission = Globals::status->getCurrentCampaignMission();
        if (occurrence == 0 &&
            ((itemIndex > 0xc3 && Globals::options[0x37] != 0) ||
             (itemIndex < 0xc4 && Globals::options[0x35] != 0))) {
            occurrence = 0;
            int type = item->getType();
            if (itemIndex != 0x55 && type != 4 &&
                item->getIngredients() == 0 &&
                (itemIndex != 0xb5 || mission > 0x3a) &&
                (((sort != 0x22 && (sort | 2) != 0x23) || mission > 0x8d) &&
                 sort != 0x24 && (mission > 0x8d || sort != 0x2b)) &&
                ((1 < (uint32_t)((itemIndex & 0xfffffff7) - 0xd1)) &&
                 (mission > 0x5d || itemIndex != 0xcd))) {
                bool blocked = false;
                for (uint32_t j = 0; j < 9; ++j) {
                    if (kaamoSpecialItems[j] == itemIndex) {
                        blocked = true;
                        break;
                    }
                }
                if (!blocked &&
                    (sort != 0x1d || Globals::status->inBlackMarketSystem() != 0)) {
                    int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(30);
                    occurrence =
                            (int) ((float) roll +
                                   ((float) item->getTecLevel() / -10.0f + 1.0f) *
                                   30.0f);
                }
            }
        }

        int itemTec = item->getTecLevel();
        bool accept = forcedStation;
        if (!accept) {
            if (item->getIngredients() == 0 &&
                (uint32_t)((i & 0x7fffffff) - 0xd9) > 1 && i != 0xa4 &&
                i != 0xaf &&
                ((itemTec <= station->getTecLevel()) & (occurrence != 0)) &&
                item->getSinglePrice() != 0) {
                if (item->getAttribute(0x3c) == 1 &&
                    systems->data()[station->getSystem()]->getRace() !=
                    1) {
                    continue;
                }
                if (stationId != 0x6a && systemGood &&
                    item->getIndex() != station->getSystem() + 0x84) {
                    continue;
                }
                accept = true;
            }
        }

        if (!accept) {
            continue;
        }
        if (Globals::status->hardCoreMode() &&
            (item->getSort() == 0x17 || item->getSort() == 0x18)) {
            continue;
        }
        if (stationId == 0x6b && item->getType() != 3) {
            continue;
        }
        if (stationId == 0x69 &&
            (!item->isWeapon() && item->getSort() != 0x1c)) {
            continue;
        }
        if (stationId == 0x65 && !item->isWeapon()) {
            continue;
        }
        if (stationId == 0x6a && item->getType() != 4) {
            continue;
        }

        int randomGate = (int) (campaignFactor * (float) occurrence);
        if (!forcedStation) {
            if ((itemTec > stationTec && !systemGood) ||
                static_cast<AERandom *>(Globals::rnd)->nextInt(100) >= randomGate) {
                continue;
            }
            int index = item->getIndex();
            if (!(itemTec >= minTec || index == 0x7a ||
                  static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 60)) {
                continue;
            }
        }

        int minSystem = item->getMinPriceSystem();
        int minX = systems->data()[minSystem]->getX();
        int minY = systems->data()[item->getMinPriceSystem()]->getY();
        int stationSystem = station->getSystem();
        int stationX = systems->data()[stationSystem]->getX();
        int stationY = systems->data()[station->getSystem()]->getY();
        int distance =
                galaxy->invDistancePercent(stationX, stationY, minX, minY);
        int amountRoll = static_cast<AERandom *>(Globals::rnd)->nextInt(15);
        int amount = amountRoll + 5;

        if (item->getType() == 4 || item->getType() == 1) {
            if (item->getIndex() == 0x6d) {
                amount /= 2;
                if (amountRoll < -3) {
                    amount = 1;
                }
            } else if (item->getType() == 4) {
                if (distance > 0x32) {
                    float hard = Globals::status->hardCoreMode() ? 2.0f : 20.0f;
                    int scaled = (int) (((float) (distance - 0x32) / 50.0f) *
                                        hard);
                    if (scaled < 2) {
                        scaled = 1;
                    }
                    amount *= scaled;
                }
                if (item->getIndex() == 0x6e &&
                    static_cast<AERandom *>(Globals::rnd)->nextInt(10) + 10 < amount) {
                    amount = static_cast<AERandom *>(Globals::rnd)->nextInt(10) + 10;
                }
            }
        } else {
            amount /= 5;
            if (amountRoll < 0) {
                amount = 1;
            }
        }

        Item *newItem = item->makeItem(amount);
        ArrayAdd(newItem, *result);
    }

    return result;
}

// Android 2.0.16 literal table at 0x1fb6dc.
static const volatile int g_Generator_typeChances[5] = {
    10, 40, 2, 10, 100
};

Array<int> *Generator::getLootList(int itemIndex, int amount) {
    if (itemIndex >= 0) {
        Array<int> *result = new Array<int>();
        ArrayAdd(itemIndex, *result);
        ArrayAdd(amount, *result);
        return result;
    }

    Array<Item *> *itemsForOccurence = static_cast<Array<Item *> *>(Globals::items);

    int pairCount = static_cast<AERandom *>(Globals::rnd)->nextInt(3);
    if (static_cast<AERandom *>(Globals::rnd)->nextInt(3) == 0) {
        return nullptr;
    }

    Array<int> *result = new Array<int>();
    uint32_t length = (uint32_t)(pairCount << 1);
    if (pairCount == 0) {
        length = 2;
    }
    ArraySetLength(length, *result);

    Array<Item *> *items = static_cast<Array<Item *> *>(Globals::items);
    int type = 0;
    for (uint32_t out = 0; out < result->size(); out += 2) {
        uint32_t found = 0;
        int selected = 0;

        for (uint32_t tries = 0;; ++tries) {
            if ((found << 31) != 0 || tries >= 100) {
                break;
            }
            selected = static_cast<AERandom *>(Globals::rnd)->nextInt(itemsForOccurence->size());
            type = items->data()[selected]->getType();
            if (items->data()[selected]->getIngredients() != nullptr ||
                static_cast<AERandom *>(Globals::rnd)->nextInt(100) >= g_Generator_typeChances[type]) {
                found = 0;
                continue;
            }
            int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(100);
            if (roll >= itemsForOccurence->data()[selected]->getOccurence() ||
                items->data()[selected]->getSinglePrice() < 1) {
                found = 0;
                continue;
            }
            found = 0;
            if ((uint32_t)(selected - 0xd9) < 2 || selected == 0xa4 || selected == 0xaf) {
                continue;
            }
            if (type != 4 && items->data()[selected]->getTecLevel() > 7) {
                continue;
            }
            found = 1;
        }

        if (found & 1) {
            result->data()[out] = selected;
        } else {
            result->data()[out] = static_cast<AERandom *>(Globals::rnd)->nextInt(10) + 0x9a;
            type = 4;
        }
        int count;
        if (type != 4) {
            count = static_cast<AERandom *>(Globals::rnd)->nextInt(3);
        } else {
            count = static_cast<AERandom *>(Globals::rnd)->nextInt(9);
            type = 4;
        }
        result->data()[out | 1] = count + 1;
    }

    Ship *ship = Globals::status->getShip();
    if (ship->hasJumpDrive() != 0) {
        ship = Globals::status->getShip();
        if (ship->hasCargo(0x7a, 1) == 0 &&
            static_cast<AERandom *>(Globals::rnd)->nextInt(100) < 10) {
            result->data()[0] = 0x7a;
        }
    }
    return result;
}

bool Generator::isKaamoSpecialItem(int item) {
    for (uint32_t i = 0;;) {
        if (i > 8) {
            return false;
        }
        int current = kaamoSpecialItems[i];
        ++i;
        if (current == item) {
            return true;
        }
    }
}
