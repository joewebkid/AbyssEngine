#include "engine/render/Sprite.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/PaintCanvas.h"


static PaintCanvas **g_Sprite_canvas = &PaintCanvas::gCanvas;

static PaintCanvas **g_Sprite_draw_image_canvas = &PaintCanvas::gCanvas;

static PaintCanvas **g_Sprite_draw_region_canvas = &PaintCanvas::gCanvas;

void Sprite::setPosition(int x, int y) {
    this->posX = x;
    this->posY = y;
}

void Sprite::setRefPixelPosition(int x, int y) {
    this->posX = x;
    this->posY = y;
}

Sprite::~Sprite() {
    delete[] this->frames;
    this->frames = nullptr;
}

void Sprite::defineReferencePixel(int x, int y) {
    this->refPixelX = x;
    this->refPixelY = y;
}

void Sprite::nextFrame() {
    return setFrame(this->currentFrame + 1);
}

void Sprite::draw(float scaleX, float scaleY) {
    uint32_t *frames = this->frames;

    if (frames == nullptr) {
        int refX = this->refPixelX;
        int x = this->posX - refX;
        int y = this->posY - refX;
        (*g_Sprite_draw_region_canvas)->DrawRegion2D(
            this->image, this->frameSrcX, this->frameSrcY,
            this->frameWidth, this->frameHeight, 0.0f, 0, 0, x, y);
        return;
    }

    PaintCanvas *canvas = *g_Sprite_draw_image_canvas;
    uint32_t image = frames[this->currentFrame];
    int refX = this->refPixelX;
    int x = this->posX - refX;
    int y = this->posY - refX;

    if (scaleX == 1.0f || scaleY == 1.0f) {
        return canvas->DrawImage2D(image, x, y);
    }

    float frameWidth = (float) this->frameWidth;
    float frameHeight = (float) this->frameHeight;
    int scaledWidth = (int) (frameWidth * scaleX);
    int scaledHeight = (int) (frameHeight * scaleY);
    int drawX = (int) ((float) x - ((scaleX - 1.0f) * frameWidth) * 0.5f);
    int drawY = (int) ((float) y - ((scaleY - 1.0f) * frameHeight) * 0.5f);

    canvas->DrawImage2D(image, drawX, drawY, scaledWidth, scaledHeight,
                        (unsigned char) 0x11, (unsigned char) 0x11, (unsigned char) 0);
}

Sprite::Sprite(uint32_t image, int frameWidth, int frameHeight) {
    PaintCanvas *canvas = *g_Sprite_canvas;

    this->frames = nullptr;
    this->image = image;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;

    this->imageWidth = canvas->GetImage2DWidth(image);
    this->imageHeight = canvas->GetImage2DHeight(image);

    this->refPixelX = 0;
    this->refPixelY = 0;
    this->posX = 0;
    this->posY = 0;

    this->columns = this->imageWidth / frameWidth;
    this->rows = this->imageHeight / frameHeight;
    this->frameCount = this->rows * this->columns;
    setFrame(0);
}

void Sprite::prevFrame() {
    return setFrame(this->currentFrame - 1);
}

void Sprite::drawRegion(int srcX, int srcY, int w, int h) {
    PaintCanvas *canvas = *g_Sprite_canvas;
    int refX = this->refPixelX;
    int x = this->posX + srcX - refX;
    int y = this->posY + srcY - refX;

    if (this->frames != nullptr) {
        uint32_t image = this->frames[this->currentFrame];
        canvas->DrawRegion2D(image, srcX, srcY, w, h, 0.0f, 0, 0, x, y);
        return;
    }

    canvas->DrawRegion2D(this->image, srcX + this->frameSrcX, srcY + this->frameSrcY,
                         w, h, 0.0f, 0, 0, x, y);
}

Sprite::Sprite(uint32_t *frames, int frameCount, int frameWidth, int frameHeight) {
    PaintCanvas *canvas = *g_Sprite_canvas;

    this->frames = frames;
    this->image = (uint32_t) - 1;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;

    this->imageWidth = canvas->GetImage2DWidth(frames[0]);
    this->imageHeight = canvas->GetImage2DHeight(frames[0]);

    this->refPixelX = 0;
    this->refPixelY = 0;
    this->posX = 0;
    this->posY = 0;

    this->columns = 1;
    this->rows = 1;
    this->frameCount = frameCount;
    setFrame(0);
}

int Sprite::getFrameWidth() {
    return this->frameWidth;
}

int Sprite::getFrameHeight() {
    return this->frameHeight;
}

int Sprite::getFrame() {
    return this->currentFrame;
}

int Sprite::getRawFrameCount() {
    return this->frameCount;
}

void Sprite::setFrame(int frame) {
    if (frame < 0) {
        frame = -frame;
    }

    this->currentFrame = frame;
    if (frame >= this->frameCount) {
        frame = frame % this->frameCount;
        this->currentFrame = frame;
    }

    int frameWidth = this->frameWidth;
    int frameHeight = this->frameHeight;
    int columns = this->columns;
    int row = frame / columns;

    int column = frame - row * columns;
    int frameY = row * frameHeight;
    int frameX = column * frameWidth;

    this->frameSrcX = frameX;
    this->frameSrcY = frameY;
}
