#include <cassert>
#include <string>
#include "engine/core/Array.h"

namespace AbyssEngine {
struct String {
    std::string value;
    static int copies;
    explicit String(const char *text) : value(text) {}
    String(const String &other) : value(other.value) { ++copies; }
    int Compare(const String &other) const { return value.compare(other.value); }
};
int String::copies = 0;
}
using AbyssEngine::String;

struct Status {
    Array<String *> *wingmen = nullptr;
    int cleared = 0;
    Array<String *> *getWingmen() { return wingmen; }
    void setWingmen(Array<String *> *list) {
        wingmen = list;
        ++cleared;
    }
};
struct Globals { static Status *status; };
Status *Globals::status = nullptr;
struct Level { void wingmanDied(const String &name); };

// PRODUCTION_METHOD

int main() {
    Status status;
    Globals::status = &status;
    Level level;
    String alpha("alpha"), beta("beta"), gamma("gamma"), absent("absent");

    level.wingmanDied(absent);
    assert(status.cleared == 0);

    Array<String *> single;
    ArrayAdd(&alpha, single);
    status.wingmen = &single;
    level.wingmanDied(absent);
    assert(status.wingmen == nullptr && status.cleared == 1);

    Array<String *> list;
    ArrayAdd(&alpha, list);
    ArrayAdd(&beta, list);
    ArrayAdd(&gamma, list);
    status.wingmen = &list;
    level.wingmanDied(absent);
    assert(list.size_ == 3 && list.capacity_ == 3 && status.cleared == 1);
    level.wingmanDied(beta);
    assert(list.size_ == 2 && list.capacity_ == 2);
    assert(list[0] == &alpha && list[1] == &gamma);
    assert(String::copies == 0 && status.cleared == 1);

    Array<String *> duplicates;
    ArrayAdd(&alpha, duplicates);
    ArrayAdd(&beta, duplicates);
    ArrayAdd(&gamma, duplicates);
    ArrayAdd(&beta, duplicates);
    status.wingmen = &duplicates;
    level.wingmanDied(beta);
    assert(duplicates.size_ == 2 && duplicates.capacity_ == 2);
    assert(duplicates[0] == &alpha && duplicates[1] == &gamma);
}
