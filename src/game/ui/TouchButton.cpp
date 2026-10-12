#include "game/ui/TouchButton.h"
#include "engine/audio/FModSound.h"
#include "game/mission/Achievements.h"
#include "game/ui/Layout.h"
#include "game/core/String.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/GameText.h"
#include "game/core/Globals.h"

void TouchButton::setVisible(bool value) {
    this->visible = value;
}

void TouchButton::setPosition(int x, int y, unsigned char flags) {
    unsigned int f = (unsigned int) flags;
    this->x = x;
    this->y = y;
    this->flags0 = flags;
    if ((flags & 0x20) != 0) {
        y = y - this->layoutHeight;
        this->y = y;
    }
    if ((int) (f << 0x1e) < 0) {
        x = x - this->width;
        this->x = x;
    }
    if ((int) (f << 0x19) < 0) {
        this->y = y - this->layoutHeight / 2;
    }
    if ((int) (f << 0x1d) < 0) {
        this->x = x - this->width / 2;
    }
}

void TouchButton::translate(int dx, int dy) {
    this->x = dx + this->x;
    this->y = dy + this->y;
}

void TouchButton::setNumberText(String const &value) {
    this->numberText = value;
}

TouchButton::~TouchButton() {
}

unsigned int TouchButton::OnTouchMove(int px, int py) {
    if (this->visible != 0 && this->halfTransparent == 0) {
        unsigned int r;
        if (this->touched == 0)
            r = 0;
        else
            r = touchedInside(px, py);
        this->touched = (unsigned char) r;
        return r;
    }
    return 0;
}

uint8_t TouchButton::isVisible() {
    return this->visible;
}

void TouchButton::setPressProgress(float value) {
    this->pressProgress = value;
}

int TouchButton::isTouched() {
    return this->touched;
}

String TouchButton::getText() {
    return this->text;
}

void TouchButton::replaceTextKeepSize(String const &text) {
    this->text = text;
    if (this->kind == 10) {
        int w = this->width;
        int tw = PaintCanvas::gCanvas->GetTextWidth(this->fontId, this->text);
        this->textOffsetX = w / 2 - tw / 2;
    }
}

void TouchButton::setSplitText(String const &value) {
    this->splitText = value;
}

Vector TouchButton::getPosition() {
    Vector pos;
    pos.x = static_cast<float>(this->x);
    pos.y = static_cast<float>(this->y);
    pos.z = 0.0f;
    return pos;
}

void TouchButton::setTextColor(int color) {
    this->textColor = color;
}

void TouchButton::setAlwaysPressed(bool value) {
    this->alwaysPressed = value;
}

void TouchButton::resetTouch() {
    this->touched = 0;
}

static FModSound **g_TB_sound = nullptr;

bool TouchButton::OnTouchBegin(int px, int py) {
    if (this->visible == 0 || this->halfTransparent != 0)
        return false;
    int r = touchedInside(px, py);
    this->touched = (unsigned char) r;
    if (r == 0)
        return false;
    (*g_TB_sound)->play(0x7c, 0, 0, 0.0f);
    return this->touched != 0;
}

unsigned int TouchButton::OnTouchEnd(int px, int py) {
    unsigned int res;
    if (this->visible == 0 || this->halfTransparent != 0) {
        res = 0;
    } else if (this->touched == 0 || touchedInside(px, py) == 0) {
        res = 0;
        this->touched = 0;
    } else {
        this->touched = 0;
        (*g_TB_sound)->play(0x7b, 0, 0, 0.0f);
        res = 1;
    }
    return res;
}

int TouchButton::getWidth() {
    return this->width;
}

void TouchButton::setPosition(int x, int y) {
    setPosition(x, y, this->flags0);
}

void TouchButton::setGamePadButtonImage(unsigned int image) {
    this->adornImage = static_cast<int>(image);
}

void TouchButton::setHalfTransparent(bool value) {
    this->halfTransparent = value;
}

void TouchButton::setPressProgressHighlight(bool value) {
    this->progressHighlight = value;
}

int TouchButton::getHeight() {
    return this->height;
}

// Android 2.0.16 .rodata at 0x1fb888..0x1fb8c8 and 0x2060bc.
static constexpr uint32_t g_TB_iconTexIds[2][4] = {
    {0x2198ff2f, 0xfad10eff, 0xffffffff, 0xce8258ff},
    {0xfa792160, 0xfa7921ff, 0xffffffff, 0xce8258ff},
};
static constexpr uint16_t g_TB_iconImgIds[2][4] = {
    {0x970, 0x96e, 0x96f, 0x96d},
    {0x1f6d, 0x1f63, 0x96f, 0x96d},
};
static constexpr uint16_t g_TB_medalSmallIds[45] = {
    0x948, 0x949, 0x94a, 0x94b, 0x94c, 0x94d, 0x94e, 0x94f, 0x950,
    0x951, 0x952, 0x953, 0x954, 0x955, 0x956, 0x957, 0x958, 0x959,
    0x95a, 0x95b, 0x95c, 0x95d, 0x95e, 0x95f, 0x960, 0x961, 0x962,
    0x963, 0x964, 0x965, 0x966, 0x967, 0x968, 0x969, 0x96a, 0x96b,
    0x1f64, 0x1f65, 0x1f66, 0x1f67, 0x1f68, 0x1f69, 0x1f6a, 0x1f6b,
    0x1f6c,
};

static unsigned int TB_iconTexId(int eliteVariant, int stage) {
    return g_TB_iconTexIds[eliteVariant != 0][stage];
}

static unsigned short TB_iconImgId(int eliteVariant, int stage) {
    return g_TB_iconImgIds[eliteVariant != 0][stage];
}

static unsigned short TB_medalSmallId(int medalId) {
    return g_TB_medalSmallIds[medalId];
}

// Android 2.0.16 .rodata at 0x206170 (iPad) and 0x206464 (other devices).
static constexpr uint32_t g_TB_frameIds[2][21][9] = {
    {
        {0x45c, 0x45b, 0x45d, 0x462, 0x461, 0x463, 0x462, 0x461, 0x463},
        {0x465, 0x464, 0x467, 0x469, 0x468, 0x46b, 0x46d, 0x46c, 0x46f},
        {0x45a, 0x45b, 0x45d, 0x460, 0x461, 0x463, 0x460, 0x461, 0x463},
        {0x465, 0x464, 0x466, 0x469, 0x468, 0x46a, 0x46d, 0x46c, 0x46e},
        {0x45c, 0x45b, 0x45d, 0x462, 0x461, 0x463, 0x462, 0x461, 0x463},
        {0x458, 0x45b, 0x45d, 0x45e, 0x461, 0x463, 0x45e, 0x461, 0x463},
        {0x45c, 0x45b, 0x459, 0x462, 0x461, 0x45f, 0x462, 0x461, 0x45f},
        {0x6b1, 0x6b0, 0x6b2, 0x6b6, 0x6b5, 0x6ad, 0x6b6, 0x6b5, 0x6ad},
        {0x6ae, 0x6b0, 0x6b2, 0x6b3, 0x6b5, 0x6ad, 0x6b3, 0x6b5, 0x6ad},
        {0x6b1, 0x6b0, 0x6af, 0x6b6, 0x6b5, 0x6b4, 0x6b6, 0x6b5, 0x6b4},
        {0x45c, 0x45b, 0x45d, 0x462, 0x461, 0x463, 0x462, 0x461, 0x463},
        {0x2522, 0x2524, 0x2526, 0x2523, 0x2525, 0x2527, 0x2523, 0x2525, 0x2527},
    },
    {
        {0x45c, 0x45b, 0x45d, 0x462, 0x461, 0x463, 0x462, 0x461, 0x463},
        {0x465, 0x464, 0x467, 0x469, 0x468, 0x46b, 0x46d, 0x46c, 0x46f},
        {0x45a, 0x45b, 0x45d, 0x460, 0x461, 0x463, 0x460, 0x461, 0x463},
        {0x465, 0x464, 0x466, 0x469, 0x468, 0x46a, 0x46d, 0x46c, 0x46e},
        {0x45c, 0x45b, 0x45d, 0x462, 0x461, 0x463, 0x462, 0x461, 0x463},
        {0x458, 0x45b, 0x45d, 0x45e, 0x461, 0x463, 0x45e, 0x461, 0x463},
        {0x45c, 0x45b, 0x459, 0x462, 0x461, 0x45f, 0x462, 0x461, 0x45f},
        {0x45c, 0x45b, 0x45d, 0x462, 0x461, 0x463, 0x462, 0x461, 0x463},
        {0x458, 0x45b, 0x45d, 0x45e, 0x461, 0x463, 0x45e, 0x461, 0x463},
        {0x45c, 0x45b, 0x459, 0x462, 0x461, 0x45f, 0x462, 0x461, 0x45f},
        {0x45c, 0x45b, 0x45d, 0x462, 0x461, 0x463, 0x462, 0x461, 0x463},
        {0x2522, 0x2524, 0x2526, 0x2523, 0x2525, 0x2527, 0x2523, 0x2525, 0x2527},
    },
};
static_assert(g_TB_frameIds[0][7][0] == 0x6b1 && g_TB_frameIds[1][7][0] == 0x45c,
              "TouchButton device frame IDs");

static unsigned short TB_frameId(int useAltSkin, unsigned int kind, int slot) {
    return static_cast<unsigned short>(g_TB_frameIds[useAltSkin ? 0 : 1][kind][slot]);
}

static const char g_TB_emptyStr[] = "";

int TouchButton::init(String const &text, unsigned int subId, int kind, int medalId, int medalStage,
                      int x, int y, int requestedWidth, unsigned char flags0, unsigned char flags1) {
    PaintCanvas *canvas = Globals::Canvas;

    this->kind = (int) kind;
    this->visible = 1;
    this->text = text;
    this->subId = subId;
    this->textColor = -1;
    this->requestedWidth = requestedWidth;
    this->field_0x0 = 0;
    this->field_0x4 = 0;
    this->flags1 = flags1;
    this->flags0 = flags0;
    this->touched = 0;
    this->alwaysPressed = 0;
    this->textOffsetX = 0;
    this->textOffsetY = 0;
    this->leftWidth = 0;
    this->midWidth = 0;
    this->rightWidth = 0;
    this->progressHighlight = 1;
    this->pressProgress = 0;
    this->touchMargin = 0;
    this->height = 0;
    this->initX = x;
    this->initY = y;

    this->numberText = g_TB_emptyStr;
    this->splitText = g_TB_emptyStr;
    this->adornImage = -1;

    switch (kind) {
        case 4: {
            Achievements *ach = static_cast<Achievements *>(Globals::achievements);
            int elite = (ach->isEliteMedal(medalId) != 0) ? 1 : 0;
            this->iconTexId = TB_iconTexId(elite, medalStage);
            canvas->Image2DCreate(TB_iconImgId(elite, medalStage), this->iconImage);
            this->height = canvas->GetImage2DHeight(this->iconImage);
            int w = canvas->GetImage2DWidth(this->iconImage);
            this->layoutHeight = this->height;
            this->width = w;
            this->leftWidth = w;
            this->textOffsetY = this->height + 5;
            int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, this->text);
            this->iconOverlay = -1;
            this->textOffsetX = w / 2 - tw / 2;
            this->iconSmall = -1;
            unsigned int iconSmallH;
            canvas->Image2DCreate(TB_medalSmallId(medalId), iconSmallH);
            this->iconSmall = iconSmallH;
            if (medalStage != 0 || ach->isEliteMedal(medalId) != 0) {
                unsigned int iconOverlayH;
                ((PaintCanvas *) (canvas))->Image2DCreate(0x96c, iconOverlayH);
                this->iconOverlay = iconOverlayH;
            }
            break;
        }
        case 10: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(9000, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x2329, frameH);
            this->imgFrameTL = frameH;
            this->imgFrameBL = this->imgFrameTL;
            this->height = canvas->GetImage2DHeight(this->imgFrameL);
            int w = canvas->GetImage2DWidth(this->imgFrameL);
            this->layoutHeight = this->height;
            this->width = w;
            this->leftWidth = w;
            int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, this->text);
            this->textOffsetX = w / 2 - tw / 2;
            this->textOffsetY = ((PaintCanvas *) (canvas))->GetTextHeight(this->fontId);
            this->touchMargin = Globals::layout->field_0x80_touchMargin;
            break;
        }
        case 0xc: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x472, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x473, frameH);
            this->imgFrameTL = frameH;
            {
                this->height = canvas->GetImage2DHeight(this->imgFrameL);
                int w = canvas->GetImage2DWidth(this->imgFrameL);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = canvas->GetImage2DWidth(subId);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = canvas->GetImage2DHeight(subId);
                this->textOffsetY = h / 2 - th / 2;
            }
            break;
        }
        case 0xd: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x517, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x518, frameH);
            this->imgFrameTL = frameH;
            this->imgFrameBL = this->imgFrameTL;
            {
                this->height = canvas->GetImage2DHeight(this->imgFrameL);
                int w = canvas->GetImage2DWidth(this->imgFrameL);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = canvas->GetImage2DWidth(subId);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = canvas->GetImage2DHeight(subId);
                this->textOffsetY = h / 2 - th / 2;
            }
            break;
        }
        case 0xe: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x53c, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x53b, frameH);
            this->imgFrameTL = frameH;
            goto wide_text_layout;
        }
        case 0xf: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x53e, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x53d, frameH);
            this->imgFrameTL = frameH;
            goto wide_text_layout;
        }
        case 0x10: {
            unsigned int frameH;
            this->imgFrameL = subId;
            ((PaintCanvas *) (canvas))->Image2DCreate(0xbb9, frameH);
            this->imgFrameTL = frameH;
            {
                this->height = canvas->GetImage2DHeight(this->imgFrameL);
                int w = canvas->GetImage2DWidth(this->imgFrameL);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = canvas->GetImage2DWidth(subId);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = canvas->GetImage2DHeight(subId);
                this->textOffsetY = h / 2 - th / 2;
            }
            break;
        }
        case 0x11: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0xbc0, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0xbc1, frameH);
            this->imgFrameTL = frameH;
            goto wide_text_layout;
        }
        case 0x12: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0xbc0, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0xbc1, frameH);
            this->imgFrameTL = frameH;
            goto wide_text_layout;
        }
        case 0x14: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x1f6e, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x1f6f, frameH);
            this->imgFrameTL = frameH;
            goto wide_text_layout;
        }
        case 0x13:
            this->imgFrameL = subId;
            this->imgFrameTL = (int) this->image;
            {
                this->height = canvas->GetImage2DHeight(this->imgFrameL);
                int w = canvas->GetImage2DWidth(this->imgFrameL);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = canvas->GetImage2DWidth(subId);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = canvas->GetImage2DHeight(subId);
                this->textOffsetY = h / 2 - th / 2;
            }
            break;
        default: {
            int alt = Globals::iPad != 0 ? 1 : 0;
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 0), frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 1), frameH);
            this->imgFrameM = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 2), frameH);
            this->imgFrameR = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 3), frameH);
            this->imgFrameTL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 4), frameH);
            this->imgFrameT = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 5), frameH);
            this->imgFrameTR = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 6), frameH);
            this->imgFrameBL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 7), frameH);
            this->imgFrameB = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_frameId(alt, kind, 8), frameH);
            this->imgFrameBR = frameH;

            this->height = canvas->GetImage2DHeight(this->imgFrameTL);
            this->leftWidth = canvas->GetImage2DWidth(this->imgFrameTL);
            this->midWidth = canvas->GetImage2DWidth(this->imgFrameT);
            int rightW = canvas->GetImage2DWidth(this->imgFrameTR);
            this->rightWidth = rightW;

            if (kind != 0xb)
                this->layoutHeight = Globals::layout->touchButtonLayoutHeight;
            else
                this->layoutHeight = this->height;

            if (kind < 7 && ((1 << (kind & 0xff)) & 0x61) != 0) {
                this->rightWidth = rightW - 2;
            } else if ((kind - 7) < 3 && Globals::iPad != 0) {
                int hh;
                if (Globals::iPadHD != 0)
                    hh = 0x46;
                else
                    hh = (Globals::iPadLarge != 0) ? 0x64 : 0x32;
                this->layoutHeight = hh;
            }
            setText(this->text);
            break;
        }
    }

    goto done;

wide_text_layout: {
        unsigned int lang = GameText::getLanguage();
        float factor;
        if ((lang & 0xffff) < 0x10 && ((1 << (lang & 0xff)) & 0x8c00) != 0)
            factor = 1.0f;
        else
            factor = (lang == 0xe) ? 1.0f : 1.5f;

        this->imgFrameBL = this->imgFrameTL;
        this->height = canvas->GetImage2DHeight(this->imgFrameL);
        int w = canvas->GetImage2DWidth(this->imgFrameL);
        this->layoutHeight = this->height;
        this->width = w;
        this->leftWidth = w;
        int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, this->text);
        int h = this->height;
        this->textOffsetX = w / 2 - tw / 2;
        int th = ((PaintCanvas *) (canvas))->GetTextHeight(this->fontId);
        this->textOffsetY = (int) ((float) (h / 2) + factor * (float) th);
        this->touchMargin = Globals::layout->field_0x80_touchMargin;
    }

done:
    setPosition(x, y, flags0);
    return 0;
}

TouchButton::TouchButton(unsigned int kind, int a, int b, int c, int d,
                         unsigned char flags0, unsigned char flags1) {
    this->fontId = Globals::font;
    this->fontSpacing = Globals::Canvas->FontGetSpacing(this->fontId);

    String tmp(g_TB_emptyStr, false);
    this->init(tmp, kind, a, -1, -1, b, c, d, flags0, flags1);
}

// Android 2.0.16 TouchButton::draw literal at 0x16991c: these kinds skip the three-slice pattern.
static constexpr unsigned int g_TB_d_noPatternMask = 0x001ef400;
static_assert((g_TB_d_noPatternMask & (1u << 10)) != 0 &&
              (g_TB_d_noPatternMask & (1u << 11)) == 0, "TouchButton frame pattern kinds");

void TouchButton::draw() {
    PaintCanvas *canvas = Globals::Canvas;
    unsigned int savedColor = canvas->GetColor();

    if (this->visible == 0)
        return;

    if (this->halfTransparent != 0) {
        canvas->SetColor(0xffffff2f);
        Globals::layout->setDrawColor(-0xd1);
    } else {
        canvas->SetColor(0xffffffff);
    }

    short savedSpacing = (short) ((PaintCanvas *) (canvas))->FontGetSpacing(this->fontId);
    ((PaintCanvas *) (canvas))->FontSetSpacing(this->fontId, (short) this->fontSpacing);

    unsigned int kind = this->kind;
    int icon = -1;
    int iconX = 0;
    int iconY = 0;
    bool tailIcon = false;

    if (kind == 0x10) {
        ((PaintCanvas *) (canvas))->DrawImage2D(this->imgFrameL, this->x, this->y);
        if (this->touched != 0 || this->alwaysPressed != 0) {
            icon = this->imgFrameTL;
            iconX = this->x;
            iconY = this->y;
            tailIcon = true;
        }
    } else if (kind == 4) {
        ((PaintCanvas *) (canvas))->DrawImage2D(this->iconImage, this->x, this->y);
        if (this->iconSmall != -1) {
            canvas->SetColor(this->iconTexId);
            ((PaintCanvas *) (canvas))->DrawImage2D(this->iconSmall, this->x + (this->width >> 1),
                                                    (this->y + (this->height >> 1)) - 1, (unsigned char) 0x11,
                                                    (unsigned char) 0x44);
            canvas->SetColor(0xffffffff);
            if (this->touched != 0 || this->alwaysPressed != 0)
                ((PaintCanvas *) (canvas))->DrawImage2D(this->iconOverlay, this->x, this->y);
        }
        canvas->SetColor(this->iconSmall == -1 ? 0x777777ff : 0xffffffff);
        ((PaintCanvas *) (canvas))->DrawString(this->fontId, this->text, this->x + this->textOffsetX,
                                               this->y + this->textOffsetY, false);
    } else {
        int base;
        if (this->touched != 0)
            base = this->imgFrameTL;
        else if (this->alwaysPressed != 0)
            base = this->imgFrameBL;
        else
            base = this->imgFrameL;

        if (kind > 0x14 || ((1u << kind) & g_TB_d_noPatternMask) == 0) {
            unsigned int frameLeft;
            int frameMid;
            if (this->touched != 0) {
                frameLeft = (unsigned int) this->imgFrameT;
                frameMid = this->imgFrameTR;
            } else if (this->alwaysPressed != 0) {
                frameLeft = (unsigned int) this->imgFrameB;
                frameMid = this->imgFrameBR;
            } else {
                frameLeft = (unsigned int) this->imgFrameM;
                frameMid = this->imgFrameR;
            }
            Globals::layout->drawBGPattern(frameLeft, this->leftWidth + this->x, this->y, this->midStretch,
                                           this->height);
            ((PaintCanvas *) (canvas))->DrawImage2D(frameMid, this->x + this->leftWidth + this->midStretch, this->y);
        }
        ((PaintCanvas *) (canvas))->DrawImage2D(base, this->x, this->y);

        Layout *layoutC = Globals::layout;
        layoutC->setDrawColor(-1);

        float prog = this->pressProgress;
        if (prog > 0.0f) {
            canvas->SetColor(0xffffff80);
            layoutC->setDrawColor(-0x80);
            int span = this->leftWidth;
            int total = this->rightWidth + this->midStretch + span;
            int filled = (int) (prog * (float) total);
            int leftImg = (this->progressHighlight == 0) ? this->imgFrameL : this->imgFrameTL;
            int drawW = (filled < span) ? filled : span;
            canvas->DrawRegion2D((unsigned int) leftImg, 0, 0, drawW, this->height, 0.0f,
                                 0, 0, this->x, this->y);

            int mid = this->leftWidth;
            if (mid < filled) {
                int midImg = (this->progressHighlight == 0) ? this->imgFrameM : this->imgFrameT;
                int patW = this->midStretch;
                if (filled - mid < patW) patW = filled - mid;
                layoutC->drawBGPattern((unsigned int) midImg, mid + this->x, this->y, patW, this->height);
                mid = this->leftWidth;
            }
            int rstart = this->midStretch + mid;
            if (rstart < filled) {
                int rImg = (this->progressHighlight == 0) ? this->imgFrameR : this->imgFrameTR;
                int rW = (filled - mid) - this->midStretch;
                if (this->rightWidth < rW) rW = this->rightWidth;
                canvas->DrawRegion2D((unsigned int) rImg, 0, 0, rW, this->height, 0.0f,
                                     0, 0, rstart + this->x, this->y);
            }
            layoutC->setDrawColor(-1);
        }

        unsigned int lblColor = (unsigned int) this->textColor;
        if (this->halfTransparent != 0)
            canvas->SetColor((unsigned char) (lblColor >> 24), (unsigned char) (lblColor >> 16),
                             (unsigned char) (lblColor >> 8), (unsigned char) 0x2f);
        else
            canvas->SetColor(lblColor);

        if (this->subId == -1) {
            ((PaintCanvas *) (canvas))->DrawString(this->fontId, this->text, this->x + this->textOffsetX,
                                                   this->y + this->textOffsetY, false);

            if (this->splitText.size() != 0) {
                String *t = &this->splitText;
                int px = this->x;
                int w = this->width;
                int tx, ty;
                if (this->kind == 10) {
                    int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, *t);
                    int th = canvas->GetTextHeight(Globals::font);
                    ty = this->y + this->height + th * -2;
                    tx = (px + w / 2) - tw / 2;
                } else {
                    int off = this->textOffsetX;
                    int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, *t);
                    ty = this->y + this->textOffsetY;
                    tx = ((w + px) - off) - tw;
                }
                ((PaintCanvas *) (canvas))->DrawString(this->fontId, this->splitText, tx, ty, false);
            }

            if (this->numberText.size() != 0) {
                canvas->SetColor(0x777777ff);
                int tw = canvas->GetTextWidth(Globals::font, this->numberText);
                canvas->DrawString(Globals::font, this->numberText, (this->leftWidth + this->x) - tw,
                                   this->y + this->textOffsetY, false);
                canvas->SetColor(lblColor);
            }

            if (this->adornImage != -1) {
                canvas->SetColor(0xffffffff);
                ((PaintCanvas *) (canvas))->DrawImage2D(this->adornImage, (this->x + this->width + 6) - this->leftWidth,
                                                        this->y + 1, (unsigned char) 0x11, (unsigned char) 0x14);
                canvas->SetColor(lblColor);
            }
        } else if (this->kind != 0x13) {
            icon = this->subId;
            iconX = this->x + this->textOffsetX;
            iconY = this->y + this->textOffsetY;
            tailIcon = true;
        }
    }

    if (tailIcon)
        canvas->DrawImage2D(icon, iconX, iconY);

    Globals::layout->setDrawColor(-1);
    ((PaintCanvas *) (canvas))->FontSetSpacing(this->fontId, savedSpacing);
    canvas->SetColor(savedColor);
}

TouchButton::TouchButton(String const &text, int type, int x, int y, int p5, unsigned char p6, unsigned char p7) {
    this->fontId = Globals::font;
    this->fontSpacing = Globals::Canvas->FontGetSpacing(this->fontId);
    init(text, 0xffffffff, type, -1, -1, x, y, p5, p6, p7);
}

void TouchButton::setText(String const &text) {
    this->text = text;
    int w = Globals::Canvas->GetTextWidth(this->fontId, this->text);
    if (this->subId != -1)
        w = Globals::Canvas->GetImage2DWidth(this->subId);
    int a94 = this->leftWidth;
    int x;
    if (this->requestedWidth < 1)
        x = w;
    else
        x = (this->requestedWidth - a94) - this->rightWidth;
    this->midStretch = x;
    x = this->rightWidth + x + a94;
    this->width = x;
    unsigned char fl = this->flags1;
    if ((fl & 2) == 0) {
        if ((fl & 1) != 0) {
            this->textOffsetX = a94;
            goto height;
        }
        x = (x - w) / 2;
        this->textOffsetX = x;
        if (this->kind == 6) {
            x = x + -5;
        } else {
            if (this->kind != 5)
                goto height;
            x = x + 5;
        }
    } else {
        x = x - (w + a94);
    }
    this->textOffsetX = x;
height:
    int h = this->height;
    int th = Globals::Canvas->GetTextHeight(this->fontId);
    th = (h - th) / 2;
    this->textOffsetY = th;
    if (this->kind == 3)
        this->textOffsetY = th + 2;
    if (this->subId != -1) {
        h = this->height;
        int ih = Globals::Canvas->GetImage2DHeight(this->subId);
        this->textOffsetY = (h - ih) / 2;
        if (this->kind == 1)
            this->textOffsetX = this->textOffsetX + 3;
    }
    setPosition(this->initX, this->initY, this->flags0);
}

void TouchButton::setYPosition(int y) {
    setPosition(this->x, y, this->flags0);
}

bool TouchButton::touchedInside(int px, int py) {
    int x = this->x;
    int xm1 = x - 1;
    int h;
    int top;
    if (this->kind == 3) {
        int v = Globals::layout->field_0x38;
        if (xm1 + v > px)
            return false;
        if (this->width + ((x + 1) - v) <= px)
            return false;
        top = this->y;
        if (top - 1 > py)
            return false;
        h = this->layoutHeight;
    } else {
        int m = this->touchMargin;
        if (xm1 - m > px)
            return false;
        if (this->width + (x + m) + 1 <= px)
            return false;
        top = this->y + ~m;
        if (top > py)
            return false;
        h = this->layoutHeight;
        top = this->y + m;
    }
    return h + top + 1 >= py;
}

TouchButton::TouchButton(unsigned int kind, unsigned int image,
                         int a, int b, int c, unsigned char flag) {
    this->image = image;
    this->fontId = Globals::font;
    this->fontSpacing = Globals::Canvas->FontGetSpacing(this->fontId);

    String tmp(g_TB_emptyStr, false);
    this->init(tmp, kind, a, -1, -1, b, c, 0, flag, 0x44);
}

TouchButton::TouchButton(String const &text, int x, int y, int p4, unsigned char p5) {
    this->fontId = Globals::font;
    this->fontSpacing = Globals::Canvas->FontGetSpacing(this->fontId);
    init(text, 0xffffffff, x, -1, -1, y, p4, 0, p5, 0x44);
}

TouchButton::TouchButton(String const &text,
                         int a, int b, int c, int d,
                         unsigned char flags0, unsigned char flags1,
                         unsigned int spacing, int kerning) {
    this->fontSpacing = kerning;
    this->fontId = spacing;

    PaintCanvas *canvas = Globals::Canvas;
    short prev = canvas->FontGetSpacing(this->fontId);
    canvas->FontSetSpacing(spacing, (short) kerning);

    this->init(text, 0xffffffff, a, -1, -1, b, c, d, flags0, flags1);

    canvas->FontSetSpacing(spacing, prev);
}

TouchButton::TouchButton(int x, int y, String const &text, int p4, int p5, unsigned char p6) {
    this->fontId = Globals::font;
    this->fontSpacing = Globals::Canvas->FontGetSpacing(this->fontId);
    init(text, 0xffffffff, 4, x, y, p4, p5, 0, p6, 0x44);
}

TouchButton::TouchButton(unsigned int kind, int a, int b, int c, unsigned char flag) {
    this->fontId = Globals::font;
    this->fontSpacing = Globals::Canvas->FontGetSpacing(this->fontId);

    String tmp(g_TB_emptyStr, false);
    this->init(tmp, kind, a, -1, -1, b, c, 0, flag, 0x44);
}
