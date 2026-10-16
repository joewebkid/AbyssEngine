"""Exercise the extracted CutScene mode-2 ship placement branch."""
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
    body = production[production.index("void CutScene::initialize()"):]
    start = body.index("long long pt = Status::gStatus->getPlayingTime();")
    end = body.index("\n    } else if", start)
    fragment = body[start:end]
    source = """#include <cassert>
struct Vector { float x, y, z; };
struct KIPlayer {
    Vector position{};
    int calls = 0;
    void setPosition(const Vector &v) { position = v; ++calls; }
};
template <typename T> struct Array {
    T items[2];
    unsigned count = 2;
    unsigned size() const { return count; }
    T operator[](unsigned i) const { assert(i < count); return items[i]; }
};
struct Level {
    Array<KIPlayer *> enemies;
    bool noEnemies = false;
    int calls = 0;
    Array<KIPlayer *> *getEnemies() {
        ++calls; return noEnemies ? nullptr : &enemies;
    }
};
struct Status {
    static Status *gStatus;
    long long playingTime;
    long long getPlayingTime() { return playingTime; }
};
Status *Status::gStatus = nullptr;
struct CutScene {
    Level *level;
    void place(int rx, int ry) {""" + fragment + """\n    }
};
int main() {
    KIPlayer first, last;
    Level level{{{&first, &last}}};
    Status status{1};
    Status::gStatus = &status;
    CutScene scene{&level};
    scene.place(11, 12345);
    assert(level.calls == 1 && first.calls == 1 && last.calls == 1);
    assert(first.position.x == -23989 && first.position.y == -30);
    assert(first.position.z == 51845);
    assert(last.position.x == -23389 && last.position.y == 50);
    assert(last.position.z == 50945);
    status.playingTime = 0;
    scene.place(12, 12346);
    status.playingTime = -1;
    scene.place(12, 12346);
    assert(level.calls == 1 && first.calls == 1 && last.calls == 1);
    status.playingTime = 1;
    level.noEnemies = true;
    scene.place(12, 12346);
    assert(level.calls == 2 && first.calls == 1 && last.calls == 1);
    level.noEnemies = false;
    level.enemies.count = 1;
    scene.place(12, 12346);
    assert(first.calls == 1 && last.calls == 1);
    level.enemies.count = 2;
    level.enemies.items[0] = nullptr;
    scene.place(12, 12346);
    assert(first.calls == 1 && last.calls == 1);
    level.enemies.items[0] = &first;
    level.enemies.items[1] = nullptr;
    scene.place(12, 12346);
    assert(first.calls == 1 && last.calls == 1);
}
"""
    (ROOT / "_work").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="cutscene-init-mode2-ships-", dir=ROOT / "_work") as temp:
        body_file = Path(temp) / "mode2_ships.cpp"
        body_file.write_text(source)
        for level in ("-O0", "-O2"):
            exe = Path(temp) / (level[1:] + ".exe")
            subprocess.run([args.cxx, "-std=c++17", level, str(body_file), "-o", str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == "__main__":
    main()
