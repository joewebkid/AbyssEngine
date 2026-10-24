"""Complete Pan's ordinary-friend loop with original assets on the host."""

import run_station_binary_asset_gate as runner
import run_level_stage1_second_friend_ship_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level,
        "            if (i == 1) throw FirstFriendCompleteBoundary{};\n", "")
    level = replace_once(level, "    throw MissionSceneBoundary{};",
        "    throw FirstFriendCompleteBoundary{};")
    level_path.write_text(level)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate,
        "status->system->securityLevel = 2; // Exercise two friends with Pan assets.",
        "status->system->securityLevel = 3; // Exercise at least three friends with Pan assets.")
    gate = replace_once(gate, "            && friends >= 2 && firstFriendRace == race",
        "            && friends >= 3 && firstFriendRace == race")
    start = gate.index("    const unsigned firstMaterials = 1 +")
    end = gate.index("    if (fighter->lootList)", start)
    gate = gate[:start] + '''    unsigned materialIndex = 0;
    for (int i = 0; i < level->friendCount; ++i) {
        auto *ship = dynamic_cast<PlayerFighter *>((*level->enemies)[i]);
        assert(ship && ship->type >= 0 && ship->type < 37
               && ship->player && ship->player->kiPlayer == ship
               && ship->geometry && ship->parentGeometry
               && ship->level == level && ship->shipGroupFlag == ship->type
               && ship->route && ship->routeClone
               && ship->explosion && ship->explosion->type == 0);
        for (int j = 0; j < i; ++j)
            assert(ship != (*level->enemies)[j]);
        const unsigned count = 1 +
            (gGSG_lodTable[ship->type * 3] != 0xffff) +
            (gGSG_lodTable[ship->type * 3 + 1] != 0xffff);
        for (unsigned j = 0; j < count; ++j)
            assert(materialIndex < changedShipMaterials.size() &&
                   changedShipMaterials[materialIndex++] == 0x7dc8 + ship->type);
    }
    assert(materialIndex == changedShipMaterials.size());
''' + gate[end:]
    gate = replace_once(gate,
        "    for (unsigned i = 2; i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);",
        "    for (unsigned i = level->friendCount; i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);")
    gate = replace_once(gate,
        "    assert(level->lodManager->objects->size() == (unsigned) expectedCount + 4);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2] == fighter->parentGeometry);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 3] == second->parentGeometry);",
        """    assert(level->lodManager->objects->size() ==
           (unsigned) expectedCount + 2 + (unsigned) level->friendCount);
    for (int i = 0; i < level->friendCount; ++i)
        assert((*level->lodManager->objects)[expectedCount + 2 + i] ==
               (*level->enemies)[i]->parentGeometry);""")
    gate = replace_once(gate,
        'std::printf("Pan first two ordinary Fighters entered enemies[0..1]: OK\\n");',
        'std::printf("Pan complete ordinary-friend loop, %d Fighters: OK\\n", level->friendCount);')
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
