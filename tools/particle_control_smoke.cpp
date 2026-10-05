#include "engine/render/IParticleSystem.h"
#include "engine/render/ParticleSystemManager.h"
#include "engine/render/ParticleSettingsRef.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>

struct Probe : IParticleSystem {
    int resets = 0, submissions = 0;
    bool changeSlotOnSubmit = false;
    Vector position{}, velocity{};
    float life = 0, size0 = 0, size1 = 0, uv[4]{};
    uint32_t color = 0;
    bool masked = false;
    Vector velocities[2]{};
    int ages[2]{-1, -1};
    int8_t ids[2]{-1, -1};

    Probe() {
        emitEnabled = renderEnabled = updateEnabled = 1;
        flags = 0; currentParticle = 0; maxParticles = 2;
        emitterVelocity = Vector{1, 2, 3};
        particleVelocities = velocities; particleAges = ages; particleSetIds = ids;
        particleSets.data_[0] = ParticleSettings::ParticleSet_dummy;
        particleSets.size_ = 1;
    }
    int init(uint32_t, uint16_t) override { return 0; }
    void emit(int) override {}
    void reset() override { ++resets; }
    void release() override {}
    int getQuadCount() override { return 0; }
    void updateSingle(int, float) override {}
    void setParticle(const Vector &p, float l, uint32_t c,
                     float u0, float u1, float v0, float v1, bool m,
                     float a, float b, const Vector &v) override {
        ++submissions; position = p; velocity = v;
        life = l; color = c; masked = m; size0 = a; size1 = b;
        uv[0] = u0; uv[1] = u1; uv[2] = v0; uv[3] = v1;
        if (changeSlotOnSubmit) currentParticle = 1;
    }
};

static bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }

int main() {
    Probe sprite, mesh;
    // Use the real constructor; resources stay uninitialized in this focused probe.
    // Do not invoke the GPU/resource destructor. The process owns this one manager.
    auto *manager = new ParticleSystemManager(nullptr, ParticleSettings::CameraSet_dummy,
                                             0, false, 0, false);
    manager->spriteSystems.data_[0] = reinterpret_cast<ParticleSystemSprite *>(&sprite);
    manager->spriteSystems.size_ = 1;
    reinterpret_cast<IParticleSystem **>(manager->meshSystems)[0] = &mesh;
    manager->meshSystemCount = 1;

    const int handles[]{0, 0x4000, 0x40004000};
    for (int handle : handles) {
        Probe &p = handle == 0 ? sprite : mesh;
        manager->setParticleSetByIndex(handle, 7);
        assert(p.particleSetIndex == 7);
        manager->enableSystemUpdate(handle, false); assert(p.updateEnabled == 0);
        manager->enableSystemUpdate(handle, true); assert(p.updateEnabled == 1);
        p.emitTimer = 42; p.emitEnabled = 0;
        manager->enableSystemEmit(handle, true); assert(p.emitTimer == 0);
        p.emitTimer = 42;
        manager->enableSystemEmit(handle, true); assert(p.emitTimer == 42);
        manager->enableSystemEmit(handle, false); assert(p.emitTimer == 42);
        p.renderEnabled = 1;
        int before = p.resets;
        manager->enableSystemRender(handle, false); assert(p.resets == before + 1);
        manager->enableSystemRender(handle, false); assert(p.resets == before + 1);
        manager->enableSystemRender(handle, true); assert(p.renderEnabled == 1);
        manager->resetSystem(handle); assert(p.resets == before + 2);
    }
    manager->setParticleSetByIndex(-1, 0);
    manager->enableSystemUpdate(-1, false);
    manager->enableSystemEmit(-1, false);
    manager->enableSystemRender(-1, false);
    manager->resetSystem(-1);

    auto &def = ParticleSettingsRef::cur.sets[0];
    def.lifeBase = 12; def.startSize = 3; def.endSize = 4;
    def.color0 = 0x12345678; def.fadeFrames = 1; def.velocityFromSlot = 2;
    def.uvU0 = 0.1f; def.uvU1 = 0.2f; def.uvV0 = 0.3f; def.uvV1 = 0.4f;
    Vector pos{10, 20, 30}, vel{2, 4, 6};
    manager->emitManual(-1, pos, 0, -1);
    manager->emitManual(-1, pos, 0, vel, -1);
    assert(sprite.submissions == 0 && mesh.submissions == 0);
    manager->emitManual(0, pos, -1, -1); assert(sprite.submissions == 0);
    manager->emitManual(0, pos, 0, -1);
    assert(sprite.submissions == 1 && sprite.ages[0] == 0 && sprite.ids[0] == 0);
    assert(sprite.life == 12 && sprite.size0 == 3 && sprite.size1 == 4);
    assert(sprite.color == def.color0 && sprite.masked && sprite.currentParticle == 1);
    assert(near(sprite.uv[0], 0.1f) && near(sprite.uv[1], 0.3f));
    assert(near(sprite.uv[2], 0.2f) && near(sprite.uv[3], 0.4f));

    def.drag = 0.5f;
    manager->emitManual(0x4000, pos, 0, vel, 9);
    assert(mesh.life == 9 && near(mesh.velocity.x, -2));
    assert(near(mesh.velocities[0].x, 0) && near(mesh.velocities[0].y, 0));
    assert(near(mesh.velocities[0].z, 0));
    mesh.changeSlotOnSubmit = true; mesh.currentParticle = 0;
    manager->emitManual(0x4000, pos, 0, -1);
    assert(mesh.currentParticle == 0 && near(mesh.velocities[1].x, 1));

    def.drag = 0; sprite.flags = 0x02000000; sprite.currentParticle = 1;
    manager->emitManual(0, pos, 0, -1);
    float original[4]{def.uvU0, def.uvU1, def.uvV0, def.uvV1}, rotated[4];
    float *expected = sprite.rotateUVs(original, 1, rotated);
    assert(near(sprite.uv[0], expected[0]) && near(sprite.uv[1], expected[2]));
    assert(near(sprite.uv[2], expected[1]) && near(sprite.uv[3], expected[3]));
    sprite.flags = 0x80000000; sprite.currentParticle = 1;
    manager->emitManual(0, pos, 0, -1);
    assert(near(sprite.uv[0], def.uvU0) && near(sprite.uv[1], def.uvV0));

    def.lifeRandom = 8;
    sprite.random.setSeed(123);
    AERandom expectedRandom(123);
    float expectedLife = 12 + expectedRandom.nextInt(8);
    float expectedSize0 = 3 + expectedRandom.nextInt(8);
    float expectedSize1 = 4 + expectedRandom.nextInt(8);
    manager->emitManual(0, pos, 0, -1);
    assert(sprite.life == expectedLife && sprite.size0 == expectedSize0);
    assert(sprite.size1 == expectedSize1);
    assert(sprite.random.seedLow == expectedRandom.seedLow);
    def.lifeRandom = 0;
    manager->emitManual(0, pos, 0, std::numeric_limits<float>::quiet_NaN());
    assert(sprite.life == def.lifeBase);

    def.velSpread = 4; def.velBaseY = 7; def.posSpread = 5; def.ySpread = 6;
    def.drag = 0.5f; sprite.emitterVelocity = Vector{1, 2, 3};
    sprite.currentParticle = 0; sprite.random.setSeed(567);
    AERandom spreadRandom(567);
    Vector expectedVelocity;
    expectedVelocity.x = float(spreadRandom.nextInt(8) - 4);
    expectedVelocity.y = 7 + float(spreadRandom.nextInt(8) - 4);
    expectedVelocity.z = float(spreadRandom.nextInt(8) - 4);
    Vector expectedPosition = pos;
    expectedPosition.x += float(spreadRandom.nextInt(10) - 5);
    expectedPosition.z += float(spreadRandom.nextInt(10) - 5);
    expectedPosition.y += float(spreadRandom.nextInt(12) - 6);
    manager->emitManual(0, pos, 0, vel, -1);
    assert(near(sprite.position.x, expectedPosition.x));
    assert(near(sprite.position.y, expectedPosition.y));
    assert(near(sprite.position.z, expectedPosition.z));
    assert(near(sprite.velocity.x, 2 * (expectedVelocity.x - 1)));
    assert(near(sprite.velocity.y, 2 * (expectedVelocity.y - 2)));
    assert(near(sprite.velocity.z, 2 * (expectedVelocity.z - 3)));
    assert(near(sprite.velocities[0].x, expectedVelocity.x));
    assert(near(sprite.velocities[0].y, expectedVelocity.y));
    assert(near(sprite.velocities[0].z, expectedVelocity.z));
    assert(sprite.random.seedLow == spreadRandom.seedLow);
    std::puts("Particle control host smoke: OK");
}
