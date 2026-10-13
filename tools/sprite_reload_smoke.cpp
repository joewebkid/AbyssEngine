#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

struct PaintCanvas {
    static PaintCanvas *gCanvas;
    int creates = 0, simpleDraws = 0, scaledDraws = 0, regions = 0;
    unsigned lastImage = 0;
    int lastX = 0, lastY = 0, lastWidth = 0, lastHeight = 0;
    void Image2DCreate(unsigned short resource, unsigned &out) {
        out = resource + 100;
        ++creates;
    }
    int GetImage2DWidth(unsigned) { return 64; }
    int GetImage2DHeight(unsigned) { return 64; }
    void DrawImage2D(unsigned image, int x, int y) {
        ++simpleDraws;
        lastImage = image;
        lastX = x;
        lastY = y;
    }
    void DrawImage2D(unsigned image, int x, int y, int w, int h,
                     unsigned char a, unsigned char b, unsigned char c) {
        assert(a == 0x11 && b == 0x11 && c == 0);
        ++scaledDraws;
        lastImage = image;
        lastX = x;
        lastY = y;
        lastWidth = w;
        lastHeight = h;
    }
    void DrawRegion2D(unsigned image, int sx, int sy, int w, int h,
                      float angle, int, int, int x, int y) {
        assert(angle == 0.0f);
        ++regions;
        lastImage = image;
        lastX = x;
        lastY = y;
        lastWidth = w;
        lastHeight = h;
        assert(sx >= 0 && sy >= 0);
    }
};
PaintCanvas *PaintCanvas::gCanvas = nullptr;

struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct Sprite {
    uint32_t *frames;
    uint32_t image;
    int32_t refPixelX, refPixelY, posX, posY;
    int32_t frameWidth, frameHeight, imageWidth, imageHeight;
    int32_t frameSrcX, frameSrcY, columns, rows, currentFrame, frameCount;
    Sprite(uint32_t image, int frameWidth, int frameHeight);
    Sprite(uint32_t *frames, int frameCount, int frameWidth, int frameHeight);
    ~Sprite();
    void setFrame(int frame);
    void setPosition(int x, int y);
    void draw(float scaleX, float scaleY);
    void drawRegion(int srcX, int srcY, int w, int h);
};

struct ImageFactory {
    Sprite *sprite = nullptr;
    unsigned itemImage = 0, shipImage = 0;
    void reload();
};

// PRODUCTION_BODIES

int main() {
    PaintCanvas canvas, wrongCanvas;
    Globals::Canvas = &canvas;
    PaintCanvas::gCanvas = &wrongCanvas;

    ImageFactory factory;
    factory.reload();
    assert(factory.sprite && factory.sprite->frameCount == 6);
    assert(canvas.creates == 8 && wrongCanvas.creates == 0);
    assert(factory.itemImage == 0x485 + 100 && factory.shipImage == 0x511 + 100);
    factory.sprite->setFrame(5);
    factory.sprite->setPosition(100, 200);
    factory.sprite->draw(1.0f, 1.0f);
    assert(canvas.simpleDraws == 1 && canvas.lastImage == 0x4fc + 100);
    assert(canvas.lastX == 100 && canvas.lastY == 200);

    // ARM tests scaleX alone: scaleY==1 must not discard horizontal scaling.
    factory.sprite->draw(2.0f, 1.0f);
    assert(canvas.scaledDraws == 1 && canvas.lastWidth == 128 && canvas.lastHeight == 64);
    assert(canvas.lastX == 68 && canvas.lastY == 200);
    factory.sprite->draw(1.0f, 2.0f);
    assert(canvas.simpleDraws == 2 && canvas.scaledDraws == 1);
    factory.sprite->drawRegion(2, 3, 10, 11);
    assert(canvas.regions == 1 && canvas.lastImage == 0x4fc + 100);
    assert(canvas.lastX == 102 && canvas.lastY == 203);

    factory.reload();
    assert(canvas.creates == 16 && factory.sprite->currentFrame == 0);
    delete factory.sprite;
    factory.sprite = nullptr;

    Sprite sheet(200, 32, 32);
    assert(sheet.columns == 2 && sheet.rows == 2 && sheet.frameCount == 4);
    sheet.setFrame(3);
    sheet.setPosition(100, 200);
    sheet.draw(2.0f, 1.0f);
    assert(canvas.regions == 2 && canvas.lastImage == 200);
    assert(canvas.lastX == 100 && canvas.lastY == 200);
    sheet.drawRegion(2, 3, 10, 11);
    assert(canvas.regions == 3 && canvas.lastX == 102 && canvas.lastY == 203);
    assert(wrongCanvas.simpleDraws == 0 && wrongCanvas.scaledDraws == 0 && wrongCanvas.regions == 0);
    std::puts("Sprite reload/draw host smoke: OK");
}
