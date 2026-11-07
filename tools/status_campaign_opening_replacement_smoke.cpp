#include <cassert>
#include <cstdint>
#include <vector>

template <typename T> struct Array {
    std::vector<T> values;
    unsigned size() const { return static_cast<unsigned>(values.size()); }
    decltype(auto) operator[](unsigned index) { return values[index]; }
};

template <typename T> void ArrayAdd(T value, Array<T> &array) {
    array.values.push_back(value);
}
template <typename T> void ArraySetLength(unsigned count, Array<T> &array) {
    array.values.resize(count);
}

struct Mission {
    int type;
    int param;
    int station;
    int statusValue = -1;
    int productionGoodsA = -1;
    int productionGoodsB = -1;
    bool campaign = false;
    bool visible = true;
    Mission() : Mission(0, 0, 0) {}
    Mission(int t, int p, int s) : type(t), param(p), station(s) {}
    int getStatusValue() { return statusValue; }
    void setStatusValue(int value) { statusValue = value; }
    void setProductionGoods(int a, int b) { productionGoodsA = a; productionGoodsB = b; }
    void setVisible(bool value) { visible = value; }
};

struct Item {
    int prototypeIndex = -1;
    int amount = 0;
    bool unsaleable = false;
    int price = 0;
    Item *makeItem(int quantity = 1) {
        Item *copy = new Item(*this);
        copy->amount = quantity;
        return copy;
    }
    Item *makeItem(int quantity, int newPrice) {
        Item *copy = makeItem(quantity);
        copy->price = newPrice;
        return copy;
    }
    void setUnsaleable(bool value) { unsaleable = value; }
    int getIndex() { return prototypeIndex; }
    int getSinglePrice() { return price; }
    void setPrice(int value) { price = value; }
};

struct BluePrint {
    int index = -1;
    bool unlocked = false;
    bool locked = false;
    Item *addedItem = nullptr;
    int addedAmount = -1;
    int addedStation = -1;
    int getIndex() { return index; }
    void unlock() { unlocked = true; }
    void lock() { locked = true; }
    void addItem(Item *item, int amount, int station) {
        addedItem = item;
        addedAmount = amount;
        addedStation = station;
    }
};

struct Ship {
    struct EquipmentCall { int index; int slot; Item *item; };
    int prototypeIndex = -1;
    int madePrice = 0;
    std::vector<EquipmentCall> equipmentCalls;
    Item *oldEquipment = nullptr;
    Item *removed = nullptr;
    Item *added = nullptr;
    std::vector<Item *> addedCargo;
    int requestedSort = -1;
    int cargoSetCount = 0;
    int cargoClearCount = 0;
    int race = 1;
    int raceSetCount = 0;
    int removedCargoIndex = -1;
    int removedCargoAmount = -1;
    int removedAllCargoIndex = -1;
    Array<Item *> equipment;
    Array<Item *> cargo;
    ~Ship() { for (const EquipmentCall &call : equipmentCalls) delete call.item; }
    Ship *makeShip(int price) {
        Ship *copy = new Ship();
        copy->prototypeIndex = prototypeIndex;
        copy->madePrice = price;
        return copy;
    }
    void setCargo(int value) { assert(value == 0); ++cargoSetCount; }
    void removeAllCargo() { ++cargoClearCount; }
    int removeCargo(int index, int amount) {
        removedCargoIndex = index;
        removedCargoAmount = amount;
        return 0;
    }
    void removeCargo(int index) { removedAllCargoIndex = index; }
    Array<Item *> *getEquipment() { return &equipment; }
    Array<Item *> *getCargo() { return &cargo; }
    Item *getFirstEquipmentOfSort(int sort) {
        requestedSort = sort;
        return oldEquipment;
    }
    void removeEquipment(Item *item) { removed = item; }
    void addEquipment(Item *item) { added = item; }
    void addCargo(Item *item) { addedCargo.push_back(item); }
    void setEquipment(Item *item, int slot) {
        equipmentCalls.push_back({item->prototypeIndex, slot, item});
    }
    void setRace(int value) { race = value; ++raceSetCount; }
};

struct Station {
    std::vector<Item *> addedItems;
    int removeShipsCount = 0;
    void addItem(Item *item) { addedItems.push_back(item); }
    void removeShips() { ++removeShipsCount; }
};

struct Status;
struct Globals { static void *items; static void *ships; static Status *status; };
void *Globals::items = nullptr;
void *Globals::ships = nullptr;
Status *Globals::status = nullptr;

struct Status {
    int64_t playingTime = 0;
    int field_0x100 = 0;
    int field_0x104 = 0;
    int currentCampaignMission = 0;
    int missionCount = 0;
    int32_t credits = 0;
    int field_0x30 = -1;
    int field_7c = -1;
    int field_80 = -1;
    int field_10c = -1;
    unsigned char field_0x17c = 0;
    Array<bool> *systemVisibilities = nullptr;
    Array<BluePrint *> *bluePrints = nullptr;
    Array<int> *field_90 = nullptr;
    Ship *ship = nullptr;
    Station *station = nullptr;
    Ship *previousShip = nullptr;
    Mission *mission = nullptr;
    Array<Mission *> *missions = nullptr;
    void setCampaignMission(Mission *value) {
        value->campaign = true;
        (*missions)[0] = value;
    }
    void setPreviousShip(Ship *value) { previousShip = value; }
    Ship *getPreviousShip() const { return previousShip; }
    void setShip(Ship *value) {
        if (ship != nullptr) delete ship;
        ship = value;
    }
    void nextCampaignMission(bool advance);
};

#include "status_campaign_opening_replacement_under_test.inc"

int main() {
    Item prototypes[182];
    Item *catalog[182];
    for (int i = 0; i != 182; ++i) {
        prototypes[i].prototypeIndex = i;
        catalog[i] = &prototypes[i];
    }
    Array<Item *> items;
    items.values.assign(catalog, catalog + 182);
    Globals::items = &items;

    Ship shipPrototypes[42];
    Ship *shipCatalog[42];
    for (int i = 0; i != 42; ++i) {
        shipPrototypes[i].prototypeIndex = i;
        shipCatalog[i] = &shipPrototypes[i];
    }
    Array<Ship *> ships;
    ships.values.assign(shipCatalog, shipCatalog + 42);
    Globals::ships = &ships;

    Array<Mission *> missions;
    missions.values.resize(2);
    Array<bool> visibilities;
    visibilities.values.resize(34);
    BluePrint otherBlueprint;
    otherBlueprint.index = 84;
    BluePrint targetBlueprint;
    targetBlueprint.index = 85;
    BluePrint productionBlueprint;
    productionBlueprint.index = 179;
    Array<BluePrint *> blueprints;
    blueprints.values = {&otherBlueprint, &targetBlueprint, &productionBlueprint};
    Ship ship;
    Station station;
    Status status;
    Globals::status = &status;
    status.playingTime = 0x1234567887654321LL;
    status.missionCount = 41;
    status.ship = &ship;
    status.station = &station;
    status.missions = &missions;
    status.systemVisibilities = &visibilities;
    status.bluePrints = &blueprints;

    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 1);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 11 && missions[0]->param == 0);
    assert(missions[0]->station == 78 && status.mission == missions[0]);
    assert(status.field_0x100 == static_cast<int>(status.playingTime));
    assert(status.field_0x104 == static_cast<int>(status.playingTime >> 32));
    delete missions[0];
    missions[0] = nullptr;
    status.mission = nullptr;

    Item old;
    ship.oldEquipment = &old;
    Item equipped;
    Item carried;
    equipped.price = 1;
    carried.price = 2;
    equipped.unsaleable = true;
    carried.unsaleable = true;
    prototypes[0].price = 110;
    ship.equipment.values.push_back(&equipped);
    ship.cargo.values.push_back(&carried);
    Item questCargo;
    questCargo.prototypeIndex = 0x83;
    ship.cargo.values.push_back(&questCargo);
    const int types[54] = {11, 154, 11, 154, 11, 158, 4, 11, 11, 11,
                           11, 11, 150, 4, 11, 4, 11, 156, 156, 189, 4,
                           11, 11, 4, 156, 4, 11, 4, 4, 156, 11, 11, 8, 11, 11, 12, 160, 4, 11,
                           161, 4, 160, 11, 11, 0, 0, 11, 11, 156, 156, 156, 160, 0, 11};
    const int stations[54] = {78, 78, 78, 78, 78, 78, 78, 78, 78, 79,
                              76, 79, 0, 79, 98, 98, 98, 56, 55, 55, 55,
                              55, 10, 48, -1, 48, 10, 91, -1, 91, 98, 10, 10, 30, 29, 27, 27, 22, 30,
                              -1, -1, -1, 10, 10, 0, 0, 74, 58, 58, 62, 25, 25, 0, 74};
    for (int step = 1; step <= 44; ++step) {
        if (step == 44) status.credits = 100;
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        assert(status.currentCampaignMission == step + 1);
        assert(missions[0] != nullptr && missions[0]->campaign);
        assert(missions[0]->type == types[step]);
        assert(missions[0]->param == (step == 22 ? 20000 : step == 30 ? 30000 : 0));
        assert(missions[0]->station == stations[step]);
        assert(missions[0]->statusValue ==
               (step == 1 ? 10 : step == 3 ? 25 : step == 12 ? 42 :
                step == 19 ? 6 : -1));
        assert(missions[0]->visible == (step != 12 && step != 36 && step != 44));
        if (step == 3) assert(ship.cargoSetCount == 1);
        if (step == 5) assert(ship.cargoClearCount == 1);
        if (step == 7) {
            assert(!equipped.unsaleable && !carried.unsaleable);
            assert(equipped.price == 110 && carried.price == 110);
        }
        if (step == 9) {
            assert(ship.requestedSort == 19 && ship.removed == &old);
            assert(ship.added != nullptr && ship.added->prototypeIndex == 86);
            assert(ship.added->amount == 1);
        }
        if (step == 17) assert(ship.race == 0 && ship.raceSetCount == 1);
        if (step == 22) assert(visibilities[6]);
        if (step == 23) assert(status.field_7c == 9 && status.field_80 == 48);
        if (step == 24) assert(questCargo.unsaleable && !carried.unsaleable);
        if (step == 25) assert(status.field_7c == -1 && status.field_80 == -1);
        if (step == 27) assert(status.field_7c == 18 && status.field_80 == 91);
        if (step == 32) {
            assert(missions[0]->productionGoodsA == 164);
            assert(missions[0]->productionGoodsB == 50);
        }
        if (step == 33) {
            assert(ship.removedCargoIndex == 164 && ship.removedCargoAmount == 50);
            assert(targetBlueprint.unlocked && !otherBlueprint.unlocked);
            assert(targetBlueprint.addedItem != nullptr);
            assert(targetBlueprint.addedItem->prototypeIndex == 164);
            assert(targetBlueprint.addedAmount == 50 && targetBlueprint.addedStation == 10);
        }
        if (step == 41) assert(status.field_7c == -10 && status.field_80 == -10);
        if (step == 44) {
            assert(status.field_7c == -10 && status.field_80 == -10);
            assert(status.credits == 40100);
        }
        delete missions[0];
        missions[0] = nullptr;
    }
    status.currentCampaignMission = 45;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 46 && missions[0] == nullptr);

    status.field_0x30 = 123;
    for (int step = 46; step <= 48; ++step) {
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        assert(status.currentCampaignMission == step + 1);
        assert(missions[0] != nullptr && missions[0]->campaign);
        assert(missions[0]->type == types[step] && missions[0]->param == 0);
        assert(missions[0]->station == stations[step]);
        if (step == 46) assert(status.ship == &ship);
        if (step == 47) {
            assert(status.previousShip == &ship && status.ship != &ship);
            assert(status.ship->prototypeIndex == 9 && status.ship->madePrice == -1);
            assert(status.ship->race == 1 && status.ship->raceSetCount == 1);
            assert(status.field_0x30 == 0 && status.ship->equipmentCalls.size() == 2);
            assert(status.ship->equipmentCalls[0].index == 58 && status.ship->equipmentCalls[0].slot == 0);
            assert(status.ship->equipmentCalls[1].index == 83 && status.ship->equipmentCalls[1].slot == 1);
        }
        if (step == 48) {
            assert(status.previousShip == &ship && status.ship != &ship);
            assert(status.ship->prototypeIndex == 41 && status.ship->madePrice == -1);
            assert(status.ship->race == 1 && status.ship->raceSetCount == 1);
            const int indices[6] = {177, 177, 177, 58, 83, 52};
            const int slots[6] = {0, 1, 2, 0, 1, 2};
            assert(status.ship->equipmentCalls.size() == 6);
            for (int i = 0; i < 6; ++i) {
                assert(status.ship->equipmentCalls[i].index == indices[i]);
                assert(status.ship->equipmentCalls[i].slot == slots[i]);
            }
        }
        delete missions[0];
        missions[0] = nullptr;
    }

    for (int step = 49; step <= 53; ++step) {
        if (step == 52) {
            g_ncmAchTable[0] = 53;
            status.field_0x17c = 0;
        }
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        const int selectedStep = step == 52 ? 53 : step;
        assert(status.currentCampaignMission == (step == 52 ? 54 : step + 1));
        assert(missions[0] != nullptr && missions[0]->campaign);
        assert(missions[0]->type == types[selectedStep]);
        assert(missions[0]->param == (selectedStep == 53 ? 200000 : 0));
        assert(missions[0]->station == stations[selectedStep]);
        assert(missions[0]->visible == (selectedStep != 51));
        if (step == 52) {
            assert(status.field_0x17c == 1);
            g_ncmAchTable[0] = 0;
        }
        delete missions[0];
        missions[0] = nullptr;
    }

    g_ncmAchTable[0] = 54;
    status.field_0x17c = 0;
    status.currentCampaignMission = 52;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 54 && status.field_0x17c == 1);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 11 && missions[0]->param == 200000);
    delete missions[0];
    missions[0] = nullptr;
    g_ncmAchTable[0] = 0;

    status.currentCampaignMission = 44;
    status.credits = 0x7fffffff;
    status.nextCampaignMission(true);
    assert(status.credits == 0 && missions[0] != nullptr && !missions[0]->visible);
    delete missions[0];
    missions[0] = nullptr;

    assert(!visibilities[23] && status.previousShip == &ship && status.ship != &ship);
    status.currentCampaignMission = 54;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 55 && visibilities[23]);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 11 && missions[0]->param == 0 && missions[0]->station == 101);
    assert(status.ship == &ship && status.previousShip == nullptr);
    delete missions[0];
    missions[0] = nullptr;

    status.currentCampaignMission = 55;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 56);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 4 && missions[0]->param == 0 && missions[0]->station == 102);
    assert(status.previousShip == &ship && status.ship != &ship);
    assert(status.ship->prototypeIndex == 39 && status.ship->madePrice == -1);
    assert(status.ship->race == 1 && status.ship->raceSetCount == 1);
    const int step55Indices[4] = {181, 52, 58, 83};
    const int step55Slots[4] = {0, 0, 1, 2};
    assert(status.ship->equipmentCalls.size() == 4);
    for (int i = 0; i < 4; ++i) {
        assert(status.ship->equipmentCalls[i].index == step55Indices[i]);
        assert(status.ship->equipmentCalls[i].slot == step55Slots[i]);
    }
    delete missions[0];
    missions[0] = nullptr;

    status.currentCampaignMission = 56;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 57);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 11 && missions[0]->param == 150000 && missions[0]->station == 101);
    assert(status.previousShip == &ship && status.ship != &ship);
    delete missions[0];
    missions[0] = nullptr;

    status.currentCampaignMission = 57;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 58);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 166 && missions[0]->param == 0 && missions[0]->station == 101);
    assert(missions[0]->productionGoodsA == 179 && missions[0]->productionGoodsB == 10);
    assert(productionBlueprint.unlocked && productionBlueprint.addedItem == &prototypes[127]);
    assert(productionBlueprint.addedAmount == 5 && productionBlueprint.addedStation == 101);
    assert(status.ship == &ship && status.previousShip == nullptr);
    delete missions[0];
    missions[0] = nullptr;

    blueprints.values.pop_back();
    status.ship = shipPrototypes[39].makeShip(-1);
    status.previousShip = &ship;
    status.currentCampaignMission = 57;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 58 && status.ship == &ship);
    assert(status.previousShip == nullptr && missions[0]->productionGoodsA == 179);
    delete missions[0];
    missions[0] = nullptr;

    BluePrint secondProductionBlueprint;
    secondProductionBlueprint.index = 179;
    blueprints.values.push_back(&productionBlueprint);
    blueprints.values.push_back(&secondProductionBlueprint);
    status.field_90 = new Array<int>();
    status.field_90->values = {-1};
    status.field_10c = 123;
    status.currentCampaignMission = 58;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 59);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 163 && missions[0]->param == 0 && missions[0]->station == 101);
    assert(missions[0]->statusValue == 0 && status.field_10c == 0);
    assert(status.field_90->values == std::vector<int>({56, 45, 22}));
    assert(productionBlueprint.locked && secondProductionBlueprint.locked);
    Mission *previousMission = missions[0];
    status.currentCampaignMission = 59;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 60);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 11 && missions[0]->param == 50000 && missions[0]->station == 101);
    assert(missions[0]->statusValue == 0);
    delete previousMission;
    delete missions[0];
    missions[0] = nullptr;

    Mission completedMission(163, 0, 101);
    completedMission.statusValue = 2;
    missions[0] = &completedMission;
    status.currentCampaignMission = 59;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 60 && missions[0]->campaign);
    assert(missions[0]->type == 11 && missions[0]->param == 150000);
    assert(missions[0]->statusValue == 1);
    delete missions[0];
    missions[0] = nullptr;

    completedMission.statusValue = -1;
    missions[0] = &completedMission;
    status.currentCampaignMission = 59;
    status.nextCampaignMission(true);
    assert(missions[0]->param == 0 && missions[0]->statusValue == 0);
    delete missions[0];
    missions[0] = nullptr;

    assert(!visibilities[22] && !visibilities[24]);
    for (int step = 60; step <= 62; ++step) {
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        assert(status.currentCampaignMission == step + 1);
        assert(missions[0] != nullptr && !missions[0]->campaign);
        assert(missions[0]->type == (step == 62 ? 4 : step == 60 ? 164 : 11));
        assert(missions[0]->param == 0 && missions[0]->station == (step == 62 ? 103 : step == 60 ? 101 : 100));
        assert(visibilities[22] == (step >= 61));
        assert(visibilities[24] == (step >= 62));
        delete missions[0];
        missions[0] = nullptr;
    }

    for (int step = 63; step <= 65; ++step) {
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        assert(status.currentCampaignMission == step + 1);
        assert(missions[0] != nullptr && !missions[0]->campaign);
        assert(missions[0]->type == (step == 63 ? 4 : 11));
        assert(missions[0]->param == 0 && missions[0]->station == (step == 63 ? 104 : step == 64 ? 100 : 101));
        delete missions[0];
        missions[0] = nullptr;
    }

    for (int i = 60; i <= 62; ++i) prototypes[i].price = 42;
    status.currentCampaignMission = 66;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 67);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 4 && missions[0]->param == 0 && missions[0]->station == 104);
    assert(missions[0]->statusValue == 0);
    assert(station.addedItems.size() == 3);
    for (int i = 0; i < 3; ++i) {
        assert(station.addedItems[i]->prototypeIndex == 60 + i);
        assert(station.addedItems[i]->amount == 5 && station.addedItems[i]->price == 0);
    }
    delete missions[0];
    missions[0] = nullptr;
    for (Item *item : station.addedItems) delete item;

    status.currentCampaignMission = 67;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 68);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 8 && missions[0]->param == 0 && missions[0]->station == 66);
    assert(missions[0]->productionGoodsA == 175 && missions[0]->productionGoodsB == 1);
    delete missions[0];
    missions[0] = nullptr;

    status.currentCampaignMission = 68;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 69 && ship.removedAllCargoIndex == 175);
    assert(missions[0] != nullptr && !missions[0]->campaign);
    assert(missions[0]->type == 6 && missions[0]->param == 0 && missions[0]->station == 66);
    delete missions[0];
    missions[0] = nullptr;

    for (int step = 69; step <= 70; ++step) {
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        assert(status.currentCampaignMission == step + 1);
        assert(missions[0] != nullptr && !missions[0]->campaign);
        assert(missions[0]->type == (step == 69 ? 6 : 11));
        assert(missions[0]->param == 0 && missions[0]->station == (step == 69 ? 65 : 66));
        delete missions[0];
        missions[0] = nullptr;
    }

    BluePrint firstFinalBlueprint;
    BluePrint secondFinalBlueprint;
    firstFinalBlueprint.index = 183;
    secondFinalBlueprint.index = 183;
    blueprints.values.push_back(&firstFinalBlueprint);
    blueprints.values.push_back(&secondFinalBlueprint);
    prototypes[175].price = 77;
    status.field_10c = 9;
    status.currentCampaignMission = 71;
    status.nextCampaignMission(true);
    assert(status.currentCampaignMission == 72);
    assert(missions[0] != nullptr && missions[0]->campaign);
    assert(missions[0]->type == 164 && missions[0]->param == 150000 && missions[0]->station == 101);
    assert(firstFinalBlueprint.unlocked && secondFinalBlueprint.unlocked);
    assert(ship.addedCargo.size() == 1 && ship.addedCargo[0]->prototypeIndex == 175);
    assert(ship.addedCargo[0]->amount == 1 && ship.addedCargo[0]->price == 77);
    assert(status.field_10c == 0);
    delete missions[0];
    missions[0] = nullptr;
    delete ship.addedCargo[0];

    for (int step = 72; step <= 74; ++step) {
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        assert(status.currentCampaignMission == step + 1);
        assert(missions[0] != nullptr && !missions[0]->campaign);
        assert(missions[0]->type == (step == 72 ? 10 : 11));
        assert(missions[0]->param == 0 && missions[0]->station == (step == 72 ? 81 : 100));
        assert(station.removeShipsCount == (step == 74 ? 1 : 0));
        delete missions[0];
        missions[0] = nullptr;
    }
    delete status.field_90;

    status.ship = nullptr;
    assert(ship.raceSetCount == 1);
    delete ship.added;
    delete targetBlueprint.addedItem;
}
