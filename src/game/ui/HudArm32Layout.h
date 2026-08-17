#ifndef GOF2_HUD_ARM32_LAYOUT_H
#define GOF2_HUD_ARM32_LAYOUT_H

#include <cstddef>
#include <cstdint>

// Fixed-width evidence model for the Android ARMv7 Hud object. Pointer fields
// are stored as 32-bit handles so this contract can also be checked by a
// 64-bit host compiler. It is not instantiated as a runtime Hud.
struct HudArm32StringSlot {
    std::uint32_t vtable;
    std::uint32_t data;
    std::int32_t length;
};

enum class HudArm32ImageOffset : std::uint16_t {
    QuickMenuTop = 0x298,
    QuickMenuBottom = 0x29c,
    QuickMenuMiddle = 0x2a0,
    ShieldFrame = 0x2a4,
    ShieldFrameHit = 0x2a8,
    ShieldBarBackground = 0x2ac,
    ShieldBarFill = 0x2b0,
    ArmorFrame = 0x2b4,
    ArmorFrameLow = 0x2b8,
    ArmorBarBackground = 0x2bc,
    ArmorRegenFill = 0x2c0,
    ArmorBarFill = 0x2c4,
    GammaBarFill = 0x2c8,
    GammaBarBackground = 0x2cc,
    GammaFrame = 0x2d0,
    BarDivider = 0x2d4,
    MainActionPressed = 0x2e0,
    MainActionIdle = 0x2e4,
    TargetContextOverlay = 0x2e8,
    SecondaryPressed = 0x2ec,
    SecondaryIdle = 0x2f0,
    PausePressed = 0x2f4,
    PauseIdle = 0x2f8,
    BoostPressed = 0x2fc,
    BoostIdle = 0x300,
    SteeringKnobPressed = 0x304,
    SteeringKnobIdle = 0x308,
    DockPressed = 0x30c,
    DockIdle = 0x310,
    AutoTurretEnabled = 0x314,
    AutoTurretDisabled = 0x318,
    SteeringBase = 0x31c,
    MissionTimerPanel = 0x320,
    CargoPanel = 0x324,
    PassengerPanel = 0x334,
    ProductionCargoPanel = 0x338,
    ProductionRemainingPanel = 0x33c,
    VolatileCargoOverlay = 0x340,
    MissionStatusPanel = 0x344,
    Reticle = 0x348,
    QuickMenuPressed = 0x34c,
    QuickMenuIdle = 0x350,
    EventBanner = 0x354,
    SecondaryWeaponBanner = 0x358,
    QuickMenuHeader = 0x35c,
    HitVerticalArmor = 0x360,
    HitHorizontalArmor = 0x364,
    HitVerticalShield = 0x368,
    HitHorizontalShield = 0x36c,
    FuelGaugeIcon = 0x370,
    FuelGaugeBar = 0x374,
    ProgressPanel = 0x378,
    ChargeProgressFill = 0x37c,
    DockTransferMissionMarker = 0x380,
    DockTransferProductionMarker = 0x384,
    ProgressPanelDuplicate = 0x388,
    DockTransferFill = 0x38c,
};

struct HudArm32Layout {
    std::uint8_t field_000;
    std::uint8_t visible;
    std::uint8_t field_002;
    std::uint8_t field_003;
    std::int32_t iPadFireImage;
    std::int32_t iPadFirePressedImage;
    std::uint16_t iPadFireCoord0;
    std::uint16_t iPadFireCoord1;
    std::int32_t iPadSteerAnchor;
    std::int32_t iPadFireAnchor;
    std::uint32_t menuButtons;
    HudArm32StringSlot strings_01c_100[20];
    std::uint8_t unknown_10c[0x50];
    std::int32_t image_15c;
    std::int32_t eventLineX;
    std::int32_t eventLineY;
    std::uint8_t unknown_168[0x5c];
    std::int32_t factionLogoImage;
    std::uint8_t unknown_1c8[8];
    std::int32_t field_1d0;
    std::uint8_t unknown_1d4[4];
    std::int32_t eventScrollTick;
    std::uint8_t unknown_1dc[2];
    std::uint8_t eventScrolls;
    std::uint8_t unknown_1df;
    HudArm32StringSlot eventLine;
    std::uint8_t letterbox;
    std::uint8_t unknown_1ed[7];
    HudArm32StringSlot cargoEventLine;
    HudArm32StringSlot auxiliaryEventLine;
    std::uint8_t unknown_20c[0x0c];
    std::int32_t secondaryLabelX;
    std::uint8_t unknown_21c;
    std::uint8_t hasCloak;
    std::uint8_t hasBoostButton;
    std::uint8_t hasShieldBar;
    std::uint8_t hasArmorRegen;
    std::uint8_t hasAutofireUI;
    std::uint8_t unknown_222[6];
    HudArm32StringSlot secondaryEventLine;
    std::uint8_t unknown_234;
    std::uint8_t cargoFullFlag;
    std::uint8_t unknown_236[2];
    std::int32_t quickMenuType;
    std::uint8_t unknown_23c[8];
    std::uint8_t shieldHitFlash;
    std::uint8_t unknown_245[0x13];
    std::uint32_t currentSecondaryWeapon;
    std::uint32_t equipmentArray;
    std::uint8_t unknown_260[4];
    std::uint32_t eventQueue;
    std::int32_t eventQueueTimer;
    std::uint8_t eventQueueDirty;
    std::uint8_t unknown_26d[3];
    std::int32_t eventQueuePaused;
    std::uint8_t jumpMapSelectedFlag;
    std::uint8_t field_275;
    std::uint8_t cloakProgressActive;
    std::uint8_t jumpDriveProgressActive;
    std::uint8_t dockTransferProgressActive;
    std::uint8_t dockTransferReverse;
    std::uint8_t dockTransferShowMissionMarkers;
    std::uint8_t field_27b;
    std::int32_t fuelGaugeValue;
    std::uint8_t field_280;
    std::uint8_t field_281;
    std::uint8_t quickMenuOpen;
    std::uint8_t quickMenuEmpty;
    std::uint32_t touchFlags;
    std::uint32_t field_288;
    std::uint32_t keyArray;
    std::uint32_t elementBits;
    std::uint32_t unknown_294;
    std::int32_t imageSlots_298_3b0[71];
    HudArm32StringSlot secondaryWeaponLabel;
    std::int32_t unknown_3c0;
    std::int32_t menuFrameX;
    std::int32_t menuOriginY;
    std::int32_t menuRowHeight;
    std::int32_t menuRowStride;
    std::int32_t menuHeaderOffset;
    std::int32_t menuBaseY;
    std::int32_t menuFrameWidth;
    std::uint16_t coordinates_3e0_462[66];
    std::int32_t chargeProgressFadeTimer;
    std::int32_t dockTransferFadeTimer;
    std::int32_t hitFlashTimer;
    std::int32_t field_470;
    std::uint8_t boostReadyLatched;
    std::uint8_t unknown_475;
    std::uint8_t cloakReadyLatched;
    std::uint8_t unknown_477;
    std::uint8_t unknown_478[0x0c];
    std::int32_t boostFlashRemaining;
    std::int32_t boostFlashPulse;
    std::int32_t secondaryFlashRemaining;
    std::int32_t secondaryFlashPulse;
    std::int32_t unknown_494;
    std::int32_t quickMenuFlashRemaining;
    std::int32_t quickMenuFlashPulse;
    std::uint8_t autofireEnabled;
    std::uint8_t unknown_4a1[4];
    std::uint8_t fireForTutorial;
    std::uint8_t unknown_4a6[6];
    std::int32_t hitDirectionLeftTimer;
    std::int32_t hitDirectionRightTimer;
    std::int32_t hitDirectionTopTimer;
    std::int32_t hitDirectionBottomTimer;
    std::int32_t timeExtenderTimer;
    std::int32_t timeExtenderDuration;
    std::int32_t miningHintPulseTimer;
    std::uint8_t messageActive;
    std::uint8_t unknown_4c9[0x0b];
    std::int32_t menuOriginX;
    std::int32_t menuOriginYBase;
    std::int32_t touchHalfExtent;
    std::int32_t touchHalfExtentSmall;
    std::int32_t analogStickRadius;
    std::int32_t eventLineMargin;
    std::int32_t field_4ec;
    std::int32_t eventLineMarginAlt;
    std::int32_t cameraIdleImages[4];
    std::int32_t cameraPressedImages[4];
    std::int32_t previousCameraMode;
    std::int32_t cameraModeLabelTimer;
    HudArm32StringSlot cameraModeLabel;
    std::uint8_t hackingGameActive;
    std::uint8_t unknown_529[3];
    std::int32_t cargoAggregateCount;
    std::uint32_t uintArray;
    std::uint32_t digitSprite;
    std::int32_t multiplierIconImage;
};

static_assert(sizeof(HudArm32StringSlot) == 0x0c, "Android String slot must be 12 bytes");
static_assert(alignof(HudArm32StringSlot) == 4, "Android String slot must be 4-byte aligned");
static_assert(offsetof(HudArm32Layout, iPadFireImage) == 0x004, "");
static_assert(offsetof(HudArm32Layout, iPadFireCoord0) == 0x00c, "");
static_assert(offsetof(HudArm32Layout, iPadSteerAnchor) == 0x010, "");
static_assert(offsetof(HudArm32Layout, menuButtons) == 0x018, "");
static_assert(offsetof(HudArm32Layout, strings_01c_100) == 0x01c, "");
static_assert(offsetof(HudArm32Layout, image_15c) == 0x15c, "");
static_assert(offsetof(HudArm32Layout, eventLineX) == 0x160, "");
static_assert(offsetof(HudArm32Layout, eventLineY) == 0x164, "");
static_assert(offsetof(HudArm32Layout, factionLogoImage) == 0x1c4, "");
static_assert(offsetof(HudArm32Layout, field_1d0) == 0x1d0, "");
static_assert(offsetof(HudArm32Layout, eventScrollTick) == 0x1d8, "");
static_assert(offsetof(HudArm32Layout, eventScrolls) == 0x1de, "");
static_assert(offsetof(HudArm32Layout, eventLine) == 0x1e0, "");
static_assert(offsetof(HudArm32Layout, letterbox) == 0x1ec, "");
static_assert(offsetof(HudArm32Layout, cargoEventLine) == 0x1f4, "");
static_assert(offsetof(HudArm32Layout, auxiliaryEventLine) == 0x200, "");
static_assert(offsetof(HudArm32Layout, secondaryLabelX) == 0x218, "");
static_assert(offsetof(HudArm32Layout, hasCloak) == 0x21d, "");
static_assert(offsetof(HudArm32Layout, hasAutofireUI) == 0x221, "");
static_assert(offsetof(HudArm32Layout, secondaryEventLine) == 0x228, "");
static_assert(offsetof(HudArm32Layout, cargoFullFlag) == 0x235, "");
static_assert(offsetof(HudArm32Layout, quickMenuType) == 0x238, "");
static_assert(offsetof(HudArm32Layout, shieldHitFlash) == 0x244, "");
static_assert(offsetof(HudArm32Layout, currentSecondaryWeapon) == 0x258, "");
static_assert(offsetof(HudArm32Layout, equipmentArray) == 0x25c, "");
static_assert(offsetof(HudArm32Layout, eventQueue) == 0x264, "");
static_assert(offsetof(HudArm32Layout, eventQueueTimer) == 0x268, "");
static_assert(offsetof(HudArm32Layout, eventQueueDirty) == 0x26c, "");
static_assert(offsetof(HudArm32Layout, eventQueuePaused) == 0x270, "");
static_assert(offsetof(HudArm32Layout, jumpMapSelectedFlag) == 0x274, "");
static_assert(offsetof(HudArm32Layout, cloakProgressActive) == 0x276, "");
static_assert(offsetof(HudArm32Layout, dockTransferProgressActive) == 0x278, "");
static_assert(offsetof(HudArm32Layout, dockTransferShowMissionMarkers) == 0x27a, "");
static_assert(offsetof(HudArm32Layout, fuelGaugeValue) == 0x27c, "");
static_assert(offsetof(HudArm32Layout, touchFlags) == 0x284, "");
static_assert(offsetof(HudArm32Layout, keyArray) == 0x28c, "");
static_assert(offsetof(HudArm32Layout, imageSlots_298_3b0) == 0x298, "");
static_assert(offsetof(HudArm32Layout, secondaryWeaponLabel) == 0x3b4, "");
static_assert(offsetof(HudArm32Layout, coordinates_3e0_462) == 0x3e0, "");
static_assert(offsetof(HudArm32Layout, chargeProgressFadeTimer) == 0x464, "");
static_assert(offsetof(HudArm32Layout, dockTransferFadeTimer) == 0x468, "");
static_assert(offsetof(HudArm32Layout, hitFlashTimer) == 0x46c, "");
static_assert(offsetof(HudArm32Layout, boostReadyLatched) == 0x474, "");
static_assert(offsetof(HudArm32Layout, cloakReadyLatched) == 0x476, "");
static_assert(offsetof(HudArm32Layout, boostFlashRemaining) == 0x484, "");
static_assert(offsetof(HudArm32Layout, secondaryFlashRemaining) == 0x48c, "");
static_assert(offsetof(HudArm32Layout, quickMenuFlashRemaining) == 0x498, "");
static_assert(offsetof(HudArm32Layout, autofireEnabled) == 0x4a0, "");
static_assert(offsetof(HudArm32Layout, fireForTutorial) == 0x4a5, "");
static_assert(offsetof(HudArm32Layout, hitDirectionLeftTimer) == 0x4ac, "");
static_assert(offsetof(HudArm32Layout, timeExtenderTimer) == 0x4bc, "");
static_assert(offsetof(HudArm32Layout, miningHintPulseTimer) == 0x4c4, "");
static_assert(offsetof(HudArm32Layout, messageActive) == 0x4c8, "");
static_assert(offsetof(HudArm32Layout, menuOriginX) == 0x4d4, "");
static_assert(offsetof(HudArm32Layout, eventLineMargin) == 0x4e8, "");
static_assert(offsetof(HudArm32Layout, eventLineMarginAlt) == 0x4f0, "");
static_assert(offsetof(HudArm32Layout, cameraIdleImages) == 0x4f4, "");
static_assert(offsetof(HudArm32Layout, cameraPressedImages) == 0x504, "");
static_assert(offsetof(HudArm32Layout, previousCameraMode) == 0x514, "");
static_assert(offsetof(HudArm32Layout, cameraModeLabel) == 0x51c, "");
static_assert(offsetof(HudArm32Layout, hackingGameActive) == 0x528, "");
static_assert(offsetof(HudArm32Layout, cargoAggregateCount) == 0x52c, "");
static_assert(offsetof(HudArm32Layout, uintArray) == 0x530, "");
static_assert(offsetof(HudArm32Layout, digitSprite) == 0x534, "");
static_assert(offsetof(HudArm32Layout, multiplierIconImage) == 0x538, "");
static_assert(sizeof(HudArm32Layout) == 0x53c, "Android Hud allocation is 0x53c bytes");

#endif
