#include "game/ship/PlayerFixedObject.h"
#include "engine/core/AERandom.h"
#include "game/ship/TargetFollowCamera.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/mission/Generator.h"
#include "engine/render/LODManager.h"
#include "engine/render/ParticleSystemManager.h"
#include "engine/math/Transform.h"
#include "game/mission/Achievements.h"
#include "game/mission/Explosion.h"
#include "game/ui/Hud.h"
#include "game/ship/KIPlayer.h"
#include "game/world/Level.h"
#include "game/mission/Mission.h"
#include "game/ship/PlayerEgo.h"
#include "game/world/Standing.h"
#include "game/mission/Status.h"
#include "engine/math/BoundingVolume.h"
#include "game/ship/Player.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"
#include "game/world/Station.h"

namespace AbyssEngine {
    namespace AEMath {
        float VectorLength(const Vector &value);
    }
}

int PlayerFixedObject::getDockingType() {
    return this->dockingType;
}

void PlayerFixedObject::setBV(Array<BoundingVolume *> *bv) {
    this->boundingVolumes = bv;
}

void PlayerFixedObject::hideShip() {
    this->shipHidden = 1;
}

int PlayerFixedObject::getTransportID() {
    return this->transportID;
}

void PlayerFixedObject::setDockingType(int v) {
    this->dockingType = v;
}

void PlayerFixedObject::setPosition(const Vector &v) {
    this->setPosition(v.x, v.y, v.z);
}

void PlayerFixedObject::translate(const Vector &d) {
    float x = (float) this->intPosX;
    float y = (float) this->intPosY;
    float z = (float) this->intPosZ;
    this->setPosition(d.x + x, d.y + y, d.z + z);
}

String PlayerFixedObject::getName() {
    return String(this->name, false);
}

void *PlayerFixedObject::setName(String name) {
    return &(this->name = name);
}

void PlayerFixedObject::setMoving(bool v) {
    this->moving = v;
}

V3 PlayerFixedObject::projectCollisionOnSurface(const Vector &vec) {
    Array<BoundingVolume *> *bv = this->wreckCollision;
    if (bv != 0 && this->state == 4) {
        return BoundingVolume::staticProjectCollisionOnSurface(vec, bv);
    }
    Array<BoundingVolume *> *bv2 = this->boundingVolumes;
    if (bv2 != 0) {
        return BoundingVolume::staticProjectCollisionOnSurface(vec, bv2);
    }
    V3 z = {0.0f, 0.0f, 0.0f};
    return z;
}

void PlayerFixedObject::setTransportID(int v) {
    this->transportID = v;
}

void PlayerFixedObject::outerCollide(Vector v) {
    this->outerCollide(v.x, v.y, v.z);
}

V3 PlayerFixedObject::getPosition() {
    V3 pos = {(float) intPosX, (float) intPosY, (float) intPosZ};
    return pos;
}

static inline bool typeIsPirateOrE(PlayerFixedObject *self) {
    int k = self->kind;
    return k == 0x37a3 || k == 0xe;
}

void PlayerFixedObject::update(int dt) {
    PlayerFixedObject *self = this;
    self->deltaTime = dt;

    bool kiFlag = (self->aiActiveCounter != -1) && (self->moving != 0);
    ((Player *) (self->player))->update(dt, kiFlag);

    Player *player = (Player *) self->player;
    unsigned char enemyFlag = 0;
    if ((self->faction & 0xfffffffe) == 8) {
        reinterpret_cast<uint8_t *>(&player->enemyFlags)[0] = 1;
        enemyFlag = 0;
    } else {
        int st = Globals::status->getStanding();
        unsigned char e = ((Standing *) ((void *) (long) st))->isEnemy(self->faction);
        player = (Player *) self->player;
        reinterpret_cast<uint8_t *>(&player->enemyFlags)[0] = e;
        if ((self->faction & 0xfffffffe) == 8) {
            enemyFlag = 0;
        } else {
            void *st2 = (void *) (long) Globals::status->getStanding();
            enemyFlag = ((Standing *) (st2))->isFriend(self->faction);
            player = (Player *) self->player;
        }
    }
    reinterpret_cast<uint8_t *>(&player->enemyFlags)[1] = enemyFlag;

    if (reinterpret_cast<Player *>(self->player)->turnedEnemy() != 0)
        ((Player *) self->player)->enemyFlags = 1;
    if (((Player *) (self->player))->isAlwaysFriend() != 0)
        ((Player *) self->player)->enemyFlags = 0x100;

    if (self->state != 6) {
        float bomb = ((Player *) (self->player))->getBombForce();
        float emp = ((Player *) (self->player))->getEmpForce();
        if (bomb > 0.0f) {
            float nb = bomb * 0.98f;
            if (nb < 0.05f) nb = 0.0f;
            ((Player *) (self->player))->setBombForce(nb);
        }
        if (emp > 0.0f) {
            float ne = emp - (float) dt;
            float clamped = ne;
            if (ne < 0.05f) clamped = 0.0f;
            self->empActive = !(ne < 0.05f);
            ((Player *) (self->player))->setEmpForce(clamped);
        }
    }

    if (self->empActive == 0 && (unsigned int) (self->state - 3) >= 2) {
        int kind = self->kind;
        bool doMove = (kind != 0x37a3);
        if (doMove) doMove = (self->moving != 0);
        if (doMove) self->moveForward(dt);

        int cm = Globals::status->getCurrentCampaignMission();
        int k2 = self->kind;
        bool skip = (cm == 0x5b && k2 == 0x494e);
        if (!skip) {
            if (k2 == 0x494a) {
                if (Globals::status->getCurrentCampaignMission() == 0x91) goto afterMotion;
                k2 = self->kind;
            }
            if (k2 != 0x4220) {
                void *t = Globals::Canvas->TransformGetTransform(
                    ((AEGeometry *) self->geometry)->transform);
                ((AbyssEngine::Transform *) (t))->Update((long long) dt, true);
            }
        }
    }
afterMotion:

    if (((Player *) (self->player))->getHitpoints() < 1 && (unsigned int) (self->state - 3) >= 2) {
        if ((char) ((Player *) self->player)->enemyFlags == 0) {
            ((Level *) (self->level))->friendDied();
        } else {
            self->level->enemyDied(self->kind, self->player->destroyedByte != 0);
        }
        if (self->kind == 0x37a3)
            self->level->pirateStationAction(false);

        self->moving = 0;
        self->state = 3;
        int cargo = ((KIPlayer *) (self))->cargoAvailable();
        // Android +0x4c is accessed as one byte in this subclass.
        reinterpret_cast<uint8_t *>(&self->hasCargo)[0] = static_cast<uint8_t>(cargo);
        if (cargo != 0) ((KIPlayer *) (self))->createCrate(0);
        self->setExhaustVisible(false);

        AEGeometry *wreck = self->wreckGeometry;
        wreck->setMatrix(((AEGeometry *) (self->geometry))->getMatrix());
        wreck = self->wreckGeometry;

        void *expl;
        if (wreck != 0) {
            wreck->setMatrix(((AEGeometry *) (self->geometry))->getMatrix());
            void *t = Globals::Canvas->TransformGetTransform(wreck->transform);
            ((AbyssEngine::Transform *) (t))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
            if (self->faction == 3 && self->moving != 0 &&
                (int) ((AEGeometry *) self->geometry)->parentTransform != -1) {
                self->wreckGeometry->addChild(self->geometry->parentTransform);
            }

            Level *lod = self->level;
            int sys = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
            lod->field_74->systemSetMatrix(sys, &self->wreckGeometry->getMatrix());
            Globals::sound->play(0x14, Globals::options[15] ? &self->position : nullptr,
                                 nullptr, 0.0f);

            lod = self->level;
            int emitHandle = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
            lod->field_74->enableSystemEmit(emitHandle, true);
            Explosion *explosion = new Explosion(0);
            self->explosion = explosion;
            explosion->addFireStreaks();
            expl = self->explosion;
            Vector pos = self->wreckGeometry->getPosition();
            Vector zeroDir = {0.0f, 0.0f, 0.0f};
            static_cast<Explosion *>(expl)->start(pos, zeroDir);
        } else {
            Level *lod = self->level;
            int sys = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
            lod->field_74->systemSetMatrix(sys, &self->geometry->getMatrix());
            Globals::sound->play(0x14, Globals::options[15] ? &self->position : nullptr,
                                 nullptr, 0.0f);
            lod = self->level;
            int emitHandle = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
            lod->field_74->enableSystemEmit(emitHandle, true);
            Explosion *explosion = new Explosion(0);
            self->explosion = explosion;
            explosion->addFireStreaks();
            expl = self->explosion;
            Vector pos = self->geometry->getPosition();
            Vector zeroDir = {0.0f, 0.0f, 0.0f};
            static_cast<Explosion *>(expl)->start(pos, zeroDir);
        }

        if (self->kind == 0xe) {
            for (unsigned int i = 0; i < self->level->getEnemies()->size(); i++) {
                KIPlayer *obj = (*self->level->getEnemies())[i];

                if ((char) obj->field_0x3e != 0) {
                    obj = (*self->level->getEnemies())[i];
                    obj->player->damage(9999999);
                }
            }
            if (self->kind == 0xe &&
                (char) ((Player *) self->player)->destroyed == 0) {
                ++Globals::status->field_118;
                if (static_cast<Achievements *>(Globals::achievements)->hasMedal(0x27, 1) == 0) {
                    float cur = (float) Globals::status->field_118;
                    float val = (float) static_cast<Achievements *>(Globals::achievements)->getValue(0x27, 1);
                    if ((int) (cur / val) < 2) {
                        void *ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                        void *hud = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getHUD();
                        cur = (float) Globals::status->field_118;
                        val = (float) static_cast<Achievements *>(Globals::achievements)->getValue(0x27, 1);
                        ((Hud *) (hud))->hudEventMedal(0x27, (int) ((cur / val) * 100.0f));
                    }
                }
            }
        }
    }

    int state = self->state;
    if (state == 3) {
        if (self->explosion != 0)
            self->explosion->update(dt, static_cast<TargetFollowCamera *>(nullptr));
        if (self->kind != 0x37a3) {
            if (self->moving != 0) {
                self->intPosZ = self->intPosZ + dt;
                self->wreckGeometry->moveForward((float) dt);
                if (self->secondaryGeometry != 0)
                    self->secondaryGeometry->moveForward((float) dt);
            }
            Matrix &m = self->wreckGeometry->getMatrix();
            *(Matrix *) ((Player *) self->player)->transform = m;
            Vector p = self->wreckGeometry->getPosition();
            self->position = p;
            Array<BoundingVolume *> *bv = self->boundingVolumes;
            if (bv != 0) {
                for (unsigned int i = 0; i < bv->size(); i++) {
                    BoundingVolume *o = (*bv)[i];
                    o->update(self->position.x, self->position.y, self->position.z);
                    bv = self->boundingVolumes;
                }
            }
        }
        self->finished = 0;
        void *t = Globals::Canvas->TransformGetTransform(
            self->wreckGeometry->transform);
        ((AbyssEngine::Transform *) (t))->Update((long long) dt, false);
        t = Globals::Canvas->TransformGetTransform(
            self->wreckGeometry->transform);
        if (((AbyssEngine::Transform *) t)->animating == 0) {
            Level *lod = (Level *) self->level;
            self->state = 4;
            {
                int emitHandle = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
                lod->field_74->enableSystemEmit(emitHandle, false);
            }
            self->explosion->reset();
            float scale = 6.0f;
            if (self->kind == 0x37e7) scale = 8.0f;
            if (self->kind == 0x37a3) scale = 8.0f;
            self->explosion->setScaling(scale);
            Vector zeroDir = {0.0f, 0.0f, 0.0f};
            self->explosion->start(self->position, zeroDir);
            self->rumbleTimer = 1;
            self->explosionElapsed = 0;
            if (((Level *) self->level)->getPlayer() != 0) {
                void *ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                if (((PlayerEgo *) (ego))->getTargetFollowCamera() != 0) {
                    ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                    void *cam = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getTargetFollowCamera();
                    Vector diff = self->position - *static_cast<TargetFollowCamera *>(cam)->getPosition();
                    float len = AbyssEngine::AEMath::VectorLength(diff);
                    float maxd = 30000.0f;
                    float use = (len < maxd) ? len : maxd;
                    self->rumblePercentage = 1.0f - use / maxd;
                    ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                    cam = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getTargetFollowCamera();
                    ((TargetFollowCamera *) (cam))->setRumblePercentage(self->rumblePercentage, 0x32);
                }
            }
        }
    } else if (state == 4) {
        self->explosionElapsed = self->explosionElapsed + dt;
        if (self->explosion != 0)
            self->explosion->update(dt, static_cast<TargetFollowCamera *>(nullptr));
        self->explosionTimer = self->explosionTimer + dt;

        bool spin = reinterpret_cast<uint8_t *>(&self->hasCargo)[0] != 0 &&
                    ((Player *) (self->player))->isActive() != 0 &&
                    self->secondaryGeometry != 0;
        if (spin) {
            // Android literals 0x1548d0/0x1548d4: 0x37800000/0x40c90fdb.
            float r = (float) (dt >> 1) * 0.0000152587890625f;
            r = (float) (int) (r * 6.2831854820251465f);
            self->secondaryGeometry->rotate(r, r, r);
            if (self->explosionTimer >= 0xea61) {
                self->KIPlayer::setActive(false);
                self->finished = 1;
            }
        } else {
            if (self->explosion != 0 && self->explosion->isPlaying() == 0) {
                if (self->explosionTimer >= 0xea61)
                    self->KIPlayer::setActive(false);
                self->finished = 1;
            }
        }

        if (self->wreckType >= 0) {
            if (self->wreckCollision != 0 && self->explosionElapsed >= 0x8d &&
                self->wreckMaterial <= -1) {
                self->position = self->geometry->getPosition();
                for (unsigned int i = 0; i < self->wreckCollision->size(); i++) {
                    BoundingVolume *o = (*self->wreckCollision)[i];
                    o->update(self->position.x, self->position.y, self->position.z);
                }
                unsigned short mat;
                switch ((unsigned int) self->wreckType) {
                    case 0: mat = 0x824c;
                        break;
                    case 1: mat = 0x8248;
                        break;
                    case 2: mat = 0x8249;
                        break;
                    case 3: mat = 0x824a;
                        break;
                    case 4: mat = 0x824b;
                        break;
                    default: mat = 0x824d;
                        break;
                }
                self->wreckMaterial = mat;
                unsigned int matOut;
                Globals::Canvas->MaterialCreate(mat, matOut);
                Globals::Canvas->MeshChangeMaterial(
                    self->wreckGeometry->meshId,
                    static_cast<uint16_t>(matOut));
            }

            if (((Level *) self->level)->getPlayer() != 0) {
                void *ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                void *cam = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getTargetFollowCamera();
                if (cam != 0 && self->rumbleTimer > 0) {
                    int v = self->rumbleTimer + dt;
                    if (v >= 0x7d0) v = 0x7d0;
                    self->rumbleTimer = v;
                    ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                    cam = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getTargetFollowCamera();
                    ((TargetFollowCamera *) (cam))->setRumblePercentage(
                        self->rumblePercentage * ((float) self->rumbleTimer / -2000.0f + 1.0f), 0x32);
                    if (self->explosion != 0 &&
                        self->explosion->isPlaying() == 0) {
                        ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                        cam = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getTargetFollowCamera();
                        ((TargetFollowCamera *) (cam))->setRumblePercentage(0.0f, 0);
                        self->rumbleTimer = 0;
                    }
                }
            }
        }
    } else if (state == 5) {
        Array<Player *> *enemies = ((Player *) (self->player))->getEnemies();
        if (enemies != nullptr) {
            self->targetEnemy = 0;
            for (unsigned int i = 0; i < enemies->size(); i++) {
                if ((*enemies)[i]->isActive() != 0) {
                    self->targetPos = (*enemies)[i]->getPosition();
                    float dx = self->position.x - self->targetPos.x;
                    float dy = self->position.y - self->targetPos.y;
                    float dz = self->position.z - self->targetPos.z;
                    const float lo = -50000.0f, hi = 50000.0f;
                    if (dx < hi && dx > lo && dy < hi && dy > lo && dz < hi && dz > lo) {
                        Player *target = self->player->getEnemy(i);
                        self->targetEnemy = static_cast<int32_t>(reinterpret_cast<intptr_t>(target));
                        self->targetPos = target->getPosition();
                        self->homingTarget.x = self->targetPos.x;
                        self->homingTarget.y = self->targetPos.y;
                        self->homingTarget.z = self->targetPos.z;
                        break;
                    }
                }
            }
        }
        float vx = self->homingTarget.x - self->position.x;
        float vy = self->homingTarget.y - self->position.y;
        float vz = self->homingTarget.z - self->position.z;
        self->homingDir.x = vx;
        self->homingDir.y = vy;
        self->homingDir.z = vz;
        const float lo = -50000.0f, hi = 50000.0f;
        if (vx < hi && vx > lo && vy < hi && vy > lo && vz < hi) {
            self->state = 1;
            self->player->setActive(true);
        }
    }

    Player *p = (Player *) self->player;
    p->mirrorPosX = self->intPosX;
    p->mirrorPosY = self->intPosY;
    p->mirrorPosZ = self->intPosZ;
}

void PlayerFixedObject::setExhaustVisible(bool v) {
    AEGeometry *geom = (AEGeometry *) this->geometry;
    if (geom != 0 && (int) geom->childTransform != -1) {
        return ((AbyssEngine::Transform *) (Globals::Canvas->TransformGetTransform(geom->childTransform)))->SetVisible(v);
    }
}

int PlayerFixedObject::collide(float x, float y, float z) {
    PlayerFixedObject *self = this;
    Array<BoundingVolume *> *a = self->wreckCollision;
    // Android collision gate is the byte at +0x8c, not KIPlayer's +0x40 flag.
    if ((a != 0 || self->state != 4) && reinterpret_cast<uint8_t *>(&self->field_0x8c)[0] != 0) {
        if (a != 0 && self->state == 4) {
            for (uint32_t i = 0; i < a->size(); i++) {
                BoundingVolume *bv = (*a)[i];
                if (bv->collide(x, y, z) != 0) return 1;
                a = self->wreckCollision;
            }
        } else {
            Array<BoundingVolume *> *b = self->boundingVolumes;
            if (b != 0) {
                for (uint32_t i = 0; i < b->size(); i++) {
                    BoundingVolume *bv = (*b)[i];
                    if (bv->collide(x, y, z) != 0) return 1;
                    b = self->boundingVolumes;
                }
            }
        }
    }
    return 0;
}

void PlayerFixedObject::setBV(BoundingVolume *bv) {
    Array<BoundingVolume *> *a = new Array<BoundingVolume *>();
    this->boundingVolumes = a;
    ArrayAdd(bv, *a);
}

void PlayerFixedObject::reset() {
    this->KIPlayer::reset();

    this->setPosition(this->spawnX, this->spawnY, this->spawnZ);

    this->targetEnemy = 0;

    {
        Vector spawn = {this->spawnX, this->spawnY, this->spawnZ};
        this->respawnPos = spawn;
    }
    {
        Vector position = this->respawnPos;
        this->position = position;
    }

    this->deltaTime = 0;
    if (this->state != 5)
        this->state = 0;

    this->targetPos = Vector{0.0f, 0.0f, 0.0f};
    this->homingTarget = Vector{0.0f, 0.0f, 0.0f};
    this->homingDir = Vector{0.0f, 0.0f, 0.0f};
}

void PlayerFixedObject::setWreckedMeshId(int meshId) {
    this->wreckMeshId = (uint16_t) meshId;
    AEGeometry *geom = new AEGeometry((uint16_t) meshId, Globals::Canvas, true);
    this->wreckGeometry = geom;
    void *t = Globals::Canvas->TransformGetTransform(geom->transform);
    *(int *) &((AbyssEngine::Transform *) t)->boundingRadius = 0x48f42400;

    int kind = this->kind;
    int sel;
    if (kind == 0xd) {
        sel = 4;
    } else if (kind == 0xe) {
        sel = 0;
    } else if (kind == 0xf) {
        if (this->faction == 3) sel = 1;
        else sel = (this->faction == 2) ? 2 : 3;
    } else if (kind == 0x37a3) {
        sel = 5;
    } else {
        sel = this->wreckType;
        if (sel < 0) return;
        this->wreckCollision = static_cast<Globals *>(Globals::globals)->getWreckCollision(sel, this->wreckGeometry);
        return;
    }
    this->wreckType = sel;
    this->wreckCollision = static_cast<Globals *>(Globals::globals)->getWreckCollision(sel, this->wreckGeometry);
}

V3 PlayerFixedObject::getProjectionVector(const Vector &vec) {
    PlayerFixedObject *self = this;
    Array<BoundingVolume *> *bv = self->wreckCollision;
    if (bv != 0 && self->state == 4) {
        int idx = self->collisionIndex;
        return (*bv)[idx]->getProjectionVector(vec);
    }
    Array<BoundingVolume *> *bv2 = self->boundingVolumes;
    if (bv2 != 0) {
        int idx = self->collisionIndex;
        return (*bv2)[idx]->getProjectionVector(vec);
    }
    V3 z = {0.0f, 0.0f, 0.0f};
    return z;
}

PlayerFixedObject::~PlayerFixedObject() {
    PlayerFixedObject *self = this;
    AEGeometry *wreck = self->wreckGeometry;
    if (wreck != self->geometry) {
        if (wreck != 0) delete wreck;
        self->wreckGeometry = 0;
    }
    Array<BoundingVolume *> *bvB = self->boundingVolumes;
    if (bvB != 0) {
        ArrayReleaseClasses(*bvB);
        delete self->boundingVolumes;
    }
    self->boundingVolumes = 0;
    Array<BoundingVolume *> *bvA = self->wreckCollision;
    if (bvA != 0) {
        ArrayReleaseClasses(*bvA);
        delete self->wreckCollision;
    }
    self->wreckCollision = 0;
    Explosion *expl = self->explosion;
    if (expl != 0) delete expl;
    self->explosion = 0;
}

void PlayerFixedObject::render() {
    if (secondaryGeometry != nullptr && shipHidden == 0)
        secondaryGeometry->render();
    switch (state) {
        case 5:
        renderShip:
            if (shipHidden == 0) geometry->render();
            return;
        case 4:
            if (shipHidden == 0) wreckGeometry->render();
            if (explosion != nullptr) explosion->render();
            return;
        case 3:
            if (shipHidden == 0) wreckGeometry->render();
            return explosion->render();
        default:
            if (player->isActive() != 0) goto renderShip;
            return;
    }
}


PlayerFixedObject::PlayerFixedObject(int kind, int param2, Player *player, AEGeometry *geom,
                                     float x, float y, float z)
    : KIPlayer(kind, -1, player, geom, x, y, z, false),
      respawnPos{}, homingTarget{}, homingDir{}, field_0x15c(0), field_0x160(0) {
    PlayerFixedObject * self = this;

    self->explosion = 0;
    self->wreckMeshId = 0;
    self->faction = param2;
    self->wreckGeometry = 0;
    self->boundingVolumes = 0;
    self->wreckCollision = 0;
    self->deltaTime = 0;
    self->targetEnemy = 0;
    self->collisionIndex = 0;
    self->field_0x170 = 0;
    self->shipHidden = 0;
    self->explosionElapsed = 0;
    self->field_0x174 = 0;
    self->intPosX = 0;
    self->intPosY = 0;
    self->intPosZ = 0;

    Vector p = {x, y, z};
    self->position = p;

    self->moving = 0;
    self->wreckType = -1;
    self->wreckMaterial = -1;
    self->dockingType = 0;
    self->intPosX = (int32_t) x;
    self->intPosY = (int32_t) y;
    self->intPosZ = (int32_t) z;

    {
        String tmp("", false);
        self->name = tmp;
    }

    void *mission = Globals::status->getMission();
    int campaign = ((Mission *) (mission))->isCampaignMission();
    bool special = false;
    if (campaign != 0) {
        int cm = Globals::status->getCurrentCampaignMission();
        if (cm == 0x28) special = true;
        else {
            cm = Globals::status->getCurrentCampaignMission();
            if (cm == 0x29) special = true;
        }
    }

    if (special) {
        if (self->lootList != 0) {
            delete self->lootList;
        }
        self->lootList = 0;
    } else {
        Generator *gen = new Generator();
        if (kind == 0x37a3) {
            self->field_0x41 = 1;
            void *station = Globals::status->getStation();
            int sidx = ((Station *) station)->getIndex();
            // Android rodata 0x1fb7e8 and 0x1fdcd4.
            static const int pirateStations[] = {1, 33, 47, 86};
            static const int pirateLoot[] = {45, 5, 28, 1, 4, 1, 17, 1};
            for (uint32_t i = 0; i < 4; i++) {
                if (pirateStations[i] == sidx) {
                    self->lootList = gen->getLootList(pirateLoot[i * 2], pirateLoot[i * 2 + 1]);
                }
            }
        } else {
            Array<int> *loot = gen->getLootList(-1, -1);
            self->lootList = loot;
            if (loot != 0) {
                int second = (kind != 0x498e) ? 0x4a88 : 0x498e;
                if (kind != 0x498e && kind != second) {
                    for (int idx = 1; (uint32_t)(idx - 1) < self->lootList->size(); idx += 2) {
                        if (kind == 0xe) {
                            int r = static_cast<AERandom *>(Globals::rnd)->nextInt(8);
                            int &cell = (*self->lootList)[idx];
                            cell = cell * (r + 5);
                        } else {
                            int r = static_cast<AERandom *>(Globals::rnd)->nextInt(4);
                            int &base = (*self->lootList)[idx];
                            base = base * (r + 2);
                            int r2 = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
                            int &cell = (*self->lootList)[idx];
                            if (cell < r2 + 8) cell = r2 + 8;
                        }
                    }
                }
            }
        }
        delete gen;
    }

    ((Player *) self->player)->spawnedFlag = 1;
    if (kind != 0x37a3) {
        self->aiActiveCounter = 0x2f;
        if (kind == 0xe) {
            self->aiActiveCounter = -1;
            self->moving = 0;
        }
    }
}

void PlayerFixedObject::setPosition(float x, float y, float z) {
    this->spawnX = x;
    this->spawnY = y;
    this->spawnZ = z;
    this->intPosX = (int32_t) x;
    this->intPosY = (int32_t) y;
    this->intPosZ = (int32_t) z;

    ((AEGeometry *) (this->geometry))->setPosition(x, y, z);
    void *m = (void *) &((AEGeometry *) (this->geometry))->getMatrix();
    *(Matrix *) ((Player *) this->player)->transform = *(const Matrix *) (m);

    this->position = this->geometry->getPosition();

    Array<BoundingVolume *> *bv = this->boundingVolumes;
    if (bv != 0) {
        for (uint32_t i = 0; i < bv->size(); i++) {
            BoundingVolume *o = (*bv)[i];
            o->update(this->position.x, this->position.y, this->position.z);
            bv = this->boundingVolumes;
        }
    }

    if (this->wreckGeometry != 0) {
        this->wreckGeometry->setPosition(x, y, z);
    }
}

void PlayerFixedObject::setDeadButSelectable() {
    void *player = this->player;
    this->moving = 0;
    ((Player *) (player))->setHitpoints(1);
    ((Player *) (this->player))->setVulnerable(false);
    this->level->lodManager->removeObject((AEGeometry *) this->geometry);
    void *geom = this->geometry;
    if (geom != 0) delete (AEGeometry *) geom;
    AEGeometry *newGeom = this->wreckGeometry;
    this->geometry = newGeom;
    void *t = Globals::Canvas->TransformGetTransform(newGeom->transform);
    ((AbyssEngine::Transform *) (t))->SetAnimationRangeInTime(
        ((AbyssEngine::Transform *) t)->animationLength,
        ((AbyssEngine::Transform *) t)->animationLength);
}

int PlayerFixedObject::outerCollide(float x, float y, float z) {
    PlayerFixedObject * self = this;
    Array<BoundingVolume *> *a = self->wreckCollision;
    if ((a != 0 || self->state != 4) && reinterpret_cast<uint8_t *>(&self->field_0x8c)[0] != 0) {
        if (a != 0 && self->state == 4) {
            for (uint32_t i = 0; i < a->size(); i++) {
                BoundingVolume *bv = (*a)[i];
                if (bv->outerCollide(x, y, z) != 0) {
                    self->collisionIndex = i;
                    return 1;
                }
                a = self->wreckCollision;
            }
        } else {
            Array<BoundingVolume *> *b = self->boundingVolumes;
            if (b != 0) {
                for (uint32_t i = 0; i < b->size(); i++) {
                    BoundingVolume *bv = (*b)[i];
                    if (bv->outerCollide(x, y, z) != 0) {
                        self->collisionIndex = i;
                        return 1;
                    }
                    b = self->boundingVolumes;
                }
            }
        }
    }
    return 0;
}

void PlayerFixedObject::moveForward(int amount) {
    float d = (float) amount;
    this->intPosZ = amount + this->intPosZ;
    ((AEGeometry *) (this->geometry))->moveForward(d);
    void *m = &((AEGeometry *) (this->geometry))->getMatrix();
    *(Matrix *) ((Player *) this->player)->transform = *(const Matrix *) (m);
    this->position = this->geometry->getPosition();
    if (this->wreckGeometry != 0) {
        this->wreckGeometry->moveForward(d);
    }
    Array<BoundingVolume *> *bv = this->boundingVolumes;
    if (bv != 0) {
        for (uint32_t i = 0; i < bv->size(); i++) {
            BoundingVolume *o = (*bv)[i];
            o->update(this->position.x, this->position.y, this->position.z);
            bv = this->boundingVolumes;
        }
    }
}
