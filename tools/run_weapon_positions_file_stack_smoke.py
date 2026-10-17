"""Compile production loaders, station state, and routing with a host file boundary."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess

from run_particle_control_smoke import function


ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--asset-dir", type=Path, required=True)
    parser.add_argument("--names-asset-dir", type=Path)
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
    run_asset_dir = asset_dir
    if args.names_asset_dir is not None:
        names_dir = args.names_asset_dir.resolve()
        names = list(names_dir.glob("names_*.bin"))
        if len(names) < 13:
            parser.error("--names-asset-dir must contain the 13 name binaries")
        run_asset_dir = out / "asset_overlay"
        run_asset_dir.mkdir(exist_ok=True)
        for name in ("weapons_hd.bin", "items.bin", "ships.bin", "stations.bin",
                     "systems.bin", "agents.bin", "wanted.bin"):
            shutil.copyfile(asset_dir / name, run_asset_dir / name)
        for name in names:
            shutil.copyfile(name, run_asset_dir / name.name)
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
    generator_source = (ROOT / "src/game/mission/Generator.cpp").read_text()
    galaxy_source = (ROOT / "src/game/world/Galaxy.cpp").read_text()
    globals_source = (ROOT / "src/game/core/Globals.cpp").read_text()
    image_factory_source = (ROOT / "src/engine/render/ImageFactory.cpp").read_text()
    generator_begin = status_source.index("struct Generator {")
    generator_end = status_source.index("};", generator_begin) + 2
    achievements_source = (ROOT / "src/game/mission/Achievements.cpp").read_text()
    blueprint_source = (ROOT / "src/game/mission/BluePrint.cpp").read_text()
    mission_source = (ROOT / "src/game/mission/Mission.cpp").read_text()
    standing_source = (ROOT / "src/game/world/Standing.cpp").read_text()
    pending_source = (ROOT / "src/game/mission/PendingProduct.cpp").read_text()
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
        '#include "game/world/Level.h"\n#include "game/core/GameSettings.h"\n'
        '#include "game/world/SystemPathFinder.h"\n'
        '#include "game/mission/Achievements.h"\n#include "game/mission/BluePrint.h"\n'
        '#include "game/mission/Mission.h"\n#include "game/mission/PendingProduct.h"\n'
        '#include "engine/core/AERandom.h"\n'
        '#include "engine/render/ImageFactory.h"\n'
        '#include "engine/render/ImageFactoryPortraitData.h"\n'
        'using AbyssEngine::Vector;\n'
        'FileRead::FileRead() {}\nFileRead::~FileRead() {}\n'
        + status_source[generator_begin:generator_end].replace(
            "    Array<Agent *> *createAgents(Station *station);",
            "    Array<Agent *> *createAgents(Station *station);\n"
            "    Agent *createAgent(Station *station);") + '\n'
        + generator_source[generator_source.index("static const volatile int kaamoSpecialItems[9]"):
                           generator_source.index("};", generator_source.index("static const volatile int kaamoSpecialItems[9]")) + 2] + '\n'
        + generator_source[generator_source.index("static const volatile int g_Generator_kaamoAllowed[10]"):
                           generator_source.index("};", generator_source.index("static const volatile int g_Generator_kaamoAllowed[10]")) + 2] + '\n'
        + generator_source[generator_source.index("static const volatile int g_Generator_shipRaces[64]"):
                           generator_source.index("};", generator_source.index("static const volatile int g_Generator_shipRaces[64]")) + 2] + '\n'
        + 'extern int g_station_generator_calls;\n'
        + 'Generator::Generator() { ++g_station_generator_calls; }\n'
        + function(generator_source, "Array<Item *> *Generator::getItemBuyList(Station *station)")
        + 'Array<Item *> *generateRealMarketItems(Station *station) { '
          'Generator generator; return generator.getItemBuyList(station); }\n'
        + function(generator_source, "Array<Ship *> *Generator::getShipBuyList(Station *station)")
        + 'Array<Agent *> *Generator::createAgents(Station *) { ++g_station_generator_calls; return new Array<Agent *>(); }\n'
        + function(generator_source, "Agent *Generator::createAgent(Station *station)")
        + 'Agent *generateRealAgent(Station *station) { '
          'Generator generator; return generator.createAgent(station); }\n'
        + 'extern int g_wanted_random_station_id;\n'
        + 'extern int g_wanted_random_station_calls;\n'
        # shortcut: the random choice is deterministic; exercise the real RNG distribution separately.
        + 'Station *Globals::getRandomStation() { ++g_wanted_random_station_calls; '
          'if (g_wanted_random_station_id < 0) std::abort(); '
          'return Galaxy::gGalaxy->getStation(g_wanted_random_station_id); }\n'
        + source[begin:end]
        + source[items_begin:ships_begin]
        + source[ships_begin:]
        + source[station_begin:station_end]
        + function(source, "Array<String *> *FileRead::loadNamesBinary(")
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
        + function(item_source, "int Item::getAmount()")
        + function(item_source, "int Item::getSinglePrice()")
        + function(item_source, "int Item::getMaxPrice()")
        + function(item_source, "IntArray *Item::getIngredients()")
        + function(item_source, "IntArray *Item::getQuantities()")
        + function(item_source, "int Item::getType()")
        + function(item_source, "int Item::getTecLevel()")
        + function(item_source, "int Item::getOccurence()")
        + function(item_source, "int Item::getMinPriceSystem()")
        + function(item_source, "bool Item::isWeapon()")
        + function(item_source, "void Item::setStationAmount(int value)")
        + function(item_source, "Item *Item::clone()")
        + function(item_source, "Item *Item::makeItem()")
        + function(item_source, "Item *Item::makeItem(int amount)")
        + function(item_source, "Item *Item::makeItem(int amount, int price)")
        + function(ship_source, "Ship::Ship(int index,")
        + function(ship_source, "Ship::~Ship()")
        + function(ship_source, "int Ship::getCargoValue()")
        + function(ship_source, "void Ship::refreshValue()")
        + function(ship_source, "void Ship::setCargo(Array<Item *> *cargo)")
        + function(ship_source, "void Ship::setEquipment(Item *item, int slot)")
        + function(ship_source, "void Ship::addMod(int mod)")
        + function(ship_source, "Ship *Ship::clone()")
        + function(ship_source, "Ship *Ship::makeShip(int price)")
        + ship_source[ship_source.index("const int shipRaces[64]"):
                      ship_source.index("};", ship_source.index("const int shipRaces[64]")) + 2] + '\n'
        + function(ship_source, "void Ship::adjustPrice()")
        + function(ship_source, "unsigned int Ship::hasJumpDriveIntegrated()")
        + function(ship_source, "int Ship::getIndex()")
        + function(ship_source, "int Ship::getPrice()")
        + function(ship_source, "void Ship::setRace(int race)")
        + function(ship_source, "void Ship::priceDecline()")
        + function(ship_source, "int Ship::getMaxHP()")
        + function(ship_source, "int Ship::getMaxShieldHP()")
        + function(ship_source, "int Ship::getMaxArmorHP()")
        + function(ship_source, "Array<Item *> *Ship::getCargo()")
        + function(ship_source, "bool Ship::hasCargo(int index, int amount)")
        + function(station_source, "Station::Station(String name,")
        + function(station_source, "bool Station::equals(Station *other)")
        + function(station_source, "Array<Item *> *Station::getItems()")
        + function(station_source, "Array<Ship *> *Station::getShips()")
        + function(station_source, "void Station::setItems(Array<Item *> *items, bool deep)")
        + function(station_source, "void Station::setShips(Array<Ship *> *ships, bool deep)")
        + function(station_source, "void Station::setAgents(Array<Agent *> *agents)")
        + function(system_source, "SolarSystem::SolarSystem(int unk0,")
        + function(system_source, "SolarSystem::~SolarSystem()")
        + function(system_source, "int SolarSystem::stationIsInSystem(int idx)")
        + function(system_source, "uint32_t *SolarSystem::getStations()")
        + function(system_source, "uint32_t *SolarSystem::getRoutes()")
        + function(system_source, "int SolarSystem::getWarpGateIndex()")
        + function(system_source, "int SolarSystem::getIndex()")
        + function(system_source, "int SolarSystem::getRace()")
        + function(system_source, "int SolarSystem::getX()")
        + function(system_source, "int SolarSystem::getY()")
        + function(system_source, "uint8_t SolarSystem::isVisible()")
        + function(agent_source, "Agent::Agent(int kind,")
        + function(agent_source, "Agent::~Agent() noexcept(false)")
        + function(agent_source, "void Agent::setImageParts(int *parts)")
        + function(agent_source, "int Agent::getStation()")
        + function(agent_source, "int Agent::getSystem()")
        + function(agent_source, "int Agent::getRace()")
        + function(agent_source, "int Agent::getCosts()")
        + function(agent_source, "void Agent::setCosts(int costs)")
        + function(agent_source, "int Agent::getOffer()")
        + function(agent_source, "void Agent::setOffer(int offer)")
        + function(agent_source, "void Agent::setWingmanFriendNames(")
        + function(agent_source, "Triple *Agent::setSellItemData(")
        + function(wanted_source, "Wanted::Wanted(int index,")
        + function(wanted_source, "Wanted::~Wanted()")
        + function(wanted_source, "void Wanted::setImageParts(int *parts)")
        + function(wanted_source, "uint8_t Wanted::isActive()")
        + function(wanted_source, "uint8_t Wanted::isTerminated()")
        + function(wanted_source, "void Wanted::setActive(bool v)")
        + function(wanted_source, "int Wanted::getCurrentLocation()")
        + function(wanted_source, "int Wanted::getTravelsTo()")
        + function(wanted_source, "void Wanted::setCurrentLocation(int v)")
        + function(wanted_source, "void Wanted::setTravelsTo(int v)")
        + function(wanted_source, "void Wanted::setLastSeen(int v)")
        + function(status_source, "Status::Status()")
        + function(status_source, "Status::~Status()")
        + function(status_source, "Standing *Status::getStanding()")
        + function(status_source, "void Status::setShip(Ship *s)")
        + 'static int *g_campaignSentinel = nullptr;\n'
        + function(status_source, "void Status::setCampaignMission(Mission *m)")
        + function(status_source, "void Status::loadAgents()")
        + function(status_source, "void Status::loadWanted()")
        + function(status_source, "void Status::resetGame()")
        + function(status_source, "void Status::setStation(Station *s)")
        + function(status_source, "Station *Status::isOnStack(Station *s)")
        + function(status_source, "bool Status::inAlienOrbit()")
        + function(status_source, "int Status::getCurrentCampaignMission()")
        + function(status_source, "bool Status::dlc1Won()")
        + function(status_source, "SolarSystem *Status::getSystem()")
        + function(status_source, "Station *Status::getStation()")
        + function(status_source, "int Status::inSupernovaSystem()")
        + function(status_source, "bool Status::inBlackMarketSystem()")
        + function(status_source, "bool Status::hardCoreMode()")
        + function(status_source, "Array<bool> *Status::getSystemVisibilities()")
        + function(status_source, "int Status::addStationToStack(Station *s)")
        + 'static int g_dsTypeMask = 0x8B782B;\n'
        + function(status_source, "void Status::departStation(Station *dest)")
        + function(status_source, "void Status::moveWanted()")
        + galaxy_source[galaxy_source.index("static const int kStationCount"):
                        galaxy_source.index("Galaxy::Galaxy()")]
        + function(galaxy_source, "Galaxy::Galaxy()")
        + function(galaxy_source, "Galaxy::~Galaxy()")
        + function(galaxy_source, "void Galaxy::reset()")
        + function(galaxy_source, "int Galaxy::distancePercent(int x1, int y1, int x2, int y2)")
        + function(galaxy_source, "int Galaxy::invDistancePercent(int x1, int y1, int x2, int y2)")
        + function(galaxy_source, "SolarSystem *Galaxy::getSystem(int index)")
        + function(galaxy_source, "Station *Galaxy::getStation(int index)")
        + function(galaxy_source, "Array<SolarSystem *> *Galaxy::getSystems()")
        + function(globals_source, "float Globals::sqrt(float x)")
        + globals_source[globals_source.index("static const int gREF_table[37]"):
                          globals_source.index("};", globals_source.index("static const int gREF_table[37]")) + 2] + '\n'
        + function(globals_source, "unsigned Globals::getRandomEnemyFighter(int kind)")
        + function(globals_source, "String Globals::getRandomName(int kind, bool both)")
        + function(image_factory_source, "int *ImageFactory::createChar(bool isMale, int race)")
        + function(achievements_source, "Achievements::Achievements()")
        + function(achievements_source, "Achievements::~Achievements()")
        + function(achievements_source, "uint8_t Achievements::gotAllGoldMedals()")
        + function(achievements_source, "uint8_t Achievements::gotAllSupernovaMedals()")
        + function(achievements_source, "int Achievements::init()")
        + function(blueprint_source, "BluePrint::BluePrint(int item)")
        + function(blueprint_source, "BluePrint::~BluePrint()")
        + function(blueprint_source, "int BluePrint::getIndex()")
        + function(blueprint_source, "Array<int> *BluePrint::getIngredientList()")
        + function(blueprint_source, "Array<int> *BluePrint::getQuantityList()")
        + function(blueprint_source, "int BluePrint::getIngredientsValue()")
        + function(blueprint_source, "int BluePrint::getAutoCompletionPrice()")
        + function(mission_source, "Mission::Mission(int id, int goods, int station)")
        + function(mission_source, "Mission::Mission()")
        + function(mission_source, "Mission::~Mission()")
        + function(mission_source, "void Mission::setCampaignMission(bool flag)")
        + function(mission_source, "int Mission::getType()")
        + function(mission_source, "bool Mission::isCampaignMission()")
        + function(mission_source, "bool Mission::isEmpty()")
        + function(mission_source, "int Mission::getTargetStation()")
        + function(mission_source, "int Mission::getProductionGoodIndex()")
        + function(standing_source, "Standing::Standing()")
        + function(standing_source, "Standing::~Standing()")
        + function(standing_source, "void Standing::setPlayerSignatureRace(int race)")
        + function(pending_source, "PendingProduct::~PendingProduct()")
        + function(station_source, "Station::Station()")
        + function(station_source, "int Station::getIndex()")
        + function(station_source, "int Station::getSystem()")
        + function(station_source, "int Station::getTecLevel()")
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
        "src/engine/core/AERandom.cpp",
        "src/engine/core/Node.cpp",
        "src/game/world/SystemPathFinder.cpp",
    ]
    for opt in ("-O0", "-O2"):
        exe = out / f"weapon_file_stack_{opt[1:]}.exe"
        subprocess.run([
            args.cxx, "-std=c++17", opt, "-fpermissive", "-ffunction-sections",
            "-fdata-sections", "-Wl,--gc-sections", "-I.", "-Isrc", "-Ithird_party/jni",
            "-Ithird_party/gl", "-Ithird_party/fmod/inc", *sources,
            "-o", str(exe),
        ], cwd=ROOT, env=compiler_env, check=True)
        command = [str(exe), str(run_asset_dir)]
        if args.names_asset_dir is not None:
            command.append("names")
        subprocess.run(command, cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
