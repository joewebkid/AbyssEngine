"""Execute the production flashScreen body for every ARM-observed mode."""

from pathlib import Path
import argparse
import subprocess
import tempfile

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default="C:/msys64/ucrt64/bin/g++.exe")
    args = parser.parse_args()
    source = (ROOT / "src/game/world/Level.cpp").read_text()
    body = function(source, "void Level::flashScreen(int type) {")
    clamp = function(source, "static inline float clampChannel(float scaled) {")
    fixture = '''#include <cassert>
struct Vector { float x, y, z; };
struct PlayerEgo { int hits = 0; void hitCamera() { ++hits; } };
struct Level {
    static float i_r, i_g, i_b;
    Vector flashColor{};
    float flashField = 0;
    int flashDurationA = 0, flashDurationB = 0, flashType = -1;
    unsigned char flashActive = 0;
    PlayerEgo *player;
    void flashScreen(int type);
};
float Level::i_r = 10, Level::i_g = 20, Level::i_b = 30;
// SOURCE_CLAMP
// SOURCE_BODY
static void check(Level &level, int type, int duration, float r, float g, float b,
                  int cameraHits) {
    level.flashScreen(type);
    assert(level.flashType == type && level.flashActive == 1);
    assert(level.flashDurationA == duration && level.flashDurationB == duration);
    assert(level.flashColor.x == r && level.flashColor.y == g && level.flashColor.z == b);
    assert(level.flashField == 255.0f && level.player->hits == cameraHits);
}
int main() {
    PlayerEgo player;
    Level level{};
    level.player = &player;
    check(level, 0, 2000, 50, 100, 150, 0);
    check(level, 1, 7000, 80, 160, 240, 0);
    check(level, 2, 2000, 15, 30, 45, 0);
    check(level, 3, 10000, 0, 0, 0, 1);
    check(level, 4, 10000, 0, 0, 255, 1);
    Level::i_r = -2; Level::i_g = 60; Level::i_b = 2;
    check(level, 5, 10000, 10, 255, 10, 1);
}
'''.replace("// SOURCE_CLAMP", clamp).replace("// SOURCE_BODY", body)
    with tempfile.TemporaryDirectory(prefix="level-flash-", dir=ROOT / "_work") as path:
        source_path = Path(path) / "flash.cpp"
        source_path.write_text(fixture)
        for opt in ("-O0", "-O2"):
            executable = Path(path) / ("flash" + opt[1:] + ".exe")
            subprocess.run([args.cxx, "-std=gnu++14", opt, str(source_path), "-o", str(executable)],
                           cwd=ROOT, check=True)
            subprocess.run([str(executable)], cwd=ROOT, check=True)
    print("flashScreen: six modes and camera effect passed at O0/O2")


if __name__ == "__main__":
    main()
