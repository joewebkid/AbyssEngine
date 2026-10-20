#include <cassert>
#include <vector>

template<class T> struct Array {
    std::vector<T> values;
};
template<class T> void ArrayAdd(T value, Array<T> &array) {
    array.values.push_back(value);
}
struct Mission {
    int type;
    int calls = 0;
    int getType() { ++calls; return type; }
};
struct Status {
    Mission *mission;
    int calls = 0;
    Mission *getMission() { ++calls; return mission; }
};
struct Globals { static Status *status; };
Status *Globals::status = nullptr;
struct PlayerFighter {
    bool isMissionCrate = false;
    Array<int> *lootList = nullptr;
    void setMissionCrate(bool on);
};

// PRODUCTION_METHOD

int main() {
    Mission mission{5};
    Status status{&mission};
    Globals::status = &status;
    PlayerFighter off;
    off.setMissionCrate(false);
    assert(!off.isMissionCrate && off.lootList == nullptr);
    assert(status.calls == 0 && mission.calls == 0);

    PlayerFighter special;
    special.setMissionCrate(true);
    assert(special.isMissionCrate && special.lootList);
    assert((special.lootList->values == std::vector<int>{0x74, 1}));
    delete special.lootList;

    mission.type = 2;
    PlayerFighter ordinary;
    ordinary.setMissionCrate(true);
    assert((ordinary.lootList->values == std::vector<int>{0x75, 1}));
    delete ordinary.lootList;
    assert(status.calls == 2 && mission.calls == 2);
}
