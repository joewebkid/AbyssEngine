// Production Level/PlayerStation chain; host services are deliberately bounded.
#include "station_level_host.inc"
#include "engine/file/FileRead.h"
#include "engine/math/BoundingVolume.h"
#include "engine/math/BoundingAAB.h"
#include "engine/math/BoundingSphere.h"
#include "game/world/Route.h"
#include "game/world/StarSystem.h"

Status *Globals::status;
uint32_t AEFile::FileExist(const String &path) {
    char *name = path.GetAEChar();
    std::ifstream file(assetBase + "/" + name, std::ios::binary);
    delete[] name;
    return file.good();
}
void MatrixIdentity(void *, void *);
void Array_KIPlayer_ctor(void *);
void Array_Station_release(void *);
void *Array_Station_dtor(void *);
int StarSystem::orbitPlanetIndex;
#include "station_chain_under_test.inc"

// Reaching the next unvalidated object must fail closed, never return an object.
struct FlareImageBoundary {};
void AbyssEngine::PaintCanvas::Image2DCreate(unsigned short id, unsigned int &) {
    assert(id == 0x508);
    auto *resource = static_cast<Resource *>(FindResource(id));
    assert(resource && resource->kind == 3 && resource->handle == -1);
    throw FlareImageBoundary{};
}

#define TRAP(symbol) extern "C" void symbol() { unexpected(#symbol); }
#include "level_station_traps.inc"

static void checkManagers(const Level &level, PaintCanvas *canvas) {
    assert(level.lodManager && level.lodManager->objects);
    assert(level.lodManager->objects->size() == 2 && level.lodManager->timer == 1001);
    const ParticleSystemManager *systems[] = {
        level.particleEmitBoolPtr, level.particleRenderBoolPtr, level.field_74,
        level.field_80, level.field_98, level.particleSystemMgr, level.skybox2Mesh,
        level.field_8c, level.field_9c, level.field_94,
    };
    const int ids[] = {0x4e85, 0x6a72, 0x4e83, 0x4e7a, 0x5e20,
                       0x4e7f, 0x4e7c, 0x6a7c, 0x6ab9, 0x6aaf};
    for (unsigned i = 0; i < 10; ++i) {
        assert(systems[i] && systems[i]->canvas == canvas);
        assert(systems[i]->spriteTextureId == ids[i]);
        assert(systems[i]->cameraSet == ParticleSettings::CameraSet_1);
        assert(systems[i]->flags == 0x101);
    }
}

static void checkCollision(PlayerStation *station) {
    auto *volumes = station->boundingVolumes;
    assert(volumes && volumes->size() == 44);
    unsigned boxes = 0, spheres = 0;
    for (auto *volume : *volumes) {
        assert(volume && volume->extentsX == 0 && volume->extentsY == 0 && volume->extentsZ == 0);
        boxes += dynamic_cast<BoundingAAB *>(volume) != nullptr;
        spheres += dynamic_cast<BoundingSphere *>(volume) != nullptr;
    }
    assert(boxes == 23 && spheres == 21);
    // Independent decoded record 7 in original collision.bin. These values
    // reject the former axis/argument-order and zero-radius reconstruction.
    auto *box = dynamic_cast<BoundingAAB *>((*volumes)[0]);
    assert(box && box->centerX == -25951 && box->centerY == -273 && box->centerZ == 25988);
    assert(box->halfExtentX == 5859 && box->halfExtentY == 5028 && box->halfExtentZ == 5918);
    assert(box->outerCollide(-25951, -273, 25988));
    assert(!box->outerCollide(-25951 + 5860, -273, 25988));
    BoundingAAB shiftedBox(10, 20, 30, 1, 2, 3, 10, 8, 6);
    const Vector boxProjection = shiftedBox.projectCollisionOnSurface({11, 22, 35});
    assert(boxProjection.x == 11 && boxProjection.y == 22 && boxProjection.z == 36);
    auto *sphere = dynamic_cast<BoundingSphere *>((*volumes)[22]);
    assert(sphere && sphere->centerX == -2 && sphere->centerY == 9865 && sphere->centerZ == 9357);
    assert(sphere->radius == 2000);
    assert(sphere->collide(-2, 9865, 9357));
    assert(!sphere->outerCollide(1999, 9865, 9357));
    BoundingSphere shifted(10, 20, 30, 1, 2, 3, 5);
    assert(shifted.outerCollide(14, 22, 33));
    assert(!shifted.outerCollide(16, 22, 33));
    const Vector projected = shifted.projectCollisionOnSurface({12, 22, 33});
    assert(projected.x == 16 && projected.y == 22 && projected.z == 33);
    const Vector normal = shifted.getCollisionNormal({12, 22, 33});
    assert(normal.x == -1 && normal.y == 0 && normal.z == 0);
    assert(station->outerCollide(-2, 9865, 9357));
    assert(!station->outerCollide(1e9f, 0, 0));
}

static void checkJumpgates(const Level &level, PaintCanvas &canvas) {
    for (int slot = 1; slot <= 2; ++slot) {
        auto *gate = dynamic_cast<PlayerJumpgate *>((*level.landmarks)[slot]);
        assert(gate && gate->geometry && gate->player);
        assert(gate->isVisible() == (slot == 1));
        assert(gate->player->radius == (slot == 1 ? 7500 : 0));
        assert(gate->activated == 0 && gate->transformHandle != 0xffffffffU);
        auto *animation = static_cast<AbyssEngine::Transform *>(canvas.TransformGetTransform(gate->transformHandle));
        assert(animation && animation->meshes.size() == 1);
        assert(animation->meshes[0]->animation); // Actual animated AEM/clone path.
        AEGeometry *g = gate->geometry;
        assert((*level.lodManager->objects)[slot - 1] == g);
        assert(g->mesh == 0x3a98 && g->rotation.y == 3.1415927f);
        assert(g->lodCount == 2 && g->lodMeshes[0] == 0x3aa8 && g->lodMeshes[1] == 0x3aac);
        assert(g->lodDistancesSq[0] == 1600000000ULL && g->lodDistancesSq[1] == 4900000000ULL);
        if (slot == 1) {
            assert(gate->boundingVolumes && gate->boundingVolumes->size() == 1);
            auto *sphere = dynamic_cast<BoundingSphere *>((*gate->boundingVolumes)[0]);
            assert(sphere && sphere->radius == 7500 && sphere->centerY == 0);
        } else {
            assert(!gate->boundingVolumes);
        }
    }
    assert(!(*level.landmarks)[3]); // gameWon fixture deliberately excludes wormhole.
}

int main(int argc, char **argv) {
#ifdef _WIN32
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif
    assert(argc == 2);
    assetBase = argv[1];
    openSucceeds = true;
    Engine::enableShader = 0;
    Engine::vboSupported = false;
    Engine engine;
    PaintCanvas canvas(&engine);
    Globals::Canvas = PaintCanvas::gCanvas = &canvas;
    AERandom random(14);
    Globals::rnd = AERandom::gRandom = &random;
    auto *status = new Status();
    Globals::status = Status::gStatus = status;
    status->station = status->playerStation;
    status->station->index = 7;
    status->field_80 = -1;
    status->currentCampaignMission = 50;
    int rgb[] = {31, 60, 90};
    status->system = new SolarSystem(4, String(), 0, true, 0, 0, 0, 0,
                                     7, 2, rgb, nullptr, nullptr, nullptr);
    registerResources(&canvas);
    assert(canvas.resources.size() == 2490);
    for (auto *resource : canvas.resources)
        assert(resource && resource->payload && resource->handle == -1);
    alignas(Level) unsigned char storage[sizeof(Level)];
    std::memset(storage, 0xa5, sizeof(storage));
    Level *level = new (storage) Level(3);
    assert(level->field_134 == 0 && !level->playerRoute && !level->enemyRoute);
    bool reached = false;
    try {
        (void) level->init();
        unexpected("Level::init returned past the flare-image boundary");
    } catch (const FlareImageBoundary &) {
        reached = true;
    }
    assert(reached && level->field_134 == 0 && !level->player && !level->starSystem);
    checkManagers(*level, &canvas);
    assert(level->landmarks && level->landmarks->size() == 4);
    auto *station = dynamic_cast<PlayerStation *>((*level->landmarks)[0]);
    assert(station && station->player && station->stationIndex == 7);
    assert(station->player == static_cast<KIPlayer *>(station)->player);
    assert(station->rootGeometry && station->rootGeometry->mesh == 21007);
    assert(!station->rootGeometry->hasLod());
    assert(station->player->radius == 15000);
    checkCollision(station);
    checkJumpgates(*level, canvas);
    assert(station->_pad_0x71 == 1);
    assert(station->rootGeometry->rotation.y == 3.1415927f);
    auto *root = static_cast<AbyssEngine::Transform *>(canvas.TransformGetTransform(station->rootGeometry->transform));
    assert(root && root->meshes.size() == 1 && root->meshes[0]->vertexCount > 0);
    assert(station->collisionRadius == int(root->boundingRadius + 5000.0f));
    assert(opened == 14 && opened == closed);
    assert(canvas.meshCount == 17 && canvas.transformCount == 15);
    const std::vector<std::string> expectedFiles = {
        "data/assets/main/3d/meshes/skyboxes/skybox_stars.aem",
        "data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_001.aei",
        "data/assets/main/3d/meshes/skyboxes/skybox_002.aem",
        "data/assets/main/3d/textures/low/etc/skyboxes/skybox_002.aei",
        "data/bin/collision.bin",
        "data/assets/main/3d/meshes/stations/station_007_terran.aem",
        "data/assets/main/3d/meshes/stations/station_007_terran_emissive.aem",
        "data/assets/main/3d/meshes/stations/station_007_terran_lights_add.aem",
        "data/assets/main/3d/meshes/jumpgates/jumpgate_terran.aem",
        "data/assets/main/3d/meshes/jumpgates/jumpgate_terran_emissive.aem",
        "data/assets/main/3d/meshes/jumpgates/jumpgate_terran_anim_add.aem",
        "data/assets/main/3d/meshes/jumpgates/jumpgate_terran_jump_anim_add.aem",
        "data/assets/main/3d/meshes/jumpgates/jumpgate_terran_lod_1.aem",
        "data/assets/main/3d/meshes/jumpgates/jumpgate_terran_lod_2.aem",
    };
    assert(stationOpenedPaths == expectedFiles);
    const std::vector<unsigned> expectedMaterials = {
        20010, 20011, 20004, 20003, 3, 20004, 20003, 3, 3, 20004, 20004,
    };
    assert(stationMaterialIds == expectedMaterials);
    for (const auto &path : stationOpenedPaths) std::printf("asset: %s\n", path.c_str());
    std::printf("material boundary IDs:");
    for (unsigned id : stationMaterialIds) std::printf(" %u", id);
    std::printf("\n");
    std::printf("Level -> production PlayerStation -> jumpgates -> StarSystem -> LensFlare image boundary: OK; "
                "%u meshes, %u transforms, %u collision volumes, %d/%d files; stage 0 incomplete\n",
                canvas.meshCount, canvas.transformCount, station->boundingVolumes->size(), opened, closed);
    for (int collisionId : {1002, 2000}) {
        auto *volumes = static_cast<Array<BoundingVolume *> *>(level->getBoundingVolume(collisionId, nullptr));
        assert(volumes && !volumes->empty());
        unsigned shapes = 0;
        for (auto *volume : *volumes) shapes += volume != nullptr;
        assert(volumes->size() == (collisionId == 1002 ? 12u : 26u));
        assert(shapes == volumes->size() && opened == closed);
        if (collisionId == 1002) {
            // First collision.bin record: sphere (0, 0, -3287), radius -20762.
            auto *sphere = dynamic_cast<BoundingSphere *>((*volumes)[0]);
            assert(sphere && sphere->centerX == 0 && sphere->centerY == 0 && sphere->centerZ == 0);
            assert(sphere->extentsX == 0 && sphere->extentsY == -3287 && sphere->extentsZ == 0);
            assert(std::fabs(sphere->radius - 12457.2f) < 0.01f);
        } else {
            // First static_collisions.bin record: box (-53, 1262, 2154), signed sizes.
            auto *box = dynamic_cast<BoundingAAB *>((*volumes)[0]);
            assert(box && box->centerX == 0 && box->centerY == 0 && box->centerZ == 0);
            assert(box->extentsX == -53 && box->extentsY == 2154 && box->extentsZ == -1262);
            assert(std::fabs(box->halfExtentX - 7024.8f) < 0.01f);
            assert(std::fabs(box->halfExtentY - 4976.4f) < 0.01f);
            assert(std::fabs(box->halfExtentZ - 15610.8f) < 0.01f);
        }
        std::printf("Level getBoundingVolume %d: %u records, %u shapes\n",
                    collisionId, volumes->size(), shapes);
        ArrayReleaseClasses(*volumes);
        delete volumes;
    }
}
