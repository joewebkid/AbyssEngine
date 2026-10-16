#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>

struct PaintCanvas {
    struct Rgba { uint32_t resource; uint16_t id; float a, r, g, b; };
    std::vector<Rgba> colors;
    void SpriteSystemSetRGBA(uint32_t resource, uint16_t id,
                             float a, float r, float g, float b) {
        colors.push_back({resource, id, a, r, g, b});
    }
};

struct IParticleSystem {
    PaintCanvas *canvas = nullptr;
    uint32_t resource = 9;
    uint32_t resourceOffset = 100;
    uint8_t alphaFade = 0;
    uint8_t initialized = 0;
    virtual void reset() = 0;
};

struct ParticleSystemSprite : IParticleSystem {
    PaintCanvas recording;
    int resetCalls = 0;
    ParticleSystemSprite() { canvas = &recording; }
    void reset() override { ++resetCalls; }
    void init(uint32_t spriteId, uint16_t idOffset);
    void setAlpha(int index, uint32_t color, float alpha);
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
}
