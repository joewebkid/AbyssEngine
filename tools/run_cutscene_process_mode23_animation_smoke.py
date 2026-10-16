"""Exercise the extracted mode-23 animation path from CutScene::process."""
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
    branch = process.index("} else if (kind == 2 && this->geometries != nullptr) {")
    start = process.index("unsigned int n = this->geometries->size();", branch)
    end = process.index("\n        }\n\n        Array<KIPlayer *> *enemies", start)
    fragment = process[start:end]
    source = """#include <cassert>
#include <cstdint>
namespace AbyssEngine {
enum AnimationMode { Off = 0, Play = 1, Stop = 3 };
struct Transform {
    int updates = 0, checks = 0, stateCount = 0;
    bool running = false;
    long long lastDelta = 0;
    AnimationMode states[2]{};
    void Update(long long delta, bool flag) {
        assert(!flag); ++updates; lastDelta = delta;
    }
    int IsRunning() { ++checks; return running; }
    void SetAnimationState(AnimationMode mode, void *data) {
        assert(data == nullptr && stateCount < 2);
        states[stateCount++] = mode;
    }
};
}
using AbyssEngine::AnimationMode;
struct AEGeometry { unsigned transform, childTransform; };
template <typename T> struct Array {
    T items[2];
    unsigned size() const { return 2; }
    T operator[](unsigned i) const { assert(i < 2); return items[i]; }
};
struct PaintCanvas {
    AbyssEngine::Transform first, second, firstChild, secondChild;
    void *TransformGetTransform(unsigned id) {
        switch (id) {
        case 11: return &first;
        case 22: return &second;
        case 111: return &firstChild;
        case 222: return &secondChild;
        default: assert(false); return nullptr;
        }
    }
};
struct AERandom {
    static AERandom *gRandom;
    int calls = 0;
    int nextInt(int bound) { assert(bound == 100); return calls++ == 0 ? 19 : 20; }
};
AERandom *AERandom::gRandom = nullptr;
struct CutScene {
    Array<AEGeometry *> *geometries;
    uint32_t frameDelta;
    unsigned fogTimer84;
    void animate(PaintCanvas *canvas) {""" + fragment + """}
};
int main() {
    AEGeometry first{11, 111}, second{22, 222};
    Array<AEGeometry *> geometries{{&first, &second}};
    AERandom random;
    AERandom::gRandom = &random;
    PaintCanvas canvas;
    CutScene scene{&geometries, 0x80000001u, 3001};
    scene.animate(&canvas);
    assert(scene.fogTimer84 == 0 && random.calls == 2);
    assert(canvas.first.updates == 1 && canvas.second.updates == 1);
    assert(canvas.first.lastDelta == static_cast<int32_t>(scene.frameDelta));
    assert(canvas.second.lastDelta == static_cast<int32_t>(scene.frameDelta));
    assert(canvas.first.checks == 1 && canvas.second.checks == 0);
    assert(canvas.first.stateCount == 2 && canvas.firstChild.stateCount == 2);
    assert(canvas.first.states[0] == AbyssEngine::Stop && canvas.first.states[1] == AbyssEngine::Play);
    assert(canvas.firstChild.states[0] == AbyssEngine::Stop && canvas.firstChild.states[1] == AbyssEngine::Play);
    assert(canvas.second.stateCount == 0 && canvas.secondChild.stateCount == 0);

    AERandom quietRandom;
    AERandom::gRandom = &quietRandom;
    PaintCanvas quietCanvas;
    CutScene quiet{&geometries, 33, 2000};
    quiet.animate(&quietCanvas);
    assert(quiet.fogTimer84 == 2000 && quietRandom.calls == 0);
    assert(quietCanvas.first.updates == 1 && quietCanvas.second.updates == 1);

    AERandom runningRandom;
    AERandom::gRandom = &runningRandom;
    PaintCanvas runningCanvas;
    runningCanvas.first.running = true;
    CutScene running{&geometries, 33, 3001};
    running.animate(&runningCanvas);
    assert(runningCanvas.first.checks == 1 && runningCanvas.first.stateCount == 0);
    assert(runningCanvas.firstChild.stateCount == 0);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-process-animation-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "process_animation.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
