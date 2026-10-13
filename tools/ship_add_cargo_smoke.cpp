#include "engine/core/Array.h"
#include <cassert>
#include <cstdint>
#include <cstdio>

class Item;
using ItemArray = Array<Item *>;

class Item {
public:
    int index;
    int amount;
    int getAmount();
    static ItemArray *combineItems(ItemArray *items, ItemArray *stationItems);
};

struct Status {
    static Status *gStatus;
    int field_dc = 0;
};
Status *Status::gStatus = nullptr;

class Ship {
public:
    int currentLoad = 0;
    int baseLoad = 8;
    int cargoPlus = 0;
    ItemArray *cargo = nullptr;
    void refreshValue() {}
    void addCargo(ItemArray *items);
    void addCargo(Item *item);
    void setCargo(ItemArray *items);
    int getFreeSpace();
};

// PRODUCTION_METHODS

int main() {
    Status status;
    Status::gStatus = &status;
    Ship ship;

    Item core{165, 1};
    Item ore{154, 2};
    Item moreOre{154, 3};

    ship.addCargo(&core);
    assert(ship.cargo->size() == 1 && (*ship.cargo)[0] == &core);
    assert(ship.getFreeSpace() == 7 && status.field_dc == 7);

    ship.addCargo(&ore);
    assert(ship.cargo->size() == 2);
    assert((*ship.cargo)[0] == &core && (*ship.cargo)[1] == &ore);
    assert(ship.currentLoad == 3 && ship.getFreeSpace() == 5);

    ship.addCargo(&moreOre);
    assert(ship.cargo->size() == 2);
    assert((*ship.cargo)[0] == &core && (*ship.cargo)[1] == &ore);
    assert(ore.amount == 5 && ship.currentLoad == 6 && ship.getFreeSpace() == 2);
    std::puts("Ship addCargo handoff smoke: OK");
}
