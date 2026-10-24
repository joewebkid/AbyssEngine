"""Run the ARM-backed small-outpost branch and five turrets in Pan's host chain."""

import run_station_binary_asset_gate as runner
import run_level_stage1_alternate_large_outpost_pan_gate as prior
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)
    mission = extract("src/game/world/Level.cpp", ["void Level::createMission() {"])
    branch_start = mission.index("            Vector position;", mission.index("        } else if (index == start && smallOutpost) {"))
    branch_end = mission.index("        } else {", branch_start)
    branch = mission[branch_start:branch_end]
    branch = replace_once(branch, "            Vector position;",
        "            freighterMoveBeforeLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveBeforeHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;\n"
        "            Vector position;")
    branch = replace_once(branch,
        "            position.z = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 60000);",
        "            position.z = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 60000);\n"
        "            freighterMoveAfterLow = static_cast<AERandom *>(Globals::rnd)->seedLow;\n"
        "            freighterMoveAfterHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;")
    small_turrets = mission[mission.index("    if (smallOutpost) {"):mission.index("    if (attackers) {", mission.index("    if (smallOutpost) {"))]

    path = out / "level_init_under_test.inc"
    level = path.read_text()
    level = replace_once(level,
        "largeOutpost = alternateLarge = true; smallOutpost = false; // Test variant after the original draws.",
        "smallOutpost = true; largeOutpost = alternateLarge = false; // Forced after faction-one selection draws.")
    level = replace_once(level, "void Level::createMission() {",
        "void Level::createMission() {\n"
        "    Globals::status->system->faction = 1; // Pan fixture changes faction only at this test boundary.")
    start = level.index("    for (; index < friendCount + field_164 + field_168; ++index) {")
    end = level.index("    if (largeTurrets) {", start)
    level = level[:start] + """    for (; index < friendCount + field_164 + field_168; ++index) {
        int start = friendCount + field_164;
        assert(index == start && smallOutpost && !largeOutpost && race == 1);
        freighterIndex = index;
        freighterSeedBeforeLow = static_cast<AERandom *>(Globals::rnd)->seedLow;
        freighterSeedBeforeHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;
""" + branch + "    }\n" + small_turrets + level[level.index("    throw FirstFriendCompleteBoundary{};", end):]
    path.write_text(level)

    path = out / "sun_host.inc"
    host = path.read_text()
    assert host.count("id == 24076") == 2
    path.write_text(host.replace("id == 24076",
        "id == 24076 || id == 24077 || id == 29041 || id == 29042"
        " || id == 20031 || id == 20032 || id == 20033"))

    static_source = extract("src/game/world/Level.cpp", [
        "intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {"])
    start = static_source.index("        } else if (type == 19051) {")
    end = static_source.index("        } else if (type == 21113) {", start)
    small_branch = static_source[start + len("        } else if (type == 19051) {\n"):end]
    small_branch = replace_once(small_branch,
        "            auto *points = file->loadSpacePoints(8, -1);",
        "            auto *points = file->loadSpacePoints(8, -1);\n"
        "            assert(points && points->size() > 0);")
    small_branch = replace_once(small_branch, "            goto installBounds;",
        "            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(collisionIndex, nullptr)));")
    path = out / "station_binary_chain.inc"
    chain = path.read_text()
    original = function(chain, "intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {")
    original = replace_once(original, "    assert(type == 6772);",
        """    if (type == 19051) {
        AEGeometry *geometry = new AEGeometry(static_cast<unsigned short>(type), Globals::Canvas, false);
        (void) geometry;
        KIPlayer *object = nullptr;
        int collisionIndex;
""" + small_branch + """        object->setLevel(this);
        return reinterpret_cast<intptr_t>(object);
    }
    assert(type == 6774);""")
    chain = replace_once(chain, function(chain, "intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {"), original)
    chain += "\n" + extract("src/game/ship/PlayerFixedObject.cpp", [
        "void *PlayerFixedObject::setName(String name) {"])
    path.write_text(chain)

    path = out / "station_binary_gate_under_test.inc"
    gate = path.read_text()
    gate = replace_once(gate, "    assert(key == 1666);",
        "    assert(key == 1666 || key == 1667);")
    gate = replace_once(gate, "    return &label;",
        "    static String smallLabel(\"Small Outpost\", false);\n    return key == 1667 ? &smallLabel : &label;")
    gate = replace_once(gate, "        largeTurrets = 8; // Match the forced alternate large outpost.",
        "        int smallTurrets = 5; // Match the forced faction-one small outpost.")
    gate = replace_once(gate, "        assert(race == 0); // Pan's ordinary non-DLC faction.",
        "        assert(race == 1); // Faction-one variant in Pan's original-asset host.")
    start = gate.index("        static const int armRaceTable[37] = {")
    end = gate.index("        int jitterX =", start)
    gate = gate[:start] + "        unsigned firstType = 9; // Faction one selects this fighter without a draw.\n" + gate[end:]
    gate = replace_once(gate, "friends + f164 + f168 + hostile + largeTurrets)",
        "friends + f164 + f168 + hostile + smallTurrets)")
    start = gate.index("    AERandom expectedMove(0);")
    end = gate.index("    auto *fighter = dynamic_cast<PlayerFighter *>", start)
    gate = gate[:start] + """    AERandom expectedMove(0);
    expectedMove.seedLow = freighterMoveBeforeLow;
    expectedMove.seedHigh = freighterMoveBeforeHigh;
    const float originX = float(expectedMove.nextInt(80000) - 40000);
    const float originY = float(expectedMove.nextInt(40000) - 20000);
    const float originZ = float(expectedMove.nextInt(80000) + 60000);
    assert(freighterMoveAfterLow == expectedMove.seedLow
           && freighterMoveAfterHigh == expectedMove.seedHigh);
    auto *freighter = dynamic_cast<PlayerFixedObject *>((*level->enemies)[outpostIndex]);
    assert(freighter && freighter->type == 19051 && freighter->player
           && freighter->player->kiPlayer == freighter
           && freighter->level == level && freighter->geometry
           && freighter->geometry->mesh == 19051
           && freighter->player->hitpoints == 9999999
           && !freighter->moving && freighter->cargo == nullptr
           && freighter->spacePoints == nullptr
           && freighter->field_0x70 == 0
           && freighter->position.x == originX
           && freighter->position.y == originY
           && freighter->position.z == originZ
           && freighter->intPosX == int(originX)
           && freighter->intPosY == int(originY)
           && freighter->intPosZ == int(originZ)
           && freighter->boundingVolumes
           && freighter->boundingVolumes->size() > 0
           && freighter->name.Compare("Small Outpost") == 0);
    for (unsigned id : {19051, 19052, 19053, 18300}) {
        auto *resource = static_cast<AbyssEngine::Resource *>(canvas.FindResource(id));
        assert(resource && resource->handle >= 0);
    }
    assert(std::count(stationOpenedPaths.begin(), stationOpenedPaths.end(),
                      "data/bin/docks_hd.bin") == 1 && opened == closed);
    std::printf("Pan small outpost object: OK\\n");
""" + gate[end:]
    start = gate.index("    static const float turretRecords[8][6] = {")
    end = gate.index("    };", start)
    gate = gate[:start] + """    static const float turretRecords[5][6] = {
        {10283.1796875f, -1122.7913818359375f, 26039.26171875f, 0.0f, 0.0f, 0.0f},
        {-10171.173828125f, -1082.7020263671875f, 25906.552734375f, 0.0f, 0.0f, 0.0f},
        {-15624.38671875f, -787.19921875f, -7569.43017578125f, 0.0f, 0.0f, 0.0f},
        {15624.38671875f, -787.19921875f, -7569.43017578125f, 0.0f, 0.0f, 0.0f},
        {0.0f, 4901.47314453125f, 3083.96435546875f, 0.0f, 0.0f, 0.0f}
""" + gate[end:]
    gate = replace_once(gate, "    for (int i = 0; i < 8; ++i) {\n        auto *turret =",
        "    for (int i = 0; i < 5; ++i) {\n        auto *turret =")
    gate = replace_once(gate, "turret->type == 6772", "turret->type == 6774")
    gate = replace_once(gate, "turret->baseGeometry->mesh == 6772", "turret->baseGeometry->mesh == 6774")
    gate = replace_once(gate, "turret->turretGeometry->mesh == 6773", "turret->turretGeometry->mesh == 6775")
    gate = replace_once(gate, "turret->shipGroup == status->system->getRace()", "turret->shipGroup == 1")
    gate = replace_once(gate, "Pan eight alternate large-outpost turrets: OK", "Pan five small-outpost turrets: OK")
    path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
