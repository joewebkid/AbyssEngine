#include <cassert>
#include <cstdio>
#include <vector>

struct ImageCall { unsigned image; int x, y; unsigned char flagsA, flagsB; };
struct RegionCall {
    unsigned image;
    int srcX, srcY, width, height;
    float angle;
    int pivotX, pivotY, x, y;
};

struct PaintCanvas {
    std::vector<ImageCall> images;
    std::vector<RegionCall> regions;
    void DrawImage2D(unsigned image, int x, int y, unsigned char a, unsigned char b) {
        images.push_back({image, x, y, a, b});
    }
    void DrawRegion2D(unsigned image, int srcX, int srcY, int width, int height,
                      float angle, int pivotX, int pivotY, int x, int y) {
        regions.push_back({image, srcX, srcY, width, height, angle, pivotX, pivotY, x, y});
    }
};

struct Standing {
    float rates[2] = {0.5f, -0.25f};
    float getStandingRate(int index) { return rates[index]; }
};
struct Status {
    static Status *gStatus;
    Standing standing;
    Standing *getStanding() { return &standing; }
};
Status *Status::gStatus = nullptr;

struct Layout { int field_0x2d8 = 30; int field_0x1c = 12; };
struct StatusWindow {
    unsigned standingEmblemImage = 3;
    unsigned standingBarImage = 4;
    unsigned standingFrameImage = 5;
    int charImageHeight = 21;
    int standingBarWidth = 40;
    int standingBarHeight = 8;

    void drawStandingFixture(PaintCanvas *canvas, Layout *layout,
                             int y, int x0, int boxW, int pad) {
// PRODUCTION_STANDING
    }
};

int main() {
    PaintCanvas canvas;
    Layout layout;
    Status status;
    Status::gStatus = &status;
    StatusWindow window;
    window.drawStandingFixture(&canvas, &layout, 20, 10, 100, 5);

    assert(canvas.images.size() == 4 && canvas.regions.size() == 2);
    assert(canvas.images[0].image == 3 && canvas.images[0].x == 35 && canvas.images[0].y == 89);
    assert(canvas.images[0].flagsA == 0x11 && canvas.images[0].flagsB == 0x14);
    assert(canvas.images[1].image == 5 && canvas.images[1].x == 15 && canvas.images[1].y == 93);
    assert(canvas.images[1].flagsA == 0x11 && canvas.images[1].flagsB == 0x44);
    assert(canvas.images[2].image == 3 && canvas.images[2].x == 88 && canvas.images[2].y == 89);
    assert(canvas.images[3].image == 5 && canvas.images[3].x == 98 && canvas.images[3].y == 93);

    const RegionCall &left = canvas.regions[0];
    assert(left.image == 4 && left.srcX == 40 && left.srcY == 0);
    assert(left.width == -20 && left.height == 8 && left.angle == 0.0f);
    assert(left.pivotX == 0 && left.pivotY == 0 && left.x == 35 && left.y == 89);
    const RegionCall &right = canvas.regions[1];
    assert(right.image == 4 && right.width == 10 && right.height == 8);
    assert(right.angle == 0.0f && right.x == 88 && right.y == 89);
    std::puts("StatusWindow standing host smoke: OK");
}
