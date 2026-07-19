#include "engine/render/ParticleSettingsRef.h"
#include "engine/render/ParticleSettings.h"

static ParticleSettings *g_PSR_settingsA = nullptr;
static ParticleSettings *g_PSR_settingsB = nullptr;
static int g_PSR_counter = 0;

void ParticleSettingsRef::initialize() {
    g_PSR_settingsA->init();
    g_PSR_settingsB->init();
    g_PSR_counter = 0x2a;
}

// Static data members present in the original binary (defined for symbol parity).
int ParticleSettingsRef::assertInit;
unsigned char ParticleSettingsRef::cur[7680];
unsigned char ParticleSettingsRef::init[7680];
