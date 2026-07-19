#include "game/weapons/BombGun.h"

#include "engine/render/AEGeometry.h"
#include "game/ship/Player.h"
#include "engine/math/Transform.h"
#include "engine/math/AEMath.h"
#include "game/world/LevelScript.h"
#include "engine/render/PaintCanvas.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/TargetFollowCamera.h"
#include "engine/audio/FModSound.h"
#include "game/weapons/RocketGun.h"
#include "game/weapons/Gun.h"
#include "game/mission/Status.h"
#include "game/mission/Explosion.h"

static PaintCanvas **g_PaintCanvas = nullptr;
static FModSound **g_FMod_singleton = nullptr;
static Status *g_mining_status = nullptr;

static inline PaintCanvas *activeCanvas() {
    return *g_PaintCanvas;
}

static inline FModSound *sound() {
    return *g_FMod_singleton;
}

static inline Status *gameStatus() {
    return g_mining_status;
}

BombGun::~BombGun() {
    delete this->explosion;
    this->explosion = nullptr;
}

int BombGun::isBombGun() {
    return 1;
}

BombGun::BombGun(Gun *gun, uint32_t meshId, int rocketArg, int bombType, bool simpleMesh,
                 Level *level)
    : RocketGun(rocketArg, gun, meshId, 0, 0, bombType, false, level) {
    this->detonationPosition = Vector{0.0f, 0.0f, 0.0f};
    this->cameraTargetOffset = Vector{0.0f, 0.0f, 0.0f};

    int explosionType;
    if (bombType == ITEM_SORT_EMP_BOMB) {
        explosionType = 7;
    } else if (bombType == ITEM_SORT_SHOCK_BLAST) {
        explosionType = 0xb;
    } else if (gun->itemIndex == 0xe8) {
        explosionType = 0xd;
    } else {
        explosionType = 0;
    }

    this->explosion = new Explosion(explosionType);
    this->explosion->setWeaponIndex(gun->itemIndex);

    this->playerControlled = simpleMesh ? 1 : 0;
    this->detonationPending = 1;
    this->bombType = bombType;
    this->geometryTransformId = 0xffffffff;

    if (bombType == ITEM_SORT_SHOCK_BLAST) {
        this->explosion->setScaling(50000.0f);
    }

    PaintCanvas *canvas = activeCanvas();

    if (!simpleMesh) {
        int item = gun->itemIndex;
        bool customWeapon = item != 0xe8;
        int sel = customWeapon ? gun->weaponType : item;
        if (customWeapon && sel != ITEM_SORT_IONIZING_MISSILE) {
            uint16_t geomMesh = 0x395d;
            if (meshId == 0x395a) geomMesh = 0x395b;
            if (meshId == 0x3958) geomMesh = 0x3959;

            AEGeometry *geo = new AEGeometry(geomMesh, canvas, false);

            this->geometryTransformId = geo->transform;
            canvas->TransformAddChild(this->transform, geo->transform);

            AbyssEngine::Transform *transform =
                    (AbyssEngine::Transform *) canvas->TransformGetTransform(geo->transform);
            AbyssEngine::AnimationMode mode =
                    (meshId == 0x395c) ? (AbyssEngine::AnimationMode) 2 : (AbyssEngine::AnimationMode) 1;
            transform->SetAnimationState(mode, nullptr);

            delete geo;
        }
    } else {
        canvas->TransformCreate(this->meshTransformId);
        canvas->TransformAddMesh(this->meshTransformId, this->meshId, false);

        AbyssEngine::Transform *transform =
                (AbyssEngine::Transform *) canvas->TransformGetTransform(this->meshTransformId);
        transform->SetAnimationState((AbyssEngine::AnimationMode) 1, nullptr);

        AEGeometry *geo = new AEGeometry((uint16_t) 0x37d6, canvas, false);
        canvas->TransformAddChild(this->meshTransformId, geo->transform);
        canvas->TransformRemoveMesh(this->transform, this->meshTransformId);
        canvas->TransformAddChild(this->transform, this->meshTransformId);

        delete geo;
    }

    this->cameraOffset = Vector{0.0f, 450.0f, -1400.0f};
    this->cameraTargetOffset = Vector{0.0f, 0.0f, 1700.0f};

    this->player = nullptr;

    this->trailGeometry = new AEGeometry(canvas);
}

void BombGun::setPlayer(PlayerEgo *player) {
    this->player = player;
}

void BombGun::render() {
    RocketGun::render();
    if (this->gun->ignited != 0)
        this->explosion->render();
}

void BombGun::update(int elapsed) {
    PlayerEgo *player = this->player;
    if (player == nullptr)
        return;

    Gun *gun = this->gun;
    if (gun->ignited == 0) {
        if (this->bombType == ITEM_SORT_SHOCK_BLAST) {
            this->detonationPosition = player->getPosition();
        } else {
            this->detonationPosition = *(const Vector *) gun->positions;
        }
    }

    gun = this->gun;
    bool steerable = this->playerControlled != 0;
    bool updateTransforms = false;

    if (!steerable) {
        updateTransforms = gun->active != 0;
    } else if (gun->active != 0) {
        if (gun->ignited != 0) {
            updateTransforms = true;
        } else {
            PaintCanvas *canvas = activeCanvas();

            if (gun->hitSmall != 0) {
                gun->hitSmall = 0;
                AbyssEngine::Transform *transform =
                        (AbyssEngine::Transform *) canvas->TransformGetTransform(this->transform);
                transform->SetAnimationState((AbyssEngine::AnimationMode) 3, nullptr);
                transform = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->transform);
                transform->SetAnimationState((AbyssEngine::AnimationMode) 1, nullptr);

                player->rocketReturnMatrix() = ((Player *) player->player)->transformMatrix;
                sound()->play(0x45c, nullptr, nullptr, 0.0f);
            }

            AEGeometry *geometry = this->trailGeometry;
            geometry->setMatrix(*(const Matrix *) canvas->TransformGetLocal(this->transform));
            Vector offset = 350.0f * *(const Vector *) gun->velocities;
            geometry->setPosition(*(const Vector *) gun->positions + offset);
            geometry->updateReferenceMatrix();

            TargetFollowCamera *camera =
                    (TargetFollowCamera *) (intptr_t) player->getTargetFollowCamera();
            camera->setTarget(geometry);
            camera->setCamOffset(this->cameraOffset);
            camera->setTargetOffset(this->cameraTargetOffset);
            camera->useTargetsUpVector(false);
            player->setRocketControl(gun, geometry);

            if (*(int *) gun->lifetimes < gun->initialLifetime - 500) {
                AbyssEngine::Transform *transform =
                        (AbyssEngine::Transform *) canvas->TransformGetTransform(this->transform);
                transform->Update((int64_t) elapsed, false);
            }

            float bank = player->getRocketBanking() * 0.2f;
            this->bankingAngle = bank;
            sound()->setParamValue(0, 0x45c, bank * 0.5f);
        }
    }

    if (updateTransforms) {
        PaintCanvas *canvas = activeCanvas();
        AbyssEngine::Transform *transform =
                (AbyssEngine::Transform *) canvas->TransformGetTransform(this->transform);
        transform->Update((int64_t) elapsed, false);
        if (this->geometryTransformId != 0xffffffff) {
            transform = (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geometryTransformId);
            transform->Update((int64_t) elapsed, false);
        }
    }

    RocketGun::update(elapsed);
    gun = this->gun;
    if (gun->ignited == 0)
        return;

    if (this->detonationPending != 0) {
        if (this->playerControlled != 0) {
            player = this->player;
            player->levelScript->resetCamera(player->level);
            player->setRocketControl(nullptr, this->trailGeometry);
            sound()->stop(0x45c);
        }

        this->rumbleTimer = 0;
        player = this->player;
        Vector toPlayer = this->detonationPosition - player->getPosition();
        float distance = AbyssEngine::AEMath::VectorLength(toPlayer);
        float magnitude = (float) this->gun->getMagnitude();
        float force = (((magnitude * 0.5f) - distance) / (magnitude * 0.5f)) * 0.5f;
        if (force > 1.0f)
            force = 1.0f;
        if (force < 0.0f)
            force = 0.0f;
        if (this->bombType == ITEM_SORT_SHOCK_BLAST)
            force *= 0.2f;
        if (gameStatus()->hardCoreMode() != 0) {
            ((Player *) player->player)->damage((int) (force * (float) this->gun->damage));
        }
        player->addNukeVolatileForce(force);

        float capped = (distance < 30000.0f) ? distance : 30000.0f;
        this->rumbleStrength = 1.0f - capped / 30000.0f;
        ((TargetFollowCamera *) (intptr_t) player->getTargetFollowCamera())
                ->setRumblePercentage(this->rumbleStrength, 0x32);

        Vector direction =
                (this->bombType == ITEM_SORT_SHOCK_BLAST) ? player->GetDirVector() : Vector{0.0f, 0.0f, 0.0f};
        this->explosion->start(this->detonationPosition, direction);
        this->detonationPending = 0;
    }

    // Native call at _ZN7BombGun6updateEi is Explosion::update(explosion, elapsed, 0).
    this->explosion->update(elapsed, static_cast<TargetFollowCamera *>(nullptr));
    int timer = this->rumbleTimer + elapsed;
    if (timer > 2000)
        timer = 2000;
    this->rumbleTimer = timer;

    player = this->player;
    float rumble = this->rumbleStrength * ((float) timer / -2000.0f + 1.0f);
    ((TargetFollowCamera *) (intptr_t) player->getTargetFollowCamera())
            ->setRumblePercentage(rumble, 0x32);

    if (this->explosion->isPlaying() == 0) {
        ((TargetFollowCamera *) (intptr_t) player->getTargetFollowCamera())
                ->setRumblePercentage(0.0f, 0);
        this->rumbleTimer = 0;
        this->gun->ignited = 0;
        this->detonationPending = 1;
        this->explosion->reset();
    }
}
