"""Exercise the source-extracted empty-mission prefix to createScene entry."""

import run_station_binary_asset_gate as runner
import run_level_stage1_mission_dispatch_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct MissionDispatchBoundary {};",
                        "struct MissionDispatchBoundary {};\nstruct MissionSceneBoundary {};")
    host = replace_once(host, '#include "game/ship/PlayerGasCloud.h"',
                        '#include "game/ship/PlayerGasCloud.h"\n#include <ctime>')
    (out / "sun_host.inc").write_text(host)

    body = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    prefix = body[:body.index("    if (Globals::status->inAlienOrbit()) {")]
    tail_start = body.index("    static_cast<AERandom *>(Globals::rnd)->reset();")
    tail = body[tail_start:body.index("    missionPtr = 3;", tail_start)]
    # Unsupported branches fail closed; only the original empty-path statements run.
    selected = (prefix
        + "    if (Globals::status->inAlienOrbit()) throw MissionDispatchBoundary{};\n"
        + "    if (!mission->isEmpty()) throw MissionDispatchBoundary{};\n"
        + tail + "}\n")
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        "void Level::createMission() { throw MissionDispatchBoundary{}; }", selected)
    level += "\nvoid Level::createScene() { throw MissionSceneBoundary{}; }\n"
    (out / "level_init_under_test.inc").write_text(level)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN5Level11createSceneEv)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "    bool reachedMissionDispatch = false;",
                        "    auto missionStartSecond = std::time(nullptr);\n"
                        "    bool reachedMissionDispatch = false;")
    gate = replace_once(gate, "catch (const MissionDispatchBoundary &)",
                        "catch (const MissionSceneBoundary &)")
    gate = replace_once(gate,
        "    assert(reachedMissionDispatch && level->field_134 == 1\n"
        "           && status->getMission() == emptyMission);",
        "    assert(reachedMissionDispatch && level->field_134 == 1\n"
        "           && level->missionPtr == 0\n"
        "           && status->getMission() == emptyMission);\n"
        "    auto missionEndSecond = std::time(nullptr);\n"
        "    auto *rng = static_cast<AERandom *>(Globals::rnd);\n"
        "    bool resetSeedMatches = false;\n"
        "    for (long long s = missionStartSecond; s <= missionEndSecond; ++s) {\n"
        "        uint32_t low = 0xdeece66dU ^ static_cast<uint32_t>(s);\n"
        "        uint32_t high = (static_cast<uint32_t>(s >> 31) & 0xffffU) ^ 5U;\n"
        "        resetSeedMatches |= rng->seedLow == low && rng->seedHigh == high;\n"
        "    }\n"
        "    assert(resetSeedMatches);")
    gate = replace_once(gate,
        "stage 1 reached production empty-mission dispatch after gas clouds: OK",
        "stage 1 executed empty-mission prefix to createScene entry: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
