#include "game/world/Level.h"

float Level::r;
float Level::g;
float Level::b;

#include "game/mission/Mission.h"
#include "game/mission/Generator.h"
#include "game/ship/Ship.h"
#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"
#include "game/mission/Achievements.h"
#include "game/ui/Hud.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "game/weapons/Gun.h"
#include "game/ship/PlayerTurret.h"
#include "game/world/Waypoint.h"
#include "game/world/SolarSystem.h"
#include "game/world/Route.h"
#include "game/mission/Objective.h"
#include "game/mission/Item.h"
#include "engine/core/GameText.h"
#include "game/world/Station.h"
#include "engine/render/LODManager.h"
#include "game/world/Wanted.h"
#include "game/world/Standing.h"
#include "game/mission/Status.h"
#include "game/ship/KIPlayer.h"
#include "game/weapons/ObjectGun.h"
#include "game/core/RadioMessage.h"
#include "engine/render/ParticleSystemManager.h"
#include "engine/render/ParticleSettingsRef.h"
#include "game/world/StarSystem.h"
#include "engine/core/AbyssEngine.h"
#include "engine/core/AERandom.h"
#include "engine/core/ApplicationManager.h"
#include "engine/file/FileRead.h"
#include "engine/math/AEMath.h"
#include "engine/math/Matrix.h"
#include "engine/math/BoundingSphere.h"
#include "engine/math/BoundingAAB.h"
#include "game/core/Globals.h"
#include "game/ship/PlayerAsteroid.h"
#include "game/ship/PlayerFighter.h"
#include "game/ship/PlayerGasCloud.h"
#include "game/ship/PlayerStatic.h"
#include "game/ship/PlayerStation.h"
#include "game/ship/PlayerJumpgate.h"
#include "game/ship/PlayerWormHole.h"
#include "game/ship/PlayerJunk.h"
#include "game/weapons/RocketGun.h"
#include "game/world/Galaxy.h"

#include "game/ship/PlayerFixedObject.h"
#include <new>
#include "game/ship/Agent.h"
#include "engine/render/Engine.h"
#include "game/weapons/Radar.h"
#include "game/weapons/BeamGun.h"
#include "game/weapons/BombGun.h"
#include "game/weapons/MineGun.h"
#include "game/weapons/SentryGun.h"
#include "engine/math/Transform.h"
#include "engine/render/Mesh.h"
#include "game/core/GameSettings.h"
#include "game/mission/DifficultyRecord.h"
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace {

constexpr unsigned int kFirstEngineParticleSet = 0x1d;

// Android rodata: 0x1fc3d0 (one 1-based atlas variant per ship index).
constexpr uint8_t kEngineParticleVariantByShip[64] = {
    3, 0, 8, 3, 2, 0, 3, 0, 9, 1, 0, 8, 2, 0, 0, 0,
    2, 0, 2, 3, 3, 2, 0, 8, 8, 8, 0, 0, 0, 8, 3, 2,
    8, 0, 0, 2, 0, 0, 0, 1, 0, 1, 1, 2, 1, 3, 3, 3,
    3, 1, 1, 0, 8, 1, 1, 0, 3, 2, 0, 0, 8, 1, 3, 1,
};

// Android rodata: 0x1fdba0, 0x1fdbd0, 0x1fdc00, 0x1fdc30.
constexpr float kEngineParticleUvU0[9] = {
    0.376953125f, 0.12890625f, 0.005859375f, 0.251953125f, 0.251953125f,
    0.251953125f, 0.251953125f, 0.501953125f, 0.626953125f,
};
constexpr float kEngineParticleUvV0[9] = {
    0.001953125f, 0.005859375f, 0.005859375f, 0.001953125f, 0.001953125f,
    0.001953125f, 0.001953125f, 0.001953125f, 0.001953125f,
};
constexpr float kEngineParticleUvU1[9] = {
    0.498046875f, 0.24609375f, 0.119140625f, 0.373046875f, 0.373046875f,
    0.373046875f, 0.373046875f, 0.623046875f, 0.748046875f,
};
constexpr float kEngineParticleUvV1[9] = {
    0.123046875f, 0.12109375f, 0.119140625f, 0.123046875f, 0.123046875f,
    0.123046875f, 0.123046875f, 0.123046875f, 0.123046875f,
};

// Android rodata: 0x1fc4d0. Indexed directly by SolarSystem::getTextureIndex().
constexpr uint32_t kSystemParticleTintByTexture[19] = {
    0x00a020ffu, 0x8c1402ffu, 0x006d7bffu, 0x2c9bd8ffu, 0x7a7a7affu,
    0xce5a46ffu, 0x69a07dffu, 0xa7a07effu, 0x7eb59fffu, 0xcedee1ffu,
    0x9274d4ffu, 0xdb6923ffu, 0xffffffffu, 0x9ae4ffffu, 0xaeb77dffu,
    0xdb6923ffu, 0x47665effu, 0x738d95ffu, 0xaba075ffu,
};

ParticleSettings::SetDefinition *levelCurrentParticleSets() {
    return ParticleSettingsRef::cur.sets;
}

// Android Level::createScene(mode 23) rodata. These tables assemble the
// station hangar, not the separate ListItemWindow ship-preview renderer.
constexpr int kHangarShipVerticalOffsets[64] = {
    160, 150, 220, 310, 160, 180, 140, 170, 170, 110, 250, 205, 240, 250, 250, 250,
    170, 170, 190, 460, 170, 210, 140, 170, 160, 190, 170, 240, 220, 280, 200, 200,
    140, 160, 180, 150, 310, 390, 480, 200, 300, 300, 170, 200, 170, 370, 370, 300,
    440, 340, 170, 470, 170, 170, 170, 170, 200, 170, 250, 150, 170, 250, 270, 170,
};

constexpr int kHangarBaseMeshIds[9][4] = {
    {14376, 14302, 14301, -1},
    {14377, 14304, -1, -1},
    {14378, 14308, 14307, -1},
    {14379, 14310, 14264, -1},
    {-1, -1, -1, -1},
    {-1, -1, -1, -1},
    {-1, -1, -1, -1},
    {14362, 14358, 14360, 14359},
    {14354, 14355, 14357, 14356},
};

constexpr int kHangarVariantChildCounts[4] = {11, 0, 10, 12};
constexpr int kHangarVariantChildStarts[4] = {14380, -1, 14403, 14391};
constexpr int kHangarSeatCounts[9] = {2, 9, 2, 3, 0, 0, 0, 3, 0};

struct HangarSeat {
    int x;
    int y;
    int z;
};

constexpr HangarSeat kHangarDefaultSeats[2] = {
    {4096, 0, 0},
    {4096, 0, 4096},
};

constexpr HangarSeat kHangarTerranSeats[2] = {
    {0, 0, 2891},
    {0, 0, 5781},
};

constexpr HangarSeat kHangarVosskSeats[9] = {
    {-3715, 0, 661}, {-6983, 36, 2548}, {-9408, 36, 5438},
    {-9408, 36, 16301}, {-6983, 36, 19191}, {-3715, 36, 21078},
    {-9408, 5359, 16301}, {-6983, 5359, 19191}, {-3715, 5359, 21078},
};

constexpr HangarSeat kHangarMidorianSeats[3] = {
    {-4096, 0, 0}, {0, 0, 4096}, {-4096, 0, 4096},
};

constexpr HangarSeat kHangarDeepScienceSeats[3] = {
    {-1961, 0, 8512}, {-1961, 0, 5562}, {-1961, 0, 2610},
};

const HangarSeat *levelGetHangarSeats(int race) {
    switch (race) {
    case 0:
        return kHangarTerranSeats;
    case 1:
        return kHangarVosskSeats;
    case 3:
        return kHangarMidorianSeats;
    case 7:
        return kHangarDeepScienceSeats;
    default:
        return kHangarDefaultSeats;
    }
}

} // anonymous namespace

Matrix *CameraGetLocal(void *canvas, uint32_t index);


// Android 2.0.16 .rodata at 0x1fc54c and 0x1fc8f0, 233 item IDs each.
static const int g_cg_meshTable[233] = {
    6754, 6755, 6756, 6760, 6761, 6762, 6763, 6764, 6765, -1, -1, -1,
    6788, 6789, 6790, 6791, 6792, 6793, 6794, 6795, 6796, 6797, 6798, 6799,
    6800, 6801, 6802, 6803, 14236, 14237, 14238, 14247, 14247, 14247, 14247, 14247,
    14249, 14249, 14249, 14249, 14249, 14684, 14684, 14684, 14680, 14680, 14682, 6792,
    6797, 6789, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    14050, 14052, 14054, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, 6900, 6901, 6902, 14293,
    6905, 6906, 6799, 14297, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 14239, 14235, -1, -1, 14247, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, 6788, 6788, 6788, 14247, 14247,
    14247, -1, -1, -1, -1, 14247, -1, -1, 6796, -1, -1, -1,
    -1, 19091, 6803, 19094, 27338
};
static const int g_cg_soundTable[233] = {
    52, 53, 54, 56, 55, 57, 58, 59, 60, 50, 49, 50,
    78, 75, 79, 77, 72, 73, 74, 76, 80, 81, 66, 67,
    63, 64, 68, 65, 69, 70, 71, 82, 83, 84, 85, 86,
    87, 88, 89, 90, 91, 6, 7, 8, 9, 10, 11, 92,
    93, 94, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    1098, 1100, 1099, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, 1111, 1113, 1112, 1117,
    1095, 1096, 1097, 1101, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 1118, -1, -1, -1, 2254, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 2271, 2272, -1, -1, 2263, 2263, 2263, 2265, 2265,
    2265, -1, -1, -1, -1, 2254, 2267, 2268, 2252, -1, 2269, -1,
    2275, 2276, 2278, 2277, 2280
};

static int *g_crm_counts8 = nullptr;

static int *g_crm_table8 = nullptr;

static float g_up_eqMax = 0;

static int **g_cso_textA = nullptr;

static int **g_cso_textB = nullptr;

static float g_cso_posX = 0;

static float g_cso_posZ = 0;


static int *g_ag_diffRec = nullptr;

static int **g_ag_shipTbl = nullptr;

static int **g_ag_itemTblA = nullptr;

static int *g_ag_weaponDmg = nullptr;

static int **g_ag_statusB = nullptr;

static int **g_ag_alienCnt = nullptr;

static int **g_ag_itemTblB = nullptr;

static float g_ag_perLevel = 0;

static int *g_ips_envA = nullptr;

static int *g_ips_envB = nullptr;

static int **g_cwm_seedSrc = nullptr;

static inline float bitsToFloat(int bits) {
    float f;
    __builtin_memcpy(&f, &bits, sizeof(f));
    return f;
}

struct PlayerEgoVtable {
    void (*slot[7])();
    void (*placeAtStation)(void *self, int stationStack);
};

struct PlayerEgoPolymorphic {
    PlayerEgoVtable *vtable;
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PlayerEgoVtable, placeAtStation) == 0x1c,
              "PlayerEgo vtable placeAtStation slot offset");
#endif

static inline void levelSpawnFar(Level *self, int *kiPlayer);

static inline void levelPlaceAlien(Level *self, int *kiPlayer, int alienInOrbit);

static inline void levelPlaceWingman(Level *self, int *kiSlot, unsigned i);

bool Level::hasMiningPlant() {
    return miningPlant > 0;
}

Route *Level::getFriendRoute() {
    return friendRoute;
}

int Level::getEnemiesLeft() {
    return enemiesLeft;
}

void Level::render2D() {
    if (starSystem != 0)
        starSystem->render2D();
}

int Level::checkGameOver(int param) {
    Objective *objective = objectivesB;
    if (objective == nullptr) {
        return 0;
    }

    return (int) objective->achieved(param);
}

void Level::updateAsteroidCluster() {
}

Waypoint *Level::getAsteroidWaypoint() {
    return asteroidWaypoint;
}

int Level::getAsteroidsLeft() {
    return asteroidsLeft;
}

void Level::junkDied() {
    Status::gStatus->field_b0 += 1;
    enemiesLeft -= 1;
}

void Level::enableMovingStars(bool enable) {
    int index = movingStarsIndex;
    if (index < 0) {
        return;
    }

    (skybox2Mesh)->enableSystemRender(index, enable);
}

void Level::setInitStreamOut() {
    initStreamOutPosition = 1;
}

int Level::getMiningPlant() {
    int index = miningPlantIndex;
    if (index < 0) {
        return 0;
    }
    return (int) (intptr_t)(*this->enemies)[index];
}

Array<KIPlayer *> *Level::getGasClouds() {
    return gasClouds;
}

void Level::asteroidDied() {
    asteroidsLeft -= 1;
}

int Level::checkObjective(int param) {
    Objective *objective = objectivesA;
    if (objective != nullptr) {
        return (int) objective->achieved(param);
    }
    return 0;
}

int Level::getNumDeliveredOre() {
    return numDeliveredOre;
}

void Level::setPlayerRoute(Route *route) {
    Route *old = playerRoute;
    if (old != nullptr) {
        old->~Route();
        operator delete(old);
    }
    playerRoute = route;
}

void Level::enableFog(bool enable) {
    particleSystemMgr->enableSystemRender(field_284, enable);
}

int Level::isInAsteroidCenterRange(Vector v) {
    if (collisionVolume != nullptr) {
        return collisionVolume->collide(v.x, v.y, v.z);
    }
    return 0;
}

Array<KIPlayer *> *Level::getAsteroids() {
    return asteroids;
}

int Level::collide(Vector v, bool /*param*/) {
    if (collisionVolume != nullptr) {
        return collisionVolume->collide(v.x, v.y, v.z);
    }
    return 0;
}

int Level::getFriendsLeft() {
    return friendsLeft;
}

void Level::incNumDeliveredOre(int delta) {
    numDeliveredOre += delta;
}

void Level::enableParticleEffects(bool emit, bool render) {
    particleSystemMgr->enableSystemEmit(field_284, render);

    particleSystemMgr->enableSystemRender(field_284, render);
    *(unsigned char *) particleRenderBoolPtr = render;
    *(unsigned char *) particleEmitBoolPtr = render;
}

void Level::switchSkyboxForIntro() {
    PaintCanvas::gCanvas->MeshCreate(0x4591, reinterpret_cast<unsigned int &>(skyboxMesh), false);
    PaintCanvas::gCanvas->TextureCreate(0x275a, reinterpret_cast<unsigned int &>(field_198), false);
    if (this->asteroids != nullptr) {
        for (unsigned int i = 0; i < this->asteroids->size(); i = i + 1) {
            (*this->asteroids)[i]->setDead();
        }
    }
}

void Level::switchSkyboxForSupernovaReversal() {
    int tex = ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getTextureIndex();
    PaintCanvas::gCanvas->MeshCreate((unsigned short) (tex + 0x4588),
                                    reinterpret_cast<unsigned int &>(skyboxMesh), false);
    int tex2 = ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getTextureIndex();
    PaintCanvas::gCanvas->TextureCreate((unsigned short) (tex2 + 0x2751),
                                       reinterpret_cast<unsigned int &>(field_198), false);
    skyboxTexture = -1;
}

PlayerEgo *Level::getPlayer() {
    return player;
}

void Level::killWanted(int /*param*/) {
    if (field_29d == 0) {
        field_29d = 1;

        createRadioMessage(0x11, 0);
    }
}

Array<AbstractGun *> *Level::getEnemyGuns() {
    return enemyGuns;
}

void Level::stealFriendCargo() {
    friendCargoStolen = 1;
}

Array<KIPlayer *> *Level::getEnemies() {
    return enemies;
}

void Level::applyKills() {
    if (Status::gStatus->getMission() != 0) {
        Status::gStatus->addKills(kills);
        kills = 0;
    }
}

uint8_t Level::friendCargoWasStolen() {
    return friendCargoStolen;
}

Array<void *> *Level::getMessages() {
    return (Array<void *> *) messages;
}

Route *Level::getEnemyRoute() {
    return enemyRoute;
}

Array<AbstractGun *> *Level::getPlayerGuns() {
    return playerGuns;
}

void Level::renderPause(long long /*ctx*/) {
    if (this->playerGuns != nullptr) {
        for (unsigned int i = 0; i < this->playerGuns->size(); i = i + 1) {
            ((ObjectGun *) (*this->playerGuns)[i])->render();
        }
    }
    if (this->enemyGuns != nullptr) {
        for (unsigned int i = 0; i < this->enemyGuns->size(); i = i + 1) {
            ((ObjectGun *) (*this->enemyGuns)[i])->render();
        }
    }
    if (this->enemies != nullptr) {
        for (unsigned int i = 0; i < this->enemies->size(); i = i + 1) {
            (*this->enemies)[i]->render();
        }
    }
    if (this->asteroids != nullptr) {
        for (unsigned int i = 0; i < this->asteroids->size(); i = i + 1) {
            (*this->asteroids)[i]->render();
        }
    }
    if (this->gasClouds != nullptr) {
        for (unsigned int i = 0; i < this->gasClouds->size(); i = i + 1) {
            (*this->gasClouds)[i]->render();
        }
    }
    if (this->landmarks != nullptr) {
        for (unsigned int i = 0; i < this->landmarks->size(); i = i + 1) {
            KIPlayer *o = (*this->landmarks)[i];
            if (o != nullptr) {
                o->render();
            }
        }
    }
}

Route *Level::getPlayerRoute() {
    return playerRoute;
}

StarSystem *Level::getStarSystem() {
    return starSystem;
}

void Level::pirateStationAction(bool param) {
    if (param) {
        if (field_1b0 != 0) {
            return;
        }
    } else {
        if (field_68 != 0) {
            return;
        }
        if (Status::gStatus->getStation()->getPirateStationIndex() < 0) {
            return;
        }
        int tbl = (int) (intptr_t) Status::gStatus->field_4c;
        int idx = Status::gStatus->getStation()->getPirateStationIndex();
        *(unsigned char *) (*(int *) (tbl + 4) + idx) = 1;

        *((unsigned char *) &Status::gStatus->field_f8 + 1) = 1;
    }

    createRadioMessage(param ? 3 : 4, 8);
}

int Level::getNumDockingTargets() {
    if (this->enemies != nullptr) {
        int total = 0;
        for (unsigned int i = 0; i < this->enemies->size(); i = i + 1) {
            KIPlayer *e = (*this->enemies)[i];
            total = total + (uint8_t) e->field_0x70;
        }
        return total;
    }
    return 0;
}

void Level::removeObjectives() {
    objectivesA = nullptr;
    objectivesB = nullptr;
}

int Level::getDockingTarget(int index) {
    if (this->enemies != nullptr) {
        int found = 0;
        for (unsigned int i = 0; i < this->enemies->size(); i = i + 1) {
            KIPlayer *obj = (*this->enemies)[i];
            if ((char) obj->field_0x70 != 0) {
                if (found == index) {
                    return (int) (intptr_t) obj;
                }
                found = found + 1;
            }
        }
    }
    return 0;
}

int Level::getTimeLimit() {
    return timeLimit;
}

Array<KIPlayer *> *Level::getLandmarks() {
    return landmarks;
}

static inline void actorPreRender(KIPlayer *o, int ctx) {
    o->update(ctx);
}

void Level::render(int ctx) {
    if (this->playerGuns != nullptr) {
        for (unsigned int i = 0; i < this->playerGuns->size(); i = i + 1) {
            ((ObjectGun *) (*this->playerGuns)[i])->render();
        }
    }
    if (this->enemyGuns != nullptr) {
        for (unsigned int i = 0; i < this->enemyGuns->size(); i = i + 1) {
            ((ObjectGun *) (*this->enemyGuns)[i])->render();
        }
    }
    if (this->enemies != nullptr) {
        for (unsigned int i = 0; i < this->enemies->size(); i = i + 1) {
            KIPlayer *o = (*this->enemies)[i];
            actorPreRender(o, ctx);
            o->render();
        }
    }
    if (this->asteroids != nullptr) {
        for (unsigned int i = 0; i < this->asteroids->size(); i = i + 1) {
            KIPlayer *o = (*this->asteroids)[i];
            actorPreRender(o, ctx);
            o->render();
        }
    }
    if (this->gasClouds != nullptr) {
        for (unsigned int i = 0; i < this->gasClouds->size(); i = i + 1) {
            KIPlayer *o = (*this->gasClouds)[i];
            actorPreRender(o, ctx);
            o->render();
        }
    }
    if (this->landmarks != nullptr) {
        for (unsigned int i = 0; i < this->landmarks->size(); i = i + 1) {
            KIPlayer *o = (*this->landmarks)[i];
            if (o != nullptr) {
                actorPreRender(o, ctx);
                o->render();
            }
        }
    }
    if (field_80 != 0) {
        field_80->render3d();
    }
    if (field_74 != 0) {
        field_74->render3d();
    }
    if (particleEmitBoolPtr != nullptr) {
        particleEmitBoolPtr->render3d();
    }
    if (particleSystemMgr != nullptr) {
        particleSystemMgr->render3d();
    }
    if (skybox2Mesh != 0) {
        skybox2Mesh->render3d();
    }
    if (particleRenderBoolPtr != nullptr) {
        particleRenderBoolPtr->render3d();
    }
    if (field_8c != 0) {
        field_8c->render3d();
    }
    if (field_98 != 0) {
        field_98->render3d();
    }
    if (field_94 != 0) {
        field_94->render3d();
    }
    if (field_9c != 0) {
        field_9c->render3d();
    }
    if (starSystem != 0)
        starSystem->renderSunStreak();
}

int Level::collideStream(Vector v) {
    PlayerFixedObject *obj = (PlayerFixedObject *) (*this->landmarks)[1];
    if (obj != nullptr) {
        return obj->collide(v.x, v.y, v.z);
    }
    return 0;
}

int Level::getNumDeliveredPassengers() {
    return numDeliveredPassengers;
}

Level::~Level() {
    skyboxMesh = -1;
    field_08 = -1;
    skyboxTexture = -1;
    delete objectivesA;
    objectivesA = nullptr;
    delete objectivesB;
    objectivesB = nullptr;
    delete collisionVolume;
    collisionVolume = nullptr;
    delete asteroidWaypoint;
    asteroidWaypoint = nullptr;
    delete starSystem;
    starSystem = nullptr;
    delete player;
    player = nullptr;
    delete field_180;
    field_180 = nullptr;
    delete field_80;
    field_80 = nullptr;
    delete skybox2Mesh;
    skybox2Mesh = nullptr;
    delete field_74;
    field_74 = nullptr;
    delete particleEmitBoolPtr;
    particleEmitBoolPtr = nullptr;
    delete particleSystemMgr;
    particleSystemMgr = nullptr;
    delete field_90;
    field_90 = nullptr;
    delete particleRenderBoolPtr;
    particleRenderBoolPtr = nullptr;
    delete field_98;
    field_98 = nullptr;
    delete field_9c;
    field_9c = nullptr;
    if (field_a4) {
        ArrayReleaseClasses(*field_a4);
        delete field_a4;
        field_a4 = nullptr;
    }
    if (field_a8) {
        delete field_a8;
        field_a8 = nullptr;
    }
    if (playerGuns) {
        ArrayReleaseClasses(*playerGuns);
        delete playerGuns;
        playerGuns = nullptr;
    }
    if (enemyGuns) {
        ArrayReleaseClasses(*enemyGuns);
        delete enemyGuns;
        enemyGuns = nullptr;
    }
    if (enemies) {
        ArrayReleaseClasses(*enemies);
        delete enemies;
        enemies = nullptr;
    }
    if (asteroids) {
        ArrayReleaseClasses(*asteroids);
        delete asteroids;
        asteroids = nullptr;
    }
    if (gasClouds) {
        ArrayReleaseClasses(*gasClouds);
        delete gasClouds;
        gasClouds = nullptr;
    }
    if (landmarks) {
        ArrayReleaseClasses(*landmarks);
        delete landmarks;
        landmarks = nullptr;
    }
    if (messages) {
        ArrayReleaseClasses(*messages); delete messages;
        messages = nullptr;
    }
    if (field_104) {
        ArrayReleaseClasses(*field_104);
        delete field_104;
        field_104 = nullptr;
    }
    delete lodManager;
    lodManager = nullptr;
    delete field_a0;
    field_a0 = nullptr;
    if (field_b0) {
        delete field_b0;
        field_b0 = nullptr;
    }
}

void Level::incNumDeliveredPassengers(int delta) {
    numDeliveredPassengers += delta;
}

void Level::friendDied() {
    friendsLeft -= 1;
}

Route *Level::createRoute(int count) {
    unsigned int n = count * 3;
    int *pts = new int[n];
    int *p = pts + 1;
    for (int i = -1; i + 1 < (int) n; i = i + 3) {
        int sign = (AERandom::gRandom->nextInt(2) == 0) ? 1 : -1;
        p[-1] = (AERandom::gRandom->nextInt(30000) + 50000) * sign;
        p[0] = AERandom::gRandom->nextInt(20000) - 10000;
        if (i == -1) {
            pts[2] = AERandom::gRandom->nextInt() + 50000;
        } else {
            int prev = p[-2];
            p[1] = AERandom::gRandom->nextInt() + prev + 50000;
        }
        p = p + 3;
    }
    Route *r = (Route *) ::operator new(0x18);
    new(r) Route(pts, n);
    return r;
}

void Level::alarmAllFriends(int race, bool message) {
    if (this->enemies != nullptr) {
        for (unsigned int i = 0; i < this->enemies->size(); i = i + 1) {
            KIPlayer *obj = (*this->enemies)[i];
            if (obj->shipGroup == race) {
                obj->player->setAlwaysEnemy(1);
            }
        }
    }

    if (this->alarmRequested == 0 && message) {
        int type = 1;
        this->alarmRequested = (unsigned char) type;
        if (Status::gStatus->inBlackMarketSystem() != 0) {
            type = 0xc;
        }
        createRadioMessage(type, race);
        if (((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getRace() == race) {
            Status::gStatus->getStation()->setAttackedFriends(true);
        }
    }
}

void Level::setPlayerEngineColor(short color) {
    int c = color;
    if (player != nullptr && field_a4 != nullptr) {
        ParticleSettings::SetDefinition *particleSets = levelCurrentParticleSets();
        int count = (int) field_a4->size();
        for (int i = 0; i < count; i = i + 1) {
            particleSets[kFirstEngineParticleSet + i].color0 =
                c << 0x10 | c << 0x18 | c << 8 | 0xff;
        }
    }
}

Gun *Level::createGun(int idx, int owner, int kind, int hp, int dmg, int rate, int cool, int color) {
    ObjectGun *obj = 0;
    Gun *gun = 0;

    switch (kind) {
        case ITEM_SORT_LASER:
        case ITEM_SORT_BLASTER:
        case ITEM_SORT_THERMO: {
            int res = g_cg_meshTable[idx];
            if (res < 0) {
                gun = (Gun *) ::operator new(0x114);
                new(gun) Gun(owner, dmg, 1, hp, cool, rate, (float) color, Vector{0.0f, 0.0f, 0.0f},
                             Vector{0.0f, 0.0f, 0.0f});
                gun->setIndex(idx);
                gun->weaponType = static_cast<ItemSort>(kind);
                gun->setPlayerGun(1);
                obj = (ObjectGun *) ::operator new(0x24);
                new(obj) BeamGun(owner, gun, idx, this);
            } else {
                int barrels = ((unsigned) (idx - 9) < 3 || idx == 0xe4) ? 1 : 0x14;
                gun = (Gun *) ::operator new(0x114);
                if (kind == ITEM_SORT_THERMO) {
                    new(gun) Gun(owner, dmg, barrels, hp, cool, rate, (float) color,
                                 Vector{0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
                    gun->setIndex(idx);
                    gun->weaponType = ITEM_SORT_THERMO;
                    gun->setPlayerGun(1);
                    gun->setErrorMagnitudePercentage(0x14);
                    obj = (ObjectGun *) ::operator new(0xe8);
                    new(obj) RocketGun(owner, gun, res, 0, 0, 0, 1, this);
                } else {
                    new(gun) Gun(owner, dmg, barrels, hp, cool, rate, (float) color, Vector{0.0f, 0.0f, 0.0f},
                                 Vector{0.0f, 0.0f, 0.0f});
                    gun->setIndex(idx);
                    gun->weaponType = static_cast<ItemSort>(kind);
                    gun->setPlayerGun(1);
                    obj = (ObjectGun *) ::operator new(0xb0);
                    new(obj) ObjectGun(owner, gun, res, 1000, this);
                }
            }
            break;
        }
        case ITEM_SORT_AUTO_CANNON:
        case ITEM_SORT_SCATTER_GUN:
            gun = (Gun *) ::operator new(0x114);
            new(gun) Gun(owner, dmg, 0x19, hp, cool, rate, (float) color, Vector{0.0f, 0.0f, 0.0f},
                         Vector{0.0f, 0.0f, 0.0f});
            gun->setIndex(idx);
            gun->weaponType = static_cast<ItemSort>(kind);
            gun->setPlayerGun(1);
            obj = (ObjectGun *) ::operator new(0xb0);
            new(obj) ObjectGun(owner, gun, g_cg_meshTable[idx], 1000, this);
            gun->setErrorMagnitudePercentage(2);
            break;
        case ITEM_SORT_ROCKET:
        case ITEM_SORT_MISSILE:
        case ITEM_SORT_CLUSTER_MISSILE: {
            gun = (Gun *) ::operator new(0x114);
            int barrels = (kind == ITEM_SORT_CLUSTER_MISSILE) ? (idx - 0xd3) : 5;
            new(gun) Gun(owner, dmg, barrels, hp, cool, rate, (float) color, Vector{0.0f, 0.0f, 0.0f},
                         Vector{0.0f, 0.0f, 0.0f});
            gun->setIndex(idx);
            gun->weaponType = static_cast<ItemSort>(kind);
            gun->setPlayerGun(1);
            obj = (ObjectGun *) ::operator new(0xe8);
            new(obj) RocketGun(owner, gun, g_cg_meshTable[idx], 0, 0, kind,
                               (kind == ITEM_SORT_CLUSTER_MISSILE || kind == ITEM_SORT_MISSILE) ? 1 : 0, this);
            Globals::gGlobals->addSoundResourceToList(g_cg_soundTable[idx]);
            break;
        }
        case ITEM_SORT_EMP_BOMB:
        case ITEM_SORT_NUKE:
        case ITEM_SORT_IONIZING_MISSILE: {
            gun = (Gun *) ::operator new(0x114);
            new(gun) Gun(owner, dmg, 1, hp, cool, rate, (float) color, Vector{0.0f, 0.0f, 0.0f},
                         Vector{0.0f, 0.0f, 0.0f});
            gun->setIndex(idx);
            gun->weaponType = static_cast<ItemSort>(kind);
            gun->setPlayerGun(1);
            int attr = static_cast<Array<Item *> *>(Globals::items)->data()[idx]->getAttribute(0xf);
            obj = (ObjectGun *) ::operator new(300);
            new(obj) BombGun(gun, g_cg_meshTable[idx], 1, kind, attr == 1 ? 1 : 0,
                             this);
            Globals::gGlobals->addSoundResourceToList(g_cg_soundTable[idx]);
            break;
        }
        case ITEM_SORT_TURRET:
        case ITEM_SORT_PLASMA_COLLECTOR: {
            gun = (Gun *) ::operator new(0x114);
            new(gun) Gun(owner, dmg, 0xf, hp, cool, rate, (float) color,
                         Vector{idx == 0x30 ? 80.0f : 0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
            gun->setIndex(idx);
            gun->weaponType = static_cast<ItemSort>(kind);
            gun->setPlayerGun(1);
            if ((idx == 0x30 || idx == 0xe0 || idx == 0xb5)) {
                gun->field_0xa4 = 1;
                if (idx == 0xe0) gun->field_0xa5 = 1;
            }
            obj = (ObjectGun *) ::operator new(0xb0);
            new(obj) ObjectGun(owner, gun, g_cg_meshTable[idx], 1000, this);
            break;
        }
        case ITEM_SORT_MINE: {
            gun = (Gun *) ::operator new(0x114);
            new(gun) Gun(owner, dmg, 10, hp, cool, rate, 2.0f, Vector{0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
            gun->setIndex(idx);
            gun->weaponType = ITEM_SORT_MINE;
            gun->setPlayerGun(1);
            obj = (ObjectGun *) ::operator new(0xd4);
            new(obj) MineGun(gun, g_cg_meshTable[idx], 1, ITEM_SORT_MINE, this);
            Globals::gGlobals->addSoundResourceToList(g_cg_soundTable[idx]);
            break;
        }
        case ITEM_SORT_SENTRY_GUN: {
            gun = (Gun *) ::operator new(0x114);
            new(gun) Gun(owner, 0, 3, hp, cool, rate, 2.0f, Vector{0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
            gun->setIndex(idx);
            gun->weaponType = ITEM_SORT_SENTRY_GUN;
            gun->setPlayerGun(1);
            obj = (ObjectGun *) ::operator new(0xb4);
            new(obj) SentryGun(gun, g_cg_meshTable[idx], 1, ITEM_SORT_SENTRY_GUN, this);
            Globals::gGlobals->addSoundResourceToList(g_cg_soundTable[idx]);
            break;
        }
        case ITEM_SORT_SHOCK_BLAST: {
            gun = (Gun *) ::operator new(0x114);
            new(gun) Gun(owner, dmg, 1, hp, 1, rate, (float) color, Vector{0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
            gun->setIndex(idx);
            gun->weaponType = ITEM_SORT_SHOCK_BLAST;
            gun->setPlayerGun(1);
            obj = (ObjectGun *) ::operator new(300);
            new(obj) BombGun(gun, g_cg_meshTable[idx], 1, ITEM_SORT_SHOCK_BLAST, 0, this);
            Globals::gGlobals->addSoundResourceToList(g_cg_soundTable[idx]);
            break;
        }
        default:
            gun = 0;
            break;
    }

    switch (idx) {
        case 0x29: Globals::gGlobals->addSoundResourceToList(15);
        case 0x2a: Globals::gGlobals->addSoundResourceToList(16);
        case 0x2b: Globals::gGlobals->addSoundResourceToList(17);
        case 0x2c: Globals::gGlobals->addSoundResourceToList(14);
        case 0x2d: Globals::gGlobals->addSoundResourceToList(13);
        case 0x2e: Globals::gGlobals->addSoundResourceToList(12);
        default: break;
    }

    gun->setLevel(this);
    if (this->playerGuns == nullptr) {
        this->playerGuns = new Array<AbstractGun *>();
    }
    ArrayAdd((AbstractGun *) obj, *(this->playerGuns));
    return gun;
}

void Level::createSpace() {
    if (skyboxMesh == -1) {
        PaintCanvas *canvas = Globals::Canvas;
        if (!Status::gStatus->inAlienOrbit()) {
            SolarSystem *system = static_cast<SolarSystem *>(Status::gStatus->getSystem());
            const int variant = system->getIndex() % 3;
            canvas->MeshCreate(static_cast<unsigned short>(0x45ba + variant),
                               reinterpret_cast<unsigned int &>(field_08), false);
            canvas->TextureCreate(static_cast<unsigned short>(0x2766 + variant),
                                  reinterpret_cast<unsigned int &>(field_19c), false);

            const int mission = Status::gStatus->getCurrentCampaignMission();
            if (mission == 0 && missionPtr == 3) {
                canvas->MeshCreate(0x458b, reinterpret_cast<unsigned int &>(skyboxMesh), false);
                canvas->TextureCreate(0x2754, reinterpret_cast<unsigned int &>(field_198), false);
            } else {
                if (Status::gStatus->inSupernovaSystem()) {
                    if (mission == 89) {
                        canvas->MeshCreate(0x458d, reinterpret_cast<unsigned int &>(skyboxMesh), false);
                        canvas->TextureCreate(0x2756, reinterpret_cast<unsigned int &>(field_198), false);
                    } else {
                        if (mission <= 157) {
                            canvas->MeshCreate(0x45a0, reinterpret_cast<unsigned int &>(skyboxTexture), false);
                            canvas->TransformCreate(reinterpret_cast<unsigned int &>(field_10));
                            canvas->TransformAddMeshId(field_10, skyboxTexture);
                            static_cast<AbyssEngine::Transform *>(canvas->TransformGetTransform(field_10))->Update(1000, false);
                            canvas->MeshCreate(0x45a1, reinterpret_cast<unsigned int &>(field_14), false);
                            canvas->TransformCreate(reinterpret_cast<unsigned int &>(field_18));
                            canvas->TransformAddMeshId(field_18, field_14);
                            static_cast<AbyssEngine::Transform *>(canvas->TransformGetTransform(field_18))->Update(1000, false);
                            canvas->TextureCreate(mission >= 106 ? 0x2765 : 0x2764,
                                                  reinterpret_cast<unsigned int &>(field_1a0), false);
                        }
                        canvas->MeshCreate(0x4597, reinterpret_cast<unsigned int &>(skyboxMesh), false);
                        canvas->TextureCreate(0x2760, reinterpret_cast<unsigned int &>(field_198), false);
                    }
                } else {
                    const int textureIndex = system->getTextureIndex();
                    canvas->MeshCreate(static_cast<unsigned short>(0x4588 + textureIndex),
                                       reinterpret_cast<unsigned int &>(skyboxMesh), false);
                    canvas->TextureCreate(static_cast<unsigned short>(0x2751 + textureIndex),
                                          reinterpret_cast<unsigned int &>(field_198), false);
                }

                const int systemIndex = system->getIndex();
                if ((systemIndex | 2) == 26 || systemIndex == 25) {
                    supernovaFlareActive = 1;
                    canvas->MeshCreate(0x3827, reinterpret_cast<unsigned int &>(supernovaFlareMesh), false);
                    canvas->TextureCreate(0x8282, reinterpret_cast<unsigned int &>(supernovaFlareTexture), false);
                    canvas->TextureCreate(0x8283, reinterpret_cast<unsigned int &>(field_1c8), false);
                }
                if (Status::gStatus->inPlanetRingOrbit()) {
                    unsigned int ringMesh = 0xffffffffU;
                    canvas->MeshCreate(0x499f, ringMesh, false);
                    canvas->TransformCreate(reinterpret_cast<unsigned int &>(field_1b4));
                    canvas->TransformAddMeshId(field_1b4, ringMesh);
                    canvas->TextureCreate(0x715a, reinterpret_cast<unsigned int &>(field_1b8), false);
                }
                if (Status::gStatus->inStormOrbit()) {
                    unsigned int stormMesh = 0xffffffffU;
                    canvas->MeshCreate(0x49a0, stormMesh, false);
                    canvas->TransformCreate(reinterpret_cast<unsigned int &>(field_1bc));
                    canvas->TransformAddMeshId(field_1bc, stormMesh);
                    canvas->TextureCreate(0x715b, reinterpret_cast<unsigned int &>(field_1c0), false);
                }
            }

            if (system->getTextureIndex() >= 16 &&
                static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->IsPostEffectActivated()) {
                canvas->MeshGetPointer(skyboxMesh)->meshPostEffectFlag = 1046562734;
            }
        } else {
            canvas->MeshCreate(0x45bc, reinterpret_cast<unsigned int &>(field_08), false);
            canvas->TextureCreate(0x2768, reinterpret_cast<unsigned int &>(field_19c), false);
            canvas->MeshCreate(0x4592, reinterpret_cast<unsigned int &>(skyboxMesh), false);
            canvas->TextureCreate(0x275b, reinterpret_cast<unsigned int &>(field_198), false);
        }

        if (Status::gStatus->inFogSkyboxOrbit()) {
            skyRotX = skyRotY = skyRotZ = 0.0f;
        } else {
            AERandom *random = static_cast<AERandom *>(Globals::rnd);
            Station *station = Status::gStatus->getStation();
            random->setSeed(station ? 2 * station->getIndex() : -1);
            skyRotX = float(random->nextInt(0x10000)) * (1.0f / 65536.0f) * 6.2832f;
            skyRotY = float(random->nextInt(0x10000)) * (1.0f / 65536.0f) * 6.2832f;
            skyRotZ = float(random->nextInt(0x10000)) * (1.0f / 65536.0f) * 6.2832f;
            random->reset();
        }
    }

    int mode = this->missionPtr;
    if (mode != 4 && mode != 0x17) {
    bool showWormhole = (comingFromAlienWorld ||
                         Status::gStatus->getStation()->isAttackedByAliens() ||
                         Status::gStatus->inAlienOrbit()) &&
                        Status::gStatus->getCurrentCampaignMission() < 43;
    if (showWormhole) {
        Globals::gGlobals->addSoundResourceToList(34);
    }

    this->landmarks = new Array<KIPlayer *>();
    ArraySetLength(4, *(this->landmarks));

    bool createStation = !Status::gStatus->inEmptyOrbit();
    if (!createStation && !Status::gStatus->inAlienOrbit()) {
        int stationIndex = Status::gStatus->getStation()->getIndex();
        createStation = stationIndex == 27 || stationIndex == 110 || stationIndex == 111;
    }
    if (createStation) {
        PlayerStation *station = new PlayerStation(Status::gStatus->getStation());
        (*this->landmarks)[0] = station;
        this->lodManager->addObject(station->rootGeometry);
        if (Status::gStatus->dlc1Won()) {
            Station *nameSource = static_cast<Galaxy *>(Globals::galaxy)->getStation(101);
            station->name = nameSource->getName();
            delete nameSource;
        }
    }

    // Android ARM rodata at 0x1fdc54 and 0x1fb7f8, indexed by system race.
    static const uint16_t gateMeshes[4][4] = {
        {0x3a98, 0x3a9a, 0x3a99, 0x3a9b},
        {0x3a9c, 0x3a9e, 0x3a9d, 0x3a9f},
        {0x3aa0, 0x3aa2, 0x3aa1, 0x3aa3},
        {0x3aa4, 0x3aa6, 0x3aa5, 0x3aa7},
    };
    static const uint16_t gateLodMeshes[4][2] = {
        {0x3aa8, 0x3aac}, {0x3aa9, 0x3aad},
        {0x3aaa, 0x3aae}, {0x3aab, 0x3aaf},
    };

    field_138 = 0;
    AERandom *random = static_cast<AERandom *>(Globals::rnd);
    Station *currentStation = Status::gStatus->getStation();
    random->setSeed(currentStation ? 2 * currentStation->getIndex() : -1);
    for (int slot = 1; slot < 3; ++slot) {
        if (slot == 1) {
            SolarSystem *system = static_cast<SolarSystem *>(Status::gStatus->getSystem());
            if (!system || !system->currentOrbitHasWarpGate()) {
                continue;
            }
        }

        Vector position = missionSpawnPosition;
        if (currentStation) {
            int direction = random->nextInt(2);
            int variation = random->nextInt(500);
            field_138 += (direction ? 250 + variation : -(250 + variation)) * 16;
            missionSpawnPosition.z = (slot == 1 ? 90000.0f : 120000.0f) + float(field_138) * 3.0f;
            Matrix rotation;
            AbyssEngine::AEMath::MatrixSetRotation(rotation, 0.0f, float(field_138) * (1.0f / 65536.0f) * 6.2832f, 0.0f);
            position = AbyssEngine::AEMath::MatrixRotateVector(rotation, missionSpawnPosition);
            position.y = 0.0f;
        }

        int race = Status::gStatus->inAlienOrbit()
                       ? 0
                       : static_cast<SolarSystem *>(Status::gStatus->getSystem())->getRace();
        AEGeometry *geometry = new AEGeometry(gateMeshes[race][0], PaintCanvas::gCanvas, false);
        for (int child = 1; child <= 2; ++child) {
            unsigned int transform = 0xffffffffU;
            PaintCanvas::gCanvas->TransformCreate(transform);
            PaintCanvas::gCanvas->TransformAddMesh(transform, gateMeshes[race][child], false);
            geometry->addChild(transform);
        }

        PlayerJumpgate *gate = new PlayerJumpgate(15, geometry, position.x, position.y, position.z, slot != 2);
        (*landmarks)[slot] = gate;
        if (slot == 2) {
            gate->player->radius = 0;
        }
        unsigned int animation = 0xffffffffU;
        PaintCanvas::gCanvas->TransformCreate(animation);
        PaintCanvas::gCanvas->TransformAddMesh(animation, gateMeshes[race][3], false);
        gate->addJumpAnimationHandle(animation);

        uint16_t lodMeshes[2] = {gateLodMeshes[race][0], gateLodMeshes[race][1]};
        int lodDistances[2] = {40000, 70000};
        geometry->setLodMeshes(lodMeshes, lodDistances, 2);
        lodManager->addObject(geometry);
        if (race == 1) {
            uint16_t childLodMeshes[2] = {0x3ab0, 0x3ab1};
            geometry->setLodChildMeshes(childLodMeshes);
        }
    }
    random->reset();

    if (Status::gStatus->inAlienOrbit()) {
        Vector position = {0.0f,
                           float(random->nextInt(20000) - 10000),
                           float(random->nextInt(50000) + 170000)};
        KIPlayer *exitGate = (*landmarks)[2];
        exitGate->setPosition(position);
        Vector facing = AbyssEngine::AEMath::operator-(position);
        exitGate->geometry->setDirection(facing, Vector{0.0f, 1.0f, 0.0f});
        field_138 = 0;
    }

    if (!Status::gStatus->gameWon()) {
        AEGeometry *geometry = new AEGeometry(0x4262, PaintCanvas::gCanvas, false);
        float x = float(random->nextInt(80000) - 40000);
        float y = float(random->nextInt(40000) - 20000);
        float z = float(random->nextInt(40000) + 40000);
        PlayerWormHole *wormhole = new PlayerWormHole(16994, geometry, x, y, z, showWormhole);
        (*landmarks)[3] = wormhole;
        wormhole->setLevel(this);
    }

    field_138 += 0x8000;
    if (!initStreamOutPosition) {
        field_138 = random->nextInt(2) == 0 ? 1600 : -1600;
    }
    }

    this->starSystem = new StarSystem(mode);

    SolarSystem *system = static_cast<SolarSystem *>(Status::gStatus->getSystem());
    r = system ? float(system->starR) : 10.0f;
    g = system ? float(system->starG) : 136.0f;
    b = system ? float(system->starB) : 10.0f;
    i_r = r;
    i_g = g;
    i_b = b;
    r_min = int(r / 3.0f);
    g_min = int(g / 3.0f);
    b_min = int(b / 3.0f);
}

void Level::createRadioMessage(int type, int sub) {
    unsigned r2 = (unsigned) sub;

    if (this->player == nullptr || this->player->field_0x1c == 0)
        return;

    Status::gStatus->getMission();
    if (((Mission *) Status::gStatus->getMission())->isEmpty() == 0)
        return;

    if (this->messages != nullptr) {
        if (this->messages) {
            ArrayReleaseClasses(*this->messages); delete this->messages;
            this->messages = nullptr;
        }
    }
    this->messages = new Array<RadioMessage *>();

    int speaker;
    if (r2 == 0) speaker = 0x40;
    else if (r2 == 2) speaker = 0x41;
    else if (r2 == 3) speaker = 0x15;
    else speaker = (r2 == 8) ? 9 : 0x3f;

    int extraDelay = 0;
    unsigned id = 0x1ba;
    bool aborted = false;
    bool builtInline = false;

    switch (type) {
        case 0:
        case 1: {
            int *stations = (int *) Status::gStatus->field_90;
            bool atStation = false;
            if (stations != 0) {
                for (unsigned k = 0; k < (unsigned) *stations; k = k + 1) {
                    int sidx = *(int *) (((int *) stations[1])[k]);
                    Station *st = (Station *) Status::gStatus->getStation();
                    if (sidx == ((Station *) st)->getIndex()) {
                        atStation = true;
                        break;
                    }
                }
            }
            if (atStation) {
                aborted = true;
                break;
            }
            int base = (type == 0) ? 0x1aa : 0x1ad;
            id = AERandom::gRandom->nextInt() + base;
            break;
        }
        case 3:
            this->field_1b0 = 1;
            id = AERandom::gRandom->nextInt() + 0x1b3;
            break;
        case 4:
            this->field_68 = 1;
            id = AERandom::gRandom->nextInt() + 0x1b6;
            break;
        case 5: id = 0x1bb;
            extraDelay = 0;
            break;
        case 6: id = 0x1bc;
            extraDelay = 0;
            break;
        case 9: id = AERandom::gRandom->nextInt() + 0x1c1;
            break;
        case 10: id = AERandom::gRandom->nextInt() + 0x1c3;
            break;
        case 0xb: id = AERandom::gRandom->nextInt() + 0x1c5;
            break;
        case 0xc: id = 0x1c5;
            extraDelay = 0;
            break;
        case 0xd: id = AERandom::gRandom->nextInt() + 0x1c7;
            break;
        case 0xe: {
            int *st = (int *) Status::gStatus->field_90;
            id = 0x88f;
            if (st != 0) {
                for (int k = 0; k != *st; k = k + 1)
                    if (-1 < *(int *) (st[1] + k * 4)) id = id - 2;
            }
            break;
        }
        case 0xf: {
            int *st = (int *) Status::gStatus->field_90;
            id = 0x88e;
            if (st != 0) {
                for (int k = 0; k != *st; k = k + 1)
                    if (-1 < *(int *) (st[1] + k * 4)) id = id - 2;
                if (id - 0x889 < 5) {
                    extraDelay = 0;
                    break;
                }
            }
            aborted = true;
            break;
        }
        case 8: {
            int stage = AERandom::gRandom->nextInt();
            int off = 0;
            for (int k = 0; k < stage; k = k + 1)
                off = off + g_crm_counts8[k];
            int cnt = g_crm_counts8[stage];
            RadioStageEntry *tbl = (RadioStageEntry *) g_crm_table8 + off;
            for (int k = 0; k < cnt; k = k + 1) {
                int delay = (k == 0) ? 5000 : (k - 1);
                int arg = tbl[k].radioStageEntry;
                RadioMessage *m = (RadioMessage *) ::operator new(0x28);
                int kind = (k == 0) ? 5 : 6;
                new(m) RadioMessage(tbl[k].textID, arg, kind, delay);
                ArrayAdd(m, *(this->messages));
            }
            builtInline = true;
            break;
        }
        case 0x13: {
            RadioMessage *m = (RadioMessage *) ::operator new(0x28);
            AbyssEngine::AERandom *rng = AERandom::gRandom;
            new(m) RadioMessage(rng->nextInt() + 0xaf4, 0, 5, 0x5dc);
            ArrayAdd(m, *(this->messages));
            m = (RadioMessage *) ::operator new(0x28);
            new(m) RadioMessage(rng->nextInt() + 0xafa, 0, 6, 0);
            ArrayAdd(m, *(this->messages));
            builtInline = true;
            break;
        }
        case 0x15: id = 0xc54;
            extraDelay = 0;
            break;
        case 0x16: id = 0xc55;
            extraDelay = 0;
            break;
        case 0x17: id = 0xc56;
            extraDelay = 0;
            break;
        case 0x18: id = 0xc57;
            extraDelay = 0;
            break;
        case 0x19: id = 0xc58;
            extraDelay = 0;
            break;
        case 0x1a: id = 0xc59;
            extraDelay = 0;
            break;
        case 0x1b: {
            RadioMessage *m = (RadioMessage *) ::operator new(0x28);
            new(m) RadioMessage(r2 * 2 + 0xc60, 6, 5, 0x5dc);
            ArrayAdd(m, *(this->messages));
            m = (RadioMessage *) ::operator new(0x28);
            new(m) RadioMessage(r2 * 2 + 0xc61, 0, 6, 0);
            ArrayAdd(m, *(this->messages));
            builtInline = true;
            break;
        }
        case 0x1c:
            speaker = 0x26;
            id = r2;
            extraDelay = 0;
            break;
        case 7:
        default:
            if (type == 7 || type == 0xc || type == 0xe) {
            } else {
                id = AERandom::gRandom->nextInt() + 0x1bd;
            }
            break;
    }

    if (aborted) {
        if (this->messages != nullptr)
            if (this->messages) {
                ArrayReleaseClasses(*this->messages); delete this->messages;
                this->messages = nullptr;
            }
        PlayerEgo *ego = this->player;
        this->crm_dispatch(ego->field_0x1c, 0);
        return;
    }

    if (!builtInline) {
        RadioMessage *m = (RadioMessage *) ::operator new(0x28);
        new(m) RadioMessage(id, speaker, 5, extraDelay);
        ArrayAdd(m, *(this->messages));
    }

    PlayerEgo *ego = this->player;
    this->crm_dispatch(ego->field_0x1c, (void *) this->messages);
}

int Level::init() {
    Level *thisptr = this;

    int stage = this->field_134;
    if (stage == 0) {
        this->field_68 = 0;
        this->field_1b0 = 0;
        this->alarmRequested = 0;
        this->field_18a = 0;
        if (this->playerRoute != nullptr)
            (this->playerRoute->~Route(), ::operator delete(this->playerRoute));
        this->playerRoute = nullptr;
        if (this->enemyRoute != nullptr)
            (this->enemyRoute->~Route(), ::operator delete(this->enemyRoute));
        this->enemyRoute = nullptr;
        if (this->friendRoute != nullptr)
            (this->friendRoute->~Route(), ::operator delete(this->friendRoute));
        this->friendRoute = nullptr;
        if (this->objectivesB != 0)
            ((this->objectivesB)->~Objective(), ::operator delete(this->objectivesB));
        this->objectivesB = nullptr;
        if (this->objectivesA != 0)
            ((this->objectivesA)->~Objective(), ::operator delete(this->objectivesA));
        this->objectivesA = nullptr;

        LODManager *lod = (LODManager *) ::operator new(sizeof(LODManager));
        new(lod) LODManager();
        this->lodManager = lod;

        PaintCanvas *canvas = Globals::Canvas;

        bool dustVariant = false;
        if (Status::gStatus->inAlienOrbit() == 0) {
            Status::gStatus->getSystem();
            dustVariant = ((SolarSystem *) Status::gStatus->getSystem())->getTextureIndex() == 0xc;
        }

        ParticleSystemManager *psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x4e85, false, 0xffff, false);
        this->particleEmitBoolPtr = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x6a72, false, 0xffff, false);
        this->particleRenderBoolPtr = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x4e83, true, 0xffff, false);
        this->field_74 = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x4e7a, true, 0x4e7a, true);
        this->field_80 = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x5e20, true, 0x5e20, true);
        this->field_98 = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1,
                                       (unsigned short) (dustVariant ? 0x4ea9 : 0x4e7f), true, 0, false);
        this->particleSystemMgr = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x4e7c, false, 0x4e7c, false);
        this->skybox2Mesh = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x6a7c, true, 0x6a7c, true);
        this->field_8c = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x6ab9, true, 0xffff, false);
        this->field_9c = psm;
        psm = (ParticleSystemManager *) ::operator new(sizeof(ParticleSystemManager));
        new(psm) ParticleSystemManager(canvas, ParticleSettings::CameraSet_1, 0x6aaf, true, 0xffff, false);
        this->field_94 = psm;

        thisptr->createSpace();

        if (this->missionPtr == 3) {
            thisptr->createPlayer();
            if (initStreamOutPosition != 0 && this->player->geometry != nullptr) {
                bool useStationOrbit = false;
                Array<Station *> *stack = Status::gStatus->getStationStack();
                if (stack && stack->size() > 1 && (*stack)[1]) {
                    SolarSystem *system = static_cast<SolarSystem *>(Status::gStatus->getSystem());
                    if (system && !system->currentOrbitHasWarpGate()) {
                        Array<int> *stationIds = reinterpret_cast<Array<int> *>(system->getStations());
                        int stationIndex = (*stack)[1]->getIndex();
                        int planetIndex = -1;
                        for (unsigned int i = 0; i < stationIds->size(); ++i) {
                            if ((*stationIds)[i] == stationIndex) {
                                planetIndex = int(i);
                                break;
                            }
                        }
                        Vector position = {0.0f, 0.0f, 25000.0f};
                        if (planetIndex >= 0) {
                            Array<AEGeometry *> *planets = static_cast<Array<AEGeometry *> *>(starSystem->getPlanets());
                            position = (*planets)[planetIndex + 1]->getPosition();
                        }
                        position *= 4.0f;
                        this->player->setPosition(position);
                        Vector facing = AbyssEngine::AEMath::VectorNormalize(-position);
                        this->player->geometry->setDirection(facing, Vector{0.0f, 1.0f, 0.0f});
                        useStationOrbit = true;
                    }
                }
                if (!useStationOrbit) {
                    this->player->setPosition((*landmarks)[2]->getPosition());
                }
            } else {
                this->player->setPosition(10.0f, 10.0f, 10000.0f);
            }
            if (Status::gStatus->getCurrentCampaignMission() == 1)
                this->player->setPosition(0.0f, 0.0f, -110000.0f);
        }
        stage = this->field_134;
    }

    if (stage != 1) {
        this->field_134 = stage + 1;
        return 0;
    }

    if (this->missionPtr != 4 && this->missionPtr != 0x17) {
        thisptr->createAsteroids();
        thisptr->createGasClouds();
    }

    Mission *m = (Mission *) Status::gStatus->getMission();
    if (m == 0)
        Status::gStatus->setMission(reinterpret_cast<Mission *>(Mission::empty));

    int mode = this->missionPtr;
    if (mode == 3) {
        bool campaign = ((Mission *) Status::gStatus->getMission())->isEmpty() == 0 && m->isCampaignMission() != 0;
        bool madeScene = false;
        if (campaign) {
            if (this->missionPtr != 3) { madeScene = true; } else {
                int won = Status::gStatus->gameWon();
                GameSettings *settings = reinterpret_cast<GameSettings *>(Globals::options);
                bool skip = won != 0 && settings->settingSkipIntroFlag == 0 &&
                            settings->settingSkipCampaignFlag == 0;
                if (skip) {
                    thisptr->createMission();
                    if (initStreamOutPosition != 0 && Status::gStatus->inBlackMarketSystem() != 0) {
                        if (thisptr->player != nullptr) {
                            PlayerEgoPolymorphic *ego = (PlayerEgoPolymorphic *) thisptr->player;
                            ego->vtable->placeAtStation(ego, 0);
                        }
                    }
                } else if (this->missionPtr != 3) {
                    madeScene = true;
                } else {
                    Status::gStatus->getMission();
                    if (((Mission *) Status::gStatus->getMission())->isEmpty() == 0) {
                        Mission *mm = (Mission *) Status::gStatus->getMission();
                        if (mm->isCampaignMission() != 0)
                            thisptr->createCampaignMission();
                    }
                }
            }
        } else {
            thisptr->createMission();
            if (initStreamOutPosition != 0 && Status::gStatus->inBlackMarketSystem() != 0) {
                if (thisptr->player != nullptr) {
                    PlayerEgoPolymorphic *ego = (PlayerEgoPolymorphic *) thisptr->player;
                    ego->vtable->placeAtStation(ego, 0);
                }
            }
        }
        if (madeScene) {
            thisptr->createScene();
            this->missionPtr = mode;
        }
    } else {
        thisptr->createScene();
        this->missionPtr = mode;
    }

    thisptr->createStaticObjects();
    mode = this->missionPtr;
    if (mode != 0x17 && mode != 4 &&
        (mode != 2 || Status::gStatus->getCurrentCampaignMission() != 0x2b)) {
        thisptr->createSentryGuns();
        thisptr->createFighterTurrets();
        thisptr->createWingmen();
    }
    thisptr->assignGuns();
    if (this->missionPtr != 3)
        this->missionPtr = 3;
    thisptr->connectPlayers();
    if (this->player != nullptr)
        this->player->setRoute(this->playerRoute);

    int initial = 0;
    int enemies = 0;
    if (this->enemies != nullptr) {
        for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
            KIPlayer *e = (*this->enemies)[i];
            if (e->deadFlag == 0 && e->inactiveFlag == 0 && e->field_0x3f == 0) {
                int wm = e->isWingMan();
                if (wm == 0) {
                    if ((char) e->field_0x44 == 0 && e->stealFlagByte == 0)
                        enemies = enemies + (e->countsAsEnemyExcludeFlag ^ 1);
                }
            }
        }
        enemies = enemies - this->field_120;
    }
    this->asteroidsLeft = 0;
    this->enemiesLeft = enemies;
    if (this->asteroids != nullptr)
        initial = (int) this->asteroids->size();
    this->field_184 = 0;
    this->field_188 = 0;
    this->field_1c = 0;
    this->asteroidsLeft = initial;
    this->kills = 0;
    this->friendCargoStolen = 0;
    this->field_70 = 1;
    return 1;
}

void Level::createFighterTurrets() {
    if (this->enemies == nullptr)
        return;

    for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
        KIPlayer *ki = (*this->enemies)[i];
        if (ki != 0) {
            int kind = ki->shipGroupFlag;
            if (kind == 0x2d || kind == 0x33) {
                PlayerTurret *t = (PlayerTurret *) this->createStaticObject((Waypoint *) (intptr_t) 0, 0x1a74, 1);
                t->player->setVulnerable(0);
                Player *pp = t->player;
                int hp = pp->getMaxHitpoints();
                pp->setMaxHitpoints(hp);
                char offset[12] = {0};
                t->setHost(ki, *(Vector *) offset);
                *(PlayerTurret **) &ki->field_0x10 = t;
                t->shipGroup = (kind == 0x2d) ? 8 : 0;
                t->noTargetFlag = 1;
                ArrayAdd((KIPlayer *) t, *(this->enemies));
            }
        }
    }
}

void Level::updateAlienAttackers(int dt) {
    Level *thisptr = this;
    this->alienAttackTimer = this->alienAttackTimer + dt;

    Mission *m = (Mission *) Status::gStatus->getMission();
    bool blocked = (m != 0) && m->isCampaignMission() != 0 &&
                   (Status::gStatus->getCurrentCampaignMission() == 0x28 ||
                    Status::gStatus->getCurrentCampaignMission() == 0x93 ||
                    Status::gStatus->getCurrentCampaignMission() == 0x9a);
    if (blocked)
        return;

    int elapsed = this->alienAttackTimer;
    int period = (Status::gStatus->getCurrentCampaignMission() == 0x29) ? 10000 : 45000;
    if (elapsed <= period)
        return;

    this->alienAttackTimer = 0;
    if (this->enemies == nullptr)
        return;

    for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
        int *ki = (int *) (*this->enemies)[i];

        if (((KIPlayer *) ki)->shipGroup == 9 && ((KIPlayer *) ki)->isDead() != 0 && ((KIPlayer *) ki)->player->
            isActive() == 0) {
            ((PlayerFighter *) ki)->revive();
            int inOrbit = (Status::gStatus->inAlienOrbit() != 0);
            if (!inOrbit) {
                Station *st = (Station *) Status::gStatus->getStation();
                if (((Station *) st)->isAttackedByAliens() == 0)
                    inOrbit = 1;
            }
            levelPlaceAlien(thisptr, ki, inOrbit);
        }
    }
}

// Android 2.0.16 0xaffd8; lengths checked against ARM call arguments.
void Level::createMission() {
    Mission *mission = Globals::status->getMission();
    if (!mission) return;
    Vector egoPosition = player->geometry->getPosition();
    if (Globals::status->inAlienOrbit()) {
        int count = 2;
        int level = Globals::status->getLevel();
        int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        float base = float(level) * 0.5f - 1.0f;
        if (base + float(roll) >= 2.0f)
            count = int(base + float(static_cast<AERandom *>(Globals::rnd)->nextInt(2)));
        int campaign = Globals::status->getCurrentCampaignMission();
        if (Globals::status->getCurrentCampaignMission() == 33) count = 2;
        if (campaign == 68) count = 2;
        enemies = new Array<KIPlayer *>();
        ArraySetLength(count, *enemies);
        for (int i = 0; i < count; ++i) {
            int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(9);
            (*enemies)[i] = createShip(9, 0, type, nullptr, true, false);
            KIPlayer *spawned = (*enemies)[i];
            float x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(120000) - 60000);
            float y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);
            float z = float(static_cast<AERandom *>(Globals::rnd)->nextInt(120000) - 60000);
            spawned->setPosition(x, y, z);
            (*enemies)[i]->player->setAlwaysEnemy(true);
        }
        return;
    }
    if (!mission->isEmpty()) {
        int race = Globals::status->getSystem()->getRace();
        int enemyRace;
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) >= 75)
            enemyRace = Globals::status->getStanding()->getEnemyRace(race);
        else enemyRace = 8;
        switch (mission->getType()) {
            case 1: {
                int coords[9];
                coords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(100000) - 50000;
                coords[1] = 0; coords[2] = 50000;
                coords[3] = static_cast<AERandom *>(Globals::rnd)->nextInt(100000) - 50000;
                coords[4] = 0; coords[5] = 75000;
                coords[6] = static_cast<AERandom *>(Globals::rnd)->nextInt(100000) - 50000;
                coords[7] = 0; coords[8] = 100000;
                enemyRoute = new Route(coords, 9);
                float difficulty = float(Globals::status->getMission()->getDifficulty()) / 10.0f * 5.0f;
                int friendRoll = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
                int base = int(difficulty) + 3;
                int count = int(float(base) + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * float(base));
                enemies = new Array<KIPlayer *>();
                ArraySetLength(count + friendRoll + 3, *enemies);
                for (int i = 0; i < count; ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(enemyRace);
                    (*enemies)[i] = createShip(enemyRace, 0, type, nullptr, true, false);
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                }
                for (unsigned i = count; i < enemies->size(); ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);
                    Route *spawnRoute = enemyRoute;
                    int index = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                    Waypoint *wp = spawnRoute->getWaypoint(index);
                    (*enemies)[i] = createShip(race, 0, type, wp, true, false);
                    (*enemies)[i]->player->setAlwaysFriend(true);
                }
                objectivesA = new Objective(7, count, this);
                return;
            }
            case 2: {
                int coords[6];
                coords[0] = int(float(-20000 - static_cast<AERandom *>(Globals::rnd)->nextInt(20000))); coords[1] = 0;
                coords[2] = int(float(-20000 - static_cast<AERandom *>(Globals::rnd)->nextInt(20000)));
                coords[3] = coords[4] = coords[5] = 0;
                enemyRoute = new Route(coords, 6);
                int amount = mission->getProductionGoodAmount();
                int count = int(float(mission->getDifficulty()) / 10.0f * 4.0f) + 2;
                enemies = new Array<KIPlayer *>();
                ArraySetLength(count + amount, *enemies);
                for (int i = 0; i < count; ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(enemyRace);
                    (*enemies)[i] = createShip(enemyRace, 0, type, nullptr, true, false);
                    KIPlayer *spawned = (*enemies)[i];
                    int x = enemyRoute->getWaypoint(0)->x + i * 2000;
                    int y = enemyRoute->getWaypoint(0)->y + i * 2000;
                    int z = enemyRoute->getWaypoint(0)->z + i * 2000;
                    spawned->setPosition(float(x), float(y), float(z));
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                    (*enemies)[i]->setRoute(enemyRoute->clone());
                    (*enemies)[i]->getRoute()->reachWaypoint(0);
                }
                for (unsigned i = 0; i + count < enemies->size(); ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);
                    (*enemies)[count + i] = createShip(race, 0, type, nullptr, true, false);
                    (*enemies)[count + i]->player->setAlwaysFriend(true);
                    Vector position;
                    position = (*asteroids)[i + (asteroids->size() >> 1)]->getPosition();
                    (*enemies)[count + i]->setPosition(position.x, position.y + 2000.0f, position.z);
                    (*enemies)[count + i]->cargo = nullptr;
                    *reinterpret_cast<unsigned char *>(&(*enemies)[count + i]->hasCargo) = 0;
                    (*enemies)[count + i]->setSpeed(0.0f);
                    (*enemies)[count + i]->setRotationSpeed(0.0f);
                    Player *ship = (*enemies)[count + i]->player;
                    ship->setHitpoints(ship->getMaxHitpoints() * 3);
                }
                objectivesA = new Objective(18, 0, count, this);
                objectivesB = new Objective(18, count, count + amount, this);
                return;
            }
            case 3:
            case 5: {
                int coords[3];
                int distance = static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 40000;
                coords[0] = (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance;
                coords[1] = 0;
                distance = static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 40000;
                coords[2] = (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance;
                playerRoute = new Route(coords, 3);
                enemies = new Array<KIPlayer *>();
                ArraySetLength(mission->getProductionGoodAmount(), *enemies);
                for (unsigned i = 0; i < enemies->size(); ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                    (*enemies)[i] = createShip(8, 0, type, playerRoute->getWaypoint(0), true, false);
                    (*enemies)[i]->setToSleep();
                }
                static_cast<PlayerFighter *>((*enemies)[enemies->size() - 1])->setMissionCrate(true);
                (*enemies)[enemies->size() - 1]->name = *Globals::gameText->getText(1611);
                objectivesA = new Objective(11, enemies->size() - 1, this);
                objectivesB = new Objective(12, enemies->size() - 1, this);
                return;
            }
            case 4: {
                if (static_cast<AERandom *>(Globals::rnd)->nextInt(2)) playerRoute = createRoute(static_cast<AERandom *>(Globals::rnd)->nextInt(2) + 2);
                else {
                    int coords[] = {int(field_c8), int(field_cc), int(field_d0)};
                    playerRoute = new Route(coords, 3);
                }
                int base = int(float(Globals::status->getMission()->getDifficulty()) / 10.0f * 5.0f) + 2;
                int count = int(float(base) + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * float(base));
                enemies = new Array<KIPlayer *>();
                ArraySetLength(count, *enemies);
                for (unsigned i = 0; i < enemies->size(); ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                    Route *spawnRoute = playerRoute;
                    int index = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                    (*enemies)[i] = createShip(8, 0, type, spawnRoute->getWaypoint(index), true, false);
                    (*enemies)[i]->setToSleep();
                }
                objectivesA = new Objective(18, 0, count, this);
                return;
            }
            case 6: {
                int coords[3];
                int distance = static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 60000;
                coords[0] = (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance;
                coords[1] = 0;
                distance = static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 60000;
                coords[2] = (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance;
                playerRoute = new Route(coords, 3);
                enemies = new Array<KIPlayer *>();
                ArraySetLength(1, *enemies);
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[0] = createShip(8, 0, type, playerRoute->getWaypoint(0), true, false);
                (*enemies)[0]->setToSleep();
                Player *ship = (*enemies)[0]->player;
                ship->setMaxHitpoints(3 * ship->getMaxHitpoints());
                (*enemies)[0]->setSpeed(3.0f);
                static_cast<PlayerFighter *>((*enemies)[0])->setRotate(3);
                objectivesA = new Objective(1, 0, this);
                return;
            }
            case 7: {
                int coords[3];
                coords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(40000) - 20000;
                coords[1] = static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                coords[2] = static_cast<AERandom *>(Globals::rnd)->nextInt(30000) + 40000;
                Route *route = new Route(coords, 3);
                float difficulty = float(Globals::status->getMission()->getDifficulty()) / 10.0f;
                int guards = int(difficulty + difficulty);
                int otherDifficulty = Globals::status->getMission()->getDifficulty();
                enemies = new Array<KIPlayer *>();
                ArraySetLength(int(float(otherDifficulty) / 10.0f * 20.0f) + guards + 15, *enemies);
                for (unsigned i = 0; i < enemies->size() - guards; ++i) {
                    (*enemies)[i] = reinterpret_cast<KIPlayer *>(createStaticObject(route->getWaypoint(0), 16917, true));
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                }
                for (unsigned i = enemies->size() - guards; i < enemies->size(); ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                    (*enemies)[i] = createShip(8, 0, type, nullptr, true, false);
                }
                objectivesA = new Objective(7, enemies->size() - guards, this);
                timeLimit = 121000;
                static_cast<Globals *>(Globals::globals)->addSoundResourceToList(22);
                return;
            }
            case 8: case 11: case 13: case 14: return;
            case 9: {
                int coords[] = {10000, 0, 100000, 10000, 0, 150000, 10000, 0, 200000};
                enemyRoute = new Route(coords, 9);
                int base = int(float(Globals::status->getMission()->getDifficulty()) / 10.0f * 5.0f) + 3;
                int count = int(float(base) + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * float(base));
                enemies = new Array<KIPlayer *>();
                ArraySetLength(count + 5, *enemies);
                enemyRace = Globals::status->getStanding()->getEnemyRace(mission->getClientRace());
                int freighterType = 15;
                int clientRace = 0;
                if (mission->getClientRace() <= 3) {
                    clientRace = mission->getClientRace();
                    if (clientRace == 1) freighterType = 13;
                }
                for (unsigned i = 0; i < enemies->size(); ++i) {
                    if (int(i) >= count) {
                        (*enemies)[i] = createShip(clientRace, 1, freighterType, nullptr, true, false);
                        Player *ship = (*enemies)[i]->player;
                        int level = Globals::status->getLevel() <= 20 ? Globals::status->getLevel() : 20;
                        float health = float(2 * level) + 150.0f;
                        int campaign = Globals::status->gameWon() ? 45 : Globals::status->getCurrentCampaignMission();
                        float factor = *reinterpret_cast<float *>(Globals::options + 0x2c) > 0.7f ? 1.4f : 1.0f;
                        ship->setMaxHitpoints(int((health + float(2 * campaign)) * factor));
                        static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(true);
                        (*enemies)[i]->player->setAlwaysFriend(true);
                        (*enemies)[i]->cargo = nullptr;
                        *reinterpret_cast<unsigned char *>(&(*enemies)[i]->hasCargo) = 0;
                    } else {
                        int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(enemyRace);
                        Route *spawnRoute = enemyRoute;
                        int index = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                        (*enemies)[i] = createShip(enemyRace, 0, type, spawnRoute->getWaypoint(index), true, false);
                        (*enemies)[i]->setToSleep();
                        (*enemies)[i]->player->setAlwaysEnemy(true);
                    }
                }
                (*enemies)[count]->setPosition(-2500.0f, -300.0f, 27000.0f);
                (*enemies)[count + 1]->setPosition(6500.0f, 3000.0f, 24000.0f);
                (*enemies)[count + 2]->setPosition(-4000.0f, -2000.0f, 19000.0f);
                (*enemies)[count + 3]->setPosition(9000.0f, -6000.0f, 17000.0f);
                (*enemies)[count + 4]->setPosition(3000.0f, 7000.0f, 15000.0f);
                objectivesA = new Objective(18, 0, count, this);
                objectivesB = new Objective(18, count, count + 5, this);
                return;
            }
            case 10: {
                int coords[6];
                coords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(5000) - 2500;
                coords[1] = static_cast<AERandom *>(Globals::rnd)->nextInt(5000) - 2500;
                coords[2] = static_cast<AERandom *>(Globals::rnd)->nextInt(30000) + 80000;
                coords[3] = static_cast<AERandom *>(Globals::rnd)->nextInt(5000) - 2500;
                coords[4] = static_cast<AERandom *>(Globals::rnd)->nextInt(5000) - 2500;
                coords[5] = static_cast<AERandom *>(Globals::rnd)->nextInt(30000) + 120000;
                playerRoute = new Route(coords, 6);
                int freighters = static_cast<AERandom *>(Globals::rnd)->nextInt(2) + 2;
                int base = int(float(Globals::status->getMission()->getDifficulty()) / 10.0f * 5.0f) + 3;
                enemyRace = Globals::status->getStanding()->getEnemyRace(mission->getClientRace());
                if (enemyRace == 8) enemyRace = 0;
                enemies = new Array<KIPlayer *>();
                int count = int(float(base) + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * float(base));
                ArraySetLength(count + freighters, *enemies);
                int freighterType = enemyRace == 1 ? 13 : 15;
                for (int i = 0; i < freighters; ++i) {
                    (*enemies)[i] = createShip(enemyRace, 1, freighterType, playerRoute->getWaypoint(1), true, false);
                    (*enemies)[i]->setToSleep();
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                    static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                    int x = playerRoute->getWaypoint(1)->x;
                    x += static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                    int y = playerRoute->getWaypoint(1)->y;
                    y += static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                    int z = playerRoute->getWaypoint(1)->z;
                    z += static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                    (*enemies)[i]->setPosition(float(x), float(y), float(z));
                    Player *ship = (*enemies)[i]->player;
                    float health = float(ship->getMaxHitpoints()) * 0.7f;
                    float factor = *reinterpret_cast<float *>(Globals::options + 0x2c) > 0.7f ? 1.4f : 1.0f;
                    ship->setMaxHitpoints(int(health * factor));
                }
                for (unsigned i = freighters; i < enemies->size(); ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(enemyRace);
                    Route *spawnRoute = playerRoute;
                    int index = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                    (*enemies)[i] = createShip(enemyRace, 0, type, spawnRoute->getWaypoint(index), true, false);
                    (*enemies)[i]->setToSleep();
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                }
                objectivesA = new Objective(7, freighters, this);
                return;
            }
            case 12: {
                playerRoute = createRoute(static_cast<AERandom *>(Globals::rnd)->nextInt(2) + 3);
                int count = int(float(mission->getDifficulty()) / 10.0f * 4.0f) + 3;
                if ((count & 1) == 0) ++count;
                ++count;
                enemies = new Array<KIPlayer *>();
                ArraySetLength(count, *enemies);
                int agentRace = mission->getAgent()->getRace();
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(mission->getAgent()->getRace());
                (*enemies)[0] = createShip(agentRace, 0, type, nullptr, true, false);
                float x = egoPosition.x + float(static_cast<AERandom *>(Globals::rnd)->nextInt(1400) - 700);
                float y = egoPosition.y + float(static_cast<AERandom *>(Globals::rnd)->nextInt(1400) - 700);
                (*enemies)[0]->setPosition(x, y, egoPosition.z + 1000.0f);
                (*enemies)[0]->setSpeed(3.0f);
                static_cast<PlayerFighter *>((*enemies)[0])->setRotate(3);
                (*enemies)[0]->player->setHitpoints(9999999);
                (*enemies)[0]->setRoute(playerRoute->clone());
                (*enemies)[0]->player->setAlwaysFriend(true);
                (*enemies)[0]->name = mission->getAgent()->getName();
                (*enemies)[0]->cargo = nullptr;
                for (unsigned i = 1; i < enemies->size(); ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                    Route *spawnRoute = playerRoute;
                    int index = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                    (*enemies)[i] = createShip(8, 0, type, spawnRoute->getWaypoint(index), true, false);
                    (*enemies)[i]->setToSleep();
                }
                objectivesA = new Objective(20, 1, count, this);
                objectivesB = new Objective(21, 1, count, this);
                return;
            }
            case 15: {
                int coords[9];
                coords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000; coords[1] = 0; coords[2] = 70000;
                coords[3] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000; coords[4] = 0; coords[5] = 100000;
                coords[6] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000; coords[7] = 0; coords[8] = 140000;
                enemyRoute = new Route(coords, 3); // Original passes three words despite filling nine.
                float difficulty = float(Globals::status->getMission()->getDifficulty()) / 10.0f;
                int base = int(difficulty + difficulty) + 1;
                int count = int(float(base) + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * float(base));
                enemyRace = Globals::status->getStanding()->getEnemyRace(mission->getClientRace());
                int clientRace = mission->getClientRace();
                miningPlant = 1;
                enemies = new Array<KIPlayer *>();
                ArraySetLength(count + miningPlant + 2, *enemies);
                for (int i = 0; i < count; ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(enemyRace);
                    Route *spawnRoute = enemyRoute;
                    int index = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                    (*enemies)[i] = createShip(enemyRace, 0, type, spawnRoute->getWaypoint(index), true, false);
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                }
                miningPlantIndex = count;
                (*enemies)[miningPlantIndex] = reinterpret_cast<KIPlayer *>(createStaticObject(asteroidWaypoint, 19080, true));
                static_cast<PlayerFixedObject *>((*enemies)[miningPlantIndex])->setDockingType(1);
                static_cast<PlayerFixedObject *>((*enemies)[miningPlantIndex])->setMoving(false);
                (*enemies)[miningPlantIndex]->setPosition(Vector{field_c8, field_cc, field_d0});
                int dockingCoords[] = {int(field_c8 + 0.0f), int(field_cc + 0.0f), int(field_d0 - 30000.0f), -1, -1, -1};
                int times[] = {0, 6000};
                Array<KIPlayer *> *targets = new Array<KIPlayer *>();
                ArraySetLength(2, *targets);
                (*targets)[1] = (*enemies)[miningPlantIndex];
                Route *route = new Route(dockingCoords, targets, times, 6);
                route->setLoop(true);
                Route *clone = route->clone();
                for (int i = count + miningPlant; i < count + miningPlant + 2; ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(clientRace);
                    Route *assigned;
                    if (i == count + miningPlant) {
                        (*enemies)[i] = createShip(clientRace, 0, type, route->getWaypoint(0), true, false);
                        assigned = route;
                    } else {
                        (*enemies)[i] = createShip(clientRace, 0, type, clone->getWaypoint(0), true, false);
                        assigned = clone;
                    }
                    (*enemies)[i]->setRoute(assigned);
                    (*enemies)[i]->player->setAlwaysFriend(true);
                    (*enemies)[i]->player->setNeverAttack(true);
                }
                int amount = mission->getProductionGoodAmount();
                int index = mission->getProductionGoodIndex();
                objectivesA = new Objective(28, amount, index, this);
                return;
            }
            case 184: {
                int coords[9];
                coords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000; coords[1] = 0; coords[2] = 70000;
                coords[3] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000; coords[4] = 0; coords[5] = 100000;
                coords[6] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000; coords[7] = 0; coords[8] = 140000;
                enemyRoute = new Route(coords, 3);
                Globals::status->getMission()->getDifficulty();
                Globals::status->getStanding()->getEnemyRace(mission->getClientRace());
                int clientRace = mission->getClientRace();
                enemies = new Array<KIPlayer *>();
                ArraySetLength(4, *enemies);
                Waypoint *wp = new Waypoint(-30000, 0, 40000, nullptr);
                (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(wp, 19080, true));
                static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(false);
                static_cast<PlayerFixedObject *>((*enemies)[0])->setDockingType(2);
                static_cast<PlayerFixedObject *>((*enemies)[0])->setName(String(*Globals::gameText->getText(3208), false));
                wp = new Waypoint(30000, 0, 40000, nullptr);
                (*enemies)[1] = reinterpret_cast<KIPlayer *>(createStaticObject(wp, 18830, true));
                static_cast<PlayerFixedObject *>((*enemies)[1])->setMoving(false);
                static_cast<PlayerFixedObject *>((*enemies)[1])->setDockingType(1);
                static_cast<PlayerFixedObject *>((*enemies)[1])->setName(String(*Globals::gameText->getText(3209), false));
                int dockingCoords[] = {-1, -1, -1, 0, 0, 40000, -1, -1, -1}; // 0x1fd040
                int times[] = {10000, 0, 10000};
                Array<KIPlayer *> *targets = new Array<KIPlayer *>();
                ArraySetLength(3, *targets);
                (*targets)[0] = (*enemies)[0]; (*targets)[2] = (*enemies)[1];
                Route *route = new Route(dockingCoords, targets, times, 9);
                route->setLoop(true);
                Route *clone = route->clone();
                for (int i = 2; i != 4; ++i) {
                    int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(clientRace);
                    Route *assigned;
                    if (i == 2) {
                        (*enemies)[i] = createShip(clientRace, 0, type, route->getWaypoint(0), true, false);
                        assigned = route;
                    } else {
                        (*enemies)[i] = createShip(clientRace, 0, type, clone->getWaypoint(0), true, false);
                        assigned = clone;
                    }
                    (*enemies)[i]->setRoute(assigned);
                    (*enemies)[i]->player->setAlwaysFriend(true);
                    (*enemies)[i]->player->setNeverAttack(true);
                }
                int amount = mission->getProductionGoodAmount();
                int index = mission->getProductionGoodIndex();
                objectivesA = new Objective(29, amount, index, this);
                return;
            }
            case 183: {
                int coords[] = {-55000, 0, -35000, -30000, 0, -50000}; // 0x1fcff0 includes words 2..5.
                enemyRoute = new Route(coords, 6);
                enemies = new Array<KIPlayer *>();
                ArraySetLength(14, *enemies);
                for (int i = 0; i != 8; ++i) {
                    (*enemies)[i] = createShip(10, 0, 44, enemyRoute->getWaypoint(0), true, false);
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                    (*enemies)[i]->player->setMaxHitpoints(250);
                }
                (*enemies)[8] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 18781, false));
                static_cast<PlayerFixedObject *>((*enemies)[8])->setMoving(false);
                static_cast<PlayerFixedObject *>((*enemies)[8])->setDockingType(2);
                static_cast<PlayerFixedObject *>((*enemies)[8])->setName(String(*Globals::gameText->getText(3208), false));
                (*enemies)[8]->player->setAlwaysFriend(true);
                (*enemies)[8]->geometry->rotate(Vector{0.0f, -2.356194496154785f, 0.0f});
                (*enemies)[8]->shipGroup = 10;
                (*enemies)[8]->field_0x74 = 1;
                if ((*enemies)[8]->cargo) delete (*enemies)[8]->cargo;
                (*enemies)[8]->cargo = nullptr;
                Waypoint *wp = new Waypoint(-70000, -5000, -35000, nullptr);
                (*enemies)[9] = reinterpret_cast<KIPlayer *>(createStaticObject(wp, 18804, true));
                (*enemies)[9]->setPosition(wp->getPosition());
                static_cast<PlayerFixedObject *>((*enemies)[9])->setMoving(false);
                (*enemies)[9]->field_0x70 = 0;
                static_cast<PlayerFixedObject *>((*enemies)[9])->setDockingType(1);
                (*enemies)[9]->player->setAlwaysFriend(true);
                (*enemies)[9]->player->setHitpoints(9999999);
                (*enemies)[9]->shipGroup = 10;
                (*enemies)[9]->field_0x74 = 1;
                if ((*enemies)[9]->cargo) delete (*enemies)[9]->cargo;
                (*enemies)[9]->cargo = nullptr;
                int dockingCoords[] = {-1, -1, -1, 30000, 1000, 40000, -1, -1, -1};
                int times[] = {20000, 0, 20000};
                Array<KIPlayer *> *targets = new Array<KIPlayer *>();
                ArraySetLength(3, *targets);
                (*targets)[0] = (*enemies)[9]; (*targets)[2] = (*enemies)[8];
                Route *route = new Route(dockingCoords, targets, times, 9);
                route->setLoop(true);
                for (int i = 10; i != 14; ++i) {
                    (*enemies)[i] = createShip(0, 0, 51, route->getWaypoint(0), true, false);
                    (*enemies)[i]->setRoute(route->clone());
                    (*enemies)[i]->player->setAlwaysFriend(true);
                    // Original STRH clears fighter extension bytes +0x13d/+0x13e.
                    *(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 0x19) = 0;
                    *(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 0x1a) = 0;
                    Vector position;
                    switch ((i - 10) & 0x7fffffff) {
                        case 2: position.x = 25000.0f; position.y = 0.0f; position.z = -36000.0f; break;
                        case 1: position.x = 10000.0f; position.y = -7000.0f; position.z = -30000.0f; break;
                        case 0: position.x = 20000.0f; position.y = -3000.0f; position.z = -50000.0f; break;
                        default: position.x = 12000.0f; position.y = 6000.0f; position.z = -45000.0f; break;
                    }
                    (*enemies)[i]->setPosition(position);
                }
                player->setPosition(-30000.0f, 5000.0f, -100000.0f);
                missionSpawnPosition = player->getPosition();
                timeLimit = 181000;
                objectivesA = new Objective(3, 181000, this);
                return;
            }
            default: return;
        }
    }
    static_cast<AERandom *>(Globals::rnd)->reset();
    missionPtr = 0;
    createScene();
    missionPtr = 3;
    bool earlySystem = false;
    if (Globals::status->getSystem()->getIndex() == 15)
        earlySystem = Globals::status->getCurrentCampaignMission() < 16;
    int stationIndex = Globals::status->getStation()->getIndex();
    int attackRoll = static_cast<AERandom *>(Globals::rnd)->nextInt(100);
    Mission *freelance = Globals::status->getFreelanceMission();
    int freelanceEnemies = 0;
    if (freelance && (freelance->getType() == 0 || freelance->getType() == 11 || freelance->getType() == 15))
        freelanceEnemies = int(float(freelance->getDifficulty()) / 10.0f * 5.0f);
    if (Globals::status->getSystem() && Globals::status->getSystem()->hasPirateBase())
        freelanceEnemies = static_cast<AERandom *>(Globals::rnd)->nextInt(1) + 2;
    int security = Globals::status->getSystem()->getSecurityLevel();
    security -= static_cast<unsigned char>(Globals::status->hardCoreMode()) & (security > 0);
    bool attackers;
    if (earlySystem && !Globals::status->hardCoreMode()) attackers = false;
    else {
        int probability = security == 0 ? 90 : security == 1 ? 65 : security == 2 ? 35 : 10;
        attackers = attackRoll < probability;
    }
    int hostileCoords[3];
    hostileCoords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(100000) - 50000;
    hostileCoords[1] = 0;
    hostileCoords[2] = static_cast<AERandom *>(Globals::rnd)->nextInt(50000) + 50000;
    field_180 = new Route(hostileCoords, 3);
    int race = Globals::status->getSystem()->getRace();
    int enemyRace;
    if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) >= 75)
        enemyRace = Globals::status->getStanding()->getEnemyRace(race);
    else enemyRace = 8;
    hostileCount = attackers ? static_cast<AERandom *>(Globals::rnd)->nextInt(4) : 0;
    if (hostileCount >= 1) {
        if (Globals::status->hardCoreMode()) hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(6) + 2;
        hostileCount = int(float(hostileCount) + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * float(hostileCount));
        hostileCount += Globals::status->getLevel() / 4;
    }
    if (security == 3 && attackers && *reinterpret_cast<float *>(Globals::options + 0x2c) < 1.0f)
        hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(2) + 1;
    int additionalFriends = 0;
    if (stationIndex == 78) { field_164 = 0; field_168 = 0; }
    else {
        field_164 = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        field_168 = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
        additionalFriends = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
    }
    if (earlySystem) security = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
    friendCount = security + additionalFriends + field_168 / 4;
    if (attackers && Globals::status->getCurrentCampaignMission() >= 32 && static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 7)
        friendCount = hostileCount = 9;
    int pirateBase = static_cast<int>(Globals::status->getStation()->stationHasPirateBase());
    int pirateGuards = pirateBase ? int((*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * 5.0f + 5.0f) : 0;
    if (Globals::status->getSystem() && Globals::status->getSystem()->hasPirateBase()) {
        freelanceEnemies = static_cast<AERandom *>(Globals::rnd)->nextInt(1) + 2;
        if (Globals::status->hardCoreMode()) freelanceEnemies = static_cast<AERandom *>(Globals::rnd)->nextInt(3) + 2 * freelanceEnemies;
        attackers = false; hostileCount = 0;
    }
    int blueprint = static_cast<int>(Globals::status->getStation()->stationHasHiddenBlueprint(true));
    if (comingFromAlienWorld || Globals::status->getStation()->isAttackedByAliens()) {
        if (Globals::status->getCurrentCampaignMission() != 42) {
            if (field_168 <= 2) field_168 = 2;
            hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(4) + 2;
            if (landmarks && (*landmarks)[3]) field_180->setNewCoords((*landmarks)[3]->getPosition());
            attackers = true; enemyRace = 9;
        }
    }
    Wanted *wanted = Globals::status->getWantedInCurrentOrbit();
    if (wanted && friendCount >= 3) friendCount = 2;
    float ninjaChanceFloat = 0.0f;
    if (Globals::status->getCurrentCampaignMission() >= 101 && Globals::status->getCurrentCampaignMission() <= 144)
        ninjaChanceFloat = float(Globals::status->getCurrentCampaignMission()) / 144.0f * 15.0f + 5.0f;
    int ninjaChance = int(ninjaChanceFloat) << Globals::status->hardCoreMode();
    int ninjas = 0;
    if (ninjaChance >= 1 && static_cast<AERandom *>(Globals::rnd)->nextInt(100) < ninjaChance)
        ninjas = (static_cast<AERandom *>(Globals::rnd)->nextInt(3) + 2) << Globals::status->hardCoreMode();
    bool namedFirstFriend = false;
    if (freelance && freelance->getType() == 13 && !Globals::status->field_0xf1 && freelance->getTargetStation() == Globals::status->getStation()->getIndex()) {
        field_164 = field_168 = freelanceEnemies = hostileCount = 0; attackers = false;
        friendCount = Globals::status->field_0xf0 ? 6 : 7;
        namedFirstFriend = !Globals::status->field_0xf0;
    } else {
        if (Globals::status->getStation()->hasAttackedFriends()) {
            if (friendCount <= 7) friendCount = 7;
            alarmRequested = field_18a = 1;
        }
        if (field_164 + friendCount + field_168 + hostileCount + freelanceEnemies == 0) friendCount = 4;
    }
    int currentStation = Globals::status->getStation()->getIndex();
    if (currentStation == 102 || currentStation == 103 || currentStation == 104) {
        friendCount = field_164 = field_168 = 0;
        hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(5) + 3;
        attackers = true; enemyRace = 8;
    } else if (currentStation == 100 || currentStation == 101 || currentStation == 108 || currentStation == 10) {
        friendCount = field_164 = field_168 = hostileCount = 0; attackers = false;
    }
    if ((Globals::status->getCurrentCampaignMission() == 36 || Globals::status->getCurrentCampaignMission() == 37)
        && Globals::status->getSystem() && Globals::status->getSystem()->getIndex() == 5) {
        hostileCount = friendCount = 0; attackers = false;
    }
    if (Globals::status->getCurrentCampaignMission() >= 42 && Globals::status->getCurrentCampaignMission() <= 43 && !Globals::status->inAlienOrbit()) {
        freelanceEnemies = hostileCount = 0; attackers = false;
    }
    if (Globals::status->inBlackMarketSystem()) {
        freelanceEnemies = friendCount = 0;
        hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(4) + 6;
        if (Globals::status->hardCoreMode()) hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(3) + 2 * hostileCount;
        field_168 = field_164 = 0; attackers = true; enemyRace = 8;
    }
    if (Globals::status->inPirateLootOrbit()) {
        freelanceEnemies = friendCount = 0;
        hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(4) + 10;
        if (Globals::status->hardCoreMode()) hostileCount = static_cast<AERandom *>(Globals::rnd)->nextInt(3) + 2 * hostileCount;
        field_168 = field_164 = 0; attackers = true; enemyRace = 8;
    }
    if (Globals::status->getStation()->getIndex() == 108) {
        if (Globals::status->field_114) { attackers = false; hostileCount = 0; }
        else {
            friendCount = field_164 = field_168 = 0;
            field_288 = 1;
            hostileCount = Globals::status->hardCoreMode() ? 8 : 6;
            enemies = new Array<KIPlayer *>();
            ArraySetLength(hostileCount + 4, *enemies);
            for (int i = 0; i != 4; ++i) {
                (*enemies)[i] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 14243, true));
                (*enemies)[i]->name = *Globals::gameText->getText(441);
            }
            (*enemies)[0]->setPosition(45000.0f, 5000.0f, 55000.0f);
            (*enemies)[1]->setPosition(80000.0f, -30000.0f, -12000.0f);
            (*enemies)[2]->setPosition(-15000.0f, 45000.0f, 55000.0f);
            (*enemies)[3]->setPosition(-65000.0f, -20000.0f, -45000.0f);
            for (unsigned i = 4; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, nullptr, true, false);
            }
            // Android data 0x2151c0 is the seven-part client image used here.
            static int clientImage[] = {4, 2, 0, 2, 2, 0, 0};
            Mission *generated = new Mission(4, String(*Globals::gameText->getText(1601), false), clientImage, 4, 0, 108, 0);
            Globals::status->setFreelanceMission(generated);
            Globals::status->setMission(generated);
            objectivesA = new Objective(18, 0, enemies->size(), this);
            return;
        }
    }
    bool guardedFreighter = false;
    if (Globals::status->getCampaignMission() && reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getType() == 163 && Globals::status->field_90) {
        for (unsigned i = 0; i < Globals::status->field_90->size(); ++i) {
            if ((*Globals::status->field_90)[i] == Globals::status->getStation()->getIndex()) { guardedFreighter = true; break; }
        }
    }
    int friendCoords[3];
    friendCoords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
    friendCoords[1] = static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
    friendCoords[2] = static_cast<AERandom *>(Globals::rnd)->nextInt(30000) + 20000;
    Route *ambientFriendRoute = new Route(friendCoords, 3);
    if (guardedFreighter) {
        hostileCount = field_164 = 0;
        enemies = new Array<KIPlayer *>();
        ArraySetLength(friendCount + 8, *enemies);
        int distance = static_cast<AERandom *>(Globals::rnd)->nextInt(30000) + 80000;
        int coords[3];
        coords[0] = (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance;
        coords[1] = static_cast<AERandom *>(Globals::rnd)->nextInt(3000) - 6000;
        coords[2] = static_cast<AERandom *>(Globals::rnd)->nextInt(50000) + 120000;
        playerRoute = new Route(coords, 3);
        enemyRoute = playerRoute->clone();
        enemyRoute->setLoop(true);
        (*enemies)[0] = createShip(race, 1, race == 1 ? 13 : 15, playerRoute->getWaypoint(), true, false);
        static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(false);
        (*enemies)[0]->setPosition(float(coords[0]), float(coords[1]), float(coords[2]));
        if ((*enemies)[0]->cargo) {
            int item = static_cast<AERandom *>(Globals::rnd)->nextInt(6);
            static const int cargoItems[] = {36, 22, 23}; // 0x1fdb90
            if (unsigned(item - 3) < 3) item = cargoItems[item - 3];
            (*(*enemies)[0]->cargo)[0] = item;
            (*enemies)[0]->name = *Globals::gameText->getText(1664);
        }
        for (int i = 1; i != 6; ++i) {
            int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);
            (*enemies)[i] = createShip(race, 0, type, playerRoute->getWaypoint(), true, false);
            (*enemies)[i]->setRoute(enemyRoute->clone());
        }
        Matrix matrix;
        const float identity[] = {1,0,0,0,0,1,0,0,0,0,1,0,1,1,1};
        std::memcpy(&matrix, identity, sizeof(matrix));
        Vector rotated{0.0f, 0.0f, 0.0f};
        Vector position = (*enemies)[0]->getPosition();
        static const float freighterTurrets[4][2][7] = {
            {{0.0f, 0.0f, -1097.1400146484375f, -4178.22998046875f, 0.0f, 0.0f, 3.1415927410125732f}, {0.0f, 0.0f, 1158.0899658203125f, 1180.5899658203125f, 0.0f, 0.0f, 0.0f}},
            {{1.0f, 0.0f, -1096.97998046875f, -2691.52001953125f, 0.0f, 0.0f, 3.1415927410125732f}, {1.0f, 0.0f, 1893.47998046875f, 1068.0699462890625f, 0.0f, 0.0f, 0.0f}},
            {{2.0f, 0.0f, -484.718994140625f, 1741.219970703125f, 0.0f, 0.0f, 3.1415927410125732f}, {2.0f, 0.0f, 1458.8399658203125f, -2304.280029296875f, 0.0f, 0.0f, 0.0f}},
            {{3.0f, 0.0f, -516.0800170898438f, -3744.449951171875f, 0.0f, 0.0f, 3.1415927410125732f}, {3.0f, 0.0f, 515.7659912109375f, -3744.449951171875f, 0.0f, 0.0f, 0.0f}}
        }; // Android rodata 0x1fcc94
        for (int i = 0; i != 2; ++i) {
            (*enemies)[i + 6] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 14363, true));
            (*enemies)[i + 6]->player->setMaxHitpoints(1000);
            const float *record = freighterTurrets[race][i];
            rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, Vector{record[4], record[5], record[6]});
            (*enemies)[i + 6]->geometry->setRotation(rotated);
            rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, Vector{record[1], record[2], record[3]});
            (*enemies)[i + 6]->setPosition(Vector{position.x + rotated.x, position.y + rotated.y, position.z + rotated.z});
            static_cast<PlayerTurret *>((*enemies)[i + 6])->setScaling(0.3f);
            (*enemies)[i + 6]->shipGroup = race;
        }
        for (int i = 8; i < friendCount + 8; ++i) {
            int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);
            (*enemies)[i] = createShip(race, 0, type, ambientFriendRoute->getWaypoint(), true, false);
        }
        return;
    }
    bool miningStation = false;
    if (Globals::status->getCampaignMission() && reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getType() == 167)
        miningStation = Globals::status->getStation()->getIndex() == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation();
    bool largeOutpost = false, smallOutpost = false, alternateLarge = false;
    if (race == 0 && field_168 > 0) {
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 29) {
            if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 29) {
                alternateLarge = Globals::status->getCurrentCampaignMission() > 103;
                largeOutpost = true;
            } else largeOutpost = true;
        }
    } else if (race == 1 && field_168 > 0) {
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 29)
            smallOutpost = Globals::status->getCurrentCampaignMission() > 140;
    } else if (race != 1) {
        largeOutpost = false;
    }
    if (Globals::status->inSupernovaSystem()) {
        friendCount = field_164 = field_168 = hostileCount = 0;
        largeOutpost = false; attackers = false;
    }
    if (wanted) friendCount += wanted->getNumWingmen() + 1;
    int largeTurrets = largeOutpost ? (alternateLarge ? 8 : 7) : 0;
    int smallTurrets = smallOutpost ? 5 : 0;
    enemies = new Array<KIPlayer *>();
    ArraySetLength(friendCount + field_164 + field_168 + hostileCount + freelanceEnemies + largeTurrets + smallTurrets + pirateBase + pirateGuards + ninjas + miningStation + blueprint, *enemies);
    for (int i = 0; i < friendCount; ++i) {
        if (i == 0 && wanted) {
            int wantedRace = unsigned(wanted->getRace()) > 3 ? 8 : wanted->getRace();
            int type = wanted->getShip();
            (*enemies)[0] = createShip(wantedRace, 0, type, ambientFriendRoute->getWaypoint(), true, false);
            int health = wanted->getHitpoints();
            int level = Globals::status->getLevel() <= 20 ? Globals::status->getLevel() : 20;
            health += 15 * level;
            int campaign = Globals::status->gameWon() ? 45 : Globals::status->getCurrentCampaignMission();
            health += 4 * campaign;
            int adjustedHealth = int(float(health) + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * float(health));
            (*enemies)[0]->player->setMaxHitpoints(adjustedHealth);
            (*enemies)[0]->player->setHitpoints(adjustedHealth);
            (*enemies)[0]->name = wanted->getName();
            if ((*enemies)[0]->cargo) delete (*enemies)[0]->cargo;
            (*enemies)[0]->cargo = nullptr;
            (*enemies)[0]->cargo = new Array<int>();
            ArrayAdd(wanted->getLoot(), *(*enemies)[0]->cargo);
            ArrayAdd(wanted->getLootAmount(), *(*enemies)[0]->cargo);
            (*enemies)[0]->field_0x42 = 1;
            (*enemies)[0]->field_0x48 = wanted->getIndex();
            (*enemies)[0]->setSpeed(4.5f);
            for (int j = 1; j <= wanted->getNumWingmen(); ++j) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(wantedRace);
                (*enemies)[j] = createShip(wantedRace, 0, type, ambientFriendRoute->getWaypoint(), true, false);
                (*enemies)[j]->player->setMaxHitpoints(adjustedHealth / 2);
                (*enemies)[j]->player->setHitpoints(adjustedHealth / 2);
            }
        } else if (!wanted || i > wanted->getNumWingmen()) {
            int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);
            (*enemies)[i] = createShip(race, 0, type, ambientFriendRoute->getWaypoint(), true, false);
        }
    }
    int index = friendCount;
    for (; index < friendCount + field_164; ++index) {
        int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(race);
        (*enemies)[index] = createShip(race, 0, type, nullptr, true, false);
        (*enemies)[index]->setDead();
        int coords[3];
        coords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(400000) - 200000;
        coords[1] = static_cast<AERandom *>(Globals::rnd)->nextInt(200000) - 100000;
        coords[2] = static_cast<AERandom *>(Globals::rnd)->nextInt(100000) + 50000;
        (*enemies)[index]->setRoute(new Route(coords, 3));
        (*enemies)[index]->setJumper(true);
    }
    int foreignRace = race == 2 ? 0 : race;
    for (; index < friendCount + field_164 + field_168; ++index) {
        int start = friendCount + field_164;
        if (index == start && largeOutpost) {
            if (alternateLarge) {
                Vector position;
                position.x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);
                position.y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(40000) - 20000);
                position.z = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 100000);
                Waypoint *wp = new Waypoint(int(position.x), int(position.y), int(position.z), nullptr);
                (*enemies)[index] = reinterpret_cast<KIPlayer *>(createStaticObject(wp, 18804, true));
                (*enemies)[index]->setSpacePoints(nullptr);
                (*enemies)[index]->setPosition(position);
                static_cast<PlayerFixedObject *>((*enemies)[index])->setMoving(false);
                (*enemies)[index]->player->setHitpoints(9999999);
                if ((*enemies)[index]->cargo) delete (*enemies)[index]->cargo;
                (*enemies)[index]->cargo = nullptr;
                (*enemies)[index]->field_0x70 = 0;
            } else (*enemies)[index] = createShip(race, 1, 14, nullptr, true, false);
            static_cast<PlayerFixedObject *>((*enemies)[index])->setMoving(false);
            KIPlayer *outpost = (*enemies)[index];
            float x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);
            float y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(10000) - 5000);
            float z = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 40000);
            outpost->setPosition(x, y, z);
        } else if (index == start && smallOutpost) {
            Vector position;
            position.x = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) - 40000);
            position.y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(40000) - 20000);
            position.z = float(static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 60000);
            Waypoint *wp = new Waypoint(int(position.x), int(position.y), int(position.z), nullptr);
            (*enemies)[index] = reinterpret_cast<KIPlayer *>(createStaticObject(wp, 19051, true));
            (*enemies)[index]->setSpacePoints(nullptr);
            (*enemies)[index]->setPosition(position);
            static_cast<PlayerFixedObject *>((*enemies)[index])->setMoving(false);
            (*enemies)[index]->player->setHitpoints(9999999);
            static_cast<PlayerFixedObject *>((*enemies)[index])->setName(String(*Globals::gameText->getText(1667), false));
            (*enemies)[index]->field_0x70 = 0;
            if ((*enemies)[index]->cargo) delete (*enemies)[index]->cargo;
            (*enemies)[index]->cargo = nullptr;
        } else {
            bool foreign = static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 29;
            int shipRace = foreign ? (race == 0 ? 2 : foreignRace) : race;
            (*enemies)[index] = createShip(shipRace, 1, shipRace == 1 ? 13 : 15, nullptr, true, false);
            static_cast<PlayerFixedObject *>((*enemies)[index])->setMoving(true);
            KIPlayer *freighter = (*enemies)[index];
            int distance = static_cast<AERandom *>(Globals::rnd)->nextInt(60000) - 80000;
            float x = float((static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance);
            float y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(40000) - 20000);
            float z = float(static_cast<AERandom *>(Globals::rnd)->nextInt(160000) - 80000);
            freighter->setPosition(x, y, z);
        }
    }
    if (largeTurrets) {
        Matrix matrix;
        const float identity[] = {1,0,0,0,0,1,0,0,0,0,1,0,1,1,1};
        std::memcpy(&matrix, identity, sizeof(matrix));
        Vector rotated{0.0f, 0.0f, 0.0f};
        Vector position = (*enemies)[friendCount + field_164]->getPosition();
        static const float alternateTurrets[8][6] = {
            {-6725.87890625f, 2458.2626953125f, -66.0760498046875f, -0.0f, 1.570796012878418f, 0.0f},
            {5824.43408203125f, 2458.2626953125f, 10243.14453125f, -0.0f, -1.570796012878418f, -0.0f},
            {-6725.87890625f, 2458.2626953125f, 2099.96435546875f, -0.0f, 1.570796012878418f, 0.0f},
            {5824.43408203125f, 2458.2626953125f, 8436.650390625f, -0.0f, -1.570796012878418f, -0.0f},
            {-5548.7109375f, -2743.15185546875f, 11954.466796875f, -3.1415929794311523f, 1.570796012878418f, -0.0f},
            {-5548.7109375f, -2743.15185546875f, -3710.50390625f, -3.1415929794311523f, 1.570796012878418f, -0.0f},
            {4981.2587890625f, -2743.15185546875f, 14659.005859375f, 3.1415929794311523f, -2.1816608905792236f, 0.0f},
            {0.0f, 2458.2626953125f, -32811.1015625f, -0.0f, -0.0f, -0.0f}
        }; // Android rodata 0x1fcd74
        static const float largeTurretRecords[7][6] = {
            {5321.740234375f, 1893.0999755859375f, -15570.7001953125f, 0.0f, 0.0f, -1.5707963705062866f},
            {2355.669921875f, 1115.47998046875f, 7324.10009765625f, 0.0f, 0.0f, -1.5707963705062866f},
            {0.0f, -4261.27978515625f, 4876.27978515625f, 0.0f, 0.0f, 3.1415927410125732f},
            {-5321.740234375f, 1893.0999755859375f, -15570.7001953125f, 0.0f, 0.0f, 1.5707963705062866f},
            {0.0f, 5668.83984375f, -13871.7001953125f, 0.0f, 0.0f, 0.0f},
            {-2355.669921875f, 1115.47998046875f, 7324.10009765625f, 0.0f, 0.0f, 1.5707963705062866f},
            {0.00020068499725311995f, 2855.300048828125f, 4161.14990234375f, 0.0f, 0.0f, 0.0f}
        }; // Android rodata 0x1fce34
        for (int i = 0; index < friendCount + field_164 + field_168 + largeTurrets; ++index, ++i) {
            (*enemies)[index] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 6772, true));
            PlayerTurret *turret = static_cast<PlayerTurret *>((*enemies)[index]);
            turret->player->setMaxHitpoints(1000);
            const float *record = alternateLarge ? alternateTurrets[i] : largeTurretRecords[i];
            rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, Vector{record[3], record[4], record[5]});
            turret->geometry->setRotation(rotated);
            rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, Vector{record[0], record[1], record[2]});
            turret->setPosition(Vector{position.x + rotated.x, position.y + rotated.y, position.z + rotated.z});
            turret->setScaling(6.0f);
            turret->cargo = nullptr;
            turret->shipGroup = race;
        }
    }
    if (smallOutpost) {
        Matrix matrix;
        const float identity[] = {1,0,0,0,0,1,0,0,0,0,1,0,1,1,1};
        std::memcpy(&matrix, identity, sizeof(matrix));
        Vector rotated{0.0f, 0.0f, 0.0f};
        Vector position = (*enemies)[friendCount + field_164]->getPosition();
        static const float smallTurretRecords[5][6] = {
            {10283.1796875f, -1122.7913818359375f, 26039.26171875f, 0.0f, 0.0f, 0.0f},
            {-10171.173828125f, -1082.7020263671875f, 25906.552734375f, 0.0f, 0.0f, 0.0f},
            {-15624.38671875f, -787.19921875f, -7569.43017578125f, 0.0f, 0.0f, 0.0f},
            {15624.38671875f, -787.19921875f, -7569.43017578125f, 0.0f, 0.0f, 0.0f},
            {0.0f, 4901.47314453125f, 3083.96435546875f, 0.0f, 0.0f, 0.0f}
        }; // Android rodata 0x1fcedc
        for (int i = 0; index < friendCount + field_164 + field_168 + smallTurrets; ++index, ++i) {
            (*enemies)[index] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 6774, true));
            PlayerTurret *turret = static_cast<PlayerTurret *>((*enemies)[index]);
            turret->player->setMaxHitpoints(1000);
            const float *record = smallTurretRecords[i];
            rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, Vector{record[3], record[4], record[5]});
            turret->geometry->setRotation(rotated);
            rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, Vector{record[0], record[1], record[2]});
            turret->setPosition(Vector{position.x + rotated.x, position.y + rotated.y, position.z + rotated.z});
            turret->setScaling(6.0f);
            turret->cargo = nullptr;
            turret->shipGroup = 1;
        }
    }
    if (attackers) {
        int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(enemyRace);
        bool hardcore = Globals::status->hardCoreMode();
        if (enemyRace == 8 && initStreamOutPosition) {
            int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(100);
            int threshold = hardcore ? 40 : 20;
            if (roll < Globals::status->getLevel() + threshold) {
                missionSpawnPosition = player->getPosition() / 3.0f;
                field_180->setNewCoords(missionSpawnPosition);
            }
        }
        for (; index < friendCount + field_164 + field_168 + largeTurrets + smallTurrets + hostileCount; ++index) {
            (*enemies)[index] = createShip(enemyRace, 0, type, field_180->getWaypoint(), true, false);
            if (Globals::status->inPirateLootOrbit()) {
                Player *ship = (*enemies)[index]->player;
                ship->setHitpoints(ship->getMaxHitpoints() * 2);
                (*enemies)[index]->setSpeed(3.5f);
                static_cast<PlayerFighter *>((*enemies)[index])->setRotate(3);
                missionSpawnPosition = player->getPosition();
                int distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
                missionSpawnPosition.x += float((static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance + 20000);
                distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
                missionSpawnPosition.y += float((static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance + 10000);
                distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
                missionSpawnPosition.z += float((static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance + 20000);
                (*enemies)[index]->setPosition(missionSpawnPosition);
            }
        }
    } else index = friendCount + field_164 + field_168 + largeTurrets + smallTurrets + hostileCount;
    for (; index < friendCount + field_164 + field_168 + largeTurrets + smallTurrets + hostileCount + freelanceEnemies; ++index) {
        int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
        (*enemies)[index] = createShip(8, 0, type, nullptr, true, false);
        missionSpawnPosition = player->getPosition();
        KIPlayer *spawned = (*enemies)[index];
        float x = missionSpawnPosition.x - 80000.0f + float(static_cast<AERandom *>(Globals::rnd)->nextInt(160000));
        float y = missionSpawnPosition.y - 50000.0f + float(static_cast<AERandom *>(Globals::rnd)->nextInt(100000));
        float z = missionSpawnPosition.z - 80000.0f + float(static_cast<AERandom *>(Globals::rnd)->nextInt(160000));
        spawned->setPosition(x, y, z);
    }
    if (pirateBase) {
        Vector basePosition{0.0f, 0.0f, 0.0f};
        static const int piratePositions[4][3] = {
            {-200000, -100000, 50000},
            {50000, 200000, -100000},
            {140000, -150000, 7000},
            {170000, -170000, 10000}
        }; // Android rodata 0x1fcf54
        for (; index < friendCount + field_164 + field_168 + largeTurrets + smallTurrets + hostileCount + freelanceEnemies + pirateBase; ++index) {
            int base = Globals::status->getStation()->getPirateStationIndex();
            int coords[] = {piratePositions[base][0], piratePositions[base][1], piratePositions[base][2]};
            Route *route = new Route(coords, 3);
            (*enemies)[index] = reinterpret_cast<KIPlayer *>(createStaticObject(route->getWaypoint(), 14243, true));
            basePosition = route->getWaypoint()->getPosition();
            (*enemies)[index]->setToSleep();
            (*enemies)[index]->name = *Globals::gameText->getText(441);
        }
        for (; index < friendCount + field_164 + field_168 + largeTurrets + smallTurrets + hostileCount + freelanceEnemies + pirateBase + pirateGuards; ++index) {
            int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
            (*enemies)[index] = createShip(8, 0, type, nullptr, true, false);
            int dx = static_cast<AERandom *>(Globals::rnd)->nextInt(20000);
            int sx = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
            int dy = static_cast<AERandom *>(Globals::rnd)->nextInt(20000);
            int sy = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
            int dz = static_cast<AERandom *>(Globals::rnd)->nextInt(20000);
            int sz = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
            (*enemies)[index]->setPosition(float(int(basePosition.x + float((sx == 0 ? 1 : -1) * dx + 10000))),
                                          float(int(basePosition.y + float((sy == 0 ? 1 : -1) * dy + 10000))),
                                          float(int(basePosition.z + float((sz == 0 ? 1 : -1) * dz + 10000))));
            (*enemies)[index]->setToSleep();
            // Fighter extension byte +0x12f; do not use the duplicated derived fields.
            *(reinterpret_cast<unsigned char *>((*enemies)[index]) + sizeof(KIPlayer) + 0x0b) = 1;
        }
    }
    if (ninjas >= 1) {
        missionSpawnPosition = player->getPosition();
        int distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
        missionSpawnPosition.x += float((static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance + 20000);
        distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
        missionSpawnPosition.y += float((static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance + 10000);
        distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
        missionSpawnPosition.z += float((static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1) * distance + 20000);
        int coords[] = {int(missionSpawnPosition.x), int(missionSpawnPosition.y), int(missionSpawnPosition.z)};
        Route *route = new Route(coords, 3);
        for (; index < friendCount + field_164 + field_168 + largeTurrets + smallTurrets + hostileCount + freelanceEnemies + pirateBase + pirateGuards + ninjas; ++index) {
            (*enemies)[index] = createShip(10, 0, 44, route->getWaypoint(), true, false);
            (*enemies)[index]->player->setAlwaysEnemy(true);
        }
        delete route;
    }
    if (miningStation) {
        miningPlantIndex = friendCount + field_164 + field_168 + hostileCount + freelanceEnemies + largeTurrets + smallTurrets + pirateBase + pirateGuards + ninjas;
        Waypoint *wp = new Waypoint(-90000, 0, 20000, nullptr);
        (*enemies)[miningPlantIndex] = reinterpret_cast<KIPlayer *>(createStaticObject(wp, 19080, true));
        static_cast<PlayerFixedObject *>((*enemies)[miningPlantIndex])->setDockingType(1);
        static_cast<PlayerFixedObject *>((*enemies)[miningPlantIndex])->setMoving(false);
        (*enemies)[miningPlantIndex]->setPosition(Vector{field_c8, field_cc, field_d0});
        (*enemies)[miningPlantIndex]->player->setAlwaysFriend(true);
        static_cast<PlayerFixedObject *>((*enemies)[miningPlantIndex])->setName(String(*Globals::gameText->getText(3210), false));
    }
    if (blueprint) {
        int hidden = Globals::status->getStation()->getHiddenBlueprintIndex();
        static const int blueprintRaces[5] = {
            1, 3, 2, 0, 2
        }; // Android rodata 0x1fcf84
        static const float blueprintPositions[5][3] = {
            {-20000.0f, 30000.0f, 80000.0f},
            {40000.0f, -30000.0f, 100000.0f},
            {-80000.0f, 80000.0f, -90000.0f},
            {40000.0f, 20000.0f, 140000.0f},
            {40000.0f, 20000.0f, 140000.0f}
        }; // Android rodata 0x1fcf98
        static const int blueprintMeshes[5] = {
            14, 11, 12, 13, 12
        }; // Android rodata 0x1fcfd4
        int target = friendCount + field_164 + field_168 + hostileCount + freelanceEnemies + largeTurrets + smallTurrets + pirateBase + pirateGuards + ninjas + miningStation;
        (*enemies)[target] = createShip(blueprintRaces[hidden], 1, hidden == 0 ? 13 : 15, nullptr, true, false);
        PlayerFixedObject *object = static_cast<PlayerFixedObject *>((*enemies)[target]);
        object->setDockingType(3);
        object->field_0x70 = Globals::status->getStation()->stationHasHiddenBlueprint(false);
        (*enemies)[target]->setPosition(Vector{field_c8, field_cc, field_d0});
        object->setDeadButSelectable();
        (*enemies)[target]->setPosition(blueprintPositions[hidden][0], blueprintPositions[hidden][1], blueprintPositions[hidden][2]);
        if (object->cargo) delete object->cargo;
        object->cargo = nullptr;
        if (Globals::status->getStation()->stationHasHiddenBlueprint(false)) {
            Generator *generator = new Generator();
            object->cargo = generator->getLootList(115, 1);
            delete generator;
        }
        FileRead *reader = new FileRead();
        Array<SpacePoint *> *points = reader->loadSpacePoints(blueprintMeshes[hidden], -1);
        delete reader;
        object->setSpacePoints(points);
    }
    if (namedFirstFriend) (*enemies)[0]->name = *Globals::gameText->getText(1663);
}


#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(Waypoint) == 0x138, "Waypoint ARM allocation size");
static_assert(sizeof(AEGeometry) == 0xc0, "AEGeometry ARM allocation size");
#endif
static_assert(sizeof(BoundingSphere) <= 0x48, "BoundingSphere allocation bound");

void Level::createAsteroids() {
    AERandom *rng = static_cast<AERandom *>(Globals::rnd);

    int colBase;
    if (Status::gStatus->inAlienOrbit() == 0) {
        Status::gStatus->getSystem();
        colBase = (((SolarSystem *) Status::gStatus->getSystem())->getIndex() == 0x16) ? 2 : 0;
    } else {
        colBase = 0;
    }

    this->asteroids = new Array<KIPlayer *>();

    Galaxy *gal = Galaxy::gGalaxy;
    Station *st = (Station *) Status::gStatus->getStation();
    int *prob = static_cast<int *>(gal->getAsteroidProbabilities(st));

    Station *st2 = (Station *) Status::gStatus->getStation();
    rng->setSeed(st2->getIndex());

    int countRoll = rng->nextInt(0x28);
    ArraySetLength((unsigned) (countRoll + 0x28), *(this->asteroids));

    int rx = rng->nextInt(100000);
    int ry = rng->nextInt(100000);
    int rz = rng->nextInt(100000);

    int alien = Status::gStatus->inAlienOrbit();
    int camp = Status::gStatus->getCurrentCampaignMission();

    int ox, oy, oz;
    if (alien != 0) {
        oy = 0;
        oz = 30000;
        ox = -30000;
        if (camp == 0x9a)
            ox = -70000;
    } else {
        bool placed = false;
        if (camp == 0x72) {
            Station *s = (Station *) Status::gStatus->getStation();
            if (((Station *) s)->getIndex() == 0x53) {
                oz = 80000;
                ox = 30000;
                oy = 0;
                placed = true;
            }
        }
        if (!placed && camp == 0x59 && Status::gStatus->inSupernovaOrbit() != 0) {
            ox = -100000;
            oz = -50000;
            oy = 0;
            placed = true;
        }
        if (!placed && camp == 0x5b) {
            Station *s = (Station *) Status::gStatus->getStation();
            if (((Station *) s)->getIndex() == 0x6e) {
                oy = 0;
                oz = 50000;
                ox = 60000;
                placed = true;
            }
        }
        if (!placed && camp == 0x91) {
            Station *s = (Station *) Status::gStatus->getStation();
            if (((Station *) s)->getIndex() == 0x70) {
                oz = 70000;
                ox = 50000;
                oy = 0;
                placed = true;
            }
        }
        if (!placed) {
            ox = rx - 50000;
            oz = rz + 20000;
            oy = ry - 50000;
            if (Status::gStatus->getCurrentCampaignMission() == 0 && this->missionPtr == 3) {
                oz = 0;
                ox = 0;
                oy = 0;
            }
        }
    }

    rng->reset();

    this->field_c8 = (float) ox;
    this->field_cc = (float) oy;
    this->field_d0 = (float) oz;

    Waypoint *wp = (Waypoint *) ::operator new(sizeof(Waypoint));
    new(wp) Waypoint(ox, oy, oz, 0);
    this->asteroidWaypoint = wp;

    BoundingSphere *bs = (BoundingSphere *) ::operator new(0x48);
    new(bs) BoundingSphere((float) ox, (float) oy, (float) oz,
                           0.0f, 0.0f, 0.0f, 100000.0f);
    *(BoundingSphere **) &this->collisionVolume = bs;

    int density = rng->nextInt(8) + 2;
    int alien2 = Status::gStatus->inAlienOrbit();

    void *canvas = (void *) PaintCanvas::gCanvas;
    int kind = 0x9a;
    int probCursor = 0;

    for (unsigned i = 0; i < this->asteroids->size(); i = i + 1) {
        if (alien2 == 0) {
            bool ok = false;
            int cursor = probCursor;
            while (!ok) {
                int roll = rng->nextInt(100);
                int next = 0;
                if (roll < prob[cursor + 1]) {
                    kind = prob[cursor];
                    next = cursor + 2;
                    if (cursor > 8)
                        next = 0;
                    ok = (kind == 0xd9) || (kind < 0xa4);
                }
                cursor = next;
            }
            probCursor = cursor;
        } else {
            kind = 0xa4;
        }

        unsigned spread = (int) i < density ? 60000u : 100000u;
        float half = (float) (spread >> 1);

        int colVariant = colBase;
        if (Status::gStatus->inAlienOrbit() != 0)
            colVariant = 1;
        if (kind == 0xd9)
            colVariant = 3;

        // Android 2.0.16 .rodata at 0x1fb808.
        static const int meshTable[4] = {0x4204, 0x4219, 0x4208, 0x4994};
        int mesh = meshTable[colVariant & 3];

        Vector pos;
        for (;;) {
            pos.x = (this->field_c8 - half) + (float) rng->nextInt(spread);
            pos.y = (this->field_cc - half) + (float) rng->nextInt(spread);
            pos.z = (this->field_d0 - half) + (float) rng->nextInt(spread);
            if (i == 0 || (int) i >= density)
                break;
            bool farEnough = false;
            for (unsigned j = 0; j < i; j = j + 1) {
                float d = 1.0e30f;
                if (this->asteroids != nullptr) {
                    Vector c = (*this->asteroids)[j]->getPosition();
                    Vector dv;
                    dv.x = c.x - pos.x;
                    dv.y = c.y - pos.y;
                    dv.z = c.z - pos.z;
                    d = AbyssEngine::AEMath::VectorLength(dv);
                }
                if (8000 < (int) d) {
                    farEnough = true;
                    break;
                }
            }
            if (farEnough)
                break;
        }

        AEGeometry *geo = (AEGeometry *) ::operator new(sizeof(AEGeometry));
        new((void *) geo) AEGeometry((uint16_t) mesh, (PaintCanvas *) canvas, 0);

        bool near = (int) i < density;
        {
            unsigned short lodMeshes[3] = {
                (unsigned short) (mesh + 1),
                (unsigned short) (mesh + 2),
                (unsigned short) (mesh + 3)
            };
            int lodDist[3];
            if (Globals::iPad) {
                lodDist[0] = 100000;
                lodDist[1] = 120000;
                lodDist[2] = 150000;
            } else if (near) {
                lodDist[0] = 60000;
                lodDist[1] = 100000;
                lodDist[2] = 120000;
            } else {
                lodDist[0] = 40000;
                lodDist[1] = 70000;
                lodDist[2] = 90000;
            }
            geo->setLodMeshes(lodMeshes, lodDist, 3);
        }
        this->lodManager->addObject(geo);

        int radius = rng->nextInt(near ? 100 : 70) + (near ? 120 : 30);
        float scale = (float) radius * 0.01f;
        double scaleForQuality = scale;
        int quality;
        if (scaleForQuality < 0.4)
            quality = 4;
        else if (scaleForQuality < 0.7)
            quality = 5;
        else if (scaleForQuality < 0.92)
            quality = 6;
        else if (rng->nextInt(2))
            quality = rng->nextInt(3) + 4;
        else
            quality = 7;

        PlayerAsteroid *a = (PlayerAsteroid *) ::operator new(sizeof(PlayerAsteroid));
        new(a) PlayerAsteroid(mesh, geo, colVariant, kind, pos, scale, quality);
        (*this->asteroids)[i] = a;

        (*this->asteroids)[i]->setLevel(this);

        ((PlayerAsteroid *) (*this->asteroids)[i])->setAsteroidCenter(
            Vector{this->field_c8, this->field_cc, this->field_d0});
    }

    if (prob != 0)
        ::operator delete[](prob);
}

// Android 0xb4bb8: campaign dispatch; route payloads are read from 2.0.16 rodata.
void Level::createCampaignMission() {
    Vector egoPosition = player->geometry->getPosition();
    int campaign = Globals::status->getCurrentCampaignMission();
    switch (campaign) {
        case 0:
            enemies = new Array<KIPlayer *>();
            ArraySetLength(3, *enemies);
            for (unsigned i = 0; i < enemies->size(); ++i) {
                (*enemies)[i] = createShip(8, 0, i == 1 ? 23 : 2, nullptr, true, false);
                (*enemies)[i]->setToSleep();
                (*enemies)[i]->player->setAlwaysEnemy(true);
                (*enemies)[i]->setPosition(50000.0f, 50000.0f, 50000.0f);
                (*enemies)[i]->cargo = nullptr;
                *reinterpret_cast<unsigned char *>(&(*enemies)[i]->hasCargo) = 0;
                (*enemies)[i]->player->setHitpoints(150);
                if (i <= 2) static_cast<PlayerFighter *>((*enemies)[i])->setExhaustVisible(false);
            }
            static_cast<Player *>(player->player)->setHitpoints(9999999);
            Globals::status->field_64 = 9999999;
            static_cast<Player *>(player->player)->field_5e = 1;
            break;
        case 1: {
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            (*enemies)[0] = createShip(3, 0, 30, nullptr, true, false);
            (*enemies)[0]->setPosition(300.0f, 50.0f, -6000.0f);
            int coords[] = {0, 0, -5000, 0, 0, 0}; // 0x1fd078, 6 words
            enemyRoute = new Route(coords, 6);
            (*enemies)[0]->setRoute(enemyRoute);
            (*enemies)[0]->setSpeed(0.0f);
            static_cast<PlayerFighter *>((*enemies)[0])->setExhaustVisible(false);
            break;
        }
        case 4:
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            (*enemies)[0] = createShip(8, 0, 2, nullptr, true, false);
            (*enemies)[0]->setInitActive(false);
            (*enemies)[0]->player->setAlwaysEnemy(true);
            (*enemies)[0]->setPosition(0.0f, 0.0f, -200000.0f);
            (*enemies)[0]->setToSleep();
            break;
        case 7: {
            int coords[] = {-4000, -3000, 80000, 10000, 7000, 160000}; // 0x1fd090
            playerRoute = new Route(coords, 6);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(4, *enemies);
            for (int i = 0; i != 3; ++i) {
                (*enemies)[i] = createShip(8, 0, 2, playerRoute->getWaypoint(1), true, false);
                (*enemies)[i]->setToSleep();
            }
            (*enemies)[3] = createShip(3, 0, 30, nullptr, true, false);
            (*enemies)[3]->player->setAlwaysFriend(true);
            (*enemies)[3]->setPosition(egoPosition.x + 700.0f, egoPosition.y + 50.0f, egoPosition.z + 6000.0f);
            (*enemies)[3]->setRoute(playerRoute->clone());
            (*enemies)[3]->player->setHitpoints(9999999);
            (*enemies)[3]->name = *Globals::gameText->getText(1599);
            static_cast<PlayerFighter *>((*enemies)[3])->setBoostProb(0);
            objectivesA = new Objective(18, 0, 3, this);
            break;
        }
        case 14: {
            int coords[] = {40000, 5000, 30000}; // 0x1fd0a8
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(7, *enemies);
            for (int i = 0; i != 3; ++i)
                (*enemies)[i] = createShip(8, 0, 0, enemyRoute->getWaypoint(), true, false);
            for (int i = 3; i != 5; ++i)
                (*enemies)[i] = createShip(0, 0, 5, enemyRoute->getWaypoint(), true, false);
            for (int i = 5; i != 7; ++i)
                (*enemies)[i] = createShip(0, 1, 14, enemyRoute->getWaypoint(), true, false);
            Vector position = enemyRoute->getWaypoint()->getPosition();
            position.x += 7000.0f;
            (*enemies)[enemies->size() - 2]->setPosition(position);
            position = enemyRoute->getWaypoint()->getPosition();
            position.x -= 9000.0f;
            position.y += 2000.0f;
            position.z += 7000.0f;
            (*enemies)[enemies->size() - 1]->setPosition(position);
            player->setPosition(40000.0f, 0.0f, 120000.0f);
            objectivesA = new Objective(31, 0, this);
            break;
        }
        case 16: {
            int coords[] = {0, 0, 170000}; // 0x1fd0b8
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(10, *enemies);
            Waypoint *waypoint = enemyRoute->getWaypoint();
            for (int i = 0; i != 3; ++i) {
                (*enemies)[i] = createShip(0, 1, 15, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                Player *body = (*enemies)[i]->player;
                body->setMaxHitpoints(body->getMaxHitpoints() / 3);
                delete (*enemies)[i]->cargo;
                (*enemies)[i]->cargo = nullptr;
            }
            (*enemies)[0]->setPosition(float(waypoint->x), float(waypoint->y), float(waypoint->z));
            Player *body = (*enemies)[0]->player;
            body->setMaxHitpoints(body->getMaxHitpoints() / 6);
            (*enemies)[1]->setPosition(float(waypoint->x + 3000), float(waypoint->y + 2000), float(waypoint->z - 3000));
            (*enemies)[2]->setPosition(float(waypoint->x - 9000), float(waypoint->y - 8000), float(waypoint->z - 7000));
            for (int i = 3; i != 7; ++i) {
                (*enemies)[i] = createShip(9, 0, 8, enemyRoute->getWaypoint(), true, false);
                Player *enemy = (*enemies)[i]->player;
                enemy->setMaxHitpoints(10 * enemy->getMaxHitpoints());
                KIPlayer *spawned = (*enemies)[i];
                int x = waypoint->x + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                int y = waypoint->y + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                spawned->setPosition(float(x), float(y), float(waypoint->z + 50000));
            }
            for (int i = 7; i != 10; ++i) {
                (*enemies)[i] = createShip(0, 0, 5, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                (*enemies)[i]->player->setHitpoints(600);
                KIPlayer *spawned = (*enemies)[i];
                float x = egoPosition.x + float(static_cast<AERandom *>(Globals::rnd)->nextInt(4000) - 2000);
                float y = egoPosition.y + float(static_cast<AERandom *>(Globals::rnd)->nextInt(3400) - 1700);
                float z = (egoPosition.z + 2000.0f) + float(static_cast<AERandom *>(Globals::rnd)->nextInt(4000) - 2000);
                spawned->setPosition(x, y, z);
            }
            if ((*landmarks)[3]) {
                (*landmarks)[3]->setVisible(true);
                (*landmarks)[3]->setPosition(float(waypoint->x), float(waypoint->y), float(waypoint->z + 40000));
            }
            objectivesA = new Objective(22, 0, this);
            break;
        }
        case 21: {
            int playerCoords[] = {40000, -40000, 120000, -10000, 20000, 190000}; // 0x1fd0c8
            playerRoute = new Route(playerCoords, 6);
            int firstCoords[] = {40000, -40000, 120000, 0, 0, 0}; // 0x1fd0e0
            Route *firstRoute = new Route(firstCoords, 3);
            firstRoute->setLoop(true);
            int secondCoords[] = {-10000, 20000, 190000, 0, 0, 0}; // 0x1fd0f8
            Route *secondRoute = new Route(secondCoords, 3);
            secondRoute->setLoop(true);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(4, *enemies);
            (*enemies)[1] = createShip(0, 0, 5, firstRoute->getWaypoint(), true, false);
            (*enemies)[2] = createShip(0, 0, 5, firstRoute->getWaypoint(), true, false);
            (*enemies)[3] = createShip(0, 0, 5, firstRoute->getWaypoint(), true, false);
            (*enemies)[0] = createShip(0, 0, 17, secondRoute->getWaypoint(), true, false);
            (*enemies)[0]->name = *Globals::gameText->getText(1611);
            (*enemies)[0]->player->setAlwaysEnemy(true);
            for (unsigned i = 0; i < enemies->size(); ++i) {
                (*enemies)[i]->setToSleep();
                (*enemies)[i]->setRoute(i ? firstRoute->clone() : secondRoute);
                (*enemies)[i]->geometry->setRotation(0.0f, 3.1416f, 0.0f);
            }
            KIPlayer *lead = (*enemies)[0];
            int x = secondRoute->getWaypoint()->x + 1000;
            int y = secondRoute->getWaypoint()->y;
            int z = secondRoute->getWaypoint()->z + 2000;
            lead->setPosition(float(x), float(y), float(z));
            objectivesA = new Objective(22, 0, this);
            objectivesB = new Objective(7, 1, this);
            delete firstRoute;
            break;
        }
        case 24: {
            int coords[] = {100000, 0, 0, 100000, 0, -30000}; // 0x1fd110
            enemyRoute = new Route(coords, 6);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(5, *enemies);
            for (int i = 0; i != 3; ++i) {
                (*enemies)[i] = createShip(9, 0, 8, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->setRoute(enemyRoute->clone());
            }
            for (int i = 3; i != 5; ++i) {
                (*enemies)[i] = createShip(2, 1, 15, enemyRoute->getWaypoint(), true, false);
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                Player *body = (*enemies)[i]->player;
                body->setMaxHitpoints(body->getMaxHitpoints() / 3);
                delete (*enemies)[i]->cargo;
                (*enemies)[i]->cargo = nullptr;
            }
            (*landmarks)[3]->setVisible(false);
            objectivesA = new Objective(31, 0, this);
            break;
        }
        case 25:
        case 29:
            enemies = new Array<KIPlayer *>();
            ArraySetLength(3, *enemies);
            for (int i = 0; i != 3; ++i) {
                (*enemies)[i] = createShip(9, 0, 8, nullptr, true, false);
                int signRoll = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
                int distanceX = static_cast<AERandom *>(Globals::rnd)->nextInt(80000);
                int signY = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                int y = (static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 20000) * signY;
                int x = (signRoll == 0 ? 1 : -1) * (distanceX + 20000);
                int signZ = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                float z = float((static_cast<AERandom *>(Globals::rnd)->nextInt(80000) + 20000) * signZ);
                (*enemies)[i]->setPosition(float(x), float(y), z);
            }
            break;
        case 26:
            enemies = new Array<KIPlayer *>();
            ArraySetLength(2, *enemies);
            for (int i = 0; i != 2; ++i) {
                (*enemies)[i] = createShip(9, 0, 8, nullptr, true, false);
                Vector position = player->getPosition();
                Vector direction = player->geometry->getDirection();
                direction = AbyssEngine::AEMath::VectorNormalize(direction);
                position -= direction * 8000.0f;
                position.x += float(static_cast<AERandom *>(Globals::rnd)->nextInt(1400) - 700);
                position.y += float(static_cast<AERandom *>(Globals::rnd)->nextInt(1400) - 700);
                position.z += float(static_cast<AERandom *>(Globals::rnd)->nextInt(1400) - 700);
                (*enemies)[i]->setPosition(position);
            }
            objectivesA = new Objective(7, 2, this);
            break;
        case 28: {
            (*landmarks)[3]->setPosition(100000.0f, -50000.0f, -40000.0f);
            Vector position = (*landmarks)[3]->getPosition();
            missionSpawnPosition = position;
            int coords[] = {int(missionSpawnPosition.x), int(missionSpawnPosition.y), int(missionSpawnPosition.z)};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(8, *enemies);
            for (int i = 0; i != 5; ++i)
                (*enemies)[i] = createShip(9, 0, 8, enemyRoute->getWaypoint(), true, false);
            for (int i = 5; i != 8; ++i) {
                (*enemies)[i] = createShip(0, 1, 15, enemyRoute->getWaypoint(), true, false);
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                Player *body = (*enemies)[i]->player;
                body->setMaxHitpoints(body->getMaxHitpoints() / 4);
                delete (*enemies)[i]->cargo;
                (*enemies)[i]->cargo = nullptr;
            }
            break;
        }
        case 36: {
            Mission *mission = Globals::status->getMission();
            mission->setAgent(new Agent(-1, String(*static_cast<GameText *>(Globals::gameText)->getText(1604), false),
                                       29, 5, 1, true, -1, -1, -1, -1));
            int coords[] = {110000, -10000, -80000, 70000, 0, -100000,
                            -100000, 10000, -80000, -130000, -50000, -150000}; // 0x1fd128
            playerRoute = new Route(coords, 12);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(8, *enemies);
            (*enemies)[0] = createShip(1, 0, 9, nullptr, true, false);
            KIPlayer *lead = (*enemies)[0];
            float x = egoPosition.x + float(static_cast<AERandom *>(Globals::rnd)->nextInt(1400) - 700);
            float y = egoPosition.y + float(static_cast<AERandom *>(Globals::rnd)->nextInt(1400) - 700);
            lead->setPosition(x, y, egoPosition.z + 1000.0f);
            (*enemies)[0]->setSpeed(3.0f);
            static_cast<PlayerFighter *>((*enemies)[0])->setRotate(3);
            (*enemies)[0]->player->setHitpoints(9999999);
            (*enemies)[0]->setRoute(playerRoute->clone());
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1604);
            (*enemies)[0]->cargo = nullptr;
            for (unsigned i = 1; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                Route *spawnRoute = playerRoute;
                int waypoint = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                (*enemies)[i] = createShip(8, 0, type, spawnRoute->getWaypoint(waypoint), true, false);
                (*enemies)[i]->setToSleep();
            }
            objectivesA = new Objective(20, 1, 8, this);
            objectivesB = new Objective(21, 1, 8, this);
            break;
        }
        case 38: {
            int coords[] = {90000, 10000, 80000};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(7, *enemies);
            for (int i = 0; i != 2; ++i) {
                (*enemies)[i] = createShip(2, 1, 15, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                KIPlayer *spawned = (*enemies)[i];
                int x = enemyRoute->getWaypoint()->x + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                int y = enemyRoute->getWaypoint()->y + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                int z = enemyRoute->getWaypoint()->z + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                spawned->setPosition(float(x), float(y), float(z));
            }
            for (unsigned i = 2; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(3);
                (*enemies)[i] = createShip(3, 0, type, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            objectivesA = new Objective(18, 2, 7, this);
            objectivesB = new Objective(7, 2, this);
            break;
        }
        case 40: {
            int enemyCoords[] = {-20000, -3000, 200000};
            int friendCoords[] = {-20000, -3000, 35000, -20000, -3000, 200000}; // 0x1fd180 + immediates
            enemyRoute = new Route(enemyCoords, 3);
            friendRoute = new Route(friendCoords, 6);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(13, *enemies);
            (*enemies)[0] = createShip(0, 1, 13, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1604);
            (*enemies)[0]->player->setMaxHitpoints(5 * Globals::status->getLevel() + 1800);
            (*enemies)[0]->setPosition(-9999999.0f, -9999999.0f, -9999999.0f);
            (*enemies)[0]->setToSleep();
            (*enemies)[0]->setVisible(false);
            for (int i = 1; i != 5; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(0);
                (*enemies)[i] = createShip(0, 0, type, nullptr, true, false);
                (*enemies)[i]->setRoute(friendRoute->clone());
                (*enemies)[i]->player->setAlwaysFriend(true);
                if (i == 2) (*enemies)[2]->name = *static_cast<GameText *>(Globals::gameText)->getText(1605);
            }
            for (int i = 5; i != 9; ++i) {
                (*enemies)[i] = createShip(9, 0, 8, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            for (int i = 9; i != 13; ++i) {
                (*enemies)[i] = createShip(9, 0, 8, nullptr, true, false);
                (*enemies)[i]->setPosition(Vector{-500000.0f, -500000.0f, -500000.0f});
                (*enemies)[i]->player->setAlwaysEnemy(true);
                (*enemies)[i]->setToSleep();
            }
            (*landmarks)[3]->setPosition(-20000.0f, -3000.0f, 200000.0f);
            if (initStreamOutPosition) {
                player->setPosition(-105000.0f, 0.0f, 80000.0f);
                player->geometry->setRotation(0.0f, 1.5708f, 0.0f);
            }
            objectivesB = new Objective(7, 1, this);
            break;
        }
        case 41:
            enemies = new Array<KIPlayer *>();
            ArraySetLength(8, *enemies);
            (*enemies)[0] = createShip(1, 1, 13, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1604);
            if (lastMissionFreighterHitpoints <= 0)
                lastMissionFreighterHitpoints = int(float(5 * Globals::status->getLevel() + 1800) * 0.7f);
            (*enemies)[0]->player->setHitpoints(lastMissionFreighterHitpoints);
            static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(true);
            (*enemies)[0]->setPosition(0.0f, 0.0f, -300000.0f);
            for (int i = 1; i != 8; ++i) {
                (*enemies)[i] = createShip(9, 0, 8, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            (*enemies)[1]->setPosition(0.0f, 0.0f, -260000.0f);
            (*enemies)[2]->setPosition(-10000.0f, 10000.0f, -240000.0f);
            (*enemies)[3]->setPosition(13000.0f, 2000.0f, -220000.0f);
            (*enemies)[4]->setPosition(-72000.0f, -4000.0f, -210000.0f);
            (*enemies)[5]->setPosition(60000.0f, -40000.0f, -190000.0f);
            (*enemies)[6]->setPosition(-18000.0f, 30000.0f, -160000.0f);
            (*enemies)[7]->setPosition(17000.0f, 40000.0f, -140000.0f);
            player->setPosition(3000.0f, 2000.0f, -320000.0f);
            player->geometry->setRotation(0.0f, 0.0f, 0.0f);
            objectivesA = new Objective(25, 0, this);
            objectivesB = new Objective(7, 1, this);
            break;
        case 48:
        case 49:
            enemies = new Array<KIPlayer *>();
            ArraySetLength(5, *enemies);
            for (unsigned i = 0; i < enemies->size(); ++i) {
                (*enemies)[i] = createShip(1, 0, 9, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysEnemy(false);
                (*enemies)[i]->player->setAlwaysFriend(true);
            }
            break;
        case 50:
            if ((*landmarks)[1]) missionSpawnPosition = (*landmarks)[1]->getPosition();
            else missionSpawnPosition = Vector{0.0f, 0.0f, 40000.0f};
            enemies = new Array<KIPlayer *>();
            ArraySetLength(4, *enemies);
            for (unsigned i = 0; i < enemies->size(); ++i) {
                (*enemies)[i] = createShip(1, 0, 9, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysEnemy(false);
                (*enemies)[i]->player->setAlwaysFriend(true);
            }
            break;
        case 51: {
            if ((*landmarks)[1]) missionSpawnPosition = (*landmarks)[1]->getPosition();
            else missionSpawnPosition = Vector{0.0f, 0.0f, 40000.0f};
            int coords[] = {int(missionSpawnPosition.x), int(missionSpawnPosition.y), int(missionSpawnPosition.z)};
            enemyRoute = new Route(coords, 3);
            enemyRoute->setLoop(true);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(6, *enemies);
            (*enemies)[0] = createShip(1, 1, 13, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysEnemy(true);
            missionSpawnPosition = missionSpawnPosition + (player->getPosition() - missionSpawnPosition) / 3.0f;
            (*enemies)[0]->setPosition(missionSpawnPosition.x, missionSpawnPosition.y, missionSpawnPosition.z);
            for (unsigned i = 1; i < enemies->size(); ++i) {
                (*enemies)[i] = createShip(1, 0, 9, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                (*enemies)[i]->setRoute(enemyRoute->clone());
            }
            break;
        }
        case 52: {
            if ((*landmarks)[1]) missionSpawnPosition = (*landmarks)[1]->getPosition();
            else missionSpawnPosition = Vector{0.0f, 0.0f, 40000.0f};
            int coords[] = {int(missionSpawnPosition.x), int(missionSpawnPosition.y), int(missionSpawnPosition.z)};
            enemyRoute = new Route(coords, 3);
            enemyRoute->setLoop(true);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(8, *enemies);
            (*enemies)[0] = createShip(1, 1, 13, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysEnemy(true);
            missionSpawnPosition = missionSpawnPosition + (player->getPosition() - missionSpawnPosition) / 2.0f;
            (*enemies)[0]->setPosition(missionSpawnPosition.x, missionSpawnPosition.y, missionSpawnPosition.z);
            (*enemies)[1] = createShip(1, 1, 13, nullptr, true, false);
            (*enemies)[1]->player->setAlwaysEnemy(true);
            missionSpawnPosition.x += 16000.0f;
            missionSpawnPosition.y += 13000.0f;
            missionSpawnPosition.z += 16000.0f;
            (*enemies)[1]->setPosition(missionSpawnPosition.x, missionSpawnPosition.y, missionSpawnPosition.z);
            for (unsigned i = 2; i < enemies->size(); ++i) {
                (*enemies)[i] = createShip(1, 0, 9, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                (*enemies)[i]->setRoute(enemyRoute->clone());
            }
            break;
        }
        case 56: {
            int coords[] = {0, 0, -60000, -12000, -7000, -110000, 15000, 3000, -160000}; // 0x1fd190
            playerRoute = new Route(coords, 9);
            friendRoute = playerRoute->clone();
            enemies = new Array<KIPlayer *>();
            ArraySetLength(9, *enemies);
            for (int i = 0; i != 3; ++i) {
                (*enemies)[i] = createShip(8, 0, 24, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                (*enemies)[i]->player->setAlwaysEnemy(false);
                (*enemies)[i]->setToSleep();
                missionSpawnPosition = player->getPosition() * 0.8f;
                KIPlayer *spawned = (*enemies)[i];
                float x = missionSpawnPosition.x - 3500.0f + float(static_cast<AERandom *>(Globals::rnd)->nextInt(7000));
                float y = missionSpawnPosition.y - 3500.0f + float(static_cast<AERandom *>(Globals::rnd)->nextInt(7000));
                float z = missionSpawnPosition.z - 3500.0f + float(static_cast<AERandom *>(Globals::rnd)->nextInt(7000));
                spawned->setPosition(x, y, z);
                (*enemies)[i]->setRoute(friendRoute->clone());
                (*enemies)[i]->player->setHitpoints(9999999);
            }
            for (unsigned i = 3; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                int waypoint = i < 5 ? 0 : (i < 10 ? 1 : 2);
                (*enemies)[i] = createShip(8, 0, type, playerRoute->getWaypoint(waypoint), true, false);
                (*enemies)[i]->setToSleep();
            }
            objectivesA = new Objective(18, 3, 9, this);
            break;
        }
        case 63: {
            enemies = new Array<KIPlayer *>();
            ArraySetLength(7, *enemies);
            int coords[] = {0, 0, 0};
            Route *route = new Route(coords, 3);
            (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(route->getWaypoint(), 14243, true));
            Vector position = route->getWaypoint()->getPosition();
            (*enemies)[0]->setToSleep();
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(441);
            for (unsigned i = 1; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, nullptr, true, false);
                int dx = static_cast<AERandom *>(Globals::rnd)->nextInt(20000);
                int sx = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
                int dy = static_cast<AERandom *>(Globals::rnd)->nextInt(20000);
                int sy = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                int y = sy * dy + 10000;
                int x = (sx == 0 ? 1 : -1) * dx + 10000;
                int dz = static_cast<AERandom *>(Globals::rnd)->nextInt(20000);
                int sz = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                float z = float(int(position.z + float(sz * dz + 10000)));
                (*enemies)[i]->setPosition(float(int(position.x + float(x))), float(int(position.y + float(y))), z);
                (*enemies)[i]->setToSleep();
            }
            objectivesA = new Objective(27, 4, this);
            break;
        }
        case 64: {
            PlayerEgo *ego = player;
            Vector position = player->getPosition();
            Vector direction = player->geometry->getDirection();
            ego->setPosition(position - direction * 40000.0f);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(9, *enemies);
            int coords[] = {100000, 0, 0};
            enemyRoute = new Route(coords, 3);
            (*enemies)[0] = createShip(0, 0, 38, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1617);
            (*enemies)[0]->setPosition(0.0f, 0.0f, 0.0f);
            (*enemies)[0]->geometry->setDirection(Vector{1.0f, 0.0f, 0.0f}, Vector{0.0f, 1.0f, 0.0f});
            (*enemies)[0]->setRoute(enemyRoute->clone());
            (*enemies)[0]->cargo = nullptr;
            for (unsigned i = 1; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, nullptr, true, false);
                int dx = static_cast<AERandom *>(Globals::rnd)->nextInt(1500);
                int sx = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
                int dy = static_cast<AERandom *>(Globals::rnd)->nextInt(1500);
                int sy = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                int y = sy * (dy + 1000);
                int x = (sx == 0 ? 1 : -1) * (dx + 1000);
                int dz = static_cast<AERandom *>(Globals::rnd)->nextInt(1500);
                int sz = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                (*enemies)[i]->setPosition(float(x), float(y), float(sz * (dz + 1000)));
                (*enemies)[i]->geometry->setDirection(Vector{1.0f, 0.0f, 0.0f}, Vector{0.0f, 1.0f, 0.0f});
                (*enemies)[i]->setRoute(enemyRoute->clone());
            }
            objectivesA = new Objective(22, 0, this);
            objectivesB = new Objective(1, 0, this);
            break;
        }
        case 65: {
            int coords[] = {0, 0, 0};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            (*enemies)[0] = createShip(0, 0, 38, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1617);
            (*enemies)[0]->setRoute(enemyRoute->clone());
            (*enemies)[0]->player->setHitpoints(9999999);
            break;
        }
        case 67: {
            int coords[] = {220000, -20000, -10000}; // 0x1fd1c8
            enemyRoute = new Route(coords, 3);
            playerRoute = enemyRoute->clone();
            enemies = new Array<KIPlayer *>();
            ArraySetLength(11, *enemies);
            (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 14243, true));
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(441);
            (*enemies)[0]->setPosition(float(coords[0]), float(coords[1]), float(coords[2]));
            (*enemies)[0]->player->setAlwaysEnemy(false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->setToSleep();
            for (unsigned i = 1; i < enemies->size() - 1; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, enemyRoute->getWaypoint(), true, false);
                (*enemies)[i]->setToSleep();
                // Android writes the first fighter extension word +4 (+0x128).
                *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 4) = 150000;
                if (i >= 5) (*enemies)[i]->setPosition(800000.0f, 800000.0f, 800000.0f);
            }
            (*enemies)[1]->setPosition(float(coords[0] + 30000), float(coords[1]), float(coords[2]));
            (*enemies)[enemies->size() - 1] = createShip(0, 0, 27, nullptr, true, false);
            Vector position = player->getPosition();
            (*enemies)[enemies->size() - 1]->setPosition(position + Vector{2000.0f, 500.0f, -7000.0f});
            (*enemies)[enemies->size() - 1]->player->setAlwaysFriend(true);
            (*enemies)[enemies->size() - 1]->name = *static_cast<GameText *>(Globals::gameText)->getText(1633);
            (*enemies)[enemies->size() - 1]->player->setMaxHitpoints(9999999);
            (*enemies)[enemies->size() - 1]->setRoute(playerRoute->clone());
            objectivesA = new Objective(22, 0, this);
            objectivesB = new Objective(1, 0, this);
            break;
        }
        case 69: {
            Vector position = (*static_cast<Array<AEGeometry *> *>(starSystem->getPlanets()))[1]->getPosition();
            position *= 4.0f;
            int coords[] = {int(position.x), int(position.y), int(position.z),
                            int(position.x * 10.0f), int(position.y * 10.0f), int(position.z * 10.0f)};
            enemyRoute = new Route(coords, 6);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(5, *enemies);
            (*enemies)[0] = createShip(0, 0, 12, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1631);
            (*enemies)[0]->setRoute(enemyRoute->clone());
            (*enemies)[0]->setPosition(position);
            (*enemies)[0]->setSpeed(5.5f);
            position = AbyssEngine::AEMath::VectorNormalize(position);
            (*enemies)[0]->geometry->setDirection(position, Vector{0.0f, 1.0f, 0.0f});
            int friendCoords[] = {0, 0, 20000};
            friendRoute = new Route(friendCoords, 3);
            for (unsigned i = 1; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(0);
                (*enemies)[i] = createShip(0, 0, type, friendRoute->getWaypoint(), true, false);
            }
            objectivesA = new Objective(22, 0, this);
            break;
        }
        case 70: {
            PlayerEgo *ego = player;
            Vector position = player->getPosition();
            Vector direction = player->geometry->getDirection();
            ego->setPosition(position - direction * 120000.0f);
            Vector landmarkPosition = (*landmarks)[1]->getPosition();
            Vector playerPosition = player->getPosition();
            int coords[] = {int(landmarkPosition.x), int(landmarkPosition.y), int(landmarkPosition.z)};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            (*enemies)[0] = createShip(0, 0, 12, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1631);
            (*enemies)[0]->setRoute(enemyRoute->clone());
            missionSpawnPosition = playerPosition + (landmarkPosition - playerPosition) / 4.0f;
            (*enemies)[0]->setPosition(missionSpawnPosition);
            Vector facing = AbyssEngine::AEMath::VectorNormalize(landmarkPosition - playerPosition);
            (*enemies)[0]->geometry->setDirection(facing, Vector{0.0f, 1.0f, 0.0f});
            Player *body = (*enemies)[0]->player;
            body->setHitpoints(int(float(body->getMaxHitpoints()) * 2.5f));
            objectivesA = new Objective(1, 0, this);
            break;
        }
        case 73: {
            int coords[] = {80000, 0, 60000, 150000, 0, -50000}; // 0x1fd1e8
            enemyRoute = new Route(coords, 6);
            player->setPosition(-30000.0f, 0.0f, 150000.0f);
            Vector target = {float(coords[3] + 20000), float(coords[4]), float(coords[5])};
            Vector direction = AbyssEngine::AEMath::VectorNormalize(target - player->getPosition());
            player->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            enemies = new Array<KIPlayer *>();
            ArraySetLength(12, *enemies);
            for (int i = 0; i != 8; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                (*enemies)[i]->setToSleep();
                if (i >= 4) (*enemies)[i]->setPosition(Vector{-800000.0f, -800000.0f, -800000.0f});
            }
            for (unsigned i = 8; i < enemies->size(); ++i) {
                (*enemies)[i] = createShip(0, 1, 15, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                (*enemies)[i]->cargo = nullptr;
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(true);
                KIPlayer *spawned = (*enemies)[i];
                int x = enemyRoute->getWaypoint(1)->x + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                int y = enemyRoute->getWaypoint(1)->y + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                int z = enemyRoute->getWaypoint(1)->z + static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
                spawned->setPosition(float(x), float(y), float(z));
            }
            objectivesA = new Objective(18, 0, 8, this);
            objectivesB = new Objective(18, 8, 12, this);
            break;
        }
        case 78: {
            Matrix matrix;
            const float initial[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1}; // Android 0xb7ed8/0xb7ee0
            std::memcpy(matrix.m, initial, sizeof(initial));
            struct TurretPlacement { int flag; Vector position; Vector rotation; };
            // First two 28-byte records from Android rodata 0x1fd200.
            static const TurretPlacement placements[] = {
                {0, {-3994.97f, 23359.0f, -7378.10f}, {0.0f, 0.0f, 1.57079637f}},
                {0, {3994.96f, 23359.0f, -7378.10f}, {0.0f, 0.0f, -1.57079637f}}
            };
            enemies = new Array<KIPlayer *>();
            ArraySetLength(22, *enemies);
            Vector rotated{0.0f, 0.0f, 0.0f};
            for (int i = 0; i != 2; ++i) {
                (*enemies)[i] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 14363, true));
                Vector mountPosition{placements[i].position.x, placements[i].position.y, placements[i].position.z};
                Vector mountRotation{placements[i].rotation.x, placements[i].rotation.y, placements[i].rotation.z};
                rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, mountRotation);
                (*enemies)[i]->geometry->setRotation(rotated);
                rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, mountPosition);
                Vector position = {rotated.x + (i == 0 ? 1200.0f : -1200.0f), rotated.y, rotated.z};
                (*enemies)[i]->setPosition(position);
            }
            int baseX = -17000;
            for (unsigned i = 2; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                KIPlayer *spawned = (*enemies)[i];
                int x = static_cast<AERandom *>(Globals::rnd)->nextInt(2000) + baseX;
                float y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(10000) - 5000);
                int z = static_cast<AERandom *>(Globals::rnd)->nextInt(10000) + 155000;
                spawned->setPosition(Vector{float(x), y, float(z)});
                (*enemies)[i]->geometry->setDirection(Vector{0.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
                (*enemies)[i]->setToSleep();
                baseX += 2000;
            }
            break;
        }
        case 79:
            enemies = new Array<KIPlayer *>();
            ArraySetLength(7, *enemies);
            for (unsigned i = 0; i < enemies->size(); ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(9);
                (*enemies)[i] = createShip(9, 0, type, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            break;
        case 81: {
            enemies = new Array<KIPlayer *>();
            Station *station = static_cast<Galaxy *>(Globals::galaxy)->getStation(101);
            ArrayAdd(static_cast<KIPlayer *>(new PlayerStation(station)), *enemies);
            (*enemies)[0]->name = station->getName();
            (*enemies)[0]->shipGroup = 8;
            delete station;
            for (int i = 8; i; --i) ArrayAdd(static_cast<KIPlayer *>(createShip(9, 0, 8, nullptr, true, false)), *enemies);
            break;
        }
        case 87:
            objectivesA = new Objective(22, 0, this);
            break;
        case 80: {
            enemies = new Array<KIPlayer *>();
            Vector stationPosition = {0.0f, 0.0f, 160000.0f};
            Station *station = static_cast<Galaxy *>(Globals::galaxy)->getStation(101);
            ArrayAdd(static_cast<KIPlayer *>(new PlayerStation(station)), *enemies);
            (*enemies)[0]->name = station->getName();
            (*enemies)[0]->shipGroup = 8;
            (*enemies)[0]->setPosition(stationPosition);
            (*enemies)[0]->geometry->setRotation(Vector{0.0f, 0.0f, 0.0f});
            delete station;
            Matrix matrix;
            const float initial[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1};
            std::memcpy(matrix.m, initial, sizeof(initial));
            AbyssEngine::AEMath::MatrixSetRotation(matrix, 0.0f, 3.1416f, 0.0f);
            int hp;
            if (Globals::status->getLevel() <= 20) hp = 15 * Globals::status->getLevel() + 220;
            else hp = 520;
            struct TurretPlacement { float flag; Vector position; Vector rotation; };
            // Twelve 28-byte records, Android rodata 0x1fd200..0x1fd350.
            static const TurretPlacement placements[] = {
                {0.0f, {-3994.969970703125f, 23359.0f, -7378.10009765625f}, {0.0f, 0.0f, 1.5707963705062866f}},
                {0.0f, {3994.9599609375f, 23359.0f, -7378.10009765625f}, {0.0f, 0.0f, -1.5707963705062866f}},
                {1.0f, {1988.030029296875f, -37327.1015625f, -4511.4599609375f}, {0.0f, 0.0f, -1.5707963705062866f}},
                {1.0f, {-1995.02001953125f, -37327.1015625f, -4511.4599609375f}, {0.0f, 0.0f, 1.5707963705062866f}},
                {0.0f, {-3264.75f, -22848.599609375f, 791.5239868164062f}, {0.0f, 0.0f, 1.5707963705062866f}},
                {0.0f, {3273.2900390625f, -22848.599609375f, 791.5239868164062f}, {0.0f, 0.0f, -1.5707963705062866f}},
                {0.0f, {-29726.69921875f, -10994.7998046875f, -3765.530029296875f}, {0.0f, 0.0f, 3.1415927410125732f}},
                {0.0f, {29716.19921875f, -10994.7998046875f, -3765.530029296875f}, {0.0f, 0.0f, 3.1415927410125732f}},
                {0.0f, {29716.19921875f, 4854.22021484375f, -3758.77001953125f}, {0.0f, 0.0f, 0.0f}},
                {1.0f, {17013.0f, -764.3909912109375f, -1690.6500244140625f}, {0.0f, 0.0f, 0.0f}},
                {1.0f, {-17013.30078125f, -764.5120239257812f, -1690.6500244140625f}, {0.0f, 0.0f, 0.0f}},
                {0.0f, {-29726.69921875f, 4854.22021484375f, -3758.77001953125f}, {0.0f, 0.0f, 0.0f}},
            };
            Vector rotated{0.0f, 0.0f, 0.0f};
            for (int i = 0; i != 12; ++i) {
                int type = (0x9f3u & (1u << i)) ? 14363 : 14365;
                ArrayAdd(reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, type, true)), *enemies);
                KIPlayer *turret = (*enemies)[i + 1];
                turret->player->setMaxHitpoints(hp);
                Vector mountPosition{placements[i].position.x, placements[i].position.y, placements[i].position.z};
                Vector mountRotation{placements[i].rotation.x, placements[i].rotation.y, placements[i].rotation.z};
                rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, mountRotation);
                turret->geometry->setRotation(rotated);
                rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, mountPosition);
                turret->setPosition(Vector{stationPosition.x + rotated.x, stationPosition.y + rotated.y,
                                           stationPosition.z + rotated.z});
                turret->shipGroup = 8;
                turret->player->setAlwaysEnemy(true);
            }
            int coords[] = {0, 0, int(stationPosition.z * 0.5f)};
            enemyRoute = new Route(coords, 3);
            for (int i = 6; i; --i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                KIPlayer *fighter = createShip(8, 0, type, enemyRoute->getWaypoint(), true, false);
                fighter->player->setAlwaysEnemy(true);
                fighter->player->setAlwaysFriend(false);
                ArrayAdd(fighter, *enemies);
            }
            for (int i = 3; i; --i) {
                KIPlayer *fighter = createShip(0, 0, 17, enemyRoute->getWaypoint(), true, false);
                fighter->player->setAlwaysFriend(true);
                fighter->player->setAlwaysEnemy(false);
                ArrayAdd(fighter, *enemies);
            }
            player->setPosition(-70000.0f, 0.0f, -30000.0f);
            player->geometry->setDirection(Vector{1.0f, 0.0f, 1.0f}, Vector{0.0f, 1.0f, 0.0f});
            objectivesA = new Objective(22, 0, this);
            break;
        }
        case 91: {
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            Waypoint *waypoint = new Waypoint(-20000, 0, 60000, nullptr);
            (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(waypoint, 18766, false));
            static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[0])->setDockingType(0);
            (*enemies)[0]->setPosition(waypoint->getPosition());
            (*enemies)[0]->player->setAlwaysFriend(true);
            Player *body = (*enemies)[0]->player;
            body->setHitpoints(body->getHitpoints() / 20);
            (*enemies)[0]->geometry->setRotation(Vector{0.0f, 5.49778748f, 0.0f});
            (*enemies)[0]->setActive(false);
            delete (*enemies)[0]->cargo;
            (*enemies)[0]->cargo = nullptr;
            objectivesB = new Objective(1, 0, this);
            break;
        }
        case 92: {
            int coords[9];
            coords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            coords[1] = 0;
            coords[2] = -70000;
            coords[3] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            coords[4] = 0;
            coords[5] = -100000;
            coords[6] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            coords[7] = 0;
            coords[8] = -140000;
            enemyRoute = new Route(coords, 9);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(4, *enemies);
            for (int i = 0; i != 3; ++i) {
                Route *spawnRoute = enemyRoute;
                int waypoint = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                (*enemies)[i] = createShip(10, 0, 44, spawnRoute->getWaypoint(waypoint), true, false);
                (*enemies)[i]->setToSleep();
                (*enemies)[i]->player->setAlwaysEnemy(true);
                static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                static_cast<PlayerFighter *>((*enemies)[i])->setCloakingPossible(false);
            }
            Waypoint *waypoint = new Waypoint(80000, 0, 110000, nullptr);
            (*enemies)[3] = reinterpret_cast<KIPlayer *>(createStaticObject(waypoint, 17049, false));
            static_cast<PlayerFixedObject *>((*enemies)[3])->setMoving(false);
            (*enemies)[3]->player->setAlwaysFriend(true);
            static_cast<PlayerFixedObject *>((*enemies)[3])->setDockingType(1);
            static_cast<PlayerFixedObject *>((*enemies)[3])->setName(
                String(*static_cast<GameText *>(Globals::gameText)->getText(3209), false));
            delete (*enemies)[3]->cargo;
            (*enemies)[3]->cargo = nullptr;
            objectivesA = new Objective(22, 0, this);
            objectivesB = new Objective(1, 3, this);
            break;
        }
        case 94: {
            int coords[] = {-700000, 0, -700000};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(10, *enemies);
            for (int i = 0; i != 6; ++i) {
                Route *spawnRoute = enemyRoute;
                int waypoint = static_cast<AERandom *>(Globals::rnd)->nextInt(spawnRoute->length());
                (*enemies)[i] = createShip(10, 0, 44, spawnRoute->getWaypoint(waypoint), true, false);
                (*enemies)[i]->setToSleep();
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            (*enemies)[6] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 18781, false));
            static_cast<PlayerFixedObject *>((*enemies)[6])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[6])->setDockingType(2);
            static_cast<PlayerFixedObject *>((*enemies)[6])->setName(
                String(*static_cast<GameText *>(Globals::gameText)->getText(3208), false));
            (*enemies)[6]->player->setAlwaysFriend(true);
            (*enemies)[6]->geometry->rotate(Vector{0.0f, -2.35619450f, 0.0f});
            delete (*enemies)[6]->cargo;
            (*enemies)[6]->cargo = nullptr;
            Waypoint *waypoint = new Waypoint(30000, -5000, 40000, nullptr);
            (*enemies)[7] = reinterpret_cast<KIPlayer *>(createStaticObject(waypoint, 17049, false));
            static_cast<PlayerFixedObject *>((*enemies)[7])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[7])->setDockingType(1);
            static_cast<PlayerFixedObject *>((*enemies)[7])->setName(
                String(*static_cast<GameText *>(Globals::gameText)->getText(3209), false));
            (*enemies)[7]->player->setAlwaysFriend(true);
            delete (*enemies)[7]->cargo;
            (*enemies)[7]->cargo = nullptr;
            int patrolCoords[] = {-1, -1, -1, 20000, -3000, 30000, -1, -1, -1}; // 0x1fd390
            int times[] = {12000, 0, 12000};
            Array<KIPlayer *> *targets = new Array<KIPlayer *>();
            ArraySetLength(3, *targets);
            (*targets)[0] = (*enemies)[6];
            (*targets)[2] = (*enemies)[7];
            Route *route = new Route(patrolCoords, targets, times, 9);
            route->setLoop(true);
            Route *clone = route->clone();
            for (int i = 8; i != 10; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(3);
                if (i == 8) {
                    (*enemies)[8] = createShip(3, 0, type, route->getWaypoint(0), true, false);
                    (*enemies)[8]->setRoute(route);
                } else {
                    (*enemies)[i] = createShip(3, 0, type, clone->getWaypoint(0), true, false);
                    (*enemies)[i]->setRoute(clone);
                }
                (*enemies)[i]->player->setAlwaysFriend(true);
                (*enemies)[i]->player->setNeverAttack(true);
            }
            objectivesB = new Objective(1, 7, this);
            break;
        }
        case 97: {
            int coords[] = {0, 0, 50000};
            enemyRoute = new Route(coords, 3);
            unsigned weak = static_cast<unsigned char>(Globals::isRunningHDonWeakDevice);
            int enemyCount = weak ? 8 : 12;
            unsigned total = weak ? 11 : 19;
            enemies = new Array<KIPlayer *>();
            ArraySetLength(total, *enemies);
            for (int i = 0; i != enemyCount; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            unsigned friendEnd = weak ? 11 : 16;
            for (unsigned i = enemyCount; i < friendEnd; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(2);
                (*enemies)[i] = createShip(2, 0, type, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
            }
            for (unsigned i = friendEnd; i < total; ++i) {
                (*enemies)[i] = createShip(2, 1, 15, enemyRoute->getWaypoint(0), true, false);
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                (*enemies)[i]->player->setAlwaysFriend(true);
            }
            objectivesA = new Objective(18, 0, enemyCount, this);
            break;
        }
        case 100: {
            enemies = new Array<KIPlayer *>();
            ArraySetLength(2, *enemies);
            missionSpawnPosition = player->getPosition();
            int distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
            int sign = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
            missionSpawnPosition.x += float(sign * distance + 20000);
            distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
            sign = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
            missionSpawnPosition.y += float(sign * distance + 10000);
            distance = static_cast<AERandom *>(Globals::rnd)->nextInt(50000);
            sign = static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
            missionSpawnPosition.z += float(sign * distance + 20000);
            int coords[] = {int(missionSpawnPosition.x), int(missionSpawnPosition.y), int(missionSpawnPosition.z)};
            Route *route = new Route(coords, 3);
            for (int i = 0; i != 2; ++i) {
                (*enemies)[i] = createShip(10, 0, 44, route->getWaypoint(), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            objectivesA = new Objective(4, 1, this);
            break;
        }
        case 89: {
            int friendCoords[] = {-63000, 0, 75000, -60000, 0, 110000}; // 0x1fd350
            friendRoute = new Route(friendCoords, 6);
            friendRoute->setLoop(true);
            int enemyCoords[] = {-63000, -5000, 75000, -60000, -5000, 110000}; // 0x1fd368
            enemyRoute = new Route(enemyCoords, 6);
            enemyRoute->setLoop(true);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(12, *enemies);
            (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 16992, false));
            static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(false);
            Player *body = new Player(7500, 9999999, false, false, false);
            (*enemies)[1] = new PlayerFixedObject(18781, 0, body, nullptr, 0.0f, 0.0f, 0.0f);
            AEGeometry *geometry = new AEGeometry(21076, static_cast<PaintCanvas *>(Globals::Canvas), false);
            AEGeometry *childA = new AEGeometry(21876, static_cast<PaintCanvas *>(Globals::Canvas), false);
            AEGeometry *childB = new AEGeometry(22076, static_cast<PaintCanvas *>(Globals::Canvas), false);
            geometry->addChild(childA->transform);
            geometry->addChild(childB->transform);
            delete childA;
            delete childB;
            (*enemies)[1]->setShipGroup(geometry, -1, false);
            (*enemies)[1]->setPosition(Vector{-77000.0f, -3000.0f, 90000.0f});
            (*enemies)[1]->geometry->setRotation(0.0f, 1.5708f, 0.0f);
            body = new Player(7500, 9999999, false, false, false);
            (*enemies)[2] = new PlayerFixedObject(18781, 0, body, nullptr, 0.0f, 0.0f, 0.0f);
            geometry = new AEGeometry(21076, static_cast<PaintCanvas *>(Globals::Canvas), false);
            childA = new AEGeometry(21876, static_cast<PaintCanvas *>(Globals::Canvas), false);
            childB = new AEGeometry(18832, static_cast<PaintCanvas *>(Globals::Canvas), false);
            AEGeometry *childC = new AEGeometry(18833, static_cast<PaintCanvas *>(Globals::Canvas), false);
            geometry->addChild(childA->transform);
            geometry->addChild(childB->transform);
            geometry->addChild(childC->transform);
            delete childA;
            delete childB;
            delete childC;
            (*enemies)[2]->setShipGroup(geometry, -1, false);
            (*enemies)[2]->setVisible(false);
            (*enemies)[2]->setPosition(Vector{-77000.0f, -3000.0f, 90000.0f});
            (*enemies)[2]->geometry->setRotation(0.0f, 1.5708f, 0.0f);
            for (int i = 0; i != 8; ++i) {
                Route *route = i < 4 ? friendRoute : enemyRoute;
                int waypoint = i < 4 ? (i > 1) : (i < 7);
                Waypoint *spawn = route->getWaypoint(waypoint);
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(3);
                KIPlayer *fighter = createShip(3, 0, type, spawn, true, false);
                fighter->setRoute(friendRoute->clone());
                missionSpawnPosition = fighter->getPosition();
                if (missionSpawnPosition.y > -4000.0f && missionSpawnPosition.y < 2000.0f)
                    fighter->setPosition(missionSpawnPosition.x, missionSpawnPosition.y + missionSpawnPosition.y,
                                         missionSpawnPosition.z);
                // Android fighter extension byte +0x1a (+0x13e), purpose not yet named.
                *(reinterpret_cast<unsigned char *>(fighter) + sizeof(KIPlayer) + 0x1a) = 0;
                (*enemies)[i + 3] = fighter;
            }
            (*enemies)[enemies->size() - 1] = createShip(3, 1, 15, nullptr, true, false);
            (*enemies)[enemies->size() - 1]->setPosition(-63000.0f, -3000.0f, 65000.0f);
            break;
        }
        case 102: {
            int enemyCoords[9];
            enemyCoords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            enemyCoords[1] = 0;
            enemyCoords[2] = -70000;
            enemyCoords[3] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            enemyCoords[4] = 0;
            enemyCoords[5] = -100000;
            enemyCoords[6] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            enemyCoords[7] = 0;
            enemyCoords[8] = -140000;
            enemyRoute = new Route(enemyCoords, 9);
            int friendCoords[9];
            friendCoords[0] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            friendCoords[1] = 0;
            friendCoords[2] = -20000;
            friendCoords[3] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            friendCoords[4] = 0;
            friendCoords[5] = -500000;
            friendCoords[6] = static_cast<AERandom *>(Globals::rnd)->nextInt(140000) - 70000;
            friendCoords[7] = 0;
            friendCoords[8] = -70000;
            friendRoute = new Route(friendCoords, 9);
            Waypoint *first = new Waypoint(-50000, 1000, 70000, nullptr);
            Waypoint *second = new Waypoint(20000, 10000, 10000, nullptr);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(10, *enemies);
            (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(first, 18804, true));
            (*enemies)[0]->setPosition(first->getPosition());
            static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(false);
            (*enemies)[0]->field_0x70 = 0;
            static_cast<PlayerFixedObject *>((*enemies)[0])->setDockingType(1);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->player->setHitpoints(9999999);
            (*enemies)[0]->field_0x74 = 1;
            delete (*enemies)[0]->cargo;
            (*enemies)[0]->cargo = nullptr;
            (*enemies)[1] = reinterpret_cast<KIPlayer *>(createStaticObject(first, 21113, true));
            (*enemies)[1]->setPosition(0.0f, 0.0f, 0.0f);
            static_cast<PlayerFixedObject *>((*enemies)[1])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[1])->setDockingType(0);
            (*enemies)[1]->player->setAlwaysFriend(true);
            (*enemies)[1]->player->setHitpoints(9999999);
            (*enemies)[1]->geometry->rotate(Vector{0.0f, 3.14159274f, 0.0f});
            (*enemies)[1]->setActive(false);
            delete (*enemies)[1]->cargo;
            (*enemies)[1]->cargo = nullptr;
            int x = 25000;
            int z = -30000;
            for (int i = 2; i != 6; ++i) {
                (*enemies)[i] = createShip(0, 0, 51, second, true, false);
                Vector position = first->getPosition();
                Vector offset;
                if (i == 2) { offset.x = 10000.0f; offset.y = 6000.0f; offset.z = -20000.0f; }
                else { offset.x = float(x); offset.y = 10000.0f; offset.z = float(z); }
                (*enemies)[i]->setPosition(position + offset);
                (*enemies)[i]->player->setAlwaysFriend(true);
                z -= 5000;
                x += 5000;
                // Android clears two adjacent fighter extension flags +0x19/+0x1a.
                *(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 0x19) = 0;
                *(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 0x1a) = 0;
            }
            int coords[] = {-1, -1, -1, -30000, 1000, 40000, -1, -1, -1}; // 0x1fd3d8
            int times[] = {20000, 0, 20000};
            Array<KIPlayer *> *targets = new Array<KIPlayer *>();
            ArraySetLength(3, *targets);
            (*targets)[0] = (*enemies)[0];
            (*targets)[2] = (*enemies)[1];
            Route *route = new Route(coords, targets, times, 9);
            route->setLoop(true);
            for (int i = 2; i != 6; ++i) (*enemies)[i]->setRoute(i == 2 ? route : route->clone());
            for (int i = 6; i != 10; ++i) {
                (*enemies)[i] = createShip(10, 0, 44, second, true, false);
                (*enemies)[i]->setPosition(Vector{1000000.0f, 1000000.0f, 1000000.0f});
                (*enemies)[i]->setToSleep();
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            objectivesA = new Objective(4, 8, this);
            objectivesB = new Objective(18, 2, 6, this);
            break;
        }
        case 105: {
            int coords[] = {7000000, 7000000, 7000000, 0, 0, 0}; // 0x1fd410; constructor consumes three words
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(5, *enemies);
            for (int i = 0; i != 2; ++i) {
                (*enemies)[i] = createShip(0, 0, 37, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
            }
            for (int i = 2; i != 5; ++i) {
                (*enemies)[i] = createShip(10, 0, 44, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->setToSleep();
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            Vector direction = AbyssEngine::AEMath::VectorNormalize(starSystem->getLightDirection());
            int flightCoords[] = {int(direction.x * 650000.0f), 0, int(direction.z * 650000.0f)};
            Route *route = new Route(flightCoords, 3);
            player->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            player->setRoute(route);
            setPlayerRoute(route);
            for (int i = 0; i != 2; ++i) {
                Vector position = player->getPosition();
                Vector right = player->geometry->getRightVector();
                Vector lateral = position + right * (i == 0 ? 2100.0f : -2100.0f);
                Vector forward = player->geometry->getDirection();
                (*enemies)[i]->setPosition(lateral + forward * 2000.0f);
                (*enemies)[i]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                (*enemies)[i]->setRoute(route->clone());
                (*enemies)[i]->setEnemies(nullptr);
                *(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 0x19) = 0;
                static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
            }
            break;
        }
        case 106: {
            int coords[] = {-500000, 0, -1700000, -500000, 0, -3700000}; // 0x1fd428
            enemyRoute = new Route(coords, 6);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            (*enemies)[0] = createShip(10, 0, 44, enemyRoute->getWaypoint(0), true, false);
            (*enemies)[0]->setPosition(enemyRoute->getWaypoint(0)->getPosition());
            (*enemies)[0]->geometry->setDirection(Vector{-1.0f, 0.0f, 0.0f}, Vector{0.0f, 1.0f, 0.0f});
            (*enemies)[0]->setRoute(enemyRoute->clone());
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->player->setAlwaysEnemy(false);
            (*enemies)[0]->player->setHitpoints(1);
            static_cast<PlayerFighter *>((*enemies)[0])->setCloakingPossible(false);
            break;
        }
        default: {
            switch (campaign) {
        case 131:
            objectivesA = new Objective(4, 0, this);
            break;
        case 135: {
            int coords[] = {7000000, 7000000, 7000000};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(3, *enemies);
            (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 19080, true));
            (*enemies)[0]->setPosition(0.0f, 0.0f, 0.0f);
            static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[0])->setName(
                String(*static_cast<GameText *>(Globals::gameText)->getText(3210), false));
            static_cast<PlayerFixedObject *>((*enemies)[0])->setDockingType(1);
            (*enemies)[0]->player->setAlwaysFriend(true);
            delete (*enemies)[0]->cargo;
            (*enemies)[0]->cargo = nullptr;
            for (int i = 1; i != 3; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->setToSleep();
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            for (unsigned i = 0; i < asteroids->size(); ++i)
                static_cast<PlayerAsteroid *>((*asteroids)[i])->setAsteroidIndex(155);
            break;
        }
        case 137:
            objectivesA = new Objective(4, 4, this);
            break;
        case 139: {
            Globals::status->field_178 = 0;
            int enemyCoords[] = {-70000, 0, 80000, -30000, 0, 60000}; // 0x1fd488
            enemyRoute = new Route(enemyCoords, 6);
            enemyRoute->setLoop(true);
            int friendCoords[] = {-220000, 0, 40000, -180000, 0, 20000}; // 0x1fd4a0
            friendRoute = new Route(friendCoords, 6);
            friendRoute->setLoop(true);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(22, *enemies);
            Waypoint *first = new Waypoint(-50000, -1500, 70000, nullptr);
            Waypoint *second = new Waypoint(-200000, -1500, 30000, nullptr);
            (*enemies)[0] = reinterpret_cast<KIPlayer *>(createStaticObject(first, 19051, true));
            (*enemies)[0]->setPosition(first->getPosition());
            static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[0])->setDockingType(3);
            (*enemies)[0]->player->setHitpoints(9999999);
            static_cast<PlayerFixedObject *>((*enemies)[0])->setName(
                *static_cast<GameText *>(Globals::gameText)->getText(1667) + String(" 1", false));
            delete (*enemies)[0]->cargo;
            (*enemies)[0]->cargo = nullptr;
            (*enemies)[1] = reinterpret_cast<KIPlayer *>(createStaticObject(second, 19051, true));
            (*enemies)[1]->setPosition(second->getPosition());
            static_cast<PlayerFixedObject *>((*enemies)[1])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[1])->setDockingType(3);
            (*enemies)[1]->player->setHitpoints(9999999);
            static_cast<PlayerFixedObject *>((*enemies)[1])->setName(
                *static_cast<GameText *>(Globals::gameText)->getText(1667) + String(" 2", false));
            delete (*enemies)[1]->cargo;
            (*enemies)[1]->cargo = nullptr;
            Matrix matrix;
            const float initial[] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1}; // 0xb9d70
            std::memcpy(matrix.m, initial, sizeof(initial));
            // Five 24-byte position/rotation records from Android 0x1fcedc.
            struct TurretPlacement { Vector position; Vector rotation; };
            static const TurretPlacement placements[] = {
                {{10283.1796875f, -1122.7913818359375f, 26039.26171875f}, {0, 0, 0}},
                {{-10171.173828125f, -1082.7020263671875f, 25906.552734375f}, {0, 0, 0}},
                {{-15624.38671875f, -787.19921875f, -7569.43017578125f}, {0, 0, 0}},
                {{15624.38671875f, -787.19921875f, -7569.43017578125f}, {0, 0, 0}},
                {{0, 4901.47314453125f, 3083.96435546875f}, {0, 0, 0}}
            };
            int placement = 0;
            Vector rotated{0.0f, 0.0f, 0.0f};
            for (int i = 2; i != 12; ++i) {
                (*enemies)[i] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 6774, true));
                PlayerTurret *turret = static_cast<PlayerTurret *>((*enemies)[i]);
                turret->player->setMaxHitpoints(1000);
                Vector mountPosition{placements[placement].position.x, placements[placement].position.y, placements[placement].position.z};
                Vector mountRotation{placements[placement].rotation.x, placements[placement].rotation.y, placements[placement].rotation.z};
                rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, mountRotation);
                turret->geometry->setRotation(rotated);
                rotated = AbyssEngine::AEMath::MatrixRotateVector(matrix, mountPosition);
                Waypoint *anchor = i >= 7 ? second : first;
                float x = anchor->getPosition().x + rotated.x;
                float y = anchor->getPosition().y + rotated.y;
                float z = anchor->getPosition().z + rotated.z;
                turret->setPosition(Vector{x, y, z});
                turret->setScaling(6.0f);
                turret->cargo = nullptr;
                turret->shipGroup = 1;
                placement = placement == 4 ? 0 : placement + 1;
            }
            for (int i = 12; i != 22; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(1);
                Route *route;
                if (i > 16) {
                    (*enemies)[i] = createShip(1, 0, type, second, true, false);
                    route = friendRoute;
                } else {
                    (*enemies)[i] = createShip(1, 0, type, first, true, false);
                    route = enemyRoute;
                }
                (*enemies)[i]->setRoute(route->clone());
            }
            break;
        }
        case 142: {
            int coords[] = {90000, 0, 42000};
            playerRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            (*enemies)[0] = createShip(3, 0, 30, nullptr, true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            (*enemies)[0]->setRoute(playerRoute->clone());
            (*enemies)[0]->player->setHitpoints(9999999);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1599);
            static_cast<PlayerFighter *>((*enemies)[0])->setBoostProb(0);
            KIPlayer *escort = (*enemies)[0];
            Vector position = player->getPosition();
            Vector right = player->geometry->getRightVector();
            Vector lateral = position + right * 2100.0f;
            Vector direction = player->geometry->getDirection();
            escort->setPosition(lateral + direction * 2000.0f);
            Vector facing = player->GetDirVector();
            (*enemies)[0]->geometry->setDirection(facing, Vector{0.0f, 1.0f, 0.0f});
            objectivesA = new Objective(4, 4, this);
            break;
        }
        case 144: {
            int coords[] = {60000, 10000, 100000, -50000, 0, 50000}; // 0x1fd4c8
            enemyRoute = new Route(coords, 6);
            enemyRoute->getWaypoint(0)->reached();
            enemies = new Array<KIPlayer *>();
            ArraySetLength(13, *enemies);
            Vector first = enemyRoute->getWaypoint(1)->getPosition();
            Vector second = enemyRoute->getWaypoint(0)->getPosition();
            Vector direction = first - second;
            direction.y = 0.0f;
            direction = AbyssEngine::AEMath::VectorNormalize(direction);
            Vector origin = enemyRoute->getWaypoint(0)->getPosition();
            (*enemies)[0] = createShip(2, 0, 49, nullptr, true, false);
            (*enemies)[0]->setPosition(enemyRoute->getWaypoint(0)->getPosition());
            (*enemies)[0]->player->setAlwaysEnemy(true);
            (*enemies)[0]->setRoute(enemyRoute->getExactClone());
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1636);
            (*enemies)[0]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(true);
            static_cast<PlayerFighter *>((*enemies)[0])->setCloakingPossible(false);
            Matrix matrix = (*enemies)[0]->geometry->getMatrix();
            missionSpawnPosition = origin;
            int rearX = -65000;
            int frontX = -5000;
            int middleX = -19500;
            int otherFrontX = 8000;
            for (int i = 1; i != 13; ++i) {
                (*enemies)[i] = createShip(10, 0, 44, nullptr, true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                (*enemies)[i]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                static_cast<PlayerFighter *>((*enemies)[i])->setCloakingPossible(false);
                (*enemies)[i]->setRoute(enemyRoute->getExactClone());
                Vector offset;
                if (i <= 2) { offset.x = float(frontX); offset.y = 300.0f; offset.z = 200.0f; }
                else if (i <= 4) { offset.x = float(otherFrontX); offset.y = 300.0f; offset.z = 200.0f; }
                else if (i <= 9) { offset.x = float(middleX); offset.y = i == 7 ? -300.0f : 500.0f; offset.z = -5000.0f; }
                else { offset.x = float(rearX); offset.y = -400.0f; offset.z = -5000.0f; }
                missionSpawnPosition = offset;
                missionSpawnPosition.x += float(static_cast<AERandom *>(Globals::rnd)->nextInt(60) - 30);
                missionSpawnPosition.y += float(static_cast<AERandom *>(Globals::rnd)->nextInt(60) - 30);
                missionSpawnPosition.z += float(static_cast<AERandom *>(Globals::rnd)->nextInt(60) - 30);
                missionSpawnPosition = AbyssEngine::AEMath::MatrixTransformVector(matrix, missionSpawnPosition);
                (*enemies)[i]->setPosition(missionSpawnPosition);
                rearX += 6500;
                frontX += 1500;
                middleX += 3250;
                otherFrontX -= 1500;
            }
            break;
        }
        case 145: {
            int coords[] = {-45000, 0, 60000};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(14, *enemies);
            (*enemies)[0] = createShip(2, 0, 49, nullptr, true, false);
            missionSpawnPosition = Vector{30000.0f, 0.0f, 80000.0f};
            (*enemies)[0]->setPosition(missionSpawnPosition);
            (*enemies)[0]->player->setAlwaysEnemy(true);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1636);
            (*enemies)[0]->setRoute(enemyRoute->clone());
            *(reinterpret_cast<unsigned char *>((*enemies)[0]) + sizeof(KIPlayer) + 0x19) = 0;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(Vector{-50000.0f, 0.0f, 50000.0f} - (*enemies)[0]->getPosition());
            (*enemies)[0]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(true);
            int x = 23000;
            int z = 72000;
            for (int i = 1; i != 13; ++i) {
                (*enemies)[i] = createShip(10, 0, 44, enemyRoute->getWaypoint(0), true, false);
                int y = static_cast<AERandom *>(Globals::rnd)->nextInt(1000);
                y += (static_cast<AERandom *>(Globals::rnd)->nextInt(1000) - 500) * i;
                int randomZ = static_cast<AERandom *>(Globals::rnd)->nextInt(2500);
                (*enemies)[i]->setPosition(float(x), float(y), float(z + randomZ));
                (*enemies)[i]->setRoute(enemyRoute->clone());
                (*enemies)[i]->player->setAlwaysEnemy(true);
                static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                *(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 0x19) = 0;
                static_cast<PlayerFighter *>((*enemies)[i])->setCloakingPossible(false);
                Vector facing = AbyssEngine::AEMath::VectorNormalize(Vector{-50000.0f, 0.0f, 50000.0f} - (*enemies)[i]->getPosition());
                (*enemies)[i]->geometry->setDirection(facing, Vector{0.0f, 1.0f, 0.0f});
                x += 3000;
                z += 2000;
            }
            (*enemies)[enemies->size() - 1] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 19050, true));
            (*enemies)[enemies->size() - 1]->setPosition(-50000.0f, 0.0f, 50000.0f);
            (*enemies)[enemies->size() - 1]->setVisible(false);
            (*enemies)[enemies->size() - 1]->setActive(false);
            (*enemies)[enemies->size() - 1]->name = *static_cast<GameText *>(Globals::gameText)->getText(3207);
            (*enemies)[enemies->size() - 1]->shipGroup = 3;
            (*enemies)[enemies->size() - 1]->player->setHitpoints(9999999);
            (*enemies)[enemies->size() - 1]->field_0x74 = 1;
            objectivesA = new Objective(4, 2, this);
            break;
        }
        case 147:
            objectivesA = new Objective(4, 1, this);
            break;
        case 155:
            objectivesA = new Objective(4, 2, this);
            break;
        case 154: {
            int coords[] = {200000, 0, 70000};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(22, *enemies);
            int friendCoords[] = {0, 0, 40000};
            friendRoute = new Route(friendCoords, 3);
            (*enemies)[0] = createShip(0, 0, 51, friendRoute->getWaypoint(0), true, false);
            (*enemies)[0]->player->setAlwaysFriend(true);
            KIPlayer *escort = (*enemies)[0];
            Vector position = player->getPosition();
            Vector right = player->geometry->getRightVector();
            Vector lateral = position + right * 2100.0f;
            Vector forward = player->geometry->getDirection();
            escort->setPosition(lateral + forward * 2000.0f);
            Vector facing = player->GetDirVector();
            (*enemies)[0]->geometry->setDirection(facing, Vector{0.0f, 1.0f, 0.0f});
            (*enemies)[0]->setEnemies(nullptr);
            *(reinterpret_cast<unsigned char *>((*enemies)[0]) + sizeof(KIPlayer) + 0x19) = 0;
            delete (*enemies)[0]->cargo;
            (*enemies)[0]->cargo = nullptr;
            (*enemies)[1] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 16928, false));
            static_cast<PlayerFixedObject *>((*enemies)[1])->setMoving(false);
            static_cast<PlayerFixedObject *>((*enemies)[1])->setDockingType(3);
            static_cast<PlayerFixedObject *>((*enemies)[1])->setName(
                String(*static_cast<GameText *>(Globals::gameText)->getText(77), false));
            (*enemies)[1]->player->setAlwaysFriend(true);
            (*enemies)[1]->geometry->setRotation(0.0f, 3.1416f, 0.0f);
            (*enemies)[1]->field_0x70 = 0;
            (*enemies)[1]->field_0x74 = 1;
            delete (*enemies)[1]->cargo;
            (*enemies)[1]->cargo = nullptr;
            int patrolCoords[] = {-10000, 1000, 15000, -1, -1, -1, -10000, 1000, 15000}; // 0x1fd510
            int times[] = {0, 10000, 0};
            Array<KIPlayer *> *targets = new Array<KIPlayer *>();
            ArraySetLength(3, *targets);
            (*targets)[1] = (*enemies)[1];
            Route *route = new Route(patrolCoords, targets, times, 9);
            (*enemies)[0]->setRoute(route);
            *(reinterpret_cast<unsigned char *>((*enemies)[0]) + sizeof(KIPlayer) + 0x1a) = 0;
            (*enemies)[0]->player->setVulnerable(false);
            Vector center = {0.0f, 0.0f, 40000.0f};
            for (int i = 2; i != 22; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(9);
                (*enemies)[i] = createShip(9, 0, type, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->setActive(true);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                KIPlayer *spawned = (*enemies)[i];
                int sign = ((~(2 * i) & 2) - 1);
                Vector offset;
                if (i > 10) {
                    int x = (static_cast<AERandom *>(Globals::rnd)->nextInt(100) + 1600) * sign;
                    float y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(100) - 200);
                    int z = static_cast<AERandom *>(Globals::rnd)->nextInt(500) + 1000;
                    offset.x = float(x); offset.y = y; offset.z = float(z);
                } else {
                    int x = (static_cast<AERandom *>(Globals::rnd)->nextInt(100) + 900) * sign;
                    float y = float(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 200);
                    int z = static_cast<AERandom *>(Globals::rnd)->nextInt(400) - 200;
                    offset.x = float(x); offset.y = y; offset.z = float(z);
                }
                spawned->setPosition(center + float(i) * offset);
                AEGeometry *facingGeometry = (*enemies)[i]->geometry;
                Vector ego = player->getPosition();
                Vector fighter = (*enemies)[i]->getPosition();
                Vector direction = AbyssEngine::AEMath::VectorNormalize(ego - fighter);
                facingGeometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            }
            objectivesA = new Objective(4, 11, this);
            break;
        }
        case 157: {
            int friendCoords[] = {70000, 0, 20000, 30000, 10000, 60000}; // 0x1fd548
            friendRoute = new Route(friendCoords, 6);
            friendRoute->getWaypoint(0)->reached();
            int enemyCoords[] = {-110000, 10000, 170000, -110000, 0, 20000}; // 0x1fd560
            enemyRoute = new Route(enemyCoords, 6);
            enemyRoute->getWaypoint(0)->reached();
            enemies = new Array<KIPlayer *>();
            unsigned weak = static_cast<unsigned char>(Globals::isRunningHDonWeakDevice);
            unsigned optional = weak ^ 1;
            ArraySetLength((weak ? 10 : 20) + optional + 3, *enemies);
            unsigned count = weak ? 5 : 10;
            unsigned enemyStart = count + optional;
            unsigned boss = enemyStart + count;
            unsigned stationIndex = boss + 2;
            Station *station = static_cast<Galaxy *>(Globals::galaxy)->getStation(101);
            (*enemies)[stationIndex] = new PlayerStation(station);
            (*enemies)[stationIndex]->name = station->getName();
            (*enemies)[stationIndex]->shipGroup = 3;
            (*enemies)[stationIndex]->setPosition(Vector{-120000.0f, 0.0f, 20000.0f});
            delete (*enemies)[stationIndex]->cargo;
            (*enemies)[stationIndex]->cargo = nullptr;
            delete station;
            for (unsigned i = 0; i != count; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(0);
                (*enemies)[i] = createShip(0, 0, type, friendRoute->getWaypoint(0), true, false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                (*enemies)[i]->setRoute(friendRoute->getExactClone());
            }
            for (unsigned i = count; i < enemyStart; ++i) {
                (*enemies)[i] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 18804, true));
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                (*enemies)[i]->setPosition(30000.0f, -15000.0f, -50000.0f);
                delete (*enemies)[i]->cargo;
                (*enemies)[i]->cargo = nullptr;
            }
            Vector first = enemyRoute->getWaypoint(1)->getPosition();
            Vector second = enemyRoute->getWaypoint(0)->getPosition();
            Vector direction = first - second;
            direction.y = 0.0f;
            direction = AbyssEngine::AEMath::VectorNormalize(direction);
            Vector origin = enemyRoute->getWaypoint(0)->getPosition();
            int firstX = -1000;
            for (unsigned i = 0; i < count; ++i) {
                unsigned slot = enemyStart + i;
                (*enemies)[slot] = createShip(10, 0, 44, nullptr, true, false);
                (*enemies)[slot]->player->setAlwaysEnemy(true);
                (*enemies)[slot]->setVisible(false);
                (*enemies)[slot]->setActive(false);
                (*enemies)[slot]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                static_cast<PlayerFighter *>((*enemies)[slot])->setAIDisabled(true);
                static_cast<PlayerFighter *>((*enemies)[slot])->setCloakingPossible(false);
                (*enemies)[slot]->setRoute(enemyRoute->getExactClone());
                int x = firstX;
                float z = 5000.0f;
                if (i >= 2) {
                    if (i < 5) { x = 3000 * int(i) - 9000; z = 7000.0f; }
                    else if (i < 9) { x = 3000 * int(i) - 20000; z = 9000.0f; }
                    else { x = 0; z = 11000.0f; }
                }
                missionSpawnPosition = Vector{float(x), 0.0f, z};
                missionSpawnPosition.x += float(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100);
                missionSpawnPosition.y += float(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100);
                missionSpawnPosition.z += float(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100);
                (*enemies)[slot]->setPosition(origin + missionSpawnPosition);
                firstX += 2000;
            }
            (*enemies)[boss] = createShip(2, 0, 49, nullptr, true, false);
            (*enemies)[boss]->setPosition(origin);
            (*enemies)[boss]->player->setAlwaysEnemy(true);
            static_cast<PlayerFighter *>((*enemies)[boss])->setAIDisabled(true);
            (*enemies)[boss]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            (*enemies)[boss]->setActive(false);
            (*enemies)[boss]->setVisible(false);
            (*enemies)[boss]->setSpeed(5.0f);
            (*enemies)[boss]->name = *static_cast<GameText *>(Globals::gameText)->getText(1636);
            delete (*enemies)[boss]->cargo;
            (*enemies)[boss]->cargo = nullptr;
            for (unsigned i = optional + 2 * count + 1; i < stationIndex; ++i) {
                (*enemies)[i] = createShip(3, 0, 20, nullptr, true, false);
                (*enemies)[i]->setPosition(50000.0f, 50000.0f, 50000.0f);
                static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                (*enemies)[i]->setActive(false);
                (*enemies)[i]->setVisible(false);
                (*enemies)[i]->name = *static_cast<GameText *>(Globals::gameText)->getText(1623);
            }
            player->setPosition(100000.0f, 0.0f, 20000.0f);
            Vector facing = AbyssEngine::AEMath::VectorNormalize(Vector{-1.0f, -0.2f, -0.5f});
            player->geometry->setDirection(facing, Vector{0.0f, 1.0f, 0.0f});
            break;
        }
        case 158: {
            enemies = new Array<KIPlayer *>();
            ArraySetLength(1, *enemies);
            (*enemies)[0] = createShip(10, 0, 49, nullptr, true, false);
            (*enemies)[0]->setPosition(9000.0f, 0.0f, -13000.0f);
            Vector direction = AbyssEngine::AEMath::VectorNormalize(Vector{-1.0f, 0.0f, 3.0f});
            (*enemies)[0]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            (*enemies)[0]->player->setAlwaysEnemy(true);
            (*enemies)[0]->setSpeed(5.3f);
            Player *body = (*enemies)[0]->player;
            body->setMaxEmpPoints(3 * body->getMaxEmpPoints());
            static_cast<PlayerFighter *>((*enemies)[0])->setCloakingPossible(false);
            (*enemies)[0]->name = *static_cast<GameText *>(Globals::gameText)->getText(1636);
            static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(true);
            delete (*enemies)[0]->cargo;
            (*enemies)[0]->cargo = nullptr;
            for (int i = 3; i; --i) {
                KIPlayer *turret = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 18882, true));
                turret->player->setRadius(800);
                turret->player->setAlwaysEnemy(true);
                turret->player->setAlwaysFriend(false);
                turret->player->setMaxHitpoints(100);
                turret->setPosition(50000.0f, 50000.0f, 50000.0f);
                turret->setActive(false);
                ArrayAdd(turret, *enemies);
            }
            objectivesA = new Objective(22, 0, this);
            break;
        }
        case 132:
        case 133:
        case 134:
        case 136:
        case 138:
        case 140:
        case 141:
        case 143:
        case 146:
        case 148:
        case 149:
        case 150:
        case 151:
        case 152:
        case 153:
        case 156:
            break;
        default: {
            switch (campaign) {
        case 114: {
            enemies = new Array<KIPlayer *>();
            ArraySetLength(6, *enemies);
            Vector position = {0.0f, 0.0f, 0.0f};
            int coords[] = {0, 0, 30000};
            enemyRoute = new Route(coords, 3);
            for (int i = 0; i != 6; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                (*enemies)[i] = createShip(8, 0, type, enemyRoute->getWaypoint(0), true, false);
                position = (*asteroids)[i + (asteroids->size() >> 1)]->getPosition();
                (*enemies)[i]->setPosition(position.x, position.y + 2000.0f, position.z);
                (*enemies)[i]->setToSleep();
                *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>((*enemies)[i]) + sizeof(KIPlayer) + 4) = 7000;
                Player *body = (*enemies)[i]->player;
                body->setMaxHitpoints(2 * body->getMaxHitpoints());
                body = (*enemies)[i]->player;
                body->setHitpoints(body->getMaxHitpoints());
            }
            objectivesA = new Objective(18, 0, 6, this);
            break;
        }
        case 120: {
            int coords[] = {0, 0, -80000};
            enemyRoute = new Route(coords, 3);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(2, *enemies);
            for (int i = 0; i != 2; ++i) {
                (*enemies)[i] = createShip(10, 0, 44, enemyRoute->getWaypoint(0), true, false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                Vector position = player->getPosition();
                Vector right = player->geometry->getRightVector();
                Vector lateral = position + right * 50000.0f;
                Vector forward = player->geometry->getDirection();
                (*enemies)[i]->setPosition(lateral + forward * (i == 0 ? 4000.0f : -4000.0f));
            }
            objectivesA = new Objective(4, 0, this);
            break;
        }
        case 123:
            objectivesA = new Objective(4, 3, this);
            break;
        case 125: {
            int coords[] = {0, 0, 0, 7000000, 7000000, 7000000}; // 0x1fd460
            enemyRoute = new Route(coords, 6);
            enemies = new Array<KIPlayer *>();
            ArraySetLength(15, *enemies);
            for (int i = 0; i != 8; ++i) {
                int type = static_cast<Globals *>(Globals::globals)->getRandomEnemyFighter(8);
                if (i < 2) (*enemies)[i] = createShip(8, 0, type, enemyRoute->getWaypoint(0), true, false);
                else {
                    (*enemies)[i] = createShip(8, 0, type, enemyRoute->getWaypoint(1), true, false);
                    (*enemies)[i]->setToSleep();
                }
                (*enemies)[i]->player->setAlwaysEnemy(true);
            }
            Waypoint *first = new Waypoint(-80000, 0, -160000, nullptr);
            Waypoint *second = new Waypoint(-72000, 0, -190000, nullptr);
            Waypoint *third = new Waypoint(-66000, 53000, -170000, nullptr);
            (*enemies)[8] = reinterpret_cast<KIPlayer *>(createStaticObject(first, 18785, true));
            (*enemies)[9] = reinterpret_cast<KIPlayer *>(createStaticObject(second, 18785, true));
            (*enemies)[10] = reinterpret_cast<KIPlayer *>(createStaticObject(third, 18785, true));
            for (int i = 8; i != 11; ++i) {
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                static_cast<PlayerFixedObject *>((*enemies)[i])->setDockingType(3);
                (*enemies)[i]->player->setAlwaysEnemy(false);
                (*enemies)[i]->player->setAlwaysFriend(true);
                (*enemies)[i]->field_0x74 = 1;
                delete (*enemies)[i]->cargo;
                (*enemies)[i]->cargo = nullptr;
            }
            int x = -72000;
            int y = 0;
            int z = -145000;
            for (int i = 11; i != 15; ++i) {
                (*enemies)[i] = reinterpret_cast<KIPlayer *>(createStaticObject(nullptr, 18786, true));
                static_cast<PlayerFixedObject *>((*enemies)[i])->setMoving(false);
                int randomY = static_cast<AERandom *>(Globals::rnd)->nextInt(1500);
                (*enemies)[i]->setPosition(float(x), float(y + randomY), float(z));
                (*enemies)[i]->geometry->rotate(float(i) - 4.3197f, float(i) - 11.519f, float(i) - 5.7596f);
                (*enemies)[i]->field_0x74 = 1;
                delete (*enemies)[i]->cargo;
                (*enemies)[i]->cargo = nullptr;
                z -= 4000;
                x += 4000;
                y -= 200;
            }
            objectivesA = new Objective(4, 7, this);
            break;
        }
        default: break;
            }
            break;
        }
            }
            break;
        }
    }
    messages = createRadioMessages(Globals::status->getCurrentCampaignMission());
}

void Level::updateOrbit(int dt) {
    Level *thisptr = this;

    int t178 = this->field_178 + dt;
    this->orbitWaveTimer = this->orbitWaveTimer + dt;
    this->field_178 = t178;

    if (this->field_18a != 0) {
        if (Status::gStatus->getSystem() != 0 && this->player->field_0x1c != 0) {
            Status::gStatus->getSystem();
            int race = ((SolarSystem *) Status::gStatus->getSystem())->getRace();
            thisptr->alarmAllFriends(race, 0 != 0);
            Status::gStatus->getSystem();
            ((SolarSystem *) Status::gStatus->getSystem())->getRace();
            thisptr->createRadioMessage(2, 0);
            this->field_18a = 0;
        }
        t178 = this->field_178;
    }

    if (10000 < t178) {
        this->field_178 = 0;
        if (this->enemies != nullptr) {
            for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
                KIPlayer *ki = (*this->enemies)[i];
                if (ki->isJumper() != 0 && ki->isDead() != 0 &&
                    ki->player->isActive() == 0 && (uint8_t) ki->field_0x42 == 0) {
                    ((PlayerFighter *) ki)->revive();
                    ki->setPosition(0, 0, 0);
                    break;
                }
            }
        }
    }

    if (45000 < this->orbitWaveTimer) {
        int hostileAlive = 0;
        this->orbitWaveTimer = 0;
        if (0 < this->hostileCount) {
            if (this->enemies == nullptr)
                return;
            for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
                if ((this->enemies->size() - (unsigned) this->hostileCount) <= i &&
                    ((*this->enemies)[i])->isWingMan() == 0 && ((*this->enemies)[i])->isDead() != 0) {
                    hostileAlive = hostileAlive + ((*this->enemies)[i]->player->isActive() ^ 1);
                }
            }
        }
        if (this->enemies != nullptr) {
            int spawned = 0;
            for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
                int *ki = (int *) (*this->enemies)[i];

                if (0 < this->friendCount && (int) i < this->friendCount &&
                    ((KIPlayer *) ki)->isDead() != 0 && ((KIPlayer *) ki)->player->isActive() == 0 &&
                    (uint8_t)((KIPlayer *) ki)->field_0x42 == 0) {
                    ((PlayerFighter *) ki)->revive();
                    ((KIPlayer *) ki)->setPosition(0, 0, 0);
                }

                if (1 < hostileAlive && this->field_184 < 2 &&
                    0 < this->hostileCount &&
                    (unsigned) ((int) this->enemies->size() - this->hostileCount) <= i) {
                    int race = ((KIPlayer *) ki)->shipGroup;
                    bool secZero = false, secOne = false;
                    if (Status::gStatus->inAlienOrbit() == 0) {
                        SolarSystem *ss = (SolarSystem *) Status::gStatus->getSystem();
                        secZero = ((SolarSystem *) ss)->getSecurityLevel() == 0;
                    }
                    if (Status::gStatus->inAlienOrbit() == 0) {
                        SolarSystem *ss = (SolarSystem *) Status::gStatus->getSystem();
                        secOne = ((SolarSystem *) ss)->getSecurityLevel() == 1;
                    }
                    if (((KIPlayer *) ki)->isDead() != 0 && ((KIPlayer *) ki)->player->isActive() == 0 &&
                        (uint8_t)((KIPlayer *) ki)->field_0x42 == 0) {
                        bool doSpawn;
                        if (race != 9 && !secZero) {
                            doSpawn = secOne && this->field_17c <= 2;
                        } else {
                            doSpawn = true;
                        }
                        if (doSpawn) {
                            this->field_17c = this->field_17c + 1;
                            levelSpawnFar(thisptr, ki);
                            spawned = 1;
                        }
                    }
                }
            }
            if (spawned & 1)
                this->field_184 = this->field_184 + 1;
        }
    }
}

void Level::friendTurnedEnemy(int /*race*/) {
    if (field_188 == 0) {
        field_188 = 1;
        createRadioMessage(0, 0);
    }
}

void Level::reset() {
    if (playerRoute != nullptr) {
        playerRoute->reset();
    }
    if (enemyRoute != nullptr) {
        enemyRoute->reset();
    }
    if (friendRoute != nullptr) {
        friendRoute->reset();
    }
    if (this->enemies != nullptr) {
        for (unsigned int i = 0; i < this->enemies->size(); i = i + 1) {
            (*this->enemies)[i]->reset();
        }
    }
    createPlayer();
    assignGuns();
    connectPlayers();
    if (this->messages != nullptr) {
        for (unsigned int i = 0; i < this->messages->size(); i = i + 1) {
            ((RadioMessage *) (*this->messages)[i])->reset();
        }
    }
    player->setRoute(playerRoute);
    int count;
    if (this->enemies != nullptr) {
        count = 0;
        for (unsigned int i = 0; i < this->enemies->size(); i = i + 1) {
            KIPlayer *e = (*this->enemies)[i];
            if (e->deadFlag == 0 && e->inactiveFlag == 0 && e->field_0x3f == 0) {
                int wm = e->isWingMan();
                if (wm == 0) {
                    if ((char) e->field_0x44 == 0 && e->stealFlagByte == 0) {
                        count = count + (e->countsAsEnemyExcludeFlag ^ 1);
                    }
                }
            }
        }
        count = count - field_120;
    } else {
        count = 0;
    }
    enemiesLeft = count;
    int ast;
    if (asteroids == nullptr) {
        ast = 0;
    } else {
        ast = (int) asteroids->size();
    }
    asteroidsLeft = ast;
    kills = 0;
}

void Level::createSentryGuns() {
    int ship = (int) (intptr_t) Status::gStatus->getShip();
    if (((Ship *) (ship))->getFirstEquipmentOfSort(0x27) != 0) {
        field_b0 = new Array<KIPlayer *>();
        ArraySetLength(9, *field_b0);
        if (enemies == nullptr) {
            enemies = new Array<KIPlayer *>();
        }
        int color = 0x9923e035;
        for (unsigned int i = 0; i < field_b0->size(); i = i + 1) {
            int obj = createStaticObject((Waypoint *) (intptr_t) 0, i / 3 + 0x49c0, 1);
            (*field_b0)[i] = (KIPlayer *) (intptr_t) obj;
            KIPlayer *k = (*field_b0)[i];
            k->player->setRadius(800);
            k->player->setAlwaysFriend(1);
            k->player->setMaxHitpoints(100);
            k->setPosition((float) color, (float) color, (float) color);
            ArrayAdd(k, *enemies);
        }
    }
}

int Level::collideStation(Vector v) {
    if (this->landmarks != nullptr &&
        (*this->landmarks)[0] != 0 &&
        Status::gStatus->inEmptyOrbit() == 0) {
        PlayerFixedObject *obj = (PlayerFixedObject *) (*this->landmarks)[0];
        return obj->collide(v.x, v.y, v.z);
    }
    return 0;
}

void Level::uncoverWanted(int index) {
    if (field_29c == 0) {
        createRadioMessage(0x12, index);
        int **g = (int **) &Status::gStatus;

        for (int i = 1;
             i - 1 < ((Wanted *) ((int *) (*(int *) (*g) + 4))[index])->getNumWingmen();
             i = i + 1) {
            (*enemies)[i]->player->setAlwaysEnemy(1);
            (*enemies)[i]->player->turnEnemy();
        }
    }
}

void Level::update(long long time, bool param) {
    Level *thisptr = this;
    const unsigned int dt = static_cast<unsigned int>(time);

    if (this->flashActive != 0) {
        int remaining = static_cast<int>(static_cast<unsigned>(this->flashDurationA) - dt);
        this->flashDurationA = remaining;
        if (remaining < 0)
            this->flashActive = 0;

        float frac = (float) remaining / (float) this->flashDurationB;
        float red = flashColor.x * frac;
        float green = flashColor.y * frac;
        float blue = flashColor.z * frac;
        float alpha = flashField * frac;
        flashColor.x = i_r > red ? i_r : red;
        flashColor.y = i_g > green ? i_g : green;
        flashColor.z = i_b > blue ? i_b : blue;
        flashField = i_a > alpha ? i_a : alpha;
    }

    bool didMission = false;
    if (Status::gStatus->getMission() != 0) {
        Status::gStatus->getMission();
        if (((Mission *) Status::gStatus->getMission())->isEmpty() == 0) {
            thisptr->updateMissionOrbit(dt);
            didMission = true;
        }
    }
    if (!didMission)
        thisptr->updateOrbit(dt);

    Station *st = (Station *) Status::gStatus->getStation();
    if (((Station *) st)->isAttackedByAliens() != 0 || Status::gStatus->inAlienOrbit() != 0)
        thisptr->updateAlienAttackers(dt);

    if (this->playerGuns != nullptr) {
        for (unsigned i = 0; i < this->playerGuns->size(); i = i + 1) {
            ((ObjectGun *) (*this->playerGuns)[i])->update(dt);
        }
    }
    if (this->enemyGuns != nullptr) {
        for (unsigned i = 0; i < this->enemyGuns->size(); i = i + 1) {
            ((ObjectGun *) (*this->enemyGuns)[i])->update(dt);
        }
    }

    Station *st2 = Status::gStatus->getStation();
    union {
        int bits;
        float value;
    } gammaRate = {Status::gStatus->getGammaRayDamagePerSecond(
        st2->getIndex(), Status::gStatus->getCurrentCampaignMission())};
    PlayerEgo *ego = this->player;
    if (gammaRate.value > 0.0f && ego != nullptr) {
        Item *equipment = Status::gStatus->getShip()->getFirstEquipmentOfSort(0x26);
        float damageRate = gammaRate.value;
        if (equipment != nullptr) {
            int attr = equipment->getAttribute(0x34);
            if (attr > 0)
                damageRate *= (100.0f - static_cast<float>(attr)) / 100.0f;
        }
        Player *playerObject = static_cast<Player *>(ego->player);
        int hpBefore = playerObject->getGammaHP();
        playerObject->damageGamma(damageRate * static_cast<float>(dt) / 1000.0f);
        if (hpBefore >= 15 && playerObject->getGammaHP() <= 14) {
            int hud = this->player->getHUD();
            ((Hud *) (intptr_t) hud)->hudEvent(0x2c, this->player, 0);
        }
        ego = this->player;
    }

    if (ego != nullptr) {
        char dummy[16];
        if (this->field_80 != 0) {
            *(Vec3 *) dummy = ego->getPosition();
            this->field_b4 = *(int *) dummy;
            (this->field_80)->update(dt);
        }
        if (this->field_74 != 0) (this->field_74)->update(dt);
        if (this->particleEmitBoolPtr != nullptr) this->particleEmitBoolPtr->update(dt);
        if (this->skybox2Mesh != 0) {
            *(Vec3 *) dummy = ego->getPosition();
            this->field_b4 = *(int *) dummy;
            (this->skybox2Mesh)->update(dt);
        }
        if (this->particleSystemMgr != nullptr) this->particleSystemMgr->update(dt);
        if (this->particleRenderBoolPtr != nullptr) this->particleRenderBoolPtr->update(dt);
        if (this->field_8c != 0) this->field_8c->update(dt);
        if (this->field_98 != 0) (this->field_98)->update(dt);
        if (this->field_94 != 0) this->field_94->update(dt);
        if (this->field_9c != 0) (this->field_9c)->update(dt);
    }

    if (!param)
        this->lodManager->update(dt);
}

void Level::connectPlayers() {
    if (ApplicationManager::gAppManager->currentModuleId == 5)
        return;

    if (this->enemies != nullptr && this->player != nullptr) {
        Array<Player *> arr;
        ArraySetLength(this->enemies->size(), arr);
        for (unsigned j = 0; j < arr.size(); j = j + 1)
            arr[j] = (*this->enemies)[j]->player;
        ((Player *) this->player->player)->setEnemies(&arr);
    }

    if (this->asteroids != nullptr && this->player != nullptr) {
        Array<Player *> arr;
        ArraySetLength(this->asteroids->size(), arr);
        for (unsigned j = 0; j < arr.size(); j = j + 1)
            arr[j] = (*this->asteroids)[j]->player;
        ((Player *) this->player->player)->addEnemies(&arr);
    }
    if (this->gasClouds != nullptr && this->player != nullptr) {
        Array<Player *> arr;
        ArraySetLength(this->gasClouds->size(), arr);
        for (unsigned j = 0; j < arr.size(); j = j + 1)
            arr[j] = (*this->gasClouds)[j]->player;
        ((Player *) this->player->player)->addEnemies(&arr);
    }

    if (this->enemies == nullptr)
        return;

    int camp = Status::gStatus->getCurrentCampaignMission();
    for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
        int e = (int) (intptr_t)(*this->enemies)[i];
        int eFaction = ((KIPlayer *) (intptr_t) e)->shipGroup;
        int wmAll = ((KIPlayer *) e)->isWingMan();
        unsigned count = 0;
        bool notM24 = camp != 0x24;
        bool notFirst = i != 0;

        for (unsigned k = 0; k < this->enemies->size(); k = k + 1) {
            int o = (int) (intptr_t)(*this->enemies)[k];
            if (o != e && (((~wmAll) & (((KIPlayer *) (intptr_t) o)->shipGroup == eFaction)) == 0)) {
                bool consider;
                if (notM24 || notFirst) {
                    consider = true;
                    if (camp == 0x9a) {
                        int alien = Status::gStatus->inAlienOrbit();
                        if (k == 8 && alien != 0 && ((KIPlayer *) (intptr_t) e)->shipGroup == 8)
                            consider = false;
                    } else if (camp == 0x40 && !(i != 0 && e != 0 && k != 0)) {
                        consider = false;
                    }
                } else {
                    consider = ((KIPlayer *) o)->isWingMan() == 0;
                }
                if (consider) {
                    bool skip = false;
                    if ((char) ((KIPlayer *) (intptr_t) e)->field_0x40 != 0 && ((PlayerFixedObject *) (intptr_t) e)->
                        getDockingType() == 3) {
                        Station *st = (Station *) Status::gStatus->getStation();
                        if (((Station *) st)->stationHasHiddenBlueprint(1) != 0)
                            skip = true;
                    }
                    if (!skip)
                        count = count + 1;
                }
            }
        }

        Array<Player *> arr;
        if (this->player != nullptr)
            count = count + 1;
        ArraySetLength(count, arr);

        Mission *m = (Mission *) Status::gStatus->getMission();
        int mtype = ((Mission *) Status::gStatus->getMission())->getType();
        bool branchA = !(((i & 1) == 0 && mtype == 0xc)) &&
                       ((Mission *) Status::gStatus->getMission())->getType() != 2 && ((Mission *) Status::gStatus->getMission())->
                       getType() != 9;

        bool jumpAlwaysFriend = false;
        if (branchA) {
            int tmp;
            if (m->isCampaignMission() != 0) {
                tmp = Status::gStatus->getCurrentCampaignMission();
                if (tmp == 0x10 && ((KIPlayer *) (intptr_t) e)->shipGroup == 9) jumpAlwaysFriend = true;
            }
            if (!jumpAlwaysFriend && m->isCampaignMission() != 0) {
                tmp = Status::gStatus->getCurrentCampaignMission();
                if (tmp == 0x18 && ((KIPlayer *) (intptr_t) e)->shipGroup == 9) jumpAlwaysFriend = true;
            }
            if (!jumpAlwaysFriend && m->isCampaignMission() != 0) {
                tmp = Status::gStatus->getCurrentCampaignMission();
                if (tmp == 0x1c && ((KIPlayer *) (intptr_t) e)->shipGroup == 9) jumpAlwaysFriend = true;
            }
            if (!jumpAlwaysFriend) {
                if (m->isCampaignMission() != 0) Status::gStatus->getCurrentCampaignMission();
                if (m->isCampaignMission() != 0) Status::gStatus->getCurrentCampaignMission();

                int slot = 0;
                PlayerEgo *ego = this->player;
                if (ego != nullptr) {
                    arr[0] = (Player *) ego->player;
                    slot = 1;
                }
                for (unsigned k = 0; k < this->enemies->size(); k = k + 1) {
                    int o = (int) (intptr_t)(*this->enemies)[k];
                    if (o != e && (((~wmAll) & (((KIPlayer *) (intptr_t) o)->shipGroup == eFaction)) == 0)) {
                        bool consider;
                        if (notM24 || notFirst) {
                            consider = true;
                            if (camp == 0x9a) {
                                int alien = Status::gStatus->inAlienOrbit();
                                if (k == 8 && alien != 0 && ((KIPlayer *) (intptr_t) e)->shipGroup == 8)
                                    consider = false;
                            } else if (camp == 0x40 && !(i != 0 && e != 0 && k != 0)) {
                                consider = false;
                            }
                        } else {
                            consider = ((KIPlayer *) o)->isWingMan() == 0;
                        }
                        if (consider) {
                            bool skip = false;
                            if ((char) ((KIPlayer *) (intptr_t) e)->field_0x40 != 0 &&
                                ((PlayerFixedObject *) (intptr_t) e)->getDockingType() == 3) {
                                Station *st = (Station *) Status::gStatus->getStation();
                                if (((Station *) st)->stationHasHiddenBlueprint(1) != 0)
                                    skip = true;
                            }
                            if (!skip) {
                                arr[slot] = (*this->enemies)[k]->player;
                                slot = slot + 1;
                            }
                        }
                    }
                }
            }
        }

        if (!branchA || jumpAlwaysFriend) {
            if (((KIPlayer *) (intptr_t) e)->player->isAlwaysFriend() == 0) {
                int slot = 0;
                for (unsigned k = 0; k < this->enemies->size(); k = k + 1) {
                    int o = (int) (intptr_t)(*this->enemies)[k];
                    if (o != e && ((KIPlayer *) (intptr_t) o)->stealFlagByte == 0 &&
                        (((~wmAll) & (((KIPlayer *) (intptr_t) o)->shipGroup == eFaction)) == 0)) {
                        bool add;
                        if (notM24 || notFirst) {
                            add = !(camp == 0x40 && !(i != 0 && e != 0 && k != 0));
                        } else {
                            add = ((KIPlayer *) o)->isWingMan() == 0;
                        }
                        if (add) {
                            arr[slot] = (*this->enemies)[k]->player;
                            slot = slot + 1;
                        }
                    }
                }
            } else {
                ArraySetLength(1, arr);
            }
            arr[arr.size() - 1] = (Player *) this->player->player;
        }

        ((KIPlayer *) (intptr_t) e)->player->addEnemies(&arr);

        Status::gStatus->getMission();
        if (eFaction == 10 && ((Mission *) Status::gStatus->getMission())->isEmpty() != 0)
            ((KIPlayer *) (intptr_t) e)->player->setEnemy((Player *) this->player->player);
    }
}

void Level::enemyDied(int r1, bool r2arg) {
    Level *thisptr = this;
    int r2 = (int) r2arg;
    (void) r1;

    this->enemiesLeft = this->enemiesLeft - 1;
    this->kills = this->kills + 1;

    if (r2 != 0) {
        this->killCountA = this->killCountA + 1;
        return;
    }

    Status **statusHolder = &Status::gStatus;
    (*statusHolder)->incKills();
    this->killCountB = this->killCountB + 1;
    if (this->player == nullptr)
        return;

    if ((static_cast<Radar *>(this->player->field_0x14))->hasScanner() == 0) {
        Achievements **achA = &Achievements::gAchievements;
        if (((Achievements *) (*achA))->hasMedal(0x28, 1) == 0) {
            Status *st = *statusHolder;
            int v = st->field_11c;
            if ((char) st->field_120 == 0) {
                v = v + 1;
                st->field_11c = v;
            }
            int goal = ((Achievements *) (*achA))->getValue(0x28, 1);
            int prog = (int) (((float) v / (float) goal) * 100.0f);
            if ((prog % 10) == 0) {
                int hud = this->player->getHUD();
                int cur = (*statusHolder)->field_11c;
                int g2 = ((Achievements *) (*achA))->getValue(0x28, 1);
                ((Hud *) (hud))->hudEventMedal(0x28, (int) (((float) cur / (float) g2) * 100.0f));
            }
            int cur2 = (*statusHolder)->field_11c;
            if (((Achievements *) (*achA))->getValue(0x28, 1) <= cur2)
                (*statusHolder)->field_120 = 1;
        }
    }

    if (this->player != nullptr &&
        this->player->emergencySystemActive() != 0) {
        Achievements **achB = &Achievements::gAchievements;
        if (((Achievements *) (*achB))->hasMedal(0x2b, 1) == 0) {
            Status *st = *statusHolder;
            int v = st->field_13c + 1;
            st->field_13c = v;
            int goal = ((Achievements *) (*achB))->getValue(0x2b, 1);
            if (0 < (int) ((float) v / (float) goal)) {
                int hud = this->player->getHUD();
                int cur = (*statusHolder)->field_13c;
                int g2 = ((Achievements *) (*achB))->getValue(0x2b, 1);
                ((Hud *) (hud))->hudEventMedal(0x2b, (int) (((float) cur / (float) g2) * 100.0f));
            }
            int cur2 = (*statusHolder)->field_13c;
            if (((Achievements *) (*achB))->getValue(0x2b, 1) <= cur2)
                (*statusHolder)->field_140 = 1;
        }
    }
}

// Android 2.0.16 0xc0de4..0xc3ae8: ordered constructor calls, not a rodata table.
Array<RadioMessage *> *Level::createRadioMessages(int set) {
    messages = nullptr;
    switch (set) {
        case 0:
            messages = new Array<RadioMessage *>();
            ArraySetLength(23, *messages);
            (*messages)[0] = new RadioMessage(1668, 17, 5, 1500);
            (*messages)[1] = new RadioMessage(1669, 0, 6, 0);
            (*messages)[2] = new RadioMessage(1670, 0, 6, 1);
            (*messages)[3] = new RadioMessage(1671, 10, 6, 2);
            (*messages)[4] = new RadioMessage(1672, 11, 6, 3);
            (*messages)[5] = new RadioMessage(1673, 9, 6, 4);
            (*messages)[6] = new RadioMessage(1674, 9, 6, 5);
            (*messages)[7] = new RadioMessage(1675, 9, 6, 6);
            (*messages)[8] = new RadioMessage(1676, 0, 6, 7);
            (*messages)[9] = new RadioMessage(1683, 0, 9, 0, 3);
            (*messages)[10] = new RadioMessage(1684, 0, 6, 9);
            (*messages)[11] = new RadioMessage(1685, 0, 6, 10);
            (*messages)[12] = new RadioMessage(1686, 15, 6, 11);
            (*messages)[13] = new RadioMessage(1687, 0, 6, 12);
            (*messages)[14] = new RadioMessage(1688, 0, 6, 13);
            (*messages)[15] = new RadioMessage(1689, 0, 6, 14);
            (*messages)[16] = new RadioMessage(1690, 0, 27, 12);
            (*messages)[17] = new RadioMessage(1691, 15, 6, 16);
            (*messages)[18] = new RadioMessage(1692, 0, 6, 17);
            (*messages)[19] = new RadioMessage(1693, 15, 6, 18);
            (*messages)[20] = new RadioMessage(1694, 0, 6, 19);
            (*messages)[21] = new RadioMessage(1695, 0, 6, 20);
            (*messages)[22] = new RadioMessage(1696, 15, 6, 21);
            break;
        case 1:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(1697, 2, 5, 10000);
            (*messages)[1] = new RadioMessage(1698, 2, 6, 0);
            (*messages)[2] = new RadioMessage(1699, 2, 6, 1);
            break;
        case 7:
            messages = new Array<RadioMessage *>();
            ArraySetLength(2, *messages);
            (*messages)[0] = new RadioMessage(1756, 2, 16, 0);
            (*messages)[1] = new RadioMessage(1757, 0, 6, 0);
            break;
        case 14:
            messages = new Array<RadioMessage *>();
            ArraySetLength(5, *messages);
            (*messages)[0] = new RadioMessage(1820, 18, 5, 10000);
            (*messages)[1] = new RadioMessage(1821, 18, 20, 1);
            (*messages)[2] = new RadioMessage(1822, 0, 6, 1);
            (*messages)[3] = new RadioMessage(1823, 18, 6, 2);
            (*messages)[4] = new RadioMessage(1824, 17, 6, 3);
            break;
        case 16:
            messages = new Array<RadioMessage *>();
            ArraySetLength(5, *messages);
            (*messages)[0] = new RadioMessage(1838, 19, 5, 10000);
            (*messages)[1] = new RadioMessage(1839, 0, 6, 0);
            (*messages)[2] = new RadioMessage(1840, 0, 9, 0, 3);
            (*messages)[3] = new RadioMessage(1841, 1, 6, 2);
            (*messages)[4] = new RadioMessage(1842, 0, 6, 3);
            break;
        case 21:
            messages = new Array<RadioMessage *>();
            ArraySetLength(5, *messages);
            (*messages)[0] = new RadioMessage(1881, 10, 16, 0);
            (*messages)[1] = new RadioMessage(1882, 0, 6, 0);
            (*messages)[2] = new RadioMessage(1883, 10, 25, 2);
            (*messages)[3] = new RadioMessage(1884, 14, 8, 0);
            (*messages)[4] = new RadioMessage(1885, 14, 21, 0);
            break;
        case 24:
            messages = new Array<RadioMessage *>();
            ArraySetLength(5, *messages);
            (*messages)[0] = new RadioMessage(1917, 19, 5, 12000);
            (*messages)[1] = new RadioMessage(1918, 6, 6, 0);
            (*messages)[2] = new RadioMessage(1919, 0, 6, 1);
            (*messages)[3] = new RadioMessage(1920, 6, 22, 3);
            (*messages)[4] = new RadioMessage(1921, 6, 6, 3);
            break;
        case 25:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(1925, 0, 5, 20000);
            (*messages)[1] = new RadioMessage(1926, 6, 6, 0);
            (*messages)[2] = new RadioMessage(1927, 0, 6, 1);
            break;
        case 28:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(1944, 0, 5, 20000);
            (*messages)[1] = new RadioMessage(1945, 19, 6, 0);
            (*messages)[2] = new RadioMessage(1946, 0, 6, 1);
            break;
        case 29:
            messages = new Array<RadioMessage *>();
            ArraySetLength(6, *messages);
            (*messages)[0] = new RadioMessage(1948, 0, 23, 0);
            (*messages)[1] = new RadioMessage(1949, 0, 6, 0);
            (*messages)[2] = new RadioMessage(1950, 0, 6, 1);
            (*messages)[3] = new RadioMessage(1951, 0, 6, 2);
            (*messages)[4] = new RadioMessage(1952, 19, 5, 120000);
            (*messages)[5] = new RadioMessage(1953, 0, 6, 4);
            break;
        case 38:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2029, 21, 5, 15000);
            break;
        case 40:
            messages = new Array<RadioMessage *>();
            ArraySetLength(7, *messages);
            (*messages)[0] = new RadioMessage(2042, 0, 5, 10000);
            (*messages)[1] = new RadioMessage(2043, 8, 6, 0);
            (*messages)[2] = new RadioMessage(2044, 0, 6, 1);
            (*messages)[3] = new RadioMessage(2045, 7, 5, 40000);
            (*messages)[4] = new RadioMessage(2046, 0, 6, 3);
            (*messages)[5] = new RadioMessage(2047, 7, 12, 0);
            (*messages)[6] = new RadioMessage(2048, 0, 24, 0);
            break;
        case 41:
            messages = new Array<RadioMessage *>();
            ArraySetLength(8, *messages);
            (*messages)[0] = new RadioMessage(2052, 0, 5, 80000);
            (*messages)[1] = new RadioMessage(2053, 7, 6, 0);
            (*messages)[2] = new RadioMessage(2054, 0, 6, 1);
            (*messages)[3] = new RadioMessage(2055, 7, 6, 2);
            (*messages)[4] = new RadioMessage(2056, 7, 26, -100000);
            (*messages)[5] = new RadioMessage(2057, 7, 6, 4);
            (*messages)[6] = new RadioMessage(2063, 0, 1, 0);
            (*messages)[7] = new RadioMessage(2064, 0, 6, 6);
            break;
        case 49:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2120, 0, 5, 8000);
            break;
        case 50:
            messages = new Array<RadioMessage *>();
            ArraySetLength(4, *messages);
            (*messages)[0] = new RadioMessage(2121, 63, 5, 8000);
            (*messages)[1] = new RadioMessage(2122, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2123, 63, 6, 1);
            (*messages)[3] = new RadioMessage(2124, 0, 6, 2);
            break;
        case 51:
            messages = new Array<RadioMessage *>();
            ArraySetLength(2, *messages);
            (*messages)[0] = new RadioMessage(2125, 63, 5, 8000);
            (*messages)[1] = new RadioMessage(2126, 0, 6, 0);
            break;
        case 52:
            messages = new Array<RadioMessage *>();
            ArraySetLength(2, *messages);
            (*messages)[0] = new RadioMessage(2127, 63, 5, 8000);
            (*messages)[1] = new RadioMessage(2128, 0, 6, 0);
            break;
        case 53:
        case 54:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2129, 0, 5, 8000);
            break;
        case 55:
            messages = new Array<RadioMessage *>();
            ArraySetLength(2, *messages);
            (*messages)[0] = new RadioMessage(2138, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2139, 0, 6, 0);
            break;
        case 56:
            messages = new Array<RadioMessage *>();
            ArraySetLength(6, *messages);
            (*messages)[0] = new RadioMessage(2154, 27, 5, 8000);
            (*messages)[1] = new RadioMessage(2155, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2156, 28, 16, 0);
            (*messages)[3] = new RadioMessage(2157, 0, 20, 3);
            (*messages)[4] = new RadioMessage(2158, 0, 6, 3);
            (*messages)[5] = new RadioMessage(2159, 27, 28, 0);
            break;
        case 62:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2209, 0, 5, 8000);
            break;
        case 63:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(2223, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2224, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2225, 29, 6, 1);
            break;
        case 64:
            messages = new Array<RadioMessage *>();
            ArraySetLength(7, *messages);
            (*messages)[0] = new RadioMessage(2228, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2229, 20, 6, 0);
            (*messages)[2] = new RadioMessage(2230, 0, 6, 1);
            (*messages)[3] = new RadioMessage(2231, 30, 6, 2);
            (*messages)[4] = new RadioMessage(2232, 0, 6, 3);
            (*messages)[5] = new RadioMessage(2233, 30, 20, 5);
            (*messages)[6] = new RadioMessage(2234, 0, 6, 5);
            break;
        case 65:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(2251, 0, 5, 12000);
            (*messages)[1] = new RadioMessage(2252, 20, 6, 0);
            (*messages)[2] = new RadioMessage(2253, 0, 6, 1);
            break;
        case 67:
            messages = new Array<RadioMessage *>();
            ArraySetLength(12, *messages);
            (*messages)[0] = new RadioMessage(2287, 0, 16, 0);
            (*messages)[1] = new RadioMessage(2288, 31, 6, 0);
            (*messages)[2] = new RadioMessage(2289, 30, 6, 1);
            (*messages)[3] = new RadioMessage(2290, 0, 20, 2);
            (*messages)[4] = new RadioMessage(2291, 31, 6, 3);
            (*messages)[5] = new RadioMessage(2292, 0, 6, 4);
            (*messages)[6] = new RadioMessage(2293, 0, 20, 8);
            (*messages)[7] = new RadioMessage(2294, 31, 6, 6);
            (*messages)[8] = new RadioMessage(2295, 0, 6, 7);
            (*messages)[9] = new RadioMessage(2296, 31, 6, 8);
            (*messages)[10] = new RadioMessage(2297, 0, 6, 9);
            (*messages)[11] = new RadioMessage(2298, 31, 6, 10);
            break;
        case 69:
            messages = new Array<RadioMessage *>();
            ArraySetLength(2, *messages);
            (*messages)[0] = new RadioMessage(2318, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2319, 0, 6, 0);
            break;
        case 70:
            messages = new Array<RadioMessage *>();
            ArraySetLength(4, *messages);
            (*messages)[0] = new RadioMessage(2320, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2321, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2322, 34, 6, 1);
            (*messages)[3] = new RadioMessage(2323, 0, 6, 2);
            break;
        case 73:
            messages = new Array<RadioMessage *>();
            ArraySetLength(8, *messages);
            (*messages)[0] = new RadioMessage(2347, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2348, 0, 16, 0);
            (*messages)[2] = new RadioMessage(2349, 11, 6, 1);
            (*messages)[3] = new RadioMessage(2350, 0, 6, 2);
            (*messages)[4] = new RadioMessage(2351, 0, 27, 1);
            (*messages)[5] = new RadioMessage(2352, 33, 27, 2);
            (*messages)[6] = new RadioMessage(2353, 0, 27, 3);
            (*messages)[7] = new RadioMessage(2354, 0, 6, 6);
            break;
        case 78:
            messages = new Array<RadioMessage *>();
            ArraySetLength(4, *messages);
            (*messages)[0] = new RadioMessage(2409, 0, 5, 2000);
            (*messages)[1] = new RadioMessage(2410, 0, 27, 9);
            (*messages)[2] = new RadioMessage(2411, 0, 27, 11);
            (*messages)[3] = new RadioMessage(2412, 11, 6, 2);
            break;
        case 79:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2413, 0, 5, 6000);
            break;
        case 80:
            messages = new Array<RadioMessage *>();
            ArraySetLength(12, *messages);
            (*messages)[0] = new RadioMessage(2414, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2415, 6, 5, 25000);
            (*messages)[2] = new RadioMessage(2416, 26, 6, 1);
            (*messages)[3] = new RadioMessage(2417, 0, 6, 2);
            (*messages)[4] = new RadioMessage(2418, 6, 6, 3);
            (*messages)[5] = new RadioMessage(2419, 26, 6, 4);
            (*messages)[6] = new RadioMessage(2420, 6, 6, 5);
            (*messages)[7] = new RadioMessage(2421, 26, 6, 6);
            (*messages)[8] = new RadioMessage(2422, 0, 6, 7);
            (*messages)[9] = new RadioMessage(2423, 26, 9, 1, 18);
            (*messages)[10] = new RadioMessage(2424, 26, 27, 12);
            (*messages)[11] = new RadioMessage(2425, 26, 6, 10);
            break;
        case 81:
            messages = new Array<RadioMessage *>();
            ArraySetLength(4, *messages);
            (*messages)[0] = new RadioMessage(2432, 26, 5, 16000);
            (*messages)[1] = new RadioMessage(2433, 31, 6, 0);
            (*messages)[2] = new RadioMessage(2434, 26, 6, 1);
            (*messages)[3] = new RadioMessage(2435, 26, 6, 2);
            break;
        case 87:
            messages = new Array<RadioMessage *>();
            ArraySetLength(6, *messages);
            (*messages)[0] = new RadioMessage(2480, 6, 5, 1500);
            (*messages)[1] = new RadioMessage(2481, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2482, 6, 6, 1);
            (*messages)[3] = new RadioMessage(2483, 0, 6, 2);
            (*messages)[4] = new RadioMessage(2484, 6, 6, 3);
            (*messages)[5] = new RadioMessage(2485, 0, 6, 4);
            break;
        case 89:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2494, 17, 5, 2000);
            break;
        case 91:
            messages = new Array<RadioMessage *>();
            ArraySetLength(7, *messages);
            (*messages)[0] = new RadioMessage(2504, 0, 5, 1500);
            (*messages)[1] = new RadioMessage(2505, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2506, 0, 6, 1);
            (*messages)[3] = new RadioMessage(2507, 60, 6, 2);
            (*messages)[4] = new RadioMessage(2508, 0, 6, 3);
            (*messages)[5] = new RadioMessage(2509, 0, 27, 2);
            (*messages)[6] = new RadioMessage(2510, 60, 27, 4);
            break;
        case 92:
            messages = new Array<RadioMessage *>();
            ArraySetLength(10, *messages);
            (*messages)[0] = new RadioMessage(2513, 59, 5, 1500);
            (*messages)[1] = new RadioMessage(2514, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2515, 59, 6, 1);
            (*messages)[3] = new RadioMessage(2516, 0, 6, 2);
            (*messages)[4] = new RadioMessage(2517, 0, 27, 2);
            (*messages)[5] = new RadioMessage(2518, 0, 6, 4);
            (*messages)[6] = new RadioMessage(2519, 0, 27, 6);
            (*messages)[7] = new RadioMessage(2520, 0, 6, 6);
            (*messages)[8] = new RadioMessage(2521, 0, 9, 0, 3);
            (*messages)[9] = new RadioMessage(2522, 59, 27, 10);
            break;
        case 94:
            messages = new Array<RadioMessage *>();
            ArraySetLength(6, *messages);
            (*messages)[0] = new RadioMessage(2541, 0, 5, 1500);
            (*messages)[1] = new RadioMessage(2542, 59, 6, 0);
            (*messages)[2] = new RadioMessage(2543, 0, 6, 1);
            (*messages)[3] = new RadioMessage(2544, 0, 27, 1);
            (*messages)[4] = new RadioMessage(2545, 0, 6, 3);
            (*messages)[5] = new RadioMessage(2546, 0, 6, 4);
            break;
        case 95:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2555, 17, 27, 1);
            break;
        case 97:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(2582, 0, 5, 1500);
            (*messages)[1] = new RadioMessage(2583, 22, 6, 0);
            (*messages)[2] = new RadioMessage(2584, 0, 6, 1);
            break;
        case 99:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2600, 17, 27, 1);
            break;
        case 100:
            messages = new Array<RadioMessage *>();
            ArraySetLength(2, *messages);
            (*messages)[0] = new RadioMessage(2613, 0, 5, 1500);
            (*messages)[1] = new RadioMessage(2614, 0, 6, 0);
            break;
        case 102:
            messages = new Array<RadioMessage *>();
            ArraySetLength(9, *messages);
            (*messages)[0] = new RadioMessage(2625, 18, 5, 8000);
            (*messages)[1] = new RadioMessage(2626, 18, 6, 0);
            (*messages)[2] = new RadioMessage(2627, 18, 6, 1);
            (*messages)[3] = new RadioMessage(2628, 18, 27, 3);
            (*messages)[4] = new RadioMessage(2629, 0, 30, 1);
            (*messages)[5] = new RadioMessage(2630, 0, 30, 2);
            (*messages)[6] = new RadioMessage(2631, 0, 30, 3);
            (*messages)[7] = new RadioMessage(2632, 18, 27, 5);
            (*messages)[8] = new RadioMessage(2633, 0, 27, 7);
            break;
        case 105:
            messages = new Array<RadioMessage *>();
            ArraySetLength(9, *messages);
            (*messages)[0] = new RadioMessage(2657, 20, 5, 0);
            (*messages)[1] = new RadioMessage(2658, 61, 27, 2);
            (*messages)[2] = new RadioMessage(2659, 0, 27, 5);
            (*messages)[3] = new RadioMessage(2660, 20, 27, 6);
            (*messages)[4] = new RadioMessage(2661, 0, 6, 3);
            (*messages)[5] = new RadioMessage(2662, 0, 27, 7);
            (*messages)[6] = new RadioMessage(2663, 20, 6, 5);
            (*messages)[7] = new RadioMessage(2664, 0, 27, 9);
            (*messages)[8] = new RadioMessage(2665, 0, 27, 10);
            break;
        case 106:
            messages = new Array<RadioMessage *>();
            ArraySetLength(6, *messages);
            (*messages)[0] = new RadioMessage(2666, 0, 5, 1500);
            (*messages)[1] = new RadioMessage(2667, 20, 6, 0);
            (*messages)[2] = new RadioMessage(2668, 20, 6, 1);
            (*messages)[3] = new RadioMessage(2669, 0, 6, 2);
            (*messages)[4] = new RadioMessage(2670, 0, 6, 3);
            (*messages)[5] = new RadioMessage(2671, 0, 27, 6);
            break;
        case 109:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2687, 17, 27, 1);
            break;
        case 114:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(2708, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2709, 10, 27, 1);
            (*messages)[2] = new RadioMessage(2710, 0, 6, 1);
            break;
        case 119:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2745, 17, 27, 1);
            break;
        case 120:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2757, 0, 5, 1500);
            break;
        case 123:
            messages = new Array<RadioMessage *>();
            ArraySetLength(4, *messages);
            (*messages)[0] = new RadioMessage(2785, 22, 5, 8000);
            (*messages)[1] = new RadioMessage(2786, 38, 6, 0);
            (*messages)[2] = new RadioMessage(2787, 22, 6, 1);
            (*messages)[3] = new RadioMessage(2788, 0, 6, 2);
            break;
        case 125:
            messages = new Array<RadioMessage *>();
            ArraySetLength(8, *messages);
            (*messages)[0] = new RadioMessage(2804, 0, 5, 1500);
            (*messages)[1] = new RadioMessage(2814, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2815, 0, 27, 2);
            (*messages)[3] = new RadioMessage(2816, 10, 27, 3);
            (*messages)[4] = new RadioMessage(2817, 0, 27, 4);
            (*messages)[5] = new RadioMessage(2818, 0, 27, 5);
            (*messages)[6] = new RadioMessage(2820, 0, 27, 6);
            (*messages)[7] = new RadioMessage(2821, 0, 27, 7);
            break;
        case 126:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2828, 17, 27, 1);
            break;
        case 131:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2859, 0, 27, 2);
            break;
        case 133:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(2867, 17, 27, 1);
            break;
        case 135:
            messages = new Array<RadioMessage *>();
            ArraySetLength(4, *messages);
            (*messages)[0] = new RadioMessage(2883, 49, 27, 1);
            (*messages)[1] = new RadioMessage(2884, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2885, 0, 27, 2);
            (*messages)[3] = new RadioMessage(2886, 0, 27, 3);
            break;
        case 137:
            messages = new Array<RadioMessage *>();
            ArraySetLength(5, *messages);
            (*messages)[0] = new RadioMessage(2895, 50, 5, 1500);
            (*messages)[1] = new RadioMessage(2896, 0, 6, 0);
            (*messages)[2] = new RadioMessage(2897, 50, 6, 1);
            (*messages)[3] = new RadioMessage(2898, 0, 6, 2);
            (*messages)[4] = new RadioMessage(2899, 50, 6, 3);
            break;
        case 139:
            messages = new Array<RadioMessage *>();
            ArraySetLength(6, *messages);
            (*messages)[0] = new RadioMessage(2924, 0, 5, 8000);
            (*messages)[1] = new RadioMessage(2925, 0, 27, 1);
            (*messages)[2] = new RadioMessage(2926, 50, 6, 1);
            (*messages)[3] = new RadioMessage(2927, 50, 6, 2);
            (*messages)[4] = new RadioMessage(2928, 0, 27, 2);
            (*messages)[5] = new RadioMessage(2929, 0, 27, 3);
            break;
        case 142:
            messages = new Array<RadioMessage *>();
            ArraySetLength(5, *messages);
            (*messages)[0] = new RadioMessage(2952, 2, 5, 1500);
            (*messages)[1] = new RadioMessage(2953, 2, 27, 1);
            (*messages)[2] = new RadioMessage(2954, 0, 6, 1);
            (*messages)[3] = new RadioMessage(2955, 2, 6, 2);
            (*messages)[4] = new RadioMessage(2956, 2, 27, 3);
            break;
        case 144:
            messages = new Array<RadioMessage *>();
            ArraySetLength(4, *messages);
            (*messages)[0] = new RadioMessage(2968, 39, 5, 7000);
            (*messages)[1] = new RadioMessage(2969, 39, 6, 0);
            (*messages)[2] = new RadioMessage(2970, 0, 6, 1);
            (*messages)[3] = new RadioMessage(2971, 39, 6, 2);
            break;
        case 145:
            messages = new Array<RadioMessage *>();
            ArraySetLength(3, *messages);
            (*messages)[0] = new RadioMessage(2972, 0, 5, 7000);
            (*messages)[1] = new RadioMessage(2973, 39, 6, 0);
            (*messages)[2] = new RadioMessage(2974, 39, 27, 5);
            break;
        case 147:
            messages = new Array<RadioMessage *>();
            ArraySetLength(2, *messages);
            (*messages)[0] = new RadioMessage(2988, 0, 5, 7000);
            (*messages)[1] = new RadioMessage(2989, 0, 6, 0);
            break;
        case 154:
            messages = new Array<RadioMessage *>();
            ArraySetLength(12, *messages);
            (*messages)[0] = new RadioMessage(3039, 0, 5, 9000);
            (*messages)[1] = new RadioMessage(3040, 26, 6, 0);
            (*messages)[2] = new RadioMessage(3041, 0, 6, 1);
            (*messages)[3] = new RadioMessage(3042, 26, 6, 2);
            (*messages)[4] = new RadioMessage(3043, 0, 6, 3);
            (*messages)[5] = new RadioMessage(3044, 26, 6, 4);
            (*messages)[6] = new RadioMessage(3045, 0, 6, 5);
            (*messages)[7] = new RadioMessage(3046, 26, 27, 9);
            (*messages)[8] = new RadioMessage(3047, 0, 27, 10);
            (*messages)[9] = new RadioMessage(3048, 55, 6, 8);
            (*messages)[10] = new RadioMessage(3049, 0, 6, 9);
            (*messages)[11] = new RadioMessage(3050, 0, 27, 11);
            break;
        case 157:
            messages = new Array<RadioMessage *>();
            ArraySetLength(17, *messages);
            (*messages)[0] = new RadioMessage(3077, 0, 5, 7000);
            (*messages)[1] = new RadioMessage(3078, 38, 6, 0);
            (*messages)[2] = new RadioMessage(3079, 0, 6, 1);
            (*messages)[3] = new RadioMessage(3080, 39, 6, 2);
            (*messages)[4] = new RadioMessage(3081, 1, 27, 5);
            (*messages)[5] = new RadioMessage(3082, 0, 6, 4);
            (*messages)[6] = new RadioMessage(3083, 6, 12, 21);
            (*messages)[7] = new RadioMessage(3084, 0, 6, 6);
            (*messages)[8] = new RadioMessage(3085, 6, 27, 6);
            (*messages)[9] = new RadioMessage(3086, 26, 6, 8);
            (*messages)[10] = new RadioMessage(3087, 6, 6, 9);
            (*messages)[11] = new RadioMessage(3088, 1, 6, 10);
            (*messages)[12] = new RadioMessage(3089, 39, 27, 8);
            (*messages)[13] = new RadioMessage(3090, 0, 27, 9);
            (*messages)[14] = new RadioMessage(3091, 26, 6, 13);
            (*messages)[15] = new RadioMessage(3092, 0, 6, 14);
            (*messages)[16] = new RadioMessage(3093, 26, 6, 15);
            break;
        case 158:
            messages = new Array<RadioMessage *>();
            ArraySetLength(11, *messages);
            (*messages)[0] = new RadioMessage(3094, 0, 5, 10000);
            (*messages)[1] = new RadioMessage(3095, 0, 6, 0);
            (*messages)[2] = new RadioMessage(3096, 39, 6, 1);
            (*messages)[3] = new RadioMessage(3097, 0, 6, 2);
            (*messages)[4] = new RadioMessage(3098, 39, 6, 3);
            (*messages)[5] = new RadioMessage(3099, 39, 31, 0);
            (*messages)[6] = new RadioMessage(3100, 0, 6, 5);
            (*messages)[7] = new RadioMessage(3101, 39, 12, 0);
            (*messages)[8] = new RadioMessage(3102, 0, 6, 7);
            (*messages)[9] = new RadioMessage(3103, 39, 19, 0);
            (*messages)[10] = new RadioMessage(3104, 0, 1, 0);
            break;
        case 160:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(3121, 17, 27, 1);
            break;
        case 161:
            messages = new Array<RadioMessage *>();
            ArraySetLength(1, *messages);
            (*messages)[0] = new RadioMessage(3125, 17, 27, 1);
            break;
        default:
            break;
    }
    return messages;
}

static inline float clampChannel(float scaled) {
    int v = (int) scaled;
    if (v > 0xfe) {
        v = 0xff;
    }
    if (v < 0xb) {
        v = 10;
    }
    return (float) v;
}

void Level::flashScreen(int type) {
    flashType = type;
    flashActive = 1;
    int dur = 2000;
    if (type == 1) {
        dur = 7000;
    }
    if (type > 2) {
        dur = 10000;
    }
    flashDurationA = dur;
    flashDurationB = dur;
    if (type == 2) {
        flashColor.x = i_r * 1.5f;
        flashColor.y = i_g * 1.5f;
        flashColor.z = i_b * 1.5f;
    } else if (type == 4) {
        flashColor.x = 0;
        flashColor.y = 0;
        flashColor.z = 255.0f;
    } else if (type == 3) {
        flashColor.x = 0;
        flashColor.y = 0;
        flashColor.z = 0;
        player->hitCamera();
    } else {
        float k = (type == 1) ? 8.0f : 5.0f;
        flashColor.x = clampChannel(k * i_r);
        flashColor.y = clampChannel(k * i_g);
        flashColor.z = clampChannel(k * i_b);
    }
    flashField = 255.0f;
}

void Level::createPlayer() {
    Ship *ship = (Ship *) Status::gStatus->getShip();
    Array<Item *> *equip = ship->getEquipment();

    int slots[3];
    slots[0] = ship->getUsedSlots(0);
    slots[1] = ship->getUsedSlots(1);
    slots[2] = ship->getUsedSlots(2);

    Player *pl = (Player *) ::operator new(sizeof(Player));
    new(pl) Player(0x4b0, ship->getMaxHP(), slots[0], slots[1], slots[2]);
    pl->setMaxShieldHP(ship->getMaxShieldHP());
    pl->setMaxArmorHP(ship->getMaxArmorHP());
    pl->field_69 = 1;

    PlayerEgo *ego = (PlayerEgo *) ::operator new(sizeof(PlayerEgo));
    new(ego) PlayerEgo(pl);
    this->player = ego;
    ego->setShip(ship->getIndex(), ship->getRace());
    ego->setLevel(this);

    float yaw = (float) this->field_138 * (1.0f / 65536.0f) * 6.2831855f;
    Vector rot = {0.0f, yaw, 0.0f};
    this->player->geometry->setRotation(rot);

    Array<Array<Gun *> *> *buckets = new Array<Array<Gun *> *>();
    ArraySetLength(3, *buckets);
    for (int t = 0; t < 3; t++) {
        if (slots[t] > 0) {
            Array<Gun *> *b = new Array<Gun *>();
            (*buckets)[t] = b;
            ArraySetLength(slots[t], *b);
        }
    }

    if (equip != nullptr) {
        FileRead *fr = new FileRead();
        Array<Array<Vector *> *> *wpos = fr->loadWeaponPositions(ship->getIndex());
        delete fr;

        if ((*wpos)[3] != nullptr) {
            this->field_a4 = new Array<AEGeometry *>();
            for (unsigned k = 0; k < (*wpos)[3]->size(); k += 2) {
                AEGeometry *g = new AEGeometry(PaintCanvas::gCanvas);
                ArrayAdd(g, *(this->field_a4));
                g->setPosition(*(*(*wpos)[3])[k]);
                g->setScaling(*(*(*wpos)[3])[k + 1]);
            }
        }

        for (unsigned i = 0; i < equip->size(); i++) {
            Item *it = (*equip)[i];
            if (it == nullptr || !it->isWeapon())
                continue;

            int amount = (it->getType() == 1) ? it->getAmount() : -1;
            int dmg = it->getAttribute(0x9);
            int rate = it->getAttribute(0xb);

            if (it->getType() == 0) {
                float dmgFactor = bitsToFloat(ship->getDamageFactor());
                float fireFactor;
                if (dmg > 9 || dmgFactor >= 0.0f) {
                    dmg = (int) (dmgFactor * (float) dmg);
                    fireFactor = bitsToFloat(ship->getFireRateFactor());
                } else {
                    fireFactor = bitsToFloat(ship->getFireRateFactor()) * 0.7f;
                }
                rate = (int) (fireFactor * (float) rate);
            }

            Gun *gun = createGun(it->getIndex(), i, it->getSort(), amount,
                                 dmg, rate, it->getAttribute(0xc), it->getAttribute(0xd));
            gun->itemIndex = it->getIndex();
            gun->weaponType = static_cast<ItemSort>(it->getSort());
            gun->setMagnitude(it->getAttribute(0xe));

            int type = it->getType();
            if (type == 2) {
                (*(*buckets)[2])[--slots[2]] = gun;
                // ARM 0xaf5de-0xaf5ec: the turret uses the first type-2 vector.
                Vector *pos = (*(*wpos)[2])[0];
                ego->setTurretPosition(*pos);
            } else if (type == 0 || type == 1) {
                (*(*buckets)[type])[--slots[type]] = gun;
                gun->setOffset((*(*wpos)[type])[ship->getSlotPos(it)]);
            }
        }

        if (wpos != nullptr) {
            for (unsigned k = 0; k < wpos->size(); k++) {
                if ((*wpos)[k] != nullptr) {
                    ArrayReleaseClasses(*(*wpos)[k]);
                    delete (*wpos)[k];
                    (*wpos)[k] = nullptr;
                }
            }
            ArrayReleaseClasses(*wpos);
            delete wpos;
        }
    }

    for (unsigned t = 0; t < buckets->size(); t++) {
        if ((*buckets)[t] != nullptr)
            this->player->addGun((*buckets)[t], (int) t);
    }
}

void Level::wingmanDied(AbyssEngine::String const &name) {
    Array<String *> *list = Globals::status->getWingmen();
    if (list == 0) {
        return;
    }
    if (__builtin_expect(list->size() < 2, 0)) {
        return Globals::status->setWingmen((Array<String *> *) 0);
    }
    for (unsigned int i = 0; i < list->size(); i = i + 1) {
        String *w = (*list)[i];
        if (w->Compare(name) == 0) {
            ArrayRemove<String *>(w, *list);
            return;
        }
    }
}

static inline void zero16(void *p) {
    ((int *) p)[0] = 0;
    ((int *) p)[1] = 0;
    ((int *) p)[2] = 0;
    ((int *) p)[3] = 0;
}

Level::Level(int mission) {
    field_b4 = 0;
    field_b8 = 0;
    field_bc = 0;
    field_c8 = 0;
    field_cc = 0;
    field_d0 = 0;
    field_18c = 0;
    field_190 = 0;
    field_194 = 0;
    zero16(&field_1c);
    // ARM 0xadb20/28/30 clear four pointer slots, including on wider hosts.
    memset(&this->field_90, 0, 4 * sizeof(field_90));
    memset(&this->particleRenderBoolPtr, 0, 4 * sizeof(particleRenderBoolPtr));
    memset(&this->field_74, 0, 4 * sizeof(field_74));
    skyboxMesh = -1;
    field_08 = -1;
    skyboxTexture = -1;
    field_10 = -1;
    missionPtr = mission;
    collisionVolume = 0;
    field_b0 = nullptr;
    flashColor.x = 0;
    flashColor.y = 0;
    flashColor.z = 0;
    flashDurationA = 0;
    flashDurationB = 0;
    flashActive = 0;

    objectivesA = nullptr;
    objectivesB = nullptr;
    // ARM 0xadb60 clears +0xd8 through the byte at +0x13c inclusive.
    memset(&this->asteroidWaypoint, 0,
           offsetof(Level, friendCargoStolen) - offsetof(Level, asteroidWaypoint) + 1);
    zero16(&this->hostileCount);
    zero16(&this->flashType);
    field_18a = 0;
    field_188 = 0;
    alarmRequested = 0; // ARM 0xadb7a stores a halfword at +0x188.
    movingStarsIndex = -1;
    field_284 = -1;
    field_34 = -1;
    field_38 = -1;
    field_3c = -1;
    field_48 = -1;
    field_4c = -1;
    field_17c = 0;
    field_180 = 0;
    field_184 = 0;
    manualSecondaryActive = 0;
    field_6c = 0;
    field_288 = 0;
    supernovaFlareActive = 0; // ARM 0xadb9c: halfword at +0x288.
    field_a0 = 0;
    field_a4 = nullptr;
    field_a8 = nullptr;

    playerGuns = nullptr;
    enemyGuns = nullptr;
    gasClouds = nullptr;
    enemies = nullptr;
    asteroids = nullptr;
    landmarks = nullptr;
    field_104 = nullptr;
    messages = nullptr;
    miningPlantIndex = -1;
    miningPlant = 0;
    numDeliveredOre = 0;
    numDeliveredPassengers = 0;
    field_29c = 0;
    field_29d = 0; // ARM 0xadbb0: halfword at +0x29c.
    field_29e = 0;
    field_1b4 = -1;
    field_1b8 = -1;
    field_1bc = -1;
    field_1c0 = -1;
}

void Level::createStaticObjects() {
    if (Status::gStatus->inAlienOrbit() == 0) {
        Station *st = (Station *) Status::gStatus->getStation();
        if (((Station *) st)->getIndex() == 0x70 &&
            0x7f < Status::gStatus->getCurrentCampaignMission()) {
            int type;
            if (Status::gStatus->getCurrentCampaignMission() < 0x83) type = 0x493e;
            else if (Status::gStatus->getCurrentCampaignMission() < 0x87) type = 0x4941;
            else if (Status::gStatus->getCurrentCampaignMission() < 0x8a) type = 0x4944;
            else if (Status::gStatus->getCurrentCampaignMission() < 0x8e) type = 0x4947;
            else if (0x91 < Status::gStatus->getCurrentCampaignMission()) type = -1;
            else type = 0x494a;

            if (type != -1) {
                KIPlayer *o = (KIPlayer *) this->createStaticObject((Waypoint *) (intptr_t) 0, type, 0);
                ((PlayerFixedObject *) o)->setPosition(g_cso_posX, 0, g_cso_posZ);
                ((PlayerFixedObject *) o)->setMoving(0);
                AEGeometry *geo = o->geometry;
                o->field_0x70 = 0;

                Vector dir = this->starSystem->getLightDirection();

                Vector up = {0.0f, 1.0f, 0.0f};
                geo->setDirection(dir, up);
                String *txt = GameText::gGameText->getText(**g_cso_textA);
                o->name = *txt;
                o->player->setAlwaysFriend(1);
                if (this->enemies == nullptr) {
                    this->enemies = new Array<KIPlayer *>();
                }
                ArrayAdd(o, *(this->enemies));
                if (o->cargo != 0)
                    delete (Array<void *> *) o->cargo;
                o->cargo = 0;
            }
        }
    }

    if (0x54 < Status::gStatus->getCurrentCampaignMission() &&
        Status::gStatus->getCurrentCampaignMission() != 0x87 &&
        Status::gStatus->inAlienOrbit() == 0) {
        Station *st = (Station *) Status::gStatus->getStation();
        if (((Station *) st)->getIndex() == 0x67) {
            KIPlayer *o = (KIPlayer *) this->createStaticObject((Waypoint *) (intptr_t) 0, 0x4a88, 0);
            ((PlayerFixedObject *) o)->setPosition(0, 0, 0);
            ((PlayerFixedObject *) o)->setMoving(0);
            o->field_0x70 = 1;
            String *txt = GameText::gGameText->getText(**g_cso_textB);
            String name;
            name.Set((txt)->data);
            ((PlayerFixedObject *) o)->setName(name);
            ((PlayerFixedObject *) o)->setDockingType(1);
            if (o->cargo != 0)
                delete (Array<void *> *) o->cargo;
            o->cargo = 0;
            o->player->setAlwaysFriend(1);
            if (this->enemies == nullptr) {
                this->enemies = new Array<KIPlayer *>();
            }
            ArrayAdd(o, *(this->enemies));
        }
    }
}

// Android 2.0.16 0xbe6c0..0xc00c4; iOS sub_12283C independently confirms the branches.
intptr_t Level::createStaticObject(Waypoint *wp, int type, bool jitter) {
    int x = wp ? wp->x : 0;
    int y = wp ? wp->y : 0;
    int z = wp ? wp->z : 0;
    if (jitter) {
        x += static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
        y += static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
        z += static_cast<AERandom *>(Globals::rnd)->nextInt(20000) - 10000;
    }
    KIPlayer *object = nullptr;
    int collisionIndex;
    if (type == 16917) {
        int roll = static_cast<AERandom *>(Globals::rnd)->nextInt(3);
        unsigned short mesh = 16919;
        if (roll == 1) mesh = 16918;
        if (roll == 0) mesh = 16917;
        auto *geometry = new AEGeometry(mesh, Globals::Canvas, false);
        auto *body = new Player(1000, 1, false, false, false);
        object = new PlayerJunk(16917, body, geometry, float(x), float(y), float(z));
        object->geometry->rotate(float(x), float(y), float(z));
    } else {
        // The original creates this wrapper before dispatch, even when a branch
        // subsequently installs its own composite. It does not delete it here.
        AEGeometry *geometry;
        if (type == 6772 || type == 14363 || type == 6774)
            geometry = new AEGeometry(Globals::Canvas);
        else
            geometry = new AEGeometry(static_cast<unsigned short>(type), Globals::Canvas, false);
        if ((unsigned)(type - 18880) < 3 || type == 6772 || type == 6774) {
        createTurret:
            auto *body = new Player(1000, 100, false, false, false);
            object = new PlayerTurret(type, body, geometry, float(x), float(y), float(z));
            object->setLevel(this);
            object->name = *Globals::gameText->getText(1666);
            object->cargo = nullptr;
        } else if (type == 14243) {
            int level;
            if (Globals::status->getLevel() > 20) level = 320;
            else level = 15 * Globals::status->getLevel() + 20;
            int campaign = Globals::status->gameWon() ? 180 : 4 * Globals::status->getCurrentCampaignMission();
            float hp = float(5 * (campaign + level));
            hp += (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * hp;
            auto *body = new Player(7500, int(hp), false, false, false);
            auto *fixed = new PlayerFixedObject(14243, 8, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(14243, Globals::Canvas, false);
            fixed->setShipGroup(base, -1, false);
            auto *childA = new AEGeometry(14244, Globals::Canvas, false);
            auto *childB = new AEGeometry(14245, Globals::Canvas, false);
            fixed->geometry->addChild(childA->transform);
            fixed->geometry->addChild(childB->transform);
            delete childB;
            delete childA;
            fixed->setWreckedMeshId(14246);
            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(1002, nullptr)));
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            object = fixed;
        } else if (type == 14363) {
            goto createTurret;
        } else if (type == 14365) {
            auto *body = new Player(1000, 100, false, false, false);
            object = new PlayerTurret(14365, body, geometry, float(x), float(y), float(z));
            object->setLevel(this);
            object->name = *Globals::gameText->getText(1665);
            object->field_0x25 = 0;
            object->cargo = nullptr;
        } else if (type == 16928) {
            auto *body = new Player(1000, 9999999, false, false, false);
            auto *fixed = new PlayerFixedObject(16928, 0, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(16928, Globals::Canvas, false);
            fixed->setShipGroup(base, -1, false);
            auto *childA = new AEGeometry(16929, Globals::Canvas, false);
            auto *childB = new AEGeometry(16930, Globals::Canvas, false);
            fixed->geometry->addChild(childA->transform);
            fixed->geometry->addChild(childB->transform);
            delete childB;
            delete childA;
            auto *transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(fixed->geometry->transform));
            transform->Update(static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(fixed->geometry->transform))->animationLength, false);
            transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(fixed->geometry->childTransform));
            transform->Update(static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(fixed->geometry->childTransform))->animationLength, false);
            transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(fixed->geometry->parentTransform));
            transform->Update(static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(fixed->geometry->parentTransform))->animationLength, false);
            fixed->field_0x70 = 1;
            fixed->setWreckedMeshId(14246);
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(7, -1);
            delete file;
            fixed->setSpacePoints(points);
            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(1003, nullptr)));
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            object = fixed;
        } else if (type == 19080) {
            auto *body = new Player(1000, 9999999, false, false, false);
            body->setVulnerable(false);
            auto *fixed = new PlayerFixedObject(19080, 3, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(19080, Globals::Canvas, false);
            auto *childA = new AEGeometry(19085, Globals::Canvas, false);
            auto *childB = new AEGeometry(19084, Globals::Canvas, false);
            auto *childC = new AEGeometry(19081, Globals::Canvas, false);
            auto *childD = new AEGeometry(19082, Globals::Canvas, false);
            auto *childE = new AEGeometry(19083, Globals::Canvas, false);
            base->addChild(childA->transform);
            base->addChild(childB->transform);
            base->addChild(childC->transform);
            auto *childF = new AEGeometry(19086, Globals::Canvas, false);
            auto *childG = new AEGeometry(19088, Globals::Canvas, false);
            auto *childH = new AEGeometry(19087, Globals::Canvas, false);
            base->addChild(childF->transform);
            base->addChild(childG->transform);
            base->addChild(childH->transform);
            delete childF;
            delete childG;
            delete childH;
            base->addChild(childD->transform);
            base->addChild(childE->transform);
            delete childA;
            delete childB;
            delete childC;
            delete childD;
            delete childE;
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(19080);
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(1, -1);
            delete file;
            fixed->setSpacePoints(points);
            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(2000, nullptr)));
            fixed->player->radius = 0;
            object = fixed;
        } else if (type == 19050) {
            // Player allocation is an original side effect; PlayerStatic has no Player parameter.
            new Player(1000, 9999999, false, false, false);
            object = new PlayerStatic(19050, geometry, float(x), float(y), float(z));
        } else if ((unsigned)(type - 18750) <= 31 && (0x1249u & (1u << (type - 18750)))) {
            auto *body = new Player(1000, 9999999, false, false, false);
            body->setVulnerable(false);
            auto *fixed = new PlayerFixedObject(type, 3, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(static_cast<unsigned short>(type), Globals::Canvas, false);
            auto *childA = new AEGeometry(static_cast<unsigned short>(type + 2), Globals::Canvas, false);
            auto *childB = new AEGeometry(static_cast<unsigned short>(type + 1), Globals::Canvas, false);
            base->addChild(childA->transform);
            base->addChild(childB->transform);
            if (type == 18762 && Globals::status->getCurrentCampaignMission() == 145) {
                auto *extra = new AEGeometry(18765, Globals::Canvas, false);
                base->addChild(extra->transform);
                delete extra;
            }
            delete childA;
            delete childB;
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(2001, nullptr)));
            object = fixed;
        } else if (type == 18766) {
            int level;
            if (Globals::status->getLevel() > 20) level = 400;
            else level = 15 * Globals::status->getLevel() + 100;
            int campaign = Globals::status->gameWon() ? 180 : 4 * Globals::status->getCurrentCampaignMission();
            float hp = float(5 * (campaign + level));
            hp += (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * hp;
            auto *body = new Player(1000, int(hp), false, false, false);
            auto *fixed = new PlayerFixedObject(18766, 3, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(19072, Globals::Canvas, false);
            auto *childA = new AEGeometry(18766, Globals::Canvas, false);
                auto *childB = new AEGeometry(19073, Globals::Canvas, false);
                auto *childC = new AEGeometry(19074, Globals::Canvas, false);
                auto *childD = new AEGeometry(19075, Globals::Canvas, false);
                auto *childE = new AEGeometry(19076, Globals::Canvas, false);
                base->addChild(childA->transform);
                childA->addChild(childB->transform);
                childA->addChild(childC->transform);
                childA->addChild(childD->transform);
                childA->addChild(childE->transform);
                delete childA;
                delete childB;
                delete childC;
                delete childD;
                delete childE;
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x40b = 1;
            fixed->field_0x70 = 1;
            auto *bounds = new Array<BoundingVolume *>();
            ArraySetLength(2, *bounds);
            (*bounds)[0] = new BoundingAAB(float(x), float(y), float(z), 0.0f, -199.0f, 4708.0f, 980.0f, 1530.0f, 1240.0f);
            (*bounds)[1] = new BoundingAAB(float(x), float(y), float(z), 0.0f, -14.0f, -98.0f, 4500.0f, 1405.0f, 8860.0f);
            fixed->setBV(bounds);
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(18300);
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(3, -1);
            delete file;
            fixed->setSpacePoints(points);
            object = fixed;
        } else if (type == 18781) {
            auto *body = new Player(1000, 9999999, false, false, false);
                                auto *fixed = new PlayerFixedObject(18781, 3, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(21076, Globals::Canvas, false);
            auto *childA = new AEGeometry(21876, Globals::Canvas, false);
            auto *childB = new AEGeometry(22076, Globals::Canvas, false);
            auto *childC = new AEGeometry(18781, Globals::Canvas, false);
            auto *childD = new AEGeometry(18781, Globals::Canvas, false);
            auto *childE = new AEGeometry(18782, Globals::Canvas, false);
            auto *childF = new AEGeometry(18783, Globals::Canvas, false);
            auto *childG = new AEGeometry(18784, Globals::Canvas, false);
            base->addChild(childG->transform);
            base->addChild(childA->transform);
            base->addChild(childB->transform);
            base->addChild(childC->transform);
            base->addChild(childD->transform);
            base->addChild(childE->transform);
            base->addChild(childF->transform);
            delete childA;
            delete childB;
            delete childC;
            delete childD;
            delete childE;
            delete childF;
            delete childG;
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(18300);
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(6, -1);
            delete file;
            fixed->setSpacePoints(points);
            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(111, nullptr)));
            object = fixed;
        } else if (type == 16992) {
            auto *body = new Player(1000, 9999999, false, false, false);
            auto *fixed = new PlayerFixedObject(16992, 0, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(16992, Globals::Canvas, false);
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(18300);
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(2, -1);
            delete file;
            fixed->setSpacePoints(points);
            object = fixed;
        } else if (type == 17049) {
            int level;
            if (Globals::status->getLevel() > 20) level = 400;
            else level = 15 * Globals::status->getLevel() + 100;
            int campaign = Globals::status->gameWon() ? 180 : 4 * Globals::status->getCurrentCampaignMission();
            float hp = float(5 * (campaign + level));
            hp += (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * hp;
            auto *body = new Player(1000, int(hp), false, false, false);
            auto *fixed = new PlayerFixedObject(17049, 3, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(17049, Globals::Canvas, false);
            unsigned int transform = -1u;
                Globals::Canvas->TransformCreate(transform);
                Globals::Canvas->TransformAddMesh(transform, 17055, false);
                base->addChild(transform);
                unsigned int firstA = -1u, firstB = -1u;
                for (int i = 0; i != 3; ++i) {
                    unsigned int a = -1u, b = -1u;
                    Globals::Canvas->TransformCreate(a);
                    Globals::Canvas->TransformCreate(b);
                    Globals::Canvas->TransformAddMesh(a, 17052, false);
                    Globals::Canvas->TransformAddMesh(b, 17053, false);
                    float offset = i == 0 ? -2150.0f : i == 1 ? 2150.0f : 4300.0f;
                    MatrixSetTranslation(*static_cast<Matrix *>(Globals::Canvas->TransformGetLocal(a)), 0.0f, 0.0f, offset);
                    MatrixSetTranslation(*static_cast<Matrix *>(Globals::Canvas->TransformGetLocal(b)), 0.0f, 0.0f, offset);
                    if (firstA == -1u) { firstA = a; firstB = b; }
                    else { Globals::Canvas->TransformAddChild(firstA, a); Globals::Canvas->TransformAddChild(firstB, b); }
                }
                base->addChild(firstA);
                transform = -1u;
                Globals::Canvas->TransformCreate(transform);
                Globals::Canvas->TransformAddMesh(transform, 17054, false);
                base->addChild(transform);
                unsigned short lod = 17050;
                int distance = 35000;
                base->setLodMeshes(&lod, &distance, 1);
                base->setLodChildTransform(firstB);
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x40b = 1;
            fixed->field_0x70 = 1;
            auto *bounds = new Array<BoundingVolume *>();
            ArraySetLength(2, *bounds);
            (*bounds)[0] = new BoundingAAB(float(x), float(y), float(z), 0.0f, -199.0f, 4708.0f, 980.0f, 1530.0f, 1240.0f);
            (*bounds)[1] = new BoundingAAB(float(x), float(y), float(z), 0.0f, -14.0f, -98.0f, 4500.0f, 1405.0f, 8860.0f);
            fixed->setBV(bounds);
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(18300);
            fixed->shipGroup = 3;
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(4, -1);
            delete file;
            fixed->setSpacePoints(points);
            object = fixed;
        } else if (type == 18785) {
            auto *body = new Player(1000, 9999999, false, false, false);
            auto *fixed = new PlayerFixedObject(18785, 2, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(18785, Globals::Canvas, false);
            auto *child0 = new AEGeometry(18831, Globals::Canvas, false);
            base->addChild(child0->transform);
            delete child0;
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(18300);
            fixed->shipGroup = 2;
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(2, -1);
            delete file;
            fixed->setSpacePoints(points);
            fixed->setBV(static_cast<Array<BoundingVolume *> *>(getBoundingVolume(2003, nullptr)));
            object = fixed;
        } else if (type == 18786) {
            auto *body = new Player(0, 9999999, false, false, false);
            auto *fixed = new PlayerFixedObject(18786, 2, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(18786, Globals::Canvas, false);
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 0;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(18300);
            object = fixed;
        } else if (type == 18804) {
            int level;
            if (Globals::status->getLevel() > 20) level = 400;
            else level = 15 * Globals::status->getLevel() + 100;
            int campaign = Globals::status->gameWon() ? 180 : 4 * Globals::status->getCurrentCampaignMission();
            float hp = float(5 * (campaign + level));
            hp += (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * hp;
            auto *body = new Player(0, int(hp), false, false, false);
            auto *fixed = new PlayerFixedObject(18804, 0, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(18804, Globals::Canvas, false);
            auto *childA = new AEGeometry(18805, Globals::Canvas, false);
                auto *childB = new AEGeometry(18807, Globals::Canvas, false);
                auto *childC = new AEGeometry(18808, Globals::Canvas, false);
                auto *childD = new AEGeometry(18809, Globals::Canvas, false);
                auto *childE = new AEGeometry(18810, Globals::Canvas, false);
                auto *childF = new AEGeometry(18806, Globals::Canvas, false);
                base->addChild(childA->transform);
                base->addChild(childB->transform);
                base->addChild(childC->transform);
                base->addChild(childD->transform);
                base->addChild(childE->transform);
                base->addChild(childF->transform);
                delete childA;
                delete childB;
                delete childC;
                delete childD;
                delete childE;
                delete childF;
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            fixed->setWreckedMeshId(18766);
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(5, -1);
            delete file;
            fixed->setSpacePoints(points);
            object = fixed;
            collisionIndex = 2005;
            goto installBounds;
        } else if (type == 19051) {
            auto *body = new Player(0, 9999999, false, false, false);
            auto *fixed = new PlayerFixedObject(19051, 1, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(19051, Globals::Canvas, false);
            auto *child0 = new AEGeometry(19052, Globals::Canvas, false);
            base->addChild(child0->transform);
            delete child0;
            auto *child1 = new AEGeometry(19053, Globals::Canvas, false);
            base->addChild(child1->transform);
            delete child1;
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(18300);
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(8, -1);
            delete file;
            fixed->setSpacePoints(points);
            object = fixed;
            collisionIndex = 2006;
            goto installBounds;
        } else if (type == 21113) {
            auto *body = new Player(0, 9999999, false, false, false);
            body->setVulnerable(false);
            auto *fixed = new PlayerFixedObject(16800, 1, body, nullptr, float(x), float(y), float(z));
            auto *base = new AEGeometry(16800, Globals::Canvas, false);
            fixed->setShipGroup(base, -1, false);
            fixed->field_0x70 = 1;
            static_cast<KIPlayer *>(fixed)->setPosition(float(x), float(y), float(z));
            fixed->setWreckedMeshId(21113);
            auto *file = new FileRead();
            auto *points = file->loadSpacePoints(10, -1);
            delete file;
            fixed->setSpacePoints(points);
            object = fixed;
            collisionIndex = 2002;
            goto installBounds;
        }
    }
    goto setObjectLevel;
installBounds:
    // Android shares this tail for collision IDs 2005, 2006 and 2002.
    static_cast<PlayerFixedObject *>(object)->setBV(
        static_cast<Array<BoundingVolume *> *>(getBoundingVolume(collisionIndex, nullptr)));
setObjectLevel:
    object->setLevel(this);
    return reinterpret_cast<intptr_t>(object);
}

void *Level::getBoundingVolume(int index, AEGeometry * /*unused*/) {
    FileRead *fr = new FileRead();

    Array<int32_t> *coll;
    if (index < 2000)
        coll = fr->loadStationCollision(index);
    else
        coll = fr->loadStaticCollision(index);
    delete fr;

    void *result = 0;
    if (coll != 0) {
        unsigned n = static_cast<unsigned>(coll->data_[0]);
        Array<BoundingVolume *> *arr = new Array<BoundingVolume *>();
        ArraySetLength(n, *arr);
        result = arr;

        int cursor = 1;
        for (unsigned i = 0; i < n; i = i + 1) {
            const int *data = coll->data_;
            const int *r = data + cursor;
            int shape = data[cursor];
            BoundingVolume *bv = 0;
            if (shape == 1) {
                bv = new BoundingAAB(0.0f, 0.0f, 0.0f,
                                     (float) r[1], (float) r[3], -(float) r[2],
                                     2.4f * (float) r[4], 2.4f * (float) r[6], 2.4f * (float) r[5]);
                cursor = cursor + 7;
            } else if (shape == 0) {
                float radius = 0.6f * (float) r[4];
                if (radius < 0.0f) radius = -radius;
                bv = new BoundingSphere(0.0f, 0.0f, 0.0f,
                                        (float) r[1], (float) r[3], -(float) r[2], radius);
                cursor = cursor + 5;
            } else {
                cursor = cursor + 1;
            }
            (*arr)[i] = bv;
        }
        ArrayRelease(*coll);
        delete coll;
    }
    return result;
}

PlayerFixedObject *Level::createShip(int race, int shipClass, int type, Waypoint *wp, bool hostile, bool group) {
    Level * thisptr = this;
    int camp = Globals::status->getCurrentCampaignMission();

    int x = 0, y = 0, z = 0;
    if (wp != 0) {
        x = wp->x;
        y = wp->y;
        z = wp->z;
    }
    int jx = static_cast<AERandom *>(Globals::rnd)->nextInt(40000);
    int jy = static_cast<AERandom *>(Globals::rnd)->nextInt(40000);
    int jz = static_cast<AERandom *>(Globals::rnd)->nextInt(40000);

    int lvl = Globals::status->getLevel();
    if (0x15 <= lvl) lvl = 0x14;
    int ramp = (Globals::status->gameWon() != 0) ? 0xb4 : (camp << 2);
    int hp = ramp + lvl * 0xe + 0x14;
    if (type == 0x33) hp = (int) ((float) hp * 1.7f);
    else if (type == 0x31) hp = (int) ((float) hp * 17.0f);
    else if (type == 0x2c) hp = (int) ((float) hp * 2.25f);
    if ((unsigned) (camp - 0x31) < 8 && (0x8fU >> ((unsigned) (camp - 0x31) & 0xff) & 1) != 0)
        hp = 0x10e;

    int dmgLvl = Globals::status->getLevel();
    if (0x15 <= dmgLvl) dmgLvl = 0x14;
    int empA = dmgLvl * 5 + 0x28;
    int empB;
    if (shipClass == 1) {
        int mul = (type == 0xe) ? 0x19 : 5;
        empA = empA * 3;
        hp = mul * hp;
        empB = 45000;
    } else {
        empB = 15000;
    }
    hp = (int) ((float) hp + (*reinterpret_cast<float *>(Globals::options + 0x2c) - 0.5f) * (float) hp);
    if (camp == 0x9a) {
        int alien = Globals::status->inAlienOrbit();
        hp = hp << (alien & (race == 9));
    }

    int playerHp = (Globals::status->hardCoreMode() != 0) ? 0x28a : 1000;
    Player *pl = new Player(playerHp, hp, 1, 1, 0);
    float fx = (float) (x + jx - 20000);
    float fy = (float) (y + jy - 20000);
    float fz = (float) (z + jz - 20000);
    pl->setEmpData(empA, empB);

    PlayerFixedObject *obj = 0;
    if (shipClass == 0) {
        PlayerFighter *pf = new PlayerFighter(type, race, pl, 0, fx, fy, fz, hostile);
        obj = (PlayerFixedObject *) pf;
        AEGeometry *gg = static_cast<Globals *>(Globals::globals)->getShipGroup(type, race, group);
        obj->setShipGroup(gg, type, hostile != 0);
        if (this->missionPtr != 1 && this->missionPtr != 0x17) {
            AEGeometry *g = obj->parentGeometry;
            if (g == 0) g = obj->geometry;
            this->lodManager->addObject(g);
        }
        if (type == 0x2c || type == 0x31 || type == 0x33) {
            if (type == 0x33) obj->field_0x25 = 0;
            if (type != 0x33 && obj->cargo != 0) {
                delete (Array<void *> *) obj->cargo;
                obj->cargo = 0;
            }
        }
    } else if (shipClass == 1) {
        obj = new PlayerFixedObject(type, race, pl, 0, fx, fy, fz);

        auto *bounds = new Array<BoundingVolume *>();
        // Android createShip 0xc0214: freighter bounds and wrecks.
        switch (race) {
            case 0:
                if (type == 14) {
                    ArraySetLength(11, *bounds);
                (*bounds)[0] = new BoundingAAB(fx, fy, fz, 0.0f, -2240.0f, 21608.0f, 760.0f, 640.0f, 2890.0f);
                (*bounds)[1] = new BoundingAAB(fx, fy, fz, 0.0f, -2342.0f, 18392.0f, 1260.0f, 3560.0f, 3670.0f);
                (*bounds)[2] = new BoundingAAB(fx, fy, fz, 0.0f, -786.0f, 8926.0f, 7410.0f, 7740.0f, 15540.0f);
                (*bounds)[3] = new BoundingAAB(fx, fy, fz, 0.0f, -4950.0f, 9656.0f, 820.0f, 1040.0f, 7150.0f);
                (*bounds)[4] = new BoundingAAB(fx, fy, fz, 0.0f, 3420.0f, -452.0f, 1310.0f, 940.0f, 4880.0f);
                (*bounds)[5] = new BoundingAAB(fx, fy, fz, 0.0f, -270.0f, -9018.0f, 7520.0f, 6870.0f, 20470.0f);
                (*bounds)[6] = new BoundingAAB(fx, fy, fz, 0.0f, -5088.0f, -7962.0f, 1300.0f, 2900.0f, 13210.0f);
                (*bounds)[7] = new BoundingAAB(fx, fy, fz, 0.0f, 3526.0f, -11604.0f, 3260.0f, 880.0f, 10640.0f);
                (*bounds)[8] = new BoundingAAB(fx, fy, fz, 0.0f, 5212.0f, -12332.0f, 3260.0f, 2640.0f, 8410.0f);
                (*bounds)[9] = new BoundingAAB(fx, fy, fz, 0.0f, 2004.0f, -15852.0f, 15860.0f, 2510.0f, 10910.0f);
                (*bounds)[10] = new BoundingAAB(fx, fy, fz, 0.0f, -2610.0f, -15852.0f, 15860.0f, 2510.0f, 10910.0f);
                    obj->setWreckedMeshId(18304);
                } else {
                    ArraySetLength(3, *bounds);
                (*bounds)[0] = new BoundingAAB(fx, fy, fz, 0.0f, -73.0f, 123.0f, 3000.0f, 2860.0f, 10430.0f);
                (*bounds)[1] = new BoundingAAB(fx, fy, fz, 0.0f, -280.0f, -4257.0f, 3470.0f, 1815.0f, 2200.0f);
                (*bounds)[2] = new BoundingAAB(fx, fy, fz, 0.0f, -770.0f, -4279.0f, 5220.0f, 680.0f, 2270.0f);
                    obj->setWreckedMeshId(18302);
                }
                break;
            case 1:
                ArraySetLength(5, *bounds);
                (*bounds)[0] = new BoundingAAB(fx, fy, fz, 0.0f, 351.0f, 5827.0f, 3930.0f, 1590.0f, 2070.0f);
                (*bounds)[1] = new BoundingAAB(fx, fy, fz, 0.0f, 616.0f, 4573.0f, 4440.0f, 2120.0f, 1035.0f);
                (*bounds)[2] = new BoundingAAB(fx, fy, fz, 0.0f, 780.0f, 3303.0f, 4970.0f, 2620.0f, 1895.0f);
                (*bounds)[3] = new BoundingAAB(fx, fy, fz, 0.0f, 456.0f, -309.0f, 2695.0f, 3530.0f, 6600.0f);
                (*bounds)[4] = new BoundingAAB(fx, fy, fz, 0.0f, 248.0f, -5587.0f, 3325.0f, 3020.0f, 4205.0f);
                obj->setWreckedMeshId(18303);
                break;
            case 2:
                ArraySetLength(3, *bounds);
                (*bounds)[0] = new BoundingAAB(fx, fy, fz, 0.0f, -85.0f, 24.0f, 4335.0f, 1245.0f, 10880.0f);
                (*bounds)[1] = new BoundingAAB(fx, fy, fz, 0.0f, 710.0f, 292.0f, 2990.0f, 935.0f, 11450.0f);
                (*bounds)[2] = new BoundingAAB(fx, fy, fz, 0.0f, 1510.0f, -2886.0f, 2750.0f, 1010.0f, 2750.0f);
                obj->setWreckedMeshId(18301);
                break;
            case 3:
                ArraySetLength(2, *bounds);
                (*bounds)[0] = new BoundingAAB(fx, fy, fz, 0.0f, -199.0f, 4708.0f, 980.0f, 1530.0f, 1240.0f);
                (*bounds)[1] = new BoundingAAB(fx, fy, fz, 0.0f, -14.0f, -98.0f, 4500.0f, 1405.0f, 8860.0f);
                obj->setWreckedMeshId(18300);
                break;
            default:
                break;
        }
        obj->setBV(bounds);
        AEGeometry *gg = static_cast<Globals *>(Globals::globals)->getShipGroup(type, race, 0);
        obj->setShipGroup(gg, type, false);
        this->lodManager->addObject(obj->geometry);
        *(unsigned char *) &obj->field_0x40 = 1;
    }

    if (obj != 0)

        obj->setLevel(thisptr);
    return obj;
}

void Level::almostKillWanted(int index) {
    if (field_29e != 0) {
        return;
    }
    field_29e = 1;
    if (Status::gStatus->isStorylineWanted(index) == 0) {
        return;
    }
    int m = (int) (intptr_t) ::operator new(0x78);
    new((void *) (intptr_t) m) Mission(4, 0, Status::gStatus->getStation()->getIndex());
    ((Mission *) (m))->setCampaignMission(true);
    ((Mission *) (m))->setWon(1);
    Status::gStatus->setMission((Mission *) (intptr_t) m);
    Status::gStatus->setCampaignMission((Mission *) (intptr_t) m);
    delete objectivesA;
    objectivesA = nullptr;
    objectivesA = new Objective(3, 0, 0, this);
    KIPlayer *e = (*this->enemies)[0];
    e->player->setAlwaysEnemy(0);
    ((Player *) (int) (intptr_t)(*this->enemies)[1])->resetDamageDoneByPlayer();
    *(unsigned char *) &e->player->enemyFlags = 0;
    e->reviveLockFlag = 1;
    Array<Wanted *> *w = Status::gStatus->getWanted();
    return w->wantedListData[index]->setActive(0 != 0);
}

void Level::assignGuns() {
    if (this->enemyGuns != nullptr) {
        if (this->enemyGuns) {
            ArrayReleaseClasses(*this->enemyGuns); delete this->enemyGuns;
            this->enemyGuns = nullptr;
        }
    }
    this->enemyGuns = nullptr;

    float lvlPower = (float) (Status::gStatus->getLevel() - 2) * g_ag_perLevel;
    if (lvlPower >= 20.0f) lvlPower = 20.0f;
    if (lvlPower < 0.0f) lvlPower = (float) (Status::gStatus->getLevel() - 2) * g_ag_perLevel;

    float diffScale = ((DifficultyRecord *) (intptr_t) * g_ag_diffRec)->hpScale;
    int camp = Status::gStatus->getCurrentCampaignMission();
    int basePower = (int) (lvlPower + lvlPower * (diffScale - 0.5f));
    Wanted *wanted = (Wanted *) Status::gStatus->getWantedInCurrentOrbit();
    if (0x15 < basePower) basePower = 0x16;
    if (this->enemies == nullptr)
        return;

    unsigned slots = 0;
    for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
        int e = (int) (intptr_t)(*this->enemies)[i];
        if (e != 0 && (char) ((KIPlayer *) (intptr_t) e)->field_0x25 != 0) {
            int add = (((KIPlayer *) e)->isWingMan() != 0) ? 2 : 1;
            slots = slots + add;
        }
    }

    this->enemyGuns = new Array<AbstractGun *>();
    ArraySetLength(slots, *(this->enemyGuns));

    int baseDmg = (basePower == 0) ? 3 : (basePower + 2);
    if (camp == 4) baseDmg = 1;

    int outIdx = 0;
    for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
        int e = (int) (intptr_t)(*this->enemies)[i];
        if (e == 0)
            continue;
        if ((char) ((KIPlayer *) (intptr_t) e)->field_0x25 == 0)
            goto wingmanExtra;

        {
            if (this->missionPtr == 2)
                ((KIPlayer *) (intptr_t) e)->player->setPlayShootSound(0, 2);

            int color = 0x41800000;
            int dmg;
            Status::gStatus->getMission();
            if (((Mission *) Status::gStatus->getMission())->getType() == 6 && ((KIPlayer *) (intptr_t) e)->player->
                isAlwaysFriend() == 0) {
                color = 0x41e00000;
                dmg = Status::gStatus->getLevel() + baseDmg;
            } else {
                Status::gStatus->getMission();
                dmg = baseDmg;
                if (((Mission *) Status::gStatus->getMission())->getType() == 0xc && ((KIPlayer *) (intptr_t) e)->player->
                    isAlwaysFriend() != 0) {
                    color = 0x41e00000;
                    dmg = Status::gStatus->getLevel() + baseDmg;
                }
            }

            if (((KIPlayer *) e)->isWingMan() == 0 && ((KIPlayer *) (intptr_t) e)->player->isAlwaysFriend() == 0 &&
                (*this->enemies)[i] &&
                (*this->enemies)[i]->shipGroup == 9) {
                if (camp != 0x10) dmg = (int) ((float) dmg * 0.8f);
                else dmg = dmg + dmg;
            } else {
                int hard = ((0x8fU >> ((unsigned) (camp - 0x31) & 0xff) & 1) != 0 &&
                            (unsigned) (camp - 0x31) <= 7)
                               ? 5
                               : dmg;
                dmg = hard;
            }

            if (camp == 0x50 && ((PlayerTurret *) (*this->enemies)[i])->field_0x3e != 0)
                dmg = (int) ((float) dmg * 1.7f);

            if (camp == 0x46 && ((KIPlayer *) e)->isWingMan() == 0)
                dmg = (int) ((float) dmg * 2.5f);

            if (Status::gStatus->getMission() != 0) {
                Status::gStatus->getMission();
                if (((Mission *) Status::gStatus->getMission())->getType() == 0xb7) dmg = 1;
            }

            Gun *gun = (Gun *) ::operator new(0x114);
            int won = Status::gStatus->gameWon();
            int rampMis = (won != 0) ? 0x2d : Status::gStatus->getCurrentCampaignMission();
            new(gun) Gun(0, dmg, 4, -1, 3000, rampMis * -2 + 600, (float) color, Vector{0.0f, 0.0f, 0.0f},
                         Vector{0.0f, 0.0f, 0.0f});
            gun->setFriendGun(1);
            gun->setLevel(this);
            gun->setIndex(0);
            gun->weaponType = ITEM_SORT_LASER;

            int res;
            switch ((int) (intptr_t)(*this->enemies)[i] + 0x28
                        ? (*this->enemies)[i]->shipGroup
                        : 0) {
                case 0: gun->weaponType = ITEM_SORT_LASER;
                    gun->setIndex(0);
                    res = 0x1a62;
                    break;
                case 1: gun->setIndex(3);
                    res = 0x1a68;
                    break;
                case 2: gun->weaponType = ITEM_SORT_LASER;
                    gun->setIndex(7);
                    res = 0x1a6c;
                    break;
                case 3: gun->setIndex(0x19);
                    res = 0x1a92;
                    break;
                case 9: gun->setIndex(5);
                    res = 0x1a6a;
                    break;
                case 10: gun->setIndex(0xe5);
                    res = 0x4a93;
                    gun->damage = (int) ((float) gun->damage * 0.7f);
                    break;
                default: gun->weaponType = ITEM_SORT_BLASTER;
                    gun->setIndex(0x13);
                    res = 0x1a8b;
                    break;
            }

            int camp2 = Status::gStatus->getCurrentCampaignMission();
            PlayerTurret *turret = (PlayerTurret *) (*this->enemies)[i];
            if (turret->field_0x3e != 0) {
                int host = (int) (intptr_t) turret->getHost();
                if (host != 0 && (((KIPlayer *) (intptr_t) host)->shipGroupFlag == 0x2d || ((KIPlayer *) (intptr_t)
                                      host)->shipGroupFlag == 0x33)) {
                    gun->weaponType = ITEM_SORT_AUTO_CANNON;
                    gun->setIndex(0x16);
                    res = 0x1a8e;
                    gun->damage = (int) ((double) gun->damage * 0.5);
                } else {
                    KIPlayer *k = (*this->enemies)[i];
                    if ((uint8_t) k->field_0x3f == 0) {
                        gun->weaponType = ITEM_SORT_BLASTER;
                        if ((uint8_t) k->shipGroup == 1) {
                            gun->setIndex(0xf);
                            res = 0x1a87;
                        } else {
                            gun->setIndex(0x14);
                            res = 0x1a8c;
                        }
                    } else {
                        int kt = k->getType();
                        int statRow;
                        if (kt == 0x49c1) {
                            gun->weaponType = ITEM_SORT_BLASTER;
                            gun->setIndex(0x14);
                            gun->offset = Vector{0.0f, 0.0f, 250.0f};
                            statRow = 0xd4;
                            res = 0x1a8d;
                        } else if (kt == 0x49c0) {
                            gun->weaponType = ITEM_SORT_LASER;
                            gun->setIndex(2);
                            gun->offset = Vector{0.0f, 0.0f, 250.0f};
                            statRow = 0xd3;
                            res = 0x1a64;
                        } else {
                            gun->weaponType = ITEM_SORT_BLASTER;
                            gun->setIndex(0xe);
                            gun->offset = Vector{0.0f, 0.0f, 300.0f};
                            statRow = 0xd5;
                            res = 0x1a86;
                        }
                        gun->field_0xa8 = 1;

                        Item *stat = (Item *) (intptr_t)(*(int *) (*(int *) (*g_ag_shipTbl + 4) + statRow * 4));
                        gun->damage = stat->getAttribute(0x9);
                        gun->fireDelay = stat->getAttribute(0xb);
                        gun->initialLifetime = stat->getAttribute(0xc);
                        gun->field_0x50 = (float) stat->getAttribute(0xd);
                        if (camp2 == 0x9e && kt == 0x49c2 && ((KIPlayer *) (intptr_t) e)->player->isAlwaysEnemy() !=
                            0) {
                            gun->field_0x50 = gun->field_0x50 * 1.2f;
                            gun->damage = (int) ((float) gun->damage * 1.5f);
                            Player *pp = (*this->enemies)[i]->player;
                            int mhp = pp->getMaxHitpoints();
                            pp->setMaxHitpoints((int) ((float) mhp * 5.0f));
                        }
                    }
                }
            }

            if (camp2 == 7) {
                if ((*this->enemies)[i]->shipGroup == 8)
                    gun->damage = (int) ((float) gun->damage * 0.5f);
            } else if (camp2 == 0x46 && ((KIPlayer *) e)->isWingMan() == 0) {
                gun->setIndex(0xb7);
                res = 0x37d9;
            }

            if (Status::gStatus->getMission() != 0 && ((Mission *) Status::gStatus->getMission())->isCampaignMission() != 0) {
                if (**g_ag_statusB == Status::gStatus->getCurrentCampaignMission() &&
                    2 < **g_ag_alienCnt &&
                    ((*this->enemies)[i])->isEnemy() != 0)
                    gun->damage = (int) ((float) gun->damage * 0.7f);
            } else {
                int host7c = ((KIPlayer *) (*this->enemies)[i])->shipGroupFlag;
                if (camp2 == 0x91 && host7c == 0x31) {
                    gun->weaponType = ITEM_SORT_CLUSTER_MISSILE;
                    gun->setIndex(0xd6);
                    gun->damage = gun->damage << 1;
                    res = 0x37a0;
                } else if ((unsigned) (camp2 - 0x9d) < 2 && host7c == 0x31) {
                    gun->weaponType = ITEM_SORT_LASER;
                    gun->setIndex(7);
                    gun->damage = gun->damage * 3;
                    res = 0x1a6c;
                } else if (wanted != 0 && ((KIPlayer *) (*this->enemies)[i])->field_0x42 != 0) {
                    int w = wanted->getWeapon();
                    gun->setIndex(w);
                    int attr = ((Item *) (intptr_t)(*(int *) (*(int *) (*g_ag_itemTblA + 4) + w * 4)))->
                            getAttribute(0x2);
                    res = g_ag_weaponDmg[w];
                    gun->weaponType = static_cast<ItemSort>(attr);
                    gun->damage = gun->damage << 2;
                }
            }

            int sc = gun->weaponType;
            if (sc == ITEM_SORT_CLUSTER_MISSILE || sc == ITEM_SORT_MISSILE) {
                RocketGun *r = (RocketGun *) ::operator new(0xe8);
                new(r) RocketGun(gun->itemIndex, gun, res, 0, 0, sc,
                                 sc == ITEM_SORT_MISSILE ? 1 : 0, this);
                (*this->enemyGuns)[outIdx] = (ObjectGun *) r;
                gun->field_0x50 = 8.0f;
                gun->initialLifetime = 10000;
                gun->fireDelay = 3000;
                gun->damage = gun->damage << 2;
            } else {
                ObjectGun *o = (ObjectGun *) ::operator new(0xb0);
                new(o) ObjectGun(0, gun, res, 0x2711, this);
                (*this->enemyGuns)[outIdx] = o;
            }
            ((KIPlayer *) (*this->enemies)[i])->addGun((Gun *) gun, 0);

            Globals::gGlobals
                    ->addSoundResourceToList((*this->enemies)[i]->shipGroup == 9 ? 0x3e : 0x3d);

            KIPlayer *shipNow = (*this->enemies)[i];
            if (wanted != 0 && ((KIPlayer *) (intptr_t) shipNow)->field_0x42 != 0 &&
                (unsigned) (shipNow->shipGroupFlag - 0x2d) < 4) {
                Gun *gun2 = (Gun *) ::operator new(0x114);
                new(gun2) Gun(0, dmg << 2, 4, -1, 10000, 3000, (float) color,
                              Vector{0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
                gun2->setFriendGun(1);
                gun2->setLevel(this);
                gun2->weaponType = ITEM_SORT_ROCKET;
                gun2->setIndex(0x1f);
                RocketGun *r2 = (RocketGun *) ::operator new(0xe8);
                new(r2) RocketGun(gun2->itemIndex, gun2, 0x37a0, 0, 0, gun2->weaponType, false, this);
                ArrayAdd((AbstractGun *) r2, *(this->enemyGuns));
                ((KIPlayer *) (*this->enemies)[i])->addGun((Gun *) gun2, 0);
                Globals::gGlobals->addSoundResourceToList(0x54);
            }

            if ((unsigned) (camp2 - 0x9d) < 2 && ((KIPlayer *) (*this->enemies)[i])->shipGroupFlag == 0x31) {
                Gun *gun3 = (Gun *) ::operator new(0x114);
                int won3 = Status::gStatus->gameWon();
                int rampMis3 = (won3 != 0) ? 0x2d : Status::gStatus->getCurrentCampaignMission();
                new(gun3) Gun(0, dmg, 4, -1, 3000, rampMis3 * -2 + 600, (float) color,
                              Vector{0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
                gun3->setFriendGun(1);
                gun3->setLevel(this);
                gun3->weaponType = ITEM_SORT_CLUSTER_MISSILE;
                gun3->setIndex(0xd6);
                RocketGun *r3 = (RocketGun *) ::operator new(0xe8);
                new(r3) RocketGun(gun3->itemIndex, gun3, 0x37a0, 0, 0, gun3->weaponType,
                                  gun3->weaponType == ITEM_SORT_MISSILE, this);
                ArrayAdd((AbstractGun *) r3, *(this->enemyGuns));
                gun3->field_0x50 = 8.0f;
                gun3->initialLifetime = 10000;
                gun3->fireDelay = 3000;
                gun3->damage = gun3->damage << 2;
                ((KIPlayer *) (*this->enemies)[i])->addGun((Gun *) gun3, 0);
                Globals::gGlobals->addSoundResourceToList(0x54);
            }

            outIdx = outIdx + 1;
        }

    wingmanExtra:
        if (((KIPlayer *) e)->isWingMan() != 0 &&
            (char) (*this->enemies)[i]->field_0x25 != 0) {
            Gun *gun = (Gun *) ::operator new(0x114);
            new(gun) Gun(0x12, 0, 4, -1, 3000, 400, 16.0f, Vector{0.0f, 0.0f, 0.0f}, Vector{0.0f, 0.0f, 0.0f});
            gun->setFriendGun(1);
            gun->setLevel(this);
            gun->itemIndex = 0x12;
            gun->weaponType = ITEM_SORT_BLASTER;
            ObjectGun *o = (ObjectGun *) ::operator new(0xb0);
            new(o) ObjectGun(0x12, gun, 0x1a8a, 0x2711, this);
            (*this->enemyGuns)[outIdx] = o;
            gun->setIndex(0x12);
            int attr = ((Item *) ((ItemTable *) (intptr_t)(*(int *) (*g_ag_itemTblB + 4)))->itemTableEntry0x12)->
                    getAttribute(0xa);
            gun->empDamage = attr;
            ((KIPlayer *) (*this->enemies)[i])->addGun((Gun *) gun, 0);

            Globals::gGlobals->addSoundResourceToList(0x4a);
            outIdx = outIdx + 1;
        }
    }
}

#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(PlayerGasCloud) == 0x16c, "PlayerGasCloud ARM allocation size");
#endif

void Level::createGasClouds() {
    auto *status = Globals::status;
    int *prob = static_cast<int *>(static_cast<Galaxy *>(Globals::galaxy)->
        getPlasmaProbabilities(status->getStation()));
    if (status->getShip()->getFirstEquipmentOfSort(0x21) == nullptr || status->inAlienOrbit())
        return;

    SolarSystem *system = status->getSystem();
    if (system->getIndex() != 10 && *prob == 0xcc && !system->getRoutes())
        return;

    gasClouds = new Array<KIPlayer *>();
    bool boss = status->getCurrentCampaignMission() == 0x8e &&
                status->getStation()->getIndex() == 0x4f;
    auto *rng = static_cast<AERandom *>(Globals::rnd);
    float countF = (boss ? 3.0f : 0.0f) +
                   ((float) prob[1] / 100.0f) * (float) (rng->nextInt(4) + 4);
    unsigned count = countF > 0.0f ? (unsigned) countF : 0;
    ArraySetLength(count, *gasClouds);

    for (unsigned i = 0; i < gasClouds->size(); ++i) {
        Vector pos;
        do {
            do {
                pos.x = (float) (rng->nextInt(160000) - 80000);
                pos.y = (float) (rng->nextInt(160000) - 80000);
                pos.z = (float) (rng->nextInt(160000) - 80000);
            } while (AbyssEngine::AEMath::VectorLength(pos) <= 25000.0f);
            Vector fromCenter = pos - Vector{field_c8, field_cc, field_d0};
            if (AbyssEngine::AEMath::VectorLength(fromCenter) > 30000.0f)
                break;
        } while (true);
        if (boss && i == 0)
            pos = Vector{92000.0f, 0.0f, 36000.0f};

        auto *geo = static_cast<AEGeometry *>(::operator new(sizeof(AEGeometry)));
        new(geo) AEGeometry(0x37d1, Globals::Canvas, false);
        auto *cloud = static_cast<PlayerGasCloud *>(::operator new(sizeof(PlayerGasCloud)));
        new(cloud) PlayerGasCloud(*prob, field_94, geo, pos);
        (*gasClouds)[i] = cloud;
        cloud->setLevel(this);
    }
}

void Level::updateMissionOrbit(int dt) {
    if (this->field_288 != 0) {
        Status::gStatus->getMission();
        if (((Mission *) Status::gStatus->getMission())->isEmpty() == 0) {
            int t = this->orbitWaveTimer;
            this->orbitWaveTimer = t + dt;
            if (0x57e4 < t + dt) {
                this->orbitWaveTimer = 0;
                int aliveCore = 0;
                for (int j = 0; j != 4; j = j + 1)
                    aliveCore = aliveCore + (((*this->enemies)[j])->isDead() ^ 1);
                if (aliveCore != 0 && this->enemies != nullptr) {
                    for (unsigned i = 4; i < this->enemies->size(); i = i + 1) {
                        int *ki = (int *) (*this->enemies)[i];
                        if (((KIPlayer *) ki)->isDead() != 0 && ((KIPlayer *) ki)->player->isActive() == 0) {
                            AbyssEngine::AERandom *rng = AERandom::gRandom;
                            Vector p = (this->player == 0 ? Vector{0.0f, 0.0f, 0.0f} : this->player->getPosition());
                            int span = 0 ? 40000 : 120000;
                            float ox = (float) (rng->nextInt() % span - span / 2);
                            float oy = (float) (rng->nextInt() % (span * 2 / 3) - span / 3);
                            float oz = (float) (rng->nextInt() % span - span / 2);
                            ((KIPlayer *) ki)->setPosition(p.x + ox, p.y + oy, p.z + oz);
                        }
                    }
                }
            }
        }
    }

    if (Status::gStatus->getMission() != 0) {
        Status::gStatus->getMission();
        if (((Mission *) Status::gStatus->getMission())->getType() == 0xb7) {
            int t = this->orbitWaveTimer;
            this->orbitWaveTimer = t + dt;
            if (0x1d4c < t + dt) {
                this->orbitWaveTimer = 0;
                if (this->enemies != nullptr)
                    for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
                        int *ki = (int *) (*this->enemies)[i];
                        if (((KIPlayer *) ki)->isDead() != 0 && ((KIPlayer *) ki)->player->isActive() == 0 &&
                            ((KIPlayer *) ki)->shipGroupFlag != 0x33) {
                            AbyssEngine::AERandom *rng = AERandom::gRandom;
                            Vector p = (this->player == 0 ? Vector{0.0f, 0.0f, 0.0f} : this->player->getPosition());
                            int span = 1 ? 40000 : 120000;
                            float ox = (float) (rng->nextInt() % span - span / 2);
                            float oy = (float) (rng->nextInt() % (span * 2 / 3) - span / 3);
                            float oz = (float) (rng->nextInt() % span - span / 2);
                            ((KIPlayer *) ki)->setPosition(p.x + ox, p.y + oy, p.z + oz);
                            ((KIPlayer *) ki)->cargo = nullptr;
                        }
                    }
            }
        }
    }

    Status::gStatus->getMission();
    if (((Mission *) Status::gStatus->getMission())->isEmpty() == 0) {
        Status::gStatus->getMission();
        if (((Mission *) Status::gStatus->getMission())->getType() == 0xf) {
            int t = this->orbitWaveTimer;
            this->orbitWaveTimer = t + dt;
            if (50000 < t + dt && this->enemies != nullptr) {
                this->orbitWaveTimer = 0;
                unsigned count = this->enemies->size();

                bool anyAlive = false;
                for (unsigned i = 0; i + 1 < count; i = i + 1) {
                    if (((*this->enemies)[i])->isDead() == 0) {
                        anyAlive = true;
                        break;
                    }
                }
                if (anyAlive) {
                    for (unsigned i = 0; i + 1 < count; i = i + 1) {
                        int *ki = (int *) (*this->enemies)[i];
                        if (((KIPlayer *) ki)->isDead() != 0 && ((KIPlayer *) ki)->player->isActive() == 0) {
                            AbyssEngine::AERandom *rng = AERandom::gRandom;
                            Vector p = (this->player == 0 ? Vector{0.0f, 0.0f, 0.0f} : this->player->getPosition());
                            int span = 0 ? 40000 : 120000;
                            float ox = (float) (rng->nextInt() % span - span / 2);
                            float oy = (float) (rng->nextInt() % (span * 2 / 3) - span / 3);
                            float oz = (float) (rng->nextInt() % span - span / 2);
                            ((KIPlayer *) ki)->setPosition(p.x + ox, p.y + oy, p.z + oz);
                        }
                        count = this->enemies->size();
                    }
                }
            }
        }
    }
}

void Level::attackWanted(int index) {
    if (field_29c == 0) {
        field_29c = 1;
        createRadioMessage(0x10, index);
        int **slot = (int **) &Status::gStatus;

        Wanted *wanted = (Wanted *) (intptr_t)(((int *) (*(int *) ((*(int *) *slot) + 4)))[index]);
        int numWingmen = (wanted != nullptr) ? wanted->getNumWingmen() : 0;
        for (int i = 1; i - 1 < numWingmen; i = i + 1) {
            (*enemies)[i]->player->setAlwaysEnemy(1);
            (*enemies)[i]->player->turnEnemy();
        }
    }
}

void Level::initParticleSystems() {
    if (this->player != nullptr) {
        if (this->field_a4 != nullptr) {
            this->field_a8 = new Array<int>();
            ArraySetLength(this->field_a4->size(), *(this->field_a8));

            // Android Level::initParticleSystems @ 0xbd6f8 creates one
            // player-transform-attached sprite system per engine nozzle, then
            // specializes ParticleSettingsRef::cur[29 + nozzle] in place.
            ParticleSettings::SetDefinition *particleSets = levelCurrentParticleSets();
            Player *playerData = static_cast<Player *>(this->player->player);
            Ship *ship = Status::gStatus->getShip();
            unsigned int atlasVariant =
                static_cast<unsigned int>(kEngineParticleVariantByShip[ship->getIndex()]) - 1u;

            for (unsigned int i = 0; i < this->field_a4->size(); i = i + 1) {
                AEGeometry *nozzle = (*this->field_a4)[i];
                ParticleSettings::SetDefinition &settings = particleSets[kFirstEngineParticleSet + i];

                int handle = this->field_80->addSystem(
                    &playerData->transformMatrix,
                    static_cast<ParticleSettings::ParticleSet>(kFirstEngineParticleSet + i), false);
                (*this->field_a8)[i] = handle;

                Vector position = nozzle->getPosition();
                settings.posRight = position.x;
                position = nozzle->getPosition();
                settings.posUp = position.y;

                Vector scaling = nozzle->getScaling();
                float scaledSize = scaling.x * 1.5f;
                settings.lifetime = scaledSize < 1.0f
                    ? static_cast<int>(scaledSize * 80.0f)
                    : 80;

                scaling = nozzle->getScaling();
                settings.lifeBase = scaling.x * 250.0f;
                settings.count = 20;
                settings.flLifetime = 8.0f;
                settings.posBase = -1000;

                position = nozzle->getPosition();
                settings.posDir = position.z;
                scaling = nozzle->getScaling();
                scaledSize = scaling.x * 1.5f;
                settings.velDir = scaledSize < 1.0f
                    ? scaledSize * -4000.0f
                    : -4000.0f;
                settings.color0 = 0xDDDDDDFF;
                settings.color1 = 0;
                settings.colorFlag = 0.8f;

                settings.uvU0 = 0.251953125f;
                settings.uvV0 = 0.001953125f;
                settings.uvU1 = 0.373046875f;
                settings.uvV1 = 0.123046875f;
                if (atlasVariant <= 8u) {
                    settings.uvU0 = kEngineParticleUvU0[atlasVariant];
                    settings.uvV0 = kEngineParticleUvV0[atlasVariant];
                    settings.uvU1 = kEngineParticleUvU1[atlasVariant];
                    settings.uvV1 = kEngineParticleUvV1[atlasVariant];
                }
            }
        }

        PaintCanvas *canvas = PaintCanvas::gCanvas;
        canvas->CameraGetCurrent();
        Matrix *local = CameraGetLocal(canvas, canvas->CameraGetCurrent());
        int sys = (this->skybox2Mesh)->addSystem(local, ParticleSettings::ParticleSet_4, false);
        this->movingStarsIndex = sys;

        if (Status::gStatus->getSystem() != 0) {
            SolarSystem *ss = (SolarSystem *) Status::gStatus->getSystem();
            if (((SolarSystem *) ss)->hasPirateBase() != 0 && this->enemies != nullptr) {
                for (unsigned i = 0; i < this->enemies->size(); i = i + 1) {
                    KIPlayer *k = (*this->enemies)[i];
                    if (k != 0 && k->getType() == 0x37a3) {
                        AEGeometry *kg = k->geometry;
                        kg->updateReferenceMatrix();
                        Matrix const *ref = &kg->getReferenceMatrix();
                        this->particleSystemMgr->addSystem(ref, ParticleSettings::ParticleSet_8, false);
                        break;
                    }
                }
            }
        }

        canvas->CameraGetCurrent();
        local = CameraGetLocal(canvas, canvas->CameraGetCurrent());
        sys = this->particleSystemMgr->addSystem(local, ParticleSettings::ParticleSet_7, false);
        this->field_284 = sys;

        // Android Level::initParticleSystems @ 0xbda20..0xbdb46 writes the
        // system-space star particle colours into ParticleSettingsRef::cur.
        ParticleSettings::SetDefinition *particleSets = levelCurrentParticleSets();
        if (Status::gStatus->inAlienOrbit()) {
            particleSets[ParticleSettings::ParticleSet_7].color0 =
                (particleSets[ParticleSettings::ParticleSet_7].color0 & 0x000000ffu) |
                0x9274d400u;
            particleSets[ParticleSettings::ParticleSet_8].color0 =
                (particleSets[ParticleSettings::ParticleSet_8].color0 & 0x000000ffu) |
                0x9274d400u;
        } else {
            particleSets[ParticleSettings::ParticleSet_8].color0 = 0xe2282880u;
            particleSets[ParticleSettings::ParticleSet_8].color1 = 0x00ff0080u;

            int tint = static_cast<int>(kSystemParticleTintByTexture[
                Status::gStatus->getSystem()->getTextureIndex()]);
            bool supernova = Status::gStatus->inSupernovaSystem() != 0;
            float scale = supernova ? 0.5f : 0.6f;
            int red = static_cast<int>(scale * static_cast<float>((tint >> 24) & 0xff));
            int green = static_cast<int>(scale * static_cast<float>((tint >> 16) & 0xff));
            int blue = static_cast<int>(scale * static_cast<float>((tint >> 8) & 0xff));
            int alpha = supernova ? 0xff : 0xbb;
            int color = alpha | (green << 16) | (red << 24) | (blue << 8);
            particleSets[ParticleSettings::ParticleSet_7].color0 = color;
            particleSets[ParticleSettings::ParticleSet_7].color1 = color;
        }
    }

    // Android Level::initParticleSystems @ 0xbdb4a..0xbdbe0 keeps the star
    // particle velocity neutral except for active supernova campaign scenes.
    ParticleSettings::SetDefinition &starParticles =
        levelCurrentParticleSets()[ParticleSettings::ParticleSet_7];
    if (Status::gStatus->inSupernovaSystem() == 0 ||
        Status::gStatus->getCurrentCampaignMission() == 0x59 ||
        Status::gStatus->getCurrentCampaignMission() > 0x9d) {
        starParticles.velBaseX = 0.0f;
        starParticles.velBaseY = 0.0f;
        starParticles.velBaseZ = 0.0f;
    } else {
        Vector direction = -AbyssEngine::AEMath::VectorNormalize(
            this->starSystem->getLightDirection());
        starParticles.velBaseX = direction.x * 2000.0f;
        starParticles.velBaseY = direction.y * 2000.0f;
        starParticles.velBaseZ = direction.z * 2000.0f;
    }

    if (this->field_80 != 0)
        (this->field_80)->init();
    if (this->particleSystemMgr != nullptr)
        (this->particleSystemMgr)->init();
    if (this->skybox2Mesh != 0)
        (this->skybox2Mesh)->init();
    if (this->field_8c != 0)
        this->field_8c->init();
    if (this->field_98 != 0)
        (this->field_98)->init();

    // Android Level::initParticleSystems @ 0xbd6f8 constructs these sprite
    // systems on Level+0x74 with an identity transform before this manager is
    // initialized. They are not enabled at construction time.
    ParticleSystemManager *levelEffects = this->field_74;
    AbyssEngine::AEMath::Matrix field38Identity;
    this->field_38 = levelEffects->addSystem(
        &field38Identity, ParticleSettings::ParticleSet_0xa, false);
    AbyssEngine::AEMath::Matrix field3cIdentity;
    this->field_3c = levelEffects->addSystem(
        &field3cIdentity, ParticleSettings::ParticleSet_0xb, false);
    AbyssEngine::AEMath::Matrix field48Identity;
    this->field_48 = levelEffects->addSystem(
        &field48Identity, ParticleSettings::ParticleSet_0x14, false);
    AbyssEngine::AEMath::Matrix field34Identity;
    this->field_34 = levelEffects->addSystem(
        &field34Identity, ParticleSettings::ParticleSet_0x15, false);
    AbyssEngine::AEMath::Matrix field50Identity;
    this->field_50 = levelEffects->addSystem(
        &field50Identity, ParticleSettings::ParticleSet_0x16, false);
    AbyssEngine::AEMath::Matrix field54Identity;
    this->field_54 = levelEffects->addSystem(
        &field54Identity, ParticleSettings::ParticleSet_0x17, false);

    if (Status::gStatus->getCurrentCampaignMission() == 0x50) {
        // Android ARM call sites 0xbdd56 and 0xbdd82 use independent
        // identity matrices, ParticleSet_0x18 and false for both handles.
        AbyssEngine::AEMath::Matrix firstMission80Identity;
        this->field_58 = levelEffects->addSystem(
            &firstMission80Identity, ParticleSettings::ParticleSet_0x18, false);
        AbyssEngine::AEMath::Matrix secondMission80Identity;
        this->field_5c = levelEffects->addSystem(
            &secondMission80Identity, ParticleSettings::ParticleSet_0x18, false);
    }

    levelEffects->init();
    levelEffects->enableSystemEmit(this->field_50, false);
    levelEffects->enableSystemEmit(this->field_54, false);
    if (Status::gStatus->getCurrentCampaignMission() == 0x50) {
        levelEffects->enableSystemEmit(this->field_58, false);
        levelEffects->enableSystemEmit(this->field_5c, false);
    }

    this->field_9c->init();
    this->particleEmitBoolPtr->init();
    this->particleRenderBoolPtr->init();
    if (this->field_94 != 0)
        this->field_94->init();
}

void Level::createWingmen() {
    if (Status::gStatus->inSupernovaSystem() != 0 ||
        Status::gStatus->getCurrentCampaignMission() == 0x9e ||
        Status::gStatus->getWingmen() == 0 ||
        this->player == nullptr)
        return;

    Array<KIPlayer *> *arr = new Array<KIPlayer *>();
    Array<String *> *wm = Status::gStatus->getWingmen();
    ArraySetLength(wm->size(), *arr);

    unsigned n = arr->size();
    for (unsigned i = 0; i < n; i = i + 1) {
        int seed = **g_cwm_seedSrc;
        Status::gStatus->getWingmen();
        ((AbyssEngine::AERandom *) (intptr_t) seed)->setSeed((long long) seed);
        int fighter = (int) Globals::gGlobals->getRandomEnemyFighter(Status::gStatus->field_0x30);
        int ship = (int) (intptr_t) createShip(5, 0, fighter, 0, 1, 0);
        (*arr)[i] = (KIPlayer *) (intptr_t) ship;

        int *slot = (int *) &(*arr)[i];
        levelPlaceWingman(this, slot, i);

        ((*arr)[i])->setWingman(true, i);
        (*arr)[i]->player->setAlwaysFriend(1);
        (*arr)[i]->player->setHitpoints(0x258);

        (*arr)[i]->name = *(*wm)[i];
        (*arr)[i]->shipGroup = Status::gStatus->field_0x30;

        Status::gStatus->getMission();
        if (((Mission *) Status::gStatus->getMission())->getType() == 0xc)
            (*arr)[i]->field_0x25 = 0;
    }

    if (this->enemies == nullptr) {
        this->enemies = arr;
    } else {
        for (unsigned i = 0; i < n; i = i + 1) {
            ArrayAdd((*arr)[i], *(this->enemies));
            n = arr->size();
        }
        delete arr;
    }
    ((AbyssEngine::AERandom *) (intptr_t) * *g_cwm_seedSrc)->reset();
}

void Level::createScene() {
    if (this->enemies != nullptr) {
        if (this->enemies) {
            ArrayReleaseClasses(*this->enemies); delete this->enemies;
            this->enemies = nullptr;
        }
    }
    int mode = this->missionPtr;
    this->enemies = nullptr;

    if (mode == 2) {
        createPlayer();
        Status::gStatus->setMission((Mission *) Status::gStatus->wanted);
        createMission();
        if (Status::gStatus->getCurrentCampaignMission() == 0x2b) {
            void *canvas = (void *) PaintCanvas::gCanvas;
            AEGeometry *g = (AEGeometry *) ::operator new(0xc0);
            new((void *) g) AEGeometry((uint16_t) 0x37d0, (PaintCanvas *) canvas, 0);
            PlayerStatic *p = (PlayerStatic *) ::operator new(0x130);
            new(p) PlayerStatic(-1, g, 0.0f, 0.0f, 0.0f);
            ArrayAdd((KIPlayer *) p, *(this->enemies));
            g = (AEGeometry *) ::operator new(0xc0);
            new((void *) g) AEGeometry((uint16_t) 0x37d1, (PaintCanvas *) canvas, 0);
            p = (PlayerStatic *) ::operator new(0x130);
            new(p) PlayerStatic(-1, g, 0.0f, 0.0f, 0.0f);
            ArrayAdd((KIPlayer *) p, *(this->enemies));
        }
        return;
    }

    if (mode == 4) {
        Status::gStatus->getSystem();
        int race = ((SolarSystem *) Status::gStatus->getSystem())->getRace();
        unsigned crew = (race == 1) ? 2 : 3;
        Station *st = (Station *) Status::gStatus->getStation();
        int *agents = (int *) ((Station *) st)->getAgents();
        char taken[7];
        void *canvas = (void *) PaintCanvas::gCanvas;

        if (agents == 0) {
            this->enemies = new Array<KIPlayer *>();
            ArraySetLength(3, *(this->enemies));
        } else {
            int nAgents = *agents;
            this->enemies = new Array<KIPlayer *>();
            ArraySetLength(nAgents * 3 + crew, *(this->enemies));
            for (int j = 0; j != 7; j = j + 1) taken[j] = 0;

            for (int a = 0; a < nAgents; a = a + 1) {
                Agent *ag = *(Agent **) (agents[1] + a * 4);
                int part = ag->getRace();
                if (part == 3) {
                    if (ag->getImageParts() == 0) part = 3;
                    else {
                        int *ip = ag->getImageParts();
                        part = (*ip != 2) ? 3 : *ip;
                    }
                }
                int seat;
                do { seat = AERandom::gRandom->nextInt(); } while (taken[seat] != 0);
                taken[seat] = 1;

                AEGeometry *g = (AEGeometry *) ::operator new(0xc0);
                new((void *) g) AEGeometry((uint16_t)(unsigned)part, (PaintCanvas *) canvas, 0);
                PlayerStatic *p = (PlayerStatic *) ::operator new(0x130);
                new(p) PlayerStatic(-1, g, 0.0f, 0.0f, 0.0f);
                (*this->enemies)[a] = (KIPlayer *) p;

                g = (AEGeometry *) ::operator new(0xc0);
                new((void *) g) AEGeometry((uint16_t)(unsigned)mode, (PaintCanvas *) canvas, 0);
                p = (PlayerStatic *) ::operator new(0x130);
                new(p) PlayerStatic(-1, g, 0.0f, 0.0f, 0.0f);
                (*this->enemies)[nAgents + a] = (KIPlayer *) p;

                g = (AEGeometry *) ::operator new(0xc0);
                new((void *) g) AEGeometry((uint16_t) 0x380c, (PaintCanvas *) canvas, 0);
                p = (PlayerStatic *) ::operator new(0x130);
                new(p) PlayerStatic(-1, g, 0.0f, 0.0f, 0.0f);
                (*this->enemies)[nAgents * 2 + a] = (KIPlayer *) p;
            }
        }
        for (unsigned u = 0; u < crew; u = u + 1) {
            AEGeometry *g = (AEGeometry *) ::operator new(0xc0);
            new((void *) g) AEGeometry((uint16_t)(unsigned)mode, (PaintCanvas *) canvas, 0);
            PlayerStatic *p = (PlayerStatic *) ::operator new(0x130);
            new(p) PlayerStatic(-1, g, 0.0f, 0.0f, 0.0f);
            (*this->enemies)[this->enemies->size() + (u - crew)] = (KIPlayer *) p;
        }
        return;
    }

    if (mode == 0x17) {
        Station *station = Status::gStatus->getStation();
        int race = 0;
        if (station->getIndex() == 101) {
            race = 8;
        } else if (station->getIndex() == 100) {
            race = 7;
        } else {
            race = static_cast<SolarSystem *>(Status::gStatus->getSystem())->getRace();
        }

        this->enemies = new Array<KIPlayer *>();
        ArraySetLength(1, *this->enemies);

        Ship *activeShip = Status::gStatus->getShip();
        int activeShipIndex = activeShip->getIndex();
        KIPlayer *activeActor = static_cast<KIPlayer *>(
                createShip(activeShip->getRace(), 0, activeShipIndex, nullptr, false, false));
        (*this->enemies)[0] = activeActor;
        activeActor->setPosition(0.0f, static_cast<float>(kHangarShipVerticalOffsets[activeShipIndex]), 0.0f);
        static_cast<PlayerFighter *>(activeActor)->removeTrail();
        static_cast<PlayerFighter *>(activeActor)->setExhaustVisible(false);
        activeActor->setToSleep();
        activeActor->player->setAlwaysFriend(1);

        for (int layer = 0; layer < 4; ++layer) {
            int meshId = kHangarBaseMeshIds[race][layer];
            if (meshId < 0) {
                continue;
            }

            AEGeometry *geometry = new AEGeometry(static_cast<uint16_t>(meshId), PaintCanvas::gCanvas, false);
            geometry->setRotation(0.0f, 0.0f, 0.0f);
            PlayerStatic *staticObject = new PlayerStatic(-1, geometry, 0.0f, 0.0f, 0.0f);

            if (race <= 3 && race != 1) {
                for (int childIndex = 0; childIndex < kHangarVariantChildCounts[race]; ++childIndex) {
                    AEGeometry *child = new AEGeometry(
                            static_cast<uint16_t>(kHangarVariantChildStarts[race] + childIndex),
                            PaintCanvas::gCanvas, false);
                    geometry->addChild(child->transform);
                    delete child;
                }
            }
            ArrayAdd(static_cast<KIPlayer *>(staticObject), *this->enemies);
        }

        const int seatCount = kHangarSeatCounts[race];
        const bool useStationShips = station->getIndex() == 108 && Status::gStatus->field_114 == 3;
        AbyssEngine::AERandom *rng = AERandom::gRandom;
        int spawnCount = rng->nextInt(seatCount + 1);
        Array<Ship *> *stationShips = station->getShips();
        if (useStationShips) {
            spawnCount = stationShips == nullptr ? 0 : static_cast<int>(stationShips->size());
            if (spawnCount > seatCount) {
                spawnCount = seatCount;
            }
        }

        bool *occupiedSeats = new bool[seatCount]{};
        const HangarSeat *seatPositions = levelGetHangarSeats(race);
        for (int spawned = 0; spawned < spawnCount; ++spawned) {
            int shipRace = race;
            if (rng->nextInt(100) <= 29) {
                shipRace = rng->nextInt(4);
                if (rng->nextInt(100) < 30) {
                    shipRace = 8;
                }
            }

            int shipIndex = static_cast<int>(Globals::gGlobals->getRandomEnemyFighter(shipRace));
            if (station->getIndex() == 100) {
                int pick = rng->nextInt(3);
                shipIndex = pick == 1 ? 38 : (pick == 0 ? 37 : 40);
            }
            if (useStationShips) {
                shipIndex = (*stationShips)[spawned]->getIndex();
            }

            KIPlayer *actor = static_cast<KIPlayer *>(createShip(0, 0, shipIndex, nullptr, false, false));
            int seat = rng->nextInt(seatCount);
            for (int attempts = -100; attempts != 0 && occupiedSeats[seat]; ++attempts) {
                seat = rng->nextInt(seatCount);
            }
            occupiedSeats[seat] = true;

            const HangarSeat &position = seatPositions[seat];
            actor->setPosition(static_cast<float>(position.x),
                               static_cast<float>(position.y + kHangarShipVerticalOffsets[shipIndex]),
                               static_cast<float>(position.z));
            actor->player->setAlwaysFriend(1);
            actor->setToSleep();
            actor->geometry->setRotation(0.0f, static_cast<float>(rng->nextInt(300)) / 100.0f, 0.0f);
            static_cast<PlayerFighter *>(actor)->setExhaustVisible(false);
            ArrayAdd(actor, *this->enemies);
        }
        delete[] occupiedSeats;
    }
}

void Level::renderBG(int t) {
    uintptr_t canvas = (uintptr_t) PaintCanvas::gCanvas;

    ((PaintCanvas *) (long) (canvas))->SetColor(0xffffffffu);
    ((PaintCanvas *) (long) (canvas))->BeginBG();
    ((PaintCanvas *) (long) (canvas))->CameraGetCurrent();
    ((PaintCanvas *) (long) (canvas))->CameraGetLocal(((PaintCanvas *) (long) (canvas))->CameraGetCurrent());

    Matrix *sky = &this->skyMatrix;
    (*sky = AbyssEngine::AEMath::MatrixGetInverse(*sky));
    sky->e7 = 0;
    sky->e3 = 0;
    sky->e11 = 0;

    Matrix *cloudMtx = &this->cloudMatrix;
    if (Status::gStatus->inAlienOrbit() == 0 &&
        ((SolarSystem *) Status::gStatus->getSystem())->getIndex() == 0x1b) {
        Vector dir = AbyssEngine::AEMath::VectorNormalize(this->starSystem->getLightDirection());
        Vector right = AbyssEngine::AEMath::VectorNormalize(
            AbyssEngine::AEMath::VectorCross(Vector{1.0f, 0.0f, 0.0f}, dir));
        Vector up = AbyssEngine::AEMath::VectorNormalize(
            AbyssEngine::AEMath::VectorCross(right, dir));
        AbyssEngine::AEMath::MatrixSetRotation(*cloudMtx, -up, dir, right);
    } else {
        AbyssEngine::AEMath::MatrixSetRotation(*cloudMtx, this->skyRotX, this->skyRotY, this->skyRotZ);
    }
    *sky *= *cloudMtx;

    ((PaintCanvas *) (long) (canvas))->SetWorldViewMatrix(this->skyMatrix);
    ((PaintCanvas *) (long) (canvas))->SetTexture((unsigned int) (*(unsigned *) &this->field_19c), 0);
    ((PaintCanvas *) (long) (canvas))->SetBlendMode(AbyssEngine::BlendMode_dummy);
    ((PaintCanvas *) (long) (canvas))->DrawMesh((unsigned int) (*(unsigned *) &this->field_08));
    ((PaintCanvas *) (long) (canvas))->SetTexture((unsigned int) (*(unsigned *) &this->field_198), 0);
    ((PaintCanvas *) (long) (canvas))->SetBlendMode(AbyssEngine::BlendMode_2);
    ((PaintCanvas *) (long) (canvas))->DrawMesh((unsigned int) (*(unsigned *) &this->skyboxMesh));

    if (this->field_1b4 != -1) {
        ((PaintCanvas *) (long) (canvas))->SetTexture((unsigned int) (*(unsigned *) &this->field_1b8), 0);
        ((PaintCanvas *) (long) (canvas))->SetBlendMode(AbyssEngine::BlendMode_1);
        ((PaintCanvas *) (long) (canvas))->CameraGetCurrent();
        ((PaintCanvas *) (long) (canvas))->CameraGetLocal(((PaintCanvas *) (long) (canvas))->CameraGetCurrent());
        (*sky = AbyssEngine::AEMath::MatrixGetInverse(*sky));
        (*sky = *sky);
        sky->e7 = 0;
        sky->e3 = 0;
        sky->e11 = 0;
        (*sky = AbyssEngine::AEMath::MatrixGetInverse(*sky));
        ((PaintCanvas *) (long) (canvas))->
                DrawTransform((unsigned int) (long) (*(Matrix **) &this->field_1b4), nullptr);
    }

    (*(StarSystem **) &this->starSystem)->render();

    if (Status::gStatus->inSupernovaSystem() != 0 && this->skyboxTexture != -1) {
        int camp = Status::gStatus->getCurrentCampaignMission();
        ((PaintCanvas *) (long) (canvas))->SetTexture((unsigned int) (*(unsigned *) &this->field_1a0), 0);
        ((PaintCanvas *) (long) (canvas))->SetBlendMode(AbyssEngine::BlendMode_2);
        float scale = (0x6a < camp) ? 1.5f : 1.0f;
        int flag = (int) (scale * t);
        int xf = (int) (long) ((PaintCanvas *) (long) (canvas))->TransformGetTransform(0);
        ((AbyssEngine::Transform *) (intptr_t) xf)->Update((int64_t) flag, true);
        (*sky = AbyssEngine::AEMath::MatrixGetInverse(*sky));
        ((PaintCanvas *) (long) (canvas))->DrawTransform((unsigned int) (long) (*(Matrix **) &this->field_10), nullptr);
        xf = (int) (long) ((PaintCanvas *) (long) (canvas))->TransformGetTransform(0);
        ((AbyssEngine::Transform *) (intptr_t) xf)->Update((int64_t) flag, true);
        ((PaintCanvas *) (long) (canvas))->DrawTransform((unsigned int) (long) (*(Matrix **) &this->field_18), nullptr);
    }

    if (this->field_1bc != -1) {
        ((PaintCanvas *) (long) (canvas))->SetTexture((unsigned int) (*(unsigned *) &this->field_1c0), 0);
        ((PaintCanvas *) (long) (canvas))->SetBlendMode(AbyssEngine::BlendMode_2);
        ((PaintCanvas *) (long) (canvas))->CameraGetCurrent();
        ((PaintCanvas *) (long) (canvas))->CameraGetLocal(((PaintCanvas *) (long) (canvas))->CameraGetCurrent());
        (*sky = AbyssEngine::AEMath::MatrixGetInverse(*sky));
        (*sky = *sky);
        int xf = (int) (long) ((PaintCanvas *) (long) (canvas))->TransformGetTransform(0);
        int before = *(int *) (xf + 0x110);
        int xf2 = (int) (long) ((PaintCanvas *) (long) (canvas))->TransformGetTransform(0);
        ((AbyssEngine::Transform *) (intptr_t) xf2)->Update((int64_t)(int)t, true);
        if (*(int *) (xf + 0x110) < before) {
            AbyssEngine::AERandom *rng = AERandom::gRandom;
            const float toAngle = (1.0f / 65536.0f) * 6.2831855f;
            float ax = (float) rng->nextInt(0x10000) * toAngle;
            float ay = (float) rng->nextInt(0x10000) * toAngle;
            float az = (float) rng->nextInt(0x10000) * toAngle;
            AbyssEngine::AEMath::MatrixSetRotation(this->reversalMatrix, ax, ay, az);
        }
        (*sky *= this->reversalMatrix);
        sky->e3 = 0;
        sky->e7 = 0;
        sky->e11 = 0;
        (*sky = AbyssEngine::AEMath::MatrixGetInverse(*sky));
        ((PaintCanvas *) (long) (canvas))->
                DrawTransform((unsigned int) (long) (*(Matrix **) &this->field_1bc), nullptr);
    }

    if (this->supernovaFlareActive != 0 &&
        1.0f <= gEngine->explosionTimeline) {
        ((PaintCanvas *) (long) (canvas))->CameraGetCurrent();
        ((PaintCanvas *) (long) (canvas))->CameraGetLocal(((PaintCanvas *) (long) (canvas))->CameraGetCurrent());
        (*sky = AbyssEngine::AEMath::MatrixGetInverse(*sky));
        (*sky = *sky);
        sky->e7 = 0;
        sky->e3 = 0;
        sky->e11 = 0;
        ((PaintCanvas *) (long) (canvas))->SetWorldViewMatrix(this->skyMatrix);
        ((PaintCanvas *) (long) (canvas))->SetColor(0xffffffffu);
        const AbyssEngine::AEMath::Matrix *eng =
                (const AbyssEngine::AEMath::Matrix *) PaintCanvas::gCanvas->engine;
        gEngine->SetModelMatrix(*eng);
        ((PaintCanvas *) (long) (canvas))->SetTexture((unsigned int) (unsigned) this->supernovaFlareTexture, 0);
        ((PaintCanvas *) (long) (canvas))->SetBlendMode(AbyssEngine::BlendMode_8);
        ((Engine *) PaintCanvas::gCanvas->engine)->LightSetLight(0x4000);
        gEngine->GlEnable((unsigned) (uintptr_t) PaintCanvas::gCanvas->engine, 0);
        ((PaintCanvas *) (long) (canvas))->DrawMesh((unsigned int) (unsigned) this->supernovaFlareMesh);
        gEngine->GlEnable((unsigned) (uintptr_t) PaintCanvas::gCanvas->engine, 0);
        ((Engine *) PaintCanvas::gCanvas->engine)->LightEnable(false);
    }

    ((PaintCanvas *) (long) (canvas))->EndBG();
}

static inline void levelSpawnFar(Level *self, int *kiPlayer) {
    AbyssEngine::AERandom *rng = AERandom::gRandom;
    Vector p = (self->player == 0 ? Vector{0.0f, 0.0f, 0.0f} : self->player->getPosition());
    float ox = (float) (rng->nextInt() % 120000 - 60000);
    float oy = (float) (rng->nextInt() % 80000 - 40000);
    float oz = (float) (rng->nextInt() % 120000 - 60000);
    ((KIPlayer *) kiPlayer)->setPosition(p.x + ox, p.y + oy, p.z + oz);
}

static inline void levelPlaceAlien(Level *self, int *kiPlayer, int alienInOrbit) {
    AbyssEngine::AERandom *rng = AERandom::gRandom;
    Vector base;
    base.x = base.y = base.z = 0.0f;
    if (alienInOrbit)
        base = (self->player == 0 ? Vector{0.0f, 0.0f, 0.0f} : self->player->getPosition());
    float ox = (float) (rng->nextInt() % 100000 - 50000);
    float oy = (float) (rng->nextInt() % 60000 - 30000);
    float oz = (float) (rng->nextInt() % 100000 - 50000);
    ((KIPlayer *) kiPlayer)->setPosition(base.x + ox, base.y + oy, base.z + oz);
}

static inline void levelPlaceWingman(Level *self, int *kiSlot, unsigned i) {
    if (kiSlot == 0)
        return;
    Vector p = (self->player == 0 ? Vector{0.0f, 0.0f, 0.0f} : self->player->getPosition());

    float side = ((i & 1) ? -1.0f : 1.0f) * (float) (2000 + (int) (i / 2) * 1500);
    float back = (float) (2000 + (int) (i / 2) * 2500);
    ((KIPlayer *) kiSlot)->setPosition(p.x + side, p.y, p.z - back);
}

// Static data members present in the original binary (defined for symbol parity).
unsigned char Level::doInstantJump;
void *Level::programmedStation;
unsigned char Level::comingFromAlienWorld;
unsigned char Level::initStreamOutPosition;
int Level::energyCellsForNextJump;
int Level::lastMissionFreighterHitpoints;
unsigned char Level::initStreamOutPositionAfterCutscene;
float Level::i_a;
float Level::i_b;
float Level::i_g;
float Level::i_r;
int Level::b_min;
int Level::g_min;
int Level::r_min;
