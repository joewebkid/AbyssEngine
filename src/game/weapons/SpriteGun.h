#ifndef GOF2_SPRITEGUN_H
#define GOF2_SPRITEGUN_H
#include <cstdint>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Gun.h"
#include "AbstractGun.h"

class Player;


class Gun;


class SpriteGun : public AbstractGun {
public:
    int32_t field_0x4;
    Gun *gun;

    SpriteGun(Gun *gun, int kind);

    ~SpriteGun() override;

    void setEnemies(Array<Player *> *enemies) override;

    void setEnemy(Player *enemy) override;

    void update(int elapsed) override;

    void render() override;
};
#endif
