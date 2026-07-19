#ifndef GOF2_LAYOUT_H
#define GOF2_LAYOUT_H

#include <cstdint>

#include "ChoiceWindow.h"
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "TouchButton.h"
#include "engine/render/PaintCanvas.h"
#include "Blk16.h"

class ChoiceWindow;
class TouchButton;


namespace AbyssEngine {
    class PaintCanvas;
}
using ::AbyssEngine::PaintCanvas;

class Layout {
public:
    union {
        uint8_t choiceWindowOpen;
        uint8_t layoutVisibleFlag;
    };

    uint8_t _pad_0x1[3];
    int field_0x4;

    union {
        int field_0x8;
        int windowTopInset;
    };

    union {
        int field_0xc;
        int field_0xc_leftMargin;
    };

    union {
        int field_0x10;
        int field_0x10_rightMargin;
    };

    union {
        int field_0x14;
        int footerTextInset;
    };

    union {
        int field_0x18;
        int headerTitleY;
    };

    int field_0x1c;

    union {
        int field_0x20;
        int field_0x20_top;
    };

    int field_0x24;

    union {
        int field_0x28;
        int buttonInsetX;
    };

    union {
        int field_0x2c;
        int field_0x2c_rowHeight;
    };

    union {
        int field_0x30;
        int field_0x30_rowHeight;
    };

    int field_0x34;
    int field_0x38;
    int field_0x3c;
    int field_0x40;
    int field_0x44;
    int field_0x48;
    int field_0x4c;
    int field_0x50;
    int field_0x54;
    int field_0x58;
    int field_0x5c;
    int field_0x60;
    int field_0x64;
    uint8_t _pad_0x68[8];

    union {
        int field_0x70;
        int field_0x70_rowHeight;
        int promptYOffset;
    };

    int field_0x74;
    int field_0x78;
    int field_0x7c;
    int field_0x80_touchMargin;
    int field_0x84;
    uint8_t _pad_0x88[4];
    int field_0x8c;
    int field_0x90;
    int field_0x94;
    int field_0x98;
    int field_0x9c;
    int field_0xa0;
    int field_0xa4;
    int field_0xa8;
    int field_0xac;
    uint8_t _pad_0xb0[28];
    int field_0xcc;
    int centerYBase;
    int marqueeWidth;
    int progressBarYBase;
    int field_0xdc;
    float animSpeedScale;
    float driftSpeedDivisor;
    float ringRadiusScale;
    int ringNearMidThreshold;
    uint8_t _pad_0xf0[4];
    int ringMidFarThreshold;
    uint8_t _pad_0xf8[4];
    int hudInset;
    int oreLabelYOffset;
    int oreTextYOffset;
    int field_0x108;
    int field_0x10c_rowBaseY;
    int field_0x110_strip58;

    union {
        int field_0x114;
        int field_0x114_strip5c;
    };

    uint8_t _pad_0x118[12];
    int field_0x124;
    int field_0x128;
    uint8_t _pad_0x12c[4];
    int field_0x130;
    int field_0x134;
    int field_0x138;
    int field_0x13c;
    uint8_t _pad_0x140[24];
    int field_0x158;
    int field_0x15c;
    uint8_t _pad_0x160[24];
    int field_0x178;
    int field_0x17c;
    uint8_t _pad_0x180[16];
    int field_0x190;
    int field_0x194;
    int field_0x198;
    int field_0x19c;
    int field_0x1a0;
    int field_0x1a4;
    uint8_t _pad_0x1a8[56];
    int field_0x1e0;
    int field_0x1e4;
    int field_0x1e8;
    int field_0x1ec;
    int field_0x1f0;
    int field_0x1f4;
    int field_0x1f8;
    int field_0x1fc;
    int field_0x200;
    uint8_t _pad_0x204[48];
    int field_0x234;
    union {
        int field_0x238;
        Blk16 field_0x238_blk16;
    };
    int field_0x248;
    int field_0x24c;
    int field_0x250;
    int field_0x254;
    int field_0x258;
    int field_0x25c;
    int field_0x260;
    int field_0x264;
    int field_0x268;
    int field_0x26c;
    int field_0x270;
    int field_0x274;
    int field_0x278;
    int field_0x27c;
    int field_0x280;

    union {
        uint8_t field_0x284;
        uint8_t field_0x284_sliderSlot5Enabled;
    };

    uint8_t field_0x285;
    uint8_t field_0x286;
    uint8_t _pad_0x287[1];
    int field_0x288;
    int field_0x28c;
    int field_0x290;
    int field_0x294_buttonWidth;
    int field_0x298_buttonYBias;
    int field_0x29c_buttonRowGap;
    int field_0x2a0_listRowGap;
    int field_0x2a4_metricA;
    int field_0x2a8_metricB;
    int field_0x2ac_metricC;
    uint8_t _pad_0x2b0[4];
    int field_0x2b4;
    int field_0x2b8;
    int field_0x2bc;
    int field_0x2c0;
    int field_0x2c4;
    int field_0x2c8;
    int field_0x2cc;
    uint8_t _pad_0x2d0[4];
    int field_0x2d4;
    int field_0x2d8;
    int windowX;
    int windowY;
    int windowWidth;
    int windowHeight;
    uint8_t rewardMessageActive;
    uint8_t rewardMessageFlag;
    uint8_t _pad_0x2ee[2];
    int backButtonWidth;
    int field_0x2f4;
    int field_0x2f8;
    int field_0x2fc;
    int field_0x300;
    int field_0x304;
    union {
        int field_0x308;
        int choiceWindowTitleHeight;
    };

    int field_0x30c;
    int field_0x310;
    int field_0x314;
    int field_0x318;

    union {
        int field_0x31c;
        int helpWindowX;
    };

    union {
        int field_0x320;
        int helpWindowY;
    };

    unsigned bgPatternImage;

    union {
        int field_0x328;
        int headerPatternImage;
    };

    union {
        unsigned titleBarImage;
        unsigned headerIconImage;
    };

    union {
        int field_0x330;
        int headerCapImage;
    };
    unsigned footerImageLeft;
    union {
        unsigned footerFillImage;
        unsigned field_0x338;
    };
    int footerImageRight;
    unsigned field_0x340;
    unsigned footerPatternImage;
    unsigned field_0x348;
    unsigned field_0x34c;
    unsigned field_0x350;
    unsigned field_0x354;
    unsigned field_0x358;
    unsigned field_0x35c;
    unsigned field_0x360;
    unsigned field_0x364;
    unsigned field_0x368;
    unsigned field_0x36c;
    unsigned field_0x370;
    int scrollBarImage;

    union {
        unsigned field_0x378;
        unsigned scrollBarFillImage;
    };
    unsigned field_0x37c;
    unsigned field_0x380;
    int field_0x384;
    unsigned field_0x388;
    unsigned field_0x38c;
    unsigned field_0x390;
    int field_0x394;
    int tipBoxImage;
    unsigned field_0x39c;
    int field_0x3a0;

    union {
        unsigned rewardIconImage;
        unsigned field_0x3a4;
    };
    int field_0x3a8;

    union {
        int field_0x3ac;
        int textBaselineAdjust;
    };

    unsigned drawColor;
    TouchButton *backButton;
    TouchButton *secondaryButton;
    TouchButton *helpButton;
    uint8_t helpPressedFlag;
    uint8_t _pad_0x3c1[3];
    ChoiceWindow *choiceWindow;
    Array<String *> *tipLines;
    uint8_t helpButtonEnabled;
    uint8_t _pad_0x3cd[3];
    int rewardMessageTimer;
    int rewardCredits;
    int field_0x3d8;
    uint8_t field_0x3dc;
    uint8_t _pad_0x3dd[3];
    union {
        int field_0x3e0;
        int scrollBarInset;
    };

    union {
        int field_0x3e4;
        int scrollBarHandle;
    };

    union {
        int field_0x3e8;
        int rewardBoxWidth;
    };

    union {
        int field_0x3ec;
        int rewardBoxHeight;
    };

    union {
        int field_0x3f0;
        int rewardBoxX;
    };

    union {
        int field_0x3f4;
        int rewardBoxY;
    };

    union {
        int field_0x3f8;
        int rewardBoxY2;
    };

    union {
        int field_0x3fc;
        int footerButtonOffset;
    };
    uint8_t fading;
    uint8_t fadeOut;
    uint8_t _pad_0x402[2];
    unsigned fadeColor;

    union {
        struct {
            int fadeProgress;
            int fadeDuration;
            uint8_t fillScreen;
        };
        struct {
            uint8_t _skip_0x408[1];
            int field_0x409;
        };
        struct {
            uint8_t _skip_0x40d[5];
            int field_0x40d;
        };
    };

    Layout();

    ~Layout();

    int OnTouchBegin(int x, int y);

    int OnTouchEnd(int x, int y);

    int OnTouchMove(int x, int y);

    void drawBG();

    void drawBGBorder(unsigned corner, unsigned edge, int x, int y, int w, int h, int inset, int pad);

    void drawBGPattern(unsigned img, int x, int y, int w, int h);

    void drawBox(int style, int x, int y, int w, int h, String text, unsigned char flags);

    void drawEmptyFooter(bool showBack);

    uint8_t drawFade();

    void drawFooter();

    void drawFooterNoBackButton();

    void drawFooterStation();

    void drawHeader();

    void drawHeader(String title);

    void drawHeader(String title, bool transition);

    void drawHelpWindow();

    void drawMissionRewardMessage(bool transition);

    void drawScrollBar(int x, int y, int trackH, int pos, int range);

    void drawTip();

    void enableFillScreen(bool v);

    static String formatNumber(int n);

    static String tagString(String in);

    static String formatCredits(int n);

    int getFooterTransitionWidth();

    int getHelpButtonOffset();

    float getPulseValue(float speed);

    uint8_t helpPressed();

    void initHelpWindow(String text);

    void initTip();

    uint8_t isFading();

    void reload();

    void resetWindowDimensions();

    void setDrawColor(int color);

    void setWindowDimensions(int p1, int p2, int p3, int p4);

    void showMissionRewardMessage(int show, bool flag);

    void startFade(bool fadeOut, int color, int duration);

    void update(int dt);

    void drawWindow(String title, int x, int y, int w, int h);

    void drawWindow(String title);

    void drawWindow(String title, bool drawBG);

    void drawWindow(String title, int x, int y, int w, int h, bool drawBG);

    void drawMask();

    void drawMask(int x, int y, int w, int h);

    void drawBGBorder(unsigned corner, unsigned edge, int x, int y, int w, int h);

    void drawBox(int style, int x, int y, int w, int h, String text);

    void drawFooter(bool stationMode, bool showBack);
};
#endif
