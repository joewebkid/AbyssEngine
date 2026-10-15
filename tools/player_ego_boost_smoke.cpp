#include <cassert>

struct Vector {};
struct Item {
    int index = 0;
    int getIndex() { return index; }
};
struct Ship {
    int delay = 0;
    int speed = 100;
    int time = 5000;
    Item *boostItem = nullptr;
    int getBoostDelay() { return delay; }
    int getBoostSpeed() { return speed; }
    int getBoostTime() { return time; }
    Item *getFirstEquipmentOfSort(int sort) { assert(sort == 14); return boostItem; }
};
struct FModSound {
    int plays = 0;
    int lastId = -1;
    void play(int id, Vector *position, Vector *velocity, float gain) {
        assert(position == nullptr && velocity == nullptr && gain == 0.0f);
        lastId = id;
        ++plays;
    }
};
struct GlobalsService {
    int registrations = 0;
    int lastId = -1;
    void addSoundResourceToList(int id) { lastId = id; ++registrations; }
};
struct Globals {
    static FModSound *sound;
    static GlobalsService *gGlobals;
};
FModSound *Globals::sound = nullptr;
GlobalsService *Globals::gGlobals = nullptr;
struct PlayerEgo {
    int boostTimer = -9;
    unsigned char boostingFlag = 2;
    int rocketControlGun = 2;
    int boostSpeed = 0;
    int field_0xcc = 0;
    int field_0xd0 = 0;
    int boostSoundId = -1;
    unsigned char field_0x146 = 2;
    float speedValue = 0.0f;
    void initializeBoostDefaults();
    void initializeBoostFromShip(Ship *ship);
    void boost();
};
void PlayerEgo::initializeBoostDefaults() {
// PRODUCTION_BOOST_DEFAULTS
}
void PlayerEgo::initializeBoostFromShip(Ship *ship) {
// PRODUCTION_BOOST_SHIP
}
// PRODUCTION_BOOST_METHOD

int main() {
    FModSound sound;
    GlobalsService service;
    Globals::sound = &sound;
    Globals::gGlobals = &service;
    Ship ship;
    PlayerEgo ego;
    ego.initializeBoostDefaults();
    assert(ego.boostTimer == 0 && ego.boostingFlag == 0 && ego.rocketControlGun == 0);
    assert(ego.boostSpeed == 5 && ego.field_0xcc == 5000 && ego.field_0xd0 == 20000);
    assert(ego.boostSoundId == 0 && ego.field_0x146 == 0);

    ego.initializeBoostFromShip(&ship);
    assert(ego.field_0x146 == 0 && ego.boostSpeed == 4 && ego.field_0xcc == 5000);
    ego.boost();
    assert(sound.plays == 0 && service.registrations == 0);

    ship.delay = 2000;
    ship.speed = 150;
    ship.time = 7000;
    ego.initializeBoostFromShip(&ship);
    assert(ego.field_0x146 == 1 && ego.boostSpeed == 5);
    assert(ego.field_0xcc == 7000 && ego.field_0xd0 == 2000);
    ego.rocketControlGun = 1;
    ego.boost();
    assert(sound.plays == 0);
    ego.rocketControlGun = 0;
    ego.boostTimer = -1;
    ego.boost();
    assert(sound.plays == 0);
    ego.boostTimer = 0;
    ego.boost();
    assert(sound.plays == 1 && sound.lastId == 0);
    assert(ego.boostingFlag == 1 && ego.boostTimer == 0 && ego.speedValue == 5.0f);
    ego.boost();
    assert(sound.plays == 1);

    const int indexes[] = {71, 72, 73, 74, 195, 200};
    const int ids[] = {38, 39, 40, 41, 1102, 0};
    for (int i = 0; i < 6; ++i) {
        Item item{indexes[i]};
        ship.boostItem = &item;
        PlayerEgo candidate;
        candidate.initializeBoostDefaults();
        candidate.initializeBoostFromShip(&ship);
        assert(candidate.boostSoundId == ids[i] && service.lastId == ids[i]);
        assert(service.registrations == i + 1);
        candidate.boost();
        assert(sound.lastId == ids[i] && candidate.speedValue == 5.0f);
    }
}
