"""Run Pan's second Level::init call through the full asteroid field."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_asteroid_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct AsteroidFirstObjectBoundary {};",
                        "struct AsteroidFieldBoundary {};")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createAsteroids() {"])
    end = production.index("    }\n\n    if (prob != 0)",
                           production.index("        PlayerAsteroid *a ="))
    level = replace_once(level,
        "        observedAsteroid = a;\n"
        "        throw AsteroidFirstObjectBoundary{};\n"
        "    }\n}\n",
        "        if (i == 0) observedAsteroid = a;\n" + production[end:])
    level = replace_once(level,
        "        observedAsteroidGeometry = geo;\n"
        "        observedAsteroidPosition = pos;\n"
        "        observedAsteroidRadius = radius;\n"
        "        observedAsteroidScale = scale;\n"
        "        observedAsteroidQuality = quality;",
        "        if (i == 0) {\n"
        "            observedAsteroidGeometry = geo;\n"
        "            observedAsteroidPosition = pos;\n"
        "            observedAsteroidRadius = radius;\n"
        "            observedAsteroidScale = scale;\n"
        "            observedAsteroidQuality = quality;\n"
        "        }")
    # The next service is deliberately a boundary, not a gas-cloud simulation.
    level += "\nvoid Level::createGasClouds() { throw AsteroidFieldBoundary{}; }\n"
    (out / "level_init_under_test.inc").write_text(level)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN5Level15createGasCloudsEv)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "catch (const AsteroidFirstObjectBoundary &)",
                        "catch (const AsteroidFieldBoundary &)")
    gate = replace_once(gate, "    delete[] asteroidProbabilityBuffer;\n", "")
    gate = replace_once(gate,
        "    assert(level->lodManager->objects->size() == 3);",
        "    assert(level->lodManager->objects->size() == (unsigned) expectedCount + 2);\n"
        "    int nearCount = 0;\n"
        "    for (int i = 0; i < expectedCount; ++i) {\n"
        "        auto *asteroid = static_cast<PlayerAsteroid *>((*level->asteroids)[i]);\n"
        "        assert(asteroid && asteroid->level == level\n"
        "               && asteroid->asteroidIndex == 0x9a\n"
        "               && asteroid->geometry && asteroid->geometry->mesh == 0x4204\n"
        "               && asteroid->geometry->lodCount == 3\n"
        "               && (*level->lodManager->objects)[i + 2] == asteroid->geometry\n"
        "               && asteroid->player && asteroid->player->kiPlayer == asteroid\n"
        "               && asteroid->player->hitpoints == asteroid->player->maxHitpoints\n"
        "               && asteroid->minable && asteroid->rotationEnabled\n"
        "               && asteroid->quality >= 4 && asteroid->quality <= 7\n"
        "               && asteroid->explosion && asteroid->explosion->type == 2);\n"
        "        bool isNear = asteroid->scaling >= 1.0f;\n"
        "        if (isNear) assert(i == nearCount++);\n"
        "        int radiusRoll = (int) (asteroid->scaling * 100.0f + 0.5f);\n"
        "        assert(radiusRoll >= (isNear ? 120 : 30)\n"
        "               && radiusRoll <= (isNear ? 219 : 99));\n"
        "        float half = isNear ? 30000.0f : 50000.0f;\n"
        "        Vector pos = asteroid->getPosition();\n"
        "        assert(pos.x >= expectedX - half && pos.x < expectedX + half\n"
        "               && pos.y >= expectedY - half && pos.y < expectedY + half\n"
        "               && pos.z >= expectedZ - half && pos.z < expectedZ + half);\n"
        "        assert(asteroid->geometry->lodDistancesSq[0] ==\n"
        "               (isNear ? 60000ULL * 60000 : 40000ULL * 40000));\n"
        "    }\n"
        "    assert(nearCount >= 2 && nearCount <= 9);")
    gate = replace_once(gate,
        "stage 1 built first PlayerAsteroid: OK",
        "stage 1 completed asteroid field and reached gas-cloud boundary: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
