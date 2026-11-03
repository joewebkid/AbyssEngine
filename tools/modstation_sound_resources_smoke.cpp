#include <cassert>
#include <cstdint>
#include <string>
#include <vector>

struct String {
    std::string value;
    String() = default;
    String(const char *text) : value(text) {}
};
struct Station {
    int index = 5;
    int getIndex() { return index; }
    String getName() { return String("Orion"); }
};
struct Status {
    int32_t field_70 = 100;
    int32_t field_74 = 1;
    int64_t playingTime = (int64_t(1) << 32) + 100 + 30001;
    Station *station = nullptr;
    Station *getStation() { return station; }
    int64_t getPlayingTime() { return playingTime; }
};
struct Globals {
    static void *globals;
    static Status *status;
    int starts = 0;
    std::vector<int> sounds;
    void startNewSoundResourceList() { ++starts; sounds.clear(); }
    void addSoundResourceToList(int id) { sounds.push_back(id); }
};
void *Globals::globals = nullptr;
Status *Globals::status = nullptr;
struct Generator {
    static int constructions, trades;
    static Station *lastStation;
    Generator() { ++constructions; }
    void computerTradeGoods(Station *station) {
        ++trades; lastStation = station;
    }
};
int Generator::constructions = 0;
int Generator::trades = 0;
Station *Generator::lastStation = nullptr;
struct ModStation {
    int introCountdown = 0;
    struct Flags { unsigned char bytes[4] = {}; } dragFlags;
    String stationName;
    int finalNext = -1;
    void loadSoundResources();
};

// PRODUCTION_SOUND_RESOURCES

int main() {
    Globals globals;
    Station station;
    Status status;
    status.station = &station;
    Globals::globals = &globals;
    Globals::status = &status;
    ModStation mod;
    const std::vector<int> expected = {
        0x5f, 0x7a, 0x6c, 0x60, 0x61, 0x62, 0x63, 0x65,
        0x64, 0x66, 0x68, 0x69, 0x6a, 0x6b, 0x67, 0x7e
    };

    mod.loadSoundResources();
    assert(globals.starts == 1 && globals.sounds == expected);
    assert(mod.introCountdown == 50 && mod.dragFlags.bytes[1] == 1);
    assert(mod.stationName.value == "Orion" && mod.finalNext == 80);
    assert(Generator::constructions == 1 && Generator::trades == 1);
    assert(Generator::lastStation == &station);

    status.playingTime -= 1;
    mod.loadSoundResources();
    assert(globals.starts == 2 && globals.sounds == expected);
    assert(Generator::trades == 1);

    station.index = 0x6c;
    status.playingTime += 1;
    mod.loadSoundResources();
    assert(globals.starts == 3 && globals.sounds == expected);
    assert(Generator::trades == 1);
}
