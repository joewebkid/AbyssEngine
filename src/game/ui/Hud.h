#ifndef GOF2_HUD_H
#define GOF2_HUD_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "ListItem.h"
#include "TouchButton.h"
#include "game/mission/Item.h"
#include "game/ship/PlayerEgo.h"


#include "game/ui/HudEventDisplay.h"

#include "game/ui/CargoBay.h"
class Radar;


class Item;
class Level;
class ListItem;
class PlayerEgo;
class TouchButton;

// Host-side mirror for Hud::init Image2D fields whose gameplay role has not
// yet been named. Each member keeps the original 32-bit Hud byte offset.
struct HudInitImageSlots {
    int image_0x004;
    int image_0x008;
    int image_0x2c8;
    int image_0x2cc;
    int image_0x2d0;
    int image_0x2e8;
    int image_0x2ec;
    int image_0x2f0;
    int image_0x2fc;
    int image_0x300;
    int image_0x304;
    int image_0x308;
    int image_0x314;
    int image_0x318;
    int image_0x31c;
    int image_0x320;
    int image_0x324;
    int image_0x334;
    int image_0x338;
    int image_0x33c;
    int image_0x340;
    int image_0x344;
    int image_0x34c;
    int image_0x350;
    int image_0x360;
    int image_0x364;
    int image_0x368;
    int image_0x36c;
    int image_0x370;
    int image_0x374;
    int image_0x378;
    int image_0x37c;
    int image_0x380;
    int image_0x384;
    int image_0x388;
    int image_0x38c;
    int image_0x390;
    int image_0x394;
    int image_0x398;
    int image_0x39c;
    int image_0x3a0;
    int image_0x3a4;
    int image_0x3a8;
    int image_0x3ac;
    int image_0x3b0;
    int image_0x4f4;
    int image_0x4f8;
    int image_0x4fc;
    int image_0x500;
    int image_0x504;
    int image_0x508;
    int image_0x50c;
    int image_0x510;
};




class Hud {
public:
    unsigned char field_0x0;
    unsigned char visible;
    unsigned char field_0x2;
    unsigned char field_0x3;
    Array<TouchButton *> *menuButtons;

    String field_0x1c;
    String field_0x28;
    String field_0x34;
    String field_0x40;
    String field_0x4c;
    String field_0x58;
    String field_0x64;
    String field_0x70;
    String field_0x7c;
    String field_0x88;
    String field_0x94;
    String field_0xa0;
    String field_0xac;
    String field_0xb8;
    String field_0xc4;
    String field_0xd0;
    String field_0xdc;
    String field_0xe8;
    String field_0xf4;
    String field_0x100;
    int eventLineX;
    int eventLineY;
    int factionLogoImage;
    int field_0x1d0;
    int eventScrollTick;
    unsigned char eventScrolls;
    String field_0x1e0;
    unsigned char letterbox;
    String field_0x1f4;
    String field_0x200;
    unsigned char hasBoostButton;
    unsigned char hasShieldBar;
    unsigned char hasArmorRegen;
    unsigned char hasAutofireUI;
    String field_0x228;
    unsigned char cargoFullFlag;
    Level *menuLevel;
    unsigned char shieldHitFlash;
    Item *currentSecondaryWeapon;
    Array<Item *> *equipmentArray;
    Array<ListItem *> *eventQueue;
    int eventQueueTimer;
    unsigned char eventQueueDirty;
    int eventQueuePaused;
    unsigned char jumpMapSelectedFlag;
    unsigned char field_0x275;
    unsigned short field_0x276;
    unsigned short weaponSelectState;
    unsigned char field_0x27a;
    unsigned char field_0x27b;
    int fuelGaugeValue;
    unsigned char field_0x280;
    unsigned char field_0x281;
    unsigned char quickMenuOpen;
    unsigned char quickMenuEmpty;
    unsigned int touchFlags;
    unsigned char autoTurretFlags;
    int field_0x288;
    Array<void *> *keyArray;
    int *elementBits;
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
    int lockBracketImage;
    int lockBracketLockedImage;
    int orbitMarkerActiveImage;
    int orbitMarkerIdleImage;
    int autoTurretOnImage;
    int autoTurretOffImage;
    int reticleImage;
    int missionBannerImage;
    int eventBannerImage;
    int quickMenuHeaderImage;
    int fuelGaugeIconImage;
    int fuelGaugeBarImage;
    String field_0x3b4;
    int secondaryLabelX;
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
    unsigned short field_0x41e;
    unsigned short field_0x420;
    unsigned short field_0x422;
    unsigned short field_0x424;
    unsigned short field_0x426;
    unsigned short lockBracketX;
    unsigned short lockBracketY;
    unsigned short reticleX;
    unsigned short reticleY;
    unsigned short field_0x42c;
    unsigned short field_0x42e;
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
    int field_0x468;
    int hitFlashTimer;
    int field_0x470;
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
    int secondaryLabelTimerSeed;
    int secondaryLabelTimer;
    String field_0x51c;
    unsigned char hackingGameActive;
    int cargoAggregateCount;
    Array<unsigned int> *uintArray;
    void *digitSprite;
    int multiplierIconImage;
    HudInitImageSlots initImageSlots;
    unsigned short iPadFireCoord_0x0c;
    unsigned short iPadFireCoord_0x0e;
    int iPadSteerAnchor;
    int iPadFireAnchor;

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

static_assert(__builtin_offsetof(HudEventDisplay, eventBannerDisplayScale) == 0x1e0,
              "HudEventDisplay::eventBannerDisplayScale must live at +0x1e0");
static_assert(__builtin_offsetof(HudEventDisplay, eventBannerDisplayBase) == 0x1e4,
              "HudEventDisplay::eventBannerDisplayBase must live at +0x1e4");
static_assert(__builtin_offsetof(CargoBay, cargoCurrent) == 0x54,
              "CargoBay::cargoCurrent must live at +0x54");
static_assert(__builtin_offsetof(CargoBay, cargoMax) == 0x58,
              "CargoBay::cargoMax must live at +0x58");
#endif
