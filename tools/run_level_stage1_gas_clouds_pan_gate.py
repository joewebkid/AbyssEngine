"""Run Pan's second Level::init call through production gas-cloud creation."""

import run_station_binary_asset_gate as runner
import run_level_stage1_asteroid_field_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct AsteroidFieldBoundary {};",
                        '#include "game/ship/PlayerGasCloud.h"\n'
                        "struct GasCloudCompleteBoundary {};")
    assert host.count("id == 20051") == 2
    host = host.replace("id == 20051", "id == 20051 || id == 20111 || id == 29045")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        "void Level::createGasClouds() { throw AsteroidFieldBoundary{}; }",
        extract("src/game/world/Level.cpp", ["void Level::createGasClouds() {"]))
    level = replace_once(level, "        thisptr->createGasClouds();",
                         "        thisptr->createGasClouds();\n"
                         "        throw GasCloudCompleteBoundary{};")
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += """
static int *plasmaProbabilityBuffer;
void *Galaxy::getPlasmaProbabilities(Station *station) {
    assert(station == Status::gStatus->getStation());
    plasmaProbabilityBuffer = new int[8]{0xc9, 100};
    return plasmaProbabilityBuffer;
}
"""
    chain += "\n" + extract("src/game/ship/PlayerGasCloud.cpp", [
        "PlayerGasCloud::PlayerGasCloud(int itemId, ParticleSystemManager *",
        "PlayerGasCloud::~PlayerGasCloud() {",
        "void PlayerGasCloud::setPosition(const Vector &position) {",
        "Vector PlayerGasCloud::getPosition() {",
    ])
    chain += "\n" + extract("src/game/world/SolarSystem.cpp", [
        "uint32_t *SolarSystem::getRoutes() {",
    ])
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps += "\n" + "\n".join("TRAP(" + symbol + ")" for symbol in (
        "_ZN14PlayerGasCloud9translateERKN11AbyssEngine6AEMath6VectorE",
        "_ZN14PlayerGasCloud6renderEv",
        "_ZN14PlayerGasCloud6updateEi",
    )) + "\n"
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "    bool reachedAsteroidPrelude = false;",
        "    auto *plasmaEquipment = new Item(nullptr, nullptr, nullptr);\n"
        "    plasmaEquipment->sort = 0x21;\n"
        "    (*status->ship->equipment)[3] = plasmaEquipment;\n"
        "    bool reachedAsteroidPrelude = false;")
    gate = replace_once(gate, "catch (const AsteroidFieldBoundary &)",
                        "catch (const GasCloudCompleteBoundary &)")
    gate = replace_once(gate,
        "    assert(level->asteroids && level->asteroids->size() == (unsigned) expectedCount);",
        "    assert(plasmaProbabilityBuffer && level->gasClouds\n"
        "           && level->gasClouds->size() >= 4 && level->gasClouds->size() <= 7);\n"
        "    delete[] plasmaProbabilityBuffer;\n"
        "    for (unsigned i = 0; i < level->gasClouds->size(); ++i) {\n"
        "        auto *cloud = static_cast<PlayerGasCloud *>((*level->gasClouds)[i]);\n"
        "        assert(cloud && cloud->level == level && cloud->itemId == 0xc9\n"
        "               && cloud->geometry && cloud->geometry->mesh == 0x37d1\n"
        "               && cloud->modelGeometry && cloud->modelGeometry->mesh == 0x4a35\n"
        "               && cloud->sparkMeshId == 0x4a39\n"
        "               && cloud->player && cloud->player->kiPlayer == cloud\n"
        "               && cloud->player->maxHitpoints == 1\n"
        "               && cloud->visibleFlag && cloud->hasCargo);\n"
        "        Vector pos = cloud->getPosition();\n"
        "        Vector fromCenter = pos - Vector{level->field_c8, level->field_cc, level->field_d0};\n"
        "        assert(AbyssEngine::AEMath::VectorLength(pos) > 25000.0f\n"
        "               && AbyssEngine::AEMath::VectorLength(fromCenter) > 30000.0f);\n"
        "    }\n"
        "    auto *cloudsBeforeNoEquipment = level->gasClouds;\n"
        "    (*status->ship->equipment)[3] = nullptr;\n"
        "    level->createGasClouds();\n"
        "    assert(level->gasClouds == cloudsBeforeNoEquipment && plasmaProbabilityBuffer);\n"
        "    delete[] plasmaProbabilityBuffer;\n"
        "    (*status->ship->equipment)[3] = plasmaEquipment;\n"
        "    assert(level->asteroids && level->asteroids->size() == (unsigned) expectedCount);")
    gate = replace_once(gate,
        "stage 1 completed asteroid field and reached gas-cloud boundary: OK",
        "stage 1 completed equipped gas clouds before mission dispatch: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
