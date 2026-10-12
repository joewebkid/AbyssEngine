#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

struct String {
    std::string value;
    explicit String(int number) : value(std::to_string(number)) {}
};

struct RegionCall {
    unsigned image;
    int srcX, srcY, width, height;
    float angle;
    int pivotX, pivotY, x, y;
};
struct TextCall { unsigned font; std::string text; int x, y; bool centered; };

class PaintCanvas {
public:
    std::vector<unsigned> colors;
    std::vector<RegionCall> regions;
    std::vector<TextCall> texts;
    int heightQueries = 0;

    void SetColor(unsigned color) { colors.push_back(color); }
    int GetImage2DWidth(unsigned image) { assert(image == 9); return 40; }
    int GetImage2DHeight(unsigned image) { assert(image == 9); ++heightQueries; return 20; }
    void DrawRegion2D(unsigned image, int srcX, int srcY, int width, int height,
                      float angle, int pivotX, int pivotY, int x, int y) {
        regions.push_back({image, srcX, srcY, width, height, angle, pivotX, pivotY, x, y});
    }
    int GetTextWidth(unsigned font, const String &text) {
        assert(font == 7 && text.value == "50");
        return 8;
    }
    void DrawString(unsigned font, const String &text, int x, int y, bool centered) {
        texts.push_back({font, text.value, x, y, centered});
    }
};

struct Globals {
    static PaintCanvas *Canvas;
    static unsigned font;
};
PaintCanvas *Globals::Canvas = nullptr;
unsigned Globals::font = 7;

struct Position { float x, y, z; };
struct PlayerEgo {
    int throttleStarted = 0;
    int throttle = 0;
    int hpGaugeImage = 9;
    float thrust = 0.0f;
    static Position crosshairPos;
    void drawThrottle();
};
Position PlayerEgo::crosshairPos = {100.0f, 50.0f, 0.0f};

static const float g_PE_t_timerDiv = 500.0f;
static const float g_PE_t_pctScale = 100.0f;
static const float g_PE_t_textDiv = 1.6f;

// PRODUCTION_METHOD

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    PlayerEgo ego;
    ego.drawThrottle();
    assert(canvas.colors.empty() && canvas.regions.empty() && canvas.texts.empty());

    ego.throttleStarted = 1;
    ego.throttle = 250;
    ego.thrust = 0.5f;
    ego.drawThrottle();
    assert(canvas.colors.size() == 2 && canvas.colors[0] == 0xffffff7f);
    assert(canvas.colors[1] == 0xffffffff);
    assert(canvas.heightQueries == 2 && canvas.regions.size() == 1);
    const RegionCall &bar = canvas.regions[0];
    assert(bar.image == 9 && bar.srcX == 0 && bar.srcY == 10);
    assert(bar.width == 40 && bar.height == 10 && bar.angle == 0.0f);
    assert(bar.pivotX == 0 && bar.pivotY == 0 && bar.x == 80 && bar.y == 60);
    assert(canvas.texts.size() == 1 && canvas.texts[0].font == 7);
    assert(canvas.texts[0].text == "50" && canvas.texts[0].x == 95);
    assert(canvas.texts[0].y == 62 && !canvas.texts[0].centered);
    std::puts("PlayerEgo throttle host smoke: OK");
}
