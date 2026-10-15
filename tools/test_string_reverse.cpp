#include "engine/core/GameText.h"
#include "game/core/String.h"

#include <cassert>
#include <cstdlib>

// String.cpp retains unrelated entry points in this standalone link.
void String_printImpl(const char *) { std::abort(); }
uint16_t *String_computeFloatString(float, int, int *, int *) { std::abort(); }
namespace AbyssEngine {
String operator+(const String &, const String &) { std::abort(); }
}

static int language = 9;

int GameText::getLanguage() { return language; }

int main() {
    const unsigned short text[] = {'a', 'b', 'c', 'd', 0};
    String normal(text, false);
    normal.Reverse();
    assert(normal.length == 4);
    assert(normal.data[0] == 'd' && normal.data[1] == 'c' &&
           normal.data[2] == 'b' && normal.data[3] == 'a');

    String otherLanguage(text, false);
    language = 8;
    otherLanguage.Reverse();
    assert(otherLanguage.data[0] == 'a' && otherLanguage.data[3] == 'd');

    String embeddedZero(text, false);
    embeddedZero.data[1] = 0;
    language = 9;
    embeddedZero.Reverse();
    assert(embeddedZero.length == 4);
    assert(embeddedZero.data[0] == 0 && embeddedZero.data[1] == 0 &&
           embeddedZero.data[2] == 0 && embeddedZero.data[3] == 'a');

    String empty;
    empty.Reverse();
    assert(empty.data == nullptr && empty.length == 0);
}
