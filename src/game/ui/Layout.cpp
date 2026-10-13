#include "game/ui/Layout.h"
#include "game/ship/Ship.h"
#include "engine/core/AERandom.h"
#include "game/ui/ChoiceWindow.h"
#include "engine/audio/FModSound.h"
#include "game/mission/Status.h"
#include "game/core/Globals.h"
#include "engine/core/GameText.h"
#include "game/core/String.h"
#include "game/ui/TouchButton.h"
#include "engine/render/PaintCanvas.h"


void TouchButton_footerAnim(...);

void Globals_drawLines(...);

uint8_t Layout::isFading() {
    return this->fading;
}


int Layout::getHelpButtonOffset() {
    int w = this->helpButton->getWidth();
    return w - Globals::layout->field_0x38;
}

void Layout::update(int dt) {
    if (this->choiceWindow)
        this->choiceWindow->update(dt);
    if (this->rewardMessageActive != 0) {
        int t = this->rewardMessageTimer + dt;
        this->rewardMessageTimer = t;
        if (t >= 7000)
            this->rewardMessageActive = 0;
    }
    if (this->fading != 0) {
        int t = this->fadeProgress + dt;
        int dur = this->fadeDuration;
        this->fadeProgress = t;
        if (t > dur) {
            this->fading = 0;
            this->fadeProgress = 0;
        }
    }
}


int Layout::getFooterTransitionWidth() {
    PaintCanvas *canvas = Globals::Canvas;
    int w1 = canvas->GetImage2DWidth(this->footerImageRight);
    int w2 = canvas->GetImage2DWidth(this->footerImageLeft);
    return w2 + w1;
}

int Layout::OnTouchBegin(int x, int y) {
    if (this->helpButtonEnabled != 0)
        this->helpButton->OnTouchBegin(x, y);
    if (this->choiceWindow != nullptr && this->choiceWindowOpen != 0) {
        this->choiceWindow->OnTouchBegin(x, y);
        return 0;
    }
    TouchButton *btn = this->backButton->isVisible() == 0
                           ? this->secondaryButton
                           : this->backButton;
    return btn->OnTouchBegin(x, y);
}

float Sinf(float);

float Layout::getPulseValue(float speed) {
    Status *status = Status::gStatus;
    float a = Sinf((float) status->getPlayingTime() * speed);
    float b = Sinf((float) status->getPlayingTime() * speed);
    return a > 0.0f ? b : -b;
}

void Layout::setDrawColor(int color) {
    this->drawColor = color;
}


void Layout::drawBG() {
    this->drawBGPattern(this->bgPatternImage, 0, 0, Globals::w, Globals::h);
}

void Layout::enableFillScreen(bool v) {
    this->fillScreen = v;
}

Layout::~Layout() {
    delete this->backButton;
    this->backButton = nullptr;
    delete this->secondaryButton;
    this->secondaryButton = nullptr;
    delete this->helpButton;
    this->helpButton = nullptr;
    delete this->choiceWindow;
    this->choiceWindow = nullptr;
}

uint8_t Layout::helpPressed() {
    return this->helpPressedFlag;
}


void Layout::drawFooterNoBackButton() {
    this->drawFooter(false, false);
}

void Layout::drawFooter() {
    this->drawFooter(false, true);
}

int Layout::OnTouchMove(int x, int y) {
    if (this->helpButtonEnabled != 0)
        this->helpButton->OnTouchMove(x, y);
    if (this->choiceWindow != nullptr && this->choiceWindowOpen != 0) {
        this->choiceWindow->OnTouchMove(x, y);
        return 0;
    }
    TouchButton *btn = this->backButton->isVisible() == 0
                           ? this->secondaryButton
                           : this->backButton;
    return static_cast<int>(btn->OnTouchMove(x, y));
}

int Layout::OnTouchEnd(int x, int y) {
    if (this->choiceWindowOpen == 0 && this->helpButtonEnabled != 0)
        this->helpPressedFlag = this->helpButton->OnTouchEnd(x, y);
    if (this->choiceWindow != nullptr && this->choiceWindowOpen != 0)
        return this->choiceWindow->OnTouchEnd(x, y) == 0;
    TouchButton *btn = this->backButton->isVisible() == 0
                           ? this->secondaryButton
                           : this->backButton;
    return static_cast<int>(btn->OnTouchEnd(x, y));
}

void Layout::startFade(bool fadeOut, int color, int duration) {
    this->fadeOut = fadeOut;
    this->fading = 1;
    this->fadeColor = color & 0xffffff00u;
    this->fadeProgress = 0;
    this->fadeDuration = duration;
}


void Layout::initTip() {
    if (this->tipLines != nullptr) {
        ArrayReleaseClasses(*this->tipLines);
        delete this->tipLines;
        this->tipLines = nullptr;
    }
    this->tipLines = new Array<String *>();
    unsigned int font = Globals::font;
    GameText *gameText = Globals::gameText;
    PaintCanvas *canvas = Globals::Canvas;
    // Android 2.0.16 selects one of tip text IDs 340..352.
    int textId = reinterpret_cast<AERandom *>(Globals::rnd)->nextInt(13) + 340;
    String *text = gameText->getText(textId);
    int width = Globals::layout->field_0x78 - 2 * Globals::layout->field_0x4c;
    canvas->GetLineArray(font, *text, width, this->tipLines);
}


void Layout::drawEmptyFooter(bool showBack) {
    Globals::Canvas->SetColor(this->drawColor);
    int w = Globals::Canvas->GetImage2DWidth(this->field_0x340);
    Globals::Canvas->DrawImage2D(this->footerImageLeft, 0, Globals::h,
                                  (unsigned char) 0x11, (unsigned char) 0x21);

    int footerH = Globals::layout->field_0x10;
    this->drawBGPattern(this->footerPatternImage, w, Globals::h - footerH,
                        Globals::w - w * 2, footerH);
    Globals::Canvas->DrawImage2D(this->footerImageLeft,
                                  Globals::w - w, Globals::h - Globals::layout->field_0x10,
                                  (unsigned char) 0x01);
    if (showBack) {
        this->backButton->setVisible(true);
        this->backButton->draw();
    }
}


static char *g_cfgHd = nullptr;

static char *g_cfgWide = nullptr;

static char *g_cfgScale = nullptr;

static unsigned char *g_resByte = nullptr;

static char *g_cfgA = nullptr;

static char *g_cfgB = nullptr;

static char *g_cfgC = nullptr;

static char *g_cfgD = nullptr;

Layout::Layout() {
    int hd = (*g_cfgHd != 0);
    int wide = (*g_cfgWide != 0);
    int scale = (*g_cfgScale != 0);
    int res = *g_resByte;

    memset(&this->field_0x4, 0, 0x2d8 - 0x04);

    // Android Layout::Layout @ 0x000d1028. ListItemWindow::draw uses these
    // two fields to place the ImageFactory item/ship Sprite in its title box.
    // Keep the platform branches explicit: they are direct constructor writes,
    // not values inferred from the current 1280x720 shell.
    if (Globals::iPadHD) {
        this->listItemIconTopInsetY = 1;
        this->listItemIconSpriteHeight = 63;
    } else {
        this->listItemIconTopInsetY = (Globals::n9 || Globals::retinaDisplay) ? 2 : 1;
        if (Globals::n9) {
            this->listItemIconSpriteHeight = 45;
        } else if (Globals::iPadLarge) {
            this->listItemIconSpriteHeight = 90;
        } else if (Globals::retinaDisplay) {
            this->listItemIconSpriteHeight = 60;
        } else {
            this->listItemIconSpriteHeight = Globals::iPad ? 45 : 30;
        }
    }

    // Android Layout::Layout @ 0x000d1028, ordinary non-iPad/non-n9 branch.
    // HangarWindow::initialize copies these fields into its list-entry width,
    // selected-row action offset, and icon placement state. Keep this branch
    // separate from the generic UI metrics above: these are direct ARM
    // constructor writes, not values inferred from the Godot reference layout.
    if (!Globals::iPadHD && !Globals::n9 && !Globals::iPad) {
        this->hangarListEntryFixedWidth = Globals::retinaDisplay ? 340 : 170;
        this->hangarSelectedRowActionOffsetY = Globals::retinaDisplay ? -2 : 0;
        this->hangarRowIconOffsetY = Globals::retinaDisplay ? 2 : 1;
    }

    if (!hd) {
        this->field_0x238 = 0x46;
        this->field_0x27c = 0x1c;
        this->field_0x280 = 0;
        this->scrollBarInset = 1;
        this->scrollBarHandle = 0;
        this->rewardBoxWidth = 0x8c;
        this->rewardBoxY = 0x4d;
        this->rewardBoxY2 = 0x84;
    }

    if (hd) {
        this->rewardBoxHeight = (res == 0) ? 0xc4 : 0x188;
        this->rewardBoxX = (res == 0) ? 0x32 : 100;
    } else {
        this->rewardBoxHeight = 0x160;
        this->rewardBoxX = 0x4b;
    }

    int v288, v28c, v3fc_, v200, v1f0, v1f4, v1f8, v2bc;
    if (!hd) {
        int wb = (res != 0) ? 4 : 2;
        if (*g_cfgA == 0) wb = 2;
        int below = (res == 0) ? 0xfffffff3 : 0xffffffe6;
        if (!wide) below = 0xffffffe6;
        int o1f0 = (res == 0) ? 0x13 : 0x26;
        if (!wide) o1f0 = 0x22;
        int o3fc = (res == 0) ? 3 : 6;
        if (!wide) o3fc = 5;
        int o1f8 = (*g_cfgA == 0) ? 2 : 4;
        if (res == 0) o1f8 = 2;
        v1f4 = (*g_cfgA == 0) ? (res ? 0x20 : 0x10) : wb;
        v2bc = wide ? 2 : 3;
        v200 = below;
        v1f0 = o1f0;
        v3fc_ = o3fc;
        v1f8 = o1f8;
        v288 = wide ? 0 : 0x14;
        v28c = wide ? 0 : 0xffffffed;
    } else {
        v200 = 0xffffffee;
        v1f4 = 0x16;
        v1f0 = 0x1a;
        v3fc_ = 4;
        v1f8 = 2;
        v2bc = 2;
        v288 = 0x14;
        v28c = 0xffffffed;
    }
    this->field_0x2bc = v2bc;
    this->footerButtonOffset = v3fc_;
    this->field_0x200 = v200;
    this->field_0x1f0 = v1f0;
    this->field_0x1f4 = v1f4;
    this->field_0x1f8 = v1f8;
    this->field_0x284 = 1;
    this->field_0x285 = 1;
    this->field_0x286 = 1;
    this->field_0x288 = v288;
    this->field_0x28c = v28c;
    this->field_0x290 = wide ? (res == 0 ? 3 : 7) : 4;

    int cwTitle;
    if (!hd) {
        int cw2f4;
        if (*g_cfgWide != 0) {
            cw2f4 = 0xa0;
        } else if (*g_cfgB != 0) {
            cw2f4 = 0x50;
        } else {
            cw2f4 = wide ? 0x3c : 0x2d;
            if (res == 0) cw2f4 = 0x1e;
        }
        this->field_0x2f4 = cw2f4;
        this->field_0x2f8 = 0;
        this->field_0x2fc = (*g_cfgWide != 0) ? 0x48 : 0x24;
        this->field_0x300 = (*g_cfgWide != 0) ? 400 : 200;
        this->field_0x304 = (*g_cfgWide != 0) ? 700 : 0x15e;
        cwTitle = (*g_cfgWide != 0) ? 500 : 0xfa;
    } else {
        this->field_0x2f4 = 0x70;
        this->field_0x2f8 = 0;
        this->field_0x2fc = 0x32;
        this->field_0x300 = 0x119;
        this->field_0x304 = 0x1ec;
        cwTitle = 0x15f;
    }
    this->choiceWindowTitleHeight = cwTitle;

    int b318;
    if (!hd) {
        if (*g_cfgC == 0) {
            if (wide) {
                this->field_0x30c = 7;
                this->field_0x310 = (res == 0) ? 0xfffffffb : 0xfffffff7;
                this->field_0x314 = (res == 0) ? 0xfffffffb : 0xfffffff8;
            } else {
                this->field_0x30c = 4;
                this->field_0x310 = 0xfffffff8;
                this->field_0x314 = 0xfffffff8;
            }
            b318 = 0;
        } else {
            this->field_0x30c = 3;
            this->field_0x310 = 0xfffffff4;
            this->field_0x314 = 0xfffffffb;
            b318 = 4;
        }
    } else {
        this->field_0x30c = 6;
        this->field_0x310 = 0xffffffe8;
        this->field_0x314 = 0xfffffff6;
        b318 = 8;
    }
    this->field_0x318 = b318;

    if (!hd) {
        if (wide) {
            this->helpWindowX = (*g_cfgD != 0) ? 0x76 : 0xec;
            this->helpWindowY = 0;
        } else {
            this->helpWindowX = 0x1d8;
            this->helpWindowY = 0x120;
        }
    } else {
        this->helpWindowX = 0x14b;
        this->helpWindowY = 0xca;
    }
}

String Layout::formatCredits(int n) {
    String num = Layout::formatNumber(n);
    String suffix("$");
    return num + suffix;
}


void Layout::drawBGPattern(unsigned img, int x, int y, int w, int h) {
    PaintCanvas *pc = Globals::Canvas;
    pc->SetColor(this->drawColor);
    int iw = pc->GetImage2DWidth(img);
    int cols = w / iw;
    int fullW = cols * iw;
    int ih = pc->GetImage2DHeight(img);
    int rows = h / ih;
    int fullH = rows * ih;

    for (int r = 0; r < rows; r++) {
        int py = y + r * ih;
        int px = x;
        for (int c = 0; c < cols; c++) {
            pc->DrawImage2D(img, px, py);
            px += iw;
        }
    }

    int remH = h - fullH;
    int remW = w - fullW;
    if (remW > 0) {
        int py = y;
        for (int r = 0; r < rows; r++) {
            pc->DrawRegion2D(img, 0, 0, remW, ih, 0, 0, 0, x + fullW, py);
            py += ih;
        }
    }
    if (remH > 0) {
        int px2 = x;
        for (int c = 0; c < cols; c++) {
            pc->DrawRegion2D(img, 0, 0, iw, remH, 0, 0, 0, px2, y + fullH);
            px2 += iw;
        }
    }
    if (remW > 0 || remH > 0) {
        pc->DrawRegion2D(img, 0, 0, remW, remH, 0, 0, 0, x + fullW, y + fullH);
    }
}


static const char g_fnEmpty[] = "";

static const char g_fnSepA[] = "";

static const char g_fnSepB[] = "";

static const char g_fnOverflow[] = "";

String Layout::formatNumber(int value) {
    unsigned mag = (value < 0) ? (unsigned) (-value) : (unsigned) value;

    String digits;
    digits.Set((long long) (int) mag);
    int len = (int) digits.size();

    String out(g_fnEmpty);

    String sep;
    unsigned short lang = GameText::getLanguage();
    if (lang < 0xc && ((1u << (lang & 0xff)) & 0xc01u) != 0)
        sep = String(g_fnSepA);
    else
        sep = String(g_fnSepB);

    if (len < 4) {
        out = digits;
    } else {
        int i = len;
        while (i > 2) {
            i -= 3;
            String grp;
            grp = digits.SubString(i, len);
            String prefix = (out.size() < 2) ? String() : sep;
            out = prefix + grp + out;
        }
        String head;
        head = digits.SubString(0, i);
        if (head.size() != 0)
            out = head + sep + out;
    }

    if ((unsigned) value > 0x7fffffff)
        out = String(g_fnOverflow) + out;

    return out;
}


static int *gW1 = nullptr;

static int *gW2 = nullptr;

static int *gW3 = nullptr;

void Layout::setWindowDimensions(int p1, int p2, int p3, int p4) {
    this->windowX = p1;
    this->windowY = p2;
    this->windowWidth = p3;
    this->windowHeight = p4;
    this->helpButton->setPosition(p3 + p1, p2, 0x12);

    Layout *layout = (Layout *) Globals::layout;
    int buttonX = layout->buttonInsetX + this->windowX;
    int buttonY = (this->windowY + this->windowHeight) - this->footerButtonOffset;
    this->backButton->setPosition(buttonX, buttonY, 0x21);
    this->secondaryButton->setPosition(buttonX, buttonY, 0x21);

    if (this->backButton != nullptr) {
        Vector posX = this->backButton->getPosition();
        Globals::other_buttons_x[0] = (int) posX.x;
        Vector posY = this->backButton->getPosition();
        Globals::other_buttons_y[0] = (int) posY.y;
    }
}


static FModSound **gFmod = nullptr;

void Layout::showMissionRewardMessage(int show, bool flag) {
    if (show == 0)
        return;
    this->rewardMessageFlag = flag;
    this->rewardMessageActive = 1;
    FModSound *sound = *gFmod;
    this->rewardMessageTimer = 0;
    this->rewardCredits = show;
    sound->play(0x24, 0, 0, 0.0f);
}


void Layout::drawBGBorder(unsigned corner, unsigned edge, int x, int y, int w, int h, int inset, int pad) {
    PaintCanvas *pc = Globals::Canvas;
    int cw = pc->GetImage2DWidth(corner);
    int ch = pc->GetImage2DHeight(corner);
    int ew = pc->GetImage2DWidth(edge);
    int eh = pc->GetImage2DHeight(edge);

    pc->DrawImage2D(corner, inset + x, inset + y);
    int rightX = ((w + x) - cw) - inset;
    pc->DrawImage2D(corner, rightX, inset + y, (unsigned char) (1));
    int bottomY = ((y + h) - ch) - inset;
    pc->DrawImage2D(corner, inset + x, bottomY, (unsigned char) (2));
    pc->DrawImage2D(corner, rightX, bottomY, (unsigned char) (3));

    int spanW = w + cw * -2 + inset * -2;
    int colsH = spanW / ew;
    int baseX = pad + x;
    int tileX = cw + pad + x;
    int topRowY = pad + y;
    int botRowY = (h - inset) + pad + y;
    for (int i = 0; i < colsH; i++) {
        pc->DrawImage2D(edge, tileX, topRowY);
        pc->DrawImage2D(edge, tileX, botRowY, (unsigned char) (2));
        tileX += ew;
    }
    int remW = spanW - colsH * ew;
    if (remW > 0) {
        pc->DrawRegion2D(edge, 0, 0, remW, eh, 0, 0, 0,
                         colsH * ew + baseX + cw, topRowY);
        pc->DrawRegion2D(edge, 0, 0, remW, eh, 3.1416f, 0, 0,
                         spanW + baseX + cw, y + h - pad);
    }

    int spanH = h + ch * -2 + inset * -2;
    int rowsV = spanH / ew;
    int coveredH = rowsV * ew;
    int leftX = pad + x;
    int rightEdgeX = w + x + pad;
    int rowY = ch + pad + y;
    for (int i = 0; i < rowsV; i++) {
        pc->DrawRegion2D(edge, 0, 0, ew, eh, -1.5708f, 0, 0, leftX, ew + rowY);
        pc->DrawRegion2D(edge, 0, 0, ew, eh, 1.5708f, 0, 0, rightEdgeX, rowY);
        // Android ARM advances by the full tiled span, not one edge width.
        rowY += coveredH;
    }
    int remH = spanH - coveredH;
    if (remH > 0) {
        pc->DrawRegion2D(edge, 0, 0, remH, eh, -1.5708f, 0, 0,
                         leftX, spanH + pad + y + ch);
        pc->DrawRegion2D(edge, 0, 0, remH, eh, 1.5708f, 0, 0,
                         rightEdgeX + inset * -2, pad + y + ch + coveredH);
    }
}


void Layout::drawScrollBar(int x, int y, int trackH, int pos, int range) {
    PaintCanvas *pc = Globals::Canvas;
    int iw = pc->GetImage2DWidth(this->scrollBarImage);
    int ih = pc->GetImage2DHeight(this->scrollBarImage);

    pc->SetColor(0x111417ffu);
    pc->SetColor(0x193043ffu);

    int inset = this->scrollBarInset;
    pc->DrawRectangle(x, inset + y, Globals::layout->field_0x48, trackH - inset * 2);
    pc->SetColor(this->drawColor);

    int thumb = range - 1;
    if (thumb <= ih * 2) thumb = ih * 2;
    int off = pos + 1;
    if (trackH <= thumb + off)
        off = (trackH - thumb) - inset;

    int handle = this->scrollBarHandle;
    thumb = thumb + handle * -4;
    off = off + handle * 2;
    if (ih * 2 < thumb) {
        this->drawBGPattern(this->scrollBarFillImage, x + 1 + handle, ih + y + off, iw, thumb + ih * -2);
        handle = this->scrollBarHandle;
    } else {
        int lim = trackH + ih * -2;
        if (lim <= off) off = lim;
    }

    pc->DrawImage2D(this->scrollBarImage, handle + x + 1, y + off);
    pc->DrawImage2D(this->scrollBarImage,
                    this->scrollBarHandle + x + 1, (thumb - ih) + y + off, (unsigned char) (0x02));
}


uint8_t Layout::drawFade() {
    if (this->fading != 0) {
        PaintCanvas *pc = Globals::Canvas;
        unsigned saved = pc->GetColor();

        float t = (float) this->fadeProgress / (float) this->fadeDuration;
        if (this->fadeOut == 0)
            t = 1.0f - t;
        float capped = 1.0f;
        if (t < 1.0f) capped = t;

        unsigned color = this->fadeColor;
        if (capped > 0.0f)
            color += (int) (capped * 255.0f);
        pc->SetColor(color);
        pc->FillRectangle(0, 0, Globals::w, Globals::h);
        pc->SetColor(saved);
    }
    if (this->fillScreen != 0) {
        PaintCanvas *pc = Globals::Canvas;
        unsigned saved = pc->GetColor();
        pc->SetColor(0xff);
        pc->FillRectangle(0, 0, Globals::w, Globals::h);
        pc->SetColor(saved);
    }
    return this->fading;
}


void Layout::drawBox(int style, int x, int y, int w, int h, String text, unsigned char flags) {
    PaintCanvas *pc = Globals::Canvas;
    unsigned saved = pc->GetColor();
    pc->SetColor(this->drawColor);

    switch (style) {
        case 0: {
            int iw = pc->GetImage2DWidth(this->field_0x348);
            pc->DrawImage2D(this->field_0x348, x, y);
            this->drawBGPattern(this->field_0x34c, iw + x, y, w + iw * -2, h);
            pc->DrawImage2D(this->field_0x348, (w + x) - iw, y, (unsigned char) (0x01));
            if (text.size() == 0) break;
            int tx = Globals::layout->field_0x44;
            if ((flags & 2) == 0) {
                if ((flags & 4) != 0) {
                    int tw = pc->GetTextWidth(Globals::font, text);
                    tx = w / 2 - tw / 2;
                }
            } else {
                int tw = pc->GetTextWidth(Globals::font, text);
                tx = (w - tx) - tw;
            }
            int ty = (y + (Globals::layout->field_0x1c >> 1) + 1) - this->textBaselineAdjust;
            pc->DrawString(Globals::font, text, tx + x, ty, false);
            break;
        }
        case 1: {
            int iw = pc->GetImage2DWidth(this->field_0x350);
            pc->DrawImage2D(this->field_0x350, x, y);
            this->drawBGPattern(this->field_0x354, iw + x, y, w + iw * -2, h);
            pc->DrawImage2D(this->field_0x350, (w + x) - iw, y, (unsigned char) (0x01));
            if (text.size() == 0) break;
            int tx = Globals::layout->field_0x44;
            if ((flags & 2) == 0) {
                if ((flags & 4) != 0) {
                    int tw = pc->GetTextWidth(Globals::font, text);
                    tx = w / 2 - tw / 2;
                }
            } else {
                int tw = pc->GetTextWidth(Globals::font, text);
                tx = (w - tx) - tw;
            }
            int ty = (y + (Globals::layout->field_0x5c >> 1) + 1) - this->textBaselineAdjust;
            pc->DrawString(Globals::font, text, tx + x, ty, false);
            break;
        }
        case 2:
            this->drawBGPattern(this->bgPatternImage, x, y, w, h);
            break;
        case 3: {
            int iw = pc->GetImage2DWidth(this->field_0x358);
            pc->DrawImage2D(this->field_0x358, x, y);
            this->drawBGPattern(this->field_0x35c, iw + x, y, w + iw * -2, h);
            pc->DrawImage2D(this->field_0x358, (w + x) - iw, y, (unsigned char) (0x01));
            break;
        }
        case 4: {
            int iw = pc->GetImage2DWidth(this->field_0x36c);
            pc->DrawImage2D(this->field_0x36c, x, y);
            this->drawBGPattern(this->field_0x370, iw + x, y, w + iw * -2, h);
            pc->DrawImage2D(this->field_0x36c, (w + x) - iw, y, (unsigned char) (0x01));
            break;
        }
        case 5: {
            this->drawBGBorder(this->field_0x380, this->field_0x384, x, y, w, h, 0, 0);
            break;
        }
        case 6: {
            int iw = pc->GetImage2DWidth(this->field_0x388);
            pc->DrawImage2D(this->field_0x388, x, y);
            this->drawBGPattern(this->field_0x38c, iw + x, y, w + iw * -2, h);
            pc->DrawImage2D(this->field_0x388, (w + x) - iw, y, (unsigned char) (0x01));
            if (text.size() == 0) break;
            int tx = Globals::layout->field_0x44;
            if ((flags & 2) == 0) {
                if ((flags & 4) != 0) {
                    int tw = pc->GetTextWidth(Globals::font, text);
                    tx = w / 2 - tw / 2;
                }
            } else {
                int tw = pc->GetTextWidth(Globals::font, text);
                tx = (w - tx) - tw;
            }
            int ty = (y + (h >> 1) + 1) - this->textBaselineAdjust;
            pc->DrawString(Globals::font, text, tx + x, ty, false);
            break;
        }
        case 7: {
            int hdr = Globals::layout->field_0x8;
            this->drawBGPattern(this->bgPatternImage, x, hdr + y, w, h - hdr);
            int ih = pc->GetImage2DHeight(this->field_0x394);
            this->drawBGBorder(this->field_0x390, this->field_0x394, x, hdr + y, w, h - hdr, -ih, -ih);
            if (text.size() == 0) break;
            pc->SetColor(0xffffffff);
            pc->DrawImage2D(this->headerIconImage, x, y);
            int ty = (y + (Globals::layout->field_0x8 / 2) + 1) - this->textBaselineAdjust;
            pc->DrawString(Globals::font, text,
                           Globals::layout->field_0x28 + x, ty, false);
            break;
        }
        case 8: {
            this->drawBGBorder(this->field_0x39c, this->field_0x3a0, x, y, w, h, 0, 0);
            break;
        }
        case 9: {
            int iw = pc->GetImage2DWidth(this->field_0x360);
            pc->DrawImage2D(this->field_0x360, x, y);
            this->drawBGPattern(this->field_0x364, iw + x, y, w + iw * -2, h);
            pc->DrawImage2D(this->field_0x360, (w + x) - iw, y, (unsigned char) (0x01));
            break;
        }
        case 10: {
            int iw = pc->GetImage2DWidth(this->field_0x368);
            pc->DrawImage2D(this->field_0x368, x, y);
            this->drawBGPattern(this->field_0x370, iw + x, y, w + iw * -2, h);
            pc->DrawImage2D(this->field_0x368, (w + x) - iw, y, (unsigned char) (0x01));
            break;
        }
        default:
            break;
    }

    pc->SetColor(saved);
}


void Layout::drawWindow(String title, int x, int y, int w, int h, bool drawBG) {
    unsigned saved = Globals::Canvas->GetColor();
    if (drawBG) {
        int top = Globals::layout->field_0x8;
        this->drawBGPattern(this->bgPatternImage, x, top + y, w, h - top);
    }
    Globals::Canvas->SetColor(0x255d8dff);
    int top = Globals::layout->field_0x8;
    Globals::Canvas->DrawRectangle(x, top + y, w, h - top);
    Globals::Canvas->SetColor(this->drawColor);
    Globals::Canvas->DrawImage2D(this->headerIconImage, x, y);
    // Android 2.0.16 compares a nonempty title with the empty literal at 0x1ca472.
    if (title.size() != 0 && title.Compare_char("") == 0) {
        int ty = y + Globals::layout->field_0x8 / 2 + 1 - this->textBaselineAdjust;
        Globals::Canvas->DrawString(Globals::font, title,
                                     Globals::layout->field_0x28 + x, ty, false);
    }
    Globals::Canvas->SetColor(saved);
}


void Layout::drawTip() {
    if (this->tipLines == nullptr)
        return;

    Globals::Canvas->SetColor(0xffffffffu);
    int boxW = Globals::layout->field_0x78;
    Globals::layout->drawBox(5, (Globals::w >> 1) - (boxW >> 1),
                             (Globals::h >> 1) + 13, boxW, 100, String(""));

    Globals::Canvas->DrawImage2D(this->tipBoxImage,
                                  Globals::w >> 1, (Globals::h >> 1) + 63,
                                  (unsigned char) 0x11, (unsigned char) 0x44);

    unsigned int linesHeight = (unsigned int) (Globals::layout->field_0x4 * this->tipLines->size());
    int y = (Globals::h >> 1) + 63 - (int) (linesHeight >> 1);
    reinterpret_cast<Globals *>(Globals::globals)->drawLines(Globals::font,
                                                              this->tipLines,
                                                              Globals::w >> 1, y, true);
}

void Layout::drawHeader(String title) {
    this->drawHeader(title, true);
}

void Layout::drawHelpWindow() {
    if (this->choiceWindow != nullptr)
        this->choiceWindow->draw();
}


static FModSound **gFmodHelp = nullptr;

void Layout::initHelpWindow(String text) {
    if (this->choiceWindow == nullptr)
        this->choiceWindow = new ChoiceWindow();
    (*gFmodHelp)->play(0x7e, 0, 0, 0);
    String *title = GameText::gGameText->getText(0x187);
    this->choiceWindow->set(*title, text);
    this->choiceWindowOpen = 1;
    this->helpPressedFlag = 0;
}


void Layout::resetWindowDimensions() {
    int h = Globals::h;
    int w = Globals::w;

    this->windowX = 0;
    this->windowY = 0;
    this->windowWidth = w;
    this->windowHeight = h;

    this->helpButton->setPosition(w, 0, (unsigned char) 0x12);

    int inset = Globals::layout->buttonInsetX;
    int btnY = (this->windowY + this->windowHeight) - this->footerButtonOffset;
    this->backButton->setPosition(inset + this->windowX, btnY, (unsigned char) 0x21);
    this->secondaryButton->setPosition(inset + this->windowX, btnY, (unsigned char) 0x21);

    if (this->backButton != nullptr) {
        Vector pos = this->backButton->getPosition();
        Globals::other_buttons_x[0] = (int) pos[0];
        pos = this->backButton->getPosition();
        Globals::other_buttons_y[0] = (int) pos[1];
    }
}


static const String *g_tagBaseString = nullptr;

static const char g_tagLit0[] = "";

static const char g_tagLit1[] = "";

static const char g_tagLit2[] = "";

String Layout::tagString(String in) {
    return String(g_tagLit0) + *g_tagBaseString + String(g_tagLit1) + in + String(g_tagLit2);
}


void Layout::reload() {
    this->bgPatternImage = -1;
    this->headerPatternImage = -1;
    this->headerIconImage = -1;
    this->field_0x3a8 = -1;

    memset(&this->footerImageLeft, 0xff, 0x70);

    PaintCanvas *canvas = Globals::Canvas;
    canvas->Image2DCreate(static_cast<unsigned short>(0x503), reinterpret_cast<unsigned int &>(this->tipBoxImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x47e), reinterpret_cast<unsigned int &>(this->bgPatternImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x4ff),
                          reinterpret_cast<unsigned int &>(this->headerPatternImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x500), reinterpret_cast<unsigned int &>(this->headerCapImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x474), reinterpret_cast<unsigned int &>(this->headerIconImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x502), reinterpret_cast<unsigned int &>(this->footerImageLeft));
    canvas->Image2DCreate(static_cast<unsigned short>(0x506), reinterpret_cast<unsigned int &>(this->field_0x340));
    canvas->Image2DCreate(static_cast<unsigned short>(0x501), reinterpret_cast<unsigned int &>(this->field_0x338));
    canvas->Image2DCreate(static_cast<unsigned short>(0x507),
                          reinterpret_cast<unsigned int &>(this->footerPatternImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x4fe), reinterpret_cast<unsigned int &>(this->footerImageRight));
    canvas->Image2DCreate(static_cast<unsigned short>(0x482), reinterpret_cast<unsigned int &>(this->field_0x348));
    canvas->Image2DCreate(static_cast<unsigned short>(0x481), reinterpret_cast<unsigned int &>(this->field_0x34c));
    canvas->Image2DCreate(static_cast<unsigned short>(0x486),
                          reinterpret_cast<unsigned int &>(this->scrollBarFillImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x487), reinterpret_cast<unsigned int &>(this->scrollBarImage));
    canvas->Image2DCreate(static_cast<unsigned short>(0x48b), reinterpret_cast<unsigned int &>(this->field_0x37c));
    canvas->Image2DCreate(static_cast<unsigned short>(0x52d), reinterpret_cast<unsigned int &>(this->field_0x3a4));

    if (!Globals::iPad) {
        canvas->Image2DCreate(static_cast<unsigned short>(0x480), reinterpret_cast<unsigned int &>(this->field_0x350));
        canvas->Image2DCreate(static_cast<unsigned short>(0x47f), reinterpret_cast<unsigned int &>(this->field_0x354));
        canvas->Image2DCreate(static_cast<unsigned short>(0x479), reinterpret_cast<unsigned int &>(this->field_0x358));
        canvas->Image2DCreate(static_cast<unsigned short>(0x478), reinterpret_cast<unsigned int &>(this->field_0x35c));
        canvas->Image2DCreate(static_cast<unsigned short>(0x489), reinterpret_cast<unsigned int &>(this->field_0x36c));
        canvas->Image2DCreate(static_cast<unsigned short>(0x488),
                              reinterpret_cast<unsigned int &>(this->field_0x370));
    } else {
        canvas->Image2DCreate(static_cast<unsigned short>(0x6bb), reinterpret_cast<unsigned int &>(this->field_0x350));
        canvas->Image2DCreate(static_cast<unsigned short>(0x6ba), reinterpret_cast<unsigned int &>(this->field_0x354));
        canvas->Image2DCreate(static_cast<unsigned short>(0x6b9), reinterpret_cast<unsigned int &>(this->field_0x358));
        canvas->Image2DCreate(static_cast<unsigned short>(0x6b8), reinterpret_cast<unsigned int &>(this->field_0x35c));
        canvas->Image2DCreate(static_cast<unsigned short>(0x6b7), reinterpret_cast<unsigned int &>(this->field_0x36c));
        canvas->Image2DCreate(static_cast<unsigned short>(0x6bc),
                              reinterpret_cast<unsigned int &>(this->field_0x370));
    }

    canvas->Image2DCreate(static_cast<unsigned short>(0x530), reinterpret_cast<unsigned int &>(this->field_0x360));
    canvas->Image2DCreate(static_cast<unsigned short>(0x531), reinterpret_cast<unsigned int &>(this->field_0x364));
    canvas->Image2DCreate(static_cast<unsigned short>(0x52f), reinterpret_cast<unsigned int &>(this->field_0x368));
    canvas->Image2DCreate(static_cast<unsigned short>(0x47c), reinterpret_cast<unsigned int &>(this->field_0x384));
    canvas->Image2DCreate(static_cast<unsigned short>(0x47d), reinterpret_cast<unsigned int &>(this->field_0x380));
    canvas->Image2DCreate(static_cast<unsigned short>(0x47b), reinterpret_cast<unsigned int &>(this->field_0x388));
    canvas->Image2DCreate(static_cast<unsigned short>(0x47a), reinterpret_cast<unsigned int &>(this->field_0x38c));
    canvas->Image2DCreate(static_cast<unsigned short>(0x484), reinterpret_cast<unsigned int &>(this->field_0x390));
    canvas->Image2DCreate(static_cast<unsigned short>(0x483), reinterpret_cast<unsigned int &>(this->field_0x394));
    canvas->Image2DCreate(static_cast<unsigned short>(0x50c), reinterpret_cast<unsigned int &>(this->field_0x3a0));
    canvas->Image2DCreate(static_cast<unsigned short>(0x50d), reinterpret_cast<unsigned int &>(this->field_0x39c));

    String *txt = Globals::gameText->getText(170);
    TouchButton *bBack = new TouchButton(*txt, 2, this->buttonInsetX, Globals::h - 3, '!');
    this->backButton = bBack;
    this->backButtonWidth = bBack->getWidth();

    unsigned img535 = 0xffffffff;
    canvas->Image2DCreate(0x535, img535);
    TouchButton *b2;
    if (img535 == 0xffffffff) {
        String *t = Globals::gameText->getText(171);
        b2 = new TouchButton(*t, 2, this->buttonInsetX,
                             Globals::h - this->footerButtonOffset, '!');
    } else {
        b2 = new TouchButton(img535, 2, this->buttonInsetX,
                             Globals::h - this->footerButtonOffset, '!');
    }
    this->secondaryButton = b2;

    unsigned img471 = 0xffffffff;
    canvas->Image2DCreate(0x471, img471);
    TouchButton *bHelp = new TouchButton(img471, 1, Globals::w, 0,
                                          this->field_0x3c, 0x12, 0x04);
    this->helpButton = bHelp;

    int th = canvas->GetTextHeight(Globals::font);
    this->choiceWindowOpen = 0;
    this->choiceWindow = nullptr;
    this->tipLines = nullptr;
    this->textBaselineAdjust = th / 2 - 1;
    this->drawColor = -1;
    this->resetWindowDimensions();
    this->rewardMessageTimer = 0;
    this->helpButtonEnabled = 0;
    this->field_0x3d8 = 0;
    this->fading = 0;
    this->field_0x3dc = 0;
    this->fadeColor = 0;
    this->fadeProgress = 0;
    this->rewardMessageActive = 0;
    this->rewardMessageFlag = 0;
    this->fadeOut = 0;
    this->field_0x40d = 0;
    this->field_0x409 = 0;
}

void Layout::drawHeader() {
    this->drawHeader(String(""), false);
}

void Layout::drawFooterStation() {
    this->drawFooter(true, false);
}


static unsigned *g_dhColor = nullptr;

static int **g_dhMetric = nullptr;

static void ***g_dhFont = nullptr;

void Layout::drawHeader(String title, bool transition) {
    PaintCanvas *pc = PaintCanvas::gCanvas;
    unsigned img = *g_dhColor;
    pc->SetColor(this->drawColor);
    int iw = pc->GetImage2DWidth(img);
    int ih = pc->GetImage2DHeight(img);
    pc->DrawImage2D(this->headerCapImage, this->windowX, this->windowY);
    this->drawBGPattern(this->headerPatternImage, this->windowX + iw, this->windowY, this->windowWidth + iw * -2, ih);
    pc->DrawImage2D(this->headerCapImage,
                    this->windowWidth + this->windowX,
                    this->windowY, iw, ih, (unsigned char) (0x11), (unsigned char) (0x12), (unsigned char) (0x01));
    if (title.size() != 0) {
        pc->DrawImage2D(this->headerIconImage, this->windowX, this->windowY);
        int *m = *g_dhMetric;
        pc->DrawString((unsigned) (unsigned long) (**g_dhFont), title,
                       m[0x28 / 4] + m[0x44 / 4] + this->windowX,
                       this->headerTitleY + this->windowY, false);
    }
    this->helpButtonEnabled = transition;
    if (transition && this->choiceWindowOpen == 0) {
        alignas(4) unsigned char sp[8];
        TouchButton_footerAnim(this->helpButton, 0, 0, sp);
    }
}


static PaintCanvas **g_mrCanvas = nullptr;

static int *g_mrDimA = nullptr;

static const char g_mrLit0[] = "";

static const char g_mrLit1[] = "";

static int *g_mrDimB = nullptr;

static int *g_mrTextId = nullptr;

static void **g_mrFont = nullptr;

static const char g_mrLit2[] = "";

void Layout::drawMissionRewardMessage(bool transition) {
    if (this->rewardMessageActive != 0) {
        PaintCanvas *pc = *g_mrCanvas;
        unsigned saved = pc->GetColor();
        unsigned origColor = this->drawColor;
        PaintCanvas::gCanvas->SetColor(0xffffffff);

        int t = this->rewardMessageTimer;
        float a;
        if (t < 2000) {
            a = (float) t / 6000.0f;
        } else if (t > 5000) {
            a = (float) (7000 - t) / 6000.0f;
        } else {
            a = 1.0f;
        }
        unsigned col = (unsigned) ((int) (a * 256.0f)) - 0x100;
        PaintCanvas::gCanvas->SetColor(col);

        unsigned newColor = pc->GetColor();
        int boxW = this->rewardBoxWidth;
        int boxH = this->rewardBoxHeight;
        int boxX = this->rewardBoxX;
        int boxY = this->rewardBoxY;
        this->drawColor = newColor;

        if (transition != 0) {
            int sw = *g_mrDimA;
            String s0(g_mrLit0);
            this->drawBox(2, (sw >> 1) - (boxH >> 1), boxX, boxH, boxW, s0, 0u);

            sw = *g_mrDimA;
            String s1(g_mrLit1);
            this->drawBox(8, (sw >> 1) - (boxH >> 1), boxX, boxH, boxW, s1, 0u);
        }

        int sh = *g_mrDimB;
        pc->DrawImage2D(this->field_0x3a4, sh >> 1, (char) boxX, (unsigned char) (0x11));

        String *txt = GameText::gGameText->getText(*g_mrTextId);
        String line(*txt);

        sh = *g_mrDimB;
        void *font = *g_mrFont;
        int tw = pc->GetTextWidth((unsigned) (unsigned long) (font), line);
        pc->DrawString((unsigned) (unsigned long) (font), line, (sh >> 1) - (tw >> 1), boxX + boxY, false);

        String suffix(g_mrLit2);
        String credits = Layout::formatCredits(this->rewardCredits);
        line = suffix + credits;

        sh = *g_mrDimB;
        tw = pc->GetTextWidth((unsigned) (unsigned long) (font), line);
        pc->DrawString((unsigned) (unsigned long) (font), line, (sh >> 1) - (tw >> 1),
                       this->rewardBoxY2 + boxX, false);
        PaintCanvas::gCanvas->SetColor(saved);
        this->drawColor = origColor;
    }
}

void Layout::drawWindow(String title, int x, int y, int w, int h) {
    this->drawWindow(title, x, y, w, h, true);
}

void Layout::drawMask() {
    this->drawMask(0, 0, Globals::w, Globals::h);
}

void Layout::drawBGBorder(unsigned corner, unsigned edge, int x, int y, int w, int h) {
    this->drawBGBorder(corner, edge, x, y, w, h, 0, 0);
}

void Layout::drawBox(int style, int x, int y, int w, int h, String text) {
    String tmp(text);
    this->drawBox(style, x, y, w, h, tmp, 0u);
}

void Layout::drawFooter(bool stationMode_, bool showBack_) {
    int stationMode = stationMode_;
    int showBack = showBack_;
    PaintCanvas *pc = Globals::Canvas;
    pc->SetColor(this->drawColor);
    int wRight = pc->GetImage2DWidth(this->footerImageRight);
    int wLeft = pc->GetImage2DWidth(this->footerImageLeft);

    pc->DrawImage2D(this->footerImageLeft, this->windowX,
                    this->windowY + this->windowHeight, (unsigned char) 0x11, (unsigned char) 0x21);
    int footerH = Globals::layout->field_0x10;
    pc->DrawImage2D(this->footerImageRight, this->windowX + wLeft,
                    (this->windowY + this->windowHeight) - footerH);
    int both = wLeft + wRight;
    this->drawBGPattern(this->field_0x338, both + this->windowX, (this->windowY + this->windowHeight) - footerH,
                        this->windowWidth + both * -2, footerH);
    pc->DrawImage2D(this->footerImageRight,
                    (this->windowX - both) + this->windowWidth,
                    (this->windowHeight + this->windowY) - footerH, (unsigned char) (0x01));
    pc->DrawImage2D(this->footerImageLeft,
                    (this->windowX - wRight) + this->windowWidth,
                    (this->windowHeight + this->windowY) - footerH, (unsigned char) (0x01));

    int backVis = (!stationMode) && showBack;
    this->backButton->setVisible(backVis);
    if (stationMode)
        this->secondaryButton->draw();
    else if (showBack)
        this->backButton->draw();

    Status *status = Globals::status;
    int load = status->getShip()->getCurrentLoad();
    int maxLoad = status->getShip()->getMaxLoad();
    if (maxLoad < load)
        pc->SetColor(0xff2a00ff);

    String sLoad(status->getShip()->getCurrentLoad());
    String sMax(status->getShip()->getMaxLoad());
    String loadStr = sLoad + String(" / ") + sMax + String("t");

    {
        int x = this->windowX;
        int w = this->windowWidth;
        unsigned font = Globals::font;
        int tw = pc->GetTextWidth(font, loadStr);
        pc->DrawString(font, loadStr, (x + w / 2) - tw / 2,
                       (this->windowHeight + this->windowY) - this->footerTextInset, false);
    }
    pc->SetColor(this->drawColor);

    String credStr = Layout::formatCredits(status->getCredits());

    {
        int x = this->windowX;
        int w = this->windowWidth;
        unsigned font = Globals::font;
        if (stationMode) {
            int rightInset = Globals::layout->field_0x74;
            int tw = pc->GetTextWidth(font, credStr);
            pc->DrawString(font, credStr, ((w + x) - rightInset) - tw / 2,
                           (this->windowHeight + this->windowY) - this->footerTextInset, false);
        } else {
            int tw = pc->GetTextWidth(font, credStr);
            pc->DrawString(font, credStr, (w + x - 10) - tw,
                           (this->windowHeight + this->windowY) - this->footerTextInset, false);
        }
    }
}

void Layout::drawMask(int x, int y, int w, int h) {
    PaintCanvas *pc = Globals::Canvas;
    unsigned saved = pc->GetColor();
    pc->SetColor(0x80);
    pc->FillRectangle(x, y, w, h);
    pc->SetColor(saved);
}

void Layout::drawWindow(String title) {
    String tmp(title);
    this->drawWindow(tmp, true);
}

void Layout::drawWindow(String title, bool drawBG) {
    String tmp(title);
    int p4 = *gW2;
    int p5 = *gW3 - ((int *) gW1)[2];
    this->drawWindow(tmp, 0, 0, p4, p5, drawBG);
}
