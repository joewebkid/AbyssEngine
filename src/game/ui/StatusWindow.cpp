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

static Layout **g_SWm_layout = nullptr;
static int *g_SWm_height = nullptr;
static int *g_SWm_force = nullptr;
static unsigned char *g_SWm_btnFlag = nullptr;
static Layout **g_StatusWindow_layout = nullptr;
static unsigned char *g_StatusWindow_btnFlag = nullptr;

StatusWindow::~StatusWindow() {
    if (this->tabButtons != 0) {
        ArrayReleaseClasses(*this->tabButtons); ArrayRemoveAll(*(this->tabButtons));
        delete this->tabButtons;
    }
    this->tabButtons = 0;

    if (this->medalButtons != 0) {
        ArrayReleaseClasses(*this->medalButtons); ArrayRemoveAll(*(this->medalButtons));
        delete this->medalButtons;
    }
    this->medalButtons = 0;

    if (this->imageParts != 0) {
        ArrayReleaseClasses(*this->imageParts); ArrayRemoveAll(*(this->imageParts));
        delete this->imageParts;
    }
    this->imageParts = 0;

    if (this->detailLines != 0) {
        ArrayReleaseClasses(*this->detailLines); ArrayRemoveAll(*(this->detailLines));
        delete this->detailLines;
    }
    this->detailLines = 0;
}

int StatusWindow::OnTouchMove(int x, int y) {
    Layout *layout = *g_SWm_layout;
    if ((layout->field_0xc < y && y < *g_SWm_height - layout->field_0x10) || *g_SWm_force != 0) {
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
                layout = *g_SWm_layout;
            }
        }
    }
    layout->OnTouchMove(x, y);
    if (*g_SWm_btnFlag == 0) {
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

void Status_replaceHash(void *out, void *key, void *a, void *b);

static Layout **g_swe_layout = nullptr;
static char *g_swe_dialogBlock = nullptr;
static void *g_swe_status = nullptr;
static GameText **g_swe_gameText = nullptr;
static Globals **g_swe_globals = nullptr;
static void *g_swe_font = nullptr;

void StatusWindow::OnTouchEnd(int x, int y) {
    int vy = this->scrollVelocity;
    int newOff = this->scrollOffset + vy;
    float vf = (float) vy;
    int absvy = vy < 0 ? -vy : vy;
    Layout *layout = *g_swe_layout;

    this->scrollDamping = 0.9f;
    this->isDragging = 0;
    this->scrollOffset = newOff;
    this->scrollTarget = newOff;
    this->scrollVelocityF = (absvy > 3) ? vf : 0.0f;

    if (layout->OnTouchEnd(x, y) != 0)
        return;

    if (*g_swe_dialogBlock == 0) {
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
                Achievements *ach = Achievements::gAchievements;
                int *medals = ach->getMedals();
                int elite = ach->isEliteMedal(i);
                if (elite != 0 || medals[i] != 0) {
                    if (this->selectedMedal >= 0) {
                        (*this->medalButtons)[this->selectedMedal]->setAlwaysPressed(false);
                    }
                    this->selectedMedal = i;

                    if (this->detailLines != 0) {
                        ArrayReleaseClasses(*this->detailLines); ArrayRemoveAll(*(this->detailLines));
                        delete this->detailLines;
                    }
                    this->detailLines = new Array<String *>();

                    int count = (elite == 0) ? medals[this->selectedMedal] : 1;

                    String hdr, valStr, valTmp, full;
                    void *key = *(void **) g_swe_status;
                    String *t = (*g_swe_gameText)->getText(this->selectedMedal + 0x610);
                    hdr.Set((t)->data);
                    void *val = (void *) (intptr_t) ach->getValue(this->selectedMedal, count);
                    valStr.Set(((String *) val)->data);
                    valTmp.Set((valStr).data);
                    Status_replaceHash(&full, key, &hdr, &valTmp);

                    String hint = this->getMedalHintText(this->selectedMedal);
                    full += hint;

                    (*g_swe_globals)->getLineArray(
                        static_cast<unsigned int>(reinterpret_cast<std::size_t>(*(void **) g_swe_font)),
                        full, this->boxWidth - layout->field_0x4c * 2, this->detailLines);
                    (*this->medalButtons)[i]->setAlwaysPressed(true);
                }
                break;
            }
        }
    }

    if (layout->helpPressed() != 0) {
        if (this->activeTab == 1) {
            String title;
            String *t = (*g_swe_gameText)->getText(0x287);
            title.Set((t)->data);
            layout->initHelpWindow(title);
        } else if (this->activeTab == 0) {
            String title;
            String *t = (*g_swe_gameText)->getText(0x280);
            title.Set((t)->data);
            layout->initHelpWindow(title);
        }
    }
}

int StatusWindow::OnTouchBegin(int x, int y) {
    Layout *layout = *g_StatusWindow_layout;
    this->touchStartY = y;
    this->lastTouchY = y;
    this->scrollVelocity = 0;
    this->isDragging = 1;
    layout->OnTouchBegin(x, y);
    if (*g_StatusWindow_btnFlag == 0) {
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

static GameText **g_swh_gameText = nullptr;

String StatusWindow::getMedalHintText(int medalIndex) {
    int *medals = Achievements::gAchievements->getMedals();
    int state = medals[medalIndex];

    String out;
    { if (out.data) delete[] out.data; out.data = nullptr; out.length = 0; }

    if (state != 0) {
        String tmpA;
        String tmpB;

        if (medalIndex == 2 && state == 2) {
            tmpA.ctor_char("\n", false);
            String *hdr = (*g_swh_gameText)->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *base = Status::gStatus;
            Array<bool> *list = base->field_94;
            for (unsigned int i = 0; i < list->size(); i++) {
                if ((*list)[i] == 0) {
                    tmpA.ctor_char("\n", false);
                    String *t = (*g_swh_gameText)->getText(0x594 + (int) i);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 3 && state == 2) {
            tmpA.ctor_char("\n", false);
            String *hdr = (*g_swh_gameText)->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *base = Status::gStatus;
            Array<bool> *list = base->field_98;
            for (unsigned int i = 0; i < list->size(); i++) {
                if ((*list)[i] == 0) {
                    tmpA.ctor_char("\n", false);
                    String *t = (*g_swh_gameText)->getText(0x59f + (int) i);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 9 && state == 2) {
            tmpA.ctor_char("\n", false);
            String *hdr = (*g_swh_gameText)->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *base = Status::gStatus;
            Array<bool> *list = base->field_ac;
            for (unsigned int i = 0; i < list->size(); i++) {
                if ((*list)[i] == 0) {
                    tmpA.ctor_char("\n", false);
                    String *t = (*g_swh_gameText)->getText(0x57e + (int) i);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 0xd && state == 2) {
            tmpA.ctor_char("\n", false);
            String *hdr = (*g_swh_gameText)->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *root = Status::gStatus;
            for (unsigned int i = 0; i < 0xd; i++) {
                BluePrint *bp = (*root->bluePrints)[i];
                if (bp->locked == 0) {
                    tmpA.ctor_char("\n", false);
                    int idx = bp->getIndex();
                    String *t = (*g_swh_gameText)->getText(idx + 0x4fa);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        } else if (medalIndex == 0xe && state == 2) {
            tmpA.ctor_char("\n", false);
            String *hdr = (*g_swh_gameText)->getText(0x114);
            tmpB = tmpA + *hdr;
            out += tmpB;

            Status *root = Status::gStatus;
            for (unsigned int i = 0; i < 0xd; i++) {
                BluePrint *bp = (*root->bluePrints)[i];
                if (bp->productionCount == 0) {
                    tmpA.ctor_char("\n", false);
                    int idx = bp->getIndex();
                    String *t = (*g_swh_gameText)->getText(idx + 0x4fa);
                    tmpB = tmpA + *t;
                    out += tmpB;
                }
            }
        }
    }

    return out;
}

void *Achievements_get();

int Achievements_isUnlocked(void *ach, int index);

static ImageFactory **g_sw_imageFactory = nullptr;
static int g_sw_charDef = 0;

void StatusWindow::reInit() {
    this->imageParts = (*g_sw_imageFactory)->loadChar(&g_sw_charDef);

    int a0 = Achievements_isUnlocked(Achievements_get(), 0);
    int a1 = Achievements_isUnlocked(Achievements_get(), 1);
    int a2 = Achievements_isUnlocked(Achievements_get(), 2);
    int a3 = Achievements_isUnlocked(Achievements_get(), 3);

    PaintCanvas *canvas = PaintCanvas::gCanvas;

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

void Globals_drawLines(void *globals, void *font, void *arr, int y, char clip);

static int *g_swd_dimW = nullptr;
static int *g_swd_dimH = nullptr;
static Layout **g_swd_layout = nullptr;
static ImageFactory **g_swd_imageFactory = nullptr;
static void *g_swd_font = nullptr;
static Globals **g_swd_globals = nullptr;
static char *g_swd_landscape = nullptr;
static int *g_swd_textId = nullptr;
static GameText **g_swd_gameText = nullptr;

void StatusWindow::draw() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;
    Layout *layout = *g_swd_layout;
    void *font = *(void **) g_swd_font;
    char *land = g_swd_landscape;
    int screenW = *g_swd_dimW;
    int screenH = *g_swd_dimH;

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
    if (*land == 0) {
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
        sep.ctor_char("\xa1", false);
    else
        sep.ctor_char(":", false);

    int tab = this->activeTab;
    char drewStats = 0;

    if (tab == 0 || *land != 0) {
        int boxW = this->boxWidth;
        int x0 = layout->buttonInsetX;
        int pad = layout->field_0x2c;
        String lbl;

        String *t = (*g_swd_gameText)->getText(*g_swd_textId);
        lbl.Set((t)->data);
        layout->drawBox(0, x0, top, boxW, layout->field_0x1c, lbl, 0);

        int y = layout->field_0x1c + top + pad;

        lbl.ctor_char("", false);
        layout->drawBox(5, x0, y, (boxW >> 1) - pad, layout->field_0x2d8, lbl, 0);
        (*g_swd_imageFactory)->drawChar(this->imageParts, layout->field_0x4c + x0, y, false);
        String credTmp = Layout::formatCredits(Status::gStatus->getCredits());
        creditStr = credTmp;
        int tw = canvas->GetTextWidth((unsigned) (uintptr_t) font, creditStr);
        canvas->DrawString((unsigned) (uintptr_t) font, creditStr, (((boxW >> 1) - pad) - x0) - tw, y, false);

        String lvlPrefix, lvlText, lvlFull;
        String *lt = (*g_swd_gameText)->getText(*g_swd_textId);
        lvlPrefix.ctor_char(" ", false);
        lvlText = *lt;
        int lvl = Status::gStatus->getLevel();
        lvlFull = lvlText;
        lvlFull += lvl;
        creditStr = lvlFull;
        tw = canvas->GetTextWidth((unsigned) (uintptr_t) font, creditStr);
        canvas->DrawString((unsigned) (uintptr_t) font, creditStr, (((boxW >> 1) - pad) - x0) - tw, y, false);

        String timeStr;
        (*g_swd_globals)->longToTimeStringNoSeconds(Status::gStatus->getPlayingTime(), timeStr);
        tw = canvas->GetTextWidth((unsigned) (uintptr_t) font, creditStr);
        canvas->DrawString((unsigned) (uintptr_t) font, creditStr, (((boxW >> 1) - pad) - x0) - tw, y, false);

        lbl.ctor_char("", false);
        layout->drawBox(5, (boxW >> 1) + x0 + pad, y, (boxW >> 1) - pad, layout->field_0x2d8, lbl, 0);
        (*g_swd_imageFactory)->drawShip(Status::gStatus->getShip()->getIndex(), x0 + (boxW >> 1) + pad * 2, y);
        String *shipNameTxt = (*g_swd_gameText)->getText(Status::gStatus->getShip()->getIndex());
        canvas->DrawString((unsigned) (uintptr_t) font, *shipNameTxt,
                           x0 + (boxW >> 1) + pad * 3 + layout->field_0x2cc, y, false);

        String frac, fracStr, pct;
        int firePow = Status::gStatus->getShip()->getFirePower();
        float firePowF = *(float *) &firePow;
        frac.Set((long long) (int) ((firePowF - (float) (int) firePowF) * 100.0f));
        fracStr = frac.SubString(0, 2);
        int fp2 = Status::gStatus->getShip()->getFirePower();
        pct.ctor_char("%", false);
        String fpFull = (fp2 + pct) + fracStr;
        creditStr = fpFull;
        tw = canvas->GetTextWidth((unsigned) (uintptr_t) font, creditStr);
        canvas->DrawString((unsigned) (uintptr_t) font, creditStr, ((y + x0) - pad) - tw, y, false);

        String hpStr;
        hpStr.Set((long long) (Status::gStatus->getShip())->getCombinedHP());
        tw = canvas->GetTextWidth((unsigned) (uintptr_t) font, hpStr);
        canvas->DrawString((unsigned) (uintptr_t) font, hpStr, ((y + x0) - pad) - tw, y, false);

        int standingX0 = x0 + (boxW >> 2);
        Standing *standing = (Standing *) (intptr_t) Status::gStatus->getStanding();
        float rate = standing->getStandingRate(0);
        canvas->DrawImage2D(this->standingEmblemImage, standingX0, y, (unsigned char) '\x11');
        canvas->DrawRegion2D(this->standingBarImage, this->standingBarWidth, 0,
                             (int) -(rate * (float) this->standingBarWidth), this->standingBarHeight,
                             -(rate * (float) this->standingBarWidth), 0, 0, 0, standingX0);
        canvas->DrawImage2D(this->standingFrameImage,
                            (int) ((float) standingX0 - rate * (float) this->standingBarWidth),
                            y, (unsigned char) '\x11');

        int standingX1 = (boxW - ((boxW - pad * 2) >> 2)) + pad * 2;
        Standing *standing2 = (Standing *) (intptr_t) Status::gStatus->getStanding();
        float rate2 = standing2->getStandingRate(1);
        canvas->DrawImage2D(this->standingEmblemImage, standingX1, y, (unsigned char) '\x11');
        canvas->DrawRegion2D(this->standingBarImage, this->standingBarWidth, 0,
                             (int) -(rate2 * (float) this->standingBarWidth), this->standingBarHeight,
                             -(rate2 * (float) this->standingBarWidth), 0, 0, 0, standingX1);
        canvas->DrawImage2D(this->standingFrameImage,
                            (int) ((float) standingX1 - rate2 * (float) this->standingBarWidth),
                            y, (unsigned char) '\x11');

        Status *st = Status::gStatus;
        String rowStr;
        int rowX = layout->field_0x4c + x0;
        for (unsigned r = 0; r < 6; r++) {
            int rowVal;
            switch (r) {
                case 0: rowVal = st->getMissionCount();
                    break;
                case 1: rowVal = st->getKills();
                    break;
                case 2: rowVal = st->getCapturedCrates();
                    break;
                case 3: rowVal = st->getStationsVisited();
                    break;
                case 4: rowVal = st->getJumpgateUsed();
                    break;
                default: rowVal = st->getGoodsProduced();
                    break;
            }
            String *labelTxt = (*g_swd_gameText)->getText(*g_swd_textId);
            canvas->DrawString((unsigned) (uintptr_t) font, *labelTxt, rowX, y, false);
            rowStr.Set((long long) (rowVal));
            tw = canvas->GetTextWidth((unsigned) (uintptr_t) font, rowStr);
            canvas->DrawString((unsigned) (uintptr_t) font, rowStr, ((y + x0) - pad) - tw, y, false);
        }

        drewStats = *land;
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
            String *t = (*g_swd_gameText)->getText(*g_swd_textId);
            hdr.Set((t)->data);
            layout->drawBox(0, boxW + x0 * 2, top, x0 + boxW, layout->field_0x1c, hdr, 0);
            gridY0 += layout->field_0x1c + layout->field_0x2c;
        }

        for (int i = 0; i < this->medalCount; i++) {
            int col = (int) ((unsigned) i / 3u);
            int by = col * rowH + gridY0 + this->scrollOffset;
            (*this->medalButtons)[i]->setPosition((i - col * 3) * third + gridX0, by, 0x44);
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

            static_cast<Globals *>(*g_swd_globals)->drawLines((unsigned) (uintptr_t) font, this->detailLines,
                                                              layout->field_0x4c + layout->buttonInsetX,
                                                              (char) screenH);
        }
    }

    String header;
    String *ht = (*g_swd_gameText)->getText(*g_swd_textId);
    header.Set((ht)->data);
    layout->drawHeader(header);
    layout->drawFooter();

    if (*land == 0) {
        for (unsigned int i = 0; i < this->tabButtons->size(); i++)
            (*this->tabButtons)[i]->draw();
    }
}

static void *g_sw_gameTextDef = nullptr;
static Layout **g_sw_layout = nullptr;
static void *g_sw_layoutW = nullptr;
static unsigned char *g_sw_tabIndex = nullptr;
static int g_sw_screenH = 0;

StatusWindow::StatusWindow() {
    this->tabButtons = new Array<TouchButton *>();
    ArraySetLength(2, *(this->tabButtons));

    Layout *layout = *g_sw_layout;
    int layoutW = *(int *) *(void **) g_sw_layoutW;

    String *t0 = ((GameText *) *(void **) g_sw_gameTextDef)->getText(0xa8);
    int helpOff = layout->getHelpButtonOffset();
    TouchButton *b0 = new TouchButton(*t0, 3, layoutW - helpOff, 0, 0x12);
    (*this->tabButtons)[1] = b0;

    String *t1 = ((GameText *) *(void **) g_sw_gameTextDef)->getText(0x241);
    int helpOff2 = layout->getHelpButtonOffset();
    TouchButton *b1 = new TouchButton(*t1, 3, 0, 0, 0x12);
    int w1 = b1->getWidth();
    b1->setPosition(((layoutW - helpOff2) - w1) + layout->field_0x38, 0, 0x12);
    (*this->tabButtons)[0] = b1;

    unsigned int defTab = *g_sw_tabIndex;
    this->activeTab = defTab;
    (*this->tabButtons)[defTab]->setAlwaysPressed(true);

    this->detailLines = 0;
    this->medalRowHeight = layout->field_0x84;
    this->selectedMedal = -1;

    this->medalButtons = new Array<TouchButton *>();
    this->medalCount = 0x2d;
    ArraySetLength(0x2d, *(this->medalButtons));

    int *medalIds = Achievements::gAchievements->getMedals();
    for (int i = 0; i < this->medalCount; i++) {
        int medal = medalIds[i];
        String *txt = ((GameText *) *(void **) g_sw_gameTextDef)->getText(0x5e3 + i);
        TouchButton *btn = new TouchButton(i, medal, *txt, 0, 0, 'D');
        (*this->medalButtons)[i] = btn;
    }

    this->reInit();

    PaintCanvas *canvas = PaintCanvas::gCanvas;
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
    if (*g_sw_tabIndex == 0) {
        lineH = this->medalRowHeight * (this->medalCount / 3);
    } else {
        lineH = (this->medalCount / 3) * this->medalRowHeight +
                layout->field_0x1c + layout->field_0x2c;
    }
    heights[1] = lineH + 10;

    this->contentHeight = heights[this->activeTab];
    this->viewportHeight =
    (((g_sw_screenH - layout->field_0x10) - layout->field_0xc) -
     layout->field_0x20) - layout->field_0x24;
}
