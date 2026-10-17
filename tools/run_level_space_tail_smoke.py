#!/usr/bin/env python3
"""Exercise source-extracted alien gate, wormhole and entry-angle branches."""

import argparse
import pathlib
import subprocess


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cxx", required=True)
    parser.add_argument("--out-dir", type=pathlib.Path, required=True)
    args = parser.parse_args()

    root = pathlib.Path(__file__).resolve().parents[1]
    source_text = (root / "src/game/world/Level.cpp").read_text(encoding="utf-8")
    prelude_start = source_text.index("    bool showWormhole = (comingFromAlienWorld")
    prelude_end = source_text.index("    this->landmarks =", prelude_start)
    tail_start = source_text.index("    random->reset();", prelude_end)
    tail_end = source_text.index("\n    }\n\n    this->starSystem = new StarSystem(mode);", tail_start)
    prelude = source_text[prelude_start:prelude_end]
    tail = source_text[tail_start:tail_end]
    worm_source = (root / "src/game/ship/PlayerWormHole.cpp").read_text(encoding="utf-8")
    placement_start = worm_source.index("            AERandom *random = static_cast<AERandom *>(Globals::rnd);")
    placement_end = worm_source.index("\n            if (mission ==", placement_start)
    placement = worm_source[placement_start:placement_end]

    args.out_dir.mkdir(parents=True, exist_ok=True)
    source = args.out_dir / "level_space_tail_extracted.cpp"
    source.write_text(
        r"""
#include <array>
#include <cstdio>
#include <vector>

struct Vector { float x, y, z; };
namespace AbyssEngine { namespace AEMath {
Vector operator-(const Vector &v) { return {-v.x, -v.y, -v.z}; }
}}
struct Station { bool attacked = false; bool isAttackedByAliens() const { return attacked; } };
struct DummyStatus {
    Station *station;
    bool alien = false, won = false;
    int mission = 10;
    Station *getStation() const { return station; }
    bool inAlienOrbit() const { return alien; }
    bool gameWon() const { return won; }
    int getCurrentCampaignMission() const { return mission; }
};
struct Status { static DummyStatus *gStatus; };
DummyStatus *Status::gStatus;
struct AERandom {
    std::vector<int> values;
    int calls = 0, resets = 0;
    void reset() { ++resets; }
    int nextInt(int bound) { return values.at(calls++) % bound; }
};
struct Globals {
    static void *rnd;
    static Globals *gGlobals;
    std::vector<int> sounds;
    void addSoundResourceToList(int sound) { sounds.push_back(sound); }
};
void *Globals::rnd;
Globals *Globals::gGlobals;
struct PaintCanvas { static PaintCanvas *gCanvas; };
PaintCanvas *PaintCanvas::gCanvas;
struct AEGeometry {
    int mesh = 0;
    Vector facing{0, 0, 0}, up{0, 0, 0};
    AEGeometry() = default;
    AEGeometry(int value, PaintCanvas *, bool) : mesh(value) {}
    void setDirection(const Vector &direction, const Vector &axis) { facing = direction; up = axis; }
};
struct Level;
struct KIPlayer {
    AEGeometry *geometry;
    Vector position{0, 0, 0};
    Level *level = nullptr;
    explicit KIPlayer(AEGeometry *g) : geometry(g) {}
    virtual ~KIPlayer() { delete geometry; }
    virtual void setPosition(const Vector &v) { position = v; }
    virtual void setLevel(Level *value) { level = value; }
};
struct PlayerWormHole : KIPlayer {
    bool visible;
    int id;
    PlayerWormHole(int playerId, AEGeometry *g, float x, float y, float z, bool show)
        : KIPlayer(g), visible(show), id(playerId) { position = {x, y, z}; }
};
struct Level {
    static bool comingFromAlienWorld;
    static bool initStreamOutPosition;
    std::array<KIPlayer *, 4> slots{};
    std::array<KIPlayer *, 4> *landmarks = &slots;
    int field_138 = 1234;
    void makeTail();
    ~Level() { for (KIPlayer *entry : slots) delete entry; }
};
bool Level::comingFromAlienWorld;
bool Level::initStreamOutPosition;
void Level::makeTail() {
""" + prelude + r"""
    AERandom *random = static_cast<AERandom *>(Globals::rnd);
""" + tail + r"""
}
int wormholeSign(int value) { return value == 0 ? 1 : -1; }
Vector updatePosition() {
    DummyStatus *status = Status::gStatus;
""" + placement + r"""
    return {float(x), float(y), float(z)};
}
int main() {
    Station station;
    DummyStatus status{&station};
    AERandom random;
    Globals globals;
    PaintCanvas canvas;
    Status::gStatus = &status;
    Globals::rnd = &random;
    Globals::gGlobals = &globals;
    PaintCanvas::gCanvas = &canvas;

    random.values = {40000, 10000, 5000, 0};
    {
        Level level;
        level.slots[2] = new KIPlayer(new AEGeometry);
        level.makeTail();
        auto *hole = dynamic_cast<PlayerWormHole *>(level.slots[3]);
        if (!hole || hole->visible || hole->id != 16994 || hole->geometry->mesh != 0x4262 ||
            hole->position.x != 0 || hole->position.y != -10000 || hole->position.z != 45000 ||
            hole->level != &level || level.field_138 != 1600 || random.calls != 4 ||
            random.resets != 1 || !globals.sounds.empty()) return 1;
    }

    station.attacked = true;
    status.mission = 42;
    Level::initStreamOutPosition = true;
    random = AERandom{{1000, 2000, 3000}};
    {
        Level level;
        level.slots[2] = new KIPlayer(new AEGeometry);
        level.makeTail();
        auto *hole = dynamic_cast<PlayerWormHole *>(level.slots[3]);
        if (!hole || !hole->visible || level.field_138 != 1234 + 0x8000 ||
            random.calls != 3 || globals.sounds.size() != 1 || globals.sounds[0] != 34) return 2;
    }

    station.attacked = false;
    status.alien = true;
    status.won = true;
    random = AERandom{{10000, 0}};
    {
        Level level;
        level.slots[2] = new KIPlayer(new AEGeometry);
        level.makeTail();
        if (level.slots[3] || level.slots[2]->position.z != 170000 ||
            level.slots[2]->geometry->facing.z != -170000 ||
            level.slots[2]->geometry->up.y != 1 || level.field_138 != 0x8000 ||
            random.calls != 2 || random.resets != 1 || globals.sounds.size() != 2) return 3;
    }

    status.alien = false;
    random = AERandom{{1, 0, 2, 1, 3, 0}};
    Vector normal = updatePosition();
    if (normal.x != 20001 || normal.y != -20002 || normal.z != 20003 || random.calls != 6) return 4;
    status.alien = true;
    random = AERandom{{1, 0, 2, 3}};
    Vector alien = updatePosition();
    if (alien.x != 30001 || alien.y != 20002 || alien.z != -60003 || random.calls != 4) return 5;
    std::puts("Level space tail: ok");
}
""",
        encoding="utf-8",
    )
    for opt in ("-O0", "-O2"):
        executable = args.out_dir / f"level_space_tail_{opt[2:]}.exe"
        subprocess.run([args.cxx, "-std=c++17", opt, str(source), "-o", str(executable)], check=True)
        subprocess.run([str(executable)], check=True)


if __name__ == "__main__":
    main()
