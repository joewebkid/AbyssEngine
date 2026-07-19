#include "game/ship/PlayerCreature.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/world/Level.h"
#include "engine/render/ParticleSystemManager.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerJunk.h"
#include "game/ship/PlayerEgo.h"
#include "game/weapons/Radar.h"


static int PlayerCreature_weightTable[16] = {};
static int PlayerCreature_rageTable[16] = {};
static int PlayerCreature_enduranceTable[16] = {};
static int *PlayerCreature_randomMax = nullptr;
static FModSound **PlayerCreature_sound = nullptr;

void PlayerJunk_render(PlayerJunk * self);

namespace AbyssEngine {
    namespace AEMath {
        Matrix operator*(const Matrix &, const Matrix &);

        Matrix MatrixSetRotation(Matrix &, float, float, float);
    }
}

namespace AbyssEngine {
    namespace AERandom {
        int nextInt(int rng, int bound);
    }
}

void *ParticleSystemManager_emitManual_v(
    void *self, int handle, const float *pos, void *ret, const float *vel, float p5);

uint8_t PlayerCreature::isHooked() {
    return this->hooked;
}

void PlayerCreature::calmDown() {
    this->rageTimer = 0;
    this->raging = 0;
    this->endurance = this->maxEndurance;
}

void PlayerCreature::unhook() {
    this->caught = 0;
    this->rageTimer = 0;
    this->raging = 0;
    this->endurance = this->maxEndurance;
}

void PlayerCreature::render() {
    if (this->renderGeometry != nullptr) {
        this->renderGeometry->render();
    }

    if ((uint32_t)(this->state - 3) >= 2) {
        PlayerJunk_render((PlayerJunk *) this);
    }
}

int PlayerCreature::getEndurance() {
    return this->endurance;
}

int PlayerCreature::getWeight() {
    return PlayerCreature_weightTable[this->creatureType];
}

uint8_t PlayerCreature::isCaught() {
    return this->caught;
}

void PlayerCreature::rage(int amount) {
    float amountScale = (float) amount / -100.0f;
    float typeScale = (float) PlayerCreature_rageTable[this->creatureType];
    this->rageTimer = 0;
    this->raging = 0x101;
    this->rageScale = (amountScale + 1.0f) * (typeScale + 1.0f);
}

int PlayerCreature::getMaxEndurance() {
    return this->maxEndurance;
}

int PlayerCreature::getItemIndex() {
    return this->itemIndex;
}

PlayerCreature::PlayerCreature(int kind, int itemIndex, Player *player, AEGeometry *geometry,
                               float x, float y, float z)
    : KIPlayer(0, 0, player, geometry, x, y, z, true) {
    this->rageMatrix = AbyssEngine::AEMath::Matrix();

    this->caught = 0;
    this->itemIndex = itemIndex;
    this->rageScale = 1.0f;
    this->rageTimer = 0;
    this->raging = 0;

    int hitpoints = this->player->getHitpoints();
    int endurance = (int) (((float) PlayerCreature_enduranceTable[kind] / 10.0f) * 8000.0f) + 8000;
    this->maxEndurance = endurance;
    this->endurance = endurance;
    this->lastHitpoints = hitpoints;
    reset();
}

PlayerCreature::~PlayerCreature() {
}

void PlayerCreature::hook(int value) {
    this->hooked = 1;

    this->rage(value);
}

void PlayerCreature::update(int elapsed) {
    this->lastElapsed = elapsed;

    int lastHitpoints = this->lastHitpoints;
    int hitpoints = this->player->getHitpoints();
    if (lastHitpoints > hitpoints && this->state != 4) {
        this->raging = 1;
    }
    this->lastHitpoints = this->player->getHitpoints();

    if (this->raging != 0) {
        float elapsedFloat = (float) elapsed;
        int rageTime = this->rageTimer + elapsed;
        this->rageTimer = rageTime;
        this->endurance -= elapsed;
        float rageScale = this->rageScale + elapsedFloat * 0.0007f;
        elapsed = (int) (rageScale * elapsedFloat);
        this->rageScale = rageScale;

        if (rageTime < 4000) {
            Matrix shaken = this->geometry->getMatrix() * this->rageMatrix;
            this->geometry->setMatrix(shaken);
        } else {
            int half = elapsed >> 1;
            float negativeHalf = (float) half * -0.5f;
            int first = AbyssEngine::AERandom::nextInt(*PlayerCreature_randomMax, half);
            int second = AbyssEngine::AERandom::nextInt(*PlayerCreature_randomMax, half);
            float xAngle = ((negativeHalf + (float) first) * 0.000244140625f) * 2.0f * 3.1415927f;
            float zAngle = ((negativeHalf + (float) second) * 0.000244140625f) * 2.0f * 3.1415927f;
            AbyssEngine::AEMath::MatrixSetRotation(this->rageMatrix, xAngle, zAngle, 0.0f);
            this->rageTimer = 0;
        }

        if (this->endurance < 1) {
            if (this->hooked == 0) {
                this->rageTimer = 0;
                this->raging = 0;
                this->endurance = this->maxEndurance;
            } else {
                this->raging = 0;
                this->caught = 1;
            }
        }
    }

    if (this->player->getHitpoints() < 1) {
        if ((uint32_t)(this->state - 3) >= 2) {
            this->state = 3;
            (*PlayerCreature_sound)->play(0x16, nullptr, nullptr, 0.0f);
            this->player->setActive(false);

            Level *level = this->level;
            PlayerEgo *ego = level->getPlayer();
            Radar *radar = (Radar *) ego->field_0x14;
            if (radar->field_0x1c == (void *) this) {
                ego = level->getPlayer();
                radar = (Radar *) ego->field_0x14;
                radar->field_0x1c = nullptr;
            }

            Vector zero;
            zero.x = 0.0f;
            zero.y = 0.0f;
            zero.z = 0.0f;
            Vector position;
            position.x = this->posX;
            position.y = this->posY;
            position.z = this->posZ;
            Level *levelObject = this->level;
            ParticleSystemManager_emitManual_v(
                (void *) (intptr_t) levelObject->particleSystemMgr,
                levelObject->field_34,
                (const float *) &position, nullptr, (const float *) &zero, 0.0f);
        }
    }

    if (this->state == 3) {
        this->state = 4;
    } else if (this->state == 0 && this->caught == 0) {
        this->geometry->moveForward((float) elapsed);
    }

    Matrix current = this->geometry->getMatrix();
    memcpy(this->player->transform, &current, sizeof(this->player->transform));
}

int PlayerCreature::collide(float /*x*/, float /*y*/, float /*z*/) {
    return 0;
}

int PlayerCreature::outerCollide(float /*x*/, float /*y*/, float /*z*/) {
    return 0;
}

void PlayerCreature::reset() {
    KIPlayer::reset();
    this->state = 0;
    this->endurance = this->maxEndurance;
    this->lastHitpoints = this->player->getHitpoints();
    this->rageMatrix = AbyssEngine::AEMath::Matrix();
}
