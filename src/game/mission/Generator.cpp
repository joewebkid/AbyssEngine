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
            AbyssEngine::AERandom *random = AERandom::gRandom;
            for (uint32_t i = 0; i < items->size(); ++i) {
                Item *item = items->data()[i];
                int take = random->nextInt();
                if (take < item->getAmount()) {
                    item->changeAmount(-take);
                }
            }
        }
    }
}

static int volatile
g_Generator_stationBlockList[0x34];

static int status_system_index(Status *status) {
    SolarSystem *system = status != nullptr ? status->getSystem() : nullptr;
    return system != nullptr ? system->getIndex() : 0;
}

int Generator::generateStationIndex(Array<SolarSystem *> *systems, int station) {
    AbyssEngine::AERandom *random = AERandom::gRandom;
    Status *status = Status::gStatus;
    bool accepted = false;
    int selected = 0;

    do {
        if (accepted) {
            return selected;
        }

        int roll = random->nextInt();
        selected = station;
        if (roll >= 20) {
            roll = random->nextInt();
            if (roll < 40) {
                int currentIndex = status_system_index(status);
                Array<int> *stations =
                        (Array<int> *) systems->data()[currentIndex]->getStations();
                int pick = random->nextInt();
                selected = stations->data()[pick];
            } else {
                selected = random->nextInt();
            }
        }

        if (status_system_index(status) == 0xf) {
            Array<int> *stations =
                    (Array<int> *) systems->data()[status_system_index(status)]->getStations();
            int pick = random->nextInt();
            selected = stations->data()[pick];
        }

        uint32_t blocked = 0;
        while (blocked <= 0x33) {
            if (selected == g_Generator_stationBlockList[blocked]) {
                break;
            }
            ++blocked;
        }
        accepted = blocked > 0x33;

        uint32_t systemIndex = 0;
        while (systemIndex < systems->size()) {
            if (systems->data()[systemIndex]->stationIsInSystem(selected) != 0) {
                break;
            }
            ++systemIndex;
        }
        if (systemIndex >= systems->size()) {
            systemIndex = 0;
        }

        if ((uint32_t)(selected - 0x6d) < 5) {
            accepted = false;
        }

        Array<bool> *visibilities = status->getSystemVisibilities();
        if (visibilities != nullptr && systemIndex < visibilities->size()) {
            accepted = (*visibilities)[systemIndex];
        }

        if (systems->data()[systemIndex]->getRoutes() == nullptr) {
            if (systemIndex != (uint32_t) status_system_index(status)) {
                accepted = false;
            }
        }
    } while (true);
}

static Array<Item *> **volatile
g_Generator_agentsItems;
static int volatile
g_Generator_offerItemIds[12];
static int volatile
g_Generator_offerShipIds[6];
static Array<Ship *> **volatile
g_Generator_agentsShips;
static int *volatile
g_Generator_storyNames;
static ImageFactory **volatile
g_Generator_storyImages;
static int volatile
g_Generator_enemyRaces[4];
static int *volatile
g_Generator_enemyNames;
static ImageFactory **volatile
g_Generator_enemyImages;

Array<Agent *> *Generator::createAgents(Station *station) {
    Status *status = Status::gStatus;
    AbyssEngine::AERandom *random = AERandom::gRandom;

    Array<Agent *> *result = nullptr;
    if (status->inSupernovaSystem() == 0) {
        Array<Agent *> *existing = (Array<Agent *> *) (long) status->getAgents();
        int mission = status->getCurrentCampaignMission();
        bool keepExisting = mission > 0x10;
        if (!status->dlc1Won()) {
            keepExisting = mission > 0x10 && station->getIndex() != 0x6a;
        }

        uint32_t count = 0;
        for (uint32_t i = 0; i < existing->size(); ++i) {
            if (existing->data()[i]->getStation() == station->getIndex() &&
                keepExisting) {
                ++count;
            }
        }

        if (station->getIndex() != 0x6c) {
            int roll = random->nextInt();
            if ((int) (roll + count + 3) < 5) {
                count += 3 + random->nextInt();
            } else {
                count = 5;
            }
        }

        result = new Array<Agent *>();
        ArraySetLength(count, *result);

        uint32_t out = 0;
        for (uint32_t i = 0; i < existing->size(); ++i) {
            Agent *agent = existing->data()[i];
            if (agent->getStation() == station->getIndex() && keepExisting) {
                result->data()[out++] = agent;
                int offer = agent->getOffer();
                if (offer == 9) {
                    int index;
                    if (status->getCurrentCampaignMission() < 0x8e) {
                        index = random->nextInt() + 2;
                    } else {
                        index = random->nextInt();
                    }
                    int itemId = g_Generator_offerItemIds[index];
                    Item *item = (*g_Generator_agentsItems)->data()[itemId];
                    int amount = ((uint32_t)(index - 3) < 2) ? 10 : 1;
                    agent->setSellItemData(itemId, amount,
                                           item->getSinglePrice() * amount);
                    agent->setOfferAccepted(false);
                } else if (offer == 10) {
                    Array<int> *choices = new Array<int>();
                    for (int j = 0; j != 6; ++j) {
                        int shipId = g_Generator_offerShipIds[j];
                        if (((Station *) (intptr_t) status->field_14c)
                            ->hasShip(shipId) == 0) {
                            if (status->getShip()->getIndex() != shipId) {
                                ArrayAdd(shipId, *choices);
                            }
                        }
                    }
                    if (choices->size() != 0) {
                        int pick = random->nextInt();
                        int shipId = choices->data()[pick];
                        agent->setSellItemData(
                            shipId, 1,
                            (*g_Generator_agentsShips)->data()[shipId]->getPrice());
                        agent->setOfferAccepted(false);
                    } else {
                        agent->setOfferAccepted(true);
                    }
                    delete choices;
                }
            }
        }

        Array<SolarSystem *> *systems =
                Galaxy::gGalaxy->getSystems();
        if (status->getCurrentCampaignMission() == 0x17 &&
            station->getIndex() == 10) {
            AbyssEngine::String name = Globals::gGlobals->getRandomName(0, true);
            Agent *agent = new Agent(-1, name, station->getIndex(),
                                     status_system_index(status), 0, 1, -1, -1, -1, -1);
            agent->setOffer(2);
            agent->setSellItemData(0x44, 1, 0);
            agent->setImageParts((*g_Generator_storyImages)->createChar(1, 0));
            result->data()[out++] = agent;
        }

        bool hasWingman = false;
        for (uint32_t i = out; i < result->size(); ++i) {
            result->data()[i] = createAgent(station);
            Agent *agent = result->data()[i];
            if (agent->getOffer() == 6) {
                if (hasWingman) {
                    agent->setOffer(1);
                } else {
                    hasWingman = true;
                }
            } else if (agent->getOffer() == 0) {
                agent->setMission(createMission(agent, systems));
            }
        }

        if (random->nextInt() < 0x23) {
            Standing *standing = (Standing *) (intptr_t) status->getStanding();
            for (uint32_t raceIndex = 0; raceIndex < 4; ++raceIndex) {
                int race = g_Generator_enemyRaces[raceIndex];
                if (standing->isEnemy(race)) {
                    for (uint32_t i = 0; i < result->size(); ++i) {
                        Agent *agent = result->data()[i];
                        if (agent->isGenericAgent() && agent->getOffer() != 7) {
                            result->data()[i] = nullptr;
                            AbyssEngine::String name =
                                    Globals::gGlobals->getRandomName(race, true);
                            agent = new Agent(-1, name, station->getIndex(),
                                              status_system_index(status), race, 1, -1,
                                              -1, -1, -1);
                            result->data()[i] = agent;
                            agent->setOffer(7);
                            agent->setImageParts(
                                (*g_Generator_enemyImages)->createChar(1, race));
                            break;
                        }
                    }
                }
            }
        }

        if ((status->getCurrentCampaignMission() == 0x17 &&
             station->getIndex() == 10) ||
            random->nextInt() == 1) {
            for (; out < result->size(); ++out) {
                Agent *agent = result->data()[out];
                if (!agent->isStoryAgent() && agent->getOffer() == 0 &&
                    agent->getMission() != nullptr) {
                    Mission *m = agent->getMission();
                    int reward = m->getReward();
                    if (reward < 50000) {
                        int newReward = 50000;
                        if (reward * 10 < 50000) {
                            newReward = reward * 10;
                        }
                        m->setReward(newReward);
                        break;
                    }
                }
            }
        }
    }
    return result;
}

static uint8_t *volatile
g_Generator_missionFlags;
static Array<Item *> **volatile
g_Generator_missionItems;
static int *volatile
g_Generator_targetNames;

Mission *Generator::createMission(Agent *agent,
                                  Array<SolarSystem *> *systems) {
    Status *status = Status::gStatus;
    AbyssEngine::AERandom *random = AERandom::gRandom;

    int agentStation = agent->getStation();
    int targetStation = generateStationIndex(systems, agentStation);
    if (status_system_index(status) == 0xf) {
        Array<int> *stations =
                (Array<int> *) systems->data()[status_system_index(status)]->getStations();
        targetStation = stations->data()[0] + random->nextInt();
    }

    uint32_t race = agent->getRace();
    uint32_t type = 0;
    bool selected = false;
    int guard = 999;
    Array<uint8_t> *used = (Array<uint8_t> *) status->field_50;
    while (!selected) {
        selected = guard < 1;
        uint32_t pick = random->nextInt();
        --guard;
        type = pick;
        if (pick == 8 || (pick == 10 && race >= 4)) {
            continue;
        }
        if (used != nullptr) {
            uint8_t *data = used->data();
            if (data[pick] != 0) {
                uint32_t sum = 0;
                for (uint32_t i = 0; i < used->size(); ++i) {
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
            if (status_system_index(status) != 0x19) {
                type = pick;
            } else {
                type = 0xc;
            }
        } else {
            selected = true;
        }
    }

    if (status->getCurrentCampaignMission() < 0x10 ||
        (type == 0xf && g_Generator_missionFlags[0x37] == 0)) {
        switch (random->nextInt()) {
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
        if (type < 0xf && ((1U << (type & 0xff)) & 0x4801U) != 0) {
            while (targetStation == status->getStation()->getIndex()) {
                targetStation = generateStationIndex(systems, agent->getStation());
            }
        }
        if (race < 4 && type == 0xd) {
            SolarSystem *agentSystem = systems->data()[agent->getSystem()];
            if (agentSystem->getRoutes() == nullptr) {
                type = random->nextInt() == 0 ? 1 : 4;
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
    int difficulty = random->nextInt() + 1;

    int itemId = 0;
    int amount = 0;
    if (type == 8) {
        Array<Item *> *items = *g_Generator_missionItems;
        do {
            itemId = random->nextInt() + 0x61;
        } while (items->data()[itemId]->getOccurence() == 0 ||
                 items->data()[itemId]->getSinglePrice() == 0 ||
                 items->data()[itemId]->getIngredients() != nullptr ||
                 ((itemId - 0x61) & 0xfffffffe) == 0x78 ||
                 itemId == 0x75 || ((itemId - 0x61) & 0xfffffffe) == 0x12 ||
                 itemId == 0x83 || itemId == 0xa4 || itemId == 0xaf);
        amount = random->nextInt() + 5;
    } else {
        switch (type) {
            case 0:
                itemId = random->nextInt();
                amount = (int) (((float) difficulty / 10.0f) * 35.0f) + 5;
                break;
            case 2:
                amount = (int) ((float) random->nextInt()) + 2;
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
                amount = random->nextInt() + 30;
                for (uint32_t i = 0; i < systems->size(); ++i) {
                    if (systems->data()[i]->stationIsInSystem(targetStation)) {
                        int *prob =
                                (int *) Galaxy::gGalaxy
                                ->getAsteroidProbabilities(status->getStation());
                        itemId = prob[random->nextInt() * 2];
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
            systems->data()[status->getStation()->getSystem()];
    SolarSystem *to = systems->data()
    [((Station *) (intptr_t) Galaxy::gGalaxy
        ->getStation(targetStation))->getSystem()];
    int distance = (int) Galaxy::gGalaxy->distance(to, from);
    float reward = ((float) distance / 1000.0f + 1.0f) *
                   (float) ((int) (((float) difficulty / 10.0f) * 1400.0f) +
                            1500);
    if (type == 9) {
        reward *= 0.25f;
    } else if (type == 7) {
        reward *= 0.5f;
    } else if (type == 8) {
        reward = (float) (amount * (*g_Generator_missionItems)
                          ->data()[itemId]
                          ->getSinglePrice()) *
                 1.2f;
    } else if (type == 3 || type == 5) {
        reward += reward;
    } else if (type == 0xb) {
        reward = reward * 0.4f + ((reward * 0.4f) / 5.0f) * (float) amount;
    }

    int level = status->getLevel();
    reward += (float) (level * level * level * 10);

    int bonus = 0;
    if ((type | 4) != 0xc) {
        Standing *standing = (Standing *) (intptr_t) status->getStanding();
        int rawBonus =
                (int) (reward * standing->getMissionBonus(agent->getRace()));
        bonus = rawBonus + rawBonus % 50;
        if (bonus % 50 != 0) {
            bonus = rawBonus - rawBonus % 50;
        }
    }

    int rewardInt = (int) reward;
    int remainder = rewardInt % 50;
    rewardInt = ((rewardInt + remainder) % 50 == 0)
                    ? rewardInt + remainder
                    : rewardInt - remainder;

    Mission *mission = new Mission();
    AbyssEngine::String missionName;
    Mission_ctor_full(mission, type, &missionName, agent->getImageParts(), race,
                      rewardInt, targetStation, difficulty);

    int costs = rewardInt / 10 + random->nextInt();
    if (type == 8) {
        costs = (int) ((float) costs * 0.5f);
    } else if (type == 6) {
        AbyssEngine::String targetName = Globals::gGlobals->getRandomName(0, true);
        mission->setTargetName(targetName);
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

static int *volatile
g_Generator_nameSource;
static ImageFactory **volatile
g_Generator_imageFactory;
static Array<Item *> **volatile
g_Generator_agentItems;

Agent *Generator::createAgent(Station *station) {
    Status *status = Status::gStatus;
    AbyssEngine::AERandom *random = AERandom::gRandom;

    SolarSystem *system = status->getSystem();
    int race = system != nullptr ? system->getRace() : 0;
    if (random->nextInt() < 20) {
        race = random->nextInt();
    }

    int offer = -1;
    bool valid = false;
    do {
        offer = random->nextInt();
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

    if (random->nextInt() < 0x21) {
        offer = 0;
    } else if ((uint32_t)(offer - 5) < 2 &&
               status->getCurrentCampaignMission() < 0x10) {
        offer = 0;
    }

    int male = 1;
    if (race == 0 && offer != 6) {
        male = 0;
        if (random->nextInt() < 0x3c) {
            male = 1;
        }
    }

    AbyssEngine::String name = Globals::gGlobals->getRandomName(race, male != 0);
    Agent *agent = new Agent(-1, name, station->getIndex(), status_system_index(status),
                             race, male, -1, -1, -1, -1);
    agent->setOffer(offer);
    agent->setImageParts((*g_Generator_imageFactory)->createChar(male, race));

    if (agent->getOffer() == 6) {
        uint32_t count = random->nextInt();
        Array<AbyssEngine::String *> *friendNames =
                new Array<AbyssEngine::String *>();
        ArraySetLength(count, *friendNames);
        for (int i = 0; i < (int) count; ++i) {
            AbyssEngine::String *friendName = new AbyssEngine::String(
                Globals::gGlobals->getRandomName(agent->getRace(), true));
            friendNames->data()[i] = friendName;
        }
        agent->setWingmanFriendNames(friendNames);
        int costRoll = random->nextInt();
        agent->setCosts((costRoll + 700) * (count + 1));
        if (status->hardCoreMode()) {
            agent->setCosts(agent->getCosts() * 7);
        }
    } else if (agent->getOffer() == 2) {
        Array<Item *> *items = *g_Generator_agentItems;
        int selected;
        do {
            do {
                selected = random->nextInt();
            } while ((uint32_t)(selected - 0xd9) < 2);
        } while (selected == 0x83 || selected == 0xa4 || selected == 0xaf ||
                 items->data()[selected]->getIngredients() != nullptr ||
                 items->data()[selected]->getSinglePrice() == 0 ||
                 items->data()[selected]->getOccurence() == 0);

        Item *item = items->data()[selected];
        int amount = random->nextInt();
        int type = item->getType();
        if (type == 3 || type == 0 || type == 2) {
            amount = 1;
        } else {
            amount += 5;
        }
        float factor = (float) (random->nextInt() + 0x28) / 100.0f;
        int price = item->getSinglePrice();
        agent->setSellItemData(selected, amount,
                               amount * (int) (factor * (float) price));
    }

    return agent;
}

static Array<Ship *> **volatile
g_Generator_allShips;
static int volatile
g_Generator_shipRaces[0x40];
static uint8_t *volatile
g_Generator_shipFlags;
static void **volatile
g_Generator_wantedList;

Array<Ship *> *Generator::getShipBuyList(Station *station) {
    Status *status = Status::gStatus;
    if ((station->getSystem() == 0xf &&
         status->getCurrentCampaignMission() < 0x10) ||
        station->getIndex() == 0x65 || station->getIndex() == 0x6c ||
        status->inSupernovaSystem() != 0) {
        return nullptr;
    }

    Array<Ship *> *allShips = *g_Generator_allShips;

    if (station->getIndex() == 100 && status->dlc1Won()) {
        Array<Ship *> *result = new Array<Ship *>();
        for (int i = 0; i != 0x40; ++i) {
            Ship *base = allShips->data()[i];
            if (base->hasJumpDriveIntegrated() && base->getIndex() != 0x25) {
                Ship *ship = base->makeShip(-1);
                ArrayAdd(ship, *result);
                Ship *added = result->data()[result->size() - 1];
                added->setRace(g_Generator_shipRaces[i]);
                added->adjustPrice();
            }
        }
        return result;
    }

    if (station->getIndex() == 0x6b) {
        Array<Ship *> *result = new Array<Ship *>();
        for (int i = 0; i != 0x32; ++i) {
            if (g_Generator_shipRaces[i] == 8) {
                Ship *ship = allShips->data()[i]->makeShip(-1);
                ArrayAdd(ship, *result);
                Ship *added = result->data()[result->size() - 1];
                added->setRace(8);
                added->adjustPrice();
            }
        }
        if (g_Generator_shipFlags[0x37] != 0) {
            Array<void *> *wanted = *(Array<void *> **) *g_Generator_wantedList;
            if (wanted != nullptr && wanted->size() != 0) {
                const int offsets[4] = {0x18, 0x30, 0x48, 0x60};
                const int ships[4] = {0x2d, 0x2e, 0x2f, 0x30};
                for (int i = 0; i != 4; ++i) {
                    void *w = *(void **) ((char *) wanted->data() + offsets[i]);
                    if (((Wanted *) w)->isTerminated()) {
                        Ship *ship = allShips->data()[ships[i]]->makeShip(-1);
                        ArrayAdd(ship, *result);
                        Ship *added = result->data()[result->size() - 1];
                        added->setRace(8);
                        added->adjustPrice();
                    }
                }
            }
        }
        return result;
    }

    SolarSystem *system = status->getSystem();
    int race = system != nullptr ? system->getRace() : 0;
    bool gold = station->getIndex() == 10 &&
                Achievements::gAchievements->gotAllGoldMedals();
    int stationIndex = station->getIndex();
    int count;
    if (gold) {
        count = 1;
    } else {
        count = AERandom::gRandom->nextInt();
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
        bool first = i == 0;
        bool forced = gold || (first && stationIndex == 0x29);
        int shipIndex = forced ? forcedShip : 0;
        bool unique = false;
        while (!unique) {
            if (!forced && (stationIndex != 0x4e || !first)) {
                shipIndex =
                        Globals::gGlobals->getRandomEnemyFighter(race);
            }
            if (count > 1 &&
                AERandom::gRandom->nextInt() < 0x16) {
                int roll = AERandom::gRandom->nextInt();
                if (!forced && (stationIndex != 0x4e || !first)) {
                    int enemyRace = roll;
                    if (enemyRace == race || enemyRace == 4) {
                        enemyRace = 8;
                    }
                    shipIndex = Globals::gGlobals->getRandomEnemyFighter(enemyRace);
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
        result->data()[i] = allShips->data()[shipIndex]->makeShip(-1);
        result->data()[i]->setRace(g_Generator_shipRaces[shipIndex]);
        result->data()[i]->adjustPrice();
    }

    bool terranBonus = false;
    int raceFlag = 0;
    if (race == 0) {
        if (AERandom::gRandom->nextInt() == 0) {
            Ship *ship = allShips->data()[0x3e]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(3);
            added->adjustPrice();
        }
        terranBonus = true;
    } else if (race == 1) {
        raceFlag = 1;
        if (AERandom::gRandom->nextInt() == 0) {
            Ship *ship = allShips->data()[0x3f]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            added->adjustPrice();
        }
    } else if (race == 2 &&
               AERandom::gRandom->nextInt() == 0) {
        Ship *ship = allShips->data()[0x3d]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(1);
        added->adjustPrice();
    }

    if (g_Generator_shipFlags[0x35] && status->dlc1Won() && raceFlag) {
        if (AERandom::gRandom->nextInt() == 0) {
            Ship *ship = allShips->data()[0x27]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            added->adjustPrice();
        }
        if (AERandom::gRandom->nextInt() == 0) {
            Ship *ship = allShips->data()[0x29]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            added->adjustPrice();
        }
    }

    if (g_Generator_shipFlags[0x37] != 0) {
        if (raceFlag &&
            AERandom::gRandom->nextInt() == 0) {
            Ship *ship = allShips->data()[0x36]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            added->adjustPrice();
        }
        if (station->getIndex() == 0x78 &&
            status->getCurrentCampaignMission() > 0x9e &&
            (status->hardCoreMode() ||
             Achievements::gAchievements->gotAllSupernovaMedals())) {
            Ship *ship = allShips->data()[0x2c]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            added->adjustPrice();
        }
        if (station->getIndex() == 0x78 &&
            status->getCurrentCampaignMission() > 0x9e) {
            Ship *ship = allShips->data()[0x31]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(1);
            added->adjustPrice();
        }
        if (terranBonus &&
            AERandom::gRandom->nextInt() == 0) {
            Ship *ship = allShips->data()[0x33]->makeShip(-1);
            ArrayAdd(ship, *result);
            Ship *added = result->data()[result->size() - 1];
            added->setRace(0);
            added->adjustPrice();
        }
    }

    if (station->getSystem() == 0x11 &&
        AERandom::gRandom->nextInt() == 0) {
        Ship *ship = allShips->data()[0x2a]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(1);
        added->adjustPrice();
    }
    if (station->getSystem() == 0x11 &&
        AERandom::gRandom->nextInt() == 0) {
        Ship *ship = allShips->data()[0x2b]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(2);
        added->adjustPrice();
    }
    if (station->getSystem() == 0x11 &&
        AERandom::gRandom->nextInt() == 0) {
        Ship *ship = allShips->data()[0x34]->makeShip(-1);
        ArrayAdd(ship, *result);
        Ship *added = result->data()[result->size() - 1];
        added->setRace(0);
        added->adjustPrice();
    }

    return result;
}

typedef Item *(*ItemFactory)(Item *, int, int);

static Array<Item *> **volatile
g_Generator_allItems;
static ItemFactory volatile
g_Generator_introFactory;
static int *volatile
g_Generator_jumpDriveBoost;
static int *volatile
g_Generator_weaponBoost;
static uint8_t *volatile
g_Generator_unlockFlags;
static int volatile
g_Generator_kaamoAllowed[10];
static int volatile
g_Generator_blockedItems[9];

Array<Item *> *Generator::getItemBuyList(Station *station) {
    Status *status = Status::gStatus;
    int stationIndex = station->getIndex();

    if (stationIndex == 0x4e &&
        status->getCurrentCampaignMission() < 7) {
        Array<Item *> *items = new Array<Item *>();
        ArraySetLength(3, *items);
        Array<Item *> *all = *g_Generator_allItems;
        ItemFactory makeIntro = g_Generator_introFactory;
        items->data()[0] = makeIntro(all->data()[0], 1, 0);
        items->data()[1] = makeIntro(all->data()[0x16], 1, 0);
        items->data()[2] = makeIntro(all->data()[0x37], 1, 0);
        return items;
    }

    if (station->getIndex() == 0x6c ||
        status->inSupernovaSystem() != 0) {
        return nullptr;
    }

    uint32_t stationId = station->getIndex();
    Array<Item *> *result = new Array<Item *>();

    Array<Item *> *allItems = *g_Generator_allItems;
    Galaxy *galaxy = Galaxy::gGalaxy;
    Array<SolarSystem *> *systems = galaxy->getSystems();
    int stationTec = station->getTecLevel();
    int minTec = stationTec / 2;
    if (stationTec < 4) {
        minTec = 1;
    }
    if ((stationId | 2) == 0x6b) {
        minTec = 0;
    }

    if (status->getCurrentCampaignMission() == 0x8b &&
        station->getSystem() == 0x19) {
        ArrayAdd(allItems->data()[0xbe]->makeItem(), *result);
    }

    if (stationId == 0x7e) {
        Item *item = allItems->data()[0xd1]->makeItem();
        int amount = 1;
        if (status->getCurrentCampaignMission() != 0x75) {
            amount = AERandom::gRandom->nextInt() + 1;
        }
        item->setAmount(amount);
        ArrayAdd(item, *result);
    }

    AbyssEngine::AERandom *random = AERandom::gRandom;
    float campaignFactor =
            (float) (status->getCurrentCampaignMission() + 0x19) / 100.0f;
    if (campaignFactor > 1.5f) {
        campaignFactor = 1.5f;
    }

    for (uint32_t i = 0; i < allItems->size(); ++i) {
        Item *item = allItems->data()[i];
        int itemIndex = item->getIndex();
        bool systemGood = itemIndex >= 0x84 && itemIndex < 0x9a;
        bool forcedStation = false;

        if (station->getIndex() == item->getAttribute(0x3d)) {
            if ((item->getIndex() < 0xc4 || item->getIndex() > 0xc4 ||
                 status->getCurrentCampaignMission() > 0x8d) &&
                (item->getIndex() < 0xc6 || item->getIndex() > 0xc8 ||
                 status->getCurrentCampaignMission() > 0x8d)) {
                forcedStation = true;
            }
        }

        if (stationId == 0x6a) {
            bool allowed = false;
            for (uint32_t j = 0; j < 10; ++j) {
                if (g_Generator_kaamoAllowed[j] == item->getIndex() ||
                    systemGood) {
                    allowed = true;
                    break;
                }
            }
            if (!allowed) {
                continue;
            }
        }

        int occurrence = item->getOccurence();
        if (item->getIndex() == 0x7a && *g_Generator_jumpDriveBoost != 0) {
            occurrence = (int) ((float) occurrence +
                                (float) (occurrence *
                                         *g_Generator_jumpDriveBoost) *
                                0.05f);
        }
        if (item->getType() == 1 && *g_Generator_weaponBoost != 0) {
            occurrence =
                    (int) ((float) occurrence +
                           (float) (occurrence * *g_Generator_weaponBoost) * 0.05f);
        }

        int sort = item->getSort();
        int mission = status->getCurrentCampaignMission();
        if (occurrence == 0 &&
            ((itemIndex > 0xc3 && g_Generator_unlockFlags[0x37] != 0) ||
             (itemIndex < 0xc4 && g_Generator_unlockFlags[0x35] != 0))) {
            occurrence = 0;
            if (itemIndex != 0x55 && item->getType() != 4 &&
                item->getIngredients() == 0 &&
                (itemIndex != 0xb5 || mission > 0x3a) &&
                (((sort != 0x22 && (sort | 2) != 0x23) || mission > 0x8d) &&
                 (sort != 0x24 && (mission > 0x8d || sort != 0x2b))) &&
                ((1 < (uint32_t)((itemIndex & 0xfffffff7) - 0xd1)) &&
                 (mission > 0x5d || itemIndex != 0xcd))) {
                bool blocked = false;
                for (uint32_t j = 0; j < 9; ++j) {
                    if (g_Generator_blockedItems[j] == itemIndex) {
                        blocked = true;
                    }
                }
                if (!blocked &&
                    (sort != 0x1d || status->inBlackMarketSystem() != 0)) {
                    int roll = random->nextInt();
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
                i != 0xaf && occurrence != 0 &&
                itemTec <= station->getTecLevel() &&
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
        if (status->hardCoreMode() &&
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
                random->nextInt() >= randomGate) {
                continue;
            }
            if (item->getIndex() != 0x7a && itemTec < minTec &&
                random->nextInt() >= 0x3d) {
                continue;
            }
        }

        int minSystem = item->getMinPriceSystem();
        int minX = systems->data()[minSystem]->getX();
        int minY = systems->data()[minSystem]->getY();
        int stationSystem = station->getSystem();
        int stationX = systems->data()[stationSystem]->getX();
        int stationY = systems->data()[stationSystem]->getY();
        int distance =
                galaxy->invDistancePercent(stationX, stationY, minX, minY);
        int amount = random->nextInt() + 5;

        if (item->getType() == 4 || item->getType() == 1) {
            if (item->getIndex() != 0x6d && item->getType() == 4) {
                if (distance > 0x32) {
                    float hard = status->hardCoreMode() ? 2.0f : 20.0f;
                    int scaled = (int) (((float) (distance - 0x32) / 100.0f) *
                                        hard);
                    if (scaled < 2) {
                        scaled = 1;
                    }
                    amount *= scaled;
                }
                if (item->getIndex() == 0x6e &&
                    random->nextInt() + 10 < amount) {
                    amount = random->nextInt() + 10;
                }
            }
        } else {
            amount /= 5;
            if (amount < 1) {
                amount = 1;
            }
        }

        Item *newItem = item->makeItem();
        newItem->setAmount(amount);
        ArrayAdd(newItem, *result);
    }

    return result;
}

static Array<Item *> **volatile
g_Generator_lootItems;
static int volatile
g_Generator_typeChances[8];

Array<int> *Generator::getLootList(int itemIndex, int amount) {
    if (itemIndex >= 0) {
        Array<int> *result = new Array<int>();
        ArrayAdd(itemIndex, *result);
        ArrayAdd(amount, *result);
        return result;
    }

    Array<Item *> **itemsPtr = g_Generator_lootItems;
    Array<Item *> *itemsForOccurence = *itemsPtr;
    Array<Item *> *items = *itemsPtr;
    int itemCount = (int) itemsForOccurence->size();

    int pairCount = AERandom::gRandom->nextInt();
    if (AERandom::gRandom->nextInt() == 0) {
        return nullptr;
    }

    Array<int> *result = new Array<int>();
    uint32_t length = (uint32_t)(pairCount << 1);
    if (pairCount == 0) {
        length = 2;
    }
    ArraySetLength(length, *result);

    for (uint32_t out = 0; out < result->size(); out += 2) {
        bool found = false;
        int type = 0;
        int selected = 0;

        for (uint32_t tries = 0; !found && tries < 100; ++tries) {
            selected = AERandom::gRandom->nextInt();
            Item *item = items->data()[selected];
            type = item->getType();
            if (item->getIngredients() == 0 &&
                AERandom::gRandom->nextInt() < g_Generator_typeChances[type]) {
                int roll = AERandom::gRandom->nextInt();
                int occurrence =
                        itemsForOccurence->data()[selected]->getOccurence();
                if (roll < occurrence && item->getSinglePrice() >= 1) {
                    found = true;
                    if ((uint32_t)(selected - 0xd9) < 2 || selected == 0xa4 ||
                        selected == 0xaf) {
                        found = false;
                    } else if (type != 4 && item->getTecLevel() > 7) {
                        found = false;
                    }
                }
            }
        }

        int count;
        if (found) {
            result->data()[out] = selected;
            if (type == 4) {
                count = AERandom::gRandom->nextInt();
                type = 4;
            } else {
                count = AERandom::gRandom->nextInt();
            }
        } else {
            result->data()[out] = AERandom::gRandom->nextInt() + 0x9a;
            count = AERandom::gRandom->nextInt();
            type = 4;
        }
        result->data()[out | 1] = count + 1;
    }

    Ship *ship = Status::gStatus->getShip();
    if (ship->hasJumpDrive() != 0) {
        ship = Status::gStatus->getShip();
        if (ship->hasCargo(0x7a, 1) == 0 &&
            AERandom::gRandom->nextInt() < 10) {
            result->data()[0] = 0x7a;
        }
    }
    return result;
}

static const volatile int
kaamoSpecialItems[9] = {};

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
