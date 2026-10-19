#include <cassert>
#include <cmath>
#include <cstdint>

namespace AbyssEngine {
enum FogMode { Linear = 0x2601 };
struct Vector { float x, y, z; };
struct Engine {
    static unsigned char enableShader;
    float fogMinDist = -1.0f;
    float fogMaxDist = -1.0f;
    Vector fogColor = {-1.0f, -1.0f, -1.0f};
};
unsigned char Engine::enableShader = 0;
struct PaintCanvas {
    Engine *engine;
    void FogSetParameter(FogMode mode, float start, float end, float density, unsigned color);
};
}

using AbyssEngine::Engine;
using AbyssEngine::PaintCanvas;
using AbyssEngine::Vector;

static unsigned calls[4], colorCalls;
static float values[4], glColor[4];
float paintcanvas_ext_fsp_unsignedtofloat(unsigned v, unsigned) { return float(v); }
void paintcanvas_ext_fsp_glFogf(unsigned name, float value) {
    const unsigned expected[] = {0xb65, 0xb62, 0xb63, 0xb64};
    assert(calls[0] < 4 && name == expected[calls[0]]);
    values[calls[0]++] = value;
}
void paintcanvas_ext_fsp_glFogfv(unsigned name, void *data) {
    assert(name == 0xb66);
    ++colorCalls;
    const float *components = static_cast<const float *>(data);
    for (int i = 0; i < 4; ++i) glColor[i] = components[i];
}

#include "canvas_fog_parameter_under_test.inc"

static bool near(float a, float b) { return std::fabs(a - b) < 0.00001f; }

int main() {
    Engine engine;
    PaintCanvas canvas{&engine};
    canvas.FogSetParameter(AbyssEngine::Linear, 12.0f, 96.0f, 0.25f, 0x80402010);
    assert(calls[0] == 4 && colorCalls == 1);
    assert(values[0] == float(AbyssEngine::Linear));
    assert(values[1] == 0.25f && values[2] == 12.0f && values[3] == 96.0f);
    assert(near(glColor[0], 128.0f / 255.0f));
    assert(near(glColor[1], 64.0f / 255.0f));
    assert(near(glColor[2], 32.0f / 255.0f));
    assert(near(glColor[3], 16.0f / 255.0f));
    assert(engine.fogMinDist == -1.0f && engine.fogMaxDist == -1.0f);

    Engine::enableShader = 1;
    canvas.FogSetParameter(AbyssEngine::Linear, 3.0f, 45.0f, 0.5f, 0xff336699);
    assert(calls[0] == 4 && colorCalls == 1);
    assert(engine.fogMinDist == 3.0f && engine.fogMaxDist == 45.0f);
    assert(near(engine.fogColor.x, 1.0f));
    assert(near(engine.fogColor.y, 51.0f / 255.0f));
    assert(near(engine.fogColor.z, 102.0f / 255.0f));
    return 0;
}
