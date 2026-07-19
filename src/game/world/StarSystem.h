#ifndef GOF2_STARSYSTEM_H
#define GOF2_STARSYSTEM_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/render/LensFlare.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"
class AEGeometry;
class LensFlare;


class StarSystem {
public:
    Vector sunLightColor;
    uint8_t supernovaSystem;
    uint32_t supernovaSunTexture;
    Array<uint32_t> *texturesArray;
    void *playerTargets;
    Array<AEGeometry *> *planetsArray;
    Array<Vector> *positionsArray;
    Array<int> *stationIdxArray;
    uint8_t abstractSystem;
    LensFlare *lensFlare;
    Vector lightDirection;
    uint32_t tintColor;
    AEGeometry *sunStreak;
    AEGeometry *planetRing;
    uint32_t planetRingTexture;
    int planetRingIndex;
    uint32_t selectedStationSlot;
    uint8_t fogEnabled;
    float planetScale;
    float planetRingScaleOffset;

    static int orbitPlanetIndex;

    StarSystem(int mode);

    ~StarSystem();

    void *getPlanetTargets();

    void *getPlanets();

    Array<int> *getStationIndices();

    float getPlanetScaleFactor();

    Vector getLightDirection();

    void initLight();

    void rotate(int x, int y, int z);

    void render();

    void render2D();

    void renderSunStreak();

    void scaleSunDuringSupernovaIntro(int amount);

    void switchPlanetForIntro();

    void switchSunForSupernovaExpansion();

    void switchSunForSupernovaIntro();

    void switchSunForSupernovaReversal();

    void updateSupernova(int dt);

};
#endif
