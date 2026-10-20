"""Exercise the source-extracted mode-zero scene cleanup and mission return."""

import run_station_binary_asset_gate as runner
import run_level_stage1_empty_mission_prefix_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    scene = extract("src/game/world/Level.cpp", ["void Level::createScene() {"])
    scene = (scene[:scene.index("    if (mode == 2) {")]
             + "    if (mode != 0) throw MissionDispatchBoundary{};\n}\n")
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        "void Level::createScene() { throw MissionSceneBoundary{}; }", scene)
    level = replace_once(level,
        "    missionPtr = 0;\n    createScene();\n}",
        "    missionPtr = 0;\n    createScene();\n"
        "    missionPtr = 3;\n    throw MissionSceneBoundary{};\n}")
    (out / "level_init_under_test.inc").write_text(level)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "    auto missionStartSecond = std::time(nullptr);",
        "    assert(level->enemies == nullptr);\n"
        "    level->enemies = new Array<KIPlayer *>();\n"
        "    assert(level->enemies->empty());\n"
        "    auto missionStartSecond = std::time(nullptr);")
    gate = replace_once(gate,
        "           && level->missionPtr == 0\n"
        "           && status->getMission() == emptyMission);",
        "           && level->missionPtr == 3 && level->enemies == nullptr\n"
        "           && status->getMission() == emptyMission);")
    gate = replace_once(gate,
        "stage 1 executed empty-mission prefix to createScene entry: OK",
        "stage 1 returned from mode-zero createScene and restored mission mode: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
