#include <cassert>
#include <vector>

template<class T> struct Array {
    std::vector<T> values;
    unsigned size() const { return static_cast<unsigned>(values.size()); }
    T *data() { return values.data(); }
    typename std::vector<T>::reference operator[](unsigned index) { return values[index]; }
};

namespace AbyssEngine {
struct AERandom {
    int bound = 0, calls = 0, result = 0;
    int nextInt(int value) { bound = value; ++calls; return result; }
};
}

struct Item {
    int index = 0, amount = 1, type = 0, changed = 0, unsaleable = 0;
    Item *made = nullptr;
    int madeAmount = -1;
    Item *makeItem(int value) {
        madeAmount = value;
        if (made) made->amount = value;
        return made;
    }
    int getIndex() const { return index; }
    int getAmount() const { return amount; }
    int getType() const { return type; }
    void setUnsaleable(int value) { unsaleable = value; }
    void changeAmount(int value) { changed += value; amount += value; }
};
struct Ship {
    int freeSpace = 10, available = 1, addCalls = 0;
    Item *added = nullptr;
    Array<Item *> *equipment = nullptr;
    int getFreeSpace() { return freeSpace; }
    int spaceAvailable(int) { return available; }
    Array<Item *> *getEquipment() { return equipment; }
    void addCargo(Item *item) { ++addCalls; added = item; }
};
struct Standing {
    int calls = 0, group = -1;
    void applyStealCargo(int value) { ++calls; group = value; }
};
struct Status {
    Ship *ship = nullptr;
    Standing *standing = nullptr;
    Array<bool> *field_ac = nullptr;
    int field_cc = 0, captured = 0;
    Ship *getShip() { return ship; }
    Standing *getStanding() { return standing; }
    void crateCaptured(int value) { captured += value; }
};
struct Globals {
    static void *rnd, *items;
    static Status *status;
};
void *Globals::rnd = nullptr;
void *Globals::items = nullptr;
Status *Globals::status = nullptr;
struct Player { int carriesFriendCargoFlag = 0; };
struct Level {
    int field_1c = 0, stolen = 0;
    void stealFriendCargo() { ++stolen; }
};
struct Hud {
    int calls = 0, item = -1, amount = 0;
    bool single = false, mission = false, extender = false, slot = false, aggregate = false;
    void catchCargo(int id, int count, bool a, bool b, bool c, bool d, bool e) {
        ++calls; item = id; amount = count;
        single = a; mission = b; extender = c; slot = d; aggregate = e;
    }
};
struct KIPlayer {
    int state = 3, field_0x101 = 0, hasCargo = 1, stealFlag = 1, shipGroup = 8;
    int carriesMissionCrate = 0, diedWithMissionCrate = 0, lostMissionCrateToEgo = 0;
    int activeCalls = 0;
    bool active = true;
    Array<int> *cargo = nullptr;
    void *crateGeometry = nullptr;
    Player *player = nullptr;
    Level *level = nullptr;
    void setActive(bool value) { active = value; ++activeCalls; }
    void captureCrate(Hud *hud);
};

// PRODUCTION_METHOD

int main() {
    AbyssEngine::AERandom random;
    Array<Item *> prototypes;
    prototypes.values.resize(200);
    Ship ship;
    Standing standing;
    Array<bool> discoveries;
    discoveries.values.resize(22);
    Status status{&ship, &standing, &discoveries};
    Player player;
    Level level;
    Hud hud;
    KIPlayer ki;
    ki.player = &player;
    ki.level = &level;
    Globals::rnd = &random;
    Globals::items = &prototypes;
    Globals::status = &status;

    ki.field_0x101 = 1;
    ki.crateGeometry = &ki;
    ki.captureCrate(&hud);
    assert(ki.state == 3 && ki.hasCargo == 0 && ki.activeCalls == 1 && !ki.active);
    assert(ki.crateGeometry == nullptr && hud.calls == 0);

    Item mission{116, 1, 1}, clone{116, 1, 1}, equipped{116, 3, 1};
    mission.made = &clone;
    prototypes[116] = &mission;
    Array<int> cargo{{116, 5}};
    Array<Item *> equipment{{&equipped}};
    ki.state = 2;
    ki.cargo = &cargo;
    ki.carriesMissionCrate = 1;
    ki.stealFlag = 0;
    player.carriesFriendCargoFlag = 1;
    ship.freeSpace = 2;
    ship.equipment = &equipment;
    random.result = 4;
    ki.captureCrate(&hud);
    assert(random.calls == 1 && random.bound == 5);
    assert(mission.madeAmount == 2 && cargo[1] == 3 && clone.amount == 2);
    assert(level.stolen == 1 && standing.calls == 1 && standing.group == 8);
    assert(status.captured == 2 && clone.unsaleable == 1);
    assert(equipped.amount == 5 && equipped.changed == 2 && ship.addCalls == 0);
    assert(level.field_1c == 2 && ki.lostMissionCrateToEgo == 1);
    assert(hud.calls == 1 && hud.item == 116 && hud.amount == 2);
    assert(!hud.single && hud.mission && !hud.extender && !hud.slot && !hud.aggregate);

    Item full{117, 1, 0}, fullClone{117, 1, 0};
    full.made = &fullClone;
    prototypes[117] = &full;
    Array<int> fullCargo{{117, 2}};
    ki.state = 3;
    ki.cargo = &fullCargo;
    ship.freeSpace = 10;
    ship.available = 0;
    ki.captureCrate(&hud);
    assert(full.madeAmount == 2 && fullCargo[1] == 0 && ki.diedWithMissionCrate == 1);
    assert(status.captured == 2 && hud.calls == 2 && hud.single && !hud.mission);
    assert(!hud.extender && !hud.slot && !hud.aggregate);

    Item ordinary{5, 1, 0}, ordinaryClone{5, 1, 0};
    ordinary.made = &ordinaryClone;
    prototypes[5] = &ordinary;
    Array<int> ordinaryCargo{{5, 1}};
    ki.cargo = &ordinaryCargo;
    ki.carriesMissionCrate = 0;
    ki.shipGroup = 9;
    ship.available = 1;
    ki.captureCrate(&hud);
    assert(ship.addCalls == 1 && ship.added == &ordinaryClone);
    assert(status.field_cc == 1 && status.captured == 3);
    assert(hud.calls == 3 && !hud.single && !hud.mission);

    Item hidden{132, 1, 0}, hiddenClone{132, 1, 0};
    hidden.made = &hiddenClone;
    prototypes[132] = &hidden;
    Array<int> hiddenCargo{{132, 1}};
    ki.cargo = &hiddenCargo;
    ki.shipGroup = 8;
    ki.captureCrate(&hud);
    assert(discoveries[0] && ship.addCalls == 2);

    Item missing{7, 1, 0};
    prototypes[7] = &missing;
    Array<int> missingCargo{{7, 1}};
    ki.cargo = &missingCargo;
    ki.captureCrate(&hud);
    assert(missing.madeAmount == 1 && missingCargo[1] == 0 && hud.calls == 4);
}
