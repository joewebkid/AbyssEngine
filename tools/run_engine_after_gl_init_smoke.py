"""Exercise the production Engine::AfterGLInit body with host GL fixtures."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
BODY = function(
    (ROOT / "src/engine/render/Engine.cpp").read_text(),
    "void Engine::AfterGLInit() {",
)

HOST = r'''
#include <cassert>
#include <cstdint>
#include <string>

namespace AbyssEngine {
struct String {
    std::string value;
    String() = default;
    String(const char *text) : value(text) {}
};
struct Mesh { void *indices; };
struct Engine {
    static String vendor;
    static String renderer;
    Mesh *quadMesh = nullptr;
    int resetCount = 0;
    void ResetLightParam() { ++resetCount; }
    void AfterGLInit();
};
String Engine::vendor;
String Engine::renderer;
uint32_t indices[3] = {};
Mesh quad{indices};
int createCount = 0;
int MeshCreate(Engine *engine, unsigned short vertices, unsigned short faces,
               signed char format, Mesh **out) {
    assert(engine != nullptr && vertices == 4 && faces == 2 && format == 0x13);
    ++createCount;
    *out = &quad;
    return 1;
}
}
using AbyssEngine::Engine;
using AbyssEngine::String;

const unsigned char *glGetString(unsigned int name) {
    if (name == 0x1f00) return (const unsigned char *)"Vendor";
    if (name == 0x1f01) return (const unsigned char *)"Renderer";
    assert(false);
    return nullptr;
}
'''

MAIN = r'''
int main() {
    Engine engine;
    engine.AfterGLInit();
    assert(engine.resetCount == 1 && AbyssEngine::createCount == 1);
    assert(engine.quadMesh == &AbyssEngine::quad);
    assert(AbyssEngine::indices[0] == 0x20000);
    assert(AbyssEngine::indices[1] == 1);
    assert(AbyssEngine::indices[2] == 0x30002);
    assert(Engine::vendor.value == "Vendor");
    assert(Engine::renderer.value == "Renderer");
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="gof2-engine-gl-") as directory:
        cpp = Path(directory) / "engine_after_gl_init.cpp"
        cpp.write_text(HOST + BODY + MAIN)
        for optimization in ("-O0", "-O2"):
            executable = Path(directory) / ("engine_after_gl_init" + optimization + ".exe")
            subprocess.run(["g++", "-std=c++17", optimization, str(cpp), "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
    print("Engine AfterGLInit body: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
