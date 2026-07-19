#ifndef GOF2_PLAYERSTATICFAR_H
#define GOF2_PLAYERSTATICFAR_H

#include <cstdint>
#include <vector>

#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/PlayerStatic.h"

#include "engine/math/Vector.h"

class Player;

class AEGeometry;
class BoundingVolume;


class PlayerStaticFar : public PlayerStatic {
public:


    Player *player;
    Vector initPosition;
    Vector cameraPosition;
    Vector objectPosition;
    Array<BoundingVolume *> *boundingVolumes;
    Vector viewDirection;

    PlayerStaticFar(int playerId, AEGeometry *geometry, float x, float y, float z);

    ~PlayerStaticFar();

    Vector getProjectionVector(const Vector &value) override;

    void render() override;

    Vector projectCollisionOnSurface(const Vector &value) override;

    int outerCollide(float x, float y, float z) override;

    int outerCollide(Vector value);

    Vector getInitPosition(Vector value);

    void setYRotation(int yRotation);

    void update(int delta) override;

    int collide(float x, float y, float z) override;
};

#endif
