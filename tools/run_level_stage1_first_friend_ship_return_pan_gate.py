"""Run Pan's class-zero createShip branch through first array insertion."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_friend_fighter_ctor_pan_gate as first
from run_gles_atlas_asset_gate import ROOT, SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    low = (ROOT / "src/engine/render/LowResourceTable.cpp").read_text()
    materials = []
    for resource_id in range(0x7dc8, 0x7ded):
        needle = f"res->id = 0x{resource_id:x};"
        if needle not in low:
            continue
        pos = low.index(needle)
        material = low[low.rfind("    {", 0, pos):low.index("    }", pos) + 5]
        assert "res->kind = 6;" in material
        materials.append(material)
    assert len(materials) >= 3
    resources = (out / "level_resource_under_test.inc").read_text()
    resources = replace_once(resources, "\n}\nnamespace AbyssEngine {\nResourceMaterial::ResourceMaterial",
        "\n" + "\n".join(materials) + "\n}\nnamespace AbyssEngine {\nResourceMaterial::ResourceMaterial")
    (out / "level_resource_under_test.inc").write_text(resources)
    host = (out / "sun_host.inc").read_text()
    assert host.count("id == 27253") == 2
    host = host.replace("id == 27253", "id == 27253 || (id >= 0x7dc8 && id <= 0x7dec)")
    (out / "sun_host.inc").write_text(host)

    ship = extract("src/game/world/Level.cpp", [
        "PlayerFixedObject *Level::createShip(int race, int shipClass, int type, Waypoint *wp, bool hostile, bool group) {"])
    branch = ship[ship.index("    PlayerFixedObject *obj = 0;"):
                  ship.index("    } else if (shipClass == 1) {")]
    footer = ship[ship.rindex("    if (obj != 0)"):]
    prelude = ship[:ship.index("    PlayerFixedObject *obj = 0;")]
    live_ship = prelude + '''    assert(shipClass == 0); // This gate covers the Pan ordinary-fighter branch only.
    firstFriendRace = race; firstFriendClass = shipClass; firstFriendType = type;
    firstFriendWaypoint = wp; firstFriendHostile = hostile; firstFriendGroup = group;
    firstFriendPlayer = pl;
    firstFriendX = fx; firstFriendY = fy; firstFriendZ = fz;
    firstFriendSeedLow = static_cast<AERandom *>(Globals::rnd)->seedLow;
    firstFriendSeedHigh = static_cast<AERandom *>(Globals::rnd)->seedHigh;
''' + branch + "    }\n" + footer
    chain = (out / "station_binary_chain.inc").read_text()
    chain = replace_once(chain,
        function(chain, "PlayerFixedObject *Level::createShip(int race, int shipClass, int type,"),
        live_ship)
    chain = replace_once(chain,
        "void PlayerFighter::setShipGroup(AEGeometry *, int, bool) { std::abort(); }",
        extract("src/game/ship/PlayerFighter.cpp", [
            "void PlayerFighter::setShipGroup(AEGeometry *geom, int group, bool flag) {"]))
    chain = replace_once(chain,
        "void PlayerFighter::setLevel(Level *) { std::abort(); }",
        extract("src/game/ship/PlayerFighter.cpp", ["void PlayerFighter::setLevel(Level *lvl) {"]))
    chain += extract("src/game/ship/KIPlayer.cpp", [
        "void KIPlayer::setShipGroup(AEGeometry *geom, int group, bool flag) {"])
    chain = replace_once(chain,
        "void PaintCanvas::MeshChangeResourceMaterial(unsigned int, unsigned short) { std::abort(); }",
        '''// Record the material handoff; the full renderer remains outside this gate.
static std::vector<unsigned short> changedShipMaterials;
void PaintCanvas::MeshChangeResourceMaterial(unsigned int meshIndex, unsigned short resId) {
    auto *resource = static_cast<AbyssEngine::Resource *>(FindResource(resId));
    assert(meshIndex < meshCount && meshes[meshIndex]);
    assert(resource && resource->kind == 6 && resId >= 0x7dc8 && resId <= 0x7dec);
    changedShipMaterials.push_back(resId);
}''')
    chain = chain.replace("std::abort(); }",
        'std::fprintf(stderr, "Unrecovered test stub: %s\\n", __PRETTY_FUNCTION__); std::abort(); }')
    (out / "station_binary_chain.inc").write_text(chain)
    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, "TRAP(_ZN8KIPlayer12setShipGroupEP10AEGeometryib)", "")
    (out / "station_binary_traps.inc").write_text(traps)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, "struct FirstFriendPlayerBoundary {};",
        "struct FirstFriendPlayerBoundary {};\nstruct FirstFriendCompleteBoundary {};\n"
        "static uint32_t firstFriendSeedLow, firstFriendSeedHigh;")
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    call = "            (*enemies)[i] = createShip(race, 0, type, ambientFriendRoute->getWaypoint(), true, false);"
    level = replace_once(level, call,
        call + "\n            if (i == 0) throw FirstFriendCompleteBoundary{};")
    (out / "level_init_under_test.inc").write_text(level)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, "canvas.resources.size() == 2725",
        f"canvas.resources.size() == {2725 + len(materials)}")
    gate = replace_once(gate,
        "catch (const FirstFriendPlayerBoundary &) { reachedMissionDispatch = true; }",
        "catch (const FirstFriendCompleteBoundary &) { reachedMissionDispatch = true; }")
    gate = replace_once(gate,
        "static_cast<AERandom *>(Globals::rnd)->seedLow == expected.seedLow",
        "firstFriendSeedLow == expected.seedLow")
    gate = replace_once(gate,
        "static_cast<AERandom *>(Globals::rnd)->seedHigh == expected.seedHigh",
        "firstFriendSeedHigh == expected.seedHigh")
    gate = replace_once(gate,
        '''    auto *fighter = new PlayerFighter(firstFriendType, firstFriendRace,
        firstFriendPlayer, nullptr, firstFriendX, firstFriendY, firstFriendZ,
        firstFriendHostile);''',
        "    auto *fighter = dynamic_cast<PlayerFighter *>((*level->enemies)[0]);")
    gate = replace_once(gate,
        "    for (unsigned i = 0; i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);",
        "    assert((*level->enemies)[0] == fighter);\n"
        "    for (unsigned i = 1; i < level->enemies->size(); ++i)\n"
        "        assert((*level->enemies)[i] == nullptr);\n")
    gate = replace_once(gate,
        "    assert(level->lodManager->objects->size() == (unsigned) expectedCount + 2);",
        "    assert(level->lodManager->objects->size() == (unsigned) expectedCount + 3);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2] == fighter->parentGeometry);")
    gate = replace_once(gate,
        "           && fighter->hitpoints == firstFriendPlayer->getHitpoints());",
        "           && fighter->hitpoints == firstFriendPlayer->getHitpoints()\n"
        "           && fighter->geometry && fighter->parentGeometry\n"
        "           && fighter->level == level\n"
        "           && fighter->shipGroupFlag == firstFriendType\n"
        "           && fighter->engineTrailSystem >= 100\n"
        "           && particleAdds.size() == particleEmits.size());")
    gate = replace_once(gate,
        "    if (fighter->lootList)",
        "    assert(changedShipMaterials.size() == 1 +\n"
        "           (gGSG_lodTable[firstFriendType * 3] != 0xffff) +\n"
        "           (gGSG_lodTable[firstFriendType * 3 + 1] != 0xffff));\n"
        "    for (unsigned short id : changedShipMaterials)\n"
        "        assert(id == 0x7dc8 + firstFriendType);\n"
        "    if (fighter->lootList)")
    gate = replace_once(gate,
        "Pan first-friend production PlayerFighter constructor: OK",
        "Pan first-friend createShip returned Fighter into enemies[0]: OK")
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
