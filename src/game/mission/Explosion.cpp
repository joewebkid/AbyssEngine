#include "game/mission/Explosion.h"
#include "game/ship/TargetFollowCamera.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "engine/math/Transform.h"
#include "engine/core/AERandom.h"
#include "engine/render/PaintCanvas.h"


using AbyssEngine::Matrix;
using AbyssEngine::Transform;

void MatrixSetRotation(Matrix *out, Matrix *base, int zero1, int zero2, float angle);

float VectorLength(const Vector *self);

void MatrixGetPosition(Vector *out, const Matrix *matrix);

void MatrixSetTranslation(Matrix *out, Matrix *base, float x, float y, float z);

void MatrixSetScaling(Matrix *out, Matrix *base, float x, float y, float z);

void MatrixGetUp(Vector *out, const Matrix *matrix);

void MatrixGetDir(Vector *out, const Matrix *matrix);

void MatrixGetLookAt(Matrix *out, const Vector *position, const Vector *target, const Vector *up);

namespace {
int Explosion_paintCanvas;
void *Explosion_random;
}

static inline PaintCanvas *explosionCanvas() {
    return (PaintCanvas *) (intptr_t) Explosion_paintCanvas;
}

static inline AbyssEngine::AERandom *explosionRandom() {
    return (AbyssEngine::AERandom *) Explosion_random;
}

void Explosion::reset() {
    PaintCanvas *canvas = explosionCanvas();

    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->SetAnimationState((AbyssEngine::AnimationMode) 3, 0);
    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);

    uint32_t lodTransform = this->primaryMesh->altTransform;
    if (lodTransform != 0xffffffffu) {
        ((Transform *) canvas->TransformGetTransform(lodTransform))->SetAnimationState((AbyssEngine::AnimationMode) 3, 0);
        ((Transform *) canvas->TransformGetTransform(this->primaryMesh->altTransform))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
    }

    AEGeometry *secondary = this->secondaryMesh;
    if (secondary != 0) {
        ((Transform *) canvas->TransformGetTransform(secondary->transform))->SetAnimationState((AbyssEngine::AnimationMode) 3, 0);
        ((Transform *) canvas->TransformGetTransform(this->secondaryMesh->transform))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
    }

    if (this->type == 6) {
        ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->SetAnimationRangeInTime(0x8fcLL, 10000000LL);
        ((Transform *) canvas->TransformGetTransform(this->secondaryMesh->transform))->SetAnimationRangeInTime(0x8fcLL, 10000000LL);
    }

    Array<AEGeometry *> *streaks = this->fireStreaks;
    if (streaks != 0) {
        for (uint32_t i = 0; i < streaks->size(); i++) {
            AEGeometry *geometry = (*streaks)[i];
            ((Transform *) canvas->TransformGetTransform(geometry->transform))->SetAnimationState((AbyssEngine::AnimationMode) 3, 0);
            ((Transform *) canvas->TransformGetTransform(geometry->transform))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
        }
    }

    this->elapsed = 0;
    this->playing = 0;
}

Explosion::~Explosion() {
    if (this->primaryMesh != 0) {
        delete this->primaryMesh;
    }
    this->primaryMesh = 0;

    if (this->secondaryMesh != 0) {
        delete this->secondaryMesh;
    }
    this->secondaryMesh = 0;

    Array<AEGeometry *> *streaks = this->fireStreaks;
    if (streaks != 0) {
        for (AEGeometry *e: *streaks) {
            delete e;
        }
        ArrayRemoveAll(*streaks);
        delete streaks;
    }
    this->fireStreaks = 0;
}

uint8_t Explosion::isPlaying() {
    return this->playing;
}

void Explosion::setWeaponIndex(int index) {
    this->weaponIndex = index;
}

void Explosion::translate(const Vector &v) {
    this->primaryMesh->translate(v);
    AEGeometry *secondary = this->secondaryMesh;
    if (secondary != 0) {
        return secondary->translate(v);
    }
}

void Explosion::setScaling(float scale) {
    this->scale = scale;
    this->primaryMesh->setScaling(scale, scale, scale);

    AEGeometry *secondary = this->secondaryMesh;
    if (secondary != 0) {
        secondary->setScaling(scale, scale, scale);
    }

    float speed = 1.0f;
    if (scale < 1.0f) {
        speed = (1.0f - scale) * 3.0f + 1.0f;
    }

    int type = this->type;
    if (type == 0xb) {
        speed = speed * 0.5f;
    }
    if ((uint32_t)(type - 8) < 3) {
        speed = 0.7f + (float) explosionRandom()->nextInt(0x3c) * 0.01f;
    }

    PaintCanvas *canvas = explosionCanvas();
    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->SetAnimationSpeed(speed);

    secondary = this->secondaryMesh;
    if (secondary != 0) {
        ((Transform *) canvas->TransformGetTransform(secondary->transform))->SetAnimationSpeed(speed);
    }

    Array<AEGeometry *> *streaks = this->fireStreaks;
    if (streaks != 0) {
        for (uint32_t i = 0; i < streaks->size(); i++) {
            AEGeometry *geometry = (*streaks)[i];
            ((Transform *) canvas->TransformGetTransform(geometry->transform))->SetAnimationSpeed(speed);
        }
    }

    this->duration = (long long) ((float) this->duration / speed);
}

bool Explosion::peakReached() {
    return this->elapsed > 0x8fcLL;
}

void Explosion::start(const Vector &position, const Vector &direction) {
    this->primaryMesh->setPosition(position);

    PaintCanvas *canvas = explosionCanvas();
    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->animating = true;

    uint32_t lodTransform = this->primaryMesh->altTransform;
    if (lodTransform != 0xffffffffu) {
        ((Transform *) canvas->TransformGetTransform(lodTransform))->animating = true;
    }

    AEGeometry *secondary = this->secondaryMesh;
    if (secondary != 0) {
        secondary->setPosition(position);
        ((Transform *) canvas->TransformGetTransform(this->secondaryMesh->transform))->animating = true;
    }

    int type = this->type;
    if ((uint32_t)(type - 8) < 3) {
        Matrix rotation;
        float angle = (float) explosionRandom()->nextInt(0xc45) / 1000.0f;
        MatrixSetRotation(&rotation, &this->rotation, 0, 0, angle);

        float scale = 0.6f + (float) explosionRandom()->nextInt(0x28) * 0.01f;
        this->setScaling(scale);
    } else if (type == 0xb) {
        Vector up;
        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;
        this->primaryMesh->setDirection(direction, up);
        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;
        this->secondaryMesh->setDirection(direction, up);
    }

    Array<AEGeometry *> *streaks = this->fireStreaks;
    if (streaks != 0) {
        for (uint32_t i = 0; i < streaks->size(); i++) {
            AEGeometry *geometry = (*streaks)[i];
            geometry->setPosition(position);
            ((Transform *) canvas->TransformGetTransform(geometry->transform))->animating = true;
        }
    }

    this->playing = 1;
    Vector soundPosition = position;
    this->playSound(&soundPosition);
}

void Explosion::update(int dt, TargetFollowCamera *camera) {
    if (this->playing == 0) {
        return;
    }

    PaintCanvas *canvas = explosionCanvas();
    long long delta = (long long) dt;
    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->Update(delta, 0);

    uint32_t lodTransform = this->primaryMesh->altTransform;
    if (lodTransform != 0xffffffffu) {
        ((Transform *) canvas->TransformGetTransform(lodTransform))->Update(delta, 0);
    }

    AEGeometry *secondary = this->secondaryMesh;
    if (secondary != 0) {
        ((Transform *) canvas->TransformGetTransform(secondary->transform))->Update(delta, 0);
    }

    Array<AEGeometry *> *streaks = this->fireStreaks;
    if (streaks != 0) {
        for (uint32_t i = 0; i < streaks->size(); i++) {
            AEGeometry *geometry = (*streaks)[i];
            ((Transform *) canvas->TransformGetTransform(geometry->transform))->Update(delta, 0);
        }
    }

    if (camera != 0 && (uint32_t) this->type < 2) {
        Vector cameraPosition;
        Vector position = this->primaryMesh->getPosition();

        unsigned int current = canvas->CameraGetCurrent();
        MatrixGetPosition(&cameraPosition, (const Matrix *) canvas->CameraGetLocal(current));
        Vector diff = position - cameraPosition;
        float distance = VectorLength(&diff);

        Transform *transform = ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform));
        int anim = (int) transform->currentTime;
        if (anim <= 0x7d0) {
            float capped = 30000.0f;
            if (distance < 30000.0f) {
                capped = distance;
            }
            float value = (1.0f - capped / 30000.0f) * ((float) anim / -2000.0f + 1.0f);
            camera->setRumblePercentage(value, 0x32);
        }
    }

    long long elapsed = this->elapsed + delta;
    this->elapsed = elapsed;
    if (this->duration < elapsed) {
        this->reset();
        if (camera != 0) {
            camera->setRumblePercentage(0.0f, 0);
        }
    }
}

Explosion::Explosion(int type) {
    this->rotation = AbyssEngine::AEMath::Matrix();
    this->scale = 1.0f;
    this->type = type;
    this->primaryMesh = 0;
    this->secondaryMesh = 0;
    this->fireStreaks = 0;

    PaintCanvas *canvas = explosionCanvas();

    switch (type) {
        case 0:
        case 6: {
            this->primaryMesh = new AEGeometry((uint16_t) 0x41b5, canvas, false);
            AEGeometry *geometry = new AEGeometry((uint16_t) 0x41b4, canvas, false);
            this->primaryMesh->addChild(geometry->transform);
            delete geometry;
            break;
        }
        default:
            if (type == 13) {
                this->primaryMesh = new AEGeometry((uint16_t) 0x41a9, canvas, false);
                this->setScaling(0.25f);
                ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->SetAnimationSpeed(1.0f);
            } else {
                this->primaryMesh = new AEGeometry((uint16_t) 0x4213, canvas, false);
                this->secondaryMesh = new AEGeometry((uint16_t) 0x4211, canvas, false);
            }
            break;
        case 3:
            this->primaryMesh = new AEGeometry((uint16_t) 0x4213, canvas, false);
            this->secondaryMesh = new AEGeometry((uint16_t) 0x421d, canvas, false);
            break;
        case 4:
            this->primaryMesh = new AEGeometry((uint16_t) 0x420d, canvas, false);
            this->secondaryMesh = new AEGeometry((uint16_t) 0x420c, canvas, false);
            break;
        case 5:
            this->primaryMesh = new AEGeometry((uint16_t) 0x4999, canvas, false);
            this->secondaryMesh = new AEGeometry((uint16_t) 0x4998, canvas, false);
            break;
        case 7:
            this->primaryMesh = new AEGeometry((uint16_t) 0x41a5, canvas, false);
            break;
        case 8:
            this->primaryMesh = new AEGeometry((uint16_t) 0x41a6, canvas, false);
            break;
        case 9:
            this->primaryMesh = new AEGeometry((uint16_t) 0x41a7, canvas, false);
            break;
        case 10:
            this->primaryMesh = new AEGeometry((uint16_t) 0x41a8, canvas, false);
            break;
        case 11:
            this->primaryMesh = new AEGeometry((uint16_t) 0x4a34, canvas, false);
            this->secondaryMesh = new AEGeometry((uint16_t) 0x4a33, canvas, false);
            break;
        case 12:
            this->primaryMesh = new AEGeometry((uint16_t) 0x4a7e, canvas, false);
            break;
    }

    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);

    if (this->secondaryMesh != 0) {
        ((Transform *) canvas->TransformGetTransform(this->secondaryMesh->transform))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
        ((Transform *) canvas->TransformGetTransform(this->secondaryMesh->transform))->boundingRadius = 10000.0f;
    }

    uint64_t primaryDuration =
            (uint64_t) ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->animationLength;
    uint64_t duration = 0;
    if (this->secondaryMesh != 0) {
        uint64_t secondaryDuration =
                (uint64_t) ((Transform *) canvas->TransformGetTransform(this->secondaryMesh->transform))->animationLength;
        duration = secondaryDuration < primaryDuration ? primaryDuration : secondaryDuration;
    } else if (primaryDuration != 0) {
        duration = primaryDuration;
    }
    this->duration = duration;

    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->boundingRadius = 10000.0f;
    this->weaponIndex = -1;
    this->reset();
}

static int Explosion_soundDefault;
static int Explosion_soundFallback;
static int Explosion_soundSpecial;
static char Explosion_soundSettings[0x10];

void Explosion::playSound(Vector *pos) {
    Vector *soundPos = pos;
    int soundId = this->weaponIndex;
    int cue;
    int sound;
    int enabled;

    if (soundId >= 0) {
        uint32_t offset = (uint32_t)(soundId - 0x29);
        if (offset <= 0x15) {
            cue = 0xf;
            switch (soundId) {
                case 0x29:
                    break;
                case 0x2a:
                    cue = 0x10;
                    break;
                case 0x2b:
                    cue = 0x11;
                    break;
                case 0x2c:
                    cue = 0xe;
                    break;
                case 0x2d:
                    cue = 0xd;
                    break;
                case 0x2e:
                    cue = 0xc;
                    break;
                case 0x2f:
                case 0x30:
                case 0x31:
                case 0x32:
                case 0x33:
                case 0x34:
                case 0x35:
                case 0x36:
                case 0x37:
                case 0x38:
                case 0x39:
                case 0x3a:
                case 0x3b:
                    return;
                case 0x3c:
                case 0x3d:
                case 0x3e:
                    cue = 0x16;
                    break;
            }
        } else {
            offset = (uint32_t)(soundId - 0xb0);
            if (offset < 3) {
                cue = 0x16;
            } else if (soundId == 0xb3) {
                cue = 0xc;
            } else if (soundId == 0xc5 || soundId == 0xdd) {
                cue = 0x8cd;
            } else {
                if (soundId != 0xe8) {
                    return;
                }
                cue = 0x8e7;
            }
        }
        sound = Explosion_soundDefault;
        enabled = Explosion_soundSettings[0xf];
    } else {
        int type = this->type;
        uint32_t typeOffset = (uint32_t)(type - 2);
        if (typeOffset < 4) {
            sound = Explosion_soundSpecial;
            enabled = Explosion_soundSettings[0xf];
            if (enabled == 0) {
                soundPos = 0;
            }
            ((FModSound *) (intptr_t) sound)->play(0x15, soundPos, 0, 0);
            return;
        }
        if (type != 0) {
            return;
        }
        sound = Explosion_soundFallback;
        int random = explosionRandom()->nextInt(2);
        enabled = Explosion_soundSettings[0xf];
        cue = 0x13;
        if (random == 0) {
            cue = 0x12;
        }
    }

    if (enabled == 0) {
        soundPos = 0;
    }
    ((FModSound *) (intptr_t) sound)->play(cue, soundPos, 0, 0);
}

void Explosion::start(const Matrix &matrix) {
    Vector position;

    int type = this->type;
    if (type == 6 || type == 0) {
        MatrixGetPosition(&position, &matrix);
        this->primaryMesh->setPosition(position);
        MatrixGetPosition(&position, &matrix);
        this->secondaryMesh->setPosition(position);
    } else {
        this->primaryMesh->setMatrix(*(const AbyssEngine::AEMath::Matrix *) &matrix);
        AEGeometry *secondary = this->secondaryMesh;
        if (secondary != 0) {
            secondary->setMatrix(*(const AbyssEngine::AEMath::Matrix *) &matrix);
        }
    }

    PaintCanvas *canvas = explosionCanvas();
    Array<AEGeometry *> *streaks = this->fireStreaks;
    if (streaks != 0) {
        for (uint32_t i = 0; i < streaks->size(); i++) {
            AEGeometry *geometry = (*streaks)[i];
            MatrixGetPosition(&position, &matrix);
            geometry->setPosition(position);
            ((Transform *) canvas->TransformGetTransform(geometry->transform))->animating = true;
        }
    }

    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->animating = true;
    uint32_t lodTransform = this->primaryMesh->altTransform;
    if (lodTransform != 0xffffffffu) {
        ((Transform *) canvas->TransformGetTransform(lodTransform))->animating = true;
    }
    AEGeometry *secondary = this->secondaryMesh;
    if (secondary != 0) {
        ((Transform *) canvas->TransformGetTransform(secondary->transform))->animating = true;
    }

    this->playing = 1;
    MatrixGetPosition(&position, &matrix);
    this->playSound(&position);
}

void Explosion::render() {
    Matrix cameraLocal;
    Matrix work;
    Vector position;
    Vector cameraPosition;
    Vector up;

    if (this->playing != 0) {
        uint32_t type = (uint32_t) this->type;
        if ((type > 0xd || ((1u << (type & 0xff)) & 0x2780u) == 0) && this->secondaryMesh != 0) {
            this->secondaryMesh->render();
        }

        PaintCanvas *canvas = explosionCanvas();
        unsigned int current = canvas->CameraGetCurrent();
        memcpy(&cameraLocal, canvas->CameraGetLocal(current), 0x3c);

        position = this->primaryMesh->getPosition();

        if (type < 0xd && ((1u << (type & 0xff)) & 0x1804u) != 0) {
            MatrixSetTranslation(&work, &cameraLocal, position.x, position.y, position.z);
        } else {
            MatrixGetPosition(&cameraPosition, &cameraLocal);
            MatrixGetUp(&up, &cameraLocal);
            MatrixGetLookAt(&work, &cameraPosition, &position, &up);
            cameraLocal = work;
        }

        float scale = this->scale;
        MatrixSetScaling(&work, &cameraLocal, scale, scale, scale);

        if (type - 8 < 3) {
            AbyssEngine::AEMath::MatrixMultiply(*(Matrix *) &cameraLocal, *(const Matrix *) &this->rotation);
        }

        this->primaryMesh->setMatrix(*(const AbyssEngine::AEMath::Matrix *) &cameraLocal);
        this->primaryMesh->setPosition(position);

        current = canvas->CameraGetCurrent();
        cameraLocal = *(const Matrix *) canvas->CameraGetLocal(current);
        Vector direction;
        MatrixGetDir(&direction, &cameraLocal);
        MatrixGetUp(&cameraPosition, &cameraLocal);
        this->primaryMesh->setDirection(direction, cameraPosition);
        this->primaryMesh->render();

        Array<AEGeometry *> *streaks = this->fireStreaks;
        if (streaks != 0) {
            for (uint32_t i = 0; i < streaks->size(); i++) {
                AEGeometry *geometry = (*streaks)[i];
                geometry->render();
            }
        }
    }
}

void Explosion::update(int dt, const Vector &position) {
    if (this->playing == 0) {
        return;
    }

    PaintCanvas *canvas = explosionCanvas();
    long long delta = (long long) dt;
    ((Transform *) canvas->TransformGetTransform(this->primaryMesh->transform))->Update(delta, 0);

    uint32_t lodTransform = this->primaryMesh->altTransform;
    if (lodTransform != 0xffffffffu) {
        ((Transform *) canvas->TransformGetTransform(lodTransform))->Update(delta, 0);
    }

    AEGeometry *secondary = this->secondaryMesh;
    if (secondary != 0) {
        ((Transform *) canvas->TransformGetTransform(secondary->transform))->Update(delta, 0);
    }

    Array<AEGeometry *> *streaks = this->fireStreaks;
    if (streaks != 0) {
        for (uint32_t i = 0; i < streaks->size(); i++) {
            AEGeometry *geometry = (*streaks)[i];
            geometry->setPosition(position);
            ((Transform *) canvas->TransformGetTransform(geometry->transform))->Update(delta, 0);
        }
    }

    long long elapsed = this->elapsed + delta;
    this->elapsed = elapsed;
    if (this->duration < elapsed) {
        return this->reset();
    }
}

void Explosion::addFireStreaks() {
    if (this->fireStreaks != 0) {
        return;
    }

    this->fireStreaks = new Array<AEGeometry *>();

    int length = explosionRandom()->nextInt(7) + 3;
    ArraySetLength(length, *(this->fireStreaks));

    PaintCanvas *canvas = explosionCanvas();

    for (uint32_t i = 0; i < this->fireStreaks->size(); i++) {
        AEGeometry *geometry = new AEGeometry((uint16_t) 0x37d4, canvas, false);
        (*this->fireStreaks)[i] = geometry;

        ((Transform *) canvas->TransformGetTransform(geometry->transform))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
        ((Transform *) canvas->TransformGetTransform(geometry->transform))->boundingRadius = 10000.0f;

        float x = (float) explosionRandom()->nextInt(0x168);
        float y = (float) explosionRandom()->nextInt(0x168);
        float z = (float) explosionRandom()->nextInt(0x168);
        geometry->setRotation((x / 180.0f) * 3.1415927f,
                              (y / 180.0f) * 3.1415927f,
                              (z / 180.0f) * 3.1415927f);

        int scaleInt = explosionRandom()->nextInt(0x32) + 0x32;
        float scale = (float) scaleInt / 100.0f;
        geometry->setScaling(scale, scale, scale);
    }
}
