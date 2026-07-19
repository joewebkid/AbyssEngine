#include "game/ui/TouchSlider.h"
#include "engine/core/AbyssEngine.h"
#include "engine/render/PaintCanvas.h"
#include <cstddef>

// The TouchSlider constructor reads the global touch-padding configuration from
// the application handle stored in g_TouchSlider_app. That handle is an untyped
// pointer (no concrete engine class binds this offset), so we model only the one
// field this code touches: an int "touchPadding" living at byte offset 0x7c.
struct TouchSliderAppConfig {
    char _reserved[0x7c];
    int touchPadding;
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(TouchSliderAppConfig, touchPadding) == 0x7c,
              "touchPadding must sit at app offset 0x7c");
#endif

void TouchSlider::setPosition(int x, int y) {
    float trackRange = (float) (this->trackWidth - this->knobWidth);
    float knobBase = (float) (x + this->knobWidth / 2);
    this->x = x;
    this->y = y;
    this->knobY = y + this->trackHeight / 2;
    this->knobX = (int) (knobBase + this->value * trackRange);
}


static PaintCanvas **g_TouchSlider_canvas = &PaintCanvas::gCanvas;

static void *g_TouchSlider_app_storage = nullptr;
static void **g_TouchSlider_app = &g_TouchSlider_app_storage;

TouchSlider::TouchSlider(int type, int x, int y, float value) {
    this->type = type;
    this->value = value;
    PaintCanvas **canvas = g_TouchSlider_canvas;
    unsigned int knobImageOut;
    (*canvas)->Image2DCreate(0x51a, knobImageOut);
    this->knobImage = knobImageOut;

    unsigned short trackImageId = 0x51b;
    if (type == 1)
        trackImageId = 0x51c;
    if (type == 0)
        trackImageId = 0x519;
    unsigned int trackImageOut;
    (*canvas)->Image2DCreate(trackImageId, trackImageOut);
    this->trackImage = trackImageOut;

    this->knobWidth = (*canvas)->GetImage2DWidth(this->knobImage);
    this->knobHeight = (*canvas)->GetImage2DHeight(this->knobImage);
    this->trackWidth = (*canvas)->GetImage2DWidth(this->trackImage);
    this->trackHeight = (*canvas)->GetImage2DHeight(this->trackImage);

    this->isDragging = 0;
    this->isDisabled = 0;
    setPosition(x, y);
    this->numSteps = 0;

    this->touchPadding = ((TouchSliderAppConfig *) *g_TouchSlider_app)->touchPadding;
}

TouchSlider::~TouchSlider() {
}

int TouchSlider::getWidth() {
    return this->trackWidth;
}

void TouchSlider::setFixedScale(int numSteps) {
    this->numSteps = numSteps;
}

int TouchSlider::OnTouchBegin(int x, int y) {
    if (this->isDisabled != 0)
        return 0;
    int inside = touchedInside(x, y);
    this->isDragging = (uint8_t) inside;
    return inside;
}

int TouchSlider::OnTouchEnd(int x, int y) {
    if (this->isDisabled != 0)
        return 0;

    int result;
    if (this->isDragging == 0) {
        result = 0;
    } else {
        int steps = this->numSteps;
        if (steps > 0) {
            float stepCount = (float) (steps + 1);
            float currentValue = getValue();
            float snappedValue;
            float threshold = 0.0f;
            int step = 0;
            while (true) {
                snappedValue = currentValue;
                if (step >= steps + 2)
                    break;
                snappedValue = threshold;
                if (currentValue <= (1.0f / stepCount) * 0.5f + threshold)
                    break;
                threshold = 1.0f / stepCount + threshold;
                step = step + 1;
            }
            setValue(snappedValue);
        }
        result = 1;
    }
    this->isDragging = 0;
    return result;
}

void TouchSlider::draw() {
    PaintCanvas **canvas = g_TouchSlider_canvas;
    int color = this->isDisabled != 0 ? 0xFFFFFF2F : -1;
    (*canvas)->SetColor((unsigned int) color);
    (*canvas)->DrawImage2D(this->trackImage, this->x, this->y);
    (*canvas)->DrawImage2D(this->knobImage, this->knobX, this->knobY, (unsigned char) 0x11, (unsigned char) 0x44);
}

bool TouchSlider::OnTouchMove(int x, int y) {
    if (this->isDisabled != 0)
        return false;
    if (this->isDragging != 0) {
        int halfKnob = this->knobWidth / 2;
        int minX = this->x + halfKnob;
        int maxX = (this->x + this->trackWidth) - halfKnob;
        int clampedX = maxX;
        if (x < clampedX)
            clampedX = x;
        if (clampedX <= minX)
            clampedX = minX;
        this->knobX = clampedX;
    }
    return this->isDragging != 0;
}

float TouchSlider::getValue() {
    float trackRange = (float) (this->trackWidth - this->knobWidth);
    float knobOffset = (float) ((this->knobX - this->knobWidth / 2) - this->x);
    return knobOffset / trackRange;
}

void TouchSlider::setHalfTransparent(bool transparent) {
    this->isDisabled = transparent;
}

int TouchSlider::touchedInside(int x, int y) {
    int halfKnob = this->knobWidth >> 1;
    int centerX = this->knobX;
    int pad = this->touchPadding;
    if (centerX - halfKnob - pad > x)
        return 0;
    if (x >= halfKnob + centerX + pad)
        return 0;
    int halfKnobH = this->knobHeight >> 1;
    int centerY = this->knobY;
    if (centerY - pad - halfKnobH > y)
        return 0;
    return y <= halfKnobH + centerY + pad;
}

void TouchSlider::setValue(float value) {
    float trackRange = (float) (this->trackWidth - this->knobWidth);
    float knobBase = (float) (this->x + this->knobWidth / 2);
    this->knobX = (int) (knobBase + trackRange * value);
}
