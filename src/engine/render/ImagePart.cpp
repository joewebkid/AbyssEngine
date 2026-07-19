#include "engine/render/ImagePart.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/PaintCanvas.h"

static void **g_ImagePart_canvas;
static void **g_ImagePart_draw_canvas;

ImagePart::ImagePart(unsigned id, int field04, int posY) {
    this->id = id;
    this->f_4 = field04;
    this->pos_y = posY;
    void **holder = g_ImagePart_canvas;
    this->scale_x = ((PaintCanvas *) *holder)->GetImage2DWidth(id);
    this->scale_y = ((PaintCanvas *) *holder)->GetImage2DHeight(id);
}

ImagePart::~ImagePart() {
}

void ImagePart::draw(int x, int y, bool b) {
    void **holder = g_ImagePart_draw_canvas;
    ((PaintCanvas *) *holder)->DrawImage2D(this->id, x,
                                           this->pos_y + y,
                                           this->scale_x, this->scale_y,
                                           (unsigned char) 0x11, (unsigned char) (this->f_4 | 1), (unsigned char) b);
}
