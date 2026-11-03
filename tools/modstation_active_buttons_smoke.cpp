#include <cassert>
#include <string>
#include "engine/core/Array.h"

std::string events;
struct Vector { float x, y; };
struct TouchButton {
    float x, y;
    int reads = 0;
    Vector getPosition() { ++reads; return {x, y}; }
};
struct FModSound {
    int playId = -1, reverb = -1;
    bool downPitch = true;
    void *pos = reinterpret_cast<void *>(1), *vel = reinterpret_cast<void *>(1);
    float pitch = -1.0f;
    void play(int id, void *p, void *v, float f) {
        events.push_back('P'); playId = id; pos = p; vel = v; pitch = f;
    }
    void enableReverb(int value) { events.push_back('E'); reverb = value; }
    void setDownPitch(bool value) { events.push_back('D'); downPitch = value; }
};
struct Globals {
    static int sub_menu_buttons_x[10];
    static int sub_menu_buttons_y[10];
    static int sub_menu_button_count;
    static FModSound *sound;
    static void *globals;
    static int switch_to_target_setting;
    int musicMode = -1;
    void playMusicAndFadeOutCurrent(int mode) {
        events.push_back('M'); musicMode = mode;
    }
};
int Globals::sub_menu_buttons_x[10];
int Globals::sub_menu_buttons_y[10];
int Globals::sub_menu_button_count = -1;
FModSound *Globals::sound = nullptr;
void *Globals::globals = nullptr;
int Globals::switch_to_target_setting = -1;
struct ModStation {
    int *buttonRow = nullptr;
    int stationActive = 0, state = 0;
    void activate();
};

// PRODUCTION_ACTIVE_BUTTONS

int main() {
    Array<TouchButton *> row;
    ArraySetLength<TouchButton *>(12, row);
    TouchButton buttons[12];
    for (int i = 0; i < 12; ++i) {
        buttons[i].x = static_cast<float>(i) + 0.9f;
        buttons[i].y = -static_cast<float>(i) - 0.9f;
        row[i] = &buttons[i];
    }
    ModStation station;
    FModSound sound;
    Globals globals;
    Globals::sound = &sound;
    Globals::globals = &globals;
    Globals::switch_to_target_setting = 3;
    station.buttonRow = reinterpret_cast<int *>(&row);
    station.activate();
    assert(events == "PEDM" && globals.musicMode == 3);
    assert(Globals::switch_to_target_setting == -1);
    assert(sound.playId == 0x7a && sound.pos == nullptr && sound.vel == nullptr);
    assert(sound.pitch == 0.0f && sound.reverb == 0 && !sound.downPitch);
    assert(station.stationActive == 1 && station.state == 100);
    assert(Globals::sub_menu_button_count == 12);
    for (int i = 0; i < 10; ++i) {
        assert(Globals::sub_menu_buttons_x[i] == i);
        assert(Globals::sub_menu_buttons_y[i] == -i);
        assert(buttons[i].reads == 2);
    }
    assert(buttons[10].reads == 0 && buttons[11].reads == 0);

    station.buttonRow = nullptr;
    Globals::sub_menu_button_count = 42;
    events.clear();
    station.activate();
    assert(events == "PED" && globals.musicMode == 3);
    assert(Globals::sub_menu_button_count == 42);
}
