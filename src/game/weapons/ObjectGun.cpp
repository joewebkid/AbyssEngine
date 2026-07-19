#include "game/weapons/ObjectGun.h"

#include <cstddef>

#include "game/world/Level.h"
#include "engine/math/Transform.h"
#include "game/mission/Explosion.h"
#include "game/ship/PlayerEgo.h"
#include "game/weapons/Gun.h"
#include "game/ship/Player.h"
#include "engine/render/AEGeometry.h"


struct MeshId {
    uint16_t id;
    uint16_t pad;
};

// View used by the ego-orientation branch of render(): in the shipped binary the
// player object stores an orientation Matrix starting at byte 0x40 (the same
// storage Player.h names `radius`/`destroyed`/...). Model it with named fields so
// the access is a struct member rather than raw pointer arithmetic.
struct EgoPlayerView {
    char header[0x40];
    Matrix orientation;        // at 0x40
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(EgoPlayerView, orientation) == 0x40,
              "EgoPlayerView.orientation must sit at 0x40");
#endif

namespace AbyssEngine {
    namespace AEMath {
        void MatrixMultiply(const Matrix &, const Matrix &);
    }
}

static void *g_PaintCanvas = nullptr;

void TransformRemoveMesh(void *canvas, uint32_t transform, uint16_t mesh);

void TransformAddMesh(void *canvas, uint32_t transform, uint16_t mesh, int flags);

void TransformCreate(void *canvas, uint32_t *transform);

uint32_t TransformGetTransform(void *canvas, uint32_t transform);

void TransformSetLocal(void *canvas, uint32_t transform, Matrix *matrix);

void DrawTransform(void *canvas, uint32_t transform, int flags);

void MatrixRotateVector(void *out, const void *matrix, const void *vec);

void *CameraGetCurrent(void *canvas);

void *CameraGetLocal(void *canvas, void *camera);

void MatrixGetDir(Vector *out, const Matrix *matrix);

void MatrixGetUp(Vector *out, const Matrix *matrix);

void MatrixSetRotation(Matrix *dst, float x, float y, float z);

void MatrixSetTranslation(Matrix *dst, float x, float y, float z);

void MatrixSetScaling(Matrix *dst, float x, float y, float z);

void VectorNormalize(Vector *out, const Vector *in);

void VectorCross(Vector *out, const Vector *a, const Vector *b);

void ObjectGun_setEnemies_impl(void *items);

static void *g_ObjectGunScaleFlag = nullptr;
static void *g_ObjectGunRenderScaleFlag = nullptr;
static MeshId g_ObjectGunGeometryIds[256] = {};
static int g_ObjectGunPlayerGunIds[256] = {};

static uint32_t (*g_TransformGetObject)(void *canvas, uint32_t mesh) = nullptr;

static void (*g_TransformSetState)(uint32_t object, int state, int value) = nullptr;

ObjectGun::ObjectGun(int /*unused*/, Gun *gun, int mesh, uint32_t /*param*/, Level *level) {
    this->dir.x = 0.0f;
    this->dir.y = 0.0f;
    this->dir.z = 0.0f;
    this->up.y = 0.0f;
    this->up.z = 0.0f;
    this->side.x = 0.0f;
    this->side.z = 0.0f;
    this->orientation = AbyssEngine::AEMath::Matrix();

    void **canvas = (void **) g_PaintCanvas;
    this->useEgoOrientation = 0;
    this->explosions = nullptr;
    this->explosionReady = nullptr;
    this->unusedSlot = -1;
    this->gun = gun;
    this->level = level;
    this->secondaryTransform = -1;
    TransformCreate(*canvas, &this->transform);
    this->meshId = mesh;
    TransformAddMesh(*canvas, this->transform, (uint16_t) mesh, 0);
    this->rollAngle = 0.0f;
    this->scaleX = 1.0f;
    this->scaleY = 1.0f;
    this->scaleZ = 1.0f;
    this->spinAngle = 0.0f;

    int type = gun->weaponType;
    this->visible = (type <= 8 && ((1u << type) & 0x10aU) != 0) ? 1 : 0;

    if (*(uint8_t *) g_ObjectGunScaleFlag != 0) {
        if (type == 1 || type == 3 || type == 8) {
            this->scaleX = 0.6f;
            this->scaleY = 0.6f;
            this->scaleZ = 0.6f;
        }
    }
    if (type == 0xb) {
        this->scaleX = 0.7f;
        this->scaleY = 0.7f;
        this->scaleZ = 0.7f;
    } else if (type == 0x19) {
        this->explosions = new Array<Explosion *>();
        ArraySetLength(gun->count, *(this->explosions));
        this->explosionReady = new uint8_t[this->explosions->size()];

        for (uint32_t i = 0; i < this->explosions->size(); ++i) {
            int explosionType = 10;
            if (gun->itemIndex == 0xb1)
                explosionType = 9;
            if (gun->itemIndex == 0xb0)
                explosionType = 8;
            Explosion *explosion = new Explosion(explosionType);
            this->explosions->data()[i] = explosion;
            explosion->setWeaponIndex(gun->itemIndex);
            this->explosionReady[i] = 1;
        }
    }

    AEGeometry *geometry = nullptr;
    bool createGeometry = true;
    if (gun->field_0xa8 == 0) {
        if (gun->isPlayerGun() == 0 || g_ObjectGunPlayerGunIds[gun->itemIndex] < 0) {
            this->hasGeometry = 0;
            createGeometry = false;
        } else {
            this->hasGeometry = (gun->weaponType != ITEM_SORT_MINE) ? 1 : 0;
            createGeometry = this->hasGeometry != 0;
        }
    } else {
        this->hasGeometry = 1;
    }

    if (createGeometry)
        geometry = new AEGeometry((uint16_t) g_ObjectGunGeometryIds[gun->itemIndex].id,
                                  (PaintCanvas *) *canvas, false);

    this->wasFiring = 0;
    this->geometry = geometry;
    this->deltaTime = 0;
}

ObjectGun::~ObjectGun() {
    delete this->geometry;
    this->geometry = nullptr;

    if (this->explosions != nullptr) {
        ArrayReleaseClasses(*this->explosions); ArrayRemoveAll(*(this->explosions));
        delete this->explosions;
        this->explosions = nullptr;
    }

    delete[] this->explosionReady;
    this->explosionReady = nullptr;
}

void ObjectGun::setScaling(int x, int y, int /*z*/) {
    this->scaleX = (float) x;
    this->scaleY = (float) y;
}

void ObjectGun::replaceGun(unsigned int mesh, int /*unused*/) {
    void **canvas = (void **) g_PaintCanvas;
    TransformRemoveMesh(*canvas, this->transform, this->meshId);
    this->meshId = (int) mesh;
    TransformAddMesh(*canvas, this->transform, (uint16_t) mesh, 0);
}

void ObjectGun::setEnemies(Array<Player *> *enemies) {
    ObjectGun_setEnemies_impl((void *) enemies->data());
}

void ObjectGun::setEnemy(Player * /*enemy*/) {
}

void ObjectGun::translate(const Vector & /*v*/) {
}

void ObjectGun::update(int dt) {
    Matrix playerMatrix;
    Matrix workMatrix;
    Matrix cameraMatrix;
    Vector position;
    Vector dir;
    Vector up;
    Vector offsets;
    Vector zero = {0.0f, 0.0f, 0.0f};

    void **canvas = (void **) g_PaintCanvas;
    uint32_t transform = TransformGetTransform(*canvas, this->transform);
    ((AbyssEngine::Transform *) ((uint64_t) transform))->Update((int64_t) dt, 0);

    this->deltaTime = dt;
    Gun *gun = this->gun;
    gun->update(dt);

    if (this->hasGeometry == 0) {
        if (gun->isPlayerGun() == 0) {
            Player *owner = gun->owner;
            if (owner != nullptr) {
                KIPlayer *ki = owner->getKIPlayer();

                if (ki != nullptr && ki->field_0x3f_b != 0) {
                    this->hasGeometry = 1;
                    this->geometry = new AEGeometry(
                        (uint16_t) g_ObjectGunGeometryIds[this->gun->itemIndex].id,
                        (PaintCanvas *) *canvas, false);
                }
            }
        }
        if (this->hasGeometry == 0)
            goto after_geometry;
    }

    gun = this->gun;
    if (gun->delayActive == 0) {
        g_TransformSetState(g_TransformGetObject(*canvas, this->geometry->transform), 0, 0);
        g_TransformSetState(g_TransformGetObject(*canvas, this->geometry->transform), 3, 0);
        g_TransformSetState(g_TransformGetObject(*canvas, this->geometry->transform), 1, 0);
        goto after_geometry;
    }

    {
        Player *player = (Player *) (uint64_t) this->level->getPlayer();
        if (gun->isPlayerGun() != 0) {
            position = ((PlayerEgo *) player)->getPosition();
        } else {
            position = gun->owner->getPosition();
        }

        gun = this->gun;
        // The matrix source is a Player object: for the player's own gun it is the
        // PlayerEgo's wrapped Player (first pointer field), otherwise the gun owner.
        Player *matrixPlayer;
        if (gun->isPlayerGun() == 0)
            matrixPlayer = gun->owner;
        else
            matrixPlayer = (Player *) ((PlayerEgo *) player)->player;

        memcpy(&playerMatrix, matrixPlayer->transform, 0x3c);

        offsets.x = gun->offset.x;
        offsets.y = gun->offset.y;
        offsets.z = gun->offset.z + 8.0f;
        MatrixRotateVector(&workMatrix, &playerMatrix, &offsets);
        position += *(const Vector *) &workMatrix;
        this->geometry->setPosition(position);

        transform = TransformGetTransform(*canvas, this->geometry->transform);
        ((AbyssEngine::Transform *) ((uint64_t) transform))->Update((int64_t) dt, 0);

        if (this->gun->weaponType != ITEM_SORT_TURRET) {
            void *paint = *canvas;
            void *camera = CameraGetCurrent(paint);
            cameraMatrix = *(const Matrix *) CameraGetLocal(paint, camera);
            MatrixGetDir(&dir, &cameraMatrix);
            MatrixGetUp(&up, &cameraMatrix);
            this->geometry->setDirection(dir, up);
            goto after_geometry;
        }

        AbyssEngine::AEMath::MatrixMultiply(workMatrix, this->geometry->getMatrix());

        gun = this->gun;
        int weapon = gun->itemIndex;
        offsets.x = 0.0f;
        offsets.y = 0.0f;
        offsets.z = -3.5f;
        if (weapon == 0xb5) {
            offsets.z = -1.5f;
        } else if (weapon == 0x31) {
            offsets.z = 7.0f;
        } else if (weapon == 0xb4) {
            offsets.z = -6.0f;
        } else if (weapon == 0x30) {
            offsets.z = 15.0f;
        } else if (weapon == 0xe0) {
            offsets.z = -13.0f;
        } else if (weapon == 0xb6) {
            offsets.z = 13.0f;
        }

        if (gun->field_0xa4 != 0) {
            if (weapon == 0xb5) {
                offsets.x = gun->field_0xa6 != 0 ? -1.5f : 1.5f;
            } else {
                float left = weapon == 0x30 ? 20.0f : 15.0f;
                float right = weapon == 0x30 ? -20.0f : -15.0f;
                offsets.x = left - gun->offset.x;
                if (gun->field_0xa6 != 0)
                    offsets.x = gun->offset.x + right;
            }
        }
        if (gun->field_0xa5 != 0) {
            if (weapon == 0xe0) {
                offsets.y = gun->field_0xa7 == 0 ? 13.0f : -13.0f;
            } else if (weapon == 0xb5) {
                offsets.y = gun->field_0xa7 == 0 ? 2.5f : -2.5f;
            }
        }
        MatrixRotateVector(&cameraMatrix, &workMatrix, &offsets);
        this->geometry->setMatrix(workMatrix);
        this->geometry->translate(*(Vector *) &cameraMatrix);
    }

after_geometry:
    gun = this->gun;
    this->wasFiring = gun->delayActive;
    if (gun->weaponType == ITEM_SORT_SCATTER_GUN) {
        for (uint32_t i = 0; i < gun->count; ++i) {
            if (gun->hitFlags[i] != 0) {
                Explosion *explosion = this->explosions->data()[i];
                if (this->explosionReady[i] != 0) {
                    explosion->start(((Vector *) gun->hitPositions)[i], zero);
                    this->explosionReady[i] = 0;
                }
                explosion->update(dt, static_cast<TargetFollowCamera *>(nullptr));
                if (explosion->isPlaying() == 0) {
                    gun = this->gun;
                    gun->hitFlags[i] = 0;
                    gun->ignited = 0;
                    this->explosionReady[i] = 1;
                    explosion->reset();
                }
            }
            gun = this->gun;
        }
    }
}

void ObjectGun::render() {
    Matrix local;
    Matrix cameraLocal;
    Matrix rotate;
    Matrix scaleMatrix;
    Vector muzzle;
    Vector dir;
    Vector up;
    Vector side;

    Gun *gun = this->gun;
    gun->render();

    if (gun->weaponType == ITEM_SORT_SCATTER_GUN) {
        for (uint32_t i = 0; i < gun->count; ++i) {
            if (gun->hitFlags[i] != 0) {
                this->explosions->data()[i]->render();
                gun = this->gun;
            }
        }
    }

    if (gun->active != 0) {
        for (uint32_t i = 0; i < 15; ++i)
            cameraLocal.m[i] = 0.0f;
        cameraLocal.m[0] = 1.0f;
        cameraLocal.m[5] = 1.0f;
        cameraLocal.m[14] = 1.0f;
        if (gun->weaponType == ITEM_SORT_TURRET) {
            void **canvas = (void **) g_PaintCanvas;
            void *paint = *canvas;
            void *camera = CameraGetCurrent(paint);
            cameraLocal = *(const Matrix *) CameraGetLocal(paint, camera);
            if (this->visible != 0) {
                for (uint32_t mi = 0; mi < 15; ++mi)
                    rotate.m[mi] = 0.0f;
                rotate.m[0] = 1.0f;
                rotate.m[5] = 1.0f;
                rotate.m[14] = 1.0f;
                MatrixSetRotation(&scaleMatrix, this->spinAngle, 0.0f, 0.0f);
                rotate = scaleMatrix;
                AbyssEngine::AEMath::MatrixMultiply(cameraLocal, rotate);
            }
        }

        uint32_t inactive = 0;
        for (uint32_t i = 0; i < gun->count; ++i) {
            Vector *gunPos = &((Vector *) gun->positions)[i];
            if (gunPos->x != -1000.0f) {
                MatrixSetTranslation(&local, gunPos->x, gunPos->y, gunPos->z);
                ::VectorNormalize((Vector *) &local, &((Vector *) gun->velocities)[i]);
                this->dir = *(const Vector *) &local;

                if (this->visible != 0) {
                    muzzle.x = -this->dir.x;
                    muzzle.y = -this->dir.y;
                    muzzle.z = -this->dir.z;
                    if ((uint32_t)(gun->itemIndex - 0xb4) > 2) {
                        void **canvas = (void **) g_PaintCanvas;
                        void *paint = *canvas;
                        void *camera = CameraGetCurrent(paint);
                        cameraLocal = *(const Matrix *) CameraGetLocal(paint, camera);
                        MatrixGetDir(&dir, &cameraLocal);
                        muzzle = dir;
                        gun = this->gun;
                    }

                    this->orientation.m[0] = cameraLocal.m[0];
                    this->orientation.m[1] = cameraLocal.m[1];
                    this->orientation.m[4] = cameraLocal.m[4];
                    this->orientation.m[5] = cameraLocal.m[5];
                    this->orientation.m[8] = cameraLocal.m[10];
                    this->orientation.m[9] = cameraLocal.m[11];
                    this->orientation.m[2] = -muzzle.x;
                    this->orientation.m[6] = -muzzle.y;
                    this->orientation.m[10] = -muzzle.z;

                    int scale = gun->lifetimes[i];
                    if (scale < 1000) {
                        float fscale = (float) scale / 750.0f;
                        MatrixSetScaling(&local, fscale, fscale, fscale);
                    } else if (gun->hitSmall != 0) {
                        MatrixSetScaling(&local, 1.0f, 1.0f, 1.0f);
                    }
                } else if (this->useEgoOrientation == 0) {
                    Vector base;
                    if (this->level == nullptr || this->level->getPlayer() == 0 ||
                        gun->isPlayerGun() == 0) {
                        base.x = 0.0f;
                        base.y = 1.0f;
                        base.z = 0.0f;
                    } else {
                        base = ((const Vector *) gun->upVectors)[i];
                    }
                    this->up = base;
                    VectorCross(&side, &this->up, &this->dir);
                    ::VectorNormalize(&this->side, &side);
                    VectorCross(&side, &this->side, &this->dir);
                    ::VectorNormalize(&this->up, &side);

                    this->orientation.m[0] = this->side.x;
                    this->orientation.m[1] = this->up.x;
                    this->orientation.m[2] = this->dir.x;
                    this->orientation.m[4] = this->side.y;
                    this->orientation.m[5] = this->up.y;
                    this->orientation.m[6] = this->dir.y;
                    this->orientation.m[8] = this->side.z;
                    this->orientation.m[9] = this->up.z;
                    this->orientation.m[10] = this->dir.z;

                    int scale = gun->lifetimes[i];
                    if (scale < 1000) {
                        float fscale = (float) scale / 750.0f;
                        MatrixSetScaling(&local, fscale, fscale, fscale);
                    } else if (gun->hitSmall != 0) {
                        MatrixSetScaling(&local, 1.0f, 1.0f, 1.0f);
                        if (this->isRocketGun() == 0)
                            gun->hitSmall = 0;
                    }
                } else {
                    Player *player = (Player *) (uint64_t) this->level->getPlayer();
                    EgoPlayerView *playerView = (EgoPlayerView *) player;
                    for (uint32_t mi = 0; mi < 15; ++mi)
                        local.m[mi] = 0.0f;
                    local.m[0] = 1.0f;
                    local.m[5] = 1.0f;
                    local.m[14] = 1.0f;

                    MatrixSetRotation(&scaleMatrix, player->empPointsF, player->maxEmpPointsF, 0.0f);
                    AbyssEngine::AEMath::MatrixMultiply(playerView->orientation, local);
                    this->orientation = playerView->orientation;
                    MatrixSetTranslation(&scaleMatrix, gunPos->x, gunPos->y, gunPos->z);
                    MatrixGetDir(&dir, &scaleMatrix);
                    ::VectorNormalize(&muzzle, &dir);
                    muzzle *= gun->field_0x50;
                    ((Vector *) gun->velocities)[i] = muzzle;
                    player->empPointsF = 0.0f;
                    player->maxEmpPointsF = 0.0f;
                    MatrixSetRotation(&scaleMatrix, this->rollAngle, 0.0f, 0.0f);
                    TransformSetLocal(*(void **) g_PaintCanvas, this->secondaryTransform, &scaleMatrix);
                }

                if (*(uint8_t *) g_ObjectGunRenderScaleFlag != 0)
                    MatrixSetScaling(&local, this->scaleX, this->scaleY, this->scaleZ);

                if (this->gun->weaponType == ITEM_SORT_MINE) {
                    Array<Vector *> *spins = (Array<Vector *> *) this->gun->wobbleOffsets;
                    Vector *spin = spins->data()[i];
                    if (spin != nullptr && this->deltaTime > 0) {
                        MatrixSetRotation(&local, spin->x, spin->y, spin->z);
                        float step = (float) this->deltaTime * 0.02f;
                        float neg = -step;
                        spin->x += spin->x < 0.0f ? neg : step;
                        spin->y += spin->y < 0.0f ? neg : step;
                        spin->z += spin->z < 0.0f ? neg : step;
                    }
                    MatrixSetScaling(&local, this->scaleX, this->scaleY, this->scaleZ);
                }

                void **canvas = (void **) g_PaintCanvas;
                TransformSetLocal(*canvas, this->transform, &this->orientation);
                DrawTransform(*canvas, this->transform, 0);
            } else {
                ++inactive;
            }
            gun = this->gun;
        }
        if (gun->directionCount <= (int) inactive)
            gun->active = 0;
    }

    if (this->wasFiring != 0 && this->geometry != nullptr)
        this->geometry->render();
    this->deltaTime = 0;
}
