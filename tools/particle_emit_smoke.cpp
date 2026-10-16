#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSettingsRef.h"
#include <cassert>
#include <cmath>

struct EmitProbe : IParticleSystem {
    Vector velocities[3]{};
    int ages[3]{-1, -1, -1};
    int8_t ids[3]{-1, -1, -1};
    float updateDeltas[3]{};
    int updates = 0;

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
    void setParticle(const Vector &, float, uint32_t, float, float, float, float,
                     bool, float, float, const Vector &) override {}
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
    assert(probe.currentParticle == 2);

    // ARM: S17 is the speed Newton factor and S26 is its inverse-speed
    // estimate. Travel distance must not enter this update time.
    float speed2 = 100.0f;
    union { float f; int i; } bits{speed2};
    bits.i = 0x5f3759df - (bits.i >> 1);
    float correction = speed2 * -0.5f * bits.f * bits.f + 1.5f;
    float expected = (correction * bits.f) * correction * 1000.0f;
    assert(std::fabs(probe.updateDeltas[0] - expected) < 0.001f);
    assert(probe.updateDeltas[1] == 0.0f);

    EmitProbe shorterFrame(&transform);
    shorterFrame.emit(150);
    assert(shorterFrame.updates == 2);
    assert(std::fabs(shorterFrame.updateDeltas[0] - expected) < 0.001f);
}
