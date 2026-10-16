#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>

namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
Vector operator*(Vector v, float scale) {
    return {v.x * scale, v.y * scale, v.z * scale};
}
float VectorSignedToFloat(int value, unsigned char) { return (float) value; }
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::VectorSignedToFloat;

constexpr float kMillisecondsToSeconds = 0.001f;
static float g_uvRoundBias = 1.0f / 1024.0f;

struct ParticleSettings {
    struct SetDefinition {
        int lifetime = 100;
        int posBase = 1000;
        int frames = 0;
        float uvU0 = 0.1f, uvV0 = 0.2f, uvU1 = 0.6f, uvV1 = 0.8f;
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
    void SpriteSystemSetRGBA(uint32_t resource, uint16_t id,
                             float a, float r, float g, float b) {
        colors.push_back({resource, id, a, r, g, b});
        order.push_back('C');
    }
    void SpriteSystemSetPosition(uint32_t, uint16_t id, float x, float y, float z) {
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
        movements.push_back({id, x, y, z}); order.push_back('M');
    }
};

struct IParticleSystem {
    PaintCanvas *canvas = nullptr;
    uint32_t resource = 9;
    uint32_t resourceOffset = 100;
    uint8_t alphaFade = 0;
    uint8_t initialized = 0;
    uint32_t flags = 0;
    int ages[2]{0, 0};
    int *particleAges = ages;
    int8_t setIds[2]{0, 0};
    int8_t *particleSetIds = setIds;
    Vector velocities[2]{};
    Vector *particleVelocities = velocities;
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
    int resetCalls = 0;
    int areaCalls = 0;
    ParticleSystemSprite() { canvas = &recording; }
    void reset() override { ++resetCalls; }
    void init(uint32_t spriteId, uint16_t idOffset);
    void setAlpha(int index, uint32_t color, float alpha);
    void updateSingle(int index, float dt);
    void updateAreaExitParticle(int, float) { ++areaCalls; }
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
    areaExit.updateSingle(0, 1.0f);
    assert(areaExit.areaCalls == 1 && areaExit.ages[0] == 25);
    assert(areaExit.recording.order.empty());
}
