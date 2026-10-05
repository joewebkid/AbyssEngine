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
    union {
        struct {
            int field_0x8;
            int field_0xc;
        };
        long long scriptTime;
    };
    union {
        uint16_t m_nFlags;
        struct {
            uint8_t flag_0x10;
            uint8_t cameraControlled; // Android process returns this byte.
        };
    };
    uint16_t field_0x12;
    TargetFollowCamera *m_pCamera;
    Level *m_pLevel;
    int m_nState;
    union {
        uint16_t sequenceState;
        struct {
            uint8_t m_bStartSequence;
            uint8_t m_bStartSequenceOver;
        };
    };
    uint8_t _sequence_padding[2];
    int field_0x24;
    union {
        struct {
            int field_0x28;
            int field_0x2c;
            int field_0x30;
        };
        Vector vector_0x28;
    };
    union {
        struct {
            int field_0x34;
            int field_0x38;
            int field_0x3c;
        };
        Vector vector_0x34;
    };
    union {
        struct {
            int field_0x40;
            int field_0x44;
            int field_0x48;
        };
        Vector vector_0x40;
    };
    Matrix m_matrix;
    int field_0x88;
    int field_0x8c;
    union {
        struct {
            int m_nScriptTimerA;
            int m_nScriptCounterA;
        };
        long long scriptTimerA;
    };
    union {
        struct {
            int m_nScriptTimerB;
            int m_nScriptCounterB;
        };
        struct {
            uint16_t timerBLo;
            uint16_t timerBHi;
        };
        long long scriptTimerB;
    };
    union {
        struct {
            int field_0xa0;
            int field_0xa4;
        };
        long long scriptTimerC;
    };
    uint8_t m_bRenderParticles;
    uint8_t flag_0xa9;
    uint8_t savedAutoTurret;
    uint8_t savedTurretMode;
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
    int particleSystem_0xe0;
    int particleSystem_0xe4;

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

    bool process(int delta);

    void lookBehind();

    int getEvent();

    void setEvent(int event);
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(LevelScript, m_bStartSequence) == 0x20,
              "Android start-sequence byte is +0x20");
static_assert(__builtin_offsetof(LevelScript, m_bStartSequenceOver) == 0x21,
              "Android start-sequence-over byte is +0x21");
static_assert(__builtin_offsetof(LevelScript, m_matrix) == 0x4c,
              "Android script matrix is +0x4c");
static_assert(__builtin_offsetof(LevelScript, m_pHud) == 0xd0,
              "Android script HUD pointer is +0xd0");
static_assert(__builtin_offsetof(LevelScript, m_nScriptTimerA) == 0x90,
              "Android first script timer is +0x90");
static_assert(sizeof(LevelScript) == 0xe8,
              "Android LevelScript allocation is 0xe8 bytes");
#endif
#endif
