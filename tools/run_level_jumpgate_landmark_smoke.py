#!/usr/bin/env python3
"""Exercise the source-extracted jumpgate branch of Level::createSpace."""

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
    begin = level_source.index("    // Android ARM rodata at 0x1fdc54")
    end = level_source.index("    random->reset();", begin) + len("    random->reset();")
    branch = level_source[begin:end]
    args.out_dir.mkdir(parents=True, exist_ok=True)
    source = args.out_dir / "level_jumpgate_landmark_extracted.cpp"
    source.write_text(
        r"""
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>

struct Vector { float x, y, z; };
struct Matrix { float yaw = 0; };
namespace AbyssEngine { namespace AEMath {
void MatrixSetRotation(Matrix &matrix, float, float yaw, float) { matrix.yaw = yaw; }
Vector MatrixRotateVector(const Matrix &matrix, const Vector &v) {
    return {v.x * std::cos(matrix.yaw) + v.z * std::sin(matrix.yaw), v.y,
            v.z * std::cos(matrix.yaw) - v.x * std::sin(matrix.yaw)};
}
}}
struct AERandom {
    int seed = 0, calls = 0, resets = 0;
    std::vector<int> values{1, 0, 0, 0};
    void setSeed(long long value) { seed = int(value); calls = 0; }
    int nextInt(int bound) { int value = values.at(calls++); return value % bound; }
    void reset() { ++resets; }
};
struct Station { int index = 7; int getIndex() const { return index; } };
struct SolarSystem {
    bool warp = true;
    int race = 1;
    bool currentOrbitHasWarpGate() const { return warp; }
    int getRace() const { return race; }
};
struct DummyStatus {
    Station *station;
    SolarSystem *system;
    bool alien = false;
    Station *getStation() const { return station; }
    SolarSystem *getSystem() const { return system; }
    bool inAlienOrbit() const { return alien; }
};
struct Status { static DummyStatus *gStatus; };
DummyStatus *Status::gStatus;
struct Globals { static void *rnd; };
void *Globals::rnd;
struct PaintCanvas {
    static PaintCanvas *gCanvas;
    unsigned int nextTransform = 1;
    std::vector<uint16_t> meshIds;
    void TransformCreate(unsigned int &handle) { handle = nextTransform++; }
    void TransformAddMesh(unsigned int, uint16_t mesh, bool) { meshIds.push_back(mesh); }
};
PaintCanvas *PaintCanvas::gCanvas;
struct AEGeometry {
    uint16_t mesh;
    std::vector<unsigned int> children;
    std::array<uint16_t, 2> lod{};
    std::array<int, 2> distances{};
    std::array<uint16_t, 2> childLod{};
    AEGeometry(uint16_t value, PaintCanvas *, bool) : mesh(value) {}
    void addChild(unsigned int handle) { children.push_back(handle); }
    void setLodMeshes(uint16_t *m, int *d, int) { lod = {m[0], m[1]}; distances = {d[0], d[1]}; }
    void setLodChildMeshes(uint16_t *m) { childLod = {m[0], m[1]}; }
};
struct Player { int radius = 7500; };
struct PlayerJumpgate {
    AEGeometry *geometry;
    Player *player = new Player;
    Vector position;
    bool visible;
    unsigned int animation = 0;
    PlayerJumpgate(int, AEGeometry *g, float x, float y, float z, bool v)
        : geometry(g), position{x, y, z}, visible(v) {}
    ~PlayerJumpgate() { delete geometry; delete player; }
    void addJumpAnimationHandle(unsigned int handle) { animation = handle; }
};
struct LODManager {
    std::vector<AEGeometry *> objects;
    void addObject(AEGeometry *object) { objects.push_back(object); }
};
struct Level {
    std::array<PlayerJumpgate *, 4> slots{};
    std::array<PlayerJumpgate *, 4> *landmarks = &slots;
    LODManager lod;
    LODManager *lodManager = &lod;
    Vector missionSpawnPosition{10, 20, 30};
    int field_138 = -1;
    void makeGates();
    ~Level() { for (auto *gate : slots) delete gate; }
};
void Level::makeGates() {
""" + branch + r"""
}
int main() {
    Station station;
    SolarSystem system;
    DummyStatus status{&station, &system};
    AERandom random;
    PaintCanvas canvas;
    Status::gStatus = &status;
    Globals::rnd = &random;
    PaintCanvas::gCanvas = &canvas;

    {
        Level level;
        level.makeGates();
        auto *entry = level.slots[1];
        auto *exit = level.slots[2];
        if (!entry || !exit || !entry->visible || exit->visible) return 1;
        if (entry->geometry->mesh != 0x3a9c || entry->geometry->children.size() != 2 ||
            entry->geometry->lod != std::array<uint16_t, 2>{0x3aa9, 0x3aad} ||
            entry->geometry->childLod != std::array<uint16_t, 2>{0x3ab0, 0x3ab1}) return 2;
        if (entry->position.y != 0 || exit->position.y != 0 ||
            std::fabs(entry->position.z - 94500.0f) > 1000.0f ||
            std::fabs(exit->position.z - 120000.0f) > 1000.0f) return 3;
        if (exit->player->radius != 0 || level.field_138 != 0 || random.seed != 14 ||
            random.calls != 4 || random.resets != 1 || level.lod.objects.size() != 2) return 4;
        if (canvas.meshIds != std::vector<uint16_t>{0x3a9e, 0x3a9d, 0x3a9f,
                                                    0x3a9e, 0x3a9d, 0x3a9f}) return 5;
    }

    system.warp = false;
    random.resets = 0;
    {
        Level level;
        level.makeGates();
        if (level.slots[1] || !level.slots[2] || random.calls != 2 ||
            level.lod.objects.size() != 1 || random.resets != 1) return 6;
    }

    status.station = nullptr;
    status.alien = true;
    system.race = 3;
    random.resets = 0;
    {
        Level level;
        level.makeGates();
        if (random.seed != -1 || random.calls != 0 || random.resets != 1 ||
            level.slots[2]->geometry->mesh != 0x3a98 ||
            level.slots[2]->position.z != 30.0f) return 7;
    }
    std::puts("Level jumpgate landmarks: ok");
}
""",
        encoding="utf-8",
    )
    for opt in ("-O0", "-O2"):
        executable = args.out_dir / f"level_jumpgate_landmark_{opt[2:]}.exe"
        subprocess.run([args.cxx, "-std=c++17", opt, str(source), "-o", str(executable)], check=True)
        subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
