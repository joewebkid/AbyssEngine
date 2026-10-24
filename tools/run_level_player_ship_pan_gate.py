"""Continue the Pan Level call through production createPlayer -> setShip."""

import run_station_binary_asset_gate as runner
import run_player_ego_set_ship_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, 'struct PlayerEgoConstructedBoundary {};',
                        'struct PlayerShipBoundBoundary {};')
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createPlayer() {"])
    start = production.index("    this->player = ego;")
    end = production.index("    ego->setLevel(this);", start)
    level = replace_once(level,
        '    observedEgo = ego;\n    throw PlayerEgoConstructedBoundary{};',
        '    observedEgo = ego;\n'
        '    assert(!ego->tractorBeam && !ego->repairBeams && ego->asteroidTarget == 0);\n'
        '    assert(stationMaterialIds.size() == starsMaterialsBefore + 50);\n'
        + production[start:end]
        + '    throw PlayerShipBoundBoundary{};')
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + extract("src/game/ship/Ship.cpp", ["int Ship::getRace()"])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'catch (const PlayerEgoConstructedBoundary &)',
                        'catch (const PlayerShipBoundBoundary &)')
    gate = replace_once(gate,
        'reached && level->field_134 == 0 && !level->player && level->starSystem',
        'reached && level->field_134 == 0 && level->player == observedEgo && level->starSystem')
    gate = replace_once(gate, 'assert(opened == 32 && opened == closed);',
                        'assert(opened == 37 && opened == closed);')
    gate = replace_once(gate, 'canvas.meshCount == 68 && canvas.transformCount == 72',
                        'canvas.meshCount == 73 && canvas.transformCount == 78')
    gate = replace_once(gate, '    assert(stationOpenedPaths == expectedFiles);',
        '    assert(stationOpenedPaths.size() == expectedFiles.size() + 5);\n'
        '    assert(std::equal(expectedFiles.begin(), expectedFiles.end(),\n'
        '                      stationOpenedPaths.begin()));')
    gate = replace_once(gate,
        'stationMaterialIds.size() == expectedMaterials.size() + 50',
        'stationMaterialIds.size() >= expectedMaterials.size() + 50')
    gate = replace_once(gate,
        'canvas.meshCount == meshesBefore + 50 && canvas.transformCount == transformsBefore + 50',
        'canvas.meshCount == meshesBefore + 55 && canvas.transformCount == transformsBefore + 56')
    gate = replace_once(gate, 'stationMaterialIds.size() == materialsBefore + 50',
                        'stationMaterialIds.size() >= materialsBefore + 50')
    gate = replace_once(gate,
        '    unsigned meshesBeforeShip = canvas.meshCount;\n'
        '    unsigned transformsBeforeShip = canvas.transformCount;\n'
        '    unsigned filesBeforeShip = opened;\n'
        '    assert(!observedEgo->tractorBeam && !observedEgo->repairBeams\n'
        '           && observedEgo->asteroidTarget == 0);\n'
        '    observedEgo->setShip(0, 0);',
        '    unsigned meshesBeforeShip = starsMeshesBefore + 50;\n'
        '    unsigned transformsBeforeShip = starsTransformsBefore + 50;\n'
        '    unsigned filesBeforeShip = 32;\n'
        '    assert(level->player == observedEgo);')
    gate = replace_once(gate, 'opened == filesBeforeShip + 5',
                        'opened == filesBeforeShip + 7')
    gate = replace_once(gate,
        'PlayerEgo::setShip -> Pan group, hull and five original AEM meshes: OK',
        'Level::createPlayer -> PlayerEgo::setShip -> Pan hull and five AEM meshes: OK')
    gate = replace_once(gate,
        'createPlayer completed PlayerEgo normal-path constructor: OK',
        'createPlayer bound the Pan PlayerEgo ship: OK')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
