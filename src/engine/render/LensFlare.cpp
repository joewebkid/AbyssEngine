#include "engine/render/LensFlare.h"

static PaintCanvas *const gLensFlareCanvas = nullptr;
static PaintCanvas *const gLF_Canvas2 = nullptr;

// ELF .rodata, Android libgof2hdaa.so:
// dword_201E40 / dword_201E20 / dword_201E00.
static const uint32_t gFlareR[5] = {200, 200, 255, 255, 255};
static const uint32_t gFlareG[5] = {200, 255, 200, 255, 255};
static const uint32_t gFlareB[5] = {255, 200, 200, 255, 200};
static const float gFlareIntensityBase = 64.0f;       // flt_11D690
static const float gFlareIntensityBaseColor5 = 80.0f; // flt_11D694

LensFlare::LensFlare(PaintCanvas *canvas) {
    this->images = new uint32_t[3];
    PaintCanvas *singleton = *(PaintCanvas **) gLensFlareCanvas;
    for (int i = 0; i != 3; ++i)
        LensFlare_Image2DCreate(singleton, (short) (i + 0x508), &this->images[i]);
    this->width = LensFlare_GetWidth(canvas);
    this->height = LensFlare_GetHeight(canvas);
    this->canvas = canvas;
}

LensFlare::~LensFlare() {
    delete[] this->images;
    this->images = nullptr;
}

void LensFlare::render2D(float srcX, float srcY, float alpha, int colorIndex) {
    uint32_t r, g, b;
    if ((unsigned) colorIndex <= 4) {
        r = gFlareR[colorIndex];
        g = gFlareG[colorIndex];
        b = gFlareB[colorIndex];
    } else {
        r = 0xff;
        g = 0xff;
        b = 0xff;
    }

    if (alpha >= 0.0f)
        return;

    int w = this->width;
    if ((float) (-w) >= srcX)
        return;
    if ((float) (w * 2) <= srcX)
        return;

    int h = this->height;
    if ((float) (-h) >= srcY)
        return;
    if ((float) (h * 2) <= srcY)
        return;

    float cx = (float) (w >> 1);
    float cy = (float) (h >> 1);
    float dx = cx - srcX;
    float dy = cy - srcY;
    float dist = LensFlare_sqrtf(dx * dx + dy * dy);

    float ndx = -dx * 1.5f;
    float ndy = -dy * 1.5f;
    if (dist > 0.0f) {
        ndx = ndx / dist;
        ndy = ndy / dist;
    }

    float halfH = (float) (h >> 1);
    float base = (colorIndex == 5) ? gFlareIntensityBaseColor5 : gFlareIntensityBase;
    float fade = base * (1.0f - dist / halfH);
    this->intensity = fade;

    PaintCanvas *canvas = *(PaintCanvas **) gLF_Canvas2;
    void *img0 = (void *) (uintptr_t) this->images[0];
    void *img1 = (void *) (uintptr_t) this->images[1];
    void *img2 = (void *) (uintptr_t) this->images[2];

    {
        uint32_t a = (uint32_t)(1.0f - (1.0f - fade));
        uint32_t color = (a << 0) | (r << 8) | (g << 16) | (b << 24);
        LensFlare_setColor(canvas, color);
        float px = cx + ndx * 0.5f * 0.5f * 17.0f;
        float py = cy + ndy * 0.5f * 0.5f * 17.0f;
        LensFlare_drawScaled(canvas, img0, (int) px, (int) py);
    }

    {
        int iw = LensFlare_imgHandle(img0);
        float s = (float) iw * 0.75f;
        LensFlare_drawScaled(canvas, img0, (int) (cx + ndx * s), (int) (cy + ndy * s));
    }

    {
        int ih = LensFlare_imgHandle(img1);
        float s = (float) ih * 0.5f;
        LensFlare_drawScaled(canvas, img1, (int) (cx + ndx * s * 17.0f),
                             (int) (cy + ndy * s * 17.0f));
        float s2 = (float) ih * 0.25f;
        LensFlare_drawScaled(canvas, img1, (int) (cx + ndx * s2 * 17.0f),
                             (int) (cy + ndy * s2 * 17.0f));
    }

    {
        uint32_t a = (uint32_t)(1.0f - this->intensity);
        uint32_t color = (a << 0) | (r << 8) | (g << 16) | (b << 24);
        LensFlare_setColor(canvas, color);
    }
    if (1.0f - this->intensity < 0.625f) {
        int iw = LensFlare_imgWidth(canvas, img0);
        int ih = LensFlare_imgWidth(canvas, img0);
        float s = (float) iw * 1.25f;
        LensFlare_drawScaled(canvas, img0, (int) (cx + ndx * (float) ih * 0.125f * 17.0f),
                             (int) (cy + ndy * s));
    }

    {
        uint32_t a = (uint32_t)(1.0f - this->intensity);
        uint32_t color = (a << 0) | (r << 8) | (g << 16) | (b << 24);
        LensFlare_setColor(canvas, color);
        int iw0 = LensFlare_imgWidth(canvas, img2);
        int iw1 = LensFlare_imgWidth(canvas, img2);
        float s = (float) iw0 * 2.0f;
        LensFlare_drawScaled(canvas, img2, (int) (cx + ndx * (float) iw1 * 0.5f),
                             (int) (cy + ndy * s));
    }

    if (this->intensity > 0.0f) {
        int saved = (int) (uintptr_t) this->canvas;
        LensFlare_pushState(this->canvas);
        uint32_t color = (g << 16) | (b << 8) | (r << 24);
        uint32_t shifted = color + (uint32_t)(int)
        this->intensity;
        LensFlare_setBlend(this->canvas, shifted);
        LensFlare_drawFinal(this->canvas, this->images, 0, this->height);
        LensFlare_restoreState(this->canvas, saved);
    }
}

void LensFlare::update(int) {
    this->intensity = 0.0f;
}
