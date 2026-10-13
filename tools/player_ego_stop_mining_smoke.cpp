#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

template<class T> struct Array {
    T *values;
    T *data() { return values; }
    T &operator[](int index) { return values[index]; }
};

struct Item {
    int id = 0;
    int amount = 0;
    Item *makeItem(int count) { return new Item{id, count}; }
};

struct Ship {
    int freeSpace = 0;
    std::vector<Item *> cargo;
    ~Ship() { for (Item *item : cargo) delete item; }
    int getFreeSpace() { return freeSpace; }
    void addCargo(Item *item) { cargo.push_back(item); freeSpace -= item->amount; }
};

struct Status {
    Ship *ship = nullptr;
    Array<bool> *field_94 = nullptr;
    Array<bool> *field_98 = nullptr;
    int field_a0 = 0;
    int field_a4 = 0;
    int field_d8 = 1;
    Ship *getShip() { return ship; }
    bool hardCoreMode();
};

struct CargoEvent { int id, count; bool single, mission, extender, slot, aggregate; };
struct Hud {
    std::vector<CargoEvent> events;
    void catchCargo(int id, int count, bool single, bool mission, bool extender,
                    bool slot, bool aggregate) {
        events.push_back({id, count, single, mission, extender, slot, aggregate});
    }
};

static int destroyedMiningGames = 0;
struct MiningGame {
    int ore;
    bool won;
    bool core;
    ~MiningGame() { ++destroyedMiningGames; }
    int getOreAmount() { return ore; }
    bool gameWon() { return won; }
    bool gotCore() { return core; }
};

struct Player {
    int lastHitpoints = 0;
    void setHitpoints(int value) { lastHitpoints = value; }
};
struct PlayerAsteroid {
    Player *player = nullptr;
    int hasCargo = 0;
    int asteroidIndex = 0;
};
struct Radar {};
struct FModSound {
    std::vector<int> stopped;
    int resumed = -1;
    void stop(int id) { stopped.push_back(id); }
    void resume(int id) { resumed = id; }
};
struct Globals {
    static Status *status;
    static void *items;
    static FModSound *sound;
    static unsigned char options[100];
};
Status *Globals::status = nullptr;
void *Globals::items = nullptr;
FModSound *Globals::sound = nullptr;
unsigned char Globals::options[100] = {};

struct PlayerEgo {
    union { int asteroidTarget; void *asteroidTargetPtr; };
    union { int miningGame; void *miningGamePtr; };
    union { int hud; void *hudPtr; };
    void *field_0x14 = nullptr;
    int field_0x1c = 0;
    bool exhaustVisible = false;
    bool docked = false;
    void setExhaustVisible(bool value) { exhaustVisible = value; }
    void dockToAsteroid(void *target, Radar *radar) {
        assert(target == nullptr && radar == (Radar *) field_0x14);
        docked = true;
    }
    void stopMining();
};

// PRODUCTION_METHOD

struct Fixture {
    Item prototypes[219];
    Item *entries[219];
    Array<Item *> items{entries};
    bool oreFlags[11]{};
    bool coreFlags[11]{};
    Array<bool> ores{oreFlags};
    Array<bool> cores{coreFlags};
    Ship ship;
    Status status;
    Hud hud;
    Player player;
    PlayerAsteroid asteroid;
    FModSound sound;
    Radar radar;
    PlayerEgo ego{};

    Fixture(int id, int space, int ore, bool won, bool core, bool hardcore = false) {
        for (int i = 0; i < 219; ++i) {
            prototypes[i].id = i;
            entries[i] = &prototypes[i];
        }
        ship.freeSpace = space;
        status.ship = &ship;
        status.field_94 = &ores;
        status.field_98 = &cores;
        asteroid.player = &player;
        asteroid.asteroidIndex = id;
        asteroid.hasCargo = 0x12345601;
        ego.asteroidTargetPtr = &asteroid;
        ego.miningGamePtr = new MiningGame{ore, won, core};
        ego.hudPtr = &hud;
        ego.field_0x14 = &radar;
        ego.field_0x1c = 42;
        Globals::status = &status;
        Globals::items = &items;
        Globals::sound = &sound;
        float difficulty = hardcore ? 1.5f : 0.5f;
        std::memcpy(Globals::options + 0x2c, &difficulty, sizeof(difficulty));
        assert(status.hardCoreMode() == hardcore);
    }

    void checkFinish() {
        assert(ego.miningGamePtr == nullptr);
        assert(ego.exhaustVisible && ego.docked);
        assert(asteroid.hasCargo == 0x12345600);
        assert(player.lastHitpoints == -1 && status.field_d8 == 0);
        assert(sound.stopped.size() == 2 && sound.stopped[0] == 1 && sound.stopped[1] == 3);
        assert(sound.resumed == 42);
    }
};

int main() {
    {
        Fixture f(154, 10, 4, true, false);
        f.ego.stopMining();
        assert(f.ship.cargo.size() == 1 && f.ship.cargo[0]->id == 154);
        assert(f.ship.cargo[0]->amount == 4 && f.status.field_a0 == 4 && f.oreFlags[0]);
        assert(f.hud.events.size() == 1 && f.hud.events[0].count == 4);
        assert(f.hud.events[0].extender && !f.hud.events[0].single);
        f.checkFinish();
    }
    {
        Fixture f(154, 3, 5, true, true);
        f.ego.stopMining();
        assert(f.ship.cargo.size() == 2);
        assert(f.ship.cargo[0]->id == 165 && f.ship.cargo[0]->amount == 1);
        assert(f.ship.cargo[1]->id == 154 && f.ship.cargo[1]->amount == 2);
        assert(f.status.field_a4 == 1 && f.status.field_a0 == 2);
        assert(f.coreFlags[0] && f.oreFlags[0]);
        assert(f.hud.events.size() == 3 && f.hud.events[2].single);
        assert(f.hud.events[0].id == 165 && f.hud.events[0].count == 1);
        assert(!f.hud.events[0].extender && !f.hud.events[0].slot);
        assert(f.hud.events[1].id == 154 && f.hud.events[1].count == 2);
        assert(f.hud.events[1].extender && !f.hud.events[1].single);
        assert(f.hud.events[2].count == 2 && f.ship.freeSpace == 0);
        f.checkFinish();
    }
    {
        Fixture f(154, 1, 5, true, true);
        f.ego.stopMining();
        assert(f.ship.cargo.size() == 1 && f.ship.cargo[0]->id == 165);
        assert(f.status.field_a4 == 1 && f.status.field_a0 == 0);
        assert(f.hud.events.size() == 2 && f.hud.events[1].single);
        assert(f.hud.events[1].count == 0 && f.ship.freeSpace == 0);
        f.checkFinish();
    }
    {
        Fixture f(154, 10, 5, false, false, true);
        f.ego.stopMining();
        assert(f.ship.cargo.size() == 1 && f.ship.cargo[0]->amount == 2);
        assert(f.status.field_a0 == 2);
        f.checkFinish();
    }
    {
        Fixture f(154, 0, 5, true, true);
        f.ego.stopMining();
        assert(f.ship.cargo.empty() && f.status.field_a0 == 0 && f.status.field_a4 == 0);
        assert(f.hud.events.size() == 1 && f.hud.events[0].single);
        assert(f.hud.events[0].count == 0 && f.hud.events[0].extender);
        f.checkFinish();
    }
    {
        Fixture f(217, 2, 1, true, true);
        f.ego.stopMining();
        assert(f.ship.cargo.size() == 2 && f.ship.cargo[0]->id == 218);
        assert(f.ship.cargo[1]->id == 217);
        assert(f.status.field_a4 == 1 && f.status.field_a0 == 1);
        for (bool flag : f.oreFlags) assert(!flag);
        for (bool flag : f.coreFlags) assert(!flag);
        f.checkFinish();
    }
    assert(destroyedMiningGames == 6);
    std::puts("PlayerEgo stopMining smoke: OK");
}
