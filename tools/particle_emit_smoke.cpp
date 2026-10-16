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
    float uv[3][4]{};
    Vector emittedVelocity{};
    float life = 0, size0 = 0, size1 = 0;
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
    void setParticle(const Vector &position, float lifetime, uint32_t,
                     float u0, float u1, float v0, float v1,
                     bool, float startSize, float endSize, const Vector &velocity) override {
        positions[submissions] = position;
        uv[submissions][0] = u0;
        uv[submissions][1] = u1;
        uv[submissions][2] = v0;
        uv[submissions][3] = v1;
        ++submissions;
        life = lifetime;
        size0 = startSize;
        size1 = endSize;
        emittedVelocity = velocity;
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

    // ARM 0x181f62-0x181f92 retains the fractional interval; 0x1826c4-0x1826cc wraps the ring.
    def = {};
    def.flLifetime = 5.0f;
    def.lifeBase = 100.0f;
    def.oneShot = 1;
    EmitProbe rateBased(&transform);
    rateBased.flags = 0x20;
    rateBased.emit(100);
    assert(rateBased.submissions == 0 && rateBased.updates == 0);
    assert(rateBased.emitTimer == 100.0f && rateBased.currentParticle == 0);
    rateBased.emit(200);
    assert(rateBased.submissions == 1 && rateBased.emitTimer == 100.0f);
    assert(rateBased.currentParticle == 1 && rateBased.ages[0] == 0);
    rateBased.emit(100);
    assert(rateBased.submissions == 2 && rateBased.emitTimer == 0.0f);
    assert(rateBased.currentParticle == 2 && rateBased.ages[1] == 0);
    rateBased.emit(200);
    assert(rateBased.submissions == 3 && rateBased.updates == 3);
    assert(rateBased.emitTimer == 0.0f && rateBased.currentParticle == 0);
    assert(rateBased.ages[2] == 0);

    def = {};
    def.count = 1;
    def.lifeBase = 100;
    def.lifeRandom = 7;
    def.startSize = 1;
    def.endSize = 2;
    def.velocityFromSlot = 1;
    def.velSpread = 4;
    def.velBaseX = 3;
    def.velBaseY = 4;
    def.velBaseZ = 5;
    def.posRight = 5;
    EmitProbe randomPosition(&transform);
    randomPosition.flags = 0x80;
    randomPosition.random.setSeed(123);
    AERandom expectedRandom(123);
    Vector expectedVelocity{
        3.0f + float(expectedRandom.nextInt(8) - 4),
        4.0f + float(expectedRandom.nextInt(8) - 4),
        5.0f + float(expectedRandom.nextInt(8) - 4)};
    expectedRandom.nextInt(10000); // Phase draw precedes random position.
    Vector expectedPosition{
        float(expectedRandom.nextInt(10) - 5),
        float(expectedRandom.nextInt(10) - 5),
        float(expectedRandom.nextInt(10) - 5)};
    float expectedLife = 100.0f + float(expectedRandom.nextInt(7));
    float expectedStartSize = 1.0f + float(expectedRandom.nextInt(7));
    float expectedEndSize = 2.0f + float(expectedRandom.nextInt(7));
    randomPosition.emit(200);
    assert(randomPosition.submissions == 1 && randomPosition.updates == 1);
    assert(randomPosition.positions[0].x == expectedPosition.x);
    assert(randomPosition.positions[0].y == expectedPosition.y);
    assert(randomPosition.positions[0].z == expectedPosition.z);
    assert(randomPosition.velocities[0].x == expectedVelocity.x);
    assert(randomPosition.velocities[0].y == expectedVelocity.y);
    assert(randomPosition.velocities[0].z == expectedVelocity.z);
    assert(randomPosition.emittedVelocity.x == expectedVelocity.x);
    assert(randomPosition.emittedVelocity.y == expectedVelocity.y);
    assert(randomPosition.emittedVelocity.z == expectedVelocity.z);
    assert(randomPosition.life == expectedLife);
    assert(randomPosition.size0 == expectedStartSize);
    assert(randomPosition.size1 == expectedEndSize);
    assert(randomPosition.updateDeltas[0] == 0.0f);
    assert(randomPosition.random.seedLow == expectedRandom.seedLow);
    assert(randomPosition.random.seedHigh == expectedRandom.seedHigh);
    randomPosition.emit(200);
    assert(randomPosition.submissions == 1);
    assert(randomPosition.random.seedLow == expectedRandom.seedLow);
    assert(randomPosition.random.seedHigh == expectedRandom.seedHigh);

    def = {};
    def.count = 1;
    def.posRight = 1;
    def.posUp = 2;
    def.posDir = 3;
    def.posDirRandom = 4;
    def.posSpread = 5;
    def.ySpread = 6;
    EmitProbe spreadPosition(&transform);
    spreadPosition.random.setSeed(456);
    AERandom spreadRandom(456);
    float phase = float(spreadRandom.nextInt(10000)) * 0.0001f;
    int direction = spreadRandom.nextInt(4);
    int xSpread = spreadRandom.nextInt(10) - 5;
    int zSpread = spreadRandom.nextInt(10) - 5;
    int ySpread = spreadRandom.nextInt(12) - 6;
    spreadPosition.emit(200);
    assert(spreadPosition.submissions == 1 && spreadPosition.updates == 1);
    assert(std::fabs(spreadPosition.positions[0].x - (2.0f * (phase - 1.0f) + 1.0f + xSpread)) < 0.01f);
    assert(spreadPosition.positions[0].y == 2.0f + ySpread);
    assert(spreadPosition.positions[0].z == 3.0f + direction + zSpread);
    assert(spreadPosition.random.seedLow == spreadRandom.seedLow);
    assert(spreadPosition.random.seedHigh == spreadRandom.seedHigh);

    def = {};
    def.count = 1;
    def.oneShot = 1;
    def.uvU0 = 0.1f;
    def.uvU1 = 0.2f;
    def.uvV0 = 0.3f;
    def.uvV1 = 0.4f;
    EmitProbe rotatedUV(&transform);
    rotatedUV.flags = 0x02000000u;
    rotatedUV.currentParticle = 1;
    float originalUV[4]{def.uvU0, def.uvU1, def.uvV0, def.uvV1};
    float rotated[4];
    rotatedUV.rotateUVs(originalUV, 1, rotated);
    rotatedUV.emit(200);
    assert(rotatedUV.submissions == 1);
    assert(rotatedUV.uv[0][0] == rotated[0]);
    assert(rotatedUV.uv[0][1] == rotated[2]);
    assert(rotatedUV.uv[0][2] == rotated[1]);
    assert(rotatedUV.uv[0][3] == rotated[3]);

    EmitProbe unrelatedHighBit(&transform);
    unrelatedHighBit.flags = 0x80000000u;
    unrelatedHighBit.currentParticle = 1;
    unrelatedHighBit.emit(200);
    assert(unrelatedHighBit.submissions == 1);
    assert(unrelatedHighBit.uv[0][0] == def.uvU0);
    assert(unrelatedHighBit.uv[0][1] == def.uvV0);
    assert(unrelatedHighBit.uv[0][2] == def.uvU1);
    assert(unrelatedHighBit.uv[0][3] == def.uvV1);

    float uvSource[4]{0.125f, 0.25f, 0.5f, 0.75f};
    for (int seed = -128; seed < 128; ++seed) {
        AERandom expectedUVRandom((long long) seed);
        unsigned value = (unsigned) expectedUVRandom.nextInt(40000);
        unsigned invertedLowBits = ~(value & 3u);
        float result[4]{};
        assert(rotatedUV.rotateUVs(uvSource, seed, result) == result);
        assert(result[0] == uvSource[value & 1]);
        assert(result[1] == uvSource[invertedLowBits & 1]);
        assert(result[2] == uvSource[2 + ((value >> 1) & 1)]);
        assert(result[3] == uvSource[2 + ((invertedLowBits >> 1) & 1)]);
    }

    EmitProbe oneShot(&transform);
    oneShot.flags = 0x40;
    oneShot.emit(200);
    assert(oneShot.submissions == 1 && oneShot.emitEnabled == 0);
    oneShot.emit(200);
    assert(oneShot.submissions == 1);
}
