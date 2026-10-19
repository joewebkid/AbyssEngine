// Intact production Level bodies, real layouts, and a fail-closed host boundary.
#define GOF2_LEVEL_INTEGRATION
#define GOF2_LEVEL_INIT_INTEGRATION
#include "mesh_create_smoke.cpp"
#include "game/world/Level.h"
#include "game/world/SolarSystem.h"
#include "game/world/Galaxy.h"
#include "game/ship/PlayerStation.h"
#include "game/ship/PlayerJumpgate.h"
#include "game/ship/PlayerWormHole.h"
#include "game/mission/Status.h"
#include "game/core/Globals.h"
#include "game/core/GameSettings.h"
#include "engine/core/AERandom.h"
#include "engine/render/ResourceMesh.h"
#include "engine/render/ResourceMaterial.h"
#include "engine/render/ResourceTexture.h"
#include <cstdio>

static void unexpected(const char *name) {
    std::fprintf(stderr, "Outside Level init gate: %s\n", name);
    std::abort();
}

uint32_t AEFile::Skip(uint32_t bytes, uint32_t handle) {
    assert(handle == 1);
    if (cursor + bytes > limit) return 0;
    cursor += bytes;
    return bytes;
}
struct PCTexInfoView { char *path; float value; };
namespace AbyssEngine { void ImageRelease(Image **slot); }
#include "level_image_under_test.inc"

namespace AbyssEngine {
static unsigned int nextTexture = 100;
int TextureCreateFromFile(Engine *, const char *, ImageCallback, void *,
                          unsigned int *, bool, float) { unexpected(__func__); return -1; }
int TextureCreateFromFileIntern(Engine *engine, const char *path, ImageCallback callback,
                                void *user, unsigned int *out, float,
                                AELoadedTexture *, bool) {
    Image *image = nullptr;
    if (ImageCreateFromFile(engine, path, &image) != 1) return -1;
    assert(!callback && !user);
    assert(image && image->format == 11 && image->width && image->height);
    assert(image->data && image->dataLen);
    ImageRelease(&image);
    *out = nextTexture++;
    return 1;
}
// Deliberately bounded host services, not recovered platform initialization.
Engine::Engine() {}
Engine::~Engine() {}
PaintCanvas::PaintCanvas(Engine *value) : meshCount(0), meshes(nullptr),
    mask2dImage(nullptr), engine(value), transformCount(0), transforms(nullptr),
    transforms_cap(0) {}
PaintCanvas::~PaintCanvas() {}
PaintCanvas *PaintCanvas::gCanvas;
ApplicationManager *ApplicationManager::gAppManager;
void PaintCanvas::MaterialCreate(unsigned short id, unsigned int &out) {
    assert(id == 20010 || id == 20011);
    out = 0xffffffffU;
}
}

Status *Status::gStatus;
PaintCanvas *Globals::Canvas;
void *Globals::rnd;
void *Globals::galaxy;
Globals *Globals::gGlobals;
unsigned char Globals::options[100];
void *Mission::empty;
float Level::r, Level::g, Level::b, Level::i_r, Level::i_g, Level::i_b;
int Level::r_min, Level::g_min, Level::b_min;
unsigned char Level::comingFromAlienWorld, Level::initStreamOutPosition;

#include "level_texture_canvas_under_test.inc"
#include "level_resource_under_test.inc"
#include "level_status_under_test.inc"
#include "level_init_under_test.inc"

// Unexecuted dependencies below are intentional traps, never successful mocks.
// The station constructor is the measured stopping point, before any player,
// collision geometry, StarSystem, or stage-1 work has been claimed to run.
struct StationBoundary {};
extern "C" void _ZN13PlayerStationC1EP7Station(void *, Station *station) {
    assert(station == Status::gStatus->getStation());
    throw StationBoundary{};
}

#define TRAP(symbol) extern "C" void symbol() { unexpected(#symbol); }
// Keep this list explicit: any newly linked dependency must be reviewed.
TRAP(_Z16String_GetAECharPN11AbyssEngine6StringE)
TRAP(_Z16String_printImplPKc)
TRAP(_ZN10AEGeometry11getPositionEv)
TRAP(_ZN10AEGeometry12setDirectionERKN11AbyssEngine6AEMath6VectorES4_)
TRAP(_ZN10AEGeometry12setLodMeshesEPtPii)
TRAP(_ZN10AEGeometry17setLodChildMeshesEPt)
TRAP(_ZN10AEGeometry8addChildEj)
TRAP(_ZN10AEGeometryC1EtPN11AbyssEngine11PaintCanvasEb)
TRAP(_ZN10LODManager9addObjectEP10AEGeometry)
TRAP(_ZN10StarSystem10getPlanetsEv)
TRAP(_ZN10StarSystemC1Ei)
TRAP(_ZN11AbyssEngine11PaintCanvas14MeshGetPointerEj)
TRAP(_ZN11AbyssEngine11PaintCanvas15TransformCreateERj)
TRAP(_ZN11AbyssEngine11PaintCanvas16TransformAddMeshEjtb)
TRAP(_ZN11AbyssEngine11PaintCanvas18TransformAddMeshIdEjj)
TRAP(_ZN11AbyssEngine11PaintCanvas21TransformGetTransformEj)
TRAP(_ZN11AbyssEngine18ApplicationManager9GetEngineEv)
TRAP(_ZN11AbyssEngine18computeFloatStringEfiPiS0_i)
TRAP(_ZN11AbyssEngine6AEMath17MatrixSetRotationERNS0_6MatrixEfff)
TRAP(_ZN11AbyssEngine6AEMath18MatrixRotateVectorERKNS0_6MatrixERKNS0_6VectorE)
TRAP(_ZN11AbyssEngine6AEMath6VectormLEf)
TRAP(_ZN11AbyssEngine6AEMathngERKNS0_6VectorE)
TRAP(_ZN11AbyssEngine6Engine21IsPostEffectActivatedEv)
TRAP(_ZN11AbyssEngine9Transform6UpdateExb)
TRAP(_ZN11AbyssEngineplERKNS_6StringES2_)
TRAP(_ZN11SolarSystem11getStationsEv)
TRAP(_ZN11SolarSystem23currentOrbitHasWarpGateEv)
TRAP(_ZN11SolarSystem7getRaceEv)
TRAP(_ZN14PlayerJumpgate22addJumpAnimationHandleEj)
TRAP(_ZN14PlayerJumpgateC1EiP10AEGeometryfffb)
TRAP(_ZN14PlayerWormHoleC1EiP10AEGeometryfffb)
TRAP(_ZN5Level10assignGunsEv)
TRAP(_ZN5Level11createSceneEv)
TRAP(_ZN5Level12createPlayerEv)
TRAP(_ZN5Level13createMissionEv)
TRAP(_ZN5Level13createWingmenEv)
TRAP(_ZN5Level14connectPlayersEv)
TRAP(_ZN5Level15createAsteroidsEv)
TRAP(_ZN5Level15createGasCloudsEv)
TRAP(_ZN5Level16createSentryGunsEv)
TRAP(_ZN5Level19createStaticObjectsEv)
TRAP(_ZN5Level20createFighterTurretsEv)
TRAP(_ZN5Level21createCampaignMissionEv)
TRAP(_ZN5RouteD1Ev)
TRAP(_ZN6Galaxy10getStationEi)
TRAP(_ZN6Status10getMissionEv)
TRAP(_ZN6Status10setMissionEP7Mission)
TRAP(_ZN6Status15getStationStackEv)
TRAP(_ZN6Status19inBlackMarketSystemEv)
TRAP(_ZN6Status7dlc1WonEv)
TRAP(_ZN6Status7gameWonEv)
TRAP(_ZN7Globals22addSoundResourceToListEi)
TRAP(_ZN7Mission17isCampaignMissionEv)
TRAP(_ZN7Mission7isEmptyEv)
TRAP(_ZN7Station7getNameEv)
TRAP(_ZN7StationD1Ev)
TRAP(_ZN8GameText11getLanguageEv)
TRAP(_ZN8KIPlayer9isWingManEv)
TRAP(_ZN8KIPlayer8setLevelEP5Level)
TRAP(_ZN9ObjectiveD1Ev)
TRAP(_ZN9PlayerEgo11setPositionEN11AbyssEngine6AEMath6VectorE)
TRAP(_ZN9PlayerEgo11setPositionEfff)
TRAP(_ZN9PlayerEgo8setRouteEP5Route)

static void checkManagers(const Level &level, PaintCanvas *canvas) {
    assert(level.lodManager && level.lodManager->objects);
    assert(level.lodManager->objects->size() == 0 && level.lodManager->timer == 1001);
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

int main(int argc, char **argv) {
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
    Status::gStatus = status;
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
        unexpected("Level::init returned past the station boundary");
    } catch (const StationBoundary &) {
        reached = true;
    }
    assert(reached && level->field_134 == 0 && !level->player && !level->starSystem);
    checkManagers(*level, &canvas);
    assert(level->landmarks && level->landmarks->size() == 4);
    for (auto *landmark : *level->landmarks) assert(!landmark);
    assert(level->field_08 == 0 && level->skyboxMesh == 1);
    assert(level->field_19c == 100 && level->field_198 == 101);
    assert(canvas.meshCount == 2 && opened == 4 && closed == 4);
    assert(collected == 0 && rangeCalls == 0); // Animation services are outside this gate.
    for (unsigned i = 0; i < canvas.meshCount; ++i) {
        auto *mesh = reinterpret_cast<Mesh *>(canvas.meshes[i]);
        assert(mesh && !mesh->animation && mesh->vertexCount && mesh->positions && mesh->boundsRadius > 0);
    }
    AERandom expected(14);
    const float rotations[] = {level->skyRotX, level->skyRotY, level->skyRotZ};
    for (float rotation : rotations)
        assert(std::fabs(rotation - float(expected.nextInt(65536)) / 65536.0f * 6.2832f) < 0.000001f);
    std::puts("Level ctor -> init stage-0 -> skybox assets -> station boundary: OK; "
              "2488 table + 2 low texture records, 10 managers, 2 AEM + 2 AEI, "
              "4/4 file lifetimes; init incomplete");
}
