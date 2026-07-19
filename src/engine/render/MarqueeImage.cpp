#include "engine/render/MarqueeImage.h"
#include "engine/render/PaintCanvas.h"

static PaintCanvas **g_MarqueeImage_canvas = &PaintCanvas::gCanvas;

MarqueeImage::~MarqueeImage() {
}

void MarqueeImage::setSpeed(float speed) {
    this->speed = speed;
}

void MarqueeImage::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

MarqueeImage::MarqueeImage(uint16_t image, int width, int x, int y, float speed) {
    PaintCanvas **holder = g_MarqueeImage_canvas;

    (*holder)->Image2DCreate(image, this->image);

    this->x = x;
    this->y = y;
    this->visibleWidth = width;
    this->speed = speed;

    this->imageWidth = (*holder)->GetImage2DWidth(this->image);
    this->imageHeight = (*holder)->GetImage2DHeight(this->image);
    this->scrollPosition = 0;
}

void MarqueeImage::draw() {
    return draw(this->x, this->y);
}

void MarqueeImage::update(int dt) {
    float position = this->scrollPosition;
    float delta = ((float) dt / 1000.0f) * this->speed;
    float width = (float) this->imageWidth;
    position += delta;
    this->scrollPosition = position;
    if (width < position) {
        position = (position - width) + delta * 0.5f;
        this->scrollPosition = position;
    }

    this->scrollOffset = (int) (width - position);
}

void MarqueeImage::draw(int x, int y) {
    int32_t offset = this->scrollOffset;
    this->x = x;
    this->y = y;

    int32_t visibleWidth = this->visibleWidth;

    if (offset > -1) {
        int drawWidth = offset;
        if (drawWidth > visibleWidth) {
            drawWidth = visibleWidth;
        }

        (*g_MarqueeImage_canvas)->DrawRegion2D(this->image, (int) this->scrollPosition,
                                               0, drawWidth, this->imageHeight, 0.0f, 0, 0, x, y);
        offset = this->scrollOffset;
    }

    if (offset <= visibleWidth) {
        (*g_MarqueeImage_canvas)->DrawRegion2D(this->image, 0, 0,
                                               visibleWidth - offset,
                                               this->imageHeight, 0.0f, 0, 0,
                                               offset + x, y);
    }
}
