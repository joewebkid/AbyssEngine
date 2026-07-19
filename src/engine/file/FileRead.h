#ifndef GOF2_FILEREAD_H
#define GOF2_FILEREAD_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/file/AEFile.h"
#include "engine/math/Vector.h"
#include "game/world/NewsItem.h"
#include "game/world/SolarSystem.h"
#include "game/world/SpacePoint.h"
#include "game/world/Wanted.h"

#include "engine/math/AEMath.h"


class Agent;
class Item;
class NewsItem;
class Ship;
class SolarSystem;
class SpacePoint;
class Station;
class Wanted;


class FileRead {
public:
    FileRead();

    ~FileRead();

    int32_t loadStation(int32_t id);

    int32_t loadStationsBinary();

    Array<Array<AbyssEngine::AEMath::Vector *> *> *loadWeaponPositions(int32_t id);

    Array<SpacePoint *> *loadSpacePoints(int32_t id, int32_t group);

    Array<SolarSystem *> *loadSystemsBinary();

    Array<Wanted *> *loadWanted();

    Array<NewsItem *> *loadTicker();

    Array<Station *> *loadStationsBinary(int16_t *ids, int32_t count);

    Array<String *> *loadNamesBinary(int32_t type, bool first, bool second);

    Array<Station *> *loadStationsBinary(SolarSystem *system);

    Array<Agent *> *loadAgents();

    Array<int32_t> *loadWreckCollision(int32_t id);

    Array<int32_t> *loadStationCollision(int32_t id);

    Array<int32_t> *loadStaticCollision(int32_t id);

    Array<int32_t> *loadStationParts(int32_t id, int32_t special);

    Array<int32_t> *loadShipParts(int32_t id);

    Array<Item *> *loadItemsBinary();

    Array<Ship *> *loadShipsBinary();
};
#endif
