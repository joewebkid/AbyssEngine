"""Continue Pan Level startup through the original weapon-position file."""

import hashlib

import run_station_binary_asset_gate as runner
import run_level_player_rotation_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    data = (root / "data/bin/weapons_hd.bin").read_bytes()
    assert len(data) == 7484 and hashlib.sha256(data).hexdigest() == (
        "eebc7b6361bfdb19248f3f40071f915dad0cca919b292ad015d8d890f5570f82")
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct PlayerRotatedBoundary {};",
                        "struct PlayerWeaponPositionsBoundary {};\n"
                        "static Array<Array<Vector *> *> *observedWeaponPositions;")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    production = extract("src/game/world/Level.cpp", ["void Level::createPlayer() {"])
    start = production.index("    if (equip != nullptr) {")
    end = production.index("        for (unsigned i = 0; i < equip->size(); i++) {", start)
    level = replace_once(level,
        "    observedGunBuckets = buckets;\n    throw PlayerRotatedBoundary{};",
        "    observedGunBuckets = buckets;\n" + production[start:end] +
        "        observedWeaponPositions = wpos;\n"
        "        throw PlayerWeaponPositionsBoundary{};\n"
        "    }")
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/engine/file/FileRead.cpp", [
        "Array<Array<Vector *> *> *FileRead::loadWeaponPositions(int32_t id) {"])
    chain += "\n" + extract("src/engine/file/AEFile.cpp", [
        "uint32_t AEFile::Read(int16_t &value, uint32_t handle)",
        "uint32_t AEFile::Read(float &value, uint32_t handle)"])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "catch (const PlayerRotatedBoundary &)",
                        "catch (const PlayerWeaponPositionsBoundary &)")
    assert gate.count("opened == 37") == 1
    gate = gate.replace("opened == 37", "opened == 38")
    gate = gate.replace("canvas.transformCount == 78", "canvas.transformCount == 82")
    gate = replace_once(gate, "stationOpenedPaths.size() == expectedFiles.size() + 5",
                        "stationOpenedPaths.size() == expectedFiles.size() + 6")
    gate = replace_once(gate, "canvas.transformCount == transformsBefore + 56",
                        "canvas.transformCount == transformsBefore + 60")
    gate = replace_once(gate, "canvas.transformCount == transformsBeforeShip + 6",
                        "canvas.transformCount == transformsBeforeShip + 10")
    gate = replace_once(gate, "opened == filesBeforeShip + 7",
                        "opened == filesBeforeShip + 8")
    gate = replace_once(gate, "    AEGeometry *group = observedEgo->field_0x4;",
        "    assert(stationOpenedPaths[filesBeforeShip + 5] == \"data/bin/weapons_hd.bin\");\n"
        "    assert(observedWeaponPositions && observedWeaponPositions->size() == 4);\n"
        "    assert((*observedWeaponPositions)[0]->size() == 1);\n"
        "    assert((*observedWeaponPositions)[1]->size() == 1);\n"
        "    assert(!(*observedWeaponPositions)[2]);\n"
        "    assert((*observedWeaponPositions)[3]->size() == 8);\n"
        "    assert(level->field_a4 && level->field_a4->size() == 4);\n"
        "    const float positions[4][3] = {{-160, 99, -256}, {159, 100, -256},\n"
        "        {53, 82, -282}, {-53, 82, -282}};\n"
        "    const float scales[4] = {0.5f, 0.5f, 1.0f, 1.0f};\n"
        "    for (int i = 0; i < 4; ++i) {\n"
        "        auto *geo = (*level->field_a4)[i];\n"
        "        assert(geo && geo->scalingX == scales[i] && geo->scalingY == scales[i]\n"
        "               && geo->scalingZ == scales[i]);\n"
        "        const auto &m = geo->getMatrix();\n"
        "        assert(m.m[3] == positions[i][0] && m.m[7] == positions[i][1]\n"
        "               && m.m[11] == positions[i][2]);\n"
        "    }\n"
        "    AEGeometry *group = observedEgo->field_0x4;")
    gate = replace_once(gate,
        "Level::createPlayer -> setLevel -> Y hull rotation and empty gun buckets: OK",
        "Level::createPlayer -> original weapon positions and four geometries: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
