#include "engine/render/ImageFactory.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/ImagePart.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/Sprite.h"
#include "game/core/Globals.h"

namespace AbyssEngine {
    namespace AERandom {
        int nextInt(void *random, int limit);
    }
}

static unsigned *g_drawChar_canvas;
static unsigned *g_IF_drawShip_canvas;
static unsigned *g_drawItem_canvas;
static int *g_IF_idTable;
static unsigned *g_IF_drawItem4_canvas;
static char *g_ctor_flagA;
static char *g_ctor_flagB;
static int *g_ctor_dst;
static int *g_ctor_src;

static void *gCreateChar2Rng1;
static int gCreateChar2Table;
static void *gCreateChar2Rng2;
static void *gCreateCharRng;

static unsigned *g_IF_li_canvas;
static char *g_IF_flagA;
static char *g_IF_flagB;
static int *g_IF_posTableA;
static int *g_IF_posTableB;
static int *g_IF_posTableC;
static char *g_IF_flagC;
static int *g_IF_posTableD;

int ImageFactory::getItemImageId(int itemId) {
    int base = 0xef0;
    if (itemId < 0xb0) base = 0x898;
    return base + itemId;
}

ImageFactory::~ImageFactory() {
    delete this->sprite;
    this->sprite = nullptr;
}

ImageFactory::ImageFactory() {
    this->sprite = nullptr;
    if ((*g_ctor_flagA | *g_ctor_flagB) != 0) {
        int *dst = g_ctor_dst;
        int *src = g_ctor_src;
        for (int r = 0; r != 0xd; ++r) {
            int *d = dst;
            int *s = src;
            for (int c = 0; c != 4; ++c) {
                for (int k = 0; k != 2; ++k)
                    d[k] = s[k];
                s += 2;
                d += 2;
            }
            dst += 8;
            src += 8;
        }
    }
    this->reload();
}

void ImageFactory::reload() {
    unsigned *ids = new unsigned[6];
    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
    canvas->Image2DCreate(0x4fa, ids[0]);
    canvas->Image2DCreate(0x4fb, ids[1]);
    canvas->Image2DCreate(0x4f7, ids[2]);
    canvas->Image2DCreate(0x4f8, ids[3]);
    canvas->Image2DCreate(0x4f9, ids[4]);
    canvas->Image2DCreate(0x4fc, ids[5]);

    delete this->sprite;
    this->sprite = nullptr;

    int w = canvas->GetImage2DWidth(ids[0]);
    int h = canvas->GetImage2DHeight(ids[0]);
    this->sprite = new Sprite(ids, 6, w, h);

    canvas->Image2DCreate(0x485, this->itemImage);
    canvas->Image2DCreate(0x511, this->shipImage);
}

void ImageFactory::drawChar(Array<ImagePart *> *parts, int x, int y, bool flag) {
    PaintCanvas *pc = (PaintCanvas *) (long) *g_drawChar_canvas;
    pc->SetColor(0xffffffffu);
    pc->DrawImage2D(this->itemImage, x, y);
    for (unsigned i = 0; i < parts->size(); ++i) {
        ImagePart *part = (*parts)[i];
        if (part != nullptr) part->draw(x, y, flag);
    }
    pc->DrawImage2D(this->shipImage, x, y);
}

void ImageFactory::drawShip(int shipId, int x, int y) {
    PaintCanvas *pc = (PaintCanvas *) (long) *g_IF_drawShip_canvas;
    unsigned classIcon = 0xffffffffu;
    pc->SetColor(0xffffffffu);
    this->sprite->setFrame(5);
    this->sprite->setPosition(x, y);
    this->sprite->draw(1.0f, 1.0f);
    pc->Image2DCreate((unsigned short) (shipId + 0x971), classIcon);
    pc->DrawImage2D(classIcon, x, y);
}

void ImageFactory::drawItem(int itemId, int x, int y) {
    PaintCanvas *pc = (PaintCanvas *) (long) *g_drawItem_canvas;
    unsigned icon = 0xffffffffu;
    pc->SetColor(0xffffffffu);
    int base = 0xef0;
    if (itemId < 0xb0) base = 0x898;
    pc->Image2DCreate((unsigned short) (base + itemId), icon);
    pc->DrawImage2D(icon, x, y);
}

void *ImageFactory::loadImage(int row, int col, int frameBase) {
    int id = g_IF_idTable[row * 4 + col];
    if (id < 0)
        return nullptr;

    unsigned image = 0;
    ((PaintCanvas *) (long) *g_IF_li_canvas)
            ->Image2DCreate((unsigned short) ((short) id + (short) frameBase), image);

    int *posBase;
    // Each (row,col) cell occupies two consecutive ints (px,py); rows have a
    // stride of 8 ints and columns a stride of 2 ints.
    int cell = row * 8 + col * 2;
    if (*g_IF_flagA != 0) {
        posBase = g_IF_posTableA;
    } else if (*g_IF_flagB != 0) {
        posBase = g_IF_posTableB;
    } else {
        posBase = g_IF_posTableC;
        if (*g_IF_flagC == 0)
            posBase = g_IF_posTableD;
    }

    int px = posBase[cell];
    int py = posBase[cell + 1];
    return new ImagePart(image, px, py);
}

void ImageFactory::drawItem(int itemId, int frame, int x, int y) {
    PaintCanvas *pc = (PaintCanvas *) (long) *g_IF_drawItem4_canvas;
    unsigned icon = 0xffffffffu;
    pc->SetColor(0xffffffffu);
    this->sprite->setFrame(frame);
    this->sprite->setPosition(x, y);
    this->sprite->draw(1.0f, 1.0f);
    int base = 0xef0;
    if (itemId < 0xb0) base = 0x898;
    pc->Image2DCreate((unsigned short) (base + itemId), icon);
    pc->DrawImage2D(icon, x, y);
}

Array<ImagePart *> *ImageFactory::loadChar(int *desc) {
    if (desc == nullptr) return nullptr;
    Array<ImagePart *> *parts = new Array<ImagePart *>();
    ArraySetLength<ImagePart *>(4, *parts);
    int race = *desc++;
    for (int col = 0; col != 4; ++col) {
        int frameBase = desc[col];
        if (frameBase != -1) {
            (*parts)[col] = (ImagePart *) this->loadImage(race, col, frameBase);
        }
    }
    ImagePart *tmp = (*parts)[0];
    (*parts)[0] = (*parts)[2];
    (*parts)[2] = tmp;
    return parts;
}

void ImageFactory::createChar(int race) {
    int sexRoll = AbyssEngine::AERandom::nextInt(*(void **) gCreateCharRng, 2);
    this->createChar(sexRoll == 0, race);
}

int *ImageFactory::createChar(bool isMale, int race) {
    if (race == 3) {
        int reroll = AbyssEngine::AERandom::nextInt(*(void **) gCreateChar2Rng1, 4);
        race = (reroll != 0) ? 2 : 0;
    }
    int row = race;
    int *table = &gCreateChar2Table;
    if (race == 0) row = 10;
    if (isMale) row = race;
    if (row == 5) row = 0;
    int *desc = new int[5];
    desc[0] = row;
    // gCreateChar2Table is a contiguous run of rows, each holding 4 part counts.
    int *partCounts = &table[row * 4];
    for (int i = 0; i != 4; ++i)
        desc[i + 1] = AbyssEngine::AERandom::nextInt(*(void **) gCreateChar2Rng2, partCounts[i]);
    return desc;
}
