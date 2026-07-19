#ifndef GOF2_TRACTORBEAM_H
#define GOF2_TRACTORBEAM_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/KIPlayer.h"

class Radar;


class AEGeometry;
class Hud;
class KIPlayer;
class Level;


class TractorBeam {
public:
    float dirX;
    float dirY;
    float dirZ;
    KIPlayer *grabbedCrate;
    uint8_t active;
    uint8_t soundPlaying;
    AEGeometry *beamGeometry;
    int storedHitpoints;

    TractorBeam(AEGeometry *unused, int kind);

    ~TractorBeam();

    void render();

    void update(int frameTime, Radar *radar, Level *level, Hud *hud);
};

#endif
