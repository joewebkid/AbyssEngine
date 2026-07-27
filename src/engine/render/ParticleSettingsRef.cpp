#include "engine/render/ParticleSettingsRef.h"

void ParticleSettingsRef::initialize() {
    // Android HD 0xe3964 initialises both static objects, then writes 42.
    // `init` remains the baseline while `cur` is the live table.
    cur.init();
    init.init();
    assertInit = 0x2a;
}

int ParticleSettingsRef::assertInit;
ParticleSettings ParticleSettingsRef::cur;
ParticleSettings ParticleSettingsRef::init;
