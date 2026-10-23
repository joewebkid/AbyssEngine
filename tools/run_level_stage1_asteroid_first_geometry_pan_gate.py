"""Run Pan's second Level::init call through its first asteroid geometry."""

import run_station_binary_asset_gate as runner
import run_level_stage1_asteroid_prelude_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct AsteroidPreludeBoundary {};",
                        "struct AsteroidFirstGeometryBoundary {};\n"
                        "static AEGeometry *observedAsteroidGeometry;\n"
                        "static Vector observedAsteroidPosition;\n"
                        "static int observedAsteroidRadius;\n"
                        "static float observedAsteroidScale;\n"
                        "static int observedAsteroidQuality;")
    host = replace_once(host, '#include "level_init_under_test.inc"',
                        '#include "engine/math/BoundingSphere.h"\n'
                        '#include "level_init_under_test.inc"')
    assert host.count("id == 34811") == 2
    host = host.replace("id == 34811", "id == 34811 || id == 20023")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createAsteroids() {"])
    remainder = production[production.index("    Waypoint *wp ="):
                           production.index("        PlayerAsteroid *a =")]
    level = replace_once(level, "    throw AsteroidPreludeBoundary{};\n}",
        remainder +
        "        observedAsteroidGeometry = geo;\n"
        "        observedAsteroidPosition = pos;\n"
        "        observedAsteroidRadius = radius;\n"
        "        observedAsteroidScale = scale;\n"
        "        observedAsteroidQuality = quality;\n"
        "        throw AsteroidFirstGeometryBoundary{};\n"
        "    }\n}\n")
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/ship/Player.cpp", ["void Player::setActive(bool value) {"])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "catch (const AsteroidPreludeBoundary &)",
                        "catch (const AsteroidFirstGeometryBoundary &)")
    gate = replace_once(gate,
        "    assert(opened == 38 && opened == closed);\n"
        "    assert(asteroidProbabilityBuffer);",
        "    assert(asteroidProbabilityBuffer);")
    gate = replace_once(gate,
        "    delete[] asteroidProbabilityBuffer;",
        "    delete[] asteroidProbabilityBuffer;\n"
        "    assert(level->asteroidWaypoint && level->asteroidWaypoint->x == expectedX\n"
        "           && level->asteroidWaypoint->y == expectedY\n"
        "           && level->asteroidWaypoint->z == expectedZ\n"
        "           && !level->asteroidWaypoint->route);\n"
        "    auto *asteroidSphere = dynamic_cast<BoundingSphere *>(level->collisionVolume);\n"
        "    assert(asteroidSphere && asteroidSphere->centerX == expectedX\n"
        "           && asteroidSphere->centerY == expectedY\n"
        "           && asteroidSphere->centerZ == expectedZ\n"
        "           && asteroidSphere->radius == 100000.0f);\n"
        "    assert(observedAsteroidGeometry && observedAsteroidGeometry->mesh == 0x4204\n"
        "           && observedAsteroidGeometry->lodCount == 3);\n"
        "    assert(observedAsteroidGeometry->lodMeshes[0] == 0x4205\n"
        "           && observedAsteroidGeometry->lodMeshes[1] == 0x4206\n"
        "           && observedAsteroidGeometry->lodMeshes[2] == 0x4207);\n"
        "    assert(observedAsteroidGeometry->lodDistancesSq[0] == 60000ULL * 60000\n"
        "           && observedAsteroidGeometry->lodDistancesSq[1] == 100000ULL * 100000\n"
        "           && observedAsteroidGeometry->lodDistancesSq[2] == 120000ULL * 120000);\n"
        "    assert(level->lodManager->objects->size() == 3);\n"
        "    assert(observedAsteroidRadius >= 120 && observedAsteroidRadius <= 219\n"
        "           && observedAsteroidScale == float(observedAsteroidRadius) * 0.01f\n"
        "           && observedAsteroidQuality >= 4 && observedAsteroidQuality <= 7);\n"
        "    assert(observedAsteroidPosition.x >= expectedX - 30000\n"
        "           && observedAsteroidPosition.x < expectedX + 30000\n"
        "           && observedAsteroidPosition.y >= expectedY - 30000\n"
        "           && observedAsteroidPosition.y < expectedY + 30000\n"
        "           && observedAsteroidPosition.z >= expectedZ - 30000\n"
        "           && observedAsteroidPosition.z < expectedZ + 30000);\n"
        "    assert(opened == closed && opened >= 42);\n"
        "    assert(canvas.meshCount >= 77 && canvas.transformCount >= 86);")
    gate = replace_once(gate, "stage 1 entered asteroid prelude: OK",
                        "stage 1 built first asteroid geometry: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
