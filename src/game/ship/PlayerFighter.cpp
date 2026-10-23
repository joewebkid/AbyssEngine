#include "game/ship/PlayerFighter.h"
#include "engine/core/AERandom.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/Mesh.h"
#include "engine/render/Material.h"
#include "game/mission/Mission.h"
#include "engine/render/AEGeometry.h"
#include "game/mission/Generator.h"
#include "game/core/Globals.h"
#include "game/world/Level.h"
#include "game/mission/Status.h"
#include "game/world/Station.h"
#include "engine/core/GameText.h"
#include "engine/render/Trail.h"
#include "game/mission/Explosion.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerFixedObject.h"
#include "game/ship/Ship.h"
#include "game/ship/TargetFollowCamera.h"
#include "game/world/Route.h"
#include "game/world/Standing.h"
#include "game/world/SpacePoint.h"
#include "game/weapons/Radar.h"
#include "engine/math/Transform.h"
#include "engine/math/EaseInOutMatrix.h"
#include "engine/audio/FModSound.h"
#include "game/ui/Layout.h"
#include <cmath>
#include <cstring>



// PaintCanvas::gCanvas is declared extern in engine/render/PaintCanvas.h (included above).

void PF_update_body(PlayerFighter *self, int dt);

namespace AbyssEngine {
    namespace AEMath {
        float VectorLength(const Vector &value);

        Vector operator-(const Vector &lhs, const Vector &rhs);

        Vector operator*(float lhs, const Vector &rhs);
    }
}

int AERandom_nextIntB(int rng, int bound);

uint8_t PlayerFighter::hasMissionCrateLost() {
    return this->missionCrateLost;
}

uint8_t PlayerFighter::hasMissionCrateCaptured() {
    return this->missionCrateCaptured;
}

void PlayerFighter::setShootError(int v) {
    this->shootError = (float) v;
}

void PlayerFighter::setAIDisabled(bool v) {
    this->aiDisabled = v;
}

void PlayerFighter::setShipGroup(AEGeometry *geom, int group, bool flag) {
    KIPlayer::setShipGroup(geom, group, flag);
}

void PlayerFighter::awake() {
    this->state = 1;
    ((Player *) (this->player))->setActive(1);
    ((PlayerFighter *) (this))->setExhaustVisible(true);
    AEGeometry *geom = this->subGeometry;
    this->field_0xf5 = 1;
    if (geom == 0) {
        geom = this->geometry;
    }

    return geom->setVisible(true);
}

void PlayerFighter::setBV(Array<BoundingVolume *> *v) {
    this->boundingVolumes = v;
}

void PlayerFighter::setBoostProb(int v) {
    this->boostProb = v;
}

void PlayerFighter::setCloakingPossible(bool v) {
    PlayerFighter *self = this;
    self->cloakingPossible = v;
    if (!v && self->field_0x13c != 0) {
        self->cloakTimer = self->cloakDuration + 1;

        return self->handleCloaking();
    }
}

void PlayerFighter::removeTrail() {
    delete this->trail;
    this->trail = 0;
}

PlayerFighter::~PlayerFighter() {
    delete this->route;
    this->route = 0;

    if (this->boundingVolumes != 0) {
        ArrayReleaseClasses(*this->boundingVolumes); ArrayRemoveAll(*(this->boundingVolumes));
        delete this->boundingVolumes;
    }
    this->boundingVolumes = 0;

    delete this->trail;
    this->trail = 0;

    delete this->explosion;
    this->explosion = 0;

    delete this->easeMatrix;
    this->easeMatrix = 0;
}

uint8_t PlayerFighter::hasCrateCaptured() {
    return this->crateCaptured == 0;
}

void PlayerFighter::setPosition(const Vector &v) {
    this->setPosition(v.x, v.y, v.z);
}

void PlayerFighter::setRotate(int v) {
    float f = (float) v;
    this->field_0x13d = 0;
    this->rotate = f;
    this->currentRotate = f;
}

uint8_t PlayerFighter::hasCrateLost() {
    return this->crateLost;
}

void PlayerFighter::setLevel(Level *lvl) {
    KIPlayer::setLevel(lvl);
    ParticleSystemManager *manager = this->level->field_74;
    this->engineTrailSystem = manager->addSystem(&this->geometry->getReferenceMatrix(), ParticleSettings::ParticleSet_9, false);
    manager->enableSystemEmit(this->engineTrailSystem, false);

    manager = this->level->particleEmitBoolPtr;
    this->field_0x80 = manager->addSystem(&this->geometry->getReferenceMatrix(), ParticleSettings::ParticleSet_0xf, false);
    manager->enableSystemEmit(this->field_0x80, false);

    manager = this->level->particleRenderBoolPtr;
    this->field_0x84 = manager->addSystem(&this->geometry->getReferenceMatrix(), ParticleSettings::ParticleSet_0x2a, false);
    manager->enableSystemEmit(this->field_0x84, false);

    manager = this->level->field_8c;
    this->field_0x134 = manager->addSystem(&this->geometry->getReferenceMatrix(), static_cast<ParticleSettings::ParticleSet>(17), false);
    manager->enableSystemEmit(this->field_0x134, false);
    this->field_0x138 = manager->addSystem(&this->geometry->getReferenceMatrix(), static_cast<ParticleSettings::ParticleSet>(18), false);
    manager->enableSystemEmit(this->field_0x138, false);
}

PlayerFighter::PlayerFighter(int faction, int wingmanCmd, Player *player, AEGeometry *geom,
                             float x, float y, float z, bool flag)
    : KIPlayer(faction, -1, player, geom, x, y, z, flag) {
    PlayerFighter * self = this;

    self->field_0x200 = 0;
    self->field_0x204 = 0;
    self->field_0x208 = 0;

    self->workingPosition = (Vector)
    {
        0, 0, 0
    };
    self->resetVecB = (Vector)
    {
        0, 0, 0
    };
    self->resetVecC = (Vector)
    {
        0, 0, 0
    };
    AERandom *rng = static_cast<AERandom *>(Globals::rnd);

    float wp[12];
    int r;
    r = rng->nextInt(25000);
    wp[0] = static_cast<float>(r - 30000);
    int r1 = rng->nextInt(10000);
    int r2 = rng->nextInt(25000);
    wp[2] = static_cast<float>(r2 + 20000);
    wp[1] = static_cast<float>(r1 - 10000);
    r = rng->nextInt(25000);
    wp[3] = static_cast<float>(r + 5000);
    r1 = rng->nextInt(10000);
    r2 = rng->nextInt(25000);
    wp[5] = static_cast<float>(r2 + 20000);
    wp[4] = static_cast<float>(r1 - 10000);
    r = rng->nextInt(25000);
    wp[6] = static_cast<float>(r + 5000);
    r1 = rng->nextInt(10000);
    r2 = rng->nextInt(25000);
    wp[8] = static_cast<float>(r2 + 55000);
    wp[7] = static_cast<float>(r1 - 10000);
    r = rng->nextInt(25000);
    wp[9] = static_cast<float>(r - 30000);
    r1 = rng->nextInt(10000);
    r2 = rng->nextInt(25000);
    wp[10] = static_cast<float>(r1 - 10000);
    wp[11] = static_cast<float>(r2 + 55000);

    int count = rng->nextInt(3) * 3 + 6;
    char used[4] = {};
    int *pts = new int[count];
    for (int i = 0; i < count; i += 3) {
        int idx;
        do {
            idx = rng->nextInt(4);
        } while (used[idx] != 0);
        used[idx] = 1;
        pts[i] = (int) wp[idx * 3];
        pts[i + 1] = (int) wp[idx * 3 + 1];
        pts[i + 2] = (int) wp[idx * 3 + 2];
    }
    self->route = new Route(pts, (unsigned) count);
    delete[] pts;

    if (stationRouteAliens == nullptr) {
        int stationPts[12] = {
            40000, 0, 40000,
            40000, 0, -40000,
            -40000, 0, -40000,
            -40000, 0, 40000,
        };
        stationRouteAliens = new Route(stationPts, 12);
    }

    self->field_0x130 = -1;
    self->field_0x134 = -1;
    self->field_0x138 = -1;
    self->field_0x128 = 50000;
    self->rotate = 2.0f;
    self->shootError = 0.00762939f;
    self->speed = 2.0f;
    self->field_0x1b0 = 0x5dc;
    self->boostProb = 5;
    self->field_0x38 = 0;
    self->field_0x140 = 0;
    self->field_0x12d = 0;
    self->field_0x12e = 0;
    self->field_0x13d = 0;
    self->field_0x1f8 = 0;
    self->deathTimer = 0;
    self->field_0x12f = 0;
    self->field_0x1fc = 0;
    self->field_0x1c8 = 0;
    self->field_0x1e0 = 0;
    self->field_0x1f4 = 0;
    self->shipGroup = wingmanCmd;

    self->maneuverTimer = 0;
    self->field_0x1c0 = 0;
    self->field_0x1c4 = 0;
    self->field_0x1c8 = 0;
    self->field_0x148 = 0;
    self->commandRoute = 0;
    self->boundingVolumes = 0;
    self->trail = 0;
    self->deltaTime = 0;
    self->deltaTimeHi = 0;
    self->hitpoints = 0;
    self->field_0x1dc = 0;
    self->field_0x1e4 = 0;
    self->currentSpeed = 0;
    self->field_0x1ec = 0;
    self->currentRotate = 0;

    self->workingPosition.x = x;
    self->workingPosition.y = y;
    self->workingPosition.z = z;
    self->field_0x13d = 1;
    self->crateCaptured = 1;
    self->currentSpeed = self->speed;
    self->currentRotate = self->rotate;
    self->route->setLoop(true);
    stationRouteAliens->setLoop(true);
    self->routeClone = 0;

    if (Status::gStatus->getCurrentCampaignMission() != 0x29) {
        if (wingmanCmd == 9) {
            self->routeClone = stationRouteAliens->clone();
        } else {
            self->routeClone = self->route->clone();
        }
    }
    self->field_0x12d = 0;

    if (wingmanCmd == 9) {
        self->lootList = 0;
    } else {
        Generator *g = new Generator();
        self->lootList = g->getLootList(-1, -1);
        delete g;
    }

    self->field_0x128 = (Status::gStatus->inAlienOrbit() != 0) ? 100000 : 50000;

    Explosion *exp = new Explosion(0);
    self->explosion = exp;
    exp->addFireStreaks();
    self->field_0x13e = 1;
    self->hitpoints = ((Player *) (self->player))->getHitpoints();
    self->field_0x1dc = 0;
    self->field_0x1e0 = 0;
    self->field_0xe4 = 1;
    self->field_0x25 = 1;
    self->engineTrailSystem = -1;

    int fov;
    if (Status::gStatus->getCurrentCampaignMission() == 1) {
        fov = -1;
    } else {
        fov = (self->field_0xdc == 0) ? 0x2e : 0x30;
    }
    self->fov = fov;

    self->targetRoll = 0.0f;
    self->smoothRoll = 0.0f;
    self->field_0x214 = 0;
    self->field_0x294 = 750.0f;
    self->field_0x298 = 15.139357f; // ARM bits 0x41723ACE.
    self->rollSamples[0] = 0.0f;
    self->rollSamples[1] = 0.0f;
    self->rollSamples[2] = 0.0f;
    self->rollSamples[3] = 0.0f;
    self->rollSamples[4] = 0.0f;
    self->rollBufferFilled = 0;
    self->rollSampleIndex = 0;
    self->rollActive = 0;
    self->field_0x254 = 0;
    self->easeMatrix = 0;
    self->field_0x13c = 0;
    self->spacePoint = 0;
    self->cloakTimer = 0;
    self->cloakDuration = 0;
    self->cloakActive = 0;
    self->cloakCooldown = 0;
    self->field_0x2c4 = 0;
    self->cloakingPossible = 1;
    self->cloakMaterial = -1;
    self->aiDisabled = 0;
    self->gunSwitchTimer = 0;

    return;
}

void PlayerFighter::update(int dt) {
    if (this->state == 4 && this->explosion->isPlaying() == 0 &&
        (this->crateCaptured == 0 || 60000 < this->deathTimer)) {
        KIPlayer::setActive(false);
        return;
    }

    this->field_0x1c0 += dt;
    this->maneuverTimer += dt;
    if (this->shipGroup == 1) {
        this->shipGroup = 1;
    }
    this->deltaTime = dt;
    this->deltaTimeHi = dt >> 31;

    this->renderPosition = this->geometry->getPosition();

    Player *body = this->player;
    if (this->field_0x43 == 0) {
        unsigned char enemy;
        if ((this->shipGroup & 0xfffffffe) == 8) {
            enemy = 1;
        } else if (((KIPlayer *) (this))->isWingMan() != 0) {
            enemy = 0;
        } else {
            enemy = (unsigned char) Globals::status->getStanding()->isEnemy(this->shipGroup);
        }
        body->enemyFlagsLo = enemy;

        unsigned char friendly;
        if ((this->shipGroup & 0xfffffffe) == 8) {
            friendly = 0;
        } else if (KIPlayer::isWingMan() != 0) {
            friendly = 1;
        } else {
            friendly = (unsigned char) Globals::status->getStanding()->isFriend(this->shipGroup);
        }
        body->carriesFriendCargoFlag = friendly;
    } else if (body->damageDoneByPlayer > body->getMaxHitpoints() / 20) {
        body->enemyFlagsLo = 1;
    }

    if (this->field_0x42 != 0) {
        this->gunSwitchTimer += dt;
        if (this->gunSwitchTimer > 20000) {
            this->gunSwitchTimer = 0;
            if (body->gunAvailable(1))
                this->field_0x140 = this->field_0x140 == 0;
        }
        if (!body->isAlwaysEnemy() && this->level->getPlayer() != nullptr &&
            static_cast<Radar *>(this->level->getPlayer()->field_0x14)->lockedEnemy == this &&
            this->field_0x43 == 0) {
            body->setAlwaysEnemy(true);
            this->level->uncoverWanted(this->field_0x48);
        }
    }

    if (Globals::status->inBlackMarketSystem() && Globals::status->field_110 &&
        this->shipGroup == 8)
        body->enemyFlags = 0;
    if (body->turnedEnemy())
        body->enemyFlags = 1;
    if (body->isAlwaysFriend())
        body->enemyFlags = 0x100;

    if (!this->wingmanFlag && !this->routeClone)
        this->routeClone = this->route->clone();

    this->field_0x1c8 += dt;
    if (this->field_0x1c8 >= 201) {
        if (this->trail) {
            this->trail->update(this->renderPosition, this->workingPosition);
            this->workingPosition = this->renderPosition;
        }
        this->field_0x1c8 = 0;
    }

    body->transformMatrix = this->geometry->getMatrix();
    if (this->subGeometry)
        body->transformMatrix *= this->subGeometry->getMatrix();

    bool wingmanRoute = this->wingmanFlag && this->state != 4 && this->routeClone;
    if (wingmanRoute) {
        this->targetPos = this->level->getPlayer()->geometry->getPosition();

        if (this->field_0xe4 == 1) {
            switch (this->wingmanCommand) {
            case 0:
            case 1:
                this->objectPosition = this->level->getPlayer()->geometry->getRightVector();
                this->objectPosition *= 4000.0f;
                if (this->wingmanCommand == 0)
                    this->targetPos -= this->objectPosition;
                else
                    this->targetPos += this->objectPosition;
                this->targetPos -= AbyssEngine::AEMath::VectorNormalize(
                    this->level->getPlayer()->geometry->getDirection()) * 3000.0f;
                break;
            case 2:
                this->objectPosition = this->level->getPlayer()->geometry->getUpVector();
                this->objectPosition *= 2000.0f;
                this->targetPos += this->objectPosition;
                this->targetPos -= AbyssEngine::AEMath::VectorNormalize(
                    this->level->getPlayer()->geometry->getDirection()) * 2000.0f;
                break;
            }

            if (this->routeClone->length() < 2) {
                this->routeClone->setNewCoords(this->targetPos);
            } else {
                int coords[3] = {static_cast<int>(this->targetPos.x),
                                 static_cast<int>(this->targetPos.y),
                                 static_cast<int>(this->targetPos.z)};
                delete this->routeClone;
                this->routeClone = new Route(coords, 3);
            }
            this->routeClone->setLoop(true);
        }
    }

    if (this->state == 3 || this->state == 4) {
        body->StopEngineSound();
    } else {
        body->update(dt, this->state != 5 && this->fov != -1);
        Array<Player *> *enemies = body->getEnemies();
        if (!this->aiDisabled) {
            if (enemies) {
                if (static_cast<unsigned int>(this->field_0x38) >= enemies->size())
                    this->field_0x38 = -1;
                if (!this->field_0x12e)
                    this->field_0x38 = -1;
                else if (this->field_0x38 >= 0 && !(*enemies)[this->field_0x38]->isActive())
                    this->field_0x12e = 0;

                this->field_0x148 = 0;
                if (this->maneuverTimer < 5001) {
                    if (!this->field_0x12e) {
                        for (unsigned int i = 0; i < enemies->size(); ++i) {
                            Player *enemy = (*enemies)[i];
                            if (enemy && enemy->isActive() && !enemy->isDead()) {
                                this->targetPos = enemy->getPosition();
                                float upper = static_cast<float>(this->field_0x128);
                                float lower = static_cast<float>(-this->field_0x128);
                                float dx = this->renderPosition.x - this->targetPos.x;
                                float dy = this->renderPosition.y - this->targetPos.y;
                                float dz = this->renderPosition.z - this->targetPos.z;
                                if ((this->shipGroup != 8 && body->turnedEnemy()) ||
                                    (dx < upper && dx > lower && dy < upper && dy > lower &&
                                     dz < upper && dz > lower)) {
                                    this->field_0x12e = 1;
                                    this->field_0x38 = static_cast<int>(i);
                                    break;
                                }
                            }
                        }
                    }
                } else {
                    AERandom *rng = static_cast<AERandom *>(Globals::rnd);
                    this->field_0x12d = !this->field_0x12d && rng->nextInt(100) < 20;
                    this->maneuverTimer = 0;
                    if (rng->nextInt(100) <= 29 && enemies->size() >= 2) {
                        this->field_0x12e = 0;
                        for (int attempt = 0; attempt < 5 && !this->field_0x12e; ++attempt) {
                            this->field_0x38 = rng->nextInt(static_cast<int>(enemies->size()));
                            Player *enemy = (*enemies)[this->field_0x38];
                            if (enemy->isActive()) {
                                this->targetPos = enemy->getPosition();
                                float upper = static_cast<float>(this->field_0x128);
                                float lower = static_cast<float>(-this->field_0x128);
                                float dx = this->renderPosition.x - this->targetPos.x;
                                float dy = this->renderPosition.y - this->targetPos.y;
                                float dz = this->renderPosition.z - this->targetPos.z;
                                if ((this->shipGroup != 8 && body->turnedEnemy()) ||
                                    (dx < upper && dx > lower && dy < upper && dy > lower &&
                                     dz < upper && dz > lower))
                                    this->field_0x12e = 1;
                            }
                        }
                        if (!this->field_0x12e)
                            this->field_0x38 = 0;
                    } else {
                        this->field_0x38 = 0;
                    }

                    if (this->wingmanFlag && this->field_0xe4 == 3 && this->wingmanTarget) {
                        if (this->wingmanTarget->isDead()) {
                            this->setWingmanCommand(1, nullptr);
                        } else {
                            for (unsigned int i = 0; i < enemies->size(); ++i) {
                                Player *enemy = (*enemies)[i];
                                if (enemy == this->wingmanTarget->player && enemy->isActive() &&
                                    !enemy->isAlwaysFriend()) {
                                    this->field_0x38 = static_cast<int>(i);
                                    break;
                                }
                            }
                        }
                    }

                    Player *selected = (*enemies)[this->field_0x38];
                    if (!selected->isActive() || selected->isDead()) {
                        this->field_0x12e = 0;
                        this->field_0x38 = -1;
                    } else {
                        this->targetPos = selected->getPosition();
                        float upper = static_cast<float>(this->field_0x128);
                        float lower = static_cast<float>(-this->field_0x128);
                        float dx = this->renderPosition.x - this->targetPos.x;
                        float dy = this->renderPosition.y - this->targetPos.y;
                        float dz = this->renderPosition.z - this->targetPos.z;
                        if (!(dx < upper && dx > lower && dy < upper && dy > lower &&
                              dz < upper && dz > lower))
                            this->field_0x38 = -1;
                    }
                }

                if (!body->enemyFlagsLo && this->field_0x38 == 0) {
                    this->field_0x12e = 0;
                    this->field_0x38 = 1;
                }

                if ((!this->wingmanFlag || this->field_0xe4 != 3) && this->field_0x38 >= 1) {
                    this->field_0x38 = -1;
                    for (unsigned int i = 1; i < enemies->size(); ++i) {
                        Player *enemy = (*enemies)[i];
                        if (!enemy->isActive() || enemy->isDead())
                            continue;

                        KIPlayer *owner = enemy->getKIPlayer();
                        if (!owner && !enemy->field_69)
                            continue;
                        int group = enemy->field_69 ? 0 : enemy->getKIPlayer()->shipGroup;

                        bool eligible;
                        if (this->wingmanFlag) {
                            eligible = enemy->enemyFlagsLo ||
                                (this->wingmanTarget && enemy == this->wingmanTarget->player);
                        } else {
                            eligible = (group == 8 && this->shipGroup != 8) ||
                                (group != 8 && this->shipGroup == 8) ||
                                (group == 9 && this->shipGroup != 9) ||
                                (group != 9 && this->shipGroup == 9) ||
                                (group == 0 && this->shipGroup == 1) ||
                                (group == 1 && this->shipGroup == 0) ||
                                (group == 3 && this->shipGroup == 2) ||
                                (group == 2 && this->shipGroup == 3) ||
                                (group == 10 && this->shipGroup != 10) ||
                                (group != 10 && this->shipGroup == 10);
                        }
                        if (eligible) {
                            this->field_0x12e = 1;
                            this->field_0x38 = static_cast<int>(i);
                            break;
                        }
                    }
                }

                this->field_0x12c = 0;
                if (this->wingmanFlag && !this->field_0x12e && this->field_0xe4 == 1)
                    this->field_0x38 = -1;

                bool useRoute = this->field_0x38 == -1 && this->routeClone;
                if (!useRoute)
                    useRoute = body->doesNeverAttack();
                if (useRoute) {
                    if (!this->routeClone->getWaypoint() || !body->isActive()) {
                        this->field_0x38 = 0;
                        this->field_0x148 = body->getEnemy(0);
                        if (this->field_0x148) {
                            this->targetPos = this->field_0x148->getPosition();
                            this->resetVecB = this->targetPos;
                        }
                    } else {
                        this->routeClone->update(this->geometry->getPosition());
                        if (this->routeClone->getWaypoint()) {
                            KIPlayer *dockingTarget = this->routeClone->getDockingTarget();
                            if (!dockingTarget) {
                                this->resetVecB.x = static_cast<float>(this->routeClone->getWaypoint()->x);
                                this->resetVecB.y = static_cast<float>(this->routeClone->getWaypoint()->y);
                                this->resetVecB.z = static_cast<float>(this->routeClone->getWaypoint()->z);
                                this->field_0x12c = 1;
                            } else {
                                if (this->state == 1)
                                    this->state = 7;
                                if (this->state == 7) {
                                    SpacePoint *navigationPoint = dockingTarget->getNearestNavigationPoint(
                                        this->getPosition(), this->field_0x2c4);
                                    if (navigationPoint) {
                                        if (this->field_0x2c4 != navigationPoint) {
                                            if (this->field_0x2c4)
                                                this->field_0x2c4->giveFree();
                                            this->field_0x2c4 = navigationPoint;
                                            navigationPoint->take();
                                        }
                                        this->objectPosition = dockingTarget->getPosition() +
                                            AbyssEngine::AEMath::MatrixRotateVector(
                                                dockingTarget->geometry->getMatrix(), navigationPoint->position);
                                        int distance = static_cast<int>(AbyssEngine::AEMath::VectorLength(
                                            this->objectPosition - this->getPosition()));
                                        if (distance < 2000) {
                                            this->state = 8;
                                            delete this->easeMatrix;
                                            this->easeMatrix = nullptr;

                                            Vector approach = dockingTarget->getPosition() +
                                                AbyssEngine::AEMath::MatrixRotateVector(
                                                    dockingTarget->geometry->getMatrix(), navigationPoint->position);
                                            SpacePoint *nearest = dockingTarget->getNearestDockingPoint(approach);
                                            Vector direction = AbyssEngine::AEMath::VectorNormalize(
                                                AbyssEngine::AEMath::MatrixRotateVector(
                                                    dockingTarget->geometry->getMatrix(), nearest->direction));
                                            Vector up = AbyssEngine::AEMath::VectorNormalize(
                                                AbyssEngine::AEMath::MatrixRotateVector(
                                                    dockingTarget->geometry->getMatrix(), navigationPoint->position) -
                                                AbyssEngine::AEMath::MatrixRotateVector(
                                                    dockingTarget->geometry->getMatrix(), nearest->position));
                                            Vector right = AbyssEngine::AEMath::VectorNormalize(
                                                AbyssEngine::AEMath::VectorCross(up, direction));
                                            Matrix destination;
                                            AbyssEngine::AEMath::MatrixIdentity(destination);
                                            AbyssEngine::AEMath::MatrixSetRotation(destination, right, up, direction);
                                            this->easeMatrix = new AbyssEngine::EaseInOutMatrix(
                                                this->player->transformMatrix, destination, 3000);
                                        } else {
                                            this->resetVecB = this->objectPosition;
                                        }
                                    }
                                }
                                this->field_0x12c = 1;
                            }
                        }
                    }
                } else {
                    if (this->field_0x38 < 0 ||
                        static_cast<unsigned int>(this->field_0x38) >= body->getEnemies()->size())
                        this->field_0x38 = 0;
                    this->field_0x148 = body->getEnemy(this->field_0x38);
                    this->targetPos = this->field_0x148->getPosition();
                    this->resetVecB = this->targetPos;
                }
            } else if (this->routeClone) {
                this->routeClone->update(this->geometry->getPosition());
                if (this->routeClone->getWaypoint()) {
                    this->resetVecB.x = static_cast<float>(this->routeClone->getWaypoint()->x);
                    this->resetVecB.y = static_cast<float>(this->routeClone->getWaypoint()->y);
                    this->field_0x12c = 1;
                    this->resetVecB.z = static_cast<float>(this->routeClone->getWaypoint()->z);
                }
            } else {
                this->state = 5;
            }

            if (this->jumperFlag && this->field_0x12c) {
                this->field_0xf0 += dt;
                if (this->field_0xf0 >= 20000) {
                    this->state = 6;
                    this->field_0xf0 = 0;
                    this->field_0x12c = 0;
                }
            } else {
                this->field_0xf0 = 0;
            }
        }

        if (this->wingmanFlag && this->field_0xe4 == 2) {
            if (!this->commandRoute) {
                this->setWingmanCommand(1, nullptr);
            } else {
                this->commandRoute->update(this->geometry->getPosition());
                if (this->commandRoute->getCurrent() > this->field_0x1e4) {
                    delete this->commandRoute;
                    this->commandRoute = nullptr;
                    this->setWingmanCommand(1, nullptr);
                } else if (this->commandRoute->getWaypoint()) {
                    this->resetVecB.x = static_cast<float>(this->commandRoute->getWaypoint()->x);
                    this->resetVecB.y = static_cast<float>(this->commandRoute->getWaypoint()->y);
                    this->field_0x12c = 1;
                    this->resetVecB.z = static_cast<float>(this->commandRoute->getWaypoint()->z);
                }
            }
        }
    }

    this->resetVecC = this->resetVecB - this->renderPosition;

    if (body->enemyFlagsLo &&
        (this->state == 5 || (this->wingmanFlag && this->field_0xe4 == 1)) &&
        this->field_0x128 >= 1) {
        PlayerEgo *ego = this->level->getPlayer();
        if (ego->isInRocketControl())
            this->targetPos = this->level->getPlayer()->getTargetFollowCamera()->getTarget()->getPosition();
        else
            this->targetPos = static_cast<Player *>(this->level->getPlayer()->player)->getPosition();

        this->resetVecC = this->targetPos - this->renderPosition;
        if (this->resetVecC.x < 25000.0f && this->resetVecC.x > -25000.0f &&
            this->resetVecC.y < 25000.0f && this->resetVecC.y > -25000.0f &&
            this->resetVecC.z < 25000.0f) {
            this->state = 1;
            (this->subGeometry ? this->subGeometry : this->geometry)->setVisible(true);
            body->setActive(true);
        }
    }

    if (this->boundingVolumes) {
        for (unsigned int i = 0; i < this->boundingVolumes->size(); ++i)
            (*this->boundingVolumes)[i]->update(this->renderPosition.x,
                                                this->renderPosition.y,
                                                this->renderPosition.z);
    }

    int hitpoints = body->getHitpoints();
    if (this->field_0x1fc) {
        if (static_cast<float>(body->getMaxHitpoints()) * 0.33f <=
            static_cast<float>(hitpoints)) {
            this->field_0x1fc = 0;
            this->level->particleEmitBoolPtr->enableSystemEmit(this->field_0x80, false);
            this->level->particleRenderBoolPtr->enableSystemEmit(this->field_0x84, false);
        }
    } else if (static_cast<float>(body->getMaxHitpoints()) * 0.33f >
               static_cast<float>(hitpoints)) {
        float particleQuality;
        std::memcpy(&particleQuality, Globals::options + 0x28, sizeof(particleQuality));
        if (particleQuality > 0.0f) {
            bool emit = !this->isDocked();
            this->level->particleEmitBoolPtr->enableSystemEmit(this->field_0x80, emit);
            this->level->particleRenderBoolPtr->enableSystemEmit(this->field_0x84, emit);
        }
        this->field_0x1fc = 1;
    }

    if (hitpoints <= 0 && this->state != 3 && this->state != 4) {
        Status *status = Globals::status;
        Mission *freelance = status->getFreelanceMission();
        if (freelance && freelance->getType() == 13 &&
            status->getStation()->getIndex() == freelance->getTargetStation() &&
            status->field_0xf1 == 0) {
            if (this->name.data &&
                this->name.Compare(*Globals::gameText->getText(1663)) == 0) {
                status->field_0xf0 = 1;
            } else {
                status->field_0xf0 = 0;
                status->field_0xf1 = 1;
            }
        }

        if (this->field_0x42) {
            Wanted *wanted = (*status->wanted)[this->field_0x48];
            wanted->setTerminated(true);
            wanted->setActive(false);
            Globals::layout->showMissionRewardMessage(wanted->getReward(), true);
            status->setCredits(status->getCredits() + wanted->getReward());
            status->incCollectedBounties(wanted->getBoard());
        }

        if (body->enemyFlagsLo) {
            if (this->shipGroup == 9) {
                this->cargo = new Array<int>();
                ArrayAdd<int>(131, *this->cargo);
                ArrayAdd<int>(static_cast<AERandom *>(Globals::rnd)->nextInt(3) + 1,
                              *this->cargo);
            } else if (this->shipGroup == 8) {
                if (!body->destroyedByte)
                    status->incPirateKills();
            } else if (!this->isMissionCrate && this->crateCaptured &&
                       Globals::options[0x35] &&
                       // The ARM branch checks a negative draw despite the normal RNG range.
                       static_cast<AERandom *>(Globals::rnd)->nextInt(100) < 0 &&
                       this->shipGroup <= 3) {
                delete this->cargo;
                this->cargo = new Array<int>();
                ArrayAdd<int>(this->shipGroup + 189, *this->cargo);
                ArrayAdd<int>(1, *this->cargo);
            }
            this->level->enemyDied(this->kind, body->destroyedByte != 0);
        } else {
            this->level->friendDied();
            if (this->wingmanFlag)
                this->level->wingmanDied(this->name);
        }

        this->state = 3;
        this->field_0x73 = 0;
        this->field_0x1f8 = static_cast<AERandom *>(Globals::rnd)->nextInt(1500) + 1500;
        this->deathDirection = this->geometry->getDirection();
        const Vector zero = {0.0f, 0.0f, 0.0f};
        this->level->field_74->emitManual(this->level->field_3c,
                                          this->renderPosition, 0, zero, -1.0f);
        Globals::sound->play(20, Globals::options[0x0f] ? &this->renderPosition : nullptr,
                             nullptr, 0.0f);
        Vector impulse = {
            static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100),
            static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100),
            static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100)
        };
        this->deathDrift = AbyssEngine::AEMath::VectorNormalize(impulse);
        this->deathDrift *= 0.05f;
        this->level->field_74->enableSystemEmit(this->engineTrailSystem, true);
        this->level->field_8c->enableSystemEmit(this->field_0x130, false);
        this->level->field_8c->enableSystemEmit(this->field_0x134, false);
        this->level->field_8c->enableSystemEmit(this->field_0x138, false);
        this->crateCaptured = static_cast<uint8_t>(this->cargoAvailable());
        this->setExhaustVisible(false);
    }

    if (this->aiDisabled) {
        this->geometry->updateReferenceMatrix();
        this->handleCloaking();
        return;
    }

    if (this->state == 3) {
        this->finished = 0;
        this->field_0x13e = 0;
        Matrix rotation;
        AbyssEngine::AEMath::MatrixSetRotation(rotation, this->deathDrift.x,
                                              this->deathDrift.y, this->deathDrift.z);
        if (dt >= 1) {
            Matrix combined = this->geometry->getMatrix() * rotation;
            this->geometry->setMatrix(combined);
        }
        Vector movement = this->deathDirection * static_cast<float>(dt);
        movement = movement * this->currentSpeed;
        this->geometry->translate(movement);
        this->geometry->updateReferenceMatrix();

        this->field_0x1f8 -= dt;
        if (this->field_0x1f8 >= 0)
            return;

        const Vector zero = {0.0f, 0.0f, 0.0f};
        this->explosion->start(this->renderPosition, zero);
        this->level->field_74->enableSystemEmit(this->engineTrailSystem, false);
        float particleQuality;
        std::memcpy(&particleQuality, Globals::options + 0x28, sizeof(particleQuality));
        if (particleQuality > 0.0f) {
            this->level->particleEmitBoolPtr->enableSystemEmit(this->field_0x80, false);
            this->level->particleRenderBoolPtr->enableSystemEmit(this->field_0x84, false);
        }
        this->player->setBombForce(
            50.0f + static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(50)) * 0.01f);
        Vector impulse = {
            static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100),
            static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100),
            static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100)
        };
        this->postExplosionDirection = AbyssEngine::AEMath::VectorNormalize(impulse);
        this->field_0x1f8 = 0;
        this->state = 4;
        if (this->field_0x2c4 && !this->field_0x2c4->isFree()) {
            this->field_0x2c4->giveFree();
            this->field_0x2c4 = nullptr;
        }
        if (this->crateCaptured) {
            if (this->isMissionCrate && !this->missionCrateCaptured) {
                this->crateCaptured = 0;
                this->missionCrateLost = 1;
            } else {
                this->createCrate(this->shipGroup == 9 ? 4 : 0);
            }
        }
        return;
    }

    if (this->state == 4) {
        this->field_0x1f8 += dt;
        if (this->field_0x1f8 < 1) {
            Matrix rotation;
            AbyssEngine::AEMath::MatrixSetRotation(rotation, this->deathDrift.x,
                                                  this->deathDrift.y, this->deathDrift.z);
            if (dt >= 1) {
                Matrix combined = this->geometry->getMatrix() * rotation;
                this->geometry->setMatrix(combined);
            }
            Vector movement = this->deathDirection * static_cast<float>(dt);
            movement = movement * this->currentSpeed;
            this->geometry->translate(movement);
            this->geometry->updateReferenceMatrix();
        } else {
            this->field_0x73 = 0;
        }
        this->geometry->updateReferenceMatrix();

        TargetFollowCamera *camera = nullptr;
        if (this->level->getPlayer())
            camera = this->level->getPlayer()->getTargetFollowCamera();
        this->explosion->update(dt, camera);
        this->deathTimer += dt;

        if (this->crateCaptured && this->player->isActive() && dt >= 1 && this->crateGeometry) {
            const float force = this->player->getBombForce();
            if (force > 0.0f) {
                this->targetPos = this->postExplosionDirection * force;
                this->crateGeometry->translate(this->targetPos);
                this->geometry->translate(this->targetPos);
                float nextForce = force * 0.98f;
                if (nextForce < 0.05f)
                    nextForce = 0.0f;
                this->player->setBombForce(nextForce);
            }
            float spin = static_cast<float>(dt >> 1) * 0.0000152587890625f;
            spin = static_cast<float>(static_cast<int>(spin * 6.2831855f));
            this->crateGeometry->rotate(spin, spin, spin);
            this->player->transformMatrix = this->crateGeometry->getMatrix();
            if (this->deathTimer <= 60000 || this->explosion->isPlaying())
                return;

            if (this->level->getPlayer()) {
                Radar *radar = static_cast<Radar *>(this->level->getPlayer()->field_0x14);
                if (radar && radar->field_0x1c == this)
                    radar->field_0x1c = nullptr;
            }
            if (this->isMissionCrate && !this->missionCrateCaptured)
                this->missionCrateLost = 1;
            delete this->crateGeometry;
            this->crateGeometry = nullptr;
            this->deathTimer = 0;
            KIPlayer::setActive(false);
            this->finished = 1;
            return;
        }

        if (!this->explosion->isPlaying()) {
            if (this->isMissionCrate && !this->missionCrateCaptured)
                this->missionCrateLost = 1;
            if (this->deathTimer > 60000)
                KIPlayer::setActive(false);
            this->finished = 1;
        }
        return;
    }

    if (this->state == 5) {
        if (body->enemyFlagsLo && Globals::status->getCurrentCampaignMission() >= 2)
            (this->subGeometry ? this->subGeometry : this->geometry)->setVisible(false);

        if (!this->field_0x148 || this->field_0x148->field_5e)
            return;

        float upper = static_cast<float>(this->field_0x128);
        float lower = static_cast<float>(-this->field_0x128);
        if (!(this->resetVecC.x < upper && this->resetVecC.x > lower &&
              this->resetVecC.y < upper && this->resetVecC.y > lower &&
              this->resetVecC.z < upper && this->resetVecC.z > lower))
            return;

        this->state = 1;
        (this->subGeometry ? this->subGeometry : this->geometry)->setVisible(true);
        body->setActive(true);
        if (this->field_0x12f)
            this->level->pirateStationAction(true);
        return;
    }

    if (this->state == 6) {
        this->currentSpeed *= 1.1f;
        this->geometry->moveForward(static_cast<float>(
            static_cast<int>(this->currentSpeed * static_cast<float>(dt))));
        if (this->currentSpeed > 100.0f)
            KIPlayer::setDead();
        return;
    }

    if (this->state == 8) {
        // Android rodata 0x1fe130; all 64 entries match Level's hangar ship offsets.
        static constexpr int kDockReachByKind[64] = {
            160, 150, 220, 310, 160, 180, 140, 170, 170, 110, 250, 205, 240, 250, 250, 250,
            170, 170, 190, 460, 170, 210, 140, 170, 160, 190, 170, 240, 220, 280, 200, 200,
            140, 160, 180, 150, 310, 390, 480, 200, 300, 300, 170, 200, 170, 370, 370, 300,
            440, 340, 170, 470, 170, 170, 170, 170, 200, 170, 250, 150, 170, 250, 270, 170,
        };
        KIPlayer *dockingTarget = this->routeClone->getDockingTarget();
        Vector approach = dockingTarget->getPosition() +
            AbyssEngine::AEMath::MatrixRotateVector(
                dockingTarget->geometry->getMatrix(), this->field_0x2c4->position);
        SpacePoint *nearest = dockingTarget->getNearestDockingPoint(approach);
        this->objectPosition = dockingTarget->getPosition() +
            AbyssEngine::AEMath::MatrixRotateVector(
                dockingTarget->geometry->getMatrix(), nearest->position);
        int distance = static_cast<int>(AbyssEngine::AEMath::VectorLength(
            this->objectPosition - this->getPosition()));
        if (distance < kDockReachByKind[this->kind]) {
            this->setExhaustVisible(false);
            this->spacePoint = 0;
            this->field_0x2c0 = 0;
            this->state = 9;
            if (this->field_0x1fc) {
                this->level->particleEmitBoolPtr->enableSystemEmit(this->field_0x80, false);
                this->level->particleRenderBoolPtr->enableSystemEmit(this->field_0x84, false);
            }
            return;
        }

        this->easeMatrix->Increase(static_cast<float>(dt));
        Matrix dockingMatrix = this->easeMatrix->GetValue();
        Vector currentPosition = this->getPosition();
        Vector direction = this->objectPosition - this->getPosition();
        Vector displacement = direction * static_cast<float>(dt);
        displacement = displacement * 0.0006f;
        Vector nextPosition = currentPosition + displacement;
        AbyssEngine::AEMath::MatrixSetTranslation(dockingMatrix, nextPosition);
        this->geometry->setMatrix(dockingMatrix);
        this->player->transformMatrix = dockingMatrix;
        return;
    }

    if (this->state == 9) {
        Mission *mission = Globals::status->getMission();
        this->spacePoint += dt;
        if (!mission->isEmpty() && mission->getType() == 184 &&
            static_cast<PlayerFixedObject *>(this->routeClone->getDockingTarget())->getDockingType() == 1) {
            int previousCount = this->field_0x2c0;
            float interval = this->shipGroupFlag == 51 ? 200.0f : 1500.0f;
            this->field_0x2c0 = static_cast<int>(static_cast<float>(this->spacePoint) / interval);
            if ((Globals::status->getCurrentCampaignMission() != 94 ||
                 mission->getStatusValue() > Globals::status->getShip()->getMaxPassengers()) &&
                this->field_0x2c0 > previousCount &&
                this->level->getNumDeliveredPassengers() < mission->getProductionGoodAmount()) {
                mission->setStatusValue(mission->getStatusValue() + previousCount - this->field_0x2c0);
                if (mission->getStatusValue() <= -1)
                    mission->setStatusValue(0);
            }
        }
        if (this->spacePoint > this->routeClone->getDockingTime()) {
            this->setExhaustVisible(true);
            this->routeClone->reachWaypoint(this->routeClone->getCurrent());
            this->state = 1;
            if (this->field_0x1fc) {
                this->level->particleEmitBoolPtr->enableSystemEmit(this->field_0x80, true);
                this->level->particleRenderBoolPtr->enableSystemEmit(this->field_0x84, true);
            }
            if (this->field_0x2c4) {
                this->field_0x2c4->giveFree();
                this->field_0x2c4 = nullptr;
            }
        }
        return;
    }

    if (this->state == 1 || this->state == 7) {
        this->handleCloaking();
        if (this->field_0x13d) {
            if (this->hitpoints > hitpoints) {
                this->field_0x1dc += this->hitpoints - hitpoints;
                this->hitpoints = hitpoints;
                if ((static_cast<float>(this->field_0x1dc) /
                     static_cast<float>(body->getMaxHitpoints())) * 100.0f > 40.0f) {
                    this->field_0x1c0 = 10000;
                    this->field_0x1dc = 0;
                    this->field_0x1e0 = 1;
                }
            }
            if (this->field_0x1c0 > 5000 && !this->field_0x1f4) {
                this->field_0x1c0 = 0;
                AERandom *rng = static_cast<AERandom *>(Globals::rnd);
                if (this->field_0x1e0 || rng->nextInt(100) < this->boostProb) {
                    this->field_0x1c4 = rng->nextInt(3000) + 5000;
                    this->field_0x1f4 = 1;
                    this->field_0x1ec = 5.5f;
                    this->currentRotate = 1.3f;
                }
            }
            if (this->field_0x1f4) {
                if (this->field_0x1c0 > this->field_0x1c4) {
                    this->field_0x1e0 = 0;
                    this->field_0x1c0 = 0;
                    this->field_0x1ec = this->speed;
                    this->currentRotate = this->rotate;
                }
                if (this->field_0x1ec > 0.0f) {
                    this->currentSpeed *= this->field_0x1ec > this->currentSpeed ? 1.05f : 0.95f;
                    if (this->currentSpeed >= 5.5f || this->currentSpeed < this->speed) {
                        this->currentSpeed = this->field_0x1ec;
                        if (this->field_0x1ec == this->speed)
                            this->field_0x1f4 = 0;
                        this->field_0x1ec = 0.0f;
                    }
                }
            }
        }

        if (this->field_0x148 && !this->field_0x12c) {
            int reach = 8000;
            if (this->field_0x148->field_69) {
                PlayerEgo *ego = this->level->getPlayer();
                if (ego && ego->isDockedToDockingPoint())
                    reach = 12000;
            }
            float upper = static_cast<float>(reach);
            float lower = static_cast<float>(-reach);
            if (this->resetVecC.x < upper && this->resetVecC.x > lower &&
                this->resetVecC.y < upper && this->resetVecC.y > lower &&
                this->resetVecC.z < upper && this->resetVecC.z > lower)
                this->resetVecC = this->geometry->getRightVector();
        }
        Vector desiredWorld = AbyssEngine::AEMath::VectorNormalize(this->resetVecC);
        this->resetVecC = AbyssEngine::AEMath::MatrixRotateVector(
            AbyssEngine::AEMath::MatrixGetInverse(body->transformMatrix),
            desiredWorld);
        this->resetVecC.y = -this->resetVecC.y;
        this->field_0x24 = body->empDisabledByte;
        if (this->field_0x12e && !this->field_0x12c && this->field_0x148 &&
            !body->doesNeverAttack()) {
            Player *target = this->field_0x148;
            if (target->field_5e) {
                this->field_0x12e = 0;
                if (this->shootingFlag) {
                    body->stopShooting(this->field_0x140);
                    this->shootingFlag = 0;
                }
            } else {
                float limit = this->shootError;
                Vector displacement = this->targetPos - this->renderPosition;
                if (this->resetVecC.x < limit && this->resetVecC.x > -limit &&
                    this->resetVecC.y < limit && this->resetVecC.y > -limit &&
                    displacement.x < 35000.0f && displacement.x > -35000.0f &&
                    displacement.y < 35000.0f && displacement.y > -35000.0f &&
                    displacement.z < 35000.0f && displacement.z > -35000.0f) {
                    bool blocked = !this->field_0x25 || !target->isActive() || target->isDead();
                    if (!blocked && target->field_69 && this->level->getPlayer() &&
                        this->level->getPlayer()->isDockedToDockingPoint())
                        blocked = target->getPosition().y > body->getPosition().y;
                    if (blocked) {
                        this->field_0x12e = 0;
                    } else {
                        body->shoot(this->field_0x140, static_cast<long long>(dt), false);
                        this->shootingFlag = 1;
                    }
                }
            }
        }
        Matrix matrix = this->geometry->getMatrix();

        if (this->field_0x12d) {
            this->rollSampleIndex = 0;
            this->targetRoll = 0.0f;
            this->rollBufferFilled = 0;
        } else {
            Vector direction = AbyssEngine::AEMath::MatrixGetDir(matrix);
            Vector correction = AbyssEngine::AEMath::VectorNormalize(desiredWorld - direction);
            correction *= static_cast<float>(dt * 48) * 0.0000152587890625f;
            this->resetVecC = AbyssEngine::AEMath::VectorNormalize(direction + correction);
            Vector remaining = this->resetVecC - desiredWorld;
            if (std::fabs(remaining.x) + std::fabs(remaining.y) +
                std::fabs(remaining.z) < 0.0625f)
                this->resetVecC = desiredWorld;

            float dot = AbyssEngine::AEMath::VectorDot(
                AbyssEngine::AEMath::VectorNormalize(direction), this->resetVecC);
            float angle = 0.0f;
            if (dot < 1.0f && dot > -1.0f)
                angle = AbyssEngine::AEMath::ACosf(dot);
            float signedAngle = -angle;
            if (angle > 0.0f)
                signedAngle = angle;
            if (signedAngle != 0.0f &&
                AbyssEngine::AEMath::ACosf(AbyssEngine::AEMath::VectorDot(
                    AbyssEngine::AEMath::MatrixGetRight(matrix), this->resetVecC)) < 1.5707964f)
                signedAngle = -signedAngle;

            int index = this->rollSampleIndex;
            this->rollSamples[index] = signedAngle;
            float average = signedAngle;
            if (this->rollBufferFilled) {
                float sum = 0.0f;
                for (int i = 0; i < 5; ++i)
                    sum += this->rollSamples[i];
                average = sum / 5.0f;
            } else if (index > 0) {
                float sum = 0.0f;
                for (int i = 0; i < index; ++i)
                    sum += this->rollSamples[i];
                average = sum / static_cast<float>(index);
            }
            this->rollSampleIndex = index + 1;
            if (index >= 4) {
                this->rollSampleIndex = 0;
                this->rollBufferFilled = 1;
            }

            float roll = average * this->field_0x294 * this->field_0x298;
            this->targetRoll = roll;
            if (roll > this->field_0x294)
                this->targetRoll = this->field_0x294;
            else if (roll < -this->field_0x294)
                this->targetRoll = -this->field_0x294;

            Vector right = AbyssEngine::AEMath::VectorNormalize(AbyssEngine::AEMath::VectorCross(
                AbyssEngine::AEMath::MatrixGetUp(matrix), this->resetVecC));
            Vector up = AbyssEngine::AEMath::VectorNormalize(
                AbyssEngine::AEMath::VectorCross(this->resetVecC, right));
            matrix = AbyssEngine::AEMath::MatrixSetRotation(matrix, right, up, this->resetVecC);
        }

        if (this->rollActive)
            this->roll(dt);

        float target = this->targetRoll;
        float current = this->smoothRoll;
        if (current != target) {
            float step = static_cast<float>(dt) * 1.25f / 3.9f;
            if (current < target) {
                current += step;
                if (current > target)
                    current = target;
            } else if (current > target) {
                current -= step;
                if (current < target)
                    current = target;
            }
            this->smoothRoll = current;
        }

        int32_t timer = static_cast<int32_t>(this->field_0xfc);
        if (timer >= 1) {
            timer -= dt;
            this->field_0xfc = static_cast<uint32_t>(timer);
            if (timer <= 0)
                this->rollActive = 1;
        }
        if (target == current) {
            if (timer >= 1) {
                timer -= dt;
                this->field_0xfc = static_cast<uint32_t>(timer);
                if (timer <= 0)
                    this->rollActive = 1;
            }
        } else {
            this->field_0xfc = 750;
        }

        Matrix bank;
        bank = AbyssEngine::AEMath::MatrixSetRotation(
            bank, 0.0f, 0.0f, current * 0.000244140625f * 3.1415927f);
        if (this->subGeometry)
            this->subGeometry->setMatrix(bank);

        const bool effectMode = body->empDisabledByte != 0;
        if (!effectMode) {
            float distance = static_cast<float>(static_cast<int>(
                this->currentSpeed * static_cast<float>(dt)));
            matrix.m[3] += matrix.m[2] * distance;
            matrix.m[7] += matrix.m[6] * distance;
            matrix.m[11] += matrix.m[10] * distance;
            if (this->rollActive)
                matrix *= this->rollMatrix;
            Globals::Canvas->TransformSetLocal(this->geometry->transform, matrix);
            body->transformMatrix = matrix;
            if (this->subGeometry)
                body->transformMatrix *= this->subGeometry->getMatrix();
        }
        this->level->field_8c->enableSystemEmit(this->field_0x130, effectMode);
        this->level->field_8c->enableSystemEmit(this->field_0x134, effectMode);
        this->level->field_8c->enableSystemEmit(this->field_0x138, effectMode);
        this->geometry->updateReferenceMatrix();
    }

    // shortcut: post-movement landmark/enemy reactions remain unreconstructed.
    PF_update_body(this, dt);

    return;
}

void PlayerFighter::setPosition(float x, float y, float z) {
    this->posX = x;
    this->posY = y;
    this->posZ = z;

    this->geometry->setPosition(x, y, z);
    this->workingPosition = Vector{x, y, z};
    if (this->trail) {
        this->trail->reset(this->workingPosition);
    }
    this->player->transformMatrix = this->geometry->getMatrix();
}

void PlayerFighter::roll(int angle) {
    if (!this->rollActive) return;

    Matrix &matrix = this->geometry->getMatrix();
    const float rx = matrix.m11_rightY;
    const float ry = matrix.m12_upY;
    float rate = rx > 0.0f ? rx : -rx;
    if (angle >= 60) angle = 60;

    if (ry > 0.0f && rate < 0.015f) {
        this->rollMatrix = AbyssEngine::AEMath::MatrixIdentity(this->rollMatrix);
        this->rollActive = 0;
        this->field_0x254 = 0;
        this->field_0x255 = 0;
        return;
    }

    if (this->field_0x254) {
        // ARM's comparison of ry with itself keeps NaN on the negative-rate path.
        rate = rx < 0.0f && !std::isnan(ry) ? 0.0002f : -0.0002f;
    } else if (rx > 0.0f && this->field_0x255 == 1) {
        rate = -0.00035f;
        this->field_0x254 = 1;
    } else if (rx < 0.0f && this->field_0x255 == 2) {
        rate = 0.00035f;
        this->field_0x254 = 1;
    } else if (rx >= 0.0f) {
        if (ry < 0.0f)
            rate = -0.00075f;
        else if (rx > 0.0f && ry > 0.0f)
            rate = rx > 0.3f ? -0.00035f : -0.00025f;
    } else if (rx < 0.0f) {
        if (ry < 0.0f)
            rate = 0.00075f;
        else if (ry >= 0.0f)
            rate = rx > -0.3f ? 0.00025f : 0.00035f;
    }

    if (rx != 0.0f && !std::isnan(rx))
        this->field_0x255 = rx < 0.0f ? 1 : 2;
    this->rollActive = 1;
    this->rollMatrix = AbyssEngine::AEMath::MatrixSetRotation(
        this->rollMatrix, 0.0f, 0.0f, rate * static_cast<float>(angle));
}

void PlayerFighter::cloak(int dur, bool b) {
    unsigned v;
    if (dur > 0) {
        v = (unsigned) dur;
    } else {
        v = static_cast<AERandom *>(Globals::rnd)->nextInt(5000) + 5000;
    }
    this->cloakActive = 1;
    this->cloakDuration = v + 4000;
    this->field_0x2d9 = b;
}

void PlayerFighter::setMissionCrate(bool on) {
    this->isMissionCrate = on;
    if (on) {
        this->lootList = nullptr;
        this->lootList = new Array<int>();
        int type = Globals::status->getMission()->getType();
        int item = (type == 5) ? 0x74 : 0x75;
        ArrayAdd(item, *(this->lootList));

        ArrayAdd<int>(1, *this->lootList);
    }
}

int PlayerFighter::collide(float x, float y, float z) {
    if ((unsigned) (this->state - 3) > 1) {
        Array<BoundingVolume *> *a = this->boundingVolumes;
        if (a != 0) {
            for (unsigned i = 0; i < a->size(); i++) {
                BoundingVolume *e = a->data()[i];
                if (e->collide(x, y, z) != 0) {
                    return 1;
                }
                a = this->boundingVolumes;
            }
        }
    }
    return 0;
}

void PlayerFighter::setBV(BoundingVolume *bv) {
    Array<BoundingVolume *> *a = new Array<BoundingVolume *>();
    this->boundingVolumes = a;

    ArrayAdd<BoundingVolume *>(bv, *a);
}

void PlayerFighter::setWingmanCommand(int cmd, KIPlayer *target) {
    int saved = this->field_0xe4;
    ((KIPlayer *) (this))->setWingmanCommand(cmd, target);
    if ((unsigned) (cmd - 2) < 2) {
        this->maneuverTimer = 0x1389;
        if (this->currentSpeed != 5.5f) {
            this->field_0x1c0 = 0x1389;
        }
        if (cmd == 2) {
            if (((Level *) (this->level))->getPlayerRoute() != 0) {
                Route *r = ((Level *) (this->level))->getPlayerRoute();
                this->commandRoute = r->getExactClone();
                this->field_0x1e4 = this->commandRoute->getCurrent();
                goto done;
            }
        } else if (target != 0) {
            goto done;
        }
        this->field_0xe4 = saved;
    } else if (cmd == 0) {
        this->field_0xe4 = saved;
        this->field_0x140 = (this->field_0x140 == 0);
    } else if (cmd == 1) {
        this->field_0x13d = 0;
        this->field_0x1e0 = 0;
    }
done:
    this->currentSpeed = this->speed;
    this->currentRotate = this->rotate;
}

void PlayerFighter::setSpeed(float v) {
    this->speed = v;
    this->field_0x13d = 0;
    this->currentSpeed = v;
}

int PlayerFighter::outerCollide(float x, float y, float z) {
    if ((unsigned) (this->state - 3) > 1) {
        Array<BoundingVolume *> *a = this->boundingVolumes;
        if (a != 0) {
            for (unsigned i = 0; i < a->size(); i++) {
                BoundingVolume *e = a->data()[i];
                if (e->outerCollide(x, y, z) != 0) {
                    return 1;
                }
                a = this->boundingVolumes;
            }
        }
    }
    return 0;
}

void PlayerFighter::initPush(const Vector &target, int radius) {
    Vector current = this->getPosition();
    Vector offset = target - current;
    float ratio = AbyssEngine::AEMath::VectorLength(offset) / static_cast<float>(radius);
    float clamped = ratio < 1.0f ? ratio : 1.0f;
    int duration = static_cast<int>((1.0f - clamped) * 5000.0f);
    this->pushTimer = duration;
    this->pushDuration = duration;

    Vector here = this->getPosition();
    this->pushNormal = AbyssEngine::AEMath::VectorNormalize(here - target);

    auto *random = static_cast<AERandom *>(Globals::rnd);
    float spinX = static_cast<float>(random->nextInt(200) - 100);
    float spinY = static_cast<float>(random->nextInt(200) - 100);
    float spinZ = static_cast<float>(random->nextInt(200) - 100);
    Vector spin = {spinX, spinY, spinZ};
    this->pushImpulse = 0.2f * AbyssEngine::AEMath::VectorNormalize(spin);
}

void PlayerFighter::setExhaustVisible(bool vis) {
    if (!this->geometry) return;
    AEGeometry *geom = this->subGeometry ? this->subGeometry : this->geometry;
    const int id = geom->childTransform;
    if (id != -1)
        static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(id))->SetVisible(vis);
}

void PlayerFighter::render() {
    if (this->wreckGeometry)
        this->wreckGeometry->render();

    const bool active = this->player->isActive() != 0;
    int st = this->state;
    if (!active) {
        if (st != 5) return;
    } else if ((unsigned) (st - 3) < 2) {
        if (this->explosion) {
            this->explosion->render();
            st = this->state;
        }
        if (st == 4) {
            if (this->field_0x1f8 >= 300) return;
        } else if (st != 3) {
            return;
        }

        if (st == 4 || this->subGeometry) {
            const unsigned id = this->subGeometry->transform;
            Matrix saved = *static_cast<Matrix *>(Globals::Canvas->TransformGetLocal(id));
            Globals::Canvas->TransformSetLocal(id, this->player->transformMatrix);
            this->subGeometry->render();
            Globals::Canvas->TransformSetLocal(id, saved);
        } else {
            this->geometry->render();
        }
        return;
    }

    if (this->subGeometry) {
        const unsigned id = this->subGeometry->transform;
        Matrix saved = *static_cast<Matrix *>(Globals::Canvas->TransformGetLocal(id));
        Globals::Canvas->TransformSetLocal(id, this->player->transformMatrix);
        this->subGeometry->render();
        Globals::Canvas->TransformSetLocal(id, saved);
    } else {
        this->geometry->render();
    }
    if (this->trail)
        this->trail->render();
}

void PlayerFighter::push(int dt) {
    if (this->pushTimer < 1) return;

    this->pushTimer -= dt;
    const float duration = static_cast<float>(this->pushDuration);
    const float fraction = static_cast<float>(this->pushTimer) / duration;
    Matrix basis{1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, 0.0f,
                 1.0f, 1.0f, 1.0f};
    Matrix rotated = AbyssEngine::AEMath::MatrixSetRotation(
        basis, fraction * this->pushImpulse.x,
        fraction * this->pushImpulse.y, fraction * this->pushImpulse.z);

    const int64_t frameDelta = static_cast<int64_t>(this->deltaTimeHi) * 0x100000000LL +
                               static_cast<uint32_t>(this->deltaTime);
    if (frameDelta >= 1) {
        rotated = this->geometry->getMatrix() * basis;
        this->geometry->setMatrix(rotated);
    }

    const Vector step = this->pushNormal * static_cast<float>(frameDelta);
    const Vector atSpeed = step * this->currentSpeed;
    const float scale = (2.0f - fraction) * 3.0f * (duration / 5000.0f);
    this->geometry->translate(atSpeed * scale);
}

void PlayerFighter::reset() {
    KIPlayer::reset();
    this->crateCaptured = 1;

    Vector spawn = {this->posX, this->posY, this->posZ};
    this->workingPosition = spawn;
    this->renderPosition = spawn;

    this->deltaTime = 0;
    this->deltaTimeHi = 0;
    this->field_0x38 = 0;
    this->field_0x12e = 0;
    this->field_0x148 = 0;
    this->field_0x12c = 0;
    this->maneuverTimer = 0;
    this->field_0x1c0 = 0;
    this->field_0x1c4 = 0;
    this->field_0x1c8 = 0;
    if (this->state != 5) {
        this->state = 0;
    }

    const Vector zero = {0, 0, 0};
    this->resetVecA = zero;
    this->resetVecB = zero;
    this->resetVecC = zero;

    this->isMissionCrate = 0;
    this->missionCrateLost = 0;
    this->missionCrateCaptured = 0;
    this->crateLost = 0;
    this->crateCaptured = 1;
    this->field_0x140 = 0;
    this->field_0x1fc = 0;
    this->field_0x13c = 0;
    this->cloakActive = 0;
    this->cloakTimer = 0;
    this->cloakDuration = 0;
    this->cloakCooldown = 0;
    this->cloakingPossible = 1;
    this->aiDisabled = 0;
}

void PlayerFighter::handleCloaking() {
    if (this->shipGroup != 10) return;
    if (!this->geometry) return;
    if (this->isDead()) return;
    if (this->player->empDisabledByte) return;
    if (!this->cloakingPossible) return;

    PaintCanvas *canvas = Globals::Canvas;
    if (this->cloakActive) {
        int delta = this->cloakTimer;
        if (delta == 0) {
            this->field_0x13c = 1;
            if (this->cloakMaterial == 0xffffffff) {
                canvas->MeshCloneMaterial(this->subGeometry->meshId, this->cloakMaterial);
                AbyssEngine::Mesh *mesh = canvas->MeshGetPointer(this->subGeometry->meshId);
                this->cloakSavedMode = static_cast<AbyssEngine::Material *>(mesh->material)->materialMode;
            }
            static_cast<AbyssEngine::Material *>(canvas->MaterialGetMaterial(this->cloakMaterial))->materialMode = 14;
            canvas->MeshChangeMaterial(this->subGeometry->meshId, this->cloakMaterial);
            if (this->field_0x2d9) {
                this->field_0x2d9 = 0;
                delta = 2000;
                this->cloakTimer = 2000;
            }
        }
        int total = static_cast<int>(static_cast<unsigned>(delta) + static_cast<unsigned>(this->deltaTime));
        this->cloakTimer = total;

        // ARM compares against the timer value from before this frame's increment.
        if (total - this->deltaTime <= 2000) {
            if (total >= 2000) {
                this->setExhaustVisible(false);
                this->exhaustHidden = 1;
            }
            AbyssEngine::Mesh *mesh = canvas->MeshGetPointer(this->subGeometry->meshId);
            canvas->MeshChangeShaderAnimValue(mesh, static_cast<float>(this->cloakTimer) / 2000.0f, 1);
            return;
        }
        if (total > this->cloakDuration) {
            this->cloakTimer = 0;
            this->cloakActive = 0;
            this->field_0x13c = 0;
            static_cast<AbyssEngine::Material *>(canvas->MaterialGetMaterial(this->cloakMaterial))->materialMode =
                this->cloakSavedMode;
            this->setExhaustVisible(true);
            return;
        }
        if (total <= this->cloakDuration - 2000) return;
        this->exhaustHidden = 0;
        AbyssEngine::Mesh *mesh = canvas->MeshGetPointer(this->subGeometry->meshId);
        float alpha = (static_cast<float>(this->cloakTimer) -
                       static_cast<float>(this->cloakDuration - 2000)) / -2000.0f + 1.0f;
        canvas->MeshChangeShaderAnimValue(mesh, alpha, 1);
        return;
    }

    if (this->field_0x1e0 && static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 49) {
        this->cloak(-1, false);
        return;
    }
    int acc = this->deltaTime + this->cloakCooldown;
    this->cloakCooldown = acc;
    if (8000 < acc) {
        this->cloakCooldown = 0;
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(100) <= 29)
            this->cloak(-1, false);
    }
}

void PlayerFighter::revive() {
    int enemy = ((Player *) (intptr_t) this->player)->turnedEnemyFlag;
    ((Player *) (this->player))->reset();
    if (enemy != 0) {
        ((Player *) (this->player))->turnEnemy();
    }
    this->wreckGeometry = 0;
    this->state = 1;
    this->field_0x12e = 0;
    this->field_0x38 = -1;
    this->routeClone->reset();
    this->hitpoints = ((Player *) (this->player))->getHitpoints();
    this->field_0x1dc = 0;
    this->field_0x1e0 = 0;
    this->deathTimer = 0;
    this->field_0xf0 = 0;
    this->currentSpeed = this->speed;
    this->explosion->reset();
    this->pushTimer = 0;
    this->field_0x24 = 0;
    this->setExhaustVisible(true);

    AEGeometry *geom = this->subGeometry;
    this->field_0xf5 = 1;
    if (geom == 0) {
        geom = this->geometry;
    }
    ((AEGeometry *) (geom))->setVisible(1);

    if ((unsigned) (this->shipGroup - 9) < 2) {
        delete this->lootList;
        this->lootList = 0;
    } else {
        Generator *g = new Generator();
        delete this->lootList;
        this->lootList = 0;
        this->lootList = g->getLootList(-1, -1);
        delete g;
    }
}

// Static data members present in the original binary (defined for symbol parity).
Route *PlayerFighter::stationRouteAliens;
