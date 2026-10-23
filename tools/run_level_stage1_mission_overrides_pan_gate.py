"""Exercise Pan's post-route mission overrides up to station 108 dispatch."""

import run_station_binary_asset_gate as runner
import run_level_stage1_hostile_route_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    body = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = body.index("    int pirateBase =")
    end = body.index("    if (Globals::status->getStation()->getIndex() == 108) {", start)
    post = body[start:end]
    assert "stationHasHiddenBlueprint(true)" in post
    assert "if (Globals::status->inPirateLootOrbit())" in post
    assert "if (field_164 + friendCount + field_168 + hostileCount + freelanceEnemies == 0) friendCount = 4;" in post
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level, "    throw MissionSceneBoundary{};\n}",
                         post + "    throw MissionSceneBoundary{};\n}")
    (out / "level_init_under_test.inc").write_text(level)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, '#include "game/world/Standing.h"',
                        '#include "game/world/Standing.h"\n#include "game/world/Wanted.h"')
    (out / "sun_host.inc").write_text(host)

    station = (ROOT / "src/game/world/Station.cpp").read_text()
    tables = station[station.index("static const int kHiddenBlueprints"):
                     station.index("Station::Station()")]
    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + tables + extract("src/game/world/Station.cpp", [
        "uint8_t Station::hasAttackedFriends() {",
        "uint32_t Station::stationHasHiddenBlueprint(",
        "uint32_t Station::stationHasPirateBase() {"])
    chain += "\n" + extract("src/game/mission/Status.cpp", [
        "Wanted *Status::getWantedInCurrentOrbit() {",
        "intptr_t Status::getCampaignMission() {",
        "bool Status::inPirateLootOrbit() {"])
    chain += "\n" + extract("src/game/world/Wanted.cpp", [
        "int Wanted::getRequiredBounties() {",
        "int Wanted::getCurrentLocation() {",
        "uint8_t Wanted::isTerminated() {",
        "uint8_t Wanted::isActive() {"])
    chain += "\n" + extract("src/game/world/Route.cpp", [
        "void Route::setNewCoords("])
    chain += "\n" + extract("src/game/mission/Mission.cpp", [
        "int Mission::getTargetStation() {"])
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN6Status18getCampaignMissionEv)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        "           && status->station->getIndex() != 78);",
        "           && status->station->getIndex() != 78\n"
        "           && status->station->getIndex() != 108\n"
        "           && !status->station->stationHasPirateBase()\n"
        "           && !status->station->stationHasHiddenBlueprint(true)\n"
        "           && !status->station->isAttackedByAliens()\n"
        "           && !status->station->hasAttackedFriends()\n"
        "           && !status->getWantedInCurrentOrbit()\n"
        "           && !status->inBlackMarketSystem()\n"
        "           && !status->inPirateLootOrbit());")
    gate = replace_once(gate,
        "        countsMatchReset |= waypoint->x == x && waypoint->z == z",
        "        if (f164 + friends + f168 + hostile == 0) friends = 4;\n"
        "        countsMatchReset |= waypoint->x == x && waypoint->z == z")
    gate = replace_once(gate,
        "    assert(observedAsteroidRadius >= 120 && observedAsteroidRadius <= 219\n",
        "    if (!(observedAsteroidRadius >= 120 && observedAsteroidRadius <= 219\n"
        "          && observedAsteroidScale == float(observedAsteroidRadius) * 0.01f\n"
        "          && observedAsteroidQuality >= 4 && observedAsteroidQuality <= 7))\n"
        "        std::fprintf(stderr, \"Asteroid range: radius=%d scale=%.9g quality=%d\\n\",\n"
        "                     observedAsteroidRadius, observedAsteroidScale, observedAsteroidQuality);\n"
        "    assert(observedAsteroidRadius >= 120 && observedAsteroidRadius <= 219\n")
    gate = replace_once(gate,
        "stage 1 created the Pan hostile route and initial counts: OK",
        "stage 1 applied Pan mission count overrides before station-108 dispatch: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
