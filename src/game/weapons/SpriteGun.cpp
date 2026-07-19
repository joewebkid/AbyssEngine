#include "game/weapons/SpriteGun.h"
#include "game/weapons/Gun.h"

SpriteGun::SpriteGun(Gun * /*gun*/, int /*kind*/) {
    // ARM ctor only writes vptr + field_0x4; gun @0x8 is read by delegates but not initialized here.
    this->field_0x4 = 0;
}

SpriteGun::~SpriteGun() {
}

void SpriteGun::setEnemies(Array<Player *> *enemies) {
    this->gun->setEnemies(enemies);
}

void SpriteGun::setEnemy(Player *enemy) {
    this->gun->setEnemy(enemy);
}

void SpriteGun::update(int elapsed) {
    this->gun->update(elapsed);
}

void SpriteGun::render() {
}
