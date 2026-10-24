"""Run Pan's non-alternate large outpost through class-one creation."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_freighter_motion_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    mission = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = mission.index("(*enemies)[index] = createShip(race, 1, 14, nullptr, true, false);")
    end = mission.index("        } else if (index == start && smallOutpost)", start)
    assert end > start
    body = mission[start:end]
    body = replace_once(body,
        "            float x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);",
        "            freighterMoveBeforeLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveBeforeHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;\n"
        "            float x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);")
    body = replace_once(body, "            outpost->setPosition(x, y, z);",
        "            outpost->setPosition(x, y, z);\n"
        "            freighterMoveAfterLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveAfterHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;")
    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level,
        "largeOutpost = smallOutpost = false; // Test variant after the original draws.",
        "largeOutpost = true; smallOutpost = alternateLarge = false; // Test variant after the original draws.")
    start = level.index("    int foreignRace = race == 2 ? 0 : race;\n    for (; index < friendCount + field_164 + field_168; ++index) {")
    end = level.index("    throw FirstFriendCompleteBoundary{};", start)
    replacement = '''    for (; index < friendCount + field_164 + field_168; ++index) {
        int start = friendCount + field_164;
        assert(index == start && largeOutpost && !alternateLarge && !smallOutpost);
        freighterIndex = index;
        freighterSeedBeforeLow = static_cast<AERandom *>(Globals::rnd)->seedLow;
        freighterSeedBeforeHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;
            ''' + body + '''    }
'''
    level = level[:start] + replacement + level[end:]
    level_path.write_text(level)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate,
        "largeTurrets = 0; // The test variant excludes outposts after their draws.",
        "largeTurrets = 7; // Match the test-only non-alternate large outpost.")
    gate = replace_once(gate,
        "    bool foreign = expectedFreighter.nextInt(100) <= 29;\n"
        "    int expectedRace = foreign ? 2 : status->system->getRace();",
        "    int expectedRace = status->system->getRace();")
    gate = replace_once(gate,
        "freighterType == 15 && !freighterWaypoint",
        "freighterType == 14 && !freighterWaypoint")
    gate = gate.replace("freighterPlayer->hitpoints == 2025", "freighterPlayer->hitpoints == 10125")
    gate = gate.replace("freighterPlayer->maxHitpoints == 2025", "freighterPlayer->maxHitpoints == 10125")
    gate = replace_once(gate,
        """    int distance = expectedMove.nextInt(60000) - 80000;
    float moveX = float((expectedMove.nextInt(2) == 0 ? 1 : -1) * distance);
    float moveY = float(expectedMove.nextInt(40000) - 20000);
    float moveZ = float(expectedMove.nextInt(160000) - 80000);""",
        """    float moveX = float(expectedMove.nextInt(80000) - 40000);
    float moveY = float(expectedMove.nextInt(10000) - 5000);
    float moveZ = float(expectedMove.nextInt(80000) + 40000);""")
    gate = replace_once(gate, "freighter->moving && freighter->boundingVolumes",
                        "!freighter->moving && freighter->boundingVolumes")
    gate = replace_once(gate, "freighter->aiActiveCounter == 0x2f",
                        "freighter->aiActiveCounter == -1")
    gate = replace_once(gate,
        """freighter->boundingVolumes->size() == 3
           && freighter->wreckCollision->size() > 0
           && freighter->wreckMeshId == (freighterRace == 2 ? 18301 : 18302)
           && freighter->wreckType == (freighterRace == 2 ? 2 : 3)""",
        """freighter->boundingVolumes->size() == 11
           && freighter->wreckCollision->size() > 0
           && freighter->wreckMeshId == 18304
           && freighter->wreckType == 0""")
    gate = replace_once(gate,
        "assert(freighterType == 15 && materialIndex == changedShipMaterials.size());",
        "assert(freighterType == 14 && materialIndex == changedShipMaterials.size());")
    gate = replace_once(gate,
        "// getShipGroup(15) takes the special freighter branch before material handoff.",
        "// getShipGroup(14) takes the special outpost branch before material handoff.")
    gate = replace_once(gate,
        "    auto geomPos = freighter->geometry->getPosition();",
        """    assert(freighter->geometry->mesh == 14311
           && freighter->geometry->lodCount == 2
           && freighter->geometry->lodMeshes[0] == 14312
           && freighter->geometry->lodMeshes[1] == 14313
           && freighter->geometry->lodChildMeshes[0] == 14316
           && freighter->geometry->lodChildMeshes[1] == 14316
           && freighter->geometry->scalingX == 2.0f
           && freighter->geometry->scalingY == 2.0f
           && freighter->geometry->scalingZ == 2.0f);
    auto geomPos = freighter->geometry->getPosition();""")
    gate = replace_once(gate, "Pan first class-one freighter moving and relocation: OK",
                        "Pan non-alternate large outpost ship and relocation: OK")
    end = gate.rfind("    return 0;\n}")
    assert end >= 0
    gate = gate[:end] + '''    // Exercise the adjacent ARM-recovered kind-13 composite with original assets.
    auto *kind13 = static_cast<Globals *>(Globals::globals)->getShipGroup(13, 1, false);
    assert(kind13 && kind13->mesh == 17013 && kind13->lodCount == 2
           && kind13->lodMeshes[0] == 17139 && kind13->lodMeshes[1] == 17140
           && kind13->lodChildMeshes[0] == 17040
           && kind13->lodChildMeshes[1] == 17041
           && opened == closed);
    std::printf("Pan kind-13 composite ship group: OK\\n");
''' + gate[end:]
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
