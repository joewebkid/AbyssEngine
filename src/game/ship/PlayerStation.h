#ifndef GOF2_PLAYERSTATION_H
#define GOF2_PLAYERSTATION_H

#include <cstdint>

#include "game/ship/PlayerStaticFar.h"
#include "game/world/Station.h"

#include "engine/math/Vector.h"
class AEGeometry;
class Station;


class PlayerStation : public PlayerStaticFar {
public:


    AEGeometry *rootGeometry;
    uint32_t meshTransform;
    int32_t stationIndex;
    AEGeometry *secondGeometry;
    uint32_t collisionIndex;
    int32_t collisionRadius;
    uint32_t field_0x158;
    uint32_t field_0x15c;
    uint32_t field_0x160;
    uint32_t animTransform0;
    uint32_t animTransform1;
    uint32_t animTransform2;
    uint32_t animTransform3;

    explicit PlayerStation(Station *station);

    ~PlayerStation();

    void setVisible(bool visible);

    void setPosition(const Vector &position);

    Vector projectCollisionOnSurface(const Vector &position) override;

    void *getRoot();

    Vector getProjectionVector(const Vector &position) override;

    void render() override;

    int outerCollide(const Vector &position) override;

    int outerCollide(float x, float y, float z) override;

    Vector getPosition();

    int collide(float x, float y, float z) override;

    void update(int delta) override;

    void translate(float x, float y, float z);
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PlayerStation, rootGeometry) == 0x140, "station root");
static_assert(__builtin_offsetof(PlayerStation, stationIndex) == 0x148, "station index");
static_assert(__builtin_offsetof(PlayerStation, collisionRadius) == 0x154, "station radius");
static_assert(__builtin_offsetof(PlayerStation, animTransform3) == 0x170, "station animation tail");
static_assert(sizeof(PlayerStation) == 0x174, "station ARM size");
#endif

#endif
