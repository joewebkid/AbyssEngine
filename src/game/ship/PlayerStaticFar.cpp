#include "game/ship/PlayerStaticFar.h"

#include <vector>

#include "engine/math/AEMath.h"
#include "engine/math/BoundingVolume.h"
#include "engine/render/AEGeometry.h"
#include "game/ship/PlayerStatic.h"
#include "game/ship/Player.h"
#include "engine/render/PaintCanvas.h"

static PaintCanvas *g_cameraHolder;
static int g_distLimit;
static float g_radius;
static float g_scaleNum;
static float g_scaleFactor;

PlayerStaticFar::PlayerStaticFar(int playerId, AEGeometry *geometry, float x, float y, float z)
    : PlayerStatic(playerId, geometry, x, y, z) {
    this->viewDirection = {0.0f, 0.0f, 0.0f};
    this->initPosition = {x, y, z};
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
        Vector out;
        reinterpret_cast<BoundingVolume *>(&out)
                ->staticProjectCollisionOnSurface(value, this->boundingVolumes);
        return out;
    }
    return {0.0f, 0.0f, 0.0f};
}

int PlayerStaticFar::outerCollide(float x, float y, float z) {
    return this->collide(x, y, z);
}

Vector PlayerStaticFar::getInitPosition(Vector /*value*/) {
    return this->initPosition;
}

void PlayerStaticFar::setYRotation(int /*yRotation*/) {
}

int PlayerStaticFar::outerCollide(Vector value) {
    return this->collide(value.x, value.y, value.z);
}

void PlayerStaticFar::update(int /*delta*/) {
    if (this->geometry == nullptr)
        return;

    PaintCanvas *camera = g_cameraHolder;
    unsigned int cur = camera->CameraGetCurrent();
    AbyssEngine::AEMath::Matrix *matrix = reinterpret_cast<AbyssEngine::AEMath::Matrix *>(camera->CameraGetLocal(cur));

    this->cameraPosition = MatrixGetPosition(*matrix);
    this->objectPosition = {(float) this->posX, (float) this->posY, (float) this->posZ};
    this->viewDirection = this->objectPosition - this->cameraPosition;

    float len = VectorLength(this->viewDirection);

    if ((int) len < g_distLimit) {
        this->geometry->setScaling(1.0f);
        this->geometry->setPosition(
            Vector{(float) this->posX, (float) this->posY, (float) this->posZ});
    } else {
        this->viewDirection = VectorNormalize(this->viewDirection);
        this->viewDirection *= g_radius;
        this->viewDirection += this->cameraPosition;
        this->geometry->setPosition(this->viewDirection);

        float s = (float) (int) ((g_radius / (float) (int) len) * g_scaleNum);
        s = s * g_scaleFactor;
        this->geometry->setScaling(s);
    }
}

int PlayerStaticFar::collide(float x, float y, float z) {
    int boundI = this->player->radius;
    float bound = (float) boundI;
    float negBound = (float) (-boundI);

    float dx = x - (float) this->posX;
    if (dx >= bound || dx <= negBound)
        return false;

    float dy = y - (float) this->posY;
    if (dy >= bound || dy <= negBound)
        return false;

    float dz = z - (float) this->posZ;
    if (dz >= bound || dz <= negBound)
        return false;

    return true;
}
