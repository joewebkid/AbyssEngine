"""Run two ordinary Pan friend ships through production createShip and insertion."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_friend_ship_return_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    chain_path = out / "station_binary_chain.inc"
    chain = chain_path.read_text()
    capture = '''    firstFriendRace = race; firstFriendClass = shipClass; firstFriendType = type;
    firstFriendWaypoint = wp; firstFriendHostile = hostile; firstFriendGroup = group;
    firstFriendPlayer = pl;
    firstFriendX = fx; firstFriendY = fy; firstFriendZ = fz;
    firstFriendSeedLow = static_cast<AERandom *>(Globals::rnd)->seedLow;
    firstFriendSeedHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;'''
    chain = replace_once(chain, capture,
        "    if (firstFriendType < 0) {\n" + capture + "\n    }")
    chain_path.write_text(chain)

    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level, "if (i == 0) throw FirstFriendCompleteBoundary{};",
        "if (i == 1) throw FirstFriendCompleteBoundary{};")
    level_path.write_text(level)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate, "    auto missionStartSecond = std::time(nullptr);",
        "    status->system->securityLevel = 2; // Exercise two friends with Pan assets.\n"
        "    auto missionStartSecond = std::time(nullptr);")
    gate = replace_once(gate, "            && friends > 0 && firstFriendRace == race",
        "            && friends >= 2 && firstFriendRace == race")
    gate = replace_once(gate,
        "    auto *fighter = dynamic_cast<PlayerFighter *>((*level->enemies)[0]);",
        "    auto *fighter = dynamic_cast<PlayerFighter *>((*level->enemies)[0]);\n"
        "    auto *second = dynamic_cast<PlayerFighter *>((*level->enemies)[1]);\n"
        "    assert(second && second->type >= 0 && second->type < 37);")
    gate = replace_once(gate,
        "    assert((*level->enemies)[0] == fighter);\n"
        "    for (unsigned i = 1; i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);",
        """    assert((*level->enemies)[0] == fighter && second && second != fighter
           && second->type >= 0 && second->type < 37
           && second->player && second->player->kiPlayer == second
           && second->geometry && second->parentGeometry
           && second->level == level && second->shipGroupFlag == second->type
           && second->route && second->route != fighter->route
           && second->routeClone && second->routeClone != second->route
           && second->explosion && second->explosion->type == 0);
    for (unsigned i = 2; i < level->enemies->size(); ++i)
        assert((*level->enemies)[i] == nullptr);""")
    gate = replace_once(gate,
        "    assert(level->lodManager->objects->size() == (unsigned) expectedCount + 3);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2] == fighter->parentGeometry);",
        "    assert(level->lodManager->objects->size() == (unsigned) expectedCount + 4);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2] == fighter->parentGeometry);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 3] == second->parentGeometry);")
    gate = replace_once(gate,
        "    assert(changedShipMaterials.size() == 1 +\n"
        "           (gGSG_lodTable[firstFriendType * 3] != 0xffff) +\n"
        "           (gGSG_lodTable[firstFriendType * 3 + 1] != 0xffff));\n"
        "    for (unsigned short id : changedShipMaterials)\n"
        "        assert(id == 0x7dc8 + firstFriendType);",
        """    const unsigned firstMaterials = 1 +
        (gGSG_lodTable[firstFriendType * 3] != 0xffff) +
        (gGSG_lodTable[firstFriendType * 3 + 1] != 0xffff);
    const unsigned secondMaterials = 1 +
        (gGSG_lodTable[second->type * 3] != 0xffff) +
        (gGSG_lodTable[second->type * 3 + 1] != 0xffff);
    assert(changedShipMaterials.size() == firstMaterials + secondMaterials);
    for (unsigned i = 0; i < changedShipMaterials.size(); ++i)
        assert(changedShipMaterials[i] == 0x7dc8 +
               (i < firstMaterials ? firstFriendType : second->type));""")
    gate = replace_once(gate,
        'Pan first-friend createShip returned Fighter into enemies[0]: OK',
        'Pan first two ordinary Fighters entered enemies[0..1]: OK')
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
