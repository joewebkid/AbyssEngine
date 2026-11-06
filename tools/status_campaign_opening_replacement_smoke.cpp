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
    void setUnsaleable(bool value) { unsaleable = value; }
    int getIndex() { return prototypeIndex; }
    int getSinglePrice() { return price; }
    void setPrice(int value) { price = value; }
};

struct Ship {
    Item *oldEquipment = nullptr;
    Item *removed = nullptr;
    Item *added = nullptr;
    int requestedSort = -1;
    int cargoSetCount = 0;
    int cargoClearCount = 0;
    int race = 1;
    int raceSetCount = 0;
    Array<Item *> equipment;
    Array<Item *> cargo;
    void setCargo(int value) { assert(value == 0); ++cargoSetCount; }
    void removeAllCargo() { ++cargoClearCount; }
    Array<Item *> *getEquipment() { return &equipment; }
    Array<Item *> *getCargo() { return &cargo; }
    Item *getFirstEquipmentOfSort(int sort) {
        requestedSort = sort;
        return oldEquipment;
    }
    void removeEquipment(Item *item) { removed = item; }
    void addEquipment(Item *item) { added = item; }
    void setRace(int value) { race = value; ++raceSetCount; }
};

struct Globals { static void *items; };
void *Globals::items = nullptr;

struct Status {
    int64_t playingTime = 0;
    int field_0x100 = 0;
    int field_0x104 = 0;
    int currentCampaignMission = 0;
    int missionCount = 0;
    int field_7c = -1;
    int field_80 = -1;
    unsigned char field_0x17c = 0;
    Array<bool> *systemVisibilities = nullptr;
    Ship *ship = nullptr;
    Mission *mission = nullptr;
    Array<Mission *> *missions = nullptr;
    void setCampaignMission(Mission *value) {
        value->campaign = true;
        (*missions)[0] = value;
    }
    void nextCampaignMission(bool advance);
};

#include "status_campaign_opening_replacement_under_test.inc"

int main() {
    Item prototypes[87];
    Item *catalog[87];
    for (int i = 0; i != 87; ++i) {
        prototypes[i].prototypeIndex = i;
        catalog[i] = &prototypes[i];
    }
    Array<Item *> items;
    items.values.assign(catalog, catalog + 87);
    Globals::items = &items;

    Array<Mission *> missions;
    missions.values.resize(2);
    Array<bool> visibilities;
    visibilities.values.resize(7);
    Ship ship;
    Status status;
    status.playingTime = 0x1234567887654321LL;
    status.missionCount = 41;
    status.ship = &ship;
    status.missions = &missions;
    status.systemVisibilities = &visibilities;

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
    const int types[33] = {11, 154, 11, 154, 11, 158, 4, 11, 11, 11,
                           11, 11, 150, 4, 11, 4, 11, 156, 156, 189, 4,
                           11, 11, 4, 156, 4, 11, 4, 4, 156, 11, 11, 8};
    const int stations[33] = {78, 78, 78, 78, 78, 78, 78, 78, 78, 79,
                              76, 79, 0, 79, 98, 98, 98, 56, 55, 55, 55,
                              55, 10, 48, -1, 48, 10, 91, -1, 91, 98, 10, 10};
    for (int step = 1; step <= 32; ++step) {
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
        assert(missions[0]->visible == (step != 12));
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
        delete missions[0];
        missions[0] = nullptr;
    }
    assert(ship.raceSetCount == 1);
    delete ship.added;
}
