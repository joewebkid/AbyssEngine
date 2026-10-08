#ifndef GOF2_ROUTE_H
#define GOF2_ROUTE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Waypoint.h"

#include "engine/math/Vector.h"
class KIPlayer;
class Waypoint;


class Route {
public:
    int32_t currentIndex;
    uint8_t loop;
    // Android constructors leave +0x08 untouched; all three array pointers
    // are at +0x0c/+0x10/+0x14, and callers allocate 0x18 bytes.
    int32_t field_08;
    Array<Waypoint *> *waypoints;
    Array<KIPlayer *> *dockingTargets;
    Array<int> *dockingTimes;

    Route(int *coords, int count);

    Route(int *coords, Array<KIPlayer *> *targets, int *times, int count);

    ~Route();

    int getCurrent();

    void setLoop(bool loop);

    Route *clone();

    KIPlayer *getDockingTarget();

    KIPlayer *getDockingTarget(int index);

    int getDockingTime();

    int getDockingTime(int index);

    Route *getExactClone();

    Waypoint *getLastWaypoint();

    Waypoint *getWaypoint();

    Waypoint *getWaypoint(int index);

    int length();

    void reachWaypoint(int index);

    void reset();

    void setNewCoords(Vector v);

    void translate(const Vector &v);

    void update(const Vector &v);

    // Android 0x11bf70 has no result computation and tail-calls setActive.
    void update(float x, float y, float z);

    bool waypointDefined();
};

#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(Route) == 0x18, "Route ARM allocation size");
static_assert(__builtin_offsetof(Route, waypoints) == 0x0c, "Route::waypoints offset");
static_assert(__builtin_offsetof(Route, dockingTargets) == 0x10, "Route::dockingTargets offset");
static_assert(__builtin_offsetof(Route, dockingTimes) == 0x14, "Route::dockingTimes offset");
#endif

#endif
