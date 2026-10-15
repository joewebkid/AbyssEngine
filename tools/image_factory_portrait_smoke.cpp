#include "engine/core/Array.h"
#include "engine/render/ImageFactoryPortraitData.h"
#include <cassert>
#include <cstdio>
#include <vector>

struct PaintCanvas {
    std::vector<unsigned> drawIds;
    unsigned lastResource = 0;
    int detailedY = 0;
    unsigned detailedFlags = 0;
    int creates = 0;
    void Image2DCreate(unsigned short resource, unsigned &out) {
        lastResource = resource;
        out = resource + 10000;
        ++creates;
    }
    int GetImage2DWidth(unsigned) { return 17; }
    int GetImage2DHeight(unsigned) { return 29; }
    void SetColor(unsigned color) { assert(color == 0xffffffffu); }
    void DrawImage2D(unsigned id, int, int) { drawIds.push_back(id); }
    void DrawImage2D(unsigned id, int, int y, int, int,
                     unsigned char a, unsigned char b, unsigned char c) {
        drawIds.push_back(id);
        detailedY = y;
        detailedFlags = (a << 16) | (b << 8) | c;
    }
};

struct Globals {
    static PaintCanvas *Canvas;
    static void *rnd;
    static unsigned char retinaDisplay, n9, iPadHD, iPadLarge, iPad;
};
PaintCanvas *Globals::Canvas = nullptr;
void *Globals::rnd = nullptr;
unsigned char Globals::retinaDisplay = 0;
unsigned char Globals::n9 = 0;
unsigned char Globals::iPadHD = 0;
unsigned char Globals::iPadLarge = 0;
unsigned char Globals::iPad = 0;

namespace AbyssEngine {
struct AERandom {
    int nextInt(int limit) {
        assert(this == Globals::rnd && limit >= 0);
        if (limit == 2) return 0;
        if (limit == 4) return 1;
        return limit ? limit - 1 : 0;
    }
};
}

struct ImagePart {
    int id, f_4, pos_y, scale_x, scale_y;
    ImagePart(unsigned id, int field04, int posY);
    void draw(int x, int y, bool b);
};

struct Sprite {
    int frame = -1, x = 0, y = 0, draws = 0;
    void setFrame(int value) { frame = value; }
    void setPosition(int nx, int ny) { x = nx; y = ny; }
    void draw(float sx, float sy) {
        assert(sx == 1.0f && sy == 1.0f);
        ++draws;
    }
};

static int reloads = 0;
struct ImageFactory {
    Sprite *sprite;
    unsigned itemImage = 0, shipImage = 0;
    ImageFactory();
    void reload() { ++reloads; }
    int *createChar(int race);
    int *createChar(bool isMale, int race);
    void *loadImage(int row, int col, int frameBase);
    Array<ImagePart *> *loadChar(int *desc);
    void drawChar(Array<ImagePart *> *parts, int x, int y, bool flag);
    void drawShip(int shipId, int x, int y);
    void drawItem(int itemId, int x, int y);
    void drawItem(int itemId, int frame, int x, int y);
};

// PRODUCTION_BODIES

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    ImageFactory factory;
    assert(factory.sprite == nullptr && reloads == 1);

    ImagePart *part = static_cast<ImagePart *>(factory.loadImage(0, 0, 2));
    assert(part && part->id == 15002 && canvas.lastResource == 5002);
    assert(part->f_4 == 16 && part->pos_y == 45);
    assert(part->scale_x == 17 && part->scale_y == 29);
    int before = canvas.creates;
    assert(factory.loadImage(3, 0, 0) == nullptr && canvas.creates == before);
    AbyssEngine::AERandom random;
    Globals::rnd = &random;
    int *male = factory.createChar(true, 0);
    assert(male[0] == 0 && male[1] == 10 && male[4] == 10);
    delete[] male;
    int *female = factory.createChar(false, 0);
    assert(female[0] == 10 && female[1] == 1 && female[4] == 6);
    delete[] female;
    int *rerolled = factory.createChar(true, 3);
    assert(rerolled[0] == 2 && rerolled[1] == 4);
    delete[] rerolled;
    int *randomSex = factory.createChar(0);
    assert(randomSex[0] == 0 && randomSex[1] == 10);
    delete[] randomSex;
    int description[5] = {0, 2, 0, -1, 0};
    Array<ImagePart *> *loaded = factory.loadChar(description);
    assert(loaded->size_ == 4 && (*loaded)[0] == nullptr);
    assert((*loaded)[2] && (*loaded)[2]->id == 15002);
    assert((*loaded)[1] && (*loaded)[3]);
    for (unsigned i = 0; i < loaded->size_; ++i) delete (*loaded)[i];
    delete loaded;

    Array<ImagePart *> parts;
    parts.data_[0] = part;
    parts.size_ = 1;
    factory.itemImage = 41;
    factory.shipImage = 42;
    factory.drawChar(&parts, 10, 20, true);
    assert((canvas.drawIds == std::vector<unsigned>{41, 15002, 42}));
    assert(canvas.detailedY == 65 && canvas.detailedFlags == 0x111101);
    delete part;

    Sprite sprite;
    factory.sprite = &sprite;
    factory.drawShip(2, 30, 40);
    assert(sprite.frame == 5 && sprite.x == 30 && sprite.y == 40);
    assert(canvas.lastResource == 0x973 && canvas.drawIds.back() == 0x973 + 10000);
    factory.drawItem(3, 30, 40);
    assert(canvas.lastResource == 0x89b && canvas.drawIds.back() == 0x89b + 10000);
    factory.drawItem(0xb0, 4, 50, 60);
    assert(sprite.frame == 4 && sprite.x == 50 && sprite.y == 60 && sprite.draws == 2);
    assert(canvas.lastResource == 0xfa0 && canvas.drawIds.back() == 0xfa0 + 10000);

    Globals::iPad = 1;
    ImagePart *ipad = static_cast<ImagePart *>(factory.loadImage(0, 1, 0));
    assert(ipad->pos_y == 43);
    delete ipad;
    Globals::iPadLarge = 1;
    ImagePart *large = static_cast<ImagePart *>(factory.loadImage(0, 1, 0));
    assert(large->pos_y == 68);
    delete large;
    Globals::iPadHD = 1;
    ImagePart *hd = static_cast<ImagePart *>(factory.loadImage(0, 1, 0));
    assert(hd->pos_y == 48);
    delete hd;

    Globals::iPadHD = Globals::iPadLarge = Globals::iPad = 0;
    Globals::retinaDisplay = 1;
    ImageFactory retina;
    ImagePart *retinaPart = static_cast<ImagePart *>(retina.loadImage(1, 0, 0));
    assert(reloads == 2 && retinaPart->pos_y == 86);
    delete retinaPart;
    std::puts("ImageFactory portrait load/draw smoke: OK");
}
