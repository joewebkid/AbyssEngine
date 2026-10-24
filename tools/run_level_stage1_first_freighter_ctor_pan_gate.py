"""Construct Pan's first ordinary freighter from its production C++ body."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_freighter_prelude_pan_gate as prior
from run_gles_atlas_asset_gate import ROOT, SOURCES
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    prior.prepare(out, cxx, root, reuse)

    chain_path = out / "station_binary_chain.inc"
    chain = chain_path.read_text()
    source = (ROOT / "src/game/ship/PlayerFixedObject.cpp").read_text()
    start = source.index("PlayerFixedObject::PlayerFixedObject(int kind, int param2, Player *player, AEGeometry *geom,")
    chain += "\n" + source[start:source.index("\nvoid PlayerFixedObject::setPosition(float", start)]
    # shortcut: virtuals outside construction fail closed; link their real
    # bodies when the gate advances through createShip or Fighter lifetime.
    chain += '''\nPlayerFixedObject::~PlayerFixedObject() { std::abort(); }
int PlayerFixedObject::collide(float, float, float) { std::abort(); }
V3 PlayerFixedObject::getPosition() { std::abort(); }
V3 PlayerFixedObject::getProjectionVector(const Vector &) { std::abort(); }
int PlayerFixedObject::outerCollide(float, float, float) { std::abort(); }
V3 PlayerFixedObject::projectCollisionOnSurface(const Vector &) { std::abort(); }
void PlayerFixedObject::render() { std::abort(); }
void PlayerFixedObject::setPosition(float, float, float) { std::abort(); }
void PlayerFixedObject::setPosition(const Vector &) { std::abort(); }
void PlayerFixedObject::translate(const Vector &) { std::abort(); }
void PlayerFixedObject::update(int) { std::abort(); }
'''
    chain_path.write_text(chain)

    gate_path = out / "station_binary_gate_under_test.inc"
    gate = gate_path.read_text()
    gate = replace_once(gate, '#include "game/ship/PlayerFighter.h"',
        '#include "game/ship/PlayerFighter.h"\n#include "game/ship/PlayerFixedObject.h"')
    gate = replace_once(gate,
        '    std::printf("Pan first class-one freighter Player prelude: OK\\n");',
        '''    auto *freighter = new PlayerFixedObject(freighterType, freighterRace,
        freighterPlayer, nullptr, freighterX, freighterY, freighterZ);
    assert(freighter && freighter->player == freighterPlayer
           && freighterPlayer->kiPlayer == freighter
           && freighter->type == freighterType
           && freighter->faction == freighterRace
           && freighter->position.x == freighterX
           && freighter->position.y == freighterY
           && freighter->position.z == freighterZ
           && freighter->intPosX == int(freighterX)
           && freighter->intPosY == int(freighterY)
           && freighter->intPosZ == int(freighterZ)
           && !freighter->moving && !freighter->boundingVolumes
           && !freighter->wreckGeometry && !freighter->wreckCollision
           && freighter->wreckMeshId == 0 && freighter->wreckMaterial == -1
           && freighterPlayer->spawnedFlag && freighter->aiActiveCounter == 0x2f);
    if (freighter->lootList)
        assert(freighter->lootList->size() == 2 || freighter->lootList->size() == 4);
    std::printf("Pan first class-one freighter production constructor: OK\\n");''')
    gate_path.write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
