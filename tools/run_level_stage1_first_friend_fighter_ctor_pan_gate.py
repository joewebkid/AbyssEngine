"""Execute the production Fighter constructor at Pan's first-friend boundary."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_friend_loot_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\nRoute *PlayerFighter::stationRouteAliens;\n"
    chain += extract("src/game/ship/PlayerFighter.cpp", [
        "PlayerFighter::PlayerFighter(int faction, int wingmanCmd, Player *player, AEGeometry *geom,"])
    # shortcut: uncalled virtuals fail closed; link their real bodies when the
    # Pan gate advances beyond construction into updates, rendering or teardown.
    chain += '''
PlayerFighter::~PlayerFighter() { std::abort(); }
void PlayerFighter::awake() { std::abort(); }
void PlayerFighter::revive() { std::abort(); }
int PlayerFighter::collide(float, float, float) { std::abort(); }
void PlayerFighter::initPush(const Vector &, int) { std::abort(); }
int PlayerFighter::outerCollide(float, float, float) { std::abort(); }
void PlayerFighter::push(int) { std::abort(); }
void PlayerFighter::render() { std::abort(); }
void PlayerFighter::setLevel(Level *) { std::abort(); }
void PlayerFighter::setPosition(float, float, float) { std::abort(); }
void PlayerFighter::setPosition(const Vector &) { std::abort(); }
void PlayerFighter::setShipGroup(AEGeometry *, int, bool) { std::abort(); }
void PlayerFighter::setSpeed(float) { std::abort(); }
void PlayerFighter::setWingmanCommand(int, KIPlayer *) { std::abort(); }
void PlayerFighter::update(int) { std::abort(); }
'''
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, '#include "game/mission/Generator.h"',
        '#include "game/mission/Generator.h"\n#include "game/ship/PlayerFighter.h"')
    start = gate.index("    int filesBeforeItems = opened;")
    end = gate.index("    AERandom *panRandom =", start)
    item_setup = gate[start:end]
    gate = gate[:start] + gate[end:]
    gate = replace_once(gate, "    bool reachedMissionDispatch = false;",
        item_setup + "    bool reachedMissionDispatch = false;")
    gate = replace_once(gate, "    assert(countsMatchReset);", '''    assert(countsMatchReset);
    auto *fighter = new PlayerFighter(firstFriendType, firstFriendRace,
        firstFriendPlayer, nullptr, firstFriendX, firstFriendY, firstFriendZ,
        firstFriendHostile);
    assert(fighter && fighter->player == firstFriendPlayer
           && firstFriendPlayer->kiPlayer == fighter
           && fighter->type == firstFriendType
           && fighter->workingPosition.x == firstFriendX
           && fighter->workingPosition.y == firstFriendY
           && fighter->workingPosition.z == firstFriendZ
           && fighter->field_0x128 == 50000
           && fighter->route && fighter->route->loop
           && fighter->route->waypoints->size() >= 2
           && fighter->route->waypoints->size() <= 4
           && fighter->routeClone && fighter->routeClone != fighter->route
           && fighter->routeClone->waypoints->size() == fighter->route->waypoints->size()
           && fighter->explosion && fighter->explosion->type == 0
           && fighter->explosion->fireStreaks
           && fighter->explosion->fireStreaks->size() >= 3
           && fighter->explosion->fireStreaks->size() <= 9
           && fighter->hitpoints == firstFriendPlayer->getHitpoints());
    if (fighter->lootList)
        assert(fighter->lootList->size() == 2 || fighter->lootList->size() == 4);
    std::printf("Pan first-friend production PlayerFighter constructor: OK\\n");''')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
