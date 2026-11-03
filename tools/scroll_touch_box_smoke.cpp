#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace AbyssEngine {
struct String {
    std::string value;
    String(const char *text = "", bool = false) : value(text) {}
};
}
using AbyssEngine::String;

template <class T> struct Array {
    std::vector<T> values;
    std::size_t size() const { return values.size(); }
    T *data() { return values.data(); }
};
template <class T> void ArrayReleaseClasses(Array<T> &array) {
    for (T value : array.values) delete value;
    array.values.clear();
}
template <class T> void ArrayRemoveAll(Array<T> &array) { array.values.clear(); }
template <class T> void ArrayAdd(T value, Array<T> &array) { array.values.push_back(value); }

struct Layout {
    int field_0x4 = 20;
    int field_0x48 = 10;
};

struct Engine {
    static unsigned char enableReverseFlag;
};
unsigned char Engine::enableReverseFlag = 1;

struct PaintCanvas {
    struct Draw { std::string text; int x; int y; unsigned char reverseFlag; };
    std::vector<Draw> draws;
    int colors = 0;
    void SetColor(unsigned int color) { assert(color == ~0u); ++colors; }
    int GetTextHeight(unsigned int) { return 10; }
    int GetTextWidth(unsigned int, const String &text) { return static_cast<int>(text.value.size()) * 5; }
    void DrawString(unsigned int, const String &text, int x, int y, bool) {
        draws.push_back({text.value, x, y, Engine::enableReverseFlag});
    }
};

struct GameText {
    static int language;
    static int getLanguage() { return language; }
};
int GameText::language = 0;

struct Globals {
    static unsigned int font;
    static PaintCanvas *Canvas;
    static Layout *layout;
    static unsigned char retinaDisplay;
    static void *globals;
    std::vector<int> widths;
    void getLineArray(unsigned int, const String &, int width, Array<String *> *out) {
        widths.push_back(width);
        out->values.push_back(new String("one"));
        if (width > 90) out->values.push_back(new String("two"));
    }
};
unsigned int Globals::font = 7;
PaintCanvas *Globals::Canvas = nullptr;
Layout *Globals::layout = nullptr;
unsigned char Globals::retinaDisplay = 0;
void *Globals::globals = nullptr;

struct ScrollTouchBox {
    Array<String *> *lines;
    int x, y, width, height, textWidth, contentHeight, lastDelta;
    float damping, velocity;
    int touchStartY, lastTouchY;
    std::uint8_t dragging;
    int scrollOffset;
    std::uint8_t centered;
    String *font;
    ScrollTouchBox(int x, int y, int width, int height);
    void setText(AbyssEngine::String text, int font);
    void draw();
};

// PRODUCTION_CONSTRUCTOR
// PRODUCTION_SET_TEXT
// PRODUCTION_DRAW

int main() {
    Globals state;
    Layout layout;
    PaintCanvas canvas;
    Globals::globals = &state;
    Globals::layout = &layout;
    Globals::Canvas = &canvas;

    ScrollTouchBox box(10, 20, 100, 35);
    assert(reinterpret_cast<std::uintptr_t>(box.font) == 7);
    box.draw();
    assert(canvas.colors == 1 && canvas.draws.empty());
    assert(Engine::enableReverseFlag == 1);

    box.setText(String("sample"), 7);
    assert((state.widths == std::vector<int>{100, 90}));
    assert(box.width == 90 && box.contentHeight == 40);
    assert(box.lines->size() == 2);
    assert(box.lines->data()[1]->value == "\n");

    GameText::language = 0;
    box.draw();
    assert(canvas.draws.size() == 2);
    assert(canvas.draws[0].x == 10 && canvas.draws[0].y == 20);

    canvas.draws.clear();
    box.centered = 1;
    box.draw();
    assert(canvas.draws[0].x == 48);

    canvas.draws.clear();
    box.centered = 0;
    box.height = 24;
    GameText::language = 10;
    Globals::retinaDisplay = 0;
    box.draw();
    assert(canvas.draws.size() == 1);
    canvas.draws.clear();
    Globals::retinaDisplay = 1;
    box.draw();
    assert(canvas.draws.size() == 2);

    canvas.draws.clear();
    box.height = 35;
    GameText::language = 9;
    box.draw();
    assert(canvas.draws[0].x == 85);
    assert(canvas.draws[0].reverseFlag == 0);
    assert(Engine::enableReverseFlag == 1);

    ArrayReleaseClasses(*box.lines);
    delete box.lines;
}
