"""Cross the second Level::init call into the production asteroid prelude."""

import run_station_binary_asset_gate as runner
import run_level_stage0_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct PlayerCreatedBoundary {};",
                        "struct PlayerCreatedBoundary {};\n"
                        "struct AsteroidPreludeBoundary {};")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createAsteroids() {"])
    level += "\n" + production[:production.index("    Waypoint *wp =")]
    level += "    throw AsteroidPreludeBoundary{};\n}\n"
    (out / "level_init_under_test.inc").write_text(level)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN5Level15createAsteroidsEv)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += """
Galaxy *Galaxy::gGalaxy;
Galaxy::Galaxy() : visited(nullptr), systems(nullptr) {}
Galaxy::~Galaxy() {}
static int *asteroidProbabilityBuffer;
void *Galaxy::getAsteroidProbabilities(Station *station) {
    assert(station == Status::gStatus->getStation());
    asteroidProbabilityBuffer = new int[22]{};
    asteroidProbabilityBuffer[0] = 0x9a;
    asteroidProbabilityBuffer[1] = 100;
    return asteroidProbabilityBuffer;
}
"""
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        "    std::printf(\"Level::init stage 0 returned after complete Pan player creation: OK\\n\");",
        "    Galaxy galaxyFixture;\n"
        "    Galaxy::gGalaxy = &galaxyFixture;\n"
        "    AERandom expectedAsteroids(status->getStation()->getIndex());\n"
        "    int expectedCount = expectedAsteroids.nextInt(40) + 40;\n"
        "    int expectedX = expectedAsteroids.nextInt(100000) - 50000;\n"
        "    int expectedY = expectedAsteroids.nextInt(100000) - 50000;\n"
        "    int expectedZ = expectedAsteroids.nextInt(100000) + 20000;\n"
        "    Globals::galaxy = &galaxyFixture;\n"
        "    bool reachedAsteroidPrelude = false;\n"
        "    try { (void) level->init(); }\n"
        "    catch (const AsteroidPreludeBoundary &) { reachedAsteroidPrelude = true; }\n"
        "    assert(reachedAsteroidPrelude && level->field_134 == 1);\n"
        "    assert(level->asteroids && level->asteroids->size() == (unsigned) expectedCount);\n"
        "    assert(level->field_c8 == expectedX && level->field_cc == expectedY\n"
        "           && level->field_d0 == expectedZ);\n"
        "    assert(opened == 38 && opened == closed);\n"
        "    assert(asteroidProbabilityBuffer);\n"
        "    delete[] asteroidProbabilityBuffer;\n"
        "    std::printf(\"Level::init stage 0 returned and stage 1 entered "
        "asteroid prelude: OK\\n\");")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
