"""Continue the Pan Level gate through the real Player constructor and HP setup."""
import run_station_binary_asset_gate as runner
import run_station_player_slots_gate as first
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        '    throw PlayerSlotsBoundary{}; // Host boundary before Player allocation.', '')
    level = replace_once(level, '    pl->field_69 = 1;',
        '    pl->field_69 = 1;\n'
        '    assert(pl->radius == 0x4b0 && pl->maxHitpoints == 300 && pl->hitpoints == 300);\n'
        '    assert(pl->numPrimaryGuns == 0 && pl->numSecondaryGuns == 0 && pl->numTertiaryGuns == 0);\n'
        '    assert(pl->maxShieldHP == 80 && pl->shieldHP == 80.0f);\n'
        '    assert(pl->maxArmorHP == 90 && pl->armorHP == 90 && pl->field_69 == 1);\n'
        '    assert(pl->guns && pl->guns->size() == 3);\n'
        '    for (unsigned i = 0; i < 3; ++i) assert(!(*pl->guns)[i]);\n'
        '    throw PlayerAllocatedBoundary{}; // Host boundary before PlayerEgo allocation.')
    (out / "level_init_under_test.inc").write_text(level)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, 'struct PlayerSlotsBoundary {};',
                        'struct PlayerAllocatedBoundary {};')
    (out / "sun_host.inc").write_text(host)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + extract("src/game/ship/Ship.cpp", [
        "int Ship::getMaxHP()", "int Ship::getMaxShieldHP()",
        "int Ship::getMaxArmorHP()"])
    chain += '\n' + extract("src/game/ship/Player.cpp", [
        "void Player::setMaxShieldHP(", "void Player::setMaxArmorHP("])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, '    assert(status->ship->equipment->size() == 4);',
        '    assert(status->ship->equipment->size() == 4);\n'
        '    status->ship->maxShieldHP = 80;\n'
        '    status->ship->maxArmorHP = 90;')
    gate = replace_once(gate, 'catch (const PlayerSlotsBoundary &)',
                        'catch (const PlayerAllocatedBoundary &)')
    gate = replace_once(gate, 'createPlayer read Ship slots',
                        'createPlayer constructed Player and set HP/shield/armor')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    runner.prepare = prepare
    runner.main()
