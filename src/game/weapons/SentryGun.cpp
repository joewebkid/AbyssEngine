#include "game/weapons/SentryGun.h"
#include "game/weapons/Gun.h"
#include "game/world/Level.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"

SentryGun::SentryGun(Gun *gun, int mesh, int unused, int p4, Level *level)
    : ObjectGun(unused, gun, mesh, 0, level) {
    (void) p4;
    this->cooldown = gun->itemIndex * 3 - 0x279;
}

SentryGun::~SentryGun() {
}

void SentryGun::update(int dt) {
    this->gun->update(dt);

    Gun *gun = this->gun;
    if (gun->hitSmall == 0)
        return;
    gun->hitSmall = 0;

    Level *level = static_cast<Level *>(this->level);
    Array<KIPlayer *> &pool = *level->field_b0;

    int base = this->cooldown;
    for (int i = base; i < base + 3; i++) {
        KIPlayer *obj = pool[i];
        Player *owner = static_cast<Player *>(obj->player);
        if (obj->isDying() == false &&
            (owner->isActive() == 0 || owner->isDead())) {
            level->field_6c += 1;

            obj->revive();

            Gun *g = this->gun;
            AbyssEngine::AEMath::Vector *spawnPos =
                    (AbyssEngine::AEMath::Vector *) (g->positions + g->fireIndex * 12);

            obj->setPosition(spawnPos->x, spawnPos->y, spawnPos->z);
            return;
        }
        base = this->cooldown;
    }
}

void SentryGun::render() {
}
