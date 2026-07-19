#ifndef GOF2_LENSFLARE_H
#define GOF2_LENSFLARE_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

void LensFlare_Image2DCreate(AbyssEngine::PaintCanvas *canvas, short id, uint32_t *out);

int LensFlare_GetWidth(AbyssEngine::PaintCanvas * canvas);

int LensFlare_GetHeight(AbyssEngine::PaintCanvas * canvas);

float LensFlare_sqrtf(float v);

int LensFlare_imgWidth(AbyssEngine::PaintCanvas *canvas, void *img);

int LensFlare_imgHandle(void *img);

void LensFlare_setColor(AbyssEngine::PaintCanvas *canvas, uint32_t color);

void LensFlare_drawScaled(AbyssEngine::PaintCanvas *canvas, void *img, int x, int y);

void LensFlare_pushState(AbyssEngine::PaintCanvas * canvas);

void LensFlare_setBlend(AbyssEngine::PaintCanvas *canvas, uint32_t mode);

void LensFlare_drawFinal(AbyssEngine::PaintCanvas *canvas, void *img, int a, int b);

void LensFlare_restoreState(AbyssEngine::PaintCanvas *canvas, int saved);

class LensFlare {
public:
    float intensity;
    AbyssEngine::PaintCanvas *canvas;
    int width;
    int height;
    uint32_t *images;

    explicit LensFlare(AbyssEngine::PaintCanvas *canvas);

    ~LensFlare();

    void render2D(float srcX, float srcY, float alpha, int colorIndex);

    void update(int);
};

#endif
