#include "game/ship/PlayerAsteroid.h"
#include "engine/render/AEGeometry.h"
#include "engine/math/AEMath.h"
#include "game/world/Level.h"
#include "game/mission/Explosion.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "engine/math/Transform.h"

namespace AbyssEngine {
    namespace AERandom {
        int nextInt(int rng, int bound);
    }
}

AbyssEngine::Transform *PlayerAsteroidTransformGetTransform(void *canvas, uint32_t handle);

static void *g_playerAsteroidCanvas = nullptr;
static Level *g_playerAsteroidLevel = nullptr;
static int g_playerAsteroidRandom = 0;
static Vector g_playerAsteroidCenter;
static int g_playerAsteroidCenterLength = 0;

static const char *g_playerAsteroidQualityNames[3] = {"", "", ""};
static const char g_playerAsteroidQualityDefault[1] = {0};
static const char g_playerAsteroidQualityFour[1] = {0};

void PlayerAsteroid::setAsteroidIndex(int asteroidIndex) {
    this->asteroidIndex = asteroidIndex;
}

void PlayerAsteroid::translate(const Vector &delta) {
    this->geometry->translate(delta);
}

void PlayerAsteroid::render() {
    if (this->visibleFlag == 0)
        return;
    if (this->secondaryGeometry() != nullptr)
        this->secondaryGeometry()->render();
    if (this->state == 0)
        this->geometry->render();
    else if (this->state == 3)
        this->explosion->render();
}

void PlayerAsteroid::setPosition(const Vector &position) {
    this->geometry->setPosition(position);
}

Vector PlayerAsteroid::getPosition() {
    return this->geometry->getPosition();
}

void PlayerAsteroid::setRotationEnabled(bool enabled) {
    this->rotationEnabled = enabled;
}

int PlayerAsteroid::getQualityFrameIndex() {
    return 7 - this->quality;
}

int PlayerAsteroid::outerCollide(float x, float y, float z) {
    return this->collide(x, y, z);
}

int PlayerAsteroid::outerCollide(Vector point) {
    return this->collide(point.x, point.y, point.z);
}

int PlayerAsteroid::getQuality() {
    return this->quality;
}

float PlayerAsteroid::getScaling() {
    return this->scaling;
}

uint8_t PlayerAsteroid::isMinable() {
    return this->minable;
}

String PlayerAsteroid::getQualityString() {
    int quality = this->quality;
    unsigned int index = quality - 5U;
    const char *text;
    if (index < 3U) {
        text = g_playerAsteroidQualityNames[index];
    } else if (quality == 4) {
        text = g_playerAsteroidQualityFour;
    } else {
        text = g_playerAsteroidQualityDefault;
    }
    String result;
    for (const char *p = text; *p; ++p)
        { int _nl = result.length + 1; unsigned short *_nd = new unsigned short[_nl + 1]; for (int _i = 0; _i < result.length; _i++) _nd[_i] = result.data[_i]; _nd[result.length] = (unsigned short) ((char16_t) (unsigned char) *p); _nd[_nl] = 0; if (result.data) delete[] result.data; result.data = _nd; result.length = _nl; }
    return result;
}

void PlayerAsteroid::update(int delta) {
    this->lastDelta = delta;
    if (delta == 0)
        return;

    Player *player = this->player;
    if (player->isActive() == 0 && this->state == 4) {
        this->asteroidFlag = 0;
        return;
    }

    if (player->getHitpoints() <= 0 && this->state == 0) {
        this->state = 3;
        g_playerAsteroidLevel->asteroidDied();

        if (this->dropsLoot() != 0) {
            int quality = this->quality;
            bool spawn = true;
            if (quality == 7) {
                spawn = AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 100) < 4;
            } else if (quality > 6 ||
                       AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 100) > 0x13) {
                spawn = false;
            }

            if (spawn) {
                this->loot() = new Array<int>();
                int item = this->asteroidIndex;
                if (item == 0xd9) {
                    if (quality == 7)
                        item = 0xda;
                } else if (quality == 7) {
                    item += 0xb;
                }
                ArrayAdd(item, *this->loot());
                int count = 1;
                if (quality != 7)
                    count = AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 3) + 1;
                ArrayAdd(count, *this->loot());
                this->createCrate(this->asteroidIndex == 0xa4 ? 2 : 1);
            } else {
                this->dropsLoot() = 0;
                this->loot() = nullptr;
            }
        } else {
            this->dropsLoot() = 0;
            this->loot() = nullptr;
        }

        Matrix geometryMatrix = this->geometry->getMatrix();
        this->explosion->start(geometryMatrix);
        return;
    }

    if (this->state == 3) {
        this->explosion->update(delta, static_cast<TargetFollowCamera *>(nullptr));
        if (this->explosion->isPlaying() == 0) {
            this->state = 4;
            player->setBombForce(0.0f);
        }
    } else if (this->state == 4) {
        player->setBombForce(0.0f);
    }

    if (player->getHitpoints() < this->lastHitpoints) {
        this->hitFlashActive = 1;
        this->hitFlashTimer = 1.0f;
        this->lastHitpoints = player->getHitpoints();
    }

    if (this->rotationEnabled != 0) {
        Vector rotation = this->spin * ((float) delta * 0.001f);
        this->geometry->rotate(rotation);
    }

    float bombForce = player->getBombForce();
    if (bombForce > 0.0f && this->state == 3) {
        Vector hit = player->getHitVector();
        float scaling = this->scaling;
        float clamped = scaling;
        if (clamped > 1.0f)
            clamped = 1.0f;
        if (scaling < 0.5f)
            clamped = 1.0f;
        float force = (float) (int) (bombForce * 0.1f * (clamped / 2.0f + 1.0f));
        hit = hit * force;
        KIPlayer::translate(hit);
        this->explosion->translate(hit);
        if (this->secondaryGeometry() != nullptr)
            this->secondaryGeometry()->translate(hit);
        float nextForce = bombForce * 0.75f;
        if (nextForce < 0.01f)
            nextForce = 0.0f;
        player->setBombForce(nextForce);
    }
}

Vector PlayerAsteroid::getProjectionVector(const Vector &value) {
    Vector position = this->geometry->getPosition();
    Vector toCenter = position - value;
    return VectorNormalize(toCenter);
}

void PlayerAsteroid::setAsteroidCenter(Vector center) {
    g_playerAsteroidCenter = center;
    g_playerAsteroidCenterLength = (int) VectorLength(center);
}

int PlayerAsteroid::collide(float x, float y, float z) {
    if (this->player->getHitpoints() <= 0)
        return false;

    Vector position = this->geometry->getPosition();
    int radiusInt = this->player->getRadius();
    float radius = (float) radiusInt;
    float negativeRadius = (float) -radiusInt;

    float dx = x - position.x;
    if (dx >= radius || dx <= negativeRadius)
        return false;
    float dy = y - position.y;
    if (dy >= radius || dy <= negativeRadius)
        return false;
    float dz = z - position.z;
    if (dz >= radius || dz <= negativeRadius)
        return false;
    return true;
}

void PlayerAsteroid::push(int delta) {
    int remaining = this->pushTimer();
    if (remaining <= 0)
        return;

    remaining -= delta;
    this->pushTimer() = remaining;
    float t = (float) remaining / (float) this->pushDuration();

    Matrix identity;
    identity = AbyssEngine::AEMath::Matrix();
    Matrix rotation;
    MatrixSetRotation(rotation, t * this->pushSpin().x, t * this->pushSpin().y,
                      t * this->pushSpin().z);

    int frameDelta = this->lastDelta;
    AEGeometry *geometry = this->geometry;
    if (frameDelta > 0) {
        Matrix geometryMatrix = geometry->getMatrix();
        Matrix combined = rotation * geometryMatrix;
        geometry->setMatrix(combined);
        frameDelta = this->lastDelta;
    }

    Vector baseMove = this->pushDirection() * ((float) frameDelta);
    float scale = (2.0f - t) * 3.0f * ((float) this->pushDuration() / 1000.0f);
    Vector move = baseMove * scale;
    geometry->translate(move);
}

void PlayerAsteroid::initPush(const Vector &target, int duration) {
    Vector current = this->getPosition();
    Vector delta = target - current;
    float ratio = VectorLength(delta) / (float) duration;
    float clamped = (ratio < 1.0f) ? ratio : 1.0f;
    int pushFrames = (int) ((1.0f - clamped) * 1000.0f);
    this->pushTimer() = pushFrames;
    this->pushDuration() = pushFrames;

    Vector here = this->getPosition();
    Vector directionSource = here - target;
    this->pushDirection() = VectorNormalize(directionSource);

    Vector randomVector = {
        (float) (AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 200) - 100),
        (float) (AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 200) - 100),
        (float) (AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 200) - 100),
    };
    this->pushSpin() = VectorNormalize(randomVector) * 0.01f;
}

PlayerAsteroid::PlayerAsteroid(int playerId, AEGeometry *geometry, int explosionType,
                               int asteroidIndex, const Vector &position, float scaling,
                               int quality)
    : KIPlayer(playerId, 0, new Player(0x5dc, 0x1e, 0, 0, 0), geometry,
               position.x, position.y, position.z, true) {
    this->player->setKIPlayer(this);

    this->asteroidFlag = 0;
    this->asteroidIndex = asteroidIndex;
    this->scaling = scaling;
    this->quality = quality;
    this->spin = Vector{0.0f, 0.0f, 0.0f};
    this->field_0x164 = 0;
    this->field_0x168 = 0;
    this->field_0x16c = 0;

    AbyssEngine::Transform *transform =
            PlayerAsteroidTransformGetTransform(g_playerAsteroidCanvas, geometry->transform);
    this->player->setRadius((int) (transform->boundingRadius * scaling * 0.5f));
    this->player->setMaxHitpoints((int) (scaling * 100.0f + 30.0f));
    this->minable = quality > 3;
    this->lastHitpoints = this->player->getHitpoints();

    this->explosion = new Explosion(explosionType + 2);
    this->explosion->setScaling(scaling);
    this->geometry->setScaling(scaling);
    this->setPosition(position);

    this->geometry->setRotation(
        (float) AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 100) * 0.01f * 6.2831855f,
        (float) AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 100) * 0.01f * 6.2831855f,
        (float) AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 100) * 0.01f * 6.2831855f);

    this->secondaryGeometry() = nullptr;
    this->rotationEnabled = 1;
    Vector axis = {
        (float) (AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 3) - 1),
        (float) (AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 3) - 1),
        (float) (AbyssEngine::AERandom::nextInt(g_playerAsteroidRandom, 3) - 1),
    };
    this->spin = axis;

    this->dropsLoot() = 1;
    this->state = 0;
    this->hitFlashActive = 0;
    this->hitFlashTimer = 0.0f;
}

PlayerAsteroid::~PlayerAsteroid() {
    if (this->explosion != nullptr) {
        delete this->explosion;
        this->explosion = nullptr;
    }
}

// Static data members present in the original binary (defined for symbol parity).
AbyssEngine::AEMath::Vector PlayerAsteroid::tmp_vector2;
AbyssEngine::AEMath::Vector PlayerAsteroid::asteroidCenter;
float PlayerAsteroid::asteroidDistance;
AbyssEngine::AEMath::Vector PlayerAsteroid::pos;
float PlayerAsteroid::emitTime;
