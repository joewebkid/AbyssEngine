#!/usr/bin/env python3
"""Exercise the source-extracted common StarSystem/color tail of createSpace."""

import argparse
import pathlib
import subprocess


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--out-dir", type=pathlib.Path, required=True)
    args = parser.parse_args()

    root = pathlib.Path(__file__).resolve().parents[1]
    level_source = (root / "src/game/world/Level.cpp").read_text(encoding="utf-8")
    begin = level_source.index("    this->starSystem = new StarSystem(mode);", level_source.index("void Level::createSpace()"))
    end = level_source.index("\n}\n\nvoid Level::createRadioMessage", begin)
    tail = level_source[begin:end]
    args.out_dir.mkdir(parents=True, exist_ok=True)
    source = args.out_dir / "level_star_system_tail_extracted.cpp"
    source.write_text(
        r"""
#include <cstdio>
#include <new>

struct SolarSystem { int starR, starG, starB; };
struct DummyStatus {
    SolarSystem *system = nullptr;
    SolarSystem *getSystem() const { return system; }
};
struct Status { static DummyStatus *gStatus; };
DummyStatus *Status::gStatus;
struct StarSystem {
    static int created;
    int mode;
    explicit StarSystem(int value) : mode(value) { ++created; }
};
int StarSystem::created;
struct Level {
    static float r, g, b, i_r, i_g, i_b;
    static int r_min, g_min, b_min;
    int missionPtr;
    StarSystem *starSystem = nullptr;
    explicit Level(int mode) : missionPtr(mode) {}
    ~Level() { delete starSystem; }
    void makeTail();
};
float Level::r, Level::g, Level::b, Level::i_r, Level::i_g, Level::i_b;
int Level::r_min, Level::g_min, Level::b_min;
void Level::makeTail() {
    int mode = missionPtr;
""" + tail + r"""
}
int main() {
    DummyStatus status;
    Status::gStatus = &status;
    SolarSystem system{31, 60, 90};
    status.system = &system;
    Level ordinary(3);
    ordinary.makeTail();
    if (!ordinary.starSystem || ordinary.starSystem->mode != 3 ||
        Level::r != 31 || Level::g != 60 || Level::b != 90 ||
        Level::i_r != 31 || Level::i_g != 60 || Level::i_b != 90 ||
        Level::r_min != 10 || Level::g_min != 20 || Level::b_min != 30) return 1;

    status.system = nullptr;
    Level special(4);
    special.makeTail();
    if (!special.starSystem || special.starSystem->mode != 4 ||
        StarSystem::created != 2 || Level::r != 10 || Level::g != 136 ||
        Level::b != 10 || Level::r_min != 3 || Level::g_min != 45 ||
        Level::b_min != 3) return 2;
    std::puts("Level StarSystem tail: ok");
}
""",
        encoding="utf-8",
    )
    for opt in ("-O0", "-O2"):
        executable = args.out_dir / f"level_star_system_tail_{opt[2:]}.exe"
        subprocess.run([args.cxx, "-std=c++17", opt, str(source), "-o", str(executable)], check=True)
        subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
