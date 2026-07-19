#ifndef GOF2_PLAYERJUNK_H
#define GOF2_PLAYERJUNK_H
#include "game/ship/KIPlayer.h"

class Player;


class AEGeometry;


class PlayerJunk : public KIPlayer {
public:
    PlayerJunk(int type, Player *player, AEGeometry *geometry, float x, float y, float z);

    ~PlayerJunk();

    void update(int elapsed) override;

    void reset();

    void render() override;

    int collide(float x, float y, float z) override;

    int outerCollide(float x, float y, float z) override;
};
#endif
