#include "game/weapons/ObjectGun.h"

#include <cstddef>

#include "game/world/Level.h"
#include "engine/math/Transform.h"
#include "engine/math/AEMath.h"
#include "game/mission/Explosion.h"
#include "game/ship/PlayerEgo.h"
#include "game/weapons/Gun.h"
#include "game/core/Globals.h"
#include "game/ship/Player.h"
#include "engine/render/AEGeometry.h"


// Android 2.0.16 .rodata at 0x203398: one signed ID table for both the
// negative sentinel check and the 16-bit geometry ID passed to AEGeometry.
static constexpr int kObjectGunMeshIds[232] = {
    14500, 14500, 14500, 14501, 14501, 14502, 14502, 14503,
    14503, 14500, 14503, 14502, 14501, 14501, 14501, 14501,
    14504, 14504, 14504, 14505, 14505, 14505, 14506, 14506,
    14507, 14507, 14508, 14508, 14506, 14505, 14502, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 14503,
    14505, 14501, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    14506, 14503, 14502, -1, 14509, 14510, 14506, 14503,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 14500, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    14505, -1, -1, -1, 14501, 14501, 14508, 14502,
};
static_assert(kObjectGunMeshIds[31] == -1 && kObjectGunMeshIds[176] == 14506 &&
              kObjectGunMeshIds[231] == 14502, "ObjectGun mesh table anchors");

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

ObjectGun::ObjectGun(int /*unused*/, Gun *gun, int mesh, uint32_t /*param*/, Level *level) {
    this->dir.x = 0.0f;
    this->dir.y = 0.0f;
    this->dir.z = 0.0f;
    this->up.y = 0.0f;
    this->up.z = 0.0f;
    this->side.x = 0.0f;
    this->side.z = 0.0f;
    this->orientation = AbyssEngine::AEMath::Matrix();

    PaintCanvas **canvas = &Globals::Canvas;
    this->useEgoOrientation = 0;
    this->explosions = nullptr;
    this->explosionReady = nullptr;
    this->unusedSlot = -1;
    this->gun = gun;
    this->level = level;
    this->secondaryTransform = -1;
    (*canvas)->TransformCreate(this->transform);
    this->meshId = mesh;
    (*canvas)->TransformAddMesh(this->transform, (uint16_t) mesh, false);
    this->rollAngle = 0.0f;
    this->scaleX = 1.0f;
    this->scaleY = 1.0f;
    this->scaleZ = 1.0f;
    this->spinAngle = 0.0f;

    int type = gun->weaponType;
    this->visible = (type <= 8 && ((1u << type) & 0x10aU) != 0) ? 1 : 0;

    if (Globals::iPad != 0) {
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
        if (gun->isPlayerGun() == 0 || kObjectGunMeshIds[gun->itemIndex] < 0) {
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
        geometry = new AEGeometry((uint16_t) kObjectGunMeshIds[gun->itemIndex],
                                  *canvas, false);

    this->wasFiring = 0;
    this->geometry = geometry;
    this->deltaTime = 0;
}

ObjectGun::~ObjectGun() {
    delete this->geometry;
    this->geometry = nullptr;

    if (this->explosions != nullptr) {
        ArrayReleaseClasses(*this->explosions);
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
    PaintCanvas **canvas = &Globals::Canvas;
    (*canvas)->TransformRemoveMesh(this->transform, (uint16_t) this->meshId);
    this->meshId = (int) mesh;
    (*canvas)->TransformAddMesh(this->transform, (uint16_t) mesh, false);
}

void ObjectGun::setEnemies(Array<Player *> *enemies) {
    this->gun->setEnemies(enemies);
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

    PaintCanvas **canvas = &Globals::Canvas;
    auto *transform = static_cast<AbyssEngine::Transform *>(
        (*canvas)->TransformGetTransform(this->transform));
    transform->Update((int64_t) dt, false);

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
                        (uint16_t) kObjectGunMeshIds[this->gun->itemIndex],
                        *canvas, false);
                }
            }
        }
        if (this->hasGeometry == 0)
            goto after_geometry;
    }

    gun = this->gun;
    if (gun->delayActive == 0) {
        static_cast<AbyssEngine::Transform *>(
            (*canvas)->TransformGetTransform(this->geometry->transform))
            ->SetAnimationState((AbyssEngine::AnimationMode) 0, nullptr);
        static_cast<AbyssEngine::Transform *>(
            (*canvas)->TransformGetTransform(this->geometry->transform))
            ->SetAnimationState((AbyssEngine::AnimationMode) 3, nullptr);
        static_cast<AbyssEngine::Transform *>(
            (*canvas)->TransformGetTransform(this->geometry->transform))
            ->SetAnimationState((AbyssEngine::AnimationMode) 1, nullptr);
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
        position += AbyssEngine::AEMath::MatrixRotateVector(playerMatrix, offsets);
        this->geometry->setPosition(position);

        transform = static_cast<AbyssEngine::Transform *>(
            (*canvas)->TransformGetTransform(this->geometry->transform));
        transform->Update((int64_t) dt, false);

        if (this->gun->weaponType != ITEM_SORT_TURRET) {
            PaintCanvas *paint = *canvas;
            unsigned camera = paint->CameraGetCurrent();
            cameraMatrix = *static_cast<const Matrix *>(paint->CameraGetLocal(camera));
            dir = AbyssEngine::AEMath::MatrixGetDir(playerMatrix);
            up = AbyssEngine::AEMath::MatrixGetUp(cameraMatrix);
            this->geometry->setDirection(dir, up);
            goto after_geometry;
        }

        PlayerEgo *ego = (PlayerEgo *) player;
        // ARM 0x15fb16-0x15fb2a: multiply geometry matrices at ego +0x2c, then +0x28.
        workMatrix = AbyssEngine::AEMath::operator*(
            static_cast<AEGeometry *>(ego->field_0x2c)->getMatrix(),
            static_cast<AEGeometry *>(ego->turretGeometry)->getMatrix());

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
        Vector rotatedOffset = AbyssEngine::AEMath::MatrixRotateVector(workMatrix, offsets);
        this->geometry->setMatrix(workMatrix);
        this->geometry->translate(rotatedOffset);
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
            PaintCanvas *paint = Globals::Canvas;
            unsigned camera = paint->CameraGetCurrent();
            cameraLocal = *static_cast<const Matrix *>(paint->CameraGetLocal(camera));
            if (this->visible != 0) {
                for (uint32_t mi = 0; mi < 15; ++mi)
                    rotate.m[mi] = 0.0f;
                rotate.m[0] = 1.0f;
                rotate.m[5] = 1.0f;
                rotate.m[14] = 1.0f;
                scaleMatrix = AbyssEngine::AEMath::MatrixSetRotation(rotate, this->spinAngle, 0.0f, 0.0f);
                rotate = scaleMatrix;
                cameraLocal *= rotate;
            }
        }

        uint32_t inactive = 0;
        for (uint32_t i = 0; i < gun->count; ++i) {
            Vector *gunPos = &((Vector *) gun->positions)[i];
            if (gunPos->x != -1000.0f) {
                AbyssEngine::AEMath::MatrixSetTranslation(this->orientation, gunPos->x, gunPos->y, gunPos->z);
                this->dir = AbyssEngine::AEMath::VectorNormalize(((Vector *) gun->velocities)[i]);

                if (this->visible != 0) {
                    muzzle.x = -this->dir.x;
                    muzzle.y = -this->dir.y;
                    muzzle.z = -this->dir.z;
                    if ((uint32_t)(gun->itemIndex - 0xb4) > 2) {
                        PaintCanvas *paint = Globals::Canvas;
                        unsigned camera = paint->CameraGetCurrent();
                        cameraLocal = *static_cast<const Matrix *>(paint->CameraGetLocal(camera));
                        dir = AbyssEngine::AEMath::MatrixGetDir(cameraLocal);
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
                        AbyssEngine::AEMath::MatrixSetScaling(this->orientation, fscale, fscale, fscale);
                    } else if (gun->hitSmall != 0) {
                        AbyssEngine::AEMath::MatrixSetScaling(this->orientation, 1.0f, 1.0f, 1.0f);
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
                    side = AbyssEngine::AEMath::VectorCross(this->up, this->dir);
                    this->side = AbyssEngine::AEMath::VectorNormalize(side);
                    side = AbyssEngine::AEMath::VectorCross(this->side, this->dir);
                    this->up = AbyssEngine::AEMath::VectorNormalize(side);

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
                        AbyssEngine::AEMath::MatrixSetScaling(this->orientation, fscale, fscale, fscale);
                    } else if (gun->hitSmall != 0) {
                        AbyssEngine::AEMath::MatrixSetScaling(this->orientation, 1.0f, 1.0f, 1.0f);
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

                    scaleMatrix = AbyssEngine::AEMath::MatrixSetRotation(local, player->empPointsF, player->maxEmpPointsF, 0.0f);
                    playerView->orientation *= local;
                    this->orientation = playerView->orientation;
                    scaleMatrix = AbyssEngine::AEMath::MatrixSetTranslation(this->orientation, gunPos->x, gunPos->y, gunPos->z);
                    dir = AbyssEngine::AEMath::MatrixGetDir(this->orientation);
                    muzzle = AbyssEngine::AEMath::VectorNormalize(dir);
                    muzzle *= gun->field_0x50;
                    ((Vector *) gun->velocities)[i] = muzzle;
                    player->empPointsF = 0.0f;
                    player->maxEmpPointsF = 0.0f;
                    scaleMatrix = AbyssEngine::AEMath::MatrixSetRotation(local, this->rollAngle, 0.0f, 0.0f);
                    Globals::Canvas->TransformSetLocal(this->secondaryTransform, scaleMatrix);
                }

                if (Globals::iPad != 0)
                    AbyssEngine::AEMath::MatrixSetScaling(this->orientation, this->scaleX, this->scaleY, this->scaleZ);

                if (this->gun->weaponType == ITEM_SORT_MINE) {
                    Array<Vector *> *spins = (Array<Vector *> *) this->gun->wobbleOffsets;
                    Vector *spin = spins->data()[i];
                    if (spin != nullptr && this->deltaTime > 0) {
                        AbyssEngine::AEMath::MatrixSetRotation(this->orientation, spin->x, spin->y, spin->z);
                        float step = (float) this->deltaTime * 0.02f;
                        float neg = -step;
                        spin->x += spin->x < 0.0f ? neg : step;
                        spin->y += spin->y < 0.0f ? neg : step;
                        spin->z += spin->z < 0.0f ? neg : step;
                    }
                    AbyssEngine::AEMath::MatrixSetScaling(this->orientation, this->scaleX, this->scaleY, this->scaleZ);
                }

                PaintCanvas **canvas = &Globals::Canvas;
                (*canvas)->TransformSetLocal(this->transform, this->orientation);
                (*canvas)->DrawTransform(this->transform, nullptr);
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
