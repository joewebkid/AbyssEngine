#include "game/world/StarSystem.h"
#include "engine/math/Transform.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/AERandom.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"
#include "engine/render/AEGeometry.h"
#include "engine/file/FileRead.h"
#include "game/mission/Status.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/Engine.h"
#include "game/ship/PlayerStatic.h"
#include "engine/render/LensFlare.h"
#include "game/core/Globals.h"

int StarSystem::orbitPlanetIndex;


namespace AbyssEngine {
    namespace AEMath {
        Vector operator+(const Vector &lhs, const Vector &rhs);

        Vector operator*(const Vector &lhs, float rhs);
    }
}

void ArrayRelease_KIPlayer(void *array);

void *Array_KIPlayer_dtor(void *array);

static inline uint32_t flat_count(void *array) {
    return static_cast<Array<void *> *>(array)->size_;
}

static inline void *flat_data(void *array) {
    return static_cast<Array<void *> *>(array)->data_;
}

void MatrixGetPosition(Vector *out, void *matrix);

void MatrixGetUp(Vector *out, const Matrix *matrix);

void MatrixGetLookAt(Matrix *out, const Vector *from, const Vector *to, const Vector *up);

void MatrixSetScaling(Matrix *matrix, float x, float y, float z);

void StarSystem::switchSunForSupernovaExpansion() {
    AEGeometry *sun = (*this->planetsArray)[0];
    sun->setScaling(Vector{1.373291015625f, 1.373291015625f, 1.373291015625f});
}

void StarSystem::renderSunStreak() {
    uint32_t texture = (this->supernovaSystem != 0)
                           ? this->supernovaSunTexture
                           : (*this->texturesArray)[0];
    PaintCanvas::gCanvas->SetTexture(texture, (unsigned) -1);
    PaintCanvas::gCanvas->SetBlendMode(AbyssEngine::BlendMode_2);
    this->sunStreak->render();
}

void *StarSystem::getPlanetTargets() {
    return this->playerTargets;
}

void *StarSystem::getPlanets() {
    return this->planetsArray;
}

Array<int> *StarSystem::getStationIndices() {
    return this->stationIdxArray;
}

float StarSystem::getPlanetScaleFactor() {
    return this->planetRingScaleOffset;
}

void StarSystem::rotate(int x, int y, int z) {
}

Vector StarSystem::getLightDirection() {
    return this->lightDirection;
}

StarSystem::~StarSystem() {
    delete this->planetsArray;
    this->planetsArray = nullptr;

    if (this->playerTargets != nullptr) {
        ArrayRelease_KIPlayer(this->playerTargets);
        if (this->playerTargets != nullptr)
            ::operator delete(Array_KIPlayer_dtor(this->playerTargets));
    }
    this->playerTargets = nullptr;

    delete this->lensFlare;
    this->lensFlare = nullptr;

    delete this->texturesArray;
    this->texturesArray = nullptr;

    delete this->stationIdxArray;
    this->stationIdxArray = nullptr;

    delete this->positionsArray;
    this->positionsArray = nullptr;
}

typedef Engine *(*EngineGetter)(ApplicationManager *app);


static EngineGetter g_StarSystem_init_getEngine = nullptr;

// Extracted from Android libgof2hdaa.so StarSystem tables.
static const float g_StarSystem_init_sunColors[19 * 3] = {
    0.209999993f, 0.319999993f, 0.25f,
    0.5f, 0.289999992f, 0.280000001f,
    0.239999995f, 0.400000006f, 0.370000005f,
    0.219999999f, 0.319999993f, 0.430000007f,
    0.180000007f, 0.230000004f, 0.270000011f,
    0.379999995f, 0.129999995f, 0.109999999f,
    0.340000004f, 0.340000004f, 0.300000012f,
    0.239999995f, 0.25999999f, 0.25f,
    0.209999993f, 0.230000004f, 0.270000011f,
    0.430000007f, 0.469999999f, 0.469999999f,
    0.430000007f, 0.209999993f, 0.340000004f,
    0.850000024f, 0.409999996f, 0.140000001f,
    0.0799999982f, 0.239999995f, 0.479999989f,
    0.200000003f, 0.25f, 0.200000003f,
    0.300000012f, 0.349999994f, 0.150000006f,
    1.0f, 0.519999981f, 0.239999995f,
    0.389999986f, 0.479999989f, 0.439999998f,
    0.409999996f, 0.519999981f, 0.579999983f,
    0.670000017f, 0.629999995f, 0.460000008f,
};

static const float g_StarSystem_init_lightColors[19 * 3] = {
    0.209999993f, 0.319999993f, 0.25f,
    0.5f, 0.289999992f, 0.280000001f,
    0.239999995f, 0.400000006f, 0.370000005f,
    0.219999999f, 0.319999993f, 0.430000007f,
    0.180000007f, 0.230000004f, 0.270000011f,
    0.379999995f, 0.129999995f, 0.109999999f,
    0.340000004f, 0.340000004f, 0.300000012f,
    0.239999995f, 0.25999999f, 0.25f,
    0.209999993f, 0.230000004f, 0.270000011f,
    0.430000007f, 0.469999999f, 0.469999999f,
    0.430000007f, 0.209999993f, 0.340000004f,
    0.200000003f, 0.109999999f, 0.0399999991f,
    0.0799999982f, 0.239999995f, 0.479999989f,
    0.200000003f, 0.25f, 0.200000003f,
    0.300000012f, 0.349999994f, 0.150000006f,
    0.5f, 0.25999999f, 0.119999997f,
    0.389999986f, 0.479999989f, 0.439999998f,
    0.409999996f, 0.519999981f, 0.579999983f,
    0.670000017f, 0.629999995f, 0.460000008f,
};

static const float g_StarSystem_init_stationColors[27 * 3] = {
    0.289999992f, 0.349999994f, 0.310000002f,
    0.25f, 0.219999999f, 0.140000001f,
    0.200000003f, 0.170000002f, 0.0900000036f,
    0.300000012f, 0.270000011f, 0.310000002f,
    0.25f, 0.340000004f, 0.409999996f,
    0.389999986f, 0.165000007f, 0.0450000018f,
    0.370000005f, 0.430000007f, 0.25999999f,
    0.419999987f, 0.219999999f, 0.109999999f,
    0.180000007f, 0.25f, 0.280000001f,
    0.075000003f, 0.0649999976f, 0.0450000018f,
    0.200000003f, 0.430000007f, 0.349999994f,
    0.270000011f, 0.0900000036f, 0.0399999991f,
    0.379999995f, 0.289999992f, 0.159999996f,
    0.0599999987f, 0.0500000007f, 0.0299999993f,
    0.109999999f, 0.100000001f, 0.109999999f,
    0.25f, 0.230000004f, 0.200000003f,
    0.25f, 0.189999998f, 0.135000005f,
    0.170000002f, 0.349999994f, 0.479999989f,
    0.0700000003f, 0.0700000003f, 0.0700000003f,
    0.25999999f, 0.289999992f, 0.310000002f,
    0.129999995f, 0.100000001f, 0.0599999987f,
    0.310000002f, 0.310000002f, 0.209999993f,
    0.389999986f, 0.319999993f, 0.25f,
    0.639999986f, 0.5f, 0.670000017f,
    0.449999988f, 0.379999995f, 0.340000004f,
    0.479999989f, 0.519999981f, 0.579999983f,
    0.209999993f, 0.270000011f, 0.340000004f,
};

static inline uint32_t rgba_scaled(uint32_t color, float scale) {
    uint32_t r = (uint32_t)((float) ((color >> 24) & 0xff) * scale);
    uint32_t g = (uint32_t)((float) ((color >> 16) & 0xff) * scale);
    uint32_t b = (uint32_t)((float) ((color >> 8) & 0xff) * scale);
    return (r << 24) | (g << 16) | (b << 8) | 0xff;
}

void StarSystem::initLight() {
    Engine *engine = (Engine *) ApplicationManager::gAppManager->GetEngine();
    engine->field_0x32c = 2;

    Status *status = Status::gStatus;
    uint32_t baseIndex;
    bool normalSystem;
    if (this->abstractSystem == 0) {
        SolarSystem *system = (SolarSystem *) (intptr_t) status->getSystem();
        baseIndex = system->getTextureIndex() * 3;
        normalSystem = true;
    } else {
        baseIndex = 0x1e;
        normalSystem = false;
    }

    float ambientScale = 0.5f;
    this->sunLightColor.x = g_StarSystem_init_sunColors[baseIndex];
    this->sunLightColor.y = g_StarSystem_init_sunColors[baseIndex + 1];
    this->sunLightColor.z = g_StarSystem_init_sunColors[baseIndex + 2];

    if (normalSystem) {
        SolarSystem *system = (SolarSystem *) (intptr_t) status->getSystem();
        uint32_t tex = system->getTextureIndex();
        int mission = status->getCurrentCampaignMission();
        if (tex == 0x0f && (mission == 0x59 || mission > 0x9d)) {
            this->sunLightColor.x *= 0.5f;
            this->sunLightColor.y *= 0.5f;
            this->sunLightColor.z *= 0.5f;
        }
        if (this->abstractSystem == 0) {
            system = (SolarSystem *) (intptr_t) status->getSystem();
            tex = system->getTextureIndex();
            ambientScale = 0.5f;
            mission = status->getCurrentCampaignMission();
            if (tex == 0x0f && mission != 0x59) {
                ambientScale = mission < 0x9e ? 0.35f : 0.6f;
            }
        }
    }

    uint32_t stationColorIndex = 0x17;
    if (normalSystem && this->abstractSystem == 0) {
        Station *station = status->getStation();
        stationColorIndex = station->getTextureIndex() * 3;
    }

    float lr = g_StarSystem_init_stationColors[stationColorIndex];
    float lg = g_StarSystem_init_stationColors[stationColorIndex + 1];
    float lb = g_StarSystem_init_stationColors[stationColorIndex + 2];

    EngineGetter getEngine = g_StarSystem_init_getEngine;
    engine = getEngine(ApplicationManager::gAppManager);
    engine->LightSetGlobalSceneColorAmbient(ambientScale * this->sunLightColor.x,
                                            ambientScale * this->sunLightColor.y,
                                            ambientScale * this->sunLightColor.z);
    engine = getEngine(ApplicationManager::gAppManager);
    engine->LightSetRimColor(g_StarSystem_init_lightColors[baseIndex] * 3.0f,
                             g_StarSystem_init_lightColors[baseIndex + 1] * 3.0f,
                             g_StarSystem_init_lightColors[baseIndex + 2] * 3.0f);
    getEngine(ApplicationManager::gAppManager)->LightSetMaterialColorAmbient(1.0f, 1.0f, 1.0f);
    getEngine(ApplicationManager::gAppManager)->LightSetMaterialColorDiffuse(1.0f, 1.0f, 1.0f);
    getEngine(ApplicationManager::gAppManager)->LightSetMaterialColorSpecular(1.0f, 1.0f, 1.0f);
    getEngine(ApplicationManager::gAppManager)->LightSetMaterialColorShininess(0.7f);
    getEngine(ApplicationManager::gAppManager)->LightSetLightDirection(this->lightDirection.x, this->lightDirection.y,
                                                   this->lightDirection.z, 0x4000);
    getEngine(ApplicationManager::gAppManager)->LightSetLightColorAmbient(0.0f, 0.0f, 0.0f, 0x4000);
    getEngine(ApplicationManager::gAppManager)->LightSetLightColorDiffuse(this->sunLightColor.x, this->sunLightColor.y,
                                                      this->sunLightColor.z, 0x4000);
    getEngine(ApplicationManager::gAppManager)->LightSetLightColorSpecular(2.0f, 2.0f, 2.0f, 0x4000);
    getEngine(ApplicationManager::gAppManager)->LightSetLightDirection(0.0f, 0.0f, 1.0f, 0x4001);
    getEngine(ApplicationManager::gAppManager)->LightSetLightColorDiffuse(lr * 1.5f, lg * 1.5f, lb * 1.5f, 0x4001);
    getEngine(ApplicationManager::gAppManager)->LightSetLightColorSpecular(lr * 1.5f, lg * 1.5f, lb * 1.5f, 0x4001);
    getEngine(ApplicationManager::gAppManager)->LightSetParticleAmbient(this->sunLightColor.x, this->sunLightColor.y,
                                                    this->sunLightColor.z);

    PaintCanvas::gCanvas->FogEnable(0, AbyssEngine::FogMode_1);
    this->fogEnabled = 0;
    if (this->abstractSystem != 0) {
        return;
    }

    uint32_t tex = ((SolarSystem *) (intptr_t) status->getSystem())->getTextureIndex();
    uint32_t fogColor = 0;
    switch (tex) {
        case 0x0b:
            fogColor = 0x808080ff;
            break;
        case 0x0c:
            fogColor = 0xa0a0a0ff;
            break;
        case 0x0f:
            if (status->getCurrentCampaignMission() == 0x59) {
                return;
            }
            fogColor = 0x707070ff;
            PaintCanvas::gCanvas->FogSetParameter(AbyssEngine::FogMode_linear, 0.0f, 0.0f, 1.0f, fogColor);
            PaintCanvas::gCanvas->FogEnable(1, AbyssEngine::FogMode_1);
            this->fogEnabled = 1;
            for (uint32_t i = 1; i < this->planetsArray->size(); ++i) {
                AEGeometry *geom = (*this->planetsArray)[i];
                AbyssEngine::Transform *transform =
                        (AbyssEngine::Transform *) PaintCanvas::gCanvas->TransformGetTransform(geom->transform);

                transform->color = (int) fogColor;
            }
            return;
        case 0x10:
            fogColor = 0x909090ff;
            break;
        case 0x11:
            fogColor = 0x707090ff;
            break;
        case 0x12:
            fogColor = 0x909070ff;
            break;
        default:
            return;
    }

    PaintCanvas::gCanvas->FogSetParameter(AbyssEngine::FogMode_linear, 0.0f, 0.0f, 1.0f, fogColor);
    PaintCanvas::gCanvas->FogEnable(1, AbyssEngine::FogMode_1);
    this->fogEnabled = 1;
    uint32_t scaled = rgba_scaled(fogColor, 0.65f);
    for (uint32_t i = 1; i < this->planetsArray->size(); ++i) {
        AEGeometry *geom = (*this->planetsArray)[i];
        AbyssEngine::Transform *transform =
                (AbyssEngine::Transform *) PaintCanvas::gCanvas->TransformGetTransform(geom->transform);
        transform->color = (int) scaled;
    }
}

void StarSystem::render2D() {
    if (this->lensFlare == nullptr)
        return;

    PaintCanvas *canvas = Globals::Canvas;
    unsigned current = canvas->CameraGetCurrent();
    Matrix *local = (Matrix *) canvas->CameraGetLocal(current);
    Vector pos;
    pos.x = local->e3 + this->lightDirection.x * 65536.0f;
    pos.y = local->e7 + this->lightDirection.y * 65536.0f;
    pos.z = local->e11 + this->lightDirection.z * 65536.0f;
    this->lensFlare->update(0);
    if (canvas->GetScreenPosition(pos, pos))
        this->lensFlare->render2D(pos.x, pos.y, pos.z, this->tintColor);
}


// ARM rodata 0x202260/0x202334/0x2023a0: four-byte entries; consumers
// load their low halfword with index << 2 (e.g. 0x133ec6).
static const uint32_t g_StarSystem_ctor_systemPlanetTextures[19] = {
    0x272f, 0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735, 0x2736,
    0x2737, 0x2738, 0x2739, 0x2730, 0x2737, 0x2733, 0x272f, 0x2ddd,
    0x2736, 0x2731, 0x2735,
};

static const uint32_t g_StarSystem_ctor_stationPlanetTextures[27] = {
    0x273a, 0x273b, 0x273c, 0x273d, 0x273e, 0x273f, 0x2740, 0x2741,
    0x2742, 0x2743, 0x2744, 0x2745, 0x2746, 0x2747, 0x2748, 0x2749,
    0x274a, 0x274b, 0x274c, 0x274d, 0x2d68, 0x2d69, 0x2d6a, 0x2719,
    0x2e14, 0x2e15, 0x2e16,
};

static const uint32_t g_StarSystem_ctor_stationTextures[27] = {
    0x271b, 0x271c, 0x271d, 0x271e, 0x271f, 0x2720, 0x2721, 0x2722,
    0x2723, 0x2724, 0x2725, 0x2726, 0x2727, 0x2728, 0x2729, 0x272a,
    0x272b, 0x272c, 0x272d, 0x272e, 0x2d65, 0x2d66, 0x2d67, 0x2d67,
    0x2e08, 0x2e09, 0x2e0a,
};

static const uint32_t g_StarSystem_ctor_systemColors[34] = {
    0x00000003, 0x00000001, 0x00000003, 0x00000002, 0x00000000, 0x00000003,
    0x00000001, 0x00000004, 0x00000002, 0x00000004, 0x00000001, 0x00000003,
    0x00000001, 0x00000003, 0x00000002, 0x00000003, 0x00000003, 0x00000000,
    0x00000002, 0x00000001, 0x00000002, 0x00000003, 0x00000002, 0x00000000,
    0x00000003, 0x00000001, 0x00000002, 0x00000005, 0x00000003, 0x00000003,
    0x00000003, 0x00000003, 0x00000003, 0x00000003,
};

static const int g_StarSystem_ctor_sunSlotOverrides[14] = {
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 16,
};

StarSystem::StarSystem(int mode) {
    char usedSlots[0x60];
    for (unsigned i = 0; i < sizeof(usedSlots); ++i) {
        usedSlots[i] = 0;
    }

    this->lightDirection.x = 0.0f;
    this->lightDirection.y = 0.0f;
    this->lightDirection.z = 0.0f;
    this->stationIdxArray = nullptr;
    this->texturesArray = nullptr;
    this->playerTargets = nullptr;
    this->planetsArray = nullptr;

    this->abstractSystem = Globals::status->getSystem() == 0;
    this->supernovaSystem = (uint8_t) Globals::status->inSupernovaSystem();

    this->lensFlare = new LensFlare(Globals::Canvas);
    this->planetRing = nullptr;
    this->planetRingIndex = -1;

    if (this->abstractSystem != 0) {
        this->planetsArray = new Array<AEGeometry *>();
        ArraySetLength(2, *(this->planetsArray));

        AEGeometry *sun = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
        (*this->planetsArray)[0] = sun;
        sun->setScaling(Vector{1000.0f, 1000.0f, 1000.0f});
        sun->moveForward(0x447a0000);
        this->lightDirection = sun->getDirection();
        this->lightDirection.x = -this->lightDirection.x;
        this->lightDirection.y = -this->lightDirection.y;
        this->lightDirection.z = -this->lightDirection.z;

        AEGeometry *planet = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
        (*this->planetsArray)[1] = planet;
        int rnd = AERandom::gRandom->nextInt(0x4e20);
        float scale = (float) (rnd + 0x4e20) * 0.001f;
        planet->setScaling(Vector{scale, scale, scale});
        planet->setRotation(Vector{0.0f, 90.0f, 0.0f});
        planet->moveForward(0x447a0000);

        this->texturesArray = new Array<uint32_t>();
        ArraySetLength(2, *(this->texturesArray));
        Globals::Canvas->TextureCreate((uint16_t) 0x2739, (*this->texturesArray)[0], false);
        Globals::Canvas->TextureCreate((uint16_t) 0x2719, (*this->texturesArray)[1], false);

        this->positionsArray = new Array<Vector>();
        ArraySetLength(2, *(this->positionsArray));
        (*this->positionsArray)[0] = sun->getPosition();
        (*this->positionsArray)[1] = planet->getPosition();

        AEGeometry *streak = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
        this->sunStreak = streak;
        streak->setScaling(Vector{250.0f, 15.0f, 1000.0f});
        initLight();
        return;
    }

    SolarSystem *system = (SolarSystem *) (intptr_t) Globals::status->getSystem();
    uint32_t *stations = system->getStations();
    // ARM 0x133dfe-0x133e24: heap reader, a fresh system lookup, then
    // destruction before either StarSystem array is allocated.
    FileRead *reader = new FileRead();
    Array<Station *> *stationArray = reader->loadStationsBinary(Globals::status->getSystem());
    delete reader;

    this->texturesArray = new Array<uint32_t>();
    ArraySetLength(stationArray->size() + 1, *(this->texturesArray));

    this->stationIdxArray = new Array<int>();
    ArraySetLength(flat_count(stations), *(this->stationIdxArray));

    int mission = Globals::status->getCurrentCampaignMission();
    if ((mission == 0x59 && this->supernovaSystem != 0) ||
        (mission > 0x9d &&
         ((SolarSystem *) (intptr_t) Globals::status->getSystem())->getIndex() == 0x1b)) {
        Globals::Canvas->TextureCreate((uint16_t) 0x2dde, (*this->texturesArray)[0], false);
        this->tintColor = 3;
    } else {
        uint32_t tex = ((SolarSystem *) (intptr_t) Globals::status->getSystem())->getTextureIndex();
        Globals::Canvas->TextureCreate(g_StarSystem_ctor_systemPlanetTextures[tex],
                               (*this->texturesArray)[0], false);
        this->tintColor = g_StarSystem_ctor_systemColors[
            ((SolarSystem *) (intptr_t) Globals::status->getSystem())->getIndex()];
    }

    uint32_t count = flat_count(stations);
    int *stationIds = (int *) flat_data(stations);
    Station **loadedStations = stationArray->data();
    for (uint32_t i = 1; i < this->texturesArray->size(); ++i) {
        int stationIndex = stationIds[i - 1];
        int currentStation = Globals::status->getStation()->getIndex();
        Station *station = loadedStations[i - 1];
        if (stationIndex == currentStation) {
            // ARM 0x133f1c calls mission first; 0x133ff8 skips texture lookup.
            if (Globals::status->getCurrentCampaignMission() == 0 && mode == 3) {
                this->selectedStationSlot = i;
                Globals::Canvas->TextureCreate((uint16_t) 0x273b,
                                       (*this->texturesArray)[i], false);
            } else {
                uint32_t stationTex = station->getTextureIndex();
                Globals::Canvas->TextureCreate(g_StarSystem_ctor_stationPlanetTextures[stationTex],
                                       (*this->texturesArray)[i], false);
            }
        } else {
            uint32_t stationTex = station->getTextureIndex();
            Globals::Canvas->TextureCreate(g_StarSystem_ctor_stationTextures[stationTex],
                                   (*this->texturesArray)[i], false);
            if (Globals::status->orbitHasPlanetRing(station->getIndex()) != 0) {
                this->planetRing = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
                Globals::Canvas->TextureCreate((uint16_t) 0x7198,
                                       this->planetRingTexture, false);
                this->planetRingIndex = i;
            }
        }
        (*this->stationIdxArray)[i - 1] = station->getIndex();
    }

    // ARM 0x134038/0x13403e releases the Station pointers, then their Array.
    ArrayReleaseClasses(*stationArray);
    delete stationArray;

    // ARM 0x13404e/0x13405c uses Array<KIPlayer *>; its host size is not 12.
    this->playerTargets = new Array<KIPlayer *>();
    ArraySetLength(count, *static_cast<Array<KIPlayer *> *>(this->playerTargets));

    this->planetsArray = new Array<AEGeometry *>();
    ArraySetLength(count + 1, *(this->planetsArray));

    this->positionsArray = new Array<Vector>();
    ArraySetLength(count + 1, *(this->positionsArray));

    AERandom::gRandom->setSeed((long long) Globals::status->getStation()->getIndex() * 300);

    int sunSlot = AERandom::gRandom->nextInt(14) + 5;
    int stationSunSlotIndex = Globals::status->getStation()->getTextureIndex() - 9;
    if (stationSunSlotIndex >= 0 && stationSunSlotIndex < 14 &&
        (((0x3a31u >> stationSunSlotIndex) & 1u) != 0)) {
        sunSlot = g_StarSystem_ctor_sunSlotOverrides[stationSunSlotIndex];
    }
    PlayerStatic **targets = (PlayerStatic **) flat_data(this->playerTargets);
    for (uint32_t i = 0; i < this->planetsArray->size(); ++i) {
        AEGeometry *geom = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
        (*this->planetsArray)[i] = geom;

        if (i == 0) {
            float sunScale = this->supernovaSystem == 0 ? 1000.0f : 2000.0f;
            geom->setScaling(Vector{sunScale, sunScale, sunScale});
            AEGeometry *streak = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
            this->sunStreak = streak;
            streak->setScaling(Vector{
                this->supernovaSystem == 0 ? 250.0f : 500.0f,
                this->supernovaSystem == 0 ? 15.0f : 25.0f,
                this->supernovaSystem == 0 ? 1000.0f : 2000.0f
            });
            if (this->supernovaSystem != 0) {
                Globals::Canvas->TextureCreate((uint16_t) 0x2dde,
                                       this->supernovaSunTexture, false);
            }
            usedSlots[sunSlot * 4] = 1;
        } else {
            PlayerStatic *player = new PlayerStatic(0, geom, 0.0f, 0.0f, 0.0f);
            targets[i - 1] = player;
            int slot = AERandom::gRandom->nextInt(11) + 7;
            usedSlots[slot * 4] = 1;
            int dist = AERandom::gRandom->nextInt(0x4e20) + 0x4e20;
            if (Globals::status->getCurrentCampaignMission() == 0) {
                dist = (int) ((float) dist * 0.5f);
            }
            float scale = (float) dist * 0.001f;
            this->planetScale = scale;
            geom->setScaling(Vector{scale, scale, scale});
        }

        int angleSlot = (i == 0) ? sunSlot : (int) i + 6;
        geom->setRotation(Vector{0.0f, (float) (angleSlot * 0xaaa) * 0.001f, 0.0f});
        geom->moveForward(0x447a0000);
        if (i == 0) {
            this->lightDirection = geom->getDirection();
            this->lightDirection.x = -this->lightDirection.x;
            this->lightDirection.y = -this->lightDirection.y;
            this->lightDirection.z = -this->lightDirection.z;
        }
        (*this->positionsArray)[i] = geom->getPosition();
    }

    AERandom::gRandom->reset();
    initLight();
}

void StarSystem::updateSupernova(int dt) {
    AEGeometry *streak = this->sunStreak;
    if (streak != nullptr) {
        AbyssEngine::Transform *transform = (AbyssEngine::Transform *) PaintCanvas::gCanvas
                ->TransformGetTransform(streak->transform);
        transform->Update(0, dt);
    }
    AEGeometry *sun = (*this->planetsArray)[0];
    if (sun != nullptr) {
        AbyssEngine::Transform *transform = (AbyssEngine::Transform *) PaintCanvas::gCanvas
                ->TransformGetTransform(sun->transform);
        transform->Update(1, dt);
    }
}

void StarSystem::scaleSunDuringSupernovaIntro(int amount) {
    AEGeometry *sun = (*this->planetsArray)[0];
    float scale = sun->getScaling().x + (float) amount * 0.00004f;
    sun->setScaling(scale, scale, scale);
}

void StarSystem::switchPlanetForIntro() {
    PaintCanvas::gCanvas->TextureCreate((uint16_t) 0x273a,
                           (*this->texturesArray)[this->selectedStationSlot], false);
    AEGeometry *planet = (*this->planetsArray)[this->selectedStationSlot];
    planet->setScaling(planet->getScaling() * 2.0f);
}

void StarSystem::switchSunForSupernovaReversal() {
    PaintCanvas::gCanvas->TextureCreate((uint16_t) 0x2734,
                           (*this->texturesArray)[0], false);
    AEGeometry *sun = (*this->planetsArray)[0];
    sun->setScaling(Vector{0.2288818359375f, 0.2288818359375f, 0.2288818359375f});
}

static const uint32_t *g_StarSystem_intro_colors = g_StarSystem_ctor_systemColors;

void StarSystem::switchSunForSupernovaIntro() {
    Globals::Canvas->TextureCreate((uint16_t) 0x2df3, (*this->texturesArray)[0], false);
    Globals::Canvas->TextureCreate((uint16_t) 0x2df4, this->supernovaSunTexture, false);

    this->sunStreak->setMesh(0x2df2);
    this->sunStreak->setScaling(this->sunStreak->getScaling() * 5.0f);

    (*this->planetsArray)[0]->setMesh(0x2df1);
    (*this->planetsArray)[0]->setScaling(0.6866455078125f);

    auto *transform = static_cast<AbyssEngine::Transform *>(
        Globals::Canvas->TransformGetTransform((*this->planetsArray)[0]->transform));
    transform->SetAnimationState(AbyssEngine::AnimationMode_0, nullptr);
    transform = static_cast<AbyssEngine::Transform *>(
        Globals::Canvas->TransformGetTransform((*this->planetsArray)[0]->transform));
    transform->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
    transform = static_cast<AbyssEngine::Transform *>(
        Globals::Canvas->TransformGetTransform((*this->planetsArray)[0]->transform));
    transform->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);

    SolarSystem *system = (SolarSystem *) (intptr_t) Globals::status->getSystem();
    this->tintColor = g_StarSystem_intro_colors[system->getIndex()];
}


static uint32_t g_StarSystem_render_station_index_storage = 0;
static uint32_t *g_StarSystem_render_station_index =
    &g_StarSystem_render_station_index_storage;

void StarSystem::render() {
    Vector cameraPos;
    Matrix savedCamera;
    Matrix lookAt;
    Vector tempMatrix;
    Vector tempVec;
    Vector up;
    Vector scaleBytes;

    PaintCanvas *canvas = PaintCanvas::gCanvas;
    unsigned current = canvas->CameraGetCurrent();
    void *local = canvas->CameraGetLocal(current);
    MatrixGetPosition(&cameraPos, local);

    uint32_t count = this->planetsArray->size();
    AEGeometry **geoms = this->planetsArray->data();
    for (uint32_t i = 0; i < count; ++i) {
        if (count > 1) {
            if (i == 0) {
                unsigned current2 = canvas->CameraGetCurrent();
                void *local2 = canvas->CameraGetLocal(current2);
                savedCamera = *(const Matrix *) local2;
                tempVec = (*this->positionsArray)[0] + cameraPos;
                if (this->supernovaSystem == 0) {
                    MatrixGetUp(&up, &savedCamera);
                } else {
                    up.x = 1.0f;
                    up.y = 0.0f;
                    up.z = 0.0f;
                }
                MatrixGetLookAt(&lookAt, &tempVec, &cameraPos, &up);
                savedCamera = lookAt;
                geoms[0]->setRotation(0.0f, 0.0f, 0.0f);
                tempVec = geoms[0]->getScaling();

                float grow = (this->lensFlare->intensity - 10.0f) * 0.001f;
                if (grow < 0.0f) {
                    grow = 0.0f;
                }
                float sx = tempVec.x;
                float sy = tempVec.y;
                float sz = tempVec.z;
                if (this->supernovaSystem == 0) {
                    sx += grow;
                    sy += grow;
                }
                MatrixSetScaling(&lookAt, sx, sy, sz);
                geoms[0]->setMatrix(lookAt);

                if (this->supernovaSystem != 0) {
                    tempVec *= grow;
                    unsigned current3 = canvas->CameraGetCurrent();
                    void *local3 = canvas->CameraGetLocal(current3);
                    savedCamera = *(const Matrix *) local3;
                    up = (*this->positionsArray)[0] + cameraPos;
                    MatrixGetUp(&scaleBytes, &savedCamera);
                    MatrixGetLookAt(&lookAt, &up, &cameraPos, &scaleBytes);
                    savedCamera = lookAt;
                    MatrixSetScaling(&lookAt, grow + tempVec.x,
                                     tempVec.y,
                                     grow + tempVec.z);
                    this->sunStreak->setMatrix(lookAt);
                    renderSunStreak();
                }

                MatrixSetScaling(&lookAt,
                                 grow * (grow + tempVec.x + 1.0f),
                                 tempVec.y,
                                 tempVec.y / ((1.0f - grow) * 6.0f + 6.0f));
                this->sunStreak->setMatrix(lookAt);
            }

            uint32_t stationIndex = *g_StarSystem_render_station_index;
            bool selected = (i - 1) == stationIndex;
            if (selected && this->abstractSystem == 0 &&
                Status::gStatus->inPlanetRingOrbit() == 0) {
                float ring = cameraPos.z / 65536.0f;
                float clamped = ring < 0.0f ? ring : 0.0f;
                if (clamped < -1.0f) {
                    clamped = -1.0f;
                }
                this->planetRingScaleOffset = clamped;
                geoms[i]->setScaling(this->planetScale + clamped,
                                     this->planetScale + clamped,
                                     this->planetScale + clamped);
            }

            tempMatrix = (*this->positionsArray)[i] + cameraPos;
            geoms[i]->setPosition(tempMatrix);
        }

        canvas->SetTexture((*this->texturesArray)[i], (unsigned) -1);
        AbyssEngine::BlendMode blend;
        if (i == 0) {
            blend = AbyssEngine::BlendMode_2;
        } else {
            blend = this->fogEnabled == 0
                        ? AbyssEngine::BlendMode_1
                        : AbyssEngine::BlendMode_0x15;
        }
        canvas->SetBlendMode(blend);
        geoms[i]->render();

        if ((int) i == this->planetRingIndex) {
            canvas->SetTexture(this->planetRingTexture, (unsigned) -1);
            canvas->SetBlendMode(AbyssEngine::BlendMode_1);
            savedCamera = geoms[i]->getMatrix();
            MatrixSetScaling(&lookAt, 4.0f, 4.0f, 4.0f);
            this->planetRing->setMatrix(lookAt);
            this->planetRing->render();
        }
    }
}
