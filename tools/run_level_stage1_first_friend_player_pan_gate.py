"""Exercise the real first-friend createShip prelude through Player setup."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_friend_dispatch_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    ship = extract("src/game/world/Level.cpp", [
        "PlayerFixedObject *Level::createShip(int race, int shipClass, int type, Waypoint *wp, bool hostile, bool group) {"])
    prelude = ship[:ship.index("    PlayerFixedObject *obj = 0;")]
    assert "pl->setEmpData(empA, empB);" in prelude
    chain = (out / "station_binary_chain.inc").read_text()
    stub = (
        "PlayerFixedObject *Level::createShip(int race, int shipClass, int type,\n"
        "                                            Waypoint *wp, bool hostile, bool group) {\n"
        "    firstFriendRace = race; firstFriendClass = shipClass; firstFriendType = type;\n"
        "    firstFriendWaypoint = wp; firstFriendHostile = hostile; firstFriendGroup = group;\n"
        "    throw FirstFriendBoundary{};\n}\n"
    )
    live = prelude + (
        "    firstFriendRace = race; firstFriendClass = shipClass; firstFriendType = type;\n"
        "    firstFriendWaypoint = wp; firstFriendHostile = hostile; firstFriendGroup = group;\n"
        "    firstFriendPlayer = pl;\n"
        "    firstFriendX = fx; firstFriendY = fy; firstFriendZ = fz;\n"
        "    throw FirstFriendPlayerBoundary{};\n}\n"
    )
    chain = replace_once(chain, stub, live)
    chain += "\n" + extract("src/game/ship/Player.cpp", [
        "void Player::setEmpData(int points, int data) {"])
    (out / "station_binary_chain.inc").write_text(chain)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct FirstFriendBoundary {};",
        "struct FirstFriendBoundary {};\nstruct FirstFriendPlayerBoundary {};\n"
        "static Player *firstFriendPlayer;\n"
        "static float firstFriendX, firstFriendY, firstFriendZ;")
    (out / "sun_host.inc").write_text(host)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        "catch (const FirstFriendBoundary &) { reachedMissionDispatch = true; }",
        "catch (const FirstFriendPlayerBoundary &) { reachedMissionDispatch = true; }")
    gate = replace_once(gate,
        "        auto *friendRoute = observedAmbientFriendRoute;",
        "        int jitterX = expected.nextInt(40000);\n"
        "        int jitterY = expected.nextInt(40000);\n"
        "        int jitterZ = expected.nextInt(40000);\n"
        "        auto *friendRoute = observedAmbientFriendRoute;")
    gate = replace_once(gate,
        "            && firstFriendHostile && !firstFriendGroup",
        "            && firstFriendHostile && !firstFriendGroup\n"
        "            && firstFriendPlayer && firstFriendPlayer->radius == 1000\n"
        "            && firstFriendPlayer->hitpoints == 405\n"
        "            && firstFriendPlayer->maxHitpoints == 405\n"
        "            && firstFriendPlayer->empPoints ==\n"
        "               (status->getLevel() > 20 ? 20 : status->getLevel()) * 5 + 40\n"
        "            && firstFriendPlayer->maxEmpPoints == firstFriendPlayer->empPoints\n"
        "            && firstFriendPlayer->empData == 15000\n"
        "            && firstFriendX == float(friendWp->x + jitterX - 20000)\n"
        "            && firstFriendY == float(friendWp->y + jitterY - 20000)\n"
        "            && firstFriendZ == float(friendWp->z + jitterZ - 20000)")
    gate = replace_once(gate,
        "stage 1 dispatched the first Pan ordinary friend to createShip: OK",
        "stage 1 created the first Pan friend's Player before Fighter construction: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
