#ifndef GOF2_NEWSTICKER_H
#define GOF2_NEWSTICKER_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
class NewsTicker {
public:
    float scrollOffset;
    int x;
    int y;
    int width;
    int textWidth;
    String tickerText;
    uint8_t touched;
    int lastTouchX;

    NewsTicker(int x, int y, int width, int faction, int level);

    ~NewsTicker();

    void draw();

    void update(int dt);

    int getHeight();

    uint8_t OnTouchBegin(int x, int y);

    bool OnTouchMove(int x, int unused);

    uint32_t OnTouchEnd(int x, int unused);

    String replaceTokens(String text);
};
#endif
