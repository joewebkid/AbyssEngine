"""Exercise Pan's first pre-dead Fighter after ordinary friends."""

import run_station_binary_asset_gate as runner
import run_level_stage1_all_ordinary_friends_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    mission = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = mission.index("    int index = friendCount;")
    dead_loop = mission[start:mission.index("    int foreignRace =", start)]
    assert "(*enemies)[index]->setDead();" in dead_loop
    assert "(*enemies)[index]->setRoute(new Route(coords, 3));" in dead_loop
    dead_loop = replace_once(dead_loop,
        "        (*enemies)[index]->setRoute(new Route(coords, 3));",
        "        int routeDeletesBefore = routeDestructions;\n"
        "        (*enemies)[index]->setRoute(new Route(coords, 3));\n"
        "        assert(routeDestructions == routeDeletesBefore + 1);")

    host_path = out / "sun_host.inc"
    host = host_path.read_text()
    host = replace_once(host, "struct FirstFriendCompleteBoundary {};",
        "struct FirstFriendCompleteBoundary {};\nstatic int routeDestructions;")
    host_path.write_text(host)

    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level,
        "        field_164 = static_cast<AERandom *>(Globals::rnd)->nextInt(2);",
        "        field_164 = static_cast<AERandom *>(Globals::rnd)->nextInt(2);\n"
        "        field_164 = 1; // Test variant: keep the RNG draw, force one pre-dead ship.")
    level = replace_once(level, "    throw FirstFriendCompleteBoundary{};\n}",
        dead_loop + "    throw FirstFriendCompleteBoundary{};\n}")
    level_path.write_text(level)

    chain_path = out / "station_binary_chain.inc"
    chain = chain_path.read_text()
    chain += "\n" + extract("src/game/ship/KIPlayer.cpp", [
        "void KIPlayer::setActive(bool active) {",
        "void KIPlayer::setDead() {",
        "void KIPlayer::setRoute(Route *route) {",
        "void KIPlayer::setJumper(bool b) {"])
    route_destructor = extract("src/game/world/Route.cpp", ["Route::~Route() {"])
    chain += "\n" + replace_once(route_destructor, "Route::~Route() {",
        "Route::~Route() {\n    ++routeDestructions;")
    chain += "\n" + extract("src/game/ship/Player.cpp", ["Player::~Player() {"])
    chain += "\nusing VecArray = Array<Vector>;\n" + extract(
        "src/game/weapons/Gun.cpp", ["Gun::~Gun() noexcept(false) {"])
    chain_path.write_text(chain)

    traps_path = out / "station_binary_traps.inc"
    traps = traps_path.read_text()
    traps = replace_once(traps, "TRAP(_ZN5RouteD1Ev)", "")
    traps = replace_once(traps, "TRAP(_ZN6PlayerD1Ev)", "")
    traps_path.write_text(traps)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate, '#include "game/ship/PlayerFighter.h"',
        '#include "game/ship/PlayerFighter.h"\n#include "game/weapons/Gun.h"')
    gate = replace_once(gate, "        int f164 = expected.nextInt(2);",
        "        int f164 = expected.nextInt(2);\n"
        "        f164 = 1; // Match the test-only one-dead-ship variant.")
    gate = replace_once(gate,
        "    for (int i = 0; i < level->friendCount; ++i) {\n"
        "        auto *ship = dynamic_cast<PlayerFighter *>((*level->enemies)[i]);",
        "    for (int i = 0; i < level->friendCount + level->field_164; ++i) {\n"
        "        auto *ship = dynamic_cast<PlayerFighter *>((*level->enemies)[i]);")
    gate = replace_once(gate,
        "    for (unsigned i = level->friendCount; i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);",
        """    assert(level->field_164 == 1);
    auto *dead = dynamic_cast<PlayerFighter *>((*level->enemies)[level->friendCount]);
    assert(dead && dead->player && dead->state == 4 && !dead->player->active
           && dead->jumperFlag && dead->route && dead->routeClone
           && dead->routeClone == dead->initialRoute
           && dead->route != dead->routeClone && dead->routeClone->waypoints
           && dead->routeClone->waypoints->size() == 1);
    auto *deadWaypoint = (*dead->routeClone->waypoints)[0];
    assert(deadWaypoint && deadWaypoint->route == dead->routeClone
           && deadWaypoint->x >= -200000 && deadWaypoint->x < 200000
           && deadWaypoint->y >= -100000 && deadWaypoint->y < 100000
           && deadWaypoint->z >= 50000 && deadWaypoint->z < 150000);
    for (unsigned i = level->friendCount + level->field_164;
         i < level->enemies->size(); ++i)
        assert((*level->enemies)[i] == nullptr);""")
    gate = replace_once(gate,
        "           (unsigned) expectedCount + 2 + (unsigned) level->friendCount);\n"
        "    for (int i = 0; i < level->friendCount; ++i)",
        "           (unsigned) expectedCount + 2 + (unsigned) level->friendCount +\n"
        "           (unsigned) level->field_164);\n"
        "    for (int i = 0; i < level->friendCount + level->field_164; ++i)")
    gate = replace_once(gate,
        'std::printf("Pan complete ordinary-friend loop, %d Fighters: OK\\n", level->friendCount);',
        'std::printf("Pan ordinary friends + one pre-dead Fighter: %d + %d OK\\n", level->friendCount, level->field_164);')
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
