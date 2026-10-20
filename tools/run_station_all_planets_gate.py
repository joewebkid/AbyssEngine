"""Continue the original-asset Level gate through all five orbit planets."""
import run_station_selected_planet_gate as first
from run_level_station_asset_gate import replace_once
from run_level_init_asset_gate import ROOT


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)
    selected = (ROOT / "tools/station_selected_planet_services.inc").read_text()
    selected = replace_once(selected,
        '    std::puts("Second orbit and selected station planet reached; Stage 0 incomplete");\n'
        '    throw FlareImageBoundary{};',
        '    std::puts("Second orbit and selected station planet reached; continuing");')
    (out / "station_selected_planet_services.inc").write_text(selected)
    services = (out / "sun_station_services.inc").read_text()
    services += '\n#include "station_all_planets_services.inc"\n'
    (out / "sun_station_services.inc").write_text(services)
    chain = (out / "station_binary_chain.inc").read_text()
    chain = replace_once(chain,
        '        if (i == 3) stationSelectedPlanetBoundary(this);\n',
        '        if (i == 3) stationSelectedPlanetBoundary(this);\n'
        '        if (i >= 4) stationRemainingPlanetBoundary(this, i);\n')
    (out / "station_binary_chain.inc").write_text(chain)
    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'canvas.meshCount == 18 && canvas.transformCount == 20',
                        'canvas.meshCount == 18 && canvas.transformCount == 22')
    gate = replace_once(gate, 'selected station planet boundary', 'all orbit planets boundary')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    first.first.first.first.sun.station.prepare = prepare
    first.first.first.first.sun.station.main()
