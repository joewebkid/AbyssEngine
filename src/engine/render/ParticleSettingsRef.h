#ifndef GOF2_PARTICLESETTINGSREF_H
#define GOF2_PARTICLESETTINGSREF_H

#include "engine/render/ParticleSettings.h"

class ParticleSettingsRef {
public:
    static void initialize();

    // Android owns two real 0x1e00-byte ParticleSettings objects here. Every
    // particle consumer addresses their fixed 0xa0-byte ARM slots directly.
    static int assertInit;
    static ParticleSettings cur;
    static ParticleSettings init;
};

#endif
