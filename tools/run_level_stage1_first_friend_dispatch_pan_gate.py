"""Exercise Pan's first ordinary-friend dispatch, before ship construction."""

import run_station_binary_asset_gate as runner
import run_level_stage1_enemy_array_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    body = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = body.index("    for (int i = 0; i < friendCount; ++i) {")
    ordinary = (
        "            int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);\n"
        "            (*enemies)[i] = createShip(race, 0, type, ambientFriendRoute->getWaypoint(), true, false);\n"
    )
    assert ordinary in body[start:body.index("    int index = friendCount;", start)]
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level, "    throw MissionSceneBoundary{};\n}",
        "    if (wanted) throw MissionDispatchBoundary{};\n"
        "    for (int i = 0; i < friendCount; ++i) {\n"
        + ordinary + "    }\n    throw MissionSceneBoundary{};\n}")
    (out / "level_init_under_test.inc").write_text(level)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct MissionSceneBoundary {};",
        "struct MissionSceneBoundary {};\nstruct FirstFriendBoundary {};\n"
        "static int firstFriendRace = -1, firstFriendClass = -1, firstFriendType = -1;\n"
        "static Waypoint *firstFriendWaypoint;\n"
        "static bool firstFriendHostile, firstFriendGroup;")
    (out / "sun_host.inc").write_text(host)

    globals = (ROOT / "src/game/core/Globals.cpp").read_text()
    chain = (out / "station_binary_chain.inc").read_text()
    table = globals.index("static const int gREF_table[37]")
    chain += "\n" + globals[table:globals.index("void Globals::drawLines(", table)]
    chain += "\n" + extract("src/game/world/Route.cpp", [
        "Waypoint *Route::getWaypoint() {", "Waypoint *Route::getWaypoint(int index) {"])
    chain += "\nPlayerFixedObject *Level::createShip(int race, int shipClass, int type,\n"
    chain += "                                            Waypoint *wp, bool hostile, bool group) {\n"
    chain += "    firstFriendRace = race; firstFriendClass = shipClass; firstFriendType = type;\n"
    chain += "    firstFriendWaypoint = wp; firstFriendHostile = hostile; firstFriendGroup = group;\n"
    chain += "    throw FirstFriendBoundary{};\n}\n"
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        "catch (const MissionSceneBoundary &) { reachedMissionDispatch = true; }",
        "catch (const FirstFriendBoundary &) { reachedMissionDispatch = true; }")
    gate = replace_once(gate,
        "        auto *friendRoute = observedAmbientFriendRoute;",
        "        assert(race == 0); // Pan's ordinary non-DLC faction.\n"
        "        static const int armRaceTable[37] = {\n"
        "            3,0,8,3,2,0,3,0,9,1,0,8,2,0,0,0,2,0,2,3,\n"
        "            3,2,0,8,8,8,0,0,0,8,3,2,8,0,0,2,0};\n"
        "        unsigned firstType;\n"
        "        do {\n"
        "            firstType = expected.nextInt(37);\n"
        "        } while ((unsigned)((firstType & ~4u) - 9) < 2\n"
        "                 || (firstType < 16 && ((1u << firstType) & 0x8101u))\n"
        "                 || armRaceTable[firstType] != race);\n"
        "        auto *friendRoute = observedAmbientFriendRoute;")
    gate = replace_once(gate,
        "            && level->enemies->size() ==",
        "            && friends > 0 && firstFriendRace == race\n"
        "            && firstFriendClass == 0 && firstFriendType == int(firstType)\n"
        "            && firstFriendWaypoint == friendWp\n"
        "            && firstFriendHostile && !firstFriendGroup\n"
        "            && level->enemies->size() ==")
    gate = replace_once(gate,
        "stage 1 allocated the Pan ordinary enemy array before spawning: OK",
        "stage 1 dispatched the first Pan ordinary friend to createShip: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
