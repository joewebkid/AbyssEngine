"""Continue the Pan original-asset gate through Level::createSpace return."""
import run_station_light_complete_gate as first
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level, function(level, "void Level::createSpace() {"),
                         extract("src/game/world/Level.cpp", ["void Level::createSpace() {"]))
    init = function(level, "int Level::init() {")
    init = replace_once(init, '        thisptr->createSpace();',
                        '        thisptr->createSpace();\n'
                        '        throw LevelSpaceBoundary{}; // Host boundary before createPlayer.')
    level = replace_once(level, function(level, "int Level::init() {"), init)
    (out / "level_init_under_test.inc").write_text(level)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, 'struct StarSystemCtorBoundary {};',
                        'struct LevelSpaceBoundary {};')
    (out / "sun_host.inc").write_text(host)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'catch (const StarSystemCtorBoundary &)',
                        'catch (const LevelSpaceBoundary &)')
    gate = replace_once(gate,
        '    assert(star->fogEnabled == 0 && canvas.fogMode == 0 && canvas.fogEnableFlag == 1);',
        '    assert(star->fogEnabled == 0 && canvas.fogMode == 0 && canvas.fogEnableFlag == 1);\n'
        '    assert(Level::r == expectedSystems[1].rgb[0]);\n'
        '    assert(Level::g == expectedSystems[1].rgb[1]);\n'
        '    assert(Level::b == expectedSystems[1].rgb[2]);\n'
        '    assert(Level::i_r == Level::r && Level::i_g == Level::g && Level::i_b == Level::b);\n'
        '    assert(Level::r_min == int(Level::r / 3.0f));\n'
        '    assert(Level::g_min == int(Level::g / 3.0f));\n'
        '    assert(Level::b_min == int(Level::b / 3.0f));')
    gate = replace_once(gate, 'StarSystem constructor returned after real light setters',
                        'Level::createSpace returned with original Pan star color')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.first.first.first.first.first.first.first.sun.station.prepare = prepare
    first.first.first.first.first.first.first.first.sun.station.main()
