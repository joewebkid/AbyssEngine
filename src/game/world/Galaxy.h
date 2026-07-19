#ifndef GOF2_GALAXY_H
#define GOF2_GALAXY_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "SolarSystem.h"
#include "Station.h"

class SolarSystem;
class Station;


class Galaxy {
public:
    Galaxy();

    ~Galaxy();

    void reset();

    int distancePercent(int x1, int y1, int x2, int y2);

    int invDistancePercent(int x1, int y1, int x2, int y2);

    void visitStation(int index);

    void setVisited(bool *src, int count);

    int getSystem(int index);

    void *getPlasmaProbabilities(Station *station);

    void *getAsteroidProbabilities(Station *station);

    int getStation(int index);

    float distance(SolarSystem *a, SolarSystem *b);

    Array<SolarSystem *> *getSystems();

    uint8_t *getVisited();

    uint8_t *visited;
    Array<SolarSystem *> *systems;

    static Galaxy *gGalaxy;
};

#endif
