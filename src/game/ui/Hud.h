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

// Compact host-side storage for Hud::init Image2D fields whose gameplay role
// has not yet been named. Member names record original Android source slots;
// this helper does not physically preserve those offsets. HudArm32Layout is
// the fixed-width ABI evidence model.
struct HudInitImageSlots {
    union {
        int image_0x2c8;
        int gammaBarFillImage;
    };
    union {
        int image_0x2cc;
        int gammaBarBgImage;
    };
    union {
        int image_0x2d0;
        int gammaFrameImage;
    };
    union {
        int image_0x2e8;
        int targetContextOverlayImage;
    };
    union {
        int image_0x2ec;
        int secondaryPressedImage;
    };
    union {
        int image_0x2f0;
        int secondaryIdleImage;
    };
    union {
        int image_0x2fc;
        int boostPressedImage;
    };
    union {
        int image_0x300;
        int boostIdleImage;
    };
    union {
        int image_0x304;
        int steeringKnobPressedImage;
    };
    union {
        int image_0x308;
        int steeringKnobIdleImage;
    };
    union {
        int image_0x314;
        int autoTurretEnabledImage;
    };
    union {
        int image_0x318;
        int autoTurretDisabledImage;
    };
    union {
        int image_0x31c;
        int steeringBaseImage;
    };
    union {
        int image_0x320;
        int missionTimerPanelImage;
    };
    union {
        int image_0x324;
        int cargoPanelImage;
    };
    union {
        int image_0x334;
        int passengerPanelImage;
    };
    union {
        int image_0x338;
        int productionCargoPanelImage;
    };
    union {
        int image_0x33c;
        int productionRemainingPanelImage;
    };
    union {
        int image_0x340;
        int volatileCargoOverlayImage;
    };
    union {
        int image_0x344;
        int missionStatusPanelImage;
    };
    union {
        int image_0x34c;
        int quickMenuPressedImage;
    };
    union {
        int image_0x350;
        int quickMenuIdleImage;
    };
    union {
        int image_0x360;
        int hitVerticalArmorImage;
    };
    union {
        int image_0x364;
        int hitHorizontalArmorImage;
    };
    union {
        int image_0x368;
        int hitVerticalShieldImage;
    };
    union {
        int image_0x36c;
        int hitHorizontalShieldImage;
    };
    int image_0x370;
    int image_0x374;
    union {
        int image_0x378;
        int progressPanelImage;
    };
    union {
        int image_0x37c;
        int chargeProgressFillImage;
    };
    union {
        int image_0x380;
        int dockTransferMissionMarkerImage;
    };
    union {
        int image_0x384;
        int dockTransferProductionMarkerImage;
    };
    int image_0x388;
    union {
        int image_0x38c;
        int dockTransferFillImage;
    };
    int image_0x390;
    int image_0x394;
    int image_0x398;
    int image_0x39c;
    int image_0x3a0;
    int image_0x3a4;
    int image_0x3a8;
    int image_0x3ac;
    int image_0x3b0;
    int cameraIdleImages[4];       // Source slots: Android Hud+0x4f4..+0x500
    int cameraPressedImages[4];    // Source slots: Android Hud+0x504..+0x510
};




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
    unsigned char letterbox;
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
    unsigned int touchFlags;
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
    int barDividerImage;
    int pauseButtonPressedImage;
    int pauseButtonImage;
    int mainActionIdleImage;
    int mainActionPressedImage;
    int dockActionPressedImage;
    int dockActionIdleImage;
    int autoTurretEnabledImage;
    int autoTurretDisabledImage;
    int reticleImage;
    int secondaryWeaponBannerImage;
    int eventBannerImage;
    int quickMenuHeaderImage;
    int fuelGaugeIconImage;
    int fuelGaugeBarImage;
    String field_0x3b4;
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
    union {
        unsigned short field_0x42c;
        unsigned short steeringBaseX;
    };
    union {
        unsigned short field_0x42e;
        unsigned short steeringBaseY;
    };
    unsigned short field_0x430;
    unsigned short field_0x434;
    unsigned short field_0x436;
    unsigned short field_0x438;
    unsigned short field_0x43a;
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
    unsigned char cloakReadyLatched;
    int boostFlashRemaining;
    int boostFlashPulse;
    int secondaryFlashRemaining;
    int secondaryFlashPulse;
    int quickMenuFlashRemaining;
    int quickMenuFlashPulse;
    unsigned char autofireEnabled;
    unsigned char fireForTutorial;
    int timeExtenderTimer;
    int timeExtenderDuration;
    unsigned char messageActive;
    int menuOriginX;
    int menuOriginYBase;
    int touchHalfExtent;
    int touchHalfExtentSmall;
    int analogStickRadius;
    int eventLineMargin;
    int eventLineMarginAlt;
    int previousCameraMode;
    int cameraModeLabelTimer;
    String cameraModeLabel;
    int hitDirectionLeftTimer;
    int hitDirectionRightTimer;
    int hitDirectionTopTimer;
    int hitDirectionBottomTimer;
    unsigned char hackingGameActive;
    int cargoAggregateCount;
    // Host mirror for native Hud+0x4c4 until the remaining tail ABI is rebuilt.
    int miningHintPulseTimer;
    Array<unsigned int> *uintArray;
    void *digitSprite;
    int multiplierIconImage;
    HudInitImageSlots initImageSlots;

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
static_assert(__builtin_offsetof(Hud, letterbox) == 0x1ec, "Hud::letterbox @ +0x1ec");
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
static_assert(__builtin_offsetof(Hud, field_0x288) == 0x288, "Hud::field_0x288 @ +0x288");
static_assert(__builtin_offsetof(Hud, keyArray) == 0x28c, "Hud::keyArray @ +0x28c");
static_assert(__builtin_offsetof(Hud, elementBits) == 0x290, "Hud::elementBits @ +0x290");
static_assert(__builtin_offsetof(Hud, field_0x294) == 0x294, "Hud::field_0x294 @ +0x294");
static_assert(__builtin_offsetof(Hud, quickMenuTopImage) == 0x298,
              "Hud image span must begin at +0x298");
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
