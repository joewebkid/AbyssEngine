#include "engine/render/ParticleSettingsRef.h"

#include <cmath>
#include <cstdio>

namespace {

bool sameFloat(float lhs, float rhs) {
    return std::fabs(lhs - rhs) < 0.0001f;
}

int fail(const char *message) {
    std::fprintf(stderr, "ParticleSettingsRef smoke failed: %s\n", message);
    return 1;
}

} // namespace

int main() {
    ParticleSettingsRef::initialize();
    if (ParticleSettingsRef::assertInit != 0x2a)
        return fail("initialize did not set assertInit to 42");

    const ParticleSettings::SetDefinition baseline0 = ParticleSettingsRef::init.sets[0];
    if (ParticleSettingsRef::cur.sets[0].count != 20 ||
        !sameFloat(ParticleSettingsRef::cur.sets[0].flLifetime, 20.0f) ||
        !sameFloat(ParticleSettingsRef::cur.sets[0].velDir, -4000.0f)) {
        return fail("set 0 defaults do not match the recovered Android body");
    }

    ParticleSettings::multiplyAll(2.0f);
    if (ParticleSettingsRef::cur.sets[0].count != baseline0.count * 2 ||
        !sameFloat(ParticleSettingsRef::cur.sets[0].flLifetime, baseline0.flLifetime * 0.5f) ||
        ParticleSettingsRef::init.sets[0].count != baseline0.count ||
        !sameFloat(ParticleSettingsRef::init.sets[0].flLifetime, baseline0.flLifetime)) {
        return fail("multiplyAll did not preserve init as the Android baseline");
    }

    ParticleSettings::Interpolate(ParticleSettings::ParticleSet_dummy,
                                  ParticleSettings::ParticleSet_4, 0.25f,
                                  ParticleSettings::ParticleSet_0x2a);
    const ParticleSettings::SetDefinition &mixed = ParticleSettingsRef::cur.sets[0x2a];
    const float expectedLife = baseline0.lifeBase * 0.75f +
                               ParticleSettingsRef::init.sets[4].lifeBase * 0.25f;
    const float expectedEndSize = baseline0.endSize * 0.75f +
                                  ParticleSettingsRef::init.sets[4].endSize * 0.25f;
    const float expectedVelDir = baseline0.velDir * 0.75f +
                                 ParticleSettingsRef::init.sets[4].velDir * 0.25f;
    if (mixed.count != static_cast<int>(baseline0.count * 0.75f +
                                        ParticleSettingsRef::init.sets[4].count * 0.25f) ||
        !sameFloat(mixed.lifeBase, expectedLife) ||
        !sameFloat(mixed.endSize, expectedEndSize) ||
        !sameFloat(mixed.velDir, expectedVelDir)) {
        return fail("Interpolate did not use init as the Android source table");
    }

    std::puts("ParticleSettingsRef ABI/runtime smoke: OK");
    return 0;
}
