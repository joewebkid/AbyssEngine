#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

static std::vector<std::string> events;

struct String {
    int value = 1;
    String() { events.push_back("name-construct"); }
    String &operator=(const String &other) {
        value = other.value == 1 ? 0 : other.value;
        events.push_back("name-assign");
        return *this;
    }
    ~String() { events.push_back("name-destroy"); }
};
template<class T> struct Array {
    static int destroyed;
    ~Array() { ++destroyed; events.push_back("loot-delete"); }
};
template<class T> int Array<T>::destroyed = 0;
struct Player {
    bool turnedEnemyFlag = false, active = false;
    int turnedCalls = 0, resetCalls = 0, turnCalls = 0, activeCalls = 0;
    unsigned char turnedEnemy() {
        ++turnedCalls; events.push_back("turned-query"); return turnedEnemyFlag;
    }
    void reset() {
        ++resetCalls; turnedEnemyFlag = false; events.push_back("player-reset");
    }
    void turnEnemy() {
        ++turnCalls; turnedEnemyFlag = true; events.push_back("turn-enemy");
    }
    int getHitpoints() { events.push_back("get-hitpoints"); return 87; }
    void setActive(bool value) {
        ++activeCalls; active = value; events.push_back("player-active");
    }
};
struct AEGeometry {
    bool visible = false;
    int visibilityCalls = 0;
    void setVisible(bool value) {
        visible = value; ++visibilityCalls; events.push_back("geometry-visible");
    }
};
struct Route {
    int resetCalls = 0;
    void reset() { ++resetCalls; events.push_back("route-reset"); }
};
struct Explosion {
    int resetCalls = 0;
    void reset() { ++resetCalls; events.push_back("explosion-reset"); }
};
struct Generator {
    static int created, destroyed, lootCalls;
    Generator() { ++created; events.push_back("generator-construct"); }
    ~Generator() { ++destroyed; events.push_back("generator-destroy"); }
    Array<int> *getLootList(int item, int amount) {
        assert(item == -1 && amount == -1);
        ++lootCalls; events.push_back("get-loot"); return new Array<int>;
    }
};
int Generator::created = 0, Generator::destroyed = 0, Generator::lootCalls = 0;

struct KIPlayer {
    Player *player = nullptr;
    String name;
    AEGeometry *wreckGeometry = nullptr, *geometry = nullptr, *subGeometry = nullptr;
    Route *routeClone = nullptr;
    int state = 4, field_0x12e = 1, field_0x38 = 4, hitpoints = 1;
    int field_0x1dc = 9, field_0x1e0 = 1, deathTimer = 9, field_0xf0 = 9;
    int pushTimer = 8, field_0x24 = 1, field_0xf5 = 0, shipGroup = 9;
    float currentSpeed = 0.0f, speed = 2.5f;
    Explosion *explosion = nullptr;
    Array<int> *lootList = nullptr;
    int activeCalls = 0;
    void setActive(bool value) {
        ++activeCalls; events.push_back("ki-active"); player->setActive(value);
    }
};
struct PlayerFighter : KIPlayer {
    bool exhaustVisible = false;
    int exhaustCalls = 0;
    void setExhaustVisible(bool value) {
        exhaustVisible = value; ++exhaustCalls; events.push_back("exhaust-visible");
    }
    void revive();
};

// PRODUCTION_METHOD

static size_t eventIndex(const char *name) {
    auto it = std::find(events.begin(), events.end(), name);
    assert(it != events.end());
    return static_cast<size_t>(it - events.begin());
}

int main() {
    Player player;
    player.turnedEnemyFlag = true;
    AEGeometry body, sub, wreck;
    Route route;
    Explosion explosion;
    PlayerFighter fighter;
    fighter.player = &player;
    fighter.geometry = &body;
    fighter.subGeometry = &sub;
    fighter.wreckGeometry = &wreck;
    fighter.routeClone = &route;
    fighter.explosion = &explosion;
    fighter.lootList = new Array<int>;
    events.clear();

    fighter.revive();
    assert(player.turnedCalls == 1 && player.resetCalls == 1 && player.turnCalls == 1);
    assert(player.turnedEnemyFlag && player.active && player.activeCalls == 1);
    assert(fighter.name.value == 0 && fighter.wreckGeometry == nullptr);
    assert(fighter.state == 1 && fighter.field_0x12e == 0 && fighter.field_0x38 == -1);
    assert(route.resetCalls == 1 && fighter.hitpoints == 87);
    assert(fighter.field_0x1dc == 0 && fighter.field_0x1e0 == 0 && fighter.deathTimer == 0);
    assert(fighter.activeCalls == 1 && fighter.field_0xf0 == 0);
    assert(fighter.currentSpeed == 2.5f && explosion.resetCalls == 1);
    assert(fighter.pushTimer == 0 && fighter.field_0x24 == 0 && fighter.field_0xf5 == 1);
    assert(fighter.exhaustVisible && fighter.exhaustCalls == 1);
    assert(sub.visible && sub.visibilityCalls == 1 && body.visibilityCalls == 0);
    assert(fighter.lootList == nullptr && Array<int>::destroyed == 1);
    assert(Generator::created == 0);
    assert(eventIndex("turned-query") < eventIndex("player-reset"));
    assert(eventIndex("player-reset") < eventIndex("turn-enemy"));
    assert(eventIndex("turn-enemy") < eventIndex("name-assign"));
    assert(eventIndex("name-assign") < eventIndex("route-reset"));
    assert(eventIndex("route-reset") < eventIndex("ki-active"));
    assert(eventIndex("ki-active") < eventIndex("explosion-reset"));

    fighter.shipGroup = 8;
    fighter.subGeometry = nullptr;
    events.clear();
    fighter.revive();
    assert(player.turnedCalls == 2 && player.turnCalls == 2);
    assert(Generator::created == 1 && Generator::destroyed == 1 && Generator::lootCalls == 1);
    assert(fighter.lootList != nullptr && body.visible && body.visibilityCalls == 1);
    assert(eventIndex("generator-construct") < eventIndex("get-loot"));
    assert(eventIndex("get-loot") < eventIndex("generator-destroy"));

    player.turnedEnemyFlag = false;
    fighter.revive();
    assert(player.turnedCalls == 3 && player.turnCalls == 2 && !player.turnedEnemyFlag);
    assert(Array<int>::destroyed == 2 && Generator::lootCalls == 2);
    delete fighter.lootList;
}
