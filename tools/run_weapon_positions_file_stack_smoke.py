"""Compile production core loaders and station state with a host file boundary."""
import argparse
import os
from pathlib import Path
import subprocess

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--asset-dir", type=Path, required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--cxx", required=True)
    args = parser.parse_args()
    asset_dir = args.asset_dir.resolve()
    for name in ("weapons_hd.bin", "items.bin", "ships.bin", "stations.bin",
                 "systems.bin", "agents.bin", "wanted.bin"):
        if not (asset_dir / name).is_file():
            parser.error(f"--asset-dir must contain {name}")
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    compiler_env = os.environ.copy()
    compiler_env.update({"TMP": str(out), "TEMP": str(out), "TMPDIR": str(out)})
    # Isolate the unchanged production loaders so unrelated FileRead parsers
    # do not drag the whole unfinished game into this host link.
    source = (ROOT / "src/engine/file/FileRead.cpp").read_text()
    begin = source.index("Array<Array<Vector *> *> *FileRead::loadWeaponPositions(")
    end = source.index("Array<SpacePoint *> *FileRead::loadSpacePoints(", begin)
    extracted = out / "weapon_loader_production.cpp"
    items_begin = source.index("Array<Item *> *FileRead::loadItemsBinary(")
    ships_begin = source.index("Array<Ship *> *FileRead::loadShipsBinary(", items_begin)
    station_begin = source.index("Array<Station *> *FileRead::loadStationsBinary(int16_t *ids,")
    station_end = source.index("Array<String *> *FileRead::loadNamesBinary(", station_begin)
    system_stations_begin = source.index("Array<Station *> *FileRead::loadStationsBinary(SolarSystem *system)")
    system_stations_end = source.index("Array<Agent *> *FileRead::loadAgents()", system_stations_begin)
    load_one_begin = source.index("Station *FileRead::loadStation(int32_t id)")
    load_one_end = source.index("int32_t FileRead::loadStationsBinary()", load_one_begin)
    item_source = (ROOT / "src/game/mission/Item.cpp").read_text()
    ship_source = (ROOT / "src/game/ship/Ship.cpp").read_text()
    station_source = (ROOT / "src/game/world/Station.cpp").read_text()
    system_source = (ROOT / "src/game/world/SolarSystem.cpp").read_text()
    agent_source = (ROOT / "src/game/ship/Agent.cpp").read_text()
    wanted_source = (ROOT / "src/game/world/Wanted.cpp").read_text()
    status_source = (ROOT / "src/game/mission/Status.cpp").read_text()
    galaxy_source = (ROOT / "src/game/world/Galaxy.cpp").read_text()
    systems_begin = source.index("Array<SolarSystem *> *FileRead::loadSystemsBinary()")
    systems_end = source.index("Array<Wanted *> *FileRead::loadWanted()", systems_begin)
    agents_begin = source.index("Array<Agent *> *FileRead::loadAgents()")
    agents_end = source.index("Array<int32_t> *FileRead::loadWreckCollision(", agents_begin)
    wanted_begin = source.index("Array<Wanted *> *FileRead::loadWanted()")
    wanted_end = source.index("Array<NewsItem *> *FileRead::loadTicker()", wanted_begin)
    extracted.write_text(
        '#include "engine/file/FileRead.h"\n'
        '#include "game/mission/Item.h"\n#include "game/ship/Ship.h"\n'
        '#include "game/core/Globals.h"\n#include "game/mission/Status.h"\n'
        '#include "game/world/Standing.h"\n'
        '#include "game/world/Station.h"\n'
        '#include "game/world/SolarSystem.h"\n'
        '#include "game/ship/Agent.h"\n'
        '#include "game/world/Wanted.h"\n'
        '#include "game/world/Galaxy.h"\n'
        'using AbyssEngine::Vector;\n'
        'FileRead::FileRead() {}\nFileRead::~FileRead() {}\n'
        + source[begin:end]
        + source[items_begin:ships_begin]
        + source[ships_begin:]
        + source[station_begin:station_end]
        + source[system_stations_begin:system_stations_end]
        + source[load_one_begin:load_one_end]
        + source[systems_begin:systems_end]
        + source[agents_begin:agents_end]
        + source[wanted_begin:wanted_end]
        + function(item_source, "Item::~Item()")
        + function(item_source, "void Item::init()")
        + function(item_source, "Item::Item(IntArray *ingredients_")
        + function(item_source, "int Item::getAttribute(")
        + function(item_source, "int Item::getSort()")
        + function(item_source, "int Item::getIndex()")
        + function(item_source, "int Item::getTotalPrice()")
        + function(ship_source, "Ship::Ship(int index,")
        + function(ship_source, "Ship::~Ship()")
        + function(ship_source, "int Ship::getCargoValue()")
        + function(ship_source, "void Ship::refreshValue()")
        + function(station_source, "Station::Station(String name,")
        + function(system_source, "SolarSystem::SolarSystem(int unk0,")
        + function(system_source, "SolarSystem::~SolarSystem()")
        + function(system_source, "int SolarSystem::stationIsInSystem(int idx)")
        + function(system_source, "uint32_t *SolarSystem::getStations()")
        + function(agent_source, "Agent::Agent(int kind,")
        + function(agent_source, "Agent::~Agent() noexcept(false)")
        + function(agent_source, "void Agent::setImageParts(int *parts)")
        + function(wanted_source, "Wanted::Wanted(int index,")
        + function(wanted_source, "Wanted::~Wanted()")
        + function(wanted_source, "void Wanted::setImageParts(int *parts)")
        + function(status_source, "Status::Status()")
        + function(status_source, "Status::~Status()")
        + function(status_source, "void Status::setStation(Station *s)")
        + galaxy_source[galaxy_source.index("static const int kStationCount"):
                        galaxy_source.index("Galaxy::Galaxy()")]
        + function(galaxy_source, "Galaxy::Galaxy()")
        + function(galaxy_source, "Galaxy::~Galaxy()")
        + function(galaxy_source, "SolarSystem *Galaxy::getSystem(int index)")
        + function(galaxy_source, "Station *Galaxy::getStation(int index)")
        + function(station_source, "Station::Station()")
        + function(station_source, "int Station::getIndex()")
        + function(station_source, "int Station::getSystem()")
        + function(station_source, "int Station::getTextureIndex()")
        + function(station_source, "String Station::getName()")
    )
    sources = [
        "tools/weapon_positions_file_stack_smoke.cpp",
        "host/ReadOnlyAssetFileInterface.cpp",
        "src/engine/file/AEFile.cpp",
        "src/engine/file/AENormalFile.cpp",
        "src/engine/file/AEPakFile.cpp",
        str(extracted),
        "src/game/core/String.cpp",
    ]
    for opt in ("-O0", "-O2"):
        exe = out / f"weapon_file_stack_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=c++17", opt, "-fpermissive", "-ffunction-sections",
            "-fdata-sections", "-Wl,--gc-sections", "-I.", "-Isrc", "-Ithird_party/jni",
            "-Ithird_party/gl", "-Ithird_party/fmod/inc", *sources,
            "-o", str(exe),
        ], cwd=ROOT, env=compiler_env, check=True)
        subprocess.run([str(exe), str(asset_dir)], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
