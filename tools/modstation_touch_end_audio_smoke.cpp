#include <cassert>
#include <string>

struct Window {
    int result = 0;
    int calls = 0;
    int OnTouchEnd(int, int) { ++calls; return result; }
};
struct StatusWindow : Window {};
struct StarMap : Window {};
struct MissionsWindow : Window {};
struct FModSound {
    std::string *events = nullptr;
    int currentMusicEvent = 42;
    int calls = 0;
    int lastParam = -1;
    int lastEvent = -1;
    float lastValue = -1.0f;
    void setParamValue(int param, int event, float value) {
        events->push_back('A');
        ++calls;
        lastParam = param;
        lastEvent = event;
        lastValue = value;
    }
};
struct Globals { static FModSound *sound; };
FModSound *Globals::sound = nullptr;

struct ModStation {
    struct FlagWord { unsigned char bytes[4] = {}; };
    FlagWord modalFlags, subWindowFlags;
    void *statusWindow = nullptr;
    StarMap *starMap = nullptr;
    MissionsWindow *missionsWindow = nullptr;
    int lightResets = 0;
    std::string *events = nullptr;
    void resetLight() { ++lightResets; events->push_back('L'); }
    void testStatusStarMap(int x, int y);
    void testMissions(int x, int y);
};

// PRODUCTION_STATUS_STAR_MAP
// PRODUCTION_MISSIONS

int main() {
    std::string events;
    FModSound sound;
    sound.events = &events;
    Globals::sound = &sound;
    StatusWindow status;
    StarMap map;
    MissionsWindow missions;
    ModStation station;
    station.events = &events;
    station.statusWindow = &status;
    station.starMap = &map;
    station.missionsWindow = &missions;

    station.modalFlags.bytes[0] = 1;
    station.subWindowFlags.bytes[3] = 1;
    station.testStatusStarMap(3, 4);
    assert(status.calls == 1 && map.calls == 0);
    assert(station.modalFlags.bytes[0] == 1 && sound.calls == 0);

    status.result = 1;
    station.testStatusStarMap(5, 6);
    assert(station.modalFlags.bytes[0] == 0 && map.calls == 0);
    assert(sound.calls == 1 && sound.lastParam == 0 && sound.lastEvent == 42);
    assert(sound.lastValue == 0.0f && station.lightResets == 0);
    assert(events == "A");

    map.result = 1;
    station.testStatusStarMap(7, 8);
    assert(map.calls == 1 && station.subWindowFlags.bytes[3] == 0);
    assert(sound.calls == 2 && station.lightResets == 1);
    assert(events == "ALA");

    station.subWindowFlags.bytes[0] = 1;
    station.testMissions(9, 10);
    assert(missions.calls == 1 && station.subWindowFlags.bytes[0] == 1);
    assert(sound.calls == 2 && station.lightResets == 1);
    missions.result = 1;
    station.testMissions(11, 12);
    assert(station.subWindowFlags.bytes[0] == 0);
    assert(sound.calls == 3 && sound.lastEvent == 42 && sound.lastValue == 0.0f);
    assert(station.lightResets == 2);
    assert(events == "ALAAL");
}
