"""Continue the original-asset Level gate through the first non-selected planet."""
import run_station_loop_asset_gate as first
from run_level_station_asset_gate import replace_once
from run_level_init_asset_gate import ROOT


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    loop = (ROOT / "tools/station_loop_services.inc").read_text()
    loop = replace_once(loop,
        '    std::puts("First sun rotation, -20000 forward placement and light direction reached; Stage 0 incomplete");\n'
        '    throw FlareImageBoundary{};',
        '    std::puts("First sun rotation, -20000 forward placement and light direction reached; continuing");')
    (out / "station_loop_services.inc").write_text(loop)
    services = (out / "sun_station_services.inc").read_text()
    services += '\n#include "station_first_planet_services.inc"\n'
    (out / "sun_station_services.inc").write_text(services)
    chain = (out / "station_binary_chain.inc").read_text()
    chain = replace_once(chain,
        '        if (i == 0) stationSunPoseBoundary(this);\n',
        '        if (i == 0) stationSunPoseBoundary(this);\n'
        '        if (i == 1) stationFirstPlanetBoundary(this);\n')
    (out / "station_binary_chain.inc").write_text(chain)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'canvas.meshCount == 18 && canvas.transformCount == 17',
                        'canvas.meshCount == 18 && canvas.transformCount == 18')
    gate = replace_once(gate, 'first sun pose boundary', 'first non-selected planet boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.first.sun.station.prepare = prepare
    first.first.sun.station.main()
