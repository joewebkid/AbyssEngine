#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

struct String { std::string text; };
template <typename T> struct Array : std::vector<T> {};
template <typename T> void ArraySetLength(unsigned count, Array<T> &array) {
    array.resize(count);
}
struct GameText {
    String value;
    String *getText(int id) {
        value.text = "T" + std::to_string(id);
        return &value;
    }
};
struct Layout {
    int field_0x38 = 5, field_0x84 = 30, field_0x1c = 16;
    int field_0x2d8 = 40, field_0x2c = 8, field_0x4 = 12;
    int field_0x10 = 4, field_0xc = 10, field_0x20 = 6, field_0x24 = 7;
    int helpCalls = 0;
    int getHelpButtonOffset() { ++helpCalls; return 100; }
};
struct TouchButton {
    std::string text;
    int type = -1, x = -1, y = -1, medalIndex = -1, medalValue = -1;
    unsigned char flags = 0;
    bool alwaysPressed = false;
    TouchButton(const String &label, int kind, int xpos, int ypos, unsigned char f)
        : text(label.text), type(kind), x(xpos), y(ypos), flags(f) {}
    TouchButton(int index, int value, const String &label, int xpos, int ypos,
                unsigned char f)
        : text(label.text), x(xpos), y(ypos), medalIndex(index), medalValue(value), flags(f) {}
    int getWidth() { return text == "T168" ? 50 : 80; }
    void setAlwaysPressed(bool pressed) { alwaysPressed = pressed; }
};
struct Achievements {
    int medals[45];
    Achievements() {
        for (int i = 0; i < 45; ++i) medals[i] = i + 100;
    }
    int *getMedals() { return medals; }
};
struct PaintCanvas {
    std::vector<int> created;
    void Image2DCreate(unsigned short resource, unsigned &image) {
        created.push_back(resource);
        image = resource + 100;
    }
    int GetImage2DWidth(unsigned) { return 80; }
    int GetImage2DHeight(unsigned) { return 20; }
};
struct Globals {
    static Layout *layout;
    static GameText *gameText;
    static void *achievements;
    static PaintCanvas *Canvas;
    static int w, h;
    static unsigned char iPad;
};
Layout *Globals::layout = nullptr;
GameText *Globals::gameText = nullptr;
void *Globals::achievements = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
int Globals::w = 800;
int Globals::h = 480;
unsigned char Globals::iPad = 0;

struct StatusWindow {
    int medalCount = 0;
    Array<TouchButton *> *tabButtons = nullptr;
    Array<TouchButton *> *medalButtons = nullptr;
    void *imageParts = nullptr;
    void *detailLines = nullptr;
    unsigned rankImage0 = 0, rankImage1 = 0, rankImage2 = 0, rankImage3 = 0;
    unsigned standingEmblemImage = 0, standingBarImage = 0, standingFrameImage = 0;
    unsigned activeTab = 0;
    int selectedMedal = 0, scrollOffset = 0, lastTouchY = 0, scrollTarget = 0;
    int scrollVelocity = 0;
    float scrollDamping = 0, scrollVelocityF = 0;
    int touchStartY = 0;
    unsigned char isDragging = 0;
    int contentHeight = 0, viewportHeight = 0;
    int charImageWidth = 0, charImageHeight = 0;
    int *tabHeights = nullptr;
    int boxWidth = 0, standingBarWidth = 0, standingBarHeight = 0;
    int medalRowHeight = 0;
    StatusWindow();
    void reInit() { charImageHeight = 30; }
};

// PRODUCTION_CONSTRUCTOR

int main() {
    Layout layout;
    GameText text;
    Achievements achievements;
    PaintCanvas canvas;
    Globals::layout = &layout;
    Globals::gameText = &text;
    Globals::achievements = &achievements;
    Globals::Canvas = &canvas;

    StatusWindow phone;
    assert(phone.tabButtons->size() == 2 && layout.helpCalls == 2);
    TouchButton *first = (*phone.tabButtons)[1];
    TouchButton *second = (*phone.tabButtons)[0];
    assert(first->text == "T168" && first->x == 700 && first->flags == 0x12);
    assert(second->text == "T577" && second->x == 655 && second->flags == 0x12);
    assert(second->alwaysPressed && !first->alwaysPressed && phone.activeTab == 0);
    assert(phone.medalButtons->size() == 45 && phone.medalCount == 45);
    assert((*phone.medalButtons)[0]->text == "T1507");
    assert((*phone.medalButtons)[0]->medalValue == 100);
    assert((*phone.medalButtons)[44]->text == "T1551");
    assert((*phone.medalButtons)[44]->medalValue == 144);
    assert((*phone.medalButtons)[44]->flags == 0x44);
    assert(canvas.created.size() == 3 && canvas.created[0] == 0x48e);
    assert(canvas.created[1] == 0x48f && canvas.created[2] == 0x48d);
    assert(phone.standingBarWidth == 40 && phone.standingBarHeight == 20);
    assert(phone.tabHeights[0] == 266 && phone.tabHeights[1] == 460);
    assert(phone.contentHeight == 266 && phone.viewportHeight == 453);
    assert(phone.selectedMedal == -1 && phone.scrollOffset == 0);

    Globals::iPad = 1;
    StatusWindow tablet;
    assert(tablet.activeTab == 1 && (*tablet.tabButtons)[1]->alwaysPressed);
    assert(!(*tablet.tabButtons)[0]->alwaysPressed);
    assert(tablet.tabHeights[0] == 266 && tablet.tabHeights[1] == 484);
    assert(tablet.contentHeight == 484 && tablet.viewportHeight == 453);
    std::puts("StatusWindow constructor host smoke: OK");
}
