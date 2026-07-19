#ifndef GOF2_LEVELSCRIPT_H
#define GOF2_LEVELSCRIPT_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/ship/TargetFollowCamera.h"

#include "engine/math/Matrix.h"


class Radar;


class AEGeometry;
class Explosion;
class Hud;
class Level;
class TargetFollowCamera;


class LevelScript {
public:
    int m_nTimeLimit;
    int field_0x4;
    int field_0x8;
    int field_0xc;
    uint16_t m_nFlags;
    uint16_t field_0x12;
    TargetFollowCamera *m_pCamera;
    Level *m_pLevel;
    int m_nState;
    uint16_t m_bStartSequence;
    uint8_t m_bStartSequenceOver;
    int field_0x24;
    int field_0x28;
    int field_0x2c;
    int field_0x30;
    int field_0x34;
    int field_0x38;
    int field_0x3c;
    int field_0x40;
    int field_0x44;
    int field_0x48;
    Matrix m_matrix;
    int field_0x8c;
    int m_nScriptTimerA;
    int m_nScriptCounterA;
    int m_nScriptTimerB;
    int m_nScriptCounterB;
    int field_0xa0;
    int field_0xa4;
    uint8_t m_bRenderParticles;
    AEGeometry *m_pParticleGeom0;
    AEGeometry *m_pParticleGeom1;
    AEGeometry *m_pParticleGeom2;
    AEGeometry *m_pGeometry0;
    AEGeometry *m_pGeometry1;
    AEGeometry *m_pGeometry2;
    AEGeometry *m_pGeometry3;
    Explosion *m_pExplosion;
    AEGeometry *m_pGeometry4;
    Hud *m_pHud;
    Radar *m_pRadar;
    AEGeometry *m_pGeometry5;
    AEGeometry *m_pGeometry6;

    LevelScript(Level *level, Hud *hud, Radar *radar, TargetFollowCamera *camera);

    ~LevelScript();

    void render3D();

    uint8_t startSequenceOver();

    void resetCamera(Level *level);

    void skipSequence();

    uint8_t startSequence();

    void setAutoPilotToProgrammedStation();

    uint32_t canSkipCutsceneNow();

    void resetStartSequenceOver();

    void skipCutscene();

    void process(int delta);

    void lookBehind();

    int getEvent();

    void setEvent(int event);
};
#endif
