"""Check the six production Engine FBO routing methods."""

import subprocess
import tempfile
from pathlib import Path

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "src/engine/render/Engine.cpp").read_text()
methods = (
    "ActivateTextureFBO",
    "ActivateRender2TextureFBO",
    "DeactivateRender2TextureFBO",
    "ActivateRefractFBO",
    "ActivateRender2FracFBO",
    "DeactivateRender2FracFBO",
)
bodies = "".join(function(source, f"void Engine::{name}() {{") for name in methods)

HOST = r'''
#include <cassert>

struct FBOContainer {
    int activates = 0, begins = 0, ends = 0;
    void Activate() { ++activates; }
    void BeginCapture() { ++begins; }
    void EndCapture() { ++ends; }
};
struct Engine {
    FBOContainer *postEffectFBO = nullptr;
    FBOContainer *refractFBO = nullptr;
    void ActivateTextureFBO();
    void ActivateRender2TextureFBO();
    void DeactivateRender2TextureFBO();
    void ActivateRefractFBO();
    void ActivateRender2FracFBO();
    void DeactivateRender2FracFBO();
};
'''

MAIN = r'''
int main() {
    Engine engine;
    engine.ActivateTextureFBO();
    engine.ActivateRender2TextureFBO();
    engine.DeactivateRender2TextureFBO();
    engine.ActivateRefractFBO();
    engine.ActivateRender2FracFBO();
    engine.DeactivateRender2FracFBO();

    FBOContainer post, refract;
    engine.postEffectFBO = &post;
    engine.refractFBO = &refract;
    engine.ActivateTextureFBO();
    engine.ActivateRender2TextureFBO();
    engine.DeactivateRender2TextureFBO();
    engine.ActivateRefractFBO();
    engine.ActivateRender2FracFBO();
    engine.DeactivateRender2FracFBO();
    assert(post.activates == 1 && post.begins == 1 && post.ends == 1);
    assert(refract.activates == 1 && refract.begins == 1 && refract.ends == 1);
}
'''


def main() -> None:
    with tempfile.TemporaryDirectory(prefix="gof2-engine-fbo-") as directory:
        cpp = Path(directory) / "engine_fbo.cpp"
        cpp.write_text(HOST + bodies + MAIN)
        for optimization in ("-O0", "-O2"):
            executable = Path(directory) / ("engine_fbo" + optimization + ".exe")
            subprocess.run(["g++", "-std=c++17", optimization, str(cpp), "-o", str(executable)], check=True)
            subprocess.run([str(executable)], check=True)
    print("Engine FBO dispatch: OK (-O0/-O2)")


if __name__ == "__main__":
    main()
