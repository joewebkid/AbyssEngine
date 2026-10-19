"""Cross the first real station texture upload using the existing sun gate."""
import run_starsystem_sun_gate as sun
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    sun.prepare_sun(out, cxx, root, reuse)
    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + sun.station.extract("src/game/mission/Status.cpp", ["uint32_t Status::orbitHasPlanetRing(int index)"])
    (out / "station_binary_chain.inc").write_text(chain)
    services = (out / "sun_station_services.inc").read_text()
    services = replace_once(services,
        '        std::puts("Sun upload returned and published managed index 1; next boundary: station TextureCreate(10014); Level stage 0 incomplete");\n        throw FlareImageBoundary{};',
        '        return; // Continue into the intact station TextureCreate body.')
    services += '\n#include "station_texture_services.inc"\n'
    marker = 'static void stationTextureBoundary(unsigned short id, unsigned &out, bool flag) {'
    services = replace_once(services, marker, '''static void firstStationTextureReturned();
''' + marker + '''
    if (id == 10021) {
        assert(!flag && &out == starTextures->data() + 2 && out == 0);
        firstStationTextureReturned();
    }''')
    (out / "sun_station_services.inc").write_text(services)
    traps = (out / "station_binary_traps.inc").read_text()
    (out / "station_binary_traps.inc").write_text(replace_once(traps, 'TRAP(_ZN6Status18orbitHasPlanetRingEi)', ''))
    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, 'namespace AbyssEngine {\nint sunTextureUpload(',
        'namespace AbyssEngine {\nint stationTextureUpload(Engine *, const char *, ImageCallback, void *, unsigned *, float, AELoadedTexture *, bool);\nint sunTextureUpload(')
    marker = '    if (std::strcmp(path, "data/assets/main/3d/textures/low/etc/suns/sun_002.aei") == 0)'
    host = replace_once(host, marker, '''    if (std::strcmp(path, "data/assets/main/3d/textures/low/etc/planets/planet_003_small.aei") == 0)
        return stationTextureUpload(engine, path, callback, user, out, scale, record, managed);
''' + marker)
    (out / "sun_host.inc").write_text(host)
    gles = (out / "sun_gles.inc").read_text()
    gles = replace_once(gles, 'canvas.cubeTextures.size() == 2', 'canvas.cubeTextures.size() == 3')
    gles = replace_once(gles, '    canvas.cubeTextures[1] = nullptr;', '''    canvas.cubeTextures[1] = nullptr;
    auto *station = canvas.cubeTextures[2];
    assert(station && station->byteSize == 16384 && station->valid && !station->isCube);
    glDeleteTextures(1, &station->glId);
    delete station;
    canvas.cubeTextures[2] = nullptr;''')
    (out / "sun_gles.inc").write_text(gles)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'opened == 20', 'opened == 21')
    gate = replace_once(gate, '    assert(stationOpenedPaths == expectedFiles);',
        '    expectedFiles.push_back("data/assets/main/3d/textures/low/etc/planets/planet_003_small.aei");\n    assert(stationOpenedPaths == expectedFiles);')
    gate = replace_once(gate, 'sun upload complete -> station TextureCreate(10014) boundary',
        'sun and first station texture/loop iteration complete -> TextureCreate(10021) boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    sun.station.prepare = prepare
    sun.station.main()
