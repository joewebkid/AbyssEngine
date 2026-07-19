#include "game/weapons/MineGun.h"
#include "game/weapons/ObjectGun.h"
#include "game/weapons/Gun.h"
#include "game/ship/TargetFollowCamera.h"
#include "game/ship/PlayerEgo.h"
#include "game/mission/Explosion.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/AEGeometry.h"
#include "engine/math/AEMath.h"
#include "engine/math/Transform.h"


static void *g_PaintCanvas = nullptr;

MineGun::MineGun(Gun *gun, int mesh, int param, int unused, Level *level)
    : ObjectGun(param, gun, mesh, 0, level) {
    this->field_0xc0 = 0;
    this->field_0xc4 = 0;
    this->field_0xc8 = 0;

    this->explosions = new Array<Explosion *>();
    ArraySetLength(gun->count, *(this->explosions));

    this->readyFlags = new uint8_t[this->explosions->size()];
    for (uint32_t i = 0; i < this->explosions->size(); ++i) {
        int kind = (gun->damage == 0) ? 7 : 0;
        Explosion *explosion = new Explosion(kind);
        explosion->setWeaponIndex(gun->itemIndex);
        (*this->explosions)[i] = explosion;
        this->readyFlags[i] = 1;
    }

    void **canvas = (void **) g_PaintCanvas;
    this->geometry = new AEGeometry((uint16_t)(mesh + 1), (PaintCanvas *) *canvas, false);
    ((PaintCanvas *) *canvas)->TransformAddChild(this->transform, this->geometry->transform);

    void *transform = ((PaintCanvas *) *canvas)->TransformGetTransform(this->geometry->transform);
    ((AbyssEngine::Transform *) transform)->SetAnimationState((AbyssEngine::AnimationMode) 2, 0);
}

MineGun::~MineGun() {
    if (this->explosions != nullptr) {
        for (Explosion *explosion: *this->explosions) {
            delete explosion;
        }
        ArrayRemoveAll(*(this->explosions));
        delete this->explosions;
        this->explosions = nullptr;
    }

    delete[] this->readyFlags;
    this->readyFlags = nullptr;

    delete this->geometry;
    this->geometry = nullptr;
}

int MineGun::isMineGun() {
    return 1;
}

void MineGun::render() {
    ObjectGun::render();
    for (uint32_t i = 0; i < this->gun->count; ++i) {
        if (this->gun->hitFlags[i] != 0) {
            (*this->explosions)[i]->render();
        }
    }
}

void MineGun::setPlayer(PlayerEgo *player) {
    this->player = player;
}

void MineGun::update(int delta) {
    ObjectGun::update(delta);

    void **canvas = (void **) g_PaintCanvas;
    if (this->gun->active != 0) {
        void *transform =
                ((PaintCanvas *) *canvas)->TransformGetTransform(this->geometry->transform);
        ((AbyssEngine::Transform *) transform)->Update((long long) delta, 0);
    }

    const float rumbleRange = 30000.0f;
    char *positions = this->gun->hitPositions;
    for (uint32_t i = 0; i < this->gun->count; ++i, positions += 0xc) {
        if (this->gun->hitFlags[i] == 0) {
            continue;
        }

        Vector *minePosition = (Vector *) positions;
        if (this->readyFlags[i] != 0) {
            this->rumbleTimer = 0;

            Vector playerPosition = this->player->getPosition();
            Vector toPlayer =
                    AbyssEngine::AEMath::operator-(*minePosition, playerPosition);
            float distance = AbyssEngine::AEMath::VectorLength(toPlayer);
            float clamped = (distance < rumbleRange) ? distance : rumbleRange;
            this->rumblePercentage = 1.0f - clamped / rumbleRange;

            TargetFollowCamera *camera =
                    (TargetFollowCamera *) (intptr_t) this->player->getTargetFollowCamera();
            camera->setRumblePercentage(this->rumblePercentage, 0x32);

            Vector direction = {0.0f, 0.0f, 0.0f};
            (*this->explosions)[i]->start(*minePosition, direction);
            this->readyFlags[i] = 0;
        }

        Explosion *explosion = (*this->explosions)[i];
        explosion->update(delta, static_cast<TargetFollowCamera *>(nullptr));

        int timer = this->rumbleTimer + delta;
        if (timer > 1999) {
            timer = 2000;
        }
        this->rumbleTimer = timer;

        TargetFollowCamera *camera =
                (TargetFollowCamera *) (intptr_t) this->player->getTargetFollowCamera();
        float pct = this->rumblePercentage * ((float) this->rumbleTimer / -2000.0f + 1.0f);
        camera->setRumblePercentage(pct, 0x32);

        if (explosion->isPlaying() == 0) {
            TargetFollowCamera *doneCamera =
                    (TargetFollowCamera *) (intptr_t) this->player->getTargetFollowCamera();
            doneCamera->setRumblePercentage(0.0f, 0);
            this->rumbleTimer = 0;
            this->gun->hitFlags[i] = 0;
            this->gun->ignited = 0;
            this->readyFlags[i] = 1;
            explosion->reset();
        }
    }
}
