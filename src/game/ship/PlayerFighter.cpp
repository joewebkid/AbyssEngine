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
#include "engine/render/Trail.h"
#include "game/mission/Explosion.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "game/world/Route.h"
#include "game/world/Standing.h"
#include "game/weapons/Radar.h"
#include "engine/math/Transform.h"
#include "engine/math/EaseInOutMatrix.h"
#include <cmath>



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

    self->targetRoll = 0;
    self->smoothRoll = 0;
    self->field_0x214 = 0;
    self->field_0x294 = 0x443B8000;
    self->field_0x298 = 0x41723ACE;
    self->rollSamples = 0;
    self->field_0x2a0 = 0;
    self->field_0x2a4 = 0;
    self->field_0x2a8 = 0;
    self->field_0x2ac = 0;
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
        }
    }

    // shortcut: the remaining ARM update branches are not reconstructed; replace this call when they are mapped.
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
