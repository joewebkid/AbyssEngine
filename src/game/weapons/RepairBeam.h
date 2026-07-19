#ifndef GOF2_REPAIRBEAM_H
#define GOF2_REPAIRBEAM_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Radar.h"

#include "engine/math/Vector.h"
#include "engine/render/AEGeometry.h"

#include "engine/math/AEMath.h"


class Radar;


class AEGeometry;
class Hud;
class Level;


class RepairBeam {
public:
    int shipIndex;
    AbyssEngine::AEMath::Vector beamPosition;
    Array<AEGeometry *> *geometries;
    Array<int> *targetIds;
    Array<float> *charges;
    int sort;
    int timer;

    RepairBeam(int shipIndex, int sort);

    ~RepairBeam();

    void render();

    void update(int dt, Radar *radar, Level *level, Hud *hud);
};
#endif
