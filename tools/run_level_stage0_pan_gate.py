"""Return from production Level::init stage zero with original Pan assets."""

import run_station_binary_asset_gate as runner
import run_level_player_complete_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        "            thisptr->createPlayer();\n            throw PlayerCreatedBoundary{};",
        "            thisptr->createPlayer();")
    (out / "level_init_under_test.inc").write_text(level)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/ship/PlayerEgo.cpp", [
        "void PlayerEgo::setPosition(float x, float y, float z) {"])
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN9PlayerEgo11setPositionEfff)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        "    bool reached = false;\n"
        "    try {\n"
        "        (void) level->init();\n"
        "        unexpected(\"Level::init returned past the flare-image boundary\");\n"
        "    } catch (const PlayerCreatedBoundary &) {\n"
        "        reached = true;\n"
        "    }\n"
        "    assert(reached && level->field_134 == 0 && level->player == observedEgo && level->starSystem);",
        "    assert(Level::initStreamOutPosition == 0);\n"
        "    int stageResult = level->init();\n"
        "    assert(stageResult == 0 && level->field_134 == 1\n"
        "           && level->player == observedEgo && level->starSystem);\n"
        "    const auto &playerMatrix = observedEgo->geometry->getMatrix();\n"
        "    assert(playerMatrix.m[3] == 10.0f && playerMatrix.m[7] == 10.0f\n"
        "           && playerMatrix.m[11] == 10000.0f);")
    gate = replace_once(gate,
        "Level::createPlayer returned after original weapon positions and cleanup: OK",
        "Level::init stage 0 returned after complete Pan player creation: OK")
    gate = replace_once(gate, "stage 0 incomplete", "stage 0 returned")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.append("engine/render/Material")
    runner.prepare = prepare
    runner.main()
