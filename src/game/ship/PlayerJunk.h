#ifndef GOF2_PLAYERJUNK_H
#define GOF2_PLAYERJUNK_H
#include "game/ship/KIPlayer.h"

class Player;


class AEGeometry;


class PlayerJunk : public KIPlayer {
public:
    int elapsedTime; // Android update stores elapsed at +0x124; allocation is 0x128.
    PlayerJunk(int type, Player *player, AEGeometry *geometry, float x, float y, float z);

    ~PlayerJunk();

    void update(int elapsed) override;

    void reset();

    void render() override;

    int collide(float x, float y, float z) override;

    int outerCollide(float x, float y, float z) override;
};
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(PlayerJunk) == 0x128, "PlayerJunk ARM size");
#endif
#endif
