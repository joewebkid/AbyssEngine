#include "engine/render/ImageFactory.h"
#include "engine/render/ImageFactoryPortraitData.h"
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
    if ((Globals::retinaDisplay | Globals::n9) != 0) {
        int *dst = &gImagePartOffsets[0][0][0];
        const int *src = &kImagePartOffsetsIpad[0][0][0];
        for (int r = 0; r != 13; ++r) {
            int *d = dst;
            const int *s = src;
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
    PaintCanvas *pc = Globals::Canvas;
    pc->SetColor(0xffffffffu);
    pc->DrawImage2D(this->itemImage, x, y);
    for (unsigned i = 0; i < parts->size(); ++i) {
        ImagePart *part = (*parts)[i];
        if (part != nullptr) part->draw(x, y, flag);
    }
    pc->DrawImage2D(this->shipImage, x, y);
}

void ImageFactory::drawShip(int shipId, int x, int y) {
    PaintCanvas *pc = Globals::Canvas;
    unsigned classIcon = 0xffffffffu;
    pc->SetColor(0xffffffffu);
    this->sprite->setFrame(5);
    this->sprite->setPosition(x, y);
    this->sprite->draw(1.0f, 1.0f);
    pc->Image2DCreate((unsigned short) (shipId + 0x971), classIcon);
    pc->DrawImage2D(classIcon, x, y);
}

void ImageFactory::drawItem(int itemId, int x, int y) {
    PaintCanvas *pc = Globals::Canvas;
    unsigned icon = 0xffffffffu;
    pc->SetColor(0xffffffffu);
    int base = 0xef0;
    if (itemId < 0xb0) base = 0x898;
    pc->Image2DCreate((unsigned short) (base + itemId), icon);
    pc->DrawImage2D(icon, x, y);
}

void *ImageFactory::loadImage(int row, int col, int frameBase) {
    int id = kImagePartIds[row][col];
    if (id < 0)
        return nullptr;

    unsigned image = 0;
    Globals::Canvas->Image2DCreate(
        (unsigned short) ((short) id + (short) frameBase), image);

    const int *posBase;
    int cell = row * 8 + col * 2;
    if (Globals::iPadHD != 0) {
        posBase = &kImagePartOffsetsIpadHD[0][0][0];
    } else if (Globals::iPadLarge != 0) {
        posBase = &kImagePartOffsetsIpadLarge[0][0][0];
    } else if (Globals::iPad != 0) {
        posBase = &kImagePartOffsetsIpad[0][0][0];
    } else {
        posBase = &gImagePartOffsets[0][0][0];
    }

    int px = posBase[cell];
    int py = posBase[cell + 1];
    return new ImagePart(image, px, py);
}

void ImageFactory::drawItem(int itemId, int frame, int x, int y) {
    PaintCanvas *pc = Globals::Canvas;
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

int *ImageFactory::createChar(int race) {
    int sexRoll = AbyssEngine::AERandom::nextInt(Globals::rnd, 2);
    return this->createChar(sexRoll == 0, race);
}

int *ImageFactory::createChar(bool isMale, int race) {
    if (race == 3) {
        int reroll = AbyssEngine::AERandom::nextInt(Globals::rnd, 4);
        race = (reroll != 0) ? 2 : 0;
    }
    int row = race;
    if (race == 0) row = 10;
    if (isMale) row = race;
    if (row == 5) row = 0;
    int *desc = new int[5];
    desc[0] = row;
    const int *partCounts = kImagePartCounts[row];
    for (int i = 0; i != 4; ++i)
        desc[i + 1] = AbyssEngine::AERandom::nextInt(Globals::rnd, partCounts[i]);
    return desc;
}
