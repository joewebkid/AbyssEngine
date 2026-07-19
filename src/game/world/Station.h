#ifndef GOF2_STATION_H
#define GOF2_STATION_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/Agent.h"
#include "game/ship/Ship.h"

class Agent;
class Item;
class Ship;


class Station {
public:
    String name;
    int index;
    int systemIndex;
    uint8_t planet;
    int textureIndex;
    uint8_t visited;
    int techLevel;
    uint8_t attackedFriends;
    Array<Item *> *items;
    Array<Ship *> *ships;
    Array<Agent *> *agents;

    Station();

    Station(String name, int index, int systemIndex, int techLevel, int textureIndex);

    ~Station();

    Station *clone();

    void addItem(Item *item);

    void addShip(Ship *ship);

    void removeShip(Ship *ship);

    void removeShips();

    bool equals(Station *other);

    uint32_t hasItem(int index);

    uint32_t hasShip(int index);

    int getIndex();

    int getSystem();

    int getTecLevel();

    int getTextureIndex();

    bool isPlanet();

    String getName();

    Array<Agent *> *getAgents();

    Array<Item *> *getItems();

    Array<Ship *> *getShips();

    void setAgents(Array<Agent *> *agents);

    void setItems(Array<Item *> *items, bool deep);

    void setShips(Array<Ship *> *ships, bool deep);

    void setAttackedFriends(bool v);

    uint8_t hasAttackedFriends();

    bool isAttackedByAliens();

    uint8_t isDiscovered();

    void visit();

    uint32_t getHiddenBlueprintIndex();

    uint32_t getPirateStationIndex();

    uint32_t stationHasHiddenBlueprint(bool ignoreFound);

    uint32_t stationHasPirateBase();
};
#endif
