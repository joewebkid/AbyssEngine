#ifndef GOF2_HUD_H
#define GOF2_HUD_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ListItem.h"
#include "TouchButton.h"
#include "game/mission/Item.h"
#include "game/ship/PlayerEgo.h"


#include "game/ui/HudEventDisplay.h"
#include "game/ui/HudArm32Layout.h"

#include "game/ui/CargoBay.h"
class Radar;


class Item;
class Level;
class ListItem;
class PlayerEgo;
class TouchButton;




class Hud {
public:
    unsigned char field_0x0;
    unsigned char visible;
    unsigned char field_0x2;
    unsigned char field_0x3;
    int iPadFireImage;
    int iPadFirePressedImage;
    unsigned short iPadFireCoord_0x0c;
    unsigned short iPadFireCoord_0x0e;
    int iPadSteerAnchor;
    int iPadFireAnchor;
    Array<TouchButton *> *menuButtons;

    // Android constructor/destructor walk this exact 20-element String array.
    String strings_01c_100[20];
    unsigned char unknown_0x10c[0x50];
    int image_0x15c;
    int eventLineX;
    int eventLineY;
    unsigned char unknown_0x168[0x5c];
    int factionLogoImage;
    unsigned char unknown_0x1c8[8];
    int field_0x1d0;
    unsigned char unknown_0x1d4[4];
    int eventScrollTick;
    unsigned char unknown_0x1dc[2];
    unsigned char eventScrolls;
    unsigned char unknown_0x1df;
    String field_0x1e0;
    unsigned char eventTextWraps;
    unsigned char unknown_0x1ed[7];
    String field_0x1f4;
    String field_0x200;
    unsigned char unknown_0x20c[0x0c];
    int secondaryLabelX;
    unsigned char unknown_0x21c;
    unsigned char hasCloak;
    unsigned char hasBoostButton;
    unsigned char hasShieldBar;
    unsigned char hasArmorRegen;
    unsigned char hasAutofireUI;
    unsigned char unknown_0x222[6];
    String field_0x228;
    unsigned char unknown_0x234;
    unsigned char cargoFullFlag;
    unsigned char unknown_0x236[2];
    // Android Hud+0x238: active quick-menu mode written by initHudMenu().
    int quickMenuType;
    unsigned char unknown_0x23c[8];
    unsigned char shieldHitFlash;
    unsigned char unknown_0x245[0x13];
    Item *currentSecondaryWeapon;
    Array<Item *> *equipmentArray;
    unsigned char unknown_0x260[4];
    Array<ListItem *> *eventQueue;
    int eventQueueTimer;
    unsigned char eventQueueDirty;
    unsigned char unknown_0x26d[3];
    int eventQueuePaused;
    unsigned char jumpMapSelectedFlag;
    unsigned char field_0x275;
    union {
        unsigned short field_0x276;
        struct {
            unsigned char cloakProgressActive;
            unsigned char jumpDriveProgressActive;
        };
    };
    union {
        unsigned short weaponSelectState;
        struct {
            unsigned char dockTransferProgressActive;
            unsigned char dockTransferReverse;
        };
    };
    union {
        unsigned char field_0x27a;
        unsigned char dockTransferShowMissionMarkers;
    };
    unsigned char field_0x27b;
    int fuelGaugeValue;
    unsigned char field_0x280;
    unsigned char field_0x281;
    unsigned char quickMenuOpen;
    unsigned char quickMenuEmpty;
    union {
        unsigned int touchFlags;
        struct {
            unsigned char touchFlagsLow;
            unsigned char touchFlagsByte1;
            unsigned char touchFlagsByte2;
            unsigned char touchFlagsByte3;
        };
    };
    int field_0x288;
    Array<void *> *keyArray;
    int *elementBits;
    int field_0x294;
    int quickMenuTopImage;
    int quickMenuBottomImage;
    int quickMenuMiddleImage;
    int shieldFrameImage;
    int shieldFrameHitImage;
    int shieldBarBgImage;
    int shieldBarFillImage;
    int armorFrameImage;
    int armorFrameLowImage;
    int armorBarBgImage;
    int armorRegenFillImage;
    int armorBarFillImage;
    int gammaBarFillImage;
    int gammaBarBgImage;
    int gammaFrameImage;
    int barDividerImage;
    int image_0x2d8;
    int image_0x2dc;
    int mainActionPressedImage;
    int mainActionIdleImage;
    int targetContextOverlayImage;
    int secondaryPressedImage;
    int secondaryIdleImage;
    int pauseButtonPressedImage;
    int pauseButtonImage;
    int boostPressedImage;
    int boostIdleImage;
    int steeringKnobPressedImage;
    int steeringKnobIdleImage;
    int dockActionPressedImage;
    int dockActionIdleImage;
    int autoTurretEnabledImage;
    int autoTurretDisabledImage;
    int steeringBaseImage;
    int missionTimerPanelImage;
    int cargoPanelImage;
    int image_0x328;
    int image_0x32c;
    int image_0x330;
    int passengerPanelImage;
    int productionCargoPanelImage;
    int productionRemainingPanelImage;
    int volatileCargoOverlayImage;
    int missionStatusPanelImage;
    int reticleImage;
    int quickMenuPressedImage;
    int quickMenuIdleImage;
    int eventBannerImage;
    int secondaryWeaponBannerImage;
    int quickMenuHeaderImage;
    int hitVerticalArmorImage;
    int hitHorizontalArmorImage;
    int hitVerticalShieldImage;
    int hitHorizontalShieldImage;
    int fuelGaugeIconImage;
    int fuelGaugeBarImage;
    int progressPanelImage;
    int chargeProgressFillImage;
    int dockTransferMissionMarkerImage;
    int dockTransferProductionMarkerImage;
    int progressPanelDuplicateImage;
    int dockTransferFillImage;
    int image_0x390;
    int image_0x394;
    int image_0x398;
    int image_0x39c;
    int image_0x3a0;
    int image_0x3a4;
    int image_0x3a8;
    int image_0x3ac;
    int image_0x3b0;
    String field_0x3b4;
    int field_0x3c0;
    int field_0x3c4;
    int menuOriginY;
    int menuRowHeight;
    int field_0x3d0;
    int field_0x3d4;
    int menuBaseY;
    int field_0x3dc;
    unsigned short field_0x3e0;
    unsigned short field_0x3e2;
    unsigned short field_0x3e4;
    unsigned short field_0x3e6;
    unsigned short field_0x3e8;
    unsigned short field_0x3ea;
    unsigned short field_0x3ec;
    unsigned short field_0x3ee;
    unsigned short field_0x3f0;
    unsigned short field_0x3f2;
    unsigned short field_0x3f4;
    unsigned short field_0x3f6;
    unsigned short field_0x3f8;
    unsigned short field_0x3fa;
    unsigned short field_0x3fc;
    unsigned short field_0x3fe;
    unsigned short field_0x400;
    unsigned short field_0x402;
    unsigned short field_0x404;
    unsigned short field_0x406;
    unsigned short field_0x408;
    unsigned short field_0x40a;
    unsigned short field_0x40c;
    unsigned short field_0x40e;
    unsigned short field_0x410;
    unsigned short field_0x412;
    unsigned short field_0x414;
    unsigned short field_0x416;
    unsigned short field_0x418;
    unsigned short field_0x41a;
    unsigned short field_0x41c;
    union {
        unsigned short field_0x41e;
        unsigned short steeringKnobX;
    };
    union {
        unsigned short field_0x420;
        unsigned short steeringKnobY;
    };
    unsigned short field_0x422;
    union {
        unsigned short field_0x424;
        unsigned short steeringCenterX;
    };
    union {
        unsigned short field_0x426;
        unsigned short steeringCenterY;
    };
    unsigned short field_0x428;
    unsigned short field_0x42a;
    union {
        unsigned short field_0x42c;
        unsigned short steeringBaseX;
    };
    union {
        unsigned short field_0x42e;
        unsigned short steeringBaseY;
    };
    unsigned short field_0x430;
    unsigned short field_0x432;
    unsigned short field_0x434;
    unsigned short field_0x436;
    unsigned short missionPanelX;
    unsigned short missionPanelY;
    unsigned short field_0x43c;
    unsigned short field_0x43e;
    unsigned short field_0x440;
    unsigned short field_0x442;
    unsigned short field_0x444;
    unsigned short field_0x446;
    unsigned short field_0x448;
    unsigned short field_0x44a;
    unsigned short field_0x44c;
    int field_0x450;
    unsigned short field_0x454;
    unsigned short field_0x456;
    unsigned short field_0x458;
    unsigned short field_0x45a;
    unsigned short field_0x45c;
    unsigned short field_0x45e;
    unsigned short field_0x460;
    int chargeProgressFadeTimer;
    union {
        int field_0x468;
        int dockTransferFadeTimer;
    };
    int hitFlashTimer;
    int field_0x470;
    unsigned char boostReadyLatched;
    unsigned char unknown_0x475;
    unsigned char cloakReadyLatched;
    unsigned char unknown_0x477;
    int field_0x478;
    int field_0x47c;
    int field_0x480;
    int boostFlashRemaining;
    int boostFlashPulse;
    int secondaryFlashRemaining;
    int secondaryFlashPulse;
    int unknown_0x494;
    int quickMenuFlashRemaining;
    int quickMenuFlashPulse;
    unsigned char autofireEnabled;
    unsigned char unknown_0x4a1[4];
    unsigned char fireForTutorial;
    unsigned char unknown_0x4a6[6];
    int hitDirectionLeftTimer;
    int hitDirectionRightTimer;
    int hitDirectionTopTimer;
    int hitDirectionBottomTimer;
    int timeExtenderTimer;
    int timeExtenderDuration;
    int miningHintPulseTimer;
    unsigned char messageActive;
    unsigned char unknown_0x4c9[0x0b];
    int menuOriginX;
    int menuOriginYBase;
    int touchHalfExtent;
    int touchHalfExtentSmall;
    int analogStickRadius;
    int eventLineMargin;
    int field_0x4ec;
    int eventLineMarginAlt;
    int cameraIdleImages[4];
    int cameraPressedImages[4];
    int previousCameraMode;
    int cameraModeLabelTimer;
    String cameraModeLabel;
    unsigned char hackingGameActive;
    unsigned char unknown_0x529[3];
    int cargoAggregateCount;
    Array<unsigned int> *uintArray;
    void *digitSprite;
    int multiplierIconImage;

    Hud();

    ~Hud();

    void addToEventQueue(ListItem *item);

    uint8_t cargoFull();

    void catchCargo(int itemId, int count, bool single, bool missionDelivery, bool extender, bool slotMode,
                    bool aggregate);

    Hud *checkIfQuickMenuIsEmpty();

    void clearQueue();

    void closeHudMenu();

    void draw(long long t0, long long t1, PlayerEgo *ego, bool letterbox, unsigned int x, unsigned int y);

    void drawChallengeModeScore(int unused);

    void drawCredits();

    void drawBigNumber(int x, int y, int value, bool flag);

    void drawEventQueue();

    void drawEventString(String text, bool rightAlign);

    void drawMenu(int unused);

    void drawOrbitInformation();

    void drawPauseButton();

    void enableFireForTutorial(bool value);

    unsigned int firePressed();

    float getAnalogX();

    float getAnalogY();

    void hudEvent(int eventId, PlayerEgo *ego, int arg);

    void hudEventMedal(int medalId, int percent);

    int hudAction(int action, Level *lvl, Radar *radar);

    int init();

    void initHudMenu(int menuType, Level *lvl);

    uint8_t isHackingGameActive();

    uint8_t jumpMapSelected();

    void playerHit();

    void releaseAllKeys();

    void resetAnalogStick();

    unsigned int sameHudEventAsBefore(String str);

    int sameHudEventAsBeforeAggregate(String str);

    void setAutofireEnabled(bool value);

    void setCurrentSecondaryWeapon(Item *item);

    void setHackingGameActive(bool value);

    void setJumpMapSelected(bool value);

    void setTimeExtender(bool p1, bool p2, bool p3, bool p4);

    void setVisible(bool value);

    unsigned int touchBegin(unsigned int a, unsigned int b, void *key);

    unsigned int touchEnd(unsigned int a, unsigned int b, void *key);

    unsigned int touchMove(unsigned int a, unsigned int b, void *key);

    unsigned int touchedElement(unsigned int x, unsigned int y);

    void updateQueue(int dt);

    void updateSecondaryWeaponString();

    bool drawTitleImage(bool visible);

    // Static data members present in the original binary (defined for symbol parity).
    static int RADAR_WIDTH;
    static int RADAR_HEIGHT;
    static int wingmanCommand;
};

#if UINTPTR_MAX == 0xffffffffu
static_assert(sizeof(String) == 0x0c, "Android String must be 12 bytes");
static_assert(__builtin_offsetof(Hud, iPadFireImage) == 0x004, "Hud::iPadFireImage @ +0x004");
static_assert(__builtin_offsetof(Hud, iPadFirePressedImage) == 0x008,
              "Hud::iPadFirePressedImage @ +0x008");
static_assert(__builtin_offsetof(Hud, iPadFireCoord_0x0c) == 0x00c,
              "Hud::iPadFireCoord_0x0c @ +0x00c");
static_assert(__builtin_offsetof(Hud, iPadSteerAnchor) == 0x010,
              "Hud::iPadSteerAnchor @ +0x010");
static_assert(__builtin_offsetof(Hud, iPadFireAnchor) == 0x014,
              "Hud::iPadFireAnchor @ +0x014");
static_assert(__builtin_offsetof(Hud, menuButtons) == 0x018, "Hud::menuButtons @ +0x018");
static_assert(__builtin_offsetof(Hud, strings_01c_100) == 0x01c,
              "Hud::strings_01c_100 @ +0x01c");
static_assert(__builtin_offsetof(Hud, strings_01c_100) + 19 * sizeof(String) == 0x100,
              "Hud final constructor String @ +0x100");
static_assert(__builtin_offsetof(Hud, unknown_0x10c) == 0x10c, "Hud prefix tail @ +0x10c");
static_assert(__builtin_offsetof(Hud, image_0x15c) == 0x15c, "Hud::image_0x15c @ +0x15c");
static_assert(__builtin_offsetof(Hud, eventLineX) == 0x160, "Hud::eventLineX @ +0x160");
static_assert(__builtin_offsetof(Hud, factionLogoImage) == 0x1c4,
              "Hud::factionLogoImage @ +0x1c4");
static_assert(__builtin_offsetof(Hud, field_0x1d0) == 0x1d0, "Hud::field_0x1d0 @ +0x1d0");
static_assert(__builtin_offsetof(Hud, eventScrollTick) == 0x1d8,
              "Hud::eventScrollTick @ +0x1d8");
static_assert(__builtin_offsetof(Hud, eventScrolls) == 0x1de, "Hud::eventScrolls @ +0x1de");
static_assert(__builtin_offsetof(Hud, field_0x1e0) == 0x1e0, "Hud::field_0x1e0 @ +0x1e0");
static_assert(__builtin_offsetof(Hud, eventTextWraps) == 0x1ec,
              "Hud::eventTextWraps @ +0x1ec");
static_assert(__builtin_offsetof(Hud, field_0x1f4) == 0x1f4, "Hud::field_0x1f4 @ +0x1f4");
static_assert(__builtin_offsetof(Hud, field_0x200) == 0x200, "Hud::field_0x200 @ +0x200");
static_assert(__builtin_offsetof(Hud, secondaryLabelX) == 0x218,
              "Hud::secondaryLabelX @ +0x218");
static_assert(__builtin_offsetof(Hud, hasCloak) == 0x21d, "Hud::hasCloak @ +0x21d");
static_assert(__builtin_offsetof(Hud, hasAutofireUI) == 0x221,
              "Hud::hasAutofireUI @ +0x221");
static_assert(__builtin_offsetof(Hud, field_0x228) == 0x228, "Hud::field_0x228 @ +0x228");
static_assert(__builtin_offsetof(Hud, cargoFullFlag) == 0x235,
              "Hud::cargoFullFlag @ +0x235");
static_assert(__builtin_offsetof(Hud, quickMenuType) == 0x238, "Hud::quickMenuType @ +0x238");
static_assert(__builtin_offsetof(Hud, shieldHitFlash) == 0x244,
              "Hud::shieldHitFlash @ +0x244");
static_assert(__builtin_offsetof(Hud, currentSecondaryWeapon) == 0x258,
              "Hud::currentSecondaryWeapon @ +0x258");
static_assert(__builtin_offsetof(Hud, equipmentArray) == 0x25c,
              "Hud::equipmentArray @ +0x25c");
static_assert(__builtin_offsetof(Hud, eventQueue) == 0x264, "Hud::eventQueue @ +0x264");
static_assert(__builtin_offsetof(Hud, eventQueueDirty) == 0x26c,
              "Hud::eventQueueDirty @ +0x26c");
static_assert(__builtin_offsetof(Hud, eventQueuePaused) == 0x270,
              "Hud::eventQueuePaused @ +0x270");
static_assert(__builtin_offsetof(Hud, jumpMapSelectedFlag) == 0x274,
              "Hud::jumpMapSelectedFlag @ +0x274");
static_assert(__builtin_offsetof(Hud, cloakProgressActive) == 0x276,
              "Hud::cloakProgressActive @ +0x276");
static_assert(__builtin_offsetof(Hud, dockTransferProgressActive) == 0x278,
              "Hud::dockTransferProgressActive @ +0x278");
static_assert(__builtin_offsetof(Hud, dockTransferShowMissionMarkers) == 0x27a,
              "Hud::dockTransferShowMissionMarkers @ +0x27a");
static_assert(__builtin_offsetof(Hud, fuelGaugeValue) == 0x27c, "Hud::fuelGaugeValue @ +0x27c");
static_assert(__builtin_offsetof(Hud, touchFlags) == 0x284, "Hud::touchFlags @ +0x284");
static_assert(__builtin_offsetof(Hud, touchFlagsLow) == 0x284,
              "Hud::touchFlagsLow @ +0x284");
static_assert(__builtin_offsetof(Hud, touchFlagsByte1) == 0x285,
              "Hud::touchFlagsByte1 @ +0x285");
static_assert(__builtin_offsetof(Hud, touchFlagsByte3) == 0x287,
              "Hud::touchFlagsByte3 @ +0x287");
static_assert(__builtin_offsetof(Hud, field_0x288) == 0x288, "Hud::field_0x288 @ +0x288");
static_assert(__builtin_offsetof(Hud, keyArray) == 0x28c, "Hud::keyArray @ +0x28c");
static_assert(__builtin_offsetof(Hud, elementBits) == 0x290, "Hud::elementBits @ +0x290");
static_assert(__builtin_offsetof(Hud, field_0x294) == 0x294, "Hud::field_0x294 @ +0x294");
static_assert(__builtin_offsetof(Hud, quickMenuTopImage) == 0x298,
              "Hud image span must begin at +0x298");
static_assert(__builtin_offsetof(Hud, gammaBarFillImage) == 0x2c8,
              "Hud::gammaBarFillImage @ +0x2c8");
static_assert(__builtin_offsetof(Hud, mainActionPressedImage) == 0x2e0,
              "Hud::mainActionPressedImage @ +0x2e0");
static_assert(__builtin_offsetof(Hud, pauseButtonPressedImage) == 0x2f4,
              "Hud::pauseButtonPressedImage @ +0x2f4");
static_assert(__builtin_offsetof(Hud, dockActionPressedImage) == 0x30c,
              "Hud::dockActionPressedImage @ +0x30c");
static_assert(__builtin_offsetof(Hud, autoTurretEnabledImage) == 0x314,
              "Hud::autoTurretEnabledImage @ +0x314");
static_assert(__builtin_offsetof(Hud, missionTimerPanelImage) == 0x320,
              "Hud::missionTimerPanelImage @ +0x320");
static_assert(__builtin_offsetof(Hud, cargoPanelImage) == 0x324,
              "Hud::cargoPanelImage @ +0x324");
static_assert(__builtin_offsetof(Hud, passengerPanelImage) == 0x334,
              "Hud::passengerPanelImage @ +0x334");
static_assert(__builtin_offsetof(Hud, productionCargoPanelImage) == 0x338,
              "Hud::productionCargoPanelImage @ +0x338");
static_assert(__builtin_offsetof(Hud, productionRemainingPanelImage) == 0x33c,
              "Hud::productionRemainingPanelImage @ +0x33c");
static_assert(__builtin_offsetof(Hud, volatileCargoOverlayImage) == 0x340,
              "Hud::volatileCargoOverlayImage @ +0x340");
static_assert(__builtin_offsetof(Hud, missionStatusPanelImage) == 0x344,
              "Hud::missionStatusPanelImage @ +0x344");
static_assert(__builtin_offsetof(Hud, reticleImage) == 0x348, "Hud::reticleImage @ +0x348");
static_assert(__builtin_offsetof(Hud, eventBannerImage) == 0x354,
              "Hud::eventBannerImage @ +0x354");
static_assert(__builtin_offsetof(Hud, quickMenuHeaderImage) == 0x35c,
              "Hud::quickMenuHeaderImage @ +0x35c");
static_assert(__builtin_offsetof(Hud, hitVerticalArmorImage) == 0x360,
              "Hud::hitVerticalArmorImage @ +0x360");
static_assert(__builtin_offsetof(Hud, hitHorizontalArmorImage) == 0x364,
              "Hud::hitHorizontalArmorImage @ +0x364");
static_assert(__builtin_offsetof(Hud, hitVerticalShieldImage) == 0x368,
              "Hud::hitVerticalShieldImage @ +0x368");
static_assert(__builtin_offsetof(Hud, hitHorizontalShieldImage) == 0x36c,
              "Hud::hitHorizontalShieldImage @ +0x36c");
static_assert(__builtin_offsetof(Hud, fuelGaugeIconImage) == 0x370,
              "Hud::fuelGaugeIconImage @ +0x370");
static_assert(__builtin_offsetof(Hud, dockTransferFillImage) == 0x38c,
              "Hud::dockTransferFillImage @ +0x38c");
static_assert(__builtin_offsetof(Hud, image_0x390) == 0x390,
              "Hud::image_0x390 @ +0x390");
static_assert(__builtin_offsetof(Hud, image_0x398) == 0x398,
              "Hud::image_0x398 @ +0x398");
static_assert(__builtin_offsetof(Hud, image_0x3a8) == 0x3a8,
              "Hud::image_0x3a8 @ +0x3a8");
static_assert(__builtin_offsetof(Hud, image_0x3b0) == 0x3b0, "Hud final image slot @ +0x3b0");
static_assert(__builtin_offsetof(Hud, field_0x3b4) == 0x3b4, "Hud::field_0x3b4 @ +0x3b4");
static_assert(__builtin_offsetof(Hud, field_0x3c0) == 0x3c0, "Hud::field_0x3c0 @ +0x3c0");
static_assert(__builtin_offsetof(Hud, field_0x3c4) == 0x3c4, "Hud::field_0x3c4 @ +0x3c4");
static_assert(__builtin_offsetof(Hud, field_0x3e0) == 0x3e0, "Hud coordinate span @ +0x3e0");
static_assert(__builtin_offsetof(Hud, field_0x3e8) == 0x3e8, "Hud::field_0x3e8 @ +0x3e8");
static_assert(__builtin_offsetof(Hud, field_0x428) == 0x428, "Hud::field_0x428 @ +0x428");
static_assert(__builtin_offsetof(Hud, field_0x432) == 0x432, "Hud::field_0x432 @ +0x432");
static_assert(__builtin_offsetof(Hud, missionPanelX) == 0x438,
              "Hud::missionPanelX @ +0x438");
static_assert(__builtin_offsetof(Hud, missionPanelY) == 0x43a,
              "Hud::missionPanelY @ +0x43a");
static_assert(__builtin_offsetof(Hud, field_0x450) == 0x450,
              "Hud::field_0x450 @ +0x450");
static_assert(__builtin_offsetof(Hud, field_0x454) == 0x454,
              "Hud::field_0x454 @ +0x454");
static_assert(__builtin_offsetof(Hud, field_0x460) == 0x460, "Hud::field_0x460 @ +0x460");
static_assert(__builtin_offsetof(Hud, chargeProgressFadeTimer) == 0x464,
              "Hud::chargeProgressFadeTimer @ +0x464");
static_assert(__builtin_offsetof(Hud, dockTransferFadeTimer) == 0x468,
              "Hud::dockTransferFadeTimer @ +0x468");
static_assert(__builtin_offsetof(Hud, hitFlashTimer) == 0x46c,
              "Hud::hitFlashTimer @ +0x46c");
static_assert(__builtin_offsetof(Hud, boostReadyLatched) == 0x474,
              "Hud::boostReadyLatched @ +0x474");
static_assert(__builtin_offsetof(Hud, cloakReadyLatched) == 0x476,
              "Hud::cloakReadyLatched @ +0x476");
static_assert(__builtin_offsetof(Hud, field_0x47c) == 0x47c,
              "Hud::field_0x47c @ +0x47c");
static_assert(__builtin_offsetof(Hud, boostFlashRemaining) == 0x484,
              "Hud::boostFlashRemaining @ +0x484");
static_assert(__builtin_offsetof(Hud, secondaryFlashRemaining) == 0x48c,
              "Hud::secondaryFlashRemaining @ +0x48c");
static_assert(__builtin_offsetof(Hud, quickMenuFlashRemaining) == 0x498,
              "Hud::quickMenuFlashRemaining @ +0x498");
static_assert(__builtin_offsetof(Hud, autofireEnabled) == 0x4a0,
              "Hud::autofireEnabled @ +0x4a0");
static_assert(__builtin_offsetof(Hud, fireForTutorial) == 0x4a5,
              "Hud::fireForTutorial @ +0x4a5");
static_assert(__builtin_offsetof(Hud, hitDirectionLeftTimer) == 0x4ac,
              "Hud::hitDirectionLeftTimer @ +0x4ac");
static_assert(__builtin_offsetof(Hud, hitDirectionRightTimer) == 0x4b0,
              "Hud::hitDirectionRightTimer @ +0x4b0");
static_assert(__builtin_offsetof(Hud, hitDirectionTopTimer) == 0x4b4,
              "Hud::hitDirectionTopTimer @ +0x4b4");
static_assert(__builtin_offsetof(Hud, hitDirectionBottomTimer) == 0x4b8,
              "Hud::hitDirectionBottomTimer @ +0x4b8");
static_assert(__builtin_offsetof(Hud, timeExtenderTimer) == 0x4bc,
              "Hud::timeExtenderTimer @ +0x4bc");
static_assert(__builtin_offsetof(Hud, timeExtenderDuration) == 0x4c0,
              "Hud::timeExtenderDuration @ +0x4c0");
static_assert(__builtin_offsetof(Hud, miningHintPulseTimer) == 0x4c4,
              "Hud::miningHintPulseTimer @ +0x4c4");
static_assert(__builtin_offsetof(Hud, messageActive) == 0x4c8,
              "Hud::messageActive @ +0x4c8");
static_assert(__builtin_offsetof(Hud, menuOriginX) == 0x4d4,
              "Hud::menuOriginX @ +0x4d4");
static_assert(__builtin_offsetof(Hud, eventLineMargin) == 0x4e8,
              "Hud::eventLineMargin @ +0x4e8");
static_assert(__builtin_offsetof(Hud, field_0x4ec) == 0x4ec,
              "Hud::field_0x4ec @ +0x4ec");
static_assert(__builtin_offsetof(Hud, eventLineMarginAlt) == 0x4f0,
              "Hud::eventLineMarginAlt @ +0x4f0");
static_assert(__builtin_offsetof(Hud, cameraIdleImages) == 0x4f4,
              "Hud::cameraIdleImages @ +0x4f4");
static_assert(__builtin_offsetof(Hud, cameraPressedImages) == 0x504,
              "Hud::cameraPressedImages @ +0x504");
static_assert(__builtin_offsetof(Hud, previousCameraMode) == 0x514,
              "Hud::previousCameraMode @ +0x514");
static_assert(__builtin_offsetof(Hud, cameraModeLabelTimer) == 0x518,
              "Hud::cameraModeLabelTimer @ +0x518");
static_assert(__builtin_offsetof(Hud, cameraModeLabel) == 0x51c,
              "Hud::cameraModeLabel @ +0x51c");
static_assert(__builtin_offsetof(Hud, hackingGameActive) == 0x528,
              "Hud::hackingGameActive @ +0x528");
static_assert(__builtin_offsetof(Hud, cargoAggregateCount) == 0x52c,
              "Hud::cargoAggregateCount @ +0x52c");
static_assert(__builtin_offsetof(Hud, uintArray) == 0x530, "Hud::uintArray @ +0x530");
static_assert(__builtin_offsetof(Hud, digitSprite) == 0x534, "Hud::digitSprite @ +0x534");
static_assert(__builtin_offsetof(Hud, multiplierIconImage) == 0x538,
              "Hud::multiplierIconImage @ +0x538");
static_assert(sizeof(Hud) == 0x53c, "Android Hud allocation is 0x53c bytes");
#endif

static_assert(__builtin_offsetof(HudEventDisplay, eventBannerDisplayScale) == 0x1e0,
              "HudEventDisplay::eventBannerDisplayScale must live at +0x1e0");
static_assert(__builtin_offsetof(HudEventDisplay, eventBannerDisplayBase) == 0x1e4,
              "HudEventDisplay::eventBannerDisplayBase must live at +0x1e4");
static_assert(__builtin_offsetof(CargoBay, cargoCurrent) == 0x54,
              "CargoBay::cargoCurrent must live at +0x54");
static_assert(__builtin_offsetof(CargoBay, cargoMax) == 0x58,
              "CargoBay::cargoMax must live at +0x58");
#endif
