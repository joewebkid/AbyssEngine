#include "game/ui/Hud.h"
#include "game/mission/Mission.h"
#include "game/mission/Item.h"
#include "engine/render/Sprite.h"
#include "engine/core/GameText.h"
#include "game/core/Globals.h"
#include "game/ship/PlayerEgo.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"
#include "game/ui/TouchButton.h"
#include "game/ui/ListItem.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include "game/world/Level.h"
#include "game/ship/Player.h"
#include "engine/render/PaintCanvas.h"
#include "game/ui/Layout.h"

void Status_replaceHash(void *out, void *tmpl, void *a, void *b, void *c);

void Image2DCreate(void *canvas, unsigned short id, void *outField);

void Hud_loadImages(Hud * self);

void Hud_buildQuickMenu(Hud *self, int menuType);

static unsigned int g_Hud_heImportantMask = 0;

void drawControlsInterface(long long t0, long long t1, PlayerEgo *ego, bool letterbox,
                           unsigned int x, unsigned int y) {
    (void) t0;
    (void) t1;
    (void) ego;
    (void) letterbox;
    (void) x;
    (void) y;
}

void Hud::enableFireForTutorial(bool value) {
    this->fireForTutorial = value;
}

void Hud::setVisible(bool value) {
    this->visible = value;
}

void Hud::drawCredits() {
}

void Hud::drawBigNumber(int x, int y, int value, bool flag) {
    (void) x;
    (void) y;
    (void) value;
    (void) flag;
}

int Hud::hudAction(int action, Level *lvl, Radar *radar) {
    (void) action;
    (void) lvl;
    (void) radar;
    return 0;
}

void Hud::setTimeExtender(bool p1, bool p2, bool p3, bool p4) {
    this->field_0x0 = p1;
    this->field_0x280 = p3;
    this->field_0x281 = p4;
    if (p2 && p3) {
        this->timeExtenderDuration = 0x7d0;
        this->timeExtenderTimer = 0x50;
    }
}

void Hud::playerHit() {
    this->shieldHitFlash = 1;
    this->hitFlashTimer = 0;
}

void Hud::addToEventQueue(ListItem *item) {
    Array<ListItem *> *q = this->eventQueue;
    unsigned int idx = 0;
    do {
        unsigned int next = idx + 1;
        if (next >= q->size())
            return;
        idx = next;
    } while ((*q)[idx] != 0);
    (*q)[idx] = item;
    this->eventQueueDirty = 1;
}

unsigned int Hud::firePressed() {
    return (this->touchFlags >> 4) & 1;
}

void Hud::resetAnalogStick() {
    this->lockBracketX = this->reticleX;
}

float Hud::getAnalogY() {
    float num = (float) ((int) this->lockBracketY - (int) this->reticleY);
    float den = (float) this->analogStickRadius;
    return num / den;
}

uint8_t Hud::cargoFull() {
    return this->cargoFullFlag;
}

unsigned int Hud::touchEnd(unsigned int a, unsigned int b, void *key) {
    int i = 0;
    unsigned int ret = 0;
    for (; i != 0x19; i = i + 1) {
        if ((*this->keyArray)[i] == key) {
            ret = (unsigned int) this->elementBits[i];
            this->touchFlags = this->touchFlags & ~ret;
            (*this->keyArray)[i] = 0;
            this->elementBits[i] = 0;
        }
    }
    if (this->quickMenuOpen != 0) {
        Array<TouchButton *> *btns = this->menuButtons;
        if (btns != 0) {
            for (unsigned int j = 0; j < btns->size(); j = j + 1) {
                (*btns)[j]->OnTouchEnd((int) a, (int) b);
                btns = this->menuButtons;
            }
        }
    }
    return ret;
}

void Hud::releaseAllKeys() {
    this->touchFlags = 0;
    for (int i = 0; i != 0x19; i++) {
        Array<void *> *p = this->keyArray;
        if (p != 0) {
            if ((*p)[i] != 0)
                (*p)[i] = 0;
        }
        int *q = this->elementBits;
        q[i] = 0;
    }
    this->field_0x288 = 0;
}

void Hud::closeHudMenu() {
    if (this->menuButtons != 0) {
        ArrayReleaseClasses(*this->menuButtons); ArrayRemoveAll(*(this->menuButtons));
        delete this->menuButtons;
        this->menuButtons = 0;
    }
    this->quickMenuOpen = 0;
}

float Hud::getAnalogX() {
    float num = (float) ((int) this->lockBracketX - (int) this->reticleX);
    float den = (float) this->analogStickRadius;
    return num / den;
}

void Hud::setAutofireEnabled(bool value) {
    this->autofireEnabled = value;
}

uint8_t Hud::isHackingGameActive() {
    return this->hackingGameActive;
}

void Hud::setHackingGameActive(bool value) {
    this->hackingGameActive = value;
}

void Hud::setJumpMapSelected(bool value) {
    this->jumpMapSelectedFlag = value;
}

uint8_t Hud::jumpMapSelected() {
    return this->jumpMapSelectedFlag;
}

static void **g_Hud_font = nullptr;
static void **g_Hud_canvas2 = nullptr;
static void **g_Hud_screenW = nullptr;
static Level **g_Hud_level = nullptr;

void Hud::draw(long long t0, long long t1, PlayerEgo *ego, bool letterbox, unsigned int x, unsigned int y) {
    (void) t0;
    (void) t1;
    (void) x;
    (void) y;

    this->letterbox = (unsigned char) letterbox;

    PaintCanvas *canvas = PaintCanvas::gCanvas;

    // --- reticle and lock brackets ---
    canvas->DrawImage2D((unsigned) this->reticleImage, this->field_0x42c, 0);
    {
        unsigned char flags = this->touchFlags;
        unsigned short bx, by;
        int img;
        if ((flags & 0x40) != 0) {
            bx = this->reticleX;
            by = this->reticleY;
            img = this->lockBracketLockedImage;
            this->lockBracketX = bx;
            this->lockBracketY = by;
        } else {
            bx = this->lockBracketX;
            by = this->lockBracketY;
            img = this->lockBracketImage;
        }
        canvas->DrawImage2D((unsigned) img, bx, by, '\x11');
    }

    // --- radar / orbit marker ---
    {
        Status *st = Status::gStatus;
        bool show = false;
        if (st->inAlienOrbit() == 0) {
            show = true;
        } else if (st->getCurrentCampaignMission() == 0x9a) {
            Level *lvl = *g_Hud_level;
            if (lvl != 0 && lvl->getNumDockingTargets() > 0) show = true;
        }
        if (show && st->getCurrentCampaignMission() > 1) {
            int img = ((this->touchFlags & 0x80) != 0)
                          ? this->orbitMarkerActiveImage
                          : this->orbitMarkerIdleImage;
            canvas->DrawImage2D((unsigned) img, this->field_0x3f8, 0);
        }
    }

    // --- shield/armor bars ---
    {
        PlayerEgo *e = ego;
        Player *player = *(Player **) ego;
        float scale = (float) this->field_0x446;
        canvas->SetColor((unsigned) 0xffffffffu);

        unsigned short barX = this->field_0x442;
        unsigned short barY = this->field_0x44a;
        if (this->hasShieldBar != 0) {
            int shp = player->getShieldHP();
            int frame = (shp < 2 || this->shieldHitFlash == 0) ? this->shieldFrameImage : this->shieldFrameHitImage;
            canvas->DrawImage2D((unsigned) frame, this->field_0x43c, this->field_0x442);
            canvas->DrawImage2D((unsigned) this->barDividerImage, this->field_0x43e, this->field_0x442);
            canvas->DrawImage2D((unsigned) this->shieldBarBgImage, this->field_0x440, this->field_0x44a);
            int rate = player->getShieldDamageRate();
            int w = (int) ((float) rate * scale);
            canvas->DrawRegion2D((unsigned) this->shieldBarFillImage, 0, 0, w, this->field_0x44c,
                                 (float) w, 0, 0, 0, this->field_0x440);
            barX = this->field_0x444;
            barY = this->field_0x448;
        }

        int ahp = player->getArmorHP();
        int aframe = (ahp < 1) ? this->armorFrameLowImage : this->armorFrameImage;
        canvas->DrawImage2D((unsigned) aframe, this->field_0x43c, barX);
        canvas->DrawImage2D((unsigned) this->barDividerImage, this->field_0x43e, barX);
        canvas->DrawImage2D((unsigned) this->armorBarBgImage, this->field_0x440, barY);
        int hrate = e->getHullDamageRate();
        int hw = (int) ((float) hrate * scale);
        canvas->DrawRegion2D((unsigned) this->armorBarFillImage, 0, 0, hw, this->field_0x44c,
                             (float) hw, 0, 0, 0, this->field_0x440);

        if (this->hasArmorRegen != 0) {
            int arate = player->getArmorDamageRate();
            int aw = (int) ((float) arate * scale);
            canvas->DrawRegion2D((unsigned) this->armorRegenFillImage, 0, 0, aw, this->field_0x44c,
                                 (float) aw, 0, 0, 0, this->field_0x440);
        }
    }

    // --- secondary weapon panel ---
    {
        Level *lvl = *g_Hud_level;
        PlayerEgo *player = (PlayerEgo *) (lvl ? (void *) (long) lvl->getPlayer() : (void *) 0);

        if (player != 0 && player->hasAutoTurret() != 0) {
            bool on = player->autoTurretIsEnabled() != 0 || ((this->autoTurretFlags & 0x20) != 0);
            int img = on ? this->autoTurretOnImage : this->autoTurretOffImage;
            canvas->DrawImage2D((unsigned) img, this->field_0x3fe, 0);
        } else {
            if (this->secondaryLabelTimer > 0) {
                void *font = *g_Hud_font;
                int screenW = *(int *) *g_Hud_screenW;
                unsigned short iconW = this->field_0x3ec;
                canvas->SetColor((unsigned char) 0xff, 0xff, 0xff, 0xff);
                canvas->DrawImage2D((unsigned) this->eventBannerImage, this->field_0x3ec, 0);
                int textW = PaintCanvas::gCanvas->GetTextWidth(
                    (unsigned) (long) canvas, *(String *) (font));
                int tx = this->field_0x3ec + ((screenW - iconW) - textW) / 2;
                PaintCanvas::gCanvas->DrawString((unsigned) (long) canvas,
                                    this->field_0x51c, tx, 0, false);
                canvas->SetColor((unsigned) 0xffffffffu);
                int t = this->secondaryLabelTimer;
                if (t > 4000) t = 0;
                this->secondaryLabelTimer = t;
            }
        }
    }

    drawOrbitInformation();

    // --- mission banner ---
    canvas->SetColor((unsigned) 0xffffffffu);
    canvas->DrawImage2D((unsigned) this->missionBannerImage, this->field_0x438, 0);

    drawEventQueue();

    if (this->quickMenuOpen != 0)
        drawMenu(0);

    if (Status::gStatus != nullptr && Status::gStatus->inSupernovaSystem() != 0)
        drawChallengeModeScore(0);

    drawPauseButton();

    // --- message ---
    if (this->messageActive != 0) {
        canvas->SetColor((unsigned char) 0xff, 0xff, 0xff, 0xff);
        void *font = *g_Hud_font;
        int screenW = *(int *) *g_Hud_screenW;
        int w = PaintCanvas::gCanvas->GetTextWidth((unsigned) (long) canvas, *(String *) (font));
        PaintCanvas::gCanvas->DrawString((unsigned) (long) canvas,
                            this->field_0x51c, screenW / 2 - w / 2, this->field_0x3e2, false);
        canvas->SetColor((unsigned) 0xffffffffu);
    }
}

void Hud::updateQueue(int dt) {
    int t = this->eventQueueTimer + dt;
    this->eventQueueTimer = t;
    int v;
    if (t >= 0xfa1) {
        this->eventQueueTimer = 0;
        Array<ListItem *> *q = this->eventQueue;
        delete (*q)[0];
        (*q)[0] = 0;
        unsigned int i = 0;
        while (true) {
            if (q->size() <= i + 1)
                break;
            (*q)[i] = (*q)[i + 1];
            i = i + 1;
        }
        if ((*q)[1] == 0) {
            this->eventQueueDirty = 0;
        }
        v = 0;
    } else {
        if (t < 0x7d1)
            return;
        if (this->eventQueuePaused != 0)
            return;
        v = 1;
    }
    this->eventQueuePaused = v;
}


static void **g_Hud_oiLayout = nullptr;

static void **g_Hud_oiStatus = nullptr;

static void **g_Hud_oiFont = nullptr;

static GameText **g_Hud_oiGameText = nullptr;
static const char g_Hud_oiSep[1] = {0};
static const unsigned char g_Hud_secColors[4 * 0xc] = {0};

void Hud::drawOrbitInformation() {
    if (Status::gStatus->inAlienOrbit() != 0) return;

    int *layout = (int *) *g_Hud_oiLayout;
    PaintCanvas::gCanvas->SetColor((unsigned) (-1));
    int x = PaintCanvas::gCanvas->GetImage2DWidth((unsigned) (0)) + layout[0x87];

    if (((SolarSystem *) (((void *) (long) Status::gStatus->getSystem())))->hasNoOwner() == 0)
        PaintCanvas::gCanvas->DrawImage2D((unsigned) this->factionLogoImage, 3, 0);

    void *font = *g_Hud_oiFont;

    {
        char name[12];
        ((Station *) (name))->getName();
        PaintCanvas::gCanvas->DrawString((unsigned) (long) (font), *(String *) (name), (x), (char) layout[0x88], false);
        { String *_s = ((String *) (name)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    }
    PaintCanvas::gCanvas->SetColor((unsigned) (-1));

    if (Status::gStatus->getCurrentCampaignMission() <= 0xf) return;

    void *sys = ((void *) (long) Status::gStatus->getSystem());
    int sec = ((SolarSystem *) (sys))->getSecurityLevel();
    int idx = ((SolarSystem *) (((void *) (long) Status::gStatus->getSystem())))->getIndex();
    int *status = (int *) *g_Hud_oiStatus;
    if (idx == 0x1a && status[0x45] > 1) sec = 3;

    {
        char sysName[12], copy[12], sep[12], acc[12], full[12];
        ((SolarSystem *) (sysName))->getName();
        ((String *) (copy))->Set(((String *) (sysName))->data);
        ((String *) (sep))->ctor_char(g_Hud_oiSep, false);
        *(String *) acc = *(String *) copy + *(String *) sep;
        void *txt = (*g_Hud_oiGameText)->getText(0);
        *(String *) full = *(String *) acc + *(String *) txt;
        PaintCanvas::gCanvas->DrawString((unsigned) (long) (font), *(String *) (full), (x), (char) layout[0x89], false);
        { String *_s = ((String *) (full)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (acc)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (sep)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (copy)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (sysName)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    }

    const unsigned char *row = g_Hud_secColors + sec * 0xc;
    PaintCanvas::gCanvas->SetColor((unsigned char) (row[0]), (unsigned char) (row[4]), (unsigned char) (row[8]),
                      (unsigned char) (0xff));
    void *secTxt = (*g_Hud_oiGameText)->getText(sec);
    PaintCanvas::gCanvas->DrawString((unsigned) (long) (font), *(String *) (secTxt), (x), (char) layout[0x8a], false);
}

unsigned int Hud::touchMove(unsigned int a, unsigned int b, void *key) {
    unsigned int i = 0;
    for (; i <= 0x18; i = i + 1) {
        if ((*this->keyArray)[i] == key && this->elementBits[i] == 0x20)
            goto found;
    }

    return touchBegin(a, (unsigned int) -1, key);
found:
    int dx = (int) a - (int) this->reticleX;
    int dy = (int) b - (int) this->reticleY;
    float f = (float) (dy * dy + dx * dx);
    float r = Globals::gGlobals->sqrt(f);
    int denom = this->analogStickRadius;
    int len = (int) r;
    if (denom < len) {
        short s = (short) (denom * dx / len);
        short base = this->reticleY;
        this->lockBracketX = s + this->reticleX;
        s = (short) (denom * dy / len);
        this->lockBracketY = s + base;
    } else {
        this->lockBracketX = (short) a;
        this->lockBracketY = (short) b;
    }
    return 0x20;
}


static void **g_Hud_teCinematic = nullptr;

static void **g_Hud_teScreenW = nullptr;

static void **g_Hud_teScreenH = nullptr;

static inline bool span(unsigned short o, int w, unsigned int v) {
    return o <= v && v <= (unsigned int) o + w;
}

static inline bool cspan(unsigned short o, int w, unsigned int v) {
    return (unsigned int) o - w <= v && v <= (unsigned int) o + w;
}

unsigned int Hud::touchedElement(unsigned int x, unsigned int y) {
    Array<TouchButton *> *menu = this->menuButtons;
    if (this->quickMenuOpen != 0 && menu != 0) {
        for (unsigned int i = 0; i < menu->size(); i++) {
            if ((*menu)[i]->OnTouchBegin((int) x, (int) y) != 0)
                return *(unsigned int *) (*this->menuButtons)[i];
            menu = this->menuButtons;
        }
        return 0;
    }

    int w = this->touchHalfExtent;
    int w2 = this->touchHalfExtentSmall;

    bool cinematic = *(char *) *g_Hud_teCinematic != 0;

    if (cinematic) {
        if (span(this->field_0x40a, w, x) && span(this->field_0x40c, w, y)) return 1;
        if (this->hasBoostButton != 0 && span(this->field_0x410, w, x) && span(this->field_0x412, w, y)) return 2;
        if (span(this->field_0x3f8, w, x) && span(this->field_0x3fa, w, y)) return 0x40;
        if (span(this->field_0x404, w, x) && span(this->field_0x406, w, y)) return 0x100;
        if (cspan(this->reticleX, w, x) && cspan(this->reticleY, w2, y)) return 0x20;
        if (span(this->field_0x3f2, w, x) && span(this->field_0x3f4, w, y)) {
            this->field_0x470 = 1000;
            return 0x80;
        }
        if (cspan(this->field_0x3ec, w, x) && span(this->field_0x3ee, w, y)) return 8;
        if (cspan(this->field_0x3e4, w2 >> 1, x) && cspan(this->field_0x3e6, w2 >> 1, y)) return 0x10;
        if (this->quickMenuEmpty == 0 && span(this->field_0x416, this->field_0x41a, x) &&
            span(this->field_0x418, this->field_0x41c, y))
            return 4;
        if (span(this->field_0x3fe, w, x) && span(this->field_0x400, w, y)) return 0x20000000;
        if (this->hackingGameActive != 0) {
            if (span(this->field_0x454, w, x) && span(this->field_0x456, w, y)) return 0x200;
            if (span(this->field_0x458, w, x) && span(this->field_0x45a, w, y)) return 0x400;
            if (span(this->field_0x45e, w, x) && span(this->field_0x460, w, y)) return 0x800;
        }
        return 0;
    }

    if (this->hackingGameActive != 0) {
        if (span(this->field_0x454, w, x) && span(this->field_0x456, w, y)) return 0x200;
        if (span(this->field_0x458, w, x) && span(this->field_0x45a, w, y)) return 0x400;
        if (span(this->field_0x45e, w, x) && span(this->field_0x460, w, y)) return 0x800;
    }

    int screenW = *(int *) *g_Hud_teScreenW;
    int screenH = *(int *) *g_Hud_teScreenH;

    if (y < (unsigned int) (screenH >> 2)) {
        if (span(this->field_0x40a, w, x) && span(this->field_0x40c, w, y)) return 1;
    } else if (x < (unsigned int) (screenW >> 1)) {
        if (this->hasBoostButton != 0 && cspan(this->field_0x410, w, x) && span(this->field_0x412, w, y)) return 2;
        if (span(this->field_0x3f8, w, x) && span(this->field_0x3fa, w, y)) return 0x40;
        if (span(this->field_0x404, w, x) && span(this->field_0x406, w, y)) return 0x100;
        if (cspan(this->reticleX, w, x) && cspan(this->reticleY, w2, y)) return 0x20;
    } else {
        if (span(this->field_0x3f2, w, x) && span(this->field_0x3f4, w, y)) {
            this->field_0x470 = 1000;
            return 0x80;
        }
        if (cspan(this->field_0x3ec, w, x) && span(this->field_0x3ee, w, y)) return 8;
        if (span(this->field_0x3e4, w2, x) && span(this->field_0x3e6, w2, y)) return 0x10;
        if (this->quickMenuEmpty == 0 && span(this->field_0x416, this->field_0x41a, x) &&
            span(this->field_0x418, this->field_0x41c, y))
            return 4;
        if (span(this->field_0x3fe, w, x) && span(this->field_0x400, w, y)) return 0x20000000;
    }
    return 0;
}

Hud::Hud() {
    init();
}


static GameText **g_Hud_ccGameText = nullptr;

static void **g_Hud_ccTemplate = nullptr;
static const char g_Hud_ccHashX[1] = {0};
static const char g_Hud_ccHashN[1] = {0};
static const char g_Hud_ccUnit[1] = {0};
static const char g_Hud_ccUnit2[1] = {0};

void Hud::catchCargo(int itemId, int count, bool single, bool missionDelivery, bool extender,
                     bool slotMode, bool aggregate) {
    this->field_0x1d0 = 0;
    this->cargoFullFlag = single ? 1 : 0;

    if (missionDelivery) {
        GameText *gt = *g_Hud_ccGameText;
        void *base = gt->getText(0x219);
        void *dst = &this->field_0x1f4;
        *((String *) (dst)) = *((String *) (base));

        void *tmpl = *g_Hud_ccTemplate;
        char a40[12];
        ((String *) (a40))->Set(((String *) (dst))->data);
        int type = Status::gStatus->getMission()->getType();
        void *typeTxt = gt->getText(type == 3 ? 0x56e : 0x56f);
        char a4c[12];
        ((String *) (a4c))->Set(((String *) (typeTxt))->data);
        char a58[12];
        ((String *) (a58))->ctor_char(g_Hud_ccHashX, false);
        char out1[12];
        Status_replaceHash(out1, tmpl, a40, a4c, a58);
        *((String *) (dst)) = *((String *) (out1));
        { String *_s = ((String *) (out1)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a58)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a4c)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a40)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

        tmpl = *g_Hud_ccTemplate;
        char a64[12];
        ((String *) (a64))->Set(((String *) (dst))->data);
        char a70[12];
        ((String *) (a70))->Set((long long) (1));
        char a7c[12];
        ((String *) (a7c))->ctor_char(g_Hud_ccHashN, false);
        char out2[12];
        Status_replaceHash(out2, tmpl, a64, a70, a7c);
        *((String *) (dst)) = *((String *) (out2));
        { String *_s = ((String *) (out2)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a7c)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a70)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a64)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

        String *str = new String(*(String *) dst);
        ListItem *item = new ListItem(str);
        item->field_0x2c = itemId;
        addToEventQueue(item);
        return;
    }

    if (single) {
        GameText *gt = *g_Hud_ccGameText;
        void *txt = gt->getText(0x142);
        *((String *) (&this->field_0x1f4)) = *((String *) (txt));
        String *str = new String(this->field_0x1f4);
        ListItem *item = new ListItem(str, 1);
        addToEventQueue(item);
        return;
    }

    if (count < 1) return;

    GameText *gt = *g_Hud_ccGameText;

    if (aggregate && this->eventQueueDirty != 0) {
        char a0[12];
        ((String *) (a0))->Set((long long) (this->cargoAggregateCount));
        char ac[12];
        ((String *) (ac))->ctor_char(g_Hud_ccUnit, false);
        char a94[12];
        *(String *) a94 = *(String *) a0 + *(String *) ac;
        char a88[12];
        ((String *) (a88))->Set(((String *) (a94))->data);
        void *unit = gt->getText(itemId + 0x4fa);
        char k34[12];
        *(String *) k34 = *(String *) a88 + *(String *) unit;
        { String *_s = ((String *) (a88)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a94)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (ac)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        { String *_s = ((String *) (a0)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

        char b8[12];
        ((String *) (b8))->Set(((String *) (k34))->data);
        int idx = sameHudEventAsBeforeAggregate(*(String *) b8);
        { String *_s = ((String *) (b8)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        if (idx >= 0) {
            this->eventQueueTimer = 2000;
            this->cargoAggregateCount += count;
            char nAc[12];
            ((String *) (nAc))->Set((long long) (this->cargoAggregateCount));
            char nC4[12];
            ((String *) (nC4))->ctor_char(g_Hud_ccUnit2, false);
            char nA0[12];
            *(String *) nA0 = *(String *) nAc + *(String *) nC4;
            char n94[12];
            ((String *) (n94))->Set(((String *) (nA0))->data);
            void *u2 = gt->getText(itemId + 0x4fa);
            char n88[12];
            *(String *) n88 = *(String *) n94 + *(String *) u2;
            *((String *) ((*this->eventQueue)[idx]->name)) = *((String *) (n88));
            { String *_s = ((String *) (n88)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            { String *_s = ((String *) (n94)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            { String *_s = ((String *) (nA0)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            { String *_s = ((String *) (nC4)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            { String *_s = ((String *) (nAc)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            { String *_s = ((String *) (k34)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            return;
        }
        { String *_s = ((String *) (k34)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    }

    this->cargoAggregateCount = count;
    char a0[12];
    ((String *) (a0))->Set((long long) (this->cargoAggregateCount));
    char ac[12];
    ((String *) (ac))->ctor_char(g_Hud_ccUnit, false);
    char a94[12];
    *(String *) a94 = *(String *) a0 + *(String *) ac;
    char a88[12];
    ((String *) (a88))->Set(((String *) (a94))->data);
    void *unit = gt->getText(itemId + 0x4fa);
    char k34[12];
    *(String *) k34 = *(String *) a88 + *(String *) unit;
    *((String *) (&this->field_0x1f4)) = *((String *) (k34));
    { String *_s = ((String *) (k34)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (a88)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (a94)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (ac)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (a0)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

    String *str = new String(this->field_0x1f4);
    ListItem *item = new ListItem(str);
    item->field_0x2c = itemId;
    if (!slotMode || extender) item->field_0x30 = 2;
    if (slotMode) item->field_0x24 = 1;
    addToEventQueue(item);
}


void Hud::drawEventString(String text, bool rightAlign) {
    void *font = *g_Hud_font;
    void *canvas = *g_Hud_canvas2;
    int x;
    if (this->letterbox == 0) {
        int base = this->eventLineMargin;
        int yBase = this->eventLineX;
        if (rightAlign == 0) {
            int w = PaintCanvas::gCanvas->GetTextWidth((unsigned) (long) (canvas), *(String *) (font));
            x = (base + 3) - w;
        } else {
            x = -3 - base;
        }
        x = x + yBase;
    } else {
        if (rightAlign == 0) {
            int margin = this->eventLineMarginAlt;
            int screenW = *(int *) *g_Hud_screenW;
            int w = PaintCanvas::gCanvas->GetTextWidth((unsigned) (long) (canvas), *(String *) (font));
            x = ((screenW - 1) - margin) - w;
        } else {
            x = this->eventLineMarginAlt + 1;
        }
    }
    char y = (char) (this->eventLineY - 1);
    PaintCanvas::gCanvas->DrawString((unsigned) (long) (font), text, (x), (y), false);
}

void Hud::setCurrentSecondaryWeapon(Item *item) {
    this->currentSecondaryWeapon = item;

    updateSecondaryWeaponString();

    Ship *ship = Status::gStatus->getShip();
    Array<Item *> *equip = ship->getEquipment(1);
    this->equipmentArray = equip;

    bool hasSecondary = false;
    if (equip != 0) {
        for (unsigned int i = 0; i < equip->size(); i++) {
            if ((*equip)[i] != 0) {
                hasSecondary = true;
                break;
            }
        }
    }
    unsigned char empty;
    if (hasSecondary) {
        empty = 0;
    } else if (ship->hasJumpDrive() == 0 && Status::gStatus->getWingmen() == 0) {
        empty = (unsigned char) (ship->hasCloak() == 0);
    } else {
        empty = 0;
    }
    this->quickMenuEmpty = empty;
}

int Hud::sameHudEventAsBeforeAggregate(String str) {
    Array<ListItem *> *q = this->eventQueue;
    int i = (int) q->size();
    ListItem *e;
    do {
        i = i + -1;
        if (i < 1)
            return -1;
        e = (*q)[i];
    } while (e == 0 || ((String *) e->name)->Compare_str(&str) != 0);
    return i;
}


static GameText **g_Hud_gameText = nullptr;

static void **g_Hud_swCanvas = nullptr;

static void **g_Hud_swFont = nullptr;

static void **g_Hud_swScreenW = nullptr;
static const char g_Hud_swSep[1] = {0};
static const char g_Hud_swEnd[1] = {0};

void Hud::updateSecondaryWeaponString() {
    void *item = this->currentSecondaryWeapon;
    if (item == 0) return;

    GameText *gt = *g_Hud_gameText;
    int idx = ((Item *) (item))->getIndex();
    void *name = gt->getText(idx + 0x4fa);

    char sep[12], acc1[12], amount[12], acc2[12], end[12], acc3[12];
    ((String *) (sep))->ctor_char(g_Hud_swSep, false);
    *(String *) acc1 = *(String *) name + *(String *) sep;
    int amt = ((Item *) (item))->getAmount();
    ((String *) (amount))->Set((long long) (amt));
    *(String *) acc2 = *(String *) acc1 + *(String *) amount;
    ((String *) (end))->ctor_char(g_Hud_swEnd, false);
    *(String *) acc3 = *(String *) acc2 + *(String *) end;

    *((String *) (&this->field_0x3b4)) = *((String *) (acc3));
    { String *_s = ((String *) (acc3)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (end)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (acc2)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (amount)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (acc1)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (sep)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

    int screenW = *(int *) *g_Hud_swScreenW;
    int w = PaintCanvas::gCanvas->GetTextWidth((unsigned) (long) (*g_Hud_swCanvas), *(String *) (*g_Hud_swFont));
    this->secondaryLabelX = (screenW >> 1) - (w >> 1);
}


static void **g_Hud_eqLetterbox = nullptr;

static void **g_Hud_eqSelf = nullptr;

static void **g_Hud_eqScreenW = nullptr;

static void **g_Hud_eqFont = nullptr;

void Hud::drawEventQueue() {
    char letterbox = *(char *) *g_Hud_eqLetterbox;
    char cinematicY = (letterbox == 0) ? 0 : (char) this->field_0x3e2;

    HudEventDisplay *src = (HudEventDisplay *) *g_Hud_eqSelf;
    int dispBase = src->eventBannerDisplayBase;
    float dispScale = src->eventBannerDisplayScale;

    PaintCanvas::gCanvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff), (unsigned char) (0xff), (unsigned char) (0));
    float mul = (letterbox == 0) ? -2.0f : -1.0f;
    int yOff = (int) (mul * dispScale);

    PaintCanvas::gCanvas->DrawImage2D((unsigned) this->eventBannerImage, this->field_0x3e0, 0);

    ListItem *item = (*this->eventQueue)[1];
    if (item != 0) {
        int kind = item->buttonKind;
        int b2, b3, b4;
        if (kind == 2) {
            b2 = 0;
            b3 = 0xed;
            b4 = 0;
        } else if (kind == 1) {
            b2 = 0xff;
            b3 = 0x2a;
            b4 = 0;
        } else if (kind == 3) {
            b2 = 0xff;
            b3 = 0x80;
            b4 = 0;
        } else {
            b2 = 0xff;
            b3 = 0xff;
            b4 = 0xff;
        }
        PaintCanvas::gCanvas->SetColor((unsigned char) (0xff), (unsigned char) (b2), (unsigned char) (b3), (unsigned char) (b4));

        String *label = (String *) item->name;
        void *font = *g_Hud_eqFont;
        int screenW = *(int *) *g_Hud_eqScreenW;
        int w = PaintCanvas::gCanvas->GetTextWidth((unsigned) (long) (PaintCanvas::gCanvas), *(String *) (font));
        char y = (char) ((char) yOff + (char) dispBase + cinematicY);
        PaintCanvas::gCanvas->DrawString((unsigned) (long) (font), *label, ((screenW >> 1) - w / 2), (y), false);
    }

    PaintCanvas::gCanvas->SetColor(0xffffffffu);
}

unsigned int Hud::touchBegin(unsigned int a, unsigned int b, void *key) {
    unsigned int e = touchedElement(a, b);
    if (e == 0) {
        for (int i = 0; i != 0x19; i = i + 1) {
            if ((*this->keyArray)[i] == key) {
                this->touchFlags = this->touchFlags & ~(unsigned int) this->elementBits[i];
                this->elementBits[i] = 0;
                (*this->keyArray)[i] = 0;
            }
        }
    } else {
        unsigned int j;
        for (j = 0; j < 0x19; j = j + 1) {
            if ((*this->keyArray)[j] == key) {
                unsigned int v = (unsigned int) this->elementBits[j];
                if (e == v)
                    v = this->touchFlags;
                else
                    v = this->touchFlags & ~v;
                this->touchFlags = v | e;
                this->elementBits[j] = e;
                goto done;
            }
        }
        for (j = 0; j < 0x19; j = j + 1) {
            if ((*this->keyArray)[j] == 0) {
                (*this->keyArray)[j] = key;
                this->elementBits[j] = e;
                this->touchFlags = e | this->touchFlags;
                break;
            }
        }
    }
done:
    return this->touchFlags;
}

unsigned int Hud::sameHudEventAsBefore(String str) {
    Array<ListItem *> *q = this->eventQueue;
    int i = (int) q->size();
    while (--i >= 1) {
        ListItem *e = (*q)[i];
        if (e != 0 && ((String *) e->name)->Compare_str(&str) == 0)
            return 1;
    }
    return 0;
}


static void **g_Hud_initLayout = nullptr;

static void **g_Hud_initBound = nullptr;

static void **g_Hud_initOutX = nullptr;

static void **g_Hud_initOutY = nullptr;

static const unsigned short g_Hud_raceBadge[16] = {0};
static const char g_Hud_initMsg[1] = {0};

int Hud::init() {
    Hud_loadImages(this);

    this->messageActive = 0;
    this->hackingGameActive = 0;

    this->keyArray = new Array<void *>();
    ArraySetLength(0x19, *(this->keyArray));
    this->elementBits = new int[0x19];
    for (int i = 0; i != 0x19; i++) {
        (*this->keyArray)[i] = 0;
        this->elementBits[i] = 0;
    }
    this->touchFlags = 0;

    if (Status::gStatus->inAlienOrbit() == 0) {
        int race = ((SolarSystem *) (((void *) (long) Status::gStatus->getSystem())))->getRace();
        Image2DCreate(PaintCanvas::gCanvas, g_Hud_raceBadge[race], &this->factionLogoImage);
    }

    this->secondaryLabelTimerSeed = -1;
    this->secondaryLabelTimer = 0;
    {
        char tmp[12];
        ((String *) (tmp))->ctor_char(g_Hud_initMsg, false);
        *((String *) (&this->field_0x51c)) = *((String *) (tmp));
        { String *_s = ((String *) (tmp)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    }

    closeHudMenu();
    checkIfQuickMenuIsEmpty();
    releaseAllKeys();
    this->uintArray = 0;

    int *layout = (int *) *g_Hud_initLayout;
    int w = PaintCanvas::gCanvas->GetImage2DWidth((unsigned) (0));
    int bound = *(int *) *g_Hud_initBound;
    *(int *) *g_Hud_initOutX = (bound - w) - layout[0x65];
    *(int *) *g_Hud_initOutY = layout[0x66];
    return 0;
}

void Hud::drawPauseButton() {
    PaintCanvas::gCanvas->SetColor((unsigned) (-1));
    unsigned char flag = this->touchFlags;
    int y = this->field_0x40c;
    int x = this->field_0x40a;
    int img = (flag & 1) == 0 ? this->pauseButtonImage : this->pauseButtonPressedImage;
    return PaintCanvas::gCanvas->DrawImage2D((unsigned) (img), (x), (y));
}

Hud *Hud::checkIfQuickMenuIsEmpty() {
    Ship *ship = Status::gStatus->getShip();
    Array<Item *> *equip = ship->getEquipment(1);
    this->equipmentArray = equip;

    unsigned char empty;
    bool hasSecondary = false;
    if (equip != 0) {
        for (unsigned int i = 0; i < equip->size(); i++) {
            if ((*equip)[i] != 0) {
                hasSecondary = true;
                break;
            }
        }
    }
    if (hasSecondary) {
        empty = 0;
    } else if (ship->hasJumpDrive() == 0 && Status::gStatus->getWingmen() == 0) {
        empty = (unsigned char) ship->hasCloak();
    } else {
        empty = 0;
    }
    this->quickMenuEmpty = empty;

    updateSecondaryWeaponString();
    {
        Ship *ship2 = Status::gStatus->getShip();
        Array<Item *> *equip2 = ship2->getEquipment(1);
        this->equipmentArray = equip2;

        bool hasSecondary2 = false;
        if (equip2 != 0) {
            for (unsigned int i = 0; i < equip2->size(); i++) {
                if ((*equip2)[i] != 0) {
                    hasSecondary2 = true;
                    break;
                }
            }
        }
        unsigned char empty2;
        if (hasSecondary2) {
            empty2 = 0;
        } else if (ship2->hasJumpDrive() == 0 && Status::gStatus->getWingmen() == 0) {
            empty2 = (unsigned char) (ship2->hasCloak() == 0);
        } else {
            empty2 = 0;
        }
        this->quickMenuEmpty = empty2;
    }
    return this;
}


static void **g_Hud_dmLayout = nullptr;

static void **g_Hud_dmFont = nullptr;
static const char g_Hud_dmPrefix[1] = {0};

void Hud::drawMenu(int unused) {
    (void) unused;
    int *layout = (int *) *g_Hud_dmLayout;
    ((Layout *) (layout))->drawMask();

    PaintCanvas::gCanvas->DrawImage2D((unsigned) this->quickMenuTopImage, this->field_0x3c4 + this->menuOriginX, 0);

    int hx = this->menuOriginX + this->field_0x3d4 + this->field_0x3dc / 2;
    char hy = (char) ((char) this->menuOriginYBase + (char) this->menuOriginY + (char) (this->menuRowHeight / 2)
                      - (char) layout[0x8b]);
    PaintCanvas::gCanvas->DrawImage2D((unsigned) this->quickMenuHeaderImage, hx, hy, (unsigned char) 0x11);

    int y = this->menuOriginY + this->menuOriginYBase + this->menuRowHeight;

    if (this->menuButtons != 0 && this->menuButtons->size() != 0) {
        unsigned int count = (unsigned int) this->menuButtons->size();
        for (unsigned int i = 0; i < count - 1; i++) {
            PaintCanvas::gCanvas->DrawImage2D((unsigned) this->quickMenuMiddleImage, this->field_0x3c4 + this->menuOriginX, 0);
            y += this->field_0x3d0;
            count = (unsigned int) this->menuButtons->size();
        }
    }

    PaintCanvas::gCanvas->DrawImage2D((unsigned) this->quickMenuBottomImage, this->field_0x3c4 + this->menuOriginX, 0);

    if (this->menuButtons != 0 && this->menuButtons->size() != 0) {
        unsigned int n = (unsigned int) this->menuButtons->size();
        for (unsigned int i = 0; i < n; i++) {
            (*this->menuButtons)[i]->draw();
            n = (unsigned int) this->menuButtons->size();
        }
    }

    if (this->menuLevel != 0) return;

    Ship *ship = Status::gStatus->getShip();
    if (!ship->hasCloak() && ship->hasJumpDrive() == 0) return;

    char prefix[12], num[12], label[12];
    ((String *) (prefix))->ctor_char(g_Hud_dmPrefix, false);
    ((String *) (num))->Set((long long) (this->fuelGaugeValue));
    *(String *) label = *(String *) prefix + *(String *) num;
    { String *_s = ((String *) (num)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (prefix)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

    int gx = this->menuOriginX + this->field_0x3d4 + this->field_0x3dc / 2;
    unsigned char gy = (unsigned char) ((char) y + (char) (layout[0xc] / 2)
                                        + (char) layout[0xa2]);
    PaintCanvas::gCanvas->DrawImage2D((unsigned) this->fuelGaugeBarImage, gx, gy, (unsigned char) 0x11);
    PaintCanvas::gCanvas->DrawImage2D((unsigned) this->fuelGaugeIconImage, gx - layout[0x8c],
                         (char) layout[0xc] + (char) gy + (char) layout[0xa3], (unsigned char) 0x11);

    int barW = layout[0x8c];
    void *font = *g_Hud_dmFont;
    int ih = PaintCanvas::gCanvas->GetImage2DHeight((unsigned) (0));
    int th = PaintCanvas::gCanvas->GetTextHeight(0);
    char ty = (char) (((gy + (char) (ih / 2)) - (char) (th / 2)) + (char) layout[0x8d]);
    PaintCanvas::gCanvas->DrawString((unsigned) (long) (font), *(String *) (label), (barW + gx), (ty), false);
    { String *_s = ((String *) (label)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
}

void Hud::clearQueue() {
    unsigned int i = 1;
    while (i < this->eventQueue->size()) {
        delete (*this->eventQueue)[i];
        (*this->eventQueue)[i] = 0;
        i = i + 1;
    }
    this->eventQueuePaused = 0;
}

void Hud::hudEvent(int eventId, PlayerEgo *ego, int arg) {
    (void) ego;

    switch (eventId) {
        case 1:
        case 2:
            if (this->hasAutofireUI == 0) return;
            break;
        case 3:
            if (this->hasBoostButton == 0 || ((PlayerEgo *) ((void *) (long) arg))->readyToBoost() == 0) return;
            break;
        case 4:
            if (this->hasBoostButton == 0) return;
            break;

        case 0x23:
            this->field_0x468 = 0;
            this->field_0x27a = 1;
            this->weaponSelectState = 1;
            return;
        case 0x25:
            this->field_0x468 = 0;
            this->field_0x27a = 1;
            this->weaponSelectState = 0x101;
            return;
        case 0x27:
            this->field_0x468 = 0;
            this->field_0x27a = 0;
            this->weaponSelectState = 1;
            return;
        case 0x29:
            this->field_0x468 = 0;
            this->field_0x27a = 0;
            this->weaponSelectState = 0x101;
            return;
        case 0x24:
        case 0x26:
        case 0x28:
        case 0x2a:
            *(unsigned char *) &this->weaponSelectState = 0;
            break;

        default:
            break;
    }

    String *line = (String *) &this->field_0x1e0;
    char probe[12];
    ((String *) (probe))->Set((line)->data);
    unsigned int dup = sameHudEventAsBefore(*(String *) probe);
    { String *_s = ((String *) (probe)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    if (dup != 0)
        return;

    String *str = new String(*line);

    unsigned int idBit = (unsigned int) (eventId - 1);
    ListItem *item;
    if (idBit < 0x15 && ((1u << (idBit & 0x1f)) & g_Hud_heImportantMask) != 0)
        item = new ListItem(str, 1);
    else
        item = new ListItem(str, 0);
    addToEventQueue(item);

    void *font = *g_Hud_font;
    int w = PaintCanvas::gCanvas->GetTextWidth((unsigned) (long) (PaintCanvas::gCanvas), *(String *) (font));
    int screenW = *(int *) *g_Hud_screenW;
    this->eventScrollTick = 0;
    this->eventScrolls = 1;
    this->letterbox =
            (unsigned char) ((screenW / 2 - this->eventLineMargin) + this->eventLineMarginAlt * -2 < w);
}


static void **g_Hud_csLayout = nullptr;

static void **g_Hud_csStatus = nullptr;

static void **g_Hud_csScreenW = nullptr;
static const char g_Hud_csZero[1] = {0};

void Hud::drawChallengeModeScore(int unused) {
    (void) unused;
    int *layout = (int *) *g_Hud_csLayout;
    int *status = (int *) *g_Hud_csStatus;
    int screenW = *(int *) *g_Hud_csScreenW;
    void *sprite = this->digitSprite;

    PaintCanvas::gCanvas->SetColor((unsigned) (-1));
    int fw = ((Sprite *) (sprite))->getFrameWidth();
    int pad = layout[0xb];
    int fh = ((Sprite *) (sprite))->getFrameHeight();
    int y = layout[0xb];

    char score[12];
    ((String *) (score))->Set((long long) (status[0x61]));
    int slen = (int) ((String *) score)->size();
    if (slen < 7) {
        for (int k = 0; k < 7 - slen; k++) {
            char z[12], acc[12];
            ((String *) (z))->ctor_char(g_Hud_csZero, false);
            *(String *) acc = *(String *) z + *(String *) score;
            *((String *) (score)) = *((String *) (acc));
            { String *_s = ((String *) (acc)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            { String *_s = ((String *) (z)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
        }
    }

    PaintCanvas::gCanvas->SetColor((unsigned) (-1));
    int dw = fw - pad;
    int half = screenW / 2;
    int span = (dw * 7) / 2;
    int startX = half - span;
    {
        int len = (int) ((String *) score)->size();
        int x = startX;
        for (int i = 1; (unsigned int) (i - 1) < (unsigned int) len; i++) {
            char ch[12];
            *((String *) (ch)) = ((String *) score)->SubString(i - 1, i);
            int frame = ((String *) (ch))->ValueOf();
            { String *_s = ((String *) (ch)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            ((Sprite *) (sprite))->setFrame(frame);
            ((Sprite *) (sprite))->setPosition(x, y);
            ((Sprite *) (sprite))->draw(1.0f, 1.0f);
            x += dw;
        }
    }

    if (status[0x60] > 0 && status[0x63] > 1) {
        PaintCanvas::gCanvas->SetColor((unsigned) (-1));
        int yRow = y + fh + pad;
        int scoreVal = status[0x60];
        if (scoreVal < 0xbb9) {
            if (scoreVal % 100 >= 0x32) {
                int mult = status[0x63];
                float bonus = (float) mult;
                float base = (float) (mult * 1000);
                char bonusStr[12];
                ((String *) (bonusStr))->Set((long long) (int) ((bonus * 0.0f + 1.0f) * base));
                int bl = (int) ((String *) bonusStr)->size();
                int bx = (screenW / 2 - ((bl * dw) >> 1));
                int bonusY = fh + yRow + pad;
                int len = (int) ((String *) bonusStr)->size();
                int x = bx;
                for (int i = 1; (unsigned int) (i - 1) < (unsigned int) len; i++) {
                    char ch[12];
                    *((String *) (ch)) = ((String *) bonusStr)->SubString(i - 1, i);
                    int frame = ((String *) (ch))->ValueOf();
                    { String *_s = ((String *) (ch)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
                    ((Sprite *) (sprite))->setFrame(frame);
                    ((Sprite *) (sprite))->setPosition(x, bonusY);
                    ((Sprite *) (sprite))->draw(1.0f, 1.0f);
                    x += dw;
                }
                { String *_s = ((String *) (bonusStr)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            }
        }
        PaintCanvas::gCanvas->DrawImage2D((unsigned) this->multiplierIconImage, pad + startX, 0);

        char timeStr[12];
        ((String *) (timeStr))->Set((long long) (status[0x63]));
        int tx = (half + pad) - span + PaintCanvas::gCanvas->GetImage2DWidth((unsigned) (0));
        int len = (int) ((String *) timeStr)->size();
        int x = tx;
        for (int i = 1; (unsigned int) (i - 1) < (unsigned int) len; i++) {
            char ch[12];
            *((String *) (ch)) = ((String *) timeStr)->SubString(i - 1, i);
            int frame = ((String *) (ch))->ValueOf();
            { String *_s = ((String *) (ch)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
            ((Sprite *) (sprite))->setFrame(frame);
            ((Sprite *) (sprite))->setPosition(x, yRow);
            ((Sprite *) (sprite))->draw(1.0f, 1.0f);
            x += dw;
        }
        { String *_s = ((String *) (timeStr)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    }
    PaintCanvas::gCanvas->SetColor((unsigned) (-1));
    { String *_s = ((String *) (score)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
}


static void **g_Hud_meCanvas = nullptr;

static void **g_Hud_meFont = nullptr;

static void **g_Hud_meScreenW = nullptr;
static const char g_Hud_meSep[1] = {0};
static const char g_Hud_meEnd[1] = {0};

void Hud::hudEventMedal(int medalId, int percent) {
    GameText *gt = *g_Hud_gameText;
    void *name = gt->getText(medalId + 0x5e3);

    char sep[12], acc1[12], num[12], acc2[12], end[12], acc3[12];
    ((String *) (sep))->ctor_char(g_Hud_meSep, false);
    *(String *) acc1 = *(String *) name + *(String *) sep;
    if (percent >= 100) percent = 100;
    ((String *) (num))->Set((long long) (percent));
    *(String *) acc2 = *(String *) acc1 + *(String *) num;
    ((String *) (end))->ctor_char(g_Hud_meEnd, false);
    *(String *) acc3 = *(String *) acc2 + *(String *) end;

    void *dst = &this->field_0x1e0;
    *((String *) (dst)) = *((String *) (acc3));
    { String *_s = ((String *) (acc3)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (end)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (acc2)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (num)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (acc1)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    { String *_s = ((String *) (sep)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }

    char probe[12];
    ((String *) (probe))->Set(((String *) (dst))->data);
    int same = sameHudEventAsBefore(*(String *) probe);
    { String *_s = ((String *) (probe)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
    if (same != 0) return;

    String *str = new String(*(String *) dst);
    ListItem *item = new ListItem(str, 3);
    addToEventQueue(item);

    int w = PaintCanvas::gCanvas->GetTextWidth((unsigned) (long) (*g_Hud_meCanvas), *(String *) (*g_Hud_meFont));
    int screenW = *(int *) *g_Hud_meScreenW;
    this->eventScrollTick = 0;
    this->eventScrolls = 1;
    this->letterbox = ((screenW / 2 - this->eventLineMargin) + this->eventLineMarginAlt * -2 < w) ? 1 : 0;
}


static void **g_Hud_imLayout = nullptr;

static void **g_Hud_imLetterbox = nullptr;

static void **g_Hud_imCargoA = nullptr;

static void **g_Hud_imCargoB = nullptr;

static void **g_Hud_imFlagA = nullptr;

static void **g_Hud_imFlagB = nullptr;

void Hud::initHudMenu(int menuType, Level *lvl) {
    if (this->menuButtons != 0) {
        ArrayReleaseClasses(*this->menuButtons); ArrayRemoveAll(*(this->menuButtons));
        delete this->menuButtons;
        this->menuButtons = 0;
    }
    this->menuLevel = lvl;
    this->menuButtons = new Array<TouchButton *>();

    delete this->equipmentArray;
    this->equipmentArray = Status::gStatus->getShip()->getEquipment(1);
    updateSecondaryWeaponString();

    this->menuOriginX = 0;
    int *layout = (int *) *g_Hud_imLayout;
    int rowH = *(int *) (layout[0] + 0x1dc);

    char letterbox = *(char *) *g_Hud_imLetterbox;

    int yOrigin;
    if (letterbox == 0) {
        yOrigin = this->menuBaseY;
    } else {
        CargoBay *cargoA = (CargoBay *) *g_Hud_imCargoA;
        float v;
        if ((long) this->menuLevel == 3)
            v = (float) cargoA->cargoCurrent;
        else {
            v = (float) cargoA->cargoMax;
            float adj = 0.0f;
            if (*(char *) *g_Hud_imFlagA == 0)
                adj = (*(char *) *g_Hud_imFlagB == 0) ? 1.0f : 0.0f;
            v = v - adj;
        }
        float yf = 0.0f;
        if (v >= 0.0f) {
            if ((long) this->menuLevel == 3)
                yf = (float) ((CargoBay *) *g_Hud_imCargoA)->cargoCurrent;
            else {
                float v2 = (float) ((CargoBay *) *g_Hud_imCargoB)->cargoMax;
                float adj = (*(char *) *g_Hud_imFlagA == 0)
                                ? ((*(char *) *g_Hud_imFlagB == 0) ? 1.0f : 0.0f)
                                : 0.0f;
                yf = v2 - adj;
            }
        }
        this->menuOriginY = (int) yf;
        yOrigin = ((this->menuRowHeight + (int) yf) - rowH / 2) + 1;
        this->menuBaseY = yOrigin;
    }
    (void) yOrigin;

    Hud_buildQuickMenu(this, menuType);
    this->quickMenuOpen = 1;
}

Hud::~Hud() {
    delete this->equipmentArray;
    this->equipmentArray = 0;

    delete this->eventQueue;
    this->eventQueue = 0;

    if (this->menuButtons != 0) {
        ArrayReleaseClasses(*this->menuButtons); ArrayRemoveAll(*(this->menuButtons));
        delete this->menuButtons;
    }
    this->menuButtons = 0;

    delete this->uintArray;
    this->uintArray = 0;
}

bool Hud::drawTitleImage(bool visible) {
    return visible;
}

// Static data members present in the original binary (defined for symbol parity).
int Hud::RADAR_WIDTH;
int Hud::RADAR_HEIGHT;
int Hud::wingmanCommand;
