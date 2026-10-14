#ifndef GOF2_LENSFLARE_H
#define GOF2_LENSFLARE_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

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
