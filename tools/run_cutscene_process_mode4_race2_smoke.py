"""Exercise the extracted CutScene mode-4 race-2 transform branch."""
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
    start = process.index("if (Status::gStatus->getSystem()->getRace() != 2)")
    end = process.index("\n        }\n    }\n}", start)
    fragment = process[start:end]
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
struct AEGeometry { unsigned transform; };
struct KIPlayer { AEGeometry *geometry; };
template <typename T> struct Array {
    T items[4];
    unsigned size() const { return 4; }
    T operator[](unsigned i) const { assert(i < 4); return items[i]; }
};
struct Level {
    Array<KIPlayer *> enemies;
    int calls = 0;
    Array<KIPlayer *> *getEnemies() { ++calls; return &enemies; }
};
struct SolarSystem { int race; int getRace() { return race; } };
struct Status {
    static Status *gStatus;
    SolarSystem *system;
    SolarSystem *getSystem() { return system; }
};
Status *Status::gStatus = nullptr;
struct PaintCanvas {
    AbyssEngine::Transform penultimate, last;
    void *TransformGetTransform(unsigned id) {
        assert(id == 22 || id == 33);
        return id == 22 ? static_cast<void *>(&penultimate) : static_cast<void *>(&last);
    }
};
struct CutScene {
    Level *level;
    uint32_t frameDelta;
    void race2Step(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    AEGeometry unused{99}, secondLast{22}, finalGeom{33};
    KIPlayer players[4] = {{&unused}, {&unused}, {&secondLast}, {&finalGeom}};
    const uint32_t deltas[] = {33, 0x80000001u};
    for (uint32_t dt : deltas) {
        Level level{{{&players[0], &players[1], &players[2], &players[3]}}};
        SolarSystem system{2};
        Status status{&system};
        Status::gStatus = &status;
        PaintCanvas canvas;
        CutScene scene{&level, dt};
        scene.race2Step(&canvas);
        assert(level.calls == 4);
        assert(canvas.penultimate.calls == 1 && canvas.last.calls == 1);
        assert(canvas.penultimate.lastDelta == static_cast<int32_t>(dt));
        assert(canvas.last.lastDelta == static_cast<int32_t>(dt));
    }
    Level level{{{&players[0], &players[1], &players[2], &players[3]}}};
    SolarSystem system{1};
    Status status{&system};
    Status::gStatus = &status;
    PaintCanvas canvas;
    CutScene scene{&level, 33};
    scene.race2Step(&canvas);
    assert(level.calls == 0 && canvas.penultimate.calls == 0 && canvas.last.calls == 0);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-mode4-race2-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode4_race2.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
