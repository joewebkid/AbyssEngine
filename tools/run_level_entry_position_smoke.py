#!/usr/bin/env python3
"""Exercise the source-extracted ordinary Level entry-position branch."""

import argparse
import pathlib
import subprocess

from run_level_ctor_smoke import extract_function


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--out-dir", type=pathlib.Path, required=True)
    args = parser.parse_args()

    root = pathlib.Path(__file__).resolve().parents[1]
    level_source = (root / "src/game/world/Level.cpp").read_text(encoding="utf-8")
    entry = extract_function(
        level_source, "if (this->missionPtr == 3) {\n            thisptr->createPlayer();"
    )
    args.out_dir.mkdir(parents=True, exist_ok=True)
    source = args.out_dir / "level_entry_position_extracted.cpp"
    source.write_text(
        """
#include <cstdint>
#include <cstdio>

struct DummyGeometry {};
struct DummyEgo {
    DummyGeometry *geometry = nullptr;
    float x = 0, y = 0, z = 0;
    int moves = 0;
    void setPosition(float px, float py, float pz) {
        x = px; y = py; z = pz; ++moves;
    }
};
struct PlayerEgoVtable {
    void (*slot[7])();
    void (*placeAtStation)(void *, int);
};
struct PlayerEgoPolymorphic { PlayerEgoVtable *vtable; };
struct DummyStatus {
    int mission = 0;
    void *getStationStack() { return nullptr; }
    int getCurrentCampaignMission() { return mission; }
};
struct Status { static DummyStatus *gStatus; };
DummyStatus *Status::gStatus;
struct Level {
    static unsigned char initStreamOutPosition;
    int missionPtr = 3;
    DummyEgo ego;
    DummyEgo *player = nullptr;
    void createPlayer() { player = &ego; }
    void runEntry();
};
unsigned char Level::initStreamOutPosition;
void Level::runEntry() {
    Level *thisptr = this;
""" + entry + """
}
int main() {
    DummyStatus status;
    Status::gStatus = &status;
    Level::initStreamOutPosition = 0;
    Level normal;
    normal.runEntry();
    if (normal.ego.moves != 1 || normal.ego.x != 10.0f ||
        normal.ego.y != 10.0f || normal.ego.z != 10000.0f) return 1;
    status.mission = 1;
    Level opening;
    opening.runEntry();
    if (opening.ego.moves != 2 || opening.ego.x != 0.0f ||
        opening.ego.y != 0.0f || opening.ego.z != -110000.0f) return 2;
    status.mission = 0;
    Level::initStreamOutPosition = 1;
    Level noGeometry;
    noGeometry.runEntry();
    if (noGeometry.ego.moves != 1 || noGeometry.ego.z != 10000.0f) return 3;
    std::puts("Level ordinary entry position: ok");
}
""",
        encoding="utf-8",
    )
    for opt in ("-O0", "-O2"):
        executable = args.out_dir / f"level_entry_position_{opt[2:]}.exe"
        subprocess.run([args.cxx, "-std=c++17", opt, str(source), "-o", str(executable)], check=True)
        subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
