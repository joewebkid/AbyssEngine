#ifndef GOF2_CUTSCENE_H
#define GOF2_CUTSCENE_H
#include <cstddef>
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/math/Vector.h"


class AEGeometry;
class Level;
class PlayerEgo;
class TargetFollowCamera;


class CutScene {
public:
    Level *level;
    float cameraRotX;
    Vector vec8;
    float vec8w;
    int shipPosY;
    int shipPosZ;
    AEGeometry *rootGeom;
    float rotationSpeed;
    AEGeometry *geom28;
    AEGeometry *geom2c;
    AEGeometry *geom30;
    AEGeometry *geom34;
    Array<AEGeometry *> *geometries;
    uint64_t renderAtTime;
    uint64_t prevTime;
    uint64_t accum;
    uint32_t frameDelta;
    uint8_t initialized;
    PlayerEgo *player;
    AEGeometry *turretGeom;
    TargetFollowCamera *followCamera;
    uint32_t cameraId6c;
    uint32_t cameraId70;
    uint32_t cameraId74;
    uint32_t transformId78;
    int animTimer7c;
    int animTimer80;
    int fogTimer84;
    int mode;

    CutScene(int mode);

    ~CutScene();

    uint8_t isInitialized();

    void update();

    void update(int delta);

    void render3D();

    void render2D();

    void renderBG();

    void process(int delta);

    void replacePlayerShip(int a, int b);

    void initialize();

    void resetCamera();

    void checkForTurret();
};

#if defined(__arm__) && !defined(__aarch64__)
static_assert(offsetof(CutScene, renderAtTime) == 0x40, "CutScene time alignment");
static_assert(offsetof(CutScene, prevTime) == 0x48, "CutScene previous time offset");
static_assert(offsetof(CutScene, accum) == 0x50, "CutScene accumulator offset");
static_assert(offsetof(CutScene, frameDelta) == 0x58, "CutScene frame delta offset");
static_assert(offsetof(CutScene, initialized) == 0x5c, "CutScene initialized offset");
static_assert(offsetof(CutScene, followCamera) == 0x68, "CutScene camera offset");
static_assert(offsetof(CutScene, mode) == 0x88, "CutScene mode offset");
#endif

#endif
