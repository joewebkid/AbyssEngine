"""Exercise the extracted CutScene resetCamera for supported modes."""
import argparse
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", default=shutil.which("g++"))
    args = parser.parse_args()
    if not args.cxx:
        parser.error("specify --cxx")

    production = (ROOT / "src/game/core/CutScene.cpp").read_text()
    start = production.index("void CutScene::resetCamera() {")
    end = production.index("\n}\n\nclass FileRead", start) + 2
    fragment = production[start:end]
    source = """#include <cassert>
namespace AbyssEngine {
enum FogMode { FogMode_dummy = 0, FogMode_1 = 1, FogMode_linear = 0x2601 };
}
struct PaintCanvas {
    static PaintCanvas *gCanvas;
    unsigned current = 0, perspectiveId = 0;
    float fov = 0, near = 0, far = 0, fogEnd = 0;
    unsigned fogColor = 0;
    int fogParameters = 0, fogEnables = 0;
    void CameraSetCurrent(unsigned id) { current = id; }
    void CameraSetPerspective(unsigned id, float a, float b, float c) {
        perspectiveId = id; fov = a; near = b; far = c;
    }
    void FogSetParameter(AbyssEngine::FogMode mode, float start, float end,
                         float density, unsigned color) {
        assert(mode == AbyssEngine::FogMode_linear);
        assert(start == 0.0f && density == 1.0f);
        ++fogParameters; fogEnd = end; fogColor = color;
    }
    void FogEnable(bool enabled, AbyssEngine::FogMode mode) {
        assert(enabled && mode == AbyssEngine::FogMode_1);
        ++fogEnables;
    }
};
PaintCanvas *PaintCanvas::gCanvas = nullptr;
struct SolarSystem { int race; int getRace() { return race; } };
struct Status {
    static Status *gStatus;
    SolarSystem *system;
    SolarSystem *getSystem() { return system; }
};
Status *Status::gStatus = nullptr;
struct KIPlayer {
    int typeCalls = 0;
    int getType() { ++typeCalls; return 3; }
};
template <typename T> struct Array {
    T item;
    T operator[](int index) { assert(index == 0); return item; }
};
struct Level {
    Array<KIPlayer *> enemies;
    Array<KIPlayer *> *getEnemies() { return &enemies; }
};
struct CutScene {
    int mode;
    unsigned cameraId70 = 7, cameraId6c = 9;
    Level *level;
    void resetCamera();
};
""" + fragment + """
int main() {
    const int modes[] = {23, 4, 2};
    const int races[] = {1, 0};
    for (int mode : modes) for (int race : races) {
        PaintCanvas canvas;
        PaintCanvas::gCanvas = &canvas;
        SolarSystem system{race};
        Status status{&system};
        Status::gStatus = &status;
        KIPlayer player;
        Level level{{&player}};
        CutScene scene{mode, 7, 9, &level};
        scene.resetCamera();
        if (mode == 2) {
            assert(canvas.current == 0 && canvas.perspectiveId == 0);
            assert(canvas.fogParameters == 0 && player.typeCalls == 0);
            continue;
        }
        const unsigned id = mode == 23 ? 7 : 9;
        assert(canvas.current == id && canvas.perspectiveId == id);
        assert(canvas.fov == (mode == 23 ? 0.8f : 1.2f));
        assert(canvas.near == 200.0f && canvas.far == (mode == 23 ? 100000.0f : 48000.0f));
        assert(player.typeCalls == (mode == 23 ? 1 : 0));
        assert(canvas.fogParameters == (race == 1 ? 1 : 0));
        assert(canvas.fogEnables == (race == 1 ? 1 : 0));
        if (race == 1) {
            assert(canvas.fogEnd == (mode == 23 ? 30000.0f : 5000.0f));
            assert(canvas.fogColor == 0x011e0cffu);
        }
    }
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-reset-camera-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "reset_camera.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
