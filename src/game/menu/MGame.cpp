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
#include "game/ship/KIPlayer.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerFixedObject.h"
#include "game/ship/Agent.h"
#include "game/mission/Status.h"
#include "engine/render/PaintCanvas.h"
#include "game/weapons/Radar.h"

class Music;
class Cfg;


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


static int *g_maneuverScale;

void MGame::maneuverTouchEnd(int a, int b, void *p) {
    MGame *self = this;
    (void) b;
    (void) p;
    if (self->maneuverActive != 0 && self->maneuverHoldTime <= 0x258) {
        float f2 = (float) (*g_maneuverScale);
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


static Music **g_music;

static Cfg **g_cfg;

void MGame::OnResume() {
    Music **mp = g_music;
    if (*mp == 0) return;
    if (FModSound_tryToStopMusicForBGMusic() != 0) return;
    return Music_resume(*mp, 1, **(int **) g_cfg);
}



void MGame::maneuverTouchMove(int a, int b, void *p) {
    (void) a;
    (void) p;
    if (this->maneuverActive != 0) {
        float f2 = (float) (*g_maneuverScale);
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

long long MGame::OnKeyPress(long long key, long long /*mod*/) { return key; }
long long MGame::OnKeyRelease(long long key, long long /*mod*/) { return key; }

void MGame::showLiteScreen() {
}

int MGame::ShowLoadingScreen() { return 1; }

void MGame::pause() {
}

void MGame::OnRender3D() {
    if (this->active == 0) return;
    PaintCanvas::gCanvas->ClearBuffer(0);

    uint8_t inMenuLevel = this->pauseOpen;
    uint8_t flag15e = this->freeCamMode;

    if (inMenuLevel == 0) {
        this->level->renderBG(0);
        PaintCanvas::gCanvas->Begin3d();
        int arg = (flag15e == 0) ? this->deltaTime : 0;
        this->level->render(arg);
        int egoFlag = (this->jumpActive != 0) ? 0 : (this->jumpDriveActive == 0);
        this->player->render(egoFlag);
        if (this->jumpFlash != 0)
            ((AEGeometry *) (this->player))->render();
        this->levelScript->render3D();
        return PaintCanvas::gCanvas->End3d();
    }

    if (flag15e != 0) {
        this->level->renderBG(0);
        PaintCanvas::gCanvas->Begin3d();
        this->level->render(this->deltaTime);
        int egoFlag = (this->jumpActive != 0) ? 0 : (this->jumpDriveActive == 0);
        this->player->render(egoFlag);
        if (this->jumpFlash != 0)
            ((AEGeometry *) (this->player))->render();
        this->levelScript->render3D();
        return PaintCanvas::gCanvas->End3d();
    }

    if (this->menuTouchOpen != 0) {
        PaintCanvas::gCanvas->Begin3d();
        this->menuWindow->render3D();
        return PaintCanvas::gCanvas->End3d();
    }

    if (this->starMapOpen != 0) {
        PaintCanvas::gCanvas->Begin3d();
        this->starMap->render();
        return PaintCanvas::gCanvas->End3d();
    }

    this->level->renderBG(0);
    PaintCanvas::gCanvas->Begin3d();
    this->level->render(0);
    int egoFlag = (this->jumpActive != 0) ? 0 : (this->jumpDriveActive == 0);
    this->player->render(egoFlag);
    if (this->jumpFlash != 0)
        ((AEGeometry *) (this->player))->render();
    this->levelScript->render3D();
    return PaintCanvas::gCanvas->End3d();
}

void TFC_setActive(TargetFollowCamera *c, int v);

float TFC_useTargetsUpVector(TargetFollowCamera *c, int v);

void FModSound_setProp(int snd, int id);

void TFC_setPosition(TargetFollowCamera *c, float x, float y, float z);


static int *g_jsSound;

static int g_jsHudFlag;

static int g_jsFovDefault;

static int g_jsFovAlienA;

static int g_jsFovAlienB;

static int g_jsPostEffect;

static int g_jsOffsetX;

static int g_jsOffsetY;

static int g_jsOffsetZ;

static int g_jsOffsetZ2;

void MGame::startJumpScene() {
    ((Player *) (this->player->player))->setVulnerable(0);
    this->level->enableFog(0);

    if (this->player->isDockedToDockingPoint() != 0) {
        this->player->dockToDockingPoint(nullptr, nullptr);
        TFC_setActive(this->camera, 1);
        TFC_setLookAtCam(this->camera, 0);
        float sp = TFC_useTargetsUpVector(this->camera, 0);
        this->player->setSpeed(sp);
        this->player->setDockingState(0);
    }
    if (this->player->isInTurretMode() != 0)
        this->player->setTurretMode(0);

    int *snd = g_jsSound;
    ((FModSound *) (*snd))->stop(0x23);
    ((MGame *) (this))->switchCamera(0);
    this->field_0x70 = g_jsHudFlag;
    this->hud->releaseAllKeys();
    this->field_0x110 = 0;
    this->field_0x5c = 0;

    PaintCanvas *pc = PaintCanvas::gCanvas;
    unsigned cam = this->cameraId;
    float fov = *(float *) &g_jsFovDefault;
    if (Status::gStatus->inAlienOrbit() != 0) {
        int cm = Status::gStatus->getCurrentCampaignMission();
        fov = (cm < 0x50) ? *(float *) &g_jsFovAlienB : *(float *) &g_jsFovAlienA;
    }
    pc->CameraSetPerspective(cam, fov, *(float *) &g_jsHudFlag, 0);
    this->player->setAutoPilot(0);
    this->pauseOpen = 0;
    this->hudMenuOpen = 0;
    this->jumpDriveActive = 1;
    this->jumpActive = 1;
    this->player->setCollide(0);
    TFC_setLookAtCam(this->camera, 1);
    this->player->stopBoost();
    Engine *eng = (Engine *) this->applicationManager->GetEngine();
    ((Engine *) (eng))->SetPostEffect(g_jsPostEffect, 0);

    float camX, camY, camZ;
    if (this->usingJumpDrive == 0) {
        Array<KIPlayer *> *lm = this->level->getLandmarks();
        KIPlayer *obj = (*lm)[0];
        this->egoJumpPos = obj->getPosition();
        float nz = (float) this->egoJumpPosZ + *(float *) &g_jsOffsetZ;
        this->egoJumpPosZ = (int) nz;
        if (this->player->geometry != nullptr) {
            Vector pos = this->player->getPosition();
            ((AEGeometry *) (this->player->geometry))->setPosition(pos);
        }
        this->player->setComputerControlled(1);
        ((AEGeometry *) this->player->geometry)->setRotation((float) 0, (float) 0, (float) 0);
        this->egoJumpPosX = (int) ((float) this->egoJumpPosX + *(float *) &g_jsOffsetX);
        this->egoJumpPosY = (int) ((float) this->egoJumpPosY + *(float *) &g_jsOffsetY);
        this->egoJumpPosZ = (int) ((float) this->egoJumpPosZ + *(float *) &g_jsOffsetZ2);
        camX = (float) this->egoJumpPosX;
        camY = (float) this->egoJumpPosY;
        camZ = (float) this->egoJumpPosZ;
    } else {
        this->player->resetMovement();
        this->player->setComputerControlled(1);
        AEGeometry *geo = new AEGeometry((uint16_t) 0x3ab2, PaintCanvas::gCanvas, false);
        this->jumpFlash = geo;
        int tr = (int) (long) PaintCanvas::gCanvas->TransformGetTransform((unsigned) (uintptr_t) PaintCanvas::gCanvas);
        ((AbyssEngine::Transform *) (tr))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);

        float pos[4];
        ((PlayerEgo *) (pos))->getPosition();
        Vector *dst = &this->egoJumpPos;
        *(Vector *) (dst) = *(const Vector *) ((Vector *) pos);

        float dir[4];
        ((PlayerEgo *) (dir))->getPosition();
        *(Vector *) ((Vector *) dir) *= (*(float *) &g_jsOffsetX);
        *(Vector *) (dst) += *(const Vector *) ((Vector *) dir);
        this->jumpFlash->setPosition(*dst);

        this->jumpFlash->setScaling(2.0f);
        float zero[4];
        zero[0] = 0;
        zero[1] = 1.0f;
        zero[2] = 0;
        this->jumpFlash->setDirection(*(Vector *) zero, *(Vector *) zero /* up: arg lost in decomp */);

        float off[4];
        off[0] = (float) g_jsOffsetX;
        off[1] = (float) g_jsOffsetY;
        off[2] = (float) g_jsOffsetX;
        *(Vector *) ((Vector *) off) = *(const Vector *) ((Vector *) off);
        Vector *playerTransformPos = reinterpret_cast<Vector *>(((Player *) this->player->player)->transform);
        *playerTransformPos = AbyssEngine::AEMath::MatrixRotateVector(
            *(const AbyssEngine::Matrix *) (off), *playerTransformPos);
        *(Vector *) ((Vector *) off) = *(const Vector *) ((Vector *) off);
        *(Vector *) (dst) += *(const Vector *) ((Vector *) off);

        FModSound_setProp(*snd, this->field_0x1c);
        FModSound_setProp(*snd, 0x23);
        FModSound_setProp(*snd, 0x8d5);
        FModSound_setProp(*snd, 0x8d4);
        ((FModSound *) (*snd))->play(0x20, 0, 0 /* vel: arg lost in decomp */, 0.0f);

        camX = (float) this->egoJumpPosX;
        camZ = (float) this->egoJumpPosY;
        camY = (float) this->egoJumpPosZ;
    }
    TFC_setPosition(this->camera, camX, camY, camZ);
}

void TFC_setRotationAroundTarget(TargetFollowCamera *c, int v);

int TFC_hideShipForFirstPersonCam(TargetFollowCamera * c);

void MGame::switchCamera(int id) {
    int turretArg;
    int savedMode;

restart:

    savedMode = this->cameraMode;
    if (id == 2) id = 3;
    this->cameraMode = id;
    if (id == 1) {
        if (this->player->isDockingToAsteroid() != 0) {
            this->turretMode = 0;
        } else {
            ((PlayerEgo *) this->player)->setTurretMode(1);
            int t = 1;
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
        ((PlayerEgo *) (ego))->hideShipForFirstPersonCameraView(v != 0);
    }
}


static int *g_fcb_guard;

void MGame::freeCamTouchBegin(int x, int y, void *idPtr) {
    int id = (int) (intptr_t) idPtr;
    char buf[12];
    float fy = (float) y;
    float fx = (float) x;
    Vector *fingerSlot;
    if (this->touch0Id == 0) {
        if (this->touch1Id == 0) this->menuTime = 0;
        *(volatile float *) (buf + 4) = fy;
        *(volatile float *) (buf + 0) = fx;
        *(volatile int *) (buf + 8) = 0;
        this->touch0Id = id;
        fingerSlot = &this->freeCamFinger1;
    } else {
        if (this->menuTime >= 1000) goto tail;
        this->flCameraRoll = 0;
        float len = AbyssEngine::AEMath::VectorLength(*TFC_getCamOffset(this->camera));
        *(volatile float *) (buf + 4) = fy;
        *(volatile float *) (buf + 0) = fx;
        this->flCameraRoll = len;
        fingerSlot = &this->freeCamFinger0;
        this->touch1Id = id;
    }

    *fingerSlot = *(const Vector *) (buf);
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
    void (::PlayerEgo::*pmf)() = &::PlayerEgo::toggleCloaking;
    int(*toggleCloaking)(::PlayerEgo *);
    __builtin_memcpy(&toggleCloaking, &pmf, sizeof(toggleCloaking));
    if (toggleCloaking((::PlayerEgo *) (this->player)) != 0) return;
    if (this->choiceWindow == 0)
        this->choiceWindow = new ChoiceWindow();
    Item *eq = ((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(0x15);
    int attr = eq == 0 ? 0 : ((Item *) (eq))->getAttribute(0x26);
    ChoiceWindow *cw = this->choiceWindow;
    void *txt = ((GameText *) (*g_gameText))->getText(0x247);
    String s0, s1, s2, s3, s4, s5;
    s2.ctor_char("", false);
    s3 = *(String *) txt + s2;
    s1.Set((long long) (attr));
    s4 = s3 + s1;
    s0.ctor_char("", false);
    s5 = s4 + s0;
    ((ChoiceWindow *) (cw))->set(*(String *) &s5);
    this->pauseOpen = 1;
    this->choiceWindowOpen = 1;
    ((MGame *) (this))->pauseSounds();
}


static int **g_goWormText;

static int **g_goDeathText;

static DialogueWindow **g_goDlgA;

static int **g_goStatusA;

static int **g_goStatusB;

static int **g_goStatusC;

static int **g_goStatusD;

static int **g_goRecHandler;

static int **g_goRecId;

static int **g_goSnd;

void MGame::gameOverCheck() {
    if (this->player->getHitpoints() <= 0) {
        if (this->player->tryToStartEmergencySystem() != 0)
            return;
        if (this->player->isInWormhole() == 0) {
            this->player->setTurretMode(0);
            this->levelScript->resetCamera(this->level);
            this->player->setFreeLookMode(0);
            TFC_enableFirstPersonCam(this->camera, 0);
            this->player->hideShipForFirstPersonCameraView(0);
            this->needsRedraw = 1;
            this->player->explode();
            if (this->player->explosionEnded() != 0) {
                this->gameOverActive = 1;
                String *t = (String *) ((GameText *) (**g_goDeathText))->getText(0);
                this->gameOverTitle = *(t);
            }
        } else {
            this->gameOverActive = 1;
            String *t = (String *) ((GameText *) (**g_goWormText))->getText(0);
            this->gameOverTitle = *(t);
            this->needsRedraw = 1;
        }

        if (this->gameOverActive != 0) {
            if (Status::gStatus->getMission() != 0 &&
                ((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0) {
                int cm = Status::gStatus->getCurrentCampaignMission();
                int *sa = *g_goStatusA;
                int v;
                if (cm == *sa) {
                    v = **g_goStatusB + 1;
                } else {
                    **g_goStatusC = 0;
                    v = 1;
                }
                *sa = cm;
                **g_goStatusD = v;
            }
            int rec = (int) (intptr_t)((RecordHandler *) ((RecordHandler *) **g_goRecHandler))->recordStoreRead(
                **g_goRecId);
            this->gameRecord = rec;
        }
    }

    if (this->level->checkGameOver(0) != 0) {
        if (this->dialogueWindow == 0) {
            DialogueWindow *w = (DialogueWindow *) ::operator new(0x74);
            DialogueWindow_ctor(w);
            Level *lvl = this->level;
            this->dialogueWindow = w;
            if (lvl != 0) w->setLevel(lvl);
        } else if (this->dialogueWindow->hasLevel() == 0) {
            Level *lvl = this->level;
            if (lvl != 0) this->dialogueWindow->setLevel(lvl);
        }
        this->dialogueWindow->set(Status::gStatus->getMission(), 2, -1);
        this->cutsceneActive = 1;
        ((MGame *) (this))->pauseSounds();
        this->pauseOpen = 1;
    }

    unsigned lo = this->elapsedTime;
    int hi = this->elapsedTimeHigh;
    bool ready = !(hi < (int) (lo < 0x1389));
    if (ready) {
        this->elapsedTime = 0;
        this->elapsedTimeHigh = 0;
        int *sc = (int *) this->levelScript;
        if (sc[0] >= 1) {
            bool done = !((long long) (unsigned) sc[0] - ((long long) sc[3] << 32 | (unsigned) sc[2]) < 0);
            bool survival = false;
            if (!done) {
                int cm = Status::gStatus->getCurrentCampaignMission();
                if (cm != 0x2a) {
                    Objective *obj = this->level->objectivesA;
                    survival = (obj == 0) || (((Objective *) (obj))->isSurvivalObjective() != 0);
                }
                if (!survival) {
                    if (this->dialogueWindow == 0) {
                        DialogueWindow *w = (DialogueWindow *) ::operator new(0x74);
                        DialogueWindow_ctor(w);
                        Level *lvl = this->level;
                        this->dialogueWindow = w;
                        if (lvl != 0) w->setLevel(lvl);
                    } else if (this->dialogueWindow->hasLevel() == 0) {
                        Level *lvl = this->level;
                        if (lvl != 0) this->dialogueWindow->setLevel(lvl);
                    }
                    this->dialogueWindow->set(Status::gStatus->getMission(), 2, -1);
                    this->pauseOpen = 0x101;
                    ((MGame *) (this))->pauseSounds();
                }
            }
        }
    }

    if (this->gameOverActive != 0) {
        this->elapsedTime = 0;
        this->elapsedTimeHigh = 0;
        this->loadingTime = 0;
        ((FModSound *) (**g_goSnd))->play(0x25, 0, 0 /* vel: arg lost in decomp */, 0.0f);
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

void MGame::OnTouchBegin(int p1, int p2, void *touchId) {
    MGame *self = this;

    if (self->activeTouchId == 0)
        self->activeTouchId = touchId;

    if (self->pauseOpen == 0) {
        if (self->gameOverActive != 0 && self->loadingTime >= 4000) {
            int cm = Status::gStatus->getCurrentCampaignMission();
            if (cm == 0x9e) {
                self->active = 0;
                return self->applicationManager->SetCurrentApplicationModule(2);
            }
            if (self->gameRecord != 0) {
                ((GameRecord *) (self->gameRecord))->load();
                Globals::gGlobals->playMusicAndFadeOutCurrent(**g_tbRecordTrack);
                self->active = 0;
                return self->applicationManager->SetCurrentApplicationModule(5);
            }
            Globals::gGlobals->playMusicAndFadeOutCurrent(**g_tbMenuTrack);
            self->active = 0;
            self->applicationManager->SetCurrentApplicationModule(5);
            return;
        }
    } else {
        if (self->starMapOpen != 0) {
            Layout *hl = **g_tbStarLayout;
            if (*(uint8_t *) hl != 0) {
                ((Layout *) (hl))->OnTouchBegin(p1, p2);
                return;
            }
            uint8_t r = self->starMap->OnTouchBegin(p1, p2);
            self->starMapOpen = r ^ 1;
            return;
        }
        if (self->autopilotMenuOpen != 0 || self->choiceWindowOpen != 0 ||
            self->dockChoiceOpen != 0) {
            self->choiceWindow->OnTouchBegin(p1, p2);
            return;
        }
        if (self->cutsceneActive != 0) {
            self->dialogueWindow->OnTouchBegin(p1, p2);
            return;
        }
        if (self->menuTouchOpen != 0) {
            MGameAppData *ad = (MGameAppData *) (intptr_t) ApplicationManager_GetApplicationData();
            if (ad->modalActive != 0) return;
            if (ad->transitionActive != 0) return;
            self->menuWindow->OnTouchBegin(p1, p2, touchId);
            if (self->freeCamMode == 0) return;
            if (self->menuWindow->isShowingMessage() != 0) return;
            if (self->menuWindow->isMakingScreenshot() != 0) return;

            return;
        }
    }

    unsigned hr = self->hud->touchBegin(p1, p2, touchId);
    self->hudTouchFlags = hr;
    (void) hr;
}

void MGame::OnUpdate() {
    int delta;
    int t = this->applicationManager->GetElapsedTimeMillis();
    if (t < 0x97) {
        delta = (this->applicationManager->GetElapsedTimeMillis() < 0)
                    ? 0
                    : (this->applicationManager->GetElapsedTimeMillis() < 0x97
                           ? this->applicationManager->GetElapsedTimeMillis()
                           : 0x96);
    } else {
        delta = 0x96;
    }

    this->deltaTime = delta;

    this->frameTime += delta;

    if (this->jumpDriveActive != 0)
        this->updateJumpScene();
    this->gameOverCheck();
    this->successCheck();
}


static RecordHandler **g_record;

static FModSound **g_fmod;

void Level_onSuspend(...);

void MGame::OnSuspend() {
    if (*g_record != 0) ((RecordHandler *) (*g_record))->saveOptions();
    ((MGame *) (this))->pauseSounds();
    if (this->pauseOpen == 0) {
        if (this->menuWindow == 0)
            this->menuWindow = new MenuTouchWindow(1);
        this->pauseSnapshot = 1;
        this->pauseOpen = 1;
        ((FModSound *) (*g_fmod))->pauseAllPlaying();
        this->player->PauseEngineSound();
        Array<KIPlayer *> *e = this->level->getEnemies();
        if (e != nullptr) {
            for (uint32_t i = 0; i < e->size(); i++)
                ((KIPlayer *) ((*e)[i]))->PauseEngineSound();
        }
        MenuTouchWindow *w = this->menuWindow;
        int mode = 1;
        if (this->jumpActive == 0)
            mode = this->player->isDead();
        ((MenuTouchWindow *) (w))->setCutsceneMode(mode);
        this->menuTouchOpen = 1;
    }
    return Level_onSuspend(this->hud);
}

int Station_getIndex(Station * s);


static int *g_deAutoFlag;

static int g_dePostEffect;

static int g_deTextA;

static int g_deLitA0;

static int g_deLitA1;

static int g_deTextB;

static int g_deLitB0;

static int g_deLitB1;

static int **g_deAlienFlag;

void MGame::dockEvent(int p1, int p2) {
    (void) p1;
    (void) p2;
    float pos[4];
    ((PlayerEgo *) (pos))->getPosition();
    this->touchesStream = this->level->collideStream(*(Vector *) pos);
    ((PlayerEgo *) (pos))->getPosition();
    this->touchesStation = this->level->collideStation(*(Vector *) pos);

    Status *status = (Status *) (void *) &Status::gStatus;
    Mission *m = Status::gStatus->getMission();
    bool special = ((Mission *) (m))->isEmpty() != 0 ||
                   ((Mission *) (Status::gStatus->getMission()))->getType() == 0xb ||
                   ((Mission *) (Status::gStatus->getMission()))->getType() == 0 ||
                   ((Mission *) (Status::gStatus->getMission()))->getType() == 0xbd ||
                   ((Mission *) (Status::gStatus->getMission()))->getType() == 0xab ||
                   ((Mission *) (Status::gStatus->getMission()))->getType() == 0xac;

    if (!special) {
        if ((this->touchesStation != 0 || this->touchesStream != 0) &&
            this->player->isAutoPilot() != 0) {
            this->hud->hudEvent(0x15, this->player, 0);
        }

        return;
    }

    if (this->touchesStream != 0) {
        if (this->player->goingToStream() != 0 &&
            this->player->isDockingToStream() == 0 &&
            this->player->isDockedToStream() == 0) {
            this->player->dockToStream(0);
            this->freeCamDragging = 0;
            this->needsRedraw = 1;

            return;
        }
        if (this->touchesStream != 0) {
            {
                Player *pl = (Player *) this->player->player;
                Status::gStatus->field_64 = pl->getHitpoints();
                Status::gStatus->field_5c = pl->getShieldHP();
                Status::gStatus->field_60 = pl->getArmorHP();
                Status::gStatus->field_68 = pl->getGammaHP();
            }
            Status::gStatus->field_f4 = this->player->getCurrentSecondaryWeaponIndex();
            int autop = this->player->isAutoPilot();
            int *autoFlag = g_deAutoFlag;
            if (*autoFlag == 0 || autop == 0) {
                if (this->starMapOpen != 0) {
                    this->frameTime = 0;
                    this->frameTimeHigh = 0;

                    return;
                }
                this->player->isAutoPilot();
                if (this->player->goingToStream() == 0) {
                    return;
                }
                if (*autoFlag == 0) {
                    this->freeCamDragging = 0;
                    this->needsRedraw = 1;
                    if (this->starMap == 0) {
                        StarMap *sm = new StarMap(false, nullptr, false, -1);
                        this->starMap = sm;
                    }
                    Engine *eng = (Engine *) this->applicationManager->GetEngine();
                    ((Engine *) (eng))->SetPostEffect(g_dePostEffect, 0);
                    this->starMap->initLights();
                    this->starMap->setJumpMapMode(1, 0);
                    this->player->setAutoPilot(0);
                    this->pauseOpen = 1;
                    this->starMapOpen = 1;
                    ((MGame *) (this))->pauseSounds();
                    this->needsRedraw = 1;

                    return;
                }
                if (this->autopilotMenuOpen != 0) { return; }
                this->freeCamDragging = 0;
                ChoiceWindow *cw = this->choiceWindow;
                this->needsRedraw = 1;
                if (cw == 0) {
                    cw = new ChoiceWindow();
                    this->choiceWindow = cw;
                }
                {
                    void *txt = ((GameText *) (*g_gameText))->getText(g_deTextB);
                    String name = ((Station *) (Status::gStatus->getStation()))->getName();
                    String sPrefix, sSuffix, t1, t2, t3, result;
                    sPrefix.ctor_char((const char *) (intptr_t) g_deLitB0, false);
                    t1 = *(String *) txt + sPrefix;
                    t2 = t1 + name;
                    sSuffix.ctor_char((const char *) (intptr_t) g_deLitB1, false);
                    t3 = t2 + sSuffix;
                    result = t3 + *(String *) txt;
                    ((ChoiceWindow *) (cw))->set(*(String *) &result, true);
                    ((ChoiceWindow *) (cw))->left();
                }
            } else {
                if (this->autopilotMenuOpen != 0) { return; }
                this->freeCamDragging = 0;
                ChoiceWindow *cw = this->choiceWindow;
                this->needsRedraw = 1;
                if (cw == 0) {
                    cw = new ChoiceWindow();
                    this->choiceWindow = cw;
                }
                {
                    void *txt = ((GameText *) (*g_gameText))->getText(g_deTextA);
                    String name = ((Station *) (Status::gStatus->getStation()))->getName();
                    String sPrefix, sSuffix, t1, t2, t3, result;
                    sPrefix.ctor_char((const char *) (intptr_t) g_deLitA0, false);
                    t1 = *(String *) txt + sPrefix;
                    t2 = t1 + name;
                    sSuffix.ctor_char((const char *) (intptr_t) g_deLitA1, false);
                    t3 = t2 + sSuffix;
                    result = t3 + *(String *) txt;
                    ((ChoiceWindow *) (cw))->set(*(String *) &result, true);
                    ((ChoiceWindow *) (cw))->left();
                }
            }
            this->pauseOpen = 1;
            this->autopilotMenuOpen = 1;
            ((MGame *) (this))->pauseSounds();
            this->player->setAutoPilot(0);

            return;
        }
    }

    if (this->touchesStation == 0) {
        int tgt = this->player->getAutoPilotTarget();
        Array<KIPlayer *> *lm = this->level->getLandmarks();
        if (tgt != (int) (intptr_t) lm->data() ||
            this->player->collidesWithStation() == 0) {
            return;
        }
    }

    int cm = Status::gStatus->getCurrentCampaignMission();
    if (cm > 0x30 && Status::gStatus->getCurrentCampaignMission() < 0x37) {
        if (Station_getIndex(Status::gStatus->getStation()) != 0x4a) {
            this->hud->hudEvent(0x15, this->player, 0);

            return;
        }
    }
    if (this->player->goingToStation() != 0 &&
        Status::gStatus->inAlienOrbit() == 0 &&
        ((Status *) (*(Station **) status))->inEmptyOrbit() == 0) {
        Achievements::gAchievements->checkForNewMedal(this->player);
        **g_deAlienFlag = 0;

        {
            Player *pl = (Player *) this->player->player;
            Status::gStatus->field_64 = pl->getHitpoints();
            Status::gStatus->field_5c = pl->getShieldHP();
            Status::gStatus->field_60 = pl->getArmorHP();
            Status::gStatus->field_68 = pl->getGammaHP();
        }
        this->applicationManager->SetCurrentApplicationModule(5);
        this->active = 0;
    }
}

void MGame::freeCamTouchEnd(int p1, int p2, void *idPtr) {
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
    {
        int ix = this->dragDeltaX;
        int iy = this->dragDeltaY;
        float fx = (float) ix;
        float fy = (float) iy;
        float nx = this->flShakeX + fx;
        float ny = this->flShakeY + fy;
        if (iy < 0) iy = -iy;
        if (ix < 0) ix = -ix;

        float ox = 0.001f;
        if (ix > 3) ox = fx;
        float oy = 0.001f;
        if (iy > 3) oy = fy;
        this->needsRedraw = 1;
        this->flShakeAmpX = 0.002f;
        this->flShakeAmpY = 0.002f;
        this->freeCamDragging = 0;
        this->flShakePhaseX = ox;
        this->flShakePhaseY = oy;
        this->flShakeX = nx;
        this->flShakeY = ny;
        this->dragRotIntX = (int) nx;
        this->dragRotIntY = (int) ny;
    }
}


static int **g_kdJumpDst;

static int **g_kdVolText;

static int **g_kdAlienDst;

static int g_kdPostEffect;

void MGame::UseKhadorDrive() {
    if (this->player->isChargingDrive() != 0) return;

    Mission *m = Status::gStatus->getMission();
    bool special =
            ((Mission *) (m))->isEmpty() != 0 ||
            Status::gStatus->getCurrentCampaignMission() == 0x4e ||
            ((Mission *) (m))->getType() == 0xb ||
            ((Mission *) (m))->getType() == 0 ||
            ((Mission *) (m))->getType() == 0xbd ||
            ((Mission *) (m))->getType() == 0xd ||
            ((Mission *) (m))->getType() == 0xab ||
            ((Mission *) (m))->getType() == 0xac;

    if (!special) {
        if (Status::gStatus->getCurrentCampaignMission() == 0x41 && Status::gStatus->inAlienOrbit() == 0) {
            int idx = Station_getIndex(Status::gStatus->getStation());
            Status::gStatus->getCampaignMission();
            if (idx == ((Mission *) ((Mission *) Status::gStatus->getCampaignMission()))->getTargetStation())
                special = true;
        }
    }

    if (special) {
        PlayerEgo *player = (PlayerEgo *) (intptr_t)((Level *) (this))->getPlayer();
        return this->hud->hudEvent(0x15, player, 0);
    }

    this->player->resetGunDelay();
    if (Status::gStatus->getCurrentCampaignMission() == 0x4e) {
        **g_kdJumpDst = (int) (intptr_t) Status::gStatus->playerStation;
        this->usingJumpDrive = 1;
        ((MGame *) (this))->startChargingJumpDrive();
        this->pauseOpen = 0;
        ((MGame *) (this))->resumeSounds();
        this->hudMenuOpen = 0;
        this->hud->closeHudMenu();
        return Status::gStatus->nextCampaignMission(true);
    }

    if (Status::gStatus->inAlienOrbit() == 0) {
        if (this->player->hasVolatileGoods() != 0) {
            void *txt = ((GameText *) (**g_kdVolText))->getText(0);
            this->choiceWindow->set(*(String *) txt);
            this->pauseOpen = 1;
            this->choiceWindowOpen = 1;
            this->needsRedraw = 1;
            this->hudMenuOpen = 0;
            this->hud->closeHudMenu();
            ((MGame *) (this))->pauseSounds();
            this->maneuverActive = 0;
            return;
        }
        if (this->starMap == 0) {
            StarMap *sm = new StarMap(false, nullptr, false, -1);
            this->starMap = sm;
        }
        Engine *eng = (Engine *) this->applicationManager->GetEngine();
        ((Engine *) (eng))->SetPostEffect(g_kdPostEffect, 0);
        this->starMap->initLights();
        this->usingJumpDrive = 1;
        this->starMap->setJumpMapMode(1, 1);
        if (Status::gStatus->inAlienOrbit() == 0)
            this->starMap->askForJumpIntoAlienWorld();
        this->pauseOpen = 1;
        this->starMapOpen = 1;
        ((MGame *) (this))->pauseSounds();
        this->hudMenuOpen = 0;
        return this->hud->closeHudMenu();
    }

    if (Status::gStatus->getCurrentCampaignMission() == 0x50)
        Status::gStatus->field_84 = 100;
    int station = ((Galaxy *) (*(int *) Galaxy::gGalaxy))->getStation(
        Status::gStatus->getCurrentCampaignMission() /* index: arg lost in decomp */);
    **g_kdAlienDst = station;
    this->usingJumpDrive = 1;
    ((MGame *) (this))->startChargingJumpDrive();
    this->pauseOpen = 0;
    ((MGame *) (this))->resumeSounds();
    this->hudMenuOpen = 0;
    return this->hud->closeHudMenu();
}


static int g_initParticleFlag;

// g_initParticleFlag holds a pointer (as int) to a small particle-system global;
// byte at +0xf is a "particles enabled" flag. Modeled here for named access.
struct ParticleSystemGlobal {
    uint8_t _pad0[0xf];
    uint8_t particlesEnabled;  // offset 0xf
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(ParticleSystemGlobal, particlesEnabled) == 0xf,
              "ParticleSystemGlobal::particlesEnabled @ 0xf");
#endif

static int *g_initEngineSnd;

static int *g_initMusicArmed;

static int *g_initMusicTrack;

static int **g_initFmod;

static unsigned g_initPostEffect;

static int g_initStationMask;

static String **g_initInfoFont;

static int g_initInfoTextKey;

static int *g_initInfoWidth;

int MGame::OnInitialize() {
    MGame *self = this;
    self->missionInfoLines = 0;
    Level *level = self->level;
    self->loadProgress = 100;

    if (level == 0) {
        unsigned texSel;
        if (Status::gStatus->inAlienOrbit() == 0) {
            Status::gStatus->getSystem();
            int ti = ((SolarSystem *) (0))->getTextureIndex();
            texSel = (ti + 0x2efe) & 0xffff;
        } else {
            texSel = 0x2f08;
        }
        PaintCanvas::gCanvas->TextureCreate((unsigned short) (unsigned) (intptr_t) self->paintCanvas, 0, (void *) 0, texSel, false);
        PaintCanvas::gCanvas->ChangeCubeTexture((unsigned) (intptr_t) self->paintCanvas);

        {
            Globals::gGlobals->startNewSoundResourceList();
            static const int kCommon[] = {
                0x66, 0x68, 0x69, 0x6a, 0x6b, 0x67, 0x7e, 0x05, 0x18, 0x15,
                0x12, 0x13, 0x14, 0x1c, 0x1d, 0x1b, 0x25, 0x1a, 0x2e, 0x2f,
            };
            for (int id: kCommon)
                Globals::gGlobals->addSoundResourceToList(id);

            if (Status::gStatus->getWingmen() != 0)
                Globals::gGlobals->addSoundResourceToList(0x30);

            Globals::gGlobals->addSoundResourceToList(0x3e);
            Globals::gGlobals->addSoundResourceToList(0x3d);
            Globals::gGlobals->addSoundResourceToList(0x24);

            if (Status::gStatus->getCurrentCampaignMission() < 2) {
                Globals::gGlobals->addSoundResourceToList(0x9c);
                Globals::gGlobals->addSoundResourceToList(0x9d);
            }

            if (Status::gStatus->inAlienOrbit() == 0) {
                Status::gStatus->getSystem();
                if (((SolarSystem *) (0))->currentOrbitHasWarpGate())
                    Globals::gGlobals->addSoundResourceToList(0x1f);
            }

            int cm = Status::gStatus->getCurrentCampaignMission();
            if (cm == 0) {
                Globals::gGlobals->addSoundResourceToList(0x8f);
                Globals::gGlobals->addSoundResourceToList(0x9d);
                Globals::gGlobals->addSoundResourceToList(0x9e);
                Globals::gGlobals->addSoundResourceToList(0xa1);
                Globals::gGlobals->addSoundResourceToList(0xa0);
                Globals::gGlobals->addSoundResourceToList(0x9f);
            } else if (cm == 0xe) {
                Globals::gGlobals->addSoundResourceToList(0xf);
            } else if (cm == 0x18) {
                Globals::gGlobals->addSoundResourceToList(0x22);
            } else if (cm == 0x1d) {
                Globals::gGlobals->addSoundResourceToList(0xe);
            } else if (cm == 0x29) {
                Globals::gGlobals->addSoundResourceToList(0x9b);
                Globals::gGlobals->addSoundResourceToList(0x99);
                Globals::gGlobals->addSoundResourceToList(0x9a);
            }
        }

        Status::gStatus->checkForLevelUp();
        level = new Level(3);
        self->level = level;
    }

    if (((Level *) (level))->init() == 0) {
        self->loadProgress = 100;

        return 100;
    }

    ((MGame *) (self))->reset();

    {
        Player *pl = (Player *) self->player->player;

        if (Status::gStatus->field_64 >= 0) pl->setHitpoints(Status::gStatus->field_64);
        if (Status::gStatus->field_5c >= 0) pl->setShieldHP(Status::gStatus->field_5c);
        if (Status::gStatus->field_60 >= 0) pl->setArmorHP(Status::gStatus->field_60);
        if (Status::gStatus->field_68 >= 0) pl->setGammaHP(Status::gStatus->field_68);

        self->player->resetLastHP();

        if (Status::gStatus->getCurrentCampaignMission() != 0x5f) {
            Ship *ship = Status::gStatus->getShip();
            Status::gStatus->field_64 = ship->getMaxHP();
            Status::gStatus->field_5c = ship->getMaxShieldHP();
            Status::gStatus->field_60 = ship->getMaxArmorHP();
            Status::gStatus->field_68 = 100;

            int stIdx = ((Station *) (Status::gStatus->getStation()))->getIndex();
            int cm = Status::gStatus->getCurrentCampaignMission();
            if ((float) Status::gStatus->getGammaRayDamagePerSecond(stIdx, cm) == 0.0f)
                pl->setGammaHP(100);
        }
    }
    self->player->resetLastHP();

    if (Status::gStatus->getCurrentCampaignMission() != 0x5f) {
    }
    if (Status::gStatus->inAlienOrbit() == 0)
        Status::gStatus->field_84 = Station_getIndex(Status::gStatus->getStation());

    unsigned t = self->applicationManager->GetCurrentTimeMillis();
    self->field_0x1ac = 0;
    self->cloakAttributeMax = 0;
    self->startTime = t & 0xffff;
    self->startTimeHigh = 0;
    self->lastTime = t & 0xffff;
    self->lastTimeHigh = 0;

    if (static_cast<Radar *>(self->player->field_0x14)->hasScanner() != 0)
        Status::gStatus->field_11c = 0;
    Status::gStatus->field_12c = 0;
    Status::gStatus->field_134 = 0;
    Status::gStatus->field_13c = 0;
    Status::gStatus->field_144 = 0;

    Item *eq = ((Ship *) (((Status *) ((Status *) *((int *) &Status::gStatus)))->getShip()))->getFirstEquipmentOfSort(0x15);
    if (eq != 0) {
        self->cloakAttributeMax = ((Item *) (eq))->getAttribute(0x26);
        self->cloakAttribute = ((Item *) (eq))->getAttribute(0x26);
        self->hud->setTimeExtender(1, 0, 1, 0);
    }

    if (Status::gStatus->dlc1Won() != 0 && Status::gStatus->inAlienOrbit() != 0 &&
        Status::gStatus->getCurrentCampaignMission() < 0x93) {
        if (self->player != 0 && self->radio != 0)
            self->player->radioRef = self->radio;
        self->level->createRadioMessage(8, 0);
    }

    if (Status::gStatus->inBlackMarketSystem() == 0) {
        Status::gStatus->field_110 = 0;
    } else {
        if (self->player != 0 && self->radio != 0)
            self->player->radioRef = self->radio;
        if (Status::gStatus->field_110 == 0) {
            int id;
            Level *lvl;
            if (Status::gStatus->field_0x111 == 0) {
                Array<KIPlayer *> *enemies = self->level->getEnemies();
                if (enemies != nullptr) {
                    int n = (int) enemies->size();
                    for (int i = 0; i < n; i++) {
                        KIPlayer *e = (*enemies)[i];
                        if (e->shipGroup == 8)
                            e->field_0x25 = 0;
                    }
                }
                lvl = self->level;
                id = 9;
            } else {
                lvl = self->level;
                id = 0xd;
            }
            ((Level *) (lvl))->createRadioMessage(id, 0);
        } else {
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr) {
                int n = (int) enemies->size();
                for (int i = 0; i < n; i++) {
                    KIPlayer *e = (*enemies)[i];
                    if (e->shipGroup == 8)
                        e->field_0x25 = 0;
                }
            }
        }
    }

    {
        if (Status::gStatus->inAlienOrbit() == 0 && Status::gStatus->getCurrentCampaignMission() == 0x7d) {
            int stIdx = ((Station *) (Status::gStatus->getStation()))->getIndex();
            if (Status::gStatus->isFreighterMissionStation(stIdx) != 0) {
                Mission *m = Status::gStatus->getMission();
                int statusVal = ((Mission *) (m))->getStatusValue();
                int bit = Status::gStatus->getFreighterMissionStationBit(
                    ((Station *) (Status::gStatus->getStation()))->getIndex());
                if ((statusVal & (1 << (bit & 0xff))) == 0) {
                    int bit2 = Status::gStatus->getFreighterMissionStationBit(
                        ((Station *) (Status::gStatus->getStation()))->getIndex());
                    ((Mission *) (m))->setStatusValue(statusVal | (1 << (bit2 & 0xff)));
                    if (self->player != 0 && self->radio != 0)
                        self->player->radioRef = self->radio;
                    self->level->createRadioMessage(0x13, 0);
                }
            }
        }

        Array<Item *> *secondary = Status::gStatus->getShip()->getEquipment(1);
        if (secondary != 0) {
            int savedId = Status::gStatus->field_f4;
            if (Status::gStatus->getShip()->hasEquipment(savedId, 1) == 0) {
                Item *first = secondary->empty() ? 0 : (*secondary)[0];
                if (first != 0) {
                    self->player->setCurrentSecondaryWeaponIndex(((Item *) (first))->getIndex());
                    self->hud->setCurrentSecondaryWeapon(first);
                }
            } else {
                for (unsigned i = 0; i < secondary->size(); i++) {
                    Item *it = (*secondary)[i];
                    if (it != 0 && ((Item *) (it))->getIndex() == savedId) {
                        ((Level *) ((Level *) self->level))->getPlayer();
                        self->player->setCurrentSecondaryWeaponIndex(((Item *) (it))->getIndex());
                        self->hud->setCurrentSecondaryWeapon(it);
                        break;
                    }
                }
            }
        }

        self->field_0xc8 = 0;
        bool renderParticles =
            ((ParticleSystemGlobal *) (intptr_t) g_initParticleFlag)->particlesEnabled != 0;
        if (!renderParticles) {
            if (Status::gStatus->getCurrentCampaignMission() > 1) {
                Vec3 p = self->player->getPosition();
                (void) p;
                ((FModSound *) (*g_fmod))->play(*g_initEngineSnd,
                                                (Vector *) &self->player->field_0x1c,
                                                (Vector *) &p, 0.0f);
            }
        } else {
            self->player->PlayEngineSound();
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr)
                for (unsigned i = 0; i < enemies->size(); i++)
                    ((KIPlayer *) ((*enemies)[i]))->PlayEngineSound();
        }

        self->loadingTime = 0;

        self->level->getStarSystem();
        ((StarSystem *) (0))->initLight();
        self->level->enableParticleEffects(true, renderParticles);

        Status::gStatus->getShip();
        float fireRate = (float) Status::gStatus->getShip()->getFireRateFactor();
        if (1.0f - fireRate >= 0.0f)
            self->player->pitchAllPrimaryGuns(1.0f - fireRate);

        if (Status::gStatus->inAlienOrbit() == 0) {
            int idx = ((Station *) (Status::gStatus->getStation()))->getIndex();
            bool visit;
            unsigned off = (unsigned) (idx - 0x6d);
            if (off < 0x1a) {
                if ((g_initStationMask & (1 << (off & 0xff))) == 0)
                    visit = (off != 2) || (Status::gStatus->getCurrentCampaignMission() < 0x5e);
                else
                    visit = true;
            } else {
                visit = ((unsigned) (idx - 0x66) <= 2);
            }
            if (visit)
                ((Station *) (Status::gStatus->getStation()))->visit();
        }

        if (*g_initMusicArmed != -1)
            Globals::gGlobals->playMusicAndFadeOutCurrent(*g_initMusicTrack);
        *g_initMusicArmed = -1;
        ((FModSound *) (*g_initFmod))->setDownPitch(false);

        Engine *eng = (Engine *) self->applicationManager->GetEngine();
        ((Engine *) (eng))->SetPostEffect(g_initPostEffect, false);

        if (Status::gStatus->getCurrentCampaignMission() == 0x9e) {
            Array<KIPlayer *> *enemies = self->level->getEnemies();
            if (enemies != nullptr) {
                KIPlayer *first = (*enemies)[0];
                ((PlayerFighter *) (first))->cloak(true, false);
            }
        }

        if (self->menuWindow == 0)
            self->menuWindow = new MenuTouchWindow(1);

        self->missionInfoLines = new Array<AbyssEngine::String *>();
        String *font = *g_initInfoFont;
        String *text = (String *) ((GameText *) (*g_gameText))->getText(g_initInfoTextKey);
        Globals::gGlobals->getLineArray(static_cast<unsigned int>(reinterpret_cast<std::size_t>(font)),
                               *text, *g_initInfoWidth, self->missionInfoLines);
        self->active = 1;
    }
    return 0;
}

void TFC_zoomTarget(void *cam, float z);


static float g_fcRotScale;

static float g_fcZoomMax;

static float g_fcZoomMin;

void MGame::freeCamTouchMove(int x, int y, void *touchId) {
    int ty = (int) (intptr_t) touchId;
    if (this->player->isMining() != 0) {
        this->needsRedraw = 1;
        return;
    }
    this->needsRedraw = 0;

    int t0 = this->touch0Id;
    int t1 = this->touch1Id;
    if (t0 == 0 || t1 == 0) {
        int dy = y - this->dragLastX;
        int dx = ty - this->dragLastY;
        this->dragLastX = x;
        this->dragLastY = ty;
        this->dragDeltaX = dy;
        this->dragDeltaY = dx;
        this->flShakeAmpX = 1.0f;
        this->flShakeAmpY = 1.0f;
        this->flShakeX += (float) dy;
        this->flShakeY += (float) dx;

        if (t0 == 0) {
            float v[4];
            v[0] = (float) x;
            v[1] = (float) ty;
            v[2] = 0;
            this->freeCamFinger0 = *(const Vector *) ((Vector *) v);
        } else if (t1 == 0) {
            float v[4];
            v[0] = (float) x;
            v[1] = (float) ty;
            v[2] = 0;
            this->freeCamFinger1 = *(const Vector *) ((Vector *) v);
        }

        return;
    }

    Vector *base = &this->freeCamFinger1;
    float tmp[4];
    *(Vector *) ((Vector *) tmp) = *(const Vector *) (base) - *(const Vector *) (&this->freeCamFinger0);
    float oldLen = AbyssEngine::AEMath::VectorLength(*(const Vector *) ((Vector *) tmp));
    float newLen = oldLen;

    if (this->touch0Id == ty) {
        float v[4];
        v[0] = (float) x;
        v[1] = (float) ty;
        v[2] = 0;
        *(Vector *) ((Vector *) tmp) = *(const Vector *) (base) - *(const Vector *) ((Vector *) v);
        newLen = AbyssEngine::AEMath::VectorLength(*(const Vector *) ((Vector *) tmp));
        *(Vector *) (base) = *(const Vector *) ((Vector *) v);
    } else if (this->touch1Id == ty) {
        float v[4];
        v[0] = (float) x;
        v[1] = (float) ty;
        v[2] = 0;
        *(Vector *) ((Vector *) tmp) = *(const Vector *) (base) - *(const Vector *) ((Vector *) v);
        newLen = AbyssEngine::AEMath::VectorLength(*(const Vector *) ((Vector *) tmp));
        this->freeCamFinger0 = *(const Vector *) ((Vector *) v);
    }

    float zoom = this->flCameraRoll + (newLen - oldLen) * g_fcRotScale;
    this->flCameraRoll = zoom;
    if (zoom > g_fcZoomMax || zoom < g_fcZoomMin) {
        zoom = (zoom > g_fcZoomMax) ? g_fcZoomMax : g_fcZoomMin;
        this->flCameraRoll = zoom;
    }
    TFC_zoomTarget(this->camera, zoom);
}

namespace {

enum MGameHudAction : unsigned int {
    kHudActionPause = 0x00000001,
    kHudActionQuickMenu = 0x00000004,
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
    kHudActionOrbit = 0x00000040,
};

static void mgame_close_hud_menu(MGame *self, bool closeOrbitMenu) {
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

static void mgame_open_hud_menu(MGame *self, int menuType) {
    self->pauseOpen = 1;
    self->hudMenuOpen = 1;
    self->pauseSounds();
    self->hud->initHudMenu(menuType, self->level);
}

static uint8_t &mgame_pause_music_disabled_flag(MGame *self) {
    return reinterpret_cast<uint8_t *>(&self->field_0x1a4)[0];
}

static void mgame_open_pause_menu(MGame *self) {
    self->pauseSounds();
    if (self->pauseOpen != 0) {
        self->hud->releaseAllKeys();
        return;
    }

    if (self->menuWindow == nullptr)
        self->menuWindow = new MenuTouchWindow(1);

    const bool canSkip = self->levelScript != nullptr &&
                         self->levelScript->canSkipCutsceneNow() != 0;
    self->menuWindow->setSkipButtonVisible(canSkip);
    self->pauseOpen = 1;
    self->pauseSounds();

    FModSound *sound = Globals::sound;
    self->pauseSnapshot = self->pauseOpen;
    mgame_pause_music_disabled_flag(self) =
        sound != nullptr ? static_cast<uint8_t>(sound->IsCategoryEnabled(2) ^ 1) : 0;
    if (sound != nullptr)
        sound->pauseAllPlaying();
    self->player->PauseEngineSound();

    Array<KIPlayer *> *enemies = self->level->getEnemies();
    if (enemies != nullptr) {
        for (unsigned int i = 0; i < enemies->size(); ++i) {
            KIPlayer *enemy = (*enemies)[i];
            if (enemy != nullptr)
                enemy->PauseEngineSound();
        }
    }

    const bool cutsceneMode = self->jumpActive != 0 || self->player->isDead();
    self->menuWindow->setCutsceneMode(cutsceneMode);
    self->menuTouchOpen = 1;
    if (sound != nullptr)
        sound->play(0x7b, nullptr, nullptr, 0.0f);
    self->hud->releaseAllKeys();
}

static void mgame_handle_autopilot_menu_touch_end(MGame *self, int x, int y) {
    ChoiceWindow *choiceWindow = self->choiceWindow;
    if (choiceWindow == nullptr)
        return;

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

    Vector position = jumpGate->geometry->getPosition();
    position.z += 8000.0f;
    self->player->setPosition(position);
}

static void mgame_handle_star_map_touch_end(MGame *self, int x, int y) {
    Layout *layout = static_cast<Layout *>(Globals::layout);
    if (layout != nullptr && layout->layoutVisibleFlag != 0) {
        if (layout->OnTouchEnd(x, y) != 0)
            layout->layoutVisibleFlag = 0;
        return;
    }

    StarMap *starMap = self->starMap;
    if (starMap == nullptr || starMap->OnTouchEnd(x, y) == 0)
        return;

    StarSystem *starSystem = self->level->getStarSystem();
    if (starSystem != nullptr)
        starSystem->initLight();
    self->pauseOpen = 0;
    self->starMapOpen = 0;
    self->resumeSounds();

    if (starMap->exitRequested == 0) {
        if (self->touchesStream != 0)
            mgame_restore_stream_position(self);
        else if (Level::doInstantJump == 0)
            self->levelScript->setAutoPilotToProgrammedStation();
    } else if (self->touchesStream != 0) {
        self->usingJumpDrive = 0;
        self->startJumpScene();
    }

    delete self->starMap;
    self->starMap = nullptr;
}

static void mgame_handle_basic_choice_window_touch_end(MGame *self, int x, int y) {
    if (self->choiceWindow == nullptr)
        return;

    // This is the ordinary MGame+0xce path. The neighboring flags select
    // campaign/jump-specific ChoiceWindow handlers and must not be collapsed
    // into the basic dismissal case.
    if (self->field_0xcf == 0 && self->_bca == 0 && self->field_0x1e4 == 0 &&
        self->choiceWindow->OnTouchEnd(x, y) == 0) {
        self->pauseOpen = 0;
        self->choiceWindowOpen = 0;
        self->resumeSounds();
    }
}

static void mgame_handle_menu_touch_end(MGame *self, int x, int y, void *touchId) {
    MenuTouchWindow *menuWindow = self->menuWindow;
    if (menuWindow == nullptr)
        return;

    if (self->freeCamMode != 0) {
        MGameAppData *applicationData =
            reinterpret_cast<MGameAppData *>(static_cast<intptr_t>(ApplicationManager_GetApplicationData()));
        if (applicationData == nullptr || applicationData->modalActive != 0 ||
            applicationData->transitionActive != 0)
            return;
        if (menuWindow->isShowingMessage() == 0 && !menuWindow->isMakingScreenshot())
            self->freeCamTouchEnd(x, y, touchId);
    }

    if (menuWindow->OnTouchEnd(x, y, touchId) != 0) {
        self->pauseSnapshot = 0;
        self->pauseOpen = 0;
        self->resumeSounds();
        self->menuTouchOpen = 0;
        self->touch0Id = 0;
        self->touch1Id = 0;
        self->activeTouchId = nullptr;

        StarSystem *starSystem = self->level->getStarSystem();
        if (starSystem != nullptr)
            starSystem->initLight();
    }

    if (self->freeCamMode == 0 && menuWindow->inCinematicMode()) {
        self->setCinematicMode(true);
        self->hudTouchFlags = 0;
    } else if (self->freeCamMode != 0 && !menuWindow->inCinematicMode()) {
        self->setCinematicMode(false);
        self->hudTouchFlags = 0;
    }
}

static void mgame_dispatch_orbit_menu(MGame *self, unsigned int actions) {
    PlayerEgo *player = self->player;
    Level *level = self->level;
    Hud *hud = self->hud;

    if ((actions & kHudActionProgrammedStation) != 0)
        self->levelScript->setAutoPilotToProgrammedStation();

    Array<KIPlayer *> *landmarks = level->getLandmarks();
    if ((actions & kHudActionJumpGate) != 0 && landmarks != nullptr && landmarks->size() > 1) {
        player->setAutoPilot((*landmarks)[1]);
        hud->hudEvent(12, player, 0);
    }
    if ((actions & kHudActionStation) != 0 && landmarks != nullptr && landmarks->size() > 0) {
        player->setAutoPilot((*landmarks)[0]);
        hud->hudEvent(10, player, 0);
    }
    if ((actions & kHudActionAsteroidWaypoint) != 0) {
        player->setAutoPilot(level->getAsteroidWaypoint());
        hud->hudEvent(14, player, 0);
    }
    if ((actions & kHudActionRouteWaypoint) != 0) {
        Route *route = level->getPlayerRoute();
        if (route != nullptr) {
            player->setAutoPilot(route->getWaypoint());
            hud->hudEvent(13, player, 0);
        }
    }

    // The action-mask layout reserves bits 26..31 for docking targets.
    const int dockingTargetCount = level->getNumDockingTargets();
    for (int i = 0; i < dockingTargetCount && i < 6; ++i) {
        const unsigned int action = kHudActionDockingTarget0 << i;
        KIPlayer *target = reinterpret_cast<KIPlayer *>(
                static_cast<intptr_t>(level->getDockingTarget(i)));
        if ((actions & action) == 0 || target == nullptr)
            continue;

        // Android MGame::OnTouchEnd writes this target to Radar+0x04 before
        // starting docking, then clears the three adjacent transient slots.
        self->radar->dockTargetPtr = target;
        hud->hudEvent(34, player, 0);
        player->dockToDockingPoint(target, self->radar);
        self->radar->dockNavPtr = nullptr;
        self->radar->lockedAsteroid = nullptr;
        self->radar->candidateAsteroid = nullptr;
        hud->releaseAllKeys();
    }

    mgame_close_hud_menu(self, true);
}

static bool mgame_try_open_orbit_menu(MGame *self) {
    PlayerEgo *player = self->player;
    Status *status = Status::gStatus;

    if (player == nullptr || status == nullptr)
        return false;

    if (status->inAlienOrbit() && player->isDockingToAsteroid())
        player->dockToAsteroid(nullptr, self->radar);

    const int missionId = status->getCurrentCampaignMission();
    if ((status->inAlienOrbit() &&
         (missionId != 154 || self->level->getNumDockingTargets() < 1)) ||
        missionId < 2 || missionId == 48 || player->isDockedToDockingPoint() ||
        player->isLandingOrTakingOff()) {
        return false;
    }

    if (self->orbitMenuOpen != 0) {
        mgame_close_hud_menu(self, true);
        return true;
    }

    if (player->isMining())
        return false;

    if (player->isAutoPilot()) {
        player->setAutoPilot(nullptr);
        self->hud->hudEvent(6, player, 0);
        return true;
    }

    if (player->isDockingToAsteroid()) {
        player->dockToAsteroid(self->radar->getLockedAsteroid(), self->radar);
        self->hud->hudEvent(6, player, 0);
        return true;
    }

    if (player->isDockingToDockingPoint()) {
        self->radar->dockTargetPtr = nullptr;
        player->dockToDockingPoint(nullptr, self->radar);
        self->hud->hudEvent(6, player, 0);
        return true;
    }

    if (player->isDockingToStream()) {
        // The ARM body routes this through Radar+0x24 before the common
        // docking HUD event. The local Radar slot is source-labelled
        // `lockedStation` but its exact stream-target semantics still need
        // a dedicated Radar/PlayerEgo audit.
        player->dockToAsteroid(self->radar->lockedStation, self->radar);
        self->hud->hudEvent(6, player, 0);
        return true;
    }

    Mission *mission = status->getMission();
    if (!status->inAlienOrbit() || missionId == 154) {
        if (mission == nullptr || mission->getType() != 183) {
            mgame_open_hud_menu(self, 3);
            self->orbitMenuOpen = 1;
            return true;
        }
    }
    return false;
}

static void mgame_dispatch_hud_menu(MGame *self, unsigned int actions) {
    if (self->hudMenuOpen == 0)
        return;

    if ((actions & kHudActionOpenWeaponMenu) != 0) {
        self->hud->initHudMenu(1, self->level);
        return;
    }
    if ((actions & kHudActionOpenWingmanMenu) != 0) {
        self->hud->initHudMenu(2, self->level);
        return;
    }
    if ((actions & kHudActionCloak) != 0) {
        mgame_close_hud_menu(self, false);
        self->useCloak();
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
                self->player->setCurrentSecondaryWeaponIndex(item->getIndex());
                self->hud->setCurrentSecondaryWeapon(item);
            }
        }
        mgame_close_hud_menu(self, false);
        return;
    }

    int wingmanCommand = -1;
    if ((actions & kHudActionWingmenAttack) != 0) wingmanCommand = 1;
    else if ((actions & kHudActionWingmenDefend) != 0) wingmanCommand = 3;
    else if ((actions & kHudActionWingmenFollow) != 0) wingmanCommand = 2;
    else if ((actions & kHudActionWingmenToggle) != 0) {
        wingmanCommand = 0;
        Status::gStatus->field_f8 ^= 1;
    }
    if (wingmanCommand >= 0) {
        Array<KIPlayer *> *enemies = self->level->getEnemies();
        if (enemies != nullptr) {
            KIPlayer *target = wingmanCommand == 3 ? self->radar->getLockedEnemy() : nullptr;
            for (unsigned int i = 0; i < enemies->size(); ++i) {
                KIPlayer *wingman = (*enemies)[i];
                if (wingman != nullptr && wingman->isWingMan() && !wingman->isDead())
                    wingman->setWingmanCommand(wingmanCommand, target);
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
    if (actions == 0) {
        self->touch0Id = 0;
        self->touch1Id = 0;
        mgame_close_hud_menu(self, true);
    }
}

} // namespace

void MGame::OnTouchEnd(int p1, int p2, void *touchId) {
    if (this->activeTouchId == touchId) {
        this->activeTouchId = 0;
        this->field_0xc1 = 0;
        this->field_0xc2 = 0;
        this->field_0xc3 = 0;
    }

    if (this->player == nullptr || this->hud == nullptr)
        return;

    this->flFastForwardWeight = 1.0f;
    TFC_setFastForwardMode(this->camera, 0);
    this->player->resumeFlag = 1;

    // The Android pre-Hud branch handles these paused states before LABEL_69.
    // Dock-choice windows intentionally fall through to the HUD action path.
    if (this->pauseOpen != 0 && this->hudMenuOpen == 0 && this->orbitMenuOpen == 0) {
        if (this->choiceWindowOpen != 0) {
            mgame_handle_basic_choice_window_touch_end(this, p1, p2);
            return;
        }
        if (this->autopilotMenuOpen != 0) {
            mgame_handle_autopilot_menu_touch_end(this, p1, p2);
            return;
        }
        if (this->dockChoiceOpen == 0) {
            if (this->starMapOpen != 0) {
                mgame_handle_star_map_touch_end(this, p1, p2);
                return;
            }
            if (this->cutsceneActive != 0)
                return;
            if (this->menuTouchOpen != 0) {
                mgame_handle_menu_touch_end(this, p1, p2, touchId);
                return;
            }
            return;
        }
    }

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

    if ((actions & kHudActionOrbit) == 0 && this->orbitMenuOpen != 0) {
        mgame_dispatch_orbit_menu(this, actions);
        return;
    }

    if ((actions & kHudActionOrbit) != 0 && mgame_try_open_orbit_menu(this))
        return;

    if ((actions & kHudActionQuickMenu) != 0 && !this->player->isMining()) {
        if (this->hudMenuOpen == 0)
            mgame_open_hud_menu(this, 0);
        else
            mgame_close_hud_menu(this, true);
        return;
    }

    mgame_dispatch_hud_menu(this, actions);
}


static uint8_t **g_scFlag;

void Status_replaceHash(String * out, Status * self, String * haystack,
                                   String * needle, String * repl);

static int g_scFollowTextKey;

static int g_scFollowHashLit;

void MGame::successCheck() {
    bool timed = !(this->elapsedTimeHigh < (int) (this->elapsedTime < 0x1389));
    if (timed || this->jumpDriveActive != 0) {
        if (((Mission *) ((Mission *) Status::gStatus->getCampaignMission()))->getType() != 0xaa) goto done;
    }

    {
        int *status = ((int *) &Status::gStatus);
        Mission *mc = ((Status *) (*status))->missionCompleted(0, 0, 0);
        int obj = this->level->checkObjective(0);
        if (mc == 0 && obj == 0) goto done;
    }

    if (((Mission *) (Status::gStatus->getMission()))->getType() == 5) goto deliverFollowup;
    if (((Mission *) (Status::gStatus->getMission()))->getType() == 3) goto deliverFollowup;

    {
        int *status = ((int *) &Status::gStatus);
        if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() == 0)
            ((Status *) (status))->incMissionCount();

        if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0) {
            bool hasSuccess = false;
            if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0) {
                int cm = Status::gStatus->getCurrentCampaignMission();
                if (DialogueWindow::hasSuccessDialogue(cm) != 0)
                    hasSuccess = true;
            }
            if (!hasSuccess) {
                int cm = Status::gStatus->getCurrentCampaignMission();
                if (cm > 0x2d && ((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0) {
                    int cm2 = Status::gStatus->getCurrentCampaignMission();
                    if (DialogueWindow::hasSuccessDialogue(cm2) == 0) {
                        ((Status *) (*status))->nextCampaignMission(true);
                        this->level->removeObjectives();
                        ((Status *) (status))->setMission(
                            (Mission *) Status::gStatus->getCampaignMission() /* mission: arg lost in decomp */);
                    }
                }
                goto done;
            }

            if (this->dialogueWindow == 0) {
                DialogueWindow *w = (DialogueWindow *) ::operator new(0x74);
                DialogueWindow_ctor(w);
                Level *lvl = this->level;
                this->dialogueWindow = w;
                if (lvl != 0) w->setLevel(lvl);
            } else if (this->dialogueWindow->hasLevel() == 0) {
                Level *lvl = this->level;
                if (lvl != 0) this->dialogueWindow->setLevel(lvl);
            }
            this->dialogueWindow->set(Status::gStatus->getMission(), 1, -1);
            this->pauseOpen = 0x101;
            ((MGame *) (this))->pauseSounds();

            int cm = Status::gStatus->getCurrentCampaignMission();
            if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0 && cm == 0x26) {
                Array<KIPlayer *> *enemies = this->level->getEnemies();
                unsigned n = enemies->size();
                for (unsigned i = 0; i < n; i++) {
                    PlayerFixedObject *e = (PlayerFixedObject *) (*enemies)[i];
                    if (e->deltaTime != 0) {
                        ((Player *) (e->player))->setHitpoints((int) (intptr_t) e->player);
                        ((PlayerFixedObject *) (e))->setMoving(1);
                    }
                }
            } else if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0 && cm == 0x38) {
                StarSystem *ss = (StarSystem *) (intptr_t) this->level->getStarSystem();
                ((StarSystem *) (ss))->getPlanets();
                int pts[3] = {0, 0, 0};
                Route *route = new Route(pts, 3);
                Array<KIPlayer *> *enemies = this->level->getEnemies();
                unsigned n = enemies->size();
                for (unsigned i = 0; i < n; i++) {
                    KIPlayer *k = (*enemies)[i];
                    if (k->shipGroup == 1) {
                        Route *rc = (Route *) ((Route *) (route))->clone();
                        ((KIPlayer *) (k))->setRoute(rc);
                    }
                }
                delete route;
            } else if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0 && cm == 0x3f) {
                Array<KIPlayer *> *enemies = this->level->getEnemies();
                unsigned n = enemies->size();
                for (unsigned i = 0; i < n; i++) {
                    KIPlayer *e = (*enemies)[i];
                    if (e->shipGroup == 8)
                        ((Player *) ((Player *) e->player))->removeAllGuns();
                }
            } else if (((Mission *) (Status::gStatus->getMission()))->isCampaignMission() != 0 && cm == 0x49) {
                Array<KIPlayer *> *enemies = this->level->getEnemies();
                unsigned n = enemies->size();
                for (unsigned i = 0; i < n; i++) {
                    PlayerFixedObject *o = (PlayerFixedObject *) (*enemies)[i];
                    if (o->deltaTime != 0) {
                        ((Player *) (o->player))->setHitpoints((int) (intptr_t) o->player);
                        ((PlayerFixedObject *) (o))->setMoving(1);
                    }
                }
            } else {
                if (Station_getIndex(Status::gStatus->getStation()) == 0x70 &&
                    Status::gStatus->getCurrentCampaignMission() == 0x8f)
                    **g_scFlag = 1;
            }
            goto done;
        }

        if (this->dialogueWindow == 0) {
            DialogueWindow *w = (DialogueWindow *) ::operator new(0x74);
            DialogueWindow_ctor(w);
            Level *lvl = this->level;
            this->dialogueWindow = w;
            if (lvl != 0) w->setLevel(lvl);
        } else if (this->dialogueWindow->hasLevel() == 0) {
            Level *lvl = this->level;
            if (lvl != 0) this->dialogueWindow->setLevel(lvl);
        }

        intptr_t m = Status::gStatus->getCurrentCampaignMission() == 0
                         ? (intptr_t) Status::gStatus->getMission()
                         : (intptr_t) Status::gStatus->getCurrentCampaignMission();
        this->dialogueWindow->set((Mission *) m, 1, -1);
        this->pauseOpen = 0x101;
        ((MGame *) (this))->pauseSounds();
        goto done;
    }

deliverFollowup:

    if (this->dialogueWindow == 0) {
        DialogueWindow *w = (DialogueWindow *) ::operator new(0x74);
        DialogueWindow_ctor(w);
        Level *lvl = this->level;
        this->dialogueWindow = w;
        if (lvl != 0) w->setLevel(lvl);
    } else if (this->dialogueWindow->hasLevel() == 0) {
        Level *lvl = this->level;
        if (lvl != 0) this->dialogueWindow->setLevel(lvl);
    }
    {
        Status *status = Status::gStatus;

        Mission *cm = (Mission *) (intptr_t) status->getCampaignMission();
        Agent *agent = ((Mission *) ((Mission *) (intptr_t) status->getCampaignMission()))->getAgent();
        ((Mission *) (cm))->setTargetStation(((Agent *) (agent))->getStation());

        this->dialogueWindow->set((Mission *) (intptr_t) status->getCampaignMission(), 1, -1);
        ((Mission *) ((Mission *) (intptr_t) status->getCampaignMission()))->setType(0xb);

        this->player->setTurretMode(0);
        this->levelScript->resetCamera((Level *) this->level);
        this->player->setFreeLookMode(false);
        TFC_enableFirstPersonCam(this->camera, 0);
        this->player->hideShipForFirstPersonCameraView(false);

        this->needsRedraw = 1;

        ((Mission *) ((Mission *) (intptr_t) status->getCampaignMission()))->setStatusValue(0);
        ((Mission *) ((Mission *) (intptr_t) status->getCampaignMission()))->setWon(false);

        void *tmpl = ((GameText *) (*g_gameText))->getText(g_scFollowTextKey);
        String sTmpl, sHash, sStation, sResult;
        sTmpl = *(String *) tmpl;
        sHash.ctor_char((const char *) (intptr_t) g_scFollowHashLit, false);
        String station = (status->getMission())->getTargetStationName();
        sStation = *(String *) &station;
        Status_replaceHash(&sResult, status, &sTmpl, &sStation, &sHash);

        Agent *missionAgent = ((Mission *) (status->getMission()))->getAgent();
        missionAgent->setMissionString(sResult);

        status->setMission((Mission *) (intptr_t) status->getCampaignMission());

        this->player->setRoute(0);
        if (this->player->goingToWaypoint() != 0)
            this->player->setAutoPilot(0);
        this->player->removeRoute();
        this->level->setPlayerRoute(0);

        int *level = (int *) this->level;
        if (level[10] != 0) {
            delete (Objective *) (intptr_t) level[10];
            level = (int *) this->level;
        }
        level[10] = 0;
        if (level[11] != 0) {
            delete (Objective *) (intptr_t) level[11];
            level = (int *) this->level;
        }
        level[11] = 0;

        this->pauseOpen = 1;
        this->cutsceneActive = 1;
        ((MGame *) (this))->pauseSounds();
    }

done:
    ;
}

MGame::~MGame() {
    MGame *self = this;
    ((MGame *) (self))->OnRelease();
    { if (this->gameOverTitle.data) delete[] this->gameOverTitle.data; this->gameOverTitle.data = nullptr; this->gameOverTitle.length = 0; }
}



void MGame::resumeSounds() {
    ((FModSound *) (*g_fmod))->resumeAll();
    this->player->ResumeEngineSound();
    Array<KIPlayer *> *e = this->level->getEnemies();
    if (e == nullptr) return;
    for (uint32_t i = 0; i < e->size(); i++)
        ((KIPlayer *) ((*e)[i]))->ResumeEngineSound();
}


static int *g_jumpFlag;

static int **g_alienAmt;

static int **g_jumpCost;

static int **g_alienCost;

void MGame::startChargingJumpDrive() {
    if (this->usingJumpDrive == 0) return;
    int needed = 1;
    if (((Ship *) (Status::gStatus->getShip()))->hasCargo(0x7a, 1) == 0) {
        ChoiceWindow *w = this->choiceWindow;
        if (w == 0) {
            w = new ChoiceWindow();
            this->choiceWindow = w;
        }
        void *txt = ((GameText *) (*(int *) Status::gStatus))->getText(0x243);
        ((ChoiceWindow *) (w))->set(*(String *) txt);
        this->pauseOpen = 1;
        this->choiceWindowOpen = 1;
        ((MGame *) (this))->pauseSounds();
        **g_jumpCost = 0;
        return;
    }
    int hc = Status::gStatus->hardCoreMode();
    int *jf = g_jumpFlag;
    if (hc != 0) needed = 2;
    int cost;
    if (*jf == (int) (intptr_t) Status::gStatus->playerStation) {
        cost = needed << 1;
    } else {
        cost = **g_alienAmt;
        if (Status::gStatus->inAlienOrbit() != 0) cost = needed;
    }
    ((Ship *) (Status::gStatus->getShip()))->getCargo(0x7a);
    if (((Item *) (0))->getAmount() < cost) {
        ChoiceWindow *w = this->choiceWindow;
        if (w == 0) {
            w = new ChoiceWindow();
            this->choiceWindow = w;
        }
        int hc2 = Status::gStatus->hardCoreMode();
        void *txt = ((GameText *) (*(int *) Status::gStatus))->getText(hc2 != 0 ? 0x243 : 0x244);
        ((ChoiceWindow *) (w))->set(*(String *) txt);
        this->pauseOpen = 1;
        this->choiceWindowOpen = 1;
        ((MGame *) (this))->pauseSounds();
        *jf = 0;
        return;
    }
    this->player->startJumpDrive();
    if (*jf != (int) (intptr_t) Status::gStatus->playerStation) {
        if (Status::gStatus->inAlienOrbit() == 0) needed = **g_alienCost;
    }
    this->hud->hudEvent(0x1e, this->player, needed);
    ((Ship *) (Status::gStatus->getShip()))->removeCargo(0x7a, needed);
}



void MGame::pauseSounds() {
    this->pauseSnapshot = this->pauseOpen;
    ((FModSound *) (*g_fmod))->pauseAllPlayingSoundFXEvents();
    this->player->PauseEngineSound();
    Array<KIPlayer *> *e = this->level->getEnemies();
    if (e != nullptr) {
        for (uint32_t i = 0; i < e->size(); i++)
            ((KIPlayer *) ((*e)[i]))->PauseEngineSound();
    }
}

void Radio_ctor(Radio *r);

void *TargetFollowCamera_dtor(void *c);

void TargetFollowCamera_ctor(TargetFollowCamera *c, int cam, int target,
                                        int a, int b, int d, int e, int f, int g);

static int g_resAspectA;

static int g_resAspectB;

static int g_resAspectC;

// Native Android IDA: MGame camera perspective uses fixed FOV/near and
// switches only the far plane for early alien orbit campaign missions.
static const float g_MGame_camFovDefault = 1.22f;
static const float g_MGame_camNear = 20.0f;
static const float g_MGame_camFarNormal = 300000.0f;
static const float g_MGame_camFarAlienEarlyCampaign = 450000.0f; // mission < 0x50
static const float g_MGame_camFarAlienLateCampaign = 300000.0f;
static const float g_MGame_boostFovScale = 0.35f;

static float MGame_cameraFarPlane() {
    if (Status::gStatus->inAlienOrbit() == 0) {
        return g_MGame_camFarNormal;
    }
    int cm = Status::gStatus->getCurrentCampaignMission();
    return (cm < 0x50) ? g_MGame_camFarAlienEarlyCampaign : g_MGame_camFarAlienLateCampaign;
}

static int g_resInitB;

static int **g_resShipTune;

static uint8_t **g_resPauseSrc;

void MGame::reset() {
    this->flCameraRoll = 0;
    this->activeTouchId = 0;
    this->touch0Id = 0;
    this->touch1Id = 0;
    this->menuTime = 0;
    this->dragStartX = 0;
    this->dragStartY = 0;
    this->dragDeltaY = 0;
    this->freeCamDragging = 0;

    *(int *) &this->flShakeX = g_resAspectC;
    *(int *) &this->flShakeY = g_resAspectA;
    this->field_0x120 = g_resAspectB;
    this->dragLastX = 0;
    this->dragLastY = 0;
    this->dragRotIntX = 0;
    this->dragRotIntY = 0;
    this->dragDeltaX = 0;

    this->player = (PlayerEgo *) (intptr_t)((Level *) (this))->getPlayer();

    this->hud = new Hud();

    Radio *radio = (Radio *) ::operator new(0x48);
    Radio_ctor(radio);
    this->radio = radio;
    ((Radio *) (radio))->setMessages((Array<RadioMessage *> *) this->level->getMessages());

    PaintCanvas *pc = PaintCanvas::gCanvas;
    pc->CameraCreate(this->cameraId);
    unsigned cam = this->cameraId;
    pc->CameraSetPerspective(cam, g_MGame_camFovDefault, g_MGame_camNear, MGame_cameraFarPlane());

    if (this->camera != 0) {
        ::operator delete(TargetFollowCamera_dtor(this->camera));
        this->camera = 0;
    }
    TargetFollowCamera *tfc = (TargetFollowCamera *) ::operator new(0x178);
    TargetFollowCamera_ctor(tfc, this->cameraId,
                            (int) (intptr_t) this->player->geometry, 0, 0, 0, 0, 0, 0);
    this->camera = tfc;
    pc->CameraSetCurrent(this->cameraId);
    this->player->setTargetFollowCamera(this->camera);
    this->camera->resetShipHandling();

    Radar *radar = new Radar(this->level);
    this->radar = radar;

    if (Status::gStatus->getMission() != 0)
        this->campaignMission = (uint8_t)((Mission *) (Status::gStatus->getMission()))->isCampaignMission();

    LevelScript *script = new LevelScript(this->level, this->hud,
                                          this->radar, this->camera);
    this->levelScript = script;
    ((LevelScript *) (script))->lookBehind();
    this->level->initParticleSystems();

    ChoiceWindow *cw = new ChoiceWindow();
    this->choiceWindow = cw;

    this->elapsedTime = 0;
    this->elapsedTimeHigh = 0;
    this->cameraMode = 0;
    this->field_0x18 = 0;
    this->field_0x5c = 0;
    this->jumpActive = 0;
    this->dockChoiceOpen = 0;
    this->field_0xc6 = 0;
    this->field_0xc8 = 0;
    this->field_0xe0 = 0;
    this->freeCamMode = 0;
    this->field_0x110 = 0;
    this->flFastForwardFactor = 0x3f800000;
    this->cloakAttribute = 0;
    this->cloakAttributeMax = g_resInitB;
    this->lastTapTime = 0;
    this->lastTapTimeHigh = 0;
    this->lastAlignTime = 0;
    this->lastAlignTimeHigh = 0;
    this->thrustActive = 0;
    this->thrustThreshold = (*g_resShipTune)[0x2f4 / 4];
    this->thrustBase = 0;
    this->gameRecord = 0;
    this->maneuverHoldTime = 0;
    this->maneuverActive = 0;
    this->maneuverStartX = 0;
    this->maneuverStartY = 0;

    unsigned t = this->applicationManager->GetCurrentTimeMillis();

    *(uint16_t *) &this->needsRedraw = 0x101;
    this->field_0xcf = 0;
    this->choiceItemCount = 0;
    this->field_0xd4 = 0;
    this->field_0x1e4 = 0;
    this->field_0x1e6 = 0;
    this->startTime = t & 0xffff;
    this->startTimeHigh = 0;
    this->lastTime = t & 0xffff;
    this->lastTimeHigh = 0;
    this->field_0x1dd = **g_resPauseSrc;

    Status::gStatus->field_0x184 = 0;
    Status::gStatus->field_0x188 = 1;
    Status::gStatus->field_0x18c = 1;
}

static int g_accelTune;

// g_accelTune holds a pointer (as int) to the accelerometer-tuning config.
struct AccelTune {
    uint8_t _pad0[0x10];
    char invertSign;     // offset 0x10
    uint8_t _pad11[0xb];
    float refValue;      // offset 0x1c
    float baseValue;     // offset 0x20
};
// Accelerometer-context value buffer returned by MGame_accelCtxValue();
// double at +0x10 is the third component.
struct AccelCtxValue {
    double _v0;          // offset 0x00
    double _v1;          // offset 0x08
    double comp2;        // offset 0x10
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(AccelTune, invertSign) == 0x10, "AccelTune::invertSign @ 0x10");
static_assert(offsetof(AccelTune, refValue) == 0x1c, "AccelTune::refValue @ 0x1c");
static_assert(offsetof(AccelTune, baseValue) == 0x20, "AccelTune::baseValue @ 0x20");
static_assert(offsetof(AccelCtxValue, comp2) == 0x10, "AccelCtxValue::comp2 @ 0x10");
#endif

void MGame::handleAccelerometer() {
    Engine *eng = (Engine *) this->applicationManager->GetEngine();
    double *acc = ((Engine *) (eng))->GetAccelValue();

    float yaw = (float) (acc[1] * 2.5);
    float steer = 1.0f;
    if (yaw <= 1.0f) {
        steer = -1.0f;
        if (yaw < -1.0f) {
            this->player->left(0x32, steer * steer);
            goto afterYaw;
        }
        steer = yaw;
        if (yaw < 0.0f) {
            this->player->left(0x32, steer * steer);
            goto afterYaw;
        }
        if (yaw == 0.0f)
            goto afterYaw;
    }
    this->player->right(0x32, steer * steer);

afterYaw: {
        AbyssEngine::ApplicationManager *appMgr = this->applicationManager;

        double *v1 = ((Engine *) appMgr->GetEngine())->GetAccelValue();
        double d0 = *v1;
        AccelCtxValue *c2 =
            (AccelCtxValue *) ((Engine *) appMgr->GetEngine())->GetAccelValue();

        float base = (float) d0;
        AccelTune *tune = (AccelTune *) (intptr_t) g_accelTune;
        double dz = c2->comp2;
        float ref = tune->refValue;
        char sign = tune->invertSign;

        AccelCtxValue *c3 =
            (AccelCtxValue *) ((Engine *) appMgr->GetEngine())->GetAccelValue();

        float a, b;
        if (sign == 0) {
            float t = base;
            if (c3->comp2 > 0.0) {
                t = 1.0f;
                if (base <= 1.0f) t = (1.0f - base) + 1.0f;
            }
            a = ref - t;
            b = tune->baseValue - (float) dz;
        } else {
            float t = base;
            if (c3->comp2 > 0.0) {
                t = 1.0f;
                if (base <= 1.0f) t = (1.0f - base) + 1.0f;
            }
            a = t - ref;
            b = (float) dz - tune->baseValue;
        }

        float a3 = a * 3.0f;
        float b3 = b * 3.0f;
        float aAbs = a * -3.0f;
        if (a3 > 0.0f) aAbs = a3;
        float bAbs = b * -3.0f;
        if (b3 > 0.0f) bAbs = b3;
        float roll = (aAbs < bAbs) ? b3 : a3;

        float rollMag = 1.0f;
        int *shipField = (this->timeWarpState > 0) ? &this->field_0x44 : &this->deltaTime;
        if (roll <= 1.0f) {
            rollMag = -1.0f;
            if (roll < -1.0f) {
                return this->player->turnHorizontal(*shipField, rollMag * rollMag);
            }
            rollMag = roll;
            if (roll < 0.0f) {
                return this->player->turnHorizontal(*shipField, rollMag * rollMag);
            }
            if (roll == 0.0f) return;
        }
        return this->player->turnHorizontal(*shipField, rollMag * rollMag);
    }
}


static int **g_tmShipTune;

static int **g_tmStarMap;

static int **g_tmAppData;

void MGame::OnTouchMove(int p1, int y, void *touch) {
    MGame *self = this;
    int handledFree = 0;
    if (self->pauseOpen != 0) {
        uint8_t fc = self->freeCamMode;
        int allowFree = (fc != 0);
        if (fc != 0 &&
            self->menuWindow->isShowingMessage() == 0 &&
            self->menuWindow->isMakingScreenshot() == 0) {
            allowFree = 1;
        } else {
            allowFree = 0;
        }
        (void) allowFree;
    }

    if (self->freeCamDragging != 0 && self->hudTouchFlags == 0 &&
        self->cameraMode == 3) {
        ((MGame *) (self))->freeCamTouchMove(p1, y, touch);
        handledFree = 1;
    }

    if (!handledFree) {
        if (self->pauseOpen == 0) {
            int hh = self->hud->touchMove(p1, y, touch);
            self->hudTouchFlags = hh;
            unsigned mode = (unsigned) self->cameraMode;
            if (mode <= 1) {
                ((MGame *) (self))->maneuverTouchMove(mode, y, touch);
                if (self->thrustActive != 0 && self->jumpActive == 0) {
                    int f8 = self->hudTouchFlags;
                    int ok = (f8 == 0) ||
                             (f8 == 0x20 && self->activeTouchId != touch);
                    if (ok) {
                        bool apply = false;
                        if (self->thrustEngaged == 0) {
                            float fy = (float) y;
                            float start = (float) self->thrustStartY;
                            float thresh = (float) self->thrustThreshold;
                            float d = fy - start;
                            float ad = (d > 0.0f) ? d : -d;
                            if (ad > thresh) {
                                int dir = (start > fy) ? 1 : -1;
                                int ny = dir + y;
                                self->thrustResetX = 0;
                                self->thrustStartY = ny;
                                self->thrustBase = (int) self->player->getThrust();
                                apply = true;
                            }
                        } else {
                            apply = true;
                        }
                        if (apply) {
                            self->thrustEngaged = 1;
                            float divisor = *(float *) &(*g_resShipTune)[0x2f8 / 4];
                            float t = ((float) self->thrustStartY - (float) y -
                                       (float) self->thrustResetX) / divisor +
                                      (float) self->thrustBase;
                            if (t < 0.0f) t = 0.0f;
                            if (t > 1.0f) t = 1.0f;
                            self->player->setThrust(t);
                            self->player->throttleChanged();
                        }
                    }
                }
            }
        }
    }

    if (self->pauseOpen == 0) return;

    if (self->gameOverActive != 0 || self->autopilotMenuOpen != 0 ||
        self->choiceWindowOpen != 0 || self->dockChoiceOpen != 0) {
        self->choiceWindow->OnTouchMove(p1, y);
        return;
    }
    if (self->starMapOpen != 0) {
        int sel = **g_tmStarMap;
        if (*(uint8_t *) sel == 0)
            self->starMap->OnTouchMove(p1, y);
        else
            ((Layout *) (**g_tmStarMap))->OnTouchMove(p1, y);
        return;
    }
    if (self->cutsceneActive != 0) {
        self->dialogueWindow->OnTouchMove(p1, y);
        return;
    }
    if (self->menuTouchOpen != 0) {
        MGameAppData *ad = (MGameAppData *) (intptr_t) **g_tmAppData;
        if (ad->modalActive == 0 && ad->transitionActive == 0)
            self->menuWindow->OnTouchMove(p1, y, touch);
    }
}


static uint8_t *g_cinFlagA;

static uint8_t **g_cinFlagB;

void MGame::setCinematicMode(bool on) {
    MGame *self = this;
    self->freeCamMode = on;
    *g_cinFlagA = on;
    uint8_t *flag = *g_cinFlagB;
    if (!on) {
        *flag = self->field_0x1dd;
        ((MGame *) (self))->switchCamera(self->cinematicPrevCamMode);
        return Cam_setCinematic(self->camera, self->cinematicPrevLookAt);
    }
    self->field_0x1dd = *flag;
    *flag = 1;
    if (self->jumpDriveActive == 0 && self->jumpActive == 0) {
        self->cinematicPrevCamMode = self->cameraMode;
        self->cinematicPrevLookAt = TFC_isInLookAtMode(self->camera);
        TFC_setLookAtCam(self->camera, 0);
        ((MGame *) (self))->switchCamera(3);
        return self->level->lodManager->forceUpdate(self->deltaTime, true);
    }
}

void TFC_translate(void *cam, int x, int y, int z);

void *TFC_getPosition(void *cam);


static int g_ujZNear;

static int g_ujZSound;

static int g_ujSpeed;

static int *g_ujSound;

static int g_ujPos0;

static int g_ujPos1;

static int g_ujPos2;

static int **g_ujStation;

static uint8_t **g_ujFlagA;

static uint8_t **g_ujFlagB;

static int **g_ujFlagC;

void MGame::updateJumpScene() {
    MGame *self = this;
    bool fadeOut = true;

    if (self->usingJumpDrive != 0 && self->jumpFlash != 0) {
        AbyssEngine::Transform *tr =
            (AbyssEngine::Transform *) (intptr_t) PaintCanvas::gCanvas->TransformGetTransform((unsigned) (uintptr_t) PaintCanvas::gCanvas);
        long long ct = tr->currentTime;
        int prog = (int) ((unsigned long long) ct >> 32);
        int over = ((unsigned) ct > 0x6a4);
        if ((0 - over) - prog < 0) goto camMove;
    } else {
        Array<KIPlayer *> *lm = self->level->getLandmarks();
        if ((int) (intptr_t)(*lm)[1] != 0) {
            self->level->getLandmarks();
            int jg = (int) (intptr_t)(*lm)[1];
            if (((PlayerJumpgate *) (jg))->timeToJump() == 0) goto camMove;
        }
    }
    fadeOut = true;
    goto afterCam;

camMove: {
        int speed = self->deltaTime;
        Player *ego = (Player *) self->player->player;
        float mtx[4];
        Vector *egoTransformPos = reinterpret_cast<Vector *>(ego->transform);
        *egoTransformPos = AbyssEngine::AEMath::MatrixRotateVector(
            *(const AbyssEngine::Matrix *) (mtx), *egoTransformPos);
        self->egoJumpPos = *(const Vector *) ((Vector *) mtx);
        TFC_translate(self->camera, 0, 0, 0);
        (void) speed;
        if (self->usingJumpDrive != 0) {
            self->jumpFlash->getPosition();
        } else {
            Array<KIPlayer *> *lm = self->level->getLandmarks();
            KIPlayer *obj = (*lm)[1];
            *(Vector *) ((Vector *) mtx) = obj->getPosition();
        }
        self->egoJumpPos = *(const Vector *) ((Vector *) mtx);
        fadeOut = false;
    }

afterCam:
    if (self->usingJumpDrive != 0) {
        unsigned tr = (unsigned) (long) PaintCanvas::gCanvas->TransformGetTransform((unsigned) (uintptr_t) PaintCanvas::gCanvas);
        ((AbyssEngine::Transform *) (tr))->Update(self->deltaTime, false /* updateBounds: arg lost in decomp */);
    }

    Vector *camPos = (Vector *) TFC_getPosition(self->camera);
    float threshold = (float) self->egoJumpPosZ + *(float *) &g_ujZNear;
    if (camPos->z < threshold && self->usingJumpDrive == 0) {
        Array<KIPlayer *> *lm = self->level->getLandmarks();
        ((PlayerJumpgate *) ((int) (intptr_t)(*lm)[1]))->activate();
        float p[4];
        ((PlayerEgo *) (p))->getPosition();
        float t2 = (float) self->egoJumpPosZ + *(float *) &g_ujZSound;
        if (t2 <= p[2] && self->jumpGateSoundStarted == 0) {
            int *snd = g_ujSound;
            FModSound_setProp(*snd, self->player->field_0x1c);
            FModSound_setProp(*snd, 0x8d5);
            FModSound_setProp(*snd, 0x8d4);
            FModSound_setProp(*snd, 0x23);
            ((FModSound *) (*snd))->play(0x1f, 0, 0 /* vel: arg lost in decomp */, 0.0f);
            self->jumpGateSoundStarted = 1;
        }
    }

    if (fadeOut) {
        self->player->setSpeed(*(float *) &g_ujSpeed);
        self->player->setVisible(0);
        self->player->setExhaustVisible(0);
    }

    bool ended;
    if (self->usingJumpDrive != 0) {
        AbyssEngine::Transform *tr =
            (AbyssEngine::Transform *) (intptr_t) PaintCanvas::gCanvas->TransformGetTransform((unsigned) (uintptr_t) PaintCanvas::gCanvas);
        ended = tr->animating != 0;
    } else {
        Array<KIPlayer *> *lm = self->level->getLandmarks();
        ended = ((PlayerJumpgate *) ((int) (intptr_t)(*lm)[1]))->animationEnded() != 0;
    }
    if (!ended) goto done;

    {
        if (Status::gStatus->getCurrentCampaignMission() == 0x2a && Status::gStatus->inAlienOrbit() != 0) {
            self->levelScript->setEvent(6);
            self->player->setSpeed(0.0f);
            Array<KIPlayer *> *lm = self->level->getLandmarks();
            KIPlayer *node = (*lm)[3];
            node->setPosition(*(float *) &g_ujPos0, *(float *) &g_ujPos1, *(float *) &g_ujPos2);
            lm = self->level->getLandmarks();
            KIPlayer *node2 = (*lm)[3];
            self->egoJumpPos = node2->getPosition();
            if (self->player->geometry != nullptr) {
                Vector pos = self->player->getPosition();
                ((AEGeometry *) (self->player->geometry))->setPosition(pos);
            }
            PlayerEgo *p = self->player;
            p->inWormhole = 1;
            self->jumpDriveActive = 0;
            ((PlayerEgo *) (p))->resetChargingDrive();
        } else {
            int **stationPtr = g_ujStation;
            ((Status *) ((Station *) Status::gStatus))->departStation((Station *) *stationPtr);
            self->level->setInitStreamOut();
            if (self->usingJumpDrive == 0)
                ((Status *) ((Station *) Status::gStatus))->jumpgateUsed();
            if (((Station *) ((Station *) *stationPtr))->equals(Status::gStatus->playerStation) != 0) {
                **g_ujFlagA = 1;
                **g_ujFlagB = 1;
                ((Status *) ((Station *) Status::gStatus))->setStation(
                    (Station *) *stationPtr /* station: arg lost in decomp */);
            }
            *stationPtr = 0;
            Status::gStatus->field_64 = ((Player *) (self->player->player))->getHitpoints();
            Status::gStatus->field_5c = ((Player *) (self->player->player))->getShieldHP();
            Status::gStatus->field_60 = ((Player *) (self->player->player))->getArmorHP();
            Status::gStatus->field_68 = ((Player *) (self->player->player))->getGammaHP();
            Status::gStatus->field_f4 = self->player->getCurrentSecondaryWeaponIndex();
            **g_ujFlagC = 1;
            self->active = 0;
            self->applicationManager->SetCurrentApplicationModule(5);
        }
    }

done:
    ;
}


static int g_mgameInitVal;

MGame::MGame() {
    { if (this->gameOverTitle.data) delete[] this->gameOverTitle.data; this->gameOverTitle.data = nullptr; this->gameOverTitle.length = 0; }

    int z = 0;
    int initVal = g_mgameInitVal;

    // 0xa4 run: freeCamFinger1 (x,y,z) + freeCamFinger0X
    this->freeCamFinger1X = 0;
    this->freeCamFinger1Y = 0;
    this->freeCamFinger1Z = 0;
    this->freeCamFinger0X = 0;
    // 0x13c run: flShakeAmpX, flShakeAmpY, field_0x144, flShakePhaseX
    this->flShakeAmpX = 0;
    this->flShakeAmpY = 0;
    this->field_0x144 = 0;
    this->flShakePhaseX = 0;
    // 0x18c run
    this->field_0x18c = 0;
    this->field_0x190 = 0;
    this->field_0x194 = 0;
    this->field_0x198 = 0;

    this->elapsedTime = z;
    this->elapsedTimeHigh = z;
    this->egoJumpPosX = z;
    this->egoJumpPosY = z;
    this->egoJumpPosZ = z;
    this->field_0x1bc = z;
    this->thrustStartY = z;
    this->field_0x1c4 = z;
    this->freeCamFinger0Y = z;
    this->freeCamFinger0Z = z;
    this->flShakePhaseY = z;
    this->field_0x150 = z;
    this->field_0x19c = z;
    this->field_0x1a0 = z;

    this->loadProgress = 0x64;
    this->loadingImage = -1;
    this->cameraMode = z;

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
    // 0x80 run: radar, radio, menuWindow, dialogueWindow
    this->radar = 0;
    this->radio = 0;
    this->menuWindow = 0;
    this->dialogueWindow = 0;
    // 0x70 run: field_0x70, hud, level, levelScript
    this->field_0x70 = 0;
    this->hud = 0;
    this->level = 0;
    this->levelScript = 0;

    this->pauseSnapshot = 0;
    this->active = 0;
    this->turretMode = 0;
    this->hudMenuOpen = 0;
    this->jumpFlash = 0;
    this->camera = 0;
    *(uint8_t *) &this->field_0x1e4 = 0;
    this->field_0x1d4 = z;
    this->deltaTime = z;
    this->player = 0;
    this->field_0x5c = z;
    this->gameOverActive = 0;
    this->campaignMission = 0;
    this->starMap = 0;
    this->choiceWindow = 0;
    this->orbitMenuOpen = 0;
    this->dockChoiceOpen = z;
    this->autopilotMenuOpen = 0;
    this->field_0xc6 = 0;
    this->starMapOpen = 0;
    this->choiceWindowOpen = 0;
    this->field_0xcf = 0;
    this->field_0xca = z;
    this->field_0xd8 = z;
    this->jumpDriveActive = z;
    this->field_0x1d8 = initVal;
    this->field_0x1e0 = z;
}

void *Radio_dtor(...);

void *DialogueWindow_dtor(...);


static int g_relPostEffect;

static int *g_relSoundFlag;

static int *g_relLayout;

static int **g_relImgFactory;

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
    ((Engine *) (eng))->SetPostEffect(g_relPostEffect, 0);

    int *soundFlag = g_relSoundFlag;
    if (*soundFlag != 0) {
        ((FModSound *) (0))->setDownPitch(true /* down: arg lost in decomp */);
        ((FModSound *) ((Engine *) *soundFlag))->disableReverb();
        ((FModSound *) ((Engine *) *soundFlag))->stopAllSoundFXEvents();
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

    if (this->radio != 0)
        ::operator delete(Radio_dtor(this->radio));
    this->radio = 0;

    StarMapModule *m1 = (StarMapModule *) ApplicationManager::gAppManager->GetApplicationModule(0);
    if (m1->starMap != 0) {
        StarMapModule *m2 = (StarMapModule *) ApplicationManager::gAppManager->GetApplicationModule(0);
        delete m2->starMap;
    }
    StarMapModule *m3 = (StarMapModule *) ApplicationManager::gAppManager->GetApplicationModule(0);
    m3->starMap = 0;

    delete this->menuWindow;
    this->menuWindow = 0;

    if (this->dialogueWindow != 0)
        ::operator delete(DialogueWindow_dtor(this->dialogueWindow));
    this->dialogueWindow = 0;

    delete this->starMap;
    this->starMap = 0;

    delete this->choiceWindow;
    this->choiceWindow = 0;

    this->field_0xd8 = 0;
    this->jumpDriveActive = 0;
    this->turretMode = 0;
    this->autopilotMenuOpen = 0;
    this->touchesStream = 0;

    if (this->camera != 0)
        ::operator delete(TargetFollowCamera_dtor(this->camera));
    this->camera = 0;

    if (this->gameRecord != 0) {
        ((GameRecord *) (intptr_t) this->gameRecord)->~GameRecord();
        ::operator delete((void *) (intptr_t) this->gameRecord);
    }
    this->gameRecord = 0;

    {
        int *p = (int *) 0;
        (void) p;
    }
    PaintCanvas::gCanvas->ReleaseAllResources();

    int lang = GameText::getLanguage();
    Globals::gGlobals->loadFont(lang);

    int *layout = g_relLayout;
    if (*layout != 0) {
        ((Layout *) (*layout))->reload();
        ((ImageFactory *) (**g_relImgFactory))->reload();
        ((Layout *) (*layout))->initTip();
    }

    if (this->missionInfoLines != 0) {
        ArrayReleaseClasses(*this->missionInfoLines); ArrayRemoveAll(*(this->missionInfoLines));
        delete this->missionInfoLines;
    }
    this->missionInfoLines = 0;

    if (*soundFlag != 0)
        FModSound_restoreState();
}


static Layout ***g_r2dHelpLayout;

static int **g_r2dPauseFlag;

static int **g_r2dRewardLayout;

static Layout ***g_r2dFadeLayout;

void MGame::OnRender2D() {
    MGame * self = this;
    if (self->active == 0) {
        return;
    }

    PaintCanvas::gCanvas->Begin2d();

    if (self->pauseOpen != 0 && self->menuTouchOpen != 0) {
        bool drawSS = true;
        if (self->freeCamMode != 0 && self->freeCamDragging != 0 &&
            self->menuWindow->pendingActivate == 0) {
            drawSS = true;
        } else {
            self->menuWindow->draw();
            drawSS = (self->freeCamMode != 0);
        }
        if (drawSS) {
            self->level->getStarSystem();
            ((StarSystem *) (0))->render2D();
        }
        float v[4];
        v[0] = 0.5f;
        v[1] = 0.5f;
        v[2] = 0;
        Engine *eng = (Engine *) self->applicationManager->GetEngine();
        *(Vector *) &eng->field_0x3cc = *(const Vector *) (v);
        PaintCanvas::gCanvas->End2d();

        return;
    }

    if (self->pauseOpen == 0 || self->menuTouchOpen == 0) {
        if (self->needsRedraw == 0) {
            self->level->getStarSystem();
            ((StarSystem *) (0))->render2D();
            if (self->levelScript->startSequenceOver() != 0 ||
                self->levelScript->startSequence() == 0) {
                long long now = (long long) self->applicationManager->GetSystemTimeMillis();
                self->radio->draw(now, (PlayerEgo *) (self->player),
                                  (LevelScript *) (self->levelScript));
            }
        } else if (self->starMapOpen != 0) {
            self->starMap->draw();
            Layout *hl = **g_r2dHelpLayout;
            if (*(uint8_t *) hl != 0)
                ((Layout *) (hl))->drawHelpWindow();
        } else {
            self->level->render2D();
            if (**g_r2dPauseFlag == 0)
                self->hud->drawPauseButton();

            if (((Mission *) ((Mission *) Status::gStatus->getCampaignMission()))->getType() == 0xaa) {
                if (self->levelScript->getEvent() == 0)
                    self->hud->drawOrbitInformation();
                {
                    long long now = (long long) self->applicationManager->GetSystemTimeMillis();
                    self->radio->draw(now, (PlayerEgo *) (self->player),
                                      (LevelScript *) (self->levelScript));
                }
                if (self->cutsceneActive != 0)
                    self->dialogueWindow->draw();
                self->field_0x110 = 0;
                self->pauseOpen = 0;
            } else if (self->jumpActive != 0 || self->jumpDriveActive != 0) {
                int cm = Status::gStatus->getCurrentCampaignMission();
                if (cm > 7 && (uint8_t) self->levelScript->m_nFlags == 0 &&
                    self->jumpDriveActive == 0 &&
                    self->player->isDockingToPlanet() == 0 &&
                    self->levelScript->getEvent() == 0)
                    self->hud->drawOrbitInformation();
                if (self->levelScript->startSequenceOver() != 0 ||
                    self->levelScript->startSequence() == 0) {
                    long long now = (long long) self->applicationManager->GetSystemTimeMillis();
                    self->radio->draw(now, (PlayerEgo *) (self->player),
                                      (LevelScript *) (self->levelScript));
                }
                if (self->cutsceneActive != 0)
                    self->dialogueWindow->draw();
                self->field_0x110 = 0;
                self->pauseOpen = 0;
            } else if (self->gameOverActive == 0) {
                self->player->draw(1 /* allowHud: arg lost in decomp */);
                if (self->player->isMining() == 0 &&
                    self->starMapOpen == 0 &&
                    (self->player->isHacking() == 0 ||
                     self->menuTouchOpen != 0))
                    self->radar->draw((Player *) (self->player), (Hud *) (self->hud),
                                      (int) (intptr_t)(self->level));
                if (self->cutsceneActive == 0) {
                    ((MGame *) (self))->nextCamId(self->cameraMode);
                    {
                        long long now = (long long) self->applicationManager->GetSystemTimeMillis();
                        self->hud->draw(now, (long long) self->deltaTime, self->player,
                                        self->pauseOpen != 0, 0, 0);
                    }
                    {
                        long long now = (long long) self->applicationManager->GetSystemTimeMillis();
                        self->radio->draw(now, (PlayerEgo *) (self->player),
                                          (LevelScript *) (self->levelScript));
                    }
                    self->radar->drawCurrentLock((Hud *) (self->hud) /* hud: arg lost in decomp */);
                    ((Layout *) (**g_r2dRewardLayout))->
                            drawMissionRewardMessage(1 /* transition: arg lost in decomp */);
                } else {
                    self->dialogueWindow->draw();
                }
                if (self->autopilotMenuOpen != 0 || self->field_0xc6 != 0 ||
                    self->choiceWindowOpen != 0 || self->dockChoiceOpen != 0)
                    self->choiceWindow->draw();
                if (self->field_0xca != 0)
                    self->hud->drawMenu(0);
            } else if (!(self->elapsedTimeHigh < (int) (self->elapsedTime < 0xbb9))) {
                PaintCanvas::gCanvas->SetColor((unsigned) (intptr_t) self->paintCanvas);
                PaintCanvas::gCanvas->DrawImage2D((unsigned) self->loadingImage, 0, 0, (unsigned char) 'D');
            }
            ((Layout *) (**g_r2dFadeLayout))->drawFade();
        }
    }

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
        if (((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(8) != 0 ||
            ((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(0x23) != 0) {
            id = this->player->hasAutoTurret() == 0 ? 1 : 2;
        } else {
            id = 2;
        }
    }
    if (id == 2) id = 3;
    if (id >= 4) {
        if (this->player->isDockedToDockingPoint() == 0) return 0;
        if (((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(8) == 0 &&
            ((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(0x23) == 0) {
            return 3;
        }
        id = this->player->hasAutoTurret() != 0 ? 3 : 1;
    }
    return id;
}
