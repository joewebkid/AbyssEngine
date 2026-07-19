#ifndef GOF2_LEVEL_H
#define GOF2_LEVEL_H
#include "game/world/StarSystem.h"
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/world/Route.h"
#include "game/world/Waypoint.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerFixedObject.h"
#include "game/mission/Mission.h"
#include "game/mission/Objective.h"
#include "game/weapons/Gun.h"
#include "game/weapons/ObjectGun.h"
#include "engine/render/ParticleSystemManager.h"
#include "engine/render/AEGeometry.h"
#include "engine/render/LODManager.h"
#include "engine/render/LodMeshMerger.h"
#include "engine/math/BoundingVolume.h"

#include "engine/math/Vector.h"
#include "engine/math/Matrix.h"



#include "game/world/RadioStageEntry.h"
class AEGeometry;
class BoundingVolume;
class Gun;
class KIPlayer;
class LODManager;
class LodMeshMerger;
class ObjectGun;
class AbstractGun;
class Objective;
class ParticleSystemManager;
class PlayerEgo;
class PlayerFixedObject;
class RadioMessage;
class Route;
class StarSystem;
class Waypoint;


class Level {
public:
    LODManager *lodManager;
    int skyboxMesh;
    int field_08;
    int skyboxTexture;
    int field_10;
    int field_14;
    int field_18;
    int field_1c;
    int killCountA;
    int killCountB;
    Objective *objectivesA;
    Objective *objectivesB;
    int field_30;
    int field_34;
    int field_38;
    int field_3c;
    int field_40;
    int field_44;
    int field_48;
    int field_4c;
    int field_50;
    int field_54;
    int field_58;
    int field_5c;
    int field_60;
    int movingStarsIndex;
    uint8_t field_68;
    uint8_t field_69;
    uint8_t pad_6a[2];
    int field_6c;
    int field_70;
    ParticleSystemManager *field_74;
    ParticleSystemManager *particleEmitBoolPtr;
    ParticleSystemManager *particleSystemMgr;
    ParticleSystemManager *field_80;
    ParticleSystemManager *particleRenderBoolPtr;
    ParticleSystemManager *skybox2Mesh;
    ParticleSystemManager *field_8c;
    ParticleSystemManager *field_90;
    ParticleSystemManager *field_94;
    ParticleSystemManager *field_98;
    ParticleSystemManager *field_9c;
    LodMeshMerger *field_a0;
    Array<AEGeometry *> *field_a4;
    Array<int> *field_a8;
    int miningPlantIndex;
    Array<KIPlayer *> *field_b0;
    int field_b4;
    int field_b8;
    int field_bc;
    int missionPtr;
    BoundingVolume *collisionVolume;
    float field_c8;
    float field_cc;
    float field_d0;
    int field_d4;
    Waypoint *asteroidWaypoint;
    int field_dc;
    int field_e0;
    Array<AbstractGun *> *playerGuns;
    Array<AbstractGun *> *enemyGuns;
    StarSystem *starSystem;
    PlayerEgo *player;
    Array<KIPlayer *> *gasClouds;
    Array<KIPlayer *> *enemies;
    Array<KIPlayer *> *asteroids;
    Array<KIPlayer *> *landmarks;
    Array<AEGeometry *> *field_104;
    Route *playerRoute;
    Route *friendRoute;
    Route *enemyRoute;
    Array<RadioMessage *> *messages;
    int enemiesLeft;
    int friendsLeft;
    int field_120;
    int field_124;
    int asteroidsLeft;
    int kills;
    int timeLimit;
    int field_134;
    int field_138;
    uint8_t friendCargoStolen;
    uint8_t pad_13d[3];
    Vector flashColor;
    int flashField;
    int flashDurationA;
    int flashDurationB;
    uint8_t flashActive;
    uint8_t pad_159[3];
    int flashType;
    int friendCount;
    int field_164;
    int field_168;
    int hostileCount;
    int alienAttackTimer;
    int orbitWaveTimer;
    int field_178;
    int field_17c;
    Route *field_180;
    int field_184;
    uint8_t field_188;
    uint8_t alarmRequested;
    uint8_t field_18a;
    uint8_t pad_18b;
    int field_18c;
    int field_190;
    int field_194;
    int field_198;
    int field_19c;
    int field_1a0;
    float skyRotX;
    float skyRotY;
    float skyRotZ;
    uint8_t field_1b0;
    uint8_t pad_1b1[3];
    int field_1b4;
    int field_1b8;
    int field_1bc;
    int field_1c0;
    int supernovaFlareTexture;
    int field_1c8;
    int supernovaFlareMesh;
    AbyssEngine::AEMath::Matrix skyMatrix;
    AbyssEngine::AEMath::Matrix cloudMatrix;
    AbyssEngine::AEMath::Matrix reversalMatrix;
    int field_284;
    uint8_t field_288;
    uint8_t supernovaFlareActive;
    uint8_t pad_28a[2];
    int miningPlant;
    int numDeliveredOre;
    int numDeliveredPassengers;
    int field_298;
    uint8_t field_29c;
    uint8_t field_29d;
    uint8_t field_29e;
    uint8_t pad_29f;

    Level(int mission);

    ~Level();

    static void setInitStreamOut();

    static float r;
    static float g;
    static float b;

    int init();

    void createSpace();

    void createPlayer();

    void createAsteroids();

    void createGasClouds();

    void createMission();

    void createScene();

    void createCampaignMission();

    void createStaticObjects();

    void createSentryGuns();

    void createFighterTurrets();

    void createWingmen();

    void assignGuns();

    void connectPlayers();

    void enableParticleEffects(bool emit, bool render);

    void setPlayerEngineColor(short color);

    void initParticleSystems();

    StarSystem *getStarSystem();

    Array<KIPlayer *> *getGasClouds();

    Gun *createGun(int idx, int owner, int kind, int hp, int dmg, int rate, int cool, int color);

    int createStaticObject(Waypoint *wp, int type, bool jitter);

    PlayerFixedObject *createShip(int race, int shipClass, int type, Waypoint *wp, bool hostile, bool group);

    Route *createRoute(int count);

    void setPlayerRoute(Route *route);

    void createRadioMessages(int set);

    void *getBoundingVolume(int param, AEGeometry *kind);

    PlayerEgo *getPlayer();

    Array<KIPlayer *> *getEnemies();

    Array<KIPlayer *> *getLandmarks();

    Array<KIPlayer *> *getAsteroids();

    Waypoint *getAsteroidWaypoint();

    Route *getPlayerRoute();

    Route *getEnemyRoute();

    Route *getFriendRoute();

    void flashScreen(int type);

    void enemyDied(int r1, bool r2arg);

    void junkDied();

    void applyKills();

    void friendDied();

    void wingmanDied(AbyssEngine::String const &name);

    void asteroidDied();

    int getAsteroidsLeft();

    int getEnemiesLeft();

    int getFriendsLeft();

    Array<void *> *getMessages();

    int getTimeLimit();

    int collide(Vector v, bool param);

    int isInAsteroidCenterRange(Vector v);

    int collideStream(Vector v);

    int collideStation(Vector v);

    void renderBG(int t);

    void render(int ctx);

    void render2D();

    void renderPause(long long ctx);

    void updateMissionOrbit(int dt);

    void updateOrbit(int dt);

    void alarmAllFriends(int race, bool message);

    void createRadioMessage(int type, int param);

    void updateAlienAttackers(int dt);

    void updateAsteroidCluster();

    void update(long long time, bool param);

    int checkObjective(int param);

    void stealFriendCargo();

    uint8_t friendCargoWasStolen();

    void removeObjectives();

    Array<AbstractGun *> *getPlayerGuns();

    Array<AbstractGun *> *getEnemyGuns();

    int checkGameOver(int param);

    void pirateStationAction(bool param);

    void uncoverWanted(int index);

    void attackWanted(int index);

    void almostKillWanted(int index);

    void killWanted(int param);

    void friendTurnedEnemy(int param);

    void enableFog(bool enable);

    void enableMovingStars(bool enable);

    void reset();

    void switchSkyboxForIntro();

    void switchSkyboxForSupernovaReversal();

    int getNumDockingTargets();

    int getDockingTarget(int index);

    bool hasMiningPlant();

    int getMiningPlant();

    int getNumDeliveredOre();

    void incNumDeliveredOre(int delta);

    int getNumDeliveredPassengers();

    void incNumDeliveredPassengers(int delta);

    void crm_dispatch(int egoComm, void *queue);

    // Static data members present in the original binary (defined for symbol parity).
    static unsigned char doInstantJump;
    static void *programmedStation;
    static unsigned char comingFromAlienWorld;
    static unsigned char initStreamOutPosition;
    static int energyCellsForNextJump;
    static int lastMissionFreighterHitpoints;
    static unsigned char initStreamOutPositionAfterCutscene;
    static float i_a;
    static float i_b;
    static float i_g;
    static float i_r;
    static float b_min;
    static float g_min;
    static float r_min;
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(Level, field_188) == 0x188, "Level::field_188 offset");
static_assert(__builtin_offsetof(Level, alarmRequested) == 0x189, "Level::alarmRequested offset");
static_assert(__builtin_offsetof(Level, field_18a) == 0x18a, "Level::field_18a offset");
static_assert(__builtin_offsetof(Level, skyMatrix) == 0x1d0, "Level::skyMatrix offset");
static_assert(__builtin_offsetof(Level, cloudMatrix) == 0x20c, "Level::cloudMatrix offset");
static_assert(__builtin_offsetof(Level, reversalMatrix) == 0x248, "Level::reversalMatrix offset");
static_assert(__builtin_offsetof(Level, field_284) == 0x284, "Level::field_284 offset");
#endif


#endif
