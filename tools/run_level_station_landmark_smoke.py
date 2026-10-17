#!/usr/bin/env python3
"""Exercise the source-extracted station-landmark branch of createSpace."""

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
    begin = level_source.index("    bool createStation = !Status::gStatus->inEmptyOrbit();")
    end = level_source.index("\n}\n\nvoid Level::createRadioMessage", begin)
    branch = level_source[begin:end]
    args.out_dir.mkdir(parents=True, exist_ok=True)
    source = args.out_dir / "level_station_landmark_extracted.cpp"
    source.write_text(
        """
#include <array>
#include <cstdio>
#include <string>

struct Station {
    int index;
    std::string name;
    static int destroyed;
    Station(int i, const char *n) : index(i), name(n) {}
    ~Station() { ++destroyed; }
    int getIndex() const { return index; }
    std::string getName() const { return name; }
};
int Station::destroyed;
struct KIPlayer { virtual ~KIPlayer() = default; };
struct PlayerStation : KIPlayer {
    static int created;
    std::string name;
    int rootStorage = 7;
    void *rootGeometry = &rootStorage;
    explicit PlayerStation(Station *station) : name(station->name) { ++created; }
};
int PlayerStation::created;
struct LODManager {
    void *object = nullptr;
    void addObject(void *value) { object = value; }
};
struct DummyStatus {
    Station station{41, "Home"};
    bool empty = false, alien = false, dlc = false;
    bool inEmptyOrbit() const { return empty; }
    bool inAlienOrbit() const { return alien; }
    Station *getStation() { return &station; }
    bool dlc1Won() const { return dlc; }
};
struct Status { static DummyStatus *gStatus; };
DummyStatus *Status::gStatus;
struct Galaxy {
    Station *getStation(int index) { return new Station(index, "DLC station"); }
};
struct Globals { static void *galaxy; };
void *Globals::galaxy;
struct Level {
    std::array<KIPlayer *, 4> entries{};
    std::array<KIPlayer *, 4> *landmarks = &entries;
    LODManager lod;
    LODManager *lodManager = &lod;
    void makeStation();
};
void Level::makeStation() {
""" + branch + """
}
int main() {
    DummyStatus status;
    Galaxy galaxy;
    Status::gStatus = &status;
    Globals::galaxy = &galaxy;

    Level ordinary;
    ordinary.makeStation();
    auto *first = static_cast<PlayerStation *>(ordinary.entries[0]);
    if (!first || ordinary.lod.object != first->rootGeometry) return 1;
    delete first;

    status.empty = true;
    Level empty;
    empty.makeStation();
    if (empty.entries[0] || empty.lod.object) return 2;

    status.station.index = 27;
    Level special;
    special.makeStation();
    if (!special.entries[0]) return 3;
    delete special.entries[0];

    status.alien = true;
    Level alien;
    alien.makeStation();
    if (alien.entries[0]) return 4;

    status.empty = false;
    status.alien = false;
    status.dlc = true;
    int destroyedBefore = Station::destroyed;
    Level dlc;
    dlc.makeStation();
    auto *renamed = static_cast<PlayerStation *>(dlc.entries[0]);
    if (!renamed || renamed->name != "DLC station" ||
        Station::destroyed != destroyedBefore + 1) return 5;
    delete renamed;
    if (PlayerStation::created != 3) return 6;
    std::puts("Level station landmark: ok");
}
""",
        encoding="utf-8",
    )
    for opt in ("-O0", "-O2"):
        executable = args.out_dir / f"level_station_landmark_{opt[2:]}.exe"
        subprocess.run([args.cxx, "-std=c++17", opt, str(source), "-o", str(executable)], check=True)
        subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
