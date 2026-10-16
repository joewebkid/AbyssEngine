"""Exercise the production CutScene time-update fragments on a host compiler."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def fragment(source, method):
    body = source[source.index(f"void CutScene::{method}("):]
    start = body.index("uint64_t now = ApplicationManager::gAppManager->GetCurrentTimeMillis();")
    end = body.index("this->prevTime = now;", start) + len("this->prevTime = now;")
    return body[start:end]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    production = (ROOT / "src/game/core/CutScene.cpp").read_text()
    source = """#include <cassert>
#include <cstdint>
struct ApplicationManager {
    static ApplicationManager *gAppManager;
    uint64_t now = 0;
    uint64_t GetCurrentTimeMillis() { return now; }
};
ApplicationManager app;
ApplicationManager *ApplicationManager::gAppManager = &app;
struct CutScene {
    uint64_t renderAtTime = 0, prevTime = 0, accum = 0;
    uint32_t frameDelta = 0;
    uint8_t initialized = 0;
    void initializeTime() {""" + fragment(production, "initialize") + """}
    void processTime() {""" + fragment(production, "process") + """}
};
int main() {
    CutScene scene;
    app.now = 0x00000001fffffffeULL;
    scene.initializeTime();
    assert(scene.renderAtTime == app.now && scene.prevTime == app.now);
    app.now = 0x0000000200000002ULL;
    scene.processTime();
    assert(scene.frameDelta == 4 && scene.accum == 4);
    assert(scene.renderAtTime == app.now && scene.prevTime == app.now);
    app.now = 0x000000020000ffffULL;
    scene.processTime();
    assert(scene.frameDelta == 65533 && scene.accum == 65537);
    app.now = 0x0000000200010002ULL;
    scene.processTime();
    assert(scene.frameDelta == 3 && scene.accum == 65540);
    app.now = 0x000000020000ffffULL;
    scene.processTime();
    assert(scene.frameDelta == 0xfffffffdU && scene.accum == 65537);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-time-", dir=ROOT / "_work") as temp:
        body = Path(temp) / "time.cpp"
        body.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
