#include <cassert>
#include <string>

std::string events;
struct FModSound {
    int currentMusicEvent = 42;
    int param = -1, event = -1, stopId = -1, playId = -1;
    float value = -1.0f, pitch = -1.0f;
    void *playPos = reinterpret_cast<void *>(1);
    void *playVel = reinterpret_cast<void *>(1);
    void setParamValue(int p, int e, float v) {
        events.push_back('P');
        param = p; event = e; value = v;
    }
    void stop(int id) { events.push_back('S'); stopId = id; }
    void play(int id, void *pos, void *vel, float p) {
        events.push_back('R');
        playId = id; playPos = pos; playVel = vel; pitch = p;
    }
};
struct Globals { static FModSound *sound; };
FModSound *Globals::sound = nullptr;
struct CutScene {
    int turretChecks = 0;
    void checkForTurret() { events.push_back('T'); ++turretChecks; }
};
struct HangarWindow {
    static int destructions;
    ~HangarWindow() { events.push_back('D'); ++destructions; }
};
int HangarWindow::destructions = 0;
struct SpaceLounge {
    int touchResult = 0;
    int touchCalls = 0, updateChecks = 0;
    int lastX = -1, lastY = -1;
    bool updateHangar = false;
    int OnTouchEnd(int x, int y) {
        ++touchCalls; lastX = x; lastY = y; return touchResult;
    }
    unsigned char hangarNeedsUpdate() {
        events.push_back('H'); ++updateChecks; return updateHangar;
    }
};
struct ModStation {
    struct Flags { unsigned char bytes[4] = {}; } subWindowFlags;
    void *spaceLounge = nullptr;
    void *hangarWindow = nullptr;
    CutScene *cutScene = nullptr;
    int cameraResets = 0, lightResets = 0;
    void resetIdleCamForHangar() { events.push_back('C'); ++cameraResets; }
    void resetLight() { events.push_back('L'); ++lightResets; }
    void testLounge(int x, int y);
};

// PRODUCTION_LOUNGE_CLOSE

int main() {
    FModSound sound;
    Globals::sound = &sound;
    CutScene cutScene;
    SpaceLounge lounge;
    ModStation station;
    station.spaceLounge = &lounge;
    station.cutScene = &cutScene;
    station.hangarWindow = new HangarWindow();
    station.subWindowFlags.bytes[1] = 1;

    station.testLounge(17, 23);
    assert(lounge.touchCalls == 1 && lounge.lastX == 17 && lounge.lastY == 23);
    assert(station.subWindowFlags.bytes[1] == 1 && events.empty());

    lounge.touchResult = 1;
    station.testLounge(31, 41);
    assert(station.subWindowFlags.bytes[1] == 0);
    assert(events == "CLPSRTH");
    assert(station.cameraResets == 1 && station.lightResets == 1);
    assert(sound.param == 0 && sound.event == 42 && sound.value == 0.0f);
    assert(sound.stopId == 0x6c && sound.playId == 0x7a);
    assert(sound.playPos == nullptr && sound.playVel == nullptr && sound.pitch == 0.0f);
    assert(cutScene.turretChecks == 1 && lounge.updateChecks == 1);
    assert(station.hangarWindow != nullptr && HangarWindow::destructions == 0);

    lounge.updateHangar = true;
    station.subWindowFlags.bytes[1] = 1;
    events.clear();
    station.testLounge(43, 47);
    assert(lounge.touchCalls == 3 && lounge.lastX == 43 && lounge.lastY == 47);
    assert(events == "CLPSRTHD");
    assert(station.hangarWindow == nullptr && HangarWindow::destructions == 1);
    assert(station.cameraResets == 2 && station.lightResets == 2);
    assert(cutScene.turretChecks == 2 && lounge.updateChecks == 2);
}
