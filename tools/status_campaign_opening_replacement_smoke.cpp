#include <cassert>
#include <cstdint>
#include <vector>

template <typename T> struct Array {
    std::vector<T> values;
    unsigned size() const { return static_cast<unsigned>(values.size()); }
    T &operator[](unsigned index) { return values[index]; }
};

template <typename T> void ArrayAdd(T value, Array<T> &array) {
    array.values.push_back(value);
}

struct Mission {
    int type;
    int param;
    int station;
    int statusValue = -1;
    bool campaign = false;
    Mission() : Mission(0, 0, 0) {}
    Mission(int t, int p, int s) : type(t), param(p), station(s) {}
    void setStatusValue(int value) { statusValue = value; }
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
};

struct Globals { static void *items; };
void *Globals::items = nullptr;

struct Status {
    int64_t playingTime = 0;
    int field_0x100 = 0;
    int field_0x104 = 0;
    int currentCampaignMission = 0;
    unsigned char field_0x17c = 0;
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
    Ship ship;
    Status status;
    status.playingTime = 0x1234567887654321LL;
    status.ship = &ship;
    status.missions = &missions;

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
    const int types[10] = {11, 154, 11, 154, 11, 158, 4, 11, 11, 11};
    for (int step = 1; step <= 9; ++step) {
        status.currentCampaignMission = step;
        status.nextCampaignMission(true);
        assert(status.currentCampaignMission == step + 1);
        assert(missions[0] != nullptr && missions[0]->campaign);
        assert(missions[0]->type == types[step] && missions[0]->param == 0);
        assert(missions[0]->station == (step == 9 ? 79 : 78));
        assert(missions[0]->statusValue == (step == 1 ? 10 : step == 3 ? 25 : -1));
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
        delete missions[0];
        missions[0] = nullptr;
    }
    delete ship.added;
}
