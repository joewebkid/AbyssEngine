#include "game/ship/PlayerCreature.h"
#include "engine/core/AERandom.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/core/Globals.h"
#include "game/world/Level.h"
#include "engine/render/ParticleSystemManager.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "game/weapons/Radar.h"


static const int PlayerCreature_typeValues[4] = {1, 1, 1, 0};

namespace AbyssEngine {
    namespace AEMath {
        Matrix operator*(const Matrix &, const Matrix &);

        Matrix MatrixSetRotation(Matrix &, float, float, float);
    }
}

uint8_t PlayerCreature::isHooked() {
    return this->hooked;
}

void PlayerCreature::calmDown() {
    this->rageTimer = 0;
    this->raging = 0;
    this->hooked = 0;
    this->endurance = this->maxEndurance;
}

void PlayerCreature::unhook() {
    this->caught = 0;
    this->rageTimer = 0;
    this->raging = 0;
    this->hooked = 0;
    this->endurance = this->maxEndurance;
}

void PlayerCreature::render() {
    if (this->crateGeometry != nullptr) {
        this->crateGeometry->render();
    }

    if ((uint32_t)(this->state - 3) >= 2) {
        KIPlayer::render();
    }
}

int PlayerCreature::getEndurance() {
    return this->endurance;
}

int PlayerCreature::getWeight() {
    return PlayerCreature_typeValues[this->type];
}

uint8_t PlayerCreature::isCaught() {
    return this->caught;
}

void PlayerCreature::rage(int amount) {
    float amountScale = (float) amount / -100.0f;
    float typeScale = (float) PlayerCreature_typeValues[this->type];
    this->rageTimer = 0;
    this->raging = 1;
    this->hooked = 1;
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
    : KIPlayer(kind, -1, player, geometry, x, y, z, false) {
    this->rageMatrix = AbyssEngine::AEMath::Matrix();

    this->caught = 0;
    this->itemIndex = itemIndex;
    this->rageScale = 1.0f;
    this->rageTimer = 0;
    this->raging = 0;
    this->hooked = 0;

    int hitpoints = this->player->getHitpoints();
    int endurance = (int) (((float) PlayerCreature_typeValues[kind] / 10.0f) * 8000.0f) + 8000;
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
            auto *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);
            int first = random->nextInt(half);
            int second = random->nextInt(half);
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
            Globals::sound->play(0x16, nullptr, nullptr, 0.0f);
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
            levelObject->particleSystemMgr->emitManual(
                levelObject->field_34, position, 0, zero, -1.0f);
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
    this->setActive(true);
    this->state = 0;
    this->endurance = this->maxEndurance;
    this->lastHitpoints = this->player->getHitpoints();
    this->rageMatrix = AbyssEngine::AEMath::Matrix();
}
