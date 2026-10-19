"""Extend the original station gate through actual ETC1 sun upload/readback.

Reuse its host/Android build, transport, catalog tests and bounded bootstrap.
Only generated fixture boundaries change; production bodies stay intact.
"""
import run_station_binary_asset_gate as station
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function

prepare_station = station.prepare


def prepare_sun(out, cxx, root, reuse):
    prepare_station(out, cxx, root, reuse)
    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + station.extract("src/game/world/Station.cpp", ["int Station::getTextureIndex()"])
    (out / "station_binary_chain.inc").write_text(chain)
    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN7Station15getTextureIndexEv)", "")
    (out / "station_binary_traps.inc").write_text(traps)
    services = (station.ROOT / "tools/station_binary_services.inc").read_text()
    old = function(services, "static void stationTextureBoundary(")
    # Keep the original parser/array assertions at the sun request, but let it
    # return normally into the intact PaintCanvas::TextureCreate body.
    start = old.index('    std::puts("Real station read')
    old_prefix = old[:start] + "}\n"
    old_prefix = old_prefix.replace('    if (id != 0x2731) return;', '''    if (id == 0x271e) {
        assert(!flag && systemTextureBoundaryCalls == 1 && sunUploads == 1);
        assert(&out == starTextures->data() + 1 && out == 0 && (*starTextures)[0] == 1);
        auto *sun = static_cast<Resource *>(Globals::Canvas->FindResource(10033));
        auto *next = static_cast<Resource *>(Globals::Canvas->FindResource(id));
        assert(sun && sun->handle == 1 && next && next->handle == -1);
        assert(cursor == 43731 && opened == closed);
        std::puts("Sun upload returned and published managed index 1; next boundary: station TextureCreate(10014); Level stage 0 incomplete");
        throw FlareImageBoundary{};
    }
    if (id != 0x2731) return;''')
    services = replace_once(services, old, old_prefix)
    services = '#include "sun_etc_readback.h"\n' + services
    (out / "sun_station_services.inc").write_text(services)

    gles = (out / "station_binary_gles.inc").read_text()
    gles += '\n#include "sun_upload_services.inc"\n'
    gles = replace_once(gles, 'canvas.cubeTextures.size() == 1 && canvas.images.size() == 3',
                       'canvas.cubeTextures.size() == 2 && canvas.images.size() == 3')
    gles = replace_once(gles, '    canvas.cubeTextures[0] = nullptr;', '''    canvas.cubeTextures[0] = nullptr;
    auto *sun = canvas.cubeTextures[1];
    assert(sun && sun->byteSize == 43704 && sun->valid && !sun->isCube);
    glDeleteTextures(1, &sun->glId);
    delete sun;
    canvas.cubeTextures[1] = nullptr;''')
    (out / "sun_gles.inc").write_text(gles)

    host = (out / "station_binary_host.inc").read_text()
    marker = '    if (std::strstr(path, "data/textures/gof2_interface") == path)'
    host = replace_once(host, marker, '''    if (std::strcmp(path, "data/assets/main/3d/textures/low/etc/suns/sun_002.aei") == 0)
        return sunTextureUpload(engine, path, callback, user, out, scale, record, managed);
''' + marker)
    declaration = 'namespace AbyssEngine {\nint flareTextureBoundary('
    host = replace_once(host, declaration, 'namespace AbyssEngine {\nint sunTextureUpload(Engine *, const char *, ImageCallback, void *, unsigned int *, float, AELoadedTexture *, bool);\nint flareTextureBoundary(')
    (out / "sun_host.inc").write_text(host)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = gate.replace('"station_binary_host.inc"', '"sun_host.inc"')
    gate = gate.replace('"station_binary_gles.inc"', '"sun_gles.inc"')
    gate = gate.replace('"station_binary_services.inc"', '"sun_station_services.inc"')
    gate = gate.replace('opened == 19', 'opened == 20')
    marker = '    assert(stationOpenedPaths == expectedFiles);'
    gate = replace_once(gate, marker, '    expectedFiles.push_back("data/assets/main/3d/textures/low/etc/suns/sun_002.aei");\n' + marker)
    gate = gate.replace('StarSystem sun TextureCreate boundary', 'sun upload complete -> station TextureCreate(10014) boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    station.prepare = prepare_sun
    station.main()
