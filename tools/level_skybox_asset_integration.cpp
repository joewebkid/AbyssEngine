#define GOF2_LEVEL_INTEGRATION
#include "mesh_create_smoke.cpp"
#include "engine/render/ResourceMesh.h"
#include "engine/render/ResourceTexture.h"
#include "level_string_length_under_test.inc"
char *String_GetAEChar(AbyssEngine::String *) { std::abort(); }

uint32_t AEFile::Skip(uint32_t bytes, uint32_t handle) {
    assert(handle == 1);
    if (cursor + bytes > limit) return 0;
    cursor += bytes;
    return bytes;
}

namespace AbyssEngine { void ImageRelease(Image **slot); }
#include "level_image_under_test.inc"

struct PCTexInfoView { char *path; float value; };
namespace AbyssEngine {
static unsigned int nextTexture = 100;
int TextureCreateFromFile(Engine *, const char *, ImageCallback, void *,
                          unsigned int *, bool, float) { std::abort(); }
int TextureCreateFromFileIntern(Engine *engine, const char *path, ImageCallback callback,
                                void *user, unsigned int *out, float,
                                AELoadedTexture *, bool) {
    Image *image = nullptr;
    if (ImageCreateFromFile(engine, path, &image) != 1) return -1;
    if (callback) callback(image, user);
    assert(image);
    if (std::strstr(path, "skybox_asteroid_belt_diffuse.aei"))
        assert(image->format == 3 && image->dataLen == 4U * 1024U * 1024U);
    else
        assert(image->format == 11);
    assert(image->width && image->height && image->data && image->dataLen);
    ImageRelease(&image);
    *out = nextTexture++;
    return 1;
}
bool Engine::IsPostEffectActivated() { return false; }
void Transform::Update(longlong, bool) { std::abort(); }
class ApplicationManager {
public:
    static ApplicationManager *gAppManager;
    Engine *engine;
    void *GetEngine() { return engine; }
};
ApplicationManager *ApplicationManager::gAppManager;
PaintCanvas *PaintCanvas::gCanvas;
}

#include "level_texture_canvas_under_test.inc"
#include "level_resource_under_test.inc"

struct SolarSystem {
    int getIndex() { return 4; }
    int getTextureIndex() { return 2; }
};
struct Station { int getIndex() { return 7; } };
struct Status {
    static Status *gStatus;
    SolarSystem system;
    Station station;
    int inAlienOrbit() { return 0; }
    SolarSystem *getSystem() { return &system; }
    int getCurrentCampaignMission() { return 1; }
    int inSupernovaSystem() { return 0; }
    int inPlanetRingOrbit() { return 0; }
    int inStormOrbit() { return 0; }
    int inFogSkyboxOrbit() { return 1; }
    Station *getStation() { return &station; }
};
Status *Status::gStatus;
struct AERandom {
    void setSeed(long long) { std::abort(); }
    int nextInt(int) { std::abort(); }
    void reset() { std::abort(); }
};
struct Globals { static void *rnd; static PaintCanvas *Canvas; };
void *Globals::rnd;
PaintCanvas *Globals::Canvas;

struct Level {
    int skyboxMesh = -1, field_08 = -1, skyboxTexture = -1, field_10 = -1;
    int field_14 = -1, field_18 = -1, field_198 = -1, field_19c = -1, field_1a0 = -1;
    int field_1b4 = -1, field_1b8 = -1, field_1bc = -1, field_1c0 = -1;
    int supernovaFlareMesh = -1, supernovaFlareTexture = -1, field_1c8 = -1;
    int missionPtr = 0;
    unsigned char supernovaFlareActive = 0;
    float skyRotX = -1, skyRotY = -1, skyRotZ = -1;
    void createSpace();
};

#include "level_skybox_under_test.inc"

int main(int argc, char **argv) {
    assert(argc == 2);
    assetBase = argv[1];
    openSucceeds = true;
    Engine::enableShader = 0;
    Engine::vboSupported = false;
    Engine *engine = static_cast<Engine *>(std::calloc(1, sizeof(Engine)));
    assert(engine);
    PaintCanvas canvas;
    canvas.engine = engine;
    Globals::Canvas = PaintCanvas::gCanvas = &canvas;
    AbyssEngine::ApplicationManager app{engine};
    AbyssEngine::ApplicationManager::gAppManager = &app;
    Status status;
    Status::gStatus = &status;

    registerSkyboxResources(&canvas);
    assert(canvas.resources.count == 6);
    auto *belt = static_cast<PCResourceView *>(canvas.FindResource(33410));
    assert(belt && belt->type == 2 && belt->handle == -1);
    auto *beltTexture = static_cast<AbyssEngine::ResourceTexture *>(belt->payload);
    assert(std::strcmp(beltTexture->name,
                       "data/assets/main/3d/textures/low/etc/skyboxes/skybox_asteroid_belt_diffuse.aei") == 0);
    assert(beltTexture->value == 0.0f);
    auto *tail = static_cast<PCResourceView *>(canvas.FindResource(14287));
    assert(tail && tail->type == 4 && tail->handle == -1);
    auto *tailMesh = static_cast<AbyssEngine::ResourceMesh *>(tail->payload);
    assert(std::strcmp(tailMesh->name,
                       "data/assets/main/3d/meshes/fx/explosion_void_station_alpha_lookat.aem") == 0);
    assert(tailMesh->wResourceId == 27302 && tailMesh->flag == 0);
    const unsigned short ids[] = {0x45bb, 0x2767, 0x458a, 0x2753};
    PCResourceView *resources[4];
    for (int i = 0; i < 4; ++i) {
        resources[i] = static_cast<PCResourceView *>(canvas.FindResource(ids[i]));
        assert(resources[i] && resources[i]->handle == -1);
        assert(resources[i]->type == (i % 2 == 0 ? 4 : 2));
    }
    const char *paths[] = {
        "data/assets/main/3d/meshes/skyboxes/skybox_stars.aem",
        "data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_001.aei",
        "data/assets/main/3d/meshes/skyboxes/skybox_002.aem",
        "data/assets/main/3d/textures/low/etc/skyboxes/skybox_002.aei",
    };
    for (int i = 0; i < 4; ++i) {
        const char *path = i % 2 == 0
            ? static_cast<AbyssEngine::ResourceMesh *>(resources[i]->payload)->name
            : static_cast<AbyssEngine::ResourceTexture *>(resources[i]->payload)->name;
        assert(std::strcmp(path, paths[i]) == 0);
    }
    assert(static_cast<AbyssEngine::ResourceMesh *>(resources[0]->payload)->wResourceId == 20010);
    assert(static_cast<AbyssEngine::ResourceMesh *>(resources[2]->payload)->wResourceId == 20011);
    assert(static_cast<AbyssEngine::ResourceTexture *>(resources[1]->payload)->value == -1.0f);
    assert(static_cast<AbyssEngine::ResourceTexture *>(resources[3]->payload)->value == -1.0f);

    Level level;
    level.createSpace();
    assert(level.field_08 == 0 && level.field_19c == 100);
    assert(level.skyboxMesh == 1 && level.field_198 == 101);
    assert(level.skyRotX == 0 && level.skyRotY == 0 && level.skyRotZ == 0);
    assert(canvas.meshCount == 2 && opened == 4 && closed == 4);
    const int handles[] = {0, 100, 1, 101};
    for (int i = 0; i < 4; ++i) assert(resources[i]->handle == handles[i]);
    level.createSpace();
    assert(canvas.meshCount == 2 && opened == 4 && closed == 4);
    unsigned int beltHandle = 0xffffffffU;
    canvas.TextureCreate(33410, beltHandle, false);
    assert(beltHandle == 102 && belt->handle == 102 && opened == 5 && closed == 5);
    canvas.TextureCreate(33410, beltHandle, false);
    assert(beltHandle == 102 && opened == 5 && closed == 5);
    unsigned int tailHandle = 0xffffffffU;
    const int collectedBeforeTail = collected;
    canvas.MeshCreate(14287, tailHandle, false);
    assert(tailHandle == 2 && tail->handle == 2 && canvas.meshCount == 3);
    assert(opened == 6 && closed == 6);
    assert(collected > collectedBeforeTail && canvas.meshes[2]->animation);

    for (unsigned i = 0; i < canvas.meshCount; ++i) dispose(canvas.meshes[i]);
    std::free(canvas.meshes);
    for (auto *resource : resources) {
        if (resource->type == 4) delete static_cast<AbyssEngine::ResourceMesh *>(resource->payload);
        else delete static_cast<AbyssEngine::ResourceTexture *>(resource->payload);
        delete resource;
    }
    delete beltTexture;
    delete belt;
    delete tailMesh;
    delete tail;
    std::free(engine);
}
