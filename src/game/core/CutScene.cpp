#include "game/core/CutScene.h"
#include "engine/core/AERandom.h"
#include "game/ship/TargetFollowCamera.h"
#include "engine/render/PaintCanvas.h"
#include "game/mission/Status.h"
#include "game/core/Globals.h"
#include "game/world/SolarSystem.h"
#include "engine/render/AEGeometry.h"
#include "game/mission/Item.h"
#include "engine/render/LODManager.h"
#include "engine/math/Transform.h"
#include "engine/core/ApplicationManager.h"
#include "game/world/Level.h"
#include "game/world/LevelScript.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerFighter.h"
#include "game/ship/Ship.h"


using AbyssEngine::AERandom;
using AbyssEngine::AnimationMode;

static float CutScene_proc_tx0, CutScene_proc_tz0;
static float CutScene_proc_rx0, CutScene_proc_rz0, CutScene_proc_rz1;
static float CutScene_fogColorMode17, CutScene_fogDensityMode17;
static float CutScene_fogColorMode4, CutScene_fogDensityMode4;
static float CutScene_initStartXRotMode2;
static float CutScene_persp_fov, CutScene_persp_znear, CutScene_persp_zfar;
static float CutScene_fogDensity_mode17;
static float CutScene_fogDensity_mode4;
static float CutScene_fogColor;
static float CutScene_persp_fov_mode17;
static float CutScene_persp_fov_mode4;
static float CutScene_turret_tx, CutScene_turret_ty, CutScene_turret_tz;
static float CutScene_turret_rx, CutScene_turret_ry, CutScene_turret_rz;
static float CutScene_turret_rx2, CutScene_turret_ry2, CutScene_turret_rz2;
static int CutScene_shipBankTable[256];

float VectorSignedToFloat(int v, int mode);

void MatrixSetRotation(void *m, float x, float y, float z);

void MatrixSetTranslation(void *m, float x, float y, float z);

int Station_getIndex(void *station);

CutScene::CutScene(int mode) {
    this->shipPosY = 0;
    this->shipPosZ = 0;
    this->mode = mode;
    this->level = nullptr;
    this->turretGeom = nullptr;
    this->followCamera = nullptr;
    this->cameraId6c = (uint32_t) - 1;
    this->cameraId70 = (uint32_t) - 1;
    this->initialized = 0;
    this->geometries = nullptr;
    this->vec8 = Vector{0.0f, 0.0f, 0.0f};
    this->vec8w = 0.0f;
    this->geom28 = nullptr;
    this->geom2c = nullptr;
    this->geom30 = nullptr;
    this->geom34 = nullptr;

    uint32_t bits = 0x3851b717u;
    __builtin_memcpy(&this->rotationSpeed, &bits, sizeof bits);
}

CutScene::~CutScene() {
    delete this->rootGeom;
    this->rootGeom = nullptr;

    delete this->turretGeom;
    this->turretGeom = nullptr;

    delete this->level;
    this->level = nullptr;

    PaintCanvas::gCanvas->FogEnable(0, AbyssEngine::FogMode_dummy);

    delete this->geom28;
    this->geom28 = nullptr;
    delete this->geom2c;
    this->geom2c = nullptr;
    delete this->geom30;
    this->geom30 = nullptr;
    delete this->geom34;
    this->geom34 = nullptr;

    if (this->geometries != nullptr) {
        ArrayReleaseClasses(*this->geometries); ArrayRemoveAll(*(this->geometries));
        delete this->geometries;
    }
    this->geometries = nullptr;
}

uint8_t CutScene::isInitialized() {
    return this->initialized;
}

void CutScene::update() {
    if (this->level != nullptr)
        this->level->update((long long) (int) this->frameDelta, 0u);
}

void CutScene::update(int /*delta*/) {
    this->update();
}

void CutScene::render3D() {
    if (this->level != nullptr) {
        uint32_t t = (uint32_t) ApplicationManager::gAppManager->GetElapsedTimeMillis();
        this->frameDelta = t;
        this->level->update((long long) (int) t, 0u);
        this->level->render(this->frameDelta);
    }
    if (this->geom28 != nullptr) this->geom28->render();
    if (this->geom2c != nullptr) this->geom2c->render();
    if (this->geom30 != nullptr) this->geom30->render();
    if (this->geom34 != nullptr) this->geom34->render();

    if (this->geometries != nullptr) {
        for (AEGeometry *g: *this->geometries)
            g->render();
    }
}

void CutScene::render2D() {
    if (this->level != nullptr)
        this->level->render2D();
}

void CutScene::process(int /*delta*/) {
    if (this->initialized == 0)
        return;

    unsigned int now = (unsigned int) ApplicationManager::gAppManager->GetCurrentTimeMillis();
    unsigned int prev = this->prevTimeLo;
    unsigned int dt = now - prev;

    unsigned int lo = this->accumLo;
    this->accumLo = lo + dt;
    this->accumHi = this->accumHi + ((int) dt >> 31) + (lo + dt < lo ? 1u : 0u);
    this->frameDelta = dt;
    this->renderAtTimeLo = now & 0xffff;
    this->renderAtTimeHi = 0;
    this->prevTimeLo = now & 0xffff;
    this->prevTimeHi = 0;

    if (this->followCamera != nullptr)
        this->followCamera->update((int) this->frameDelta);

    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (this->mode == 2) {
        float ft = VectorSignedToFloat((int) this->frameDelta, 0);
        this->cameraRotX = this->cameraRotX + this->rotationSpeed * ft;
        char tmp[0x3c];
        memcpy(tmp, canvas->CameraGetLocal(0), 0x3c);
        char mtx[0x3c];
        MatrixSetRotation(mtx, 0.0f, 0.0f, 0.0f);
        canvas->CameraSetLocal(0, *(const Matrix *) (uintptr_t) this->cameraId74);

        long long pt = Status::gStatus->getPlayingTime();
        Array<KIPlayer *> *enemies = this->level->getEnemies();
        if (pt != 0 && enemies != nullptr && enemies->size() > 1) {
            unsigned int n = enemies->size();
            void *e0 = (*enemies)[n - 2];

            if (e0 != nullptr && ((KIPlayer *) e0)->geometry != nullptr) {
                AEGeometry *g0 = ((KIPlayer *) e0)->geometry;
                float f = VectorSignedToFloat(this->frameDelta, 0);
                g0->translate(f * CutScene_proc_tx0, 0.0f, 0.0f);
                float f1 = VectorSignedToFloat(this->frameDelta, 0);
                float f2 = VectorSignedToFloat(-(int) this->frameDelta, 0);
                g0->rotate(f1 * CutScene_proc_rx0, 0.0f, f2 * CutScene_proc_rz0);
            }
            void *e1 = (*enemies)[n - 1];
            if (e1 != nullptr && ((KIPlayer *) e1)->geometry != nullptr) {
                AEGeometry *g1 = ((KIPlayer *) e1)->geometry;
                float f = VectorSignedToFloat(this->frameDelta, 0);
                g1->translate(f * CutScene_proc_tx0, 0.0f, 0.0f);
                float f1 = VectorSignedToFloat(this->frameDelta, 0);
                float f2 = VectorSignedToFloat(-(int) this->frameDelta, 0);
                g1->rotate(f1 * CutScene_proc_rx0, 0.0f, f2 * CutScene_proc_rz1);
            }
        }
    } else if (this->mode == 0x17) {
        unsigned int kind;
        void *st = Status::gStatus->getStation();
        if (Station_getIndex(st) == 0x65) {
            kind = 10;
        } else {
            st = Status::gStatus->getStation();
            if (Station_getIndex(st) == 100) {
                kind = 7;
            } else {
                kind = (unsigned int) ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace() | 2;
            }
        }
        this->fogTimer84 = this->frameDelta + this->fogTimer84;
        if (((SolarSystem *) (long) Status::gStatus->getSystem())->getRace() == 1) {
            canvas->FogSetParameter(AbyssEngine::FogMode_dummy, 0, CutScene_fogColorMode17, 1.0f,
                                    CutScene_fogDensityMode17);
        } else if (kind == 2 && this->geometries != nullptr) {
            unsigned int n = this->geometries->size();
            for (unsigned int i = 0; i < n; i++) {
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                t->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
                n = this->geometries->size();
            }
            if (this->fogTimer84 > 3000) {
                this->fogTimer84 = 0;
                AERandom *rng = AERandom::gRandom;
                for (unsigned int i = 0; i < n; i++) {
                    if (rng->nextInt() < 0x14) {
                        AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                        if (t->IsRunning() == 0) {
                            AEGeometry *c0 = (*this->geometries)[i];
                            AbyssEngine::Transform *tr;
                            tr = (AbyssEngine::Transform *) ((PaintCanvas *) (uintptr_t) c0->transform)->
                                    TransformGetTransform(0);
                            tr->SetAnimationState((AnimationMode) 3, nullptr);
                            tr = (AbyssEngine::Transform *) ((PaintCanvas *) (uintptr_t) c0->transform)->
                                    TransformGetTransform(0);
                            tr->SetAnimationState((AnimationMode) 1, nullptr);
                            tr = (AbyssEngine::Transform *) ((PaintCanvas *) (uintptr_t) c0->childTransform)->
                                    TransformGetTransform(0);
                            tr->SetAnimationState((AnimationMode) 3, nullptr);
                            tr = (AbyssEngine::Transform *) ((PaintCanvas *) (uintptr_t) c0->childTransform)->
                                    TransformGetTransform(0);
                            tr->SetAnimationState((AnimationMode) 1, nullptr);
                        }
                    }
                    n = this->geometries->size();
                }
            }
        }

        char tmp[0x3c];
        memcpy(tmp, canvas->TransformGetLocal(0), 0x3c);
        char mtx[0x3c];
        MatrixSetRotation(mtx, 0.0f, 0.0f, 0.0f);
        Array<KIPlayer *> *enemies = this->level->getEnemies();

        AEGeometry *leadGeom = (*enemies)[0]->geometry;
        canvas->TransformSetLocal(0, *(const Matrix *) (uintptr_t) leadGeom->transform);
        Array<KIPlayer *> *arr = this->level->getEnemies();
        for (unsigned int i = 0; i < arr->size(); i++) {
            Array<KIPlayer *> *en = this->level->getEnemies();
            if ((*en)[i] != nullptr) {
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                t->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
            }
            arr = this->level->getEnemies();
        }
    } else if (this->mode == 4) {
        Array<KIPlayer *> *arr = this->level->getEnemies();
        for (unsigned int i = 0; i < arr->size(); i++) {
            Array<KIPlayer *> *en = this->level->getEnemies();
            if ((*en)[i] != nullptr) {
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                t->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
            }
            arr = this->level->getEnemies();
        }

        int race = ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace();
        if (race == 0) {
            AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
            t->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
        } else {
            int race1 = ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace();
            if (race1 == 1) {
                canvas->FogSetParameter(AbyssEngine::FogMode_dummy, 0, CutScene_fogColorMode4, 1.0f,
                                        CutScene_fogDensityMode4);
                canvas->FogEnable(1, AbyssEngine::FogMode_dummy);
                this->level->getEnemies();
                this->level->getEnemies();
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                t->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
                if (this->geom30 != nullptr) {
                    AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                    t2->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
                    int acc = this->frameDelta + this->animTimer7c;
                    this->animTimer7c = acc;
                    if (acc > 20000) {
                        this->animTimer7c = 0;
                        if (AERandom::gRandom->nextInt() < 100) {
                            AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                            a->SetAnimationState((AnimationMode) 3, nullptr);
                            a = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                            a->SetAnimationState((AnimationMode) 1, nullptr);
                        }
                    }
                }
                if (this->geom34 != nullptr) {
                    AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                    t2->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
                    int acc = this->frameDelta + this->animTimer80;
                    this->animTimer80 = acc;
                    if (acc > 22000) {
                        this->animTimer80 = 0;
                        if (AERandom::gRandom->nextInt() < 100) {
                            AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                            a->SetAnimationState((AnimationMode) 3, nullptr);
                            canvas->TransformGetTransform(0);
                            if (this->level != nullptr) this->level->getEnemies();
                            return;
                        }
                    }
                }
                if (this->level != nullptr) this->level->getEnemies();
                return;
            }
            int race3 = ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace();
            if (race3 == 3) {
                this->level->getEnemies();
                this->level->getEnemies();
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                t->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
                if (this->geom28 != nullptr) {
                    AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                    t2->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
                }
                AbyssEngine::Transform *t3 = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                t3->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
                int acc7c = this->animTimer7c + this->frameDelta;
                int acc80 = this->frameDelta + this->animTimer80;
                this->animTimer7c = acc7c;
                this->animTimer80 = acc80;
                if (acc7c > 1000 && this->geom28 != nullptr) {
                    this->animTimer7c = 0;
                    if (AERandom::gRandom->nextInt() < 0x28) {
                        AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                        a->SetAnimationState((AnimationMode) 3, nullptr);
                        a = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                        a->SetAnimationState((AnimationMode) 1, nullptr);
                    }
                    acc80 = this->animTimer80;
                }
                if (acc80 > 2000) {
                    this->animTimer80 = 0;
                    if (AERandom::gRandom->nextInt() < 0x1e) {
                        AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
                        a->SetAnimationState((AnimationMode) 3, nullptr);
                        canvas->TransformGetTransform(0);
                        if (this->level != nullptr) this->level->getEnemies();
                        return;
                    }
                }
                if (this->level != nullptr) this->level->getEnemies();
                return;
            }
            if (((SolarSystem *) (long) Status::gStatus->getSystem())->getRace() != 2)
                return;
            if (this->level != nullptr) this->level->getEnemies();
            if (this->level != nullptr) this->level->getEnemies();
            AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
            t->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
            if (this->level != nullptr) this->level->getEnemies();
            if (this->level != nullptr) this->level->getEnemies();
            AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
            t2->Update((long long) (unsigned) this->frameDelta, (bool) (unsigned char) this->frameDelta);
        }
    }
}

void CutScene::renderBG() {
    if (this->level != nullptr)
        this->level->renderBG((int) this->frameDelta);
}

void CutScene::replacePlayerShip(int /*a*/, int b) {
    if (this->level->getEnemies() == nullptr)
        return;

    Array<KIPlayer *> *enemies = this->level->getEnemies();

    AEGeometry *oldGeom = (*enemies)[0]->geometry;
    if (oldGeom != nullptr) {
        if (this->turretGeom != nullptr) {
            PaintCanvas *canvas = PaintCanvas::gCanvas;
            Array<KIPlayer *> *en2 = this->level->getEnemies();
            AEGeometry *lead = (*en2)[0]->geometry;
            canvas->TransformRemoveChild(lead->childTransform, this->turretGeom->childTransform);
        }

        char matrix[0x3c];
        memcpy(matrix, &oldGeom->getMatrix(), 0x3c);

        AEGeometry *grp = Globals::gGlobals->getShipGroup(b, 0, false);

        Array<KIPlayer *> *en3 = this->level->getEnemies();
        (*en3)[0]->geometry = grp;

        Array<KIPlayer *> *en4 = this->level->getEnemies();

        (*en4)[0]->geometry->setMatrix(*(const Matrix *) matrix);

        Array<KIPlayer *> *en5 = this->level->getEnemies();
        KIPlayer *ship = (*en5)[0];
        float bank = VectorSignedToFloat(CutScene_shipBankTable[b], 0);

        ship->setPosition(0.0f, bank, 0.0f);

        Array<KIPlayer *> *en6 = this->level->getEnemies();
        ((PlayerFighter *) ((*en6)[0]))->setExhaustVisible(false);

        ((LODManager *) (*(void **) this))->removeObject(oldGeom);
        delete oldGeom;
    }

    this->checkForTurret();
}

void CutScene::initialize() {
    if (this->level == nullptr)
        this->level = new Level(this->mode);
    while (this->level->init() == 0) {
    }

    this->player = (PlayerEgo *) (intptr_t) this->level->getPlayer();
    if (this->player != nullptr)
        this->player->setActive(true);

    this->level->initParticleSystems();

    char localMatrix[0x3c];
    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (this->mode == 2) {
        canvas->CameraCreate(this->cameraId74);
        canvas->CameraSetPerspective(0, CutScene_persp_fov, CutScene_persp_znear, CutScene_persp_zfar);
        canvas->CameraSetCurrent(this->cameraId74);
        char tmp[0x3c];
        memcpy(tmp, canvas->CameraGetLocal(0), 0x3c);

        AERandom *rng = AERandom::gRandom;
        int rx = rng->nextInt();
        int ry = rng->nextInt();
        float tx = VectorSignedToFloat(rx - 20000, 0);
        float ty = VectorSignedToFloat(ry + 40000, 0);
        MatrixSetTranslation(localMatrix, tx, ty, 0.0f);
        this->cameraRotX = CutScene_initStartXRotMode2;
        MatrixSetRotation(localMatrix, 0.0f, 0.0f, 0.0f);
        canvas->CameraSetLocal(0, *(const Matrix *) (uintptr_t) this->cameraId74);

        long long pt = Status::gStatus->getPlayingTime();
        Array<KIPlayer *> *enemies = this->level->getEnemies();
        if (pt != 0 && enemies != nullptr && enemies->size() > 1) {
            unsigned int n = enemies->size();
            void *e0 = (*enemies)[n - 2];
            if (e0 != nullptr && (*enemies)[n - 1] != nullptr) {
                float v[3];
                v[0] = VectorSignedToFloat(rx - 24000, 0);
                v[1] = 0.0f;
                v[2] = VectorSignedToFloat(ry + 0x9a4c, 0);

                ((KIPlayer *) e0)->setPosition(v[0], v[1], v[2]);
                v[0] = VectorSignedToFloat(rx - 0x5b68, 0);
                v[2] = VectorSignedToFloat(ry + 0x96c8, 0);
                ((KIPlayer *) (*enemies)[n - 1])->setPosition(v[0], v[1], v[2]);
            }
        }
    } else if (this->mode == 0x17) {
        this->turretGeom = nullptr;
        canvas->CameraCreate(this->cameraId70);
        canvas->CameraSetPerspective(0, CutScene_persp_fov, CutScene_persp_znear, CutScene_persp_zfar);
        canvas->CameraSetCurrent(this->cameraId70);
        char tmp[0x3c];
        memcpy(tmp, canvas->CameraGetLocal(0), 0x3c);
        MatrixSetRotation(localMatrix, 0.0f, 0.0f, 0.0f);
        MatrixSetTranslation(localMatrix, 0.0f, 0.0f, 0.0f);
        canvas->CameraSetLocal(0, *(const Matrix *) (uintptr_t) this->cameraId70);
        canvas->TransformCreate(this->transformId78);
        this->level->getEnemies();
        ((AEGeometry *) nullptr)->getPosition();
        canvas->TransformGetLocal(0);
        MatrixSetTranslation(localMatrix, 0.0f, 0.0f, 0.0f);
        canvas->TransformAddChild(this->transformId78, this->cameraId70);
        this->resetCamera();
        this->checkForTurret();
    } else if (this->mode == 4) {
        canvas->CameraCreate(this->cameraId6c);
        canvas->CameraSetPerspective(0, CutScene_persp_fov, CutScene_persp_znear, CutScene_persp_zfar);
        if (this->followCamera != nullptr) {
            delete this->followCamera;
            this->followCamera = nullptr;
        }
        canvas->CameraSetCurrent(this->cameraId6c);
        this->resetCamera();
        int race = ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace();
        if (race == 3) {
            this->geom2c = new AEGeometry((uint16_t) 0x36d6, canvas, false);
            AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
            t->SetAnimationState((AnimationMode) 0, nullptr);
        } else {
            if (((SolarSystem *) (long) Status::gStatus->getSystem())->getRace() == 0) {
                this->geom30 = new AEGeometry((uint16_t) 0x37c8, canvas, false);
                this->geom34 = new AEGeometry((uint16_t) 0x37c7, canvas, false);
                this->geom30->addChild(this->geom34->childTransform);
                delete this->geom34;
                this->geom34 = nullptr;
            } else {
                ((SolarSystem *) (long) Status::gStatus->getSystem())->getRace();
            }
        }
    }

    this->rootGeom = new AEGeometry(canvas);
    this->rootGeom->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);

    this->accumLo = 0;
    this->accumHi = 0;
    this->animTimer7c = 0;
    this->animTimer80 = 0;
    this->fogTimer84 = 0;

    unsigned int now = (unsigned int) ApplicationManager::gAppManager->GetCurrentTimeMillis();
    this->initialized = 1;
    this->renderAtTimeLo = now & 0xffff;
    this->renderAtTimeHi = 0;
    this->prevTimeLo = now & 0xffff;
    this->prevTimeHi = 0;
}

void CutScene::resetCamera() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (this->mode == 0x17) {
        if (((SolarSystem *) (long) Status::gStatus->getSystem())->getRace() == 1) {
            canvas->FogSetParameter(AbyssEngine::FogMode_dummy, 0, CutScene_fogColor, 1.0f, CutScene_fogDensity_mode17);
            canvas->FogEnable(1, AbyssEngine::FogMode_dummy);
        }
        canvas->CameraSetCurrent(this->cameraId70);
        canvas->CameraSetPerspective(0, CutScene_persp_fov_mode17, CutScene_persp_znear, CutScene_persp_zfar);

        Array<KIPlayer *> *enemies = this->level->getEnemies();
        void *lead = (*enemies)[0];
        (void) lead;
        return;
    }

    if (this->mode != 4)
        return;

    if (((SolarSystem *) (long) Status::gStatus->getSystem())->getRace() == 1) {
        canvas->FogSetParameter(AbyssEngine::FogMode_dummy, 0, CutScene_fogColor, 1.0f, CutScene_fogDensity_mode4);
        canvas->FogEnable(1, AbyssEngine::FogMode_dummy);
    }
    canvas->CameraSetCurrent(this->cameraId6c);
    canvas->CameraSetPerspective(0, CutScene_persp_fov_mode4, CutScene_persp_znear, CutScene_persp_zfar);
}

class FileRead {
public:
    FileRead();

    ~FileRead();

    Array<Array<Vector *> *> *loadWeaponPositions(int32_t id);
};

void CutScene::checkForTurret() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (this->turretGeom != nullptr) {
        Array<KIPlayer *> *enemies = this->level->getEnemies();
        AEGeometry *lead = (*enemies)[0]->geometry;
        canvas->TransformRemoveChild(lead->childTransform, this->turretGeom->childTransform);
    }

    Ship *ship = Status::gStatus->getShip();
    Array<Item *> *equip = ship->getEquipment(2);
    if (equip == nullptr || equip->size() == 0)
        return;
    Item *item = (*equip)[0];
    if (item == nullptr)
        return;

    int idx = item->getIndex();

    unsigned short id0 = 0xffff;
    unsigned short id1 = 0xffff;
    int child0 = -1;
    int child1 = -1;
    int child2 = -1;

    switch (idx) {
        case 0xe0:
            id0 = 0x499a;
            id1 = 0x499b;
            child0 = 0x499c;
            child1 = 0x499d;
            child2 = -1;
            break;
        case 0x30: id0 = 0x1a74;
            id1 = 0x1a75;
            break;
        case 0x31: id0 = 0x1a76;
            id1 = 0x1a77;
            break;
        case 0xb4: id0 = 0x1a95;
            id1 = 0x1a96;
            break;
        case 0xb5: id0 = 0x1a97;
            id1 = 0x1a98;
            break;
        case 0xb6: id0 = 0x1a99;
            id1 = 0x1a9a;
            break;
        case 0xc6:
            id0 = 0x4963;
            id1 = 0x4967;
            child0 = 0x4964;
            child1 = 0x4966;
            child2 = -1;
            break;
        case 199:
            id0 = 0x4968;
            id1 = 0x496b;
            child0 = 0x4969;
            child1 = 0x496a;
            child2 = -1;
            break;
        case 200:
            id0 = 0x496c;
            id1 = 0x496f;
            child0 = 0x496d;
            child1 = 0x496e;
            child2 = 0x4970;
            break;
        case 0x2f: id0 = 0x1a72;
            id1 = 0x1a73;
            break;
        default: id0 = 0xffff;
            id1 = 0xffff;
            break;
    }

    AEGeometry *geom0 = new AEGeometry(id0, canvas, false);
    AEGeometry *geom1 = new AEGeometry(id1, canvas, false);
    geom1->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);

    if (child0 != -1) {
        AEGeometry *c = new AEGeometry((uint16_t)(unsigned short)child0, canvas, false);
        geom0->addChild(c->childTransform);
        delete c;
    }
    if (child1 != -1) {
        AEGeometry *c = new AEGeometry((uint16_t)(unsigned short)child1, canvas, false);
        geom1->addChild(c->childTransform);
        delete c;
    }
    if (child2 != -1) {
        AEGeometry *c = new AEGeometry((uint16_t)(unsigned short)child2, canvas, false);
        geom1->addChild(c->childTransform);
        delete c;
    }

    delete this->turretGeom;
    this->turretGeom = new AEGeometry(canvas);

    FileRead fr;
    int shipIdx = Status::gStatus->getShip()->getIndex();
    Array<Array<Vector *> *> *positions = fr.loadWeaponPositions(shipIdx);

    Vector *posVec = (*(*positions)[0])[0];
    geom0->setPosition(*posVec);
    geom1->setPosition(*posVec);
    geom0->translate(CutScene_turret_tx, CutScene_turret_ty, CutScene_turret_tz);

    int idx2 = item->getIndex();
    if (idx2 >= 0xc6 && item->getIndex() <= 200) {
        geom0->rotate(CutScene_turret_rx, CutScene_turret_ry, CutScene_turret_rz);
        geom1->rotate(CutScene_turret_rx2, CutScene_turret_ry2, CutScene_turret_rz2);
    }

    this->turretGeom->addChild(geom0->childTransform);
    this->turretGeom->addChild(geom1->childTransform);
    Array<KIPlayer *> *enemies2 = this->level->getEnemies();
    (*enemies2)[0]->geometry->addChild(this->turretGeom->childTransform);

    if (positions == nullptr)
        return;

    for (unsigned int i = 0; i < positions->size(); i++) {
        Array<Vector *> *slot = (*positions)[i];
        if (slot != nullptr) {
            ArrayReleaseClasses(*slot);
            delete (*positions)[i];
            (*positions)[i] = nullptr;
        }
    }
    delete positions;
}
