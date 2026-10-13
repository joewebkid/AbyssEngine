#include "game/ui/StatusWindow.h"
#include "game/ship/Ship.h"
#include "game/mission/BluePrint.h"
#include "engine/render/PaintCanvas.h"
#include "game/mission/Status.h"
#include "game/mission/Achievements.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "engine/render/ImagePart.h"
#include "game/ui/Layout.h"
#include "game/core/Globals.h"
#include "game/world/Standing.h"
#include "game/core/String.h"
#include "game/ui/TouchButton.h"
#include <cstring>

StatusWindow::~StatusWindow() {
    if (this->tabButtons != 0) {
        ArrayReleaseClasses(*this->tabButtons);
        delete this->tabButtons;
    }
    this->tabButtons = 0;

    if (this->medalButtons != 0) {
        ArrayReleaseClasses(*this->medalButtons);
        delete this->medalButtons;
    }
    this->medalButtons = 0;

    if (this->imageParts != 0) {
        ArrayReleaseClasses(*this->imageParts);
        delete this->imageParts;
    }
    this->imageParts = 0;

    if (this->detailLines != 0) {
        ArrayReleaseClasses(*this->detailLines);
        delete this->detailLines;
    }
    this->detailLines = 0;
}

int StatusWindow::OnTouchMove(int x, int y) {
    Layout *layout = Globals::layout;
    if ((layout->field_0xc < y && y < Globals::h - layout->field_0x10) || Globals::mouse_wheel != 0) {
        int delta = y - this->lastTouchY;
        this->scrollVelocity = delta;
        this->scrollDamping = 1.0f;
        this->scrollOffset += delta;
        this->lastTouchY = y;
        if (this->selectedMedal >= 0) {
            int dragDist = this->touchStartY - y;
            if (dragDist < 0) dragDist = -dragDist;
            if (dragDist > 3) {
                (*this->medalButtons)[this->selectedMedal]->setAlwaysPressed(0);
                this->selectedMedal = -1;
                layout = Globals::layout;
            }
        }
    }
    layout->OnTouchMove(x, y);
    if (Globals::iPad == 0) {
        for (unsigned i = 0; i < this->tabButtons->size(); ++i)
            (*this->tabButtons)[i]->OnTouchMove(x, y);
    }
    if (this->activeTab == 1) {
        Achievements *ach = Achievements::gAchievements;
        int *medals = ach->getMedals();
        for (int i = 0; i < this->medalCount; ++i) {
            if (medals[i] != 0 || ach->isEliteMedal(i) != 0)
                (*this->medalButtons)[i]->OnTouchMove(x, y);
        }
    }
    return 0;
}

float StatusWindow::getRelativeScrollStartPos() {
    int range = this->scrollOffset;
    if (range > 0) return 0.0f;
    return -(float) range / (float) this->contentHeight;
}

int StatusWindow::OnTouchEnd(int x, int y) {
    int vy = this->scrollVelocity;
    int newOff = this->scrollOffset + vy;
    float vf = (float) vy;
    int absvy = vy < 0 ? -vy : vy;
    Layout *layout = Globals::layout;

    this->scrollDamping = 0.9f;
    this->isDragging = 0;
    this->scrollOffset = newOff;
    this->scrollTarget = newOff;
    this->scrollVelocityF = (absvy > 3) ? vf : 0.0f;

    if (layout->OnTouchEnd(x, y) != 0)
        return 1;

    if (Globals::iPad == 0) {
        for (unsigned int i = 0; i < this->tabButtons->size(); i++) {
            if ((*this->tabButtons)[i]->OnTouchEnd(x, y) != 0) {
                this->activeTab = i;
                this->contentHeight = this->tabHeights[i];
                this->scrollOffset = 0;
            }
        }
    }

    if (this->activeTab == 1) {
        for (int i = 0; i < this->medalCount; i++) {
            if ((*this->medalButtons)[i]->OnTouchEnd(x, y) != 0) {
                Achievements *ach = static_cast<Achievements *>(Globals::achievements);
                int *medals = ach->getMedals();
                int elite = ach->isEliteMedal(i);
                if (elite != 0 || medals[i] != 0) {
                    if (this->selectedMedal >= 0) {
                        (*this->medalButtons)[this->selectedMedal]->setAlwaysPressed(false);
                    }
                    this->selectedMedal = i;

                    if (this->detailLines != 0) {
                        ArrayReleaseClasses(*this->detailLines);
                        delete this->detailLines;
                    }
                    this->detailLines = nullptr;
                    this->detailLines = new Array<String *>();

                    int count = (elite == 0) ? medals[this->selectedMedal] : 1;

                    String hdr(*Globals::gameText->getText(this->selectedMedal + 0x610), false);
                    String valStr(ach->getValue(this->selectedMedal, count));
                    String valTmp(valStr, false);
                    String full = Globals::status->replaceHash(hdr, valTmp);

                    String hint = this->getMedalHintText(this->selectedMedal);
                    full += hint;

                    static_cast<Globals *>(Globals::globals)->getLineArray(
                        Globals::font,
                        full, this->boxWidth - layout->field_0x4c * 2, this->detailLines);
                    (*this->medalButtons)[i]->setAlwaysPressed(true);
                }
                break;
            }
        }
    }

    if (layout->helpPressed() != 0) {
        if (this->activeTab == 1) {
            String title(*Globals::gameText->getText(0x287), false);
            layout->initHelpWindow(title);
        } else if (this->activeTab == 0) {
            String title(*Globals::gameText->getText(0x280), false);
            layout->initHelpWindow(title);
        }
    }
    return 0;
}

int StatusWindow::OnTouchBegin(int x, int y) {
    Layout *layout = Globals::layout;
    this->touchStartY = y;
    this->lastTouchY = y;
    this->scrollVelocity = 0;
    this->isDragging = 1;
    layout->OnTouchBegin(x, y);
    if (Globals::iPad == 0) {
        for (unsigned i = 0; i < this->tabButtons->size(); ++i)
            (*this->tabButtons)[i]->OnTouchBegin(x, y);
    }
    if (this->activeTab == 1) {
        Achievements *ach = Achievements::gAchievements;
        int *medals = ach->getMedals();
        for (int i = 0; i < this->medalCount; ++i) {
            if (medals[i] != 0 || ach->isEliteMedal(i) != 0)
                (*this->medalButtons)[i]->OnTouchBegin(x, y);
        }
    }
    return 0;
}

float StatusWindow::getRelativeScrollHeight() {
    int a = this->contentHeight;
    int b = this->viewportHeight;
    if (b > a) return 0.0f;
    int range = this->scrollOffset;
    int num;
    if (range >= 1) {
        num = b - range;
    } else if (range < b - a) {
        num = range + a;
    } else {
        num = b;
    }
    return (float) num / (float) a;
}

void StatusWindow::update(int frameTime) {
    (void) frameTime;

    if (this->isDragging == 0) {
        float v = this->scrollDamping * this->scrollVelocityF;
        this->scrollVelocityF = v;

        float mag = v > 0.0f ? v : -v;
        if (mag > 1.0f) {
            this->scrollOffset = (int) (v + (float) this->scrollOffset);
        }
    }

    int off = this->scrollOffset;
    if (off > 0) {
        this->scrollDamping = 1.0f;
        this->scrollVelocityF = (float) (-off) * 0.5f;
    }

    int bottom = this->viewportHeight - this->contentHeight;
    if (bottom < 0) {
        if (off < bottom) {
            this->scrollDamping = 1.0f;
            this->scrollVelocityF = (float) (bottom - off) * 0.5f;
        }
    } else {
        this->scrollVelocityF = 0.0f;
        this->scrollOffset = 0;
    }

    for (unsigned int idx = 0; idx < this->tabButtons->size(); idx++) {
        (*this->tabButtons)[idx]->setAlwaysPressed(idx == this->activeTab);
    }
}

String StatusWindow::getMedalHintText(int medalIndex) {
    int *medals = static_cast<Achievements *>(Globals::achievements)->getMedals();
    int state = medals[medalIndex];

    String out;

    if (state != 0) {
        String tmpA;
        String tmpB;

        if (medalIndex == 2 && state == 2) {
            tmpA.ctor_char("\n\n", false);
            String *hdr = Globals::gameText->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *base = Globals::status;
            Array<bool> *list = base->field_94;
            for (unsigned int i = 0; i < list->size(); i++) {
                if ((*list)[i] == 0) {
                    tmpA.ctor_char("\n- ", false);
                    String *t = Globals::gameText->getText(0x594 + (int) i);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 3 && state == 2) {
            tmpA.ctor_char("\n\n", false);
            String *hdr = Globals::gameText->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *base = Globals::status;
            Array<bool> *list = base->field_98;
            for (unsigned int i = 0; i < list->size(); i++) {
                if ((*list)[i] == 0) {
                    tmpA.ctor_char("\n- ", false);
                    String *t = Globals::gameText->getText(0x59f + (int) i);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 9 && state == 2) {
            tmpA.ctor_char("\n\n", false);
            String *hdr = Globals::gameText->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *base = Globals::status;
            Array<bool> *list = base->field_ac;
            for (unsigned int i = 0; i < list->size(); i++) {
                if ((*list)[i] == 0) {
                    tmpA.ctor_char("\n- ", false);
                    String *t = Globals::gameText->getText(0x57e + (int) i);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 0xd && state == 2) {
            tmpA.ctor_char("\n\n", false);
            String *hdr = Globals::gameText->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *root = Globals::status;
            for (unsigned int i = 0; i < 0xd; i++) {
                BluePrint *bp = (*root->bluePrints)[i];
                if (bp->locked == 0) {
                    tmpA.ctor_char("\n- ", false);
                    int idx = bp->getIndex();
                    String *t = Globals::gameText->getText(idx + 0x4fa);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 0xe && state == 2) {
            tmpA.ctor_char("\n\n", false);
            String *hdr = Globals::gameText->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *root = Globals::status;
            for (unsigned int i = 0; i < 0xd; i++) {
                BluePrint *bp = (*root->bluePrints)[i];
                if (bp->productionCount == 0) {
                    tmpA.ctor_char("\n- ", false);
                    int idx = bp->getIndex();
                    String *t = Globals::gameText->getText(idx + 0x4fa);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        }
    }

    return out;
}

// Android .bss 0x21839c..0x2183af: five words consumed by loadChar.
static int g_sw_charDef[5] = {};

void StatusWindow::reInit() {
    this->imageParts = static_cast<ImageFactory *>(Globals::imageFactory)
                           ->loadChar(g_sw_charDef);

    int a0 = Globals::status->getStanding()->isEnemy(0);
    int a1 = Globals::status->getStanding()->isEnemy(1);
    int a2 = Globals::status->getStanding()->isEnemy(2);
    int a3 = Globals::status->getStanding()->isEnemy(3);

    PaintCanvas *canvas = Globals::Canvas;

    int id = 0x493;
    if (a1) id = 0x494;
    if (a0) id = 0x495;
    canvas->Image2DCreate((unsigned short) id, this->rankImage0);

    id = 0x492;
    if (a0) id = 0x496;
    if (a1) id = 0x497;
    canvas->Image2DCreate((unsigned short) id, this->rankImage1);

    id = 0x490;
    if (a3) id = 0x498;
    if (a2) id = 0x499;
    canvas->Image2DCreate((unsigned short) id, this->rankImage2);

    id = 0x491;
    if (a2) id = 0x49a;
    if (a3) id = 0x49b;
    canvas->Image2DCreate((unsigned short) id, this->rankImage3);

    this->charImageWidth = canvas->GetImage2DWidth(this->rankImage0);
    this->charImageHeight = canvas->GetImage2DHeight(this->rankImage0);
}

void StatusWindow::draw() {
    PaintCanvas *canvas = Globals::Canvas;
    Layout *layout = Globals::layout;
    unsigned font = Globals::font;
    bool land = Globals::iPad != 0;
    int screenW = Globals::w;
    int screenH = Globals::h;

    canvas->SetColor(0xffu);
    canvas->FillRectangle(0, 0, screenW, screenH);
    canvas->SetColor(0xffffffffu);
    layout->drawBG();

    float relStart = this->getRelativeScrollStartPos();
    int contentH = this->viewportHeight;
    float ch = (float) contentH;
    float relH = this->getRelativeScrollHeight();
    int barH = (int) (relH * ch);
    if (barH > 0 || (int) (relStart * ch) > 0) {
        layout->drawScrollBar((screenW - layout->field_0x48) - layout->buttonInsetX,
                              layout->field_0x20 + layout->field_0xc, contentH, (int) (relStart * ch), barH);
    }

    int top = layout->field_0x20 + layout->field_0xc;
    int colW;
    if (!land) {
        colW = screenW;
        top += this->scrollOffset;
    } else {
        colW = screenW >> 1;
    }
    if (barH > 0)
        colW = (colW - layout->field_0x48) - layout->field_0x2c;
    this->boxWidth = colW + layout->buttonInsetX * -2;

    String creditStr;
    { if (creditStr.data) delete[] creditStr.data; creditStr.data = nullptr; creditStr.length = 0; }
    String sep;
    if (GameText::getLanguage() == 9)
        sep.ctor_char("", false);
    else
        sep.ctor_char(":", false);

    int tab = this->activeTab;
    char drewStats = 0;

    if (tab == 0 || land) {
        int boxW = this->boxWidth;
        int x0 = layout->buttonInsetX;
        int pad = layout->field_0x2c;
        String lbl;

        String *t = Globals::gameText->getText(1597);
        lbl.Set((t)->data);
        layout->drawBox(0, x0, top, boxW, layout->field_0x1c, lbl, 0);

        int y = layout->field_0x1c + top + pad;

        lbl.ctor_char("", false);
        layout->drawBox(5, x0, y, (boxW >> 1) - pad, layout->field_0x2d8, lbl, 0);
        static_cast<ImageFactory *>(Globals::imageFactory)
            ->drawChar(this->imageParts, layout->field_0x4c + x0, y, false);
        String credTmp = Globals::layout->formatCredits(Globals::status->getCredits());
        creditStr = credTmp;
        int textRight = (boxW >> 1) - pad - layout->field_0x4c;
        int textY = y + pad;
        int lineH = layout->field_0x4;
        int tw = canvas->GetTextWidth((unsigned) (uintptr_t) font, creditStr);
        canvas->DrawString(font, creditStr, textRight - tw, textY, false);

        String lvlPrefix, lvlText, lvlFull;
        String *lt = Globals::gameText->getText(321);
        lvlPrefix.ctor_char(" ", false);
        lvlText = *lt;
        int lvl = Globals::status->getLevel();
        lvlFull = lvlText + lvlPrefix;
        lvlFull += lvl;
        creditStr = lvlFull;
        tw = canvas->GetTextWidth(font, creditStr);
        canvas->DrawString(font, creditStr, textRight - tw, textY + lineH, false);

        static_cast<Globals *>(Globals::globals)
            ->longToTimeStringNoSeconds(Globals::status->getPlayingTime(), creditStr);
        tw = canvas->GetTextWidth(font, creditStr);
        canvas->DrawString(font, creditStr, textRight - tw, textY + 2 * lineH, false);

        lbl.ctor_char("", false);
        layout->drawBox(5, (boxW >> 1) + x0 + pad, y, (boxW >> 1) - pad, layout->field_0x2d8, lbl, 0);
        static_cast<ImageFactory *>(Globals::imageFactory)
            ->drawShip(Globals::status->getShip()->getIndex(), x0 + (boxW >> 1) + pad * 2, y);
        String *shipNameTxt = Globals::gameText->getText(Globals::status->getShip()->getIndex() + 913);
        int shipTextX = x0 + (boxW >> 1) + pad * 3 + layout->field_0x2cc;
        int shipValueRight = x0 + boxW - layout->field_0x4c;
        canvas->DrawString(font, *shipNameTxt, shipTextX, textY, false);

        String fireLabel = *Globals::gameText->getText(569) + sep;
        canvas->DrawString(font, fireLabel, shipTextX, textY + lineH, false);
        String frac, fracStr, dot;
        int firePowBits = Globals::status->getShip()->getFirePower();
        float firePowF;
        std::memcpy(&firePowF, &firePowBits, sizeof(firePowF));
        frac.Set((long long) (int) ((firePowF - (float) (int) firePowF) * 100.0f));
        fracStr = frac.SubString(0, 2);
        dot.ctor_char(".", false);
        String fpFull = ((int) firePowF + dot) + fracStr;
        creditStr = fpFull;
        tw = canvas->GetTextWidth(font, creditStr);
        canvas->DrawString(font, creditStr, shipValueRight - tw, textY + lineH, false);

        String hpLabel = *Globals::gameText->getText(570) + sep;
        canvas->DrawString(font, hpLabel, shipTextX, textY + 2 * lineH, false);
        String hpStr;
        hpStr.Set((long long) (Globals::status->getShip())->getCombinedHP());
        tw = canvas->GetTextWidth(font, hpStr);
        canvas->DrawString(font, hpStr, shipValueRight - tw, textY + 2 * lineH, false);

        // ARM 0x15831c: the standing row follows the second stats header and boxes.
        String standingHeader = *Globals::gameText->getText(576);
        int standingHeaderY = y + layout->field_0x2d8 + pad;
        layout->drawBox(0, x0, standingHeaderY, boxW, layout->field_0x1c, standingHeader, 0);
        int standingBoxY = standingHeaderY + layout->field_0x1c + pad;
        int standingBoxH = this->charImageHeight + lineH + pad * 2;
        lbl.ctor_char("", false);
        layout->drawBox(5, x0, standingBoxY, (boxW >> 1) - pad, standingBoxH, lbl, 0);
        layout->drawBox(5, x0 + (boxW >> 1) + pad, standingBoxY,
                        (boxW >> 1) - pad, standingBoxH, lbl, 0);
        int standingRowY = y + layout->field_0x2d8 + layout->field_0x1c + pad * 3;
        int standingCenterY = pad + this->charImageHeight + standingRowY -
                              (this->charImageHeight >> 1);
        int standingBarY = standingCenterY - (this->standingBarHeight >> 1);
        int standingX0 = x0 + (boxW >> 2);
        Standing *standing = Globals::status->getStanding();
        float rate = standing->getStandingRate(0);
        canvas->DrawImage2D(this->standingEmblemImage, standingX0, standingBarY,
                            (unsigned char) 0x11, (unsigned char) 0x14);
        canvas->DrawRegion2D(this->standingBarImage, this->standingBarWidth, 0,
                             (int) -(rate * (float) this->standingBarWidth), this->standingBarHeight,
                             0.0f, 0, 0, standingX0, standingBarY);
        canvas->DrawImage2D(this->standingFrameImage,
                            (int) ((float) standingX0 - rate * (float) this->standingBarWidth),
                            standingCenterY, (unsigned char) 0x11, (unsigned char) 0x44);

        int standingX1 = (boxW - ((boxW - pad * 2) >> 2)) + pad * 2;
        Standing *standing2 = Globals::status->getStanding();
        float rate2 = standing2->getStandingRate(1);
        canvas->DrawImage2D(this->standingEmblemImage, standingX1, standingBarY,
                            (unsigned char) 0x11, (unsigned char) 0x14);
        canvas->DrawRegion2D(this->standingBarImage, this->standingBarWidth, 0,
                             (int) -(rate2 * (float) this->standingBarWidth), this->standingBarHeight,
                             0.0f, 0, 0, standingX1, standingBarY);
        canvas->DrawImage2D(this->standingFrameImage,
                            (int) ((float) standingX1 - rate2 * (float) this->standingBarWidth),
                            standingCenterY, (unsigned char) 0x11, (unsigned char) 0x44);

        // ARM 0x15831c: faction labels and two-column statistics table.
        int standingLabelY = standingRowY + this->charImageHeight + pad;
        int leftLabelX = x0 + layout->field_0x4c;
        int leftValueRight = x0 + (boxW >> 1) - pad - layout->field_0x4c;
        int rightLabelX = x0 + (boxW >> 1) + pad + layout->field_0x4c;
        int rightValueRight = x0 + boxW - layout->field_0x4c;
        String *standingLeft = Globals::gameText->getText(406);
        canvas->DrawString(font, *standingLeft, leftLabelX, standingLabelY, false);
        String *standingLeftValue = Globals::gameText->getText(407);
        tw = canvas->GetTextWidth(font, *standingLeftValue);
        canvas->DrawString(font, *standingLeftValue, leftValueRight - tw,
                           standingLabelY, false);
        String *standingRight = Globals::gameText->getText(408);
        canvas->DrawString(font, *standingRight, rightLabelX, standingLabelY, false);
        String *standingRightValue = Globals::gameText->getText(409);
        tw = canvas->GetTextWidth(font, *standingRightValue);
        canvas->DrawString(font, *standingRightValue, rightValueRight - tw,
                           standingLabelY, false);

        int statsHeaderY = standingLabelY + lineH + pad;
        String statsHeader = *Globals::gameText->getText(577);
        layout->drawBox(0, x0, statsHeaderY, boxW, layout->field_0x1c, statsHeader, 0);
        int statsBoxY = statsHeaderY + layout->field_0x1c + pad;
        int statsBoxH = pad + 6 * lineH;
        lbl.ctor_char("", false);
        layout->drawBox(5, x0, statsBoxY, (boxW >> 1) - pad, statsBoxH, lbl, 0);
        layout->drawBox(5, x0 + (boxW >> 1) + pad, statsBoxY,
                        (boxW >> 1) - pad, statsBoxH, lbl, 0);

        Status *st = Globals::status;
        String rowStr;
        int firstStatY = statsBoxY + pad;
        auto drawStat = [&](int labelId, int value, int labelX, int valueRight,
                            int rowY) __attribute__((always_inline)) {
            String label = *Globals::gameText->getText(labelId) + sep;
            canvas->DrawString(font, label, labelX, rowY, false);
            rowStr.Set((long long) value);
            tw = canvas->GetTextWidth(font, rowStr);
            canvas->DrawString(font, rowStr, valueRight - tw, rowY, false);
        };
        drawStat(568, st->getMissionCount(), leftLabelX, leftValueRight, firstStatY);
        drawStat(176, st->getKills(), leftLabelX, leftValueRight, firstStatY + lineH);
        drawStat(552, st->field_d8, leftLabelX, leftValueRight, firstStatY + 2 * lineH);
        drawStat(559, st->getCapturedCrates(), leftLabelX, leftValueRight,
                 firstStatY + 3 * lineH);
        drawStat(557, st->getStationsVisited(), leftLabelX, leftValueRight,
                 firstStatY + 4 * lineH);
        drawStat(3235, st->field_118, leftLabelX, leftValueRight,
                 firstStatY + 5 * lineH);
        drawStat(564, st->getJumpgateUsed(), rightLabelX, rightValueRight, firstStatY);
        drawStat(558, st->getGoodsProduced(), rightLabelX, rightValueRight,
                 firstStatY + lineH);
        drawStat(560, st->field_a0, rightLabelX, rightValueRight,
                 firstStatY + 2 * lineH);
        drawStat(561, st->field_a4, rightLabelX, rightValueRight,
                 firstStatY + 3 * lineH);
        drawStat(567, st->field_d4, rightLabelX, rightValueRight,
                 firstStatY + 4 * lineH);

        drewStats = land;
        tab = this->activeTab;
    }

    if (drewStats != 0 || tab == 1) {
        int boxW = this->boxWidth;
        int third = boxW / 3;
        int x0 = layout->buttonInsetX;
        int rowH = this->medalRowHeight;
        int gridX0 = drewStats ? (boxW + (third >> 1) + x0) : (x0 + (third >> 1));
        int gridY0 = layout->field_0xc + (rowH >> 1) + layout->field_0x2c;

        if (drewStats != 0) {
            String hdr;
            String *t = Globals::gameText->getText(168);
            hdr.Set((t)->data);
            layout->drawBox(0, boxW + x0 * 2, top + this->scrollOffset,
                            x0 + boxW, layout->field_0x1c, hdr, 0);
            gridY0 += layout->field_0x1c + layout->field_0x2c;
        }

        for (int i = 0; i < this->medalCount; i++) {
            int col = (int) ((unsigned) i / 3u);
            int by = col * rowH + gridY0 + this->scrollOffset;
            (*this->medalButtons)[i]->setPosition((i - col * 3) * third + gridX0, by);
            if (by >= 0 && by <= screenH)
                (*this->medalButtons)[i]->draw();
        }

        if (this->selectedMedal >= 0) {
            canvas->SetColor(0xffffffffu);
            int lines = (int) this->detailLines->size();
            int lineH = layout->field_0x4;
            String lbl;
            lbl.ctor_char("", false);
            layout->drawBox(2, layout->buttonInsetX, (((screenH - layout->field_0x10) -
                                                       layout->field_0x24) - lineH * lines) +
                                                     layout->field_0x4c * -2, this->boxWidth,
                            layout->field_0x4c * 2 + lineH * lines, lbl, 0);

            lbl.ctor_char("", false);
            layout->drawBox(5, layout->buttonInsetX, (((screenH - layout->field_0x10) -
                                                       layout->field_0x24) - lineH * lines) +
                                                     layout->field_0x4c * -2, this->boxWidth,
                            layout->field_0x4c * 2 + lineH * lines, lbl, 0);

            int detailY = screenH - layout->field_0x10 - layout->field_0x24 -
                          layout->field_0x2c - lineH * lines;
            static_cast<Globals *>(Globals::globals)->drawLines(font, this->detailLines,
                                                                layout->field_0x4c + x0,
                                                                detailY, false);
        }
    }

    String header;
    String *ht = Globals::gameText->getText(169);
    header.Set((ht)->data);
    layout->drawHeader(header);
    layout->drawFooter();

    if (!land) {
        for (unsigned int i = 0; i < this->tabButtons->size(); i++)
            (*this->tabButtons)[i]->draw();
    }
}

StatusWindow::StatusWindow() {
    this->tabButtons = new Array<TouchButton *>();
    ArraySetLength(2, *(this->tabButtons));

    Layout *layout = Globals::layout;
    int screenW = Globals::w;

    String *t0 = Globals::gameText->getText(168);
    int helpOff = layout->getHelpButtonOffset();
    TouchButton *b0 = new TouchButton(*t0, 3, screenW - helpOff, 0, 0x12);
    (*this->tabButtons)[1] = b0;

    String *t1 = Globals::gameText->getText(577);
    int helpOff2 = layout->getHelpButtonOffset();
    int firstWidth = b0->getWidth();
    TouchButton *b1 = new TouchButton(*t1, 3,
                                      screenW - helpOff2 - firstWidth + layout->field_0x38,
                                      0, 0x12);
    (*this->tabButtons)[0] = b1;

    unsigned int defTab = Globals::iPad;
    this->activeTab = defTab;
    (*this->tabButtons)[defTab]->setAlwaysPressed(true);

    this->detailLines = 0;
    this->medalRowHeight = layout->field_0x84;
    this->selectedMedal = -1;

    this->medalButtons = new Array<TouchButton *>();
    this->medalCount = 0x2d;
    ArraySetLength(0x2d, *(this->medalButtons));

    int *medalIds = static_cast<Achievements *>(Globals::achievements)->getMedals();
    for (int i = 0; i < this->medalCount; i++) {
        int medal = medalIds[i];
        String *txt = Globals::gameText->getText(1507 + i);
        TouchButton *btn = new TouchButton(i, medal, *txt, 0, 0, 'D');
        (*this->medalButtons)[i] = btn;
    }

    this->reInit();

    PaintCanvas *canvas = Globals::Canvas;
    canvas->Image2DCreate((unsigned short) 0x48e, this->standingEmblemImage);
    canvas->Image2DCreate((unsigned short) 0x48f, this->standingBarImage);
    canvas->Image2DCreate((unsigned short) 0x48d, this->standingFrameImage);
    this->standingBarWidth = canvas->GetImage2DWidth(this->standingEmblemImage) / 2;
    int img3h = canvas->GetImage2DHeight(this->standingEmblemImage);

    this->scrollDamping = 0.0f;
    this->scrollVelocityF = 0.0f;
    this->touchStartY = 0;
    this->isDragging = 0;
    this->scrollOffset = 0;
    this->lastTouchY = 0;
    this->scrollTarget = 0;
    this->scrollVelocity = 0;
    this->standingBarHeight = img3h;

    int *heights = new int[3];
    this->tabHeights = heights;
    int row = ((layout->field_0x1c * 3 + layout->field_0x2d8) +
               layout->field_0x2c * 8) + this->charImageHeight +
              layout->field_0x4 * 7;
    heights[0] = row;

    int lineH;
    if (Globals::iPad) {
        lineH = (this->medalCount / 3) * this->medalRowHeight +
                layout->field_0x1c + layout->field_0x2c;
    } else {
        lineH = this->medalRowHeight * (this->medalCount / 3);
    }
    heights[1] = lineH + 10;

    this->contentHeight = heights[this->activeTab];
    this->viewportHeight =
    (((Globals::h - layout->field_0x10) - layout->field_0xc) -
     layout->field_0x20) - layout->field_0x24;
}
