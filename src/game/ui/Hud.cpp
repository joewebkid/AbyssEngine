#include "game/ui/Hud.h"
#include "game/mission/Mission.h"
#include "game/mission/Item.h"
#include "engine/render/Sprite.h"
#include "engine/core/GameText.h"
#include "game/core/Globals.h"
#include "game/core/GameSettings.h"
#include "game/core/Radio.h"
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
#include "game/weapons/Radar.h"
#include "game/world/Level.h"
#include "game/world/LevelScript.h"
#include "game/ship/Player.h"
#include "engine/audio/FModSound.h"
#include "engine/render/PaintCanvas.h"
#include "game/ui/Layout.h"

#include <cstdint>
#include <new>

// Android Hud::hudEvent .rodata at 0x20377c. Bits are indexed from event 27.
static constexpr unsigned int kHudImportantEventMask = 0x100019;

struct HudSecurityColor {
    unsigned char r;
    unsigned char _pad_r[3];
    unsigned char g;
    unsigned char _pad_g[3];
    unsigned char b;
    unsigned char _pad_b[3];
};

// Android ARM .rodata: word_203758 (first halfword of each 32-bit slot).
static const unsigned int g_Hud_factionLogoResourceIds[4] = {0x4a6, 0x4a3, 0x4a5, 0x4a4};

// Android ARM .rodata: byte_203780, read with a 12-byte stride by drawOrbitInformation.
static const HudSecurityColor g_Hud_securityColors[4] = {
    {0xff, {0, 0, 0}, 0x2a, {0, 0, 0}, 0x00, {0, 0, 0}},
    {0xff, {0, 0, 0}, 0x6c, {0, 0, 0}, 0x00, {0, 0, 0}},
    {0xed, {0, 0, 0}, 0xed, {0, 0, 0}, 0x00, {0, 0, 0}},
    {0xed, {0, 0, 0}, 0x00, {0, 0, 0}, 0x00, {0, 0, 0}},
};
static_assert(sizeof(HudSecurityColor) == 12, "Android Hud security colors use a 12-byte stride");

static inline PaintCanvas *hud_canvas() {
    return static_cast<PaintCanvas *>(Globals::Canvas);
}

static inline unsigned int hud_font() {
    return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font));
}

static inline GameText *hud_game_text() {
    return static_cast<GameText *>(Globals::gameText);
}

// `_ZN7Globals5hintsE` is at 0x21824c in Android 2.0.16; the mining tutorial
// completion byte used by Hud::draw and MGame lives at 0x21825d.
static constexpr int kMiningTutorialHintIndex = 0x11;

static inline int hud_layout_i32(unsigned int offset) {
    return *reinterpret_cast<int *>(static_cast<char *>(Globals::layout) + offset);
}

static inline float hud_layout_f32(unsigned int offset) {
    return *reinterpret_cast<float *>(static_cast<char *>(Globals::layout) + offset);
}

static inline __attribute__((always_inline)) void
hud_draw_volatile_cargo(Hud *self, PlayerEgo *ego) {
    if (ego->hasVolatileGoods() == 0) return;

    const unsigned int image = static_cast<unsigned int>(self->volatileCargoOverlayImage);
    const int width = hud_canvas()->GetImage2DWidth(image);
    const int height = hud_canvas()->GetImage2DHeight(image);
    float force = 1.0f;
    if (ego->getVolatileForce() <= 1.0f)
        force = ego->getVolatileForce();
    hud_canvas()->DrawRegion2D(image, 0, 0,
                               static_cast<int>(force * static_cast<float>(width)), height,
                               0.0f, 0, 0,
                               self->missionPanelX - hud_layout_i32(0x1ec), self->missionPanelY);
}

static inline __attribute__((always_inline)) void hud_create_image(unsigned short resourceId, int &slot) {
    hud_canvas()->Image2DCreate(resourceId, reinterpret_cast<unsigned int &>(slot));
}

static inline __attribute__((always_inline)) void hud_load_init_images(Hud *self) {
    hud_create_image(0x4ac, self->shieldFrameImage);
    hud_create_image(0x4ad, self->shieldFrameHitImage);
    hud_create_image(0x4ae, self->shieldBarBgImage);
    hud_create_image(0x4af, self->shieldBarFillImage);
    hud_create_image(0x4aa, self->armorFrameImage);
    hud_create_image(0x4ab, self->armorFrameLowImage);
    hud_create_image(0x4a7, self->armorBarBgImage);
    hud_create_image(0x4a8, self->armorRegenFillImage);
    hud_create_image(0x524, self->armorBarFillImage);
    hud_create_image(0x1f59, self->gammaFrameImage);
    hud_create_image(0x1f5a, self->gammaBarBgImage);
    hud_create_image(0x1f5b, self->gammaBarFillImage);
    hud_create_image(0x4a9, self->barDividerImage);
    hud_create_image(0x4bb, self->quickMenuPressedImage);
    hud_create_image(0x4ba, self->quickMenuIdleImage);

    if (Globals::iPad != 0) {
        hud_create_image(0x4c6, self->iPadFireImage);
        hud_create_image(0x6aa, self->iPadFirePressedImage);
        self->reticleImage = self->iPadFireImage;
    } else {
        hud_create_image(0x4c6, self->reticleImage);
    }

    hud_create_image(0x4b5, self->mainActionPressedImage);
    hud_create_image(0x4b4, self->mainActionIdleImage);
    hud_create_image(0x536, self->targetContextOverlayImage);
    hud_create_image(0x4bd, self->secondaryPressedImage);
    hud_create_image(0x4bc, self->secondaryIdleImage);
    hud_create_image(0x4b9, self->pauseButtonPressedImage);
    hud_create_image(0x4b8, self->pauseButtonImage);
    hud_create_image(0x4b3, self->boostPressedImage);
    hud_create_image(0x4b2, self->boostIdleImage);
    hud_create_image(0x4b1, self->dockActionPressedImage);
    hud_create_image(0x4b0, self->dockActionIdleImage);
    hud_create_image(0x4b7, self->steeringKnobPressedImage);
    hud_create_image(0x4b6, self->steeringKnobIdleImage);
    hud_create_image(0x4c1, self->steeringBaseImage);
    hud_create_image(0x4c5, self->missionTimerPanelImage);
    hud_create_image(0x520, self->cargoPanelImage);
    hud_create_image(0x4c3, self->eventBannerImage);
    hud_create_image(0x4c2, self->secondaryWeaponBannerImage);
    hud_create_image(0x4cf, self->quickMenuTopImage);
    hud_create_image(0x4d1, self->quickMenuMiddleImage);
    hud_create_image(0x4d0, self->quickMenuBottomImage);
    hud_create_image(0x537, self->fuelGaugeIconImage);
    hud_create_image(0x538, self->fuelGaugeBarImage);
    hud_create_image(0x539, self->chargeProgressFillImage);
    hud_create_image(0x53a, self->progressPanelImage);
    hud_create_image(0x53a, self->progressPanelDuplicateImage);
    hud_create_image(0x1f41, self->dockTransferFillImage);
    hud_create_image(0x525, self->hitVerticalArmorImage);
    hud_create_image(0x526, self->hitHorizontalArmorImage);
    hud_create_image(0x52b, self->hitVerticalShieldImage);
    hud_create_image(0x52c, self->hitHorizontalShieldImage);
    hud_create_image(0x528, self->cameraIdleImages[0]);
    hud_create_image(0x527, self->cameraPressedImages[0]);
    hud_create_image(0x4e9, self->cameraIdleImages[1]);
    hud_create_image(0x4ea, self->cameraPressedImages[1]);
    hud_create_image(0x4be, self->cameraIdleImages[2]);
    hud_create_image(0x4bf, self->cameraPressedImages[2]);
    hud_create_image(0x52a, self->cameraIdleImages[3]);
    hud_create_image(0x529, self->cameraPressedImages[3]);
    hud_create_image(0x540, self->image_0x390);
    hud_create_image(0x541, self->image_0x394);
    hud_create_image(0x53f, self->image_0x398);
    hud_create_image(0x542, self->image_0x39c);
    hud_create_image(0x543, self->image_0x3a0);
    hud_create_image(0x546, self->autoTurretEnabledImage);
    hud_create_image(0x547, self->autoTurretDisabledImage);
    hud_create_image(0x1f58, self->image_0x3a4);
    hud_create_image(0x1f57, self->image_0x3a8);
    hud_create_image(0x4b1, self->image_0x3ac);
    hud_create_image(0x4b0, self->image_0x3b0);
    hud_create_image(0x1f43, self->passengerPanelImage);
    hud_create_image(0x1f42, self->missionStatusPanelImage);
    hud_create_image(0x1f40, self->dockTransferMissionMarkerImage);
    hud_create_image(0x1f61, self->productionCargoPanelImage);
    hud_create_image(0x1f60, self->productionRemainingPanelImage);
    hud_create_image(0x1f5f, self->dockTransferProductionMarkerImage);
    hud_create_image(0x1f5c, self->volatileCargoOverlayImage);
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

static inline __attribute__((always_inline)) void hud_apply_ipad_control_coords(Hud *self) {
    Globals *globals = static_cast<Globals *>(Globals::globals);
    GameSettings *settings = reinterpret_cast<GameSettings *>(Globals::options);
    const int steerAnchor = settings->steerAnchorX;
    const int fireAnchor = settings->fireAnchorX;

    globals->setCoordsSteer(steerAnchor,
                            hud_canvas()->GetImage2DWidth(static_cast<unsigned>(self->steeringBaseImage)),
                            hud_canvas()->GetImage2DWidth(static_cast<unsigned>(self->dockActionIdleImage)),
                            hud_canvas()->GetImage2DWidth(static_cast<unsigned>(self->boostIdleImage)),
                            self->field_0x3f8, self->field_0x3fa, self->field_0x42c, self->field_0x42e,
                            self->field_0x424, self->field_0x426, self->field_0x410, self->field_0x412,
                            self->field_0x404, self->field_0x406);

    globals->setCoordsFire(fireAnchor,
                           hud_canvas()->GetImage2DWidth(static_cast<unsigned>(self->iPadFireImage)),
                           static_cast<unsigned>(self->iPadFireImage),
                           static_cast<unsigned>(self->iPadFirePressedImage),
                           reinterpret_cast<unsigned int &>(self->reticleImage), self->iPadFireCoord_0x0c,
                           self->iPadFireCoord_0x0e, self->field_0x3e4, self->field_0x3e6,
                           self->field_0x416, self->field_0x418, self->field_0x3f2, self->field_0x3f4,
                           self->field_0x3ec, self->field_0x3ee, self->field_0x3fe, self->field_0x400);

    self->field_0x41e = self->field_0x424;
    self->field_0x420 = self->field_0x426;
    self->iPadSteerAnchor = settings->steerAnchorX;
    self->iPadFireAnchor = settings->fireAnchorX;
}

static inline __attribute__((always_inline)) void hud_init_coordinates(Hud *self) {
    const int screenW = Globals::w;
    const int screenH = Globals::h;
    const auto width = [](int image) { return hud_canvas()->GetImage2DWidth(static_cast<unsigned>(image)); };
    const auto height = [](int image) { return hud_canvas()->GetImage2DHeight(static_cast<unsigned>(image)); };

    self->field_0x434 = static_cast<unsigned short>(screenW - hud_layout_i32(0x14c));
    self->field_0x436 = static_cast<unsigned short>(screenH - hud_layout_i32(0x12c) -
                                                     hud_canvas()->GetTextHeight(hud_font()) - hud_layout_i32(0x150));
    self->field_0x3f0 = static_cast<unsigned short>(width(self->secondaryPressedImage));
    self->field_0x3e4 = static_cast<unsigned short>(screenW - hud_layout_i32(0x154) - width(self->mainActionIdleImage));
    self->field_0x3e6 = static_cast<unsigned short>(screenH - hud_layout_i32(0x158) - width(self->mainActionIdleImage));
    self->field_0x3ec = static_cast<unsigned short>(screenW - hud_layout_i32(0x15c) - self->field_0x3f0);
    self->field_0x3ee = static_cast<unsigned short>(screenH - hud_layout_i32(0x160) - self->field_0x3f0);
    self->field_0x3ea = static_cast<unsigned short>(hud_layout_i32(0x164));
    self->field_0x3e0 = static_cast<unsigned short>(screenW / 2 - width(self->eventBannerImage) / 2);
    self->field_0x3e2 = static_cast<unsigned short>(hud_layout_i32(0x168));

    self->field_0x3f6 = static_cast<unsigned short>(width(self->cameraIdleImages[0]));
    self->field_0x3f2 = static_cast<unsigned short>(screenW - self->field_0x3f6 - hud_layout_i32(0x16c));
    self->field_0x3f4 = static_cast<unsigned short>(screenH - hud_layout_i32(0x170) -
                                                     height(self->cameraIdleImages[0]));
    self->field_0x41a = static_cast<unsigned short>(width(self->quickMenuPressedImage));
    self->field_0x41c = static_cast<unsigned short>(hud_layout_i32(0x174));
    self->field_0x416 = static_cast<unsigned short>(screenW - hud_layout_i32(0x178) - self->field_0x41a);
    self->field_0x418 = static_cast<unsigned short>(screenH - hud_layout_i32(0x17c) -
                                                     height(self->quickMenuPressedImage));

    self->field_0x3fc = static_cast<unsigned short>(width(self->dockActionIdleImage));
    self->field_0x3f8 = static_cast<unsigned short>(hud_layout_i32(0x180));
    self->field_0x3fa = static_cast<unsigned short>(screenH - hud_layout_i32(0x184) - self->field_0x3fc);
    self->field_0x402 = static_cast<unsigned short>(width(self->dockActionIdleImage));
    self->field_0x3fe = static_cast<unsigned short>(screenW - hud_layout_i32(0x180) - self->field_0x3fc);
    self->field_0x400 = static_cast<unsigned short>(screenH - hud_layout_i32(0x184) - self->field_0x3fc);

    self->field_0x40e = static_cast<unsigned short>(width(self->pauseButtonPressedImage));
    self->field_0x40a = static_cast<unsigned short>(screenW - self->field_0x40e - hud_layout_i32(0x194));
    self->field_0x40c = static_cast<unsigned short>(hud_layout_i32(0x198));
    self->missionPanelX = static_cast<unsigned short>(screenW - width(self->missionTimerPanelImage) -
                                                       hud_layout_i32(0x19c));
    self->missionPanelY = static_cast<unsigned short>(hud_layout_i32(0x1a0));

    self->field_0x430 = static_cast<unsigned short>(width(self->steeringBaseImage));
    const int hackingImageWidth = width(self->image_0x3a4);
    self->field_0x45c = static_cast<unsigned short>(hackingImageWidth);
    const int hackingHalfWidth = hackingImageWidth >> 1;
    self->field_0x454 = static_cast<unsigned short>(screenW / 2 - hackingHalfWidth - hud_layout_i32(0x31c));
    self->field_0x458 = static_cast<unsigned short>(screenW / 2 - hackingHalfWidth + hud_layout_i32(0x31c));
    self->field_0x460 = self->field_0x3ee;
    self->field_0x45e = static_cast<unsigned short>(screenW / 2 - hackingHalfWidth);
    self->field_0x456 = static_cast<unsigned short>(screenH / 2 - hud_layout_i32(0x320));
    self->field_0x45a = self->field_0x456;

    self->field_0x42c = static_cast<unsigned short>(hud_layout_i32(0x1a4));
    self->field_0x42e = static_cast<unsigned short>(screenH - hud_layout_i32(0x1a8) -
                                                     height(self->steeringBaseImage));
    self->field_0x422 = static_cast<unsigned short>(width(self->steeringKnobPressedImage));
    self->field_0x424 = static_cast<unsigned short>(self->field_0x42c + self->field_0x430 / 2);
    self->field_0x41e = self->field_0x424;
    self->field_0x426 = static_cast<unsigned short>(self->field_0x42e + self->field_0x430 / 2);
    self->field_0x420 = self->field_0x426;
    self->field_0x414 = static_cast<unsigned short>(width(self->boostPressedImage));
    self->field_0x410 = static_cast<unsigned short>(hud_layout_i32(0x1ac));
    self->field_0x412 = static_cast<unsigned short>(screenH - hud_layout_i32(0x1b0) - self->field_0x414);
    self->field_0x408 = static_cast<unsigned short>(width(self->image_0x394));
    self->field_0x404 = static_cast<unsigned short>(hud_layout_i32(0x188));
    self->field_0x406 = static_cast<unsigned short>(screenH - hud_layout_i32(0x18c) - self->field_0x408);
    self->field_0x450 = hud_layout_i32(0x190);

    if (Globals::iPad != 0) {
        self->field_0x3c4 = screenW - hud_layout_i32(0x28) - width(self->quickMenuTopImage);
        self->menuOriginY = self->field_0x418 - hud_layout_i32(0x2c) - 6 * hud_layout_i32(0x30) -
                            height(self->quickMenuTopImage);
        hud_apply_ipad_control_coords(self);
    } else {
        self->field_0x3c4 = (screenW - width(self->quickMenuTopImage)) / 2;
        self->menuOriginY = hud_layout_i32(0x1b4);
        self->iPadSteerAnchor = 0;
        self->iPadFireAnchor = 0;
    }

    self->menuRowHeight = height(self->quickMenuTopImage);
    self->field_0x3d0 = height(self->quickMenuMiddleImage);
    self->field_0x3d4 = self->field_0x3c4 + hud_layout_i32(0x1b8);
    self->menuBaseY = hud_layout_i32(0x1bc) + self->menuOriginY + self->menuRowHeight -
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
    // Android ARM 0x16650c: movs r0, #0; bx lr.
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
    unsigned int index = 0;
    while (true) {
        const unsigned int next = index + 1;
        if (next >= q->size()) return;
        ListItem **items = q->data();
        if (items[index++ + 1] == nullptr) {
            items[next] = item;
            this->eventQueueDirty = 1;
            return;
        }
    }
}

unsigned int Hud::firePressed() {
    return (this->touchFlagsLow >> 4) & 1;
}

void Hud::resetAnalogStick() {
    this->steeringKnobX = this->steeringCenterX;
    this->steeringKnobY = this->steeringCenterY;
}

float Hud::getAnalogY() {
    float num = (float) ((int) this->steeringKnobY - (int) this->steeringCenterY);
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
    float num = (float) ((int) this->steeringKnobX - (int) this->steeringCenterX);
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

void Hud::draw(long long t0, long long t1, PlayerEgo *ego, bool letterbox,
               unsigned int currentCameraMode, unsigned int nextCameraMode) {
    (void) letterbox;
    const int elapsed = static_cast<int>(t0);
    const bool isMining = ego->isMining();
    PaintCanvas *canvas = hud_canvas();

    if (!isMining && this->eventQueueDirty != 0 && this->hackingGameActive == 0) {
        updateQueue(elapsed);
        drawEventQueue();
    }

    if (Globals::iPad != 0) {
        GameSettings *settings = reinterpret_cast<GameSettings *>(Globals::options);
        if (this->iPadSteerAnchor != settings->steerAnchorX ||
            this->iPadFireAnchor != settings->fireAnchorX) {
            Globals *globals = static_cast<Globals *>(Globals::globals);
            const int steerAnchor = settings->steerAnchorX;
            const int steeringWidth = canvas->GetImage2DWidth(
                static_cast<unsigned int>(this->steeringBaseImage));
            const int dockWidth = canvas->GetImage2DWidth(
                static_cast<unsigned int>(this->dockActionIdleImage));
            const int boostWidth = canvas->GetImage2DWidth(
                static_cast<unsigned int>(this->boostIdleImage));
            globals->setCoordsSteer(
                steerAnchor, steeringWidth, dockWidth, boostWidth,
                this->field_0x3f8, this->field_0x3fa, this->field_0x42c, this->field_0x42e,
                this->field_0x424, this->field_0x426, this->field_0x410, this->field_0x412,
                this->field_0x404, this->field_0x406);

            this->field_0x41e = this->field_0x424;
            this->field_0x420 = this->field_0x426;

            const int fireAnchor = settings->fireAnchorX;
            const int fireWidth = canvas->GetImage2DWidth(
                static_cast<unsigned int>(this->iPadFireImage));
            globals->setCoordsFire(
                fireAnchor, fireWidth, static_cast<unsigned int>(this->iPadFireImage),
                static_cast<unsigned int>(this->iPadFirePressedImage),
                reinterpret_cast<unsigned int &>(this->reticleImage), this->iPadFireCoord_0x0c,
                this->iPadFireCoord_0x0e, this->field_0x3e4, this->field_0x3e6,
                this->field_0x416, this->field_0x418, this->field_0x3f2, this->field_0x3f4,
                this->field_0x3ec, this->field_0x3ee, this->field_0x3fe, this->field_0x400);
            this->iPadSteerAnchor = settings->steerAnchorX;
            this->iPadFireAnchor = settings->fireAnchorX;
        }
    }

    const unsigned int initialColor = canvas->GetColor();

    if (this->shieldHitFlash != 0) {
        this->hitFlashTimer += elapsed;
        if (this->hitFlashTimer > 500) {
            this->shieldHitFlash = 0;
            this->hitFlashTimer = 0;
        }
    }
    if (this->field_0x470 >= 1) this->field_0x470 -= elapsed;

    const bool boostReady = ego->getBoostRate() == 1.0f;
    if (boostReady && this->boostReadyLatched == 0) {
        const int secondaryFlashRemaining = this->secondaryFlashRemaining;
        const int menuOriginX = this->boostReadyTextX;
        this->boostReadyLatched = 1;
        this->boostFlashRemaining = 2000;
        this->boostFlashPulse = 80;
        this->field_0x47c = secondaryFlashRemaining < 0
                                ? menuOriginX
                                : canvas->GetTextHeight(hud_font()) + menuOriginX;
    } else {
        this->boostReadyLatched = ego->getBoostRate() == 1.0f;
    }
    if (this->hasCloak != 0) {
        if (this->cloakReadyLatched != 0 || ego->isCloaked() != 0 || ego->isRechargingCloak()) {
            this->cloakReadyLatched = ego->isCloaked() != 0
                                          ? 0
                                          : static_cast<unsigned char>(!ego->isRechargingCloak());
        } else {
            this->cloakReadyLatched = 1;
            this->quickMenuFlashRemaining = 2000;
            this->quickMenuFlashPulse = 80;
        }
    }

    // --- shield/armor/gamma bars ---
    {
        const int dividerYOffset = hud_layout_i32(0x1e8);
        canvas->SetColor((unsigned) 0xffffffffu);
        Player *player = static_cast<Player *>(ego->player);

        const unsigned short *frameYAddress;
        const unsigned short *fillYAddress;
        if (this->hasShieldBar != 0) {
            int shp = player->getShieldHP();
            int frame = (shp < 2 || this->shieldHitFlash == 0) ? this->shieldFrameImage : this->shieldFrameHitImage;
            canvas->DrawImage2D((unsigned) frame, this->field_0x43c, this->field_0x442);
            canvas->DrawImage2D((unsigned) this->barDividerImage, this->field_0x43e,
                                this->field_0x442 + dividerYOffset);
            canvas->DrawImage2D((unsigned) this->shieldBarBgImage, this->field_0x440, this->field_0x44a);
            int rate = player->getShieldDamageRate();
            int w = static_cast<int>((static_cast<float>(rate) * 0.01f) *
                                     static_cast<float>(this->field_0x446));
            canvas->DrawRegion2D((unsigned) this->shieldBarFillImage, 0, 0, w, this->field_0x44c,
                                 0.0f, 0, 0, this->field_0x440, this->field_0x44a);
            frameYAddress = &this->field_0x444;
            fillYAddress = &this->field_0x448;
        } else {
            frameYAddress = &this->field_0x442;
            fillYAddress = &this->field_0x44a;
        }

        const unsigned short frameY = *frameYAddress;
        const unsigned short fillY = *fillYAddress;
        int ahp = player->getArmorHP();
        int aframe = (ahp < 1) ? this->armorFrameLowImage : this->armorFrameImage;
        canvas->DrawImage2D((unsigned) aframe, this->field_0x43c, frameY);
        canvas->DrawImage2D((unsigned) this->barDividerImage, this->field_0x43e,
                            frameY + dividerYOffset);
        canvas->DrawImage2D((unsigned) this->armorBarBgImage, this->field_0x440, fillY);
        int hrate = ego->getHullDamageRate();
        int hw = static_cast<int>((static_cast<float>(hrate) * 0.01f) *
                                  static_cast<float>(this->field_0x446));
        canvas->DrawRegion2D((unsigned) this->armorBarFillImage, 0, 0, hw, this->field_0x44c,
                             0.0f, 0, 0, this->field_0x440, fillY);

        if (this->hasArmorRegen != 0) {
            int arate = player->getArmorDamageRate();
            int aw = static_cast<int>((static_cast<float>(arate) * 0.01f) *
                                      static_cast<float>(this->field_0x446));
            canvas->DrawRegion2D((unsigned) this->armorRegenFillImage, 0, 0, aw, this->field_0x44c,
                                 0.0f, 0, 0, this->field_0x440, fillY);
        }

        Status *status = Status::gStatus;
        Station *station = status != nullptr ? status->getStation() : nullptr;
        if (station != nullptr) {
            union {
                int bits;
                float value;
            } gammaRate = {status->getGammaRayDamagePerSecond(
                station->getIndex(), status->getCurrentCampaignMission())};
            if (gammaRate.value > 0.0f) {
                const int gammaFrameBase = static_cast<int>(this->field_0x444);
                const int gammaFrameY = gammaFrameBase -
                                        static_cast<int>(this->field_0x442) + gammaFrameBase;
                const int gammaFillBase = static_cast<int>(this->field_0x448);
                const int gammaFillY = gammaFillBase -
                                       static_cast<int>(this->field_0x44a) + gammaFillBase;
                canvas->DrawImage2D((unsigned) this->gammaFrameImage,
                                    this->field_0x43c, gammaFrameY);
                canvas->DrawImage2D((unsigned) this->barDividerImage, this->field_0x43e,
                                    gammaFrameY + dividerYOffset);
                canvas->DrawImage2D((unsigned) this->gammaBarBgImage,
                                    this->field_0x440, gammaFillY);
                const int gammaWidth = static_cast<int>(
                    (static_cast<float>(player->getGammaHP()) / 100.0f) *
                    static_cast<float>(this->field_0x446));
                canvas->DrawRegion2D((unsigned) this->gammaBarFillImage,
                                     0, 0, gammaWidth, this->field_0x44c,
                                     0.0f, 0, 0, this->field_0x440, gammaFillY);
            }
        }
    }

    if (ego->isInRocketControl()) {
        if ((this->touchFlagsLow & 8u) != 0 ||
            (this->secondaryFlashRemaining >= 1 && this->secondaryFlashPulse <= 0)) {
            canvas->DrawImage2D(static_cast<unsigned int>(this->secondaryPressedImage),
                                this->field_0x3ec, this->field_0x3ee);
            if (this->secondaryFlashRemaining >= 1)
                this->secondaryFlashPulse = 80;
        } else {
            canvas->DrawImage2D(static_cast<unsigned int>(this->secondaryIdleImage),
                                this->field_0x3ec, this->field_0x3ee);
        }
        {
            if (Globals::options[0x11] == 0 ||
                (ego->isAutoPilot() || ego->isDockingToAsteroid() ||
                 ego->isDockingToDockingPoint() || this->hackingGameActive != 0 ||
                 (ego->isDockedToDockingPoint() && ego->isInTurretMode() == 0)) &&
                    (Globals::options[0x11] == 0 || ego->isInTurretMode() == 0))
                canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                                 static_cast<unsigned char>(0xff), static_cast<unsigned char>(0x32));
            else
                canvas->SetColor(static_cast<unsigned int>(0xffffffffu));

            canvas->DrawImage2D(static_cast<unsigned int>(this->steeringBaseImage),
                                this->field_0x42c, this->field_0x42e);
            if (Globals::options[0x11] != 0 && (this->touchFlagsLow & 0x20u) != 0) {
                canvas->DrawImage2D(static_cast<unsigned int>(this->steeringKnobPressedImage),
                                    this->field_0x41e, this->field_0x420, 0x11, 0x44);
            } else {
                this->field_0x41e = this->field_0x424;
                this->field_0x420 = this->field_0x426;
                canvas->DrawImage2D(static_cast<unsigned int>(this->steeringKnobIdleImage),
                                    this->field_0x41e, this->field_0x420, 0x11, 0x44);
            }
            canvas->SetColor(initialColor);
        }
        return;
    }

    // PlayerEgo+0x20 is a direction bitfield written by the collision/damage
    // path. Android Hud::draw turns each asserted side into a 300 ms pulse.
    {
        unsigned int horizontalImageOffset = 0x36c;
        if (ego->getShieldDamageRate() < 1)
            horizontalImageOffset = 0x364;
        const int horizontalImage = *reinterpret_cast<int *>(
            reinterpret_cast<char *>(this) + horizontalImageOffset);
        unsigned int verticalImageOffset = 0x368;
        if (ego->getShieldDamageRate() < 1)
            verticalImageOffset = 0x360;
        const int verticalImage = *reinterpret_cast<int *>(
            reinterpret_cast<char *>(this) + verticalImageOffset);
        const unsigned int flags = ego->hudHitDirectionFlags;
        if ((flags & 0x01u) != 0) this->hitDirectionLeftTimer = 300;
        if ((flags & 0x02u) != 0) this->hitDirectionRightTimer = 300;
        if ((flags & 0x24u) != 0) this->hitDirectionBottomTimer = 300;
        if ((flags & 0x18u) != 0) this->hitDirectionTopTimer = 300;

        Radar *radar = static_cast<Radar *>(ego->field_0x14);
        if (this->hitDirectionLeftTimer >= 1) {
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(255 * this->hitDirectionLeftTimer / 300));
            canvas->DrawImage2D(
                static_cast<unsigned int>(horizontalImage),
                (Globals::w >> 1) - radar->imageWidth,
                Globals::h >> 1,
                canvas->GetImage2DWidth(static_cast<unsigned int>(horizontalImage)),
                canvas->GetImage2DHeight(static_cast<unsigned int>(horizontalImage)),
                0x11, 0x41, 1);
            this->hitDirectionLeftTimer -= elapsed;
        }
        if (this->hitDirectionRightTimer >= 1) {
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(255 * this->hitDirectionRightTimer / 300));
            canvas->DrawImage2D(
                static_cast<unsigned int>(horizontalImage),
                (Globals::w >> 1) - radar->imageWidth,
                Globals::h >> 1, 0x12, 0x42);
            this->hitDirectionRightTimer -= elapsed;
        }
        if (this->hitDirectionTopTimer >= 1) {
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(255 * this->hitDirectionTopTimer / 300));
            canvas->DrawImage2D(
                static_cast<unsigned int>(verticalImage), Globals::w >> 1,
                (Globals::h >> 1) - radar->imageHeight,
                0x11, 0x14);
            this->hitDirectionTopTimer -= elapsed;
        }
        if (this->hitDirectionBottomTimer >= 1) {
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(255 * this->hitDirectionBottomTimer / 300));
            canvas->DrawImage2D(
                static_cast<unsigned int>(verticalImage), Globals::w >> 1,
                (Globals::h >> 1) - radar->imageHeight,
                canvas->GetImage2DWidth(static_cast<unsigned int>(verticalImage)),
                canvas->GetImage2DHeight(static_cast<unsigned int>(verticalImage)),
                0x21, 0x24, 2);
            this->hitDirectionBottomTimer -= elapsed;
        }
    }

    {
        if (Globals::options[0x11] == 0 ||
            (ego->isAutoPilot() || ego->isDockingToAsteroid() ||
             ego->isDockingToDockingPoint() || this->hackingGameActive != 0 ||
             (ego->isDockedToDockingPoint() && ego->isInTurretMode() == 0)) &&
                (Globals::options[0x11] == 0 || ego->isInTurretMode() == 0))
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff), static_cast<unsigned char>(0x32));
        else
            canvas->SetColor(static_cast<unsigned int>(0xffffffffu));

        canvas->DrawImage2D(static_cast<unsigned int>(this->steeringBaseImage),
                            this->field_0x42c, this->field_0x42e);
        if (Globals::options[0x11] != 0 && (this->touchFlagsLow & 0x20u) != 0) {
            canvas->DrawImage2D(static_cast<unsigned int>(this->steeringKnobPressedImage),
                                this->field_0x41e, this->field_0x420, 0x11, 0x44);
        } else {
            this->field_0x41e = this->field_0x424;
            this->field_0x420 = this->field_0x426;
            canvas->DrawImage2D(static_cast<unsigned int>(this->steeringKnobIdleImage),
                                this->field_0x41e, this->field_0x420, 0x11, 0x44);
        }
        canvas->SetColor(static_cast<unsigned int>(0xffffffffu));
    }

    // Cargo, passenger and timed-mission panel at Android Hud+0x438/+0x43a.
    // The mission timer is the second draw argument supplied by MGame, not wall
    // clock time.
    if (Globals::status->getMission() == nullptr ||
        Globals::status->getMission()->getType() != 12) {
        if (t1 < 1 || Globals::status->getCurrentCampaignMission() == 42) {
            if (Globals::status->getMission() != nullptr &&
                Globals::status->getMission()->getType() == 184) {
                bool usePassengerPanel;
                if (Globals::status->getCurrentCampaignMission() == 102 &&
                    Globals::status->inAlienOrbit() == 0 &&
                    Globals::status->getStation()->getIndex() == 113) {
                    usePassengerPanel = false;
                } else {
                    const bool useCargoPanel =
                        Globals::status->getCurrentCampaignMission() == 139 &&
                        Globals::status->inAlienOrbit() == 0 &&
                        Globals::status->getStation()->getIndex() == 131;
                    usePassengerPanel = !useCargoPanel;
                }

                String passengerLabel = String(Globals::status->missionPassengerCount) + String(" / ") +
                                        String(Globals::status->getShip()->getMaxPassengers());
                if (usePassengerPanel) {
                    hud_canvas()->DrawImage2D(static_cast<unsigned int>(this->passengerPanelImage),
                                              this->missionPanelX - hud_layout_i32(0x1f0),
                                              this->missionPanelY);
                } else {
                    hud_canvas()->DrawImage2D(static_cast<unsigned int>(this->cargoPanelImage),
                                              this->missionPanelX - hud_layout_i32(0x1ec),
                                              this->missionPanelY);
                }
                hud_draw_volatile_cargo(this, ego);

                if (usePassengerPanel) {
                    hud_canvas()->DrawString(hud_font(), passengerLabel,
                                             this->missionPanelX + hud_layout_i32(0x200),
                                             this->missionPanelY + 5, false);
                } else {
                    String cargoLabel = String(Globals::status->getShip()->getCurrentLoad()) +
                                        String(" / ") +
                                        String(Globals::status->getShip()->getMaxLoad()) + String("t");
                    int shift = 0;
                    if (Globals::status->getShip()->getCurrentLoad() >= 101)
                        shift = -2 * hud_layout_i32(0x2c);
                    hud_canvas()->DrawString(hud_font(), cargoLabel,
                                             this->missionPanelX + shift - hud_layout_i32(0x208),
                                             this->missionPanelY + 5, false);
                }

                String statusLabel(Globals::status->getMission()->getStatusValue());
                const int passengerHeight = hud_canvas()->GetImage2DHeight(
                    static_cast<unsigned int>(this->passengerPanelImage));
                hud_canvas()->DrawImage2D(
                    static_cast<unsigned int>(this->missionStatusPanelImage),
                    this->missionPanelX - hud_layout_i32(0x1f0),
                    this->missionPanelY + passengerHeight + hud_layout_i32(0x1f4));
                hud_canvas()->DrawString(
                    hud_font(), statusLabel,
                    this->missionPanelX + hud_layout_i32(0x200),
                    this->missionPanelY + passengerHeight + hud_layout_i32(0x1f4) +
                        2 * hud_layout_i32(0x1f8),
                    false);
            } else if (Globals::status->getMission() != nullptr &&
                       Globals::status->getMission()->getType() == 174) {
                int amount;
                if (Globals::status->getShip()->getCargo(
                        Globals::status->getMission()->getProductionGoodIndex()) != nullptr) {
                    amount = Globals::status->getShip()
                                 ->getCargo(Globals::status->getMission()->getProductionGoodIndex())
                                 ->getAmount();
                } else {
                    amount = 0;
                }
                String productionLabel = amount + String(" / ") +
                                         String(amount + Globals::status->getShip()->getFreeSpace());
                hud_canvas()->DrawImage2D(
                    static_cast<unsigned int>(this->productionCargoPanelImage),
                    this->missionPanelX - hud_layout_i32(0x1f0), this->missionPanelY);
                hud_draw_volatile_cargo(this, ego);
                hud_canvas()->DrawString(hud_font(), productionLabel,
                                         this->missionPanelX + hud_layout_i32(0x200),
                                         this->missionPanelY + 4, false);

                String remainingLabel(Globals::status->getMission()->getProductionGoodAmount() -
                                      Globals::status->getMission()->getStatusValue());
                const int passengerHeight = hud_canvas()->GetImage2DHeight(
                    static_cast<unsigned int>(this->passengerPanelImage));
                hud_canvas()->DrawImage2D(
                    static_cast<unsigned int>(this->productionRemainingPanelImage),
                    this->missionPanelX - hud_layout_i32(0x1f0),
                    this->missionPanelY + passengerHeight + hud_layout_i32(0x1f4));
                hud_canvas()->DrawString(
                    hud_font(), remainingLabel,
                    this->missionPanelX + hud_layout_i32(0x200),
                    this->missionPanelY + passengerHeight + hud_layout_i32(0x1f4) +
                        hud_layout_i32(0x1f8),
                    false);
            } else {
                String cargoLabel = String(Globals::status->getShip()->getCurrentLoad()) +
                                    String(" / ") +
                                    String(Globals::status->getShip()->getMaxLoad()) + String("t");
                int shift = 0;
                if (Globals::status->getShip()->getCurrentLoad() >= 101)
                    shift = -2 * hud_layout_i32(0x2c);
                hud_canvas()->DrawImage2D(static_cast<unsigned int>(this->cargoPanelImage),
                                          this->missionPanelX - hud_layout_i32(0x1ec),
                                          this->missionPanelY);
                hud_draw_volatile_cargo(this, ego);
                hud_canvas()->DrawString(hud_font(), cargoLabel,
                                         this->missionPanelX + shift - hud_layout_i32(0x208),
                                         this->missionPanelY + 5, false);
            }
        } else {
            String timerLabel;
            static_cast<Globals *>(Globals::globals)->longToTimeString(t1, timerLabel);
            hud_canvas()->DrawImage2D(static_cast<unsigned int>(this->missionTimerPanelImage),
                                      this->missionPanelX, this->missionPanelY);
            hud_canvas()->DrawString(hud_font(), timerLabel,
                                     this->missionPanelX + hud_layout_i32(0x204),
                                     this->missionPanelY + 5, false);
        }
    } else {
        String label = String(ego->level->killCountB) + String(" : ") +
                       String(ego->level->killCountA);
        hud_canvas()->DrawImage2D(static_cast<unsigned int>(this->cargoPanelImage),
                                  this->missionPanelX - hud_layout_i32(0x1ec),
                                  this->missionPanelY);
        hud_draw_volatile_cargo(this, ego);
        hud_canvas()->DrawString(hud_font(), label,
                                 this->missionPanelX + hud_layout_i32(0x1fc),
                                 this->missionPanelY + 5, false);
    }

    if (Globals::mouseCursorActivated != 0 || this->quickMenuOpen != 0)
        canvas->SetColor(static_cast<unsigned>(0xffffff00u));

    if (Globals::iPad != 0)
        canvas->DrawImage2D(static_cast<unsigned>(this->reticleImage),
                            this->iPadFireCoord_0x0c, this->iPadFireCoord_0x0e);
    else
        canvas->DrawImage2D(static_cast<unsigned>(this->reticleImage), Globals::w, Globals::h, 0x11, 0x22);

    if (this->hackingGameActive != 0 && ego->isInTurretMode() == 0) {
        int image = (this->touchFlagsByte1 & 2u) != 0 ? this->image_0x3a4 : this->image_0x3a8;
        canvas->DrawImage2D(static_cast<unsigned int>(image), this->field_0x454, this->field_0x456);
        image = (this->touchFlagsByte1 & 4u) != 0 ? this->image_0x3a4 : this->image_0x3a8;
        canvas->DrawImage2D(static_cast<unsigned int>(image), this->field_0x458, this->field_0x45a);
    }

    if (isMining || this->hackingGameActive != 0 || ego->isDockedToDockingPoint() ||
        ego->isLandingOrTakingOff()) {
        canvas->SetColor(static_cast<unsigned int>(0xffffff2fu));
    }

    if ((!Status::gStatus->inAlienOrbit() ||
         (Status::gStatus->inAlienOrbit() &&
          Status::gStatus->getCurrentCampaignMission() == 154 &&
          ego->level->getNumDockingTargets() >= 1)) &&
        Status::gStatus->getCurrentCampaignMission() >= 2) {
        if (Status::gStatus->getMission() == nullptr ||
            Status::gStatus->getMission()->getType() != 183) {
            int image = (this->touchFlagsLow & 0x40u) != 0
                            ? this->dockActionPressedImage
                            : this->dockActionIdleImage;
            canvas->DrawImage2D(static_cast<unsigned int>(image), this->field_0x3f8,
                                this->field_0x3fa);
        }
    }

    Radar *radar = static_cast<Radar *>(ego->field_0x14);
    if (!ego->radioRef->isShowingMessage()) {
        if ((!ego->isAutoPilot() && !ego->isDockingToAsteroid() &&
             (!ego->isDockingToDockingPoint() || ego->isLandingOrTakingOff()) &&
             this->hackingGameActive == 0) ||
            radar->field_0x54 != 0 || ego->aboutToReachAutoTarget()) {
            if (this->field_0x0 != 0) {
                if (this->timeExtenderDuration >= 0) {
                    this->timeExtenderTimer -= elapsed;
                    this->timeExtenderDuration -= elapsed;
                }
                const unsigned int savedColor = canvas->GetColor();
                if (this->field_0x280 == 0) {
                    canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                                     static_cast<unsigned char>(0xff), static_cast<unsigned char>(0x37));
                }
                canvas->DrawImage2D(static_cast<unsigned int>(this->image_0x398),
                                    this->field_0x450 + this->field_0x404, this->field_0x406);

                int image;
                if ((this->touchFlagsByte1 & 1u) != 0) {
                    image = this->image_0x39c;
                } else if (this->timeExtenderDuration >= 1 && this->timeExtenderTimer < 1) {
                    this->timeExtenderTimer = 80;
                    image = this->image_0x39c;
                } else if (this->field_0x281 == 0) {
                    image = this->image_0x3a0;
                } else if (this->timeExtenderDuration < 1) {
                    image = this->image_0x39c;
                } else {
                    this->timeExtenderTimer = 80;
                    image = this->image_0x39c;
                }
                canvas->DrawImage2D(static_cast<unsigned int>(image), this->field_0x404,
                                    this->field_0x406);
                canvas->SetColor(savedColor);
            }
        } else {
            canvas->DrawImage2D(static_cast<unsigned int>(this->image_0x398),
                                this->field_0x450 + this->field_0x404, this->field_0x406);
            const int image = (this->touchFlagsByte1 & 1u) != 0
                                  ? this->image_0x390
                                  : this->image_0x394;
            canvas->DrawImage2D(static_cast<unsigned int>(image), this->field_0x404,
                                this->field_0x406);
            if (Globals::mouseCursorActivated != 0 || this->quickMenuOpen != 0)
                canvas->SetColor(static_cast<unsigned int>(0xffffff00u));
        }
    }

    if ((ego->isAutoPilot() || ego->isInDockingProcedure()) &&
        !ego->isDockedToDockingPoint() && !ego->isLandingOrTakingOff()) {
        canvas->SetColor(static_cast<unsigned int>(0xffffffffu));
        canvas->DrawImage2D(static_cast<unsigned int>(this->dockActionPressedImage),
                            this->field_0x3f8, this->field_0x3fa);
    }

    canvas->SetColor(static_cast<unsigned>(0xffffffffu));
    if (Globals::mouseCursorActivated != 0 || this->quickMenuOpen != 0)
        canvas->SetColor(static_cast<unsigned int>(0xffffff00u));
    const int cameraImage = (this->touchFlagsLow & 0x80u) != 0
                                ? this->cameraPressedImages[nextCameraMode]
                                : this->cameraIdleImages[nextCameraMode];
    canvas->DrawImage2D(static_cast<unsigned>(cameraImage), this->field_0x3f2, this->field_0x3f4);

    if (this->previousCameraMode == -1) {
        this->previousCameraMode = static_cast<int>(currentCameraMode);
    } else if (this->previousCameraMode != static_cast<int>(currentCameraMode)) {
        this->previousCameraMode = static_cast<int>(currentCameraMode);
        this->cameraModeLabelTimer = elapsed;
        switch (currentCameraMode) {
            case 0:
                this->cameraModeLabel = *hud_game_text()->getText(217);
                break;
            case 1:
                this->cameraModeLabel = *hud_game_text()->getText(218);
                break;
            case 2:
                this->cameraModeLabel = *hud_game_text()->getText(219);
                break;
            case 3:
                this->cameraModeLabel = *hud_game_text()->getText(220);
                break;
            default:
                break;
        }
    }

    if (ego->hasAutoTurret()) {
        const int image = (ego->autoTurretIsEnabled() || (this->touchFlagsByte3 & 0x20u) != 0)
                              ? this->autoTurretEnabledImage
                              : this->autoTurretDisabledImage;
        canvas->DrawImage2D(static_cast<unsigned>(image), this->field_0x3fe, this->field_0x400);
    } else if (this->cameraModeLabelTimer >= 1) {
        const int bannerY = this->field_0x418 -
                            canvas->GetImage2DHeight(static_cast<unsigned int>(this->eventBannerImage)) -
                            hud_layout_i32(0x20c);
        const int canvasWidth = canvas->GetWidth();
        int alpha = static_cast<int>((static_cast<float>(this->cameraModeLabelTimer) / 2000.0f) * 255.0f);
        if (alpha > 255) alpha = static_cast<unsigned char>(-2 - alpha);
        const unsigned int savedColor = canvas->GetColor();
        canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                          static_cast<unsigned char>(0xff), static_cast<unsigned char>(alpha));
        canvas->DrawImage2D(static_cast<unsigned>(this->eventBannerImage), this->field_0x3ec, bannerY);
        const int textWidth = canvas->GetTextWidth(hud_font(), this->cameraModeLabel);
        const int textX = this->field_0x3ec + (canvasWidth - this->field_0x3ec - textWidth) / 2;
        canvas->DrawString(hud_font(), this->cameraModeLabel, textX,
                           bannerY + hud_layout_i32(0x210), false);
        canvas->SetColor(savedColor);
        this->cameraModeLabelTimer += elapsed;
        if (this->cameraModeLabelTimer > 4000) this->cameraModeLabelTimer = 0;
    }

    if (Globals::mouseCursorActivated != 0 || this->quickMenuOpen != 0)
        canvas->SetColor(static_cast<unsigned>(0xffffff00u));
    if (this->quickMenuEmpty == 0) {
        if ((this->touchFlagsLow & 4u) != 0 ||
            (this->quickMenuFlashRemaining >= 1 && this->quickMenuFlashPulse <= 0)) {
            canvas->DrawImage2D(static_cast<unsigned int>(this->quickMenuPressedImage),
                                this->field_0x416, this->field_0x418);
            if (this->quickMenuFlashRemaining >= 1)
                this->quickMenuFlashPulse = 80;
        } else {
            canvas->DrawImage2D(static_cast<unsigned int>(this->quickMenuIdleImage),
                                this->field_0x416, this->field_0x418);
        }
    }

    if ((this->currentSecondaryWeapon != nullptr && ego->isInTurretMode() == 0 &&
         this->currentSecondaryWeapon->getAmount() > 0) ||
        (ego->level != nullptr && ego->level->manualSecondaryActive != 0)) {
        if ((this->touchFlagsLow & 8u) != 0 ||
            (this->secondaryFlashRemaining >= 1 && this->secondaryFlashPulse <= 0)) {
            canvas->DrawImage2D(static_cast<unsigned int>(this->secondaryPressedImage),
                                this->field_0x3ec, this->field_0x3ee);
            if (this->secondaryFlashRemaining >= 1)
                this->secondaryFlashPulse = 80;
        } else {
            canvas->DrawImage2D(static_cast<unsigned int>(this->secondaryIdleImage),
                                this->field_0x3ec, this->field_0x3ee);
        }

        if (Globals::mouseCursorActivated != 0)
            canvas->SetColor(static_cast<unsigned>(0xffffffffu));
        canvas->DrawImage2D(static_cast<unsigned>(this->secondaryWeaponBannerImage),
                            Globals::w >> 1, Globals::h, 0x11, 0x24);

        String prefix(" (");
        String amount(this->currentSecondaryWeapon->getAmount());
        String prefixAmount = prefix + amount;
        String close(")");
        String suffixCore = prefixAmount + close;
        String suffix(suffixCore, false);
        String secondaryLabel = *hud_game_text()->getText(
                                    this->currentSecondaryWeapon->getIndex() + 1274) +
                                suffix;
        const int secondaryLabelX = (Globals::w >> 1) -
                                    (canvas->GetTextWidth(hud_font(), secondaryLabel) >> 1);
        canvas->DrawString(hud_font(), secondaryLabel, secondaryLabelX,
                           Globals::h - hud_layout_i32(0x04) + hud_layout_i32(0x214), false);
        if (Globals::mouseCursorActivated != 0 || this->quickMenuOpen != 0)
            canvas->SetColor(static_cast<unsigned>(0xffffff00u));
    }

    if (this->hasCloak != 0 && this->quickMenuFlashRemaining >= 0) {
        this->quickMenuFlashRemaining -= elapsed;
        this->quickMenuFlashPulse -= elapsed;
    }
    if (this->hasBoostButton != 0) {
        if (this->boostFlashRemaining >= 0) {
            this->boostFlashRemaining -= elapsed;
            this->boostFlashPulse -= elapsed;
        }
        unsigned char boostAlpha;
        if (ego->boosting() != 0)
            boostAlpha = 0;
        else if (ego->getBoostRate() >= 1.0f)
            boostAlpha = 200;
        else
            boostAlpha = static_cast<unsigned char>(static_cast<int>(ego->getBoostRate() * 75.0f));
        canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                         static_cast<unsigned char>(0xff),
                         static_cast<unsigned char>(boostAlpha + 55));

        int boostBlocked = Globals::mouseCursorActivated;
        if (Globals::mouseCursorActivated != 0)
            boostBlocked = 1;
        boostBlocked |= isMining;
        bool boostAvailable = boostBlocked == 0;
        if (boostBlocked == 0)
            boostAvailable = this->hackingGameActive == 0;
        if (!boostAvailable || ego->isDockedToDockingPoint()) {
            unsigned int boostColor;
            if (ego->getBoostRate() < 1.0f)
                boostColor = static_cast<unsigned>(0xffffff2fu);
            else
                boostColor = static_cast<unsigned>(0xffffff00u);
            canvas->SetColor(boostColor);
        }

        const int boostFlashRemaining = this->boostFlashRemaining;
        if ((this->touchFlagsLow & 2u) != 0) {
            if (boostFlashRemaining < 1) {
                canvas->DrawImage2D(static_cast<unsigned>(this->boostPressedImage),
                                    this->field_0x410, this->field_0x412);
                goto boost_drawn;
            }
        } else if (boostFlashRemaining < 1 || this->boostFlashPulse >= 1) {
            canvas->DrawImage2D(static_cast<unsigned>(this->boostIdleImage),
                                this->field_0x410, this->field_0x412);
            goto boost_drawn;
        }

        {
            this->boostFlashPulse = 80;
            if (Globals::mouseCursorActivated != 0)
                canvas->SetColor(static_cast<unsigned>(0xffffffffu));
            canvas->DrawImage2D(static_cast<unsigned>(this->boostPressedImage),
                                this->field_0x410, this->field_0x412);
        }
    }

boost_drawn:
    canvas->SetColor(static_cast<unsigned>(0xffffffffu));
    if (Globals::mouseCursorActivated != 0 || this->quickMenuOpen != 0)
        canvas->SetColor(static_cast<unsigned>(0xffffff00u));
    const int mainActionImage = ((this->touchFlagsLow & 0x10u) != 0 || this->fireForTutorial != 0)
                                    ? this->mainActionPressedImage
                                    : this->mainActionIdleImage;
    if (Globals::iPad != 0)
        canvas->DrawImage2D(static_cast<unsigned>(mainActionImage), this->field_0x3e4,
                            this->field_0x3e6, 0x11, 0x44);
    else
        canvas->DrawImage2D(static_cast<unsigned>(mainActionImage), this->field_0x3e4, this->field_0x3e6);

    if (!ego->isDockingToAsteroid() && !isMining && ego->isDockingToStream() == 0 &&
        !ego->isInDockingProcedure() && !ego->isDockingToDockingPoint() && ego->isInTurretMode() == 0) {
        Radar *targetRadar = static_cast<Radar *>(ego->field_0x14);
        if (targetRadar->lockedPlanetTarget != nullptr || targetRadar->lockedAsteroid != nullptr ||
            targetRadar->lockedStation != nullptr ||
            (targetRadar->lockedEnemy != nullptr && targetRadar->lockedEnemy->field_0x70 != 0 &&
             targetRadar->lockedEnemy->field_0x75 != 0)) {
            const int targetX = static_cast<int>(this->field_0x3e4) + this->field_0x3ea;
            const int targetY = static_cast<int>(this->field_0x3e6) + this->field_0x3ea;
            if (Globals::iPad != 0)
                canvas->DrawImage2D(static_cast<unsigned int>(this->targetContextOverlayImage),
                                    targetX, targetY, 0x11, 0x44);
            else
                canvas->DrawImage2D(static_cast<unsigned int>(this->targetContextOverlayImage),
                                    targetX, targetY);
        }
    }

    const int progressCenterX = Globals::w >> 1;
    int progressStackOffset = 0;

    if (this->dockTransferProgressActive != 0 && ego->isDockedToDockingPoint() &&
        ego->getHitpoints() >= 1) {
        const unsigned int gaugeMetricsImage = static_cast<unsigned int>(this->chargeProgressFillImage);
        const unsigned int dockFillImage = static_cast<unsigned int>(this->dockTransferFillImage);
        const int stackFillHeight = canvas->GetImage2DHeight(gaugeMetricsImage);
        const int textHeight = canvas->GetTextHeight(hud_font());
        const int transferred = ego->getDockTransferedAmount();
        const int total = ego->getDockTotalAmount();
        const bool reverse = this->dockTransferReverse != 0;
        const String *baseLabel = hud_game_text()->getText(reverse ? 3205 : 3204);
        String spacer(" ");

        float transferRate = 1.0f - static_cast<float>(transferred) / static_cast<float>(total);
        if (!reverse)
            transferRate = static_cast<float>(transferred) / static_cast<float>(total);
        progressStackOffset = static_cast<int>(static_cast<float>(textHeight) +
                                               static_cast<float>(stackFillHeight) * 2.5f);
        String label = *baseLabel + spacer;

        const int fillWidth = canvas->GetImage2DWidth(gaugeMetricsImage);
        const int fillHeight = canvas->GetImage2DHeight(gaugeMetricsImage);
        float fade = 1.0f;
        const float fadeRate = static_cast<float>(this->dockTransferFadeTimer + elapsed) / 1000.0f;
        if (fadeRate < 1.0f)
            fade = fadeRate;
        this->dockTransferFadeTimer += elapsed;
        canvas->SetColor(static_cast<unsigned int>(static_cast<int>(fade * 255.0f) - 256));

        const int panelY = 2 * static_cast<int>(this->field_0x3e2);
        canvas->DrawImage2D(static_cast<unsigned int>(this->progressPanelImage),
                            progressCenterX, panelY, 0x11, 0x14);
        canvas->DrawRegion2D(dockFillImage, 0, 0,
                             static_cast<int>(transferRate * static_cast<float>(fillWidth)), fillHeight,
                             0.0f, 0, 0, progressCenterX - fillWidth / 2,
                             hud_layout_i32(0x218) + panelY);

        const int labelY = static_cast<int>(static_cast<float>(panelY) +
                                            static_cast<float>(fillHeight) * 2.5f);
        const int labelWidth = canvas->GetTextWidth(hud_font(), label);
        canvas->DrawString(hud_font(), label, progressCenterX - labelWidth / 2, labelY, false);

        if (this->dockTransferShowMissionMarkers != 0 && Status::gStatus->getMission() != nullptr &&
            Status::gStatus->getMission()->getType() != 168) {
            canvas->DrawImage2D(
                static_cast<unsigned int>(this->dockTransferMissionMarkerImage),
                progressCenterX + canvas->GetTextWidth(hud_font(), label) / 2,
                labelY + canvas->GetTextHeight(hud_font()) / 2, 0x11, 0x41);
        }
        if (Status::gStatus->getMission() != nullptr && Status::gStatus->getMission()->getType() == 174) {
            canvas->DrawImage2D(
                static_cast<unsigned int>(this->dockTransferProductionMarkerImage),
                progressCenterX + canvas->GetTextWidth(hud_font(), label) / 2,
                labelY + canvas->GetTextHeight(hud_font()) / 2, 0x11, 0x41);
        }
    }

    {
        float rate;
        if (this->jumpDriveProgressActive != 0) {
            rate = ego->getDriveChargeRate();
        } else {
            if (this->cloakProgressActive == 0)
                goto mining_hint;
            rate = ego->getCloakRate();
        }

        float progress = 1.0f;
        if (rate * 1.05f < 1.0f) {
            if (this->jumpDriveProgressActive != 0)
                progress = ego->getDriveChargeRate() * 1.05f;
            else
                progress = ego->getCloakRate() * 1.05f;
        }

        int textId = 318;
        if (this->jumpDriveProgressActive == 0)
            textId = 317;
        String label = *hud_game_text()->getText(textId);
        const unsigned int fillImage = static_cast<unsigned int>(this->chargeProgressFillImage);
        const int halfWidth = static_cast<int>(static_cast<float>(canvas->GetImage2DWidth(fillImage)) * 0.5f);
        const int fillHeight = canvas->GetImage2DHeight(fillImage);
        const float fade = static_cast<float>(this->chargeProgressFadeTimer + elapsed) / 1000.0f;
        this->chargeProgressFadeTimer += elapsed;
        const int color = fade >= 1.0f ? -1 : static_cast<int>(fade * 255.0f) - 256;
        canvas->SetColor(static_cast<unsigned int>(color));

        const int panelY = progressStackOffset + 2 * static_cast<int>(this->field_0x3e2);
        canvas->DrawImage2D(static_cast<unsigned int>(this->progressPanelImage),
                            progressCenterX, panelY, 0x11, 0x14);
        canvas->DrawRegion2D(fillImage,
                             static_cast<int>(static_cast<float>(halfWidth) - progress * halfWidth), 0,
                             static_cast<int>(progress * halfWidth + progress * halfWidth), fillHeight,
                             0.0f, 0, 0,
                             static_cast<int>(static_cast<float>(progressCenterX) - progress * halfWidth),
                             panelY + hud_layout_i32(0x218));
        const int labelWidth = canvas->GetTextWidth(hud_font(), label);
        canvas->DrawString(hud_font(), label, progressCenterX - labelWidth / 2,
                           static_cast<int>(static_cast<float>(panelY) +
                                            static_cast<float>(fillHeight) * 2.5f),
                           false);
    }

mining_hint:
    if (Globals::hints[kMiningTutorialHintIndex] == 0 && !ego->isMining() &&
        Globals::status->getCurrentCampaignMission() == 2 && ego->levelScript->scriptTime >= 12001 &&
        !ego->isDockingToAsteroid() && !ego->isDockedToAsteroid()) {
        int pulseTimer = this->miningHintPulseTimer + elapsed;
        if (pulseTimer > 2000)
            pulseTimer = 0;
        this->miningHintPulseTimer = pulseTimer;
        int alpha = static_cast<int>((static_cast<float>(pulseTimer) / 1000.0f) * 255.0f);
        if (alpha > 255)
            alpha = 255 - alpha;
        hud_canvas()->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                               static_cast<unsigned char>(0xff), static_cast<unsigned char>(alpha));
        String label = *hud_game_text()->getText(618);
        const int labelWidth = hud_canvas()->GetTextWidth(hud_font(), label);
        hud_canvas()->DrawString(hud_font(), label, Globals::w / 2 - labelWidth / 2,
                                 hud_layout_i32(0x2c) + static_cast<int>(this->field_0x3e2), false);
        hud_canvas()->SetColor(static_cast<unsigned int>(0xffffffffu));
    }
    canvas->SetColor(initialColor);

}

int Hud::updateQueue(int dt) {
    int result = this->eventQueueTimer + dt;
    this->eventQueueTimer = result;
    if (result > 4000) {
        this->eventQueueTimer = 0;
        delete (*this->eventQueue)[0];
        (*this->eventQueue)[0] = 0;
        unsigned int i = 0;
        while (true) {
            if (i + 1 >= this->eventQueue->size())
                break;
            (*this->eventQueue)[i] = (*this->eventQueue)[i + 1];
            i = i + 1;
        }
        if ((*this->eventQueue)[1] == 0) {
            this->eventQueueDirty = 0;
        }
        result = 0;
        this->eventQueuePaused = result;
    } else if (result > 2000) {
        result = this->eventQueuePaused;
        if (result == 0) {
            result = 1;
            this->eventQueuePaused = result;
        }
    }
    return result;
}


void Hud::drawOrbitInformation() {
    if (Globals::status->inAlienOrbit()) return;

    static_cast<PaintCanvas *>(Globals::Canvas)->SetColor(0xffffffffu);
    const int x = static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DWidth(
                      static_cast<unsigned int>(this->factionLogoImage)) +
                  *reinterpret_cast<int *>(static_cast<char *>(Globals::layout) + 0x21c);
    if (Globals::status->getSystem()->hasNoOwner() == 0)
        static_cast<PaintCanvas *>(Globals::Canvas)->DrawImage2D(
            static_cast<unsigned int>(this->factionLogoImage), 3, 3);

    {
        PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
        const unsigned int font =
            static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font));
        String stationName = Globals::status->getStation()->getName();
        canvas->DrawString(
            font, stationName, x,
            *reinterpret_cast<int *>(static_cast<char *>(Globals::layout) + 0x220), false);
    }
    static_cast<PaintCanvas *>(Globals::Canvas)->SetColor(0x777777ffu);

    if (Globals::status->getCurrentCampaignMission() < 16) return;

    int securityLevel = Globals::status->getSystem()->getSecurityLevel();
    if (Globals::status->getSystem()->getIndex() == 26 && Globals::status->field_114 > 1)
        securityLevel = 3;

    {
        PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
        const unsigned int font =
            static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font));
        String systemLine =
            String(Globals::status->getSystem()->getName(), false) + String(" ") +
            *static_cast<GameText *>(Globals::gameText)->getText(137);
        canvas->DrawString(
            font, systemLine, x,
            *reinterpret_cast<int *>(static_cast<char *>(Globals::layout) + 0x224), false);
    }

    static_cast<PaintCanvas *>(Globals::Canvas)->SetColor(
        g_Hud_securityColors[securityLevel].r,
        g_Hud_securityColors[securityLevel].g,
        g_Hud_securityColors[securityLevel].b, 0xff);
    static_cast<PaintCanvas *>(Globals::Canvas)->DrawString(
        static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font)),
        *static_cast<GameText *>(Globals::gameText)->getText(securityLevel + 402), x,
        *reinterpret_cast<int *>(static_cast<char *>(Globals::layout) + 0x228), false);
}

unsigned int Hud::touchMove(unsigned int a, unsigned int b, void *key) {
    unsigned int i = 0;
    for (; i <= 0x18; i = i + 1) {
        if ((*this->keyArray)[i] == key && this->elementBits[i] == 0x20)
            goto found;
    }

    return touchBegin(a, (unsigned int) -1, key);
found:
    int dx = (int) a - (int) this->steeringCenterX;
    int dy = (int) b - (int) this->steeringCenterY;
    float f = (float) (dy * dy + dx * dx);
    float r = Globals::gGlobals->sqrt(f);
    int denom = this->analogStickRadius;
    int len = (int) r;
    if (denom < len) {
        short s = (short) (denom * dx / len);
        short base = this->steeringCenterY;
        this->steeringKnobX = s + this->steeringCenterX;
        s = (short) (denom * dy / len);
        this->steeringKnobY = s + base;
    } else {
        this->steeringKnobX = (short) a;
        this->steeringKnobY = (short) b;
    }
    return 0x20;
}


unsigned int Hud::touchedElement(unsigned int x, unsigned int y) {
    if (this->quickMenuOpen != 0) {
        Array<TouchButton *> *menu = this->menuButtons;
        if (menu != 0) {
            for (unsigned int i = 0; i < menu->size(); i++) {
                if ((*menu)[i]->OnTouchBegin((int) x, (int) y) != 0)
                    return *(unsigned int *) (*this->menuButtons)[i];
                menu = this->menuButtons;
            }
            return 0;
        }
    }

    if (Globals::iPad != 0) {
        int extent = this->touchHalfExtent;
        unsigned int origin = this->field_0x40c;
        if (origin <= y && origin + extent >= y) {
            origin = this->field_0x40a;
            if (origin <= x && origin + extent >= x)
                return 1;
        }
        if (this->hasBoostButton != 0) {
            origin = this->field_0x412;
            if (origin <= y && origin + extent >= y) {
                origin = this->field_0x410;
                if (origin <= x && origin + extent >= x)
                    return 2;
            }
        }
        origin = this->field_0x3fa;
        if (origin <= y && origin + extent >= y) {
            origin = this->field_0x3f8;
            if (origin <= x && origin + extent >= x)
                return 0x40;
        }
        origin = this->field_0x406;
        if (origin <= y && origin + extent >= y) {
            origin = this->field_0x404;
            if (origin <= x && origin + extent >= x)
                return 0x100;
        }
        int center = this->steeringCenterX;
        if (center - extent <= x && center + extent >= x) {
            center = this->steeringCenterY;
            int verticalExtent = this->touchHalfExtentSmall;
            if (center - verticalExtent <= y && center + verticalExtent >= y)
                return 0x20;
        }
        origin = this->field_0x3f2;
        if (origin <= x && origin + extent >= x &&
            (origin = this->field_0x3f4) <= y && origin + extent >= y) {
            this->field_0x470 = 1000;
            return 0x80;
        }
        center = this->field_0x3ec;
        if (center - (extent >> 1) <= x && center + extent >= x) {
            origin = this->field_0x3ee;
            if (origin <= y && origin + extent >= y)
                return 8;
        }
        int smallExtent = this->touchHalfExtentSmall;
        center = this->field_0x3e4;
        if (center - (smallExtent >> 1) <= x && center + (smallExtent >> 1) >= x) {
            center = this->field_0x3e6;
            smallExtent >>= 1;
            if (center - smallExtent <= y && center + smallExtent >= y)
                return 0x10;
        }
        if (this->quickMenuEmpty == 0) {
            origin = this->field_0x416;
            if (origin <= x && origin + this->field_0x41a >= x) {
                origin = this->field_0x418;
                if (origin <= y && origin + this->field_0x41c >= y)
                    return 4;
            }
        }
        origin = this->field_0x400;
        if (origin <= y && origin + extent >= y) {
            origin = this->field_0x3fe;
            if (origin <= x && origin + extent >= x)
                return 0x20000000;
        }
        if (this->hackingGameActive != 0) {
            origin = this->field_0x456;
            if (origin <= y && origin + extent >= y &&
                (origin = this->field_0x454) <= x && origin + extent >= x)
                return 0x200;
            origin = this->field_0x45a;
            if (origin <= y && origin + extent >= y &&
                (origin = this->field_0x458) <= x && origin + extent >= x)
                return 0x400;
            origin = this->field_0x460;
            if (origin <= y && origin + extent >= y &&
                (origin = this->field_0x45e) <= x && origin + extent >= x)
                return 0x800;
        }
        return 0;
    }

    int extent = this->touchHalfExtent;
    if (this->hackingGameActive != 0) {
        unsigned int origin = this->field_0x456;
        if (origin <= y && origin + extent >= y &&
            (origin = this->field_0x454) <= x && origin + extent >= x)
            return 0x200;
        origin = this->field_0x45a;
        if (origin <= y && origin + extent >= y &&
            (origin = this->field_0x458) <= x && origin + extent >= x)
            return 0x400;
        origin = this->field_0x460;
        if (origin <= y && origin + extent >= y &&
            (origin = this->field_0x45e) <= x && origin + extent >= x)
            return 0x800;
    }

    if (y < (unsigned int) (Globals::h >> 2)) {
        unsigned int origin = this->field_0x40c;
        if (origin <= y && origin + extent >= y) {
            origin = this->field_0x40a;
            if (origin <= x && origin + extent >= x)
                return 1;
        }
        return 0;
    }
    if (x < (unsigned int) (Globals::w >> 1)) {
        if (this->hasBoostButton != 0) {
            unsigned int origin = this->field_0x412;
            if (origin <= y && origin + extent >= y) {
                int center = this->field_0x410;
                if (center - extent <= x && center + extent >= x)
                    return 2;
            }
        }
        unsigned int origin = this->field_0x3fa;
        if (origin <= y && origin + extent >= y &&
            (origin = this->field_0x3f8) <= x && origin + extent >= x)
            return 0x40;
        origin = this->field_0x406;
        if (origin <= y && origin + extent >= y &&
            (origin = this->field_0x404) <= x && origin + extent >= x)
            return 0x100;
        int center = this->steeringCenterX;
        if (center - extent <= x && center + extent >= x) {
            center = this->steeringCenterY;
            int verticalExtent = this->touchHalfExtentSmall;
            if (center - verticalExtent <= y && center + verticalExtent >= y)
                return 0x20;
        }
        return 0;
    }

    unsigned int origin = this->field_0x3f2;
    if (origin <= x && origin + extent >= x) {
        origin = this->field_0x3f4;
        if (origin <= y && origin + extent >= y) {
            this->field_0x470 = 1000;
            return 0x80;
        }
    }
    int center = this->field_0x3ec;
    if (center - (extent >> 1) <= x && center + extent >= x) {
        origin = this->field_0x3ee;
        if (origin <= y && origin + extent >= y)
            return 8;
    }
    int smallExtent = this->touchHalfExtentSmall;
    origin = this->field_0x3e4;
    if (origin <= x && origin + smallExtent >= x) {
        origin = this->field_0x3e6;
        if (origin <= y && origin + smallExtent >= y)
            return 0x10;
    }
    if (this->quickMenuEmpty == 0) {
        origin = this->field_0x416;
        if (origin <= x && origin + this->field_0x41a >= x) {
            origin = this->field_0x418;
            if (origin <= y && origin + this->field_0x41c >= y)
                return 4;
        }
    }
    origin = this->field_0x400;
    if (origin <= y && origin + extent >= y) {
        origin = this->field_0x3fe;
        if (origin <= x && origin + extent >= x)
            return 0x20000000;
    }
    return 0;
}

Hud::Hud() {
    init();
}


void Hud::catchCargo(int itemId, int count, bool single, bool missionDelivery, bool extender,
                     bool slotMode, bool aggregate) {
    this->field_0x1d0 = 0;
    this->cargoFullFlag = single;

    if (missionDelivery) {
        this->field_0x1f4 = *static_cast<GameText *>(Globals::gameText)->getText(0x219);
        this->field_0x1f4 = Globals::status->replaceHash(
            String(this->field_0x1f4, false),
            String(*static_cast<GameText *>(Globals::gameText)->getText(
                       Globals::status->getMission()->getType() == 3 ? 0x56e : 0x56f),
                   false),
            String("#N"));
        this->field_0x1f4 = Globals::status->replaceHash(
            String(this->field_0x1f4, false), String(1), String("#Q"));

        void *itemStorage = ::operator new(sizeof(ListItem));
        String *str = new String(this->field_0x1f4, false);
        ListItem *item = new (itemStorage) ListItem(str);
        item->field_0x2c = itemId;
        addToEventQueue(item);
        return;
    }

    if (single) {
        this->field_0x1f4 = *static_cast<GameText *>(Globals::gameText)->getText(0x142);
        void *itemStorage = ::operator new(sizeof(ListItem));
        String *str = new String(this->field_0x1f4, false);
        ListItem *item = new (itemStorage) ListItem(str, true);
        return addToEventQueue(item);
    }

    if (count < 1) return;

    if (aggregate && this->eventQueueDirty != 0) {
        String previousLabel =
            String(String(this->cargoAggregateCount) + String("t "), false) +
            *static_cast<GameText *>(Globals::gameText)->getText(itemId + 0x4fa);
        int idx = sameHudEventAsBeforeAggregate(String(previousLabel, false));
        if (idx >= 0) {
            this->eventQueueTimer = 2000;
            this->cargoAggregateCount += count;
            *(*this->eventQueue)[idx]->name =
                String(String(this->cargoAggregateCount) + String("t "), false) +
                *static_cast<GameText *>(Globals::gameText)->getText(itemId + 0x4fa);
            return;
        }
    }

    this->cargoAggregateCount = count;
    this->field_0x1f4 =
        String(String(this->cargoAggregateCount) + String("t "), false) +
        *static_cast<GameText *>(Globals::gameText)->getText(itemId + 0x4fa);

    void *itemStorage = ::operator new(sizeof(ListItem));
    String *str = new String(this->field_0x1f4, false);
    ListItem *item = new (itemStorage) ListItem(str);
    item->field_0x2c = itemId;
    if (!slotMode || extender) item->field_0x30 = 2;
    if (slotMode) item->field_0x24 = 1;
    addToEventQueue(item);
}


void Hud::drawEventString(String text, bool rightAlign) {
    const unsigned int font = hud_font();
    PaintCanvas *canvas = hud_canvas();
    int x;
    if (this->eventTextWraps != 0) {
        if (rightAlign) {
            x = this->eventLineMarginAlt + 1;
        } else {
            x = Globals::w - 1 - this->eventLineMarginAlt - canvas->GetTextWidth(font, text);
        }
    } else {
        int offset;
        if (rightAlign) {
            offset = -3 - this->eventLineMargin;
        } else {
            offset = this->eventLineMargin + 3 - canvas->GetTextWidth(font, text);
        }
        x = offset + this->eventLineX;
    }
    canvas->DrawString(font, text, x, this->eventLineY - 1, false);
}

void Hud::setCurrentSecondaryWeapon(Item *item) {
    this->currentSecondaryWeapon = item;
    updateSecondaryWeaponString();
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
    if (item == nullptr) return;

    this->field_0x3b4 = *hud_game_text()->getText(item->getIndex() + 1274) +
                        String(" (") + String(this->currentSecondaryWeapon->getAmount()) + String(")");
    this->field_0x3c0 = (Globals::w >> 1) -
                       (hud_canvas()->GetTextWidth(hud_font(), this->field_0x3b4) >> 1);
}


void Hud::drawEventQueue() {
    const unsigned char targetVisible = Radar::drawTarget;
    const int targetY = Radar::drawTarget ? this->field_0x3e2 : 0;
    const int bannerBaseY = hud_layout_i32(0x1e4);
    const int rawAlpha = static_cast<int>(
            (static_cast<float>(this->eventQueueTimer) / 2000.0f) * 255.0f);
    const float bannerSlide = hud_layout_f32(0x1e0);
    unsigned char alpha = static_cast<unsigned char>(rawAlpha);
    if (rawAlpha > 255)
        alpha = static_cast<unsigned char>(-2 - rawAlpha);

    hud_canvas()->SetColor(0xff, 0xff, 0xff, alpha);
    float direction = -1.0f;
    if (!targetVisible)
        direction = -2.0f;
    const int textSlideY = static_cast<int>(direction * bannerSlide);
    const int bannerTargetY = Radar::drawTarget ? this->field_0x3e2 : 0;
    hud_canvas()->DrawImage2D(static_cast<unsigned int>(this->eventBannerImage),
                              this->field_0x3e0,
                              bannerTargetY - hud_layout_i32(0x1e4));

    ListItem *item = this->eventQueue->data()[1];
    if (item != nullptr) {
        if (item->buttonKind == 2)
            hud_canvas()->SetColor(0x00, 0xed, 0x00, alpha);
        else if (item->buttonKind == 1)
            hud_canvas()->SetColor(0xff, 0x2a, 0x00, alpha);
        else if (item->buttonKind == 3)
            hud_canvas()->SetColor(0xff, 0x80, 0x00, alpha);
        else
            hud_canvas()->SetColor(0xff, 0xff, 0xff, alpha);

        String *label = static_cast<String *>(item->name);
        const int textWidth = hud_canvas()->GetTextWidth(hud_font(), *label);
        hud_canvas()->DrawString(hud_font(), *label,
                                 (Globals::w >> 1) - textWidth / 2,
                                 textSlideY + bannerBaseY + targetY, false);
    }

    hud_canvas()->SetColor(0xffffffffu);
}

unsigned int Hud::touchBegin(unsigned int a, unsigned int b, void *key) {
    unsigned int element = touchedElement(a, b);
    if (element != 0) {
        void **keys = this->keyArray->data_;
        for (unsigned int i = 0; i < 0x19; ++i) {
            if (keys[i] == key) {
                int *bits = this->elementBits;
                unsigned int previous = static_cast<unsigned int>(bits[i]);
                unsigned int flags;
                if (element == previous)
                    flags = this->touchFlags;
                else
                    flags = this->touchFlags & ~previous;
                this->touchFlags = flags | element;
                bits[i] = static_cast<int>(element);
                return this->touchFlags;
            }
        }

        for (unsigned int i = 0; i <= 0x18; ++i) {
            if (keys[i] == nullptr) {
                keys[i] = key;
                this->elementBits[i] = static_cast<int>(element);
                this->touchFlags |= element;
                return this->touchFlags;
            }
        }
    } else {
        for (int i = 0; i != 25; ++i) {
            void **keys = this->keyArray->data_;
            if (keys[i] == key) {
                int *bits = this->elementBits;
                this->touchFlags &= ~static_cast<unsigned int>(bits[i]);
                bits[i] = 0;
                keys[i] = nullptr;
            }
        }
    }
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


void Hud::init() {
    hud_load_init_images(this);

    this->hitDirectionLeftTimer = 0;
    this->hitDirectionRightTimer = 0;
    this->hitDirectionTopTimer = 0;
    this->hitDirectionBottomTimer = 0;
    *reinterpret_cast<int *>(this->unknown_0x4a1) = 0;
    // Android copies Layout+0x12c..0x14b as two 128-bit chunks.
    typedef int HudLayoutWords __attribute__((vector_size(16)));
    HudLayoutWords *destination = reinterpret_cast<HudLayoutWords *>(&this->boostReadyTextX);
    const HudLayoutWords *source = reinterpret_cast<const HudLayoutWords *>(
        static_cast<char *>(Globals::layout) + 0x12c);
    destination[0] = source[0];
    destination[1] = source[1];
    hud_init_coordinates(this);

    this->boostFlashRemaining = 0;
    this->boostFlashPulse = 0;
    this->secondaryFlashRemaining = 0;
    this->secondaryFlashPulse = 0;
    this->autofireEnabled = 0;
    this->boostReadyLatched = 1;
    this->cloakReadyLatched = 1;
    this->quickMenuFlashRemaining = 0;
    this->quickMenuFlashPulse = 0;
    this->timeExtenderTimer = 0;
    this->miningHintPulseTimer = 0;
    this->fireForTutorial = 0;
    this->quickMenuOpen = 0;
    this->image_0x15c = -1;

    this->eventQueue = new Array<ListItem *>();
    ArraySetLength(0x14, *(this->eventQueue));

    this->eventLineX = Globals::w >> 1;
    this->eventLineY = Globals::h - this->radarBottomInset;
    Hud::RADAR_WIDTH = Globals::w - 28;
    Hud::RADAR_HEIGHT = Globals::h - 33 - this->radarBottomInset - this->field_0x4ec -
                        hud_layout_i32(0x1d8);
    this->field_0x1d0 = 10000;
    this->eventScrolls = 0;
    this->secondaryLabelX = Globals::w - 5;

    this->hasBoostButton = Globals::status->getShip()->getBoostDelay() > 0;
    this->hasShieldBar = Globals::status->getShip()->getMaxShieldHP() > 0;
    this->hasArmorRegen = Globals::status->getShip()->getMaxArmorHP() > 0;
    union {
        int integer;
        float real;
    } firePower = {Globals::status->getShip()->getFirePower()};
    this->hasAutofireUI = firePower.real > 0.0f;
    this->hasCloak = Globals::status->getShip()->hasCloak();

    this->eventQueueDirty = 0;
    this->unknown_0x234 = 0;
    this->cargoFullFlag = 0;
    this->unknown_0x236[0] = 0;
    this->visible = 1;
    this->jumpMapSelectedFlag = 0;
    this->eventQueueTimer = 0;
    this->eventQueuePaused = 0;
    this->shieldHitFlash = 0;
    this->hitFlashTimer = 0;
    this->currentSecondaryWeapon = nullptr;
    this->menuButtons = nullptr;
    this->menuOriginX = 0;
    this->menuOriginYBase = 0;
    this->field_0x276 = 0;
    this->dockTransferProgressActive = 0;
    this->fuelGaugeValue = 0;
    this->factionLogoImage = -1;
    this->field_0x0 = 0;
    this->field_0x280 = 1;
    this->field_0x281 = 0;
    this->timeExtenderTimer = -1;
    this->timeExtenderDuration = -1;
    this->cargoAggregateCount = 0;
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

    if (Globals::status->inAlienOrbit() == 0) {
        const int race = Globals::status->getSystem()->getRace();
        hud_create_image(static_cast<unsigned short>(g_Hud_factionLogoResourceIds[race]),
                         this->factionLogoImage);
    }

    this->previousCameraMode = -1;
    this->cameraModeLabelTimer = 0;
    {
        String emptyCameraMode("", false);
        this->cameraModeLabel = emptyCameraMode;
    }

    closeHudMenu();
    checkIfQuickMenuIsEmpty();
    releaseAllKeys();

    this->uintArray = nullptr;
    Globals::pause_x = Globals::w -
                       hud_canvas()->GetImage2DWidth(static_cast<unsigned>(this->pauseButtonPressedImage)) -
                       hud_layout_i32(0x194);
    Globals::pause_y = hud_layout_i32(0x198);
}

void Hud::drawPauseButton() {
    PaintCanvas::gCanvas->SetColor((unsigned) (-1));
    if ((this->touchFlagsLow & 1) != 0)
        return PaintCanvas::gCanvas->DrawImage2D(
            (unsigned int) this->pauseButtonPressedImage, this->field_0x40a, this->field_0x40c);
    return PaintCanvas::gCanvas->DrawImage2D(
        (unsigned int) this->pauseButtonImage, this->field_0x40a, this->field_0x40c);
}

void Hud::checkIfQuickMenuIsEmpty() {
    Ship *ship = Status::gStatus->getShip();
    Array<Item *> *equip = ship->getEquipment(1);
    this->equipmentArray = equip;

    unsigned char empty;
    if (equip != 0) {
        for (unsigned int i = 0; i < equip->size(); i++) {
            if ((*equip)[i] != 0) {
                empty = 0;
                goto update_string;
            }
        }
    }
    ship = Status::gStatus->getShip();
    if (ship->hasJumpDrive() != 0 || Status::gStatus->getWingmen() != 0) {
        empty = 0;
    } else {
        ship = Status::gStatus->getShip();
        empty = static_cast<unsigned char>(ship->hasCloak() ^ 1);
    }

update_string:
    this->quickMenuEmpty = empty;
    updateSecondaryWeaponString();
}

void Hud::drawMenu(int unused) {
    (void) unused;
    static_cast<Layout *>(Globals::layout)->drawMask();

    hud_canvas()->DrawImage2D(
        static_cast<unsigned>(this->quickMenuTopImage),
        this->field_0x3c4 + this->menuOriginX,
        this->menuOriginY + this->menuOriginYBase);
    hud_canvas()->DrawImage2D(
        static_cast<unsigned>(this->quickMenuHeaderImage),
        this->menuOriginX + this->field_0x3d4 + this->field_0x3dc / 2,
        this->menuOriginY + this->menuOriginYBase + this->menuRowHeight / 2 -
            hud_layout_i32(0x22c),
        0x11, 0x44);

    int rowY = this->menuRowHeight + this->menuOriginY + this->menuOriginYBase;
    Array<TouchButton *> *buttons = this->menuButtons;
    if (buttons != nullptr) {
        unsigned int count = buttons->size();
        if (count != 0) {
            for (unsigned int i = 0; i < count - 1; ++i) {
                hud_canvas()->DrawImage2D(
                    static_cast<unsigned>(this->quickMenuMiddleImage),
                    this->field_0x3c4 + this->menuOriginX, rowY);
                rowY += this->field_0x3d0;
                count = this->menuButtons->size();
            }
        }
    }
    hud_canvas()->DrawImage2D(
        static_cast<unsigned>(this->quickMenuBottomImage),
        this->field_0x3c4 + this->menuOriginX, rowY);

    buttons = this->menuButtons;
    if (buttons != nullptr) {
        unsigned int count = buttons->size();
        if (count != 0) {
            for (unsigned int i = 0; i < count; ++i) {
                (*buttons)[i]->draw();
                buttons = this->menuButtons;
                count = buttons->size();
            }
        }
    }

    if (this->quickMenuType != 0) return;

    Ship *ship = Globals::status->getShip();
    if (ship->hasCloak() == 0 && Globals::status->getShip()->hasJumpDrive() == 0) return;

    String cargoLabel = String("X ") + String(this->fuelGaugeValue);

    const int gaugeX = this->menuOriginX + this->field_0x3d4 + this->field_0x3dc / 2;
    const int gaugeY = rowY + hud_layout_i32(0x30) / 2 + hud_layout_i32(0x288);
    hud_canvas()->DrawImage2D(
        static_cast<unsigned>(this->fuelGaugeBarImage), gaugeX, gaugeY, 0x11, 0x14);
    hud_canvas()->DrawImage2D(
        static_cast<unsigned>(this->fuelGaugeIconImage), gaugeX - hud_layout_i32(0x230),
        hud_layout_i32(0x30) + gaugeY + hud_layout_i32(0x28c), 0x11, 0x12);

    hud_canvas()->DrawString(
        hud_font(), cargoLabel, hud_layout_i32(0x230) + gaugeX,
        gaugeY + hud_canvas()->GetImage2DHeight(static_cast<unsigned>(this->fuelGaugeBarImage)) / 2 -
            hud_canvas()->GetTextHeight(hud_font()) / 2 + hud_layout_i32(0x234),
        false);
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
    String &line = this->field_0x1e0;

    switch (eventId) {
        case 1:
            if (this->hasAutofireUI == 0) return;
            line = *hud_game_text()->getText(37) + String(" ") + *hud_game_text()->getText(38);
            break;
        case 2:
            if (this->hasAutofireUI == 0) return;
            line = *hud_game_text()->getText(37) + String(" ") + *hud_game_text()->getText(39);
            break;
        case 3:
            if (this->hasBoostButton == 0 || ego->readyToBoost() == 0) return;
            line = *hud_game_text()->getText(314);
            break;
        case 4:
            if (this->hasBoostButton == 0) return;
            line = *hud_game_text()->getText(315);
            break;
        case 5:
            line = *hud_game_text()->getText(571) + String(" ") + *hud_game_text()->getText(38);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 6:
            line = *hud_game_text()->getText(571) + String(" ") + *hud_game_text()->getText(39);
            Globals::sound->play(0x1d, nullptr, nullptr, 0.0f);
            break;
        case 7:
            line = *hud_game_text()->getText(553);
            break;
        case 8:
            line = *hud_game_text()->getText(539);
            break;
        case 9:
            line = *hud_game_text()->getText(540);
            break;
        case 10: {
            String stationName(Globals::status->getStation()->getName(), false);
            line = *hud_game_text()->getText(546) + String(": ") + stationName +
                   (Globals::status->getStation()->getIndex() == 101
                            ? String("", false)
                            : String(" ") + *hud_game_text()->getText(136));
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        }
        case 11:
            line = *hud_game_text()->getText(546) + String(": ") + *hud_game_text()->getText(550);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 12:
            line = *hud_game_text()->getText(546) + String(": ") + *hud_game_text()->getText(547);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 13:
            line = *hud_game_text()->getText(546) + String(": ") + *hud_game_text()->getText(548);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 14:
            line = *hud_game_text()->getText(546) + String(": ") + *hud_game_text()->getText(549);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 15:
            line = *hud_game_text()->getText(546) + String(": ") + *hud_game_text()->getText(545);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 16:
            line = *hud_game_text()->getText(307);
            break;
        case 17:
            line = *hud_game_text()->getText(308);
            break;
        case 18:
            line = *hud_game_text()->getText(309);
            break;
        case 19:
            line = this->strings_01c_100[19];
            break;
        case 20:
            line = *hud_game_text()->getText(541);
            break;
        case 21:
            line = *hud_game_text()->getText(525);
            break;
        case 22:
            line = *hud_game_text()->getText(542);
            break;
        case 23:
            line = *hud_game_text()->getText(543);
            break;
        case 24:
            line = *hud_game_text()->getText(544);
            break;

        case 25:
            this->chargeProgressFadeTimer = 0;
            this->jumpDriveProgressActive = 1;
            return;
        case 26:
            this->jumpDriveProgressActive = 0;
            return;
        case 27:
            line = *hud_game_text()->getText(322);
            break;
        case 28:
            this->chargeProgressFadeTimer = 0;
            this->cloakProgressActive = 1;
            return;
        case 29:
            this->cloakProgressActive = 0;
            return;
        case 30: {
            String amount = String("-") + String(arg) + String("t ");
            line = String(amount, false) + *hud_game_text()->getText(1396);
            clearQueue();
            break;
        }
        case 31:
            line = *hud_game_text()->getText(324);
            break;
        case 32:
            line = *hud_game_text()->getText(218) + String(" ") + *hud_game_text()->getText(38);
            break;
        case 33:
            line = *hud_game_text()->getText(218) + String(" ") + *hud_game_text()->getText(39);
            break;
        case 34:
            line = *hud_game_text()->getText(3199);
            break;

        case 0x23:
            this->dockTransferFadeTimer = 0;
            this->dockTransferShowMissionMarkers = 1;
            this->dockTransferProgressActive = 1;
            this->dockTransferReverse = 0;
            return;
        case 0x25:
            this->dockTransferFadeTimer = 0;
            this->dockTransferShowMissionMarkers = 1;
            this->dockTransferProgressActive = 1;
            this->dockTransferReverse = 1;
            return;
        case 0x27:
            this->dockTransferFadeTimer = 0;
            this->dockTransferShowMissionMarkers = 0;
            this->dockTransferProgressActive = 1;
            this->dockTransferReverse = 0;
            return;
        case 0x29:
            this->dockTransferFadeTimer = 0;
            this->dockTransferShowMissionMarkers = 0;
            this->dockTransferProgressActive = 1;
            this->dockTransferReverse = 1;
            break;
        case 0x24:
        case 0x26:
        case 0x28:
        case 0x2a:
            line = *hud_game_text()->getText(3200);
            this->dockTransferProgressActive = 0;
            break;
        case 43:
            line = *hud_game_text()->getText(3203);
            break;
        case 44:
            line = *hud_game_text()->getText(3201);
            break;
        case 45:
            line = *hud_game_text()->getText(3202);
            break;
        case 46:
            line = *hud_game_text()->getText(316);
            break;
        case 47: {
            String amount = String("-") + String(arg) + String("t ");
            line = String(amount, false) + *hud_game_text()->getText(1476);
            clearQueue();
            break;
        }

        default:
            break;
    }

    String probe(line, false);
    if (sameHudEventAsBefore(probe) != 0) return;

    const unsigned int idBit = static_cast<unsigned int>(eventId - 27);
    void *itemStorage = ::operator new(sizeof(ListItem));
    String *eventText = new String(line, false);
    ListItem *item;
    if (idBit < 0x15 && ((1u << idBit) & kHudImportantEventMask) != 0)
        item = new (itemStorage) ListItem(eventText, 1);
    else
        item = new (itemStorage) ListItem(eventText);
    addToEventQueue(item);

    PaintCanvas *canvas = hud_canvas();
    int w = canvas->GetTextWidth(hud_font(), line);
    int screenW = Globals::w;
    this->eventScrollTick = 0;
    this->eventScrolls = 1;
    this->eventTextWraps =
            (unsigned char) ((screenW / 2 - this->eventLineMargin) + this->eventLineMarginAlt * -2 < w);
}


void Hud::drawChallengeModeScore(int unused) {
    (void) unused;
    hud_canvas()->SetColor(0xffffffffu);
    const int frameWidth = static_cast<Sprite *>(this->digitSprite)->getFrameWidth();
    const int pad = static_cast<Layout *>(Globals::layout)->field_0x2c;
    const int frameHeight = static_cast<Sprite *>(this->digitSprite)->getFrameHeight();
    const int y = static_cast<Layout *>(Globals::layout)->field_0x2c;
    const int screenW = Globals::w;

    String score(Globals::status->challengeScore);
    const int scoreLength = static_cast<int>(score.size());
    if (scoreLength <= 6) {
        const int zeroCount = 7 - scoreLength;
        for (int i = 0; i < zeroCount; ++i)
            score = String("0") + score;
    }

    hud_canvas()->SetColor(0x77ccffffu);
    const int digitStep = frameWidth - pad;
    const int screenCenter = screenW / 2;
    const int sevenDigitHalfWidth = 7 * digitStep / 2;
    const int scoreStartX = screenCenter - sevenDigitHalfWidth;
    int x = scoreStartX;
    for (int i = 1; static_cast<unsigned int>(i - 1) < score.size(); ++i) {
        int frame;
        {
            String digit = score.SubString(i - 1, i);
            frame = digit.ValueOf();
        }
        static_cast<Sprite *>(this->digitSprite)->setFrame(frame);
        static_cast<Sprite *>(this->digitSprite)->setPosition(x, y);
        static_cast<Sprite *>(this->digitSprite)->draw(1.0f, 1.0f);
        x += digitStep;
    }

    if (Globals::status->challengeMultiplierTimer > 0 && Globals::status->challengeMultiplier >= 2) {
        hud_canvas()->SetColor(0xffffffffu);
        const int rowPad = static_cast<Layout *>(Globals::layout)->field_0x2c;
        const int timer = Globals::status->challengeMultiplierTimer;
        const int multiplierY = y + frameHeight + rowPad;

        if (timer <= 3000) {
            if (timer % 100 >= 50) {
                const int multiplier = Globals::status->challengeMultiplier;
                String bonus(static_cast<int>((static_cast<float>(multiplier) * 0.05f + 1.0f) *
                                              static_cast<float>(1000 * multiplier)));
                int bonusOffset = 0;
                const int bonusBaseY = frameHeight + multiplierY;
                for (int i = 1; static_cast<unsigned int>(i - 1) < bonus.size(); ++i) {
                    int frame;
                    {
                        String digit = bonus.SubString(i - 1, i);
                        frame = digit.ValueOf();
                    }
                    static_cast<Sprite *>(this->digitSprite)->setFrame(frame);
                    static_cast<Sprite *>(this->digitSprite)->setPosition(
                            Globals::w / 2 -
                                            ((static_cast<int>(bonus.size()) * digitStep) >> 1) +
                                            bonusOffset,
                            bonusBaseY + static_cast<Layout *>(Globals::layout)->field_0x2c);
                    bonusOffset += digitStep;
                    static_cast<Sprite *>(this->digitSprite)->draw(1.0f, 1.0f);
                }
            }
        }

        hud_canvas()->DrawImage2D(static_cast<unsigned int>(this->multiplierIconImage),
                                  rowPad + scoreStartX, multiplierY);
        const float growth = static_cast<float>(Globals::status->challengeMultiplierTimer - 7000) *
                             0.01f;
        float scale = growth + 1.0f;
        if (growth < 0.0f)
            scale = 1.0f;

        String multiplier(Globals::status->challengeMultiplier);
        x = screenCenter + rowPad - sevenDigitHalfWidth;
        for (int i = 1; static_cast<unsigned int>(i - 1) < multiplier.size(); ++i) {
            int frame;
            {
                String digit = multiplier.SubString(i - 1, i);
                frame = digit.ValueOf();
            }
            static_cast<Sprite *>(this->digitSprite)->setFrame(frame);
            static_cast<Sprite *>(this->digitSprite)->setPosition(
                    x + hud_canvas()->GetImage2DWidth(
                                static_cast<unsigned int>(this->multiplierIconImage)),
                    multiplierY);
            x += digitStep;
            static_cast<Sprite *>(this->digitSprite)->draw(scale, scale);
        }
    }

    hud_canvas()->SetColor(0xffffffffu);
}


void Hud::hudEventMedal(int medalId, int percent) {
    if (percent >= 100)
        percent = 100;
    this->field_0x1e0 = *hud_game_text()->getText(medalId + 0x5e3) + String(":") +
                        String(percent) + String("%");

    String probe(this->field_0x1e0, false);
    if (sameHudEventAsBefore(probe) != 0) return;

    void *itemStorage = ::operator new(sizeof(ListItem));
    String *str = new String(this->field_0x1e0, false);
    ListItem *item = new (itemStorage) ListItem(str, 3);
    addToEventQueue(item);

    int textWidth = hud_canvas()->GetTextWidth(hud_font(), this->field_0x1e0);
    this->eventScrollTick = 0;
    this->eventScrolls = 1;
    this->eventTextWraps = textWidth > Globals::w / 2 - this->eventLineMargin -
                                      2 * this->eventLineMarginAlt;
}


static inline __attribute__((always_inline)) TouchButton *hud_store_menu_button(
        Hud *self, TouchButton *button, unsigned int action) {
    button->field_0x0 = static_cast<int>(action);
    button->field_0x4 = 0;
    ArrayAdd<TouchButton *>(button, *self->menuButtons);
    return button;
}

static inline __attribute__((always_inline)) TouchButton *hud_add_menu_text_button(
        Hud *self, int textId, int y, unsigned int action) {
    auto *button = new TouchButton(*hud_game_text()->getText(textId), 0, self->field_0x3d4, y,
                                   self->field_0x3dc, 0x11, 4);
    return hud_store_menu_button(self, button, action);
}

static inline __attribute__((always_inline)) TouchButton *hud_add_menu_item_button(
        Hud *self, Item *item, int y, unsigned int action) {
    auto *button = new TouchButton(*hud_game_text()->getText(item->getIndex() + 1274), 0,
                                   self->field_0x3d4, y, self->field_0x3dc, 0x11, 4);
    return hud_store_menu_button(self, button, action);
}

static inline __attribute__((always_inline)) void hud_add_equipment_menu_button(
        Hud *self, unsigned int itemIndex, int y) {
    auto *button = new TouchButton(
            *hud_game_text()->getText((*self->equipmentArray)[itemIndex]->getIndex() + 1274) +
                    String(String(" (") + String((*self->equipmentArray)[itemIndex]->getAmount()) +
                                   String(")"),
                           false),
            0, self->field_0x3d4, y, self->field_0x3dc, 0x11, 4);
    const unsigned int action = itemIndex == 0 ? 0x2000
            : itemIndex == 1                       ? 0x4000
            : itemIndex == 2                       ? 0x8000
                                                   : 0x10000;
    hud_store_menu_button(self, button, action);
}

static inline __attribute__((always_inline)) void hud_add_station_menu_button(Hud *self, int y) {
    auto *button = new TouchButton(
            String(Globals::status->getStation()->getName(), false) +
                    (Globals::status->getStation()->getIndex() == 101
                             ? String("", false)
                             : String(" ") + *hud_game_text()->getText(136)),
            0, self->field_0x3d4, y, self->field_0x3dc, 0x11, 4);
    hud_store_menu_button(self, button, 0x800000);
}

static inline __attribute__((always_inline)) void hud_add_programmed_station_menu_button(
        Hud *self, int y) {
    auto *button = new TouchButton(
            *hud_game_text()->getText(546) + String(": ") +
                    String(static_cast<Station *>(Level::programmedStation)->getName(), false),
            0, self->field_0x3d4, y, self->field_0x3dc, 0x11, 4);
    hud_store_menu_button(self, button, 0x200000);
}

static inline __attribute__((always_inline)) void hud_add_docking_target_menu_button(
        Hud *self, Level *lvl, int targetIndex, int y) {
    auto *button = new TouchButton(
            reinterpret_cast<PlayerFixedObject *>(
                    static_cast<intptr_t>(lvl->getDockingTarget(targetIndex)))->getName(),
            0, self->field_0x3d4, y, self->field_0x3dc, 0x11, 4);
    hud_store_menu_button(self, button, 0x04000000u << targetIndex);
}

static inline __attribute__((always_inline)) void hud_compact_orbit_menu_for_phone(Hud *self) {
    if (Globals::iPad != 0 || self->menuButtons->size() < 5) return;

    const int rowGap = hud_layout_i32(0x30);
    for (unsigned int i = 0; i < self->menuButtons->size(); ++i) {
        TouchButton *button = (*self->menuButtons)[i];
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

    Array<TouchButton *> *menuButtons = new Array<TouchButton *>();
    this->quickMenuType = menuType;
    this->menuButtons = menuButtons;

    delete this->equipmentArray;
    this->equipmentArray = nullptr;
    this->equipmentArray = Globals::status->getShip()->getEquipment(1);
    updateSecondaryWeaponString();
    this->menuOriginX = 0;

    const int rowGap = hud_layout_i32(0x30);
    const int buttonHeight = hud_layout_i32(0x1dc);
    const int rowStep = rowGap + buttonHeight;
    int y = this->menuBaseY;

    if (Globals::iPad != 0) {
        GameSettings *settings = reinterpret_cast<GameSettings *>(Globals::options);
        float menuY;
        if (menuType == 3) {
            menuY = static_cast<float>(settings->steerAnchorX);
        } else {
            const float fireMenuOffset = Globals::iPadHD != 0
                    ? 112.5f
                    : (Globals::iPadLarge != 0 ? 160.0f : 80.0f);
            menuY = static_cast<float>(settings->fireAnchorX) - fireMenuOffset;
        }
        if (menuY >= 0.0f) {
            if (menuType == 3) {
                menuY = static_cast<float>(settings->steerAnchorX);
            } else {
                const float fireMenuOffset = Globals::iPadHD != 0
                        ? 112.5f
                        : (Globals::iPadLarge != 0 ? 160.0f : 80.0f);
                menuY = static_cast<float>(settings->fireAnchorX) - fireMenuOffset;
            }
        } else {
            menuY = 0.0f;
        }
        this->menuOriginY = static_cast<int>(menuY);
        y = this->menuRowHeight + this->menuOriginY - rowGap / 2 + 1;
        this->menuBaseY = y;
    }

    switch (menuType) {
        case 0: {
            if (this->equipmentArray != nullptr) {
                for (unsigned int i = 0; i < this->equipmentArray->size(); ++i) {
                    if ((*this->equipmentArray)[i] != nullptr) {
                        hud_add_menu_text_button(this, 266, y, 0x200);
                        y += rowStep;
                        break;
                    }
                }
            }
            if (Globals::status->getWingmen() != 0 && Globals::status->inSupernovaSystem() == 0 &&
                Globals::status->getCurrentCampaignMission() != 158) {
                hud_add_menu_text_button(this, 306, y, 0x400);
                y += rowStep;
            }
            if (Globals::status->getShip()->hasCloak()) {
                Item *cloak = Globals::status->getShip()->getFirstEquipmentOfSort(21);
                TouchButton *button = hud_add_menu_item_button(this, cloak, y, 0x800);
                button->setPressProgressHighlight(false);
                if (lvl->getPlayer()->isCloaked() || lvl->getPlayer()->isChargingCloak() ||
                    lvl->getPlayer()->isRechargingCloak()) {
                    if (lvl->getPlayer()->isRechargingCloak())
                        button->setPressProgress(lvl->getPlayer()->getCloakRechargeRate());
                    button->setHalfTransparent(true);
                }
                y += rowStep;
            }
            if (Globals::status->getShip()->hasJumpDrive() != 0) {
                TouchButton *button = hud_add_menu_text_button(this, 1359, y, 0x1000);
                if (lvl->getPlayer()->isChargingDrive() || lvl->getPlayer()->emergencySystemActive())
                    button->setHalfTransparent(true);
            }

            Item *cargo = Globals::status->getShip()->getCargo(122);
            this->fuelGaugeValue = cargo != nullptr ? cargo->getAmount() : 0;
            hud_create_image(0x4f5, this->quickMenuHeaderImage);
            break;
        }
        case 1: {
            if (this->equipmentArray != nullptr) {
                for (unsigned int i = 0; i < this->equipmentArray->size(); ++i) {
                    Item *item = (*this->equipmentArray)[i];
                    if (item == nullptr) continue;

                    hud_add_equipment_menu_button(this, i, y);
                    y += rowStep;
                }
            }
            hud_create_image(0x4f4, this->quickMenuHeaderImage);
            break;
        }
        case 2: {
            const int textIds[4] = {307, 308, 309, (Globals::status->field_f8 & 0xff) != 0 ? 311 : 310};
            const unsigned int actions[4] = {0x20000, 0x40000, 0x80000, 0x100000};
            for (unsigned int i = 0; i < 4; ++i) {
                hud_add_menu_text_button(this, textIds[i], y, actions[i]);
                y += rowStep;
            }
            hud_create_image(0x4f3, this->quickMenuHeaderImage);
            break;
        }
        case 3: {
            if (Globals::iPad != 0)
                this->menuOriginX = hud_layout_i32(0x28) - this->field_0x3c4;

            if (!Globals::status->inAlienOrbit()) {
                hud_add_menu_text_button(this, 549, y, 0x1000000);
                y += rowStep;

                if (!Globals::status->inEmptyOrbit()) {
                    hud_add_station_menu_button(this, y);
                    y += rowStep;
                }

                if (Globals::status->getSystem()->currentOrbitHasWarpGate()) {
                    hud_add_menu_text_button(this, 547, y, 0x400000);
                    y += rowStep;
                }

                if (lvl->getPlayer()->getRoute()) {
                    Route *route = reinterpret_cast<Route *>(
                            static_cast<intptr_t>(lvl->getPlayer()->getRoute()));
                    if ((route->getLastWaypoint()->state & 0xff) == 0) {
                        hud_add_menu_text_button(this, 573, y, 0x2000000);
                        y += rowStep;
                    }
                }

                Station *programmedStation = static_cast<Station *>(Level::programmedStation);
                if (programmedStation != nullptr) {
                    hud_add_programmed_station_menu_button(this, y);
                    y += rowStep;
                }
            }

            const int dockingTargetCount = lvl->getNumDockingTargets();
            for (int i = 0; i < dockingTargetCount; ++i) {
                auto *target = reinterpret_cast<PlayerFixedObject *>(
                        static_cast<intptr_t>(lvl->getDockingTarget(i)));
                {
                    String targetName = target->getName();
                    if (targetName.size() == 0) continue;
                }
                hud_add_docking_target_menu_button(this, lvl, i, y);
                y += rowStep;
            }

            hud_compact_orbit_menu_for_phone(this);
            hud_create_image(0x4f4, this->quickMenuHeaderImage);
            break;
        }
        default:
            break;
    }

    const unsigned int buttonCount = this->menuButtons->size();
    if (Globals::iPad != 0) {
        if (buttonCount != 0) {
            this->menuOriginYBase = static_cast<int>(4 - buttonCount) * rowStep;
            if (menuType == 3)
                this->menuOriginYBase -= rowGap;
        }
        for (unsigned int i = 0; i < buttonCount; ++i) {
            TouchButton *button = (*this->menuButtons)[i];
            button->translate(this->menuOriginX, this->menuOriginYBase);
            if (i <= 9) {
                const Vector xPosition = button->getPosition();
                Globals::sub_menu_buttons_x[i] = static_cast<int>(xPosition.x);
                const Vector yPosition = button->getPosition();
                Globals::sub_menu_buttons_y[i] = static_cast<int>(yPosition.y);
            }
        }
    } else {
        this->menuOriginYBase = buttonCount < 5 ? 0 : -rowGap;
        for (unsigned int i = 0; i < buttonCount; ++i) {
            if (i <= 9) {
                TouchButton *button = (*this->menuButtons)[i];
                const Vector xPosition = button->getPosition();
                Globals::sub_menu_buttons_x[i] = static_cast<int>(xPosition.x);
                const Vector yPosition = button->getPosition();
                Globals::sub_menu_buttons_y[i] = static_cast<int>(yPosition.y);
            }
        }
    }

    this->quickMenuOpen = 1;
}

Hud::~Hud() {
    delete this->equipmentArray;
    this->equipmentArray = 0;

    delete this->eventQueue;
    this->eventQueue = 0;

    if (this->menuButtons != 0) {
        ArrayReleaseClasses(*this->menuButtons);
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
