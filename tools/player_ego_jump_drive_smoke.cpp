#include <cassert>

struct Vector {};
struct Item {
    int charge = 0;
    int getAttribute(int id) { assert(id == 37); return charge; }
};
template<class T> struct Array {
    T entries[96]{};
    T *data() { return entries; }
};
struct Ship {
    Item *equipped = nullptr;
    bool integrated = false;
    int equipmentQueries = 0;
    int integratedQueries = 0;
    Item *getFirstEquipmentOfSort(int sort) {
        assert(sort == 18);
        ++equipmentQueries;
        return equipped;
    }
    bool hasJumpDriveIntegrated() { ++integratedQueries; return integrated; }
};
int sequence = 0;
struct FModSound {
    int plays = 0;
    int soundOrder = 0;
    void play(int id, Vector *position, Vector *velocity, float gain) {
        assert(id == 33 && position == nullptr && velocity == nullptr && gain == 0.0f);
        soundOrder = ++sequence;
        ++plays;
    }
};
struct GlobalsService {
    int sounds[2] = {-1, -1};
    int count = 0;
    void addSoundResourceToList(int id) { assert(count < 2); sounds[count++] = id; }
};
struct Globals {
    static void *items;
    static FModSound *sound;
    static GlobalsService *gGlobals;
};
void *Globals::items = nullptr;
FModSound *Globals::sound = nullptr;
GlobalsService *Globals::gGlobals = nullptr;
struct PlayerEgo;
struct Hud {
    int events = 0;
    int hudOrder = 0;
    void hudEvent(int id, PlayerEgo *player, int value) {
        assert(id == 25 && player != nullptr && value == 0);
        hudOrder = ++sequence;
        ++events;
    }
};
struct PlayerEgo {
    int driveCharge = -7;
    int driveChargeMax = -7;
    unsigned char chargingDrive = 7;
    void *hud = nullptr;
    void initializeDriveBase();
    void initializeDriveFromShip(Ship *ship);
    void startJumpDrive();
};
void PlayerEgo::initializeDriveBase() {
// PRODUCTION_DRIVE_BASE
}
void PlayerEgo::initializeDriveFromShip(Ship *ship) {
// PRODUCTION_DRIVE_ITEM
}
// PRODUCTION_START_DRIVE

int main() {
    GlobalsService service;
    Globals::gGlobals = &service;
    FModSound sound;
    Globals::sound = &sound;
    Array<Item *> items;
    Globals::items = &items;
    Hud hud;
    PlayerEgo ego;
    ego.hud = &hud;
    ego.initializeDriveBase();
    assert(ego.driveCharge == 0 && ego.driveChargeMax == 0 && ego.chargingDrive == 0);

    Ship ship;
    ego.initializeDriveFromShip(&ship);
    assert(ego.driveChargeMax == 0 && service.count == 0);
    assert(ship.equipmentQueries == 1 && ship.integratedQueries == 1);

    Item equipped{6000};
    ship.equipped = &equipped;
    ship.integrated = true;
    ego.initializeDriveFromShip(&ship);
    assert(ego.driveChargeMax == 6000 && service.count == 2);
    assert(service.sounds[0] == 32 && service.sounds[1] == 33);
    assert(ship.integratedQueries == 1);

    Item builtIn{8000};
    items.entries[85] = &builtIn;
    ship.equipped = nullptr;
    ego.initializeDriveFromShip(&ship);
    assert(ego.driveChargeMax == 8000 && service.count == 2);
    assert(ship.integratedQueries == 2);

    ego.driveCharge = 500;
    ego.chargingDrive = 1;
    ego.startJumpDrive();
    assert(sound.plays == 0 && hud.events == 0 && ego.driveCharge == 500);
    ego.chargingDrive = 0;
    ego.startJumpDrive();
    assert(sound.plays == 1 && hud.events == 1);
    assert(sound.soundOrder < hud.hudOrder);
    assert(ego.driveCharge == 0 && ego.chargingDrive == 1);
    ego.startJumpDrive();
    assert(sound.plays == 1 && hud.events == 1);
}
