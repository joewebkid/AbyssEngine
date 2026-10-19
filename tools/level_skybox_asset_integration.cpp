#define GOF2_LEVEL_INTEGRATION
#include "mesh_create_smoke.cpp"

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
    assert(image && image->format == 11);
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

    PCMeshInfoView meshes[2] = {
        {const_cast<char *>("data/assets/main/3d/meshes/skyboxes/skybox_stars.aem"), 20010},
        {const_cast<char *>("data/assets/main/3d/meshes/skyboxes/skybox_002.aem"), 20011},
    };
    PCTexInfoView textures[2] = {
        {const_cast<char *>("data/assets/main/3d/textures/low/etc/skyboxes/skybox_stars_001.aei"), -1.0f},
        {const_cast<char *>("data/assets/main/3d/textures/low/etc/skyboxes/skybox_002.aei"), -1.0f},
    };
    PCResourceView resources[4] = {
        {0x45bb, {0, 0}, 4, -1, &meshes[0]},
        {0x2767, {0, 0}, 2, -1, &textures[0]},
        {0x458a, {0, 0}, 4, -1, &meshes[1]},
        {0x2753, {0, 0}, 2, -1, &textures[1]},
    };
    for (auto &resource : resources) canvas.AddResource(&resource);

    Level level;
    level.createSpace();
    assert(level.field_08 == 0 && level.field_19c == 100);
    assert(level.skyboxMesh == 1 && level.field_198 == 101);
    assert(level.skyRotX == 0 && level.skyRotY == 0 && level.skyRotZ == 0);
    assert(canvas.meshCount == 2 && opened == 4 && closed == 4);
    const int handles[] = {0, 100, 1, 101};
    for (int i = 0; i < 4; ++i) assert(resources[i].handle == handles[i]);
    level.createSpace();
    assert(canvas.meshCount == 2 && opened == 4 && closed == 4);

    for (unsigned i = 0; i < canvas.meshCount; ++i) dispose(canvas.meshes[i]);
    std::free(canvas.meshes);
    std::free(engine);
}
