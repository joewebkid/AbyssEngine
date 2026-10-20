"""Continue the original-asset Level gate to the real initLight getter."""
import run_station_all_planets_gate as first
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    planets = (ROOT / "tools/station_all_planets_services.inc").read_text()
    planets = replace_once(planets,
        '        std::puts("All five orbit planets, including two rejected slots, reached; Stage 0 incomplete");\n'
        '        throw FlareImageBoundary{};',
        '        std::puts("All five orbit planets, including two rejected slots, reached; continuing");')
    (out / "station_all_planets_services.inc").write_text(planets)
    services = (out / "sun_station_services.inc").read_text()
    services += '\n#include "station_light_getter_services.inc"\n'
    (out / "sun_station_services.inc").write_text(services)
    star = (ROOT / "src/game/world/StarSystem.cpp").read_text()
    tables = star[star.index('static const float g_StarSystem_init_sunColors'):star.index('void StarSystem::initLight() {')]
    light = extract("src/game/world/StarSystem.cpp", ["void StarSystem::initLight() {"])
    light = replace_once(light,
        '    engine = getEngine(ApplicationManager::gAppManager);\n'
        '    engine->LightSetGlobalSceneColorAmbient(',
        '    engine = getEngine(ApplicationManager::gAppManager);\n'
        '    stationLightGetterBoundary(this, engine, ambientScale);\n'
        '    engine->LightSetGlobalSceneColorAmbient(')
    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + tables + '\n' + light
    chain += '\n' + extract("src/engine/core/ApplicationManager.cpp", ["void *ApplicationManager::GetEngine() {"])
    (out / "station_binary_chain.inc").write_text(chain)
    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, 'TRAP(_ZN10StarSystem9initLightEv)', '')
    traps = replace_once(traps, 'TRAP(_ZN11AbyssEngine18ApplicationManager9GetEngineEv)', '')
    for symbol in (
        '_ZN11AbyssEngine6Engine31LightSetGlobalSceneColorAmbientEfff',
        '_ZN11AbyssEngine6Engine16LightSetRimColorEfff',
        '_ZN11AbyssEngine6Engine28LightSetMaterialColorAmbientEfff',
        '_ZN11AbyssEngine6Engine28LightSetMaterialColorDiffuseEfff',
        '_ZN11AbyssEngine6Engine29LightSetMaterialColorSpecularEfff',
        '_ZN11AbyssEngine6Engine30LightSetMaterialColorShininessEf',
        '_ZN11AbyssEngine6Engine22LightSetLightDirectionEfffj',
        '_ZN11AbyssEngine6Engine25LightSetLightColorAmbientEfffj',
        '_ZN11AbyssEngine6Engine25LightSetLightColorDiffuseEfffj',
        '_ZN11AbyssEngine6Engine26LightSetLightColorSpecularEfffj',
        '_ZN11AbyssEngine6Engine23LightSetParticleAmbientEfff',
        '_ZN11AbyssEngine11PaintCanvas9FogEnableEbNS_7FogModeE',
        '_ZN11AbyssEngine11PaintCanvas15FogSetParameterENS_7FogModeEfffj',
    ):
        traps += '\nTRAP(' + symbol + ')'
    (out / "station_binary_traps.inc").write_text(traps + '\n')
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, '    ApplicationManager manager(&engine);',
                        '    ApplicationManager manager(&engine);\n'
                        '    ApplicationManager::gAppManager = &manager;')
    gate = replace_once(gate, 'all orbit planets boundary', 'StarSystem light getter boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.first.first.first.first.sun.station.prepare = prepare
    first.first.first.first.first.sun.station.main()
