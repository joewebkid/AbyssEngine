#ifndef GOF2_PLAYERSTATIC_H
#define GOF2_PLAYERSTATIC_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"
class AEGeometry;


class PlayerStatic : public KIPlayer {
public:


    int32_t positionX;
    int32_t positionY;
    int32_t positionZ;

    PlayerStatic(int playerId, AEGeometry *geometry, float x, float y, float z);

    ~PlayerStatic();

    void render() override;

    Vector getPosition();

    void update(int dt) override;

    void translate(const Vector &v) override;

    int collide(float x, float y, float z) override;

    int outerCollide(float x, float y, float z) override;
};

#endif
