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
#include "game/ship/PlayerFixedObject.h"
#include "game/ship/PlayerStation.h"
#include "game/ship/PlayerGasCloud.h"
#include "game/ship/Ship.h"
#include "game/world/Route.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"
#include "game/core/Globals.h"
#include "game/weapons/Radar.h"
#include "engine/audio/FModSound.h"
#include "engine/core/AERandom.h"
#include "engine/core/GameText.h"
#include "game/ship/PlayerWormHole.h"
#include "engine/math/Transform.h"
#include "game/world/Galaxy.h"
#include "game/world/Standing.h"

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
    // Android 0x137b68: this helper consumes the live Level static, not a shim.
    if (Level::programmedStation == 0) {
        return;
    }
    if (Globals::status->getStation()->equals(static_cast<Station *>(Level::programmedStation))) {
        Level::programmedStation = 0;
        return;
    }
    if (Globals::status->getSystem()->stationIsInSystem(static_cast<Station *>(Level::programmedStation))) {
        PlayerEgo *player = m_pLevel->getPlayer();
        auto *targets = static_cast<Array<KIPlayer *> *>(static_cast<StarSystem *>(m_pLevel->getStarSystem())->getPlanetTargets());
        SolarSystem *system = Globals::status->getSystem();
        int index = static_cast<Station *>(Level::programmedStation)->getIndex();
        player->setAutoPilot((*targets)[system->getStationEnumIndex(index)]);
    } else if (Globals::status->getSystem()->currentOrbitHasWarpGate()) {
        PlayerEgo *player = m_pLevel->getPlayer();
        player->setAutoPilot((*m_pLevel->getLandmarks())[1]);
    } else {
        int index = Globals::status->getSystem()->getWarpGateEnumIndex();
        if (index >= 0) {
            PlayerEgo *player = m_pLevel->getPlayer();
            auto *targets = static_cast<Array<KIPlayer *> *>(static_cast<StarSystem *>(m_pLevel->getStarSystem())->getPlanetTargets());
            player->setAutoPilot((*targets)[index]);
        }
    }
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

bool LevelScript::process(int delta) {
    // Android 0x137d3a..0x137d5e; iOS 0x1340ce..0x1340fe.
    Array<void *> *messages = m_pLevel->getMessages();
    PlayerEgo *player = m_pLevel->getPlayer();
    Array<KIPlayer *> *enemies = m_pLevel->getEnemies();
    if (Globals::status->getCurrentCampaignMission() == 0) {
        // Android 0x137e84 onward: intro states, message indexes and camera/effect transitions.
        bool messageOver = static_cast<RadioMessage *>((*messages)[2])->isOver();
        int state = m_nState;
        if (messageOver && state == 0) {
            m_nState = 1;
            player->setPosition(18000.0f, -12000.0f, -40000.0f);
            m_pCamera->setPosition(-12000.0f, 2000.0f, -500.0f);
            PaintCanvas *canvas = Globals::Canvas;
            Matrix *local = static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
            AbyssEngine::AEMath::MatrixSetTranslation(*local, -12000.0f, 2000.0f, -500.0f);
            (*enemies)[0]->setPosition(-10000.0f, 500.0f, 0.0f);
            (*enemies)[1]->setPosition(-10000.0f, -300.0f, -1700.0f);
            (*enemies)[2]->setPosition(-10000.0f, -200.0f, 2000.0f);
            for (unsigned i = 0; i != 3; ++i) {
                (*enemies)[i]->geometry->setDirection(Vector{1.0f, 0.0f, 0.0f}, Vector{0.0f, 1.0f, 0.0f});
                (*enemies)[i]->geometry->setVisible(true);
            }
            m_pLevel->lodManager->forceUpdate(30, false);
            goto commonDeparture;
        }
        if (state <= 2) {
            float bob = Globals::layout->getPulseValue(0.0005f) + -0.5f;
            for (unsigned i = 0; i != 3; ++i)
                (*enemies)[i]->geometry->translate(0.0f, bob, 0.0f);
            if (m_nState == 1 && static_cast<RadioMessage *>((*messages)[6])->isOver()) {
                m_nState = 2;
                m_pCamera->setTarget((*enemies)[0]->geometry);
                m_pCamera->setPosition(-5000.0f, 300.0f, -5000.0f);
            }
            if (m_nState == 2) {
                if (!Globals::sound->isPlaying(142)) {
                    Globals::sound->stop(Globals::sound->currentMusicEvent);
                    Globals::sound->play(142, nullptr, nullptr, 0.0f);
                }
                m_pCamera->translate((float)delta * 0.2f, 0.0f, (float)delta * 2.2f);
                if (static_cast<RadioMessage *>((*messages)[7])->isOver()) {
                    m_nState = 3;
                    for (unsigned i = 0; i != 3; ++i) {
                        static_cast<PlayerFighter *>((*enemies)[i])->setExhaustVisible(true);
                        (*enemies)[i]->geometry->setVisible(true);
                        (*enemies)[i]->awake(); // Native KIPlayer vtable +0x0c.
                        *reinterpret_cast<int *>(reinterpret_cast<char *>((*enemies)[i]) + 0x128) = 50000;
                        (*enemies)[i]->player->enemyFlagsLo = 1;
                    }
                }
            }
            goto commonDeparture;
        }
        switch (state) {
        case 3:
            m_pCamera->translate((float)delta * 0.2f, 0.0f, (float)delta * 2.2f);
            if (static_cast<RadioMessage *>((*messages)[8])->isOver()) {
                scriptTimerA = 0;
                m_nState = 4;
                player->setComputerControlled(false);
                static_cast<Player *>(player->player)->field_5e = 0;
                m_pRadar->enabled = 1;
                m_pLevel->getPlayer()->setCollide(true);
                cameraControlled = 0;
                m_pCamera->setLookAtCam(false);
                m_pCamera->setTarget(player->geometry);
                m_bStartSequence = 0;
                field_0x12 = (field_0x12 & 0xff00) | 1;
            }
            goto commonDeparture;
        case 4:
            if (static_cast<RadioMessage *>((*messages)[10])->isOver()) {
                Globals::sound->stop(Globals::sound->currentMusicEvent);
                Globals::sound->play(143, nullptr, nullptr, 0.0f);
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                cameraControlled = 1;
                m_pLevel->getPlayer()->setCollide(false);
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                static_cast<Player *>(player->player)->removeAllGuns();
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget(player->geometry);
                player->geometry->setRotation(0.0f, 1.57079637050628662109375f, 0.0f);
                m_pCamera->setPosition(player->geometry->getPosition());
                m_pCamera->translate(25000.0f, -200.0f, -1000.0f);
                m_nState = 5;
                goto commonDeparture;
            }
            state = m_nState;
            break;
        case 5:
            if (static_cast<RadioMessage *>((*messages)[12])->isOver()) {
                Vector position = player->getPosition();
                if (player->getSpeed() == 2.0f) {
                    Globals::sound->play(157, nullptr, nullptr, 0.0f);
                    Vector velocity{0.0f, 0.0f, 0.0f};
                    Globals::sound->updateEvent3DAttributes(157, &position, &velocity, false);
                    Globals::sound->play(158, nullptr, nullptr, 0.0f);
                    Vector otherVelocity{0.0f, 0.0f, 0.0f};
                    Globals::sound->updateEvent3DAttributes(158, &position, &otherVelocity, false);
                    Globals::sound->stop(player->field_0x1c);
                    Globals::sound->play(161, nullptr, nullptr, 0.0f);
                }
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(161, &position, &velocity, false);
                player->setSpeed(player->getSpeed() * 0.98f);
                if (static_cast<RadioMessage *>((*messages)[13])->isOver())
                    m_pCamera->translate((float)((double)delta * 0.3), 0.0f, (float)-delta * 0.2f);
                if (static_cast<RadioMessage *>((*messages)[14])->isTriggered()) {
                    scriptTimerA = 0;
                    m_nState = 6;
                }
                goto commonDeparture;
            }
            state = m_nState;
            break;
        }
        switch (state) {
        case 6: {
            Vector velocity{0.0f, 0.0f, 0.0f};
            Vector position = player->getPosition();
            Globals::sound->updateEvent3DAttributes(161, &position, &velocity, false);
            m_pCamera->translate((float)((double)delta * 0.3), 0.0f, (float)-delta * 0.2f);
            long long elapsed = scriptTimerA + delta;
            float rumble = 1.0f;
            if ((float)elapsed / 4000.0f < 1.0f)
                rumble = (float)elapsed / 4000.0f;
            scriptTimerA = elapsed;
            m_pCamera->setRumblePercentage(rumble, 20);
            if (static_cast<RadioMessage *>((*messages)[15])->isOver()) {
                m_nState = 7;
                m_pGeometry5->setPosition(player->getPosition());
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                Globals::sound->play(160, nullptr, nullptr, 0.0f);
                Globals::sound->stop(161);
                scriptTimerA = 0;
            }
            break;
        }
        case 7: {
            scriptTimerA += delta;
            m_pCamera->translate((float)((double)delta * 0.3), 0.0f, (float)-delta * 0.2f);
            PaintCanvas *canvas = Globals::Canvas;
            Matrix *local = static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
            m_pGeometry5->setDirection(AbyssEngine::AEMath::MatrixGetDir(*local), Vector{0.0f, 1.0f, 0.0f});
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                ->Update(delta, false);
            if (scriptTimerA >= 2001) {
                Globals::sound->stop(158);
                player->setVisible(false);
                m_pCamera->setRumblePercentage(0.0f, 0);
            }
            if (!static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))->animating) {
                Globals::sound->stop(Globals::sound->currentMusicEvent);
                Globals::sound->play(141, nullptr, nullptr, 0.0f);
                m_nState = 8;
                scriptTimerA = 0;
            }
            break;
        }
        case 8: {
            long long elapsed = scriptTimerA + delta;
            float rumble = (float)elapsed / -3000.0f + 1.0f;
            scriptTimerA = elapsed;
            m_pCamera->setRumblePercentage(rumble > 0.0f ? rumble : 0.0f, 20);
            if (scriptTimerA >= 4001) {
                m_nState = 9;
                scriptTimerA = 0;
                m_pCamera->setRumblePercentage(0.0f, 0);
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
            }
            break;
        }
        case 9:
            m_nState = 10;
            static_cast<StarSystem *>(m_pLevel->getStarSystem())->switchPlanetForIntro();
            m_pLevel->switchSkyboxForIntro();
            player->setPosition(0.0f, 0.0f, 0.0f);
            m_pLevel->particleRenderBoolPtr->reset();
            m_pLevel->particleEmitBoolPtr->reset();
            player->geometry->setDirection(Vector{0.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
            player->rotate(0.785398185253143310546875f, 0.785398185253143310546875f, 0.785398185253143310546875f);
            m_pGeometry5->setPosition(player->getPosition());
            m_pCamera->setPosition(5000.0f, 500.0f, -10000.0f);
            scriptTimerA = 0;
            break;
        case 10: {
            long long elapsed = scriptTimerA + delta;
            float rumble = 1.0f;
            if ((float)elapsed / 2000.0f < 1.0f)
                rumble = (float)elapsed / 2000.0f;
            scriptTimerA = elapsed;
            m_pCamera->setRumblePercentage(rumble, 20);
            if (scriptTimerA >= 2001) {
                Globals::sound->play(159, nullptr, nullptr, 0.0f);
                m_nState = 11;
                m_pGeometry5->setPosition(player->getPosition());
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                scriptTimerA = 0;
            }
            break;
        }
        case 11: {
            scriptTimerA += delta;
            float rumble = (float)scriptTimerA / -2500.0f + 1.0f;
            m_pCamera->setRumblePercentage(rumble > 0.0f ? rumble : 0.0f, 20);
            PaintCanvas *canvas = Globals::Canvas;
            Matrix *local = static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
            m_pGeometry5->setDirection(AbyssEngine::AEMath::MatrixGetDir(*local), Vector{0.0f, 1.0f, 0.0f});
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                ->Update(delta, false);
            if (scriptTimerA >= 2501) {
                m_nState = 12;
                player->StopEngineSound();
                player->field_0x1c = 156;
                player->PlayEngineSound();
                player->startSmokeEmission();
                player->setVisible(true);
                player->setExhaustVisible(false);
                player->setSpeed(2.0f);
            }
            break;
        }
        case 12:
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                ->Update(delta, false);
            player->rotate((float)delta / 3000.0f, (float)delta / 3000.0f, (float)delta / 3000.0f);
            if (static_cast<RadioMessage *>((*messages)[17])->isOver()) {
                vector_0x28 = player->getPosition();
                vector_0x28 += Vector{-2000.0f, -2000.0f, -5000.0f};
                m_pCamera->setPosition(vector_0x28);
                m_nState = 13;
                scriptTimerA = 0;
            }
            break;
        case 13:
            scriptTimerA += delta;
            player->rotate((float)delta / 4000.0f, (float)delta / 4000.0f, (float)delta / 4000.0f);
            if (scriptTimerA >= 6001) {
                vector_0x28 = player->getPosition();
                vector_0x28 += Vector{700.0f, 0.0f, -1700.0f};
                m_pCamera->setPosition(vector_0x28);
                m_nState = 14;
                scriptTimerA = 0;
            }
            break;
        case 14:
            scriptTimerA += delta;
            player->rotate((float)delta / 5000.0f, (float)delta / 5000.0f, (float)delta / 5000.0f);
            m_pCamera->translate(0.0f, 0.0f, (float)(-2 * delta));
            if (scriptTimerA >= 12001) {
                vector_0x28 = player->getPosition();
                vector_0x28 += Vector{-3000.0f, 3500.0f, -6700.0f};
                m_pCamera->setPosition(vector_0x28);
                m_nState = 15;
                scriptTimerA = 0;
            }
            break;
        case 15:
            if (static_cast<RadioMessage *>((*messages)[22])->isOver()) {
                Globals::layout->startFade(true, 255, 5000);
                m_nState = 16;
                break;
            }
            [[fallthrough]];
        default:
            if (m_nState == 16 && !Globals::layout->isFading()) {
                Globals::layout->enableFillScreen(true);
                Globals::status->nextCampaignMission(true);
                Globals::switch_to_target_setting = 1;
                static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
                Level::initStreamOutPosition = 0;
                return false;
            }
            break;
        }
    } else if (Globals::status->getCurrentCampaignMission() == 1) {
        // Android 0x137d82..0x137e80, mission-one flight/title transition.
        // Literal words at 0x1380ac/b0 are 0x37800000/0x40c90fdb;
        // Hex-Rays' rounded 0.000015259 and 6.2832 are not exact constants.
        m_pLevel->lodManager->forceUpdate(delta, true);
        float turn = (float)(int)((2u * (unsigned)delta) & 0xfffffff0u)
                   * 0.0000152587890625f * 6.2831854820251465f;
        player->rotate(0.0f, turn, turn);
        Vector position = (*enemies)[0]->getPosition();
        (*enemies)[0]->setToSleep();
        float speed = 1.0f;
        if (position.z / -5000.0f < 1.0f)
            speed = position.z / -5000.0f;
        (*enemies)[0]->geometry->moveForward(((float)delta * 0.2f) * speed);
        if (static_cast<RadioMessage *>((*messages)[2])->isOver() && m_nState == 0) {
            m_nState = 1;
            Globals::layout->startFade(true, 255, 5000);
        }
        if (m_nState == 1 && !Globals::layout->isFading()) {
            Globals::layout->enableFillScreen(true);
            Globals::switch_to_target_setting = 0;
            static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(5);
            return false;
        }
        m_pCamera->translate((float)((double)delta * 0.1), 0.0f, 0.0f);
    } else if (Globals::status->getCurrentCampaignMission() == 81 && Globals::status->inAlienOrbit()) {
        m_pLevel->lodManager->forceUpdate(delta, true);
        m_pCamera->translate((float)(2 * delta), 0.0f, (float)(-4 * delta));
        cameraControlled = 1;
        if (m_nState == 1) {
            long long elapsed = scriptTimerA + delta;
            float progress = 1.0f;
            if ((float)elapsed / 4000.0f < 1.0f)
                progress = (float)elapsed / 4000.0f;
            scriptTimerA = elapsed;
            m_pCamera->setRumblePercentage(1.0f - progress, 100);
            PaintCanvas *canvas = Globals::Canvas;
            Matrix *local = static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
            m_pGeometry5->setDirection(AbyssEngine::AEMath::MatrixGetDir(*local), Vector{0.0f, 1.0f, 0.0f});
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                ->Update(delta, false);
            if (scriptTime >= 14001) {
                m_nState = 2;
                m_pCamera->setRumblePercentage(0.0f, 0);
            } else if (scriptTime >= 12001) {
                AEGeometry *station = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[0]) + 0x140);
                station->setVisible(true);
            }
        } else if (m_nState == 0) {
            long long elapsed = scriptTimerA + delta;
            float progress = 1.0f;
            if ((float)elapsed / 10000.0f < 1.0f)
                progress = (float)elapsed / 10000.0f;
            scriptTimerA = elapsed;
            m_pCamera->setRumblePercentage(progress, 100);
            if (scriptTime >= 10001) {
                m_pGeometry5->setPosition(Vector{0.0f, 0.0f, 10000.0f});
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                Globals::sound->play(160, nullptr, nullptr, 0.0f);
                m_nState = 1;
                scriptTimerA = 0;
                return true;
            }
        }
        if (m_nState == 2 && static_cast<RadioMessage *>((*messages)[3])->isOver()) {
            Globals::layout->startFade(true, 255, 5000);
            m_nState = 3;
            return true;
        }
        if (m_nState == 3 && !Globals::layout->isFading()) {
            Globals::layout->enableFillScreen(true);
            Globals::status->nextCampaignMission(true);
            Status *status = Globals::status;
            Globals::switch_to_target_setting = 0;
            Station *station = static_cast<Galaxy *>(Globals::galaxy)->getStation(100);
            status->departStation(station);
            Globals::status->getStation()->setAttackedFriends(false);
            Level::comingFromAlienWorld = 0;
            static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(5);
            Level::initStreamOutPosition = 0;
            return false;
        }
        return true;
    } else if (Globals::status->getCurrentCampaignMission() == 78) {
        if (scriptTime >= 7901) {
            PaintCanvas *canvas = Globals::Canvas;
            AEGeometry *station = *reinterpret_cast<AEGeometry **>(
                reinterpret_cast<char *>((*m_pLevel->getLandmarks())[0]) + 0x140);
            static_cast<AbyssEngine::Transform *>(canvas->TransformGetTransform(station->transform))->Update(delta, false);
        }
        switch (m_nState) {
        case 1: {
            TargetFollowCamera *camera = m_pCamera;
            AEGeometry *station = *reinterpret_cast<AEGeometry **>(
                reinterpret_cast<char *>((*m_pLevel->getLandmarks())[0]) + 0x140);
            camera->setTarget(station);
            PaintCanvas *canvas = Globals::Canvas;
            station = *reinterpret_cast<AEGeometry **>(
                reinterpret_cast<char *>((*m_pLevel->getLandmarks())[0]) + 0x140);
            static_cast<AbyssEngine::Transform *>(canvas->TransformGetTransform(station->transform))
                ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
            m_nState = 2;
            break;
        }
        case 2:
            m_pCamera->translate((float)-delta * 0.5f, (float)delta * 0.3f, (float)delta);
            if (scriptTime >= 7001) {
                m_nState = 3;
                m_pCamera->setPosition(20000.0f, 5000.0f, 30000.0f);
            }
            break;
        case 3:
            if (scriptTime >= 7901) {
                if (!Globals::sound->isPlaying(1122))
                    Globals::sound->play(1122, nullptr, nullptr, 0.0f);
                if (scriptTime >= 9001)
                    m_nState = 4;
            }
            break;
        case 4:
            if (scriptTime >= 18001) {
                m_nState = 5;
                m_pCamera->setPosition(8000.0f, 31000.0f, -9000.0f);
                player->freeze = 1;
                player->setVisible(false);
            }
            break;
        case 5:
            if (scriptTime >= 18001) {
                scriptTimerA = 0;
                m_nState = 6;
            }
            break;
        case 6:
            // Android 0x13bb7c..0x13bc10: float accumulation is converted
            // back to int64; the signed compare is against (1100 - delta).
            scriptTimerA = (long long)((float)delta * 0.3f + (float)scriptTimerA);
            (*enemies)[1]->geometry->translate(Vector{(float)delta * 0.3f, 0.0f, 0.0f});
            (*enemies)[1]->geometry->rotate(Vector{(float)-delta * 0.0009f, 0.0f, 0.0f});
            if (scriptTimerA >= 1100 - delta) {
                m_nState = 7;
                scriptTimerA = 0;
            }
            break;
        case 7:
            if (scriptTime >= 27001)
                m_nState = 8;
            break;
        case 8:
            scriptTimerA = (long long)((float)delta * 0.3f + (float)scriptTimerA);
            if (scriptTimerA < 1100 - delta) {
                (*enemies)[0]->geometry->translate(Vector{-((float)delta * 0.3f), 0.0f, 0.0f});
                (*enemies)[0]->geometry->rotate(Vector{(float)-delta * 0.0009f, 0.0f, 0.0f});
            } else if (scriptTimerA >= 2001) {
                m_nState = 9;
                scriptTimerA = 0;
                m_pCamera->setPosition(-12000.0f, 5000.0f, 15000.0f);
            }
            break;
        case 9: {
            m_pCamera->translate((float)-delta * 0.7f, (float)delta, (float)(3 * delta));
            long long elapsed = scriptTimerA + delta;
            float progress = 1.0f;
            if ((float)elapsed / 10000.0f < 1.0f)
                progress = (float)elapsed / 10000.0f;
            scriptTimerA = elapsed;
            m_pCamera->setRumblePercentage(progress, 100);
            if (scriptTimerA >= 6001) {
                m_pGeometry5->setPosition(Vector{0.0f, 0.0f, 14000.0f});
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                Globals::sound->play(160, nullptr, nullptr, 0.0f);
                m_nState = 10;
                scriptTimerA = 0;
            }
            break;
        }
        case 10: {
            m_pCamera->translate((float)-delta * 0.7f, (float)delta, (float)(4 * delta));
            long long elapsed = scriptTimerA + delta;
            float progress = 1.0f;
            if ((float)elapsed / 4000.0f < 1.0f)
                progress = (float)elapsed / 4000.0f;
            scriptTimerA = elapsed;
            m_pCamera->setRumblePercentage(1.0f - progress, 100);
            PaintCanvas *canvas = Globals::Canvas;
            Matrix *local = static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
            m_pGeometry5->setDirection(AbyssEngine::AEMath::MatrixGetDir(*local), Vector{0.0f, 1.0f, 0.0f});
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))->Update(delta, false);
            if (scriptTimerA >= 4001) {
                m_nState = 11;
                scriptTimerA = 0;
                m_pCamera->setRumblePercentage(0.0f, 0);
            } else if (scriptTimerA >= 2151) {
                (*enemies)[0]->setVisible(false);
                (*enemies)[1]->setVisible(false);
                (*m_pLevel->getLandmarks())[0]->setVisible(false);
            }
            break;
        }
        case 11:
            m_pCamera->translate((float)-delta * 0.7f, (float)delta, (float)(3 * delta));
            scriptTimerA += delta;
            if (scriptTimerA >= 3001) {
                for (unsigned i = 2; i < enemies->size_; ++i) {
                    (*enemies)[i]->translate(Vector{0.0f, 0.0f, -50000.0f});
                    (*enemies)[i]->awake();
                }
                m_pCamera->setTarget((*enemies)[enemies->size_ - 10]->geometry);
                vector_0x28 = (*enemies)[enemies->size_ - 10]->geometry->getPosition();
                vector_0x28.x += 2000.0f;
                vector_0x28.y += 500.0f;
                vector_0x28.z += -20000.0f;
                m_pCamera->setPosition(vector_0x28);
                m_pLevel->lodManager->forceUpdate(30, false);
                m_nState = 12;
                scriptTimerA = 0;
            }
            break;
        case 12:
            scriptTimerA += delta;
            m_pLevel->lodManager->forceUpdate(30, false);
            m_pCamera->translate((float)-delta * 0.3f, (float)delta * 0.3f, (float)delta * 0.5f);
            if (scriptTimerA >= 5001) {
                m_pCamera->setTarget(player->geometry);
                m_pHud->visible = 1;
                m_pRadar->enabled = 1;
                cameraControlled = 0;
                m_pCamera->setLookAtCam(false);
                player->setComputerControlled(false);
                static_cast<Player *>(player->player)->setVulnerable(true);
                player->freeze = 0;
                player->setVisible(true);
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 13;
            }
            break;
        }
        if ((unsigned)(m_nState - 5) <= 3)
            m_pCamera->translate((float)-delta * 0.8f, 0.0f, 0.0f);
        return cameraControlled != 0;
    } else if (Globals::status->getCurrentCampaignMission() == 89) {
        m_pLevel->lodManager->forceUpdate(delta, true);
        m_pCamera->setTarget(m_pGeometry4);
        cameraControlled = 1;
        if (m_nState <= 2) {
            float progress = (float)scriptTime / -35000.0f + 1.0f;
            m_pCamera->translate(progress * (float)delta, 0.0f,
                                ((float)delta + (float)delta) * progress);
            Vector offset = -m_pGeometry4->getRightVector() * (float)delta * 7.0f
                          * ((float)scriptTime / -50000.0f + 1.0f);
            m_pGeometry4->translate(offset);
        }
        static_cast<PlayerFixedObject *>((*enemies)[11])->moveForward(delta);
        switch (m_nState) {
        case 1:
            if (scriptTime < 30001)
                return true;
            m_nState = 2;
            vector_0x28 = *m_pCamera->getPosition();
            vector_0x28.y += 500.0f;
            (*m_pLevel->getEnemies())[0]->setPosition(vector_0x28);
            (*m_pLevel->getEnemies())[0]->geometry->moveForward(-10000.0f);
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry6->transform))
                ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry6->transform))
                ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
            m_pGeometry6->setDirection(-(*m_pLevel->getEnemies())[0]->geometry->getDirection(),
                                      Vector{0.0f, 1.0f, 0.0f});
            m_pGeometry6->setPosition((*m_pLevel->getEnemies())[0]->geometry->getPosition());
            return true;
        case 2: {
            Vector separation = *m_pCamera->getPosition() - (*m_pLevel->getEnemies())[0]->geometry->getPosition();
            float distance = AbyssEngine::AEMath::VectorLength(separation);
            float progress = 1.0f;
            if (distance / 7000.0f < 1.0f)
                progress = distance / 7000.0f;
            m_pCamera->setRumblePercentage((1.0f - progress) * 100.0f, 30);
            if (distance < 200000.0f)
                (*m_pLevel->getEnemies())[0]->geometry->moveForward((float)(10 * delta) + distance * 0.1f);
            m_pGeometry6->setPosition((*m_pLevel->getEnemies())[0]->geometry->getPosition());
            static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry6->transform))
                ->Update((long long)((float)delta * 0.5f), false);
            if (scriptTime < 38001)
                return true;
            if (scriptTime - delta <= 38000)
                Globals::sound->play(2248, nullptr, nullptr, 0.0f);
            auto *planets = static_cast<Array<AEGeometry *> *>(static_cast<StarSystem *>(m_pLevel->getStarSystem())->getPlanets());
            (*planets)[0]->setScaling((*planets)[0]->getScaling() * 0.95f);
            if (scriptTime < 39001)
                return true;
            Globals::layout->startFade(true, -1, 500);
            m_nState = 3;
            return true;
        }
        case 3: {
            bool fading = Globals::layout->isFading();
            StarSystem *system = static_cast<StarSystem *>(m_pLevel->getStarSystem());
            if (fading) {
                auto *planets = static_cast<Array<AEGeometry *> *>(system->getPlanets());
                if ((*planets)[0]->getScaling().x < 10.0f)
                    (*planets)[0]->setScaling((*planets)[0]->getScaling() * 4.0f);
                m_pCamera->setRumblePercentage(1.0f, 100);
            } else {
                system->switchSunForSupernovaIntro();
                Globals::layout->startFade(false, -1, 10000);
                (*enemies)[2]->setVisible(true);
                for (unsigned i = 3; i < enemies->size_; ++i)
                    (*enemies)[i]->player->setHitpoints(0);
                scriptTimerA = 0;
                m_nState = 4;
            }
            break;
        }
        case 4: {
            static_cast<StarSystem *>(m_pLevel->getStarSystem())->scaleSunDuringSupernovaIntro(delta);
            static_cast<StarSystem *>(m_pLevel->getStarSystem())->updateSupernova(delta);
            long long elapsed = scriptTimerA;
            if (elapsed <= 7000 && elapsed + delta >= 7001) {
                Globals::layout->startFade(true, 255, delta + 1000);
                elapsed = scriptTimerA;
            }
            elapsed += delta;
            scriptTimerA = elapsed;
            if (elapsed < 8000)
                return true;
            m_nState = 5;
            break;
        }
        case 5:
            if (Globals::layout->isFading())
                return true;
            Globals::layout->enableFillScreen(true);
            Globals::status->nextCampaignMission(true);
            Globals::switch_to_target_setting = 0;
            Globals::status->departStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(10));
            Globals::status->getStation()->setAttackedFriends(false);
            Level::comingFromAlienWorld = 0;
            static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(5);
            Level::initStreamOutPosition = 0;
            return false;
        default:
            return true;
        }
        return true;
    } else if (Globals::status->getCurrentCampaignMission() == 106) {
        int state = m_nState;
        switch (state) {
        case 1:
            m_pCamera->translate((float)-delta * 0.3f, (float)-delta * 0.05f, (float)delta * 0.1f);
            if (static_cast<RadioMessage *>((*messages)[2])->isOver()) {
                scriptTimerA = 0;
                m_nState = 2;
                (*enemies)[0]->awake();
                (*enemies)[0]->setVisible(true);
                static_cast<PlayerFighter *>((*enemies)[0])->setCloakingPossible(false);
                static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(true);
            }
            break;
        case 2:
            scriptTimerA += delta;
            m_pLevel->lodManager->forceUpdate(0, false);
            if (scriptTimerA < 3001) {
                m_pCamera->translate((float)-delta * 0.1f, (float)-delta * 0.1f, (float)delta * 0.4f);
            } else {
                m_pCamera->setTarget((*enemies)[0]->geometry);
                vector_0x28 = (*enemies)[0]->geometry->getPosition();
                Vector direction = (*enemies)[0]->geometry->getDirection() * 8000.0f;
                vector_0x28 += direction;
                Vector up = (*enemies)[0]->geometry->getUpVector() * 500.0f;
                vector_0x28 += up;
                Vector right = (*enemies)[0]->geometry->getRightVector() * 1000.0f;
                vector_0x28 += right;
                m_pCamera->setPosition(vector_0x28);
                m_nState = 3;
                scriptTimerA = 0;
            }
            break;
        case 3:
            if (static_cast<RadioMessage *>((*messages)[4])->isTriggered()) {
                m_nState = 4;
                break;
            }
            state = m_nState;
            goto mission106Tail;
        case 4:
            if (static_cast<RadioMessage *>((*messages)[4])->isOver()) {
                m_pCamera->setFixed(true);
                static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(false);
                (*enemies)[0]->player->setHitpoints(0);
                m_nState = 5;
                scriptTimerA = 0;
                break;
            }
            state = m_nState;
            // Reload the state after the message query, as in the original.
            [[fallthrough]];
        default:
mission106Tail:
            if (state == 6 && static_cast<RadioMessage *>((*messages)[5])->isOver()) {
                Globals::status->field_64 = static_cast<Player *>(player->player)->getHitpoints();
                Globals::status->field_5c = static_cast<Player *>(player->player)->getShieldHP();
                Globals::status->field_60 = static_cast<Player *>(player->player)->getArmorHP();
                int gamma = static_cast<Player *>(player->player)->getGammaHP();
                Status *status = Globals::status;
                status->field_68 = gamma;
                status->nextCampaignMission(true);
                Globals::status->nextCampaignMission(true);
                status = Globals::status;
                status->setStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(10));
                status = Globals::status;
                status->departStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(10));
                Globals::switch_to_target_setting = 1;
                Level::initStreamOutPosition = 1;
                static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
            } else if (state == 5) {
                scriptTimerA += delta;
                if (scriptTimerA >= 3001)
                    m_nState = 6;
            }
            break;
        }
        if ((unsigned)(m_nState - 2) <= 2)
            (*enemies)[0]->geometry->moveForward((float)delta);
        cameraControlled = 1;
        return true;
    } else if (Globals::status->getCurrentCampaignMission() == 95
               || Globals::status->getCurrentCampaignMission() == 99
               || Globals::status->getCurrentCampaignMission() == 109
               || Globals::status->getCurrentCampaignMission() == 119
               || Globals::status->getCurrentCampaignMission() == 126
               || Globals::status->getCurrentCampaignMission() == 133
               || Globals::status->getCurrentCampaignMission() == 160
               || Globals::status->getCurrentCampaignMission() == 161) {
        scriptTimerA += delta;
        m_pCamera->translate((float)delta, 0.0f, (float)(-2 * delta));
        if (m_nState == 1) {
            if (static_cast<RadioMessage *>((*messages)[0])->isOver()) {
                m_nState = 2;
                scriptTimerA = 0;
                return true;
            }
        } else if (m_nState == 0) {
            if (scriptTimerA >= 2000)
                m_nState = 1;
            return true;
        }
        if (m_nState == 2 && scriptTimerA >= 2000)
            reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->setStatusValue(1);
        return true;
    }

commonDeparture:
    // Android 0x137f7c..0x13812a: common 7001-ms departure sequence.
    if (m_bStartSequence) {
        m_pLevel->lodManager->forceUpdate(30, false);
        if (Globals::status->getWingmen()) {
            Array<KIPlayer *> *wingmen = m_pLevel->getEnemies();
            for (unsigned i = 0; i < wingmen->size_; ++i) {
                KIPlayer *wingman = (*wingmen)[i];
                if (wingman->isWingMan())
                    *(reinterpret_cast<uint8_t *>(wingman) + 0x12d) = 1;
            }
        }
        field_0x24 += delta;
        if (m_pLevel->getPlayer() && Globals::status->getCurrentCampaignMission() >= 2
            && field_0x24 >= 7001) {
            field_0x24 = 0;
            sequenceState = 0x100;
            m_pCamera->setLookAtCam(false);
            static_cast<Player *>(m_pLevel->getPlayer()->player)->setVulnerable(true);
            cameraControlled = 0;
            m_pLevel->getPlayer()->setCollide(true);
            Station *programmed = static_cast<Station *>(Level::programmedStation);
            if (programmed && !programmed->equals(Globals::status->getStation()))
                m_pHud->hudEvent(5, m_pLevel->getPlayer(), 0);
            Level::initStreamOutPosition = 0;
            if (!Level::doInstantJump)
                setAutoPilotToProgrammedStation();
            if (Globals::status->getWingmen()) {
                Array<KIPlayer *> *wingmen = m_pLevel->getEnemies();
                for (unsigned i = 0; i < wingmen->size_; ++i) {
                    KIPlayer *wingman = (*wingmen)[i];
                    if (wingman->isWingMan())
                        *(reinterpret_cast<uint8_t *>(wingman) + 0x12d) = 0;
                }
            }
        }
    }
    if (Globals::status->getCurrentCampaignMission() == 5) {
        if (m_nState == 0) {
            vector_0x28 = player->getPosition();
            (*enemies)[0]->setPosition(vector_0x28.x + 5000.0f, vector_0x28.y, vector_0x28.z + 30000.0f);
            (*enemies)[0]->geometry->rotate(0.0f, 3.1415927410125732421875f, 0.0f);
            (*enemies)[0]->awake();
            m_nState = 1;
        }
    } else if (Globals::status->getCurrentCampaignMission() == 26 && Globals::status->inAlienOrbit()) {
        if (static_cast<RadioMessage *>((*messages)[1])->isTriggered() && m_nState == 0) {
            m_nState = 1;
            static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->reset(true);
        }
    } else if ((Globals::status->getCurrentCampaignMission() == 50
                || Globals::status->getCurrentCampaignMission() == 51)
               && messages && messages->size_ >= 3
               && static_cast<RadioMessage *>((*messages)[2])->isOver() && m_nState == 0) {
        for (unsigned i = 0; i < enemies->size_; ++i) {
            if (!(*enemies)[i]->isWingMan()) {
                (*enemies)[i]->player->setAlwaysFriend(false);
                (*enemies)[i]->player->setAlwaysEnemy(true);
                (*enemies)[i]->player->turnEnemy();
            }
        }
        reinterpret_cast<Standing *>(static_cast<intptr_t>(Globals::status->getStanding()))->setStanding(0, 100);
        m_nState = 1;
    }

    Mission *mission = Globals::status->getMission();
    if ((mission->isCampaignMission() && !Globals::status->getMission()->isEmpty())
        || Globals::status->getCurrentCampaignMission() == 42) {
        switch (Globals::status->getCurrentCampaignMission()) {
        case 14:
            if (static_cast<RadioMessage *>((*messages)[1])->isTriggered() && m_nState == 0) {
                m_nState = 1;
                (*m_pLevel->getEnemies())[0]->player->setHitpoints(0);
                (*m_pLevel->getEnemies())[0]->setDead();
                ParticleSystemManager *manager = m_pLevel->field_8c;
                int handle = *reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 304);
                manager->systemSetMatrix(handle, &player->geometry->getReferenceMatrix());
                m_pLevel->field_8c->enableSystemEmit(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 304), true);
                m_pLevel->field_8c->enableSystemRender(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 304), true);
                manager = m_pLevel->field_8c;
                handle = *reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 308);
                manager->systemSetMatrix(handle, &player->geometry->getReferenceMatrix());
                m_pLevel->field_8c->enableSystemEmit(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 308), true);
                m_pLevel->field_8c->enableSystemRender(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 308), true);
                manager = m_pLevel->field_8c;
                handle = *reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 312);
                manager->systemSetMatrix(handle, &player->geometry->getReferenceMatrix());
                m_pLevel->field_8c->enableSystemEmit(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 312), true);
                m_pLevel->field_8c->enableSystemRender(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 312), true);
                Globals::sound->play(15, nullptr, nullptr, 0.0f);
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[1])->isOver() && m_nState == 1) {
                Vector velocity{0.0f, 0.0f, 0.0f};
                Vector position = player->getPosition();
                Globals::sound->updateEvent3DAttributes(15, &position, &velocity, false);
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                cameraControlled = 1;
                player->stopShooting(0);
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                m_pLevel->flashScreen(3);
                player->setComputerControlled(true);
                player->setSpeed(0.0f);
                static_cast<Player *>(player->player)->removeAllGuns();
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget(player->geometry);
                m_pCamera->setPosition(player->getPosition());
                m_pCamera->translate(1000.0f, 700.0f, 1000.0f);
                for (unsigned i = 0; i < m_pLevel->getEnemies()->size_; ++i) {
                    if ((*m_pLevel->getEnemies())[i]->shipGroup == 8) {
                        (*m_pLevel->getEnemies())[i]->player->setHitpoints(0);
                        (*m_pLevel->getEnemies())[i]->setDead();
                    }
                }
                m_pLevel->lodManager->forceUpdate(delta, false);
                ++m_nState;
                break;
            }
            if (m_nState == 2) {
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->geometry->updateReferenceMatrix();
                player->geometry->rotate(0.0f, (float)delta / 10000.0f, 0.0f);
                m_pCamera->translate(0.0f, 0.0f, (float)delta);
                if (static_cast<RadioMessage *>((*messages)[3])->isOver()) {
                    player->setVisible(false);
                    m_pLevel->field_80->enableSystemEmit(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 304), false);
                    m_pLevel->field_80->enableSystemEmit(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 308), false);
                    m_pLevel->field_80->enableSystemEmit(*reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 312), false);
                    Array<KIPlayer *> *targets = m_pLevel->getEnemies();
                    KIPlayer *target = (*targets)[m_pLevel->getEnemies()->size_ - 1];
                    static_cast<PlayerFixedObject *>(target)->setMoving(true);
                    m_pCamera->setTarget(target->geometry);
                    m_pCamera->setPosition(target->getPosition());
                    m_pCamera->translate(-10000.0f, -200.0f, 5000.0f);
                    m_nState = 3;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                }
            } else if (static_cast<RadioMessage *>((*messages)[4])->isOver() && m_nState == 3) {
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 4;
                scriptTimerA = scriptTime;
            } else if (m_nState == 4 && scriptTime >= scriptTimerA + 4000) {
                Globals::status->nextCampaignMission(true);
                Globals::status->departStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(98));
                Globals::switch_to_target_setting = 0;
                static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(5);
                Level::initStreamOutPosition = 0;
                m_nState = 5;
            }
            break;
        case 16:
            if (static_cast<RadioMessage *>((*messages)[0])->isTriggered() && m_nState == 0) {
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                m_pCamera->setLookAtCam(true);
                m_pCamera->setActive(true);
                m_pCamera->setTarget((*m_pLevel->getEnemies())[0]->geometry);
                Vector position = (*m_pLevel->getEnemies())[0]->getPosition();
                position += Vector{6000.0f, 4000.0f, 47500.0f};
                m_pCamera->setPosition(position);
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 1;
                break;
            }
            if (m_nState == 1) {
                m_pCamera->translate((float)-delta, 0.0f, (float)(-3 * delta));
                m_pLevel->lodManager->forceUpdate(delta, false);
                if (static_cast<RadioMessage *>((*messages)[1])->isOver()) {
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    m_nState = 2;
                }
                break;
            }
            if (m_nState == 2 && scriptTime >= 20001) {
                m_nState = 3;
                static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->reset(true);
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[2])->isTriggered() && m_nState == 3) {
                m_nState = 4;
                static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->open();
                (*m_pLevel->getLandmarks())[3]->setVisible(true);
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                player->freeze = 1;
                m_pCamera->setLookAtCam(true);
                m_pCamera->setActive(true);
                m_pCamera->setTarget((*m_pLevel->getLandmarks())[3]->geometry);
                Vector position = (*m_pLevel->getLandmarks())[3]->getPosition();
                position += Vector{-9000.0f, -4000.0f, -30000.0f};
                m_pCamera->setPosition(position);
                position = (*m_pLevel->getLandmarks())[3]->getPosition();
                int coordinates[3] = {(int)position.x, (int)position.y, (int)(position.z + 30000.0f)};
                Route *route = new Route(coordinates, 3);
                position.z += -15000.0f;
                for (unsigned i = 0; i < m_pLevel->getEnemies()->size_; ++i) {
                    KIPlayer *enemy = (*m_pLevel->getEnemies())[i];
                    if (enemy && enemy->shipGroup == 9) {
                        float y = (position.y + -2000.0f) + (float)static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(4000);
                        float z = (position.z + -2000.0f) + (float)static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(4000);
                        enemy->setPosition(position.x, y, z);
                        Vector target = (*m_pLevel->getLandmarks())[3]->getPosition();
                        Vector enemyPosition = enemy->getPosition();
                        Vector direction = AbyssEngine::AEMath::VectorNormalize(target - enemyPosition);
                        enemy->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                        enemy->setRoute(route->clone());
                        enemy->player->setEnemies(nullptr);
                    }
                }
                m_pLevel->lodManager->forceUpdate(delta, false);
                break;
            }
            if (m_nState == 4) {
                m_pCamera->translate((float)((double)delta * 0.5), 0.0f, (float)((double)delta * 0.2));
                if (static_cast<RadioMessage *>((*messages)[3])->isOver()) {
                    m_nState = 5;
                    for (unsigned i = 0; i < m_pLevel->getEnemies()->size_; ++i) {
                        KIPlayer *enemy = (*m_pLevel->getEnemies())[i];
                        if (enemy && enemy->shipGroup == 9) {
                            enemy->setVisible(false);
                            enemy->setDead();
                        }
                    }
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->reset(true);
                }
            } else if (static_cast<RadioMessage *>((*messages)[4])->isOver() && m_nState == 5) {
                m_nState = 6;
                player->freeze = 0;
                m_pCamera->setLookAtCam(false);
                m_pCamera->setTarget(player->geometry);
                player->setComputerControlled(false);
                m_pHud->visible = 1;
                m_pRadar->enabled = 1;
                resetCamera(m_pLevel);
                cameraControlled = 0;
                m_pLevel->lodManager->forceUpdate(delta, false);
            }
            break;
        case 21:
            if (m_nState == 0) {
                for (unsigned i = 1; i != 4; ++i) {
                    if ((*m_pLevel->getEnemies())[i]->player->enemyFlagsLo) {
                        for (unsigned j = 1; j != 4; ++j)
                            (*m_pLevel->getEnemies())[j]->player->setAlwaysEnemy(true);
                        break;
                    }
                }
            }
            if (static_cast<RadioMessage *>((*messages)[2])->isTriggered() && m_nState == 0) {
                for (unsigned i = 1; i != 4; ++i)
                    (*m_pLevel->getEnemies())[i]->player->setAlwaysEnemy(true);
                ++m_nState;
            }
            break;
        case 24:
            if (static_cast<RadioMessage *>((*messages)[3])->isTriggered() && m_nState == 0) {
                player->stopShooting(0);
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                m_pCamera->setLookAtCam(true);
                static_cast<Player *>(player->player)->setVulnerable(false);
                player->geometry->setRotation(0.0f, 1.57079637050628662109375f, 0.0f);
                player->setComputerControlled(true);
                static_cast<Player *>(player->player)->removeAllGuns();
                m_pCamera->setTarget(player->geometry);
                m_pCamera->setPosition(player->geometry->getPosition());
                m_pCamera->translate(10500.0f, 700.0f, 1000.0f);
                vector_0x28 = player->geometry->getPosition();
                vector_0x40 = player->geometry->getDirection();
                vector_0x40 *= 45000.0f;
                vector_0x28 += vector_0x40;
                (*m_pLevel->getLandmarks())[3]->setPosition(vector_0x28.x, vector_0x28.y, vector_0x28.z);
                for (unsigned i = 0; i < m_pLevel->getEnemies()->size_; ++i)
                    (*m_pLevel->getEnemies())[i]->player->setEnemies(nullptr);
                m_pLevel->lodManager->forceUpdate(delta, false);
                cameraControlled = 1;
                m_nState = 1;
            }
            if (static_cast<RadioMessage *>((*messages)[4])->isTriggered() && m_nState == 1) {
                (*m_pLevel->getLandmarks())[3]->setVisible(true);
                static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->reset(false);
                static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->open();
                Globals::sound->play(34, nullptr, nullptr, 0.0f);
                scriptTimerA = 0;
                m_nState = 2;
            } else if (m_nState != 2) {
                break;
            }
            {
                Vector position = *m_pCamera->getPosition();
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(34, &position, &velocity, false);
                m_pCamera->setRumblePercentage(50.0f, 3);
                scriptTimerA += delta;
                player->rotate((float)delta / 5000.0f, (float)delta / 5000.0f, (float)delta / 5000.0f);
            }
            break;
        case 29:
            if (static_cast<RadioMessage *>((*messages)[0])->isTriggered() && m_nState == 0) {
                m_nState = 1;
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                static_cast<Player *>(player->player)->setVulnerable(false);
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->stopShooting(0);
                cameraControlled = 1;
                m_pCamera->setLookAtCam(true);
                player->setComputerControlled(true);
                m_pCamera->setPosition(player->geometry->getPosition());
                vector_0x28 = player->geometry->getDirection();
                vector_0x28 *= 25000.0f;
                Vector offset{0.0f, 0.0f, 0.0f};
                offset = player->geometry->getRightVector();
                offset *= 1000.0f;
                vector_0x28 += offset;
                offset = player->geometry->getUpVector();
                offset *= 1000.0f;
                vector_0x28 += offset;
                vector_0x28 += player->geometry->getPosition();
                m_pCamera->setPosition(vector_0x28.x, vector_0x28.y, vector_0x28.z);
                m_pGeometry6 = new AEGeometry(0x37d2, Globals::Canvas, false);
                m_pGeometry6->setPosition(player->geometry->getPosition());
                m_pGeometry6->setDirection(player->geometry->getDirection(), Vector{0.0f, 1.0f, 0.0f});
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_pCamera->setTarget(m_pGeometry6);
                Globals::sound->play(14, nullptr, nullptr, 0.0f);
                Vector position = *m_pCamera->getPosition();
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(14, &position, &velocity, false);
            } else if (static_cast<RadioMessage *>((*messages)[2])->isOver() && m_nState == 1) {
                m_nState = 2;
                m_pCamera->setTarget(player->geometry);
                m_pHud->visible = 1;
                m_pRadar->enabled = 1;
                cameraControlled = 0;
                m_pCamera->setLookAtCam(false);
                player->setComputerControlled(false);
                static_cast<Player *>(player->player)->setVulnerable(true);
                delete m_pGeometry6;
                scriptTime = 0;
                m_nTimeLimit = 180000;
                m_pGeometry6 = nullptr;
                m_pLevel->objectivesA = new Objective(3, 180000, m_pLevel);
                m_pLevel->lodManager->forceUpdate(delta, false);
            }
            if (m_pGeometry6)
                m_pGeometry6->moveForward((float)(3 * delta));
            break;
        case 40:
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[3])->isTriggered()) {
                static_cast<PlayerFixedObject *>((*enemies)[0])->setMoving(true);
                KIPlayer *enemy = (*enemies)[0];
                float x = (float)m_pLevel->getFriendRoute()->getWaypoint(0)->x;
                int y = m_pLevel->getFriendRoute()->getWaypoint(0)->y;
                Waypoint *waypoint = m_pLevel->getFriendRoute()->getWaypoint(0);
                enemy->setPosition(x, (float)y, (float)waypoint->z);
                (*enemies)[0]->awake();
                (*enemies)[0]->setVisible(true);
                (*enemies)[0]->shipGroup = 1;
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                static_cast<Player *>(player->player)->setVulnerable(false);
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->stopShooting(0);
                cameraControlled = 1;
                m_pCamera->setLookAtCam(true);
                m_pCamera->setPosition((*enemies)[0]->getPosition());
                m_pCamera->translate(-7000.0f, 500.0f, 17000.0f);
                m_pCamera->setTarget((*enemies)[0]->geometry);
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 1;
                break;
            }
            if (m_nState == 2) {
                if ((*enemies)[0]->getPosition().z >= 90000.0f) {
                    m_nState = 3;
                    for (int i = (int)enemies->size_ - 4; i < (int)enemies->size_; ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        float x = (*m_pLevel->getLandmarks())[3]->getPosition().x;
                        int rx = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(20000);
                        float y = (*m_pLevel->getLandmarks())[3]->getPosition().y;
                        int ry = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(20000);
                        float z = (*m_pLevel->getLandmarks())[3]->getPosition().z;
                        int rz = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(20000);
                        enemy->setPosition(x - 10000.0f + (float)rx,
                                           y - 10000.0f + (float)ry,
                                           z - 10000.0f + (float)rz);
                        (*enemies)[i]->awake();
                        (*enemies)[i]->awake();
                    }
                    break;
                }
            } else if (m_nState == 1) {
                m_pCamera->translate(0.0f, 0.0f, (float)(-2 * delta));
                if (!static_cast<RadioMessage *>((*messages)[4])->isOver())
                    break;
                m_pCamera->setTarget(player->geometry);
                m_pHud->visible = 1;
                m_pRadar->enabled = 1;
                cameraControlled = 0;
                m_pCamera->setLookAtCam(false);
                player->setComputerControlled(false);
                static_cast<Player *>(player->player)->setVulnerable(true);
                m_nState = 2;
                break;
            } else if (m_nState == 3) {
                float z = (*enemies)[0]->getPosition().z;
                if (z >= (*m_pLevel->getLandmarks())[3]->getPosition().z) {
                    m_nState = 4;
                    break;
                }
            }
            if (m_nState == 4) {
                PlayerFixedObject *enemy = static_cast<PlayerFixedObject *>((*enemies)[0]);
                Vector position = enemy->getPosition();
                enemy->moveForward((int)(position.z - 200000.0f - (float)delta));
                if ((*enemies)[0]->getPosition().z > 500000.0f) {
                    (*enemies)[0]->setPosition(0.0f, 0.0f, -200000.0f);
                    (*enemies)[0]->setActive(false);
                    m_nState = 5;
                }
            }
            break;
        case 41:
            if (static_cast<RadioMessage *>((*messages)[4])->isTriggered() && m_nState == 0) {
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                player->freeze = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                cameraControlled = 1;
                m_pCamera->setLookAtCam(true);
                player->setComputerControlled(true);
                static_cast<Player *>(player->player)->setVulnerable(false);
                for (unsigned i = 1; i != 4; ++i) {
                    (*enemies)[i]->revive();
                    KIPlayer *enemy = (*enemies)[i];
                    Vector position = (*enemies)[0]->getPosition();
                    enemy->setPosition(position);
                    (*enemies)[i]->player->setEnemy((*enemies)[0]->player);
                    enemy = (*enemies)[i];
                    enemy->field_0x38 = 0;
                    enemy->setState(1);
                }
                vector_0x28 = Vector{-40000.0f, 500.0f, -30000.0f};
                (*enemies)[1]->translate(vector_0x28);
                (*enemies)[1]->geometry->setDirection(VectorNormalize(-vector_0x28), Vector{0.0f, 1.0f, 0.0f});
                vector_0x28 = Vector{-41000.0f, -200.0f, -31000.0f};
                (*enemies)[2]->translate(vector_0x28);
                (*enemies)[2]->geometry->setDirection(VectorNormalize(-vector_0x28), Vector{0.0f, 1.0f, 0.0f});
                vector_0x28 = Vector{-42000.0f, 100.0f, -32000.0f};
                (*enemies)[3]->translate(vector_0x28);
                (*enemies)[3]->geometry->setDirection(VectorNormalize(-vector_0x28), Vector{0.0f, 1.0f, 0.0f});
                m_pCamera->setTarget((*enemies)[0]->geometry);
                m_pCamera->setPosition((*enemies)[0]->getPosition());
                m_pCamera->translate(-38000.0f, 0.0f, -30200.0f);
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 1;
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[5])->isTriggered() && m_nState == 1) {
                m_nState = 2;
                m_pCamera->setRotationAroundTarget(0);
                (*enemies)[0]->geometry->updateReferenceMatrix();
                m_pLevel->field_74->enableSystemEmit(particleSystem_0xe0, true);
                m_pLevel->field_74->enableSystemEmit(particleSystem_0xe4, true);
                KIPlayer *enemy = (*m_pLevel->getEnemies())[0];
                enemy->player->setHitpoints(9999999);
                static_cast<PlayerFixedObject *>(enemy)->setMoving(false);
                m_pCamera->setTarget(enemy->geometry);
                m_pCamera->setPosition(enemy->getPosition());
                m_pCamera->translate(-3000.0f, -2000.0f, 12000.0f);
                Globals::sound->play(155, nullptr, nullptr, 0.0f);
                (*enemies)[0]->player->StopEngineSound();
                scriptTimerA = 0;
                break;
            }
            if (m_nState == 3) {
                (*enemies)[0]->geometry->updateReferenceMatrix();
                KIPlayer *enemy = (*m_pLevel->getEnemies())[0];
                enemy->setPosition(2006.0f, -31500.0f, -86720.0f);
                enemy->geometry->setRotation(-0.4000000059604644775390625f, 0.0f, 1.7999999523162841796875f);
                static_cast<PlayerFixedObject *>(enemy)->setExhaustVisible(false);
                m_pCamera->setPosition(enemy->getPosition());
                m_pCamera->translate(3000.0f, 1000.0f, 2000.0f);
                m_pCamera->useTargetsUpVector(false);
                for (unsigned i = 1; i < enemies->size_; ++i) {
                    KIPlayer *other = (*enemies)[i];
                    if (other && other->shipGroup == 9)
                        other->player->setEnemy(static_cast<Player *>(player->player));
                }
                scriptTimerA = 0;
                m_nState = 4;
                break;
            }
            if (m_nState == 2) {
                (*enemies)[0]->geometry->updateReferenceMatrix();
                KIPlayer *enemy = (*m_pLevel->getEnemies())[0];
                enemy->translate(Vector{0.0f, (float)-delta, (float)(2 * delta)});
                enemy->geometry->rotate(0.0f, 0.0f, (float)delta * 0.00002999999924213625490665435791015625f);
                scriptTimerA += delta;
                if (scriptTimerA >= 15001)
                    m_nState = 3;
                break;
            }
            if (m_nState == 4) {
                (*enemies)[0]->geometry->updateReferenceMatrix();
                scriptTimerA += delta;
                m_pCamera->translate((float)delta, (float)delta, (float)(-2 * delta));
                if (scriptTimerA >= 15001) {
                    Globals::sound->stop(156);
                    m_nState = 5;
                    (*enemies)[0]->player->setHitpoints(100);
                    (*enemies)[0]->field_0x64 = 0;
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    cameraControlled = 0;
                    m_pCamera->setTarget(player->geometry);
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->useTargetsUpVector(true);
                    player->setComputerControlled(false);
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    player->freeze = 0;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                }
            }
            break;
        case 42:
            if (messages && m_nState == 5 && static_cast<RadioMessage *>((*messages)[7])->isOver()) {
                Globals::sound->play(153, nullptr, nullptr, 0.0f);
                scriptTime = 0;
                delete m_pLevel->objectivesB;
                m_pLevel->objectivesB = nullptr;
                delete m_pLevel->objectivesA;
                m_pLevel->objectivesA = nullptr;
                (*m_pLevel->getLandmarks())[3]->setPosition(25000.0f, 20000.0f, -55000.0f);
                static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->open();
                scriptTimerA = 0;
                m_nState = 6;
                break;
            }
            if (m_nState == 0 && !Globals::status->inAlienOrbit()) {
                static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->reset(true);
                m_nState = 1;
                break;
            }
            if (m_nState == 6) {
                Vector position = player->getPosition();
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(153, &position, &velocity, false);
                scriptTimerA += delta;
                m_pCamera->setRumblePercentage(0.5f, 30);
                Globals::sound->setParamValue(1, 153, 0.5f);
                if (player->isInWormhole()) {
                    player->setTurretMode(false);
                    resetCamera(m_pLevel);
                    player->setFreeLookMode(false);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    cameraControlled = 1;
                    m_pCamera->setLookAtCam(true);
                    player->setComputerControlled(true);
                    player->setVisible(false);
                    static_cast<Player *>(player->player)->setVulnerable(false);
                    m_pCamera->setPosition(player->getPosition());
                    AEGeometry *stationGeometry = *reinterpret_cast<AEGeometry **>(
                        reinterpret_cast<char *>((*m_pLevel->getLandmarks())[0]) + 320);
                    m_pCamera->setTarget(stationGeometry);
                    vector_0x28 = 5000.0f * AbyssEngine::AEMath::VectorNormalize(player->getPosition());
                    m_pCamera->translate(vector_0x28.x, vector_0x28.y, vector_0x28.z);
                    static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->freeMissionLock();
                    static_cast<PlayerWormHole *>((*m_pLevel->getLandmarks())[3])->reset(true);
                    m_bRenderParticles = 1;
                    Globals::sound->play(154, nullptr, nullptr, 0.0f);
                    scriptTimerA = 0;
                    m_nState = 7;
                }
                break;
            }
            if (m_nState == 7) {
                m_pCamera->setRumblePercentage(100.0f, 30);
                m_pCamera->translate(0.0f, 0.0f, (float)(-18 * delta));
                if (m_bRenderParticles) {
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pParticleGeom0->transform))->Update(delta, false);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pParticleGeom1->transform))->Update(delta, false);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pParticleGeom2->transform))->Update(delta, false);
                    PaintCanvas *canvas = Globals::Canvas;
                    Matrix *local = static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                    Vector direction = AbyssEngine::AEMath::MatrixGetDir(*local);
                    m_pParticleGeom1->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                    m_pParticleGeom2->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                    m_pParticleGeom1->setPosition(direction * 10000.0f);
                    if (static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pParticleGeom0->transform))->currentTime >= 4001)
                        static_cast<PlayerStation *>((*m_pLevel->getLandmarks())[0])->setVisible(false);
                    scriptTimerA += delta;
                    if (scriptTimerA >= 15001) {
                        m_bRenderParticles = 0;
                        Globals::layout->startFade(true, 255, 4000);
                        scriptTimerA = 0;
                    }
                } else {
                    scriptTimerA += delta;
                    if (Globals::layout->isFading() && scriptTimerA < 10001)
                        break;
                    Globals::status->field_64 = static_cast<Player *>(player->player)->getHitpoints();
                    Globals::status->field_5c = static_cast<Player *>(player->player)->getShieldHP();
                    Globals::status->field_60 = static_cast<Player *>(player->player)->getArmorHP();
                    Globals::status->field_68 = static_cast<Player *>(player->player)->getGammaHP();
                    int weapon = player->getCurrentSecondaryWeaponIndex();
                    Status *status = Globals::status;
                    int station = status->field_84;
                    status->field_f4 = weapon;
                    status->setStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(station));
                    Globals::status->departStation(Globals::status->getStation());
                    Level::initStreamOutPosition = 1;
                    Level::comingFromAlienWorld = 1;
                    Globals::switch_to_target_setting = 1;
                    Level::programmedStation = nullptr;
                    static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
                    bool controlled = cameraControlled != 0;
                    field_0x12 = 257;
                    return controlled;
                }
            }
            break;
        case 56:
            if (m_nState == 0) {
                if (enemies) {
                    for (unsigned i = 0; i < enemies->size_; ++i)
                        if ((*enemies)[i]->player->isAlwaysFriend())
                            (*enemies)[i]->player->setShootingEnabled(false);
                }
                m_nState = 1;
            } else if (m_nState == 1 && static_cast<Player *>(player->player)->getArmorHP() <= 0) {
                if (enemies) {
                    for (unsigned i = 0; i < enemies->size_; ++i)
                        if ((*enemies)[i]->player->isAlwaysFriend())
                            (*enemies)[i]->player->setShootingEnabled(true);
                }
                m_nState = 2;
            }
            break;
        case 64:
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[0])->isTriggered()) {
                player->freeze = 1;
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                m_pCamera->setLookAtCam(true);
                m_pCamera->setActive(true);
                m_pCamera->setTarget((*enemies)[0]->geometry);
                Vector position = (*enemies)[0]->getPosition();
                position += (*enemies)[0]->geometry->getDirection() * 10000.0f;
                position.z += 3000.0f;
                position.y += 300.0f;
                m_pCamera->setPosition(position);
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 1;
                break;
            }
            if (m_nState == 2)
                (*enemies)[0]->field_0x24 = 1;
            if (m_nState == 1) {
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_pCamera->translate(0.0f, (float)((double)delta * 0.2), 0.0f);
                if (static_cast<RadioMessage *>((*messages)[2])->isTriggered()) {
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    player->freeze = 0;
                    for (unsigned i = 0; i < enemies->size_; ++i)
                        (*enemies)[i]->setRoute(nullptr);
                    ++m_nState;
                }
                break;
            }
            if (m_nState == 2 && static_cast<RadioMessage *>((*messages)[5])->isTriggered()) {
                (*enemies)[0]->field_0x24 = 0;
                int coordinates[3] = {500000, 500000, 500000};
                Route *route = new Route(coordinates, 3);
                for (unsigned i = 1; i < enemies->size_; ++i) {
                    if (!(*enemies)[i]->isWingMan()) {
                        (*enemies)[i]->setSpeed(16.5f);
                        (*enemies)[i]->player->setEnemies(nullptr);
                        (*enemies)[i]->setRoute(route->clone());
                    }
                }
                m_nState = 3;
            } else if (m_nState == 3 && static_cast<RadioMessage *>((*messages)[6])->isOver()) {
                for (unsigned i = 1; i < enemies->size_; ++i)
                    (*enemies)[i]->setDead();
            }
            break;
        case 67:
            if (static_cast<RadioMessage *>((*messages)[1])->isTriggered() && m_nState == 0) {
                m_nState = 1;
                (*enemies)[10]->setSpeed(0.0f);
                (*enemies)[10]->setEnemies(nullptr);
                static_cast<PlayerFighter *>((*enemies)[10])->setExhaustVisible(false);
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[3])->isOver() && m_nState == 1) {
                (*enemies)[10]->setSpeed(3.0f);
                static_cast<PlayerFighter *>((*enemies)[10])->setExhaustVisible(true);
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                m_pCamera->setLookAtCam(true);
                m_pCamera->setActive(true);
                m_pCamera->setTarget((*enemies)[10]->geometry);
                m_pCamera->setPosition((*enemies)[10]->getPosition() + Vector{6000.0f, -200.0f, 1000.0f});
                static_cast<Player *>(player->player)->setVulnerable(false);
                player->freeze = 1;
                m_nState = 2;
                break;
            }
            if (m_nState == 2) {
                if (static_cast<RadioMessage *>((*messages)[4])->isOver()) {
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    player->freeze = 0;
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    (*enemies)[10]->setPosition(500000.0f, 500000.0f, 500000.0f);
                    (*enemies)[10]->setActive(false);
                    (*enemies)[10]->setVisible(false);
                    m_nState = 3;
                    cameraControlled = 0;
                }
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[5])->isTriggered() && m_nState == 3) {
                Vector position = (*enemies)[0]->getPosition();
                for (unsigned i = 5; i != 9; ++i) {
                    KIPlayer *enemy = (*enemies)[i];
                    int x = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 15000;
                    int xSign = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                    int y = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000);
                    int z = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 15000;
                    int zSign = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                    enemy->setPosition(position + Vector{(float)(xSign * x), (float)(y - 5000), (float)(zSign * z)});
                }
                m_nState = 4;
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[7])->isTriggered() && m_nState == 4) {
                Vector position = (*enemies)[0]->getPosition();
                (*enemies)[10]->setPosition(position.x, position.y, position.z + 12000.0f);
                (*enemies)[10]->geometry->setDirection(Vector{0.0f, 0.0f, 1.0f}, Vector{0.0f, 1.0f, 0.0f});
                (*enemies)[10]->setActive(true);
                (*enemies)[10]->setVisible(true);
                (*enemies)[10]->setSpeed(2.0f);
                delete m_pLevel->objectivesB;
                m_pLevel->objectivesB = nullptr;
                int coordinates[3] = {0, 0, 800000};
                Route *route = new Route(coordinates, 3);
                (*enemies)[10]->setRoute(route);
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                m_pCamera->setLookAtCam(true);
                m_pCamera->setActive(true);
                m_pCamera->setTarget((*enemies)[10]->geometry);
                m_pCamera->setPosition((*enemies)[10]->getPosition() + Vector{6000.0f, -300.0f, 10000.0f});
                static_cast<Player *>(player->player)->setVulnerable(false);
                player->freeze = 1;
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 5;
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[9])->isTriggered() && m_nState == 5) {
                static_cast<Player *>(player->player)->setVulnerable(true);
                player->freeze = 0;
                (*enemies)[10]->setSpeed(8.0f);
                m_pCamera->setLookAtCam(false);
                m_pCamera->setTarget(player->geometry);
                player->setComputerControlled(false);
                m_pHud->visible = 1;
                m_pRadar->enabled = 1;
                resetCamera(m_pLevel);
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 6;
                cameraControlled = 0;
                break;
            }
            if (static_cast<RadioMessage *>((*messages)[11])->isOver() && m_nState == 6) {
                (*enemies)[10]->setActive(false);
                (*enemies)[10]->setVisible(false);
                m_nState = 7;
            }
            break;
        case 69:
            if (static_cast<RadioMessage *>((*messages)[0])->isTriggered() && m_nState == 0) {
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                m_pCamera->setLookAtCam(true);
                m_pCamera->setActive(true);
                m_pCamera->setTarget((*m_pLevel->getEnemies())[0]->geometry);
                Vector position = (*m_pLevel->getEnemies())[0]->getPosition();
                position += (*m_pLevel->getEnemies())[0]->geometry->getDirection() * 10000.0f;
                position.x += 600.0f;
                position.y += 300.0f;
                position.z += 1000.0f;
                m_pCamera->setPosition(position);
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_nState = 1;
            } else if (m_nState == 1) {
                m_pLevel->lodManager->forceUpdate(delta, false);
                if (static_cast<RadioMessage *>((*messages)[1])->isOver()) {
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    m_nState = 2;
                    (*m_pLevel->getEnemies())[0]->setActive(false);
                    (*m_pLevel->getEnemies())[0]->setToSleep();
                    (*m_pLevel->getEnemies())[0]->setPosition(0.0f, 0.0f, -5000000.0f);
                    (*m_pLevel->getEnemies())[0]->setVisible(false);
                }
            }
            break;
        case 70:
            if (static_cast<RadioMessage *>((*messages)[0])->isTriggered() && m_nState == 0) {
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                m_pCamera->setLookAtCam(true);
                m_pCamera->setActive(true);
                m_pCamera->setTarget((*m_pLevel->getEnemies())[0]->geometry);
                Vector position = (*m_pLevel->getEnemies())[0]->getPosition();
                position += (*m_pLevel->getEnemies())[0]->geometry->getDirection() * 3000.0f;
                position.x += -600.0f;
                position.y += -300.0f;
                position.z += -1000.0f;
                m_pCamera->setPosition(position);
                m_pLevel->lodManager->forceUpdate(delta, false);
                player->freeze = 1;
                m_nState = 1;
            } else if (m_nState == 1) {
                m_pLevel->lodManager->forceUpdate(delta, false);
                if (static_cast<RadioMessage *>((*messages)[1])->isOver()) {
                    (*m_pLevel->getEnemies())[0]->player->setAlwaysFriend(false);
                    (*m_pLevel->getEnemies())[0]->player->setAlwaysEnemy(true);
                    *reinterpret_cast<int *>(reinterpret_cast<char *>((*m_pLevel->getEnemies())[0]) + 296) = 100000;
                    (*m_pLevel->getEnemies())[0]->field_0x38 = 0;
                    *(reinterpret_cast<uint8_t *>((*m_pLevel->getEnemies())[0]) + 301) = 0;
                    *(reinterpret_cast<uint8_t *>((*m_pLevel->getEnemies())[0]) + 300) = 0;
                    (*m_pLevel->getEnemies())[0]->setRoute(nullptr);
                    Vector position = (*m_pLevel->getEnemies())[0]->getPosition();
                    position += (*m_pLevel->getEnemies())[0]->geometry->getDirection() * 3000.0f;
                    position.x += -600.0f;
                    position.y += 800.0f;
                    position.z += -1000.0f;
                    m_pCamera->setPosition(position);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    m_nState = 2;
                    player->freeze = 0;
                }
            } else if (m_nState == 2) {
                m_pLevel->lodManager->forceUpdate(delta, false);
                if (static_cast<RadioMessage *>((*messages)[2])->isOver()) {
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    m_nState = 3;
                    Player *target = (*m_pLevel->getEnemies())[0]->player;
                    int hitpoints = (*m_pLevel->getEnemies())[0]->player->getMaxHitpoints();
                    target->setMaxHitpoints(3 * hitpoints);
                    (*m_pLevel->getEnemies())[0]->setSpeed(10.0f);
                    static_cast<PlayerFighter *>((*m_pLevel->getEnemies())[0])->setRotate(5);
                }
            }
            break;
        case 73:
            switch (m_nState) {
            case 0:
                if ((int)AbyssEngine::AEMath::VectorLength(player->getPosition() - (*enemies)[8]->getPosition()) <= 49999)
                    m_nState = 1;
                break;
            case 1:
                for (unsigned i = 8; i < enemies->size_; ++i) {
                    if ((*enemies)[i]->field_0x40b) {
                        int points = (*enemies)[i]->player->getEmpPoints();
                        if (points < (*enemies)[i]->player->getMaxEmpPoints()) {
                            m_nState = 2;
                            break;
                        }
                    }
                }
                break;
            case 2:
                for (unsigned i = 8; i < enemies->size_; ++i) {
                    if ((*enemies)[i]->field_0x40b && (*enemies)[i]->player->empDisabledByte) {
                        for (unsigned j = 8; j < enemies->size_; ++j) {
                            if ((*enemies)[j]->field_0x40b) {
                                (*enemies)[j]->field_0x24 = 1;
                                static_cast<PlayerFixedObject *>((*enemies)[j])->setMoving(false);
                                break;
                            }
                        }
                        m_nState = 3;
                        break;
                    }
                }
                break;
            case 3:
                if (static_cast<RadioMessage *>((*messages)[7])->isTriggered()) {
                    Vector position = player->getPosition();
                    for (unsigned i = 4; i != 8; ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        int x = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 35000;
                        int xSign = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                        int y = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000);
                        int z = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 35000;
                        int zSign = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                        enemy->setPosition(position + Vector{(float)(xSign * x), (float)(y - 5000), (float)(zSign * z)});
                        (*enemies)[i]->awake();
                    }
                    m_nState = 4;
                }
                break;
            }
            break;
        case 80: {
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[0])->isOver()) {
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                player->setComputerControlled(true);
                player->freeze = 1;
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget(m_pGeometry1);
                vector_0x28 = m_pGeometry1->getPosition();
                vector_0x28 += Vector{-20000.0f, 5000.0f, -35000.0f};
                m_pCamera->setPosition(vector_0x28);
                scriptTimerA = 0;
                m_nState = 1;
                Globals::sound->play(1121, nullptr, nullptr, 0.0f);
                break;
            }
            switch (m_nState) {
            case 1:
                m_pCamera->translate((float)-delta, (float)delta, (float)(-12 * delta));
                scriptTimerA += delta;
                if (scriptTimerA >= 3001) {
                    scriptTimerA = 0;
                    m_nState = 2;
                }
                break;
            case 2:
                scriptTimerA += delta;
                m_pCamera->translate((float)-delta, (float)delta, (float)(-12 * delta));
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry1->transform))
                    ->Update(delta, true);
                if (scriptTimerA >= 2001) {
                    scriptTimerA = 0;
                    m_nState = 6;
                    m_pGeometry1->setVisible(false);
                }
                break;
            case 6: {
                m_pGeometry6->setPosition(Vector{12487.0f, -11451.0f, 5958.0f});
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget(m_pGeometry6);
                vector_0x28 = Vector{-28855.0f, -30075.0f, 63820.0f} / 1.7000000476837158203125f;
                m_pCamera->setPosition(vector_0x28);
                Vector direction = m_pGeometry6->getPosition() - vector_0x28;
                AEGeometry *cameraGeometry = new AEGeometry(Globals::Canvas);
                cameraGeometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                cameraGeometry->setPosition(vector_0x28);
                PaintCanvas *canvas = Globals::Canvas;
                unsigned current = canvas->CameraGetCurrent();
                canvas->CameraSetLocal(current, cameraGeometry->getMatrix());
                delete cameraGeometry;
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_pGeometry0->setVisible(true);
                scriptTimerA = 0;
                m_nState = 7;
                break;
            }
            case 7:
                scriptTimerA += delta;
                if (scriptTimerA >= 801) {
                    scriptTimerA = 0;
                    m_nState = 8;
                }
                break;
            case 8:
                scriptTimerA += delta;
                if (scriptTimerA >= 801) {
                    Vector position = m_pGeometry6->getPosition();
                    Vector velocity{0.0f, 0.0f, 0.0f};
                    m_pExplosion->start(position, velocity);
                    Globals::sound->play(18, nullptr, nullptr, 0.0f);
                    int handle = m_pLevel->field_58;
                    ParticleSystemManager *manager = m_pLevel->field_74;
                    manager->systemSetMatrix(handle, &m_pGeometry6->getMatrix());
                    m_pLevel->field_74->enableSystemEmit(m_pLevel->field_58, true);
                    vector_0x28 = m_pGeometry6->getPosition();
                    vector_0x28.x += 4500.0f;
                    vector_0x28.z += 1000.0f;
                    AbyssEngine::AEMath::MatrixSetTranslation(m_matrix, vector_0x28.x, vector_0x28.y, vector_0x28.z);
                    m_pLevel->field_74->systemSetMatrix(m_pLevel->field_5c, &m_matrix);
                    m_pLevel->field_74->enableSystemEmit(m_pLevel->field_5c, true);
                    scriptTimerA = 0;
                    m_nState = 9;
                }
                break;
            case 9:
                m_pExplosion->update(delta, nullptr);
                vector_0x28.x += (float)(2 * delta);
                AbyssEngine::AEMath::MatrixSetTranslation(m_matrix, vector_0x28.x, vector_0x28.y, vector_0x28.z);
                scriptTimerA += delta;
                if (scriptTimerA >= 8001) {
                    AEGeometry *station = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*m_pLevel->getLandmarks())[0]) + 320);
                    auto *transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->transform));
                    auto *timeSource = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->transform));
                    transform->Update(timeSource->animationLength, false);
                    transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->childTransform));
                    timeSource = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->childTransform));
                    transform->Update(timeSource->animationLength, false);
                    transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->parentTransform));
                    timeSource = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->parentTransform));
                    transform->Update(timeSource->animationLength, false);
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    player->freeze = 0;
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    m_pGeometry0->setVisible(false);
                    m_nState = 10;
                    cameraControlled = 0;
                }
                break;
            case 10:
                if (static_cast<RadioMessage *>((*messages)[9])->isOver()) {
                    player->setTurretMode(false);
                    resetCamera(m_pLevel);
                    player->setFreeLookMode(false);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    player->stopShooting(0);
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    player->setComputerControlled(true);
                    player->freeze = 1;
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->setTarget(m_pGeometry1);
                    vector_0x28 = m_pGeometry1->getPosition();
                    vector_0x28 += Vector{20000.0f, 5000.0f, -20000.0f};
                    m_pCamera->setPosition(vector_0x28);
                    scriptTimerA = 0;
                    m_nState = 11;
                }
                break;
            case 11:
                scriptTimerA += delta;
                m_pCamera->translate((float)delta * 0.5f, (float)delta * 0.2f, (float)-delta * 4.0f);
                if (scriptTimerA >= 4001) {
                    vector_0x28 = m_pGeometry1->getPosition();
                    vector_0x28 += Vector{-4000.0f, 5000.0f, -6000.0f};
                    Vector velocity{0.0f, 0.0f, 0.0f};
                    m_pExplosion->start(vector_0x28, velocity);
                    scriptTimerA = 0;
                    m_nState = 12;
                }
                break;
            case 12: {
                scriptTimerA += delta;
                m_pExplosion->update(delta, nullptr);
                m_pCamera->translate((float)delta * 0.5f, (float)delta * 0.2f, (float)-delta * 4.0f);
                float rumble = 1.0f;
                if ((float)scriptTimerA / 8000.0f < 1.0f)
                    rumble = (float)scriptTimerA / 8000.0f;
                m_pCamera->setRumblePercentage(rumble, 100);
                if (scriptTimerA >= 8001) {
                    m_pGeometry5->setPosition(m_pGeometry1->getPosition() + Vector{0.0f, 0.0f, -10000.0f});
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                        ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                        ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                    Globals::sound->play(160, nullptr, nullptr, 0.0f);
                    scriptTimerA = 0;
                    m_nState = 13;
                }
                break;
            }
            case 13: {
                m_pCamera->translate((float)delta * 0.5f, (float)delta * 0.2f, (float)-delta * 4.0f);
                scriptTimerA += delta;
                PaintCanvas *canvas = Globals::Canvas;
                Matrix *local = static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                m_pGeometry5->setDirection(AbyssEngine::AEMath::MatrixGetDir(*local), Vector{0.0f, 1.0f, 0.0f});
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry5->transform))
                    ->Update(delta, false);
                float rumble = 1.0f;
                if ((float)scriptTimerA / 10000.0f < 1.0f)
                    rumble = (float)scriptTimerA / 10000.0f;
                m_pCamera->setRumblePercentage(1.0f - rumble, 100);
                if (scriptTimerA >= 2151) {
                    AEGeometry *station = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[0]) + 320);
                    station->setVisible(false);
                    if (scriptTimerA >= 10001) {
                        m_pCamera->setRumblePercentage(0.0f, 0);
                        m_pCamera->setLookAtCam(false);
                        m_pCamera->setTarget(player->geometry);
                        player->setComputerControlled(false);
                        player->freeze = 0;
                        m_pHud->visible = 1;
                        m_pRadar->enabled = 1;
                        resetCamera(m_pLevel);
                        m_pLevel->lodManager->forceUpdate(delta, false);
                        cameraControlled = 0;
                        scriptTimerA = 0;
                        m_nState = 14;
                    }
                }
                break;
            }
            }
            if ((unsigned)(m_nState - 7) <= 2) {
                m_pCamera->translate((float)delta * 0.5f, (float)delta * 0.2f, (float)-delta * 0.3f);
                if (m_nState >= 8) {
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry0->transform))
                        ->Update(delta, false);
                    AEGeometry *station = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*m_pLevel->getLandmarks())[0]) + 320);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->transform))->Update(delta, true);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->childTransform))->Update(delta, true);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(station->parentTransform))->Update(delta, true);
                }
            }
            break;
        }
        case 91: {
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[3])->isTriggered()) {
                scriptTimerB = 0;
                m_nState = 1;
                (*enemies)[0]->setActive(true);
                auto *target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(m_pLevel->getDockingTarget(0)));
                String *text = static_cast<GameText *>(Globals::gameText)->getText(3211);
                target->setName(String(*text, false));
                goto mission91Animation;
            }
            if (m_nState == 1 && player->isDockedToDockingPoint()) {
                m_nState = 2;
                goto mission91Animation;
            }
            if (m_nState == 2 && static_cast<RadioMessage *>((*messages)[5])->isOver()) {
                auto *target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(m_pLevel->getDockingTarget(0)));
                target->setDockingType(2);
                if (player->isDockedToDockingPoint())
                    player->setDockingState(2);
                m_nState = 3;
                goto mission91Animation;
            }
            if (m_nState == 4) {
                if (static_cast<RadioMessage *>((*messages)[6])->isOver()
                    && !static_cast<PlayerEgo *>(m_pLevel->getPlayer())->isDockedToDockingPoint()) {
                    scriptTimerA = 0;
                    m_nState = 5;
                    goto mission91Animation;
                }
            } else if (m_nState == 3 && Globals::status->field_178 >= 10) {
                m_nState = 4;
                delete m_pLevel->objectivesB;
                m_pLevel->objectivesB = nullptr;
                goto mission91Animation;
            }
            if (m_nState == 6) {
                scriptTimerA += delta;
                if (scriptTimerA >= 2001) {
                    if (player->getSpeed() < 100.0f)
                        player->setSpeed(player->getSpeed() * 1.0499999523162841796875f);
                    if (scriptTimerA >= 8001) {
                        Globals::status->nextCampaignMission(true);
                        Globals::status->setStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(113));
                        Globals::status->departStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(113));
                        Level::initStreamOutPosition = 1;
                        Globals::status->field_64 = static_cast<Player *>(player->player)->getHitpoints();
                        Globals::status->field_5c = static_cast<Player *>(player->player)->getShieldHP();
                        Globals::status->field_60 = static_cast<Player *>(player->player)->getArmorHP();
                        Globals::status->field_68 = static_cast<Player *>(player->player)->getGammaHP();
                        static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
                    }
                }
            } else if (m_nState == 5) {
                scriptTimerA += delta;
                if (scriptTimerA >= 3001) {
                    scriptTimerA = 0;
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->setAutoPilot(nullptr);
                    m_pRadar->lockedEnemy = nullptr;
                    m_pRadar->candidateEnemy = nullptr;
                    if (player->isInRocketControl()) {
                        resetCamera(m_pLevel);
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    player->setTurretMode(false);
                    resetCamera(m_pLevel);
                    player->setFreeLookMode(false);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    player->stopShooting(0);
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    player->setComputerControlled(true);
                    player->setCollide(false);
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->setTarget(player->geometry);
                    vector_0x28 = player->geometry->getPosition();
                    AEGeometry *geometry = player->geometry;
                    Vector playerPosition = static_cast<PlayerEgo *>(m_pLevel->getPlayer())->getPosition();
                    auto *target = reinterpret_cast<KIPlayer *>(static_cast<intptr_t>(m_pLevel->getDockingTarget(0)));
                    Vector targetPosition = target->geometry->getPosition();
                    geometry->setDirection(VectorNormalize(playerPosition - targetPosition), Vector{0.0f, 1.0f, 0.0f});
                    vector_0x40 = player->geometry->getDirection();
                    vector_0x28 += vector_0x40 * 10000.0f;
                    vector_0x40 = player->geometry->getRightVector();
                    vector_0x28 += vector_0x40 * 700.0f;
                    m_pCamera->setPosition(vector_0x28);
                    (*enemies)[0]->player->setHitpoints(0);
                    m_nState = 6;
                }
            }
mission91Animation:
            {
                Vector position = (*enemies)[0]->geometry->getPosition();
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(2281, &position, &velocity, false);
                if (Globals::status->field_178 <= 7 || scriptTime > 179999) {
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform((*enemies)[0]->geometry->childTransform))
                        ->Update(delta, true);
                } else {
                    if (scriptTimerB == 0) {
                        Globals::sound->play(2282, nullptr, nullptr, 0.0f);
                        scriptTimerB = 1;
                    }
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform((*enemies)[0]->geometry->transform))
                        ->Update(delta, true);
                }
            }
            break;
        }
        case 92:
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[2])->isTriggered())
                ++m_nState;
            switch (m_nState) {
            case 1:
                if (Globals::status->field_178 == 0) {
                    reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->setType(4);
                    scriptTimerA = 0;
                    KIPlayer *enemy = (*enemies)[0];
                    enemy->setPosition((*enemies)[3]->getPosition() + Vector{-76000.0f, 5000.0f, 5000.0f});
                    enemy = (*enemies)[1];
                    enemy->setPosition((*enemies)[3]->getPosition() + Vector{-79000.0f, 4900.0f, 7500.0f});
                    enemy = (*enemies)[2];
                    enemy->setPosition((*enemies)[3]->getPosition() + Vector{-79000.0f, 4900.0f, 2500.0f});
                    for (unsigned i = 0; i != 3; ++i) {
                        (*enemies)[i]->geometry->setDirection(Vector{1.0f, 0.0f, 0.0f}, Vector{0.0f, 1.0f, 0.0f});
                        enemy = (*enemies)[i];
                        *reinterpret_cast<int *>(reinterpret_cast<char *>(enemy) + 296) = 0;
                        enemy->field_0x38 = 3;
                        enemy->awake();
                        (*enemies)[i]->setVisible(true);
                    }
                    savedTurretMode = player->isInTurretMode();
                    player->setTurretMode(false);
                    resetCamera(m_pLevel);
                    if (player->isInRocketControl()) {
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    player->setFreeLookMode(false);
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    player->stopShooting(0);
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    m_pCamera->setTarget((*enemies)[0]->geometry);
                    vector_0x28 = (*enemies)[0]->geometry->getPosition();
                    vector_0x40 = (*enemies)[0]->geometry->getDirection();
                    vector_0x28 += vector_0x40 * 7000.0f;
                    vector_0x40 = (*enemies)[0]->geometry->getRightVector();
                    vector_0x28 += vector_0x40 * 700.0f;
                    vector_0x40 = (*enemies)[0]->geometry->getUpVector();
                    vector_0x28 += vector_0x40 * 300.0f;
                    m_pCamera->setPosition(vector_0x28);
                    auto *target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(m_pLevel->getDockingTarget(0)));
                    target->setDockingType(0);
                    target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(m_pLevel->getDockingTarget(0)));
                    target->setName(String("", false));
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 2:
                for (unsigned i = 0; i != 3; ++i) {
                    float speed = i == 0 ? 2.0f : i == 1 ? 2.0799999237060546875f : 2.0499999523162841796875f;
                    (*enemies)[i]->geometry->moveForward(speed * (float)delta);
                }
                if (static_cast<RadioMessage *>((*messages)[5])->isTriggered()) {
                    scriptTimerA = 0;
                    scriptTimerB = 0;
                    ++m_nState;
                    m_pCamera->setTarget((*enemies)[0]->geometry);
                    vector_0x28 = (*enemies)[0]->geometry->getPosition();
                    vector_0x40 = (*enemies)[0]->geometry->getDirection();
                    vector_0x28 += vector_0x40 * (float)(scriptTimerB + 3900);
                    vector_0x40 = (*enemies)[0]->geometry->getRightVector();
                    vector_0x28 += vector_0x40 * (float)(scriptTimerB + 500);
                    vector_0x40 = (*enemies)[0]->geometry->getUpVector();
                    vector_0x28 += vector_0x40 * ((float)scriptTimerB * 0.0040000001899898052215576171875f + 400.0f);
                    m_pCamera->setPosition(vector_0x28);
                }
                break;
            case 3: {
                float advance = (float)delta * 0.0500000007450580596923828125f;
                scriptTimerB = (long long)(advance + (float)scriptTimerB);
                m_pCamera->setTarget((*enemies)[0]->geometry);
                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                vector_0x40 = (*enemies)[0]->geometry->getDirection();
                vector_0x28 += vector_0x40 * ((float)delta * 1.7000000476837158203125f);
                vector_0x40 = (*enemies)[0]->geometry->getRightVector();
                vector_0x28 += vector_0x40 * ((float)-delta * 0.02999999932944774627685546875f);
                vector_0x40 = (*enemies)[0]->geometry->getUpVector();
                vector_0x28 += vector_0x40 * (advance * 0.0040000001899898052215576171875f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                for (unsigned i = 0; i != 3; ++i) {
                    float speed = i == 0 ? 2.0f : i == 1 ? 2.2999999523162841796875f : 2.2000000476837158203125f;
                    (*enemies)[i]->geometry->moveForward(speed * (float)delta);
                }
                if (static_cast<RadioMessage *>((*messages)[5])->isOver()) {
                    for (unsigned i = 0; i != 3; ++i) {
                        static_cast<PlayerFighter *>((*enemies)[i])->setCloakingPossible(true);
                        static_cast<PlayerFighter *>((*enemies)[i])->cloak(18000, false);
                    }
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            }
            case 4: {
                float advance = (float)delta * 0.0500000007450580596923828125f;
                scriptTimerA += delta;
                scriptTimerB = (long long)(advance + (float)scriptTimerB);

                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                vector_0x40 = (*enemies)[0]->geometry->getDirection();
                vector_0x28 += vector_0x40 * ((float)delta * 1.7000000476837158203125f);
                vector_0x40 = (*enemies)[0]->geometry->getRightVector();
                vector_0x28 += vector_0x40 * ((float)-delta * 0.02999999932944774627685546875f);
                vector_0x40 = (*enemies)[0]->geometry->getUpVector();
                vector_0x28 += vector_0x40 * (advance * 0.0040000001899898052215576171875f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                for (unsigned i = 0; i != 3; ++i) {
                    float speed = i == 0 ? 2.0f : i == 1 ? 2.2999999523162841796875f : 2.2000000476837158203125f;
                    (*enemies)[i]->geometry->moveForward(speed * (float)delta);
                }
                if (scriptTimerA > 2000) {
                    ++m_nState;
                    scriptTimerA = 0;
                }
                break;
            }
            case 5:
                scriptTimerA += delta;
                for (unsigned i = 0; i != 3; ++i) {
                    float speed = i == 0 ? 6.0f : i == 1 ? 4.599999904632568359375f : 4.400000095367431640625f;
                    (*enemies)[i]->geometry->moveForward(speed * (float)delta);
                }
                if (scriptTimerA >= 2001) {
                    for (unsigned i = 0; i != 3; ++i) {
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(false);
                        *reinterpret_cast<int *>(reinterpret_cast<char *>((*enemies)[i]) + 296) = 50000;
                        (*enemies)[i]->field_0x38 = 4;
                    }
                    m_pCamera->setFixed(false);
                    m_pCamera->setTarget(player->geometry);
                    bool docked = player->isDockedToDockingPoint();
                    m_pCamera->setLookAtCam(false);
                    if (docked) {
                        lookBehind();
                        m_pCamera->setRotationAroundTarget(true);
                        player->setFreeLookMode(true);
                        if (savedTurretMode)
                            player->setTurretMode(true);
                    } else {
                        m_pCamera->setPosition(vector_0x40);
                    }
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 6:
                scriptTimerA += delta;
                if (scriptTimerA >= 3001)
                    ++m_nState;
                break;
            case 7:
                if (!static_cast<RadioMessage *>((*messages)[8])->isTriggered())
                    break;
                if (player->isDockedToDockingPoint()) {
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->setAutoPilot(nullptr);
                    m_pRadar->lockedEnemy = nullptr;
                    m_pRadar->candidateEnemy = nullptr;
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->resetGunDelay();
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->dockToDockingPoint(nullptr, m_pRadar);
                    player->setDockingState(3);
                    m_pCamera->setRotationAroundTarget(false);
                }
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                if (player->isInRocketControl()) {
                    player->setRocketControl(nullptr, nullptr);
                    player->killLiberator();
                    m_pCamera->setRumblePercentage(0.0f, 0);
                }
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                static_cast<Player *>(player->player)->setVulnerable(false);
                player->setComputerControlled(true);
                player->setCollide(false);
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget(player->geometry);
                vector_0x28 = player->geometry->getPosition();
                vector_0x40 = player->geometry->getDirection();
                vector_0x28 += vector_0x40 * 7000.0f;
                vector_0x40 = player->geometry->getRightVector();
                vector_0x28 += vector_0x40 * 700.0f;
                m_pCamera->setPosition(vector_0x28);
                scriptTimerA = 0;
                ++m_nState;
                break;
            case 8:
                scriptTimerA += delta;
                (*enemies)[3]->geometry->moveForward((float)(2 * delta));
                if (scriptTimerA >= 8001) {
                    m_pCamera->setTarget((*enemies)[3]->geometry);
                    vector_0x28 = (*enemies)[3]->getPosition();
                    vector_0x40 = (*enemies)[3]->geometry->getDirection();
                    vector_0x28 += vector_0x40 * 10000.0f;
                    vector_0x40 = (*enemies)[3]->geometry->getRightVector();
                    vector_0x28 += vector_0x40 * 4800.0f;
                    m_pCamera->setPosition(vector_0x28);
                    ++m_nState;
                    scriptTimerA = 0;
                    vector_0x28 = Vector{1.0f, 1.0f, 1.0f};
                }
                break;
            case 9:
            case 10: {
                Vector position = (*enemies)[3]->geometry->getPosition();
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(2249, &position, &velocity, false);
                scriptTimerA += delta;
                vector_0x28.x += (float)delta * 0.006000000052154064178466796875f;
                (*enemies)[3]->geometry->moveForward(vector_0x28.x * (float)delta);
                if (scriptTimerA >= 2001) {
                    if (m_nState == 9)
                        m_nState = 10;
                    if (scriptTimerA >= 7001) {
                        m_pCamera->setFixed(false);
                        m_pCamera->setTarget(player->geometry);
                        m_pCamera->setLookAtCam(false);
                        Vector playerPosition = player->getPosition();
                        Vector direction = player->GetDirVector();
                        m_pCamera->setPosition(playerPosition - direction * 2400.0f);
                        m_pHud->visible = 1;
                        m_pRadar->enabled = 1;
                        resetCamera(m_pLevel);
                        m_pLevel->lodManager->forceUpdate(delta, false);
                        player->setComputerControlled(false);
                        cameraControlled = 0;
                        static_cast<Player *>(player->player)->setVulnerable(true);
                        (*enemies)[3]->setActive(false);
                        (*enemies)[3]->setVisible(false);
                        ++m_nState;
                    }
                }
                break;
            }
            case 11:
                vector_0x28.x += (float)delta * 0.0199999995529651641845703125f;
                (*enemies)[3]->geometry->moveForward(vector_0x28.x * (float)delta);
                break;
            }
            break;
        case 94:
            switch (m_nState) {
            case 0:
                if (Globals::status->field_178 > 6 || scriptTime >= 60001) {
                    int x = 50000;
                    for (unsigned i = 0; i != 2; ++i, x += 3000) {
                        (*enemies)[i]->setPosition((float)x, 1000.0f, 140000.0f);
                        (*enemies)[i]->awake();
                        (*enemies)[i]->setVisible(true);
                        (*enemies)[i]->geometry->setDirection(Vector{0.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                    }
                    savedTurretMode = player->isInTurretMode();
                    player->setTurretMode(false);
                    resetCamera(m_pLevel);
                    if (player->isInRocketControl()) {
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    player->setFreeLookMode(false);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    player->stopShooting(0);
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->setTarget((*enemies)[0]->geometry);
                    vector_0x28 = (*enemies)[0]->geometry->getPosition();
                    vector_0x40 = (*enemies)[0]->geometry->getDirection();
                    vector_0x28 += vector_0x40 * 8000.0f;
                    vector_0x40 = (*enemies)[0]->geometry->getRightVector();
                    vector_0x28 += vector_0x40 * 1200.0f;
                    vector_0x40 = (*enemies)[0]->geometry->getUpVector();
                    vector_0x28 += vector_0x40 * -300.0f;
                    m_pCamera->setPosition(vector_0x28);
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 1:
                scriptTimerA += delta;
                for (unsigned i = 0; i != 2; ++i)
                    (*enemies)[i]->geometry->moveForward((float)(3 * delta));
                if (scriptTimerA >= 6001) {
                    for (unsigned i = 0; i != 2; ++i)
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(false);
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->resetGunDelay();
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    bool docked = player->isDockedToDockingPoint();
                    m_pCamera->setLookAtCam(false);
                    if (docked) {
                        lookBehind();
                        m_pCamera->setRotationAroundTarget(1);
                        player->setFreeLookMode(true);
                        if (savedTurretMode)
                            player->setTurretMode(true);
                    } else {
                        m_pCamera->setPosition(vector_0x40);
                    }
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    scriptTimerA = 0;
                    Globals::sound->stop(Globals::sound->currentMusicEvent);
                    ++m_nState;
                }
                break;
            case 2:
                scriptTimerA += delta;
                if (scriptTimerA > 100000) {
                    scriptTimerA = 0;
                    int x = -40000;
                    for (unsigned i = 2; i != 4; ++i, x -= 3000) {
                        KIPlayer *enemy = (*enemies)[i];
                        enemy->setPosition(player->getPosition() + Vector{(float)x, 3000.0f, 20000.0f});
                        (*enemies)[i]->awake();
                        (*enemies)[i]->setVisible(true);
                    }
                    ++m_nState;
                }
                break;
            case 3:
                scriptTimerA += delta;
                if (scriptTimerA > 100000) {
                    scriptTimerA = 0;
                    int x = -40000;
                    for (unsigned i = 4; i != 6; ++i, x -= 3000) {
                        KIPlayer *enemy = (*enemies)[i];
                        enemy->setPosition(player->getPosition() + Vector{(float)x, 3000.0f, 20000.0f});
                        (*enemies)[i]->awake();
                        (*enemies)[i]->setVisible(true);
                    }
                    ++m_nState;
                }
                break;
            }
            break;
        case 102:
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[0])->isTriggered()) {
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                if (player->isInRocketControl()) {
                    player->setRocketControl(nullptr, nullptr);
                    player->killLiberator();
                    m_pCamera->setRumblePercentage(0.0f, 0);
                }
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                player->setFreeze(true);
                player->setVisible(false);
                static_cast<Player *>(player->player)->setVulnerable(false);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget((*enemies)[0]->geometry);
                m_pCamera->setPosition((*enemies)[0]->getPosition() + Vector{9000.0f, -7000.0f, 40000.0f});
                Globals::sound->stop(Globals::sound->currentMusicEvent);
                Globals::sound->play(2240, nullptr, nullptr, 0.0f);
                scriptTimerA = 0;
                ++m_nState;
                break;
            }
            switch (m_nState) {
            case 1:
                m_pCamera->translate((float)delta * 0.6f, (float)delta * 0.8f, (float)-delta * 2.5f);
                if (static_cast<RadioMessage *>((*messages)[2])->isOver())
                    scriptTimerA += delta;
                if (scriptTimerA >= 2001) {
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->resetGunDelay();
                    player->setFreeze(false);
                    player->setVisible(true);
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    Vector position = player->getPosition();
                    Vector direction = player->GetDirVector();
                    m_pCamera->setPosition(position - direction * 2400.0f);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    scriptTimerA = 0;
                    Globals::sound->stop(Globals::sound->currentMusicEvent);
                    Globals::sound->play(2241, nullptr, nullptr, 0.0f);
                    ++m_nState;
                }
                break;
            case 2:
                scriptTimerA += delta;
                if (scriptTimerA >= 30001) {
                    Vector position = player->getPosition();
                    int first = -1;
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (enemy->shipGroup == 10) {
                            enemy->awake();
                            if (first == -1)
                                first = (int)i;
                            if ((int)i == first) {
                                enemy = (*enemies)[i];
                                Vector origin = (*enemies)[0]->getPosition();
                                enemy->setPosition(origin + Vector{-82000.0f, 2000.0f, 0.0f});
                                static_cast<PlayerFighter *>((*enemies)[i])->cloak(1000, true);
                            } else if ((int)i == first + 1) {
                                enemy = (*enemies)[i];
                                Vector origin = (*enemies)[first]->getPosition();
                                enemy->setPosition(origin + Vector{-2000.0f, -800.0f, -2000.0f});
                                static_cast<PlayerFighter *>((*enemies)[i])->cloak(1000, true);
                            } else {
                                int x = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 35000;
                                int signX = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                                int y = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000);
                                int z = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 35000;
                                int signZ = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                                enemy->setPosition(position + Vector{(float)(signX * x), (float)(y - 5000), (float)(signZ * z)});
                            }
                            (*enemies)[i]->player->setEnemies(nullptr);
                            int target = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(5);
                            Player *targetPlayer = target == 4 ? static_cast<Player *>(player->player) : (*enemies)[target + 2]->player;
                            (*enemies)[i]->player->setEnemy(targetPlayer);
                            (*enemies)[i]->awake();
                            (*enemies)[i]->geometry->setDirection(Vector{1.0f, 0.0f, 0.0f}, Vector{0.0f, 1.0f, 0.0f});
                            static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                        } else if (enemy->shipGroupFlag == 51) {
                            Array<Player *> *targets = new Array<Player *>;
                            ArrayAdd((*enemies)[0]->player, *targets);
                            ArrayAdd((*enemies)[1]->player, *targets);
                            (*enemies)[i]->player->setEnemies(targets);
                        }
                    }
                    player->setTurretMode(false);
                    player->setFreeze(true);
                    player->setVisible(false);
                    resetCamera(m_pLevel);
                    if (player->isInRocketControl()) {
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    player->setFreeLookMode(false);
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    player->stopShooting(0);
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    AEGeometry *geometry = (*enemies)[first]->geometry;
                    m_pCamera->setTarget(geometry);
                    vector_0x28 = geometry->getPosition();
                    vector_0x40 = geometry->getDirection();
                    vector_0x28 += vector_0x40 * 4500.0f;
                    vector_0x40 = geometry->getRightVector();
                    vector_0x28 += vector_0x40 * 600.0f;
                    vector_0x40 = geometry->getUpVector();
                    vector_0x28 += vector_0x40 * 400.0f;
                    m_pCamera->setPosition(vector_0x28);
                    scriptTimerA = 0;
                    m_nState = 3;
                }
                break;
            case 3: {
                float advance = (float)delta * 0.05f;
                scriptTimerA += delta;
                scriptTimerB = (long long)(advance + (float)scriptTimerB);
                int first = -1;
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    KIPlayer *enemy = (*enemies)[i];
                    if (enemy->shipGroup == 10) {
                        if (first == -1)
                            first = (int)i;
                        enemy->geometry->moveForward((float)((double)delta * ((int)i == first ? 2.3 : 2.2)));
                    }
                }
                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                AEGeometry *geometry = (*enemies)[first]->geometry;
                vector_0x40 = geometry->getDirection();
                vector_0x28 += vector_0x40 * ((float)delta * 2.04f);
                vector_0x40 = geometry->getRightVector();
                vector_0x28 += vector_0x40 * ((float)delta * 0.04f);
                vector_0x40 = geometry->getUpVector();
                vector_0x28 += vector_0x40 * (advance * 0.004f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                if (scriptTimerA >= 10001) {
                    for (unsigned i = 0; i < enemies->size_; ++i)
                        if ((*enemies)[i]->shipGroup == 10)
                            static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(false);
                    player->setFreeze(false);
                    player->setVisible(true);
                    m_pCamera->setFixed(false);
                    m_pCamera->setTarget(player->geometry);
                    m_pCamera->setLookAtCam(false);
                    Vector position = player->getPosition();
                    Vector direction = player->GetDirVector();
                    m_pCamera->setPosition(position - direction * 2400.0f);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    ++m_nState;
                    scriptTimerA = 0;
                }
                break;
            }
            case 4:
                scriptTimerA += delta;
                if (scriptTimerA > 60000) {
                    Vector position = player->getPosition();
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (enemy->shipGroup == 10 && enemy->isDead()) {
                            (*enemies)[i]->revive();
                            (*enemies)[i]->awake();
                            enemy = (*enemies)[i];
                            int x = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 35000;
                            int signX = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                            int y = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000);
                            int z = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(10000) + 35000;
                            int signZ = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2) == 0 ? 1 : -1;
                            enemy->setPosition(position + Vector{(float)(signX * x), (float)(y - 5000), (float)(signZ * z)});
                        }
                    }
                    scriptTimerA = 0;
                }
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    KIPlayer *enemy = (*enemies)[i];
                    if (enemy->shipGroup == 10 && enemy->player->getEnemy(0)->isDead()) {
                        int target = -1;
                        for (int j = 2; j <= 5 && target == -1; ++j)
                            if (!(*enemies)[j]->isDead())
                                target = j;
                        enemy->player->setEnemy(target == -1 ? static_cast<Player *>(player->player) : (*enemies)[target]->player);
                    }
                }
                if (reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getStatusValue() < 10) {
                    reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->setType(4);
                    ++m_nState;
                    player->setTurretMode(false);
                    resetCamera(m_pLevel);
                    if (player->isInRocketControl()) {
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    player->setFreeLookMode(false);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    player->stopShooting(0);
                    player->setFreeze(true);
                    player->setVisible(false);
                    static_cast<Player *>(player->player)->setVulnerable(false);
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->setTarget((*enemies)[0]->geometry);
                    m_pCamera->setPosition((*enemies)[0]->getPosition() + Vector{9000.0f, -7000.0f, 40000.0f});
                    scriptTimerA = 0;
                    for (unsigned i = 2; i != 10; ++i) {
                        (*enemies)[i]->setPosition(500000.0f, 500000.0f, 500000.0f);
                        (*enemies)[i]->setActive(false);
                        (*enemies)[i]->setVisible(false);
                    }
                }
                break;
            case 5: {
                (*enemies)[0]->geometry->moveForward((float)(6 * delta));
                m_pCamera->translate((float)delta * 0.4f, (float)delta * 0.1f, (float)-delta * 1.8f);
                scriptTimerA += delta;
                Vector position = (*enemies)[0]->geometry->getPosition();
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(2250, &position, &velocity, false);
                if (scriptTimerA >= 8001) {
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            }
            case 6: {
                Vector position = (*enemies)[0]->geometry->getPosition();
                Vector velocity{0.0f, 0.0f, 0.0f};
                Globals::sound->updateEvent3DAttributes(2250, &position, &velocity, false);
                scriptTimerA += delta;
                (*enemies)[0]->geometry->moveForward((float)(200 * delta));
                if (scriptTimerA >= 4001) {
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->resetGunDelay();
                    player->setFreeze(false);
                    player->setVisible(true);
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    m_pCamera->setPosition(vector_0x40);
                    position = player->getPosition();
                    Vector direction = player->GetDirVector();
                    m_pCamera->setPosition(position - direction * 2400.0f);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            }
            }
            break;
        case 114:
            if (m_nState == 0) {
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    KIPlayer *enemy = (*enemies)[i];
                    if (enemy->shipGroup == 8 && enemy->player->isActive()) {
                        for (unsigned j = 0; j < enemies->size_; ++j) {
                            KIPlayer *other = (*enemies)[j];
                            if (other->shipGroup == 8) {
                                *reinterpret_cast<int *>(reinterpret_cast<char *>(other) + 296) = 50000;
                                other->awake();
                            }
                        }
                        ++m_nState;
                        break;
                    }
                }
            }
            break;
        case 125:
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[1])->isTriggered()) {
                ++m_nState;
                int coordinates[3] = {-70000, 0, -130000};
                Route *route = new Route(coordinates, 3);
                m_pLevel->setPlayerRoute(route);
                player->setRoute(route);
                scriptTimerA = 0;
            } else if (m_nState == 1 && player->getRoute()) {
                if (*reinterpret_cast<uint8_t *>(reinterpret_cast<char *>(player->getRoute()->getWaypoint(0)) + 304)) {
                    m_nState = 2;
                    for (unsigned i = 8; i <= 10; ++i) {
                        auto *target = static_cast<PlayerFixedObject *>((*enemies)[i]);
                        String *text = static_cast<GameText *>(Globals::gameText)->getText(3212);
                        target->setName(*text + String(" ", false) + String((int)i - 7));
                        (*enemies)[i]->setActive(true);
                        *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>((*enemies)[i]) + 116) = 0;
                    }
                }
            }
            if (m_nState == 2 && static_cast<RadioMessage *>((*messages)[2])->isOver()) {
                int offset = 0;
                for (unsigned i = 2; i != 6; ++i, offset += 2000) {
                    (*enemies)[i]->awake();
                    KIPlayer *enemy = (*enemies)[i];
                    Vector position = player->getPosition();
                    int signX = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(100);
                    int y = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(1000);
                    int z = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2000);
                    int signZ = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(100) < 50 ? 1 : -1;
                    Vector displacement{(float)((signX < 50 ? 1 : -1) * offset + 2000),
                                        (float)(y + 2000), (float)(-57000 - signZ * z)};
                    enemy->setPosition(position + displacement);
                    Vector playerPosition = player->getPosition();
                    Vector enemyPosition = (*enemies)[i]->getPosition();
                    (*enemies)[i]->geometry->setDirection(VectorNormalize(playerPosition - enemyPosition),
                                                         Vector{0.0f, 1.0f, 0.0f});
                    static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                    (*enemies)[i]->player->setEnemy(static_cast<Player *>(player->player));
                }
                player->setTurretMode(false);
                resetCamera(m_pLevel);
                if (player->isInRocketControl()) {
                    player->setRocketControl(nullptr, nullptr);
                    player->killLiberator();
                    m_pCamera->setRumblePercentage(0.0f, 0);
                }
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                m_pCamera->setLookAtCam(true);
                player->setVisible(false);
                player->setFreeze(true);
                AEGeometry *geometry = (*enemies)[4]->geometry;
                m_pCamera->setTarget(geometry);
                vector_0x28 = geometry->getPosition();
                vector_0x28 += geometry->getDirection() * 4000.0f;
                vector_0x28 += geometry->getUpVector() * -500.0f;
                vector_0x28 += geometry->getRightVector() * -500.0f;
                m_pCamera->setPosition(vector_0x28.x, vector_0x28.y, vector_0x28.z);
                scriptTimerA = 0;
                ++m_nState;
            }
            if (m_nState == 3) {
                for (unsigned i = 2; i != 6; ++i)
                    (*enemies)[i]->geometry->moveForward((float)(2 * delta));
                scriptTimerA += delta;
                if (scriptTimerA >= 7001) {
                    for (unsigned i = 2; i != 6; ++i)
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(false);
                    m_nState = 4;
                    player->setVisible(true);
                    player->setFreeze(false);
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->setAutoPilot(nullptr);
                    if (!static_cast<PlayerEgo *>(m_pLevel->getPlayer())->isDockedToDockingPoint()) {
                        m_pRadar->lockedEnemy = nullptr;
                        m_pRadar->candidateEnemy = nullptr;
                    }
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->resetGunDelay();
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->dockToDockingPoint(m_pRadar->lockedEnemy, m_pRadar);
                    Vector enemyPosition = (*enemies)[4]->getPosition();
                    Vector playerPosition = player->getPosition();
                    player->geometry->setDirection(VectorNormalize(enemyPosition - playerPosition), Vector{0.0f, 1.0f, 0.0f});
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    playerPosition = player->getPosition();
                    Vector direction = player->GetDirVector();
                    m_pCamera->setPosition(playerPosition - direction * 2400.0f);
                    player->setComputerControlled(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                }
            } else if ((unsigned)(m_nState - 4) <= 2 && player->isDockedToDockingPoint()
                       && player->hackingWon() && player->getHackingGameDockIndex() >= 0) {
                auto *target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(
                    m_pLevel->getDockingTarget(player->getHackingGameDockIndex())));
                target->setDockingType(0);
                target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(
                    m_pLevel->getDockingTarget(player->getHackingGameDockIndex())));
                target->setName(String("", false));
                target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(
                    m_pLevel->getDockingTarget(player->getHackingGameDockIndex())));
                *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>(target) + 116) = 1;
                player->deleteHackingGame();
                ++m_nState;
                if (m_nState == 6) {
                    int x = 25000;
                    int y = 2300;
                    for (unsigned i = 6; i != 8; ++i, y += 50, x += 1000) {
                        (*enemies)[i]->awake();
                        KIPlayer *enemy = (*enemies)[i];
                        enemy->setPosition(player->getPosition() + Vector{(float)x, (float)y, 25000.0f});
                    }
                }
            }
            break;
        case 131:
            if (m_nState == 1 || m_nState == 2) {
                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                vector_0x40 = player->GetDirVector();
                vector_0x28 += vector_0x40 * ((float)delta * 2.01f);
                vector_0x40 = player->geometry->getRightVector();
                vector_0x28 += vector_0x40 * ((float)delta * 0.03f);
                vector_0x40 = player->GetUpVector();
                vector_0x28 += vector_0x40 * ((float)delta * 0.01f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                if (m_nState == 1) {
                    scriptTimerA += delta;
                    if (scriptTimerA >= 3001) {
                        m_nState = 2;
                        scriptTimerA = 0;
                    }
                } else if (static_cast<RadioMessage *>((*messages)[0])->isOver()) {
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    player->setCollide(true);
                    player->setComputerControlled(false);
                    player->setTurretMode(false);
                    player->setFreeLookMode(false);
                    m_pCamera->enableFirstPersonCam(false);
                    m_pCamera->setFixed(false);
                    m_pCamera->setTarget(player->geometry);
                    m_pCamera->setLookAtCam(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    scriptTimerA = 0;
                    cameraControlled = 0;
                    m_nState = 3;
                }
            }
            break;
        case 135:
            if (m_nState == 0 && (player->isDockedToDockingPoint() || scriptTime >= 60001)) {
                m_nState = 1;
                int offset = 25000;
                for (unsigned i = 0; i < enemies->size_; ++i, offset += 1000) {
                    KIPlayer *enemy = (*enemies)[i];
                    if (enemy->shipGroup == 8) {
                        enemy->setPosition(player->getPosition() + Vector{(float)offset, 5000.0f, 25000.0f});
                        (*enemies)[i]->awake();
                        (*enemies)[i]->setVisible(true);
                        (*enemies)[i]->player->setEnemy(static_cast<Player *>(player->player));
                        delete (*enemies)[i]->cargo;
                        (*enemies)[i]->cargo = nullptr;
                    }
                }
                scriptTimerA = 0;
            } else if (m_nState >= 1) {
                scriptTimerA += delta;
                if (m_nState == 1) {
                    int amount = reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getStatusValue();
                    if (amount >= reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getProductionGoodAmount() / 2)
                        ++m_nState;
                }
                if (scriptTimerA >= 75001) {
                    if (m_nState == 2)
                        m_nState = 3;
                    int offset = 25000;
                    for (unsigned i = 0; i < enemies->size_; ++i, offset += 1000) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (enemy->shipGroup == 8 && enemy->isDead()) {
                            (*enemies)[i]->revive();
                            (*enemies)[i]->setPosition(player->getPosition() + Vector{(float)offset, 5000.0f, 25000.0f});
                            (*enemies)[i]->player->setEnemy(static_cast<Player *>(player->player));
                            delete (*enemies)[i]->cargo;
                            (*enemies)[i]->cargo = nullptr;
                        }
                    }
                    scriptTimerA = 0;
                }
            }
            break;
        case 139: {
            int unlocked = 0;
            for (unsigned i = 0; i != 2; ++i)
                if (static_cast<PlayerFixedObject *>((*enemies)[i])->getDockingType() != 3)
                    ++unlocked;
            if (m_nState == 0 && player->isDockedToDockingPoint()
                && player->hackingWon() && player->getHackingGameDockIndex() >= 0) {
                auto *target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(
                    m_pLevel->getDockingTarget(player->getHackingGameDockIndex())));
                target->setName(String("", false));
                target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(
                    m_pLevel->getDockingTarget(player->getHackingGameDockIndex())));
                target->setDockingType(0);
                ++m_nState;
            }
            if (m_nState == 1) {
                if (!(*enemies)[0]->player->isAlwaysEnemy()
                    && static_cast<RadioMessage *>((*messages)[1])->isTriggered()) {
                    for (unsigned i = 0; i < m_pLevel->getEnemies()->size_; ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (!enemy->isWingMan() && !enemy->field_0x3f_b)
                            enemy->player->turnEnemy();
                    }
                }
                if (player->isDockedToDockingPoint() && player->hackingWon()
                    && player->getHackingGameDockIndex() >= 0) {
                    auto *target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(
                        m_pLevel->getDockingTarget(player->getHackingGameDockIndex())));
                    int dockingType = target->getDockingType();
                    if (unlocked == 1 && dockingType == 3) {
                        target = reinterpret_cast<PlayerFixedObject *>(static_cast<intptr_t>(
                            m_pLevel->getDockingTarget(player->getHackingGameDockIndex())));
                        target->setDockingType(2);
                        player->setDockingState(2);
                        Globals::status->field_178 = 0;
                        ++m_nState;
                    }
                }
            }
            break;
        }
        case 142:
            if (m_nState <= 1) {
                Vector position = (*enemies)[0]->getPosition();
                Route *route = (*enemies)[0]->getRoute();
                Vector waypoint = route->getWaypoint(0)->getPosition();
                if (AbyssEngine::AEMath::VectorLength(position - waypoint) < 3000.0f)
                    (*enemies)[0]->setSpeed(0.0f);
                if (m_nState == 0 && player->getRoute()
                    && *(reinterpret_cast<uint8_t *>(player->getRoute()->getWaypoint(0)) + 304)) {
                    m_nState = 1;
                    break;
                }
                if (m_nState == 1 && m_pLevel->getGasClouds()) {
                    for (unsigned i = 0; i < m_pLevel->getGasClouds()->size_; ++i) {
                        auto *cloud = static_cast<PlayerGasCloud *>((*m_pLevel->getGasClouds())[i]);
                        if (cloud->getSparks())
                            m_nState = 2;
                    }
                    break;
                }
            }
            if (m_nState == 2 && (Globals::status->getShip()->hasCargo(201, 1)
                || Globals::status->getShip()->hasCargo(202, 1)
                || Globals::status->getShip()->hasCargo(203, 1)
                || Globals::status->getShip()->hasCargo(204, 1)
                || Globals::status->getShip()->hasCargo(205, 1))) {
                (*enemies)[0]->setSpeed(2.099999904632568359375f);
                m_nState = 3;
            }
            break;
        case 144:
            cameraControlled = 1;
            m_pGeometry4->moveForward((float)(5 * delta));
            m_pCamera->translate(0.0f, (float)delta * 0.05f, (float)delta * 0.28f);
            if (m_nState == 3) {
                scriptTimerA += delta;
                for (unsigned i = 1; i < enemies->size_; ++i) {
                    if ((*enemies)[i]->shipGroupFlag == 44) {
                        if (static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(100) <= 9
                            && !*(reinterpret_cast<uint8_t *>((*enemies)[i]) + 316)) {
                            static_cast<PlayerFighter *>((*enemies)[i])->setCloakingPossible(true);
                            static_cast<PlayerFighter *>((*enemies)[i])->cloak(20000, true);
                        }
                        (*enemies)[i]->geometry->moveForward((float)scriptTimerA * 0.05f);
                    }
                }
                if (scriptTimerA > 4000) {
                    (*enemies)[0]->geometry->moveForward((float)(scriptTimerA - 4000) * 0.05f);
                    if (scriptTimerA >= 7001) {
                        m_nState = 4;
                        Globals::status->nextCampaignMission(true);
                        Globals::status->setStation(Globals::status->getStation());
                        Globals::status->departStation(Globals::status->getStation());
                        if (Level::initStreamOutPositionAfterCutscene) {
                            Level::initStreamOutPositionAfterCutscene = 0;
                            Level::initStreamOutPosition = 1;
                        }
                        static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
                    }
                }
            } else if (m_nState == 1) {
                m_pLevel->lodManager->forceUpdate(delta, false);
                m_pCamera->setTarget(m_pGeometry4);
                m_nState = 2;
            } else if (m_nState == 2 && static_cast<RadioMessage *>((*messages)[3])->isOver()) {
                scriptTimerA = 0;
                m_nState = 3;
            }
            break;
        case 145: {
            for (unsigned i = 0; i != 13; ++i) {
                (*enemies)[i]->geometry->moveForward((float)(2 * delta));
                if (i && m_nState >= 2 && (m_nState != 4 || scriptTimerA <= 1000))
                    (*enemies)[i]->player->shoot(0, (long long)delta, false);
            }
            if (m_nState == 0) {
                for (unsigned i = 0; i != 13; ++i) {
                    (*enemies)[i]->setEnemies(nullptr);
                    (*enemies)[i]->player->setEnemy((*enemies)[14]->player);
                }
                m_pExplosion = new Explosion(0);
                m_pExplosion->addFireStreaks();
                m_pExplosion->setScaling(12.0f);
                m_nState = 1;
            }
            if (m_nState == 1 && static_cast<RadioMessage *>((*messages)[0])->isOver()) {
                player->setTurretMode(false);
                if (player->isInRocketControl()) {
                    player->setRocketControl(nullptr, nullptr);
                    player->killLiberator();
                    m_pCamera->setRumblePercentage(0.0f, 0);
                }
                resetCamera(m_pLevel);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                static_cast<Player *>(player->player)->setVulnerable(false);
                player->setVisible(false);
                player->setFreeze(true);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget((*m_pLevel->getEnemies())[0]->geometry);
                vector_0x28 = (*enemies)[0]->geometry->getPosition();
                vector_0x28 += (*enemies)[0]->geometry->getDirection() * 18000.0f;
                vector_0x28 += (*enemies)[0]->geometry->getRightVector() * 3000.0f;
                m_pCamera->setPosition(vector_0x28);
                (*enemies)[0]->setVisible(true);
                (*enemies)[0]->setActive(true);
                scriptTimerA = 0;
                ++m_nState;
                break;
            }
            switch (m_nState) {
            case 2:
                scriptTimerA += delta;
                if (scriptTimerA >= 1001) {
                    (*enemies)[0]->player->shoot(0, (long long)delta, false);
                    m_pGeometry4 = new AEGeometry(Globals::Canvas);
                    m_pGeometry4->setPosition((*enemies)[0]->getPosition());
                    Gun *gun = (*(*(*enemies)[0]->player->guns)[0])[0];
                    vector_0x28 = *reinterpret_cast<Vector *>(gun->upVectors);
                    m_pGeometry4->setDirection(vector_0x28, Vector{0.0f, 1.0f, 0.0f});
                    m_pGeometry4->setVisible(false);
                    m_pCamera->setTarget(m_pGeometry4);
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 3: {
                Gun *gun = (*(*(*enemies)[0]->player->guns)[0])[0];
                m_pGeometry4->moveForward(gun->field_0x50 * (float)delta);
                scriptTimerA += delta;
                if (scriptTimerA >= 5001) {
                    m_matrix = (*enemies)[14]->geometry->getMatrix();
                    m_pLevel->field_74->systemSetMatrix(m_pLevel->field_54, &m_matrix);
                    m_pLevel->field_74->enableSystemEmit(m_pLevel->field_54, true);
                    Vector position = AbyssEngine::AEMath::MatrixGetPosition(m_matrix);
                    m_pExplosion->start(position, Vector{0.0f, 0.0f, 0.0f});
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            }
            case 4:
                scriptTimerA += delta;
                m_pExplosion->update((int)((float)delta * 0.5f), static_cast<TargetFollowCamera *>(nullptr));
                if (scriptTimerA >= 324) {
                    (*enemies)[13]->geometry->setMatrix((*enemies)[14]->geometry->getMatrix());
                    (*enemies)[13]->setVisible(true);
                    (*enemies)[13]->setActive(true);
                    (*enemies)[14]->setVisible(false);
                    (*enemies)[14]->setActive(false);
                    (*enemies)[14]->field_0x74 = 1;
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform((*enemies)[13]->geometry->transform))
                        ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 5:
                scriptTimerA += delta;
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform((*enemies)[13]->geometry->transform))
                    ->Update((long long)((float)delta * 0.3f), false);
                for (unsigned i = 0; i != 13; ++i) {
                    vector_0x28 = (*enemies)[i]->geometry->getDirection();
                    if ((unsigned long long)(scriptTimerA - 2001) <= 2998)
                        vector_0x28 -= (*enemies)[i]->geometry->getRightVector() * ((float)scriptTimerA / 100000.0f);
                    Vector direction = AbyssEngine::AEMath::VectorNormalize(vector_0x28);
                    (*enemies)[i]->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
                    (*enemies)[i]->geometry->moveForward((float)(scriptTimerA * delta) * 0.001f);
                }
                m_pExplosion->update((int)((float)delta * 0.5f), static_cast<TargetFollowCamera *>(nullptr));
                if (scriptTimerA > 10000) {
                    m_pLevel->field_74->enableSystemEmit(m_pLevel->field_54, false);
                    for (unsigned i = 0; i != 13; ++i) {
                        (*enemies)[i]->setActive(false);
                        (*enemies)[i]->setVisible(false);
                        (*enemies)[i]->setPosition(50000.0f, 50000.0f, 50000.0f);
                    }
                    m_pLevel->getPlayer()->resetGunDelay();
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    player->setFreeze(false);
                    player->setVisible(true);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    m_pRadar->lockedEnemy = nullptr;
                    m_pRadar->candidateEnemy = nullptr;
                    cameraControlled = 0;
                    ++m_nState;
                }
                break;
            }
            break;
        }
        case 157: {
            // Android 0x13a308 onward: indices differ on weak HD devices.
            int stationIndex = Globals::isRunningHDonWeakDevice ? 12 : 23;
            int escortIndex = Globals::isRunningHDonWeakDevice ? 11 : 22;
            int fighterIndex = Globals::isRunningHDonWeakDevice ? 10 : 21;
            if (m_pGeometry2->isVisible()) {
                AEGeometry *stationGeometry = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320);
                m_pGeometry2->setPosition(stationGeometry->getPosition());
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry2->transform))->Update(delta, true);
            }
            if (m_pGeometry3->isVisible()) {
                AEGeometry *stationGeometry = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320);
                m_pGeometry3->setPosition(stationGeometry->getPosition());
                static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry3->transform))->Update(delta, true);
            }
            if (m_nState == 0) {
                m_pGeometry4 = new AEGeometry(Globals::Canvas);
                m_pGeometry4->setPosition(Vector{-120000.0f, 0.0f, 5000.0f});
                m_pGeometry4->setDirection(Vector{0.0f, 0.0f, 1.0f}, Vector{0.0f, 1.0f, 0.0f});
                m_nState = 1;
            }
            if (m_nState == 1) {
                if (static_cast<RadioMessage *>((*messages)[0])->isOver()) {
                    if (player->isInRocketControl()) {
                        resetCamera(m_pLevel);
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->setTarget(m_pGeometry4);
                    m_pCamera->setPosition(m_pGeometry4->getPosition());
                    m_pCamera->translate(-10000.0f, 5000.0f, 25000.0f);
                    static_cast<Player *>(player->player)->setVulnerable(false);
                    player->setFreeze(true);
                    scriptTimerA = 0;
                    m_nState = 2;
                }
                break;
            }
            switch (m_nState) {
            case 2:
                scriptTimerA += delta;
                m_pGeometry4->translate(0.0f, 0.0f, (float)(3 * delta));
                m_pCamera->translate(0.0f, 0.0f, (float)delta);
                if (scriptTimerA >= 16001) {
                    scriptTimerA = 0;
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (enemy->shipGroupFlag == 49 || enemy->shipGroupFlag == 44) {
                            (*enemies)[i]->setVisible(true);
                            enemy->setActive(true);
                        }
                    }
                    m_pGeometry4->setPosition((*enemies)[stationIndex]->getPosition());
                    vector_0x28 = (*enemies)[fighterIndex]->getPosition();
                    vector_0x28 += Vector{-1500.0f, -5000.0f, 2500.0f};
                    m_pCamera->setPosition(vector_0x28);
                    m_nState = 3;
                }
                break;
            case 3:
                scriptTimerA += delta;
                m_pCamera->translate(0.0f, std::max((float)scriptTimerA / -15000.0f + 1.0f, 0.0f) * (float)delta, 0.0f);
                if (scriptTimerA >= 8001) {
                    scriptTimerB = 0;
                    ++m_nState;
                }
                break;
            case 4:
                if (scriptTimerB <= 6999)
                    scriptTimerB += delta;
                scriptTimerA += delta;
                m_pCamera->translate(0.0f, std::max((float)scriptTimerA / -15000.0f + 1.0f, 0.0f) * (float)delta, 0.0f);
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    KIPlayer *enemy = (*enemies)[i];
                    if (enemy->shipGroupFlag == 49 || enemy->shipGroupFlag == 44)
                        enemy->geometry->moveForward((float)scriptTimerB * 0.0199999995529651641845703125f);
                }
                if (static_cast<RadioMessage *>((*messages)[3])->isOver() && scriptTimerA >= 18001) {
                    m_pLevel->getPlayer()->resetGunDelay();
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    player->setFreeze(false);
                    player->setVisible(true);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        KIPlayer *enemy = (*enemies)[i];
                        if (enemy->shipGroupFlag == 49 || enemy->shipGroupFlag == 44) {
                            (*enemies)[i]->setVisible(true);
                            enemy->setActive(true);
                            static_cast<PlayerFighter *>(enemy)->setAIDisabled(false);
                            Vector offset;
                            if (enemy->shipGroupFlag == 44) {
                                static_cast<PlayerFighter *>(enemy)->setCloakingPossible(true);
                                float y = (float)(static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(12000) - 6000);
                                float z = (float)(static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(30000) - 87000);
                                offset = Vector{120000.0f, y, z};
                            } else {
                                float y = (float)(static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(6000) - 3000);
                                float z = (float)(static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(6000) - 7000);
                                offset = Vector{20000.0f, y, z};
                            }
                            enemy->translate(offset);
                        }
                    }
                    ++m_nState;
                }
                break;
            case 5: {
                int dead;
                if (Globals::isRunningHDonWeakDevice) {
                    dead = 5;
                    for (unsigned i = 6; i != 12; ++i)
                        dead += (*enemies)[i]->isDead();
                } else {
                    dead = 0;
                    for (unsigned i = 11; i != 22; ++i)
                        dead += (*enemies)[i]->isDead();
                }
                scriptTimerB += delta;
                if (scriptTimerB >= 25001) {
                    scriptTimerB = 0;
                    int *flag = reinterpret_cast<int *>(reinterpret_cast<char *>((*enemies)[fighterIndex]) + 320);
                    *flag = *flag == 0;
                }
                if ((dead >= 9 && scriptTime > 200000)
                    || (*enemies)[fighterIndex]->player->getHitpoints() < (*enemies)[fighterIndex]->player->getMaxHitpoints() / 4) {
                    if (player->isInRocketControl()) {
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    static_cast<Player *>(player->player)->setVulnerable(false);
                    player->setFreeze(true);
                    m_pCamera->setLookAtCam(true);
                    KIPlayer *escort = (*enemies)[escortIndex];
                    m_pCamera->setTarget(escort->geometry);
                    vector_0x28 = (*enemies)[stationIndex]->getPosition();
                    vector_0x28 += Vector{45000.0f, 2000.0f, 11000.0f};
                    m_pCamera->setPosition(vector_0x28);
                    escort->setPosition(vector_0x28 + Vector{15000.0f, -200.0f, 1000.0f});
                    Vector position = (*enemies)[stationIndex]->getPosition();
                    Vector escortPosition = escort->getPosition();
                    escort->geometry->setDirection(VectorNormalize(position - escortPosition), Vector{0.0f, 1.0f, 0.0f});
                    escort->setVisible(true);
                    escort->setActive(true);
                    (*enemies)[fighterIndex]->player->setVulnerable(false);
                    *reinterpret_cast<int *>(reinterpret_cast<char *>((*enemies)[fighterIndex]) + 320) = 0;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    scriptTimerA = 0;
                    m_nState = 6;
                }
                break;
            }
            case 6: {
                scriptTimerA += delta;
                KIPlayer *escort = (*enemies)[escortIndex];
                m_pLevel->lodManager->forceUpdate(delta, false);
                Vector escortPosition = escort->getPosition();
                Vector stationPosition = (*enemies)[stationIndex]->getPosition();
                if (escortPosition.x > stationPosition.x)
                    escort->geometry->moveForward((float)(3 * delta));
                if (static_cast<RadioMessage *>((*messages)[10])->isTriggered()) {
                    escort->setVisible(false);
                    escort->setActive(false);
                    escort->setPosition(50000.0f, 50000.0f, 50000.0f);
                    m_pCamera->setTarget(*reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320));
                    scriptTimerA = 0;
                    m_nState = 7;
                }
                break;
            }
            case 7:
                if (static_cast<RadioMessage *>((*messages)[10])->isOver()) {
                    KIPlayer *station = (*enemies)[stationIndex];
                    Vector velocity{0.0f, 0.0f, 0.0f};
                    Vector position = station->getPosition();
                    Globals::sound->updateEvent3DAttributes(2251, &position, &velocity, false);
                    if (scriptTimerA <= 9999)
                        scriptTimerA += delta;
                    AEGeometry *stationGeometry = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>(station) + 320);
                    stationGeometry->moveForward(((float)scriptTimerA / 10000.0f) * (float)(-10 * delta));
                    if (stationGeometry->getPosition().z > 100000.0f) {
                        PlayerFighter *fighter = static_cast<PlayerFighter *>((*enemies)[fighterIndex]);
                        fighter->setPosition(*m_pCamera->getPosition() + Vector{4000.0f, -600.0f, -2000.0f});
                        Vector target = station->getPosition() + Vector{0.0f, 0.0f, 40000.0f};
                        Vector fighterPosition = fighter->getPosition();
                        vector_0x28 = VectorNormalize(target - fighterPosition);
                        static_cast<KIPlayer *>(fighter)->geometry->setDirection(vector_0x28, Vector{0.0f, 1.0f, 0.0f});
                        fighter->setAIDisabled(true);
                        fighter->setVisible(true);
                        fighter->setActive(true);
                        player->setComputerControlled(true);
                        player->geometry->setDirection(vector_0x28, Vector{0.0f, 1.0f, 0.0f});
                        player->setSpeed(4.0f);
                        player->setFreeze(false);
                        Vector playerPosition = *m_pCamera->getPosition() + Vector{5000.0f, 300.0f, -14000.0f};
                        player->setPosition(playerPosition.x, playerPosition.y, playerPosition.z);
                        m_pLevel->lodManager->forceUpdate(delta, false);
                        m_pCamera->setTarget(stationGeometry);
                        scriptTimerA = 0;
                        m_nState = 8;
                    }
                }
                break;
            case 8: {
                Vector velocity{0.0f, 0.0f, 0.0f};
                Vector position = (*enemies)[stationIndex]->getPosition();
                Globals::sound->updateEvent3DAttributes(2251, &position, &velocity, false);
                (*reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320))->moveForward((float)(-10 * delta));
                KIPlayer *fighter = (*enemies)[fighterIndex];
                fighter->geometry->moveForward((float)(4 * delta));
                fighter->player->shoot(0, (long long)delta, false);
                scriptTimerA += delta;
                if (scriptTimerA >= 12001 && static_cast<RadioMessage *>((*messages)[12])->isOver()) {
                    vector_0x28 = (*enemies)[stationIndex]->getPosition();
                    m_pCamera->setPosition(vector_0x28 + Vector{35000.0f, 0.0f, 70000.0f});
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    fighter->geometry->setDirection(Vector{0.0f, 0.0f, 1.0f}, Vector{0.0f, 1.0f, 0.0f});
                    player->geometry->setDirection(Vector{0.0f, 0.0f, 1.0f}, Vector{0.0f, 1.0f, 0.0f});
                    fighter->setVisible(false);
                    fighter->setActive(false);
                    player->setSpeed(10.0f);
                    scriptTimerA = 0;
                    scriptTimerB = 0;
                    m_nState = 9;
                }
                break;
            }
            case 9: {
                Vector velocity{0.0f, 0.0f, 0.0f};
                Vector position = (*enemies)[stationIndex]->getPosition();
                Globals::sound->updateEvent3DAttributes(2251, &position, &velocity, false);
                (*reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320))->moveForward((float)(-10 * delta));
                KIPlayer *fighter = (*enemies)[fighterIndex];
                fighter->geometry->moveForward((float)(2 * delta));
                fighter->player->shoot(0, (long long)delta, false);
                scriptTimerA += delta;
                scriptTimerB += delta;
                if (scriptTimerB <= 8999 && scriptTimerB + delta >= 9000) {
                    fighter->setPosition(*m_pCamera->getPosition() + Vector{4000.0f, -600.0f, -2000.0f});
                    fighter->setVisible(true);
                    fighter->setActive(true);
                }
                Vector target = (*enemies)[stationIndex]->getPosition();
                Vector fighterPosition = fighter->getPosition();
                vector_0x28 = VectorNormalize(target - fighterPosition);
                static_cast<KIPlayer *>(fighter)->geometry->setDirection(vector_0x28, Vector{0.0f, 1.0f, 0.0f});
                if (static_cast<RadioMessage *>((*messages)[13])->isOver() && !m_pExplosion) {
                    m_pExplosion = new Explosion(0);
                    m_pExplosion->addFireStreaks();
                    m_pExplosion->setScaling(5.0f);
                    Vector explosionPosition = (*enemies)[stationIndex]->getPosition() + Vector{-2000.0f, 1000.0f, -8000.0f};
                    m_pExplosion->start(explosionPosition, Vector{0.0f, 0.0f, 0.0f});
                    Globals::sound->play(2244, nullptr, nullptr, 0.0f);
                    scriptTimerA = 0;
                    scriptTimerC = 300;
                }
                if (scriptTimerC >= 1) {
                    scriptTimerC -= delta;
                    if (scriptTimerC <= 0) {
                        scriptTimerC = 0;
                        AEGeometry *stationGeometry = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320);
                        m_pGeometry2->setPosition(stationGeometry->getPosition());
                        m_pGeometry2->setDirection(Vector{0.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
                        m_pGeometry2->setVisible(true);
                        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry2->transform))
                            ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(2), nullptr);
                    }
                }
                if (m_pExplosion) {
                    m_pExplosion->update(delta, static_cast<TargetFollowCamera *>(nullptr));
                    scriptTimerA += delta;
                    if (scriptTimerA >= 6001) {
                        scriptTimerA = -2000;
                        Globals::sound->play(2244, nullptr, nullptr, 0.0f);
                        Vector explosionPosition = (*enemies)[stationIndex]->getPosition() + Vector{-6000.0f, 2000.0f, -10000.0f};
                        m_pExplosion->start(explosionPosition, Vector{0.0f, 0.0f, 0.0f});
                    }
                }
                if (static_cast<RadioMessage *>((*messages)[14])->isOver()) {
                    vector_0x28 = (*enemies)[stationIndex]->getPosition();
                    m_pCamera->setPosition(vector_0x28 + Vector{50000.0f, 0.0f, 70000.0f});
                    scriptTimerA = 0;
                    scriptTimerB = 0;
                    fighter->setVisible(false);
                    player->setVisible(false);
                    AEGeometry *stationGeometry = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320);
                    m_pGeometry3->setPosition(stationGeometry->getPosition());
                    m_pGeometry3->setDirection(Vector{0.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
                    m_pGeometry3->setVisible(true);
                    m_pGeometry2->setVisible(false);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry3->transform))
                        ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                    m_nState = 10;
                }
                break;
            }
            case 10: {
                Vector velocity{0.0f, 0.0f, 0.0f};
                Vector position = (*enemies)[stationIndex]->getPosition();
                Globals::sound->updateEvent3DAttributes(2251, &position, &velocity, false);
                scriptTimerB += delta;
                m_pCamera->setRumblePercentage(0.0f, 0);
                if ((unsigned long long)(scriptTimerB - 2001) <= 6998 && scriptTimerB + delta >= 9000) {
                    m_pExplosion->setScaling(10.0f);
                    Vector stationPosition = (*enemies)[stationIndex]->getPosition();
                    Vector light = m_pLevel->getStarSystem()->getLightDirection() * 3000.0f;
                    m_pExplosion->start(stationPosition - light, Vector{0.0f, 0.0f, 0.0f});
                    Globals::sound->play(2243, nullptr, nullptr, 0.0f);
                    m_pCamera->setRumblePercentage(100.0f, 30);
                }
                m_pExplosion->update(delta, static_cast<TargetFollowCamera *>(nullptr));
                m_pCamera->translate((float)(-2 * delta), 0.0f, (float)-delta * 0.3300000131130218505859375f);
                if (static_cast<RadioMessage *>((*messages)[15])->isOver()) {
                    scriptTimerA += delta;
                    if (scriptTimerA >= 3001) {
                        AEGeometry *stationGeometry = *reinterpret_cast<AEGeometry **>(reinterpret_cast<char *>((*enemies)[stationIndex]) + 320);
                        m_pGeometry1->setPosition(stationGeometry->getPosition());
                        m_pGeometry1->setDirection(Vector{0.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
                        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry1->transform))->SetVisible(false);
                        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry1->transform))
                            ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                        scriptTimerA = 0;
                        ++m_nState;
                    }
                }
                break;
            }
            case 11:
                scriptTimerA += delta;
                m_pCamera->translate((float)(-3 * delta), 0.0f, (float)-delta * 0.3300000131130218505859375f);
                if (scriptTimerA >= 6001) {
                    if (scriptTimerA - delta <= 6000)
                        Globals::sound->play(2247, nullptr, nullptr, 0.0f);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry1->transform))->SetVisible(true);
                    static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry1->transform))->Update(delta, true);
                    if (scriptTimerA >= 10001) {
                        m_pGeometry4->setPosition((*enemies)[stationIndex]->getPosition());
                        m_pGeometry4->setDirection(m_pLevel->getStarSystem()->getLightDirection(), Vector{0.0f, 1.0f, 0.0f});
                        m_pCamera->setTarget(m_pGeometry4);
                        scriptTimerA = 0;
                        (*enemies)[fighterIndex]->setVisible(false);
                        (*enemies)[fighterIndex]->setActive(false);
                        ++m_nState;
                    }
                }
                break;
            case 12:
                scriptTimerA += delta;
                if (scriptTimerA <= 2999)
                    m_pCamera->translate((float)(-20 * delta), 0.0f, (float)(-20 * delta));
                else if (scriptTimerA <= 6999)
                    m_pCamera->translate((float)(-3 * delta), 0.0f, (float)(-10 * delta));
                if (scriptTimerA <= 6999) {
                    if (scriptTimerA <= 4999)
                        m_pGeometry4->moveForward((float)(35 * delta));
                    if (scriptTimerA <= 5999)
                        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry1->transform))->Update(delta, true);
                }
                m_pCamera->setRumblePercentage(100.0f, (int)(((float)scriptTimerA / 6500.0f) * 30.0f));
                if (scriptTimerA >= 5000 && scriptTimerA - delta <= 4999)
                    m_pLevel->switchSkyboxForSupernovaReversal();
                if (scriptTimerA >= 5500 && scriptTimerA - delta <= 5499) {
                    m_pExplosion->setScaling(25.0f);
                    Vector position = (*enemies)[stationIndex]->getPosition() + Vector{0.0f, 0.0f, -12000.0f};
                    Vector light = m_pLevel->getStarSystem()->getLightDirection() * 3000.0f;
                    m_pExplosion->start(position - light, Vector{0.0f, 0.0f, 0.0f});
                }
                if (scriptTimerA >= 5700 && scriptTimerA - delta <= 5699) {
                    m_pGeometry1->setVisible(false);
                    (*enemies)[stationIndex]->setVisible(false);
                    Globals::layout->startFade(true, -1, 6600 - m_nScriptTimerA);
                    Globals::sound->play(2248, nullptr, nullptr, 0.0f);
                }
                if (scriptTimerA >= 5500)
                    m_pExplosion->update(delta, static_cast<TargetFollowCamera *>(nullptr));
                if ((unsigned long long)(scriptTimerA - 5001) <= 998) {
                    AEGeometry *planet = (*static_cast<Array<AEGeometry *> *>(m_pLevel->getStarSystem()->getPlanets()))[0];
                    if (scriptTimerA <= 5499)
                        planet->setScaling(planet->getScaling() * 0.800000011920928955078125f);
                    else
                        planet->setScaling(planet->getScaling() * 1.7000000476837158203125f);
                }
                if (scriptTimerA >= 6500) {
                    ++m_nState;
                    Globals::status->nextCampaignMission(true);
                    Globals::status->setStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(111));
                    Globals::status->departStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(111));
                    Globals::status->field_64 = static_cast<Player *>(player->player)->getHitpoints();
                    Globals::status->field_5c = static_cast<Player *>(player->player)->getShieldHP();
                    Globals::status->field_60 = static_cast<Player *>(player->player)->getArmorHP();
                    Globals::status->field_68 = static_cast<Player *>(player->player)->getGammaHP();
                    Globals::status->field_f4 = player->getCurrentSecondaryWeaponIndex();
                    static_cast<Player *>(player->player)->setGammaHP(100);
                    Globals::switch_to_target_setting = 1;
                    Level::initStreamOutPosition = 1;
                    static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
                }
                break;
            }
            break;
        }
        case 154:
            // Android campaign branch, states 0..10 (0x13b... / 0x142f..0x144a).
            if (m_nState == 0 && static_cast<RadioMessage *>((*messages)[0])->isTriggered()) {
                if (player->isInRocketControl()) {
                    resetCamera(m_pLevel);
                    player->setRocketControl(nullptr, nullptr);
                    player->killLiberator();
                    m_pCamera->setRumblePercentage(0.0f, 0);
                }
                player->setTurretMode(false);
                player->setFreeLookMode(false);
                m_pCamera->enableFirstPersonCam(false);
                player->hideShipForFirstPersonCameraView(false);
                player->stopShooting(0);
                player->setComputerControlled(true);
                static_cast<Player *>(player->player)->setVulnerable(false);
                player->setSpeed(0.0f);
                savedAutoTurret = player->autoTurretIsEnabled();
                if (savedAutoTurret)
                    player->setAutoTurret(false);
                cameraControlled = 1;
                m_pHud->visible = 0;
                m_pRadar->enabled = 0;
                m_pCamera->setLookAtCam(true);
                m_pCamera->setTarget((*m_pLevel->getEnemies())[2]->geometry);
                vector_0x28 = (*m_pLevel->getEnemies())[2]->geometry->getPosition();
                vector_0x28 += Vector{300.0f, 300.0f, 5800.0f};
                m_pCamera->setPosition(vector_0x28);
                static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(true);
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    if ((*enemies)[i]->shipGroup == 9) {
                        Vector position = m_pLevel->getPlayer()->getPosition();
                        Vector enemyPosition = (*enemies)[i]->getPosition();
                        (*enemies)[i]->geometry->setDirection(VectorNormalize(position - enemyPosition),
                                                              Vector{0.0f, 1.0f, 0.0f});
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                    }
                }
                (*enemies)[1]->field_0x70 = 1;
                scriptTimerA = 0;
                ++m_nState;
                m_pLevel->lodManager->forceUpdate(delta, false);
                break;
            }
            switch (m_nState) {
            case 1:
                player->setSpeed(0.0f);
                m_pLevel->lodManager->forceUpdate(delta, false);
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    if ((*enemies)[i]->shipGroup == 9)
                        (*enemies)[i]->geometry->moveForward((float)delta * 0.5f);
                }
                if (static_cast<RadioMessage *>((*messages)[1])->isOver())
                    scriptTimerA += delta;
                if (scriptTimerA >= 1501) {
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        if ((*enemies)[i]->shipGroup == 9) {
                            (*enemies)[i]->setActive(false);
                            (*enemies)[i]->setVisible(false);
                        }
                    }
                    m_pCamera->setTarget(player->geometry);
                    vector_0x28 = player->getPosition();
                    vector_0x28 += Vector{-350.0f, 400.0f, -1500.0f};
                    m_pCamera->setPosition(vector_0x28);
                    m_pCamera->setFixed(false);
                    ++m_nState;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    scriptTimerA = 0;
                }
                break;
            case 2: {
                player->setSpeed(0.0f);
                m_pLevel->lodManager->forceUpdate(delta, false);
                float advance = (float)delta * 0.25f;
                (*enemies)[0]->geometry->moveForward(advance);
                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                vector_0x40 = player->GetDirVector();
                vector_0x28 += vector_0x40 * (advance * 0.008000000379979610443115234375f);
                vector_0x40 = player->geometry->getRightVector();
                vector_0x28 += vector_0x40 * (advance * -0.0500000007450580596923828125f);
                vector_0x40 = player->GetUpVector();
                vector_0x28 += vector_0x40 * (advance * 0.0051000001840293407440185546875f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                if (static_cast<RadioMessage *>((*messages)[2])->isOver())
                    scriptTimerA += delta;
                if (scriptTimerA >= 1001) {
                    player->setSpeed(0.0f);
                    m_pCamera->setFixed(false);
                    m_pCamera->setTarget((*m_pLevel->getEnemies())[1]->geometry);
                    vector_0x28 = (*m_pLevel->getEnemies())[1]->geometry->getPosition();
                    vector_0x28 += Vector{0.0f, 4000.0f, 26000.0f};
                    m_pCamera->setPosition(vector_0x28);
                    scriptTimerA = 0;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    ++m_nState;
                }
                break;
            }
            case 4: {
                float advance = (float)delta * 0.25f;
                (*enemies)[0]->geometry->moveForward(advance);
                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                vector_0x40 = player->GetDirVector();
                vector_0x28 += vector_0x40 * (advance * 0.008000000379979610443115234375f);
                vector_0x40 = player->geometry->getRightVector();
                vector_0x28 += vector_0x40 * (advance * -0.0500000007450580596923828125f);
                vector_0x40 = player->GetUpVector();
                vector_0x28 += vector_0x40 * (advance * 0.004999999888241291046142578125f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                if (static_cast<RadioMessage *>((*messages)[4])->isOver())
                    scriptTimerA += delta;
                if (scriptTimerA >= 1001) {
                    player->setSpeed(0.0f);
                    m_pCamera->setFixed(false);
                    m_pCamera->setTarget((*m_pLevel->getEnemies())[1]->geometry);
                    vector_0x28 = (*m_pLevel->getEnemies())[1]->geometry->getPosition();
                    vector_0x28 += Vector{0.0f, 4000.0f, 42000.0f};
                    m_pCamera->setPosition(vector_0x28);
                    scriptTimerA = 0;

                    ++m_nState;
                }
                break;
            }
            case 6: {
                float advance = (float)delta * 0.25f;
                (*enemies)[0]->geometry->moveForward(advance);
                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                vector_0x40 = player->GetDirVector();
                vector_0x28 += vector_0x40 * (advance * 0.008000000379979610443115234375f);
                vector_0x40 = player->geometry->getRightVector();
                vector_0x28 += vector_0x40 * (advance * 0.0500000007450580596923828125f);
                vector_0x40 = player->GetUpVector();
                vector_0x28 += vector_0x40 * (advance * 0.004999999888241291046142578125f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                if (static_cast<RadioMessage *>((*messages)[6])->isOver())
                    scriptTimerA += delta;
                if (scriptTimerA >= 1001) {
                    player->setSpeed(0.0f);
                    m_pCamera->setFixed(false);
                    m_pCamera->setTarget((*m_pLevel->getEnemies())[0]->geometry);
                    vector_0x28 = (*m_pLevel->getEnemies())[0]->geometry->getPosition();
                    vector_0x28 += Vector{200.0f, 300.0f, 1800.0f};
                    m_pCamera->setPosition(vector_0x28);
                    scriptTimerA = 0;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    ++m_nState;
                }
                break;
            }
            case 3:
                if (static_cast<RadioMessage *>((*messages)[3])->isOver())
                    scriptTimerA += delta;
                m_pCamera->translate((float)delta * 0.5f, (float)delta * 0.100000001490116119384765625f,
                                    (float)delta * 0.5f);
                if (scriptTimerA >= 1001) {
                    player->setSpeed(0.0f);
                    m_pCamera->setTarget(player->geometry);
                    vector_0x28 = player->getPosition();
                    vector_0x28 += Vector{-350.0f, 400.0f, -1500.0f};
                    m_pCamera->setPosition(vector_0x28);
                    m_pCamera->setFixed(false);
                    ++m_nState;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    scriptTimerA = 0;
                }
                break;
            case 5:
                m_pCamera->translate((float)delta, 0.0f, (float)delta * 0.100000001490116119384765625f);
                if (static_cast<RadioMessage *>((*messages)[5])->isOver())
                    scriptTimerA += delta;
                if (scriptTimerA >= 1001) {
                    player->setSpeed(0.0199999995529651641845703125f);
                    m_pCamera->setTarget(player->geometry);
                    vector_0x28 = player->getPosition();
                    vector_0x28 += Vector{150.0f, 400.0f, -1500.0f};
                    m_pCamera->setPosition(vector_0x28);
                    m_pCamera->setFixed(false);
                    ++m_nState;
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    (*enemies)[0]->setPosition(Vector{-10000.0f, 0.0f, 22000.0f});
                    *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>((*enemies)[0]) + 318) = 0;
                    static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(false);
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        if ((*enemies)[i]->shipGroup == 9) {
                            (*enemies)[i]->setActive(false);
                            (*enemies)[i]->player->setVulnerable(true);
                        }
                    }
                    scriptTimerA = 0;
                }
                break;
            case 7:
                if ((*enemies)[0]->isDocked()) {
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 8:
                scriptTimerA += delta;
                if (scriptTimerA >= 2001) {
                    m_nState = 9;
                    scriptTimerA = 0;
                }
                break;
            case 9:
                if (static_cast<RadioMessage *>((*messages)[7])->isOver())
                    scriptTimerA += delta;
                if (scriptTimerA >= 2001) {
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        if ((*enemies)[i]->shipGroup == 9) {
                            *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>((*enemies)[i]) + 37) = 1;
                            (*enemies)[i]->setActive(true);
                            (*enemies)[i]->setVisible(true);
                            static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(false);
                            Player *target = i % 3 == 0 ? (*enemies)[0]->player : static_cast<Player *>(player->player);
                            (*enemies)[i]->player->setEnemy(target);
                        }
                    }
                    (*m_pLevel->getEnemies())[1]->shipGroup = 8;
                    (*m_pLevel->getEnemies())[1]->field_0x74 = 0;
                    (*m_pLevel->getEnemies())[1]->player->setAlwaysFriend(false);
                    (*m_pLevel->getEnemies())[1]->player->setAlwaysEnemy(true);
                    (*m_pLevel->getEnemies())[0]->player->setEnemies(nullptr);
                    (*m_pLevel->getEnemies())[0]->player->setVulnerable(true);
                    m_pCamera->setFixed(false);
                    m_pLevel->getPlayer()->resetGunDelay();
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setFreeze(false);
                    player->setSpeed(2.0f);
                    player->setComputerControlled(false);
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    player->setPosition(0.0f, 0.0f, 90000.0f);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    if (savedAutoTurret)
                        player->setAutoTurret(true);
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 10:
                if (static_cast<RadioMessage *>((*messages)[8])->isOver() && m_nTimeLimit <= 0) {
                    scriptTime = 0;
                    m_nTimeLimit = 91000;
                    m_pLevel->objectivesA = new Objective(3, 91000, m_pLevel);
                }
                if (player->isDockedToDockingPoint() && player->hackingWon()) {
                    m_nState = 11;
                    delete m_pLevel->objectivesA;
                    m_pLevel->objectivesA = nullptr;
                    for (unsigned i = 0; i < enemies->size_; ++i) {
                        if ((*enemies)[i]->shipGroup == 9)
                            (*enemies)[i]->player->setEnemies(nullptr);
                    }
                    m_nTimeLimit = 0;
                }
                break;
            }
            break;
        case 105:
            // Android 0x13aea4 / 0x13aeec: supernova departure states.
            if (m_nState <= 2) {
                for (unsigned i = 0; i != 2; ++i)
                    (*enemies)[i]->geometry->moveForward((float)(2 * delta));
            }
            if (m_nState <= 4)
                cameraControlled = 1;
            switch (m_nState) {
            case 1: {
                scriptTimerA += delta;
                Vector direction = player->geometry->getDirection() * 6.0799999237060546875f;
                Vector right = player->geometry->getRightVector() * 0.20000000298023223876953125f;
                Vector motion = ((direction + right) * 0.4000000059604644775390625f) * (float)delta;
                m_pCamera->translate(motion.x, motion.y, motion.z);
                if (scriptTimerA >= 13001) {
                    m_nState = 2;
                    scriptTimerA = 0;
                }
                break;
            }
            case 2: {
                Vector direction = player->geometry->getDirection() * 6.0799999237060546875f;
                Vector right = player->geometry->getRightVector() * 0.20000000298023223876953125f;
                Vector motion = ((direction + right) * 0.4000000059604644775390625f) * (float)delta;
                m_pCamera->translate(motion.x * 0.89999997615814208984375f,
                    motion.y * 0.89999997615814208984375f, motion.z * 0.89999997615814208984375f);
                if (static_cast<RadioMessage *>((*messages)[1])->isOver()) {
                    Vector rightVector = player->geometry->getRightVector();
                    Vector dirVector = player->geometry->getDirection();
                    int first[3] = {(int)(rightVector.x * 900000.0f + dirVector.x * -900000.0f),
                        (int)(rightVector.y * 900000.0f + dirVector.y * -900000.0f),
                        (int)(rightVector.z * 900000.0f - dirVector.z * 900000.0f)};
                    int second[3] = {(int)(dirVector.x * -900000.0f - rightVector.x * 900000.0f),
                        (int)(dirVector.y * -900000.0f - rightVector.y * 900000.0f),
                        (int)(dirVector.z * -90000.0f - rightVector.z * 900000.0f)};
                    Route *firstRoute = new Route(first, 3);
                    Route *secondRoute = new Route(second, 3);
                    (*enemies)[0]->setRoute(firstRoute);
                    (*enemies)[1]->setRoute(secondRoute);
                    for (unsigned i = 0; i != 2; ++i) {
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(false);
                        *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>((*enemies)[i]) + 300) = 1;
                    }
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            }
            case 3: {
                scriptTimerA += delta;
                Vector direction = player->geometry->getDirection() * 6.0799999237060546875f;
                Vector right = player->geometry->getRightVector() * 0.20000000298023223876953125f;
                Vector motion = ((direction + right) * 0.4000000059604644775390625f) * (float)delta;
                m_pCamera->translate(motion.x * 0.4000000059604644775390625f,
                    motion.y * 0.4000000059604644775390625f, motion.z * 0.4000000059604644775390625f);
                for (unsigned i = 0; i != 2; ++i)
                    *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>((*enemies)[i]) + 300) = 1;
                if (scriptTimerA > 3000) {
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            }
            case 4: {
                scriptTimerA += delta;
                Vector direction = player->geometry->getDirection() * 6.0799999237060546875f;
                Vector right = player->geometry->getRightVector() * 0.20000000298023223876953125f;
                Vector motion = ((direction + right) * 0.4000000059604644775390625f) * (float)delta;
                m_pCamera->translate(motion.x * 0.100000001490116119384765625f,
                    motion.y * 0.100000001490116119384765625f, motion.z * 0.100000001490116119384765625f);
                for (unsigned i = 0; i != 2; ++i)
                    *reinterpret_cast<uint8_t *>(reinterpret_cast<char *>((*enemies)[i]) + 300) = 1;
                if (scriptTimerA > 3000) {
                    m_pLevel->getPlayer()->resetGunDelay();
                    player->setComputerControlled(false);
                    m_pCamera->setLookAtCam(false);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    for (unsigned i = 0; i != 2; ++i)
                        (*enemies)[i]->field_0x74 = 1;
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            }
            case 5:
                scriptTimerA += delta;
                if (scriptTimerA >= 10001) {
                    for (unsigned i = 0; i != 2; ++i) {
                        (*enemies)[i]->setActive(false);
                        (*enemies)[i]->setSpeed(0.0f);
                    }
                    ++m_nState;
                    scriptTimerA = 0;
                }
                break;
            case 6:
                scriptTimerA += delta;
                if (scriptTimerA >= 24001) {
                    scriptTimerA = 0;
                    Vector light = VectorNormalize(m_pLevel->getStarSystem()->getLightDirection());
                    Vector position = player->geometry->getPosition();
                    for (unsigned i = 2; i != 5; ++i) {
                        (*enemies)[i]->awake();
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(true);
                        static_cast<PlayerFighter *>((*enemies)[i])->cloak(1000, true);
                    }
                    (*enemies)[2]->setPosition(position.x + light.x * 100000.0f, 0.0f,
                                              position.z + light.z * 100000.0f);
                    (*enemies)[2]->geometry->setDirection(-light, Vector{0.0f, 1.0f, 0.0f});
                    (*enemies)[3]->setPosition(((*enemies)[2]->getPosition()
                        + (*enemies)[2]->geometry->getRightVector() * 2200.0f)
                        + (*enemies)[2]->geometry->getUpVector() * -200.0f
                        + (*enemies)[2]->geometry->getDirection() * -2200.0f);
                    (*enemies)[3]->geometry->setDirection(-light, Vector{0.0f, 1.0f, 0.0f});
                    (*enemies)[4]->setPosition(((*enemies)[2]->getPosition()
                        + (*enemies)[2]->geometry->getRightVector() * -2200.0f)
                        + +(*enemies)[2]->geometry->getUpVector() * -200.0f
                        + (*enemies)[2]->geometry->getDirection() * -2200.0f);
                    (*enemies)[4]->geometry->setDirection(-light, Vector{0.0f, 1.0f, 0.0f});
                    for (unsigned i = 2; i != 5; ++i)
                        (*enemies)[i]->geometry->translate((*enemies)[i]->geometry->getRightVector() * -60000.0f);
                    player->setTurretMode(false);
                    player->setFreeze(true);
                    player->setVisible(false);
                    resetCamera(m_pLevel);
                    if (player->isInRocketControl()) {
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    player->setFreeLookMode(false);
                    m_pCamera->setLookAtCam(true);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    player->stopShooting(0);
                    cameraControlled = 1;
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    m_pCamera->setTarget((*enemies)[2]->geometry);
                    vector_0x28 = (*enemies)[2]->geometry->getPosition();
                    vector_0x40 = (*enemies)[2]->geometry->getDirection();
                    vector_0x28 += vector_0x40 * 6800.0f;
                    vector_0x40 = (*enemies)[2]->geometry->getRightVector();
                    vector_0x28 += vector_0x40 * 1200.0f;
                    vector_0x40 = (*enemies)[2]->geometry->getUpVector();
                    vector_0x28 += vector_0x40 * 400.0f;
                    m_pCamera->setPosition(vector_0x28);
                    scriptTimerA = 0;
                    scriptTimerB = 0;
                    ++m_nState;
                }
                break;
            case 7: {
                float advance = (float)delta * 0.0500000007450580596923828125f;
                scriptTimerA += delta;
                scriptTimerB = (long long)(advance + (float)scriptTimerB);
                for (unsigned i = 2; i != 5; ++i) {
                    switch (i) {
                    case 2: (*enemies)[i]->geometry->moveForward((float)((double)delta * 2.5)); break;
                    case 3: (*enemies)[i]->geometry->moveForward((float)((double)delta * 2.4)); break;
                    case 4: (*enemies)[i]->geometry->moveForward((float)((double)delta * 2.3)); break;
                    }
                }
                PaintCanvas *canvas = Globals::Canvas;
                Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
                vector_0x40 = (*enemies)[2]->geometry->getDirection();
                vector_0x28 += vector_0x40 * ((float)delta * 1.96000003814697265625f);
                vector_0x40 = (*enemies)[2]->geometry->getRightVector();
                vector_0x28 += vector_0x40 * ((float)delta * 0.02999999932944774627685546875f);
                vector_0x40 = (*enemies)[2]->geometry->getUpVector();
                vector_0x28 += vector_0x40 * (advance * 0.0040000001899898052215576171875f);
                m_pCamera->setFixed(true);
                AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
                m_pCamera->setLocal(local);
                if (scriptTimerA >= 10001) {
                    for (unsigned i = 2; i != 5; ++i)
                        static_cast<PlayerFighter *>((*enemies)[i])->setAIDisabled(false);
                    player->setFreeze(false);
                    player->setVisible(true);
                    m_pCamera->setFixed(false);
                    m_pCamera->setTarget(player->geometry);
                    m_pCamera->setLookAtCam(false);
                    Vector position = player->getPosition();
                    Vector direction = player->GetDirVector();
                    m_pCamera->setPosition(position - direction * 2400.0f);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    ++m_nState;
                    scriptTimerA = 0;
                }
                break;
            }
            case 8:
                scriptTimerA += delta;
                if (scriptTimerA >= 60001)
                    m_nState = 9;
                break;
            case 9:
                scriptTimerA += delta;
                if (scriptTimerA >= 110001) {
                    scriptTimerA = 0;
                    Vector light = VectorNormalize(m_pLevel->getStarSystem()->getLightDirection());
                    Vector position = player->geometry->getPosition();
                    int offset = 10000;
                    for (unsigned i = 2; i != 5; ++i, offset += 5000) {
                        if ((*enemies)[i]->isDead()) {
                            (*enemies)[i]->revive();
                            KIPlayer *enemy = (*enemies)[i];
                            int x = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(1000);
                            int y = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(1000);
                            int z = static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(1000);
                            enemy->setPosition((position.x + light.x * 100000.0f) + (float)offset + (float)x,
                                (float)y, position.z + light.z * 100000.0f + (float)z);
                            (*enemies)[i]->geometry->setDirection(-light, Vector{0.0f, 1.0f, 0.0f});
                            (*enemies)[i]->geometry->translate((*enemies)[i]->geometry->getRightVector() * -60000.0f);
                        }
                    }
                }
                if (player->getHitpoints() >= 1 && player->getRoute()) {
                    if (*reinterpret_cast<uint8_t *>(reinterpret_cast<char *>(player->getRoute()->getWaypoint(0)) + 304)) {
                        static_cast<Player *>(player->player)->setVulnerable(false);
                        player->stopBoost();
                        ++m_nState;
                        scriptTimerA = 0;
                    }
                }
                break;
            case 10:
                if (static_cast<RadioMessage *>((*messages)[7])->isOver()) {
                    player->setTurretMode(false);
                    resetCamera(m_pLevel);
                    if (player->isInRocketControl()) {
                        player->setRocketControl(nullptr, nullptr);
                        player->killLiberator();
                        m_pCamera->setRumblePercentage(0.0f, 0);
                    }
                    player->setFreeLookMode(false);
                    m_pCamera->enableFirstPersonCam(false);
                    player->hideShipForFirstPersonCameraView(false);
                    static_cast<Player *>(player->player)->setVulnerable(false);
                    m_pHud->visible = 0;
                    m_pRadar->enabled = 0;
                    player->stopShooting(0);
                    cameraControlled = 1;
                    m_pCamera->setLookAtCam(true);
                    player->setComputerControlled(true);
                    m_pCamera->setPosition(player->geometry->getPosition());
                    Vector light = VectorNormalize(m_pLevel->getStarSystem()->getLightDirection());
                    player->geometry->setDirection(light, Vector{0.0f, 1.0f, 0.0f});
                    vector_0x28 = player->geometry->getDirection();
                    vector_0x28 *= 8000.0f;
                    Vector offset{0.0f, 0.0f, 0.0f};
                    offset = player->geometry->getRightVector();
                    offset *= 1000.0f;
                    vector_0x28 += offset;
                    offset = player->geometry->getUpVector();
                    offset *= 1000.0f;
                    vector_0x28 += offset;
                    vector_0x28 += player->geometry->getPosition();
                    m_pCamera->setPosition(vector_0x28.x, vector_0x28.y, vector_0x28.z);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    scriptTimerA = 0;
                    ++m_nState;
                }
                break;
            case 11:
                scriptTimerA += delta;
                if (scriptTimerA >= 2501) {
                    if (!m_pGeometry6) {
                        m_pGeometry6 = new AEGeometry(0x37a7, Globals::Canvas, false);
                        m_pGeometry6->setPosition(player->geometry->getPosition());
                        m_pGeometry6->setDirection(player->geometry->getDirection(), Vector{0.0f, 1.0f, 0.0f});
                        m_pCamera->setTarget(m_pGeometry6);
                        Globals::sound->play(14, nullptr, nullptr, 0.0f);
                        Vector position = *m_pCamera->getPosition();
                        Vector velocity{0.0f, 0.0f, 0.0f};
                        Globals::sound->updateEvent3DAttributes(14, &position, &velocity, false);
                    }
                    if (scriptTimerA >= 9001) {
                        if (scriptTimerA - delta <= 9000)
                            Globals::sound->play(2248, nullptr, nullptr, 0.0f);
                        AEGeometry *planet = (*static_cast<Array<AEGeometry *> *>(m_pLevel->getStarSystem()->getPlanets()))[0];
                        planet->setScaling(planet->getScaling() * 0.949999988079071044921875f);
                        if (scriptTimerA >= 10001) {
                            Globals::layout->startFade(true, -1, 500);
                            ++m_nState;
                            scriptTimerA = 0;
                        }
                    }
                }
                break;
            case 12: {
                AEGeometry *planet = (*static_cast<Array<AEGeometry *> *>(m_pLevel->getStarSystem()->getPlanets()))[0];
                planet->setScaling(planet->getScaling() * 4.0f);
                m_pCamera->setRumblePercentage(50.0f, 10);
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    KIPlayer *enemy = (*enemies)[i];
                    if (enemy && enemy->shipGroup == 10) {
                        Vector position = player->getPosition();
                        enemy->initPush(position, 100000);
                    }
                }
                if (!Globals::layout->isFading()) {
                    Globals::layout->startFade(false, -1, 2000);
                    m_pCamera->setTarget(player->geometry);
                    player->geometry->setDirection(-m_pGeometry6->getDirection(), Vector{0.0f, 1.0f, 0.0f});
                    delete m_pGeometry6;
                    scriptTimerA = 0;
                    m_pGeometry6 = nullptr;
                    m_pLevel->getStarSystem()->switchSunForSupernovaExpansion();
                    ++m_nState;
                }
                break;
            }
            case 13:
                scriptTimerA += delta;
                player->rotate((float)delta / 800.0f, (float)delta / 800.0f, (float)delta / 800.0f);
                player->geometry->moveForward((float)(12 * delta));
                m_pCamera->setRumblePercentage(50.0f, 10);
                if (scriptTimerA >= 6001) {
                    Globals::status->field_64 = static_cast<Player *>(player->player)->getHitpoints();
                    Globals::status->field_5c = static_cast<Player *>(player->player)->getShieldHP();
                    Globals::status->field_60 = static_cast<Player *>(player->player)->getArmorHP();
                    Globals::status->field_68 = static_cast<Player *>(player->player)->getGammaHP();
                    Globals::status->nextCampaignMission(true);
                    Globals::status->setStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(111));
                    Globals::status->departStation(static_cast<Galaxy *>(Globals::galaxy)->getStation(111));
                    Globals::switch_to_target_setting = 1;
                    Level::initStreamOutPosition = 1;
                    static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
                }
                break;
            }
            if (m_pGeometry6)
                m_pGeometry6->moveForward((float)(13 * delta));
            break;
        case 158:
            if (m_nState == 3) {
                scriptTimerA += delta;
                scriptTimerB += delta;
                if (scriptTimerA >= 4001) {
                    scriptTimerA = 0;
                    Vector enemyPosition = (*enemies)[0]->getPosition();
                    Vector playerPosition = player->getPosition();
                    if (VectorLength(enemyPosition - playerPosition) < 25000.0f
                        && static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(100) >= 61) {
                        for (unsigned i = 1; i <= 3; ++i) {
                            if (!(*enemies)[i]->player->isActive()) {
                                (*enemies)[i]->revive();
                                (*enemies)[i]->setActive(true);
                                KIPlayer *enemy = (*enemies)[i];
                                enemy->setPosition((*enemies)[0]->getPosition());
                                (*enemies)[i]->player->turnEnemy();
                                break;
                            }
                        }
                    }
                }
                if (scriptTimerB >= 25001) {
                    scriptTimerB = 0;
                    int *flag = reinterpret_cast<int *>(reinterpret_cast<char *>((*enemies)[0]) + 320);
                    *flag = *flag == 0;
                }
            } else if (m_nState == 1) {
                long long elapsed = scriptTimerA + delta;
                float progress = (float)elapsed / -34000.0f;
                scriptTimerA = elapsed;
                m_pCamera->translate(((float)delta * 4.80000019073486328125f) * (progress + 1.0f),
                                    0.0f, ((float)-delta * 0.699999988079071044921875f) * (progress + 1.0f));
                m_pGeometry4->setDirection(VectorNormalize(Vector{9000.0f, 0.0f, -13000.0f}
                    - m_pGeometry4->getPosition()), Vector{0.0f, 1.0f, 0.0f});
                if (scriptTimerA <= 29999)
                    m_pGeometry4->moveForward((float)delta * 2.5f);
                if (scriptTimerA > 29999 || scriptTimerA > 25000) {
                    if (!(*enemies)[0]->isVisible()) {
                        (*enemies)[0]->setVisible(true);
                        (*enemies)[0]->setActive(true);
                    }
                }
                (*enemies)[0]->geometry->setPosition(m_pGeometry4->getPosition());
                m_pCamera->setTarget((*enemies)[0]->geometry);
                player->geometry->rotate((float)delta / 2000.0f, (float)delta / 2000.0f, (float)delta / 2000.0f);
                if (scriptTimerA >= 20000) {
                    static_cast<PlayerFighter *>((*enemies)[0])->setExhaustVisible(false);
                    if (scriptTimerA >= 34000) {
                        scriptTimerA = 0;
                        scriptTimerB = 0;
                        static_cast<PlayerFighter *>((*enemies)[0])->setExhaustVisible(true);
                        m_nState = 2;
                    }
                }
            } else if (m_nState == 2) {
                if (scriptTimerA <= 2999)
                    scriptTimerA += delta;
                scriptTimerB += delta;
                (*enemies)[0]->geometry->moveForward((float)scriptTimerA * 0.100000001490116119384765625f);
                if (scriptTimerB >= 12001 && static_cast<RadioMessage *>((*messages)[2])->isOver()) {
                    field_0x24 = 0;
                    static_cast<PlayerEgo *>(m_pLevel->getPlayer())->resetGunDelay();
                    m_pCamera->setLookAtCam(false);
                    m_pCamera->setTarget(player->geometry);
                    player->setComputerControlled(false);
                    player->setFreeze(false);
                    player->setVisible(true);
                    player->setSpeed(2.0f);
                    m_pHud->visible = 1;
                    m_pRadar->enabled = 1;
                    static_cast<Player *>(player->player)->setVulnerable(true);
                    resetCamera(m_pLevel);
                    m_pLevel->lodManager->forceUpdate(delta, false);
                    cameraControlled = 0;
                    scriptTimerA = 0;
                    scriptTimerB = 0;
                    static_cast<PlayerFighter *>((*enemies)[0])->setCloakingPossible(true);
                    (*enemies)[0]->player->setAlwaysEnemy(true);
                    static_cast<PlayerFighter *>((*enemies)[0])->setAIDisabled(false);
                    Vector playerPosition = player->getPosition();
                    Vector enemyPosition = (*enemies)[0]->getPosition();
                    player->geometry->setDirection(VectorNormalize(-(playerPosition - enemyPosition)),
                                                  Vector{0.0f, 1.0f, 0.0f});
                    m_nState = 3;
                }
            }
            break;
        default:
            break;
        }
    }

    // The following independent mission-type-163 tail is Android LABEL_1383 / 0x144e3e..0x144fd0.
    if (flag_0xa9) {
        if (m_nState == 0) {
            Vector position = player->getPosition();
            Vector target = (*enemies)[0]->getPosition();
            Vector separation = position - target;
            if ((int)AbyssEngine::AEMath::VectorLength(separation) <= 49999
                && !static_cast<Player *>(player->player)->field_5e) {
                for (unsigned i = 0; i < enemies->size_; ++i) {
                    (*enemies)[i]->player->setAlwaysEnemy(true);
                    if (i && (*enemies)[i - 1]->field_0x3e)
                        break;
                }
                Level *level = m_pLevel;
                int race = Globals::status->getSystem()->getRace();
                level->createRadioMessage(14, race);
                m_nState = 1;
            }
        }
        if (m_nState <= 1 && (*enemies)[0]->player->getHitpoints() <= 0) {
            for (unsigned i = 1; i < enemies->size_; ++i) {
                if ((*enemies)[i]->field_0x3e)
                    (*enemies)[i]->player->damage(9999999);
            }
            for (unsigned i = 0; i < Globals::status->field_90->size_; ++i) {
                int station = (*Globals::status->field_90)[i];
                if (station == Globals::status->getStation()->getIndex())
                    (*Globals::status->field_90)[i] = -1;
            }
            Level *level = m_pLevel;
            int race = Globals::status->getSystem()->getRace();
            level->createRadioMessage(15, race);
            player->setAutoPilot(nullptr);
            player->setRoute(nullptr);
            m_pLevel->setPlayerRoute(nullptr);
            m_nState = 2;
        }
    }
    // Android's normal exit reads +0x11 independently of the sequence flag.
    return cameraControlled != 0;
}

void LevelScript::lookBehind() {
    m_pCamera->setTargetOffset(Vector{0.0f, 0.0f, -950.0f});
    m_pCamera->setCamOffset(Vector{0.0f, 600.0f, 2230.0f});
}

LevelScript::LevelScript(Level *level, Hud *hud, Radar *radar, TargetFollowCamera *camera)
    : field_0x28(0), field_0x2c(0), field_0x30(0), field_0x34(0),
      field_0x38(0), field_0x3c(0), field_0x40(0), field_0x44(0), field_0x48(0) {
    // Android 0x135b06..0x135b20 constructs the member Matrix once, after
    // clearing the two vector spans. iOS 0x130a1c has the same member order.
    m_pHud = hud;
    m_pRadar = radar;
    m_pCamera = camera;
    m_pLevel = level;
    hud->drawTitleImage(false);

    m_nState = 0;
    sequenceState = 1;
    m_nFlags = 0x100;
    int timeLimit = level->getTimeLimit();

    field_0x8c = 0;
    m_nScriptTimerA = 0;
    m_nScriptCounterA = 0;
    // Android VST1.16 clears +0x9a..+0xa9, leaving +0x98..+0x99 alone.
    timerBHi = 0;
    m_nScriptCounterB = 0;
    field_0xa0 = 0;
    field_0xa4 = 0;
    m_bRenderParticles = 0;
    flag_0xa9 = 0;
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
    m_nTimeLimit = timeLimit;
    field_0x24 = 0;
    m_pGeometry5 = 0;
    m_pGeometry6 = 0;
    field_0x12 = 0;

    camera->setLookAtCam(true);

    // Android 0x135b9a..0x135bc6: PlayerEgo owns a separate Player at +0.
    if (level->getPlayer() == 0) {
        m_bStartSequence = 0;
    } else {
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(false);
    }
    level->getPlayer()->setCollide(false);

    // Android 0x135c8c..0x135d80; iOS sub_130A1C, mission-zero branch.
    if (Globals::status->getCurrentCampaignMission() == 0) {
        m_nScriptTimerA = 0;
        m_nScriptCounterA = 0;
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setComputerControlled(true);
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(true);
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
        camera->setPosition(Vector{-1000.0f, -500.0f, -40000.0f});
        level->getPlayer()->setPosition(0.0f, 0.0f, -60000.0f);
        level->getPlayer()->geometry->setDirection(Vector{0.0f, 0.0f, 1.0f},
                                                  Vector{0.0f, 1.0f, 0.0f});
        for (int i = 0; i != 3; ++i) {
            (*level->getEnemies())[i]->setToSleep();
            (*level->getEnemies())[i]->geometry->setVisible(false);
            // STR +0x128 is a derived fighter field. Its current C++ class
            // duplicates the base payload; do not use its mislocated member.
            *reinterpret_cast<int *>(reinterpret_cast<char *>((*level->getEnemies())[i]) + 0x128) = 0;
            (*level->getEnemies())[i]->player->enemyFlagsLo = 0;
        }
        level->lodManager->forceUpdate(30, false);
        m_pGeometry5 = new AEGeometry(0x3ab3, Globals::Canvas, false);
    } else if (Globals::status->getCurrentCampaignMission() == 1) {
        // Android 0x135bee..0x135c8a; iOS sub_130A1C mission one.
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setRotation(0.462f, 0.462f, 1.5339f);
        level->getPlayer()->setFreeze(true);
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->setPosition(0.0f, 0.0f, 0.0f);
        level->getPlayer()->setExhaustVisible(false);
        level->getPlayer()->StopEngineSound();
        (*level->getEnemies())[0]->StopEngineSound();
        camera->setPosition(1500.0f, 1600.0f, -3000.0f);
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
        camera->setTarget(level->getPlayer()->geometry);
        Globals::layout->startFade(false, 255, 5000);
    } else if (Globals::status->getCurrentCampaignMission() == 81 && Globals::status->inAlienOrbit()) {
        // Android LevelScript C1 mission 81; iOS sub_130A1C mission 81.
        scriptTimerA = 0;
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->freeze = 1;
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(false);
        level->getPlayer()->setVisible(false);
        FModSound *sound = Globals::sound;
        sound->stop(level->getPlayer()->field_0x1c);
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
        camera->setPosition(Vector{-20000.0f, 800.0f, 120000.0f});
        level->getPlayer()->setPosition(0.0f, 0.0f, 0.0f);
        // Enemy +0x140 is the station geometry, distinct from KIPlayer::geometry.
        auto *stationGeometry = *reinterpret_cast<AEGeometry **>(
            reinterpret_cast<char *>((*level->getEnemies())[0]) + 0x140);
        stationGeometry->setVisible(false);
        level->lodManager->forceUpdate(30, false);
        m_pGeometry5 = new AEGeometry(0x3ab3, Globals::Canvas, false);
        m_pGeometry5->setScaling(30.0f, 30.0f, 30.0f);
    } else if (Globals::status->getCurrentCampaignMission() == 78) {
        // Android LevelScript C1 mission 78; iOS sub_130A1C mission 78.
        camera->setPosition(2000.0f, -1500.0f, 16000.0f);
        field_0x24 = 0;
        sequenceState = 0x100;
        level->getPlayer()->geometry->setDirection(Vector{0.0f, 0.0f, 1.0f},
                                                  Vector{0.0f, 1.0f, 0.0f});
        cameraControlled = 1;
        m_nState = 1;
        m_pGeometry5 = new AEGeometry(0x3ab3, Globals::Canvas, false);
        m_pGeometry5->setScaling(20.0f, 20.0f, 20.0f);
    } else if (Globals::status->getCurrentCampaignMission() == 89) {
        // Android C1 mission 89, 0x135fca..0x136170; iOS sub_130A1C.
        scriptTimerA = 0;
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->freeze = 1;
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(false);
        level->getPlayer()->setVisible(false);
        FModSound *sound = Globals::sound;
        sound->stop(level->getPlayer()->field_0x1c);
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
        m_pGeometry4 = new AEGeometry(Globals::Canvas);
        m_pGeometry4->setPosition((*level->getEnemies())[1]->getPosition());
        m_pGeometry4->setDirection(-static_cast<StarSystem *>(level->getStarSystem())->getLightDirection(),
                                   Vector{0.0f, 1.0f, 0.0f});
        m_pGeometry4->moveForward(-100000.0f);
        m_pGeometry4->translate(m_pGeometry4->getRightVector() * 200000.0f);
        camera->setTarget(m_pGeometry4);
        camera->setPosition((*level->getEnemies())[1]->getPosition());
        camera->translate(10000.0f, 1500.0f, -20000.0f);
        AEGeometry *enemy = (*level->getEnemies())[0]->geometry;
        enemy->setPosition(Vector{-50000.0f, 800.0f, 120000.0f}
                          + 1000.0f * static_cast<StarSystem *>(level->getStarSystem())->getLightDirection());
        enemy = (*level->getEnemies())[0]->geometry;
        enemy->setDirection(static_cast<StarSystem *>(level->getStarSystem())->getLightDirection(),
                            Vector{0.0f, 1.0f, 0.0f});
        (*level->getEnemies())[0]->geometry->translate(Vector{3000.0f, 0.0f, 3000.0f});
        m_pGeometry6 = new AEGeometry(0x3795, Globals::Canvas, false);
        m_pGeometry6->setScaling(1.0f, 1.0f, 500000.0f);
        m_nState = 1;
        level->lodManager->forceUpdate(30, false);
    } else if (Globals::status->getCurrentCampaignMission() == 105 && Globals::status->inSupernovaOrbit()) {
        // Android C1 mission 105, 0x13617e..0x136296; iOS sub_130A1C.
        cameraControlled = 1;
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->setCollide(false);
        sequenceState = 0x100;
        camera->setLookAtCam(true);
        camera->setTarget(level->getPlayer()->geometry);
        camera->setPosition((*level->getEnemies())[1]->getPosition());
        camera->translate(5000.0f, 1500.0f, -20000.0f);
        Vector position = level->getPlayer()->getPosition();
        Vector direction = level->getPlayer()->GetDirVector() * -2400.0f;
        Vector cameraPosition = position + direction;
        Vector up = level->getPlayer()->GetUpVector() * 500.0f;
        camera->setPosition(cameraPosition + up);
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(true);
        if (!Level::doInstantJump)
            setAutoPilotToProgrammedStation();
        m_nState = 1;
        scriptTimerA = 0;
    } else if (Globals::status->getCurrentCampaignMission() == 106) {
        // Android C1 mission 106, 0x136314..0x136402; iOS sub_130A1C.
        level->getPlayer()->setPosition(300000.0f, 0.0f, 300000.0f);
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->freeze = 1;
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(false);
        Vector direction = AbyssEngine::AEMath::VectorNormalize(
            static_cast<StarSystem *>(level->getStarSystem())->getLightDirection());
        level->getPlayer()->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        FModSound *sound = Globals::sound;
        sound->stop(level->getPlayer()->field_0x1c);
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
        camera->setPosition(level->getPlayer()->getPosition() + Vector{2200.0f, 1300.0f, -4000.0f});
        cameraControlled = 1;
        Globals::layout->startFade(false, -1, 8000);
        m_nState = 1;
    } else if (Globals::status->getCurrentCampaignMission() == 144) {
        // Android C1 mission 144, 0x136a24..0x136b46; iOS sub_130A1C.
        m_pGeometry4 = new AEGeometry(Globals::Canvas);
        m_pGeometry4->setPosition(30000.0f, 10000.0f, 160000.0f);
        m_pGeometry4->setDirection(Vector{1.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
        cameraControlled = 1;
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->setCollide(false);
        level->getPlayer()->setActive(false);
        level->getPlayer()->setVisible(false);
        level->getPlayer()->setFreeze(true);
        sequenceState = 0x100;
        FModSound *sound = Globals::sound;
        sound->stop(level->getPlayer()->field_0x1c);
        camera->setLookAtCam(true);
        camera->setTarget(m_pGeometry4);
        vector_0x28 = (*level->getEnemies())[0]->getPosition();
        vector_0x28 += Vector{-4600.0f, 1000.0f, -4300.0f};
        camera->setPosition(vector_0x28);
        if (!Level::doInstantJump)
            setAutoPilotToProgrammedStation();
        m_nState = 1;
        scriptTimerA = 0;
    } else if (Globals::status->getCurrentCampaignMission() == 131
               && !Globals::status->inAlienOrbit()
               && Globals::status->getStation()->getIndex() == 112) {
        // Android C1 0x136b52..0x136d4a; iOS sub_130A1C station-112 branch.
        cameraControlled = 1;
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->setCollide(false);
        level->getPlayer()->setPosition(-65000.0f, 0.0f, 95000.0f);
        AEGeometry *geometry = level->getPlayer()->geometry;
        Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
        geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        sequenceState = 0x100;
        camera->setLookAtCam(true);
        camera->setTarget(level->getPlayer()->geometry);
        Vector position = level->getPlayer()->getPosition();
        Vector backward = level->getPlayer()->GetDirVector() * 2400.0f;
        Vector cameraPosition = position - backward;
        Vector up = level->getPlayer()->GetUpVector() * 500.0f;
        Vector raisedPosition = cameraPosition + up;
        Vector right = level->getPlayer()->geometry->getRightVector() * 600.0f;
        camera->setPosition(raisedPosition - right);
        camera->update(10);
        PaintCanvas *canvas = Globals::Canvas;
        Matrix local = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
        vector_0x28 = AbyssEngine::AEMath::MatrixGetPosition(local);
        camera->setFixed(true);
        AbyssEngine::AEMath::MatrixSetTranslation(local, vector_0x28);
        camera->setLocal(local);
        if (!Level::doInstantJump)
            setAutoPilotToProgrammedStation();
        m_nState = 1;
        scriptTimerA = 0;
    } else if (Globals::status->getCurrentCampaignMission() == 158
               && !Globals::status->inAlienOrbit()
               && Globals::status->getStation()->getIndex() == 111) {
        // Android C1 0x136da4..0x1370b2; iOS sub_130A1C station-111 branch.
        m_pGeometry4 = new AEGeometry(Globals::Canvas);
        m_pGeometry4->setDirection(Vector{1.0f, 0.0f, -1.0f}, Vector{0.0f, 1.0f, 0.0f});
        sequenceState = 0x100;
        level->getPlayer()->setPosition(-50000.0f, 0.0f, 15000.0f);
        hud->visible = 0;
        radar->enabled = 0;
        level->getPlayer()->setFreeze(true);
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(false);
        Vector light = AbyssEngine::AEMath::VectorNormalize(
            static_cast<StarSystem *>(level->getStarSystem())->getLightDirection());
        level->getPlayer()->geometry->setDirection(light, Vector{0.0f, 1.0f, 0.0f});
        FModSound *sound = Globals::sound;
        sound->stop(level->getPlayer()->field_0x1c);
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
        camera->setPosition(level->getPlayer()->getPosition() + Vector{-28000.0f, 500.0f, 8500.0f});
        m_pGeometry4->setPosition(*camera->getPosition());
        m_pGeometry4->setDirection(AbyssEngine::AEMath::VectorNormalize(light), Vector{0.0f, 1.0f, 0.0f});
        m_pGeometry4->moveForward(50000.0f);
        camera->setTarget(m_pGeometry4);
        cameraControlled = 1;
        Globals::layout->startFade(false, -1, 8000);
        Array<KIPlayer *> *enemies = level->getEnemies();
        (*enemies)[0]->player->setEnemies(nullptr);
        (*enemies)[0]->player->addEnemy(static_cast<Player *>(level->getPlayer()->player));
        for (unsigned i = 1; i != 4; ++i) {
            (*enemies)[i]->player->setEnemies(nullptr);
            (*enemies)[i]->player->setEnemy(static_cast<Player *>(level->getPlayer()->player));
            Gun *gun = (*(*(*enemies)[i]->player->guns)[0])[0];
            gun->damage = (int)((float)gun->damage * 0.3f);
            Array<KIPlayer *> *friends = level->field_b0;
            if (friends) {
                for (unsigned j = 0; j < friends->size_; ++j) {
                    (*enemies)[i]->player->addEnemy((*friends)[j]->player);
                    (*level->field_b0)[j]->player->addEnemy((*enemies)[i]->player);
                    friends = level->field_b0;
                }
            }
        }
        (*enemies)[0]->setActive(false);
        (*enemies)[0]->setVisible(false);
        PlayerEgo *player = level->getPlayer();
        player->setPosition(m_pGeometry4->getPosition());
        AEGeometry *geometry = level->getPlayer()->geometry;
        Vector *cameraPosition = camera->getPosition();
        Vector helperPosition = m_pGeometry4->getPosition();
        Vector direction = AbyssEngine::AEMath::VectorNormalize(
            (*cameraPosition - helperPosition) + Vector{20000.0f, 0.0f, 0.0f});
        geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        level->getPlayer()->geometry->moveForward(30000.0f);
        level->getPlayer()->setSpeed(0.1f);
        m_nState = 1;
    } else {
        // Android C1 0x1370f8..0x137756; iOS sub_130A1C default departure.
        int range = Level::initStreamOutPosition ? 500 : 2000;
        bool streamOut = Level::initStreamOutPosition != 0;
        vector_0x28.x = (float)(static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(range) + 500);
        vector_0x28.y = (float)(static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(range) + 500);
        // Android literal words at 0x137b34/38 are 7000/9000, not guessed distances.
        vector_0x28.z = streamOut ? 7000.0f : 9000.0f;
        if (!static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2))
            vector_0x28.x = -vector_0x28.x;
        if (!static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(2))
            vector_0x28.y = -vector_0x28.y;
        if (Globals::status->getCurrentCampaignMission() == 48) {
            level->getPlayer()->setPosition(0.0f, 0.0f, 55000.0f);
            Vector direction = level->getPlayer()->geometry->getDirection();
            direction.x += 0.00001f;
            level->getPlayer()->geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            PlayerEgo *player = level->getPlayer();
            player->setAutoPilot((*level->getLandmarks())[0]);
        } else if (Globals::status->getCurrentCampaignMission() == 65 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(0.0f, 0.0f, 170000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
            Vector position = level->getPlayer()->getPosition();
            position.x += -3000.0f;
            (*level->getEnemies())[0]->setPosition(position.x, position.y, position.z);
            (*level->getEnemies())[0]->geometry->setRotation(Vector{0.0f, 3.1415927410125732421875f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 87 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(0.0f, 0.0f, 210000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 91 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(0.0f, 0.0f, -130000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 92 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(0.0f, 0.0f, 200000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector target = (*level->getEnemies())[3]->getPosition();
            Vector position = level->getPlayer()->getPosition();
            geometry->setDirection(AbyssEngine::AEMath::VectorNormalize(target - position), Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 102 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(90000.0f, 6000.0f, 150000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 123 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(0.0f, 0.0f, 180000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        } else if ((Globals::status->getCurrentCampaignMission() > 84
                    || Globals::status->getCurrentCampaignMission() == 135)
                   && !Globals::status->inAlienOrbit() && Globals::status->getStation()->getIndex() == 103) {
            level->getPlayer()->setPosition(70000.0f, 0.0f, 100000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 114 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(0.0f, 0.0f, -120000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 137 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(20000.0f, 0.0f, 150000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector direction = AbyssEngine::AEMath::VectorNormalize(-level->getPlayer()->getPosition());
            geometry->setDirection(direction, Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 139 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(20000.0f, 0.0f, 190000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector target = (*level->getEnemies())[1]->getPosition();
            Vector position = level->getPlayer()->getPosition();
            geometry->setDirection(AbyssEngine::AEMath::VectorNormalize(target - position), Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->getCurrentCampaignMission() == 142 && !Globals::status->inAlienOrbit()
                   && Globals::status->getStation()->getIndex()
                      == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
            level->getPlayer()->setPosition(60000.0f, 0.0f, 120000.0f);
            AEGeometry *geometry = level->getPlayer()->geometry;
            Vector target = level->getPlayer()->getRoute()->getWaypoint(0)->getPosition();
            Vector position = level->getPlayer()->getPosition();
            geometry->setDirection(AbyssEngine::AEMath::VectorNormalize(target - position), Vector{0.0f, 1.0f, 0.0f});
            KIPlayer *enemy = (*level->getEnemies())[0];
            Vector playerPosition = level->getPlayer()->getPosition();
            Vector right = level->getPlayer()->geometry->getRightVector() * 2100.0f;
            Vector offset = playerPosition + right;
            Vector direction = level->getPlayer()->geometry->getDirection() * 2000.0f;
            enemy->setPosition(offset + direction); // Android virtual slot +0x44.
            geometry = (*level->getEnemies())[0]->geometry;
            geometry->setDirection(level->getPlayer()->GetDirVector(), Vector{0.0f, 1.0f, 0.0f});
        } else if (Globals::status->inAlienOrbit() && Globals::status->getCurrentCampaignMission() == 154) {
            level->getPlayer()->setPosition(0.0f, 0.0f, 140000.0f);
            KIPlayer *enemy = (*level->getEnemies())[0];
            Vector playerPosition = level->getPlayer()->getPosition();
            Vector right = level->getPlayer()->geometry->getRightVector() * 2600.0f;
            Vector offset = playerPosition + right;
            Vector direction = level->getPlayer()->geometry->getDirection() * 4000.0f;
            enemy->setPosition(offset + direction);
            AEGeometry *geometry = (*level->getEnemies())[0]->geometry;
            geometry->setDirection(level->getPlayer()->GetDirVector(), Vector{0.0f, 1.0f, 0.0f});
            Player *enemyPlayer = (*level->getEnemies())[0]->player;
            enemyPlayer->setEnemy((*level->getEnemies())[1]->player);
            for (unsigned i = 0; i < level->getEnemies()->size_; ++i) {
                if ((*level->getEnemies())[i]->field_0x3e)
                    (*level->getEnemies())[i]->player->setEnemies(nullptr);
                if ((*level->getEnemies())[i]->shipGroup == 9)
                    (*level->getEnemies())[i]->field_0x25 = 0;
            }
        }
        Matrix local = level->getPlayer()->geometry->getMatrix();
        vector_0x34 = AbyssEngine::AEMath::MatrixRotateVector(local, vector_0x28);
        vector_0x34 += level->getPlayer()->geometry->getPosition();
        camera->setPosition(vector_0x34);
        bool alienOrbit = Globals::status->inAlienOrbit();
        if (Level::comingFromAlienWorld || alienOrbit) {
            if (Globals::status->getCurrentCampaignMission() <= 42) {
                vector_0x28 = level->getPlayer()->getPosition();
                vector_0x40 = level->getPlayer()->geometry->getDirection();
                vector_0x40 *= 10000.0f;
                vector_0x28 -= vector_0x40;
                static_cast<PlayerWormHole *>((*level->getLandmarks())[3])->reset(true);
                (*level->getLandmarks())[3]->setPosition(vector_0x28.x, vector_0x28.y, vector_0x28.z);
                (*level->getLandmarks())[3]->setVisible(true);
            }
            Level::comingFromAlienWorld = 0;
        }
    }

    // Android constructor join 0x136406; iOS sub_130A1C final join.
    if (Globals::status->getCurrentCampaignMission() == 80) {
        m_pGeometry0 = new AEGeometry(0x3822, Globals::Canvas, false);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            m_pGeometry0->transform))->boundingRadius = 1000000.0f;
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            m_pGeometry0->transform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
        m_pGeometry0->setVisible(false);
        m_pGeometry0->setRotation(Vector{0.0f, 3.1415927410125732421875f, 0.0f});
        // Android station geometry +0x140 and animation transform +0x144.
        // The current derived station layout is not yet a native ABI model.
        AEGeometry *station = *reinterpret_cast<AEGeometry **>(
            reinterpret_cast<char *>((*level->getLandmarks())[0]) + 0x140);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            station->transform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            station->childTransform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            station->parentTransform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
        PaintCanvas *canvas = Globals::Canvas;
        uint32_t animationTransform = *reinterpret_cast<uint32_t *>(
            reinterpret_cast<char *>((*level->getLandmarks())[0]) + 0x144);
        static_cast<AbyssEngine::Transform *>(canvas->TransformGetTransform(animationTransform))
            ->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(2), nullptr);
        m_pExplosion = new Explosion(0);
        m_pExplosion->addFireStreaks();
        m_pExplosion->setScaling(3.0f);
        m_pGeometry6 = new AEGeometry(Globals::Canvas);
        m_pGeometry6->setVisible(false);
        m_pGeometry1 = new AEGeometry(0x381e, Globals::Canvas, false);
        AEGeometry *enemyStation = *reinterpret_cast<AEGeometry **>(
            reinterpret_cast<char *>((*level->getEnemies())[0]) + 0x140);
        m_pGeometry1->setPosition(enemyStation->getPosition());
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            m_pGeometry1->transform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
        m_pGeometry5 = new AEGeometry(0x3ab3, Globals::Canvas, false);
        m_pGeometry5->setScaling(20.0f, 20.0f, 20.0f);
    }
    if (Globals::status->getCurrentCampaignMission() == 41 && Globals::status->inAlienOrbit()) {
        // Android 0x1365b4..0x1366aa: set IDs 0x28 and 0x29, emit initially disabled.
        ParticleSystemManager *manager = level->field_74;
        particleSystem_0xe0 = manager->addSystem(&(*level->getEnemies())[0]->geometry->getReferenceMatrix(),
                                                static_cast<ParticleSettings::ParticleSet>(0x28), false);
        level->field_74->enableSystemEmit(particleSystem_0xe0, false);
        manager = level->field_74;
        particleSystem_0xe4 = manager->addSystem(&(*level->getEnemies())[0]->geometry->getReferenceMatrix(),
                                                static_cast<ParticleSettings::ParticleSet>(0x29), false);
        level->field_74->enableSystemEmit(particleSystem_0xe4, false);
        level->lodManager->forceUpdate(30, false);
        m_pParticleGeom0 = new AEGeometry(0x37cd, Globals::Canvas, false);
        m_pParticleGeom1 = new AEGeometry(0x37ce, Globals::Canvas, false);
        m_pParticleGeom2 = new AEGeometry(0x37cf, Globals::Canvas, false);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            m_pParticleGeom0->transform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            m_pParticleGeom1->transform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(
            m_pParticleGeom2->transform))->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
    } else if (Globals::status->getCurrentCampaignMission() == 157 && !Globals::status->inAlienOrbit()
               && Globals::status->getStation()->getIndex()
                  == reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getTargetStation()) {
        // Android 0x136728..0x136872; child transforms survive temporary wrapper deletion.
        m_pGeometry1 = new AEGeometry(0x4a75, Globals::Canvas, false);
        AEGeometry *child0 = new AEGeometry(0x4a76, Globals::Canvas, false);
        AEGeometry *child1 = new AEGeometry(0x4a77, Globals::Canvas, false);
        m_pGeometry1->addChild(child0->transform);
        m_pGeometry1->addChild(child1->transform);
        m_pGeometry2 = new AEGeometry(0x4a97, Globals::Canvas, false);
        m_pGeometry3 = new AEGeometry(0x4a99, Globals::Canvas, false);
        AEGeometry *child2 = new AEGeometry(0x4a98, Globals::Canvas, false);
        AEGeometry *child3 = new AEGeometry(0x4a9a, Globals::Canvas, false);
        m_pGeometry2->addChild(child2->transform);
        m_pGeometry3->addChild(child3->transform);
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(m_pGeometry1->transform))
            ->SetVisible(false);
        m_pGeometry2->setVisible(false);
        m_pGeometry3->setVisible(false);
        delete child0;
        delete child1;
        delete child2;
        delete child3;
    }
    Globals::layout->enableFillScreen(false);
    if (!Globals::status->inAlienOrbit() && Globals::status->getCampaignMission()
        && reinterpret_cast<Mission *>(static_cast<intptr_t>(Globals::status->getCampaignMission()))->getType() == 163) {
        Status *status = Globals::status;
        Array<int> *stations = status->field_90;
        if (stations) {
            for (unsigned i = 0; i < stations->size_; ++i) {
                int index = (*stations)[i];
                if (index == status->getStation()->getIndex()) {
                    flag_0xa9 = 1;
                    break;
                }
                status = Globals::status;
                stations = status->field_90;
            }
        }
    }
    if (Globals::status->getCurrentCampaignMission() == 95
        || Globals::status->getCurrentCampaignMission() == 99
        || Globals::status->getCurrentCampaignMission() == 109
        || Globals::status->getCurrentCampaignMission() == 119
        || Globals::status->getCurrentCampaignMission() == 126
        || Globals::status->getCurrentCampaignMission() == 133
        || Globals::status->getCurrentCampaignMission() == 160
        || Globals::status->getCurrentCampaignMission() == 161) {
        // Android LevelScript C1 final mission-ID chain; iOS sub_130A1C
        // has the same IDs and camera constants.
        hud->visible = 0;
        radar->enabled = 0;
        cameraControlled = 1;
        level->getPlayer()->setComputerControlled(true);
        level->getPlayer()->freeze = 1;
        static_cast<Player *>(level->getPlayer()->player)->setVulnerable(false);
        level->getPlayer()->setVisible(false);
        FModSound *sound = Globals::sound;
        sound->stop(level->getPlayer()->field_0x1c);
        m_bStartSequenceOver = 1;
        camera->setLookAtCam(true);
        camera->setPosition(Vector{-15000.0f, 800.0f, 75000.0f});
        level->getPlayer()->setPosition(0.0f, 0.0f, 0.0f);
        level->lodManager->forceUpdate(30, false);
        scriptTimerA = 0;
    }
}

int LevelScript::getEvent() {
    return m_nState;
}

void LevelScript::setEvent(int event) {
    m_nState = event;
}
