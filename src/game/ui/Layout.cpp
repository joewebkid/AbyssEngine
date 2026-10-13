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


Layout::Layout() {
    const bool hd = Globals::iPadHD != 0;
    const bool n9 = Globals::n9 != 0;
    const bool retina = Globals::retinaDisplay != 0;
    const bool ipad = Globals::iPad != 0;
    const bool ipadLarge = Globals::iPadLarge != 0;

    // shortcut: the remaining early metric block is not yet recovered; replace the zero fill in a full constructor pass.
    memset(&this->field_0x4, 0, 0x2d8 - 0x04);

    if (hd) {
        this->field_0x4 = 21;  this->field_0x8 = 42;
        this->field_0xc = 40;  this->field_0x10 = 52;
        this->field_0x14 = 30; this->field_0x18 = 192;
        this->field_0x1c = 28; this->field_0x20 = 112;
        this->field_0x24 = 7;  this->field_0x28 = 112;
        this->field_0x2c = 4;  this->field_0x30 = 42;
        this->field_0x34 = 8;  this->field_0x38 = 9;
        this->field_0x3c = 82; this->field_0x40 = 140;
        this->field_0x44 = 7;  this->field_0x48 = 240;
        this->field_0x4c = 7;  this->field_0x50 = 125;
        this->field_0x54 = 487; this->field_0x58 = 341;
    } else if (n9) {
        this->field_0x4 = 22;  this->field_0x8 = 45;
        this->field_0xc = 51;  this->field_0x10 = 58;
        this->field_0x14 = 37; this->field_0x18 = 176;
        this->field_0x1c = 30; this->field_0x20 = 112;
        this->field_0x24 = 7;  this->field_0x28 = 144;
        this->field_0x2c = 6;  this->field_0x30 = 51;
        this->field_0x34 = 4;  this->field_0x38 = 12;
        this->field_0x3c = 106; this->field_0x40 = 180;
        this->field_0x44 = 9;  this->field_0x48 = 256;
        this->field_0x4c = 10; this->field_0x50 = 160;
        this->field_0x54 = 520; this->field_0x58 = 364;
    } else {
        this->field_0x4 = retina ? 30 : 15;
        this->field_0x8 = retina ? 60 : 30;
        this->field_0xc = retina ? 58 : 29;
        this->field_0x10 = retina ? 74 : 37;
        this->field_0x14 = retina ? 50 : 22;
        this->field_0x18 = retina ? 14 : 9;
        this->field_0x1c = retina ? 40 : 20;
        this->field_0x20 = retina ? 10 : 5;
        this->field_0x24 = retina ? 10 : 5;
        this->field_0x28 = retina ? 10 : 5;
        this->field_0x2c = retina ? 6 : 3;
        this->field_0x30 = retina ? 60 : 30;
        this->field_0x34 = retina ? 12 : 6;
        this->field_0x38 = retina ? 14 : 7;
        this->field_0x3c = retina ? 118 : 59;
        this->field_0x40 = retina ? 200 : 100;
        this->field_0x44 = retina ? 10 : 5;
        this->field_0x48 = retina ? 22 : 11;
        this->field_0x4c = retina ? 10 : 5;
        this->field_0x50 = retina ? 178 : 89;
        this->field_0x54 = retina ? 694 : 347;
        this->field_0x58 = retina ? 486 : 243;
    }

    this->field_0x70_rowHeight = hd ? 63 : n9 ? 51 : ipadLarge ? 90
        : retina ? 60 : ipad ? 45 : 30;
    this->field_0x8c = hd ? 45 : n9 ? 57 : retina ? 64 : 32;
    // StarMap copies these original float bits from Layout+0x90.
    this->field_0x90 = hd ? 1055696996 : (n9 || ipad || retina) ? 0x3ea66666 : 0x3f266666;
    this->field_0x94 = hd ? 14 : n9 ? 15 : retina ? 20 : 10;
    this->field_0x98 = hd ? 590 : n9 ? 630 : retina ? 840 : 420;
    this->field_0x9c = hd ? 70 : n9 ? 75 : retina ? 100 : 50;

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

    this->field_0x238 = hd ? 70 : n9 ? 90 : retina ? 100 : 50;
    this->field_0x27c = hd ? 28 : n9 ? 30 : retina ? 40 : 20;
    this->field_0x280 = hd ? 0 : n9 ? 4 : retina ? 5 : 0;
    this->scrollBarInset = n9 ? 2 : retina ? 2 : 1;
    this->scrollBarHandle = n9 ? 1 : retina ? 1 : 0;

    if (hd) {
        this->rewardBoxWidth = 140;
        this->rewardBoxY = 77;
        this->rewardBoxY2 = 132;
    } else if (n9) {
        this->rewardBoxWidth = 112;
        this->rewardBoxY = 57;
        this->rewardBoxY2 = 111;
    } else {
        this->rewardBoxWidth = ipadLarge ? 200 : ipad ? 100 : retina ? 150 : 75;
        this->rewardBoxY = ipadLarge ? 110 : ipad ? 55 : retina ? 76 : 38;
        this->rewardBoxY2 = (this->rewardBoxY + 17) << (retina ? 1 : 0);
    }
    this->rewardBoxHeight = n9 ? 352 : retina ? 392 : 196;
    this->rewardBoxX = n9 ? 75 : retina ? 100 : 50;

    this->field_0x2bc = hd ? 2 : n9 ? 3 : 2;
    this->footerButtonOffset = hd ? 4 : n9 ? 5 : retina ? 6 : 3;
    this->field_0x200 = hd ? -18 : n9 ? -26 : retina ? -26 : -13;
    this->field_0x1f0 = hd ? 26 : n9 ? 34 : retina ? 38 : 19;
    this->field_0x1f4 = hd ? 22 : ipad ? (retina ? 32 : 16) : (retina ? 4 : 2);
    this->field_0x1f8 = hd ? 2 : (ipad && retina ? 4 : 2);
    this->field_0x284 = n9 ? 0 : 1;
    this->field_0x285 = 1;
    this->field_0x286 = n9 ? 0 : 1;
    this->field_0x288 = n9 ? 20 : 0;
    this->field_0x28c = n9 ? -19 : 0;
    this->field_0x290 = n9 ? 4 : retina ? 7 : 3;

    this->field_0x2f4 = hd ? 112 : ipadLarge ? 160 : ipad ? 80 : n9 ? 45 : retina ? 60 : 30;
    // The original writes IEEE-754 float bits into this 32-bit storage slot.
    this->field_0x2f8 = hd ? 1133289472 : ipadLarge ? 1137180672
        : ipad ? 1128792064 : n9 ? 1125515264
        : retina ? 1128792064 : 1120403456;
    this->field_0x2fc = hd ? 50 : ipadLarge ? 72 : 36;
    this->field_0x300 = hd ? 281 : ipadLarge ? 400 : 200;
    this->field_0x304 = hd ? 492 : ipadLarge ? 700 : 350;
    this->choiceWindowTitleHeight = hd ? 351 : ipadLarge ? 500 : 250;

    if (ipadLarge) {
        this->field_0x30c = 6;
        this->field_0x310 = -24;
        this->field_0x314 = -10;
        this->field_0x318 = 8;
    } else if (ipad) {
        this->field_0x30c = 3;
        this->field_0x310 = -12;
        this->field_0x314 = -5;
        this->field_0x318 = 4;
    } else if (n9) {
        this->field_0x30c = 4;
        this->field_0x310 = -8;
        this->field_0x314 = -8;
        this->field_0x318 = 0;
    } else {
        this->field_0x30c = retina ? 7 : 3;
        this->field_0x310 = retina ? -9 : -5;
        this->field_0x314 = retina ? -8 : -5;
        this->field_0x318 = 0;
    }

    this->helpWindowX = hd ? 331 : ipadLarge ? 472 : (ipad || n9 || retina ? 236 : 118);
    this->helpWindowY = hd ? 202 : ipadLarge ? 288 : ipad ? 144 : (n9 || retina ? 150 : 76);
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

static const char g_fnSepA[] = ",";

static const char g_fnSepB[] = ".";

static const char g_fnNegative[] = "-";

String Layout::formatNumber(int value) {
    unsigned mag = (value < 0) ? 0u - (unsigned) value : (unsigned) value;

    String digits((int) mag);
    int len = (int) digits.size();

    String out(g_fnEmpty);

    unsigned short lang = GameText::getLanguage();
    String sep = (lang < 0xc && ((1u << lang) & 0xc01u) != 0)
        ? String(g_fnSepA) : String(g_fnSepB);

    if (len >= 4) {
        int i = len;
        while (i > 2) {
            i -= 3;
            String grp = digits.SubString(i, i + 3);
            String prefix = (out.size() < 2) ? String() : String(sep, false);
            out = grp + prefix + out;
        }
        String head = digits.SubString(0, i);
        if (head.size() != 0)
            out = head + sep + out;
    } else {
        out = digits;
    }

    if (value < 0)
        out = String(g_fnNegative) + out;

    return out;
}


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
        {
            Vector posX = this->backButton->getPosition();
            Globals::other_buttons_x[0] = (int) posX.x;
        }
        {
            Vector posY = this->backButton->getPosition();
            Globals::other_buttons_y[0] = (int) posY.y;
        }
    }
}


void Layout::showMissionRewardMessage(int show, bool flag) {
    if (show == 0)
        return;
    this->rewardMessageFlag = flag;
    this->rewardMessageActive = 1;
    this->rewardMessageTimer = 0;
    this->rewardCredits = show;
    Globals::sound->play(0x24, 0, 0, 0.0f);
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
    this->choiceWindow->draw();
}


void Layout::initHelpWindow(String text) {
    if (this->choiceWindow == nullptr)
        this->choiceWindow = new ChoiceWindow();
    Globals::sound->play(0x7e, 0, 0, 0);
    ChoiceWindow *choice = this->choiceWindow;
    String *title = Globals::gameText->getText(0x187);
    choice->set(*title, text, false);
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

    this->backButton->setPosition(Globals::layout->buttonInsetX + this->windowX,
                                  (this->windowY + this->windowHeight) - this->footerButtonOffset,
                                  (unsigned char) 0x21);
    this->secondaryButton->setPosition(Globals::layout->buttonInsetX + this->windowX,
                                       (this->windowY + this->windowHeight) - this->footerButtonOffset,
                                       (unsigned char) 0x21);

    if (this->backButton != nullptr) {
        Vector posX = this->backButton->getPosition();
        Globals::other_buttons_x[0] = (int) posX.x;
        Vector posY = this->backButton->getPosition();
        Globals::other_buttons_y[0] = (int) posY.y;
    }
}


static const char g_tagLit0[] = "<c:";

static const char g_tagLit1[] = ">";

static const char g_tagLit2[] = "<c:>";

String Layout::tagString(String in) {
    return String(g_tagLit0) + String(-8388353) + String(g_tagLit1) + in + String(g_tagLit2);
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


void Layout::drawHeader(String title, bool transition) {
    Globals::Canvas->SetColor(this->drawColor);
    int iw = Globals::Canvas->GetImage2DWidth(this->headerCapImage);
    int ih = Globals::Canvas->GetImage2DHeight(this->headerCapImage);
    Globals::Canvas->DrawImage2D(this->headerCapImage, this->windowX, this->windowY);
    this->drawBGPattern(this->headerPatternImage, this->windowX + iw,
                        this->windowY, this->windowWidth - iw * 2, ih);
    Globals::Canvas->DrawImage2D(this->headerCapImage,
                                  this->windowWidth + this->windowX,
                                  this->windowY, iw, ih, (unsigned char) 0x11,
                                  (unsigned char) 0x12, (unsigned char) 0x01);
    if (title.size() != 0) {
        Globals::Canvas->DrawImage2D(this->headerIconImage, this->windowX, this->windowY);
        Globals::Canvas->DrawString(Globals::font, title,
                                     Globals::layout->field_0x28 + Globals::layout->field_0x44 + this->windowX,
                                     this->headerTitleY + this->windowY, false);
    }
    this->helpButtonEnabled = transition;
    if (transition && this->choiceWindowOpen == 0)
        this->helpButton->draw();
}


static const char g_mrLit0[] = "";

static const char g_mrLit1[] = "";

static const char g_mrLit2[] = "+ ";

void Layout::drawMissionRewardMessage(bool transition) {
    if (this->rewardMessageActive != 0) {
        PaintCanvas *pc = Globals::Canvas;
        unsigned saved = pc->GetColor();
        unsigned origColor = this->drawColor;
        pc->SetColor(0xffffffff);

        int t = this->rewardMessageTimer;
        float a;
        if (t < 2000) {
            a = (float) t / 2000.0f;
        } else if (t > 5000) {
            a = (float) (7000 - t) / 2000.0f;
        } else {
            a = 1.0f;
        }
        unsigned col = (unsigned) ((int) (a * 255.0f) - 256);
        pc->SetColor(col);

        unsigned newColor = pc->GetColor();
        int boxW = this->rewardBoxWidth;
        int boxH = this->rewardBoxHeight;
        int boxX = this->rewardBoxX;
        int boxY = this->rewardBoxY;
        this->drawColor = newColor;

        if (transition != 0) {
            int sw = Globals::w;
            this->drawBox(2, (sw >> 1) - (boxH >> 1), boxX, boxH, boxW, String(g_mrLit0));

            sw = Globals::w;
            this->drawBox(8, (sw >> 1) - (boxH >> 1), boxX, boxH, boxW, String(g_mrLit1));
        }

        int sh = Globals::w;
        pc->DrawImage2D(this->field_0x3a4, sh >> 1, boxX, (unsigned char) 0x11, (unsigned char) 0x14);

        String *txt = Globals::gameText->getText(this->rewardMessageFlag ? 3206 : 216);
        String line(*txt, false);

        sh = Globals::w;
        unsigned font = Globals::font;
        int tw = pc->GetTextWidth(font, line);
        pc->DrawString(font, line, (sh >> 1) - (tw >> 1), boxX + boxY, false);

        String suffix(g_mrLit2);
        String credits = this->formatCredits(this->rewardCredits);
        line = suffix + credits;

        sh = Globals::w;
        tw = pc->GetTextWidth(font, line);
        pc->DrawString(font, line, (sh >> 1) - (tw >> 1),
                       this->rewardBoxY2 + boxX, false);
        pc->SetColor(saved);
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
    this->drawBox(style, x, y, w, h, String(text, false), 1u);
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
    this->drawWindow(String(title, false), false);
}

void Layout::drawWindow(String title, bool drawBG) {
    this->drawWindow(String(title, false), 0, 0,
                     Globals::w, Globals::h - Globals::layout->field_0x8, drawBG);
}
