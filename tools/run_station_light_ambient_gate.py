"""Continue the original-asset Level gate through the real ambient light setter."""
import run_station_light_getter_gate as first
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    getter = (ROOT / "tools/station_light_getter_services.inc").read_text()
    getter = replace_once(getter,
        '    std::puts("All planets complete; real StarSystem::initLight reached its GetEngine result");\n'
        '    throw FlareImageBoundary{};',
        '    std::puts("All planets complete; continuing through scene ambient");')
    (out / "station_light_getter_services.inc").write_text(getter)
    services = (out / "sun_station_services.inc").read_text()
    services += '\n#include "station_light_ambient_services.inc"\n'
    (out / "sun_station_services.inc").write_text(services)
    chain = (out / "station_binary_chain.inc").read_text()
    chain = replace_once(chain,
        '    engine = getEngine(ApplicationManager::gAppManager);\n'
        '    engine->LightSetRimColor(',
        '    stationLightAmbientBoundary(this, engine);\n'
        '    engine = getEngine(ApplicationManager::gAppManager);\n'
        '    engine->LightSetRimColor(')
    chain = 'static int g_Engine_useShaders;\n' + chain
    chain += '\n' + extract("src/engine/render/Engine.cpp", [
        "void Engine::LightSetGlobalSceneColorAmbient("])
    (out / "station_binary_chain.inc").write_text(chain)
    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps,
        'TRAP(_ZN11AbyssEngine6Engine31LightSetGlobalSceneColorAmbientEfff)', '')
    (out / "station_binary_traps.inc").write_text(traps)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'StarSystem light getter boundary',
                        'StarSystem real scene ambient boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.first.first.first.first.first.sun.station.prepare = prepare
    first.first.first.first.first.first.sun.station.main()
