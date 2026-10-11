#include "game/ship/PlayerJunk.h"
#include "engine/core/AERandom.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "game/core/Globals.h"
#include "game/weapons/Radar.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/world/Level.h"
#include "engine/render/ParticleSystemManager.h"


PlayerJunk::PlayerJunk(int type, Player *player, AEGeometry *geometry,
                       float x, float y, float z)
    : KIPlayer(type, -1, player, geometry, x, y, z, false) {
    countsAsEnemyExcludeFlag = 1;
}

PlayerJunk::~PlayerJunk() {
}

void PlayerJunk::reset() {
    KIPlayer::reset();
    this->state = 0;
    this->setActive(true);
}

int PlayerJunk::collide(float x, float y, float z) {
    return 0;
}

int PlayerJunk::outerCollide(float x, float y, float z) {
    return 0;
}

void PlayerJunk::render() {
    if (this->crateGeometry != nullptr)
        this->crateGeometry->render();
    if ((uint32_t)(this->state - 3) > 1)
        KIPlayer::render();
}

void PlayerJunk::update(int elapsed) {
    this->elapsedTime = elapsed;
    if (this->player->getHitpoints() < 1) {
        if ((uint32_t)(this->state - 3) >= 2) {
            this->level->junkDied();
            this->state = 3;
            Globals::sound->play(0x16, nullptr, nullptr, 0.0f);
            auto *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);
            if (random->nextInt(100) < 10) {
                this->hasCargo = 1;
                this->cargo = new Array<int>();
                ArrayAdd(99, *this->cargo);
                ArrayAdd(random->nextInt(10) + 1, *this->cargo);
                this->createCrate(3);
                this->hasCargo = 1;
            } else {
                this->player->setActive(false);

                PlayerEgo *playerObj = this->level->getPlayer();
                Radar *radar = (Radar *) playerObj->field_0x14;
                if (radar->field_0x1c == this)
                    radar->field_0x1c = nullptr;
            }

            Vector position;
            position.x = this->posX;
            position.y = this->posY;
            position.z = this->posZ;
            Vector zero;
            zero.x = 0.0f;
            zero.y = 0.0f;
            zero.z = 0.0f;

            Level *level = this->level;
            level->field_74->emitManual(
                level->field_34, position, 0, zero, -1.0f);
        }
    }

    if (this->state == 3)
        this->state = 4;
}
