#include <cassert>
#include <string>

std::string events;
struct Vector { float x, y; };
struct TouchButton {
    float x, y;
    int reads = 0;
    Vector getPosition() { events.push_back('X'); ++reads; return {x, y}; }
};
template<class T> struct Array {
    unsigned count;
    T *data;
    unsigned size() const { return count; }
    T &operator[](unsigned i) { return data[i]; }
};
struct Item {
    int index, amount;
    int getIndex() { return index; }
    int getAmount() { return amount; }
};
struct Ship {
    Array<Item *> *cargo = nullptr;
    int index = 37, race = 2;
    Array<Item *> *getCargo() { return cargo; }
    int getIndex() { return index; }
    int getRace() { return race; }
};
struct Status {
    Ship *ship = nullptr;
    int field_a8 = 10;
    Ship *getShip() { return ship; }
};
struct FModSound {
    int currentMusicEvent = 42;
    int stopId = -1, playId = -1, param = -1, event = -1;
    void *pos = reinterpret_cast<void *>(1), *vel = reinterpret_cast<void *>(1);
    float pitch = -1.0f, value = -1.0f;
    void stop(int id) { events.push_back('S'); stopId = id; }
    void play(int id, void *p, void *v, float f) {
        events.push_back('P'); playId = id; pos = p; vel = v; pitch = f;
    }
    void setParamValue(int p, int e, float v) {
        events.push_back('A'); param = p; event = e; value = v;
    }
};
struct Globals {
    static Status *status;
    static FModSound *sound;
    static int sub_menu_buttons_x[10], sub_menu_buttons_y[10];
};
Status *Globals::status = nullptr;
FModSound *Globals::sound = nullptr;
int Globals::sub_menu_buttons_x[10];
int Globals::sub_menu_buttons_y[10];
struct CutScene {
    int replacements = 0, checks = 0, lastIndex = -1, lastRace = -1;
    void replacePlayerShip(int index, int race) {
        events.push_back('R'); ++replacements; lastIndex = index; lastRace = race;
    }
    void checkForTurret() { events.push_back('T'); ++checks; }
};
struct HangarWindow {
    int touchResult = 0, touchCalls = 0, lastX = -1, lastY = -1;
    unsigned char field_0x0 = 0;
    int sellCalls = 0;
    bool lastSellMode = true;
    int OnTouchEnd(int x, int y) {
        ++touchCalls; lastX = x; lastY = y; return touchResult;
    }
    void setSellMode(bool buy) {
        events.push_back('M'); ++sellCalls; lastSellMode = buy;
    }
};
struct ModStation {
    struct Flags { unsigned char bytes[4] = {}; } subWindowFlags;
    void *hangarWindow = nullptr;
    CutScene *cutScene = nullptr;
    int *buttonRow = nullptr;
    int field_0xd0 = 0;
    int cameraResets = 0;
    void resetIdleCamForHangar() { events.push_back('C'); ++cameraResets; }
    void testHangarClose(int x, int y);
};

// PRODUCTION_HANGAR_CLOSE

int main() {
    Item items[5] = {{131, 99}, {132, 3}, {153, 4}, {154, 99}, {145, 2}};
    Item *itemPtrs[5] = {&items[0], &items[1], &items[2], &items[3], &items[4]};
    Array<Item *> cargo{5, itemPtrs};
    Ship ship;
    ship.cargo = &cargo;
    Status status;
    status.ship = &ship;
    FModSound sound;
    Globals::status = &status;
    Globals::sound = &sound;
    TouchButton buttons[12];
    TouchButton *buttonPtrs[12];
    for (int i = 0; i < 12; ++i) {
        buttons[i].x = static_cast<float>(i) + 0.75f;
        buttons[i].y = -static_cast<float>(i) - 0.75f;
        buttonPtrs[i] = &buttons[i];
    }
    Array<TouchButton *> row{12, buttonPtrs};
    HangarWindow hangar;
    CutScene scene;
    ModStation station;
    station.hangarWindow = &hangar;
    station.cutScene = &scene;
    station.buttonRow = reinterpret_cast<int *>(&row);
    station.field_0xd0 = 5;
    station.subWindowFlags.bytes[2] = 1;

    station.testHangarClose(17, 23);
    assert(hangar.touchCalls == 1 && hangar.lastX == 17 && hangar.lastY == 23);
    assert(station.subWindowFlags.bytes[2] == 1 && events.empty());

    hangar.touchResult = 1;
    station.testHangarClose(31, 41);
    assert(hangar.touchCalls == 2 && hangar.lastX == 31 && hangar.lastY == 41);
    assert(station.subWindowFlags.bytes[2] == 0);
    assert(status.field_a8 == 14 && hangar.sellCalls == 1 && !hangar.lastSellMode);
    assert(station.cameraResets == 1 && scene.replacements == 0 && scene.checks == 1);
    assert(events == "MCTSPA" + std::string(20, 'X'));
    assert(sound.stopId == 0x5f && sound.playId == 0x7a);
    assert(sound.pos == nullptr && sound.vel == nullptr && sound.pitch == 0.0f);
    assert(sound.param == 0 && sound.event == 42 && sound.value == 0.0f);
    for (int i = 0; i < 10; ++i) {
        assert(Globals::sub_menu_buttons_x[i] == i);
        assert(Globals::sub_menu_buttons_y[i] == -i);
        assert(buttons[i].reads == 2);
    }
    assert(buttons[10].reads == 0 && buttons[11].reads == 0);

    station.field_0xd0 = 12;
    station.subWindowFlags.bytes[2] = 1;
    hangar.field_0x0 = 1;
    events.clear();
    station.testHangarClose(43, 47);
    assert(status.field_a8 == 14 && hangar.field_0x0 == 0);
    assert(scene.replacements == 1 && scene.lastIndex == 37 && scene.lastRace == 2);
    assert(scene.checks == 2 && station.cameraResets == 2);
    assert(events == "MCRTSPA" + std::string(20, 'X'));
}
