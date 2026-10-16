"""Exercise the extracted CutScene mode-4 enemy/race-0 updates."""
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
    process = production[production.index("void CutScene::process(int"):]
    mode4 = process.index("} else if (this->mode == 4)")
    start = process.index("Array<KIPlayer *> *arr = this->level->getEnemies();", mode4)
    end = process.index("\n        } else {", start)
    fragment = process[start:end] + "\n        }"
    source = """#include <cassert>
#include <cstdint>
namespace AbyssEngine {
struct Transform {
    int calls = 0;
    long long lastDelta = 0;
    void Update(long long delta, bool flag) {
        assert(!flag); ++calls; lastDelta = delta;
    }
};
}
struct PaintCanvas {
    AbyssEngine::Transform first, third, raceZero;
    void *TransformGetTransform(unsigned id) {
        switch (id) {
        case 11: return &first;
        case 22: return &third;
        case 33: return &raceZero;
        default: assert(false); return nullptr;
        }
    }
};
struct AEGeometry { unsigned transform; };
struct KIPlayer { AEGeometry *geometry; };
template <typename T> struct Array {
    T items[3];
    unsigned size() const { return 3; }
    T operator[](unsigned i) const { assert(i < 3); return items[i]; }
};
struct Level {
    Array<KIPlayer *> enemies;
    Array<KIPlayer *> *getEnemies() { return &enemies; }
};
struct SolarSystem { int getRace() { return 0; } };
struct Status {
    static Status *gStatus;
    SolarSystem system;
    SolarSystem *getSystem() { return &system; }
};
Status *Status::gStatus = nullptr;
struct CutScene {
    Level *level;
    uint32_t frameDelta;
    AEGeometry *geom30;
    void mode4Step(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    AEGeometry firstGeom{11}, thirdGeom{22}, raceGeom{33};
    KIPlayer first{&firstGeom}, third{&thirdGeom};
    Level level{{{&first, nullptr, &third}}};
    Status status;
    Status::gStatus = &status;
    const uint32_t deltas[] = {33, 0x80000001u};
    for (uint32_t dt : deltas) {
        PaintCanvas canvas;
        CutScene scene{&level, dt, &raceGeom};
        scene.mode4Step(&canvas);
        assert(canvas.first.calls == 1 && canvas.third.calls == 1);
        assert(canvas.raceZero.calls == 1);
        assert(canvas.first.lastDelta == static_cast<int32_t>(dt));
        assert(canvas.third.lastDelta == static_cast<int32_t>(dt));
        assert(canvas.raceZero.lastDelta == static_cast<int32_t>(dt));
    }
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-mode4-common-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode4_common.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
