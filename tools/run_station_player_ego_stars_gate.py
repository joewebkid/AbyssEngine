"""Extend the Pan Level gate through the complete normal-path PlayerEgo constructor."""
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
                        'Globals *Globals::gGlobals;\nvoid *Globals::globals;\nvoid *Globals::items;')
    host = replace_once(host, 'struct PlayerAllocatedBoundary {};',
                        'struct PlayerEgoConstructedBoundary {};\n'
                        'static Player *observedPlayer;\nstatic PlayerEgo *observedEgo;\n'
                        'static unsigned starsMeshesBefore, starsTransformsBefore, starsMaterialsBefore;\n'
                        'static uint32_t starsSeedLowBefore, starsSeedHighBefore;')
    host = replace_once(host, 'Status *Status::gStatus;',
                        'Status *Status::gStatus;\n' +
                        extract("src/game/ship/PlayerEgo.cpp", ["static inline Status *PE_status()"]))
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
    level = level[:level.index(level_tail) + len(level_tail)] + (
        '\n    observedEgo = ego;\n    throw PlayerEgoConstructedBoundary{};\n}')
    ctor = extract("src/game/ship/PlayerEgo.cpp", ["PlayerEgo::PlayerEgo(Player *player) {"])
    (out / "level_init_under_test.inc").write_text(level + '\n' + ctor)

    chain = (out / "station_binary_chain.inc").read_text()
    chain += '\n' + (ROOT / "tools/moving_stars_canvas_services.inc").read_text()
    chain += '\n' + extract("src/game/ship/Player.cpp", ["void Player::setPlayShootSound("])
    chain += '\n' + extract("src/game/ship/Player.cpp", ["int Player::getCombinedHP()"])
    chain += '\n' + extract("src/game/core/Globals.cpp", ["unsigned int Globals::createBillBoard("])
    chain += '\n' + extract("src/game/core/Globals.cpp", ["void Globals::addSoundResourceToList("])
    chain += '\n' + extract("src/engine/render/MovingStars.cpp", ["MovingStars::MovingStars() {"])
    chain += '\n' + extract("src/game/mission/Status.cpp", ["bool Status::hardCoreMode()"])
    chain += '\n' + extract("src/game/mission/Status.cpp", ["bool Status::inBlackMarketSystem()"])
    chain += '\n' + extract("src/game/mission/Status.cpp", ["bool Status::inSupernovaOrbit()"])
    chain += '\n' + extract("src/game/mission/Item.cpp", [
        "int Item::getAttribute(", "int Item::getSort()", "int Item::getIndex()",
        "int Item::getAmount()"])
    # Fixture-only Item construction; sort/type accessors stay production.
    chain += '\nItem::Item(IntArray *i, IntArray *q, IntArray *a) : '
    chain += 'index(0), type(99), sort(0), ingredients(i), quantities(q), attributes(a) {}\n'
    chain += '\n' + extract("src/game/ship/Ship.cpp", [
        "bool Ship::hasCargo(", "bool Ship::hasVolatileGoods()",
        "int Ship::getFreeSpace()", "Item *Ship::getFirstEquipmentOfSort(",
        "int Ship::getIndex()", "int Ship::getShieldRegen()",
        "int Ship::getAgility()", "int Ship::getBoostSpeed()",
        "int Ship::getBoostDelay()", "int Ship::getBoostTime()",
        "float Ship::getHandling()", "bool Ship::hasCloakIntegrated()",
        "unsigned int Ship::hasJumpDriveIntegrated()"])
    (out / "station_binary_chain.inc").write_text(chain)

    traps = (out / "station_binary_traps.inc").read_text()
    traps = replace_once(traps, 'TRAP(_ZN7Globals22addSoundResourceToListEi)', '')
    traps = replace_once(traps, 'TRAP(_ZN6Status19inBlackMarketSystemEv)', '')
    (out / "station_binary_traps.inc").write_text(traps)

    gles = (out / "sun_gles.inc").read_text()
    gles = replace_once(gles, '    ++flareUploadCalls;',
        '    unsigned previousIndex = engine->appManager->paintCanvas->cubeTextures.size();\n'
        '    int previousBytes = engine->textureByteCounter;\n'
        '    int previousCount = Engine::ImageCount;\n'
        '    ++flareUploadCalls;')
    gles = replace_once(gles, '    assert(result == 1 && *out == 0 && textures.size() == 1);',
        '    bool hudAtlas = std::strcmp(path, "data/textures/gof2_interface3.aei") == 0;\n'
        '    assert(result == 1 && *out == previousIndex && textures.size() == previousIndex + 1);')
    gles = replace_once(gles, '    auto *texture = textures[0];',
                        '    auto *texture = textures[previousIndex];')
    gles = replace_once(gles,
        '    unsigned side = std::strcmp(path, "data/textures/gof2_interface.aei") == 0 ? 1024 : 2048;',
        '    unsigned side = hudAtlas ? 512 : 1024;')
    gles = replace_once(gles,
        '    assert(engine->textureByteCounter == int(texture->byteSize) && Engine::ImageCount == 1);',
        '    assert(engine->textureByteCounter == previousBytes + int(texture->byteSize));\n'
        '    assert(Engine::ImageCount == previousCount + 1);')
    gles = replace_once(gles,
        '    assert(flareUploadCalls == 1 && canvas.cubeTextures.size() == 7 && canvas.images.size() == 3);',
        '    assert(flareUploadCalls == 2 && canvas.cubeTextures.size() == 8 && canvas.images.size() == 8);')
    gles = replace_once(gles,
        '    assert(texture && texture->handle == 0); // Managed index, not the GL name.',
        '    assert(texture && texture->handle == 0); // Managed index, not the GL name.\n'
        '    assert(static_cast<Resource *>(canvas.FindResource(10089))->handle == 7);')
    gles = replace_once(gles, 'assert(cached == i && flareUploadCalls == 1);',
                        'assert(cached == i && flareUploadCalls == 2);')
    gles = replace_once(gles,
        '        glDeleteTextures(1, &texture->glId); delete texture; canvas.cubeTextures[i] = nullptr;\n'
        '    }',
        '        glDeleteTextures(1, &texture->glId); delete texture; canvas.cubeTextures[i] = nullptr;\n'
        '    }\n'
        '    auto *hud = canvas.cubeTextures[7];\n'
        '    assert(hud && hud->byteSize == 512 * 512 * 4);\n'
        '    glDeleteTextures(1, &hud->glId);\n'
        '    delete hud;\n'
        '    canvas.cubeTextures[7] = nullptr;')
    (out / "sun_gles.inc").write_text(gles)

    gate = (out / "station_binary_gate_under_test.inc").read_text()
    gate = replace_once(gate, 'catch (const PlayerAllocatedBoundary &)',
                        'catch (const PlayerEgoConstructedBoundary &)')
    gate = replace_once(gate, '    assert(canvas.meshCount == 18 && canvas.transformCount == 22);',
                        '    assert(canvas.meshCount == 68 && canvas.transformCount == 72);')
    gate = replace_once(gate, '    assert(stationMaterialIds == expectedMaterials);',
                        '    assert(stationMaterialIds.size() == expectedMaterials.size() + 50);\n'
                        '    assert(std::equal(expectedMaterials.begin(), expectedMaterials.end(),\n'
                        '                      stationMaterialIds.begin()));')
    gate = replace_once(gate, '    for (unsigned id : stationMaterialIds) std::printf(" %u", id);',
                        '    for (unsigned id : expectedMaterials) std::printf(" %u", id);')
    gate = replace_once(gate, 'createPlayer constructed Player and set HP/shield/armor',
                        'createPlayer completed PlayerEgo normal-path constructor')
    gate = replace_once(gate, '    assert(opened == 26 && opened == closed);',
                        '    assert(opened == 32 && opened == closed);')
    gate = replace_once(gate, '    assert(stationOpenedPaths == expectedFiles);',
        '    expectedFiles.insert(expectedFiles.end(), {\n'
        '        "data/textures/gof2_interface.aei",\n'
        '        "data/textures/gof2_interface.aei",\n'
        '        "data/textures/gof2_interface3.aei",\n'
        '        "data/textures/gof2_interface3.aei",\n'
        '        "data/textures/gof2_interface3.aei",\n'
        '        "data/textures/gof2_interface.aei"});\n'
        '    assert(stationOpenedPaths == expectedFiles);')
    gate = replace_once(gate, '    status->ship->maxArmorHP = 90;',
                        '    status->ship->maxArmorHP = 90;\n'
                        '    status->ship->boostSpeed = 250;\n'
                        '    status->ship->boostDelay = 4200;\n'
                        '    status->ship->boostTime = 6300;\n'
                        '    status->ship->agility = 15;\n'
                        '    status->ship->cargoPlus = 0;\n'
                        '    status->ship->shieldRegen = 20;\n'
                        '    int equipmentSorts[] = {19, 9, 10};\n'
                        '    for (unsigned i = 0; i < 3; ++i) {\n'
                        '        auto *item = new Item(nullptr, nullptr, nullptr);\n'
                        '        item->sort = equipmentSorts[i];\n'
                        '        (*status->ship->equipment)[i] = item;\n'
                        '    }')
    gate = replace_once(gate, '    assert(level->field_134 == 0 && !level->playerRoute && !level->enemyRoute);',
                        '    assert(level->field_134 == 0 && !level->playerRoute && !level->enemyRoute);\n'
                        '    Globals globalsFixture;\n'
        '    Array<int> soundResources;\n'
        '    globalsFixture.soundResources = &soundResources;\n'
                        '    Globals::gGlobals = &globalsFixture;\n'
                        '    Globals::globals = &globalsFixture;\n'
                        '    assert(canvas.FindResource(20052) && !canvas.FindResource(10001));')
    marker = '    return 0;\n}'
    assert gate.count(marker) == 1
    check = '''    // Continuous Level call created five original HUD atlas regions.
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
    assert(ego->lastHP == 470 && ego->field_0x1c == 45);
    assert(ego->boostSpeed == 7 && ego->field_0xcc == 6300 && ego->field_0xd0 == 4200);
    assert(ego->field_0x146 == 1 && ego->boostSoundId == 0);
    assert(ego->handling > 52.8f && ego->handling < 53.0f);
    assert(ego->agility == 15.0f && ego->shieldRecoveryAmount == 400.0f);
    assert(ego->lastFreeSpace == 50 && !ego->volatileGoods);
    assert(!ego->shieldRegenEquipped && !ego->cloak && !ego->driveChargeMax);
    assert(!status->inBlackMarketSystem());
    assert(!status->inSupernovaSystem() && !status->inSupernovaOrbit());
    assert(ego->steerLimit > 754.0f && ego->steerLimit < 758.0f);
    assert(ego->steerScale == 15.139357f);
    for (float angle : ego->steerAngleHistory) assert(angle == 0.0f);
    assert(!ego->steerHistoryFull && ego->steerHistoryIndex == 0);
    assert(!ego->autoTurretEnabled && !ego->dockedFlag);
    assert(!ego->miningGame && !ego->hackingGame);
    assert(ego->visible && !ego->cloakChildHidden && !ego->field_0x309);
    assert(ego->requestedVisible && ego->exhaustVisible);
    assert(ego->dockingPointIndex == -1 && ego->cloakRechargeMax == 5000);
    assert(!ego->gunYawGeo && ego->engineSoundId == -1);
    const int expectedSounds[] = {45, 1, 3, 2, 25, 23};
    assert(!ego->hardCoreMode && soundResources.size() == 6);
    for (unsigned i = 0; i < 6; ++i) assert(soundResources[i] == expectedSounds[i]);
    assert(canvas.images.size() == 8 && flareUploadCalls == 2);
    const unsigned hudIds[] = {1216, 1230, 8030, 8029, 1352};
    const unsigned hudHandles[] = {ego->radarBlipImage1, ego->radarBlipImage2,
        ego->radarBlipImage3, ego->radarBlipImage4, ego->dockArrowImage};
    const unsigned hudTexture[] = {0, 0, 7, 7, 0};
    const unsigned hudRects[][4] = {{339, 814, 40, 40}, {339, 856, 40, 40},
        {67, 327, 107, 107}, {67, 218, 107, 107}, {505, 199, 82, 43}};
    int openedBeforeHudCache = opened;
    for (unsigned i = 0; i < 5; ++i) {
        assert(hudHandles[i] == i + 3);
        auto *res = static_cast<AbyssEngine::Resource *>(canvas.FindResource(hudIds[i]));
        auto *region = canvas.images[hudHandles[i]];
        assert(res && res->handle == int(hudHandles[i]) && region && region->mesh);
        assert(region->textureId == hudTexture[i] && region->restoreFlag == 0);
        assert(region->atlasW == (hudTexture[i] ? 512 : 1024));
        assert(region->atlasH == region->atlasW);
        assert(region->offX == hudRects[i][0] && region->offY == hudRects[i][1]);
        assert(region->sizeX == hudRects[i][2] && region->sizeY == hudRects[i][3]);
        unsigned cached = 0xffffffffU;
        canvas.Image2DCreate(hudIds[i], cached);
        assert(cached == hudHandles[i] && opened == openedBeforeHudCache && opened == closed);
    }
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
    std::printf("PlayerEgo constructor -> 50 MovingStars, ship defaults, five HUD images: OK\\n");
'''
    gate = replace_once(gate, marker, check + marker)
    (out / "station_binary_gate_under_test.inc").write_text(gate)


if __name__ == "__main__":
    runner.prepare = prepare
    runner.main()
