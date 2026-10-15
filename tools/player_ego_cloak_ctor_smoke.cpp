#include <cassert>
#include <cstring>

struct Item {
    int charge = 0;
    int discharge = 0;
    int getAttribute(int id) {
        assert(id == 35 || id == 36);
        return id == 35 ? charge : discharge;
    }
};
template<class T> struct Array {
    T entries[96]{};
    T *data() { return entries; }
};
struct Ship {
    bool integrated = false;
    Item *equipped = nullptr;
    int equipmentQueries = 0;
    bool hasCloakIntegrated() { return integrated; }
    Item *getFirstEquipmentOfSort(int sort) {
        assert(sort == 21);
        ++equipmentQueries;
        return equipped;
    }
};
struct GlobalsService {
    int registrations = 0;
    int lastSound = -1;
    void addSoundResourceToList(int id) { lastSound = id; ++registrations; }
};
struct Globals {
    static void *items;
    static GlobalsService *gGlobals;
    static unsigned char options[100];
};
void *Globals::items = nullptr;
GlobalsService *Globals::gGlobals = nullptr;
unsigned char Globals::options[100]{};
struct PlayerEgo {
    unsigned char cloaked = 3;
    unsigned char chargingCloak = 3;
    void *cloak = reinterpret_cast<void *>(1);
    int cloakCharge = 3;
    int cloakRechargeTimer = 3;
    int cloakChargeMax = 3;
    int cloakDischargeMax = 3;
    int cloakRechargeMax = 3;
    void initializeCloakBase();
    void initializeCloakItem(Ship *ship);
    void initializeRechargeMax();
};
void PlayerEgo::initializeCloakBase() {
// PRODUCTION_CLOAK_BASE
}
void PlayerEgo::initializeCloakItem(Ship *ship) {
// PRODUCTION_CLOAK_ITEM
}
void PlayerEgo::initializeRechargeMax() {
// PRODUCTION_RECHARGE_MAX
}

int main() {
    GlobalsService service;
    Globals::gGlobals = &service;
    Array<Item *> items;
    Globals::items = &items;
    PlayerEgo ego;
    ego.initializeCloakBase();
    assert(!ego.cloaked && !ego.chargingCloak && ego.cloak == nullptr);
    assert(ego.cloakCharge == -1 && ego.cloakRechargeTimer == 0);
    assert(ego.cloakChargeMax == 0 && ego.cloakDischargeMax == 0);

    Ship ship;
    ego.initializeCloakItem(&ship);
    assert(ego.cloak == nullptr && ship.equipmentQueries == 1 && service.registrations == 0);

    Item equipped{6000, 1500};
    ship.equipped = &equipped;
    ego.initializeCloakItem(&ship);
    assert(ego.cloak == &equipped && ego.cloakChargeMax == 6000);
    assert(ego.cloakDischargeMax == 1500 && service.lastSound == 30);
    assert(service.registrations == 1 && ship.equipmentQueries == 2);

    Item integrated{8000, 2000};
    items.entries[95] = &integrated;
    ship.integrated = true;
    ego.initializeCloakItem(&ship);
    assert(ego.cloak == &integrated && ego.cloakChargeMax == 8000);
    assert(ego.cloakDischargeMax == 2000 && service.registrations == 2);
    assert(ship.equipmentQueries == 2);

    const float difficulties[] = {0.0f, 0.5f, 1.0f, 1.5f};
    const int expected[] = {5000, 7000, 9000, 12000};
    for (int i = 0; i < 4; ++i) {
        std::memcpy(Globals::options + 0x2c, &difficulties[i], sizeof(float));
        ego.initializeRechargeMax();
        assert(ego.cloakRechargeMax == expected[i]);
    }
}
