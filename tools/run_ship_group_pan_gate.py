"""Extend the Pan original-asset gate through the real first ship-group loader."""

import run_station_binary_asset_gate as runner
import run_station_player_ego_stars_gate as first
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, '#include "engine/render/MovingStars.h"',
                        '#include "engine/render/MovingStars.h"\n#include "engine/render/Material.h"')
    host = host.replace('|| id == 20052)',
                        '|| id == 20052 || id == 0 || id == 34600 || id == 34811)')
    host = replace_once(host,
        '(!r && (id == 3 || id == 65535 || id == 20052 || id == 0 || id == 34600 || id == 34811))',
        '(!r && (id == 3 || id == 65535 || id == 20052 || id == 0))')
    (out / "sun_host.inc").write_text(host)

    host = (out / "station_mesh_host.inc").read_text()
    host = replace_once(host,
        '    void MaterialCreate(unsigned short id, unsigned int &) { lastMaterialId = id; }',
        '    void MaterialCreate(unsigned short id, unsigned int &) { lastMaterialId = id; }\n'
        '    void MeshChangeResourceMaterial(unsigned int, unsigned short);')
    (out / "station_mesh_host.inc").write_text(host)

    chain = (out / "station_binary_chain.inc").read_text()
    globals_source = (ROOT / "src/game/core/Globals.cpp").read_text()
    begin = globals_source.index("static const unsigned short gGSG_resTable[")
    end = globals_source.index("AEGeometry *Globals::getShipGroup(", begin)
    chain += '\n' + globals_source[begin:end]
    chain += '\nint AERandom_nextIntB(int, int);\n'
    chain += '\n' + extract("src/game/core/Globals.cpp", [
        "AEGeometry *Globals::getShipGroup(int kind, int variant, bool wireframe) {"])
    # The selected kind 0 never enters the special kind-15 RNG or non-wireframe material path.
    chain += '\nint AERandom_nextIntB(int, int) { std::abort(); }\n'
    chain += ('\nvoid PaintCanvas::MeshChangeResourceMaterial(unsigned int, unsigned short) '
              '{ std::abort(); }\n')
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    marker = '    return 0;\n}'
    assert gate.count(marker) == 1
    check = '''    // Original resource table: Midorian base, additive lights/engine, two LOD meshes.
    unsigned meshesBeforeShip = canvas.meshCount;
    unsigned transformsBeforeShip = canvas.transformCount;
    unsigned filesBeforeShip = opened;
    AEGeometry *group = Globals::gGlobals->getShipGroup(0, 0, true);
    assert(group && group->mesh == 17000 && group->meshId == meshesBeforeShip);
    assert(group->lodCount == 2 && group->lodMeshes[0] == 17100
           && group->lodMeshes[1] == 17101);
    assert(canvas.meshCount == meshesBeforeShip + 5);
    assert(canvas.transformCount == transformsBeforeShip + 5);
    assert(opened == filesBeforeShip + 5 && opened == closed);
    const char *shipFiles[] = {
        "data/assets/main/3d/meshes/ships/ship_000_midorian.aem",
        "data/assets/main/3d/meshes/ships/ship_000_midorian_lights_add.aem",
        "data/assets/main/3d/meshes/ships/ship_000_midorian_engine_glow_add.aem",
        "data/assets/main/3d/meshes/ships/ship_000_midorian_lod_1.aem",
        "data/assets/main/3d/meshes/ships/ship_000_midorian_lod_2.aem"};
    assert(stationOpenedPaths.size() == opened);
    for (unsigned i = 0; i < 5; ++i)
        assert(stationOpenedPaths[filesBeforeShip + i] == shipFiles[i]);
    const unsigned shipResources[] = {17000, 18700, 17900, 17100, 17101};
    for (unsigned id : shipResources) {
        auto *resource = static_cast<AbyssEngine::Resource *>(canvas.FindResource(id));
        assert(resource && resource->handle >= int(meshesBeforeShip));
    }
    std::printf("Pan ship group -> five original AEM meshes: OK\\n");
'''
    gate = replace_once(gate, marker, check + marker)
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    runner.prepare = prepare
    runner.main()
