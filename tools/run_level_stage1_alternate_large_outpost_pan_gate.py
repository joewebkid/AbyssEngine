"""Run the ARM-backed alternate large-outpost construction in Pan's host chain."""

import run_station_binary_asset_gate as runner
import run_level_stage1_large_outpost_turrets_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    mission = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    start = mission.index("                Vector position;", mission.index("            if (alternateLarge) {"))
    end = mission.index("            } else (*enemies)[index] = createShip", start)
    branch = mission[start:end]
    start = mission.index("            static_cast<PlayerFixedObject *>((*enemies)[index])->setMoving(false);", end)
    end = mission.index("        } else if (index == start && smallOutpost)", start)
    branch += mission[start:end]
    branch = replace_once(branch,
        "            float x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);",
        "            freighterMoveBeforeLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveBeforeHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;\n"
        "            float x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);")
    branch = replace_once(branch, "            outpost->setPosition(x, y, z);",
        "            outpost->setPosition(x, y, z);\n"
        "            freighterMoveAfterLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveAfterHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;")
    level_path = out / "level_init_under_test.inc"
    level = level_path.read_text()
    level = replace_once(level,
        "largeOutpost = true; smallOutpost = alternateLarge = false; // Test variant after the original draws.",
        "largeOutpost = alternateLarge = true; smallOutpost = false; // Test variant after the original draws.")
    level = replace_once(level,
        "assert(index == start && largeOutpost && !alternateLarge && !smallOutpost);",
        "assert(index == start && largeOutpost && alternateLarge && !smallOutpost);")
    start = level.index("            (*enemies)[index] = createShip(race, 1, 14, nullptr, true, false);")
    end = level.index("    }\n    if (largeTurrets) {", start)
    level = level[:start] + branch + level[end:]
    level_path.write_text(level)

    host_path = out / "sun_host.inc"
    host = host_path.read_text()
    assert host.count("id == 24076") == 2
    host_path.write_text(host.replace("id == 24076", "id == 24076 || id == 29090 || (id >= 11776 && id <= 11782)"))

    static_source = extract("src/game/world/Level.cpp", [
        "intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {"])
    start = static_source.index("        } else if (type == 18804) {")
    end = static_source.index("        } else if (type == 19051) {", start)
    alt_branch = static_source[start + len("        } else if (type == 18804) {\n"):end]
    alt_branch = replace_once(alt_branch,
        "            auto *points = file->loadSpacePoints(5, -1);",
        "            auto *points = file->loadSpacePoints(5, -1);\n"
        "            assert(points && points->size() > 0);")
    alt_branch = replace_once(alt_branch, "            goto installBounds;",
        "            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(collisionIndex, nullptr)));")
    existing_path = out / "station_binary_chain.inc"
    chain = existing_path.read_text()
    existing = function(chain, "intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {")
    existing = replace_once(existing, "    assert(type == 6772);",
        """    if (type == 18804) {
        // Preserve the original wrapper allocation before its selected branch.
        AEGeometry *geometry = new AEGeometry(static_cast<unsigned short>(type), Globals::Canvas, false);
        (void) geometry;
        KIPlayer *object = nullptr;
        int collisionIndex;
""" + alt_branch + """        object->setLevel(this);
        return reinterpret_cast<intptr_t>(object);
    }
    assert(type == 6772);""")
    chain = replace_once(chain, function(chain, "intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {"), existing)
    chain = replace_once(chain, "void PlayerFixedObject::setPosition(const Vector &) { std::abort(); }",
        extract("src/game/ship/PlayerFixedObject.cpp", ["void PlayerFixedObject::setPosition(const Vector &v) {"]))
    chain += "\n" + extract("src/game/ship/KIPlayer.cpp", [
        "void KIPlayer::setSpacePoints(Array<SpacePoint *> *pts) {"])
    chain += "\n" + extract("src/engine/file/FileRead.cpp", [
        "Array<SpacePoint *> *FileRead::loadSpacePoints(int32_t id, int32_t group) {"])
    chain += "\n" + extract("src/game/ship/Player.cpp", [
        "void Player::setHitpoints(int value) {"])
    chain += "\n" + extract("src/engine/file/AEFile.cpp", [
        "uint32_t AEFile::Read(uint16_t &value, uint32_t handle) {"])
    chain += "\n" + extract("src/game/world/SpacePoint.cpp", [
        "SpacePoint::SpacePoint(int type, const Vector &position, const Vector &direction, int param)"])
    existing_path.write_text(chain)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate,
        "largeTurrets = 7; // Match the test-only non-alternate large outpost.",
        "largeTurrets = 8; // Match the forced alternate large outpost.")
    start = gate.index("    AERandom expectedFreighter(0);")
    end = gate.index("    auto *fighter = dynamic_cast<PlayerFighter *>", start)
    gate = gate[:start] + """    const int outpostIndex = level->friendCount + level->field_164;
    assert(level->field_168 == 1 && freighterIndex == outpostIndex);
    AERandom expectedMove(0);
    expectedMove.seedLow = freighterMoveBeforeLow;
    expectedMove.seedHigh = freighterMoveBeforeHigh;
    const float moveX = float(expectedMove.nextInt(80000) - 40000);
    const float moveY = float(expectedMove.nextInt(10000) - 5000);
    const float moveZ = float(expectedMove.nextInt(80000) + 40000);
    assert(freighterMoveAfterLow == expectedMove.seedLow
           && freighterMoveAfterHigh == expectedMove.seedHigh);
    auto *freighter = dynamic_cast<PlayerFixedObject *>((*level->enemies)[outpostIndex]);
    assert(freighter && freighter->type == 18804 && freighter->player
           && freighter->player->kiPlayer == freighter
           && freighter->level == level && freighter->geometry
           && freighter->geometry->mesh == 18804
           && freighter->player->hitpoints == 9999999
           && !freighter->moving && freighter->cargo == nullptr
           && freighter->spacePoints == nullptr
           && freighter->field_0x70 == 0
           && freighter->position.x == moveX
           && freighter->position.y == moveY
           && freighter->position.z == moveZ
           && freighter->intPosX == int(moveX)
           && freighter->intPosY == int(moveY)
           && freighter->intPosZ == int(moveZ)
           && freighter->boundingVolumes
           && freighter->boundingVolumes->size() > 0);
    for (unsigned id : {18804, 18805, 18807, 18808, 18809, 18810, 18806, 18766}) {
        auto *resource = static_cast<AbyssEngine::Resource *>(canvas.FindResource(id));
        assert(resource && resource->handle >= 0);
    }
    assert(std::count(stationOpenedPaths.begin(), stationOpenedPaths.end(),
                      "data/bin/docks_hd.bin") == 1 && opened == closed);
    std::printf("Pan alternate large outpost object: OK\\n");
""" + gate[end:]
    gate = replace_once(gate,
        "    // getShipGroup(14) takes the special outpost branch before material handoff.\n"
        "    assert(freighterType == 14 && materialIndex == changedShipMaterials.size());",
        "    assert(materialIndex == changedShipMaterials.size());")
    gate = replace_once(gate, "static const float turretRecords[7][6] = {",
        "static const float turretRecords[8][6] = {")
    turret_start = gate.index("    static const float turretRecords[8][6] = {")
    turret_end = gate.index("    };", turret_start)
    gate = gate[:turret_start] + """    static const float turretRecords[8][6] = {
        {-6725.87890625f, 2458.2626953125f, -66.0760498046875f, -0.0f, 1.570796012878418f, 0.0f},
        {5824.43408203125f, 2458.2626953125f, 10243.14453125f, -0.0f, -1.570796012878418f, -0.0f},
        {-6725.87890625f, 2458.2626953125f, 2099.96435546875f, -0.0f, 1.570796012878418f, 0.0f},
        {5824.43408203125f, 2458.2626953125f, 8436.650390625f, -0.0f, -1.570796012878418f, -0.0f},
        {-5548.7109375f, -2743.15185546875f, 11954.466796875f, -3.1415929794311523f, 1.570796012878418f, -0.0f},
        {-5548.7109375f, -2743.15185546875f, -3710.50390625f, -3.1415929794311523f, 1.570796012878418f, -0.0f},
        {4981.2587890625f, -2743.15185546875f, 14659.005859375f, 3.1415929794311523f, -2.1816608905792236f, 0.0f},
        {0.0f, 2458.2626953125f, -32811.1015625f, -0.0f, -0.0f, -0.0f}
""" + gate[turret_end:]
    gate = replace_once(gate, "    for (int i = 0; i < 7; ++i) {\n        auto *turret =",
                        "    for (int i = 0; i < 8; ++i) {\n        auto *turret =")
    gate = replace_once(gate, "Pan seven large-outpost turrets: OK",
                        "Pan eight alternate large-outpost turrets: OK")
    gate = replace_once(gate,
        "           (unsigned) level->field_164 + 1);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2 + level->friendCount + level->field_164] == freighter->geometry);",
        "           (unsigned) level->field_164);")
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
