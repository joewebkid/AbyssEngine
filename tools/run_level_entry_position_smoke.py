#!/usr/bin/env python3
"""Exercise the source-extracted ordinary and stream-out Level entry branches."""

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
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>

struct Vector {
    float x, y, z;
    Vector &operator*=(float factor) { x *= factor; y *= factor; z *= factor; return *this; }
};
Vector operator-(Vector v) { return {-v.x, -v.y, -v.z}; }
namespace AbyssEngine { namespace AEMath {
Vector VectorNormalize(Vector v) {
    float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / length, v.y / length, v.z / length};
}
}}
template<class T> struct Array {
    std::vector<T> values;
    unsigned size() const { return unsigned(values.size()); }
    T &operator[](unsigned index) { return values[index]; }
};
struct AEGeometry {
    Vector position{0, 0, 0}, facing{0, 0, 0}, up{0, 0, 0};
    Vector getPosition() const { return position; }
    void setDirection(Vector direction, Vector axis) { facing = direction; up = axis; }
};
struct Station { int index; int getIndex() const { return index; } };
struct SolarSystem {
    bool warp = true;
    Array<int> stations;
    bool currentOrbitHasWarpGate() const { return warp; }
    void *getStations() { return &stations; }
};
struct StarSystem {
    Array<AEGeometry *> planets;
    void *getPlanets() { return &planets; }
};
struct KIPlayer {
    Vector position;
    Vector getPosition() const { return position; }
};
struct DummyEgo {
    AEGeometry *geometry = nullptr;
    float x = 0, y = 0, z = 0;
    int moves = 0;
    void setPosition(float px, float py, float pz) {
        x = px; y = py; z = pz; ++moves;
    }
    void setPosition(Vector v) { setPosition(v.x, v.y, v.z); }
};
struct DummyStatus {
    int mission = 0;
    Array<Station *> *stack = nullptr;
    SolarSystem *system = nullptr;
    Array<Station *> *getStationStack() { return stack; }
    SolarSystem *getSystem() { return system; }
    int getCurrentCampaignMission() { return mission; }
};
struct Status { static DummyStatus *gStatus; };
DummyStatus *Status::gStatus;
struct Level {
    static unsigned char initStreamOutPosition;
    int missionPtr = 3;
    DummyEgo ego;
    DummyEgo *player = nullptr;
    Array<KIPlayer *> *landmarks = nullptr;
    StarSystem *starSystem = nullptr;
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

    AEGeometry shipGeometry;
    KIPlayer exitGate{{500.0f, 600.0f, 700.0f}};
    Array<KIPlayer *> landmarks{{nullptr, nullptr, &exitGate}};
    Level fromGate;
    fromGate.ego.geometry = &shipGeometry;
    fromGate.landmarks = &landmarks;
    fromGate.runEntry();
    if (fromGate.ego.moves != 1 || fromGate.ego.x != 500.0f ||
        fromGate.ego.y != 600.0f || fromGate.ego.z != 700.0f) return 4;

    Station previous{42};
    Array<Station *> stack{{nullptr, &previous}};
    SolarSystem system;
    system.stations.values = {11, 42};
    status.stack = &stack;
    status.system = &system;
    Level fromWarpGate;
    fromWarpGate.ego.geometry = &shipGeometry;
    fromWarpGate.landmarks = &landmarks;
    fromWarpGate.runEntry();
    if (fromWarpGate.ego.x != 500.0f || fromWarpGate.ego.z != 700.0f) return 5;

    system.warp = false;
    AEGeometry sun, planetA, planetB;
    planetB.position = {100.0f, 200.0f, 300.0f};
    StarSystem stars{{{&sun, &planetA, &planetB}}};
    Level fromPlanet;
    fromPlanet.ego.geometry = &shipGeometry;
    fromPlanet.landmarks = &landmarks;
    fromPlanet.starSystem = &stars;
    fromPlanet.runEntry();
    if (fromPlanet.ego.x != 400.0f || fromPlanet.ego.y != 800.0f ||
        fromPlanet.ego.z != 1200.0f || shipGeometry.facing.z >= 0.0f ||
        shipGeometry.up.y != 1.0f) return 6;

    previous.index = 99;
    Level unknownPlanet;
    unknownPlanet.ego.geometry = &shipGeometry;
    unknownPlanet.landmarks = &landmarks;
    unknownPlanet.starSystem = &stars;
    unknownPlanet.runEntry();
    if (unknownPlanet.ego.x != 0.0f || unknownPlanet.ego.z != 100000.0f ||
        shipGeometry.facing.z != -1.0f) return 7;
    std::puts("Level entry position: ok");
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
