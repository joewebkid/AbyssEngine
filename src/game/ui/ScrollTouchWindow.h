#ifndef GOF2_SCROLLTOUCHWINDOW_H
#define GOF2_SCROLLTOUCHWINDOW_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ScrollTouchBox.h"

class ScrollTouchBox;


class ScrollTouchWindow {
public:
    ScrollTouchBox *scrollBox;
    String title;
    uint8_t touchActive;
    uint8_t hasFrame;
    int x;
    int y;
    int width;
    int height;

    ScrollTouchWindow(int x, int y, int w, int h);

    ScrollTouchWindow(int x, int y, int w, int h, bool hasFrame);

    ~ScrollTouchWindow();

    void OnTouchBegin(int x, int y);

    void OnTouchMove(int x, int y);

    void OnTouchEnd(int x, int y);

    void setTextCentered(bool centered);

    void setYPosition(int y);

    void update(int dt);

    void draw();

    void drawTextBG();

    void setText(AbyssEngine::String title, AbyssEngine::String text);

    void setText(AbyssEngine::String title, AbyssEngine::String text, int color);
};
#endif
