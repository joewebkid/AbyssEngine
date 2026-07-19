#ifndef GOF2_TOUCHBUTTON_H
#define GOF2_TOUCHBUTTON_H
#include "engine/math/Vector.h"
#include "game/core/String.h"

#include "engine/core/AEString.h"
#include "engine/math/AEMath.h"


class TouchButton {
public:
    int field_0x0;
    int field_0x4;
    uint32_t fontId;
    String text;
    String splitText;
    int subId;
    uint32_t image;
    String numberText;
    int adornImage;
    int imgFrameTL;
    int imgFrameT;
    int imgFrameTR;
    int imgFrameL;
    int imgFrameM;
    int imgFrameR;
    int imgFrameBL;
    int imgFrameB;
    int imgFrameBR;
    uint32_t iconImage;
    int iconOverlay;
    int iconSmall;
    int requestedWidth;
    int kind;
    unsigned char flags0;
    unsigned char flags1;
    int x;
    int y;
    int initX;
    int initY;
    int height;
    int layoutHeight;
    int width;
    int leftWidth;
    int midWidth;
    int rightWidth;
    int midStretch;
    int textOffsetX;
    int textOffsetY;
    int textColor;
    unsigned char touched;
    unsigned char alwaysPressed;
    unsigned char visible;
    unsigned char halfTransparent;
    uint32_t iconTexId;
    unsigned char progressHighlight;
    float pressProgress;
    int touchMargin;
    int fontSpacing;

    TouchButton(String const &text, int x, int y, int p4, unsigned char p5);

    TouchButton(int x, int y, String const &text, int p4, int p5, unsigned char p6);

    TouchButton(String const &text, int type, int x, int y, int p5, unsigned char p6, unsigned char p7);

    TouchButton(String const &text, int a, int b, int c, int d, unsigned char flags0, unsigned char flags1,
                unsigned int font, int kerning);

    TouchButton(unsigned int kind, int a, int b, int c, unsigned char flag);

    TouchButton(unsigned int kind, int a, int b, int c, int d, unsigned char flags0, unsigned char flags1);

    TouchButton(unsigned int kind, unsigned int image, int a, int b, int c, unsigned char flag);

    ~TouchButton();

    bool OnTouchBegin(int px, int py);

    unsigned int OnTouchEnd(int px, int py);

    unsigned int OnTouchMove(int px, int py);

    void draw();

    int getHeight();

    AbyssEngine::AEMath::Vector getPosition();

    String getText();

    int getWidth();

    int init(String const &text, unsigned int kind, int achId, int achStage, int width, int d_unused, int x, int y,
             unsigned char flags0, unsigned char flags1);

    uint8_t isTouched();

    uint8_t isVisible();

    void replaceTextKeepSize(String const &text);

    void resetTouch();

    void setAlwaysPressed(bool value);

    void setHalfTransparent(bool value);

    void setNumberText(String const &value);

    void setGamePadButtonImage(unsigned int image);

    void setPosition(int x, int y, unsigned char flags);

    void setPosition(int x, int y);

    void setPressProgress(float value);

    void setPressProgressHighlight(bool value);

    void setSplitText(String const &value);

    void setText(String const &text);

    void setTextColor(int color);

    void setVisible(bool value);

    void setYPosition(int y);

    bool touchedInside(int px, int py);

    void translate(int dx, int dy);
};
#endif
