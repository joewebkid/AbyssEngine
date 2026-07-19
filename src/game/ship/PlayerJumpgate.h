#ifndef GOF2_PLAYERJUMPGATE_H
#define GOF2_PLAYERJUMPGATE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/PlayerStaticFar.h"

class AEGeometry;


class PlayerJumpgate : public PlayerStaticFar {
public:
    uint8_t activated;

    uint32_t transformHandle;

    PlayerJumpgate(int playerId, AEGeometry *geometry, float x, float y, float z, bool visible);

    ~PlayerJumpgate();

    bool timeToJump();

    void activate();

    void addJumpAnimationHandle(uint32_t handle);

    bool animationEnded();

    void update(int delta) override;

    void setPosition(float x, float y, float z) override;
};
#endif
