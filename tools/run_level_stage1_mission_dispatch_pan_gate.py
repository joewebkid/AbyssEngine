"""Continue Pan stage one through the production empty-mission dispatch."""

import run_station_binary_asset_gate as runner
import run_level_stage1_gas_clouds_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct GasCloudCompleteBoundary {};",
                        "struct MissionDispatchBoundary {};")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        "        thisptr->createGasClouds();\n"
        "        throw GasCloudCompleteBoundary{};",
        "        thisptr->createGasClouds();")
    level += "\nvoid Level::createMission() { throw MissionDispatchBoundary{}; }\n"
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/mission/Mission.cpp", [
        "Mission::Mission() {", "Mission::~Mission() {",
        "bool Mission::isEmpty() {", "bool Mission::isCampaignMission() {",
    ])
    chain += "\n" + extract("src/game/mission/Status.cpp", [
        "Mission *Status::getMission() {", "void Status::setMission(Mission *m) {",
    ])
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    for symbol in (
        "_ZN5Level13createMissionEv",
        "_ZN6Status10getMissionEv",
        "_ZN6Status10setMissionEP7Mission",
        "_ZN7Mission7isEmptyEv",
        "_ZN7Mission17isCampaignMissionEv",
    ):
        traps = replace_once(traps, "TRAP(" + symbol + ")", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    # ARM Level::init tests the saved pointer, so the fixture must be non-null.
    gate = replace_once(gate, "    bool reachedAsteroidPrelude = false;",
        "    auto *emptyMission = new Mission();\n"
        "    assert(emptyMission->isEmpty() && !emptyMission->isCampaignMission());\n"
        "    Mission::empty = emptyMission;\n"
        "    status->mission = emptyMission;\n"
        "    assert(level->missionPtr == 3);\n"
        "    bool reachedAsteroidPrelude = false;")
    gate = replace_once(gate, "catch (const GasCloudCompleteBoundary &)",
                        "catch (const MissionDispatchBoundary &)")
    gate = replace_once(gate,
        "    assert(reachedAsteroidPrelude && level->field_134 == 1);",
        "    assert(reachedAsteroidPrelude && level->field_134 == 1\n"
        "           && status->getMission() == emptyMission);")
    gate = replace_once(gate,
        "stage 1 checked equipped, alien-orbit and boss gas clouds before mission dispatch: OK",
        "stage 1 reached production empty-mission dispatch after gas clouds: OK")
    gate = gate.replace("reachedAsteroidPrelude", "reachedMissionDispatch")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
