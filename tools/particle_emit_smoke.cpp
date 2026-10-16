#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSettingsRef.h"
#include <cassert>
#include <cmath>

struct EmitProbe : IParticleSystem {
    Vector velocities[3]{};
    int ages[3]{-1, -1, -1};
    int8_t ids[3]{-1, -1, -1};
    float updateDeltas[3]{};
    Vector positions[3]{};
    int updates = 0;
    int submissions = 0;

    explicit EmitProbe(const Matrix *transform) {
        matrix = transform;
        emitEnabled = renderEnabled = 1;
        flags = 0;
        maxParticles = 3;
        currentParticle = 0;
        emitTimer = 0;
        mirror = 0;
        emitterVelocity = Vector{10.0f, 0.0f, 0.0f};
        particleVelocities = velocities;
        particleAges = ages;
        particleSetIds = ids;
        particleSets.data_[0] = ParticleSettings::ParticleSet_dummy;
        particleSets.size_ = 1;
        particleSetIndex = 0;
    }

    int init(uint32_t, uint16_t) override { return 0; }
    void reset() override {}
    void release() override {}
    int getQuadCount() override { return 0; }
    void updateSingle(int, float delta) override { updateDeltas[updates++] = delta; }
    void setParticle(const Vector &position, float, uint32_t, float, float, float, float,
                     bool, float, float, const Vector &) override {
        positions[submissions++] = position;
    }
};

int main() {
    const Matrix transform{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0};
    auto &def = ParticleSettingsRef::cur.sets[0];
    def = {};
    def.count = 2;
    def.oneShot = 1; // deterministic phase = i + 1
    def.lifeBase = 100;
    def.startSize = def.endSize = 1;

    EmitProbe probe(&transform);
    probe.emit(200);
    assert(probe.updates == 2);
    assert(probe.submissions == 2);
    assert(probe.currentParticle == 2);
    assert(std::fabs(probe.positions[0].x + 1.0f) < 0.01f);
    assert(std::fabs(probe.positions[1].x) < 0.01f);

    // ARM reuses S17 for the emission step before calculating update time.
    assert(std::fabs(probe.updateDeltas[0] - 100.0f) < 0.5f);
    assert(probe.updateDeltas[1] == 0.0f);

    EmitProbe shorterFrame(&transform);
    shorterFrame.emit(150);
    assert(shorterFrame.updates == 2);
    assert(std::fabs(shorterFrame.updateDeltas[0] - 75.0f) < 0.5f);
    assert(std::fabs(shorterFrame.positions[0].x + 0.75f) < 0.01f);

    def.flLifetime = 0.75f;
    EmitProbe fixedSpacing(&transform);
    fixedSpacing.flags = 0x10;
    fixedSpacing.emit(200);
    assert(fixedSpacing.updates == 2);
    assert(std::fabs(fixedSpacing.updateDeltas[0] - 75.0f) < 0.5f);
    assert(std::fabs(fixedSpacing.positions[0].x + 1.25f) < 0.01f);
}
