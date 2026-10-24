"""Run four ordinary Pan freighters through the recovered mission loop."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_freighter_motion_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level,
        "field_168 = 1; // Test variant: keep the RNG draw, force one freighter.",
        "field_168 = 4; // Test variant: keep the RNG draw, exercise the maximum count.")
    level = replace_once(level,
        "assert(index == start && !largeOutpost && !smallOutpost);",
        "assert(index >= start && index < start + 4 && !largeOutpost && !smallOutpost);")
    level_path.write_text(level)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate,
        "f168 = 1; // Match the test-only one-freighter variant.",
        "f168 = 4; // Match the test-only four-freighter variant.")
    gate = replace_once(gate,
        "assert(level->field_168 == 1 && freighterIndex ==\n"
        "           level->friendCount + level->field_164",
        "assert(level->field_168 == 4 && freighterIndex ==\n"
        "           level->friendCount + level->field_164 + 3")
    gate = replace_once(gate,
        "    for (unsigned i = level->friendCount + level->field_164 + 1;\n",
        "    for (unsigned i = level->friendCount + level->field_164 + 4;\n")
    gate = replace_once(gate,
        "           (unsigned) level->field_164 + 1);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2 + level->friendCount + level->field_164] == freighter->geometry);",
        "           (unsigned) level->field_164 + 4);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2 + level->friendCount + level->field_164 + 3] == freighter->geometry);")
    gate = replace_once(gate,
        '    std::printf("Pan first class-one freighter moving and relocation: OK\\n");',
        '''    int firstFreighter = level->friendCount + level->field_164;
    for (int j = 0; j < 4; ++j) {
        auto *ship = dynamic_cast<PlayerFixedObject *>((*level->enemies)[firstFreighter + j]);
        assert(ship && ship->player && ship->player->kiPlayer == ship
               && ship->level == level && ship->type == 15
               && (ship->faction == 0 || ship->faction == 2)
               && ship->moving && ship->geometry && ship->wreckGeometry
               && ship->boundingVolumes && ship->boundingVolumes->size() == 3
               && ship->wreckCollision && !ship->wreckCollision->empty()
               && ship->wreckMeshId == (ship->faction == 2 ? 18301 : 18302)
               && ship->wreckType == (ship->faction == 2 ? 2 : 3)
               && ship->intPosX == int(ship->position.x)
               && ship->intPosY == int(ship->position.y)
               && ship->intPosZ == int(ship->position.z));
        for (int k = 0; k < j; ++k)
            assert(ship != (*level->enemies)[firstFreighter + k]);
        auto position = ship->geometry->getPosition();
        auto wreck = ship->wreckGeometry->getPosition();
        assert(position.x == ship->position.x && position.y == ship->position.y
               && position.z == ship->position.z
               && wreck.x == position.x && wreck.y == position.y && wreck.z == position.z);
        for (auto *bv : *ship->boundingVolumes)
            assert(bv && bv->centerX == position.x && bv->centerY == position.y
                   && bv->centerZ == position.z);
    }
    std::printf("Pan four ordinary class-one freighters: OK\\n");''')
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
