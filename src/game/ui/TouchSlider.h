#ifndef GOF2_TOUCHSLIDER_H
#define GOF2_TOUCHSLIDER_H
#include <cstdint>

class TouchSlider {
public:
    int x;
    int y;
    int knobX;
    int knobY;
    int type;
    int knobWidth;
    int knobHeight;
    int trackWidth;
    int trackHeight;
    float value;
    int numSteps;
    int trackImage;
    int knobImage;
    uint8_t isDragging;
    uint8_t isDisabled;
    int touchPadding;

    TouchSlider(int type, int x, int y, float value);

    ~TouchSlider();

    void setPosition(int x, int y);

    int getWidth();

    void setFixedScale(int numSteps);

    int OnTouchBegin(int x, int y);

    int OnTouchEnd(int x, int y);

    bool OnTouchMove(int x, int y);

    void draw();

    float getValue();

    void setValue(float value);

    void setHalfTransparent(bool transparent);

    int touchedInside(int x, int y);
};
#endif
