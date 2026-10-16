"""Exercise the extracted CutScene mode-2 ship-motion branch."""
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
    start = process.index("long long pt = Status::gStatus->getPlayingTime();")
    end = process.index("\n    } else if", start)
    fragment = process[start:end]
    source = """#include <cassert>
#include <cmath>
#include <cstdint>
struct AEGeometry {
    float translation[3] = {}, rotation[3] = {};
    int translates = 0, rotates = 0;
    void translate(float x, float y, float z) {
        ++translates; translation[0] = x; translation[1] = y; translation[2] = z;
    }
    void rotate(float x, float y, float z) {
        ++rotates; rotation[0] = x; rotation[1] = y; rotation[2] = z;
    }
};
struct KIPlayer { AEGeometry *geometry; };
template <typename T> struct Array {
    T items[2];
    unsigned size() const { return 2; }
    T operator[](unsigned i) const { assert(i < 2); return items[i]; }
};
struct Level {
    Array<KIPlayer *> enemies;
    int calls = 0;
    Array<KIPlayer *> *getEnemies() { ++calls; return &enemies; }
};
struct Status {
    static Status *gStatus;
    long long playingTime;
    long long getPlayingTime() { return playingTime; }
};
Status *Status::gStatus = nullptr;
float VectorSignedToFloat(int value, int) { return static_cast<float>(value); }
struct CutScene {
    Level *level;
    uint32_t frameDelta;
    void shipStep() {""" + fragment + """\n    }
};
void near(float actual, float expected) {
    assert(std::fabs(actual - expected) <= std::fabs(expected) * 0.00001f + 0.000001f);
}
int main() {
    AEGeometry first, second;
    KIPlayer firstPlayer{&first}, secondPlayer{&second};
    Level level{{{&firstPlayer, &secondPlayer}}};
    Status status{1};
    Status::gStatus = &status;
    CutScene scene{&level, 100};
    scene.shipStep();
    assert(first.translates == 1 && first.rotates == 1);
    assert(second.translates == 1 && second.rotates == 1);
    near(first.translation[0], 10.0f);
    near(first.translation[1], 0.0f);
    near(first.translation[2], 0.1f);
    near(first.rotation[0], -0.02f);
    near(first.rotation[1], 0.02f);
    near(first.rotation[2], -0.018f);
    near(second.translation[0], 10.0f);
    near(second.translation[1], 0.0f);
    near(second.translation[2], 0.0f);
    near(second.rotation[0], 0.02f);
    near(second.rotation[1], -0.05f);
    near(second.rotation[2], -0.03f);
    assert(level.calls == 1);
    status.playingTime = 0;
    scene.shipStep();
    assert(level.calls == 1 && first.translates == 1 && second.translates == 1);
    status.playingTime = -1;
    scene.shipStep();
    assert(level.calls == 1 && first.translates == 1 && second.translates == 1);
    status.playingTime = 1;
    firstPlayer.geometry = nullptr;
    scene.shipStep();
    assert(first.translates == 1 && second.translates == 2);
    firstPlayer.geometry = &first;
    scene.frameDelta = 0x80000001u;
    scene.shipStep();
    assert(first.translates == 2 && second.translates == 3);
    float signedStep = static_cast<float>(static_cast<int32_t>(scene.frameDelta));
    float reverseStep = static_cast<float>(static_cast<int32_t>(0u - scene.frameDelta));
    near(first.translation[0], signedStep * 0.1f);
    near(first.rotation[0], reverseStep * 0.0002f);
    near(second.rotation[1], reverseStep * 0.0005f);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-mode2-ship-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode2_ship.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
