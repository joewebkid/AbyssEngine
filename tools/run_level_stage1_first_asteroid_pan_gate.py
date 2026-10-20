"""Run Pan's second Level::init call through one complete PlayerAsteroid."""

import run_station_binary_asset_gate as runner
import run_level_stage1_asteroid_first_geometry_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct AsteroidFirstGeometryBoundary {};",
                        '#include "game/ship/PlayerAsteroid.h"\n'
                        "struct AsteroidFirstObjectBoundary {};\n"
                        "static PlayerAsteroid *observedAsteroid;")
    assert host.count("id == 20023") == 2
    host = host.replace("id == 20023", "id == 20023 || id == 20051")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createAsteroids() {"])
    start = production.index("        PlayerAsteroid *a =")
    end = production.index("    }\n\n    if (prob != 0)", start)
    level = replace_once(level,
        "        throw AsteroidFirstGeometryBoundary{};\n    }\n}\n",
        production[start:end] +
        "        observedAsteroid = a;\n"
        "        throw AsteroidFirstObjectBoundary{};\n"
        "    }\n}\n")
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/ship/KIPlayer.cpp", ["void KIPlayer::setLevel(Level *lvl) {"])
    chain += "\n" + extract("src/game/ship/Player.cpp", ["int Player::getHitpoints() {"])
    chain += "\n" + extract("src/game/mission/Explosion.cpp", [
        "static inline PaintCanvas *explosionCanvas() {",
        "static inline AbyssEngine::AERandom *explosionRandom() {",
        "void Explosion::reset() {", "Explosion::~Explosion() {",
        "void Explosion::setScaling(float scale) {", "Explosion::Explosion(int type) {",
    ])
    chain += "\n" + extract("src/game/ship/PlayerAsteroid.cpp", [
        "void PlayerAsteroid::setPosition(const Vector &position) {",
        "Vector PlayerAsteroid::getPosition() {",
        "void PlayerAsteroid::setAsteroidCenter(Vector center) {",
        "PlayerAsteroid::PlayerAsteroid(int playerId, AEGeometry *geometry, int explosionType,",
        "PlayerAsteroid::~PlayerAsteroid() {",
    ])
    chain += "\nAbyssEngine::AEMath::Vector PlayerAsteroid::tmp_vector2;\n"
    chain += "AbyssEngine::AEMath::Vector PlayerAsteroid::asteroidCenter;\n"
    chain += "float PlayerAsteroid::asteroidDistance;\n"
    chain += "float PlayerAsteroid::emitTime;\n"
    (out / "station_binary_chain.inc").write_text(chain)
    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN8KIPlayer8setLevelEP5Level)", "")
    # shortcut: unused virtual methods fail closed; replace them when the gate exercises them.
    traps += "\n" + "\n".join("TRAP(" + symbol + ")" for symbol in (
        "_ZN14PlayerAsteroid9translateERKN11AbyssEngine6AEMath6VectorE",
        "_ZN14PlayerAsteroid6renderEv",
        "_ZN14PlayerAsteroid8initPushERKN11AbyssEngine6AEMath6VectorEi",
        "_ZN14PlayerAsteroid4pushEi",
        "_ZN14PlayerAsteroid6updateEi",
        "_ZN14PlayerAsteroid7collideEfff",
        "_ZN14PlayerAsteroid12outerCollideEfff",
        "_ZN14PlayerAsteroid19getProjectionVectorERKN11AbyssEngine6AEMath6VectorE",
    )) + "\n"
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "catch (const AsteroidFirstGeometryBoundary &)",
                        "catch (const AsteroidFirstObjectBoundary &)")
    gate = replace_once(gate,
        "    assert(level->lodManager->objects->size() == 3);",
        "    assert(level->lodManager->objects->size() == 3);\n"
        "    assert(observedAsteroid && (*level->asteroids)[0] == observedAsteroid);\n"
        "    assert(observedAsteroid->geometry == observedAsteroidGeometry\n"
        "           && observedAsteroid->level == level\n"
        "           && observedAsteroid->asteroidIndex == 0x9a\n"
        "           && observedAsteroid->getPosition().x == observedAsteroidPosition.x\n"
        "           && observedAsteroid->getPosition().y == observedAsteroidPosition.y\n"
        "           && observedAsteroid->getPosition().z == observedAsteroidPosition.z);\n"
        "    assert(observedAsteroid->scaling == observedAsteroidScale\n"
        "           && observedAsteroid->quality == observedAsteroidQuality\n"
        "           && observedAsteroid->minable && observedAsteroid->rotationEnabled\n"
        "           && observedAsteroid->player->kiPlayer == observedAsteroid\n"
        "           && observedAsteroid->player->hitpoints == observedAsteroid->player->maxHitpoints\n"
        "           && observedAsteroid->lastHitpoints == observedAsteroid->player->hitpoints\n"
        "           && observedAsteroid->player->maxHitpoints == (int) (observedAsteroidScale * 100.0f + 30.0f)\n"
        "           && observedAsteroid->explosion && observedAsteroid->explosion->type == 2\n"
        "           && observedAsteroid->explosion->primaryMesh\n"
        "           && observedAsteroid->explosion->primaryMesh->mesh == 0x4213\n"
        "           && observedAsteroid->explosion->secondaryMesh\n"
        "           && observedAsteroid->explosion->secondaryMesh->mesh == 0x4211);\n"
        "    auto *asteroidTransform = static_cast<AbyssEngine::Transform *>(\n"
        "        Globals::Canvas->TransformGetTransform(observedAsteroidGeometry->transform));\n"
        "    assert(observedAsteroid->player->radius ==\n"
        "           (int) (asteroidTransform->boundingRadius * observedAsteroidScale * 0.7f));\n")
    gate = replace_once(gate, "stage 1 built first asteroid geometry: OK",
                        "stage 1 built first PlayerAsteroid: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
