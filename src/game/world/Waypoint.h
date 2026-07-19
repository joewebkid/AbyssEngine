#ifndef GOF2_WAYPOINT_H
#define GOF2_WAYPOINT_H

#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"


class Route;


class Waypoint : public KIPlayer {
public:
    int32_t x;
    int32_t y;
    int32_t z;
    uint16_t state;
    Route *route;

    Waypoint(int x, int y, int z, Route *route);

    ~Waypoint();

    void setActive(bool active);

    void reached();

    Vector getPosition();

    void reset();
};

#endif
