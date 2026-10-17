#include "game/ship/PlayerWormHole.h"

#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"
#include "engine/math/Transform.h"
#include "engine/math/AEMath.h"
#include "engine/core/GameText.h"
#include "game/world/Level.h"
#include "game/world/Station.h"
#include "game/mission/Status.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerStaticFar.h"
#include "game/ui/Hud.h"
#include "game/core/String.h"
#include "game/core/Globals.h"
#include "engine/core/AERandom.h"

static inline int wormholeSign(int value) {
    return value == 0 ? 1 : -1;
}

PlayerWormHole::PlayerWormHole(int playerId, AEGeometry *geometry, float x, float y, float z, bool visible)
    : PlayerStaticFar(playerId, geometry, x, y, z) {
    GameText *text = Globals::gameText;
    this->name = *text->getText(0x221);
    this->setVisible(visible);
    this->player->setRadius(40000);

    AbyssEngine::PaintCanvas *canvas = Globals::Canvas;
    AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geometry->transform);
    transform->SetAnimationState((AbyssEngine::AnimationMode) 2, 0);

    this->missionLock = 1;
    this->timer = 0;
    this->scale = 0x1000;
}

PlayerWormHole::~PlayerWormHole() {
}

bool PlayerWormHole::isShrinking() {
    return this->timer > 60000;
}

int PlayerWormHole::open() {
    this->timer = -3000;
    this->scale = 0;
    return (int) (intptr_t) this;
}

Vector PlayerWormHole::getPosition() {
    Vector result = {(float) this->positionX, (float) this->positionY, (float) this->positionZ};
    return result;
}

void PlayerWormHole::freeMissionLock() {
    this->missionLock = 0;
}

void PlayerWormHole::render() {
    if (!this->visibleFlag)
        return;
    this->geometry->render();
}

void PlayerWormHole::reset(bool shrinking) {
    this->timer = shrinking ? 59000 : 0;
    this->scale = 0x1000;
}

void PlayerWormHole::setPosition(float x, float y, float z) {
    this->posX = x;
    this->posY = y;
    this->posZ = z;
    this->positionX = (int) x;
    this->positionY = (int) y;
    this->positionZ = (int) z;
    this->geometry->setPosition(x, y, z);
}

void PlayerWormHole::update(int elapsed) {
    AbyssEngine::PaintCanvas *canvas = Globals::Canvas;
    AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) canvas->TransformGetTransform(this->geometry->transform);
    transform->Update((long long) elapsed, false);

    if (!this->visibleFlag)
        return;

    this->timer += elapsed;
    int time = this->timer;

    if (time < 0) {
        this->scale = 0x1000 - (int) (((float) -time / 3000.0f) * 4096.0f);
    } else if (time > 60000) {
        Status *status = Globals::status;
        int mission = status->getCurrentCampaignMission();
        int current = time;

        if (this->missionLock != 0) {
            bool keepOpen = false;
            if (mission == 0x2a) {
                keepOpen = status->inAlienOrbit();
            } else if (mission == 0x28 && !status->inAlienOrbit()) {
                keepOpen = true;
            }
            if (keepOpen) {
                current = 60000;
                this->timer = 60000;
            }
        }

        this->scale = 0x1000 - (int) (((float) (current - 60000) / 3000.0f) * 4096.0f);

        if (current > 63000) {
            bool stationUnderAttack =
                    status->inAlienOrbit() || status->getStation()->isAttackedByAliens();
            if (!stationUnderAttack) {
                this->visibleFlag = 0;
                this->geometry->setVisible(false);
                return;
            }

            bool closeWormhole =
                    this->missionLock != 0 && mission == 0x2a && !status->inAlienOrbit();
            if (closeWormhole) {
                this->visibleFlag = 0;
                this->geometry->setVisible(false);
                return;
            }

            this->timer = -3000;

            AERandom *random = static_cast<AERandom *>(Globals::rnd);
            int x, y, z;
            if (!status->inAlienOrbit()) {
                x = random->nextInt(40000) + 20000;
                x *= wormholeSign(random->nextInt(2));
                y = random->nextInt(40000) + 20000;
                y *= wormholeSign(random->nextInt(2));
                z = random->nextInt(40000) + 20000;
                z *= wormholeSign(random->nextInt(2));
            } else {
                x = random->nextInt(60000) + 30000;
                x *= wormholeSign(random->nextInt(2));
                y = random->nextInt(40000) + 20000;
                z = -60000 - random->nextInt(40000);
            }

            if (mission == 0x1d || mission == 0x29) {
                PlayerEgo *ego = (PlayerEgo *) (intptr_t) this->level->getPlayer();
                this->cameraPosition = ego->getPosition();
                x = (int) (this->cameraPosition.x + (float) x * 1.7f + (float) x);
                y = (int) (this->cameraPosition.y + (float) y * 1.7f + (float) y);
                z = (int) (this->cameraPosition.z + (float) z * 1.7f + (float) z);
            }

            this->setPosition((float) x, (float) y, (float) z);

            PlayerEgo *ego = (PlayerEgo *) (intptr_t) this->level->getPlayer();
            if (ego->goingToWormhole()) {
                PlayerEgo *target = this->level->getPlayer();
                Hud *hud = (Hud *) (intptr_t) target->getHUD();
                target = this->level->getPlayer();
                hud->hudEvent(6, target, 0);
                target = this->level->getPlayer();
                target->setAutoPilot(nullptr);
            }
        }
    }

    this->PlayerStaticFar::update(elapsed);

    canvas = Globals::Canvas;
    unsigned int currentCamera = canvas->CameraGetCurrent();
    this->cameraPosition =
            AbyssEngine::AEMath::MatrixGetPosition(
                *(AbyssEngine::AEMath::Matrix *) canvas->CameraGetLocal(currentCamera));

    float scale = (float) (this->scale << 4) * 0.0000152587890625f;
    this->geometry->setScaling(scale);

    this->viewDirection = this->geometry->getPosition();
    this->cameraPosition -= this->viewDirection;
    this->cameraPosition = AbyssEngine::AEMath::VectorNormalize(this->cameraPosition);
    this->cameraPosition.x += 0.5f;

    Vector up = {0.0f, 1.0f, 0.0f};
    this->geometry->setDirection(this->cameraPosition, up);
}
