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

static float CutScene_fogColorMode17, CutScene_fogDensityMode17;
static float CutScene_fogColorMode4, CutScene_fogDensityMode4;
static float CutScene_persp_fov, CutScene_persp_znear, CutScene_persp_zfar;
static float CutScene_fogDensity_mode17;
static float CutScene_fogDensity_mode4;
static float CutScene_fogColor;
static float CutScene_persp_fov_mode17;
static float CutScene_persp_fov_mode4;
static float CutScene_turret_tx, CutScene_turret_ty, CutScene_turret_tz;
static float CutScene_turret_rx, CutScene_turret_ry, CutScene_turret_rz;
static float CutScene_turret_rx2, CutScene_turret_ry2, CutScene_turret_rz2;
// Android .rodata 0x1fb934; the following five words are a separate item-ID table.
static constexpr int CutScene_shipVerticalOffsets[64] = {
    160, 150, 220, 310, 160, 180, 140, 170, 170, 110, 250, 205, 240, 250, 250, 250,
    170, 170, 190, 460, 170, 210, 140, 170, 160, 190, 170, 240, 220, 280, 200, 200,
    140, 160, 180, 150, 310, 390, 480, 200, 300, 300, 170, 200, 170, 370, 370, 300,
    440, 340, 170, 470, 170, 170, 170, 170, 200, 170, 250, 150, 170, 250, 270, 170,
};

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
    this->process(0);
}

void CutScene::update(int delta) {
    this->process(delta);
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
        for (unsigned i = 0; i < this->geometries->size(); ++i)
            (*this->geometries)[i]->render();
    }
}

void CutScene::render2D() {
    if (this->level != nullptr)
        this->level->render2D();
}

void CutScene::process(int /*delta*/) {
    if (this->initialized == 0)
        return;

    uint64_t now = ApplicationManager::gAppManager->GetCurrentTimeMillis();
    unsigned int dt = (unsigned int) now - (unsigned int) this->prevTime;

    this->accum += (int64_t) (int32_t) dt;
    this->frameDelta = dt;
    this->renderAtTime = now;
    this->prevTime = now;

    if (this->followCamera != nullptr)
        this->followCamera->update(30);

    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (this->mode == 2) {
        float ft = VectorSignedToFloat((int) this->frameDelta, 0);
        this->cameraRotX = this->cameraRotX + this->rotationSpeed * ft;
        Matrix local = *static_cast<const Matrix *>(canvas->CameraGetLocal(this->cameraId74));
        AbyssEngine::AEMath::MatrixSetRotation(local, 0.0f, this->cameraRotX, 0.0f);
        canvas->CameraSetLocal(this->cameraId74, local);

        long long pt = Status::gStatus->getPlayingTime();
        if (pt > 0) {
            Array<KIPlayer *> *enemies = this->level->getEnemies();
            if (enemies == nullptr || enemies->size() < 2)
                return;
            unsigned int n = enemies->size();
            void *e0 = (*enemies)[n - 2];

            if (e0 != nullptr && ((KIPlayer *) e0)->geometry != nullptr) {
                AEGeometry *g0 = ((KIPlayer *) e0)->geometry;
                float step = VectorSignedToFloat((int) this->frameDelta, 0);
                g0->translate(step * 0.1f, 0.0f, step * 0.001f);
                float reverseStep = VectorSignedToFloat((int) (0u - this->frameDelta), 0);
                g0->rotate(reverseStep * 0.0002f, step * 0.0002f,
                           reverseStep * 0.00018f);
            }
            void *e1 = (*enemies)[n - 1];
            if (e1 != nullptr && ((KIPlayer *) e1)->geometry != nullptr) {
                AEGeometry *g1 = ((KIPlayer *) e1)->geometry;
                float step = VectorSignedToFloat((int) this->frameDelta, 0);
                g1->translate(step * 0.1f, 0.0f, 0.0f);
                float reverseStep = VectorSignedToFloat((int) (0u - this->frameDelta), 0);
                g1->rotate(step * 0.0002f, reverseStep * 0.0005f,
                           reverseStep * 0.0003f);
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
                kind = (unsigned int) Status::gStatus->getSystem()->getRace() | 2;
            }
        }
        this->fogTimer84 = this->frameDelta + this->fogTimer84;
        if (Status::gStatus->getSystem()->getRace() == 1) {
            canvas->FogSetParameter(AbyssEngine::FogMode_linear, 0.0f, 30000.0f, 1.0f,
                                    0x011e0cffu);
        } else if (kind == 2 && this->geometries != nullptr) {
            unsigned int n = this->geometries->size();
            for (unsigned int i = 0; i < n; i++) {
                AEGeometry *geom = (*this->geometries)[i];
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(geom->transform);
                t->Update((long long) (int32_t) this->frameDelta, false);
                n = this->geometries->size();
            }
            if (this->fogTimer84 > 3000) {
                this->fogTimer84 = 0;
                AERandom *rng = AERandom::gRandom;
                for (unsigned int i = 0; i < n; i++) {
                    if (rng->nextInt(100) < 20) {
                        AEGeometry *c0 = (*this->geometries)[i];
                        AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(c0->transform);
                        if (t->IsRunning() == 0) {
                            AbyssEngine::Transform *tr = (AbyssEngine::Transform *) canvas->TransformGetTransform(c0->transform);
                            tr->SetAnimationState((AnimationMode) 3, nullptr);
                            tr = (AbyssEngine::Transform *) canvas->TransformGetTransform(c0->transform);
                            tr->SetAnimationState((AnimationMode) 1, nullptr);
                            tr = (AbyssEngine::Transform *) canvas->TransformGetTransform(c0->childTransform);
                            tr->SetAnimationState((AnimationMode) 3, nullptr);
                            tr = (AbyssEngine::Transform *) canvas->TransformGetTransform(c0->childTransform);
                            tr->SetAnimationState((AnimationMode) 1, nullptr);
                        }
                    }
                    n = this->geometries->size();
                }
            }
        }

        Array<KIPlayer *> *enemies = this->level->getEnemies();
        AEGeometry *leadGeom = (*enemies)[0]->geometry;
        Matrix local = *static_cast<const Matrix *>(canvas->TransformGetLocal(leadGeom->transform));
        AbyssEngine::AEMath::MatrixSetRotation(local, 0.0f, this->cameraRotX, 0.0f);
        AEGeometry *leadGeomForWrite = (*this->level->getEnemies())[0]->geometry;
        canvas->TransformSetLocal(leadGeomForWrite->transform, local);
        Array<KIPlayer *> *arr = this->level->getEnemies();
        for (unsigned int i = 0; i < arr->size(); i++) {
            Array<KIPlayer *> *en = this->level->getEnemies();
            if ((*en)[i] != nullptr) {
                AEGeometry *geom = (*this->level->getEnemies())[i]->geometry;
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(geom->transform);
                t->Update((long long) (int32_t) this->frameDelta, false);
            }
            arr = this->level->getEnemies();
        }
    } else if (this->mode == 4) {
        Array<KIPlayer *> *arr = this->level->getEnemies();
        for (unsigned int i = 0; i < arr->size(); i++) {
            Array<KIPlayer *> *en = this->level->getEnemies();
            if ((*en)[i] != nullptr) {
                AEGeometry *geom = (*this->level->getEnemies())[i]->geometry;
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(geom->transform);
                t->Update((long long) (int32_t) this->frameDelta, false);
            }
            arr = this->level->getEnemies();
        }

        int race = Status::gStatus->getSystem()->getRace();
        if (race == 0) {
            AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom30->transform);
            t->Update((long long) (int32_t) this->frameDelta, false);
        } else {
            int race1 = Status::gStatus->getSystem()->getRace();
            if (race1 == 1) {
                canvas->FogSetParameter(AbyssEngine::FogMode_linear, 0.0f, 5000.0f, 1.0f,
                                        0x011e0cffu);
                canvas->FogEnable(1, AbyssEngine::FogMode_1);
                Array<KIPlayer *> *enemies = this->level->getEnemies();
                unsigned int count = this->level->getEnemies()->size();
                AEGeometry *thirdFromLast = (*enemies)[count - 3]->geometry;
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(thirdFromLast->transform);
                t->Update((long long) (int32_t) this->frameDelta, false);
                if (this->geom30 != nullptr) {
                    AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom30->transform);
                    t2->Update((long long) (int32_t) this->frameDelta, false);
                    int acc = this->frameDelta + this->animTimer7c;
                    this->animTimer7c = acc;
                    if (acc > 20000) {
                        this->animTimer7c = 0;
                        if (AERandom::gRandom->nextInt(100) < 100) {
                            AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom30->transform);
                            a->SetAnimationState((AnimationMode) 3, nullptr);
                            a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom30->transform);
                            a->SetAnimationState((AnimationMode) 1, nullptr);
                        }
                    }
                }
                if (this->geom34 != nullptr) {
                    AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom34->transform);
                    t2->Update((long long) (int32_t) this->frameDelta, false);
                    int acc = this->frameDelta + this->animTimer80;
                    this->animTimer80 = acc;
                    if (acc > 22000) {
                        this->animTimer80 = 0;
                        if (AERandom::gRandom->nextInt(100) < 100) {
                            AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom34->transform);
                            a->SetAnimationState((AnimationMode) 3, nullptr);
                            a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom34->transform);
                            a->SetAnimationState((AnimationMode) 1, nullptr);
                            return;
                        }
                    }
                }
                return;
            }
            int race3 = Status::gStatus->getSystem()->getRace();
            if (race3 == 3) {
                Array<KIPlayer *> *enemies = this->level->getEnemies();
                unsigned int count = this->level->getEnemies()->size();
                AEGeometry *thirdFromLast = (*enemies)[count - 3]->geometry;
                AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(thirdFromLast->transform);
                t->Update((long long) (int32_t) this->frameDelta, false);
                if (this->geom28 != nullptr) {
                    AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom28->transform);
                    t2->Update((long long) (int32_t) this->frameDelta, false);
                }
                AbyssEngine::Transform *t3 = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom2c->transform);
                t3->Update((long long) (int32_t) this->frameDelta, false);
                int acc7c = this->animTimer7c + this->frameDelta;
                int acc80 = this->frameDelta + this->animTimer80;
                this->animTimer7c = acc7c;
                this->animTimer80 = acc80;
                if (acc7c > 1000 && this->geom28 != nullptr) {
                    this->animTimer7c = 0;
                    if (AERandom::gRandom->nextInt(100) < 40) {
                        AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom28->transform);
                        a->SetAnimationState((AnimationMode) 3, nullptr);
                        a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom28->transform);
                        a->SetAnimationState((AnimationMode) 1, nullptr);
                    }
                    acc80 = this->animTimer80;
                }
                if (acc80 > 2000) {
                    this->animTimer80 = 0;
                    if (AERandom::gRandom->nextInt(100) < 30) {
                        AbyssEngine::Transform *a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom2c->transform);
                        a->SetAnimationState((AnimationMode) 3, nullptr);
                        a = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom2c->transform);
                        a->SetAnimationState((AnimationMode) 1, nullptr);
                        return;
                    }
                }
                return;
            }
            if (Status::gStatus->getSystem()->getRace() != 2)
                return;
            Array<KIPlayer *> *enemies = this->level->getEnemies();
            unsigned int count = this->level->getEnemies()->size();
            AEGeometry *penultimate = (*enemies)[count - 2]->geometry;
            AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(penultimate->transform);
            t->Update((long long) (int32_t) this->frameDelta, false);
            enemies = this->level->getEnemies();
            count = this->level->getEnemies()->size();
            AEGeometry *last = (*enemies)[count - 1]->geometry;
            AbyssEngine::Transform *t2 = (AbyssEngine::Transform *) canvas->TransformGetTransform(last->transform);
            t2->Update((long long) (int32_t) this->frameDelta, false);
        }
    }
}

void CutScene::renderBG() {
    if (this->level != nullptr)
        this->level->renderBG((int) this->frameDelta);
}

void CutScene::replacePlayerShip(int shipIndex, int variant) {
    if (this->level->getEnemies() == nullptr)
        return;

    Array<KIPlayer *> *enemies = this->level->getEnemies();

    AEGeometry *oldGeom = (*enemies)[0]->geometry;
    if (oldGeom != nullptr) {
        if (this->turretGeom != nullptr) {
            PaintCanvas *canvas = PaintCanvas::gCanvas;
            Array<KIPlayer *> *en2 = this->level->getEnemies();
            AEGeometry *lead = (*en2)[0]->geometry;
            canvas->TransformRemoveChild(lead->transform, this->turretGeom->transform);
        }

        char matrix[0x3c];
        memcpy(matrix, &oldGeom->getMatrix(), 0x3c);

        AEGeometry *grp = Globals::gGlobals->getShipGroup(shipIndex, variant, false);

        Array<KIPlayer *> *en3 = this->level->getEnemies();
        (*en3)[0]->geometry = grp;

        Array<KIPlayer *> *en4 = this->level->getEnemies();

        (*en4)[0]->geometry->setMatrix(*(const Matrix *) matrix);

        Array<KIPlayer *> *en5 = this->level->getEnemies();
        KIPlayer *ship = (*en5)[0];
        float bank = static_cast<float>(CutScene_shipVerticalOffsets[shipIndex]);

        Vector position{0.0f, bank, 0.0f};
        ship->setPosition(position);

        Array<KIPlayer *> *en6 = this->level->getEnemies();
        ((PlayerFighter *) ((*en6)[0]))->setExhaustVisible(false);

        this->level->lodManager->removeObject(oldGeom);
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

    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (this->mode == 2) {
        canvas->CameraCreate(this->cameraId74);
        canvas->CameraSetPerspective(this->cameraId74, 0.92f, 200.0f, 200000.0f);
        canvas->CameraSetCurrent(this->cameraId74);
        Matrix cameraLocal = *static_cast<const Matrix *>(canvas->CameraGetLocal(this->cameraId74));

        AERandom *rng = AERandom::gRandom;
        int rx = rng->nextInt(20000);
        int ry = rng->nextInt(60000);
        AbyssEngine::AEMath::MatrixSetTranslation(cameraLocal, (float) (rx - 20000),
                                                   0.0f, (float) (ry + 40000));
        this->cameraRotX = -0.7853982f;
        AbyssEngine::AEMath::MatrixSetRotation(cameraLocal, 0.0f, this->cameraRotX, 0.0f);
        canvas->CameraSetLocal(this->cameraId74, cameraLocal);

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
        canvas->CameraSetPerspective(this->cameraId70, 0.8f, 200.0f, 100000.0f);
        canvas->CameraSetCurrent(this->cameraId70);
        Matrix cameraLocal = *static_cast<const Matrix *>(canvas->CameraGetLocal(this->cameraId70));
        AbyssEngine::AEMath::MatrixSetRotation(cameraLocal, 0.0f, 0.3926991f, 0.0f);
        AbyssEngine::AEMath::MatrixSetTranslation(cameraLocal, 300.0f, 700.0f, 3000.0f);
        canvas->CameraSetLocal(this->cameraId70, cameraLocal);
        canvas->TransformCreate(this->transformId78);
        Vector leadPosition = (*this->level->getEnemies())[0]->geometry->getPosition();
        Matrix *transformLocal = static_cast<Matrix *>(canvas->TransformGetLocal(this->transformId78));
        AbyssEngine::AEMath::MatrixSetTranslation(*transformLocal, leadPosition.x,
                                                   leadPosition.y, leadPosition.z);
        canvas->TransformAddChild(this->transformId78, this->cameraId70);
        this->resetCamera();
        this->checkForTurret();
    } else if (this->mode == 4) {
        canvas->CameraCreate(this->cameraId6c);
        canvas->CameraSetPerspective(this->cameraId6c, 1.2f, 200.0f, 48000.0f);
        if (this->followCamera != nullptr) {
            delete this->followCamera;
            this->followCamera = nullptr;
        }
        canvas->CameraSetCurrent(this->cameraId6c);
        this->resetCamera();
        int race = Status::gStatus->getSystem()->getRace();
        if (race == 3) {
            this->geom2c = new AEGeometry((uint16_t) 0x36d6, canvas, false);
            AbyssEngine::Transform *t = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geom2c->transform);
            t->SetAnimationState((AnimationMode) 0, nullptr);
        } else {
            if (Status::gStatus->getSystem()->getRace() == 0) {
                this->geom30 = new AEGeometry((uint16_t) 0x37c8, canvas, false);
                this->geom34 = new AEGeometry((uint16_t) 0x37c7, canvas, false);
                this->geom30->addChild(this->geom34->transform);
                delete this->geom34;
                this->geom34 = nullptr;
            } else {
                Status::gStatus->getSystem()->getRace();
            }
        }
    }

    this->rootGeom = new AEGeometry(canvas);
    this->rootGeom->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);

    this->accum = 0;
    this->animTimer7c = 0;
    this->animTimer80 = 0;
    this->fogTimer84 = 0;

    uint64_t now = ApplicationManager::gAppManager->GetCurrentTimeMillis();
    this->initialized = 1;
    this->renderAtTime = now;
    this->prevTime = now;
}

void CutScene::resetCamera() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;

    if (this->mode == 0x17) {
        if (Status::gStatus->getSystem()->getRace() == 1) {
            canvas->FogSetParameter(AbyssEngine::FogMode_linear, 0.0f, 30000.0f, 1.0f, 0x011e0cffu);
            canvas->FogEnable(1, AbyssEngine::FogMode_1);
        }
        canvas->CameraSetCurrent(this->cameraId70);
        canvas->CameraSetPerspective(this->cameraId70, 0.8f, 200.0f, 100000.0f);

        Array<KIPlayer *> *enemies = this->level->getEnemies();
        (*enemies)[0]->getType();
        return;
    }

    if (this->mode != 4)
        return;

    if (Status::gStatus->getSystem()->getRace() == 1) {
        canvas->FogSetParameter(AbyssEngine::FogMode_linear, 0.0f, 5000.0f, 1.0f, 0x011e0cffu);
        canvas->FogEnable(1, AbyssEngine::FogMode_1);
    }
    canvas->CameraSetCurrent(this->cameraId6c);
    canvas->CameraSetPerspective(this->cameraId6c, 1.2f, 200.0f, 48000.0f);
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
