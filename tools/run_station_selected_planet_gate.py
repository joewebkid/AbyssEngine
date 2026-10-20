"""Continue the original-asset Level gate through the selected station planet."""
import run_station_first_planet_gate as first
from run_level_station_asset_gate import replace_once
from run_level_init_asset_gate import ROOT


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    planet = (ROOT / "tools/station_first_planet_services.inc").read_text()
    planet = replace_once(planet,
        '    std::puts("First non-selected planet slot, size, pose and position reached; Stage 0 incomplete");\n'
        '    throw FlareImageBoundary{};',
        '    std::puts("First non-selected planet slot, size, pose and position reached; continuing");')
    (out / "station_first_planet_services.inc").write_text(planet)
    services = (out / "sun_station_services.inc").read_text()
    services += '\n#include "station_selected_planet_services.inc"\n'
    (out / "sun_station_services.inc").write_text(services)
    chain = (out / "station_binary_chain.inc").read_text()
    chain = replace_once(chain,
        '        if (i == 1) stationFirstPlanetBoundary(this);\n',
        '        if (i == 1) stationFirstPlanetBoundary(this);\n'
        '        if (i == 2) stationSecondPlanetBoundary(this);\n'
        '        if (i == 3) stationSelectedPlanetBoundary(this);\n')
    (out / "station_binary_chain.inc").write_text(chain)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'canvas.meshCount == 18 && canvas.transformCount == 18',
                        'canvas.meshCount == 18 && canvas.transformCount == 20')
    gate = replace_once(gate, 'first non-selected planet boundary',
                        'selected station planet boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.first.first.sun.station.prepare = prepare
    first.first.first.sun.station.main()
