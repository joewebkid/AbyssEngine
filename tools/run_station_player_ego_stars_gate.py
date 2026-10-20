"""Extend the Pan Player gate through the real PlayerEgo/MovingStars constructor prefix."""
import run_station_binary_asset_gate as runner
import run_station_player_alloc_gate as first
from run_level_init_asset_gate import ROOT, extract
from run_level_station_asset_gate import replace_once


def prepare(out, cxx, root, reuse):
    first.prepare(out, cxx, root, reuse)

    host = (out / "sun_host.inc").read_text()
    host = replace_once(host, '#include "game/core/Globals.h"',
                        '#include "game/core/Globals.h"\n#include "engine/render/MovingStars.h"')
    host = replace_once(host, 'Globals *Globals::gGlobals;',
                        'Globals *Globals::gGlobals;\nvoid *Globals::globals;')
    host = replace_once(host, 'struct PlayerAllocatedBoundary {};',
                        'struct PlayerEgoStarsBoundary {};\n'
                        'static Player *observedPlayer;\nstatic PlayerEgo *observedEgo;\n'
                        'static unsigned starsMeshesBefore, starsTransformsBefore, starsMaterialsBefore;\n'
                        'static uint32_t starsSeedLowBefore, starsSeedHighBefore;')
    host = host.replace('id == 65535', 'id == 65535 || id == 20052')
    (out / "sun_host.inc").write_text(host)

    level = (out / "level_init_under_test.inc").read_text()
    level = replace_once(level,
        '    throw PlayerAllocatedBoundary{}; // Host boundary before PlayerEgo allocation.',
        '    observedPlayer = pl;\n'
        '    starsMeshesBefore = Globals::Canvas->meshCount;\n'
        '    starsTransformsBefore = Globals::Canvas->transformCount;\n'
        '    starsMaterialsBefore = stationMaterialIds.size();\n'
        '    starsSeedLowBefore = AERandom::gRandom->seedLow;\n'
        '    starsSeedHighBefore = AERandom::gRandom->seedHigh;')
    level_tail = '    new(ego) PlayerEgo(pl);'
    assert level.count(level_tail) == 1
    level = level[:level.index(level_tail) + len(level_tail)] + '\n}'
    ctor = extract("src/game/ship/PlayerEgo.cpp", ["PlayerEgo::PlayerEgo(Player *player) {"])
    marker = '    this->movingStars = new MovingStars();'
    assert ctor.count(marker) == 1
    ctor = ctor[:ctor.index(marker) + len(marker)] + (
        '\n    observedEgo = this;\n    throw PlayerEgoStarsBoundary{};\n}')
    (out / "level_init_under_test.inc").write_text(level + '\n' + ctor)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + (ROOT / "tools/moving_stars_canvas_services.inc").read_text()
    chain += '\n' + extract("src/game/ship/Player.cpp", ["void Player::setPlayShootSound("])
    chain += '\n' + extract("src/game/core/Globals.cpp", ["unsigned int Globals::createBillBoard("])
    chain += '\n' + extract("src/engine/render/MovingStars.cpp", ["MovingStars::MovingStars() {"])
    (out / "station_binary_chain.inc").write_text(chain)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'catch (const PlayerAllocatedBoundary &)',
                        'catch (const PlayerEgoStarsBoundary &)')
    gate = replace_once(gate, '    assert(canvas.meshCount == 18 && canvas.transformCount == 22);',
                        '    assert(canvas.meshCount == 68 && canvas.transformCount == 72);')
    gate = replace_once(gate, '    assert(stationMaterialIds == expectedMaterials);',
                        '    assert(stationMaterialIds.size() == expectedMaterials.size() + 50);\n'
                        '    assert(std::equal(expectedMaterials.begin(), expectedMaterials.end(),\n'
                        '                      stationMaterialIds.begin()));')
    gate = replace_once(gate, '    for (unsigned id : stationMaterialIds) std::printf(" %u", id);',
                        '    for (unsigned id : expectedMaterials) std::printf(" %u", id);')
    gate = replace_once(gate, 'createPlayer constructed Player and set HP/shield/armor',
                        'createPlayer entered PlayerEgo MovingStars prefix')
    gate = replace_once(gate, '    assert(level->field_134 == 0 && !level->playerRoute && !level->enemyRoute);',
                        '    assert(level->field_134 == 0 && !level->playerRoute && !level->enemyRoute);\n'
                        '    Globals globalsFixture;\n'
                        '    Globals::gGlobals = &globalsFixture;\n'
                        '    Globals::globals = &globalsFixture;\n'
                        '    assert(canvas.FindResource(20052) && !canvas.FindResource(10001));')
    marker = '    return 0;\n}'
    assert gate.count(marker) == 1
    check = '''    // Continuous Level call reached the Ego prefix; its tail remains a test boundary.
    assert(observedPlayer && observedEgo);
    AERandom expected(0);
    expected.seedLow = starsSeedLowBefore;
    expected.seedHigh = starsSeedHighBefore;
    for (int i = 0; i < 50; ++i) expected.nextInt(4);
    unsigned meshesBefore = starsMeshesBefore, transformsBefore = starsTransformsBefore;
    auto materialsBefore = starsMaterialsBefore;
    auto *ego = observedEgo;
    auto *stars = ego->movingStars;
    assert(ego->player == observedPlayer && ego->dockCameraNode == nullptr && stars);
    assert(canvas.meshCount == meshesBefore + 50 && canvas.transformCount == transformsBefore + 50);
    assert(stationMaterialIds.size() == materialsBefore + 50);
    assert(random.seedLow == expected.seedLow && random.seedHigh == expected.seedHigh);
    assert(stars->textureHandle == 0 && stars->tickAccumulator == 0);
    assert(!stars->animResetFlag && !stars->animActiveFlag);
    for (int i = 0; i < 50; ++i) {
        assert(stationMaterialIds[materialsBefore + i] == 20052);
        assert(stars->lifeArray[i] == -1 && stars->billboardIds[i] == meshesBefore + unsigned(i));
        assert(stars->transformHandles[i] == transformsBefore + unsigned(i));
        auto *mesh = reinterpret_cast<AbyssEngine::Mesh *>(canvas.meshes[stars->billboardIds[i]]);
        assert(mesh->vertexCount == 12 && mesh->indexCount == 18);
        auto *p = static_cast<float *>(mesh->positions);
        auto *uv = static_cast<float *>(mesh->texCoords);
        auto *tri = static_cast<uint16_t *>(mesh->indices);
        assert(p[0] == -70.0f && p[2] == 500.0f && p[35] == 0.0f);
        assert(uv[0] == 0.87695f && uv[1] == 0.24805f);
        assert(tri[0] == 0 && tri[1] == 1 && tri[2] == 2);
        auto *local = static_cast<Matrix *>(canvas.TransformGetLocal(stars->transformHandles[i]));
        auto *transform = static_cast<AbyssEngine::Transform *>(
            canvas.TransformGetTransform(stars->transformHandles[i]));
        assert(transform && transform->meshes.size() == 1 && transform->meshes[0] == mesh);
        assert(local && local->m[3] == 100000.0f && local->m[7] == 100000.0f
               && local->m[11] == 100000.0f);
    }
    std::printf("PlayerEgo prefix -> 50 MovingStars billboards/transforms: OK\\n");
'''
    gate = replace_once(gate, marker, check + marker)
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    runner.prepare = prepare
    runner.main()
