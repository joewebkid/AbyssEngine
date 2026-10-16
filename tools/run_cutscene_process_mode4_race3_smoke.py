"""Exercise the extracted CutScene mode-4 race-3 transform branch."""
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
    branch = process.index("if (race3 == 3) {")
    start = process.index("Array<KIPlayer *> *enemies = this->level->getEnemies();", branch)
    end = process.index("\n            }\n            if (Status::gStatus->getSystem()->getRace() != 2)", start)
    fragment = process[start:end]
    source = """#include <cassert>
#include <cstdint>
namespace AbyssEngine {
enum AnimationMode { Play = 1, Stop = 3 };
struct Transform {
    int updates = 0, stateCount = 0;
    long long lastDelta = 0;
    AnimationMode states[2]{};
    void Update(long long delta, bool flag) {
        assert(!flag); ++updates; lastDelta = delta;
    }
    void SetAnimationState(AnimationMode mode, void *data) {
        assert(data == nullptr && stateCount < 2);
        states[stateCount++] = mode;
    }
};
}
using AbyssEngine::AnimationMode;
struct AEGeometry { unsigned transform; };
struct KIPlayer { AEGeometry *geometry; };
template <typename T> struct Array {
    T items[4];
    unsigned size() const { return 4; }
    T operator[](unsigned i) const { assert(i < 4); return items[i]; }
};
struct Level {
    Array<KIPlayer *> enemies;
    Array<KIPlayer *> *getEnemies() { return &enemies; }
};
struct PaintCanvas {
    AbyssEngine::Transform tail, mesh28, mesh2c;
    void *TransformGetTransform(unsigned id) {
        switch (id) {
        case 11: return &tail;
        case 28: return &mesh28;
        case 44: return &mesh2c;
        default: assert(false); return nullptr;
        }
    }
};
struct AERandom {
    static AERandom *gRandom;
    int calls = 0, values[2] = {39, 29};
    int nextInt(int bound) { assert(bound == 100 && calls < 2); return values[calls++]; }
};
AERandom *AERandom::gRandom = nullptr;
struct CutScene {
    Level *level;
    uint32_t frameDelta;
    AEGeometry *geom28, *geom2c;
    int animTimer7c, animTimer80;
    void race3Step(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    AEGeometry unused{99}, tailGeom{11}, mesh28{28}, mesh2c{44};
    KIPlayer players[4] = {{&unused}, {&tailGeom}, {&unused}, {&unused}};
    Level level{{{&players[0], &players[1], &players[2], &players[3]}}};
    PaintCanvas canvas;
    AERandom random;
    AERandom::gRandom = &random;
    CutScene scene{&level, 33, &mesh28, &mesh2c, 1000, 2000};
    scene.race3Step(&canvas);
    assert(canvas.tail.updates == 1 && canvas.mesh28.updates == 1 && canvas.mesh2c.updates == 1);
    assert(canvas.tail.lastDelta == 33 && canvas.mesh28.lastDelta == 33);
    assert(canvas.mesh2c.lastDelta == 33 && random.calls == 2);
    assert(scene.animTimer7c == 0 && scene.animTimer80 == 0);
    assert(canvas.mesh28.stateCount == 2 && canvas.mesh2c.stateCount == 2);
    assert(canvas.mesh28.states[0] == AbyssEngine::Stop && canvas.mesh28.states[1] == AbyssEngine::Play);
    assert(canvas.mesh2c.states[0] == AbyssEngine::Stop && canvas.mesh2c.states[1] == AbyssEngine::Play);

    PaintCanvas plainCanvas;
    AERandom plainRandom;
    AERandom::gRandom = &plainRandom;
    CutScene plain{&level, 0x80000001u, nullptr, &mesh2c, 0, 0};
    plain.race3Step(&plainCanvas);
    assert(plainCanvas.tail.updates == 1 && plainCanvas.mesh28.updates == 0);
    assert(plainCanvas.mesh2c.updates == 1);
    assert(plainCanvas.tail.lastDelta == static_cast<int32_t>(plain.frameDelta));
    assert(plainCanvas.mesh2c.lastDelta == static_cast<int32_t>(plain.frameDelta));
    assert(plainRandom.calls == 0);

    PaintCanvas missedCanvas;
    AERandom missedRandom;
    missedRandom.values[0] = 40;
    missedRandom.values[1] = 30;
    AERandom::gRandom = &missedRandom;
    CutScene missed{&level, 33, &mesh28, &mesh2c, 1000, 2000};
    missed.race3Step(&missedCanvas);
    assert(missedRandom.calls == 2);
    assert(missedCanvas.mesh28.stateCount == 0 && missedCanvas.mesh2c.stateCount == 0);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-mode4-race3-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode4_race3.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
