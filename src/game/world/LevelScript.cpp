#include "game/world/LevelScript.h"
#include "game/core/RadioMessage.h"
#include "game/ship/Player.h"
#include "game/mission/Explosion.h"
#include "game/ship/KIPlayer.h"
#include "game/ui/Hud.h"
#include "game/world/StarSystem.h"
#include "game/ship/TargetFollowCamera.h"
#include "engine/render/AEGeometry.h"
#include "game/world/Level.h"
#include "game/mission/Status.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerFighter.h"
#include "game/world/Route.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"

static Station *gProgrammedStation = nullptr;

struct StackVector {
    float v[3];

    StackVector() {
    }

    StackVector(float x, float y, float z) {
        v[0] = x;
        v[1] = y;
        v[2] = z;
    }

    operator Vector *() { return reinterpret_cast<Vector *>(v); }
};

typedef void (*RenderProc)(void *);

typedef void * (*LevelListProc)(Level *);

typedef void (*ReadWaypointProc)(StackVector *out, void *waypoint);

typedef void (*SetVectorProc)(void *self, StackVector *value);

typedef void (*VirtualCommandProc)(void *self, int a, int b, int c);

static volatile RenderProc gRenderProc;
static volatile LevelListProc gLevelListProc;

static inline void *arrayData(void *array) {
    return *reinterpret_cast<void **>(static_cast<char *>(array) + 4);
}

static inline void *firstListEntry(void *list) {
    return static_cast<void **>(arrayData(list))[0];
}

void LevelScript::render3D() {
    if (m_pGeometry6) m_pGeometry6->render();
    if (m_pGeometry5) m_pGeometry5->render();
    if (m_pGeometry0) m_pGeometry0->render();
    if (m_pGeometry1) m_pGeometry1->render();
    if (m_pGeometry2) m_pGeometry2->render();
    if (m_pGeometry3) m_pGeometry3->render();

    if (m_pExplosion &&m_pExplosion


    ->
    isPlaying()
    )
    {
        m_pExplosion->render();
    }

    if (m_bRenderParticles && m_pParticleGeom0) {
        RenderProc render = gRenderProc;
        render(m_pParticleGeom0);
        render(m_pParticleGeom1);
        render(m_pParticleGeom2);
    }
}

uint8_t LevelScript::startSequenceOver() {
    return m_bStartSequenceOver;
}

void LevelScript::resetCamera(Level *level) {
    if (level->getPlayer() != 0) {
        PlayerEgo *player = (PlayerEgo *) level->getPlayer();
        m_pCamera->setTarget(player->geometry);
        m_pCamera->setTargetOffset(Vector{0.0f, 600.0f, -650.0f});
        m_pCamera->setCamOffset(Vector{0.0f, 600.0f, -1338.0f});
    }
}

void LevelScript::skipSequence() {
    if (field_0x24 > 0 && Status::gStatus->getCurrentCampaignMission() > 0) {
        field_0x24 = 0x1b59;
        void *player = (void *) m_pLevel->getPlayer();
        (void) player; /* virtual Player flag setter — resolved via vtable */
    }
}

uint8_t LevelScript::startSequence() {
    return (uint8_t) m_bStartSequence;
}

void LevelScript::setAutoPilotToProgrammedStation() {
    if (gProgrammedStation == 0) {
        return;
    }

    Station *programmed = gProgrammedStation;
    if (Status::gStatus->getStation()->equals(programmed)) {
        gProgrammedStation = 0;
        return;
    }

    SolarSystem *system = (SolarSystem *) Status::gStatus->getSystem();
    void *player;
    void *target;

    if (system->stationIsInSystem(programmed)) {
        player = (void *) m_pLevel->getPlayer();
        void *targets = ((StarSystem *) m_pLevel->getStarSystem())->getPlanetTargets();
        int targetIndex = system->getStationEnumIndex(programmed->getIndex());
        target = static_cast<void **>(arrayData(targets))[targetIndex];
    } else if (system->currentOrbitHasWarpGate()) {
        player = (void *) m_pLevel->getPlayer();
        Array<KIPlayer *> *landmarks = m_pLevel->getLandmarks();
        target = (void *) (*landmarks)[1];
    } else {
        int warpGateIndex = system->getWarpGateEnumIndex();
        if (warpGateIndex < 0) {
            return;
        }
        player = (void *) m_pLevel->getPlayer();
        void *targets = ((StarSystem *) m_pLevel->getStarSystem())->getPlanetTargets();
        target = static_cast<void **>(arrayData(targets))[warpGateIndex];
    }

    (void) player;
    (void) target; /* virtual Player autopilot-target setter — resolved via vtable */
}

LevelScript::~LevelScript() {
    delete m_pGeometry6;
    m_pGeometry6 = 0;
    delete m_pGeometry5;
    m_pGeometry5 = 0;
    delete m_pGeometry0;
    m_pGeometry0 = 0;
    delete m_pGeometry1;
    m_pGeometry1 = 0;
    delete m_pGeometry2;
    m_pGeometry2 = 0;
    delete m_pGeometry3;
    m_pGeometry3 = 0;
    delete m_pParticleGeom0;
    m_pParticleGeom0 = 0;
    delete m_pParticleGeom1;
    m_pParticleGeom1 = 0;
    delete m_pParticleGeom2;
    m_pParticleGeom2 = 0;
    delete m_pExplosion;
    m_pExplosion = 0;
    delete m_pGeometry4;
    m_pGeometry4 = 0;
}

uint32_t LevelScript::canSkipCutsceneNow() {
    int mission = Status::gStatus->getCurrentCampaignMission();
    if (mission == 0x9a) {
        if (!((uint32_t)(m_nState - 1) < 9)) {
            return 0;
        }
    } else if (mission == 0x9d) {
        if (!((uint32_t)(m_nState - 2) < 3)) {
            return 0;
        }
    } else if (mission == 0x9e && m_nState > 1) {
        return 0;
    }
    return 1;
}

void LevelScript::resetStartSequenceOver() {
    field_0x24 = 0;
    m_bStartSequenceOver = 0;
}

void LevelScript::skipCutscene() {
    StackVector position;

    int mission = Status::gStatus->getCurrentCampaignMission();
    if (mission == 0x9a) {
        if ((uint32_t)(m_nState - 1) < 9) {
            m_nState = 9;
            for (int i = 0; i != 8; ++i) {
                Array<void *> *messages = m_pLevel->getMessages();
                ((RadioMessage *) ((*messages)[i]))->trigger();
                messages = m_pLevel->getMessages();
                ((RadioMessage *) ((*messages)[i]))->finish();
            }

            m_nScriptTimerA = 0x7d1;
            m_nScriptCounterA = 0;

            LevelListProc getList = gLevelListProc;
            void *list = getList(m_pLevel);
            ((PlayerFighter *) firstListEntry(list))->setAIDisabled(false);

            list = getList(m_pLevel);
            void *fighter = firstListEntry(list);

            list = getList(m_pLevel);
            Route *route = ((KIPlayer *) firstListEntry(list))->getRoute();
            void *waypoint = route->getWaypoint();
            ReadWaypointProc readWaypoint =
                    *(ReadWaypointProc *) (*reinterpret_cast<char **>(waypoint) + 0x28);
            readWaypoint(&position, waypoint);
            SetVectorProc setVector =
                    *(SetVectorProc *) (*reinterpret_cast<char **>(fighter) + 0x44);
            setVector(fighter, &position);
        }
    } else if (mission == 0x9d) {
        if (m_nState <= 4) {
            for (int i = 0; i != 4; ++i) {
                Array<void *> *messages = m_pLevel->getMessages();
                ((RadioMessage *) ((*messages)[i]))->trigger();
                messages = m_pLevel->getMessages();
                ((RadioMessage *) ((*messages)[i]))->finish();
            }
            m_nScriptTimerA = 0x4651;
            m_nScriptCounterA = 0;
            m_nState = 4;
        }
    } else if (mission == 0x9e) {
        m_nScriptTimerB = 0x2ee1;
        m_nScriptCounterB = 0;
        for (int i = 0; i != 3; ++i) {
            Array<void *> *messages = m_pLevel->getMessages();
            ((RadioMessage *) ((*messages)[i]))->trigger();
            messages = m_pLevel->getMessages();
            ((RadioMessage *) ((*messages)[i]))->finish();
        }

        LevelListProc getList = gLevelListProc;
        void *list = getList(m_pLevel);
        void *player = firstListEntry(list);
        VirtualCommandProc command =
                *(VirtualCommandProc *) (*reinterpret_cast<char **>(player) + 0x48);
        command(player, 0x000ba51e, 0, 0x000ba4b6);

        list = getList(m_pLevel);
        ((KIPlayer *) firstListEntry(list))->setVisible(true);

        list = getList(m_pLevel);
        m_nState = 2;
    }
}

void LevelScript::process(int delta) {
    Level *level = m_pLevel;
    level->getMessages();
    level->getPlayer();
    level->getMessages();
    int mission = Status::gStatus->getCurrentCampaignMission();

    if (mission != 0) {
        float frameDelta = (float) delta * 0.001f;
        m_pCamera->update((int) frameDelta);
    }
}

void LevelScript::lookBehind() {
    m_pCamera->setTargetOffset(Vector{0.0f, 0.0f, -950.0f});
    m_pCamera->setCamOffset(Vector{0.0f, 600.0f, 2230.0f});
}

LevelScript::LevelScript(Level *level, Hud *hud, Radar *radar, TargetFollowCamera *camera) {
    field_0x48 = 0;
    field_0x38 = 0;
    field_0x3c = 0;
    field_0x40 = 0;
    field_0x44 = 0;
    field_0x28 = 0;
    field_0x2c = 0;
    field_0x30 = 0;
    field_0x34 = 0;

    m_matrix = AbyssEngine::AEMath::Matrix();

    m_pHud = hud;
    m_pRadar = radar;
    m_pCamera = camera;
    m_pLevel = level;
    hud->drawTitleImage(false);

    m_nState = 0;
    m_bStartSequence = 1;
    m_nFlags = 0x100;

    field_0x8c = 0;
    m_nScriptTimerA = 0;
    m_nScriptCounterA = 0;
    m_nScriptTimerB = 0;
    m_pParticleGeom0 = 0;
    m_pParticleGeom1 = 0;
    m_pParticleGeom2 = 0;
    m_pGeometry0 = 0;
    m_pGeometry1 = 0;
    m_pGeometry2 = 0;
    m_pGeometry3 = 0;
    m_pExplosion = 0;
    m_pGeometry4 = 0;
    field_0x8 = 0;
    field_0xc = 0;
    m_nTimeLimit = level->getTimeLimit();
    field_0x24 = 0;
    m_pGeometry5 = 0;
    m_pGeometry6 = 0;
    field_0x12 = 0;

    camera->setLookAtCam(true);

    void *player = (void *) level->getPlayer();
    if (player == 0) {
        m_bStartSequence = 0;
    } else {
        ((Player *) player)->setVulnerable(false);
    }

    player = (void *) level->getPlayer();
    ((PlayerEgo *) player)->setCollide(false);

    if (Status::gStatus->getCurrentCampaignMission() == 0) {
        m_nScriptTimerA = 0;
        m_nScriptCounterA = 0;
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
    }
}

int LevelScript::getEvent() {
    return m_nState;
}

void LevelScript::setEvent(int event) {
    m_nState = event;
}
