"""Return from production createPlayer on the gear-free Pan Level path."""

import run_station_binary_asset_gate as runner
import run_level_player_weapon_positions_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct PlayerWeaponPositionsBoundary {};",
                        "struct PlayerCreatedBoundary {};\n"
                        "static unsigned observedWeaponCounts[4];")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createPlayer() {"])
    start = production.index("        for (unsigned i = 0; i < equip->size(); i++) {")
    level = replace_once(level,
        "        observedWeaponPositions = wpos;\n"
        "        throw PlayerWeaponPositionsBoundary{};\n"
        "    }\n}",
        "        observedWeaponPositions = wpos;\n"
        "        for (unsigned i = 0; i < 4; ++i)\n"
        "            observedWeaponCounts[i] = (*wpos)[i] ? (*wpos)[i]->size() : 0;\n"
        + production[start:])
    level = replace_once(level, "            thisptr->createPlayer();",
                         "            thisptr->createPlayer();\n"
                         "            throw PlayerCreatedBoundary{};")
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/mission/Item.cpp", ["bool Item::isWeapon() {"])
    chain += "\n" + extract("src/game/ship/Ship.cpp", [
        "int Ship::getFireRateFactor()", "int Ship::getDamageFactor()"])
    chain += """
unsigned int Ship::getSlotPos(Item *) { std::abort(); }
Gun *Level::createGun(int, int, int, int, int, int, int, int) { std::abort(); }
void Gun::setMagnitude(int) { std::abort(); }
void Gun::setOffset(Vector *) { std::abort(); }
void PlayerEgo::setTurretPosition(Vector) { std::abort(); }
void PlayerEgo::addGun(Array<Gun *> *, int) { std::abort(); }
"""
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "catch (const PlayerWeaponPositionsBoundary &)",
                        "catch (const PlayerCreatedBoundary &)")
    start = gate.index("    assert(observedWeaponPositions && observedWeaponPositions->size() == 4);")
    end = gate.index("    assert(level->field_a4 && level->field_a4->size() == 4);", start)
    gate = gate[:start] + (
        "    assert(observedWeaponCounts[0] == 1 && observedWeaponCounts[1] == 1);\n"
        "    assert(observedWeaponCounts[2] == 0 && observedWeaponCounts[3] == 8);\n"
    ) + gate[end:]
    gate = replace_once(gate,
        "Level::createPlayer -> original weapon positions and four geometries: OK",
        "Level::createPlayer returned after original weapon positions and cleanup: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
