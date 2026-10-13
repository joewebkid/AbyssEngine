#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

struct String {
    std::string data;
    String() = default;
    String(const char *s, bool) : data(s) {}
    void ctor_char(const char *s, bool) { data = s; }
    String &operator+=(const String &other) { data += other.data; return *this; }
};

String operator+(const String &a, const String &b) {
    String result;
    result.data = a.data + b.data;
    return result;
}

template <typename T> struct Array {
    std::vector<T> values;
    unsigned size() const { return static_cast<unsigned>(values.size()); }
    T operator[](unsigned index) const { return values[index]; }
};

struct Achievements {
    int medals[16] = {};
    int *getMedals() { return medals; }
};

struct BluePrint {
    unsigned char locked = 1;
    int productionCount = 1;
    int index = 0;
    int getIndex() { return index; }
};

struct Status {
    Array<bool> *field_94 = nullptr;
    Array<bool> *field_98 = nullptr;
    Array<bool> *field_ac = nullptr;
    Array<BluePrint *> *bluePrints = nullptr;
};

struct GameText {
    String value;
    String *getText(int id) {
        value.data = "T" + std::to_string(id);
        return &value;
    }
};

struct Globals {
    static void *achievements;
    static Status *status;
    static GameText *gameText;
};
void *Globals::achievements = nullptr;
Status *Globals::status = nullptr;
GameText *Globals::gameText = nullptr;

struct StatusWindow {
    String getMedalHintText(int medalIndex);
};

// PRODUCTION_MEDAL_HINT

int main() {
    Achievements achievements;
    Status status;
    GameText text;
    StatusWindow window;
    Globals::achievements = &achievements;
    Globals::status = &status;
    Globals::gameText = &text;

    assert(window.getMedalHintText(2).data.empty());
    achievements.medals[2] = 1;
    assert(window.getMedalHintText(2).data.empty());

    Array<bool> systems{{true, false, true}};
    Array<bool> stations{{false, true}};
    Array<bool> others{{false}};
    status.field_94 = &systems;
    status.field_98 = &stations;
    status.field_ac = &others;

    achievements.medals[2] = 2;
    assert(window.getMedalHintText(2).data == "\n\nT276\n- T1429");
    achievements.medals[3] = 2;
    assert(window.getMedalHintText(3).data == "\n\nT276\n- T1439");
    achievements.medals[9] = 2;
    assert(window.getMedalHintText(9).data == "\n\nT276\n- T1406");

    BluePrint blueprints[13];
    Array<BluePrint *> blueprintList;
    for (int i = 0; i < 13; ++i) {
        blueprints[i].index = i;
        blueprintList.values.push_back(&blueprints[i]);
    }
    status.bluePrints = &blueprintList;
    blueprints[7].locked = 0;
    achievements.medals[13] = 2;
    assert(window.getMedalHintText(13).data == "\n\nT276\n- T1281");
    blueprints[7].locked = 1;
    blueprints[4].productionCount = 0;
    achievements.medals[14] = 2;
    assert(window.getMedalHintText(14).data == "\n\nT276\n- T1278");

    std::puts("StatusWindow medal-hint host smoke: OK");
}
