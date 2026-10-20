"""Continue the original-asset Level gate into createPlayer's Ship slot prelude."""
import run_station_binary_asset_gate as runner
import run_station_level_space_return_gate as first
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        '        throw LevelSpaceBoundary{}; // Host boundary before createPlayer.', '')
    body = extract("src/game/world/Level.cpp", ["void Level::createPlayer() {"])
    body = replace_once(body, '    slots[2] = ship->getUsedSlots(2);',
        '    slots[2] = ship->getUsedSlots(2);\n'
        '    assert(ship == Status::gStatus->ship && equip == ship->equipment);\n'
        '    assert(slots[0] == 0 && slots[1] == 0 && slots[2] == 0);\n'
        '    throw PlayerSlotsBoundary{}; // Host boundary before Player allocation.')
    level += '\n' + extract("src/game/world/Level.cpp", [
        "static inline float bitsToFloat("]) + '\n' + body
    (out / "level_init_under_test.inc").write_text(level)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, 'struct LevelSpaceBoundary {};',
                        '#include "engine/file/FileRead.h"\nstruct PlayerSlotsBoundary {};')
    (out / "sun_host.inc").write_text(host)

    ship = (ROOT / "src/game/ship/Ship.cpp").read_text()
    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + extract("src/game/mission/Status.cpp", ["Ship *Status::getShip()"])
    chain += '\n' + extract("src/game/mission/Item.cpp", ["int Item::getType()"])
    chain += '\n' + extract("src/game/ship/Ship.cpp", [
        "Ship::Ship(int index,", "Array<Item *> *Ship::getEquipment()",
        "int Ship::getUsedSlots(int type)"])
    # shortcut: empty-equipment fixture bypasses value refresh; use real
    # catalog/equipment before claiming weapon or PlayerEgo behavior.
    chain += '\nvoid Ship::refreshValue() {}\n'
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, 'TRAP(_ZN5Level12createPlayerEv)', '')
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, '    registerResources(&canvas);',
        '    status->ship = new Ship(0, 300, 50, 100, 2, 1, 1, 0, 100.0f);\n'
        '    assert(status->ship->equipment->size() == 4);\n'
        '    registerResources(&canvas);')
    gate = replace_once(gate, 'catch (const LevelSpaceBoundary &)',
                        'catch (const PlayerSlotsBoundary &)')
    gate = replace_once(gate, 'Level::createSpace returned with original Pan star color',
                        'Level::createSpace returned -> createPlayer read Ship slots')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    runner.prepare = prepare
    runner.main()
