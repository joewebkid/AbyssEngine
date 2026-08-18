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

void Status_replaceHash(void *out, void *tmpl, void *a, void *b, void *c);

// Android Hud::hudEvent .rodata at 0x20377c. Bits are indexed from event 27.
static constexpr unsigned int kHudImportantEventMask = 0x100019;

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
hud_draw_volatile_cargo(Hud *self, PaintCanvas *canvas, PlayerEgo *ego) {
    if (ego->hasVolatileGoods() == 0) return;

    const unsigned int image = static_cast<unsigned int>(self->volatileCargoOverlayImage);
    const int width = canvas->GetImage2DWidth(image);
    const int height = canvas->GetImage2DHeight(image);
    float force = 1.0f;
    if (ego->getVolatileForce() <= 1.0f)
        force = ego->getVolatileForce();
    canvas->DrawRegion2D(image, 0, 0, static_cast<int>(force * static_cast<float>(width)), height,
                         0.0f, 0, 0, self->missionPanelX - hud_layout_i32(0x1ec), self->missionPanelY);
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
        {0x1f59, &self->gammaFrameImage},
        {0x1f5a, &self->gammaBarBgImage},
        {0x1f5b, &self->gammaBarFillImage},
        {0x4a9, &self->barDividerImage},
        {0x4bb, &self->quickMenuPressedImage},
        {0x4ba, &self->quickMenuIdleImage},
        {0x4b5, &self->mainActionPressedImage},
        {0x4b4, &self->mainActionIdleImage},
        {0x536, &self->targetContextOverlayImage},
        {0x4bd, &self->secondaryPressedImage},
        {0x4bc, &self->secondaryIdleImage},
        {0x4b9, &self->pauseButtonPressedImage},
        {0x4b8, &self->pauseButtonImage},
        {0x4b3, &self->boostPressedImage},
        {0x4b2, &self->boostIdleImage},
        {0x4b1, &self->dockActionPressedImage},
        {0x4b0, &self->dockActionIdleImage},
        {0x4b7, &self->steeringKnobPressedImage},
        {0x4b6, &self->steeringKnobIdleImage},
        {0x4c1, &self->steeringBaseImage},
        {0x4c5, &self->missionTimerPanelImage},
        {0x520, &self->cargoPanelImage},
        {0x4c3, &self->eventBannerImage},
        {0x4c2, &self->secondaryWeaponBannerImage},
        {0x4cf, &self->quickMenuTopImage},
        {0x4d1, &self->quickMenuMiddleImage},
        {0x4d0, &self->quickMenuBottomImage},
        {0x537, &self->fuelGaugeIconImage},
        {0x538, &self->fuelGaugeBarImage},
        {0x539, &self->chargeProgressFillImage},
        {0x53a, &self->progressPanelImage},
        {0x53a, &self->progressPanelDuplicateImage},
        {0x1f41, &self->dockTransferFillImage},
        {0x525, &self->hitVerticalArmorImage},
        {0x526, &self->hitHorizontalArmorImage},
        {0x52b, &self->hitVerticalShieldImage},
        {0x52c, &self->hitHorizontalShieldImage},
        {0x528, &self->cameraIdleImages[0]},
        {0x527, &self->cameraPressedImages[0]},
        {0x4e9, &self->cameraIdleImages[1]},
        {0x4ea, &self->cameraPressedImages[1]},
        {0x4be, &self->cameraIdleImages[2]},
        {0x4bf, &self->cameraPressedImages[2]},
        {0x52a, &self->cameraIdleImages[3]},
        {0x529, &self->cameraPressedImages[3]},
        {0x540, &self->image_0x390},
        {0x541, &self->image_0x394},
        {0x53f, &self->image_0x398},
        {0x542, &self->image_0x39c},
        {0x543, &self->image_0x3a0},
        {0x546, &self->autoTurretEnabledImage},
        {0x547, &self->autoTurretDisabledImage},
        {0x1f58, &self->image_0x3a4},
        {0x1f57, &self->image_0x3a8},
        {0x4b1, &self->image_0x3ac},
        {0x4b0, &self->image_0x3b0},
        {0x1f43, &self->passengerPanelImage},
        {0x1f42, &self->missionStatusPanelImage},
        {0x1f40, &self->dockTransferMissionMarkerImage},
        {0x1f61, &self->productionCargoPanelImage},
        {0x1f60, &self->productionRemainingPanelImage},
        {0x1f5f, &self->dockTransferProductionMarkerImage},
        {0x1f5c, &self->volatileCargoOverlayImage},
    };
    for (const HudImageInit &image : images)
        hud_create_image(canvas, image.resourceId, *image.slot);

    if (Globals::iPad != 0) {
        hud_create_image(canvas, 0x4c6, self->iPadFireImage);
        hud_create_image(canvas, 0x6aa, self->iPadFirePressedImage);
        self->reticleImage = self->iPadFireImage;
    } else {
        hud_create_image(canvas, 0x4c6, self->reticleImage);
    }

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
    const int steerAnchor = settings->steerAnchorX;
    const int fireAnchor = settings->fireAnchorX;

    globals->setCoordsSteer(steerAnchor,
                            canvas->GetImage2DWidth(static_cast<unsigned>(self->steeringBaseImage)),
                            canvas->GetImage2DWidth(static_cast<unsigned>(self->dockActionIdleImage)),
                            canvas->GetImage2DWidth(static_cast<unsigned>(self->boostIdleImage)),
                            self->field_0x3f8, self->field_0x3fa, self->field_0x42c, self->field_0x42e,
                            self->field_0x424, self->field_0x426, self->field_0x410, self->field_0x412,
                            self->field_0x404, self->field_0x406);

    globals->setCoordsFire(fireAnchor,
                           canvas->GetImage2DWidth(static_cast<unsigned>(self->iPadFireImage)),
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
    self->field_0x3f0 = static_cast<unsigned short>(width(self->secondaryPressedImage));
    self->field_0x3e4 = static_cast<unsigned short>(screenW - hud_layout_i32(0x154) - width(self->mainActionIdleImage));
    self->field_0x3e6 = static_cast<unsigned short>(screenH - hud_layout_i32(0x158) - width(self->mainActionIdleImage));
    self->field_0x3ec = static_cast<unsigned short>(screenW - hud_layout_i32(0x15c) - self->field_0x3f0);
    self->field_0x3ee = static_cast<unsigned short>(screenH - hud_layout_i32(0x160) - self->field_0x3f0);
    self->field_0x3ea = static_cast<unsigned short>(hud_layout_i32(0x164));
    self->field_0x3e0 = static_cast<unsigned short>((screenW - width(self->eventBannerImage)) / 2);
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
    const int hackingHalfWidth = width(self->image_0x3a4) / 2;
    self->field_0x45c = static_cast<unsigned short>(hackingHalfWidth * 2);
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
    const auto drawSteering = [this, canvas, ego](unsigned int finalColor) {
        const bool blocked = ego->isAutoPilot() != 0 || ego->isDockingToAsteroid() ||
                             ego->isDockingToDockingPoint() || this->hackingGameActive != 0 ||
                             (ego->isDockedToDockingPoint() && ego->isInTurretMode() == 0);
        const bool dimmed = Globals::touchSteeringEnabled == 0 ||
                            (blocked && (Globals::touchSteeringEnabled == 0 ||
                                         ego->isInTurretMode() == 0));
        if (dimmed)
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff), static_cast<unsigned char>(0x32));
        else
            canvas->SetColor(static_cast<unsigned>(0xffffffffu));

        canvas->DrawImage2D(static_cast<unsigned>(this->steeringBaseImage),
                            this->field_0x42c, this->field_0x42e);
        if (Globals::touchSteeringEnabled != 0 && (this->touchFlagsLow & 0x20u) != 0) {
            canvas->DrawImage2D(static_cast<unsigned>(this->steeringKnobPressedImage),
                                this->field_0x41e, this->field_0x420, 0x11, 0x44);
        } else {
            this->field_0x41e = this->field_0x424;
            this->field_0x420 = this->field_0x426;
            canvas->DrawImage2D(static_cast<unsigned>(this->steeringKnobIdleImage),
                                this->field_0x41e, this->field_0x420, 0x11, 0x44);
        }
        canvas->SetColor(finalColor);
    };

    if (!isMining && this->eventQueueDirty != 0 && this->hackingGameActive == 0) {
        updateQueue(elapsed);
        drawEventQueue();
    }

    if (Globals::iPad != 0) {
        GameSettings *settings = reinterpret_cast<GameSettings *>(Globals::options);
        if (this->iPadSteerAnchor != settings->steerAnchorX ||
            this->iPadFireAnchor != settings->fireAnchorX)
            hud_apply_ipad_control_coords(this, canvas);
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
        this->boostReadyLatched = 1;
        this->boostFlashRemaining = 2000;
        this->boostFlashPulse = 80;
        this->field_0x47c = this->secondaryFlashRemaining < 0
                                ? this->menuOriginX
                                : canvas->GetTextHeight(hud_font()) + this->menuOriginX;
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
        Player *player = static_cast<Player *>(ego->player);
        const float fillScale = 0.01f * static_cast<float>(this->field_0x446);
        const int dividerYOffset = hud_layout_i32(0x1e8);
        canvas->SetColor((unsigned) 0xffffffffu);

        unsigned short frameY = this->field_0x442;
        unsigned short fillY = this->field_0x44a;
        if (this->hasShieldBar != 0) {
            int shp = player->getShieldHP();
            int frame = (shp < 2 || this->shieldHitFlash == 0) ? this->shieldFrameImage : this->shieldFrameHitImage;
            canvas->DrawImage2D((unsigned) frame, this->field_0x43c, this->field_0x442);
            canvas->DrawImage2D((unsigned) this->barDividerImage, this->field_0x43e,
                                this->field_0x442 + dividerYOffset);
            canvas->DrawImage2D((unsigned) this->shieldBarBgImage, this->field_0x440, this->field_0x44a);
            int rate = player->getShieldDamageRate();
            int w = static_cast<int>(static_cast<float>(rate) * fillScale);
            canvas->DrawRegion2D((unsigned) this->shieldBarFillImage, 0, 0, w, this->field_0x44c,
                                 0.0f, 0, 0, this->field_0x440, this->field_0x44a);
            frameY = this->field_0x444;
            fillY = this->field_0x448;
        }

        int ahp = player->getArmorHP();
        int aframe = (ahp < 1) ? this->armorFrameLowImage : this->armorFrameImage;
        canvas->DrawImage2D((unsigned) aframe, this->field_0x43c, frameY);
        canvas->DrawImage2D((unsigned) this->barDividerImage, this->field_0x43e,
                            frameY + dividerYOffset);
        canvas->DrawImage2D((unsigned) this->armorBarBgImage, this->field_0x440, fillY);
        int hrate = ego->getHullDamageRate();
        int hw = static_cast<int>(static_cast<float>(hrate) * fillScale);
        canvas->DrawRegion2D((unsigned) this->armorBarFillImage, 0, 0, hw, this->field_0x44c,
                             0.0f, 0, 0, this->field_0x440, fillY);

        if (this->hasArmorRegen != 0) {
            int arate = player->getArmorDamageRate();
            int aw = static_cast<int>(static_cast<float>(arate) * fillScale);
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
                const int gammaFrameY = 2 * static_cast<int>(this->field_0x444) -
                                        static_cast<int>(this->field_0x442);
                const int gammaFillY = 2 * static_cast<int>(this->field_0x448) -
                                       static_cast<int>(this->field_0x44a);
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
        const bool secondaryPressed = (this->touchFlagsLow & 8u) != 0 ||
                                      (this->secondaryFlashRemaining > 0 && this->secondaryFlashPulse <= 0);
        const int secondaryImage = secondaryPressed ? this->secondaryPressedImage
                                                    : this->secondaryIdleImage;
        canvas->DrawImage2D(static_cast<unsigned>(secondaryImage), this->field_0x3ec, this->field_0x3ee);
        if (secondaryPressed && this->secondaryFlashRemaining > 0)
            this->secondaryFlashPulse = 80;
        drawSteering(initialColor);
        return;
    }

    // PlayerEgo+0x20 is a direction bitfield written by the collision/damage
    // path. Android Hud::draw turns each asserted side into a 300 ms pulse.
    {
        const int horizontalImage = ego->getShieldDamageRate() >= 1
                                        ? this->hitHorizontalShieldImage
                                        : this->hitHorizontalArmorImage;
        const int verticalImage = ego->getShieldDamageRate() >= 1
                                      ? this->hitVerticalShieldImage
                                      : this->hitVerticalArmorImage;
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
            canvas->DrawImage2D(static_cast<unsigned int>(horizontalImage),
                                (Globals::w >> 1) - radar->imageWidth, Globals::h >> 1,
                                canvas->GetImage2DWidth(static_cast<unsigned int>(horizontalImage)),
                                canvas->GetImage2DHeight(static_cast<unsigned int>(horizontalImage)),
                                0x11, 0x41, 1);
            this->hitDirectionLeftTimer -= elapsed;
        }
        if (this->hitDirectionRightTimer >= 1) {
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(255 * this->hitDirectionRightTimer / 300));
            canvas->DrawImage2D(static_cast<unsigned int>(horizontalImage),
                                (Globals::w >> 1) - radar->imageWidth, Globals::h >> 1, 0x12, 0x42);
            this->hitDirectionRightTimer -= elapsed;
        }
        if (this->hitDirectionTopTimer >= 1) {
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(255 * this->hitDirectionTopTimer / 300));
            canvas->DrawImage2D(static_cast<unsigned int>(verticalImage), Globals::w >> 1,
                                (Globals::h >> 1) - radar->imageHeight, 0x11, 0x14);
            this->hitDirectionTopTimer -= elapsed;
        }
        if (this->hitDirectionBottomTimer >= 1) {
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(255 * this->hitDirectionBottomTimer / 300));
            canvas->DrawImage2D(static_cast<unsigned int>(verticalImage), Globals::w >> 1,
                                (Globals::h >> 1) - radar->imageHeight,
                                canvas->GetImage2DWidth(static_cast<unsigned int>(verticalImage)),
                                canvas->GetImage2DHeight(static_cast<unsigned int>(verticalImage)),
                                0x21, 0x24, 2);
            this->hitDirectionBottomTimer -= elapsed;
        }
    }

    drawSteering(static_cast<unsigned>(0xffffffffu));

    // Cargo, passenger and timed-mission panel at Android Hud+0x438/+0x43a.
    // The mission timer is the second draw argument supplied by MGame, not wall
    // clock time.
    Status *status = Status::gStatus;
    const int panelX = this->missionPanelX;
    const int panelY = this->missionPanelY;
    Mission *mission = status->getMission();
    if (mission == nullptr || mission->getType() != 12) {
        if (t1 < 1 || status->getCurrentCampaignMission() == 42) {
            if (status->getMission() != nullptr && status->getMission()->getType() == 184) {
                bool usePassengerPanel;
                if (status->getCurrentCampaignMission() == 102 && status->inAlienOrbit() == 0 &&
                    status->getStation()->getIndex() == 113) {
                    usePassengerPanel = false;
                } else {
                    const bool useCargoPanel = status->getCurrentCampaignMission() == 139 &&
                                               status->inAlienOrbit() == 0 &&
                                               status->getStation()->getIndex() == 131;
                    usePassengerPanel = !useCargoPanel;
                }

                String passengerLabel = String(status->missionPassengerCount) + String(" / ") +
                                        String(status->getShip()->getMaxPassengers());
                if (usePassengerPanel) {
                    canvas->DrawImage2D(static_cast<unsigned int>(this->passengerPanelImage),
                                        panelX - hud_layout_i32(0x1f0), panelY);
                } else {
                    canvas->DrawImage2D(static_cast<unsigned int>(this->cargoPanelImage),
                                        panelX - hud_layout_i32(0x1ec), panelY);
                }
                hud_draw_volatile_cargo(this, canvas, ego);

                if (usePassengerPanel) {
                    canvas->DrawString(hud_font(), passengerLabel, panelX + hud_layout_i32(0x200),
                                       panelY + 5, false);
                } else {
                    String cargoLabel = String(status->getShip()->getCurrentLoad()) + String(" / ") +
                                        String(status->getShip()->getMaxLoad()) + String("t");
                    int shift = 0;
                    if (status->getShip()->getCurrentLoad() >= 101)
                        shift = -2 * hud_layout_i32(0x2c);
                    canvas->DrawString(hud_font(), cargoLabel,
                                       panelX + shift - hud_layout_i32(0x208), panelY + 5, false);
                }

                String statusLabel(status->getMission()->getStatusValue());
                const int passengerHeight = canvas->GetImage2DHeight(
                    static_cast<unsigned int>(this->passengerPanelImage));
                canvas->DrawImage2D(static_cast<unsigned int>(this->missionStatusPanelImage),
                                    panelX - hud_layout_i32(0x1f0),
                                    panelY + passengerHeight + hud_layout_i32(0x1f4));
                canvas->DrawString(hud_font(), statusLabel, panelX + hud_layout_i32(0x200),
                                   panelY + passengerHeight + hud_layout_i32(0x1f4) +
                                       2 * hud_layout_i32(0x1f8),
                                   false);
            } else if (status->getMission() != nullptr &&
                       status->getMission()->getType() == 174) {
                Item *productionCargo = status->getShip()->getCargo(
                    status->getMission()->getProductionGoodIndex());
                const int amount = productionCargo != nullptr ? productionCargo->getAmount() : 0;
                String productionLabel = amount + String(" / ") +
                                         String(amount + status->getShip()->getFreeSpace());
                canvas->DrawImage2D(static_cast<unsigned int>(this->productionCargoPanelImage),
                                    panelX - hud_layout_i32(0x1f0), panelY);
                hud_draw_volatile_cargo(this, canvas, ego);
                canvas->DrawString(hud_font(), productionLabel,
                                   panelX + hud_layout_i32(0x200), panelY + 4, false);

                String remainingLabel(status->getMission()->getProductionGoodAmount() -
                                      status->getMission()->getStatusValue());
                const int passengerHeight = canvas->GetImage2DHeight(
                    static_cast<unsigned int>(this->passengerPanelImage));
                canvas->DrawImage2D(static_cast<unsigned int>(this->productionRemainingPanelImage),
                                    panelX - hud_layout_i32(0x1f0),
                                    panelY + passengerHeight + hud_layout_i32(0x1f4));
                canvas->DrawString(hud_font(), remainingLabel, panelX + hud_layout_i32(0x200),
                                   panelY + passengerHeight + hud_layout_i32(0x1f4) +
                                       hud_layout_i32(0x1f8),
                                   false);
            } else {
                String cargoLabel = String(status->getShip()->getCurrentLoad()) + String(" / ") +
                                    String(status->getShip()->getMaxLoad()) + String("t");
                int shift = 0;
                if (status->getShip()->getCurrentLoad() >= 101)
                    shift = -2 * hud_layout_i32(0x2c);
                canvas->DrawImage2D(static_cast<unsigned int>(this->cargoPanelImage),
                                    panelX - hud_layout_i32(0x1ec), panelY);
                hud_draw_volatile_cargo(this, canvas, ego);
                canvas->DrawString(hud_font(), cargoLabel,
                                   panelX + shift - hud_layout_i32(0x208), panelY + 5, false);
            }
        } else {
            String timerLabel;
            static_cast<Globals *>(Globals::globals)->longToTimeString(t1, timerLabel);
            canvas->DrawImage2D(static_cast<unsigned int>(this->missionTimerPanelImage),
                                panelX, panelY);
            canvas->DrawString(hud_font(), timerLabel,
                               panelX + hud_layout_i32(0x204), panelY + 5, false);
        }
    } else {
        String label = String(ego->level->killCountB) + String(" : ") +
                       String(ego->level->killCountA);
        canvas->DrawImage2D(static_cast<unsigned int>(this->cargoPanelImage),
                            panelX - hud_layout_i32(0x1ec), panelY);
        hud_draw_volatile_cargo(this, canvas, ego);
        canvas->DrawString(hud_font(), label,
                           panelX + hud_layout_i32(0x1fc), panelY + 5, false);
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

        updateSecondaryWeaponString();
        if (Globals::mouseCursorActivated != 0)
            canvas->SetColor(static_cast<unsigned>(0xffffffffu));
        canvas->DrawImage2D(static_cast<unsigned>(this->secondaryWeaponBannerImage),
                            Globals::w >> 1, Globals::h, 0x11, 0x24);
        canvas->DrawString(hud_font(), this->field_0x3b4, this->secondaryLabelX,
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
        unsigned char alpha = 55;
        if (ego->boosting() == 0) {
            const float boostRate = ego->getBoostRate();
            alpha = boostRate >= 1.0f ? 0xff : static_cast<unsigned char>(55.0f + boostRate * 75.0f);
        }
        canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                         static_cast<unsigned char>(0xff), alpha);
        if (Globals::mouseCursorActivated != 0 || isMining || this->hackingGameActive != 0 ||
            ego->isDockedToDockingPoint()) {
            canvas->SetColor(static_cast<unsigned>(ego->getBoostRate() < 1.0f ? 0xffffff2fu : 0xffffff00u));
        }
        const bool pressed = (this->touchFlagsLow & 2u) != 0 ||
                             (this->boostFlashRemaining > 0 && this->boostFlashPulse <= 0);
        if (pressed && this->boostFlashRemaining > 0) {
            this->boostFlashPulse = 80;
            if (Globals::mouseCursorActivated != 0)
                canvas->SetColor(static_cast<unsigned>(0xffffffffu));
        }
        const int image = pressed ? this->boostPressedImage
                                  : this->boostIdleImage;
        canvas->DrawImage2D(static_cast<unsigned>(image), this->field_0x410, this->field_0x412);
    }

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
    GameText *progressText = hud_game_text();

    if (this->dockTransferProgressActive != 0 && ego->isDockedToDockingPoint() &&
        ego->getHitpoints() >= 1 && progressText != nullptr) {
        const unsigned int fillImage = static_cast<unsigned int>(this->dockTransferFillImage);
        const int fillWidth = canvas->GetImage2DWidth(fillImage);
        const int fillHeight = canvas->GetImage2DHeight(fillImage);
        const int textHeight = canvas->GetTextHeight(hud_font());
        const int transferred = ego->getDockTransferedAmount();
        const int total = ego->getDockTotalAmount();

        String label = *progressText->getText(this->dockTransferReverse != 0 ? 3205 : 3204);
        label += ' ';

        float transferRate = total != 0 ? static_cast<float>(transferred) / static_cast<float>(total) : 0.0f;
        if (this->dockTransferReverse != 0)
            transferRate = 1.0f - transferRate;

        float fade = static_cast<float>(this->dockTransferFadeTimer + elapsed) / 1000.0f;
        if (fade > 1.0f) fade = 1.0f;
        this->dockTransferFadeTimer += elapsed;
        canvas->SetColor(static_cast<unsigned int>(static_cast<int>(fade * 255.0f) - 256));

        const int panelY = 2 * static_cast<int>(this->field_0x3e2);
        canvas->DrawImage2D(static_cast<unsigned int>(this->progressPanelImage),
                            progressCenterX, panelY, 0x11, 0x14);
        canvas->DrawRegion2D(fillImage, 0, 0,
                             static_cast<int>(transferRate * static_cast<float>(fillWidth)), fillHeight,
                             0.0f, 0, 0, progressCenterX - fillWidth / 2,
                             hud_layout_i32(0x218) + panelY);

        const int labelY = static_cast<int>(static_cast<float>(panelY) +
                                            static_cast<float>(fillHeight) * 2.5f);
        const int labelWidth = canvas->GetTextWidth(hud_font(), label);
        canvas->DrawString(hud_font(), label, progressCenterX - labelWidth / 2, labelY, false);

        Mission *mission = Status::gStatus != nullptr ? Status::gStatus->getMission() : nullptr;
        if (this->dockTransferShowMissionMarkers != 0 && mission != nullptr && mission->getType() != 168) {
            canvas->DrawImage2D(
                static_cast<unsigned int>(this->dockTransferMissionMarkerImage),
                progressCenterX + labelWidth / 2, labelY + textHeight / 2, 0x11, 0x41);
        }
        if (mission != nullptr && mission->getType() == 174) {
            canvas->DrawImage2D(
                static_cast<unsigned int>(this->dockTransferProductionMarkerImage),
                progressCenterX + labelWidth / 2, labelY + textHeight / 2, 0x11, 0x41);
        }

        progressStackOffset = static_cast<int>(static_cast<float>(textHeight) +
                                               static_cast<float>(fillHeight) * 2.5f);
    }

    if ((this->jumpDriveProgressActive != 0 || this->cloakProgressActive != 0) && progressText != nullptr) {
        const bool jumpDrive = this->jumpDriveProgressActive != 0;
        float rate = jumpDrive ? ego->getDriveChargeRate() : ego->getCloakRate();
        float progress = 1.0f;
        if (rate * 1.05f < 1.0f)
            progress = (jumpDrive ? ego->getDriveChargeRate() : ego->getCloakRate()) * 1.05f;

        String label = *progressText->getText(jumpDrive ? 318 : 317);
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

    Status *progressStatus = Status::gStatus;
    LevelScript *levelScript = ego->levelScript;
    if (Globals::hints[kMiningTutorialHintIndex] == 0 && !isMining && progressStatus != nullptr &&
        progressStatus->getCurrentCampaignMission() == 2 && levelScript != nullptr &&
        !ego->isDockingToAsteroid() && !ego->isDockedToAsteroid()) {
        const unsigned long long scriptTime =
            static_cast<unsigned int>(levelScript->field_0x8) |
            (static_cast<unsigned long long>(static_cast<unsigned int>(levelScript->field_0xc)) << 32);
        if (scriptTime >= 12001 && progressText != nullptr) {
            this->miningHintPulseTimer += elapsed;
            if (this->miningHintPulseTimer > 2000)
                this->miningHintPulseTimer = 0;
            int alpha = static_cast<int>((static_cast<float>(this->miningHintPulseTimer) / 1000.0f) * 255.0f);
            if (alpha > 255)
                alpha = -1 - alpha;
            canvas->SetColor(static_cast<unsigned char>(0xff), static_cast<unsigned char>(0xff),
                             static_cast<unsigned char>(0xff), static_cast<unsigned char>(alpha));
            String label = *progressText->getText(618);
            const int labelWidth = canvas->GetTextWidth(hud_font(), label);
            canvas->DrawString(hud_font(), label, progressCenterX - labelWidth / 2,
                               hud_layout_i32(0x2c) + static_cast<int>(this->field_0x3e2), false);
        }
    }
    canvas->SetColor(initialColor);

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
        if (cspan(this->steeringCenterX, w, x) && cspan(this->steeringCenterY, w2, y)) return 0x20;
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
        if (cspan(this->steeringCenterX, w, x) && cspan(this->steeringCenterY, w2, y)) return 0x20;
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
    if (this->eventTextWraps == 0) {
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
    this->secondaryWeaponBannerImage = -1;
    this->eventBannerImage = -1;
    this->fuelGaugeIconImage = -1;
    this->fuelGaugeBarImage = -1;

    hud_load_init_images(this);
    hud_init_coordinates(this);

    this->visible = 1;
    this->eventTextWraps = 0;
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
    this->field_0x288 = 0;
    this->field_0x1d0 = 10000;
    this->cargoFullFlag = 0;
    this->shieldHitFlash = 0;
    this->hitFlashTimer = 0;
    this->field_0x470 = 0;
    this->chargeProgressFadeTimer = 0;
    this->timeExtenderTimer = 0;
    this->timeExtenderDuration = 0;
    this->cargoAggregateCount = 0;
    this->field_0x468 = 0;
    this->miningHintPulseTimer = 0;

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

    this->hasCloak = 0;
    this->hasBoostButton = 0;
    this->hasShieldBar = 0;
    this->hasArmorRegen = 0;
    this->hasAutofireUI = 0;
    if (Status::gStatus != nullptr) {
        Ship *ship = Status::gStatus->getShip();
        if (ship != nullptr) {
            this->hasCloak = ship->hasCloak();
            this->hasBoostButton = ship->getBoostDelay() > 0;
            this->hasShieldBar = ship->getMaxShieldHP() > 0;
            this->hasArmorRegen = ship->getMaxArmorHP() > 0;
            this->hasAutofireUI = ship->getFirePower() > 0.0f;
        }
    }

    this->boostReadyLatched = 1;
    this->cloakReadyLatched = 1;
    this->boostFlashRemaining = 0;
    this->boostFlashPulse = 0;
    this->secondaryFlashRemaining = 0;
    this->secondaryFlashPulse = 0;
    this->quickMenuFlashRemaining = 0;
    this->quickMenuFlashPulse = 0;
    this->previousCameraMode = -1;
    this->cameraModeLabelTimer = 0;
    this->cameraModeLabel = String("");
    this->hitDirectionLeftTimer = 0;
    this->hitDirectionRightTimer = 0;
    this->hitDirectionTopTimer = 0;
    this->hitDirectionBottomTimer = 0;

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
    if ((this->touchFlagsLow & 1) != 0)
        return PaintCanvas::gCanvas->DrawImage2D(
            (unsigned int) this->pauseButtonPressedImage, this->field_0x40a, this->field_0x40c);
    return PaintCanvas::gCanvas->DrawImage2D(
        (unsigned int) this->pauseButtonImage, this->field_0x40a, this->field_0x40c);
}

Hud *Hud::checkIfQuickMenuIsEmpty() {
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
    GameText *gameText = hud_game_text();
    String &line = this->field_0x1e0;

    switch (eventId) {
        case 1:
            if (this->hasAutofireUI == 0) return;
            line = *gameText->getText(37) + String(" ") + *gameText->getText(38);
            break;
        case 2:
            if (this->hasAutofireUI == 0) return;
            line = *gameText->getText(37) + String(" ") + *gameText->getText(39);
            break;
        case 3:
            if (this->hasBoostButton == 0 || ego->readyToBoost() == 0) return;
            line = *gameText->getText(314);
            break;
        case 4:
            if (this->hasBoostButton == 0) return;
            line = *gameText->getText(315);
            break;
        case 5:
            line = *gameText->getText(571) + String(" ") + *gameText->getText(38);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 6:
            line = *gameText->getText(571) + String(" ") + *gameText->getText(39);
            Globals::sound->play(0x1d, nullptr, nullptr, 0.0f);
            break;
        case 7:
            line = *gameText->getText(553);
            break;
        case 8:
            line = *gameText->getText(539);
            break;
        case 9:
            line = *gameText->getText(540);
            break;
        case 10: {
            Station *station = Status::gStatus->getStation();
            String suffix;
            if (station->getIndex() != 101)
                suffix = String(" ") + *gameText->getText(136);
            line = *gameText->getText(546) + String(": ") + station->getName() + suffix;
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        }
        case 11:
            line = *gameText->getText(546) + String(": ") + *gameText->getText(550);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 12:
            line = *gameText->getText(546) + String(": ") + *gameText->getText(547);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 13:
            line = *gameText->getText(546) + String(": ") + *gameText->getText(548);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 14:
            line = *gameText->getText(546) + String(": ") + *gameText->getText(549);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 15:
            line = *gameText->getText(546) + String(": ") + *gameText->getText(545);
            Globals::sound->play(0x1c, nullptr, nullptr, 0.0f);
            break;
        case 16:
            line = *gameText->getText(307);
            break;
        case 17:
            line = *gameText->getText(308);
            break;
        case 18:
            line = *gameText->getText(309);
            break;
        case 19:
            line = this->strings_01c_100[19];
            break;
        case 20:
            line = *gameText->getText(541);
            break;
        case 21:
            line = *gameText->getText(525);
            break;
        case 22:
            line = *gameText->getText(542);
            break;
        case 23:
            line = *gameText->getText(543);
            break;
        case 24:
            line = *gameText->getText(544);
            break;

        case 25:
            this->chargeProgressFadeTimer = 0;
            this->jumpDriveProgressActive = 1;
            return;
        case 26:
            this->jumpDriveProgressActive = 0;
            return;
        case 28:
            this->chargeProgressFadeTimer = 0;
            this->cloakProgressActive = 1;
            return;
        case 29:
            this->cloakProgressActive = 0;
            return;
        case 30: {
            String amount = String("-") + String(arg) + String("t ");
            line = String(amount, false) + *gameText->getText(1396);
            clearQueue();
            break;
        }
        case 31:
            line = *gameText->getText(324);
            break;
        case 32:
            line = *gameText->getText(218) + String(" ") + *gameText->getText(38);
            break;
        case 33:
            line = *gameText->getText(218) + String(" ") + *gameText->getText(39);
            break;
        case 34:
            line = *gameText->getText(3199);
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
            line = *gameText->getText(3200);
            this->dockTransferProgressActive = 0;
            break;
        case 43:
            line = *gameText->getText(3203);
            break;
        case 44:
            line = *gameText->getText(3201);
            break;
        case 45:
            line = *gameText->getText(3202);
            break;
        case 46:
            line = *gameText->getText(316);
            break;
        case 47: {
            String amount = String("-") + String(arg) + String("t ");
            line = String(amount, false) + *gameText->getText(1476);
            clearQueue();
            break;
        }

        default:
            break;
    }

    if (sameHudEventAsBefore(line) != 0) return;

    const unsigned int idBit = static_cast<unsigned int>(eventId - 27);
    ListItem *item;
    if (idBit < 0x15 && ((1u << idBit) & kHudImportantEventMask) != 0)
        item = new ListItem(new String(line), 1);
    else
        item = new ListItem(new String(line));
    addToEventQueue(item);

    PaintCanvas *canvas = hud_canvas();
    int w = canvas->GetTextWidth(hud_font(), line);
    int screenW = Globals::w;
    this->eventScrollTick = 0;
    this->eventScrolls = 1;
    this->eventTextWraps =
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
    this->eventTextWraps =
        ((screenW / 2 - this->eventLineMargin) + this->eventLineMarginAlt * -2 < w) ? 1 : 0;
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
