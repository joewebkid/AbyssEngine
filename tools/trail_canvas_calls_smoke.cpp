#include <cassert>
#include <cstdint>

struct PaintCanvas {
    int drawCalls = 0;
    int colorCalls = 0;
    unsigned lastTransform = 0xffffffffu;
    const void *lastView = reinterpret_cast<void *>(1);
    uint32_t lastColor = 0;

    void DrawTransform(unsigned transform, const void *view) {
        ++drawCalls;
        lastTransform = transform;
        lastView = view;
    }
    void TransformSetColor(unsigned transform, unsigned color) {
        ++colorCalls;
        lastTransform = transform;
        lastColor = color;
    }
};

struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct Trail {
    unsigned transformId = 19;
    void render();
    void changeType(int type);
};

// PRODUCTION_METHODS

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    Trail trail;
    trail.render();
    assert(canvas.drawCalls == 1 && canvas.lastTransform == 19 &&
           canvas.lastView == nullptr);

    const struct { int type; uint32_t color; } cases[] = {
        {0, 0xffffffffu}, {1, 0xff0000ffu}, {2, 0x00ff00ffu},
        {3, 0xffff00ffu}, {4, 0xffffffffu}, {5, 0x00ff0000u},
        {6, 0xffff00ffu}, {7, 0xff0000ffu}, {8, 0xff4000ffu},
        {9, 0x00ff00ffu}, {10, 0xffff00ffu}, {11, 0xff0000ffu},
        {12, 0xffffffffu}
    };
    for (const auto &caseValue : cases) {
        trail.changeType(caseValue.type);
        assert(canvas.lastTransform == 19 && canvas.lastColor == caseValue.color);
    }
    assert(canvas.colorCalls == 13);

    PaintCanvas replacement;
    Globals::Canvas = &replacement;
    trail.render();
    trail.changeType(1);
    assert(replacement.drawCalls == 1 && replacement.colorCalls == 1 &&
           replacement.lastTransform == 19 && replacement.lastColor == 0xff0000ffu);
    assert(canvas.drawCalls == 1 && canvas.colorCalls == 13);
}
