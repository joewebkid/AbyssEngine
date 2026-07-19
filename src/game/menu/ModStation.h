#ifndef GOF2_MODSTATION_H
#define GOF2_MODSTATION_H
#include <cstdint>
#include "../../engine/core/AEString.h"
#include "engine/math/EaseInOutMatrix.h"
#include "game/core/CutScene.h"
#include "game/ui/DialogueWindow.h"
#include "game/world/NewsTicker.h"
#include "game/world/StarMap.h"

class CutScene;
class DialogueWindow;
class NewsTicker;
class StarMap;
namespace AbyssEngine { 
    class EaseInOutMatrix;
 }


class ModStation {
public:
    // ---- byte-addressable flag word helper ----
    union FlagWord {
        uint32_t word;
        uint16_t halfword;
        uint16_t halfwords[2];
        uint8_t bytes[4];
    };

    unsigned fadeColor;
    int *field_0x08;
    int state;
    StarMap *starMap;
    CutScene *cutScene;
    char pendingHangarClose;
    NewsTicker *newsTicker;
    union {
        AbyssEngine::EaseInOutMatrix *cameraTween;   // 0x20
        FlagWord cameraTweenFlags;
    };
    char stationActive;
    int dt;
    int loadTick;
    union {
        long long accumTime;
        struct {
            FlagWord accumTimeLo;                     // 0x30 low 32 bits (bytes/halfword/word)
            FlagWord accumTimeHi;                     // 0x34 high 32 bits
        };
    };
    String stationName;
    int selectedButton;
    int *buttonState;
    union {
        int departPending;
        FlagWord departPendingFlags;
    };
    void *dlcMenu;
    int activeMission;
    void *radioMessages;
    int field_0x5c;
    FlagWord m_nStarMapWindowOpen;
    FlagWord subWindowFlags;
    FlagWord modalFlags;
    FlagWord screenFlags;
    union {
        void *choiceWindow;                          // 0x68
        FlagWord choiceWindowFlags;
    };
    void *spaceLounge;
    void *hangarWindow;
    void *statusWindow;
    DialogueWindow *m_pDialogueWindow;
    DialogueWindow *dialogueWindow;
    void *medalChoiceWindow;
    int *buttonRow;
    void *buttonLaunch;
    union {
        void *buttonCredits;                         // 0x84
        FlagWord buttonCreditsFlags;
    };
    union {
        void *scrollBox;                             // 0x88
        FlagWord scrollBoxFlags;
    };
    int introTimer;
    void *hangarShipGeom;     // 0xa0

    int field_0xa4;           // 0xa4 (trailing pad after hangarShipGeom)
    void *hangarGeom;         // 0xa8  (cached hangar ship geometry / touch x-threshold)
    int field_0xac;           // 0xac
    FlagWord cameraFlags;     // 0xb0  bytes [0]=camActive [1]=? [3]=camArmed
    int field_0xb4;           // 0xb4
    int field_0xb8;           // 0xb8
    int *medalArray;          // 0xbc  Array<int*>*
    int medalIndex;           // 0xc0
    int medalCount;           // 0xc4
    int field_0xc8;           // 0xc8
    int introCountdown;       // 0xcc  (=0x32)
    int field_0xd0;           // 0xd0
    int field_0xd4;           // 0xd4
    FlagWord alarmFlags;      // 0xd8  bytes [0]=alarmActive [1] [3]
    FlagWord hintFlags;       // 0xdc  bytes [0] [1]; halfword@+2 (0xde)
    int field_0xe0;           // 0xe0
    int camAngle;             // 0xe4
    int camScrollPos;         // 0xe8
    int field_0xec;           // 0xec
    int camScrollVel;         // 0xf0
    int field_0xf4;           // 0xf4
    int field_0xf8;           // 0xf8
    int field_0xfc;           // 0xfc
    FlagWord dragFlags;       // 0x100 byte [0]=dragging [1]=touchActive
    void *idleBox;            // 0x104
    int field_0x108;          // 0x108
    int field_0x10c;          // 0x10c
    union { int touchX; float touchXf; };          // 0x110 (radio reveal: float scroll-current)
    union { int touchY; float touchYf; };          // 0x114 (radio reveal: float scroll-target)
    union {
        FlagWord scrollFlags; // 0x118 byte [0]
        float scrollFlagsf;   //       float@0x118 hangar-light intensity
    };
    union { int scrollTarget; float scrollTargetf; }; // 0x11c (hangar-light target)
    int field_0x120;          // 0x120
    int field_0x124;          // 0x124
    void *activeTouch;        // 0x128
    FlagWord field_0x12c;     // 0x12c
    int camKeyX;              // 0x130
    int camKeyY;              // 0x134
    int camKeyZ;              // 0x138
    void *easeX;              // 0x13c  AbyssEngine::EaseInOut*
    void *easeY;              // 0x140
    void *easeZ;              // 0x144
    FlagWord field_0x148;     // 0x148
    uint8_t field_0x14c[0x278 - 0x14c];   // 0x14c..0x277 (unmodeled tail block)
    float camCoordX;          // 0x278
    float camCoordY;          // 0x27c
    float camCoordZ;          // 0x280
    uint8_t field_0x284[0x2a0 - 0x284];   // 0x284..0x29f (unmodeled block)
    int buttonCacheX[5];      // 0x2a0  cached TouchButton x positions
    int buttonCacheY[5];      // 0x2b4  cached TouchButton y positions
    int idleDeltaX;           // 0x2c8
    int idleDeltaY;           // 0x2cc

    ModStation();

    virtual ~ModStation();

    void OnInitialize();

    void OnKeyPress(long long unused, long long key);

    long long OnKeyRelease(long long unused, long long key);

    int ShowLoadingScreen();

    void OnRelease();

    void OnRender2D();

    void OnRender3D();

    void OnResume();

    void OnSuspend();

    void OnTouchBegin(int x, int y, void *touch);

    void OnTouchEnd(int x, int y, void *touch);

    void OnTouchMove(int x, int y, void *touch);

    int OnTouchBegin(int x, int y);

    int OnTouchEnd(int x, int y);

    int OnTouchMove(int x, int y);

    void OnUpdate();

    void addAchievement(int medalId, int kind);

    void autosave();

    void checkHints();

    void checkMedals();

    void checkPendingProducts();

    void enterStation();

    void leaveStation();

    void resetLight();

    void resetIdleCamForHangar();

    void setGameLoaded();

    void showCBSMessage();

    void showDlcMenu();

    void showMapWindow();
};

#endif
