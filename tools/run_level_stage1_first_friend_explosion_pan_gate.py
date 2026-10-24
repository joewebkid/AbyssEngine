"""Run Fighter's Explosion(0) and fire-streak dependency after Pan's first friend."""

import run_station_binary_asset_gate as runner
import run_level_stage1_first_friend_route_clone_pan_gate as first
from run_gles_atlas_asset_gate import SOURCES
from run_level_init_asset_gate import extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    assert host.count("id == 29045") == 2
    host = host.replace("id == 29045", "id == 29045 || id == 27252 || id == 27253")
    (out / "sun_host.inc").write_text(host)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += "\n" + extract("src/game/mission/Explosion.cpp", [
        "void Explosion::addFireStreaks() {"])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate,
        '    std::printf("Level::init stage 0 returned and stage 1 verified both first-friend Route clone branches: OK\\n");',
        '''    Explosion *fighterExplosion = new Explosion(0);
    assert(fighterExplosion->type == 0 && fighterExplosion->primaryMesh
           && fighterExplosion->primaryMesh->mesh == 0x41b5
           && fighterExplosion->secondaryMesh == nullptr
           && fighterExplosion->fireStreaks == nullptr);
    AERandom expectedStreakRng = *static_cast<AERandom *>(Globals::rnd);
    int expectedStreakCount = expectedStreakRng.nextInt(7) + 3;
    for (int i = 0; i < expectedStreakCount; ++i) {
        expectedStreakRng.nextInt(0x168);
        expectedStreakRng.nextInt(0x168);
        expectedStreakRng.nextInt(0x168);
        expectedStreakRng.nextInt(0x32);
    }
    fighterExplosion->addFireStreaks();
    assert(fighterExplosion->fireStreaks
           && fighterExplosion->fireStreaks->size() == unsigned(expectedStreakCount)
           && static_cast<AERandom *>(Globals::rnd)->seedLow == expectedStreakRng.seedLow
           && static_cast<AERandom *>(Globals::rnd)->seedHigh == expectedStreakRng.seedHigh);
    for (auto *streak : *fighterExplosion->fireStreaks) {
        assert(streak && streak->mesh == 0x37d4);
        auto *transform = static_cast<AbyssEngine::Transform *>(
            Globals::Canvas->TransformGetTransform(streak->transform));
        assert(transform && transform->boundingRadius == 10000.0f);
    }
    unsigned meshCount = canvas.meshCount;
    fighterExplosion->addFireStreaks();
    assert(canvas.meshCount == meshCount
           && fighterExplosion->fireStreaks->size() == unsigned(expectedStreakCount));
    // The full Fighter/Explosion ownership chain is outside this bounded gate.
    assert(opened == closed);
    std::printf("stage 1 first-friend Explosion(0) and fire streaks: OK\\n");''')
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    SOURCES.extend(["engine/render/Material", "game/world/Waypoint"])
    runner.prepare = prepare
    runner.main()
