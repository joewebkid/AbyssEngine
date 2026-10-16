#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>

namespace AbyssEngine { namespace AEMath {
struct Vector {
    float x, y, z;
    Vector &operator*=(float scale) {
        x *= scale; y *= scale; z *= scale; return *this;
    }
    Vector &operator+=(const Vector &other) {
        x += other.x; y += other.y; z += other.z; return *this;
    }
};
struct Matrix { Vector position; };
Vector operator-(Vector a, Vector b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
Vector operator*(Vector v, float scale) {
    return {v.x * scale, v.y * scale, v.z * scale};
}
Vector MatrixGetPosition(const Matrix &matrix) { return matrix.position; }
float VectorSignedToFloat(int value, unsigned char) { return (float) value; }
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::Matrix;
using AbyssEngine::AEMath::VectorSignedToFloat;

constexpr float kMillisecondsToSeconds = 0.001f;
constexpr float kColorChannelScale = 1.0f / 255.0f;
static float g_uvRoundBias = 1.0f / 1024.0f;

struct ParticleSettings {
    struct SetDefinition {
        int lifetime = 100;
        int posBase = 1000;
        int frames = 0;
        float uvU0 = 0.1f, uvV0 = 0.2f, uvU1 = 0.6f, uvV1 = 0.8f;
        float posRight = 10, posUp = 6, posDir = 3, colorFlag = 1;
        uint32_t color0 = 0x21426384u;
        float velBaseX = 7, velBaseY = 8, velBaseZ = 9;
    } sets[1];
};
struct ParticleSettingsRef { static ParticleSettings cur; };
ParticleSettings ParticleSettingsRef::cur;
const ParticleSettings::SetDefinition &currentParticleSet(int index) {
    return ParticleSettingsRef::cur.sets[index];
}

struct PaintCanvas {
    struct Rgba { uint32_t resource; uint16_t id; float a, r, g, b; };
    struct Position { uint16_t id; float x, y, z; };
    struct Size { uint16_t id; int value; };
    struct Uv { uint16_t id; float u0, v0, u1, v1; };
    std::vector<Rgba> colors;
    std::vector<Position> positions, movements;
    std::vector<Size> sizes, sizeDeltas;
    std::vector<Uv> uvs;
    std::vector<char> order;
    Vector currentPosition{};
    void SpriteSystemSetRGBA(uint32_t resource, uint16_t id,
                             float a, float r, float g, float b) {
        colors.push_back({resource, id, a, r, g, b});
        order.push_back('C');
    }
    void SpriteSystemSetPosition(uint32_t, uint16_t id, float x, float y, float z) {
        currentPosition = {x, y, z};
        positions.push_back({id, x, y, z}); order.push_back('P');
    }
    void SpriteSystemSetSize(uint32_t, uint16_t id, short value) {
        sizes.push_back({id, value}); order.push_back('S');
    }
    void SpriteSystemAddSize(uint32_t, uint16_t id, short value) {
        sizeDeltas.push_back({id, value}); order.push_back('A');
    }
    void SpriteSystemSetUv(uint32_t, uint16_t id,
                           float u0, float v0, float u1, float v1) {
        uvs.push_back({id, u0, v0, u1, v1}); order.push_back('U');
    }
    void SpriteSystemAddPosition(uint32_t, uint16_t id, float x, float y, float z) {
        currentPosition.x += x; currentPosition.y += y; currentPosition.z += z;
        movements.push_back({id, x, y, z}); order.push_back('M');
    }
    void SpriteSystemGetPosition(uint32_t, uint16_t, Vector &out) {
        out = currentPosition; order.push_back('G');
    }
};

struct ScriptedRandom {
    int values[4]{1000, 1000, 1000, 1000};
    int calls = 0;
    int nextInt(int limit) {
        assert(limit == 2000 && calls < 4);
        return values[calls++];
    }
};

struct IParticleSystem {
    PaintCanvas *canvas = nullptr;
    uint32_t resource = 9;
    uint32_t resourceOffset = 100;
    uint8_t alphaFade = 0;
    uint8_t initialized = 0;
    uint32_t flags = 0;
    int currentParticle = 0;
    int ages[2]{0, 0};
    int *particleAges = ages;
    int8_t setIds[2]{0, 0};
    int8_t *particleSetIds = setIds;
    Vector velocities[2]{};
    Vector *particleVelocities = velocities;
    ScriptedRandom random;
    const Matrix *matrix = nullptr;
    int interpolateCalls = 0;
    virtual void reset() = 0;
    void interpolateColor(int, float &a, float &r, float &g, float &b) {
        ++interpolateCalls;
        a = 0.1f; r = 0.2f; g = 0.3f; b = 0.4f;
    }
    int rotateCalls = 0;
    float *rotateUVs(float *src, int, float *dst) {
        ++rotateCalls;
        dst[0] = src[1]; dst[1] = src[0];
        dst[2] = src[3]; dst[3] = src[2];
        return dst;
    }
};

struct ParticleSystemSprite : IParticleSystem {
    PaintCanvas recording;
    Matrix matrixValue{{2, 3, 4}};
    int resetCalls = 0;
    ParticleSystemSprite() { canvas = &recording; matrix = &matrixValue; }
    void reset() override { ++resetCalls; }
    void init(uint32_t spriteId, uint16_t idOffset);
    void setAlpha(int index, uint32_t color, float alpha);
    void updateSingle(int index, float dt);
    void updateAreaExitParticle(int index, float dt);
    void setParticle(const Vector &pos, float p2, uint32_t color,
                     float p4, float p5, float p6, float p7,
                     bool clearColor, float p9, float p10,
                     const Vector &velocity);
};

// PRODUCTION_METHODS

static bool near(float a, float b) { return std::fabs(a - b) < 0.0001f; }

int main() {
    ParticleSystemSprite initialized;
    initialized.init(42, 65535);
    assert(initialized.resource == 42 && initialized.resourceOffset == 65535);
    assert(initialized.initialized == 1 && initialized.resetCalls == 1);

    ParticleSystemSprite blueFade;
    blueFade.setAlpha(2, 0x21426384u, 0.5f);
    assert(blueFade.recording.colors.size() == 1);
    const auto &blue = blueFade.recording.colors[0];
    assert(blue.resource == 9 && blue.id == 102);
    assert(near(blue.a, 0x21 / 255.0f) && near(blue.r, 0x42 / 255.0f));
    assert(near(blue.g, 0x63 / 255.0f) && near(blue.b, 0x84 / 510.0f));

    ParticleSystemSprite alphaFade;
    alphaFade.alphaFade = 1;
    alphaFade.setAlpha(2, 0x21426384u, 0.5f);
    assert(alphaFade.recording.colors.size() == 1);
    const auto &alpha = alphaFade.recording.colors[0];
    assert(near(alpha.a, 0x21 / 510.0f) && near(alpha.r, 0x42 / 510.0f));
    assert(near(alpha.g, 0x63 / 510.0f) && near(alpha.b, 0x84 / 255.0f));

    alphaFade.resourceOffset = 65534;
    alphaFade.setAlpha(3, 0xffffffffu, 0.0f);
    assert(alphaFade.recording.colors.size() == 2);
    const auto &wrapped = alphaFade.recording.colors[1];
    assert(wrapped.id == 1 && wrapped.a == 0.0f && wrapped.r == 0.0f);
    assert(wrapped.g == 0.0f && near(wrapped.b, 1.0f));

    ParticleSystemSprite live;
    live.ages[0] = 90;
    live.velocities[0] = {100.0f, 200.0f, 300.0f};
    live.updateSingle(0, 10.0f);
    assert(live.ages[0] == 100 && live.interpolateCalls == 1);
    assert(live.recording.order == std::vector<char>({'A', 'C', 'M'}));
    assert(live.recording.sizeDeltas[0].id == 100);
    assert(live.recording.sizeDeltas[0].value == 10);
    assert(near(live.recording.colors[0].a, 0.1f));
    assert(near(live.recording.movements[0].x, 1.0f));
    assert(near(live.recording.movements[0].y, 2.0f));
    assert(near(live.recording.movements[0].z, 3.0f));
    live.recording.order.clear();
    live.updateSingle(0, 1.0f);
    assert(live.ages[0] == -1 && live.interpolateCalls == 1);
    assert(live.recording.order == std::vector<char>({'P', 'S'}));
    assert(live.recording.positions[0].id == 100);
    assert(live.recording.positions[0].x == 4294967296.0f);
    assert(live.recording.sizes[0].value == 0);

    ParticleSettingsRef::cur.sets[0].frames = 4;
    ParticleSystemSprite animated;
    animated.ages[0] = 25;
    animated.flags = 0x02000000u;
    animated.updateSingle(0, 1.0f);
    assert(animated.ages[0] == 26 && animated.rotateCalls == 1);
    assert(animated.recording.order == std::vector<char>({'A', 'C', 'U', 'M'}));
    assert(animated.recording.uvs.size() == 1 && animated.recording.uvs[0].id == 100);
    const auto &uv = animated.recording.uvs[0];
    assert(near(uv.u0, 1.1f) && near(uv.v0, 0.8f));
    assert(near(uv.u1, 0.6f) && near(uv.v1, 0.2f));
    ParticleSettingsRef::cur.sets[0].frames = 0;

    ParticleSystemSprite areaExit;
    areaExit.flags = 0x80;
    areaExit.ages[0] = 25;
    areaExit.recording.currentPosition = {13, 3, 4};
    areaExit.updateSingle(0, 1.0f);
    assert(areaExit.ages[0] == 25 && areaExit.random.calls == 4);
    assert(areaExit.recording.order == std::vector<char>({'M', 'G', 'C'}));
    assert(near(areaExit.recording.colors[0].b, 0.0f));

    ParticleSystemSprite outerEdge;
    outerEdge.recording.currentPosition = {12.02f, 3, 4};
    outerEdge.updateAreaExitParticle(0, 0.0f);
    assert(outerEdge.random.calls == 0);
    assert(outerEdge.recording.order == std::vector<char>({'M', 'G', 'C'}));
    assert(near(outerEdge.recording.colors[0].b, 0.0f));

    ParticleSystemSprite outerExact;
    outerExact.recording.currentPosition = {12, 3, 4};
    outerExact.updateAreaExitParticle(0, 0.0f);
    assert(outerExact.random.calls == 0);
    assert(near(outerExact.recording.colors[0].b, 0.0f));

    ParticleSystemSprite reborn;
    reborn.recording.currentPosition = {13, 3, 4};
    reborn.random.values[3] = 0;
    reborn.updateAreaExitParticle(0, 0.0f);
    assert(reborn.random.calls == 4);
    assert(reborn.recording.order == std::vector<char>({'M', 'G', 'C', 'P'}));
    assert(near(reborn.recording.currentPosition.x, 2));
    assert(near(reborn.recording.currentPosition.y, 3));
    assert(near(reborn.recording.currentPosition.z, 14));
    assert(near(reborn.velocities[0].x, 7));
    assert(near(reborn.velocities[0].y, 8));
    assert(near(reborn.velocities[0].z, 9));

    ParticleSystemSprite middleFade;
    middleFade.recording.currentPosition = {10, 3, 4};
    middleFade.updateAreaExitParticle(0, 0.0f);
    assert(middleFade.random.calls == 0);
    assert(middleFade.recording.order == std::vector<char>({'M', 'G', 'C'}));
    assert(near(middleFade.recording.colors[0].b,
                (0x84 / 255.0f) * (36.0f / 64.0f)));

    ParticleSystemSprite middleExact;
    middleExact.recording.currentPosition = {8, 3, 4};
    middleExact.updateAreaExitParticle(0, 0.0f);
    assert(near(middleExact.recording.colors[0].b, 0x84 / 255.0f));

    ParticleSystemSprite middleBand;
    middleBand.recording.currentPosition = {6, 3, 4};
    middleBand.updateAreaExitParticle(0, 0.0f);
    assert(near(middleBand.recording.colors[0].b, 0x84 / 255.0f));

    ParticleSystemSprite innerFade;
    innerFade.recording.currentPosition = {4, 3, 4};
    innerFade.updateAreaExitParticle(0, 0.0f);
    assert(near(innerFade.recording.colors[0].b,
                (0x84 / 255.0f) * (3.0f / 8.0f)));

    ParticleSystemSprite innerCore;
    innerCore.recording.currentPosition = {2.5f, 3, 4};
    innerCore.updateAreaExitParticle(0, 0.0f);
    assert(near(innerCore.recording.colors[0].b, 0.0f));

    ParticleSystemSprite spawn;
    spawn.resourceOffset = 65535;
    spawn.currentParticle = 1;
    const Vector unusedVelocity{1, 2, 3};
    spawn.setParticle({3, 4, 5}, 23.75f, 0x21426384u,
                      0.1f, 0.2f, 0.6f, 0.8f, false,
                      99.25f, 7.0f, unusedVelocity);
    assert(spawn.recording.order == std::vector<char>({'P', 'S', 'U', 'C'}));
    assert(spawn.recording.positions[0].id == 0);
    assert(spawn.recording.sizes[0].id == 0);
    assert(spawn.recording.sizes[0].value == 23);
    assert(near(spawn.recording.currentPosition.x, 3));
    assert(near(spawn.recording.currentPosition.y, 4));
    assert(near(spawn.recording.currentPosition.z, 5));
    assert(near(spawn.recording.uvs[0].u0, 0.1f));
    assert(near(spawn.recording.uvs[0].v0, 0.2f));
    assert(near(spawn.recording.uvs[0].u1, 0.6f));
    assert(near(spawn.recording.uvs[0].v1, 0.8f));
    assert(near(spawn.recording.colors[0].a, 0x21 / 255.0f));
    assert(near(spawn.recording.colors[0].r, 0x42 / 255.0f));
    assert(near(spawn.recording.colors[0].g, 0x63 / 255.0f));
    assert(near(spawn.recording.colors[0].b, 0x84 / 255.0f));

    ParticleSystemSprite maskedSpawn;
    maskedSpawn.setParticle({0, 0, 0}, 5.0f, 0x21426384u,
                            0, 0, 1, 1, true, 99.0f, 0, unusedVelocity);
    assert(maskedSpawn.recording.sizes[0].value == 5);
    assert(maskedSpawn.recording.colors[0].b == 0.0f);
}
