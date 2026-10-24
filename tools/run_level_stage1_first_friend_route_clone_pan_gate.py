"""Exercise the production Route clone used by first-friend Fighter setup."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_friend_player_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/world/Route.cpp", [
        "Route *Route::clone() {",
        "Route::Route(int *coords, Array<KIPlayer *> *targets, int *times, int count) {",
        "void Route::setLoop(bool loop) {"])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "    assert(countsMatchReset);", """    assert(countsMatchReset);
    Route *source = observedAmbientFriendRoute;
    assert(source && source->waypoints->size() == 1
           && source->dockingTargets->size() == 1
           && source->dockingTimes->size() == 1);
    Waypoint *original = (*source->waypoints)[0];
    Route *plain = source->clone();
    assert(plain != source && plain->waypoints->size() == 1
           && (*plain->waypoints)[0] != original
           && (*plain->waypoints)[0]->route == plain
           && (*plain->waypoints)[0]->x == original->x
           && (*plain->waypoints)[0]->y == original->y
           && (*plain->waypoints)[0]->z == original->z
           && (*plain->dockingTargets)[0] == nullptr
           && plain->loop == source->loop);
    const bool priorLoop = source->loop;
    source->setLoop(true);
    (*source->dockingTargets)[0] = original;
    (*source->dockingTimes)[0] = 1234;
    Route *docked = source->clone();
    assert(docked != source && docked->waypoints->size() == 1
           && (*docked->waypoints)[0] != original
           && (*docked->waypoints)[0]->route == docked
           && (*docked->waypoints)[0]->x == original->x
           && (*docked->waypoints)[0]->y == original->y
           && (*docked->waypoints)[0]->z == original->z
           && docked->dockingTargets != source->dockingTargets
           && (*docked->dockingTargets)[0] == original
           && (*docked->dockingTimes)[0] == 1234
           && docked->loop);
    // The fixture ends here; the unrelated Player/Waypoint destructor chain is not linked.
    (*source->dockingTargets)[0] = nullptr;
    (*source->dockingTimes)[0] = 0;
    source->setLoop(priorLoop);
""")
    gate = replace_once(gate,
        "stage 1 created the first Pan friend's Player before Fighter construction: OK",
        "stage 1 verified both first-friend Route clone branches: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
