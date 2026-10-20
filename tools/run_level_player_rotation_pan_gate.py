"""Continue the Pan Level call through hull yaw and empty gun buckets."""

import run_station_binary_asset_gate as runner
import run_level_player_particles_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct PlayerParticlesBoundBoundary {};",
                        "struct PlayerRotatedBoundary {};\n"
                        "static Array<Array<Gun *> *> *observedGunBuckets;")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createPlayer() {"])
    start = production.index("    float yaw = ")
    end = production.index("    if (equip != nullptr) {", start)
    level = replace_once(level, "    throw PlayerParticlesBoundBoundary{};",
                         production[start:end] +
                         "    observedGunBuckets = buckets;\n"
                         "    throw PlayerRotatedBoundary{};")
    (out / "level_init_under_test.inc").write_text(level)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "catch (const PlayerParticlesBoundBoundary &)",
                        "catch (const PlayerRotatedBoundary &)")
    gate = replace_once(gate,
        "    assert(level->player == observedEgo && observedEgo->level == level);",
        "    assert(level->player == observedEgo && observedEgo->level == level);\n"
        "    assert(level->field_138 == 1600 || level->field_138 == -1600);\n"
        "    float yaw = float(level->field_138) * (1.0f / 65536.0f) * 6.2831855f;\n"
        "    const auto &rotation = observedEgo->geometry->rotation;\n"
        "    assert(rotation.x == 0.0f && rotation.y == yaw && rotation.z == 0.0f);\n"
        "    const auto &matrix = observedEgo->geometry->getMatrix();\n"
        "    assert(std::fabs(matrix.m[0] - std::cos(yaw)) < 0.0001f);\n"
        "    assert(std::fabs(matrix.m[2] - std::sin(yaw)) < 0.0001f);\n"
        "    assert(std::fabs(matrix.m[8] + std::sin(yaw)) < 0.0001f);\n"
        "    assert(std::fabs(matrix.m[10] - std::cos(yaw)) < 0.0001f);\n"
        "    assert(observedGunBuckets && observedGunBuckets->size() == 3);\n"
        "    for (int i = 0; i < 3; ++i) assert(!(*observedGunBuckets)[i]);")
    gate = replace_once(gate,
        "Level::createPlayer -> setShip -> setLevel particle handles and five AEM meshes: OK",
        "Level::createPlayer -> setLevel -> Y hull rotation and empty gun buckets: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
