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

    float update(float x, float y, float z);

    bool waypointDefined();
};

#endif
