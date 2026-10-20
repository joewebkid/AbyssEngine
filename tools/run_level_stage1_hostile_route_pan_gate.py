"""Exercise the Pan post-scene route and first hostile/friendly counts."""

import run_station_binary_asset_gate as runner
import run_level_stage1_scene_zero_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    body = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = body.index("    bool earlySystem = false;")
    post = body[start:body.index("    int pirateBase =", start)]
    assert "field_180 = new Route(hostileCoords, 3);" in post
    assert "friendCount = security + additionalFriends + field_168 / 4;" in post
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        "    missionPtr = 3;\n    throw MissionSceneBoundary{};\n}",
        "    missionPtr = 3;\n" + post +
        "    throw MissionSceneBoundary{};\n}")
    (out / "level_init_under_test.inc").write_text(level)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "#include <ctime>",
                        '#include <ctime>\n#include <cstring>\n#include "game/world/Standing.h"')
    (out / "sun_host.inc").write_text(host)

    system = (ROOT / "src/game/world/SolarSystem.cpp").read_text()
    pirate = system[system.index("static const int kPirateBaseStations"):
                    system.index("static const int kAttackRaceTable")]
    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/mission/Status.cpp", [
        "Mission *Status::getFreelanceMission() {",
        "Standing *Status::getStanding() {", "int Status::getLevel() {"])
    chain += "\n" + extract("src/game/mission/Mission.cpp", [
        "int Mission::getType() {", "int Mission::getDifficulty() {"])
    chain += "\n" + extract("src/game/world/SolarSystem.cpp", [
        "int SolarSystem::getSecurityLevel() {",
        "int SolarSystem::stationIsInSystem(int idx) {"])
    chain += "\n" + pirate
    chain += "\n" + extract("src/game/world/Route.cpp", [
        "Route::Route(int *coords, int count) {"])
    standing = (ROOT / "src/game/world/Standing.cpp").read_text()
    table_start = standing.index("static const uint32_t Standing_enemyRaceTable")
    chain += "\n" + extract("src/game/world/Standing.cpp", ["Standing::Standing() {"])
    chain += "\n" + standing[table_start:standing.index("unsigned Standing::isNeutral", table_start)]
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN6Status19getFreelanceMissionEv)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "    auto missionStartSecond = std::time(nullptr);",
        "    float difficulty = 1.0f;\n"
        "    std::memcpy(Globals::options + 0x2c, &difficulty, sizeof(difficulty));\n"
        "    status->standing = new Standing();\n"
        "    assert(!status->hardCoreMode() && level->field_180 == nullptr\n"
        "           && status->getFreelanceMission() == nullptr\n"
        "           && status->system->getIndex() != 15\n"
        "           && !status->system->hasPirateBase()\n"
        "           && status->station->getIndex() != 78);\n"
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
        "    bool countsMatchReset = false;\n"
        "    for (long long s = missionStartSecond; s <= missionEndSecond; ++s) {\n"
        "        AERandom expected(0);\n"
        "        expected.seedLow = 0xdeece66dU ^ static_cast<uint32_t>(s);\n"
        "        expected.seedHigh = (static_cast<uint32_t>(s >> 31) & 0xffffU) ^ 5U;\n"
        "        int attackRoll = expected.nextInt(100);\n"
        "        int x = expected.nextInt(100000) - 50000;\n"
        "        int z = expected.nextInt(50000) + 50000;\n"
        "        (void) expected.nextInt(100); // Enemy-race choice.\n"
        "        int security = status->system->getSecurityLevel();\n"
        "        int probability = security == 0 ? 90 : security == 1 ? 65\n"
        "                        : security == 2 ? 35 : 10;\n"
        "        bool attackers = attackRoll < probability;\n"
        "        int hostile = attackers ? expected.nextInt(4) : 0;\n"
        "        if (hostile >= 1) {\n"
        "            hostile = int(float(hostile) + (difficulty - 0.5f) * float(hostile));\n"
        "            hostile += status->getLevel() / 4;\n"
        "        }\n"
        "        int f164 = expected.nextInt(2);\n"
        "        int f168 = expected.nextInt(5);\n"
        "        int extra = expected.nextInt(2);\n"
        "        int friends = security + extra + f168 / 4;\n"
        "        if (attackers && status->getCurrentCampaignMission() >= 32\n"
        "            && expected.nextInt(100) <= 7) friends = hostile = 9;\n"
        "        countsMatchReset |= waypoint->x == x && waypoint->z == z\n"
        "            && level->hostileCount == hostile && level->friendCount == friends\n"
        "            && level->field_164 == f164 && level->field_168 == f168\n"
        "            && static_cast<AERandom *>(Globals::rnd)->seedLow == expected.seedLow\n"
        "            && static_cast<AERandom *>(Globals::rnd)->seedHigh == expected.seedHigh;\n"
        "    }\n"
        "    assert(countsMatchReset);")
    gate = replace_once(gate,
        "stage 1 returned from mode-zero createScene and restored mission mode: OK",
        "stage 1 created the Pan hostile route and initial counts: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
