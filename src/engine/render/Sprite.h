#ifndef GOF2_SPRITE_H
#define GOF2_SPRITE_H
#include <cstdint>

class Sprite {
public:
    uint32_t *frames;
    uint32_t image;
    int32_t refPixelX;
    int32_t refPixelY;
    int32_t posX;
    int32_t posY;
    int32_t frameWidth;
    int32_t frameHeight;
    int32_t imageWidth;
    int32_t imageHeight;
    int32_t frameSrcX;
    int32_t frameSrcY;
    int32_t columns;
    int32_t rows;
    int32_t currentFrame;
    int32_t frameCount;

    Sprite(uint32_t image, int frameWidth, int frameHeight);

    Sprite(uint32_t *frames, int frameCount, int frameWidth, int frameHeight);

    ~Sprite();

    void setPosition(int x, int y);

    void setRefPixelPosition(int x, int y);

    void defineReferencePixel(int x, int y);

    void setFrame(int frame);

    void nextFrame();

    void prevFrame();

    void draw(float scaleX, float scaleY);

    void drawRegion(int srcX, int srcY, int w, int h);

    int getFrameWidth();

    int getFrameHeight();

    int getFrame();

    int getRawFrameCount();
};
#endif
