#include <cassert>

struct TouchButton {
    unsigned result = 0;
    int calls = 0;
    int lastX = -1, lastY = -1;
    unsigned OnTouchEnd(int x, int y) {
        ++calls;
        lastX = x;
        lastY = y;
        return result;
    }
};
template<class T> struct Array {
    unsigned count;
    T *data;
    T &operator[](unsigned i) { return data[i]; }
};
struct RecordHandler {
    int saves = 0;
    void saveOptions() { ++saves; }
};
struct Globals {
    static unsigned char options[100];
    static void *recordHandler;
};
unsigned char Globals::options[100];
void *Globals::recordHandler = nullptr;
struct HangarWindow {
    static int constructions;
    int initializations = 0;
    int creditsShows = 0;
    HangarWindow() { ++constructions; }
    void initialize() { ++initializations; }
    void showCreditsBuyWindow() { ++creditsShows; }
};
int HangarWindow::constructions = 0;
struct ModStation {
    void *buttonLaunch = nullptr;
    void *buttonCredits = nullptr;
    int *buttonRow = nullptr;
    void *hangarWindow = nullptr;
    char closeHangarAfterCredits = 0;
    struct Flags { unsigned char bytes[4] = {}; } subWindowFlags;
    int selectedButton = 9;
    int departures = 0;
    int keyPresses = 0;
    long long lastKey = -1, lastMod = -1;
    void leaveStation() { ++departures; }
    void OnKeyPress(long long key, long long mod) {
        ++keyPresses;
        lastKey = key;
        lastMod = mod;
    }
    void testLaunch(int x, int y);
    void testCredits(int x, int y);
    void testRow(int x, int y);
};

// PRODUCTION_LAUNCH
// PRODUCTION_CREDITS
// PRODUCTION_ROW

int main() {
    TouchButton launch;
    TouchButton credits;
    TouchButton buttons[5];
    TouchButton *pointers[5];
    for (int i = 0; i < 5; ++i) pointers[i] = &buttons[i];
    Array<TouchButton *> row{5, pointers};
    ModStation station;
    station.buttonLaunch = &launch;
    station.buttonCredits = &credits;
    station.buttonRow = reinterpret_cast<int *>(&row);
    RecordHandler records;
    Globals::recordHandler = &records;

    station.testLaunch(17, 23);
    assert(launch.calls == 1 && launch.lastX == 17 && launch.lastY == 23);
    assert(station.departures == 0);
    launch.result = 1;
    station.testLaunch(31, 41);
    assert(launch.calls == 2 && launch.lastX == 31 && launch.lastY == 41);
    assert(station.departures == 1);

    station.testCredits(37, 39);
    assert(credits.calls == 1 && credits.lastX == 37 && credits.lastY == 39);
    assert(records.saves == 0 && station.hangarWindow == nullptr);
    credits.result = 1;
    station.testCredits(41, 42);
    assert(credits.calls == 2 && credits.lastX == 41 && credits.lastY == 42);
    assert(Globals::options[0x4e] == 1 && records.saves == 1);
    assert(HangarWindow::constructions == 1 && station.hangarWindow != nullptr);
    auto *hangar = static_cast<HangarWindow *>(station.hangarWindow);
    assert(hangar->initializations == 1 && hangar->creditsShows == 1);
    assert(station.closeHangarAfterCredits == 1 && station.subWindowFlags.bytes[2] == 1);
    station.testCredits(45, 46);
    assert(records.saves == 2 && HangarWindow::constructions == 1);
    assert(hangar->initializations == 2 && hangar->creditsShows == 2);

    buttons[3].result = 1;
    station.testRow(43, 47);
    assert(station.selectedButton == 3 && station.keyPresses == 1);
    assert(station.lastKey == 0x10000 && station.lastMod == 0);
    for (int i = 0; i < 5; ++i) {
        assert(buttons[i].calls == (i <= 3 ? 1 : 0));
        if (i <= 3) assert(buttons[i].lastX == 43 && buttons[i].lastY == 47);
    }

    buttons[3].result = 0;
    station.testRow(53, 59);
    assert(station.selectedButton == -1 && station.keyPresses == 1);
    for (int i = 0; i < 5; ++i) assert(buttons[i].calls == (i <= 3 ? 2 : 1));

    buttons[0].result = 1;
    station.testRow(61, 67);
    assert(station.selectedButton == 0 && station.keyPresses == 2);
    assert(buttons[0].lastX == 61 && buttons[0].lastY == 67);
    assert(buttons[1].calls == 2 && buttons[4].calls == 1);
    delete hangar;
}
