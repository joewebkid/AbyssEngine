"""Exercise CutScene's extracted nested weapon-position cleanup."""
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
    method = production[production.index("void CutScene::checkForTurret()"):]
    start = method.index("if (positions == nullptr)")
    end = method.index("delete positions;", start) + len("delete positions;")
    fragment = method[start:end]
    source = """#include <cassert>
#include "engine/core/Array.h"
struct Vector {
    static int destroyed;
    ~Vector() { ++destroyed; }
};
int Vector::destroyed = 0;
void release(Array<Array<Vector *> *> *positions) {""" + fragment + """\n}
int main() {
    release(nullptr);
    auto *positions = new Array<Array<Vector *> *>();
    ::operator delete[](positions->data_);
    positions->size_ = positions->capacity_ = 4;
    positions->data_ = static_cast<Array<Vector *> **>(
        ::operator new[](4 * sizeof(Array<Vector *> *)));
    for (int i = 0; i < 4; ++i) positions->data_[i] = nullptr;
    positions->data_[0] = new Array<Vector *>();
    positions->data_[2] = new Array<Vector *>();
    positions->data_[0]->size_ = 1;
    positions->data_[2]->size_ = 1;
    positions->data_[0]->data_[0] = new Vector();
    positions->data_[2]->data_[0] = new Vector();
    release(positions);
    assert(Vector::destroyed == 2);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-turret-cleanup-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "turret_cleanup.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, "-I", str(ROOT / "src"),
                            str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
