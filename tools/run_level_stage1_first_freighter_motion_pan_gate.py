"""Run Pan's first ordinary freighter through moving and relocation."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_freighter_ship_return_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    host_path = out / "sun_host.inc"
    host = host_path.read_text()
    host = replace_once(host, "static float freighterX, freighterY, freighterZ;",
        "static float freighterX, freighterY, freighterZ;\n"
        "static uint32_t freighterMoveBeforeLow, freighterMoveBeforeHigh;\n"
        "static uint32_t freighterMoveAfterLow, freighterMoveAfterHigh;")
    host_path.write_text(host)

    mission = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = mission.index("            static_cast<PlayerFixedObject *>((*enemies)[index])->setMoving(true);")
    end = mission.index("            freighter->setPosition(x, y, z);", start)
    motion = mission[start:end + len("            freighter->setPosition(x, y, z);")]
    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    call = "            (*enemies)[index] = createShip(shipRace, 1, shipRace == 1 ? 13 : 15, nullptr, true, false);"
    level = replace_once(level, call, call + "\n"
        "            freighterMoveBeforeLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveBeforeHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;\n"
        + motion + "\n"
        "            freighterMoveAfterLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveAfterHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;")
    level_path.write_text(level)

    chain_path = out / "station_binary_chain.inc"
    chain = chain_path.read_text()
    chain = replace_once(chain, "void PlayerFixedObject::setPosition(float, float, float) { std::abort(); }",
        extract("src/game/ship/PlayerFixedObject.cpp", [
            "void PlayerFixedObject::setPosition(float x, float y, float z) {"]))
    chain += "\n" + extract("src/game/ship/PlayerFixedObject.cpp", [
        "void PlayerFixedObject::setMoving(bool v) {"])
    chain_path.write_text(chain)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate, "    auto *freighter = dynamic_cast<PlayerFixedObject *>((*level->enemies)[freighterIndex]);",
        """    AERandom expectedMove(0);
    expectedMove.seedLow = freighterMoveBeforeLow;
    expectedMove.seedHigh = freighterMoveBeforeHigh;
    int distance = expectedMove.nextInt(60000) - 80000;
    float moveX = float((expectedMove.nextInt(2) == 0 ? 1 : -1) * distance);
    float moveY = float(expectedMove.nextInt(40000) - 20000);
    float moveZ = float(expectedMove.nextInt(160000) - 80000);
    assert(freighterMoveAfterLow == expectedMove.seedLow
           && freighterMoveAfterHigh == expectedMove.seedHigh
           && static_cast<AERandom *>(Globals::rnd)->seedLow == expectedMove.seedLow
           && static_cast<AERandom *>(Globals::rnd)->seedHigh == expectedMove.seedHigh);
    auto *freighter = dynamic_cast<PlayerFixedObject *>((*level->enemies)[freighterIndex]);""")
    gate = replace_once(gate,
        """           && freighter->position.x == freighterX
           && freighter->position.y == freighterY
           && freighter->position.z == freighterZ
           && freighter->intPosX == int(freighterX)
           && freighter->intPosY == int(freighterY)
           && freighter->intPosZ == int(freighterZ)
           && !freighter->moving && freighter->boundingVolumes""",
        """           && freighter->moving && freighter->boundingVolumes
           && freighter->spawnX == moveX && freighter->spawnY == moveY
           && freighter->spawnZ == moveZ
           && freighter->position.x == moveX
           && freighter->position.y == moveY
           && freighter->position.z == moveZ
           && freighter->intPosX == int(moveX)
           && freighter->intPosY == int(moveY)
           && freighter->intPosZ == int(moveZ)""")
    gate = replace_once(gate,
        "    if (freighter->lootList)\n        assert(freighter->lootList->size() == 2 || freighter->lootList->size() == 4);",
        """    auto geomPos = freighter->geometry->getPosition();
    auto wreckPos = freighter->wreckGeometry->getPosition();
    assert(geomPos.x == moveX && geomPos.y == moveY && geomPos.z == moveZ
           && wreckPos.x == moveX && wreckPos.y == moveY && wreckPos.z == moveZ);
    for (auto *bv : *freighter->boundingVolumes)
        assert(bv && bv->centerX == moveX && bv->centerY == moveY && bv->centerZ == moveZ);
    if (freighter->lootList)
        assert(freighter->lootList->size() == 2 || freighter->lootList->size() == 4);""")
    gate = replace_once(gate, "Pan first class-one freighter createShip return: OK",
                        "Pan first class-one freighter moving and relocation: OK")
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
