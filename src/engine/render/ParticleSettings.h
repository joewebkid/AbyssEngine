#ifndef GOF2_PARTICLESETTINGS_H
#define GOF2_PARTICLESETTINGS_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include <type_traits>

class ParticleSettings {
public:
    enum ParticleSet {
        ParticleSet_dummy = 0,
        ParticleSet_4 = 4, ParticleSet_7 = 7, ParticleSet_8 = 8, ParticleSet_9 = 9,
        ParticleSet_0xa = 0xa, ParticleSet_0xb = 0xb, ParticleSet_0xc = 0xc, ParticleSet_0xf = 0xf,
        ParticleSet_0x14 = 0x14, ParticleSet_0x15 = 0x15, ParticleSet_0x16 = 0x16,
        ParticleSet_0x17 = 0x17, ParticleSet_0x18 = 0x18, ParticleSet_0x19 = 0x19,
        ParticleSet_0x1a = 0x1a, ParticleSet_0x1b = 0x1b, ParticleSet_0x1c = 0x1c,
        ParticleSet_0x27 = 0x27, ParticleSet_0x2a = 0x2a, ParticleSet_0x2f = 0x2f
    };

    enum CameraSet { CameraSet_dummy = 0, CameraSet_1 = 1 };

    struct {
        String name;
        uint32_t flags;
        int32_t count;
        uint32_t lifeBase;
        int32_t lifeRandom;
        uint32_t startSize;
        uint32_t endSize;
        uint32_t velocityFromSlot;
        int32_t lifetime;
        uint32_t flLifetime;
        uint32_t oneShot;
        uint32_t color0;
        uint32_t color1;
        int32_t fadeFrames;
        uint32_t colorFlag;
        int32_t posBase;
        int32_t posSpread;
        int32_t ySpread;
        int32_t velSpread;
        uint32_t field_0x54;
        uint32_t velBaseX;
        uint32_t velBaseY;
        uint32_t velBaseZ;
        uint32_t drag;
        uint32_t velRight;
        uint32_t velUp;
        uint32_t velDir;
        uint32_t field_0x74;
        uint32_t posRight;
        uint32_t posUp;
        uint32_t posDir;
        uint32_t posDirRandom;
        uint32_t uvU0;
        uint32_t uvV0;
        uint32_t uvU1;
        uint32_t uvV1;
        int32_t speedThreshold;
        int32_t frames;
    } sets[48];

    using SetDefinition = std::remove_reference<decltype(sets[0])>::type;

    ParticleSettings();

    ~ParticleSettings();

    int init();

    void multiplyAll(float scale);

    void Interpolate(ParticleSet a, ParticleSet b, float t, ParticleSet out);

    // Static data members present in the original binary (defined for symbol parity).
    static int particleMultiply;
    static int pCounter;
};
#endif
