"""Exercise the production DoPostEffect body with recorded shader calls."""

from pathlib import Path
import shutil
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
BODY = function((ROOT / "src/engine/render/Engine.cpp").read_text(),
                "void Engine::DoPostEffect() {")

PREFIX = r'''
#include <cassert>
#include <cstdint>
#include <cstring>
#include <vector>

struct Engine;
struct FBOContainer { int id; };
struct Vector { float x, y, z; };
struct Event { int shader, overload, source, target; float strength; Vector center; };
static std::vector<Event> events;

struct ShaderBaseStruct {
    int id;
    FBOContainer *replacement = nullptr;
    explicit ShaderBaseStruct(int value) : id(value) {}
    virtual ~ShaderBaseStruct() = default;
    virtual void RenderEffect(FBOContainer *source, Engine *) {
        events.push_back({id, 0, source->id, -1, 0, {0, 0, 0}});
    }
    virtual void RenderEffect(FBOContainer *source, FBOContainer *&target, Engine *) {
        events.push_back({id, 1, source->id, target->id, 0, {0, 0, 0}});
        if (replacement) target = replacement;
    }
    virtual void RenderEffect(FBOContainer *source, Engine *, float strength, Vector center) {
        events.push_back({id, 2, source->id, -1, strength, center});
    }
    virtual void RenderEffect(FBOContainer *source, FBOContainer *&target, Engine *,
                              float strength, Vector center) {
        events.push_back({id, 3, source->id, target->id, strength, center});
        if (replacement) target = replacement;
    }
};
struct GlowPPShader { static int ShaderIndex; };
struct BloomShader { static int ShaderIndex; };
struct BlurShader { static int ShaderIndex; };
int GlowPPShader::ShaderIndex = 0;
int BloomShader::ShaderIndex = 1;
int BlurShader::ShaderIndex = 2;

static bool g_Engine_useShaders = true;
static uint8_t g_Engine_postEffectFlag = 0;
static constexpr uint32_t kPostEffectType2 = 0x01400002;

struct Engine {
    uint32_t postEffectFlags = 0;
    FBOContainer *postEffectFBO = nullptr;
    FBOContainer *refractFBO = nullptr;
    std::vector<ShaderBaseStruct *> shaders;
    uint32_t field_0x3c8 = 0;
    uint32_t field_0x3cc = 0, field_0x3d0 = 0, field_0x3d4 = 0;
    uint32_t clearedCode = 0;
    void DoPostEffect();
    void SetPostEffect(uint32_t code, bool enable) {
        assert(!enable);
        clearedCode = code;
    }
};
'''

SUFFIX = r'''
static void check(uint32_t flags, std::vector<Event> expected,
                  bool mutateFirst = false) {
    FBOContainer post{1}, refract{2}, replacement{3};
    ShaderBaseStruct glow{0}, bloom{1}, blur{2};
    if (mutateFirst) glow.replacement = &replacement;
    std::vector<ShaderBaseStruct *> shaders{&glow, &bloom, &blur};
    Engine engine;
    engine.postEffectFlags = flags;
    engine.postEffectFBO = &post;
    engine.refractFBO = &refract;
    engine.shaders = shaders;
    const float strength = 0.75f;
    const Vector center{0.25f, 0.5f, 0.75f};
    std::memcpy(&engine.field_0x3c8, &strength, sizeof(strength));
    std::memcpy(&engine.field_0x3cc, &center.x, sizeof(center.x));
    std::memcpy(&engine.field_0x3d0, &center.y, sizeof(center.y));
    std::memcpy(&engine.field_0x3d4, &center.z, sizeof(center.z));
    events.clear();
    engine.DoPostEffect();
    assert(events.size() == expected.size());
    for (size_t i = 0; i < events.size(); ++i) {
        const Event &got = events[i], &want = expected[i];
        assert(got.shader == want.shader && got.overload == want.overload);
        assert(got.source == want.source && got.target == want.target);
        if (got.shader == 2) {
            assert(got.strength == strength);
            assert(got.center.x == center.x && got.center.y == center.y &&
                   got.center.z == center.z);
        }
    }
    assert(engine.clearedCode == 0);
}

int main() {
    check(0, {});
    check(1, {{1, 0, 1, -1}});
    check(2, {{0, 0, 1, -1}});
    check(4, {{2, 2, 1, -1}});
    check(3, {{0, 1, 1, 2}, {1, 0, 2, -1}});
    check(5, {{1, 1, 1, 2}, {2, 2, 2, -1}});
    check(6, {{0, 1, 1, 2}, {2, 2, 2, -1}});
    check(7, {{0, 1, 1, 2}, {1, 1, 2, 1}, {2, 2, 1, -1}});
    check(3, {{0, 1, 1, 2}, {1, 0, 3, -1}}, true);

    FBOContainer post{1}, refract{2};
    ShaderBaseStruct glow{0}, bloom{1}, blur{2};
    std::vector<ShaderBaseStruct *> shaders{&glow, &bloom, &blur};
    Engine engine;
    engine.postEffectFlags = 4;
    engine.postEffectFBO = &post;
    engine.refractFBO = &refract;
    engine.shaders = shaders;
    g_Engine_postEffectFlag = 1;
    events.clear();
    engine.DoPostEffect();
    assert(engine.clearedCode == 0x01400002);
    g_Engine_postEffectFlag = 0;
    g_Engine_useShaders = false;
    events.clear();
    engine.DoPostEffect();
    assert(events.empty());
}
'''


def main():
    compiler = shutil.which("g++") or r"C:\msys64\ucrt64\bin\g++.exe"
    out = Path(tempfile.mkdtemp(prefix="posteffect-smoke-", dir=ROOT / "_work")).resolve()
    assert out.is_relative_to(ROOT.resolve())
    try:
        source = out / "posteffect.cpp"
        source.write_text(PREFIX + BODY + SUFFIX)
        for optimization in ("-O0", "-O2"):
            executable = out / ("posteffect_" + optimization[1:] + ".exe")
            subprocess.run([compiler, "-std=c++17", optimization, str(source), "-o",
                            str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
        print("DoPostEffect source-extracted routing -O0/-O2: OK")
    finally:
        assert out.is_relative_to(ROOT.resolve())
        shutil.rmtree(out)


if __name__ == "__main__":
    main()
