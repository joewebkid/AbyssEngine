"""Exercise Pan's ordinary mission path to the enemy-array allocation."""

import run_station_binary_asset_gate as runner
import run_level_stage1_mission_overrides_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    body = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = body.index("    bool guardedFreighter = false;")
    guarded = body[start:body.index("    if (guardedFreighter) {", start)]
    start = body.index("    bool miningStation = false;", start)
    ordinary = body[start:body.index("    for (int i = 0; i < friendCount; ++i) {", start)]
    assert "Route *ambientFriendRoute = new Route(friendCoords, 3);" in guarded
    assert "ArraySetLength(friendCount + field_164" in ordinary
    guarded = replace_once(guarded,
        "    Route *ambientFriendRoute = new Route(friendCoords, 3);",
        "    Route *ambientFriendRoute = new Route(friendCoords, 3);\n"
        "    observedAmbientFriendRoute = ambientFriendRoute;")
    extension = (
        "    if (Globals::status->getStation()->getIndex() == 108)\n"
        "        throw MissionDispatchBoundary{};\n"
        + guarded + "    if (guardedFreighter) throw MissionDispatchBoundary{};\n"
        + ordinary)
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level, "    throw MissionSceneBoundary{};\n}",
                         extension + "    throw MissionSceneBoundary{};\n}")
    (out / "level_init_under_test.inc").write_text(level)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct MissionSceneBoundary {};",
                        "struct MissionSceneBoundary {};\n"
                        "static Route *observedAmbientFriendRoute;")
    (out / "sun_host.inc").write_text(host)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/world/Wanted.cpp", [
        "int Wanted::getNumWingmen() {"])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        "           && !status->inPirateLootOrbit());",
        "           && !status->inPirateLootOrbit()\n"
        "           && !status->getCampaignMission()\n"
        "           && !status->inSupernovaSystem());")
    gate = replace_once(gate,
        "           && level->missionPtr == 3 && level->enemies == nullptr",
        "           && level->missionPtr == 3 && level->enemies != nullptr")
    gate = replace_once(gate,
        "        if (f164 + friends + f168 + hostile == 0) friends = 4;\n"
        "        countsMatchReset |= waypoint->x == x && waypoint->z == z",
        "        if (f164 + friends + f168 + hostile == 0) friends = 4;\n"
        "        int friendX = expected.nextInt(20000) - 10000;\n"
        "        int friendY = expected.nextInt(20000) - 10000;\n"
        "        int friendZ = expected.nextInt(30000) + 20000;\n"
        "        int largeTurrets = 0;\n"
        "        int race = status->system->getRace();\n"
        "        if (race == 0 && f168 > 0) {\n"
        "            if (expected.nextInt(100) <= 29) {\n"
        "                (void) expected.nextInt(100);\n"
        "                largeTurrets = 7; // Campaign 50 cannot select alternate large.\n"
        "            }\n"
        "        } else if (race == 1 && f168 > 0) {\n"
        "            (void) expected.nextInt(100); // Campaign 50 cannot select small.\n"
        "        }\n"
        "        auto *friendRoute = observedAmbientFriendRoute;\n"
        "        auto *friendWp = friendRoute ? (*friendRoute->waypoints)[0] : nullptr;\n"
        "        countsMatchReset |= waypoint->x == x && waypoint->z == z\n"
        "            && friendWp && friendWp->route == friendRoute\n"
        "            && friendWp->x == friendX && friendWp->y == friendY\n"
        "            && friendWp->z == friendZ\n"
        "            && level->enemies->size() ==\n"
        "               (unsigned) (friends + f164 + f168 + hostile + largeTurrets)")
    gate = replace_once(gate,
        "    assert(countsMatchReset);",
        "    assert(countsMatchReset);\n"
        "    for (unsigned i = 0; i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);")
    gate = replace_once(gate,
        "stage 1 applied Pan mission count overrides before station-108 dispatch: OK",
        "stage 1 allocated the Pan ordinary enemy array before spawning: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
