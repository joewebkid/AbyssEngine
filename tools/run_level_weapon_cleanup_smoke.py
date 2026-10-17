"""Exercise Level's extracted nested weapon-position cleanup."""
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

    production = (ROOT / "src/game/world/Level.cpp").read_text()
    method = production[production.index("void Level::createPlayer()"):]
    start = method.index("if (wpos != nullptr) {")
    depth = 0
    for end in range(start, len(method)):
        if method[end] == "{":
            depth += 1
        elif method[end] == "}":
            depth -= 1
            if depth == 0:
                fragment = method[start:end + 1]
                break
    else:
        raise AssertionError("weapon-position cleanup block is incomplete")

    source = """#include <cassert>
#include "engine/core/Array.h"
struct Vector {
    static int destroyed;
    ~Vector() { ++destroyed; }
};
int Vector::destroyed = 0;
void release(Array<Array<Vector *> *> *wpos) {""" + fragment + """
}
int main() {
    release(nullptr);
    auto *wpos = new Array<Array<Vector *> *>();
    ::operator delete[](wpos->data_);
    wpos->size_ = wpos->capacity_ = 4;
    wpos->data_ = static_cast<Array<Vector *> **>(
        ::operator new[](4 * sizeof(Array<Vector *> *)));
    for (int i = 0; i < 4; ++i) wpos->data_[i] = nullptr;
    wpos->data_[0] = new Array<Vector *>();
    wpos->data_[3] = new Array<Vector *>();
    wpos->data_[0]->data_[0] = new Vector();
    wpos->data_[0]->size_ = 1;
    ::operator delete[](wpos->data_[3]->data_);
    wpos->data_[3]->size_ = wpos->data_[3]->capacity_ = 2;
    wpos->data_[3]->data_ = static_cast<Vector **>(::operator new[](2 * sizeof(Vector *)));
    wpos->data_[3]->data_[0] = new Vector();
    wpos->data_[3]->data_[1] = new Vector();
    release(wpos);
    assert(Vector::destroyed == 3);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="level-weapon-cleanup-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "weapon_cleanup.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, "-I", str(ROOT / "src"),
                            str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
