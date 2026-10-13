#include "engine/render/ImagePart.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"

ImagePart::ImagePart(unsigned id, int field04, int posY) {
    this->id = id;
    this->f_4 = field04;
    this->pos_y = posY;
    this->scale_x = Globals::Canvas->GetImage2DWidth(id);
    this->scale_y = Globals::Canvas->GetImage2DHeight(id);
}

ImagePart::~ImagePart() {
}

void ImagePart::draw(int x, int y, bool b) {
    Globals::Canvas->DrawImage2D(this->id, x,
                                 this->pos_y + y,
                                 this->scale_x, this->scale_y,
                                 (unsigned char) 0x11, (unsigned char) (this->f_4 | 1), (unsigned char) b);
}
