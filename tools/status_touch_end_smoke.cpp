#include <cassert>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

struct String {
    std::string text;
    String() = default;
    String(const char *value) : text(value) {}
    String(const String &value, bool) : text(value.text) {}
    String(int value) : text(std::to_string(value)) {}
    String &operator+=(const String &other) { text += other.text; return *this; }
};
template <typename T> struct Array : std::vector<T> {};
int releasedLines = 0;
template <typename T> void ArrayReleaseClasses(Array<T> &items) {
    for (T item : items) delete item;
    items.clear();
    ++releasedLines;
}
struct TouchButton {
    bool tapped = false, alwaysPressed = false;
    int beginCalls = 0, moveCalls = 0;
    void OnTouchBegin(int, int) { ++beginCalls; }
    void OnTouchMove(int, int) { ++moveCalls; }
    int OnTouchEnd(int, int) { return tapped; }
    void setAlwaysPressed(bool value) { alwaysPressed = value; }
};
struct Layout {
    bool consumed = false, help = false;
    int field_0x4c = 6, helpId = -1;
    int field_0xc = 10, field_0x10 = 10;
    int beginCalls = 0, moveCalls = 0;
    void OnTouchBegin(int, int) { ++beginCalls; }
    void OnTouchMove(int, int) { ++moveCalls; }
    int OnTouchEnd(int, int) { return consumed; }
    int helpPressed() { return help; }
    void initHelpWindow(const String &title) { helpId = std::stoi(title.text.substr(1)); }
};
struct Achievements {
    static Achievements *gAchievements;
    int medals[2] = {0, 2};
    bool elite[2] = {false, false};
    int valueIndex = -1, valueCount = -1;
    int *getMedals() { return medals; }
    int isEliteMedal(int index) { return elite[index]; }
    int getValue(int index, int count) {
        valueIndex = index;
        valueCount = count;
        return count;
    }
};
Achievements *Achievements::gAchievements = nullptr;
struct GameText {
    std::map<int, String> values;
    String *getText(int id) {
        values[id].text = "T" + std::to_string(id) + "#";
        return &values[id];
    }
};
struct Status {
    String replaceHash(const String &text, const String &value) {
        String out = text;
        auto pos = out.text.find('#');
        if (pos != std::string::npos) out.text.replace(pos, 1, value.text);
        return out;
    }
};
struct Globals {
    static Layout *layout;
    static void *achievements;
    static GameText *gameText;
    static Status *status;
    static Globals *globals;
    static unsigned char iPad;
    static unsigned int font;
    static int h, mouse_wheel;
    int lastWidth = -1;
    void getLineArray(unsigned int, const String &text, int width,
                      Array<String *> *lines) {
        lastWidth = width;
        lines->push_back(new String(text));
    }
};
Layout *Globals::layout = nullptr;
void *Globals::achievements = nullptr;
GameText *Globals::gameText = nullptr;
Status *Globals::status = nullptr;
Globals *Globals::globals = nullptr;
unsigned char Globals::iPad = 0;
unsigned int Globals::font = 1;
int Globals::h = 480;
int Globals::mouse_wheel = 0;
struct StatusWindow {
    int scrollVelocity = 0, scrollOffset = 0, scrollTarget = 0;
    int lastTouchY = 0, touchStartY = 0;
    float scrollDamping = 0, scrollVelocityF = 0;
    unsigned char isDragging = 0;
    unsigned activeTab = 0;
    Array<TouchButton *> *tabButtons = nullptr, *medalButtons = nullptr;
    int contentHeight = 0, *tabHeights = nullptr, medalCount = 0;
    int selectedMedal = -1, boxWidth = 0;
    Array<String *> *detailLines = nullptr;
    int OnTouchMove(int x, int y);
    int OnTouchEnd(int x, int y);
    int OnTouchBegin(int x, int y);
    String getMedalHintText(int) { return String("+hint"); }
};

// PRODUCTION_TOUCH_MOVE

// PRODUCTION_TOUCH_END

// PRODUCTION_TOUCH_BEGIN

int main() {
    Layout layout;
    Achievements achievements;
    GameText text;
    Status status;
    Globals globals;
    Globals::layout = &layout;
    Globals::achievements = &achievements;
    Globals::gameText = &text;
    Globals::status = &status;
    Globals::globals = &globals;

    TouchButton tab0, tab1, medal0, medal1;
    Array<TouchButton *> tabs, medals;
    tabs.push_back(&tab0); tabs.push_back(&tab1);
    medals.push_back(&medal0); medals.push_back(&medal1);
    int heights[2] = {100, 200};
    StatusWindow window;
    window.tabButtons = &tabs;
    window.medalButtons = &medals;
    window.tabHeights = heights;
    window.medalCount = 2;
    window.boxWidth = 120;
    window.scrollVelocity = -4;
    window.scrollOffset = 10;
    window.isDragging = 1;

    layout.consumed = true;
    assert(window.OnTouchEnd(1, 2) == 1);
    assert(window.scrollOffset == 6 && window.scrollTarget == 6);
    assert(window.scrollVelocityF == -4 && window.scrollDamping == 0.9f);
    assert(window.isDragging == 0 && window.activeTab == 0);

    layout.consumed = false;
    tab1.tapped = true;
    assert(window.OnTouchEnd(1, 2) == 0);
    assert(window.activeTab == 1 && window.contentHeight == 200);
    assert(window.scrollOffset == 0);
    tab1.tapped = false;

    medal1.tapped = true;
    assert(window.OnTouchEnd(1, 2) == 0);
    assert(window.selectedMedal == 1 && medal1.alwaysPressed);
    assert(achievements.valueIndex == 1 && achievements.valueCount == 2);
    assert(window.detailLines->size() == 1);
    assert((*window.detailLines)[0]->text == "T15532+hint");
    assert(globals.lastWidth == 108);
    medal1.tapped = false;

    medal0.tapped = true;
    window.OnTouchEnd(1, 2);
    assert(window.selectedMedal == 1 && releasedLines == 0);
    achievements.elite[0] = true;
    window.OnTouchEnd(1, 2);
    assert(window.selectedMedal == 0 && medal0.alwaysPressed);
    assert(!medal1.alwaysPressed && releasedLines == 1);
    assert(achievements.valueIndex == 0 && achievements.valueCount == 1);
    medal0.tapped = false;

    layout.help = true;
    window.OnTouchEnd(1, 2);
    assert(layout.helpId == 647);
    Globals::iPad = 1;
    window.activeTab = 0;
    tab1.tapped = true;
    window.OnTouchEnd(1, 2);
    assert(window.activeTab == 0 && layout.helpId == 640);

    Globals::iPad = 0;
    layout.help = false;
    window.activeTab = 1;
    achievements.elite[0] = false;
    int oldOffset = window.scrollOffset;
    window.OnTouchBegin(10, 20);
    assert(window.touchStartY == 20 && window.lastTouchY == 20);
    assert(window.scrollVelocity == 0 && window.isDragging == 1);
    assert(layout.beginCalls == 1 && tab0.beginCalls == 1);
    assert(tab1.beginCalls == 1 && medal0.beginCalls == 0);
    assert(medal1.beginCalls == 1);
    window.OnTouchMove(10, 25);
    assert(window.scrollOffset == oldOffset + 5 && window.scrollVelocity == 5);
    assert(window.selectedMedal == -1 && !medal0.alwaysPressed);
    assert(window.lastTouchY == 25 && layout.moveCalls == 1);
    assert(tab0.moveCalls == 1 && tab1.moveCalls == 1);
    assert(medal0.moveCalls == 0 && medal1.moveCalls == 1);

    Globals::iPad = 1;
    window.OnTouchBegin(10, 30);
    window.OnTouchMove(10, 35);
    assert(tab0.beginCalls == 1 && tab0.moveCalls == 1);
    assert(medal1.beginCalls == 2 && medal1.moveCalls == 2);

    ArrayReleaseClasses(*window.detailLines);
    delete window.detailLines;
    std::puts("StatusWindow touch-end host smoke: OK");
}
