#include "game/ship/PlayerJumpgate.h"
#include "game/ship/Player.h"
#include "game/world/SolarSystem.h"
#include "engine/render/AEGeometry.h"
#include "game/mission/Status.h"
#include "engine/math/Transform.h"
#include "engine/math/BoundingSphere.h"
#include "engine/render/PaintCanvas.h"

static PaintCanvas *g_PaintCanvas = nullptr;
static Status **g_Status = nullptr;

static PaintCanvas *paintCanvas() {
    return g_PaintCanvas;
}

bool PlayerJumpgate::timeToJump() {
    AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) paintCanvas()->TransformGetTransform(this->transformHandle);
    return transform->currentTime > 1000LL;
}

void PlayerJumpgate::activate() {
    if (this->activated != 0)
        return;

    if (this->transformHandle != 0xffffffffU) {
        AbyssEngine::Transform *transform =
                (AbyssEngine::Transform *) paintCanvas()->TransformGetTransform(this->transformHandle);
        transform->SetAnimationState(AbyssEngine::AnimationMode_0, nullptr);

        paintCanvas()->TransformRemoveChild(this->geometry->transform,
                                            this->geometry->childTransform);
        this->geometry->addChild(this->transformHandle);
    }

    this->activated = 1;
}

void PlayerJumpgate::addJumpAnimationHandle(uint32_t handle) {
    this->transformHandle = handle;
}

bool PlayerJumpgate::animationEnded() {
    if (this->activated == 0)
        return false;

    AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) paintCanvas()->TransformGetTransform(this->transformHandle);
    return !transform->animating;
}

void PlayerJumpgate::update(int delta) {
    if (!this->isVisible())
        return;

    AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) paintCanvas()->TransformGetTransform(this->geometry->transform);
    transform->Update((longlong) delta, true);
}

void PlayerJumpgate::setPosition(float x, float y, float z) {
    this->positionX = (int) x;
    this->positionY = (int) y;
    this->positionZ = (int) z;
    Vector pos = {x, y, z};
    this->geometry->setPosition(pos);
}

PlayerJumpgate::PlayerJumpgate(int playerId, AEGeometry *geometry, float x, float y, float z,
                               bool visible)
    : PlayerStaticFar(playerId, geometry, x, y, z) {
    this->setVisible(visible);

    if (visible) {
        this->boundingVolumes = new Array<BoundingVolume *>();
        ArraySetLength(1, *(this->boundingVolumes));

        Status *status = *g_Status;
        int radius;
        if (status->inAlienOrbit()) {
            radius = 0x1d4c;
        } else {
            SolarSystem *system = (SolarSystem *) (intptr_t) status->getSystem();
            radius = system->getRace() == 1 ? 0x2bf2 : 0x1d4c;
        }

        this->player->setRadius(radius);

        (*this->boundingVolumes)[0] = new BoundingSphere(x, y, z, 0.0f, 0.0f, 0.0f, (float) radius);
    }

    this->transformHandle = 0xffffffffU;
    this->activated = 0;
    this->geometry->setRotation(0.0f, 3.1415927f, 0.0f);
}

PlayerJumpgate::~PlayerJumpgate() {
}
