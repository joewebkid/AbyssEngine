"""Run Pan's first ordinary freighter through createShip and enemy insertion."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_freighter_ctor_pan_gate as prior
from run_gles_atlas_asset_gate import ROOT, SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once
from run_particle_control_smoke import function


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    chain_path = out / "station_binary_chain.inc"
    chain = chain_path.read_text()
    ship = extract("src/game/world/Level.cpp", [
        "PlayerFixedObject *Level::createShip(int race, int shipClass, int type, Waypoint *wp, bool hostile, bool group) {"])
    freighter = ship[ship.index("    } else if (shipClass == 1) {"):
                     ship.rindex("    if (obj != 0)")]
    live_ship = function(chain, "PlayerFixedObject *Level::createShip(int race, int shipClass, int type,")
    live_ship = replace_once(live_ship, "        throw FirstFreighterPreludeBoundary{};", "        // Continue into the production class-one branch.")
    live_ship = replace_once(live_ship, "    assert(shipClass == 0);", "    assert(shipClass == 0 || shipClass == 1);")
    live_ship = replace_once(live_ship, "    }\n    if (obj != 0)", freighter + "    if (obj != 0)")
    chain = replace_once(chain, function(chain, "PlayerFixedObject *Level::createShip(int race, int shipClass, int type,"), live_ship)
    chain += "\n" + extract("src/game/ship/PlayerFixedObject.cpp", [
        "void PlayerFixedObject::setBV(Array<BoundingVolume *> *bv) {",
        "void PlayerFixedObject::setWreckedMeshId(int meshId) {"])
    chain += "\nvoid FileRead_ctor(void *p);\nvoid *FileRead_dtor(void *p);\n"
    chain += "\n" + extract("src/game/core/Globals.cpp", [
        "Array<BoundingVolume *> *Globals::getWreckCollision(int kind, AEGeometry *geom) {"])
    chain += "\n" + extract("src/engine/file/FileRead.cpp", [
        "Array<int32_t> *FileRead::loadWreckCollision(int32_t id) {"])
    chain += "\nvoid FileRead_ctor(void *p) { new (p) FileRead; }\n"
    chain += "void *FileRead_dtor(void *p) { static_cast<FileRead *>(p)->~FileRead(); return p; }\n"
    # shortcut: the original TransformRemoveChild delegates to opaque engine
    # helpers; model its observable child-array effect in this host gate.
    chain += '''\nvoid PaintCanvas::TransformRemoveChild(unsigned int parent, unsigned int child) {
    assert(parent != child && parent < transformCount && child < transformCount);
    auto *p = reinterpret_cast<Transform *>(transforms[parent]);
    auto *c = reinterpret_cast<Transform *>(transforms[child]);
    ArrayRemove(c, p->children);
    p->CollectAnimationData();
}\n'''
    chain_path.write_text(chain)

    traps_path = out / "station_binary_traps.inc"
    traps = traps_path.read_text()
    traps = replace_once(traps, "TRAP(_ZN11AbyssEngine11PaintCanvas20TransformRemoveChildEjj)", "")
    traps_path.write_text(traps)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate, '#include "game/ship/PlayerFixedObject.h"',
                        '#include "game/ship/PlayerFixedObject.h"\n#include "engine/file/FileRead.h"')
    gate = replace_once(gate,
        "catch (const FirstFreighterPreludeBoundary &) { reachedMissionDispatch = true; }",
        "catch (const FirstFriendCompleteBoundary &) { reachedMissionDispatch = true; }")
    start = gate.index("    auto *freighter = new PlayerFixedObject(freighterType, freighterRace,")
    end = gate.index('    std::printf("Pan first class-one freighter production constructor: OK\\n");', start)
    end += len('    std::printf("Pan first class-one freighter production constructor: OK\\n");')
    gate = gate[:start] + '''    auto *freighter = dynamic_cast<PlayerFixedObject *>((*level->enemies)[freighterIndex]);
    assert(freighter && freighter->player == freighterPlayer
           && freighterPlayer->kiPlayer == freighter
           && freighter->type == freighterType
           && freighter->faction == freighterRace
           && freighter->level == level && freighter->geometry
           && freighter->position.x == freighterX
           && freighter->position.y == freighterY
           && freighter->position.z == freighterZ
           && freighter->intPosX == int(freighterX)
           && freighter->intPosY == int(freighterY)
           && freighter->intPosZ == int(freighterZ)
           && !freighter->moving && freighter->boundingVolumes
           && freighter->wreckGeometry && freighter->wreckCollision
           && freighter->wreckMaterial == -1
           && freighterPlayer->spawnedFlag && freighter->aiActiveCounter == 0x2f);
    assert(freighter->boundingVolumes->size() == 3
           && freighter->wreckCollision->size() > 0
           && freighter->wreckMeshId == (freighterRace == 2 ? 18301 : 18302)
           && freighter->wreckType == (freighterRace == 2 ? 2 : 3));
    if (freighter->lootList)
        assert(freighter->lootList->size() == 2 || freighter->lootList->size() == 4);
    std::printf("Pan first class-one freighter createShip return: OK\\n");''' + gate[end:]
    gate = replace_once(gate,
        "    assert(materialIndex == changedShipMaterials.size());",
        """    // getShipGroup(15) takes the special freighter branch before material handoff.
    assert(freighterType == 15 && materialIndex == changedShipMaterials.size());""")
    gate = replace_once(gate,
        "    for (unsigned i = level->friendCount + level->field_164;\n"
        "         i < level->enemies->size(); ++i)",
        "    for (unsigned i = level->friendCount + level->field_164 + 1;\n"
        "         i < level->enemies->size(); ++i)")
    gate = replace_once(gate,
        "           (unsigned) level->field_164);\n"
        "    for (int i = 0; i < level->friendCount + level->field_164; ++i)",
        "           (unsigned) level->field_164 + 1);\n"
        "    assert((*level->lodManager->objects)[expectedCount + 2 + level->friendCount + level->field_164] == freighter->geometry);\n"
        "    for (int i = 0; i < level->friendCount + level->field_164; ++i)")
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
