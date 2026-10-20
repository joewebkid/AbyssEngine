"""Exercise the Pan post-scene mission prefix through hostile Route creation."""

import run_station_binary_asset_gate as runner
import run_level_stage1_scene_zero_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    body = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = body.index("    bool earlySystem = false;")
    post = body[start:body.index(
        "    int race = Globals::status->getSystem()->getRace();", start)]
    assert "field_180 = new Route(hostileCoords, 3);" in post
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        "    missionPtr = 3;\n    throw MissionSceneBoundary{};\n}",
        "    missionPtr = 3;\n" + post +
        "    throw MissionSceneBoundary{};\n}")
    (out / "level_init_under_test.inc").write_text(level)

    system = (ROOT / "src/game/world/SolarSystem.cpp").read_text()
    pirate = system[system.index("static const int kPirateBaseStations"):
                    system.index("static const int kAttackRaceTable")]
    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/mission/Status.cpp", [
        "Mission *Status::getFreelanceMission() {"])
    chain += "\n" + extract("src/game/mission/Mission.cpp", [
        "int Mission::getType() {", "int Mission::getDifficulty() {"])
    chain += "\n" + extract("src/game/world/SolarSystem.cpp", [
        "int SolarSystem::getSecurityLevel() {",
        "int SolarSystem::stationIsInSystem(int idx) {"])
    chain += "\n" + pirate
    chain += "\n" + extract("src/game/world/Route.cpp", [
        "Route::Route(int *coords, int count) {"])
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN6Status19getFreelanceMissionEv)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "    auto missionStartSecond = std::time(nullptr);",
        "    assert(level->field_180 == nullptr\n"
        "           && status->getFreelanceMission() == nullptr\n"
        "           && status->system->getIndex() != 15\n"
        "           && !status->system->hasPirateBase());\n"
        "    auto missionStartSecond = std::time(nullptr);")
    start = gate.index("    auto missionEndSecond = std::time(nullptr);")
    end = gate.index("    assert(plasmaProbabilityBuffer", start)
    gate = replace_once(gate, gate[start:end],
        "    auto missionEndSecond = std::time(nullptr);\n"
        "    auto *route = level->field_180;\n"
        "    assert(route && route->waypoints && route->waypoints->size() == 1\n"
        "           && route->dockingTargets->size() == 1\n"
        "           && route->dockingTimes->size() == 1);\n"
        "    auto *waypoint = (*route->waypoints)[0];\n"
        "    assert(waypoint && waypoint->route == route && waypoint->y == 0);\n"
        "    bool routeMatchesReset = false;\n"
        "    for (long long s = missionStartSecond; s <= missionEndSecond; ++s) {\n"
        "        AERandom expected(0);\n"
        "        expected.seedLow = 0xdeece66dU ^ static_cast<uint32_t>(s);\n"
        "        expected.seedHigh = (static_cast<uint32_t>(s >> 31) & 0xffffU) ^ 5U;\n"
        "        (void) expected.nextInt(100);\n"
        "        int x = expected.nextInt(100000) - 50000;\n"
        "        int z = expected.nextInt(50000) + 50000;\n"
        "        routeMatchesReset |= waypoint->x == x && waypoint->z == z;\n"
        "    }\n"
        "    assert(routeMatchesReset);")
    gate = replace_once(gate,
        "stage 1 returned from mode-zero createScene and restored mission mode: OK",
        "stage 1 created the Pan hostile route after scene return: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
