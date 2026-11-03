#include <cassert>
#include <string>
#include "engine/core/Array.h"

struct String {
    std::string value;
    String(const char *s) : value(s) {}
};
struct Layout {
    int field_0xc = 20;
    int field_0x10 = 30;
    int field_0x24 = 7;
    int field_0x28 = 112;
    int field_0x30 = 42;
    int field_0x34 = 8;
    int field_0x4c = 7;
    int field_0xcc = 253;
    String formatCredits(int n) { return String(("C" + std::to_string(n)).c_str()); }
};
struct Status {
    int getCredits() { return 123; }
};
struct GameText {
    String label = String("");
    String *getText(int id) {
        label = String(std::to_string(id).c_str());
        return &label;
    }
};
struct TouchButton {
    std::string text;
    int kind, x, y, width;
    unsigned char flags0, flags1;
    TouchButton(const String &s, int kind_, int x_, int y_, unsigned char flags)
        : text(s.value), kind(kind_), x(x_), y(y_), width(77),
          flags0(flags), flags1(0) {}
    TouchButton(const String &s, int kind_, int x_, int y_, int width_,
                unsigned char f0, unsigned char f1)
        : text(s.value), kind(kind_), x(x_), y(y_), width(width_),
          flags0(f0), flags1(f1) {}
    int getWidth() { return width; }
};
struct Globals {
    static Layout *layout;
    static Status *status;
    static GameText *gameText;
    static int w, h;
};
Layout *Globals::layout = nullptr;
Status *Globals::status = nullptr;
GameText *Globals::gameText = nullptr;
int Globals::w = 800;
int Globals::h = 600;
struct ModStation {
    int hangarLeftX = 0;
    void *hangarShipGeom = reinterpret_cast<void *>(1);
    int field_0xa4 = 0, field_0xac = 0;
    void *buttonLaunch = nullptr, *buttonCredits = nullptr;
    int *buttonRow = nullptr;
    void buildButtons();
};

// PRODUCTION_BUTTON_CREATION

int main() {
    static_assert(sizeof(void *) == 8, "host pointer-width fixture");
    Layout layout;
    Status status;
    GameText text;
    Globals::layout = &layout;
    Globals::status = &status;
    Globals::gameText = &text;
    ModStation station;
    station.buildButtons();
    assert(station.hangarLeftX == 253 && station.hangarShipGeom == nullptr);
    assert(station.field_0xa4 == 20 && station.field_0xac == 550);

    auto *launch = static_cast<TouchButton *>(station.buttonLaunch);
    auto *credits = static_cast<TouchButton *>(station.buttonCredits);
    assert(launch->text.empty() && launch->kind == 12);
    assert(launch->x == 800 && launch->y == 600 && launch->flags0 == 0x22);
    assert(credits->text == "C123" && credits->kind == 11);
    assert(credits->x == 723 && credits->y == 600 && credits->flags0 == 0x22);

    auto *row = reinterpret_cast<Array<TouchButton *> *>(station.buttonRow);
    assert(row != nullptr && row->size() == 5);
    const int ids[5] = {167, 398, 177, 129, 169};
    for (int i = 0; i < 5; ++i) {
        TouchButton *button = (*row)[i];
        assert(button != nullptr && button->text == std::to_string(ids[i]));
        assert(button->kind == 0 && button->x == 112);
        assert(button->y == 321 + 50 * i && button->width == 134);
        assert(button->flags0 == 0x11 && button->flags1 == 1);
        delete button;
    }
    delete row;
    delete launch;
    delete credits;
}
