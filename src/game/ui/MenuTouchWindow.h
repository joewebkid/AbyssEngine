#ifndef GOF2_MENUTOUCHWINDOW_H
#define GOF2_MENUTOUCHWINDOW_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ui/TouchButton.h"
#include "game/ui/ModuleTransitionThunk.h"

#include "game/ui/RefreshThunk.h"
class TouchButton;
class TouchSlider;
class ScrollTouchWindow;
class ScrollTouchBox;
class MissionsWindow;
class ChoiceWindow;
class GameRecord;

class MenuTouchWindow {
public:
    uint8_t cinematicSteerActive;
    uint8_t pendingActivate;
    uint8_t pad_0x2[2];
    Array<void *> *buttons;
    void *cinematicTouchIdA;
    void *cinematicTouchIdB;
    uint8_t pad_0x10[4];
    TouchButton *cinematicBtnA;
    TouchButton *cinematicBtnB;
    uint8_t pad_0x1c[0x74];
    int cinematicAnchorA;
    int cinematicAnchorB;
    unsigned short cinematicTouchState;
    uint8_t pad_0x9a[2];
    Array<void *> *previewStrings0;
    Array<void *> *previewStrings1;
    uint8_t pad_0xa4[8];
    Array<void *> *optionsButtons;
    Array<void *> *buttonsB0;
    Array<void *> *buttonsB4;
    Array<void *> *buttonsB8;
    void *previewRecords;
    Array<void *> *scrollEntries;
    TouchButton *okButton;
    TouchButton *backButton;
    TouchButton *optBtnCC;
    TouchButton *optBtnD0;
    TouchButton *optBtnD4;
    TouchButton *optBtnD8;
    TouchButton *optBtnDC;
    uint8_t pad_0xe0[4];
    TouchButton *scrollUpButton;
    TouchButton *scrollExtraButton;
    void *sliders;
    ScrollTouchWindow *scrollWindowA;
    ScrollTouchWindow *scrollWindowB;
    Array<void *> *scrollSlots;
    MissionsWindow *missionsWindow;
    Array<void *> *recordRows;
    ChoiceWindow *choiceWindow;
    uint8_t upButtonPressed;
    uint8_t downButtonPressed;
    uint8_t pad_0x10a[0x12];
    uint32_t scrollbarImageId;
    int field_0x120;
    uint8_t pad_0x124[0x10];
    void *heapBufA;
    void *heapBufB;
    uint8_t pad_0x13c[0x18];
    int listEntryWidth;
    int listEntryHeight;
    uint8_t pad_0x15c[0xc];
    int backgroundEnabled;
    int menuState;
    uint8_t messageShowing;
    uint8_t pad_0x171[2];
    uint8_t saveDialogShowing;
    uint8_t returnToMenuSubFlag;
    uint8_t pad_0x175[2];
    uint8_t quitConfirmShowing;
    uint8_t returnToMenuShowing;
    uint8_t leaderboardDialogShowing;
    uint8_t dlcMessageShowing;
    uint8_t pad_0x17b;
    uint8_t dlcResultDialogShowing;
    uint8_t dlcErrorDialogShowing;
    uint8_t loadFailedDialogShowing;
    uint8_t genericConfirmA;
    uint8_t supernovaMessageShowing;
    uint8_t supernovaPurchaseDialogShowing;
    uint8_t pad_0x182[2];
    int screenshotState;
    uint8_t pad_0x188;
    uint8_t storeInitDialogShowing;
    uint8_t pad_0x18a[2];
    int selectedRow;
    uint8_t purchaseRestorePending;
    uint8_t genericConfirmB;
    uint8_t pad_0x192[2];
    int scrollOffset;
    int listX;
    int listTopY;
    int listBottomY;
    uint32_t fadeValue;
    int buttonWidth;
    int buttonYBias;
    int buttonRowGap;
    int listRowGap;
    int metricA;
    int metricB;
    int metricC;
    uint8_t field_0x1c4;
    uint8_t pad_0x1c5[0x14];
    uint8_t scrollbarHit;
    uint8_t pad_0x1da;
    uint8_t listStateSuppress;
    uint8_t pad_0x1dc[4];
    int field_0x1e0;
    int contentHeightCache;
    uint8_t pad_0x1e8[0x24];
    int dragLastX;
    uint8_t pad_0x210[4];
    int dragVelocity;
    uint32_t inertiaDecay;
    float inertiaVel;
    int dragStartX;
    uint8_t dragging;
    uint8_t pad_0x225[3];
    int contentHeight;
    int pageHeight;
    int field_0x230;
    int field_0x234;
    uint8_t cutsceneMode;

    MenuTouchWindow(int menuType);

    ~MenuTouchWindow();

    void showSupernovaMessage();

    bool isInMissionScreen();

    uint8_t isShowingMessage();

    bool isMakingScreenshot();

    void hideMessage();

    void render3D();

    bool inCinematicMode();

    float getRelativeScrollStartPos();

    int OnTouchEnd(int y, int x, void *touchId);

    void createRecordButtons(bool inSaveMode);

    void startValkyrie();

    int OnTouchBegin(int y, int x, void *touchId);

    int loadGame(int slot);

    void addButton(int id, AbyssEngine::String label, int row, Array<TouchButton *> *arr, int yOff);

    void setCutsceneMode(bool mode);

    void loadPreviewRecords();

    void saveGame(int slot);

    void update(int dt);

    void startSupernovaChallenge();

    void callDlcMenu();

    void draw();

    float getRelativeScrollHeight();

    int OnTouchMove(int y, int x, void *touchId);

    void setSkipButtonVisible(bool visible);

    void drawLoadSaveMenu(bool param1);

    void startSupernova();

    void startGOF2();
};
#endif
