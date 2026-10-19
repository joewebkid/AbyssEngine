"""Continue five station textures, seed and first original-asset geometry."""
import run_station_texture_gate as first
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    services = (out / "sun_station_services.inc").read_text()
    services = replace_once(services, '''    if (id == 10021) {
        assert(!flag && &out == starTextures->data() + 2 && out == 0);
        firstStationTextureReturned();
    }''', '')
    services += '\n#include "station_loop_services.inc"\n'
    (out / "sun_station_services.inc").write_text(services)
    host = (out / "sun_host.inc").read_text()
    host = host.replace('stationTextureUpload(', 'loopTextureUpload(')
    host = replace_once(host,
        'std::strcmp(path, "data/assets/main/3d/textures/low/etc/planets/planet_003_small.aei") == 0',
        'std::strstr(path, "data/assets/main/3d/textures/low/etc/planets/planet_") == path')
    # Retain the old helper declaration for the inherited, now-unused probe.
    host = replace_once(host, 'namespace AbyssEngine {\nint loopTextureUpload(',
        'namespace AbyssEngine {\nint stationTextureUpload(Engine *, const char *, ImageCallback, void *, unsigned *, float, AELoadedTexture *, bool);\nint loopTextureUpload(')
    # Resource 6768 is the original plane mesh and names 65535 as its material.
    host = replace_once(host, 'id == 20003 || id == 3))',
                        'id == 20003 || id == 3 || id == 65535))')
    host = replace_once(host, 'id == 20003 || id == 3);',
                        'id == 20003 || id == 3 || id == 65535);')
    host = replace_once(host, '(!r && id == 3));',
                        '(!r && (id == 3 || id == 65535)));')
    (out / "sun_host.inc").write_text(host)
    chain = (out / "station_binary_chain.inc").read_text()
    chain = '#include "game/mission/Item.h"\n#include "game/mission/Mission.h"\n' + chain
    dtor = first.sun.station.extract("src/game/world/Station.cpp", ["Station::~Station() {"])
    dtor = replace_once(dtor, 'Station::~Station() {', 'Station::~Station() {\n    stationDestroySeen(this);')
    chain += '\n' + dtor
    marker = ('    AERandom::gRandom->setSeed(static_cast<int32_t>(\n'
              '        static_cast<uint32_t>(Globals::status->getStation()->getIndex()) * 300u));')
    chain = replace_once(chain, marker,
                         '    stationLoopBoundary(this);\n' + marker +
                         '\n    stationSeedBoundary(this);')
    chain = replace_once(chain, '        (*this->planetsArray)[i] = geom;\n',
                         '        (*this->planetsArray)[i] = geom;\n'
                         '        if (i == 0) stationGeometryBoundary(this, geom);\n')
    (out / "station_binary_chain.inc").write_text(chain)
    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, 'TRAP(_ZN7StationD1Ev)', '')
    # Parsed station records have no ships/items/agents; fail if those branches run.
    for symbol in ('_ZN6Status18getCampaignMissionEv', '_ZN6Status19getFreelanceMissionEv',
                   '_ZN7Mission8getAgentEv', '_ZN5Agent12isStoryAgentEv',
                   '_ZN5AgentD1Ev', '_ZN4ShipD1Ev', '_ZN4ItemD1Ev'):
        traps += '\nTRAP(' + symbol + ')\n'
    (out / "station_binary_traps.inc").write_text(traps)
    # Extend the bounded low-resource registration with its exact production block.
    low = (first.sun.station.ROOT / "src/engine/render/LowResourceTable.cpp").read_text()
    pos = low.index('res->id = 0x2748;')
    block = low[low.rfind('    {', 0, pos):low.index('    }', pos) + 5]
    resource = (out / "level_resource_under_test.inc").read_text()
    old = function(resource, 'void registerResources(PaintCanvas *canvas) {')
    # prepare(reuse=True) can retain this generated file from an earlier run.
    if 'res->id = 0x2748;' not in old:
        end = old.rfind('}')
        resource = replace_once(resource, old, old[:end] + '\n' + block + '\n' + old[end:])
    (out / "level_resource_under_test.inc").write_text(resource)
    gles = (out / "sun_gles.inc").read_text()
    gles = replace_once(gles, 'canvas.cubeTextures.size() == 3', 'canvas.cubeTextures.size() == 7')
    gles = replace_once(gles, '    canvas.cubeTextures[2] = nullptr;', '''    canvas.cubeTextures[2] = nullptr;
    for (unsigned i = 3; i < 7; ++i) {
        auto *texture = canvas.cubeTextures[i];
        assert(texture && texture->valid && !texture->isCube);
        glDeleteTextures(1, &texture->glId); delete texture; canvas.cubeTextures[i] = nullptr;
    }''')
    (out / "sun_gles.inc").write_text(gles)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'opened == 21', 'opened == 26')
    gate = replace_once(gate, 'canvas.resources.size() == 2724', 'canvas.resources.size() == 2725')
    paths = ['planet_010_small', 'planet_014_big', 'planet_013_small', 'planet_017_small']
    additions = ''.join(f'    expectedFiles.push_back("data/assets/main/3d/textures/low/etc/planets/{name}.aei");\n' for name in paths)
    additions += '    expectedFiles.push_back("data/assets/main/3d/meshes/misc/plane.aem");\n'
    gate = replace_once(gate, '    assert(stationOpenedPaths == expectedFiles);', additions + '    assert(stationOpenedPaths == expectedFiles);')
    gate = replace_once(gate, 'canvas.meshCount == 17 && canvas.transformCount == 15',
                        'canvas.meshCount == 18 && canvas.transformCount == 16')
    gate = replace_once(gate, '20004, 20004,\n', '20004, 20004, 65535,\n')
    gate = replace_once(gate, 'sun and first station texture/loop iteration complete -> TextureCreate(10021) boundary',
        'sun and five station textures/loop iterations/cleanup/arrays complete -> first geometry boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.sun.station.prepare = prepare
    first.sun.station.main()
