#include <cassert>
#include <cstdint>
#include <vector>

namespace AbyssEngine {
struct Mesh { int meshPostEffectFlag = 0; };
struct Transform {
    int updateTime = 0;
    void Update(int time, bool) { updateTime = time; }
};
}

struct SolarSystem {
    int index = 4;
    int textureIndex = 2;
    int getIndex() { return index; }
    int getTextureIndex() { return textureIndex; }
};
struct Station { int index = 7; int getIndex() { return index; } };
struct Status {
    static Status *gStatus;
    SolarSystem system;
    Station station;
    bool alien = false, supernova = false, ring = false, storm = false, fog = true;
    bool noStation = false;
    int mission = 1;
    int inAlienOrbit() { return alien; }
    SolarSystem *getSystem() { return &system; }
    int getCurrentCampaignMission() { return mission; }
    int inSupernovaSystem() { return supernova; }
    bool inPlanetRingOrbit() { return ring; }
    bool inStormOrbit() { return storm; }
    bool inFogSkyboxOrbit() { return fog; }
    Station *getStation() { return noStation ? nullptr : &station; }
};
Status *Status::gStatus;

struct AERandom {
    long long seed = 0;
    int calls = 0, resets = 0;
    void setSeed(long long value) { seed = value; }
    int nextInt(int bound) { assert(bound == 0x10000); return ++calls * 1000; }
    void reset() { ++resets; }
};
struct PaintCanvas;
struct Globals { static void *rnd; static PaintCanvas *Canvas; };
void *Globals::rnd;
PaintCanvas *Globals::Canvas;
struct Engine { bool active = false; bool IsPostEffectActivated() { return active; } };
struct ApplicationManager {
    static ApplicationManager *gAppManager;
    Engine engine;
    void *GetEngine() { return &engine; }
};
ApplicationManager *ApplicationManager::gAppManager;

struct PaintCanvas {
    static PaintCanvas *gCanvas;
    std::vector<int> meshes, textures, attachments;
    AbyssEngine::Mesh mesh;
    AbyssEngine::Transform transform;
    unsigned int nextTransform = 500;
    void MeshCreate(unsigned short id, unsigned int &out, bool) {
        meshes.push_back(id);
        out = 100000 + id;
    }
    void TextureCreate(unsigned short id, unsigned int &out, bool) {
        textures.push_back(id);
        out = 200000 + id;
    }
    void TransformCreate(unsigned int &out) { out = nextTransform++; }
    void TransformAddMeshId(unsigned int transformId, unsigned int meshId) {
        attachments.push_back(transformId);
        attachments.push_back(meshId);
    }
    void *TransformGetTransform(unsigned int) { return &transform; }
    AbyssEngine::Mesh *MeshGetPointer(unsigned int) { return &mesh; }
};
PaintCanvas *PaintCanvas::gCanvas;

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

int main() {
    Status status;
    Status::gStatus = &status;
    PaintCanvas canvas;
    PaintCanvas wrongCanvas;
    PaintCanvas::gCanvas = &wrongCanvas;
    Globals::Canvas = &canvas;
    ApplicationManager app;
    ApplicationManager::gAppManager = &app;
    AERandom random;
    Globals::rnd = &random;

    Level normal;
    normal.createSpace();
    assert(normal.field_08 == 100000 + 0x45bb);
    assert(normal.field_19c == 200000 + 0x2767);
    assert(normal.skyboxMesh == 100000 + 0x458a);
    assert(normal.field_198 == 200000 + 0x2753);
    assert(normal.skyRotX == 0 && random.calls == 0);

    const auto meshCount = canvas.meshes.size();
    normal.createSpace();
    assert(canvas.meshes.size() == meshCount && random.calls == 0);

    status.alien = true;
    Level alien;
    alien.createSpace();
    assert(alien.field_08 == 100000 + 0x45bc);
    assert(alien.field_19c == 200000 + 0x2768);
    assert(alien.skyboxMesh == 100000 + 0x4592);
    assert(alien.field_198 == 200000 + 0x275b);

    status.alien = false;
    status.mission = 0;
    status.fog = false;
    Level intro;
    intro.missionPtr = 3;
    intro.createSpace();
    assert(intro.skyboxMesh == 100000 + 0x458b);
    assert(intro.field_198 == 200000 + 0x2754);
    assert(random.seed == 14 && random.calls == 3 && random.resets == 1);
    assert(intro.skyRotX > 0 && intro.skyRotY > intro.skyRotX && intro.skyRotZ > intro.skyRotY);

    status.noStation = true;
    Level noStation;
    noStation.createSpace();
    assert(random.seed == -1 && random.calls == 6 && random.resets == 2);
    status.noStation = false;

    status.mission = 106;
    status.supernova = status.ring = status.storm = status.fog = true;
    status.system.index = 24;
    status.system.textureIndex = 16;
    app.engine.active = true;
    Level special;
    special.createSpace();
    assert(special.skyboxMesh == 100000 + 0x4597);
    assert(special.field_198 == 200000 + 0x2760);
    assert(special.skyboxTexture == 100000 + 0x45a0);
    assert(special.field_14 == 100000 + 0x45a1);
    assert(special.field_1a0 == 200000 + 0x2765);
    assert(special.field_10 == 500 && special.field_18 == 501);
    assert(special.supernovaFlareActive && special.supernovaFlareMesh == 100000 + 0x3827);
    assert(special.supernovaFlareTexture == 200000 + 0x8282 && special.field_1c8 == 200000 + 0x8283);
    assert(special.field_1b4 == 502 && special.field_1b8 == 200000 + 0x715a);
    assert(special.field_1bc == 503 && special.field_1c0 == 200000 + 0x715b);
    assert(canvas.attachments.size() == 8 && canvas.transform.updateTime == 1000);
    assert(canvas.mesh.meshPostEffectFlag == 1046562734);

    status.mission = 89;
    status.ring = status.storm = false;
    Level mission89;
    mission89.createSpace();
    assert(mission89.skyboxMesh == 100000 + 0x458d);
    assert(mission89.field_198 == 200000 + 0x2756);
    assert(mission89.skyboxTexture == -1);

    status.mission = 158;
    Level lateSupernova;
    lateSupernova.createSpace();
    assert(lateSupernova.skyboxMesh == 100000 + 0x4597);
    assert(lateSupernova.skyboxTexture == -1);
}
