#include "game/ship/PlayerStaticFar.h"

#include <vector>

#include "engine/math/AEMath.h"
#include "engine/math/BoundingVolume.h"
#include "engine/render/AEGeometry.h"
#include "game/core/Globals.h"
#include "game/ship/PlayerStatic.h"
#include "game/ship/Player.h"
#include "engine/render/PaintCanvas.h"

PlayerStaticFar::PlayerStaticFar(int playerId, AEGeometry *geometry, float x, float y, float z)
    : PlayerStatic(playerId, geometry, x, y, z) {
    this->viewDirection = {0.0f, 0.0f, 0.0f};
    this->posX = x;
    this->posY = y;
    this->posZ = z;
    this->player->setRadius(0x1d4c);
    this->boundingVolumes = nullptr;
}

PlayerStaticFar::~PlayerStaticFar() = default;

Vector PlayerStaticFar::getProjectionVector(const Vector &value) {
    if (this->boundingVolumes != nullptr) {
        return (*this->boundingVolumes)[0]->getProjectionVector(value);
    }
    return {0.0f, 0.0f, 0.0f};
}

void PlayerStaticFar::render() {
    this->geometry->render();
}

Vector PlayerStaticFar::projectCollisionOnSurface(const Vector &value) {
    if (this->boundingVolumes != nullptr) {
        return BoundingVolume::staticProjectCollisionOnSurface(value, this->boundingVolumes);
    }
    return {0.0f, 0.0f, 0.0f};
}

int PlayerStaticFar::outerCollide(float x, float y, float z) {
    return this->collide(x, y, z);
}

Vector PlayerStaticFar::getInitPosition(Vector /*value*/) {
    return {this->posX, this->posY, this->posZ};
}

void PlayerStaticFar::setYRotation(int /*yRotation*/) {
}

int PlayerStaticFar::outerCollide(Vector value) {
    return this->collide(value.x, value.y, value.z);
}

void PlayerStaticFar::update(int /*delta*/) {
    if (this->geometry == nullptr)
        return;

    PaintCanvas *camera = Globals::Canvas;
    unsigned int cur = camera->CameraGetCurrent();
    AbyssEngine::AEMath::Matrix *matrix = reinterpret_cast<AbyssEngine::AEMath::Matrix *>(camera->CameraGetLocal(cur));

    this->cameraPosition = MatrixGetPosition(*matrix);
    this->objectPosition = {(float) this->positionX, (float) this->positionY, (float) this->positionZ};
    this->viewDirection = this->objectPosition - this->cameraPosition;

    float len = VectorLength(this->viewDirection);

    if ((int) len < 295001) {
        this->geometry->setScaling(1.0f, 1.0f, 1.0f);
        this->geometry->setPosition(
            (float) this->positionX, (float) this->positionY, (float) this->positionZ);
    } else {
        this->viewDirection = VectorNormalize(this->viewDirection);
        this->viewDirection *= 295000.0f;
        this->viewDirection += this->cameraPosition;
        this->geometry->setPosition(this->viewDirection);

        float s = (float) (int) ((295000.0f / (float) (int) len) * 4096.0f);
        s = s * 0.000244140625f;
        this->geometry->setScaling(s, s, s);
    }
}

int PlayerStaticFar::collide(float x, float y, float z) {
    int boundI = this->player->radius;
    float bound = (float) boundI;
    float negBound = (float) (-boundI);

    float dx = x - (float) this->positionX;
    if (!(dx < bound) || !(dx > negBound))
        return false;

    float dy = y - (float) this->positionY;
    if (!(dy < bound) || !(dy > negBound))
        return false;

    float dz = z - (float) this->positionZ;
    if (!(dz < bound) || !(dz > negBound))
        return false;

    return true;
}
