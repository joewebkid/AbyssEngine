#include "game/menu/MGame.h"
#include "game/core/Radio.h"
#include "game/core/RadioMessage.h"
#include "game/core/Globals.h"
#include "game/ship/PlayerFighter.h"
#include "game/ship/Ship.h"
#include "game/world/SolarSystem.h"
#include "game/ship/TargetFollowCamera.h"
#include "engine/render/AEGeometry.h"
#include "game/ui/ChoiceWindow.h"
#include "engine/audio/FModSound.h"
#include "engine/file/FileRead.h"
#include "game/world/Galaxy.h"
#include "game/mission/Item.h"
#include "game/world/LevelScript.h"
#include "game/ui/MenuTouchWindow.h"
#include "game/mission/Objective.h"
#include "game/ship/PlayerJumpgate.h"
#include "game/world/StarMap.h"
#include "engine/math/Transform.h"
#include "game/mission/Achievements.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/AERandom.h"
#include "game/ui/DialogueWindow.h"
#include "engine/render/Engine.h"
#include "game/mission/GameRecord.h"
#include "engine/core/GameText.h"
#include "game/ui/Hud.h"
#include "engine/render/ImageFactory.h"
#include "game/ui/Layout.h"
#include "game/world/Level.h"
#include "game/mission/Mission.h"
#include "game/ship/Player.h"
#include "game/mission/RecordHandler.h"
#include "game/world/Route.h"
#include "game/world/StarSystem.h"
#include "game/world/Station.h"
#include "game/world/Standing.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerFixedObject.h"
#include "game/ship/Agent.h"
#include "game/mission/Status.h"
#include "engine/render/PaintCanvas.h"
#include "engine/input/VirtualInput.h"
#include "engine/math/AEMath.h"
#include "game/weapons/Radar.h"

class Music;
class Cfg;

static __attribute__((always_inline)) inline float MGame_cameraFarPlane();

static inline bool mgame_full_effects_enabled() {
    return Globals::options[0x0f] != 0;
}

static inline bool mgame_invert_y() {
    return Globals::options[0x10] != 0;
}

static __attribute__((always_inline)) inline void
mgame_set_ki_position_slot_0x48(KIPlayer *object, float x, float y, float z) {
    using SetPositionFn = void (*)(KIPlayer *, float, float, float);
    void **vtable = *reinterpret_cast<void ***>(object);
    reinterpret_cast<SetPositionFn>(vtable[0x48 / sizeof(void *)])(object, x, y, z);
}

// Android 0x17c8d8 keeps these one-shot tutorial gates in the 0x21825d..
// global block. Their behavior is confirmed; the owning Globals fields still
// need an address-level storage audit.
static uint8_t mgame_hint_mining_intro;
static uint8_t mgame_hint_mining_followup;
static uint8_t mgame_hint_mining_lost;
static uint8_t mgame_hint_cargo_full;
static uint8_t mgame_hint_booster;
static uint8_t mgame_hint_jump_drive;
static uint8_t mgame_hint_cloak;
static uint8_t mgame_hint_autopilot;
static uint8_t mgame_hint_wingman;
static uint8_t mgame_hint_standing;
static uint8_t mgame_hint_vossk_first;
static uint8_t mgame_hint_vossk_second;
static uint8_t mgame_hint_scanner;
static uint8_t mgame_hint_hacking;
static uint8_t mgame_hint_gamma;
static uint8_t mgame_hint_volatile;
static uint8_t mgame_hint_campaign91_a;
static uint8_t mgame_hint_campaign91_b;
static uint8_t mgame_hint_campaign92;
static uint8_t mgame_hint_game_won;
static uint8_t mgame_hint_dlc1_won;

// Android 0x2168e0..0x21691b: race followed by four ImageFactory part indices.
static int mgame_dialogue_face_1601[5] = {4, 2, 0, 2, 2};
static int mgame_dialogue_face_1602[5] = {0, 2, 3, 3, 10};
static int mgame_dialogue_face_1603[5] = {10, 0, 0, 1, 0};

static __attribute__((always_inline)) inline void
mgame_show_choice(MGame *self, int textId, bool replaceKeys = false) {
    self->pauseOpen = 1;
    self->pauseSounds();
    if (self->choiceWindow == 0)
        self->choiceWindow = new ChoiceWindow();
    if (replaceKeys) {
        String text = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(
            *GameText::gGameText->getText(textId));
        self->choiceWindow->set(text);
    } else {
        self->choiceWindow->set(*GameText::gGameText->getText(textId));
    }
    self->needsRedraw = 1;
    self->choiceWindowOpen = 1;
}

static __attribute__((always_inline)) inline void
mgame_show_key_choice_pause_after(MGame *self, int textId) {
    if (self->choiceWindow == 0)
        self->choiceWindow = new ChoiceWindow();
    String text = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(
        *GameText::gGameText->getText(textId));
    self->choiceWindow->set(text);
    self->needsRedraw = 1;
    self->choiceWindowOpen = 1;
    self->pauseOpen = 1;
    self->pauseSounds();
}

static __attribute__((always_inline)) inline void
mgame_show_ambient_dialogue(MGame *self, int textId, int agentTextId,
                            int *portraitParts, int soundId) {
    self->pauseOpen = 1;
    self->pauseSounds();
    delete self->dialogueWindow;
    self->dialogueWindow = new DialogueWindow(
        GameText::gGameText->getText(textId),
        GameText::gGameText->getText(agentTextId), portraitParts);
    Globals::sound->play(soundId, 0, 0, 0.0f);
    self->needsRedraw = 1;
    self->cutsceneActive = 1;
}


int FModSound_tryToStopMusicForBGMusic();

void Music_resume(Music *m, int one, int v);

Vector *TFC_getCamOffset(TargetFollowCamera * c);

namespace AbyssEngine {
    namespace AEMath {
        float VectorLength(const Vector &value);

        Vector operator-(const Vector &lhs, const Vector &rhs);

        Vector MatrixRotateVector(const Matrix &matrix, const Vector &vector);
    }
}

void TFC_setFastForwardMode(TargetFollowCamera *c, int v);

uint8_t TFC_isInLookAtMode(TargetFollowCamera * c);

void TFC_setLookAtCam(TargetFollowCamera *c, int v);

void Cam_setCinematic(TargetFollowCamera *c, int on);

void FModSound_restoreState();

void DialogueWindow_ctor(...);

void TFC_enableFirstPersonCam(TargetFollowCamera *c, int on);

int Station_getIndex(Station *station);


void MGame::maneuverTouchEnd(int a, int b, void *p) {
    MGame *self = this;
    (void) b;
    (void) p;
    if (self->maneuverActive != 0 && self->maneuverHoldTime <= 0x258) {
        float f2 = (float) Globals::w;
        int d = a - self->maneuverStartX;
        if (d < 0) d = -d;
        float f3 = (float) d;
        if ((f2 / 480.0f) * 70.0f < f3) {
            int dir = 1;
            if (self->maneuverStartX < a) dir = 2;
            self->player->initManeuver(dir);
        }
    }
    self->maneuverActive = 0;
}


void MGame::OnResume() {
    if (Globals::sound == 0) return;
    if (Globals::sound->tryToStopMusicForBGMusic() != 0) return;
    Globals::sound->setVolume(1, *(float *) Globals::options);
}



void MGame::maneuverTouchMove(int a, int b, void *p) {
    (void) a;
    (void) p;
    if (this->maneuverActive != 0) {
        float f2 = (float) Globals::h;
        int d = b - this->maneuverStartY;
        if (d < 0) d = -d;
        float f3 = (float) d;

        if ((f2 / 320.0f) * 90.0f < f3) {
            this->maneuverActive = 0;
            this->maneuverHoldTime = 0;
        }
    }
}

void MGame::maneuverTouchBegin(int x, int y, void *p) {
    (void) p;
    this->maneuverActive = 1;
    this->maneuverStartX = x;
    this->maneuverStartY = y;
    this->maneuverHoldTime = 0;
}

void MGame::OnTouchBegin(int /*p1*/, int /*p2*/) {
}

void MGame::OnTouchEnd(int /*p1*/, int /*p2*/) {
}

void MGame::OnTouchMove(int /*p1*/, int /*p2*/) {
}

long long MGame::OnKeyPress(long long /*key*/, long long /*mod*/) {}
long long MGame::OnKeyRelease(long long /*key*/, long long /*mod*/) {}

void MGame::showLiteScreen() {
}

int MGame::ShowLoadingScreen() { return 1; }

void MGame::pause() {
}

void MGame::OnRender3D() {
    Level *level;
    int backgroundDelta;
    int renderDelta;
    bool drawShip;

    if (this->active == 0)
        return;
    PaintCanvas::gCanvas->ClearBuffer(0xff);

    if (this->pauseOpen != 0) {
        if (this->freeCamMode == 0) {
            if (this->menuTouchOpen != 0) {
                PaintCanvas::gCanvas->Begin3d();
                this->menuWindow->render3D();
            } else if (this->starMapOpen != 0) {
                PaintCanvas::gCanvas->Begin3d();
                this->starMap->render();
            } else {
                drawShip = false;
                this->level->renderBG(0);
                PaintCanvas::gCanvas->Begin3d();
                this->level->render(0);
                if (this->jumpActive == 0)
                    drawShip = this->jumpDriveActive == 0;
                this->player->render(drawShip);
                goto render_tail;
            }
            PaintCanvas::gCanvas->End3d();
            return;
        }
    }

    level = this->level;
    if (this->pauseOpen == 0 && this->freeCamMode == 0)
        backgroundDelta = this->deltaTime;
    else
        backgroundDelta = 0;
    level->renderBG(backgroundDelta);
    PaintCanvas::gCanvas->Begin3d();
    renderDelta = this->freeCamMode != 0 ? 0 : this->deltaTime;
    this->level->render(renderDelta);
    drawShip = this->jumpActive == 0 && this->jumpDriveActive == 0;
    this->player->render(drawShip);

render_tail:
    if (this->jumpFlash != 0)
        this->jumpFlash->render();
    this->levelScript->render3D();
    PaintCanvas::gCanvas->End3d();
}

void TFC_setActive(TargetFollowCamera *c, int v);

float TFC_useTargetsUpVector(TargetFollowCamera *c, int v);

void FModSound_setProp(int snd, int id);

void TFC_setPosition(TargetFollowCamera *c, float x, float y, float z);

extern void PlayerEgo_hideShipRaw(PlayerEgo *, int)
    asm("_ZN9PlayerEgo32hideShipForFirstPersonCameraViewEb");


void MGame::startJumpScene() {
    static_cast<Player *>(this->player->player)->setVulnerable(false);
    this->level->enableFog(false);

    if (this->player->isDockedToDockingPoint()) {
        this->player->dockToDockingPoint(nullptr, this->radar);
        this->camera->setActive(true);
        this->camera->setLookAtCam(false);
        this->camera->useTargetsUpVector(false);
        this->player->setSpeed(0.0f);
        this->player->setDockingState(0);
    }
    if (this->player->isInTurretMode())
        this->player->setTurretMode(false);

    Globals::sound->stop(0x23);
    this->switchCamera(0);
    this->field_0x70 = 0x3f9c28f6;
    this->hud->releaseAllKeys();
    this->field_0x110 = 0;
    this->field_0x5c = 0;

    PaintCanvas::gCanvas->CameraSetPerspective(
            this->cameraId, *reinterpret_cast<float *>(&this->field_0x70),
            20.0f, MGame_cameraFarPlane());
    this->player->setAutoPilot(nullptr);
    this->pauseOpen = 0;
    this->hudMenuOpen = 0;
    this->jumpDriveActive = 1;
    this->jumpActive = 1;
    this->player->setCollide(false);
    this->camera->setLookAtCam(true);
    this->player->stopBoost();
    static_cast<Engine *>(this->applicationManager->GetEngine())->SetPostEffect(
            0x01400002, false);

    if (this->usingJumpDrive) {
        this->player->resetMovement();
        this->player->setComputerControlled(true);
        this->jumpFlash = new AEGeometry(0x3ab2, PaintCanvas::gCanvas, false);
        AbyssEngine::Transform *transform = static_cast<AbyssEngine::Transform *>(
                PaintCanvas::gCanvas->TransformGetTransform(this->jumpFlash->transform));
        transform->SetAnimationState(
                static_cast<AbyssEngine::AnimationMode>(1), nullptr);

        this->egoJumpPos = this->player->getPosition();
        Vector direction = static_cast<AEGeometry *>(this->player->geometry)->getDirection();
        direction = direction * 3000.0f;
        this->egoJumpPos += direction;
        this->jumpFlash->setPosition(this->egoJumpPos);

        Vector jumpDirection =
                static_cast<AEGeometry *>(this->player->geometry)->getDirection();
        this->jumpFlash->setScaling(2.0f, 2.0f, 2.0f);
        Vector up = {0.0f, 1.0f, 0.0f};
        this->jumpFlash->setDirection(jumpDirection, up);

        Vector cameraOffset = {-2000.0f, 300.0f, -2000.0f};
        Vector rotatedOffset = AbyssEngine::AEMath::MatrixRotateVector(
                static_cast<Player *>(this->player->player)->transformMatrix,
                cameraOffset);
        this->egoJumpPos += rotatedOffset;

        Globals::sound->stop(this->player->field_0x1c);
        Globals::sound->stop(0x23);
        Globals::sound->stop(0x8d5);
        Globals::sound->stop(0x8d4);
        Globals::sound->play(0x20, nullptr, nullptr, 0.0f);
    } else {
        Array<KIPlayer *> *landmarks = this->level->getLandmarks();
        this->egoJumpPos = (*landmarks)[1]->getPosition();
        this->egoJumpPos.z -= 10000.0f;
        this->player->setPosition(
                this->egoJumpPos.x, this->egoJumpPos.y, this->egoJumpPos.z);
        this->player->setComputerControlled(true);
        static_cast<AEGeometry *>(this->player->geometry)->setRotation(
                0.0f, 0.0f, 0.0f);
        this->egoJumpPos.x -= 2000.0f;
        this->egoJumpPos.y += 300.0f;
        this->egoJumpPos.z += 4000.0f;
    }
    this->camera->setPosition(
            this->egoJumpPos.x, this->egoJumpPos.y, this->egoJumpPos.z);
}

void TFC_setRotationAroundTarget(TargetFollowCamera *c, int v);

int TFC_hideShipForFirstPersonCam(TargetFollowCamera * c);

void MGame::switchCamera(int id) {
    int turretArg;
    int savedMode;

restart:

    savedMode = this->cameraMode;
    if (id == 2) id += 1;
    this->cameraMode = id;
    if (id == 1) {
        if (this->player->isDockingToAsteroid() != 0) {
            this->turretMode = 0;
        } else {
            int t = this->player->setTurretMode(true);
            this->turretMode = (uint8_t) t;
            if (t != 0) {
                id = this->cameraMode;
                turretArg = 1;
                goto afterTurret;
            }
        }
        id = this->cameraMode + 1;
        this->cameraMode = id;
        turretArg = 0;
    } else {
        this->turretMode = 0;
        turretArg = 0;
    }

afterTurret:
    if (id == 2) {
        this->cameraMode = 3;
    } else if (id >= 4) {
        this->cameraMode = 0;
    }
    this->field_0x18 = 0;
    this->player->setTurretMode(turretArg);

    int mode = this->cameraMode;
    switch (mode) {
        case 0:

            if (this->player->isDockedToDockingPoint() != 0) {
                id = 1;
                goto restart;
            }
            this->levelScript->resetCamera(this->level);
            TFC_setRotationAroundTarget(this->camera, 0);
            this->player->setFreeLookMode(0);
            {
                Engine *eng = (Engine *) this->applicationManager->GetEngine();
                eng->field_0x360 = 0;
            }
            goto firstPerson;
        case 1:
        case 3: {
            if (this->player->isDockedToDockingPoint() != 0)
                TFC_setLookAtCam(this->camera, 0);
            this->levelScript->lookBehind();
            TFC_setRotationAroundTarget(this->camera, 1);
            this->player->setFreeLookMode(1);
            goto firstPerson;
        }
        case 2: {
            this->levelScript->resetCamera(this->level);
            TFC_setRotationAroundTarget(this->camera, 0);
            this->player->setFreeLookMode(0);
            if (savedMode == 1)
                this->field_0x18 = 1;
            goto firstPerson;
        }
        default:
            goto firstPerson;
    }

firstPerson: {
        TFC_enableFirstPersonCam(this->camera, this->cameraMode == 2);
        PlayerEgo *ego = this->player;
        int v;
        if (this->field_0x18 == 0)
            v = TFC_hideShipForFirstPersonCam(this->camera);
        else
            v = 1;
        PlayerEgo_hideShipRaw(ego, v);
    }
}


void MGame::freeCamTouchBegin(int x, int y, void *idPtr) {
    int id = (int) (intptr_t) idPtr;
    Vector finger;
    Vector *fingerSlot;
    if (this->touch0Id == 0) {
        if (this->touch1Id == 0) this->menuTime = 0;
        this->touch0Id = id;
        finger.x = (float) x;
        finger.y = (float) y;
        finger.z = 0.0f;
        fingerSlot = &this->freeCamFinger1;
    } else {
        if (this->menuTime >= 1000) goto tail;
        this->flCameraRoll = 0;
        float len = AbyssEngine::AEMath::VectorLength(*TFC_getCamOffset(this->camera));
        this->flCameraRoll = len;
        finger.x = (float) x;
        finger.y = (float) y;
        finger.z = 0.0f;
        fingerSlot = &this->freeCamFinger0;
        this->touch1Id = id;
    }

    *fingerSlot = finger;
tail:
    this->dragLastX = x;
    this->dragLastY = y;
    this->dragStartX = x;
    this->dragStartY = y;
    this->dragDeltaX = 0;
    this->dragDeltaY = 0;
    this->freeCamDragging = 1;
}


static GameText **g_gameText;

void MGame::useCloak() {
    if (this->player->toggleCloaking() != 0) return;
    if (this->choiceWindow == 0)
        this->choiceWindow = new ChoiceWindow();
    Item *eq = Status::gStatus->getShip()->getFirstEquipmentOfSort(0x15);
    int attr = eq == 0 ? 0 : eq->getAttribute(0x26);
    ChoiceWindow *cw = this->choiceWindow;
    {
        const String *text = Globals::gameText->getText(0x247);
        String space(" ", false);
        String prefix = *text + space;
        String amount(attr);
        String withAmount = prefix + amount;
        String period(".", false);
        String message = withAmount + period;
        cw->set(message);
    }
    this->pauseOpen = 1;
    this->choiceWindowOpen = 1;
    this->pauseSounds();
}


void MGame::gameOverCheck() {
    int64_t *elapsed = reinterpret_cast<int64_t *>(&this->elapsedTime);
    if (this->player->getHitpoints() <= 0) {
        if (this->player->tryToStartEmergencySystem() != 0)
            return;

        if (this->player->isInWormhole()) {
            this->gameOverActive = 1;
            this->gameOverTitle = *Globals::gameText->getText(319);
            this->needsRedraw = 1;
        } else {
            this->player->setTurretMode(0);
            this->levelScript->resetCamera(this->level);
            this->player->setFreeLookMode(0);
            this->camera->enableFirstPersonCam(false);
            this->player->hideShipForFirstPersonCameraView(0);
            this->needsRedraw = 1;
            this->player->explode();
            if (this->player->explosionEnded() != 0) {
                this->gameOverActive = 1;
                this->gameOverTitle = *Globals::gameText->getText(319);
            }
        }

        if (this->gameOverActive != 0) {
            if (Status::gStatus->getMission() != 0 &&
                Status::gStatus->getMission()->isCampaignMission() != 0) {
                const int campaignMission = Status::gStatus->getCurrentCampaignMission();
                int failCount;
                if (campaignMission == Globals::lastCampaignMissionFailed) {
                    failCount = Globals::lastCampaignMissionFailCount + 1;
                } else {
                    Globals::lastCampaignMissionFailCount = 0;
                    failCount = 1;
                }
                Globals::lastCampaignMissionFailed = campaignMission;
                Globals::lastCampaignMissionFailCount = failCount;
            }
            this->gameRecord = static_cast<int>(reinterpret_cast<intptr_t>(
                    static_cast<RecordHandler *>(Globals::recordHandler)->recordStoreRead(
                            Globals::lastRecordWritten)));
        }
    }

    if (this->level->checkGameOver(this->levelScript->field_0x8) != 0) {
        if (this->dialogueWindow == 0) {
            this->dialogueWindow = new DialogueWindow();
            if (this->level != 0)
                this->dialogueWindow->setLevel(this->level);
        } else if (this->dialogueWindow->hasLevel() == 0) {
            if (this->level != 0)
                this->dialogueWindow->setLevel(this->level);
        }
        this->dialogueWindow->set(Status::gStatus->getMission(), 2, -1);
        this->cutsceneActive = 1;
        this->pauseSounds();
        this->pauseOpen = 1;
    }

    if (*elapsed >= 5001) {
        *elapsed = 0;
        if (this->levelScript->m_nTimeLimit >= 1 &&
            this->levelScript->m_nTimeLimit < this->levelScript->scriptTime &&
            (Status::gStatus->getCurrentCampaignMission() == 42 ||
             (this->level->objectivesA != 0 &&
              !this->level->objectivesA->isSurvivalObjective()))) {
            if (this->dialogueWindow == 0) {
                this->dialogueWindow = new DialogueWindow();
                if (this->level != 0)
                    this->dialogueWindow->setLevel(this->level);
            } else if (this->dialogueWindow->hasLevel() == 0) {
                if (this->level != 0)
                    this->dialogueWindow->setLevel(this->level);
            }
            this->dialogueWindow->set(Status::gStatus->getMission(), 2, -1);
            *reinterpret_cast<uint16_t *>(&this->pauseOpen) = 0x0101;
            this->pauseSounds();
        }
    }

    if (this->gameOverActive != 0) {
        *elapsed = 0;
        this->loadingTime = 0;
        Globals::sound->play(0x25, 0, 0, 0.0f);
    }
}

int ApplicationManager_GetApplicationData();

// Application-data block returned (as int handle) by ApplicationManager_GetApplicationData();
// bytes at +5 / +0xc gate touch input while modal/transition is active.
struct MGameAppData {
    uint8_t _pad0[5];
    uint8_t modalActive;   // offset 0x05
    uint8_t _pad6[6];
    uint8_t transitionActive; // offset 0x0c
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(MGameAppData, modalActive) == 5, "MGameAppData::modalActive @ 5");
static_assert(offsetof(MGameAppData, transitionActive) == 0xc, "MGameAppData::transitionActive @ 0xc");
#endif


static Layout ***g_tbStarLayout;

static int **g_tbRecordTrack;

static int **g_tbMenuTrack;

// Android 2.0.16 read-only ship capability table at 0x206ca4. Campaign 139
// uses value 1 together with item 190 to decide whether planet docking may
// continue. Keep the raw values until the wider ship-table owner is named.
static const int g_mgame_ship_capability_206ca4[64] = {
    3, 0, 8, 3, 2, 0, 3, 0, 9, 1, 0, 8, 2, 0, 0, 0,
    2, 0, 2, 3, 3, 2, 0, 8, 8, 8, 0, 0, 0, 8, 3, 2,
    8, 0, 0, 2, 0, 0, 0, 1, 0, 1, 1, 2, 1, 3, 3, 3,
    3, 1, 1, 0, 8, 1, 1, 0, 3, 2, 0, 0, 8, 1, 3, 1,
};

// Android 2.0.16 table at 0x206da4. OnUpdate uses these campaign gates for
// the delayed status tutorial radio message (message family 27).
static const int g_mgame_campaign_thresholds_206da4[3] = {93, 111, 143};

void MGame::OnTouchBegin(int p1, int p2, void *touchId) {
    if (this->activeTouchId == 0)
        this->activeTouchId = touchId;

    if (this->pauseOpen != 0) {
        if (this->starMapOpen != 0) {
            Layout *hl = Globals::layout;
            if (*(uint8_t *) hl != 0) {
                hl->OnTouchBegin(p1, p2);
                __asm__ volatile("" ::: "memory");
                return;
            }
            this->starMapOpen = this->starMap->OnTouchBegin(p1, p2) ^ 1;
            return;
        }
        if (this->autopilotMenuOpen != 0 || this->choiceWindowOpen != 0 ||
            this->field_0xc1 != 0) {
            this->choiceWindow->OnTouchBegin(p1, p2);
            __asm__ volatile("" ::: "memory");
            return;
        }
        if (this->cutsceneActive != 0) {
            this->dialogueWindow->OnTouchBegin(p1, p2);
            __asm__ volatile("" ::: "memory");
            return;
        }
        if (this->menuTouchOpen != 0) {
            MGameAppData *ad = reinterpret_cast<MGameAppData *>(
                static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationData());
            if (ad->modalActive != 0 || ad->transitionActive != 0)
                return;
            this->menuWindow->OnTouchBegin(p1, p2, touchId);
            if (this->freeCamMode != 0 && !this->menuWindow->isShowingMessage() &&
                !this->menuWindow->isMakingScreenshot())
                freeCamTouchBegin(p1, p2, touchId);
            return;
        }
    } else if (this->gameOverActive != 0 && this->loadingTime >= 4000) {
        if (Status::gStatus->getCurrentCampaignMission() == 0x9e) {
            this->active = 0;
            this->applicationManager->SetCurrentApplicationModule(2);
            return;
        }
        GameRecord *record = reinterpret_cast<GameRecord *>(this->gameRecord);
        if (record == nullptr) {
            static_cast<Globals *>(Globals::globals)->playMusicAndFadeOutCurrent(2);
            this->active = 0;
            this->applicationManager->SetCurrentApplicationModule(1);
        } else {
            record->load();
            static_cast<Globals *>(Globals::globals)->playMusicAndFadeOutCurrent(0);
            this->active = 0;
            this->applicationManager->SetCurrentApplicationModule(5);
            return;
        }
    }

    unsigned actions = this->hud->touchBegin(p1, p2, touchId);
    this->hudTouchFlags = actions;

    if ((actions & 0x10) != 0 && this->jumpDriveActive == 0) {
        KIPlayer *stationTarget = this->radar->lockedStation;
        KIPlayer *planetTarget = this->radar->lockedPlanetTarget;
        if (stationTarget != nullptr && Status::gStatus->inAlienOrbit() == 0 &&
            this->player->isDockedToDockingPoint() == 0) {
            if (!this->player->isAutoPilot() && !this->player->isInTurretMode()) {
                this->player->setAutoPilot(stationTarget);
                int eventId;
                if (stationTarget == (*this->level->getLandmarks())[0])
                    eventId = 10;
                else if (stationTarget == (*this->level->getLandmarks())[3])
                    eventId = 15;
                else
                    eventId = 12;
                this->hud->hudEvent(eventId, this->player, 0);
                this->hudTouchFlags = this->hud->touchEnd(p1, p2, touchId);
                return;
            }
            if (!this->player->isInTurretMode()) {
                this->hud->hudEvent(6, this->player, 0);
                this->player->setAutoPilot(0);
                this->radar->lockedStation = 0;
                this->radar->candidateStation = 0;
                this->player->resetGunDelay();
            }
            goto docking_target_path;
        }

        if (planetTarget == nullptr || this->player->isDockingToPlanet() ||
            this->player->isDockedToDockingPoint()) {
            if (this->player->isMining() || this->player->isDockedToDockingPoint())
                goto docking_target_path;

            if (this->radar->getLockedAsteroid() == nullptr ||
                this->player->isDockingToAsteroid()) {
                if (this->player->isDockingToAsteroid()) {
                    this->hud->hudEvent(6, this->player, 0);
                    this->player->dockToAsteroid(this->radar->getLockedAsteroid(), this->radar);
                    this->hud->releaseAllKeys();
                    return;
                }
                this->player->dockToAsteroid(this->radar->getLockedAsteroid(), this->radar);
                goto docking_target_path;
            }

            if (Status::gStatus->getShip()->getFreeSpace() > 0) {
                this->hud->hudEvent(11, this->player, 0);
                this->player->dockToAsteroid(this->radar->getLockedAsteroid(), this->radar);
                this->hud->releaseAllKeys();
                return;
            }
            this->hud->hudEvent(27, this->player, 0);
            return;

docking_target_path:
            if (this->player->isMining() == 0) {
                KIPlayer *dockTarget = static_cast<KIPlayer *>(this->radar->dockTargetPtr);
                if (dockTarget != nullptr && dockTarget->field_0x70 != 0)
                    goto inspect_docking_point;
                if (this->player->isDockedToDockingPoint() != 0) {
                    if (dockTarget != nullptr)
                        goto inspect_docking_point;
                    goto docking_state_path;
                }
                goto general_tap_path;

inspect_docking_point:
                if (this->player->isDockingToDockingPoint() == 0 &&
                    this->player->isDockedToDockingPoint() == 0 &&
                    dockTarget->field_0x75 != 0) {
                    this->hud->hudEvent(34, this->player, 0);
                    this->player->dockToDockingPoint(dockTarget, this->radar);
                    goto release_docking_keys;
                }

docking_state_path:
                if (this->player->isDockingToDockingPoint() == 0 &&
                    (this->player->isDockedToDockingPoint() == 0 ||
                     this->player->isInTurretMode() != 0))
                    goto after_docking_point;
                if (this->player->isDockedToDockingPoint() == 0) {
                    if (this->player->isDockingToDockingPoint() != 0 &&
                        this->player->isLandingOrTakingOff() == 0)
                        this->hud->hudEvent(6, this->player, 0);
                    goto after_docking_point;
                }
                this->player->dockToDockingPoint(dockTarget, this->radar);
                this->player->setAutoPilot(0);
                this->player->resetGunDelay();

release_docking_keys:
                this->hud->releaseAllKeys();

after_docking_point:
                this->hud->enableFireForTutorial(false);
                int64_t now = Status::gStatus->getPlayingTime();
                int64_t last = (static_cast<int64_t>(this->lastTapTimeHigh) << 32) |
                               static_cast<uint32_t>(this->lastTapTime);
                if (now - last > 249) {
                    if (this->_b5c != 0)
                        this->hudTouchFlags = this->hud->touchEnd(p1, p2, touchId);
                    this->_b5c = 0;
                } else {
                    this->_b5c = 1;
                    this->hud->enableFireForTutorial(true);
                }
                this->lastTapTime = static_cast<int>(now);
                this->lastTapTimeHigh = static_cast<int>(now >> 32);
                return;
            }

general_tap_path:
            if (this->player->isMining() != 0) {
                Status::gStatus->field_124 = 0;
                this->player->stopMining();
                return;
            }
            this->hud->enableFireForTutorial(false);
            {
                int64_t now = Status::gStatus->getPlayingTime();
                int64_t last = (static_cast<int64_t>(this->lastTapTimeHigh) << 32) |
                               static_cast<uint32_t>(this->lastTapTime);
                if (now - last > 249) {
                    if (this->_b5c != 0)
                        this->hudTouchFlags = this->hud->touchEnd(p1, p2, touchId);
                    this->_b5c = 0;
                } else {
                    this->_b5c = 1;
                    this->hud->enableFireForTutorial(true);
                }
                this->lastTapTime = static_cast<int>(now);
                this->lastTapTimeHigh = static_cast<int>(now >> 32);
            }
            this->field_0x110 = 1;
            return;
        }

        if (Status::gStatus->getCurrentCampaignMission() < 10 ||
            Status::gStatus->getCurrentCampaignMission() == 48) {
            this->hud->hudEvent(21, this->player, 0);
            return;
        }

        if (Status::gStatus->getCurrentCampaignMission() == 24) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation()) {
                if (Status::gStatus->getShip()->getFirstEquipmentOfSort(13) == nullptr ||
                    Status::gStatus->getShip()->getFirstEquipmentOfSort(17) == nullptr) {
                    this->pauseOpen = 1;
                    pauseSounds();
                    delete this->dialogueWindow;
                    this->dialogueWindow = new DialogueWindow(
                        GameText::gGameText->getText(532),
                        GameText::gGameText->getText(1603), mgame_dialogue_face_1603);
                    this->cutsceneActive = 1;
                    return;
                }
            }
        }

        if (Status::gStatus->getCurrentCampaignMission() == 135) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation() &&
                Status::gStatus->getShip()->getFirstEquipmentOfSort(19) == nullptr) {
                this->pauseOpen = 1;
                pauseSounds();
                if (this->choiceWindow == nullptr)
                    this->choiceWindow = new ChoiceWindow();
                this->choiceWindow->set(*GameText::gGameText->getText(3213));
                this->needsRedraw = 1;
                this->choiceWindowOpen = 1;
                return;
            }
        }

        if (Status::gStatus->getCurrentCampaignMission() == 91) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation() &&
                Status::gStatus->getShip()->getMaxPassengers() <= 9) {
                this->pauseOpen = 1;
                pauseSounds();
                if (this->choiceWindow == nullptr)
                    this->choiceWindow = new ChoiceWindow();
                this->choiceWindow->set(*GameText::gGameText->getText(3214));
                this->needsRedraw = 1;
                this->choiceWindowOpen = 1;
                return;
            }
        }

        if (Status::gStatus->getCurrentCampaignMission() == 94) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation() &&
                Status::gStatus->getShip()->getMaxPassengers() == 0) {
                this->pauseOpen = 1;
                pauseSounds();
                if (this->choiceWindow == nullptr)
                    this->choiceWindow = new ChoiceWindow();
                this->choiceWindow->set(*GameText::gGameText->getText(3214));
                this->needsRedraw = 1;
                this->choiceWindowOpen = 1;
                return;
            }
        }

        if (Status::gStatus->getCurrentCampaignMission() == 105) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation() &&
                Status::gStatus->getShip()->hasEquipment(206, 1) == 0) {
                this->pauseOpen = 1;
                pauseSounds();
                if (this->choiceWindow == nullptr)
                    this->choiceWindow = new ChoiceWindow();
                this->choiceWindow->set(*GameText::gGameText->getText(3217));
                this->needsRedraw = 1;
                this->choiceWindowOpen = 1;
                return;
            }
        }

        if (Status::gStatus->getCurrentCampaignMission() == 139) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation()) {
                if (Status::gStatus->getShip()->getIndex() != 42 &&
                    (g_mgame_ship_capability_206ca4[
                         Status::gStatus->getShip()->getIndex()] != 1 ||
                     Status::gStatus->getShip()->hasEquipment(190, 1) == 0)) {
                    this->pauseOpen = 1;
                    pauseSounds();
                    if (this->choiceWindow == nullptr)
                        this->choiceWindow = new ChoiceWindow();
                    this->choiceWindow->set(*GameText::gGameText->getText(3215));
                    this->needsRedraw = 1;
                    this->choiceWindowOpen = 1;
                    return;
                }
            }
        }

        if (Status::gStatus->getCurrentCampaignMission() == 142) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation() &&
                (Status::gStatus->getShip()->getFirstEquipmentOfSort(33) == nullptr ||
                 Status::gStatus->getShip()->hasEquipment(197, 15) == 0 ||
                 Status::gStatus->getShip()->getFirstEquipmentOfSort(35) == nullptr)) {
                this->pauseOpen = 1;
                pauseSounds();
                if (this->choiceWindow == nullptr)
                    this->choiceWindow = new ChoiceWindow();
                this->choiceWindow->set(*GameText::gGameText->getText(3216));
                this->needsRedraw = 1;
                this->choiceWindowOpen = 1;
                return;
            }
        }

        if (Status::gStatus->getCurrentCampaignMission() == 142) {
            int dockIndex = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(
                static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
            if (dockIndex == campaign->getTargetStation() &&
                Status::gStatus->getShip()->getFreeSpace() == 0) {
                this->pauseOpen = 1;
                pauseSounds();
                if (this->choiceWindow == nullptr)
                    this->choiceWindow = new ChoiceWindow();
                this->choiceWindow->set(*GameText::gGameText->getText(3218));
                this->needsRedraw = 1;
                this->choiceWindowOpen = 1;
                return;
            }
        }

        this->camera->setLookAtCam(true);
        this->player->dockToPlanet();
        this->field_0x110 = 0;
        this->_b5c = 0;
        this->hud->enableFireForTutorial(false);
        reinterpret_cast<uint8_t *>(this->levelScript)[17] = 1;
        this->jumpActive = 1;
        return;

        /* old condensed planet/docking path */
#if 0
            this->hud->enableFireForTutorial(false);
            int64_t now = Status::gStatus->getPlayingTime();
            if (now - ((int64_t) this->lastTapTimeHigh << 32 | (uint32_t) this->lastTapTime) > 249) {
                if (this->_b5c != 0)
                    this->hudTouchFlags = this->hud->touchEnd(p1, p2, touchId);
                this->_b5c = 0;
            } else {
                this->_b5c = 1;
                this->hud->enableFireForTutorial(true);
            }
            this->lastTapTime = (int) now;
            this->lastTapTimeHigh = (int) (now >> 32);
            if (this->player->isMining()) {
                Status::gStatus->field_124 = 0;
                this->player->stopMining();
                return;
            }
            this->field_0x110 = 1;
            return;
        }

        int missionId = Status::gStatus->getCurrentCampaignMission();
        if (missionId < 10 || missionId == 48) {
            this->hud->hudEvent(21, this->player, 0);
            return;
        }

        Mission *campaignMission = reinterpret_cast<Mission *>(
            static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
        bool atMissionPlanet = this->radar->getPlanetDockIndex() ==
                               campaignMission->getTargetStation();
        int blockingText = -1;
        if (missionId == 24 && atMissionPlanet &&
            (Status::gStatus->getShip()->getFirstEquipmentOfSort(13) == 0 ||
             Status::gStatus->getShip()->getFirstEquipmentOfSort(17) == 0)) {
            if (this->dialogueWindow != 0)
                delete this->dialogueWindow;
            this->dialogueWindow = new DialogueWindow(GameText::gGameText->getText(532),
                GameText::gGameText->getText(1603), mgame_dialogue_face_1603);
            this->pauseOpen = 1;
            this->cutsceneActive = 1;
            pauseSounds();
            return;
        }
        if (missionId == 135 && atMissionPlanet &&
            Status::gStatus->getShip()->getFirstEquipmentOfSort(19) == 0)
            blockingText = 3213;
        else if (missionId == 91 && atMissionPlanet &&
                 Status::gStatus->getShip()->getMaxPassengers() <= 9)
            blockingText = 3214;
        else if (missionId == 94 && atMissionPlanet &&
                 Status::gStatus->getShip()->getMaxPassengers() != 0)
            blockingText = 3214;
        else if (missionId == 105 && atMissionPlanet &&
                 !Status::gStatus->getShip()->hasEquipment(206, 1))
            blockingText = 3217;
        else if (missionId == 139 && atMissionPlanet &&
                 Status::gStatus->getShip()->getIndex() != 42 &&
                 !Status::gStatus->getShip()->hasEquipment(190, 1))
            blockingText = 3215;
        else if (missionId == 142 && atMissionPlanet &&
                 (Status::gStatus->getShip()->getFirstEquipmentOfSort(33) == 0 ||
                  !Status::gStatus->getShip()->hasEquipment(197, 15) ||
                  Status::gStatus->getShip()->getFirstEquipmentOfSort(35) == 0))
            blockingText = 3216;
        else if (missionId == 142 && atMissionPlanet &&
                 Status::gStatus->getShip()->getFreeSpace() == 0)
            blockingText = 3218;

        if (blockingText >= 0) {
            this->pauseOpen = 1;
            pauseSounds();
            if (this->choiceWindow == 0)
                this->choiceWindow = new ChoiceWindow();
            this->choiceWindow->set(*GameText::gGameText->getText(blockingText));
            this->needsRedraw = 1;
            this->choiceWindowOpen = 1;
            return;
        }

        this->camera->setLookAtCam(true);
        this->player->dockToPlanet();
        this->field_0x110 = 0;
        this->_b5c = 0;
        this->hud->enableFireForTutorial(false);
        this->levelScript->m_nFlags |= 0x100;
        this->jumpActive = 1;
        return;
#endif
    }

    if ((actions & 0x20) != 0) {
        int64_t now = Status::gStatus->getPlayingTime();
        int64_t last = ((int64_t) this->lastAlignTimeHigh << 32) |
                       (uint32_t) this->lastAlignTime;
        if (now - last <= 499)
            this->player->alignToHorizon();
        this->lastAlignTime = (int) now;
        this->lastAlignTimeHigh = (int) (now >> 32);
        if (this->jumpActive != 0 || this->activeTouchId == touchId || actions != 0x20)
            return;
        this->thrustActive = 1;
        *reinterpret_cast<float *>(&this->field_0x1bc) = static_cast<float>(p1);
        this->thrustStartYFloat = static_cast<float>(p2);
        return;
    }

    if ((actions & 1) != 0) {
        Globals::sound->play(0x7c, 0, 0, 0.0f);
        return;
    }

    if ((actions & 0x100) != 0 &&
        (this->player->isAutoPilot() || this->player->isDockingToAsteroid() ||
         this->player->isDockingToDockingPoint())) {
        if (static_cast<Radar *>(this->player->field_0x14)->field_0x54 == 0 &&
            !this->player->aboutToReachAutoTarget()) {
            this->flFastForwardWeight = 5.0f;
            this->camera->setFastForwardMode(true);
            return;
        }
    }

    if (this->cloakAttribute > 0 && this->timeWarpState >= 0 &&
        (this->hudTouchFlags & 0x8000) != 0) {
        if (this->timeWarpState < 1) {
            this->hud->setTimeExtender(true, false, true, true);
            Globals::sound->setDownPitch(true);
            this->timeWarpState = this->cloakAttribute;
            Globals::sound->play(0x460, 0, 0, 0.0f);
        } else {
            this->timeWarpState = -1;
            Globals::sound->setDownPitch(false);
            this->hud->setTimeExtender(true, false, false, false);
            Globals::sound->play(0x45f, 0, 0, 0.0f);
        }
        return;
    }

    if (this->cameraMode == 3) {
        if (!this->player->isMining() && this->jumpActive == 0)
            freeCamTouchBegin(p1, p2, touchId);
        return;
    }
    if (this->cameraMode <= 1) {
        this->maneuverActive = 1;
        this->maneuverStartX = p1;
        this->maneuverStartY = p2;
        this->maneuverHoldTime = 0;
        if (this->jumpActive == 0 && actions == 0) {
            this->thrustActive = 1;
            *reinterpret_cast<float *>(&this->field_0x1bc) = static_cast<float>(p1);
            this->thrustStartYFloat = static_cast<float>(p2);
        }
    }
}

void MGame::OnUpdate() {
    int delta;
    if ((int) this->applicationManager->GetElapsedTimeMillis() <= 150 &&
        (int) this->applicationManager->GetElapsedTimeMillis() < 0)
        delta = 0;
    else if ((int) this->applicationManager->GetElapsedTimeMillis() <= 150)
        delta = (int) this->applicationManager->GetElapsedTimeMillis();
    else
        delta = 150;

    this->deltaTime = delta;
    *reinterpret_cast<int64_t *>(&this->frameTime) += delta;
    *reinterpret_cast<int64_t *>(&this->field_0x38) += delta;

    void *keyboardTouch = reinterpret_cast<void *>(12345);
    if (this->freeCamMode == 0)
        goto keyboard_done;
    if (GetKeyState(const_cast<char *>("Right")) != 0) {
        freeCamTouchBegin(400, 400, keyboardTouch);
        freeCamTouchMove(396, 400, keyboardTouch);
        freeCamTouchEnd(396, 400, keyboardTouch);
        goto keyboard_vertical;
    }
    if (this->freeCamMode == 0)
        goto keyboard_done;
    if (GetKeyState(const_cast<char *>("Left")) != 0) {
        freeCamTouchBegin(400, 400, keyboardTouch);
        freeCamTouchMove(404, 400, keyboardTouch);
        freeCamTouchEnd(404, 400, keyboardTouch);
    }
keyboard_vertical:
    if (this->freeCamMode == 0)
        goto keyboard_done;
    if (GetKeyState(const_cast<char *>("Down")) != 0) {
        freeCamTouchBegin(400, 400, keyboardTouch);
        freeCamTouchMove(400, 404, keyboardTouch);
        freeCamTouchEnd(400, 404, keyboardTouch);
        goto keyboard_done;
    }
    if (this->freeCamMode != 0 && GetKeyState(const_cast<char *>("Up")) != 0) {
        freeCamTouchBegin(400, 400, keyboardTouch);
        freeCamTouchMove(400, 396, keyboardTouch);
        freeCamTouchEnd(400, 396, keyboardTouch);
    }
keyboard_done:

    int mouseX = Globals::mouseDeltaX;
    int mouseY = Globals::mouseDeltaY;
    if (this->cameraMode == 3 && Globals::mouseCursorActivated != 0 &&
        Globals::is_hacking_visible == 0 && (mouseX != 0 || mouseY != 0)) {
        freeCamTouchBegin(400, 400, keyboardTouch);
        freeCamTouchMove((mouseX - this->mouseDeltaSnapshotX) / 2 + 400,
                         (mouseY - this->mouseDeltaSnapshotY) / 2 + 400, keyboardTouch);
        freeCamTouchEnd(400, 400, keyboardTouch);
        Globals::mouseDeltaX = 0;
        Globals::mouseDeltaY = 0;
        mouseX = 0;
        mouseY = 0;
    }
    this->mouseDeltaSnapshotX = mouseX;
    this->mouseDeltaSnapshotY = mouseY;
    if (this->player->isDockingToAsteroid()) {
        Globals::mouseDeltaX = 0;
        Globals::mouseDeltaY = 0;
    }

    if (Globals::showWingmanMenu != 0) {
        if (!this->player->isMining() && Status::gStatus->getWingmen() != 0) {
            if (this->hudMenuOpen != 0) {
                this->hudMenuOpen = 0;
                this->hud->closeHudMenu();
                this->pauseOpen = 0;
                resumeSounds();
            } else {
                this->hudMenuOpen = 1;
                this->pauseOpen ^= 1;
                pauseSounds();
                this->hud->initHudMenu(2, this->level);
            }
        }
        Globals::showWingmanMenu = 0;
    }

    if (this->pauseOpen != this->pauseSnapshot) {
        if (this->pauseOpen != 0) {
            this->player->PauseEngineSound();
            Array<KIPlayer *> *enemies = this->level->getEnemies();
            if (enemies != 0)
                for (unsigned int i = 0; i < enemies->size(); ++i)
                    (*enemies)[i]->KIPlayer::PauseEngineSound();
        } else {
            this->player->ResumeEngineSound();
            Array<KIPlayer *> *enemies = this->level->getEnemies();
            if (enemies != 0)
                for (unsigned int i = 0; i < enemies->size(); ++i)
                    (*enemies)[i]->KIPlayer::ResumeEngineSound();
        }
        this->pauseSnapshot = this->pauseOpen;
    }

    if ((this->pauseOpen == 0 ||
         (this->freeCamMode != 0 && !this->menuWindow->isShowingMessage() &&
          !this->menuWindow->isMakingScreenshot())) && this->cameraMode == 3) {
        if (this->freeCamMode != 0)
            this->level->update(0, this->jumpActive != 0);
        AbyssEngine::Engine *engine =
            static_cast<AbyssEngine::Engine *>(this->applicationManager->GetEngine());
        float wheel = engine->inputWheel;
        if ((wheel < 0.0f ? -wheel : wheel) > 0.2f) {
            this->flCameraRoll -= wheel * 50.0f;
            if (this->flCameraRoll > 20000.0f)
                this->flCameraRoll = 20000.0f;
            else if (this->flCameraRoll < 1500.0f)
                this->flCameraRoll = 1500.0f;
            this->camera->zoomTarget(this->flCameraRoll);
        }
        engine = static_cast<AbyssEngine::Engine *>(
            this->applicationManager->GetEngine());
        engine->inputWheel *= 0.9f;
    }

    if (this->pauseOpen == 0) {
    if (this->timeWarpState != 0) {
        if (this->timeWarpState < 0) {
            this->timeWarpState -= this->deltaTime;
            if (this->timeWarpState < -this->cloakAttributeMax) {
                this->timeWarpState = 0;
                this->hud->setTimeExtender(true, true, true, false);
            }
        } else {
            if (this->timeWarpState <= this->deltaTime) {
                Globals::sound->setDownPitch(false);
                this->hud->setTimeExtender(true, false, false, false);
                Globals::sound->play(0x45f, 0, 0, 0.0f);
            }
            this->timeWarpState -= this->deltaTime;
            if (this->timeWarpState == 0)
                this->timeWarpState = -1;
            else if (this->timeWarpState > 0) {
                this->field_0x44 = (int) ((float) this->deltaTime * 0.7f);
                this->deltaTime = (int) ((float) this->deltaTime * 0.3f);
                this->hud->setTimeExtender(true, false, true, true);
            }
        }
    }

    Status::gStatus->incPlayingTime(this->deltaTime);
    if (this->radar->field_0x54 != 0 || this->player->aboutToReachAutoTarget() ||
        this->radio->isShowingMessage()) {
        this->flFastForwardWeight = 1.0f;
        this->camera->setFastForwardMode(false);
        reinterpret_cast<uint8_t *>(this->player)[0x84] = 1;
    } else {
        this->deltaTime = (int) (this->flFastForwardWeight * (float) this->deltaTime);
    }
    int64_t previousScriptTime = this->levelScript->scriptTime;
    this->levelScript->scriptTime += this->deltaTime;
    if (previousScriptTime <= 5000 && this->levelScript->scriptTime >= 5001 &&
        this->player->hasVolatileGoods())
        this->hud->hudEvent(45, this->player, 0);
    if (this->jumpActive == 0) {
        *reinterpret_cast<int64_t *>(&this->elapsedTime) += this->deltaTime;
        float wheel = static_cast<AbyssEngine::Engine *>(
            this->applicationManager->GetEngine())->inputWheel;
        if (this->cameraMode != 3 && !this->player->isDockingToAsteroid() &&
            !this->player->isDockedToAsteroid() && (wheel >= 1.0f || wheel <= -1.0f)) {
            this->boostTouchDuration += wheel;
            static_cast<AbyssEngine::Engine *>(
                this->applicationManager->GetEngine())->field_0x360 = 0;
            if (this->boostTouchDuration > 100.0f)
                this->boostTouchDuration = 100.0f;
            else if (this->boostTouchDuration < 0.0f)
                this->boostTouchDuration = 0.0f;
            this->player->setThrust(this->boostTouchDuration / 100.0f);
            this->boostTouchThrust = 1.0f - this->boostTouchDuration / 100.0f;
            this->player->throttleChanged();
        }
        if ((this->player->isDockingToAsteroid() || this->player->isDockedToAsteroid()) &&
            this->player->getThrust() < 1.0f) {
            this->boostTouchDuration = 100.0f;
            this->player->setThrust(1.0f);
            static_cast<AbyssEngine::Engine *>(
                this->applicationManager->GetEngine())->field_0x360 = 0;
        }
        if (this->player->isDockedToAsteroid() && this->cameraMode != 0)
            switchCamera(0);
    }

    Globals::layout->update(this->deltaTime);
    Status::gStatus->field_0x30 -= this->deltaTime;
    if (!mgame_full_effects_enabled())
        Globals::sound->updateAll(0, 0, 0, 0);

    bool wasDockedToMiningPlant = this->player->isDockedToMiningPlant();
    if (this->loadingTime < 4000) {
        int playerDelta = this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime;
        this->player->update(playerDelta, this->radar, this->hud, this->radio,
                             this->levelScript, this->field_0xd8,
                             this->jumpActive != 0, this->cameraMode);
    }
    bool wasRocketControl = this->player->isInRocketControl();
    this->level->update(this->deltaTime, this->jumpActive != 0);
    if (wasRocketControl && !this->player->isInRocketControl())
        switchCamera(0);
    if (!wasDockedToMiningPlant && this->player->isDockedToMiningPlant() &&
        this->choiceWindowOpen == 0) {
        if (this->choiceWindow == 0)
            this->choiceWindow = new ChoiceWindow();

        String message = *GameText::gGameText->getText(290) + String("\n", false);
        int requiredYield = Status::gStatus->hardCoreMode() ? 100 : 30;
        message = Status::gStatus->replaceHash(
            String(message, false), String(requiredYield), String("#A", false));
        bool listedOre = false;
        Array<Item *> *cargo = Status::gStatus->getShip()->getCargo();
        if (cargo != 0) {
            for (unsigned int i = 0; i < cargo->size(); ++i) {
                Item *item = (*cargo)[i];
                if (item != 0 && item->getSort() == 23) {
                    message += String("\n", false) + String(item->getAmount()) +
                               String("t ", false) +
                               *GameText::gGameText->getText(item->getIndex() + 1274);
                    listedOre = true;
                }
            }
        }
        if (!listedOre)
            message += String("\n", false) + *GameText::gGameText->getText(286);
        this->choiceWindow->set(message, true);
        this->pauseOpen = 1;
        this->needsRedraw = 1;
        pauseSounds();
        this->choiceWindowOpen = 1;
        this->cargoConversionChoiceOpen = 1;
    }

    if (mgame_full_effects_enabled()) {
        unsigned cameraId = PaintCanvas::gCanvas->CameraGetCurrent();
        Matrix cameraLocal = *static_cast<Matrix *>(
            PaintCanvas::gCanvas->CameraGetLocal(cameraId));
        Vector listenerVelocityOrigin =
            AbyssEngine::AEMath::MatrixGetPosition(cameraLocal);
        Vector listenerVelocity =
            ((float) Player::velocity *
             (listenerVelocityOrigin - this->soundListenerPreviousPosition)) /
            (float) this->deltaTime;
        Vector listenerForward = -AbyssEngine::AEMath::MatrixGetDir(cameraLocal);
        Vector listenerPosition = AbyssEngine::AEMath::MatrixGetPosition(cameraLocal);
        Vector listenerUp = AbyssEngine::AEMath::MatrixGetUp(cameraLocal);
        Globals::sound->updateAll(&listenerPosition, &listenerForward,
                                  &listenerUp, &listenerVelocity);
        this->soundListenerPreviousPosition =
            AbyssEngine::AEMath::MatrixGetPosition(cameraLocal);
    }
    }

    if (this->jumpActive != 0 || this->player->getHitpoints() < 1)
        goto core_update;

    if (this->pauseOpen != 0) {
        if (this->menuWindow != 0 && this->hudMenuOpen == 0 &&
            this->cutsceneActive == 0 && this->orbitMenuOpen == 0 &&
            this->autopilotMenuOpen == 0 && this->field_0xc6 == 0 &&
            this->choiceWindowOpen == 0 && this->touchesStream == 0) {
            this->menuWindow->update(this->deltaTime);
            if (this->freeCamMode != 0) {
                this->camera->setRumblePercentage(0.0f, 0);
                this->menuTime += this->deltaTime;
                if (this->freeCamDragging == 0) {
                    this->flShakePhaseX = this->flShakeAmpX * this->flShakePhaseX;
                    if ((this->flShakePhaseX < 0.0f ? -this->flShakePhaseX
                                                    : this->flShakePhaseX) > 1.0f)
                        this->flShakeX += this->flShakePhaseX;
                    this->flShakePhaseY = this->flShakeAmpY * this->flShakePhaseY;
                    if ((this->flShakePhaseY < 0.0f ? -this->flShakePhaseY
                                                    : this->flShakePhaseY) > 1.0f) {
                        this->flShakeY += this->flShakePhaseY;
                        if (this->flShakeY > 200.0f)
                            this->flShakeY = 200.0f;
                        else if (this->flShakeY < -200.0f)
                            this->flShakeY = -200.0f;
                    }
                }
                this->camera->rotateAroundTarget(this->flShakeY * -0.005f,
                                                 this->flShakeX * -0.005f, 0.0f);
                this->camera->update(this->deltaTime);
            }
            if (this->menuWindow->isInMissionScreen())
                Status::gStatus->incPlayingTime(this->deltaTime);
        }
        if (this->active != 0 && this->orbitMenuOpen == 0 &&
            this->autopilotMenuOpen == 0 && this->field_0xc6 == 0) {
            if (this->choiceWindowOpen != 0)
                this->choiceWindow->update(this->deltaTime);
            else if (this->hudMenuOpen == 0) {
                if (this->starMapOpen != 0) {
                    Status::gStatus->incPlayingTime(this->deltaTime);
                    this->starMap->update(this->deltaTime);
                } else if (this->cutsceneActive != 0) {
                    Globals::layout->update(this->deltaTime);
                    this->dialogueWindow->update(this->deltaTime);
                }
            }
        }
        return;
    }

    {
    if (this->player->isDockingToPlanet()) {
        if (Status::gStatus->getCurrentCampaignMission() == 24) {
            int planet = this->radar->getPlanetDockIndex();
            Mission *campaign = reinterpret_cast<Mission *>(static_cast<intptr_t>(
                Status::gStatus->getCampaignMission()));
            if (planet == campaign->getTargetStation() &&
                (Status::gStatus->getShip()->getFirstEquipmentOfSort(13) == 0 ||
                 Status::gStatus->getShip()->getFirstEquipmentOfSort(17) == 0)) {
                this->pauseOpen = 1;
                pauseSounds();
                delete this->dialogueWindow;
                DialogueWindow *dialogue = new DialogueWindow(
                    GameText::gGameText->getText(532),
                    GameText::gGameText->getText(1603), mgame_dialogue_face_1603);
                this->cutsceneActive = 1;
                this->dialogueWindow = dialogue;
                this->player->stopPlanetDock();
                this->player->setAutoPilot(0);
                return;
            }
        }
    }
    if (this->player->isDockedToPlanet()) {
        Station *destination = reinterpret_cast<Station *>(static_cast<intptr_t>(
            Galaxy::gGalaxy->getStation(this->radar->getPlanetDockIndex())));
        Status::gStatus->departStation(destination);
        Level::setInitStreamOut();
        Status::gStatus->field_64 =
            static_cast<Player *>(this->player->player)->getHitpoints();
        Status::gStatus->field_5c =
            static_cast<Player *>(this->player->player)->getShieldHP();
        Status::gStatus->field_60 =
            static_cast<Player *>(this->player->player)->getArmorHP();
        Status::gStatus->field_68 =
            static_cast<Player *>(this->player->player)->getGammaHP();
        Status::gStatus->field_f4 = this->player->getCurrentSecondaryWeaponIndex();
        Globals::switch_to_target_setting = 1;
        this->active = 0;
        this->applicationManager->SetCurrentApplicationModule(2);
        __asm__ volatile("" ::: "memory");
        return;
    }

    if (this->player->shouldSwitchToStandardCam())
        switchCamera(0);
    if (this->player->shouldSwitchToFreeLookCam()) {
        switchCamera(3);
        this->flShakeX = -0.175f;
        this->flShakeY = 0.49f;
        this->flCameraRoll = 15000.0f;
        this->camera->rotateAroundTarget(-0.175f, 0.49f, 0.0f);
        this->camera->zoomTarget(this->flCameraRoll);
    }

    if (this->jumpActive != 0) {
        if (updateJumpScene())
            return;
    } else if (this->player->isChargingDrive() && this->player->driveReady()) {
        startJumpScene();
        return;
    }

    bool processEvents =
        (!this->levelScript->startSequenceOver() && this->levelScript->startSequence()) ||
        this->autopilotMenuOpen != 0 || this->field_0xc6 != 0;
    if (!processEvents)
        processEvents = dockEvent(0, 0) == 0 && this->starMapOpen == 0;
    if (!processEvents)
        return;

    if (this->elapsedTime < 5001)
        goto core_update;

    if (this->levelScript->scriptTime >= 5001 &&
        this->jumpDriveActive == 0 && this->cutsceneActive == 0) {

    int64_t &lastAmbient = *reinterpret_cast<int64_t *>(
        &Status::gStatus->field_0x100);

    if (Status::gStatus->getCurrentCampaignMission() == 23 &&
        !this->player->isMining() && Status::gStatus->getMission()->isEmpty() &&
        Status::gStatus->getPlayingTime() - lastAmbient > 3600000) {
        int variant = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        mgame_show_ambient_dialogue(this, variant ? 534 : 533, 1602,
                                    mgame_dialogue_face_1602, variant ? 465 : 464);
        goto ambient_message_complete;
    }
    if (Status::gStatus->getCurrentCampaignMission() == 24 &&
        !this->player->isMining() && Status::gStatus->getMission()->isEmpty() &&
        Status::gStatus->getPlayingTime() - lastAmbient > 3600000) {
        int variant = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        mgame_show_ambient_dialogue(this, variant ? 536 : 535, 1603,
                                    mgame_dialogue_face_1603, variant ? 468 : 467);
        goto ambient_message_complete;
    }
    if (Status::gStatus->getCurrentCampaignMission() >= 122 &&
        Status::gStatus->getCurrentCampaignMission() <= 124 &&
        !this->player->isMining() && Status::gStatus->getMission()->isEmpty() &&
        Status::gStatus->getPlayingTime() - lastAmbient > 3600000 &&
        this->jumpDriveActive == 0 && this->usingJumpDrive == 0) {
        int variant = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        int text = Status::gStatus->getCurrentCampaignMission() == 122 ? 3164 : 3166;
        this->level->createRadioMessage(28, text + variant);
        goto ambient_message_complete;
    }

    if (!Status::gStatus->inAlienOrbit() && Status::gStatus->field_0x17c != 0 &&
        Status::gStatus->getPlayingTime() - lastAmbient >= 12001 &&
        this->jumpDriveActive == 0 &&
        this->usingJumpDrive == 0) {
        int thresholdIndex = 0;
        unsigned int messageIndex = static_cast<unsigned int>(-1);
        while (thresholdIndex != 3) {
            if (Status::gStatus->getCurrentCampaignMission() >=
                g_mgame_campaign_thresholds_206da4[thresholdIndex])
                ++messageIndex;
            ++thresholdIndex;
        }
        if (messageIndex < 3) {
            Status::gStatus->field_0x17c = 0;
            this->level->createRadioMessage(27, messageIndex);
        }
    }
    goto ambient_dispatch_complete;

ambient_message_complete:
    lastAmbient = Status::gStatus->getPlayingTime();

ambient_dispatch_complete:

    if (!mgame_hint_booster && Status::gStatus->getShip()->hasBooster() &&
        Status::gStatus->getCurrentCampaignMission() >= 2) {
        if (this->choiceWindow == 0)
            this->choiceWindow = new ChoiceWindow();
        String text = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(
            *GameText::gGameText->getText(596));
        this->choiceWindow->set(text);
        this->needsRedraw = 1;
        this->choiceWindowOpen = 1;
        this->pauseOpen = 1;
        pauseSounds();
        mgame_hint_booster = 1;
        return;
    } else if (!mgame_hint_jump_drive && Status::gStatus->getShip()->hasJumpDrive()) {
        if (this->choiceWindow == 0)
            this->choiceWindow = new ChoiceWindow();
        String text = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(
            *GameText::gGameText->getText(590));
        this->choiceWindow->set(text);
        this->needsRedraw = 1;
        this->choiceWindowOpen = 1;
        this->pauseOpen = 1;
        pauseSounds();
        mgame_hint_jump_drive = 1;
        return;
    } else if (!mgame_hint_cloak && Status::gStatus->getShip()->hasCloak()) {
        if (this->choiceWindow == 0)
            this->choiceWindow = new ChoiceWindow();
        String text = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(
            *GameText::gGameText->getText(593));
        this->choiceWindow->set(text);
        this->needsRedraw = 1;
        this->choiceWindowOpen = 1;
        this->pauseOpen = 1;
        pauseSounds();
        mgame_hint_cloak = 1;
        return;
    } else if (!mgame_hint_autopilot && !this->player->isAutoPilot() &&
               Status::gStatus->getCurrentCampaignMission() >= 10 &&
               Status::gStatus->getMission()->isEmpty()) {
        if (this->choiceWindow == 0)
            this->choiceWindow = new ChoiceWindow();
        String text = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(
            *GameText::gGameText->getText(584));
        this->choiceWindow->set(text);
        this->needsRedraw = 1;
        this->choiceWindowOpen = 1;
        this->pauseOpen = 1;
        pauseSounds();
        mgame_hint_autopilot = 1;
        return;
    } else if (!mgame_hint_wingman && Status::gStatus->wingmen != 0) {
        mgame_show_choice(this, 637, true);
        mgame_hint_wingman = 1;
        return;
    }
    Standing *standing = reinterpret_cast<Standing *>(static_cast<intptr_t>(
        Status::gStatus->getStanding()));
    if (!mgame_hint_standing && standing->isEnemyWithAnyone()) {
        mgame_show_choice(this, 648);
        mgame_hint_standing = 1;
        return;
    }
    if (!this->field_0x1e6 && Status::gStatus->getCurrentCampaignMission() == 5) {
        mgame_show_choice(this, 619);
        this->field_0x1e6 = 1;
        return;
    }
    if (!this->campaignDamageHintShown && !this->player->isMining() &&
        Status::gStatus->getCurrentCampaignMission() == 2) {
        Player *playerShip = static_cast<Player *>(this->player->player);
        if (playerShip->getHitpoints() < playerShip->getMaxHitpoints() ||
            this->levelScript->scriptTime >= 40001) {
            this->campaignDamageHintShown = 1;
            this->pauseOpen = 1;
            pauseSounds();
            if (this->choiceWindow == 0)
                this->choiceWindow = new ChoiceWindow();
            String message(*GameText::gGameText->getText(1725), false);
            message = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(message);
            this->choiceWindow->set(message);
            this->needsRedraw = 1;
            this->choiceWindowOpen = 1;
            return;
        }
    }
    if (!mgame_hint_mining_intro && this->player->isMining()) {
        mgame_show_choice(this, 620);
        mgame_hint_mining_intro = 1;
        return;
    }
    if (!mgame_hint_mining_followup && this->player->isMining() &&
        Status::gStatus->getCurrentCampaignMission() >= 4) {
        mgame_show_choice(this, 621);
        mgame_hint_mining_followup = 1;
        return;
    }
    if (!mgame_hint_mining_lost &&
        Status::gStatus->getCurrentCampaignMission() == 2 &&
        this->player->lostMiningGame()) {
        mgame_show_choice(this, 617);
        mgame_hint_mining_lost = 1;
        return;
    }
    if (!mgame_hint_cargo_full && this->hud->cargoFull() &&
        Status::gStatus->getCurrentCampaignMission() >= 7) {
        mgame_show_choice(this, 636);
        mgame_hint_cargo_full = 1;
        return;
    }

    if (!mgame_hint_vossk_first && !Status::gStatus->inAlienOrbit() &&
        Status::gStatus->getSystem()->getRace() == 2) {
        this->level->createRadioMessage(5, 0);
        mgame_hint_vossk_first = 1;
        this->field_0xe0 = 1;
        return;
    }
    if (mgame_hint_vossk_first && !mgame_hint_vossk_second &&
        this->field_0xe0 == 0 && !Status::gStatus->inAlienOrbit() &&
        Status::gStatus->getSystem()->getRace() == 2) {
        this->level->createRadioMessage(6, 0);
        mgame_hint_vossk_second = 1;
        return;
    }

    if (!mgame_hint_scanner &&
        Status::gStatus->getShip()->getFirstEquipmentOfSort(13) != 0 &&
        !this->player->isMining() && Status::gStatus->getMission()->isEmpty() &&
        Status::gStatus->getCurrentCampaignMission() == 24) {
        mgame_hint_scanner = 1;
        mgame_show_ambient_dialogue(this, 1912, 1603, mgame_dialogue_face_1603, 0x1cf);
        __asm__ volatile("" ::: "memory");
        return;
    }
    if (!mgame_hint_hacking && this->player->isHacking()) {
        mgame_show_choice(this, 599);
        mgame_hint_hacking = 1;
        return;
    }

    if (!mgame_hint_gamma) {
        bool gammaWarning = false;
        if (Status::gStatus->getCurrentCampaignMission() == 91 &&
            Status::gStatus->getStation()->getIndex() == 110) {
            gammaWarning = static_cast<RadioMessage *>(
                (*this->level->getMessages())[4])->isOver();
        } else if (Status::gStatus->getCurrentCampaignMission() != 91) {
            union {
                int bits;
                float value;
            } gammaRate = {Status::gStatus->getGammaRayDamagePerSecond(
                Status::gStatus->getStation()->getIndex(),
                Status::gStatus->getCurrentCampaignMission())};
            gammaWarning = gammaRate.value > 0.0f;
        }
        if (gammaWarning) {
            mgame_show_choice(this, 600);
            mgame_hint_gamma = 1;
            return;
        }
    }
    if (!mgame_hint_volatile && this->player->hasVolatileGoods()) {
        mgame_show_choice(this, 602);
        mgame_hint_volatile = 1;
        return;
    }

    if (!mgame_hint_campaign91_a &&
        Status::gStatus->getCurrentCampaignMission() == 91 &&
        Status::gStatus->getStation()->getIndex() == 110 &&
        static_cast<RadioMessage *>((*this->level->getMessages())[4])->isOver()) {
        mgame_hint_campaign91_a = 1;
        mgame_show_choice(this, 603, true);
        return;
    }
    if (!mgame_hint_campaign91_b &&
        Status::gStatus->getCurrentCampaignMission() == 91 &&
        Status::gStatus->getStation()->getIndex() == 110 &&
        this->player->isDockedToDockingPoint() &&
        Status::gStatus->missionPassengerCount == 10 &&
        static_cast<RadioMessage *>((*this->level->getMessages())[6])->isOver()) {
        mgame_hint_campaign91_b = 1;
        mgame_show_choice(this, 606, true);
        return;
    }
    if (!mgame_hint_campaign92 &&
        Status::gStatus->getCurrentCampaignMission() == 92 &&
        Status::gStatus->getStation()->getIndex() == 113 &&
        this->player->isDockedToDockingPoint() &&
        Status::gStatus->missionPassengerCount == 0) {
        mgame_hint_campaign92 = 1;
        mgame_show_choice(this, 609, true);
        return;
    }

    if (Globals::iap_hack_dlc1Bought && !mgame_hint_game_won && this->cutsceneActive == 0 &&
        !this->player->isMining() && Status::gStatus->getMission()->isEmpty() &&
        !this->player->isAutoPilot() && Status::gStatus->gameWon()) {
        mgame_hint_game_won = 1;
        this->pauseOpen = 1;
        pauseSounds();
        delete this->dialogueWindow;
        this->dialogueWindow = new DialogueWindow();
        Mission *ending = new Mission(160, 0, -1);
        this->dialogueWindow->set(ending, 1, 46);
        __asm__ volatile("" ::: "memory");
        this->needsRedraw = 1;
        this->cutsceneActive = 1;
        Status::gStatus->nextCampaignMission(true);
        Status::gStatus->nextCampaignMission(true);
        return;
    }
    if (Globals::iap_hack_dlc3Bought && !mgame_hint_dlc1_won && this->cutsceneActive == 0 &&
        !this->player->isMining() && Status::gStatus->getMission()->isEmpty() &&
        !this->player->isAutoPilot() && Status::gStatus->dlc1Won()) {
        mgame_hint_dlc1_won = 1;
        this->pauseOpen = 1;
        pauseSounds();
        delete this->dialogueWindow;
        this->dialogueWindow = new DialogueWindow();
        Mission *ending = new Mission(160, 0, -1);
        this->dialogueWindow->set(ending, 1, 85);
        __asm__ volatile("" ::: "memory");
        this->needsRedraw = 1;
        this->cutsceneActive = 1;
        Status::gStatus->nextCampaignMission(true);
        Status::gStatus->nextCampaignMission(true);
        return;
    }

    if (Status::gStatus->inBlackMarketSystem() &&
        Status::gStatus->field_110 == 0 && Status::gStatus->field_0x111 == 0 &&
        this->field_0xd4 == 0 && this->level->getMessages() != 0 &&
        static_cast<RadioMessage *>((*this->level->getMessages())[0])->isOver()) {
        int cargoValue = 0;
        Array<Item *> *cargo = Status::gStatus->getShip()->getCargo();
        if (cargo != 0) {
            for (unsigned int i = 0; i < cargo->size(); ++i) {
                Item *item = (*cargo)[i];
                if (item != 0)
                    cargoValue += item->getSinglePrice() * item->getAmount();
            }
        }
        if (cargoValue == 0)
            cargoValue = 100;
        float difficulty = reinterpret_cast<float *>(Globals::options)[11];
        float percentage = difficulty <= 0.0f ? 0.02f :
                           difficulty == 0.5f ? 0.05f :
                           difficulty == 1.0f ? 0.10f : 0.15f;
        this->choiceItemCount = (int) (percentage * (float) cargoValue);
        String message = Status::gStatus->replaceHash(
            String(*GameText::gGameText->getText(448), false),
            String(Globals::layout->formatCredits(this->choiceItemCount), false), String("#C", false));
        message = Status::gStatus->replaceHash(
            String(message, false), String((int) (percentage * 100.0f)),
            String("#P", false));
        this->pauseOpen = 1;
        pauseSounds();
        if (this->choiceWindow == 0)
            this->choiceWindow = new ChoiceWindow();
        this->choiceWindow->set(message, true);
        this->needsRedraw = 1;
        this->choiceWindowFlags = 0x101;
        this->field_0xd4 = 1;
        goto mission_event_check;
    }

    if (Status::gStatus->field_114 == 0 && !Status::gStatus->inAlienOrbit() &&
        Status::gStatus->getStation()->getIndex() == 108) {
        Status::gStatus->field_114 = 1;
        mgame_show_ambient_dialogue(this, 457, 1601, mgame_dialogue_face_1601, 0x5bc);
        __asm__ volatile("" ::: "memory");
        return;
    }

    }

mission_event_check:
    if (this->elapsedTime >= 5001 && this->cutsceneActive == 0) {
        if (!Status::gStatus->getMission()->hasFailed() &&
            !Status::gStatus->getMission()->hasWon())
            dialogueEvent();
    }
    }

core_update:
    if (this->player->boosting() && !this->player->isDockingToPlanet()) {
        static_cast<AbyssEngine::Engine *>(this->applicationManager->GetEngine())->
            SetPostEffect(0x1400002, true);
        float rawBoost = this->player->getBoostPercentage();
        float boost = 0.0f;
        if (rawBoost > 0.0f)
            boost = rawBoost;
        static_cast<AbyssEngine::Engine *>(this->applicationManager->GetEngine())->
            field_0x3c8 = *reinterpret_cast<uint32_t *>(&boost);
        Vector screenPosition = {0.0f, 0.0f, 0.0f};
        Vector playerPosition = this->player->getPosition();
        PaintCanvas::gCanvas->GetScreenPosition(playerPosition, screenPosition);
        screenPosition[0] /= (float) static_cast<AbyssEngine::Engine *>(
            this->applicationManager->GetEngine())->GetDisplayWidth();
        screenPosition[1] = 1.0f -
            screenPosition[1] / (float) static_cast<AbyssEngine::Engine *>(
                this->applicationManager->GetEngine())->GetDisplayHeight();
        *reinterpret_cast<Vector *>(reinterpret_cast<unsigned char *>(
            this->applicationManager->GetEngine()) + 0x3cc) = screenPosition;
        float fov = this->player->getBoostPercentage() * 0.35f + 1.22f;
        this->field_0x70 = *(int *) &fov;
        PaintCanvas::gCanvas->CameraSetPerspective(this->cameraId, fov, 20.0f,
                                                   MGame_cameraFarPlane());
        float cameraBoost = 0.0f;
        if (this->player->getBoostPercentage() >= 0.0f)
            cameraBoost = this->player->getBoostPercentage();
        this->camera->setBoostPercentage(cameraBoost, this->player->getBoostSpeed());
    }

    if (this->gameOverActive != 0) {
        if (this->elapsedTime >= 3001)
            this->loadingTime += this->deltaTime;
    } else {
        if (!this->player->isDead() && !Status::gStatus->getMission()->hasFailed()) {
            if (successCheck())
                return;
        }
        gameOverCheck();
    }

    bool wasJumpActive = this->jumpActive != 0;
    this->jumpActive = this->levelScript->process(this->deltaTime);
    if (!wasJumpActive && this->jumpActive) {
        this->field_0x1dd = AbyssEngine::Engine::UseAdvancedShader;
        AbyssEngine::Engine::UseAdvancedShader = 1;
        if (this->timeWarpState > 0) {
            Globals::sound->setDownPitch(false);
            this->hud->setTimeExtender(this->hud->field_0x0 != 0,
                                       false, false, false);
        }
        this->touch0Id = 0;
        this->touch1Id = 0;
        this->menuTime = 0;
        this->freeCamDragging = 0;
        this->_b5c = 0;
        this->field_0x110 = 0;
        this->hud->enableFireForTutorial(false);
        if (this->player->boosting())
            this->player->stopBoost();
        this->field_0x70 = 0x3f9c28f6;
        PaintCanvas::gCanvas->CameraSetPerspective(this->cameraId, 1.22f, 20.0f,
                                                   MGame_cameraFarPlane());
        this->camera->setBoostPercentage(0.0f, 0);
        static_cast<AbyssEngine::Engine *>(this->applicationManager->GetEngine())->
            SetPostEffect(0x1400002, false);
    } else if (wasJumpActive && !this->jumpActive) {
        AbyssEngine::Engine::UseAdvancedShader = this->field_0x1dd;
    }

    if (!this->levelScript->m_bStartSequenceOver) {
        if (this->levelScript->m_bStartSequence) {
            this->elapsedTime = 5001;
            this->elapsedTimeHigh = 0;
        }
        if (this->cameraMode == 3) {
            if (this->freeCamDragging == 0) {
                this->flShakePhaseX = this->flShakeAmpX * this->flShakePhaseX;
                if ((this->flShakePhaseX < 0.0f ? -this->flShakePhaseX
                                                : this->flShakePhaseX) > 1.0f)
                    this->flShakeX += this->flShakePhaseX;
                this->flShakePhaseY = this->flShakeAmpY * this->flShakePhaseY;
                if ((this->flShakePhaseY < 0.0f ? -this->flShakePhaseY
                                                : this->flShakePhaseY) > 1.0f)
                    this->flShakeY += this->flShakePhaseY;
                if (this->flShakeY > 200.0f)
                    this->flShakeY = 200.0f;
                else if (this->flShakeY < -200.0f)
                    this->flShakeY = -200.0f;
            }
            this->camera->rotateAroundTarget(this->flShakeY * -0.005f,
                                             this->flShakeX * -0.005f, 0.0f);
        }
        int cameraDelta = this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime;
        if (this->flFastForwardWeight <= 1.0f) {
            this->camera->update(cameraDelta);
        } else {
            for (int i = 0; i < 5; ++i) {
                int step = this->timeWarpState < 1
                    ? (int) ((float) this->deltaTime / this->flFastForwardWeight)
                    : this->field_0x44;
                this->camera->update(step);
            }
        }
        if (this->maneuverActive)
            this->maneuverHoldTime += this->deltaTime;
        if (this->cameraMode == 2) {
            Player *shipPlayer = static_cast<Player *>(this->player->player);
            this->camera->setFirstPersonMatrix(shipPlayer->transformMatrix);
            if (!this->field_0x1a5 && this->camera->hideShipForFirstPersonCam()) {
                this->field_0x1a5 = 1;
                Globals::sound->enableReverb(1);
            }
        } else if (this->field_0x1a5 && !this->camera->hideShipForFirstPersonCam()) {
            Globals::sound->disableReverb();
            this->field_0x1a5 = 0;
        }
        this->player->hideShipForFirstPersonCameraView(
            this->field_0x18 != 0 || this->camera->hideShipForFirstPersonCam());

        if (!this->gameOverActive && !this->jumpActive && !this->player->isDead()) {
            if (Level::doInstantJump != 0 && this->levelScript->scriptTime >= 5001) {
                Level::doInstantJump = 0;
                this->usingJumpDrive = 1;
                startChargingJumpDrive();
            }
            if ((this->field_0x5c != 0 || this->field_0x110 != 0 || this->hud->firePressed()) &&
                !this->player->isDockingToPlanet()) {
                this->player->shoot(this->deltaTime, 0);
                this->menuOpen = 1;
            } else if (this->menuOpen) {
                this->menuOpen = 0;
                this->player->stopShooting(0);
            }

            if (Status::gStatus->getCurrentCampaignMission() != 48) {
                if (Globals::options[0x11] == 0) {
                    handleAccelerometer();
                } else {
                    if (this->hud->getAnalogX() < 0.0f) {
                        this->player->left(
                            this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime,
                            this->hud->getAnalogX() * this->hud->getAnalogX());
                    } else if (this->hud->getAnalogX() > 0.0f) {
                        this->player->right(
                            this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime,
                            this->hud->getAnalogX() * this->hud->getAnalogX());
                    }
                    float y = this->hud->getAnalogY();
                    if (mgame_invert_y()) {
                        if (y < 0.0f) {
                            this->player->down(
                                this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime,
                                this->hud->getAnalogY() * this->hud->getAnalogY());
                        } else if (this->hud->getAnalogY() > 0.0f) {
                            this->player->up(
                                this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime,
                                this->hud->getAnalogY() * this->hud->getAnalogY());
                        }
                    } else if (y < 0.0f) {
                        this->player->up(
                            this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime,
                            this->hud->getAnalogY() * this->hud->getAnalogY());
                    } else if (this->hud->getAnalogY() > 0.0f) {
                        this->player->down(
                            this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime,
                            this->hud->getAnalogY() * this->hud->getAnalogY());
                    }
                }
            }
        }
        this->field_0x110 = 0;

        if (this->player->isInWormhole()) {
            Mission *mission = Status::gStatus->getMission();
            if (mission->isCampaignMission()) {
                int campaign = Status::gStatus->getCurrentCampaignMission();
                if (campaign == 29 || campaign == 41) {
                    static_cast<Player *>(this->player->player)->setHitpoints(0);
                    return;
                }
                if (campaign == 40) {
                    if (this->levelScript->getEvent() <= 3) {
                        static_cast<Player *>(this->player->player)->setHitpoints(0);
                        return;
                    }
                    Status::gStatus->nextCampaignMission(true);
                    Array<KIPlayer *> *enemies = this->level->getEnemies();
                    Level::lastMissionFreighterHitpoints =
                        static_cast<Player *>((*enemies)[0]->player)->getHitpoints();
                } else if (campaign != 42) {
                    Status::gStatus->nextCampaignMission(true);
                }
                this->level->removeObjectives();
                Status::gStatus->setMission((Mission *) Mission::empty);
            }
            Status::gStatus->setMission((Mission *) Mission::empty);

            if (Status::gStatus->inAlienOrbit()) {
                if (Status::gStatus->getCurrentCampaignMission() == 42)
                    return;
                Station *destination = reinterpret_cast<Station *>(static_cast<intptr_t>(
                    Galaxy::gGalaxy->getStation(Status::gStatus->field_84)));
                Status::gStatus->departStation(destination);
            } else {
                Status::gStatus->setStation(Status::gStatus->playerStation);
                Status::gStatus->departStation(Status::gStatus->playerStation);
            }

            Status::gStatus->field_64 =
                static_cast<Player *>(this->player->player)->getHitpoints();
            Status::gStatus->field_5c =
                static_cast<Player *>(this->player->player)->getShieldHP();
            Status::gStatus->field_60 =
                static_cast<Player *>(this->player->player)->getArmorHP();
            Status::gStatus->field_68 =
                static_cast<Player *>(this->player->player)->getGammaHP();
            Status::gStatus->field_f4 = this->player->getCurrentSecondaryWeaponIndex();
            Level::comingFromAlienWorld = 1;
            Level::initStreamOutPosition = 1;
            Level::programmedStation = 0;
            Globals::switch_to_target_setting = 1;
            this->active = 0;
            this->applicationManager->SetCurrentApplicationModule(2);
            __asm__ volatile("" ::: "memory");
            return;
        }
    }
}


void MGame::OnSuspend() {
    if (Globals::recordHandler != 0)
        ((RecordHandler *) Globals::recordHandler)->saveOptions();
    this->pauseSounds();
    if (this->pauseOpen == 0) {
        if (this->menuWindow == 0)
            this->menuWindow = new MenuTouchWindow(1);
        this->pauseSnapshot = 1;
        this->pauseOpen = 1;
        Globals::sound->pauseAllPlaying();
        this->player->PauseEngineSound();
        Array<KIPlayer *> *e = this->level->getEnemies();
        if (e != nullptr) {
            for (uint32_t i = 0; i < e->size(); i++)
                (*e)[i]->KIPlayer::PauseEngineSound();
        }
        MenuTouchWindow *w = this->menuWindow;
        int mode = 1;
        if (this->jumpActive == 0)
            mode = this->player->isDead();
        w->setCutsceneMode(mode);
        this->menuTouchOpen = 1;
    }
    this->hud->releaseAllKeys();
}

int MGame::dockEvent(int p1, int p2) {
    (void) p1;
    (void) p2;
    Vector streamPosition = this->player->getPosition();
    this->touchesStream = this->level->collideStream(streamPosition);
    Vector stationPosition = this->player->getPosition();
    this->touchesStation = this->level->collideStation(stationPosition);

    if (!Status::gStatus->getMission()->isEmpty() &&
        Status::gStatus->getMission()->getType() != 11 &&
        Status::gStatus->getMission()->getType() != 0 &&
        Status::gStatus->getMission()->getType() != 189 &&
        Status::gStatus->getMission()->getType() != 171 &&
        Status::gStatus->getMission()->getType() != 172) {
        if ((!this->touchesStation && !this->touchesStream) ||
            !this->player->isAutoPilot())
            return 0;
        this->hud->hudEvent(21, this->player, 0);
        return 0;
    }

    if (this->touchesStream != 0) {
        if (this->player->goingToStream() && !this->player->isDockingToStream() &&
            !this->player->isDockedToStream()) {
            this->player->dockToStream(true);
            this->freeCamDragging = 0;
            this->needsRedraw = 1;
            return 0;
        }
        if (this->touchesStream == 0)
            goto station_path;

        Status::gStatus->field_64 = static_cast<Player *>(this->player->player)->getHitpoints();
        Status::gStatus->field_5c = static_cast<Player *>(this->player->player)->getShieldHP();
        Status::gStatus->field_60 = static_cast<Player *>(this->player->player)->getArmorHP();
        Status::gStatus->field_68 = static_cast<Player *>(this->player->player)->getGammaHP();
        Status::gStatus->field_f4 = this->player->getCurrentSecondaryWeaponIndex();

        const int isAutoPilot = this->player->isAutoPilot();
        if (Level::programmedStation != 0 && isAutoPilot != 0) {
            if (this->autopilotMenuOpen != 0)
                return 0;
            this->freeCamDragging = 0;
            this->needsRedraw = 1;
            if (this->choiceWindow == 0)
                this->choiceWindow = new ChoiceWindow();
            Station *destination = static_cast<Station *>(Level::programmedStation);
            String message = *GameText::gGameText->getText(574) +
                             String(": ", false) + destination->getName() +
                             String("\n", false) +
                             *GameText::gGameText->getText(421);
            this->choiceWindow->set(message, true);
            this->choiceWindow->left();
            this->pauseOpen = 1;
            this->autopilotMenuOpen = 1;
            pauseSounds();
            this->player->setAutoPilot(0);
            return 0;
        }

        if (this->starMapOpen != 0) {
            this->frameTime = 0;
            this->frameTimeHigh = 0;
            return 1;
        }
        this->player->isAutoPilot();
        if (!this->player->goingToStream())
            return 0;

        if (Level::programmedStation != 0) {
            if (this->autopilotMenuOpen != 0)
                return 0;
            this->freeCamDragging = 0;
            this->needsRedraw = 1;
            if (this->choiceWindow == 0)
                this->choiceWindow = new ChoiceWindow();
            Station *destination = static_cast<Station *>(Level::programmedStation);
            String message = *GameText::gGameText->getText(574) +
                             String(": ", false) + destination->getName() +
                             String("\n", false) +
                             *GameText::gGameText->getText(421);
            this->choiceWindow->set(message, true);
            this->choiceWindow->left();
            this->pauseOpen = 1;
            this->autopilotMenuOpen = 1;
            pauseSounds();
            this->player->setAutoPilot(0);
            return 0;
        }

        this->freeCamDragging = 0;
        this->needsRedraw = 1;
        if (this->starMap == 0)
            this->starMap = new StarMap(false, nullptr, false, -1);
        static_cast<Engine *>(this->applicationManager->GetEngine())->
            SetPostEffect(0x1400002, false);
        this->starMap->initLights();
        this->starMap->setJumpMapMode(true, false);
        this->player->setAutoPilot(0);
        this->pauseOpen = 1;
        this->starMapOpen = 1;
        pauseSounds();
        this->needsRedraw = 1;
        return 1;
    }

station_path:
    if (this->touchesStation == 0) {
        const int autoPilotTarget = this->player->getAutoPilotTarget();
        if (autoPilotTarget != static_cast<int>(reinterpret_cast<intptr_t>(
                                       (*this->level->getLandmarks())[0])) ||
            !this->player->collidesWithStation())
            return 0;
    }

    if (Status::gStatus->getCurrentCampaignMission() >= 49 &&
        Status::gStatus->getCurrentCampaignMission() <= 54 &&
        Status::gStatus->getStation()->getIndex() != 74) {
        this->hud->hudEvent(21, this->player, 0);
        return 0;
    }
    if (this->player->goingToStation() && !Status::gStatus->inAlienOrbit()) {
        if (Status::gStatus->inEmptyOrbit())
            return 0;
        Achievements::gAchievements->checkForNewMedal(this->player);
        Globals::switch_to_target_setting = 0;
        Status::gStatus->field_64 = static_cast<Player *>(this->player->player)->getHitpoints();
        Status::gStatus->field_5c = static_cast<Player *>(this->player->player)->getShieldHP();
        Status::gStatus->field_60 = static_cast<Player *>(this->player->player)->getArmorHP();
        Status::gStatus->field_68 = static_cast<Player *>(this->player->player)->getGammaHP();
        this->applicationManager->SetCurrentApplicationModule(5);
        this->active = 0;
        return 1;
    }
    return 0;
}

int MGame::freeCamTouchEnd(int p1, int p2, void *idPtr) {
    int id = (int) (intptr_t) idPtr;
    (void) p1;
    (void) p2;
    if (this->touch0Id == id) {
        this->touch0Id = 0;
        this->menuTime = 0;
    } else if (this->touch1Id == id) {
        this->touch1Id = 0;
        this->menuTime = 0;
    }
    int absY = this->dragDeltaY;
    int absX = this->dragDeltaX;
    if (absY < 0)
        absY = -absY;
    float nextX = this->flShakeX + (float) this->dragDeltaX;
    float nextY = this->flShakeY + (float) this->dragDeltaY;
    float inertiaY = 0.0f;
    if (absX < 0)
        absX = -absX;
    float inertiaX = 0.0f;
    if (absX > 3)
        inertiaX = (float) this->dragDeltaX;
    if (absY > 3)
        inertiaY = (float) this->dragDeltaY;
    this->needsRedraw = 1;
    this->flShakeAmpX = 0.9f;
    this->flShakeAmpY = 0.9f;
    this->freeCamDragging = 0;
    this->flShakePhaseX = inertiaX;
    this->flShakePhaseY = inertiaY;
    this->flShakeX = nextX;
    this->flShakeY = nextY;
    this->dragRotIntX = (int) nextX;
    this->dragRotIntY = (int) nextY;
    return static_cast<int>(reinterpret_cast<intptr_t>(this));
}


void MGame::UseKhadorDrive() {
    if (this->player->isChargingDrive() != 0)
        return;

    Mission *mission = Status::gStatus->getMission();
    if (!(mission->isEmpty() ||
          Status::gStatus->getCurrentCampaignMission() == 78 ||
          mission->getType() == 11 || mission->getType() == 0 ||
          mission->getType() == 189 || mission->getType() == 13 ||
          mission->getType() == 171 || mission->getType() == 172) ||
        (Status::gStatus->getCurrentCampaignMission() == 65 &&
         !Status::gStatus->inAlienOrbit() &&
         Status::gStatus->getStation()->getIndex() ==
                 reinterpret_cast<Mission *>(static_cast<intptr_t>(
                         Status::gStatus->getCampaignMission()))->getTargetStation())) {
        this->hud->hudEvent(21, this->level->getPlayer(), 0);
        return;
    }

    this->player->resetGunDelay();
    if (Status::gStatus->getCurrentCampaignMission() == 78) {
        Level::programmedStation = Status::gStatus->playerStation;
        this->usingJumpDrive = 1;
        this->startChargingJumpDrive();
        this->pauseOpen = 0;
        this->resumeSounds();
        this->hudMenuOpen = 0;
        this->hud->closeHudMenu();
        Status::gStatus->nextCampaignMission(true);
        return;
    }

    if (Status::gStatus->inAlienOrbit()) {
        if (Status::gStatus->getCurrentCampaignMission() == 80)
            Status::gStatus->field_84 = 100;
        Level::programmedStation = reinterpret_cast<void *>(static_cast<intptr_t>(
                Galaxy::gGalaxy->getStation(Status::gStatus->field_84)));
        this->usingJumpDrive = 1;
        this->startChargingJumpDrive();
        this->pauseOpen = 0;
        this->resumeSounds();
        this->hudMenuOpen = 0;
        this->hud->closeHudMenu();
        return;
    }

    if (this->player->hasVolatileGoods() != 0) {
        this->choiceWindow->set(*Globals::gameText->getText(612));
        this->pauseOpen = 1;
        this->choiceWindowOpen = 1;
        this->needsRedraw = 1;
        this->hudMenuOpen = 0;
        this->hud->closeHudMenu();
        this->pauseSounds();
        this->maneuverActive = 0;
        return;
    }

    if (this->starMap == 0)
        this->starMap = new StarMap(false, nullptr, false, -1);
    static_cast<Engine *>(this->applicationManager->GetEngine())->
            SetPostEffect(0x1400002, false);
    this->starMap->initLights();
    this->usingJumpDrive = 1;
    this->starMap->setJumpMapMode(true, true);
    if (!Status::gStatus->inAlienOrbit())
        this->starMap->askForJumpIntoAlienWorld();
    this->pauseOpen = 1;
    this->starMapOpen = 1;
    this->pauseSounds();
    this->hudMenuOpen = 0;
    this->hud->closeHudMenu();
}


// Android stores the related effects controls in the packed Globals::options
// record: word_2181F7 is options+0xf and flt_218210 is options+0x28.
struct MGameOptionsView {
    uint8_t _pad0[0xf];
    uint8_t fullEffectsEnabled;  // offset 0xf
    uint8_t _pad10[0x18];
    float particleQuality;       // offset 0x28
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(MGameOptionsView, fullEffectsEnabled) == 0xf,
              "MGameOptionsView::fullEffectsEnabled @ 0xf");
static_assert(offsetof(MGameOptionsView, particleQuality) == 0x28,
              "MGameOptionsView::particleQuality @ 0x28");
#endif

static MGameOptionsView *mgame_options() {
    return reinterpret_cast<MGameOptionsView *>(Globals::options);
}

int MGame::OnInitialize() {
    MGame *self = this;
    self->field_0x1dc = 0;
    Level *level = self->level;
    self->loadProgress = 100;

    if (level == 0) {
        const int inAlienOrbit = Status::gStatus->inAlienOrbit();
        PaintCanvas *canvas = self->paintCanvas;
        unsigned texSel;
        if (inAlienOrbit != 0) {
            texSel = 0x2f08;
        } else {
            int ti = Status::gStatus->getSystem()->getTextureIndex();
            texSel = (ti + 0x2efe) & 0xffff;
        }
        unsigned cubeTexture;
        canvas->TextureCreate((unsigned short) texSel, cubeTexture, false);
        self->paintCanvas->ChangeCubeTexture(cubeTexture);

        {
            static_cast<Globals *>(Globals::globals)->startNewSoundResourceList();
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x66);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x68);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x69);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x6a);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x6b);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x67);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x7e);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(5);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x18);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x15);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x12);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x13);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x14);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x1c);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x1d);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x1b);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x25);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x1a);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x2e);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x2f);

            if (Status::gStatus->getWingmen() != 0)
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x30);

            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x3e);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x3d);
            static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x24);

            if (Status::gStatus->getCurrentCampaignMission() < 2) {
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x9c);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x9d);
            }

            if (Status::gStatus->inAlienOrbit() == 0) {
                if (Status::gStatus->getSystem()->currentOrbitHasWarpGate())
                    static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x1f);
            }

            if (Status::gStatus->getCurrentCampaignMission() == 0) {
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x8f);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x9d);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x9e);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0xa1);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0xa0);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x9f);
            } else if (Status::gStatus->getCurrentCampaignMission() == 0xe) {
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0xf);
            } else if (Status::gStatus->getCurrentCampaignMission() == 0x18) {
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x22);
            } else if (Status::gStatus->getCurrentCampaignMission() == 0x1d) {
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0xe);
            } else if (Status::gStatus->getCurrentCampaignMission() == 0x29) {
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x9b);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x99);
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(0x9a);
            }
        }

        Status::gStatus->checkForLevelUp();
        level = new Level(3);
        self->level = level;
    }

    if (((Level *) (level))->init() == 0) {
        return 100;
    }

    ((MGame *) (self))->reset();

    {
        if (Status::gStatus->field_64 >= 0)
            ((Player *) self->player->player)->setHitpoints(Status::gStatus->field_64);
        if (Status::gStatus->field_5c >= 0)
            ((Player *) self->player->player)->setShieldHP(Status::gStatus->field_5c);
        if (Status::gStatus->field_60 >= 0)
            ((Player *) self->player->player)->setArmorHP(Status::gStatus->field_60);
        if (Status::gStatus->field_68 >= 0)
            ((Player *) self->player->player)->setGammaHP(Status::gStatus->field_68);

        self->player->resetLastHP();

        if (Status::gStatus->getCurrentCampaignMission() != 0x5f) {
            Status::gStatus->field_64 = Status::gStatus->getShip()->getMaxHP();
            Status::gStatus->field_5c = Status::gStatus->getShip()->getMaxShieldHP();
            Status::gStatus->field_60 = Status::gStatus->getShip()->getMaxArmorHP();
            Status::gStatus->field_68 = 100;

            Status *status = Status::gStatus;
            int stIdx = status->getStation()->getIndex();
            int cm = Status::gStatus->getCurrentCampaignMission();
            union {
                int bits;
                float value;
            } gammaRate = {status->getGammaRayDamagePerSecond(stIdx, cm)};
            if (gammaRate.value == 0.0f)
                ((Player *) self->player->player)->setGammaHP(100);
        }
    }
    if (Status::gStatus->inAlienOrbit() == 0)
        Status::gStatus->field_84 = Station_getIndex(Status::gStatus->getStation());

    uint64_t t = static_cast<ApplicationManager *>(
        Globals::appManager)->GetCurrentTimeMillis();
    self->timeWarpState = 0;
    self->cloakAttribute = 0;
    *reinterpret_cast<uint64_t *>(&self->startTime) = t;
    *reinterpret_cast<uint64_t *>(&self->lastTime) = t;

    if (self->radar->hasScanner() != 0)
        Status::gStatus->field_11c = 0;
    Status::gStatus->field_12c = 0;
    Status::gStatus->field_134 = 0;
    Status::gStatus->field_13c = 0;
    Status::gStatus->field_144 = 0;

    Item *eq = Status::gStatus->getShip()->getFirstEquipmentOfSort(26);
    if (eq != 0) {
        self->cloakAttribute = eq->getAttribute(42);
        self->cloakAttributeMax = eq->getAttribute(43);
        self->hud->setTimeExtender(1, 0, 1, 0);
    }

    if (Status::gStatus->dlc1Won() != 0 && Status::gStatus->inAlienOrbit() != 0 &&
        Status::gStatus->getCurrentCampaignMission() < 0x93) {
        if (self->player != 0 && self->radio != 0)
            self->player->radioRef = self->radio;
        self->level->createRadioMessage(8, 0);
    }

    if (Status::gStatus->inBlackMarketSystem() != 0) {
        if (self->player != 0 && self->radio != 0)
            self->player->radioRef = self->radio;
        if (Status::gStatus->field_110 != 0) {
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr) {
                int n = (int) enemies->size();
                for (int i = 0; i != n; i++) {
                    KIPlayer *e = (*enemies)[i];
                    if (e->shipGroup == 8)
                        e->field_0x25 = 0;
                }
            }
        } else {
            int id;
            if (Status::gStatus->field_0x111 != 0) {
                id = 0xd;
            } else {
                Array<KIPlayer *> *enemies = self->level->getEnemies();
                if (enemies != nullptr) {
                    int n = (int) enemies->size();
                    for (int i = 0; i != n; i++) {
                        KIPlayer *e = (*enemies)[i];
                        if (e->shipGroup == 8)
                            e->field_0x25 = 0;
                    }
                }
                id = 9;
            }
            self->level->createRadioMessage(id, 8);
        }
    } else {
        *reinterpret_cast<uint16_t *>(&Status::gStatus->field_110) = 0;
    }

    {
        if (Status::gStatus->inAlienOrbit() == 0 && Status::gStatus->getCurrentCampaignMission() == 0x7d) {
            Status *freighterStatus = Status::gStatus;
            int stIdx = freighterStatus->getStation()->getIndex();
            if (freighterStatus->isFreighterMissionStation(stIdx) != 0) {
                Mission *m = reinterpret_cast<Mission *>(
                    static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
                int statusVal = ((Mission *) (m))->getStatusValue();
                Status *bitStatus = Status::gStatus;
                int bit = bitStatus->getFreighterMissionStationBit(
                    bitStatus->getStation()->getIndex());
                if ((statusVal & (1 << bit)) == 0) {
                    Mission *target = reinterpret_cast<Mission *>(
                        static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
                    Mission *source = reinterpret_cast<Mission *>(
                        static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
                    int updatedStatus = source->getStatusValue();
                    bitStatus = Status::gStatus;
                    int bit2 = bitStatus->getFreighterMissionStationBit(
                        bitStatus->getStation()->getIndex());
                    target->setStatusValue(updatedStatus | (1 << bit2));
                    if (self->player != 0 && self->radio != 0)
                        self->player->radioRef = self->radio;
                    self->level->createRadioMessage(0x13, 0);
                }
            }
        }

        Array<Item *> *secondary = Status::gStatus->getShip()->getEquipment(1);
        if (secondary != 0) {
            Ship *secondaryShip = Status::gStatus->getShip();
            if (secondaryShip->hasEquipment(Status::gStatus->field_f4, 1) == 0) {
                Item *first = (*secondary)[0];
                if (first != 0) {
                    self->player->setCurrentSecondaryWeaponIndex(((Item *) (first))->getIndex());
                    self->hud->setCurrentSecondaryWeapon((*secondary)[0]);
                }
            } else {
                for (unsigned i = 0; i < secondary->size(); i++) {
                    Item *it = (*secondary)[i];
                    if (it != 0 && ((Item *) (it))->getIndex() == Status::gStatus->field_f4) {
                        PlayerEgo *selectedPlayer = self->level->getPlayer();
                        selectedPlayer->setCurrentSecondaryWeaponIndex(it->getIndex());
                        self->hud->setCurrentSecondaryWeapon(it);
                        break;
                    }
                }
            }
        }

        self->field_0x1a4 = 0;
        const bool fullEffectsEnabled = mgame_options()->fullEffectsEnabled != 0;
        if (!fullEffectsEnabled) {
            if (Status::gStatus->getCurrentCampaignMission() > 1) {
                Vec3 p = self->player->getPosition();
                Globals::sound->play(self->player->field_0x1c,
                                     reinterpret_cast<Vector *>(&p), nullptr, 0.0f);
            }
        } else {
            self->player->PlayEngineSound();
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr)
                for (unsigned i = 0; i < enemies->size(); i++)
                    (*enemies)[i]->KIPlayer::PlayEngineSound();
        }

        self->loadingTime = 0;

        self->level->getStarSystem()->initLight();
        const float particleQuality = mgame_options()->particleQuality;
        self->level->enableParticleEffects(particleQuality >= 0.25f,
                                           particleQuality > 0.7f);

        int fireRateBits = Status::gStatus->getShip()->getFireRateFactor();
        float fireRate;
        __builtin_memcpy(&fireRate, &fireRateBits, sizeof(fireRate));
        if (1.0f - fireRate >= 0.0f)
            self->player->pitchAllPrimaryGuns(1.0f - fireRate);

        if (Status::gStatus->inAlienOrbit() == 0) {
            int idx = ((Station *) (Status::gStatus->getStation()))->getIndex();
            unsigned off = static_cast<unsigned>(idx - 0x6d);
            if (off <= 0x19) {
                if (((1u << off) & 0x3800003u) != 0)
                    goto visit_station;
                if (off == 2) {
                    if (Status::gStatus->getCurrentCampaignMission() < 94)
                        goto station_visited;
                    goto visit_station;
                }
            }
            if (static_cast<unsigned>(idx - 102) >= 3)
                goto station_visited;
visit_station:
            Status::gStatus->getStation()->visit();
        }

station_visited:
        if (Globals::switch_to_target_setting != -1)
            static_cast<Globals *>(Globals::globals)->playMusicAndFadeOutCurrent(Globals::switch_to_target_setting);
        Globals::switch_to_target_setting = -1;
        Globals::sound->setDownPitch(false);

        Engine *eng = (Engine *) self->applicationManager->GetEngine();
        ((Engine *) (eng))->SetPostEffect(0x1400002, false);

        if (Status::gStatus->getCurrentCampaignMission() == 0x9e) {
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr) {
                KIPlayer *first = (*self->level->getEnemies())[0];
                ((PlayerFighter *) (first))->cloak(true, true);
            }
        }

        if (self->menuWindow == 0)
            self->menuWindow = new MenuTouchWindow(1);

        self->missionInfoLines = new Array<AbyssEngine::String *>();
        int font = Globals::font;
        Globals *globals = static_cast<Globals *>(Globals::globals);
        String *text = GameText::gGameText->getText(196);
        globals->getLineArray(font, *text,
                               Globals::w - 2 * Globals::layout->field_0x28,
                               self->missionInfoLines);
        self->active = 1;
    }
    return 0;
}

void TFC_zoomTarget(void *cam, float z);


void MGame::freeCamTouchMove(int x, int y, void *touchId) {
    int id = (int) (intptr_t) touchId;
    if (this->player->isMining() != 0) {
        this->needsRedraw = 1;
        this->freeCamTouchEnd(x, y, touchId);
        return;
    }

    const int touch0 = this->touch0Id;
    this->needsRedraw = 0;
    if (touch0 != 0 && this->touch1Id != 0)
        goto pinch_zoom;

    {
        int dx = x - this->dragLastX;
        int dy = y - this->dragLastY;
        this->dragLastX = x;
        this->dragLastY = y;
        this->dragDeltaX = dx;
        this->dragDeltaY = dy;
        this->flShakeAmpX = 1.0f;
        this->flShakeAmpY = 1.0f;
        this->flShakeX += (float) dx;
        this->flShakeY += (float) dy;

        if (touch0 == 0) {
            Vector point = {(float) x, (float) y, 0.0f};
            this->freeCamFinger0 = point;
            return;
        }
        if (this->touch1Id == 0) {
            Vector point = {(float) x, (float) y, 0.0f};
            this->freeCamFinger1 = point;
            return;
        }
    }

pinch_zoom:
    Vector delta = this->freeCamFinger1 - this->freeCamFinger0;
    float oldLen = AbyssEngine::AEMath::VectorLength(delta);
    float newLen = oldLen;

    if (this->touch0Id == id || this->touch1Id == id) {
        if (this->touch0Id == id) {
            newLen = AbyssEngine::AEMath::VectorLength(
                Vector{(float) x, (float) y, 0.0f} - this->freeCamFinger0);
            this->freeCamFinger1 = Vector{(float) x, (float) y, 0.0f};
        } else {
            newLen = AbyssEngine::AEMath::VectorLength(
                Vector{(float) x, (float) y, 0.0f} - this->freeCamFinger1);
            this->freeCamFinger0 = Vector{(float) x, (float) y, 0.0f};
        }
    }

    float zoom = this->flCameraRoll + (newLen - oldLen) * -50.0f;
    this->flCameraRoll = zoom;
    if (zoom > 20000.0f) {
        zoom = 20000.0f;
        this->flCameraRoll = zoom;
    } else if (zoom < 1500.0f) {
        zoom = 1500.0f;
        this->flCameraRoll = zoom;
    }
    this->camera->zoomTarget(zoom);
}

namespace {

enum MGameHudAction : unsigned int {
    kHudActionPause = 0x00000001,
    kHudActionBoost = 0x00000002,
    kHudActionQuickMenu = 0x00000004,
    kHudActionFire = 0x00000008,
    kHudActionCamera = 0x00000080,
    kHudActionOpenWeaponMenu = 0x00000200,
    kHudActionOpenWingmanMenu = 0x00000400,
    kHudActionCloak = 0x00000800,
    kHudActionJumpDrive = 0x00001000,
    kHudActionSecondary0 = 0x00002000,
    kHudActionSecondary1 = 0x00004000,
    kHudActionSecondary2 = 0x00008000,
    kHudActionSecondary3 = 0x00010000,
    kHudActionWingmenAttack = 0x00020000,
    kHudActionWingmenDefend = 0x00040000,
    kHudActionWingmenFollow = 0x00080000,
    kHudActionWingmenToggle = 0x00100000,
    kHudActionProgrammedStation = 0x00200000,
    kHudActionJumpGate = 0x00400000,
    kHudActionStation = 0x00800000,
    kHudActionAsteroidWaypoint = 0x01000000,
    kHudActionRouteWaypoint = 0x02000000,
    kHudActionDockingTarget0 = 0x04000000,
    kHudActionAutoTurret = 0x20000000,
    kHudActionOrbit = 0x00000040,
};

static __attribute__((always_inline)) inline void
mgame_close_hud_menu(MGame *self, bool closeOrbitMenu) {
    self->hudMenuOpen = 0;
    self->hud->closeHudMenu();
    self->pauseOpen = 0;
    self->resumeSounds();

    if (closeOrbitMenu && self->orbitMenuOpen != 0) {
        self->orbitMenuOpen = 0;
        if (self->player != nullptr)
            self->player->resetGunDelay();
    }
}

static __attribute__((always_inline)) inline void
mgame_open_hud_menu(MGame *self, int menuType) {
    self->pauseOpen = 1;
    self->hudMenuOpen = 1;
    self->pauseSounds();
    self->hud->initHudMenu(menuType, self->level);
}

static void mgame_open_pause_menu(MGame *self) {
    self->pauseSounds();
    if (self->pauseOpen != 0) {
        self->hud->releaseAllKeys();
        return;
    }

    if (self->menuWindow == nullptr)
        self->menuWindow = new MenuTouchWindow(1);

    const bool canSkip = self->levelScript->canSkipCutsceneNow() != 0;
    self->menuWindow->setSkipButtonVisible(canSkip);
    self->pauseOpen = 1;
    self->pauseSounds();

    FModSound *sound = Globals::sound;
    self->pauseSnapshot = self->pauseOpen;
    self->pauseMusicCategoryDisabled = static_cast<uint8_t>(sound->IsCategoryEnabled(2) ^ 1);
    sound->pauseAllPlaying();
    self->player->PauseEngineSound();

    Array<KIPlayer *> *enemies = self->level->getEnemies();
    if (enemies != nullptr) {
        for (unsigned int i = 0; i < enemies->size(); ++i) {
            KIPlayer *enemy = (*enemies)[i];
            if (enemy != nullptr)
                enemy->KIPlayer::PauseEngineSound();
        }
    }

    const bool cutsceneMode = self->jumpActive != 0 || self->player->isDead();
    self->menuWindow->setCutsceneMode(cutsceneMode);
    self->menuTouchOpen = 1;
    sound->play(0x7b, nullptr, nullptr, 0.0f);
    self->hud->releaseAllKeys();
}

static void mgame_dispatch_combat_touch_actions(MGame *self, unsigned int actions) {
    Player *playerBody = static_cast<Player *>(self->player->player);
    if ((actions & kHudActionFire) != 0 &&
        playerBody->gunAvailable(1) && !self->player->isMining() &&
        !self->player->isInTurretMode() && !self->player->isDockedToDockingPoint() &&
        !self->player->isLandingOrTakingOff()) {
        self->player->shoot(self->deltaTime, 1);
        self->hud->checkIfQuickMenuIsEmpty();
    }

    if (self->player->isHacking()) {
        if ((actions & kHudActionOpenWeaponMenu) != 0)
            self->player->PlayerEgo::hackingRotateLCW();
        if ((self->hudTouchFlags & kHudActionOpenWingmanMenu) != 0)
            self->player->PlayerEgo::hackingRotateRCW();
    }
}

static void mgame_handle_autopilot_menu_touch_end(MGame *self, int x, int y) {
    ChoiceWindow *choiceWindow = self->choiceWindow;
    const int selection = choiceWindow->OnTouchEnd(x, y);
    if (selection == 1) {
        self->autopilotMenuOpen = 0;
        if (self->starMap == nullptr)
            self->starMap = new StarMap(false, nullptr, false, -1);

        Engine *engine = static_cast<Engine *>(self->applicationManager->GetEngine());
        engine->SetPostEffect(0x1400002u, 0);
        self->starMap->initLights();
        self->starMap->setJumpMapMode(true, false);
        self->pauseOpen = 1;
        self->starMapOpen = 1;
        self->pauseSounds();
        return;
    }

    if (selection == 0) {
        self->autopilotMenuOpen = 0;
        if (self->player->isInTurretMode())
            self->player->setTurretMode(false);
        self->usingJumpDrive = 0;
        self->startJumpScene();
        self->player->resetGunDelay();
    }
}

static void mgame_restore_stream_position(MGame *self) {
    self->player->dockToStream(false);
    self->player->setAutoPilot(nullptr);

    Array<KIPlayer *> *landmarks = self->level->getLandmarks();
    if (landmarks == nullptr || landmarks->size() <= 1)
        return;

    KIPlayer *jumpGate = (*landmarks)[1];
    if (jumpGate == nullptr || jumpGate->geometry == nullptr || self->player->geometry == nullptr)
        return;

    const Vector direction = jumpGate->geometry->getDirection();
    const Vector up = {0.0f, 1.0f, 0.0f};
    self->player->geometry->setDirection(direction, up);

    KIPlayer *positionGate = (*self->level->getLandmarks())[1];
    Vector position = positionGate->geometry->getPosition();
    Vector adjusted = position + Vector{0.0f, 0.0f, 8000.0f};
    self->player->setPosition(adjusted);
}

static void mgame_handle_star_map_touch_end(MGame *self, int x, int y) {
    Layout *layout = static_cast<Layout *>(Globals::layout);
    if (layout->layoutVisibleFlag != 0) {
        if (layout->OnTouchEnd(x, y) != 0)
            layout->layoutVisibleFlag = 0;
        return;
    }

    StarMap *starMap = self->starMap;
    if (starMap->OnTouchEnd(x, y) == 0)
        return;

    StarSystem *starSystem = self->level->getStarSystem();
    starSystem->initLight();
    self->pauseOpen = 0;
    self->starMapOpen = 0;
    self->resumeSounds();

    if (starMap->exitRequested != 0) {
        if (self->touchesStream != 0) {
            self->usingJumpDrive = 0;
            self->startJumpScene();
        } else if (Level::doInstantJump == 0) {
            self->levelScript->setAutoPilotToProgrammedStation();
        }
    } else if (self->touchesStream != 0) {
        mgame_restore_stream_position(self);
    }

    delete self->starMap;
    self->starMap = nullptr;
}

static void mgame_handle_menu_touch_end(MGame *self, int x, int y, void *touchId) {
    MenuTouchWindow *menuWindow = self->menuWindow;
    if (menuWindow == nullptr)
        return;

    if (self->freeCamMode != 0) {
        MGameAppData *applicationData =
            static_cast<MGameAppData *>(self->applicationManager->GetApplicationData());
        if (applicationData->modalActive != 0 || applicationData->transitionActive != 0)
            return;
        if (menuWindow->isShowingMessage() == 0 && !menuWindow->isMakingScreenshot())
            self->freeCamTouchEnd(x, y, touchId);
    }

    if (menuWindow->OnTouchEnd(x, y, touchId) != 0) {
        self->pauseSnapshot = 0;
        self->pauseOpen = 0;
        self->resumeSounds();

        const bool categoryEnabled = Globals::sound->IsCategoryEnabled(2) != 0;
        if (categoryEnabled && self->pauseMusicCategoryDisabled == 0) {
            self->player->ResumeEngineSound();
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr) {
                for (unsigned int i = 0; i < enemies->size(); ++i)
                    (*enemies)[i]->KIPlayer::ResumeEngineSound();
            }
        } else if (!categoryEnabled && self->pauseMusicCategoryDisabled == 0) {
            if (self->player != nullptr)
                self->player->StopEngineSound();
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr) {
                for (unsigned int i = 0; i < enemies->size(); ++i)
                    (*enemies)[i]->KIPlayer::StopEngineSound();
            }
        } else {
            self->player->PlayEngineSound();
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr) {
                for (unsigned int i = 0; i < enemies->size(); ++i)
                    (*enemies)[i]->KIPlayer::PlayEngineSound();
            }
        }

        self->menuTouchOpen = 0;
        self->touch0Id = 0;
        self->touch1Id = 0;
        self->activeTouchId = nullptr;

        StarSystem *starSystem = self->level->getStarSystem();
        if (starSystem != nullptr)
            starSystem->initLight();

        const float particleQuality = mgame_options()->particleQuality;
        self->level->enableParticleEffects(particleQuality > 0.0f, particleQuality > 0.7f);

        if (menuWindow->skipCutsceneRequested != 0) {
            menuWindow->skipCutsceneRequested = 0;
            const int campaignMission = Status::gStatus->getCurrentCampaignMission();
            const unsigned int skipIndex = static_cast<unsigned int>(campaignMission - 154);
            if (skipIndex < 5 && ((1u << skipIndex) & 0x19u) != 0) {
                self->levelScript->skipCutscene();
            } else if (campaignMission == 1) {
                Globals::switch_to_target_setting = 0;
                self->active = 0;
                static_cast<ApplicationManager *>(Globals::appManager)
                        ->SetCurrentApplicationModule(5);
            } else if (campaignMission == 0) {
                Status::gStatus->nextCampaignMission(true);
                Status::gStatus->setKills(3);
                Globals::switch_to_target_setting = 1;
                self->active = 0;
                static_cast<ApplicationManager *>(Globals::appManager)
                        ->SetCurrentApplicationModule(2);
                Level::initStreamOutPosition = 0;
            }
        }
        self->freeCamMode = 0;
    }

    if (menuWindow->inCinematicMode()) {
        self->setCinematicMode(true);
        self->hudTouchFlags = 0;
        StarSystem *starSystem = self->level->getStarSystem();
        if (starSystem != nullptr)
            starSystem->initLight();
    }
    if (!menuWindow->inCinematicMode()) {
        if (self->freeCamMode != 0) {
            self->setCinematicMode(false);
            self->hudTouchFlags = 0;
        }
    }
}

static __attribute__((always_inline)) inline void
mgame_reset_dialogue_input(MGame *self) {
    self->touch0Id = 0;
    self->touch1Id = 0;
    self->hud->resetAnalogStick();
    self->hud->releaseAllKeys();
}

static __attribute__((always_inline)) inline Station *mgame_load_station(int stationId) {
    auto *reader = new FileRead();
    Station *station = reinterpret_cast<Station *>(
            static_cast<intptr_t>(reader->loadStation(stationId)));
    delete reader;
    return station;
}

static __attribute__((always_inline)) inline Station *mgame_get_galaxy_station(int stationId) {
    return reinterpret_cast<Station *>(
            static_cast<intptr_t>(Galaxy::gGalaxy->getStation(stationId)));
}

static __attribute__((always_inline)) inline void
mgame_store_player_vitals(MGame *self, Status *status) {
    Player *player = reinterpret_cast<Player *>(self->player->player);
    status->field_64 = player->getHitpoints();
    status->field_5c = player->getShieldHP();
    status->field_60 = player->getArmorHP();
    status->field_68 = player->getGammaHP();
}

static __attribute__((always_inline)) inline void
mgame_switch_module(MGame *self, unsigned int module) {
    self->active = 0;
    self->applicationManager->SetCurrentApplicationModule(module);
}

static void mgame_clear_completed_mission(MGame *self, Status *status) {
    Mission *emptyMission = reinterpret_cast<Mission *>(Mission::empty);
    self->level->removeObjectives();
    status->setMission(emptyMission);
    self->player->setRoute(nullptr);
    if (self->player->goingToWaypoint())
        self->player->setAutoPilot(nullptr);
    self->player->removeRoute();
    self->level->setPlayerRoute(nullptr);
}

static void mgame_create_followup_drill_mission(MGame *self, Status *status) {
    self->field_0x1e0 = self->level->killCountB;
    Item *drill = (*Item::g_items)[216]->makeItem(10);
    status->getShip()->setEquipment(drill, 0);

    String client("Client", false);
    int *clientImage = static_cast<ImageFactory *>(Globals::imageFactory)->createChar(true, 0);
    Mission *followup = new Mission(183, client, clientImage, 0, 0,
                                    status->getStation()->getIndex(), 1);
    status->setMission(followup);
    status->setFreelanceMission(followup);
    mgame_switch_module(self, 2);
}

static bool mgame_handle_completed_mission(MGame *self) {
    Mission *campaignMission = reinterpret_cast<Mission *>(
            static_cast<intptr_t>(Status::gStatus->getCampaignMission()));
    const bool campaignWonBehindFreelance = campaignMission->hasWon() &&
            !Status::gStatus->getMission()->isCampaignMission();
    Mission *completedMission = Status::gStatus->getMission()->hasWon()
            ? Status::gStatus->getMission()
            : reinterpret_cast<Mission *>(
                    static_cast<intptr_t>(Status::gStatus->getCampaignMission()));

    if (completedMission->isInstantActionMission()) {
        Globals::switch_to_target_setting = 2;
        self->active = 0;
        self->applicationManager->SetCurrentApplicationModule(1);
        return false;
    }

    if (completedMission->isCampaignMission()) {
        Status::gStatus->nextCampaignMission(true);
    } else {
        Status::gStatus->setFreelanceMission(reinterpret_cast<Mission *>(Mission::empty));
        static_cast<Layout *>(Globals::layout)->showMissionRewardMessage(
                completedMission->getReward() + completedMission->getBonus(), false);
    }
    Status::gStatus->changeCredits(completedMission->getReward() + completedMission->getBonus());
    self->levelScript->m_nTimeLimit = 0;

    if (completedMission->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 15) {
        Status::gStatus->setStation(mgame_load_station(98));
        Globals::switch_to_target_setting = 0;
        mgame_store_player_vitals(self, Status::gStatus);
        mgame_switch_module(self, 5);
        return false;
    }
    if (completedMission->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 22) {
        Globals::switch_to_target_setting = 0;
        mgame_store_player_vitals(self, Status::gStatus);
        mgame_switch_module(self, 5);
        return false;
    }
    if (completedMission->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 43) {
        Status::gStatus->setStation(mgame_load_station(10));
        Globals::switch_to_target_setting = 0;
        mgame_store_player_vitals(self, Status::gStatus);
        mgame_switch_module(self, 5);
        return false;
    }
    if (completedMission->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 42) {
        delete self->level->objectivesB;
        self->level->objectivesB = nullptr;
        delete self->level->objectivesA;
        self->level->objectivesA = nullptr;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 65) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 1;
        Status::gStatus->departStation(mgame_load_station(100));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 81) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 1;
        Status::gStatus->setStation(Status::gStatus->playerStation);
        Status::gStatus->departStation(Status::gStatus->playerStation);
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 74) {
        Status::gStatus->setStation(mgame_load_station(100));
        Globals::sound->stopAll();
        Globals::switch_to_target_setting = 0;
        mgame_store_player_vitals(self, Status::gStatus);
        mgame_switch_module(self, 5);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 95) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 1;
        Status::gStatus->departStation(mgame_get_galaxy_station(10));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 96) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 1;
        Status::gStatus->departStation(mgame_get_galaxy_station(98));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 100) {
        mgame_store_player_vitals(self, Status::gStatus);
        Globals::switch_to_target_setting = 0;
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(120));
        self->applicationManager->SetCurrentApplicationModule(5);
        self->active = 0;
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 110) {
        mgame_store_player_vitals(self, Status::gStatus);
        Globals::switch_to_target_setting = 0;
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(10));
        self->applicationManager->SetCurrentApplicationModule(5);
        Globals::enterSpaceLounge = 1;
        self->active = 0;
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 120) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(126));
        mgame_switch_module(self, 5);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 126) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(120));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 127) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 1;
        Status::gStatus->departStation(mgame_get_galaxy_station(98));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 134) {
        Globals::switch_to_target_setting = 0;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(112));
        mgame_switch_module(self, 5);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 144) {
        Globals::switch_to_target_setting = 1;
        mgame_store_player_vitals(self, Status::gStatus);
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(112));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 155) {
        Level::initStreamOutPosition = 1;
        Globals::switch_to_target_setting = 1;
        Status::gStatus->departStation(
                mgame_get_galaxy_station(Status::gStatus->field_84));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 161) {
        mgame_store_player_vitals(self, Status::gStatus);
        Globals::switch_to_target_setting = 0;
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(93));
        mgame_switch_module(self, 2);
        return false;
    } else if (completedMission->isCampaignMission() &&
               Status::gStatus->getCurrentCampaignMission() == 162) {
        mgame_store_player_vitals(self, Status::gStatus);
        Globals::switch_to_target_setting = 0;
        Level::initStreamOutPosition = 0;
        Status::gStatus->departStation(mgame_get_galaxy_station(93));
        mgame_switch_module(self, 5);
        return false;
    }

    if (!campaignWonBehindFreelance)
        mgame_clear_completed_mission(self, Status::gStatus);

    if (!completedMission->isCampaignMission() && completedMission->getType() == 183) {
        mgame_create_followup_drill_mission(self, Status::gStatus);
        return false;
    }

    return true;
}

static void mgame_clear_failed_freelance_mission(MGame *self, Mission *mission) {
    Status *status = Status::gStatus;
    if (mission->getType() == 12) {
        status->changeCredits(-mission->getReward());
    } else if (mission->getType() == 3 || mission->getType() == 5 || mission->getType() == 11) {
        Ship *ship = status->getShip();
        Array<Item *> *cargo = ship != nullptr ? ship->getCargo() : nullptr;
        if (cargo != nullptr) {
            for (unsigned int i = 0; i < cargo->size(); ++i) {
                Item *item = (*cargo)[i];
                if (item != nullptr && item->isUnsaleable() &&
                    (item->getIndex() == 116 || item->getIndex() == 117)) {
                    ship->removeCargo(item);
                    break;
                }
            }
        }
    }

    Mission *emptyMission = reinterpret_cast<Mission *>(Mission::empty);
    status->setFreelanceMission(emptyMission);
    self->level->removeObjectives();
    self->levelScript->m_nTimeLimit = 0;
    status->setMission(emptyMission);
    self->player->setRoute(nullptr);
    if (self->player->goingToWaypoint())
        self->player->setAutoPilot(nullptr);
    self->player->removeRoute();
    self->level->setPlayerRoute(nullptr);
}

static void mgame_handle_cargo_conversion_choice(MGame *self, int x, int y) {
    const int selection = self->choiceWindow->OnTouchEnd(x, y);
    if (selection == 1) {
        self->choiceWindowOpen = 0;
        self->cargoConversionChoiceOpen = 0;
        self->pauseOpen = 0;
        self->resumeSounds();
        return;
    }
    if (selection != 0)
        return;

    String convertedCargo;
    Ship *ship = Status::gStatus->getShip();
    bool convertedAny = false;
    for (int sourceItem = 154; sourceItem < 166; ++sourceItem) {
        const int replacementItem = sourceItem == 165 ? 218 : sourceItem + 11;
        const int batchSize = Status::gStatus->hardCoreMode() ? 100 : 30;
        int convertedCount = 0;
        while (ship->hasCargo(sourceItem, batchSize)) {
            ship->removeCargo(sourceItem, batchSize);
            ship->addCargo((*Item::g_items)[replacementItem]->makeItem(1));
            ++convertedCount;
            convertedAny = true;
        }
        if (convertedCount != 0) {
            convertedCargo += String("\n", false) + String(convertedCount) +
                              String("t ", false) +
                              *GameText::gGameText->getText(replacementItem + 1274);
        }
    }

    self->cargoConversionChoiceOpen = 0;
    if (convertedAny) {
        self->choiceWindow->set(*GameText::gGameText->getText(291) +
                                String("\n", false) + convertedCargo);
    } else {
        self->choiceWindow->set(*GameText::gGameText->getText(292));
    }
}

static int mgame_handle_terminal_choice(MGame *self, int x, int y) {
    const bool terminalChoice = (self->field_0x1e4 & 0xff) != 0;
    const int selection = self->choiceWindow->OnTouchEnd(x, y);
    if (!terminalChoice) {
        if (selection == 0) {
            self->pauseOpen = 0;
            self->choiceWindowOpen = 0;
            self->resumeSounds();
        }
        return 0;
    }

    if (selection == 2)
        return 1;
    if (selection == 1)
        return 2;
    if (selection != 0)
        return 0;

    if (self->menuWindow == nullptr)
        self->menuWindow = new MenuTouchWindow(1);
    self->menuWindow->startSupernovaChallenge();
    return 1;
}

static void mgame_handle_boost_touch(MGame *self, unsigned int actions) {
    if ((actions & kHudActionBoost) == 0 || !self->player->isBoostRefreshed() ||
        self->player->boosting() || self->player->isMining() ||
        self->player->isDockedToDockingPoint()) {
        return;
    }

    self->player->setThrust(1.0f);
    self->boostTouchThrust = 1.0f;
    self->boostTouchDuration = 100.0f;
    static_cast<Engine *>(self->applicationManager->GetEngine())->field_0x360 = 0;
    self->player->boost();
}

static __attribute__((always_inline)) inline void
mgame_finish_autopilot_transition(MGame *self, bool wasAutoPilot) {
    if (!wasAutoPilot && self->player->isAutoPilot()) {
        self->boostTouchThrust = 1.0f;
        self->boostTouchDuration = 100.0f;
    }
}

static int mgame_handle_dialogue_touch_end(MGame *self, int x, int y) {
    if (self->dialogueWindow->OnTouchEnd(x, y) == 0)
        return 0;

    self->pauseOpen = 0;
    self->resumeSounds();
    self->cutsceneActive = 0;

    Status *status = Status::gStatus;
    Mission *mission = status->getMission();
    const int hasFailed = mission->hasFailed();
    Mission *currentMission = status->getMission();
    if (hasFailed) {
        if (currentMission->isCampaignMission() || status->getCurrentCampaignMission() == 42) {
            return 1;
        } else {
            mgame_clear_failed_freelance_mission(self, currentMission);
        }
        return 2;
    }

    if (!currentMission->hasWon()) {
        Mission *campaignMission = reinterpret_cast<Mission *>(
            static_cast<intptr_t>(status->getCampaignMission()));
        if (!campaignMission->hasWon()) {
            self->levelScript->resetStartSequenceOver();
            return 2;
        }
    }

    return mgame_handle_completed_mission(self) ? 2 : 0;
}

static void mgame_dispatch_orbit_menu(MGame *self, unsigned int actions) {
    PlayerEgo *player = self->player;
    Level *level = self->level;
    Hud *hud = self->hud;

    if ((actions & kHudActionProgrammedStation) != 0)
        self->levelScript->setAutoPilotToProgrammedStation();

    if ((actions & kHudActionJumpGate) != 0) {
        player->setAutoPilot((*level->getLandmarks())[1]);
        hud->hudEvent(12, player, 0);
    }
    if ((actions & kHudActionStation) != 0) {
        player->setAutoPilot((*level->getLandmarks())[0]);
        hud->hudEvent(10, player, 0);
    }
    if ((actions & kHudActionAsteroidWaypoint) != 0) {
        player->setAutoPilot(level->getAsteroidWaypoint());
        hud->hudEvent(14, player, 0);
    }
    if ((actions & kHudActionRouteWaypoint) != 0) {
        if (level->getPlayerRoute() != nullptr) {
            player->setAutoPilot(level->getPlayerRoute()->getWaypoint());
            hud->hudEvent(13, player, 0);
        }
    }

    // The action-mask layout reserves bits 26..31 for docking targets.
    for (int i = 0; i < level->getNumDockingTargets(); ++i) {
        const unsigned int action = kHudActionDockingTarget0 << i;
        if ((actions & action) == 0 || level->getDockingTarget(i) == 0)
            continue;

        // Android MGame::OnTouchEnd writes this target to Radar+0x04 before
        // starting docking, then clears the three adjacent transient slots.
        self->radar->dockTargetPtr = reinterpret_cast<KIPlayer *>(
                static_cast<intptr_t>(level->getDockingTarget(i)));
        hud->hudEvent(34, player, 0);
        player->dockToDockingPoint(reinterpret_cast<KIPlayer *>(
                static_cast<intptr_t>(level->getDockingTarget(i))), self->radar);
        self->radar->dockNavPtr = nullptr;
        self->radar->lockedAsteroid = nullptr;
        self->radar->candidateAsteroid = nullptr;
        hud->releaseAllKeys();
    }

    mgame_close_hud_menu(self, true);
}

static void mgame_try_open_orbit_menu(MGame *self) {
    PlayerEgo *player = self->player;
    Status *status = Status::gStatus;

    if (player == nullptr || status == nullptr)
        return;

    if (status->inAlienOrbit() && player->isDockingToAsteroid())
        player->dockToAsteroid(nullptr, self->radar);

    if ((status->inAlienOrbit() &&
         (!status->inAlienOrbit() ||
          status->getCurrentCampaignMission() != 154 ||
          self->level->getNumDockingTargets() < 1)) ||
        status->getCurrentCampaignMission() < 2 ||
        status->getCurrentCampaignMission() == 48 ||
        player->isDockedToDockingPoint() ||
        player->isLandingOrTakingOff()) {
        return;
    }

    if (self->orbitMenuOpen != 0) {
        mgame_close_hud_menu(self, true);
        return;
    }

    if (player->isMining())
        return;

    if (player->isAutoPilot()) {
        player->setAutoPilot(nullptr);
        self->hud->hudEvent(6, player, 0);
        return;
    }

    if (player->isDockingToAsteroid()) {
        player->dockToAsteroid(self->radar->getLockedAsteroid(), self->radar);
        self->hud->hudEvent(6, player, 0);
        return;
    }

    if (player->isDockingToDockingPoint()) {
        self->radar->dockTargetPtr = nullptr;
        player->dockToDockingPoint(nullptr, self->radar);
        self->hud->hudEvent(6, player, 0);
        return;
    }

    if (player->isDockingToStream()) {
        // The ARM body routes this through Radar+0x24 before the common
        // docking HUD event. The local Radar slot is source-labelled
        // `lockedStation` but its exact stream-target semantics still need
        // a dedicated Radar/PlayerEgo audit.
        player->dockToAsteroid(self->radar->lockedStation, self->radar);
        self->hud->hudEvent(6, player, 0);
        return;
    }

    if (!status->inAlienOrbit() ||
        (status->inAlienOrbit() &&
         status->getCurrentCampaignMission() == 154)) {
        if (status->getMission() == nullptr ||
            status->getMission()->getType() != 183) {
            mgame_open_hud_menu(self, 3);
            self->orbitMenuOpen = 1;
            return;
        }
    }
}

static void mgame_dispatch_hud_menu(MGame *self, unsigned int actions) {
    if (self->hudMenuOpen == 0)
        return;

    if ((actions & kHudActionOpenWeaponMenu) != 0) {
        self->hud->initHudMenu(1, self->level);
        return;
    }
    if ((actions & kHudActionCloak) != 0) {
        mgame_close_hud_menu(self, false);
        self->useCloak();
        return;
    }
    if ((actions & kHudActionOpenWingmanMenu) != 0) {
        self->hud->initHudMenu(2, self->level);
        return;
    }

    int secondaryIndex = -1;
    if ((actions & kHudActionSecondary0) != 0) secondaryIndex = 0;
    else if ((actions & kHudActionSecondary1) != 0) secondaryIndex = 1;
    else if ((actions & kHudActionSecondary2) != 0) secondaryIndex = 2;
    else if ((actions & kHudActionSecondary3) != 0) secondaryIndex = 3;
    if (secondaryIndex >= 0) {
        Ship *ship = Status::gStatus->getShip();
        Array<Item *> *equipment = ship != nullptr ? ship->getEquipment(1) : nullptr;
        if (equipment != nullptr && static_cast<unsigned int>(secondaryIndex) < equipment->size()) {
            Item *item = (*equipment)[secondaryIndex];
            if (item != nullptr) {
                self->level->getPlayer()->setCurrentSecondaryWeaponIndex(item->getIndex());
                self->hud->setCurrentSecondaryWeapon(item);
            }
        }
        self->pauseOpen = 0;
        self->hudMenuOpen = 0;
        self->resumeSounds();
        self->hud->closeHudMenu();
        return;
    }

    int wingmanCommand = -1;
    if ((actions & kHudActionWingmenAttack) != 0) wingmanCommand = 1;
    else if ((actions & kHudActionWingmenDefend) != 0) wingmanCommand = 3;
    else if ((actions & kHudActionWingmenFollow) != 0) wingmanCommand = 2;
    else if ((actions & kHudActionWingmenToggle) != 0) {
        wingmanCommand = 0;
        reinterpret_cast<uint8_t *>(&Status::gStatus->field_f8)[0] ^= 1;
    }
    if (wingmanCommand >= 0) {
        Array<KIPlayer *> *enemies = self->level->getEnemies();
        if (enemies != nullptr) {
            for (unsigned int i = 0; i < enemies->size(); ++i) {
                KIPlayer *wingman = (*enemies)[i];
                if (wingman->isWingMan() && !wingman->isDead()) {
                    KIPlayer *target = wingmanCommand == 3 ? self->radar->getLockedEnemy() : nullptr;
                    wingman->setWingmanCommand(wingmanCommand, target);
                }
            }
        }
        mgame_close_hud_menu(self, false);
        return;
    }

    if ((actions & kHudActionJumpDrive) != 0) {
        self->UseKhadorDrive();
        return;
    }

    // This is the ARM LABEL_218 fall-through: releasing off a menu entry
    // closes the menu and restores sounds, without inventing a new action.
    if (self->hudTouchFlags == 0) {
        self->hud->closeHudMenu();
        self->hudMenuOpen = 0;
        self->touch0Id = 0;
        self->touch1Id = 0;
        self->pauseOpen = 0;
        self->resumeSounds();
        if (self->orbitMenuOpen != 0) {
            self->orbitMenuOpen = 0;
            if (self->player != nullptr)
                self->player->resetGunDelay();
        }
    }
}

} // namespace

void MGame::OnTouchEnd(int p1, int p2, void *touchId) {
    if (this->activeTouchId == touchId) {
        this->activeTouchId = 0;
    }

    const bool wasAutoPilot = this->player->isAutoPilot();
    this->flFastForwardWeight = 1.0f;
    this->camera->setFastForwardMode(false);
    // Android MGame::OnTouchEnd writes the resume-input byte at PlayerEgo+0x84.
    // The wider PlayerEgo tail is not typed reliably enough to name this field yet.
    reinterpret_cast<uint8_t *>(this->player)[0x84] = 1;

    // Android 0x17a144 handles the paused modal family before LABEL_69.
    // MGame+0xce selects between the modal dispatcher and the explicit
    // ChoiceWindow path; dockChoiceOpen intentionally falls through to HUD.
    if (this->pauseOpen != 0) {
        if (this->choiceWindowOpen == 0) {
            if (this->autopilotMenuOpen != 0) {
                mgame_handle_autopilot_menu_touch_end(this, p1, p2);
                __asm__ volatile("" ::: "memory");
                return;
            }
            if (this->dockChoiceOpen != 0)
                goto hud_action_path;
            if (this->starMapOpen != 0) {
                mgame_handle_star_map_touch_end(this, p1, p2);
                __asm__ volatile("" ::: "memory");
                return;
            }
            if (this->cutsceneActive != 0) {
                const int dialogueResult = mgame_handle_dialogue_touch_end(this, p1, p2);
                if (dialogueResult == 1)
                    goto reset_game_to_main_menu;
                if (dialogueResult == 2)
                    goto reset_dialogue_input;
                __asm__ volatile("" ::: "memory");
                return;
            }
            if (this->menuTouchOpen != 0) {
                mgame_handle_menu_touch_end(this, p1, p2, touchId);
                __asm__ volatile("" ::: "memory");
                return;
            }
            goto hud_action_path;
        }

        // Android MGame::OnTouchEnd @ 0x17a144 dispatches the explicit
        // ChoiceWindow family in this order: +0xcf, +0xca, then +0x1e4.
        if (this->field_0xcf != 0) {
            const int selection = this->choiceWindow->OnTouchEnd(p1, p2);
            Status *status = Status::gStatus;

            if (selection == 1) {
                Array<KIPlayer *> *enemies = this->level->getEnemies();
                if (enemies != nullptr) {
                    for (unsigned int i = 0; i < enemies->size(); ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (enemy->shipGroup == 8)
                            enemy->field_0x25 = 1;
                    }
                }
                status->field_0x111 = 1;
                this->level->createRadioMessage(11, 8);
                this->pauseOpen = 0;
                this->choiceWindowFlags = 0;
                this->resumeSounds();
                goto hud_action_path;
            }
            if (selection == 0) {
                if (status->getCredits() < this->choiceItemCount) {
                    this->choiceWindow->set(status->replaceHash(
                        String(*GameText::gGameText->getText(203), false),
                        String(Globals::layout->formatCredits(this->choiceItemCount), false),
                        String("#C", false)), false);

                    this->field_0xcf = 0;
                    this->level->createRadioMessage(11, 8);
                    Array<KIPlayer *> *enemies = this->level->getEnemies();
                    if (enemies != nullptr) {
                        for (unsigned int i = 0; i < enemies->size(); ++i) {
                            KIPlayer *enemy = (*enemies)[i];
                            if (enemy->shipGroup == 8)
                                enemy->field_0x25 = 1;
                        }
                    }
                    status->field_0x111 = 1;
                    return;
                }

                Array<KIPlayer *> *enemies = this->level->getEnemies();
                if (enemies != nullptr) {
                    for (unsigned int i = 0; i < enemies->size(); ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (enemy->shipGroup == 8)
                            enemy->field_0x25 = 0;
                    }
                }
                status->changeCredits(-this->choiceItemCount);
                this->level->createRadioMessage(10, 8);
                status->field_110 = 1;
                this->pauseOpen = 0;
                this->choiceWindowFlags = 0;
                this->resumeSounds();
                goto hud_action_path;
            }
            return;
        }

        if (this->cargoConversionChoiceOpen != 0)
            mgame_handle_cargo_conversion_choice(this, p1, p2);
        else {
            const int terminalResult = mgame_handle_terminal_choice(this, p1, p2);
            if (terminalResult == 2)
                goto reset_game_to_main_menu;
            if (terminalResult == 1)
                return;
        }
        goto hud_action_path;

reset_game_to_main_menu:
        static_cast<Globals *>(Globals::globals)->playMusicAndFadeOutCurrent(2);
        this->active = 0;
        this->applicationManager->SetCurrentApplicationModule(1);
        Status::gStatus->resetGame();
        return;

reset_dialogue_input:
        mgame_reset_dialogue_input(this);
        return;
    }

hud_action_path:
    const unsigned int actions = this->hud->touchEnd(p1, p2, touchId);
    this->hudTouchFlags = static_cast<int>(actions);
    if (actions != 0) {
        this->touch0Id = 0;
        this->touch1Id = 0;
    }

    if ((actions & kHudActionPause) != 0) {
        mgame_open_pause_menu(this);
        return;
    }

    if (this->levelScript->startSequence() || this->player->isDead()) {
        this->levelScript->skipSequence();
        return;
    }
    if (this->jumpActive != 0 || this->jumpDriveActive != 0)
        return;

    mgame_handle_boost_touch(this, actions);
    mgame_dispatch_combat_touch_actions(this, actions);
    if (this->player->isInRocketControl())
        return;
    if (this->player->hasAutoTurret() && (actions & kHudActionAutoTurret) != 0) {
        this->player->setAutoTurret(this->player->autoTurretIsEnabled() != 1);
        this->hud->hudEvent(this->player->autoTurretIsEnabled() ? 0x20 : 0x21,
                            this->player, 0);
    }

    if ((actions & kHudActionOrbit) == 0 && this->orbitMenuOpen != 0) {
        mgame_dispatch_orbit_menu(this, actions);
    }

    if ((actions & kHudActionOrbit) != 0)
        mgame_try_open_orbit_menu(this);

    if ((actions & kHudActionQuickMenu) != 0 && !this->player->isMining()) {
        const bool menuWasOpen = this->hudMenuOpen != 0;
        this->hudMenuOpen ^= 1;
        this->pauseOpen ^= 1;
        if (!menuWasOpen) {
            this->pauseSounds();
            this->hud->initHudMenu(0, this->level);
        }
    }

    if ((actions & kHudActionCamera) != 0 && !this->player->isMining())
        this->switchCamera(this->cameraMode + 1);

    if (this->hudMenuOpen == 0) {
        if (this->cameraMode == 0) {
            this->maneuverTouchEnd(p1, p2, touchId);
            this->thrustActive = 0;
        } else if (this->cameraMode == 3) {
            this->freeCamTouchEnd(p1, p2, touchId);
        }
        mgame_finish_autopilot_transition(this, wasAutoPilot);
        return;
    }

    mgame_dispatch_hud_menu(this, actions);
    mgame_finish_autopilot_transition(this, wasAutoPilot);
}


int MGame::successCheck() {
    if ((*reinterpret_cast<int64_t *>(&this->elapsedTime) < 5001 ||
         this->jumpDriveActive != 0) &&
        reinterpret_cast<Mission *>(static_cast<intptr_t>(
                Status::gStatus->getCampaignMission()))->getType() != 170)
        return 0;

    Mission *completedMission = Status::gStatus->missionCompleted(
            false, false, this->levelScript->scriptTime);
    const int objectiveComplete = this->level->checkObjective(
            static_cast<int>(this->levelScript->scriptTime));
    if (completedMission == 0 && objectiveComplete == 0)
        return 0;

    if (((Mission *) (Status::gStatus->getMission()))->getType() == 5) goto deliverFollowup;
    if (((Mission *) (Status::gStatus->getMission()))->getType() == 3) goto deliverFollowup;

    {
    if (!Status::gStatus->getMission()->isCampaignMission())
        Status::gStatus->incMissionCount();

    if (Status::gStatus->getMission()->isCampaignMission()) {
        if (!(Status::gStatus->getMission()->isCampaignMission() &&
              DialogueWindow::hasSuccessDialogue(
                      Status::gStatus->getCurrentCampaignMission()))) {
            if (Status::gStatus->getCurrentCampaignMission() >= 46 &&
                Status::gStatus->getMission()->isCampaignMission() &&
                !DialogueWindow::hasSuccessDialogue(
                        Status::gStatus->getCurrentCampaignMission())) {
                Status::gStatus->nextCampaignMission(true);
                this->level->removeObjectives();
                Status::gStatus->setMission(
                        reinterpret_cast<Mission *>(Mission::empty));
            }
            return 0;
        }
    }

    if (this->dialogueWindow == nullptr) {
        DialogueWindow *window =
                static_cast<DialogueWindow *>(::operator new(0x74));
        DialogueWindow_ctor(window);
        Level *level = this->level;
        this->dialogueWindow = window;
        if (level != nullptr)
            window->setLevel(level);
    } else if (!this->dialogueWindow->hasLevel()) {
        Level *level = this->level;
        if (level != nullptr)
            this->dialogueWindow->setLevel(level);
    }

    Mission *dialogueMission = completedMission;
    if (dialogueMission == nullptr)
        dialogueMission = Status::gStatus->getMission();
    this->dialogueWindow->set(dialogueMission, 1, -1);
    *reinterpret_cast<uint16_t *>(&this->pauseOpen) = 0x0101;
    this->pauseSounds();

    if (Status::gStatus->getMission()->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 38) {
        Array<KIPlayer *> *enemies = this->level->getEnemies();
        for (unsigned i = 0; i < enemies->size(); ++i) {
            KIPlayer *enemy = (*enemies)[i];
            if (enemy->field_0x40b != 0 && !enemy->isDead())
                enemy->player->setHitpoints(9999999);
        }
        return 1;
    }

    if (Status::gStatus->getMission()->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 56) {
        StarSystem *starSystem = this->level->getStarSystem();
        Array<AEGeometry *> *planets =
                static_cast<Array<AEGeometry *> *>(starSystem->getPlanets());
        Vector planetPosition = (*planets)[2]->getPosition();
        Vector routePosition = planetPosition * 100.0f;
        int points[3] = {
                static_cast<int>(routePosition.x),
                static_cast<int>(routePosition.y),
                static_cast<int>(routePosition.z),
        };
        Route *route = new Route(points, 3);
        Array<KIPlayer *> *enemies = this->level->getEnemies();
        for (unsigned i = 0; i < enemies->size(); ++i) {
            KIPlayer *enemy = (*enemies)[i];
            if (enemy->shipGroup == 1)
                enemy->setRoute(route->clone());
        }
        delete route;
        return 1;
    }

    if (Status::gStatus->getMission()->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 63) {
        Array<KIPlayer *> *enemies = this->level->getEnemies();
        for (unsigned i = 0; i < enemies->size(); ++i) {
            KIPlayer *enemy = (*enemies)[i];
            if (enemy->shipGroup == 8)
                enemy->player->removeAllGuns();
        }
        return 1;
    }

    if (Status::gStatus->getMission()->isCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() == 73) {
        Array<KIPlayer *> *enemies = this->level->getEnemies();
        for (unsigned i = 0; i < enemies->size(); ++i) {
            KIPlayer *enemy = (*enemies)[i];
            if (enemy->field_0x40b != 0 && !enemy->isDead()) {
                enemy->player->setHitpoints(9999999);
                static_cast<PlayerFixedObject *>(enemy)->setMoving(true);
            }
        }
        return 1;
    }

    if (Station_getIndex(Status::gStatus->getStation()) == 112 &&
        Status::gStatus->getCurrentCampaignMission() == 143)
        Level::initStreamOutPositionAfterCutscene = 1;
    return 1;
    }

deliverFollowup:
    if (this->dialogueWindow == 0) {
        this->dialogueWindow = new DialogueWindow();
        if (this->level != 0)
            this->dialogueWindow->setLevel(this->level);
    } else if (this->dialogueWindow->hasLevel() == 0) {
        if (this->level != 0)
            this->dialogueWindow->setLevel(this->level);
    }

    Mission *mission = Status::gStatus->getMission();
    mission->setTargetStation(Status::gStatus->getMission()->getAgent()->getStation());
    this->dialogueWindow->set(Status::gStatus->getMission(), 1, -1);
    Status::gStatus->getMission()->setType(11);

    this->player->setTurretMode(false);
    this->levelScript->resetCamera(this->level);
    this->player->setFreeLookMode(false);
    this->camera->enableFirstPersonCam(false);
    this->player->hideShipForFirstPersonCameraView(false);
    this->needsRedraw = 1;

    Status::gStatus->getMission()->setStatusValue(-1);
    Status::gStatus->getMission()->setWon(false);
    String message = Status::gStatus->replaceHash(
            String(*Globals::gameText->getText(803), false),
            String(Status::gStatus->getMission()->getTargetStationName(), false), String("#S", false));
    Status::gStatus->getMission()->getAgent()->setMissionString(message);
    Status::gStatus->setMission(reinterpret_cast<Mission *>(Mission::empty));

    this->player->setRoute(nullptr);
    if (this->player->goingToWaypoint())
        this->player->setAutoPilot(nullptr);
    this->player->removeRoute();
    this->level->setPlayerRoute(nullptr);

    delete this->level->objectivesA;
    this->level->objectivesA = nullptr;
    delete this->level->objectivesB;
    this->level->objectivesB = nullptr;

    *reinterpret_cast<uint16_t *>(&this->pauseOpen) = 0x0101;
    this->pauseSounds();
    return 1;
}

MGame::~MGame() {
    this->OnRelease();
}



void MGame::resumeSounds() {
    Globals::sound->resumeAll();
    this->player->ResumeEngineSound();
    Array<KIPlayer *> *e = this->level->getEnemies();
    if (e != nullptr) {
        for (uint32_t i = 0; i < e->size(); i++)
            ((KIPlayer *) ((*e)[i]))->KIPlayer::ResumeEngineSound();
    }
}


void MGame::startChargingJumpDrive() {
    if (this->usingJumpDrive == 0) return;
    int needed = 1;
    if (Globals::status->getShip()->hasCargo(0x7a, 1) == 0) {
        ChoiceWindow *w = this->choiceWindow;
        if (w == 0) {
            w = new ChoiceWindow();
            this->choiceWindow = w;
        }
        w->set(*Globals::gameText->getText(0x243));
        this->pauseOpen = 1;
        this->choiceWindowOpen = 1;
        this->pauseSounds();
        Level::programmedStation = 0;
        return;
    }
    if (Globals::status->hardCoreMode() != 0) needed = 2;
    int cost;
    if (Level::programmedStation == Globals::status->playerStation) {
        cost = needed << 1;
    } else {
        cost = Level::energyCellsForNextJump;
        if (Globals::status->inAlienOrbit() != 0) cost = needed;
    }
    Item *cells = Globals::status->getShip()->getCargo(0x7a);
    if (cost <= cells->getAmount()) {
        this->player->startJumpDrive();
        if (Level::programmedStation != Globals::status->playerStation &&
            Globals::status->inAlienOrbit() == 0) {
            needed = Level::energyCellsForNextJump;
        }
        this->hud->hudEvent(0x1e, this->player, needed);
        Globals::status->getShip()->removeCargo(0x7a, needed);
        return;
    }

    ChoiceWindow *w = this->choiceWindow;
    if (w == 0) {
        w = new ChoiceWindow();
        this->choiceWindow = w;
    }
    int textId = Globals::status->hardCoreMode() != 0 ? 0x243 : 0x244;
    w->set(*Globals::gameText->getText(textId));
    this->pauseOpen = 1;
    this->choiceWindowOpen = 1;
    this->pauseSounds();
    Level::programmedStation = 0;
}



void MGame::pauseSounds() {
    this->pauseSnapshot = this->pauseOpen;
    Globals::sound->pauseAllPlayingSoundFXEvents();
    this->player->PauseEngineSound();
    Array<KIPlayer *> *e = this->level->getEnemies();
    if (e != nullptr) {
        for (uint32_t i = 0; i < e->size(); i++)
            ((KIPlayer *) ((*e)[i]))->KIPlayer::PauseEngineSound();
    }
}

// Native Android IDA: MGame camera perspective uses fixed FOV/near and
// switches only the far plane for early alien orbit campaign missions.
static const float g_MGame_camFovDefault = 1.22f;
static const float g_MGame_camNear = 20.0f;
static const float g_MGame_camFarNormal = 300000.0f;
static const float g_MGame_camFarAlienEarlyCampaign = 450000.0f; // mission < 0x50
static const float g_MGame_camFarAlienLateCampaign = 300000.0f;
static const float g_MGame_boostFovScale = 0.35f;

static __attribute__((always_inline)) inline float MGame_cameraFarPlane() {
    if (Status::gStatus->inAlienOrbit() == 0) {
        return g_MGame_camFarNormal;
    }
    int cm = Status::gStatus->getCurrentCampaignMission();
    return (cm < 0x50) ? g_MGame_camFarAlienEarlyCampaign : g_MGame_camFarAlienLateCampaign;
}

void MGame::reset() {
    *reinterpret_cast<uint64_t *>(&this->flCameraRoll) = 0;
    *reinterpret_cast<uint64_t *>(&this->touch0Id) = 0;
    this->menuTime = 0;
    *reinterpret_cast<uint64_t *>(&this->dragStartX) = 0;
    this->dragDeltaY = 0;
    this->freeCamDragging = 0;

    this->flShakeX = 25.0f;
    this->flShakeY = -50.0f;
    *reinterpret_cast<uint64_t *>(&this->field_0x120) = 0x451c4000ULL;
    *reinterpret_cast<uint64_t *>(&this->dragLastY) = 0;
    *reinterpret_cast<uint64_t *>(&this->dragRotIntY) = 0;

    this->player = this->level->getPlayer();

    this->hud = new Hud();

    Radio *radio = new Radio();
    this->radio = radio;
    ((Radio *) (radio))->setMessages((Array<RadioMessage *> *) this->level->getMessages());

    PaintCanvas *pc = PaintCanvas::gCanvas;
    pc->CameraCreate(this->cameraId);
    unsigned cam = this->cameraId;
    PaintCanvas::gCanvas->CameraSetPerspective(
        cam, g_MGame_camFovDefault, g_MGame_camNear, MGame_cameraFarPlane());

    if (this->camera != 0) {
        delete this->camera;
        this->camera = 0;
    }
    Vector zero = {0.0f, 0.0f, 0.0f};
    TargetFollowCamera *tfc = new TargetFollowCamera(
        this->cameraId, this->player->geometry, zero, zero);
    this->camera = tfc;
    PaintCanvas::gCanvas->CameraSetCurrent(this->cameraId);
    this->player->setTargetFollowCamera(this->camera);
    this->camera->resetShipHandling();

    Radar *radar = new Radar(this->level);
    this->radar = radar;

    if (Status::gStatus->getMission() != 0)
        this->campaignMission = (uint8_t)((Mission *) (Status::gStatus->getMission()))->isCampaignMission();

    LevelScript *script = new LevelScript(this->level, this->hud,
                                          this->radar, this->camera);
    this->levelScript = script;
    script->resetCamera(this->level);
    this->level->initParticleSystems();

    ChoiceWindow *cw = new ChoiceWindow();
    this->choiceWindow = cw;

    this->elapsedTime = 0;
    this->elapsedTimeHigh = 0;
    this->cameraMode = 0;
    this->field_0x18 = 0;
    *(uint16_t *) &this->field_0x5c = 0;
    *(uint16_t *) &this->jumpActive = 0;
    *(uint16_t *) &this->dockChoiceFlags = 0;
    this->field_0xc6 = 0;
    this->field_0xc8 = 0;
    this->field_0xe0 = 0;
    this->freeCamMode = 0;
    this->field_0x110 = 0;
    this->flFastForwardWeight = 1.0f;
    this->boostTouchThrust = 0.0f;
    this->boostTouchDuration = 100.0f;
    this->lastTapTime = 0;
    this->lastTapTimeHigh = 0;
    this->lastAlignTime = 0;
    this->lastAlignTimeHigh = 0;
    this->thrustActive = 0;
    this->thrustThreshold = Globals::layout->field_0x2f4;
    this->thrustBase = 0;
    this->gameRecord = 0;
    this->maneuverHoldTime = 0;
    this->maneuverActive = 0;
    this->maneuverStartX = 0;
    this->maneuverStartY = 0;

    uint64_t t = static_cast<ApplicationManager *>(
        Globals::appManager)->GetCurrentTimeMillis();

    *(uint16_t *) &this->needsRedraw = 0x101;
    this->field_0xcf = 0;
    this->choiceItemCount = 0;
    this->field_0xd4 = 0;
    this->field_0x1e4 = 0;
    this->field_0x1e6 = 0;
    *reinterpret_cast<uint64_t *>(&this->startTime) = t;
    *reinterpret_cast<uint64_t *>(&this->lastTime) = t;
    this->field_0x1dd = AbyssEngine::Engine::UseAdvancedShader;

    Status::gStatus->field_0x184 = 0;
    Status::gStatus->field_0x188 = 1;
    Status::gStatus->field_0x18c = 1;
}

void MGame::handleAccelerometer() {
    Engine *engine = (Engine *) this->applicationManager->GetEngine();
    double *accel = engine->GetAccelValue();
    float yaw = (float) (accel[1] * 2.5);
    float steer = 1.0f;
    int controlDelta;
    if (yaw > 1.0f) {
right:
        controlDelta = this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime;
        this->player->right(controlDelta, steer * steer);
        goto vertical_axis;
    }
    steer = -1.0f;
    if (yaw < -1.0f)
        goto left;
    steer = yaw;
    if (!(yaw < 0.0f)) {
        if (yaw > 0.0f)
            goto right;
        goto vertical_axis;
    }
left:
    controlDelta = this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime;
    this->player->left(controlDelta, steer * steer);

vertical_axis:
    double baseD = *static_cast<Engine *>(
        this->applicationManager->GetEngine())->GetAccelValue();
    double zD = static_cast<Engine *>(
        this->applicationManager->GetEngine())->GetAccelValue()[2];
    float base = (float) baseD;
    float reference = *reinterpret_cast<float *>(Globals::options + 0x1c);
    int invert = Globals::options[0x10];
    float z = (float) zD;
    double hemisphere = ((Engine *) this->applicationManager->GetEngine())->GetAccelValue()[2];

    float axisA;
    float scale = 3.0f;
    float axisB;
    if (invert != 0) {
        float folded;
        if (!(hemisphere > 0.0)) {
            folded = base;
        } else {
            folded = 1.0f;
            if (!(base > 1.0f))
                folded = (1.0f - base) + 1.0f;
        }
        axisA = folded - reference;
        axisB = z - *reinterpret_cast<float *>(Globals::options + 0x20);
    } else {
        float folded;
        if (!(hemisphere > 0.0)) {
            folded = base;
        } else {
            folded = 1.0f;
            if (!(base > 1.0f))
                folded = (1.0f - base) + 1.0f;
        }
        axisA = reference - folded;
        axisB = *reinterpret_cast<float *>(Globals::options + 0x20) - z;
    }

    float pitch = axisA * scale;
    float secondPitch = axisB * scale;
    float absPitch = axisA * -3.0f;
    float absSecondPitch = axisB * -3.0f;
    if (pitch > 0.0f)
        absPitch = pitch;
    if (secondPitch > 0.0f)
        absSecondPitch = secondPitch;
    if (absSecondPitch > absPitch)
        pitch = secondPitch;
    float amount = 1.0f;
    if (pitch > 1.0f) {
up:
        controlDelta = this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime;
        this->player->up(controlDelta, amount * amount);
        return;
    }
    amount = -1.0f;
    if (pitch < -1.0f)
        goto down;
    amount = pitch;
    if (!(pitch < 0.0f)) {
        if (pitch > 0.0f)
            goto up;
        return;
    }
down:
    controlDelta = this->timeWarpState > 0 ? this->field_0x44 : this->deltaTime;
    this->player->down(controlDelta, amount * amount);
}


void MGame::OnTouchMove(int p1, int y, void *touch) {
    bool allowFreeCam = this->pauseOpen == 0 ||
        (this->freeCamMode != 0 && this->menuWindow->isShowingMessage() == 0 &&
         this->menuWindow->isMakingScreenshot() == 0);
    if (allowFreeCam && this->freeCamDragging != 0 && this->hudTouchFlags == 0 &&
        this->cameraMode == 3) {
        this->freeCamTouchMove(p1, y, touch);
        goto routePaused;
    }

    if (this->pauseOpen == 0) {
        this->hudTouchFlags = this->hud->touchMove(p1, y, touch);
        if ((unsigned int) this->cameraMode <= 1) {
            this->maneuverTouchMove(this->cameraMode, y, touch);
            if (this->_b1b8 != 0 && this->jumpActive == 0 &&
                (this->hudTouchFlags == 0 ||
                 (this->hudTouchFlags == 0x20 && this->activeTouchId != touch))) {
                bool apply = this->thrustEngaged != 0;
                float touchY = (float) y;
                if (!apply) {
                    float delta = touchY - this->thrustStartYFloat;
                    float distance = delta > 0.0f ? delta : -delta;
                    if (distance > (float) this->thrustThreshold) {
                        int direction = this->thrustStartYFloat > touchY ? 1 : -1;
                        this->thrustResetX = 0;
                        this->thrustStartYFloat = (float) (direction + y);
                        this->thrustBaseFloat = this->player->getThrust();
                        apply = true;
                    }
                }
                if (apply) {
                    this->thrustEngaged = 1;
                    float thrust = ((this->thrustStartYFloat - touchY) -
                                    (float) this->thrustResetX) /
                                   *(float *) &Globals::layout->field_0x2f8 +
                                   this->thrustBaseFloat;
                    if (thrust < 0.0f)
                        thrust = 0.0f;
                    float clampedThrust = 1.0f;
                    if (thrust < 1.0f)
                        clampedThrust = thrust;
                    this->player->setThrust(clampedThrust);
                    this->player->throttleChanged();
                }
            }
        }
    }

routePaused:
    if (this->pauseOpen == 0) return;

    if (this->gameOverActive != 0 || this->autopilotMenuOpen != 0 ||
        this->choiceWindowOpen != 0 || this->field_0xc1 != 0) {
        this->choiceWindow->OnTouchMove(p1, y);
        __asm__ volatile("" ::: "memory");
        return;
    }
    if (this->starMapOpen != 0) {
        if (*(uint8_t *) Globals::layout == 0)
            this->starMap->OnTouchMove(p1, y);
        else
            Globals::layout->OnTouchMove(p1, y);
        __asm__ volatile("" ::: "memory");
        return;
    }
    if (this->cutsceneActive != 0) {
        this->dialogueWindow->OnTouchMove(p1, y);
        __asm__ volatile("" ::: "memory");
        return;
    }
    if (this->menuTouchOpen != 0) {
        MGameAppData *ad = reinterpret_cast<MGameAppData *>(
            static_cast<ApplicationManager *>(Globals::appManager)->GetApplicationData());
        if (ad->modalActive == 0 && ad->transitionActive == 0) {
            this->menuWindow->OnTouchMove(p1, y, touch);
            __asm__ volatile("" ::: "memory");
        }
    }
}


void MGame::setCinematicMode(bool on) {
    this->freeCamMode = on;
    Globals::isCinematicModeActive = on;
    if (!on) {
        AbyssEngine::Engine::UseAdvancedShader = this->field_0x1dd;
        switchCamera(this->cinematicPrevCamMode);
        TFC_setLookAtCam(this->camera, this->cinematicPrevLookAt);
        return;
    }

    this->field_0x1dd = AbyssEngine::Engine::UseAdvancedShader;
    AbyssEngine::Engine::UseAdvancedShader = 1;
    if (this->jumpDriveActive == 0 && this->jumpActive == 0) {
        this->cinematicPrevCamMode = this->cameraMode;
        this->cinematicPrevLookAt = this->camera->isInLookAtMode();
        this->camera->setLookAtCam(false);
        switchCamera(3);
        this->level->lodManager->forceUpdate(this->deltaTime, true);
    }
}

int MGame::updateJumpScene() {
    int exitPhase = 1;
    if (this->usingJumpDrive != 0 && this->jumpFlash != 0) {
        AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) PaintCanvas::gCanvas->
            TransformGetTransform(this->jumpFlash->transform);
        if (1700LL - transform->currentTime >= 0)
            exitPhase = 0;
    } else if ((*this->level->getLandmarks())[1] != 0 &&
               ((PlayerJumpgate *) (*this->level->getLandmarks())[1])->timeToJump() == 0) {
        exitPhase = 0;
    }

    if (!exitPhase) {
        if (this->usingJumpDrive != 0) {
            int delta = this->deltaTime;
            Player *ship = (Player *) this->player->player;
            Vector offset = {(float) (5 * delta), (float) (2 * delta),
                             (float) (-5 * delta)};
            this->egoJumpPos = AbyssEngine::AEMath::MatrixRotateVector(ship->transformMatrix, offset);
        } else {
            int delta = this->deltaTime;
            Player *ship = (Player *) this->player->player;
            Vector offset = {(float) (5 * delta), (float) (2 * delta),
                             (float) (-3 * delta)};
            this->egoJumpPos = AbyssEngine::AEMath::MatrixRotateVector(ship->transformMatrix, offset);
        }
        this->camera->translate(this->egoJumpPos.x, this->egoJumpPos.y, this->egoJumpPos.z);
        Vector jumpPosition;
        if (this->usingJumpDrive != 0)
            jumpPosition = this->jumpFlash->getPosition();
        else
            jumpPosition = (*this->level->getLandmarks())[1]->getPosition();
        this->egoJumpPos = jumpPosition;
    }

    if (this->usingJumpDrive != 0) {
        AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) PaintCanvas::gCanvas->
            TransformGetTransform(this->jumpFlash->transform);
        transform->Update(this->deltaTime, false);
    }

    if (this->camera->getPosition()->z < this->egoJumpPos.z - 10000.0f &&
        this->usingJumpDrive == 0) {
        PlayerJumpgate *gate = (PlayerJumpgate *) (*this->level->getLandmarks())[1];
        gate->activate();
        Vector playerPosition = this->player->getPosition();
        if (playerPosition.z >= this->egoJumpPos.z - 16500.0f &&
            this->jumpGateSoundStarted == 0) {
            Globals::sound->stop(this->player->field_0x1c);
            Globals::sound->stop(0x8d5);
            Globals::sound->stop(0x8d4);
            Globals::sound->stop(0x23);
            Globals::sound->play(0x1f, 0, 0, 0.0f);
            this->jumpGateSoundStarted = 1;
        }
    }

    if (exitPhase) {
        this->player->setSpeed(90.0f);
        this->player->setVisible(false);
        this->player->setExhaustVisible(false);
    }

    if (this->usingJumpDrive != 0) {
        AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) PaintCanvas::gCanvas->
            TransformGetTransform(this->jumpFlash->transform);
        if (transform->animating)
            return 0;
    } else if (!((PlayerJumpgate *) (*this->level->getLandmarks())[1])->animationEnded()) {
        return 0;
    }

    if (Status::gStatus->getCurrentCampaignMission() == 0x2a &&
        Status::gStatus->inAlienOrbit() != 0) {
        this->levelScript->setEvent(6);
        this->player->setSpeed(0.0f);
        mgame_set_ki_position_slot_0x48(
                (*this->level->getLandmarks())[3], 25000.0f, 20000.0f, -55000.0f);
        this->egoJumpPos = (*this->level->getLandmarks())[3]->getPosition();
        this->player->setPosition(this->egoJumpPos.x, this->egoJumpPos.y,
                                  this->egoJumpPos.z);
        this->player->inWormhole = 1;
        this->jumpDriveActive = 0;
        this->player->resetChargingDrive();
    } else {
        Status::gStatus->departStation((Station *) Level::programmedStation);
        Level::setInitStreamOut();
        if (this->usingJumpDrive == 0)
            Status::gStatus->jumpgateUsed();
        if (((Station *) Level::programmedStation)->equals(Status::gStatus->playerStation)) {
            Level::comingFromAlienWorld = 1;
            Level::initStreamOutPosition = 1;
            Status::gStatus->setStation(Status::gStatus->playerStation);
        }
        Level::programmedStation = 0;
        Status::gStatus->field_64 = ((Player *) this->player->player)->getHitpoints();
        Status::gStatus->field_5c = ((Player *) this->player->player)->getShieldHP();
        Status::gStatus->field_60 = ((Player *) this->player->player)->getArmorHP();
        Status::gStatus->field_68 = ((Player *) this->player->player)->getGammaHP();
        Status::gStatus->field_f4 = this->player->getCurrentSecondaryWeaponIndex();
        Globals::switch_to_target_setting = 1;
        this->active = 0;
        this->applicationManager->SetCurrentApplicationModule(2);
    }
    return 1;
}


MGame::MGame() {
    // The Android object is allocated at normal ARM word alignment even though
    // the recovered declaration remains packed to preserve its verified ABI.
    MGame *self = static_cast<MGame *>(__builtin_assume_aligned(this, 4));

    *reinterpret_cast<uint64_t *>(&self->freeCamFinger1X) = 0;
    *reinterpret_cast<uint64_t *>(&self->freeCamFinger1Z) = 0;
    *reinterpret_cast<uint64_t *>(&self->flShakeAmpX) = 0;
    *reinterpret_cast<uint64_t *>(&self->field_0x144) = 0;
    *reinterpret_cast<uint64_t *>(&self->field_0x18c) = 0;
    *reinterpret_cast<uint64_t *>(&self->field_0x194) = 0;
    self->elapsedTime = 0;
    self->elapsedTimeHigh = 0;
    self->egoJumpPosX = 0;
    self->egoJumpPosY = 0;
    self->egoJumpPosZ = 0;
    self->field_0x1bc = 0;
    self->thrustStartY = 0;
    self->field_0x1c4 = 0;
    self->freeCamFinger0Y = 0;
    self->freeCamFinger0Z = 0;
    self->flShakePhaseY = 0;
    self->field_0x150 = 0;
    *reinterpret_cast<uint64_t *>(reinterpret_cast<uint8_t *>(self) + 0x19c) = 0;

    self->loadProgress = 100;
    self->loadingImage = -1;
    self->cameraMode = 0;

    *reinterpret_cast<uint64_t *>(&self->frameTime) = 0;
    *reinterpret_cast<uint64_t *>(&self->field_0x38) = 0;
    *reinterpret_cast<uint64_t *>(&self->startTime) = 0;
    *reinterpret_cast<uint64_t *>(&self->lastTime) = 0;
    *reinterpret_cast<uint64_t *>(&self->radar) = 0;
    *reinterpret_cast<uint64_t *>(&self->menuWindow) = 0;
    *reinterpret_cast<uint64_t *>(&self->field_0x70) = 0;
    *reinterpret_cast<uint64_t *>(&self->level) = 0;

    self->pauseSnapshot = 0;
    self->active = 0;
    *reinterpret_cast<uint16_t *>(&self->turretMode) = 0;
    self->jumpFlash = 0;
    self->camera = 0;
    *(uint8_t *) &self->field_0x1e4 = 0;
    self->field_0x1d4 = 0;
    self->deltaTime = 0;
    *(uint64_t *) &self->player = 0;
    *(uint16_t *) &self->gameOverActive = 0;
    *(uint64_t *) &self->starMap = 0;
    self->orbitMenuOpen = 0;
    self->dockChoiceFlags = 0;
    self->choiceWindowFlags = 0;
    self->field_0xca = 0;
    *(uint64_t *) &self->field_0xd8 = 0;
    self->field_0x1d8 = 0x42c80000;
    self->field_0x1e0 = 0;
}

void *Radio_dtor(...);

void *DialogueWindow_dtor(...);


// The application module returned by GetApplicationModule() owns the active
// StarMap pointer at offset 0x10.
struct StarMapModule {
    uint8_t _pad0[0x10];
    StarMap *starMap;  // offset 0x10
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(StarMapModule, starMap) == 0x10, "StarMapModule::starMap @ 0x10");
#endif

void MGame::OnRelease() {
    Engine *eng = (Engine *) this->applicationManager->GetEngine();
    eng->SetPostEffect(0x1400002, false);

    if (Globals::sound != 0) {
        Globals::sound->setDownPitch(false);
        Globals::sound->disableReverb();
        Globals::sound->stopAllSoundFXEvents();
    }

    delete this->level;

    this->elapsedTime = 0;
    this->elapsedTimeHigh = 0;
    this->cameraMode = 0;
    this->level = 0;
    this->active = 0;
    this->field_0x70 = 0;
    this->deltaTime = 0;
    this->gameOverActive = 0;
    this->player = 0;
    this->field_0x5c = 0;

    // 0x30 run: frameTime, frameTimeHigh, field_0x38, field_0x3c
    this->frameTime = 0;
    this->frameTimeHigh = 0;
    this->field_0x38 = 0;
    this->field_0x3c = 0;
    // 0x20 run: startTime, startTimeHigh, lastTime, lastTimeHigh
    this->startTime = 0;
    this->startTimeHigh = 0;
    this->lastTime = 0;
    this->lastTimeHigh = 0;

    delete this->jumpFlash;
    this->jumpFlash = 0;

    delete this->hud;
    this->hud = 0;

    delete this->levelScript;
    this->levelScript = 0;

    if (this->radar != 0)
        delete this->radar;
    this->radar = 0;

    delete this->radio;
    this->radio = 0;

    StarMapModule *m1 = (StarMapModule *) ApplicationManager::gAppManager->GetApplicationModule(5);
    if (m1->starMap != 0) {
        StarMapModule *m2 = (StarMapModule *) ApplicationManager::gAppManager->GetApplicationModule(5);
        delete m2->starMap;
    }
    StarMapModule *m3 = (StarMapModule *) ApplicationManager::gAppManager->GetApplicationModule(5);
    m3->starMap = 0;

    delete this->menuWindow;
    this->menuWindow = 0;

    delete this->dialogueWindow;
    this->dialogueWindow = 0;

    delete this->starMap;
    this->starMap = 0;

    delete this->choiceWindow;
    this->choiceWindow = 0;

    *reinterpret_cast<uint64_t *>(&this->field_0xd8) = 0;
    this->choiceWindow = 0;
    *(uint16_t *) &this->turretMode = 0;
    this->jumpFlash = 0;
    *(uint32_t *) ((char *) this + 197) = 0;
    *(uint32_t *) ((char *) this + 203) = 0;

    if (this->camera != 0)
        delete this->camera;
    this->camera = 0;

    GameRecord *record = reinterpret_cast<GameRecord *>(static_cast<intptr_t>(this->gameRecord));
    if (record != nullptr) {
        record->~GameRecord();
        ::operator delete(record);
    }
    this->gameRecord = 0;

    PaintCanvas::gCanvas->ReleaseAllResources();

    Globals *globals = static_cast<Globals *>(Globals::globals);
    int lang = GameText::getLanguage();
    globals->loadFont(lang);

    if (Globals::layout != 0) {
        Globals::layout->reload();
        ((ImageFactory *) Globals::imageFactory)->reload();
        Globals::layout->initTip();
    }

    ArrayReleaseClasses(*this->missionInfoLines);
    if (this->missionInfoLines != 0) {
        delete this->missionInfoLines;
    }
    this->missionInfoLines = 0;

    if (Globals::sound != 0)
        Globals::sound->freeAllEvents();
}


void MGame::OnRender2D() {
    if (this->active == 0) return;
    PaintCanvas::gCanvas->Begin2d();
    Mission *campaign;

    if (this->pauseOpen != 0 && this->menuTouchOpen != 0) {
        if ((this->freeCamMode != 0 && this->freeCamDragging != 0 &&
             this->menuWindow->pendingActivate == 0) ||
            (this->menuWindow->draw(), this->freeCamMode != 0)) {
            this->level->getStarSystem()->render2D();
        }
        Vector center = {0.5f, 0.5f, 0.0f};
        Engine *engine = (Engine *) this->applicationManager->GetEngine();
        *(Vector *) &engine->field_0x3cc = center;
        goto end2d;
    }

    if (this->needsRedraw == 0) {
        this->level->getStarSystem()->render2D();
        if (this->levelScript->startSequenceOver() != 0 ||
            this->levelScript->startSequence() == 0) {
            this->radio->draw(this->levelScript->scriptTime, this->player,
                              this->levelScript);
        }
        PaintCanvas::gCanvas->End2d();
        return;
    }

    if (this->starMapOpen != 0) {
        this->starMap->draw();
        if (*(uint8_t *) Globals::layout != 0)
            Globals::layout->drawHelpWindow();
        PaintCanvas::gCanvas->End2d();
        return;
    }

    this->level->render2D();
    if (Globals::mouseCursorActivated == 0)
        this->hud->drawPauseButton();

    campaign = (Mission *) (intptr_t) Status::gStatus->getCampaignMission();
    if (campaign->getType() == 0xaa) {
        if (this->levelScript->getEvent() == 0)
            this->hud->drawOrbitInformation();
        this->radio->draw(this->levelScript->scriptTime, this->player,
                          this->levelScript);
        goto sequenceTail;
    }

    if (this->jumpActive == 0 && this->jumpDriveActive == 0) {
        if (this->gameOverActive != 0) {
            uint64_t elapsed = (uint64_t) this->elapsedTime |
                               ((uint64_t) (uint32_t) this->elapsedTimeHigh << 32);
            if (elapsed >= 3001) {
                int fade = this->loadingTime >= 4000
                    ? -1
                    : static_cast<int>((float) this->loadingTime / 4000.0f * 255.0f) - 256;
                this->paintCanvas->SetColor(fade);
                PaintCanvas::gCanvas->DrawImage2D(this->loadingImage, 0, 0, 0x44, 0x44);

                if (this->loadingTime >= 4000) {
                    ApplicationManager *app = static_cast<ApplicationManager *>(
                        Globals::appManager);
                    float pulseSign = AbyssEngine::AEMath::Sinf(
                        (float) app->GetSystemTimeMillis() * 0.003f);
                    float pulse = AbyssEngine::AEMath::Sinf(
                        (float) app->GetSystemTimeMillis() * 0.003f);
                    float pulseAlpha = -pulse;
                    if (pulseSign > 0.0f)
                        pulseAlpha = pulse;
                    PaintCanvas::gCanvas->SetColor(0xff, 0xff, 0xff,
                        (unsigned char) (pulseAlpha * 255.0f));
                    String text(*Globals::gameText->getText(199), false);
                    if (this->gameRecord != 0) {
                        int y = (Globals::h >> 1) +
                                (PaintCanvas::gCanvas->GetImage2DHeight(this->loadingImage) >> 1) + 10;
                        static_cast<Globals *>(Globals::globals)->drawLines(Globals::font, this->missionInfoLines,
                                                     Globals::w >> 1, y, true);
                    } else {
                        int x = (Globals::w >> 1) -
                                (PaintCanvas::gCanvas->GetTextWidth(Globals::font, text) >> 1);
                        int y = (Globals::h >> 1) +
                                (PaintCanvas::gCanvas->GetImage2DHeight(this->loadingImage) >> 1) + 10;
                        PaintCanvas::gCanvas->DrawString(Globals::font, text, x, y, false);
                    }
                }
            }
        } else {
            this->player->draw(true);
            if (this->player->isMining() == 0 && this->starMapOpen == 0 &&
                (this->player->isHacking() == 0 || this->turretMode != 0)) {
                int radarDelta = this->pauseOpen != 0 ? 0 : this->deltaTime;
                this->radar->draw((Player *) this->player->player, this->hud, radarDelta);
            }

            if (this->cutsceneActive != 0) {
                this->dialogueWindow->draw();
            } else {
                int hudDelta = this->pauseOpen != 0 ? 0 : this->deltaTime;
                int64_t missionTime = (int64_t) this->levelScript->m_nTimeLimit -
                                      this->levelScript->scriptTime;
                this->hud->draw(hudDelta, missionTime, this->player, this->_b5c,
                                this->cameraMode, this->nextCamId(this->cameraMode));
                this->radio->draw(this->levelScript->scriptTime, this->player,
                                  this->levelScript);
                this->radar->drawCurrentLock(this->hud);
                Globals::layout->drawMissionRewardMessage(false);
            }

            if (this->autopilotMenuOpen != 0 || this->field_0xc6 != 0 ||
                this->choiceWindowOpen != 0 || this->field_0xc1 != 0)
                this->choiceWindow->draw();
            if (this->hudMenuOpen != 0)
                this->hud->drawMenu(this->deltaTime);
        }
        goto fade;
    }

    if (Status::gStatus->getCurrentCampaignMission() >= 8 &&
        *reinterpret_cast<uint8_t *>(&this->levelScript->m_nFlags) == 0 &&
        this->jumpDriveActive == 0 &&
        this->player->isDockingToPlanet() == 0 && this->levelScript->getEvent() == 0) {
        this->hud->drawOrbitInformation();
    }
    if (this->levelScript->startSequenceOver() != 0 ||
        this->levelScript->startSequence() == 0) {
        this->radio->draw(this->levelScript->scriptTime, this->player,
                          this->levelScript);
    }

sequenceTail:
    if (this->cutsceneActive != 0)
        this->dialogueWindow->draw();
    this->field_0x110 = 0;
    this->_b5c = 0;

fade:
    Globals::layout->drawFade();

end2d:
    PaintCanvas::gCanvas->End2d();
}

void MGame::dialogueEvent() {
    if (this->levelScript->startSequenceOver() == 0) return;
    if (DialogueWindow::hasBriefingDialogue(Status::gStatus->getCurrentCampaignMission()) == 0) {
        if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0) return;
    }
    if (((Mission *) (Status::gStatus->getMission()))->isEmpty() != 0) return;
    if (((Mission *) (Status::gStatus->getMission()))->getType() == 8) return;
    if (((Mission *) (Status::gStatus->getMission()))->getType() == 0xa6) return;
    if (((Mission *) (Status::gStatus->getMission()))->getType() == 0) return;
    if (((Mission *) (Status::gStatus->getMission()))->getType() == 0xb7) return;
    if (((Mission *) (Status::gStatus->getMission()))->isVisible() == 0) return;
    if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() == 0) {
        if (((Mission *) (Status::gStatus->getMission()))->getType() == 0xb) return;
    }
    if (this->dialogueWindow == 0) {
        this->dialogueWindow = new DialogueWindow(
            (Mission *) (intptr_t) Status::gStatus->getMission(), this->level, 0);
    }
    this->player->setTurretMode(0);
    this->levelScript->resetCamera((Level *) (this->level) /* level: arg lost in decomp */);
    this->player->setFreeLookMode(0);
    TFC_enableFirstPersonCam(this->camera, 0);
    this->player->hideShipForFirstPersonCameraView(0);
    LevelScript *cam = this->levelScript;
    this->needsRedraw = 1;
    cam->field_0x8 = 0;
    cam->field_0xc = 0;
    this->pauseOpen = 1;
    ((MGame *) (this))->pauseSounds();
    this->cutsceneActive = 1;
}

int MGame::nextCamId(int cur) {
    int id = cur + 1;
    if (id == 2) id = cur + 2;
    if (id == 1) {
        if (((Ship *) (Globals::status->getShip()))->getFirstEquipmentOfSort(8) != 0 ||
            ((Ship *) (Globals::status->getShip()))->getFirstEquipmentOfSort(0x23) != 0) {
            id = this->player->hasAutoTurret() == 0 ? 1 : 2;
        } else {
            id = 2;
        }
    }
    if (id == 2) id = 3;
    if (id >= 4) {
        if (this->player->isDockedToDockingPoint() == 0) return 0;
        if (((Ship *) (Globals::status->getShip()))->getFirstEquipmentOfSort(8) == 0 &&
            ((Ship *) (Globals::status->getShip()))->getFirstEquipmentOfSort(0x23) == 0) {
            return 3;
        }
        id = this->player->hasAutoTurret() != 0 ? 3 : 1;
    }
    return id;
}
