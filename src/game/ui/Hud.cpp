#include "game/ui/Hud.h"
#include "game/mission/Mission.h"
#include "game/mission/Item.h"
#include "engine/render/Sprite.h"
#include "engine/core/GameText.h"
#include "game/core/Globals.h"
#include "game/core/GameSettings.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerFixedObject.h"
#include "game/world/Route.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"
#include "game/world/Waypoint.h"
#include "game/ui/TouchButton.h"
#include "game/ui/ListItem.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include "game/world/Level.h"
#include "game/ship/Player.h"
#include "engine/render/PaintCanvas.h"
#include "game/ui/Layout.h"

#include <cstdint>

void Status_replaceHash(void *out, void *tmpl, void *a, void *b, void *c);

static unsigned int g_Hud_heImportantMask = 0;

struct HudSecurityColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

// Android ARM .rodata: word_203758 (first halfword of each 32-bit slot).
static const unsigned short g_Hud_factionLogoResourceIds[4] = {0x4a6, 0x4a3, 0x4a5, 0x4a4};

// Android ARM .rodata: byte_203780, read with a 12-byte stride by drawOrbitInformation.
static const HudSecurityColor g_Hud_securityColors[4] = {
    {0xff, 0x2a, 0x00},
    {0xff, 0x6c, 0x00},
    {0xed, 0xed, 0x00},
    {0xed, 0x00, 0x00},
};

static inline PaintCanvas *hud_canvas() {
    return static_cast<PaintCanvas *>(Globals::Canvas);
}

static inline unsigned int hud_font() {
    return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font));
}

static inline GameText *hud_game_text() {
    return static_cast<GameText *>(Globals::gameText);
}

static inline int hud_layout_i32(unsigned int offset) {
    return *reinterpret_cast<int *>(static_cast<char *>(Globals::layout) + offset);
}

static inline float hud_layout_f32(unsigned int offset) {
    return *reinterpret_cast<float *>(static_cast<char *>(Globals::layout) + offset);
}

static void hud_create_image(PaintCanvas *canvas, unsigned short resourceId, int &slot) {
    unsigned int image = 0;
    canvas->Image2DCreate(resourceId, image);
    slot = static_cast<int>(image);
}

struct HudImageInit {
    unsigned short resourceId;
    int *slot;
};

static void hud_load_init_images(Hud *self) {
    PaintCanvas *canvas = hud_canvas();
    if (canvas == nullptr) return;

    self->initImageSlots = {};
    const HudImageInit images[] = {
        {0x4ac, &self->shieldFrameImage},
        {0x4ad, &self->shieldFrameHitImage},
        {0x4ae, &self->shieldBarBgImage},
        {0x4af, &self->shieldBarFillImage},
        {0x4aa, &self->armorFrameImage},
        {0x4ab, &self->armorFrameLowImage},
        {0x4a7, &self->armorBarBgImage},
        {0x4a8, &self->armorRegenFillImage},
        {0x524, &self->armorBarFillImage},
        {0x1f59, &self->initImageSlots.image_0x2d0},
        {0x1f5a, &self->initImageSlots.image_0x2cc},
        {0x1f5b, &self->initImageSlots.image_0x2c8},
        {0x4a9, &self->barDividerImage},
        {0x4bb, &self->initImageSlots.image_0x34c},
        {0x4ba, &self->initImageSlots.image_0x350},
        {0x4b5, &self->lockBracketLockedImage},
        {0x4b4, &self->lockBracketImage},
        {0x536, &self->initImageSlots.image_0x2e8},
        {0x4bd, &self->initImageSlots.image_0x2ec},
        {0x4bc, &self->initImageSlots.image_0x2f0},
        {0x4b9, &self->pauseButtonPressedImage},
        {0x4b8, &self->pauseButtonImage},
        {0x4b3, &self->initImageSlots.image_0x2fc},
        {0x4b2, &self->initImageSlots.image_0x300},
        {0x4b1, &self->orbitMarkerActiveImage},
        {0x4b0, &self->orbitMarkerIdleImage},
        {0x4b7, &self->initImageSlots.image_0x304},
        {0x4b6, &self->initImageSlots.image_0x308},
        {0x4c1, &self->initImageSlots.image_0x31c},
        {0x4c5, &self->initImageSlots.image_0x320},
        {0x520, &self->initImageSlots.image_0x324},
        {0x4c3, &self->eventBannerImage},
        {0x4c2, &self->missionBannerImage},
        {0x4cf, &self->quickMenuTopImage},
        {0x4d1, &self->quickMenuMiddleImage},
        {0x4d0, &self->quickMenuBottomImage},
        {0x537, &self->initImageSlots.image_0x370},
        {0x538, &self->initImageSlots.image_0x374},
        {0x539, &self->initImageSlots.image_0x37c},
        {0x53a, &self->initImageSlots.image_0x378},
        {0x53a, &self->initImageSlots.image_0x388},
        {0x1f41, &self->initImageSlots.image_0x38c},
        {0x525, &self->initImageSlots.image_0x360},
        {0x526, &self->initImageSlots.image_0x364},
        {0x52b, &self->initImageSlots.image_0x368},
        {0x52c, &self->initImageSlots.image_0x36c},
        {0x528, &self->initImageSlots.image_0x4f4},
        {0x527, &self->initImageSlots.image_0x504},
        {0x4e9, &self->initImageSlots.image_0x4f8},
        {0x4ea, &self->initImageSlots.image_0x508},
        {0x4be, &self->initImageSlots.image_0x4fc},
        {0x4bf, &self->initImageSlots.image_0x50c},
        {0x52a, &self->initImageSlots.image_0x500},
        {0x529, &self->initImageSlots.image_0x510},
        {0x540, &self->initImageSlots.image_0x390},
        {0x541, &self->initImageSlots.image_0x394},
        {0x53f, &self->initImageSlots.image_0x398},
        {0x542, &self->initImageSlots.image_0x39c},
        {0x543, &self->initImageSlots.image_0x3a0},
        {0x546, &self->initImageSlots.image_0x314},
        {0x547, &self->initImageSlots.image_0x318},
        {0x1f58, &self->initImageSlots.image_0x3a4},
        {0x1f57, &self->initImageSlots.image_0x3a8},
        {0x4b1, &self->initImageSlots.image_0x3ac},
        {0x4b0, &self->initImageSlots.image_0x3b0},
        {0x1f43, &self->initImageSlots.image_0x334},
        {0x1f42, &self->initImageSlots.image_0x344},
        {0x1f40, &self->initImageSlots.image_0x380},
        {0x1f61, &self->initImageSlots.image_0x338},
        {0x1f60, &self->initImageSlots.image_0x33c},
        {0x1f5f, &self->initImageSlots.image_0x384},
        {0x1f5c, &self->initImageSlots.image_0x340},
    };
    for (const HudImageInit &image : images)
        hud_create_image(canvas, image.resourceId, *image.slot);

    if (Globals::iPad != 0) {
        hud_create_image(canvas, 0x4c6, self->initImageSlots.image_0x004);
        hud_create_image(canvas, 0x6aa, self->initImageSlots.image_0x008);
        self->reticleImage = self->initImageSlots.image_0x004;
    } else {
        hud_create_image(canvas, 0x4c6, self->reticleImage);
    }

    // These aliases are consumed by already-recovered draw paths.
    self->autoTurretOnImage = self->initImageSlots.image_0x304;
    self->autoTurretOffImage = self->initImageSlots.image_0x308;
    self->fuelGaugeIconImage = self->initImageSlots.image_0x370;
    self->fuelGaugeBarImage = self->initImageSlots.image_0x374;
}

static int hud_default_steer_anchor() {
    const float scale = *reinterpret_cast<const float *>(Globals::options + 0x48);
    if (scale >= 1.0f) return 830;
    if (scale <= 0.0f) return 415;
    return 583;
}

static int hud_default_fire_anchor() {
    const float scale = *reinterpret_cast<const float *>(Globals::options + 0x48);
    if (scale >= 1.0f) return 730;
    if (scale <= 0.0f) return 365;
    return 513;
}

static void hud_apply_ipad_control_coords(Hud *self, PaintCanvas *canvas) {
    if (Globals::iPad == 0 || canvas == nullptr) return;

    Globals *globals = Globals::gGlobals != nullptr ? Globals::gGlobals : static_cast<Globals *>(Globals::globals);
    if (globals == nullptr) return;

    GameSettings *settings = reinterpret_cast<GameSettings *>(Globals::options);
    const int steerAnchor = settings->steerAnchorX != 0 ? settings->steerAnchorX : hud_default_steer_anchor();
    const int fireAnchor = settings->fireAnchorX != 0 ? settings->fireAnchorX : hud_default_fire_anchor();

    globals->setCoordsSteer(steerAnchor,
                            canvas->GetImage2DWidth(static_cast<unsigned>(self->initImageSlots.image_0x31c)),
                            canvas->GetImage2DWidth(static_cast<unsigned>(self->orbitMarkerIdleImage)),
                            canvas->GetImage2DWidth(static_cast<unsigned>(self->initImageSlots.image_0x300)),
                            self->field_0x3f8, self->field_0x3fa, self->field_0x42c, self->field_0x42e,
                            self->field_0x424, self->field_0x426, self->field_0x410, self->field_0x412,
                            self->field_0x404, self->field_0x406);

    globals->setCoordsFire(fireAnchor,
                           canvas->GetImage2DWidth(static_cast<unsigned>(self->initImageSlots.image_0x004)),
                           static_cast<unsigned>(self->initImageSlots.image_0x004),
                           static_cast<unsigned>(self->initImageSlots.image_0x008),
                           reinterpret_cast<unsigned int &>(self->reticleImage), self->iPadFireCoord_0x0c,
                           self->iPadFireCoord_0x0e, self->field_0x3e4, self->field_0x3e6,
                           self->field_0x416, self->field_0x418, self->field_0x3f2, self->field_0x3f4,
                           self->field_0x3ec, self->field_0x3ee, self->field_0x3fe, self->field_0x400);

    self->field_0x41e = self->field_0x424;
    self->iPadSteerAnchor = steerAnchor;
    self->iPadFireAnchor = fireAnchor;
}

static void hud_init_coordinates(Hud *self) {
    PaintCanvas *canvas = hud_canvas();
    if (canvas == nullptr || Globals::layout == nullptr) return;

    const int screenW = Globals::w;
    const int screenH = Globals::h;
    const auto width = [canvas](int image) { return canvas->GetImage2DWidth(static_cast<unsigned>(image)); };
    const auto height = [canvas](int image) { return canvas->GetImage2DHeight(static_cast<unsigned>(image)); };

    self->field_0x434 = static_cast<unsigned short>(screenW - hud_layout_i32(0x14c));
    self->field_0x436 = static_cast<unsigned short>(screenH - hud_layout_i32(0x12c) -
                                                     canvas->GetTextHeight(hud_font()) - hud_layout_i32(0x150));
    self->field_0x3f0 = static_cast<unsigned short>(width(self->initImageSlots.image_0x2ec));
    self->field_0x3e4 = static_cast<unsigned short>(screenW - hud_layout_i32(0x154) - width(self->lockBracketImage));
    self->field_0x3e6 = static_cast<unsigned short>(screenH - hud_layout_i32(0x158) - width(self->lockBracketImage));
    self->field_0x3ec = static_cast<unsigned short>(screenW - hud_layout_i32(0x15c) - self->field_0x3f0);
    self->field_0x3ee = static_cast<unsigned short>(screenH - hud_layout_i32(0x160) - self->field_0x3f0);
    self->field_0x3ea = static_cast<unsigned short>(hud_layout_i32(0x164));
    self->field_0x3e0 = static_cast<unsigned short>((screenW - width(self->eventBannerImage)) / 2);
    self->field_0x3e2 = static_cast<unsigned short>(hud_layout_i32(0x168));

    self->field_0x3f6 = static_cast<unsigned short>(width(self->initImageSlots.image_0x4f4));
    self->field_0x3f2 = static_cast<unsigned short>(screenW - self->field_0x3f6 - hud_layout_i32(0x16c));
    self->field_0x3f4 = static_cast<unsigned short>(screenH - hud_layout_i32(0x170) -
                                                     height(self->initImageSlots.image_0x4f4));
    self->field_0x41a = static_cast<unsigned short>(width(self->initImageSlots.image_0x34c));
    self->field_0x41c = static_cast<unsigned short>(hud_layout_i32(0x174));
    self->field_0x416 = static_cast<unsigned short>(screenW - hud_layout_i32(0x178) - self->field_0x41a);
    self->field_0x418 = static_cast<unsigned short>(screenH - hud_layout_i32(0x17c) -
                                                     height(self->initImageSlots.image_0x34c));

    self->field_0x3fc = static_cast<unsigned short>(width(self->orbitMarkerIdleImage));
    self->field_0x3f8 = static_cast<unsigned short>(hud_layout_i32(0x180));
    self->field_0x3fa = static_cast<unsigned short>(screenH - hud_layout_i32(0x184) - self->field_0x3fc);
    self->field_0x402 = static_cast<unsigned short>(width(self->orbitMarkerIdleImage));
    self->field_0x3fe = static_cast<unsigned short>(screenW - hud_layout_i32(0x180) - self->field_0x3fc);
    self->field_0x400 = static_cast<unsigned short>(screenH - hud_layout_i32(0x184) - self->field_0x3fc);

    self->field_0x40e = static_cast<unsigned short>(width(self->pauseButtonPressedImage));
    self->field_0x40a = static_cast<unsigned short>(screenW - self->field_0x40e - hud_layout_i32(0x194));
    self->field_0x40c = static_cast<unsigned short>(hud_layout_i32(0x198));
    self->field_0x438 = static_cast<unsigned short>(screenW - width(self->initImageSlots.image_0x320) -
                                                     hud_layout_i32(0x19c));
    self->field_0x43a = static_cast<unsigned short>(hud_layout_i32(0x1a0));

    self->field_0x430 = static_cast<unsigned short>(width(self->initImageSlots.image_0x31c));
    const int hackingHalfWidth = width(self->initImageSlots.image_0x3a4) / 2;
    self->field_0x45c = static_cast<unsigned short>(hackingHalfWidth * 2);
    self->field_0x454 = static_cast<unsigned short>(screenW / 2 - hackingHalfWidth - hud_layout_i32(0x31c));
    self->field_0x458 = static_cast<unsigned short>(screenW / 2 - hackingHalfWidth + hud_layout_i32(0x31c));
    self->field_0x460 = self->field_0x3ee;
    self->field_0x45e = static_cast<unsigned short>(screenW / 2 - hackingHalfWidth);
    self->field_0x456 = static_cast<unsigned short>(screenH / 2 - hud_layout_i32(0x320));
    self->field_0x45a = self->field_0x456;

    self->field_0x42c = static_cast<unsigned short>(hud_layout_i32(0x1a4));
    self->field_0x42e = static_cast<unsigned short>(screenH - hud_layout_i32(0x1a8) -
                                                     height(self->initImageSlots.image_0x31c));
    self->field_0x422 = static_cast<unsigned short>(width(self->initImageSlots.image_0x304));
    self->field_0x424 = static_cast<unsigned short>(self->field_0x42c + self->field_0x430 / 2);
    self->field_0x41e = self->field_0x424;
    self->field_0x426 = static_cast<unsigned short>(self->field_0x42e + self->field_0x430 / 2);
    self->field_0x420 = self->field_0x426;
    self->field_0x414 = static_cast<unsigned short>(width(self->initImageSlots.image_0x2fc));
    self->field_0x410 = static_cast<unsigned short>(hud_layout_i32(0x1ac));
    self->field_0x412 = static_cast<unsigned short>(screenH - hud_layout_i32(0x1b0) - self->field_0x414);
    self->field_0x408 = static_cast<unsigned short>(width(self->initImageSlots.image_0x394));
    self->field_0x404 = static_cast<unsigned short>(hud_layout_i32(0x188));
    self->field_0x406 = static_cast<unsigned short>(screenH - hud_layout_i32(0x18c) - self->field_0x408);
    self->field_0x450 = hud_layout_i32(0x190);

    if (Globals::iPad != 0) {
        self->field_0x3c4 = screenW - hud_layout_i32(0x28) - width(self->quickMenuTopImage);
        self->menuOriginY = self->field_0x418 - hud_layout_i32(0x2c) - 6 * hud_layout_i32(0x30) -
                            height(self->quickMenuTopImage);
        hud_apply_ipad_control_coords(self, canvas);
    } else {
        self->field_0x3c4 = (screenW - width(self->quickMenuTopImage)) / 2;
        self->menuOriginY = hud_layout_i32(0x1b4);
        self->iPadSteerAnchor = 0;
        self->iPadFireAnchor = 0;
    }

    self->menuRowHeight = height(self->quickMenuTopImage);
    self->field_0x3d0 = height(self->quickMenuMiddleImage);
    self->field_0x3d4 = self->field_0x3c4 + hud_layout_i32(0x1b8);
    self->menuOriginYBase = hud_layout_i32(0x1bc) + self->menuOriginY + self->menuRowHeight -
                            hud_layout_i32(0x30) / 2;
    self->field_0x3dc = width(self->quickMenuMiddleImage) - hud_layout_i32(0x1c0);

    self->field_0x43c = static_cast<unsigned short>(hud_layout_i32(0x1c4));
    self->field_0x43e = static_cast<unsigned short>(self->field_0x43c + width(self->shieldFrameImage));
    self->field_0x444 = static_cast<unsigned short>(hud_layout_i32(0x1c8));
    self->field_0x448 = static_cast<unsigned short>(hud_layout_i32(0x1cc));
    self->field_0x442 = static_cast<unsigned short>(hud_layout_i32(0x1d0));
    self->field_0x44a = static_cast<unsigned short>(hud_layout_i32(0x1d4));
    self->field_0x446 = static_cast<unsigned short>(width(self->shieldBarFillImage));
    self->field_0x440 = self->field_0x43e;
    self->field_0x44c = static_cast<unsigned short>(height(self->shieldBarFillImage));
}

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

void Hud::draw(long long t0, long long t1, PlayerEgo *ego, bool letterbox, unsigned int x, unsigned int y) {
    (void) t0;
    (void) t1;
    (void) x;
    (void) y;

    this->letterbox = (unsigned char) letterbox;

    PaintCanvas *canvas = hud_canvas();
    if (canvas == nullptr || ego == nullptr) return;

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
            Level *lvl = ego->level;
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
        Level *lvl = ego->level;
        PlayerEgo *player = lvl != nullptr ? lvl->getPlayer() : nullptr;

        if (player != 0 && player->hasAutoTurret() != 0) {
            bool on = player->autoTurretIsEnabled() != 0 || ((this->autoTurretFlags & 0x20) != 0);
            int img = on ? this->autoTurretOnImage : this->autoTurretOffImage;
            canvas->DrawImage2D((unsigned) img, this->field_0x3fe, 0);
        } else {
            if (this->secondaryLabelTimer > 0) {
                unsigned int font = hud_font();
                int screenW = Globals::w;
                unsigned short iconW = this->field_0x3ec;
                canvas->SetColor((unsigned char) 0xff, 0xff, 0xff, 0xff);
                canvas->DrawImage2D((unsigned) this->eventBannerImage, this->field_0x3ec, 0);
                int textW = canvas->GetTextWidth(font, this->field_0x3b4);
                int tx = this->field_0x3ec + ((screenW - iconW) - textW) / 2;
                canvas->DrawString(font, this->field_0x3b4, tx, 0, false);
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
        unsigned int font = hud_font();
        int screenW = Globals::w;
        int w = canvas->GetTextWidth(font, this->field_0x51c);
        canvas->DrawString(font, this->field_0x51c, screenW / 2 - w / 2, this->field_0x3e2, false);
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


void Hud::drawOrbitInformation() {
    Status *status = Status::gStatus;
    PaintCanvas *canvas = hud_canvas();
    GameText *gameText = hud_game_text();
    if (status == nullptr || canvas == nullptr || gameText == nullptr || Globals::layout == nullptr ||
        status->inAlienOrbit())
        return;

    SolarSystem *system = status->getSystem();
    Station *station = status->getStation();
    if (system == nullptr || station == nullptr) return;

    const unsigned int font = hud_font();
    const int logoWidth = this->factionLogoImage >= 0
                              ? canvas->GetImage2DWidth(static_cast<unsigned int>(this->factionLogoImage))
                              : 0;
    const int x = logoWidth + hud_layout_i32(0x21c);

    canvas->SetColor(0xffffffffu);
    if (this->factionLogoImage >= 0 && system->hasNoOwner() == 0)
        canvas->DrawImage2D(static_cast<unsigned int>(this->factionLogoImage), 3, 3);

    const String stationName = station->getName();
    canvas->DrawString(font, stationName, x, hud_layout_i32(0x220), false);
    canvas->SetColor(0x777777ffu);

    if (status->getCurrentCampaignMission() < 16) return;

    int securityLevel = system->getSecurityLevel();
    if (system->getIndex() == 26 && status->field_114 > 1) securityLevel = 3;

    String systemLine = system->getName();
    systemLine += String(" ");
    systemLine += *gameText->getText(137);
    canvas->DrawString(font, systemLine, x, hud_layout_i32(0x224), false);

    const unsigned int colorIndex = static_cast<unsigned int>(securityLevel);
    if (colorIndex < 4) {
        const HudSecurityColor &color = g_Hud_securityColors[colorIndex];
        canvas->SetColor(color.r, color.g, color.b, 0xff);
    }
    canvas->DrawString(font, *gameText->getText(securityLevel + 402), x, hud_layout_i32(0x228), false);
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

    bool cinematic = Globals::iPad != 0;

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

    int screenW = Globals::w;
    int screenH = Globals::h;

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
    PaintCanvas *canvas = hud_canvas();
    if (canvas == nullptr) return;
    const unsigned int font = hud_font();
    int x;
    if (this->letterbox == 0) {
        int base = this->eventLineMargin;
        int yBase = this->eventLineX;
        if (rightAlign == 0) {
            int w = canvas->GetTextWidth(font, text);
            x = (base + 3) - w;
        } else {
            x = -3 - base;
        }
        x = x + yBase;
    } else {
        if (rightAlign == 0) {
            int margin = this->eventLineMarginAlt;
            int screenW = Globals::w;
            int w = canvas->GetTextWidth(font, text);
            x = ((screenW - 1) - margin) - w;
        } else {
            x = this->eventLineMarginAlt + 1;
        }
    }
    char y = (char) (this->eventLineY - 1);
    canvas->DrawString(font, text, x, y, false);
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


void Hud::updateSecondaryWeaponString() {
    Item *item = this->currentSecondaryWeapon;
    PaintCanvas *canvas = hud_canvas();
    GameText *gameText = hud_game_text();
    if (item == nullptr || canvas == nullptr || gameText == nullptr) return;

    String label = *gameText->getText(item->getIndex() + 1274);
    label += String(" (");
    label += String(item->getAmount());
    label += String(")");
    this->field_0x3b4 = label;

    this->secondaryLabelX = (Globals::w >> 1) - (canvas->GetTextWidth(hud_font(), this->field_0x3b4) >> 1);
}


void Hud::drawEventQueue() {
    PaintCanvas *canvas = hud_canvas();
    Array<ListItem *> *queue = this->eventQueue;
    if (canvas == nullptr || Globals::layout == nullptr || queue == nullptr) return;

    const bool targetVisible = Radar::drawTarget != 0;
    const int targetY = targetVisible ? this->field_0x3e2 : 0;
    const int bannerBaseY = hud_layout_i32(0x1e4);
    const float bannerSlide = hud_layout_f32(0x1e0);
    const int rawAlpha = static_cast<int>((static_cast<float>(this->eventQueueTimer) / 2000.0f) * 255.0f);
    const unsigned char alpha = rawAlpha <= 255 ? static_cast<unsigned char>(rawAlpha)
                                                 : static_cast<unsigned char>(-2 - rawAlpha);

    canvas->SetColor(0xff, 0xff, 0xff, alpha);
    canvas->DrawImage2D(static_cast<unsigned int>(this->eventBannerImage), this->field_0x3e0,
                        targetY - bannerBaseY);

    if (queue->size() > 1 && (*queue)[1] != nullptr) {
        ListItem *item = (*queue)[1];
        switch (item->buttonKind) {
            case 2: canvas->SetColor(0x00, 0xed, 0x00, alpha); break;
            case 1: canvas->SetColor(0xff, 0x2a, 0x00, alpha); break;
            case 3: canvas->SetColor(0xff, 0x80, 0x00, alpha); break;
            default: canvas->SetColor(0xff, 0xff, 0xff, alpha); break;
        }

        String *label = static_cast<String *>(item->name);
        const int textWidth = canvas->GetTextWidth(hud_font(), *label);
        const float direction = targetVisible ? -1.0f : -2.0f;
        const int textY = static_cast<int>(direction * bannerSlide) + bannerBaseY + targetY;
        canvas->DrawString(hud_font(), *label, (Globals::w >> 1) - textWidth / 2, textY, false);
    }

    canvas->SetColor(0xffffffffu);
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


int Hud::init() {
    this->menuButtons = nullptr;
    this->equipmentArray = nullptr;
    this->eventQueue = nullptr;
    this->keyArray = nullptr;
    this->elementBits = nullptr;
    this->uintArray = nullptr;
    this->quickMenuType = 0;
    this->digitSprite = nullptr;
    this->quickMenuHeaderImage = -1;
    this->multiplierIconImage = -1;
    this->factionLogoImage = -1;
    this->reticleImage = -1;
    this->missionBannerImage = -1;
    this->eventBannerImage = -1;
    this->fuelGaugeIconImage = -1;
    this->fuelGaugeBarImage = -1;

    hud_load_init_images(this);
    hud_init_coordinates(this);

    this->visible = 1;
    this->letterbox = 0;
    this->messageActive = 0;
    this->hackingGameActive = 0;
    this->autofireEnabled = 0;
    this->fireForTutorial = 0;
    this->eventQueueDirty = 0;
    this->eventQueueTimer = 0;
    this->eventQueuePaused = 0;
    this->jumpMapSelectedFlag = 0;
    this->field_0x275 = 0;
    this->field_0x276 = 0;
    this->weaponSelectState = 0;
    this->field_0x27a = 0;
    this->field_0x27b = 0;
    this->field_0x280 = 0;
    this->field_0x281 = 0;
    this->quickMenuOpen = 0;
    this->quickMenuEmpty = 0;
    this->autoTurretFlags = 0;
    this->field_0x288 = 0;
    this->field_0x1d0 = 10000;
    this->cargoFullFlag = 0;
    this->shieldHitFlash = 0;
    this->hitFlashTimer = 0;
    this->field_0x470 = 0;
    this->timeExtenderTimer = 0;
    this->timeExtenderDuration = 0;
    this->cargoAggregateCount = 0;
    this->field_0x468 = 0;

    this->keyArray = new Array<void *>();
    ArraySetLength(0x19, *(this->keyArray));
    this->elementBits = new int[0x19];
    for (int i = 0; i != 0x19; i++) {
        (*this->keyArray)[i] = 0;
        this->elementBits[i] = 0;
    }
    this->touchFlags = 0;

    PaintCanvas *canvas = hud_canvas();
    if (canvas != nullptr && Status::gStatus != nullptr && Status::gStatus->inAlienOrbit() == 0) {
        SolarSystem *system = Status::gStatus->getSystem();
        const int race = system != nullptr ? system->getRace() : -1;
        if (race >= 0 && race < 4)
            hud_create_image(canvas, g_Hud_factionLogoResourceIds[race], this->factionLogoImage);
    }

    this->eventQueue = new Array<ListItem *>();
    ArraySetLength(0x14, *(this->eventQueue));

    if (Globals::layout != nullptr) {
        this->eventLineX = Globals::w >> 1;
        this->eventLineY = Globals::h - hud_layout_i32(0x13c);
        Hud::RADAR_WIDTH = Globals::w - 28;
        Hud::RADAR_HEIGHT = Globals::h - 33 - hud_layout_i32(0x13c) - hud_layout_i32(0x144) -
                            hud_layout_i32(0x1d8);
    } else {
        this->eventLineX = 0;
        this->eventLineY = 0;
        Hud::RADAR_WIDTH = 0;
        Hud::RADAR_HEIGHT = 0;
    }

    if (Status::gStatus != nullptr) {
        Ship *ship = Status::gStatus->getShip();
        if (ship != nullptr) {
            this->hasBoostButton = ship->getBoostDelay() > 0;
            this->hasShieldBar = ship->getMaxShieldHP() > 0;
            this->hasArmorRegen = ship->getMaxArmorHP() > 0;
            this->hasAutofireUI = ship->getFirePower() > 0.0f;
        }
    }

    this->secondaryLabelTimerSeed = -1;
    this->secondaryLabelTimer = 0;
    this->field_0x51c = String("");

    closeHudMenu();
    if (Status::gStatus != nullptr)
        checkIfQuickMenuIsEmpty();
    releaseAllKeys();

    if (canvas != nullptr && Globals::layout != nullptr) {
        Globals::pause_x = static_cast<float>(this->field_0x40a);
        Globals::pause_y = static_cast<float>(this->field_0x40c);
    } else {
        Globals::pause_x = 0.0f;
        Globals::pause_y = 0.0f;
    }
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

void Hud::drawMenu(int unused) {
    (void) unused;
    PaintCanvas *canvas = hud_canvas();
    if (canvas == nullptr || Globals::layout == nullptr) return;
    static_cast<Layout *>(Globals::layout)->drawMask();

    const int frameX = this->field_0x3c4 + this->menuOriginX;
    const int menuY = this->menuOriginYBase + this->menuOriginY;
    canvas->DrawImage2D(static_cast<unsigned>(this->quickMenuTopImage), frameX, menuY);

    const int headerX = this->menuOriginX + this->field_0x3d4 + this->field_0x3dc / 2;
    const int headerY = menuY + this->menuRowHeight / 2 - hud_layout_i32(0x22c);
    canvas->DrawImage2D(static_cast<unsigned>(this->quickMenuHeaderImage), headerX, headerY, 0x11, 0x44);

    int rowY = menuY + this->menuRowHeight;
    if (this->menuButtons != nullptr) {
        for (unsigned int i = 0; i + 1 < this->menuButtons->size(); ++i) {
            canvas->DrawImage2D(static_cast<unsigned>(this->quickMenuMiddleImage), frameX, rowY);
            rowY += this->field_0x3d0;
        }
    }
    canvas->DrawImage2D(static_cast<unsigned>(this->quickMenuBottomImage), frameX, rowY);

    if (this->menuButtons != nullptr) {
        for (unsigned int i = 0; i < this->menuButtons->size(); ++i) {
            TouchButton *button = (*this->menuButtons)[i];
            if (button != nullptr)
                button->draw();
        }
    }

    if (this->quickMenuType != 0) return;

    Status *status = Status::gStatus;
    Ship *ship = status != nullptr ? status->getShip() : nullptr;
    if (ship == nullptr) return;
    if (!ship->hasCloak() && ship->hasJumpDrive() == 0) return;

    String cargoLabel("X ");
    cargoLabel += this->fuelGaugeValue;

    const int gaugeX = headerX;
    const int gaugeY = rowY + hud_layout_i32(0x30) / 2 + hud_layout_i32(0x288);
    canvas->DrawImage2D(static_cast<unsigned>(this->fuelGaugeBarImage), gaugeX, gaugeY, 0x11, 0x14);
    canvas->DrawImage2D(static_cast<unsigned>(this->fuelGaugeIconImage), gaugeX - hud_layout_i32(0x230),
                        hud_layout_i32(0x30) + gaugeY + hud_layout_i32(0x28c), 0x11, 0x12);

    const int textY = gaugeY + canvas->GetImage2DHeight(static_cast<unsigned>(this->fuelGaugeBarImage)) / 2 -
                      canvas->GetTextHeight(hud_font()) / 2 + hud_layout_i32(0x234);
    canvas->DrawString(hud_font(), cargoLabel, gaugeX + hud_layout_i32(0x230), textY, false);
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

    PaintCanvas *canvas = hud_canvas();
    if (canvas == nullptr) return;
    int w = canvas->GetTextWidth(hud_font(), *line);
    int screenW = Globals::w;
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
    GameText *gt = hud_game_text();
    if (gt == nullptr) return;
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

    PaintCanvas *canvas = hud_canvas();
    if (canvas == nullptr) return;
    int w = canvas->GetTextWidth(hud_font(), *(String *) dst);
    int screenW = Globals::w;
    this->eventScrollTick = 0;
    this->eventScrolls = 1;
    this->letterbox = ((screenW / 2 - this->eventLineMargin) + this->eventLineMarginAlt * -2 < w) ? 1 : 0;
}


static TouchButton *hud_add_menu_button(Hud *self, const String &text, int y, unsigned int action) {
    if (self->menuButtons == nullptr) return nullptr;

    auto *button = new TouchButton(text, 0, self->field_0x3d4, y, self->field_0x3dc, 0x11, 4);
    button->field_0x0 = static_cast<int>(action);
    button->field_0x4 = 0;
    ArrayAdd<TouchButton *>(button, *self->menuButtons);
    return button;
}

static void hud_cache_menu_button_positions(Hud *self) {
    if (self->menuButtons == nullptr) return;

    const unsigned int count = self->menuButtons->size();
    for (unsigned int i = 0; i < count && i < 10; ++i) {
        TouchButton *button = (*self->menuButtons)[i];
        if (button == nullptr) continue;
        const Vector position = button->getPosition();
        Globals::sub_menu_buttons_x[i] = static_cast<int>(position.x);
        Globals::sub_menu_buttons_y[i] = static_cast<int>(position.y);
    }
}

static void hud_compact_orbit_menu_for_phone(Hud *self) {
    if (Globals::iPad != 0 || self->menuButtons == nullptr || self->menuButtons->size() < 5) return;

    const int rowGap = hud_layout_i32(0x30);
    for (unsigned int i = 0; i < self->menuButtons->size(); ++i) {
        TouchButton *button = (*self->menuButtons)[i];
        if (button == nullptr) continue;
        const Vector position = button->getPosition();
        button->setPosition(static_cast<int>(position.x), static_cast<int>(position.y) - rowGap);
    }
}

void Hud::initHudMenu(int menuType, Level *lvl) {
    if (this->menuButtons != nullptr) {
        ArrayReleaseClasses(*this->menuButtons);
        delete this->menuButtons;
        this->menuButtons = nullptr;
    }

    this->quickMenuType = menuType;
    this->menuButtons = new Array<TouchButton *>();
    this->menuOriginX = 0;

    PaintCanvas *canvas = hud_canvas();
    GameText *gameText = hud_game_text();
    Status *status = Status::gStatus;
    if (canvas == nullptr || gameText == nullptr || status == nullptr || Globals::layout == nullptr) {
        this->quickMenuOpen = 1;
        return;
    }

    Ship *ship = status->getShip();
    if (ship == nullptr) {
        this->quickMenuOpen = 1;
        return;
    }

    delete this->equipmentArray;
    this->equipmentArray = ship->getEquipment(1);
    updateSecondaryWeaponString();

    const int rowGap = hud_layout_i32(0x30);
    const int buttonHeight = hud_layout_i32(0x1dc);
    const int rowStep = rowGap + buttonHeight;
    int y = this->menuBaseY;

    if (Globals::iPad != 0) {
        const int anchor = menuType == 3
                ? (this->iPadSteerAnchor != 0 ? this->iPadSteerAnchor : hud_default_steer_anchor())
                : (this->iPadFireAnchor != 0 ? this->iPadFireAnchor : hud_default_fire_anchor());
        const float fireMenuOffset = Globals::iPadHD != 0 ? 112.5f : (Globals::iPadLarge != 0 ? 160.0f : 80.0f);
        const int clampedMenuY = anchor - (menuType == 3 ? 0 : static_cast<int>(fireMenuOffset));
        this->menuOriginY = clampedMenuY < 0 ? 0 : clampedMenuY;
        y = this->menuRowHeight + this->menuOriginY - rowGap / 2 + 1;
        this->menuBaseY = y;
    }

    switch (menuType) {
        case 0: {
            if (this->equipmentArray != nullptr) {
                for (unsigned int i = 0; i < this->equipmentArray->size(); ++i) {
                    if ((*this->equipmentArray)[i] != nullptr) {
                        hud_add_menu_button(this, *gameText->getText(266), y, 0x200);
                        y += rowStep;
                        break;
                    }
                }
            }
            if (status->getWingmen() != 0 && status->inSupernovaSystem() == 0 &&
                status->getCurrentCampaignMission() != 158) {
                hud_add_menu_button(this, *gameText->getText(306), y, 0x400);
                y += rowStep;
            }
            if (ship->hasCloak()) {
                Item *cloak = ship->getFirstEquipmentOfSort(21);
                if (cloak != nullptr) {
                    TouchButton *button = hud_add_menu_button(this, *gameText->getText(cloak->getIndex() + 1274), y, 0x800);
                    if (button != nullptr) {
                        button->setPressProgressHighlight(false);
                        PlayerEgo *player = lvl != nullptr ? lvl->getPlayer() : nullptr;
                        if (player != nullptr && (player->isCloaked() || player->isChargingCloak() || player->isRechargingCloak())) {
                            if (player->isRechargingCloak())
                                button->setPressProgress(player->getCloakRechargeRate());
                            button->setHalfTransparent(true);
                        }
                    }
                    y += rowStep;
                }
            }
            if (ship->hasJumpDrive() != 0) {
                TouchButton *button = hud_add_menu_button(this, *gameText->getText(1359), y, 0x1000);
                PlayerEgo *player = lvl != nullptr ? lvl->getPlayer() : nullptr;
                if (button != nullptr && player != nullptr && (player->isChargingDrive() || player->emergencySystemActive()))
                    button->setHalfTransparent(true);
            }

            Item *cargo = ship->getCargo(122);
            this->fuelGaugeValue = cargo != nullptr ? cargo->getAmount() : 0;
            hud_create_image(canvas, 0x4f5, this->quickMenuHeaderImage);
            break;
        }
        case 1: {
            if (this->equipmentArray != nullptr) {
                for (unsigned int i = 0; i < this->equipmentArray->size(); ++i) {
                    Item *item = (*this->equipmentArray)[i];
                    if (item == nullptr) continue;

                    String label = *gameText->getText(item->getIndex() + 1274);
                    label += String(" (");
                    label += item->getAmount();
                    label += String(")");
                    const unsigned int action = i == 0 ? 0x2000 : i == 1 ? 0x4000 : i == 2 ? 0x8000 : 0x10000;
                    hud_add_menu_button(this, label, y, action);
                    y += rowStep;
                }
            }
            hud_create_image(canvas, 0x4f4, this->quickMenuHeaderImage);
            break;
        }
        case 2: {
            const int textIds[4] = {307, 308, 309, (status->field_f8 & 0xff) != 0 ? 311 : 310};
            const unsigned int actions[4] = {0x20000, 0x40000, 0x80000, 0x100000};
            for (unsigned int i = 0; i < 4; ++i) {
                hud_add_menu_button(this, *gameText->getText(textIds[i]), y, actions[i]);
                y += rowStep;
            }
            hud_create_image(canvas, 0x4f3, this->quickMenuHeaderImage);
            break;
        }
        case 3: {
            if (Globals::iPad != 0)
                this->menuOriginX = hud_layout_i32(0x28) - this->field_0x3c4;

            if (!status->inAlienOrbit()) {
                hud_add_menu_button(this, *gameText->getText(549), y, 0x1000000);
                y += rowStep;

                if (!status->inEmptyOrbit()) {
                    Station *station = status->getStation();
                    if (station != nullptr) {
                        String stationLabel = station->getName();
                        // Android rodata at 0x1ca472 is an empty string for station 101.
                        if (station->getIndex() != 101) {
                            stationLabel += String(" ");
                            stationLabel += *gameText->getText(136);
                        }
                        hud_add_menu_button(this, stationLabel, y, 0x800000);
                        y += rowStep;
                    }
                }

                SolarSystem *system = status->getSystem();
                if (system != nullptr && system->currentOrbitHasWarpGate()) {
                    hud_add_menu_button(this, *gameText->getText(547), y, 0x400000);
                    y += rowStep;
                }

                PlayerEgo *player = lvl != nullptr ? lvl->getPlayer() : nullptr;
                Route *route = player != nullptr
                        ? reinterpret_cast<Route *>(static_cast<intptr_t>(player->getRoute()))
                        : nullptr;
                Waypoint *lastWaypoint = route != nullptr ? route->getLastWaypoint() : nullptr;
                if (lastWaypoint != nullptr && (lastWaypoint->state & 0xff) == 0) {
                    hud_add_menu_button(this, *gameText->getText(573), y, 0x2000000);
                    y += rowStep;
                }

                Station *programmedStation = static_cast<Station *>(Level::programmedStation);
                if (programmedStation != nullptr) {
                    String programmedLabel = *gameText->getText(546);
                    programmedLabel += String(": ");
                    programmedLabel += programmedStation->getName();
                    hud_add_menu_button(this, programmedLabel, y, 0x200000);
                    y += rowStep;
                }
            }

            if (lvl != nullptr) {
                const int dockingTargetCount = lvl->getNumDockingTargets();
                for (int i = 0; i < dockingTargetCount; ++i) {
                    auto *target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(lvl->getDockingTarget(i)));
                    if (target == nullptr) continue;
                    String targetName = target->getName();
                    if (targetName.size() == 0) continue;
                    hud_add_menu_button(this, targetName, y, 0x04000000u << i);
                    y += rowStep;
                }
            }

            hud_compact_orbit_menu_for_phone(this);
            hud_create_image(canvas, 0x4f4, this->quickMenuHeaderImage);
            break;
        }
        default:
            break;
    }

    const unsigned int buttonCount = this->menuButtons->size();
    if (Globals::iPad != 0 && buttonCount != 0) {
        this->menuOriginYBase = static_cast<int>(4 - buttonCount) * rowStep;
        if (menuType == 3)
            this->menuOriginYBase -= rowGap;
        for (unsigned int i = 0; i < buttonCount; ++i) {
            TouchButton *button = (*this->menuButtons)[i];
            if (button != nullptr)
                button->translate(this->menuOriginX, this->menuOriginYBase);
        }
    } else {
        this->menuOriginYBase = buttonCount < 5 ? 0 : -rowGap;
    }

    hud_cache_menu_button_positions(this);
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
