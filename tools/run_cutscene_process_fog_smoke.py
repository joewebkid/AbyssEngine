"""Check extracted CutScene::process fog calls for modes 23 and 4."""
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
    first = process.index("canvas->FogSetParameter(", process.index("this->fogTimer84 ="))
    first_end = process.index(");", first) + 2
    second = process.index("canvas->FogSetParameter(", process.index("int race1 ="))
    enable = process.index("canvas->FogEnable(", second)
    second_end = process.index(");", enable) + 2
    mode23 = process[first:first_end]
    mode4 = process[second:second_end]
    source = """#include <cassert>
namespace AbyssEngine {
enum FogMode { FogMode_dummy = 0, FogMode_1 = 1, FogMode_linear = 0x2601 };
}
struct PaintCanvas {
    int parameters = 0, enables = 0;
    float end = 0;
    unsigned color = 0;
    void FogSetParameter(AbyssEngine::FogMode mode, float start, float fogEnd,
                         float density, unsigned fogColor) {
        assert(mode == AbyssEngine::FogMode_linear);
        assert(start == 0.0f && density == 1.0f);
        ++parameters; end = fogEnd; color = fogColor;
    }
    void FogEnable(bool enabled, AbyssEngine::FogMode mode) {
        assert(enabled && mode == AbyssEngine::FogMode_1);
        ++enables;
    }
};
void mode23(PaintCanvas *canvas) {""" + mode23 + """}
void mode4(PaintCanvas *canvas) {""" + mode4 + """}
int main() {
    PaintCanvas first;
    mode23(&first);
    assert(first.parameters == 1 && first.enables == 0);
    assert(first.end == 30000.0f && first.color == 0x011e0cffu);
    PaintCanvas second;
    mode4(&second);
    assert(second.parameters == 1 && second.enables == 1);
    assert(second.end == 5000.0f && second.color == 0x011e0cffu);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-process-fog-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "process_fog.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
