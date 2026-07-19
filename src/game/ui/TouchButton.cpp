#include "game/ui/TouchButton.h"
#include "engine/audio/FModSound.h"
#include "game/mission/Achievements.h"
#include "game/ui/Layout.h"
#include "game/core/String.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/GameText.h"

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

uint8_t TouchButton::isTouched() {
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

unsigned int TB_iconTexId(int eliteVariant, int stage);

unsigned short TB_iconImgId(int eliteVariant, int stage);

unsigned short TB_medalSmallId(int achId);

unsigned short TB_frameId(int useAltSkin, unsigned int kind, int slot);

static char *g_TB_useAltSkin = nullptr;
static char *g_TB_langWide = nullptr;
static char *g_TB_langWide2 = nullptr;
static Layout **g_TB_layoutMetrics = nullptr;
static const char g_TB_emptyStr[] = "";

int TouchButton::init(String const &text, unsigned int kind, int achId, int achStage, int width, int d_unused, int x,
                      int y, unsigned char flags0, unsigned char flags1) {
    void *canvas = PaintCanvas::gCanvas;

    this->kind = (int) kind;
    this->visible = 1;
    this->text = text;
    this->subId = achStage;
    this->textColor = -1;
    this->requestedWidth = width;
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
            void *ach = Achievements::gAchievements;
            int elite = (((Achievements *) (ach))->isEliteMedal(achId) != 0) ? 1 : 0;
            this->iconTexId = TB_iconTexId(elite, achStage);
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_iconImgId(elite, achStage), this->iconImage);
            this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
            int w = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
            this->layoutHeight = this->height;
            this->width = w;
            this->leftWidth = w;
            this->textOffsetY = this->height + 5;
            int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, this->text);
            this->iconOverlay = -1;
            this->textOffsetX = w / 2 - tw / 2;
            this->iconSmall = -1;
            unsigned int iconSmallH;
            ((PaintCanvas *) (canvas))->Image2DCreate(TB_medalSmallId(achId), iconSmallH);
            this->iconSmall = iconSmallH;
            if (achStage != 0 || ((Achievements *) (ach))->isEliteMedal(achId) != 0) {
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
            this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
            int w = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
            this->layoutHeight = this->height;
            this->width = w;
            this->leftWidth = w;
            int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, this->text);
            this->textOffsetX = w / 2 - tw / 2;
            this->textOffsetY = ((PaintCanvas *) (canvas))->GetTextHeight(this->fontId);
            this->touchMargin = (*g_TB_layoutMetrics)->field_0x80_touchMargin;
            break;
        }
        case 0xc: {
            unsigned int frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x472, frameH);
            this->imgFrameL = frameH;
            ((PaintCanvas *) (canvas))->Image2DCreate(0x473, frameH);
            this->imgFrameTL = frameH;
            {
                this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
                int w = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
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
                this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
                int w = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
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
            this->imgFrameL = achStage;
            ((PaintCanvas *) (canvas))->Image2DCreate(0xbb9, frameH);
            this->imgFrameTL = frameH;
            {
                this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
                int w = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
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
            this->imgFrameL = achStage;
            this->imgFrameTL = (int) this->image;
            {
                this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
                int w = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                this->layoutHeight = this->height;
                this->width = w;
                this->leftWidth = w;
                int tw = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
                int h = this->height;
                this->textOffsetX = w / 2 - tw / 2;
                int th = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
                this->textOffsetY = h / 2 - th / 2;
            }
            break;
        default: {
            int alt = (*g_TB_useAltSkin != 0) ? 1 : 0;
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

            this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
            this->leftWidth = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
            this->midWidth = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
            int rightW = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
            this->rightWidth = rightW;

            if (kind != 0xb)
                this->layoutHeight = (*g_TB_layoutMetrics)->field_0x30;
            else
                this->layoutHeight = this->height;

            if (kind < 7 && ((1 << (kind & 0xff)) & 0x61) != 0) {
                this->rightWidth = rightW - 2;
            } else if ((kind - 7) < 3 && *g_TB_useAltSkin != 0) {
                int hh;
                if (*g_TB_langWide != 0)
                    hh = 0x46;
                else
                    hh = (*g_TB_langWide2 != 0) ? 0x64 : 0x32;
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
        this->height = ((PaintCanvas *) (canvas))->GetImage2DHeight(0);
        int w = ((PaintCanvas *) (canvas))->GetImage2DWidth(0);
        this->layoutHeight = this->height;
        this->width = w;
        this->leftWidth = w;
        int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, this->text);
        int h = this->height;
        this->textOffsetX = w / 2 - tw / 2;
        int th = ((PaintCanvas *) (canvas))->GetTextHeight(this->fontId);
        this->textOffsetY = (int) ((float) (h / 2) + factor * (float) th);
        this->touchMargin = (*g_TB_layoutMetrics)->field_0x80_touchMargin;
    }

done:
    setPosition(x, y, flags0);
    return 0;
}

static unsigned int *g_TB_defSpacing = nullptr;

TouchButton::TouchButton(unsigned int kind, int a, int b, int c, int d,
                         unsigned char flags0, unsigned char flags1) {
    void *canvas = PaintCanvas::gCanvas;
    this->fontId = *g_TB_defSpacing;
    this->fontSpacing = ((PaintCanvas *) (canvas))->FontGetSpacing(this->fontId);

    String tmp(g_TB_emptyStr, false);
    this->init(tmp, kind, a, b, c, d, -1, -1, flags0, flags1);
}

static Layout **g_TB_d_layoutA = nullptr;
static Layout **g_TB_d_layoutBG = nullptr;
static Layout **g_TB_d_layoutC = nullptr;
static Layout **g_TB_d_layoutEnd = nullptr;
static String **g_TB_d_unitStr = nullptr;
static unsigned int g_TB_d_frameMask = 0;

void TouchButton::draw() {
    void *canvas = PaintCanvas::gCanvas;
    unsigned int savedColor = ((PaintCanvas *) (canvas))->GetColor();

    if (this->visible == 0)
        return;

    if (this->halfTransparent != 0) {
        PaintCanvas::gCanvas->SetColor(0xffffff2f);
        (*g_TB_d_layoutA)->setDrawColor(-0xd1);
    } else {
        PaintCanvas::gCanvas->SetColor(0xffffffff);
    }

    short savedSpacing = (short) ((PaintCanvas *) (canvas))->FontGetSpacing(this->fontId);
    ((PaintCanvas *) (canvas))->FontSetSpacing(this->fontId, (short) this->fontSpacing);

    unsigned int kind = this->kind;
    int icon = -1;
    int iconY = 0;
    bool tailIcon = false;

    if (kind == 0x10) {
        ((PaintCanvas *) (canvas))->DrawImage2D(this->imgFrameL, this->x, this->y);
        if (this->touched != 0 || this->alwaysPressed != 0) {
            icon = this->imgFrameTL;
            iconY = this->x;
            tailIcon = true;
        }
    } else if (kind == 4) {
        ((PaintCanvas *) (canvas))->DrawImage2D(this->iconImage, this->x, this->y);
        if (this->iconSmall != -1) {
            PaintCanvas::gCanvas->SetColor(0xffffffff);
            ((PaintCanvas *) (canvas))->DrawImage2D(this->iconSmall, this->x + (this->width >> 1),
                                                    (this->y + (this->height >> 1)) - 1, (unsigned char) 0x11,
                                                    (unsigned char) 0x44);
            PaintCanvas::gCanvas->SetColor(0xffffffff);
            if (this->touched != 0 || this->alwaysPressed != 0)
                ((PaintCanvas *) (canvas))->DrawImage2D(this->iconOverlay, this->x, this->y);
        }
        PaintCanvas::gCanvas->SetColor(0xffffffff);
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

        if (kind <= 0x14 && ((1u << (kind & 0xff)) & g_TB_d_frameMask) != 0) {
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
            (*g_TB_d_layoutBG)->drawBGPattern(frameLeft, this->leftWidth + this->x, this->y, this->midStretch,
                                              this->height);
            ((PaintCanvas *) (canvas))->DrawImage2D(frameMid, this->x + this->leftWidth + this->midStretch, this->y);
        }
        ((PaintCanvas *) (canvas))->DrawImage2D(base, this->x, this->y);

        Layout *layoutC = *g_TB_d_layoutC;
        layoutC->setDrawColor(-1);

        float prog = this->pressProgress;
        if (prog > 0.0f) {
            PaintCanvas::gCanvas->SetColor(0xffffffff);
            layoutC->setDrawColor(-0x80);
            int span = this->leftWidth;
            int total = this->rightWidth + this->midStretch + span;
            int filled = (int) (prog * (float) total);
            int leftImg = (this->progressHighlight == 0) ? this->imgFrameL : this->imgFrameTL;
            int drawW = (filled < span) ? filled : span;
            ((PaintCanvas *) (canvas))->DrawRegion2D((unsigned int) leftImg, 0, 0, drawW, this->height, (float) filled,
                                                     0, 0, 0, this->x);

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
                ((PaintCanvas *) (canvas))->DrawRegion2D((unsigned int) rImg, 0, 0, rW, this->height, (float) filled, 0,
                                                         0, 0, rstart + this->x);
            }
            layoutC->setDrawColor(-1);
        }

        unsigned int lblColor = (unsigned int) this->textColor;
        if (this->halfTransparent != 0)
            PaintCanvas::gCanvas->SetColor((unsigned char) (lblColor >> 16), (unsigned char) (lblColor >> 8),
                              (unsigned char) lblColor, (unsigned char) (lblColor >> 24));
        else
            PaintCanvas::gCanvas->SetColor(0xffffffff);

        if (this->subId == -1) {
            ((PaintCanvas *) (canvas))->DrawString(this->fontId, this->text, this->x + this->textOffsetX,
                                                   this->y + this->textOffsetY, false);

            if (this->splitText.size() != 0) {
                String *t = &this->text;
                int px = this->x;
                int w = this->width;
                int tx, ty;
                if (this->kind == 10) {
                    int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, *t);
                    int th = ((PaintCanvas *) (canvas))->GetTextHeight(this->fontId);
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
                PaintCanvas::gCanvas->SetColor(0xffffffff);
                String *u = *g_TB_d_unitStr;
                int tw = ((PaintCanvas *) (canvas))->GetTextWidth(this->fontId, *u);
                ((PaintCanvas *) (canvas))->DrawString(this->fontId, this->numberText, (this->leftWidth + this->x) - tw,
                                                       this->y + this->textOffsetY, false);
                PaintCanvas::gCanvas->SetColor(0xffffffff);
            }

            if (this->adornImage != -1) {
                PaintCanvas::gCanvas->SetColor(0xffffffff);
                ((PaintCanvas *) (canvas))->DrawImage2D(this->adornImage, (this->x + this->width + 6) - this->leftWidth,
                                                        this->y + 1, (unsigned char) 0x11, (unsigned char) 0x14);
                PaintCanvas::gCanvas->SetColor(0xffffffff);
            }
        } else if (this->kind != 0x13) {
            icon = base;
            iconY = this->textOffsetX + this->x;
            tailIcon = true;
        }
    }

    if (tailIcon)
        ((PaintCanvas *) (canvas))->DrawImage2D(icon, this->x + this->textOffsetX + 0, iconY);

    (*g_TB_d_layoutEnd)->setDrawColor(-1);
    ((PaintCanvas *) (canvas))->FontSetSpacing(this->fontId, savedSpacing);
    PaintCanvas::gCanvas->SetColor(savedColor);
}

static int **g_TB_c1 = nullptr;

TouchButton::TouchButton(String const &text, int type, int x, int y, int p5, unsigned char p6, unsigned char p7) {
    this->fontId = (uint32_t) * *g_TB_c1;
    this->fontSpacing = PaintCanvas::gCanvas->FontGetSpacing(this->fontId);
    init(text, (unsigned int) type, x, y, p5, 0, 0, 0, p6, p7);
}

void TouchButton::setText(String const &text) {
    this->text = text;
    int w = PaintCanvas::gCanvas->GetTextWidth(this->fontId, this->text);
    if (this->subId != -1)
        w = PaintCanvas::gCanvas->GetImage2DWidth(0);
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
    int th = PaintCanvas::gCanvas->GetTextHeight(this->fontId);
    th = (h - th) / 2;
    this->textOffsetY = th;
    if (this->kind == 3)
        this->textOffsetY = th + 2;
    if (this->subId != -1) {
        h = this->height;
        int ih = PaintCanvas::gCanvas->GetImage2DHeight(0);
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
        int v = *reinterpret_cast<int *>(&PaintCanvas::gCanvas->engine);
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
    void *canvas = PaintCanvas::gCanvas;
    this->image = image;
    this->fontId = *g_TB_defSpacing;
    this->fontSpacing = ((PaintCanvas *) (canvas))->FontGetSpacing(this->fontId);

    String tmp(g_TB_emptyStr, false);
    this->init(tmp, kind, a, b, c, 0x44, -1, -1, flag, 0);
}

TouchButton::TouchButton(String const &text, int x, int y, int p4, unsigned char p5) {
    this->fontId = (uint32_t) * *g_TB_c1;
    this->fontSpacing = PaintCanvas::gCanvas->FontGetSpacing(this->fontId);
    init(text, 0xffffffff, 4, x, y, p4, 0, 0, p5, 0x44);
}

TouchButton::TouchButton(String const &text,
                         int a, int b, int c, int d,
                         unsigned char flags0, unsigned char flags1,
                         unsigned int spacing, int kerning) {
    this->fontSpacing = kerning;
    this->fontId = spacing;

    void *canvas = PaintCanvas::gCanvas;
    short prev = ((PaintCanvas *) (canvas))->FontGetSpacing(this->fontId);
    ((PaintCanvas *) (canvas))->FontSetSpacing(spacing, (short) kerning);

    this->init(text, spacing, a, b, c, d, -1, -1, flags0, flags1);

    ((PaintCanvas *) (canvas))->FontSetSpacing(spacing, prev);
}

TouchButton::TouchButton(int x, int y, String const &text, int p4, int p5, unsigned char p6) {
    this->fontId = (uint32_t) * *g_TB_c1;
    this->fontSpacing = PaintCanvas::gCanvas->FontGetSpacing(this->fontId);
    init(text, 0xffffffff, 4, x, y, p4, p5, 0, p6, 0x44);
}

TouchButton::TouchButton(unsigned int kind, int a, int b, int c, unsigned char flag) {
    void *canvas = PaintCanvas::gCanvas;
    this->fontId = *g_TB_defSpacing;
    this->fontSpacing = ((PaintCanvas *) (canvas))->FontGetSpacing(this->fontId);

    String tmp(g_TB_emptyStr, false);
    this->init(tmp, kind, a, b, c, 0x44, -1, -1, flag, 0);
}

