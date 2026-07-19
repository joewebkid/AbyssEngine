#ifndef GOF2_CUTSCENE_H
#define GOF2_CUTSCENE_H
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
    uint32_t renderAtTimeLo;
    uint32_t renderAtTimeHi;
    uint32_t prevTimeLo;
    uint32_t prevTimeHi;
    uint32_t accumLo;
    uint32_t accumHi;
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

#endif
