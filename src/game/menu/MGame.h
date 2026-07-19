#ifndef GOF2_MGAME_H
#define GOF2_MGAME_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"

#include "engine/core/IApplicationModule.h"
#include "engine/math/Vector.h"

class Radar;


class AEGeometry;
class ChoiceWindow;
class DialogueWindow;
class Hud;
class Level;
class LevelScript;
class MenuTouchWindow;
class PlayerEgo;
class Radio;
class StarMap;
class TargetFollowCamera;



namespace AbyssEngine {
    class ApplicationManager;
}
using ::AbyssEngine::ApplicationManager;

#pragma pack(push, 1)
class MGame : public IApplicationModule {
public:
    int loadProgress;
    int loadingImage;
    int cameraMode;
    uint8_t field_0x18;
    uint8_t _pad_0x19[3];
    int field_0x1c;
    unsigned startTime;
    unsigned startTimeHigh;
    unsigned lastTime;
    unsigned lastTimeHigh;
    int frameTime;
    int frameTimeHigh;
    int field_0x38;
    int field_0x3c;
    int deltaTime;
    int field_0x44;
    unsigned elapsedTime;
    int elapsedTimeHigh;
    int loadingTime;
    uint8_t active;
    uint8_t _pad_0x55[3];
    PlayerEgo *player;
    union { int field_0x5c; struct { uint8_t _b5c; uint8_t pauseOpen; uint8_t cutsceneActive; uint8_t jumpActive; }; };
    uint8_t gameOverActive;
    uint8_t campaignMission;
    uint8_t _pad_0x62[2];
    AbyssEngine::String gameOverTitle;
    int field_0x70;
    Hud *hud;
    Level *level;
    LevelScript *levelScript;
    Radar *radar;
    Radio *radio;
    MenuTouchWindow *menuWindow;
    DialogueWindow *dialogueWindow;
    StarMap *starMap;
    ChoiceWindow *choiceWindow;
    int touch0Id;
    int touch1Id;
    int menuTime;
    union { Vector freeCamFinger1; struct { int freeCamFinger1X; int freeCamFinger1Y; int freeCamFinger1Z; }; };
    union { Vector freeCamFinger0; struct { int freeCamFinger0X; int freeCamFinger0Y; int freeCamFinger0Z; }; };
    float flCameraRoll;
    void *activeTouchId;
    union {
        int dockChoiceOpen;
        struct {
            uint8_t field_0xc1;
            union { uint8_t autopilotMenuOpen; uint8_t field_0xc2; };
            union { uint8_t field_0xc6; uint8_t field_0xc3; };
            uint8_t starMapOpen;
        };
    };
    union { uint16_t field_0xc8; struct { uint8_t _bc8; uint8_t menuTouchOpen; }; };
    union { int field_0xca; struct { uint8_t _bca; uint8_t touchesStream; uint8_t touchesStation; uint8_t jumpGateSoundStarted; }; };
    uint8_t choiceWindowOpen;
    uint8_t field_0xcf;
    int choiceItemCount;
    uint8_t field_0xd4;
    uint8_t turretMode;
    uint8_t hudMenuOpen;
    uint8_t _pad_0xd7[1];
    int field_0xd8;
    uint8_t jumpDriveActive;
    uint8_t usingJumpDrive;
    uint8_t _pad_0xde[2];
    uint8_t field_0xe0;
    uint8_t _pad_0xe1[3];
    union { Vector egoJumpPos; struct { int egoJumpPosX; int egoJumpPosY; int egoJumpPosZ; }; };
    unsigned cameraId;
    TargetFollowCamera *camera;
    int hudTouchFlags;
    uint8_t _pad_0xfc[4];
    int lastTapTime;
    int lastTapTimeHigh;
    int lastAlignTime;
    int lastAlignTimeHigh;
    uint8_t field_0x110;
    uint8_t needsRedraw;
    uint8_t _pad_0x112[2];
    AEGeometry *jumpFlash;
    float flShakeX;
    float flShakeY;
    int field_0x120;
    int dragLastX;
    int dragLastY;
    int dragRotIntX;
    int dragRotIntY;
    int dragDeltaX;
    int dragDeltaY;
    float flShakeAmpX;
    float flShakeAmpY;
    int field_0x144;
    float flShakePhaseX;
    float flShakePhaseY;
    int field_0x150;
    int dragStartX;
    int dragStartY;
    uint8_t freeCamDragging;
    uint8_t menuOpen;
    uint8_t freeCamMode;
    uint8_t _pad_0x15f[1];
    int cinematicPrevCamMode;
    uint8_t cinematicPrevLookAt;
    uint8_t _pad_0x165[3];
    float flFastForwardWeight;
    int timeWarpState;
    int cloakAttributeMax;
    int cloakAttribute;
    int maneuverHoldTime;
    uint8_t maneuverActive;
    uint8_t _pad_0x17d[3];
    int maneuverStartX;
    int maneuverStartY;
    int field_0x188;
    int field_0x18c;
    int field_0x190;
    int field_0x194;
    int field_0x198;
    int field_0x19c;
    int field_0x1a0;
    uint16_t field_0x1a4;
    uint8_t pauseSnapshot;
    uint8_t _pad_0x1a7[1];
    float flFastForwardFactor;
    int field_0x1ac;
    int field_0x1b0;
    int field_0x1b4;
    union { uint16_t thrustActive; struct { uint8_t _b1b8; uint8_t thrustEngaged; }; };
    uint8_t _pad_0x1ba[2];
    int field_0x1bc;
    int thrustStartY;
    int field_0x1c4;
    int thrustResetX;
    int thrustThreshold;
    int thrustBase;
    int field_0x1d4;
    int field_0x1d8;
    uint8_t _pad_0x1dc[1];
    uint8_t field_0x1dd;
    uint8_t _pad_0x1de[2];
    int field_0x1e0;
    uint16_t field_0x1e4;
    uint8_t field_0x1e6;
    uint8_t _pad_0x1e7[1];
    int gameRecord;
    Array<AbyssEngine::String *> *missionInfoLines;

    MGame();

    ~MGame() override;

    int OnInitialize() override;

    void OnRelease() override;

    long long OnKeyPress(long long key, long long mod) override;

    long long OnKeyRelease(long long key, long long mod) override;

    void OnTouchBegin(int p1, int p2) override;

    void OnTouchMove(int p1, int p2) override;

    void OnTouchEnd(int p1, int p2) override;

    void OnTouchBegin(int p1, int p2, void *touchId) override;

    void OnTouchMove(int p1, int y, void *touch) override;

    void OnTouchEnd(int p1, int p2, void *touchId) override;

    void OnUpdate() override;

    void OnRender3D() override;

    void OnRender2D() override;

    void OnSuspend() override;

    void OnResume() override;

    int ShowLoadingScreen() override;

    void showLiteScreen();

    void pause();

    void UseKhadorDrive();

    void dialogueEvent();

    void dockEvent(int p1, int p2);

    void freeCamTouchBegin(int x, int y, void *id);

    void freeCamTouchEnd(int p1, int p2, void *id);

    void freeCamTouchMove(int x, int y, void *touchId);

    void gameOverCheck();

    void handleAccelerometer();

    void maneuverTouchBegin(int x, int y, void *p);

    void maneuverTouchEnd(int a, int b, void *p);

    void maneuverTouchMove(int a, int b, void *p);

    int nextCamId(int cur);

    void pauseSounds();

    void reset();

    void resumeSounds();

    void setCinematicMode(bool on);

    void startChargingJumpDrive();

    void startJumpScene();

    void successCheck();

    void switchCamera(int id);

    void updateJumpScene();

    void useCloak();
};
#pragma pack(pop)

#if __SIZEOF_POINTER__ == 4  // live in 32-bit MATCH build (was #ifdef GOF2_MATCH, which is never defined)
#include <cstddef>
static_assert(offsetof(MGame, loadProgress) == 12, "MGame::loadProgress @ 12");
static_assert(offsetof(MGame, deltaTime) == 64, "MGame::deltaTime @ 64");
static_assert(offsetof(MGame, field_0x44) == 68, "MGame::field_0x44 @ 68");
static_assert(offsetof(MGame, player) == 88, "MGame::player @ 88");
static_assert(offsetof(MGame, field_0x5c) == 92, "MGame::field_0x5c @ 92");
static_assert(offsetof(MGame, pauseOpen) == 93, "MGame::pauseOpen @ 93");
static_assert(offsetof(MGame, cutsceneActive) == 94, "MGame::cutsceneActive @ 94");
static_assert(offsetof(MGame, jumpActive) == 95, "MGame::jumpActive @ 95");
static_assert(offsetof(MGame, gameOverActive) == 96, "MGame::gameOverActive @ 96");
static_assert(offsetof(MGame, campaignMission) == 97, "MGame::campaignMission @ 97");
static_assert(offsetof(MGame, gameOverTitle) == 100, "MGame::gameOverTitle @ 100");
static_assert(offsetof(MGame, field_0x70) == 112, "MGame::field_0x70 @ 112");
static_assert(offsetof(MGame, hud) == 116, "MGame::hud @ 116");
static_assert(offsetof(MGame, level) == 120, "MGame::level @ 120");
static_assert(offsetof(MGame, levelScript) == 124, "MGame::levelScript @ 124");
static_assert(offsetof(MGame, radar) == 128, "MGame::radar @ 128");
static_assert(offsetof(MGame, radio) == 132, "MGame::radio @ 132");
static_assert(offsetof(MGame, menuWindow) == 136, "MGame::menuWindow @ 136");
static_assert(offsetof(MGame, menuTime) == 160, "MGame::menuTime @ 160");
static_assert(offsetof(MGame, freeCamFinger1X) == 164, "MGame::freeCamFinger1X @ 164");
static_assert(offsetof(MGame, flCameraRoll) == 188, "MGame::flCameraRoll @ 188");
static_assert(offsetof(MGame, activeTouchId) == 192, "MGame::activeTouchId @ 192");
static_assert(offsetof(MGame, dockChoiceOpen) == 196, "MGame::dockChoiceOpen @ 196");
static_assert(offsetof(MGame, autopilotMenuOpen) == 197, "MGame::autopilotMenuOpen @ 197");
static_assert(offsetof(MGame, field_0xc6) == 198, "MGame::field_0xc6 @ 198");
static_assert(offsetof(MGame, starMapOpen) == 199, "MGame::starMapOpen @ 199");
static_assert(offsetof(MGame, field_0xc8) == 200, "MGame::field_0xc8 @ 200");
static_assert(offsetof(MGame, menuTouchOpen) == 201, "MGame::menuTouchOpen @ 201");
static_assert(offsetof(MGame, field_0xca) == 202, "MGame::field_0xca @ 202");
static_assert(offsetof(MGame, touchesStream) == 203, "MGame::touchesStream @ 203");
static_assert(offsetof(MGame, touchesStation) == 204, "MGame::touchesStation @ 204");
static_assert(offsetof(MGame, jumpGateSoundStarted) == 205, "MGame::jumpGateSoundStarted @ 205");
static_assert(offsetof(MGame, choiceWindowOpen) == 206, "MGame::choiceWindowOpen @ 206");
static_assert(offsetof(MGame, choiceItemCount) == 208, "MGame::choiceItemCount @ 208");
static_assert(offsetof(MGame, hudMenuOpen) == 214, "MGame::hudMenuOpen @ 214");
static_assert(offsetof(MGame, field_0xd8) == 216, "MGame::field_0xd8 @ 216");
static_assert(offsetof(MGame, jumpDriveActive) == 220, "MGame::jumpDriveActive @ 220");
static_assert(offsetof(MGame, usingJumpDrive) == 221, "MGame::usingJumpDrive @ 221");
static_assert(offsetof(MGame, field_0xe0) == 224, "MGame::field_0xe0 @ 224");
static_assert(offsetof(MGame, egoJumpPosX) == 228, "MGame::egoJumpPosX @ 228");
static_assert(offsetof(MGame, cameraId) == 240, "MGame::cameraId @ 240");
static_assert(offsetof(MGame, camera) == 244, "MGame::camera @ 244");
static_assert(offsetof(MGame, hudTouchFlags) == 248, "MGame::hudTouchFlags @ 248");
static_assert(offsetof(MGame, lastTapTime) == 256, "MGame::lastTapTime @ 256");
static_assert(offsetof(MGame, field_0x110) == 272, "MGame::field_0x110 @ 272");
static_assert(offsetof(MGame, needsRedraw) == 273, "MGame::needsRedraw @ 273");
static_assert(offsetof(MGame, jumpFlash) == 276, "MGame::jumpFlash @ 276");
static_assert(offsetof(MGame, flShakeX) == 280, "MGame::flShakeX @ 280");
static_assert(offsetof(MGame, dragLastX) == 292, "MGame::dragLastX @ 292");
static_assert(offsetof(MGame, flShakeAmpX) == 316, "MGame::flShakeAmpX @ 316");
static_assert(offsetof(MGame, flShakePhaseX) == 328, "MGame::flShakePhaseX @ 328");
static_assert(offsetof(MGame, field_0x150) == 336, "MGame::field_0x150 @ 336");
static_assert(offsetof(MGame, dragStartX) == 340, "MGame::dragStartX @ 340");
static_assert(offsetof(MGame, freeCamDragging) == 348, "MGame::freeCamDragging @ 348");
static_assert(offsetof(MGame, freeCamMode) == 350, "MGame::freeCamMode @ 350");
static_assert(offsetof(MGame, cinematicPrevCamMode) == 352, "MGame::cinematicPrevCamMode @ 352");
static_assert(offsetof(MGame, cinematicPrevLookAt) == 356, "MGame::cinematicPrevLookAt @ 356");
static_assert(offsetof(MGame, flFastForwardWeight) == 360, "MGame::flFastForwardWeight @ 360");
static_assert(offsetof(MGame, timeWarpState) == 364, "MGame::timeWarpState @ 364");
static_assert(offsetof(MGame, cloakAttributeMax) == 368, "MGame::cloakAttributeMax @ 368");
static_assert(offsetof(MGame, cloakAttribute) == 372, "MGame::cloakAttribute @ 372");
static_assert(offsetof(MGame, maneuverHoldTime) == 376, "MGame::maneuverHoldTime @ 376");
static_assert(offsetof(MGame, maneuverActive) == 380, "MGame::maneuverActive @ 380");
static_assert(offsetof(MGame, maneuverStartX) == 384, "MGame::maneuverStartX @ 384");
static_assert(offsetof(MGame, maneuverStartY) == 388, "MGame::maneuverStartY @ 388");
static_assert(offsetof(MGame, field_0x19c) == 412, "MGame::field_0x19c @ 412");
static_assert(offsetof(MGame, field_0x1a0) == 416, "MGame::field_0x1a0 @ 416");
static_assert(offsetof(MGame, pauseSnapshot) == 422, "MGame::pauseSnapshot @ 422");
static_assert(offsetof(MGame, flFastForwardFactor) == 424, "MGame::flFastForwardFactor @ 424");
static_assert(offsetof(MGame, field_0x1ac) == 428, "MGame::field_0x1ac @ 428");
static_assert(offsetof(MGame, thrustActive) == 440, "MGame::thrustActive @ 440");
static_assert(offsetof(MGame, thrustEngaged) == 441, "MGame::thrustEngaged @ 441");
static_assert(offsetof(MGame, field_0x1bc) == 444, "MGame::field_0x1bc @ 444");
static_assert(offsetof(MGame, thrustStartY) == 448, "MGame::thrustStartY @ 448");
static_assert(offsetof(MGame, field_0x1c4) == 452, "MGame::field_0x1c4 @ 452");
static_assert(offsetof(MGame, thrustResetX) == 456, "MGame::thrustResetX @ 456");
static_assert(offsetof(MGame, thrustThreshold) == 460, "MGame::thrustThreshold @ 460");
static_assert(offsetof(MGame, thrustBase) == 464, "MGame::thrustBase @ 464");
static_assert(offsetof(MGame, field_0x1d4) == 468, "MGame::field_0x1d4 @ 468");
static_assert(offsetof(MGame, field_0x1d8) == 472, "MGame::field_0x1d8 @ 472");
static_assert(offsetof(MGame, field_0x1dd) == 477, "MGame::field_0x1dd @ 477");
static_assert(offsetof(MGame, field_0x1e0) == 480, "MGame::field_0x1e0 @ 480");
static_assert(offsetof(MGame, field_0x1e4) == 484, "MGame::field_0x1e4 @ 484");
static_assert(offsetof(MGame, field_0x1e6) == 486, "MGame::field_0x1e6 @ 486");
static_assert(offsetof(MGame, gameRecord) == 488, "MGame::gameRecord @ 488");
static_assert(offsetof(MGame, missionInfoLines) == 492, "MGame::missionInfoLines @ 492");
static_assert(sizeof(MGame) == 496, "sizeof(MGame) == 496");
#endif

#endif
