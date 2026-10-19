#include "engine/render/LensFlare.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"

// ELF .rodata, Android libgof2hdaa.so:
// dword_201E40 / dword_201E20 / dword_201E00.
static const uint32_t gFlareR[5] = {200, 200, 255, 255, 255};
static const uint32_t gFlareG[5] = {200, 255, 200, 255, 255};
static const uint32_t gFlareB[5] = {255, 200, 200, 255, 200};
static const float gFlareIntensityBase = 64.0f;       // flt_11D690
static const float gFlareIntensityBaseColor5 = 80.0f; // flt_11D694

LensFlare::LensFlare(PaintCanvas *canvas) {
    this->images = new uint32_t[3];
    for (int i = 0; i != 3; ++i)
        Globals::Canvas->Image2DCreate((unsigned short) (i + 0x508), this->images[i]);
    this->width = canvas->GetWidth();
    this->height = canvas->GetHeight();
    this->canvas = canvas;
}

LensFlare::~LensFlare() {
    // ARM 0x11d076 calls scalar operator delete for the handle buffer.
    if (this->images)
        ::operator delete(this->images);
    this->images = nullptr;
}

void LensFlare::render2D(float srcX, float srcY, float alpha, int colorIndex) {
    unsigned int r, g, b;
    if ((unsigned) colorIndex <= 4) {
        r = gFlareR[colorIndex];
        g = gFlareG[colorIndex];
        b = gFlareB[colorIndex];
    } else {
        r = 0xff;
        g = 0xff;
        b = 0xff;
    }

    if (!(alpha < 0.0f))
        return;

    int w = this->width;
    int h = this->height;
    if (!((float) -w < srcX && srcX < (float) (w * 2) &&
          (float) -h < srcY && srcY < (float) (h * 2)))
        return;

    float cx = (float) (w >> 1);
    float cy = (float) (h >> 1);
    float dx = cx - srcX;
    float dy = cy - srcY;
    float dist = static_cast<Globals *>(Globals::globals)->sqrt(dx * dx + dy * dy);

    float stepX = -(dx * 65536.0f);
    float stepY = -(dy * 65536.0f);
    if (dist > 0.0f) {
        stepX /= dist;
        stepY /= dist;
    }

    float halfH = (float) (h >> 1);
    float base = (colorIndex == 5) ? gFlareIntensityBaseColor5 : gFlareIntensityBase;
    this->intensity = base * (1.0f - dist / halfH);

    float spanX = dist * stepX;
    float spanY = dist * stepY;
    constexpr float fixedToPixel = 1.0f / 65536.0f;
    PaintCanvas *globalCanvas = Globals::Canvas;
    unsigned int img0 = this->images[0];
    unsigned int img1 = this->images[1];
    unsigned int img2 = this->images[2];

    globalCanvas->SetColor((unsigned char) r, (unsigned char) g, (unsigned char) b,
                           (unsigned char) (120.0f - (50.0f - this->intensity)));
    globalCanvas->DrawImage2D(img0,
                              (int) (cx + spanX * 0.5f * fixedToPixel),
                              (int) (cy + spanY * 0.5f * fixedToPixel), 0x11, 0x44);

    int imageWidth = globalCanvas->GetImage2DWidth(img0);
    int imageHeight = globalCanvas->GetImage2DWidth(img0);
    globalCanvas->DrawImage2D(img0,
                              (int) (cx + spanX * 0.25f * fixedToPixel),
                              (int) (cy + spanY * 0.25f * fixedToPixel),
                              (int) ((float) imageWidth * 0.75f),
                              (int) ((float) imageHeight * 0.75f), 0x11, 0x44, 0);

    imageWidth = globalCanvas->GetImage2DWidth(img1);
    imageHeight = globalCanvas->GetImage2DWidth(img1);
    float farX = spanX * 0.75f * fixedToPixel;
    float farY = spanY * 0.75f * fixedToPixel;
    globalCanvas->DrawImage2D(img1, (int) (cx + farX), (int) (cy + farY),
                              (int) ((float) imageWidth * 0.5f),
                              (int) ((float) imageHeight * 0.5f), 0x11, 0x44, 0);

    globalCanvas->SetColor((unsigned char) r, (unsigned char) g, (unsigned char) b,
                           (unsigned char) (90.0f - (50.0f - this->intensity)));
    if (50.0f - this->intensity < 90.0f) {
        imageWidth = globalCanvas->GetImage2DWidth(img0);
        imageHeight = globalCanvas->GetImage2DWidth(img0);
        globalCanvas->DrawImage2D(img0,
                                  (int) (cx + spanX * 0.125f * fixedToPixel),
                                  (int) (cy + spanY * 0.125f * fixedToPixel),
                                  (int) ((float) imageWidth * 1.25f),
                                  (int) ((float) imageHeight * 1.25f), 0x11, 0x44, 0);
    }

    globalCanvas->SetColor((unsigned char) r, (unsigned char) g, (unsigned char) b,
                           (unsigned char) (120.0f - (50.0f - this->intensity)));
    imageWidth = globalCanvas->GetImage2DWidth(img1);
    imageHeight = globalCanvas->GetImage2DWidth(img1);
    globalCanvas->DrawImage2D(img1,
                              (int) (cx + spanX / 11.0f * fixedToPixel),
                              (int) (cy + spanY / 11.0f * fixedToPixel),
                              (int) ((float) imageWidth * 0.5f),
                              (int) ((float) imageHeight * 0.5f), 0x11, 0x44, 0);

    globalCanvas->SetColor((unsigned char) r, (unsigned char) g, (unsigned char) b,
                           (unsigned char) (120.0f - (50.0f - this->intensity)));
    imageWidth = globalCanvas->GetImage2DWidth(img2);
    imageHeight = globalCanvas->GetImage2DWidth(img2);
    globalCanvas->DrawImage2D(img2, (int) (cx - farX), (int) (cy - farY),
                              imageWidth + imageWidth, imageHeight + imageHeight,
                              0x11, 0x44, 0);

    globalCanvas->SetColor((unsigned char) r, (unsigned char) g, (unsigned char) b,
                           (unsigned char) (120.0f - (50.0f - this->intensity)));
    imageWidth = globalCanvas->GetImage2DWidth(img2);
    imageHeight = globalCanvas->GetImage2DWidth(img2);
    globalCanvas->DrawImage2D(img2,
                              (int) (cx - spanX * 0.2f * fixedToPixel),
                              (int) (cy - spanY * 0.2f * fixedToPixel),
                              (int) ((float) imageWidth * 0.5f),
                              (int) ((float) imageHeight * 0.5f), 0x11, 0x44, 0);

    if (this->intensity > 0.0f) {
        unsigned int oldColor = this->canvas->GetColor();
        this->canvas->SetColor(((g << 16) | (b << 8) | (r << 24)) +
                               (int) this->intensity);
        this->canvas->FillRectangle(0, 0, this->width, this->height);
        this->canvas->SetColor(oldColor);
    }
}

void LensFlare::update(int) {
    this->intensity = 0.0f;
}
