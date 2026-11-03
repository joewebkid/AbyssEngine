#include <cassert>

struct Mission {
    static void *empty;
    static int destroyed[3];
    int id;
    bool campaign = false;

    explicit Mission(int value) : id(value) {}
    ~Mission() { ++destroyed[id]; }
    void setCampaignMission(bool value) { campaign = value; }
};

void *Mission::empty = nullptr;
int Mission::destroyed[3] = {};

template <typename T> struct Array {
    T *items;
    T *data() { return items; }
};

struct Status {
    Array<Mission *> *missions;
    void setCampaignMission(Mission *mission);
};

#include "status_campaign_sentinel_under_test.inc"

int main() {
    Mission *shared = new Mission(0);
    Mission::empty = shared;
    Mission *slots[1] = {shared};
    Array<Mission *> array{slots};
    Status status{&array};

    Mission *first = new Mission(1);
    status.setCampaignMission(first);
    assert(slots[0] == first && first->campaign);
    assert(Mission::destroyed[0] == 0);

    Mission *second = new Mission(2);
    status.setCampaignMission(second);
    assert(slots[0] == second && second->campaign);
    assert(Mission::destroyed[1] == 1 && Mission::destroyed[0] == 0);

    delete second;
    delete shared;
    Mission::empty = nullptr;
    assert(Mission::destroyed[0] == 1 && Mission::destroyed[2] == 1);
}
