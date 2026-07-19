#include "game/ship/PlayerJunk.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "game/weapons/Radar.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/world/Level.h"
#include "engine/render/ParticleSystemManager.h"


static FModSound *g_PJ_sound = nullptr;

namespace AbyssEngine {
    namespace AERandom {
        int nextInt(void *rng, int bound);
    }
}

static void *g_PJ_random = nullptr;

PlayerJunk::PlayerJunk(int type, Player *player, AEGeometry *geometry,
                       float x, float y, float z)
    : KIPlayer(type, -1, player, geometry, x, y, z, false) {
}

PlayerJunk::~PlayerJunk() {
}

void PlayerJunk::reset() {
    KIPlayer::reset();
    this->state = 0;
    this->setVisible(true);
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
    this->field_0x120 = elapsed;
    if (this->player->getHitpoints() < 1) {
        if ((uint32_t)(this->state - 3) >= 2) {
            this->level->junkDied();
            this->state = 3;
            g_PJ_sound->play(0x16, nullptr, nullptr, 0.0f);
            if (AbyssEngine::AERandom::nextInt(g_PJ_random, 100) < 10) {
                this->hasCargo = 1;
                this->cargo = new Array<int>();
                ArrayAdd(99, *this->cargo);
                ArrayAdd(AbyssEngine::AERandom::nextInt(g_PJ_random, 10) + 1, *this->cargo);
                this->createCrate(3);
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
                level->field_34, position, 0, zero, 0.0f);
        }
    }

    if (this->state == 3)
        this->state = 4;
}
