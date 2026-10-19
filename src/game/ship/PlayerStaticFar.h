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


    // Android 0x11c254: Player/position live in KIPlayer, not a second copy.
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

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PlayerStaticFar, boundingVolumes) == 0x130, "far collision array");
static_assert(__builtin_offsetof(PlayerStaticFar, viewDirection) == 0x134, "far view direction");
static_assert(sizeof(PlayerStaticFar) == 0x140, "far player ARM size");
#endif

#endif
