"""Check the extracted CutScene FileRead construction/call/destruction order."""
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
    assert '#include "engine/file/FileRead.h"' in production
    assert "class FileRead {" not in production
    method = production[production.index("void CutScene::checkForTurret()"):]
    start = method.index("FileRead *fr = new FileRead();")
    end = method.index("delete fr;", start) + len("delete fr;")
    fragment = method[start:end]
    source = """#include <cassert>
#include <vector>
struct Vector {};
template <typename T> struct Array {};
std::vector<int> events;
struct Ship {
    int index;
    int getIndex() { events.push_back(3); return index; }
};
struct Status {
    static Status *gStatus;
    Ship *ship;
    Ship *getShip() { events.push_back(2); return ship; }
};
Status *Status::gStatus = nullptr;
struct FileRead {
    static int live;
    FileRead() { ++live; events.push_back(1); }
    ~FileRead() { --live; events.push_back(5); }
    Array<Array<Vector *> *> *loadWeaponPositions(int id) {
        assert(id == 37); events.push_back(4); return &result;
    }
    static Array<Array<Vector *> *> result;
};
int FileRead::live = 0;
Array<Array<Vector *> *> FileRead::result;
Array<Array<Vector *> *> *load() {""" + fragment + """
    assert(FileRead::live == 0);
    return positions;
}
int main() {
    Ship ship{37};
    Status status{&ship};
    Status::gStatus = &status;
    assert(load() == &FileRead::result);
    assert((events == std::vector<int>{1, 2, 3, 4, 5}));
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-filread-life-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "filread_life.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
