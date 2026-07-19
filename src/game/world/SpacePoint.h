#ifndef GOF2_SPACEPOINT_H
#define GOF2_SPACEPOINT_H

#include <cstdint>

#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"


class SpacePoint {
public:
    AbyssEngine::AEMath::Vector position;
    AbyssEngine::AEMath::Vector direction;
    int type;
    uint8_t free;
    int param;

    SpacePoint(int type, const AbyssEngine::AEMath::Vector &position,
               const AbyssEngine::AEMath::Vector &direction, int param);

    ~SpacePoint();

    void giveFree();

    void take();

    uint8_t isFree();

    int getIndex();
};

#endif
