"""Exercise the production gear-free setShip after the Pan original-asset gate."""

import run_station_binary_asset_gate as runner
import run_ship_group_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, '#include "engine/render/Material.h"',
                        '#include "engine/render/Material.h"\n#include "game/weapons/RepairBeam.h"')
    host = replace_once(host, 'PaintCanvas::~PaintCanvas() {}',
        'PaintCanvas::~PaintCanvas() {}\n'
        'Mesh *PaintCanvas::MeshGetPointer(unsigned int index) {\n'
        '    // Bounded platform material service; real catalog material loading is not under test.\n'
        '    static Material shipMaterial;\n'
        '    shipMaterial.materialMode = 7;\n'
        '    auto *mesh = reinterpret_cast<Mesh *>(meshes[index]);\n'
        '    if (index == 68 && mesh && !mesh->materialBlock)\n'
        '        mesh->materialBlock = &shipMaterial;\n'
        '    return mesh;\n'
        '}\n'
        'void PaintCanvas::MeshCloneMaterial(unsigned int, unsigned int &) { std::abort(); }')
    (out / "sun_host.inc").write_text(host)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += ('\nvoid *TractorBeam_new(void *, int);\n'
              'void Vec_assign(void *, const void *);\n'
              'static const float g_PE_ss_emDiv = 500.0f;\n'
              'static const float g_PE_ss_emBias = 0.1f;\n')
    chain += '\n' + extract("src/game/ship/PlayerEgo.cpp", [
        "void PlayerEgo::setShip(int race, int group) {"])
    chain += '\n' + extract("src/game/ship/Ship.cpp", [
        "unsigned char Ship::hasEmergencySystem()"])
    chain += '\nvoid *TractorBeam_new(void *, int) { std::abort(); }\n'
    chain += 'void Vec_assign(void *, const void *) { std::abort(); }\n'
    chain += 'RepairBeam::RepairBeam(int, int) { std::abort(); }\n'
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, 'TRAP(_ZN11AbyssEngine11PaintCanvas14MeshGetPointerEj)', '')
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        'AEGeometry *group = Globals::gGlobals->getShipGroup(0, 0, true);',
        'assert(!observedEgo->tractorBeam && !observedEgo->repairBeams\n'
        '           && observedEgo->asteroidTarget == 0);\n'
        '    observedEgo->setShip(0, 0);\n'
        '    AEGeometry *group = observedEgo->field_0x4;\n'
        '    assert(observedEgo->geometry && observedEgo->geometry != group);\n'
        '    assert(observedEgo->geometry->childTransform == group->transform);\n'
        '    assert(observedEgo->originalMaterialMode == 7);\n'
        '    assert(!observedEgo->tractorBeam && !observedEgo->repairBeams);\n'
        '    assert(!observedEgo->field_0xac && !observedEgo->cloak);')
    gate = replace_once(gate,
        '    assert(canvas.transformCount == transformsBeforeShip + 5);',
        '    assert(canvas.transformCount == transformsBeforeShip + 6);')
    gate = replace_once(gate,
        'Pan ship group -> five original AEM meshes: OK',
        'PlayerEgo::setShip -> Pan group, hull and five original AEM meshes: OK')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
