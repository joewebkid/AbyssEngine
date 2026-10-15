#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>

namespace AbyssEngine {
struct Engine { static unsigned char enableShader; };
unsigned char Engine::enableShader = 0;
struct Mesh { void *texCoords; void *colors; };
struct SpriteSystem { uint16_t count; Mesh *mesh; };

// PRODUCTION_SET_ALL_UV
// PRODUCTION_SET_UV
// PRODUCTION_SET_RGBA
}

int main() {
    using namespace AbyssEngine;
    std::array<float, 16> uv;
    std::array<float, 32> colors;
    uv.fill(-1.0f);
    colors.fill(-1.0f);
    Mesh mesh{uv.data(), colors.data()};
    SpriteSystem sys{2, &mesh};

    SpriteSystemSetAllUv(0.25f, 0.5f, 0.75f, 1.0f, nullptr);
    SpriteSystem empty{0, &mesh};
    SpriteSystemSetAllUv(0.25f, 0.5f, 0.75f, 1.0f, &empty);
    assert(uv[0] == -1.0f);
    SpriteSystemSetAllUv(0.25f, 0.5f, 0.75f, 1.0f, &sys);
    const std::array<float, 8> all = {{0.25f, 0.5f, 0.75f, 0.5f,
                                        0.75f, 0.0f, 0.25f, 0.0f}};
    for (unsigned i = 0; i < uv.size(); ++i)
        assert(uv[i] == all[i % 8]);

    SpriteSystemSetUv(1, 0.0f, 1.0f, 0.25f, 0.75f, nullptr);
    SpriteSystemSetUv(2, 0.0f, 1.0f, 0.25f, 0.75f, &sys);
    assert(uv[8] == all[0]); // Out-of-range index is a no-op.
    Engine::enableShader = 0;
    SpriteSystemSetUv(1, 0.0f, 1.0f, 0.25f, 0.75f, &sys);
    const std::array<float, 8> flipped = {{0.0f, 0.75f, 1.0f, 0.75f,
                                            1.0f, 0.25f, 0.0f, 0.25f}};
    for (unsigned i = 0; i < 8; ++i)
        assert(uv[8 + i] == flipped[i]);
    Engine::enableShader = 1;
    SpriteSystemSetUv(1, 0.0f, 1.0f, 0.25f, 0.75f, &sys);
    const std::array<float, 8> unflipped = {{0.0f, 0.25f, 1.0f, 0.25f,
                                              1.0f, 0.75f, 0.0f, 0.75f}};
    for (unsigned i = 0; i < 8; ++i)
        assert(uv[8 + i] == unflipped[i]);
    for (unsigned i = 0; i < 8; ++i)
        assert(uv[i] == all[i]);

    SpriteSystemSetRGBA(1, 0.1f, 0.2f, 0.3f, 0.4f, nullptr);
    SpriteSystemSetRGBA(2, 0.1f, 0.2f, 0.3f, 0.4f, &sys);
    assert(colors[16] == -1.0f);
    SpriteSystemSetRGBA(1, 0.1f, 0.2f, 0.3f, 0.4f, &sys);
    const std::array<float, 4> rgba = {{0.1f, 0.2f, 0.3f, 0.4f}};
    for (unsigned i = 0; i < 16; ++i) {
        assert(colors[i] == -1.0f);
        assert(colors[16 + i] == rgba[i % 4]);
    }
    std::puts("Sprite UV/RGBA production smoke: OK");
}
