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

V3 BV_staticProjectCollisionOnSurface(void *vec, void *bvArray);

V3 BV_getProjectionVector(void *bv);

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
    return this->name;
}

void *PlayerFixedObject::setName(String name) {
    return this->name = name;
}

void PlayerFixedObject::setMoving(bool v) {
    this->moving = v;
}

V3 PlayerFixedObject::projectCollisionOnSurface(const Vector &vec) {
    Array<BoundingVolume *> *bv = this->wreckCollision;
    if (bv != 0 && this->state == 4) {
        return BV_staticProjectCollisionOnSurface((void *) &vec, bv);
    }
    Array<BoundingVolume *> *bv2 = this->boundingVolumes;
    if (bv2 != 0) {
        return BV_staticProjectCollisionOnSurface((void *) &vec, bv2);
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

static FModSound **g_pfo_fmod = nullptr;

static void **g_pfo_audioFlag = nullptr;

static void **g_pfo_egoA = nullptr;

static const int g_pfo_dmgVal = 0;

// Minimal models of two untyped runtime handles referenced via *g_pfo_*.
// Only the accessed fields are named; the leading padding preserves the
// byte offsets used by the original code.

// Object behind *g_pfo_audioFlag: a settings/audio block whose byte at 0xf
// gates positional explosion audio.
struct PfoAudioSettings {
    char _pad0[0xf];
    char positionalAudioEnabled; // 0xf
};

// Object behind *g_pfo_egoA: the active ego/campaign block whose int at 0x118
// is the "destroyed type-0xe enemies" kill counter (achievement 0x27).
struct PfoEgoCounters {
    char _pad0[0x118];
    int destroyedEnemyCount; // 0x118
};

#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PfoAudioSettings, positionalAudioEnabled) == 0xf,
              "PfoAudioSettings.positionalAudioEnabled offset");
static_assert(__builtin_offsetof(PfoEgoCounters, destroyedEnemyCount) == 0x118,
              "PfoEgoCounters.destroyedEnemyCount offset");
#endif

static inline bool typeIsPirateOrE(PlayerFixedObject *self) {
    int k = self->kind;
    return k == 0x37a3 || k == 0xe;
}

void PlayerFixedObject::update(int dt) {
    PlayerFixedObject *self = this;
    self->deltaTime = dt;

    bool kiFlag = (self->aiActiveCounter + 1 != 0) && (self->moving != 0);
    ((Player *) (self->player))->update(dt, kiFlag);

    Player *player = (Player *) self->player;
    unsigned char enemyFlag = 0;
    if ((self->faction & 0xfffffffe) == 8) {
        reinterpret_cast<uint8_t *>(&player->enemyFlags)[0] = 1;
        enemyFlag = 0;
    } else {
        int st = Status::gStatus->getStanding();
        unsigned char e = ((Standing *) ((void *) (long) st))->isEnemy(self->faction);
        player = (Player *) self->player;
        reinterpret_cast<uint8_t *>(&player->enemyFlags)[0] = e;
        if ((self->faction & 0xfffffffe) == 8) {
            enemyFlag = 0;
        } else {
            void *st2 = (void *) (long) Status::gStatus->getStanding();
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
            float nb = bomb * 0.95f;
            if (nb < 1.0f) nb = 0.0f;
            ((Player *) (self->player))->setBombForce(nb);
        }
        if (emp > 0.0f) {
            float ne = emp - (float) dt;
            float clamped = ne;
            if (ne < 1.0f) clamped = 0.0f;
            self->empActive = (ne >= 1.0f) ? 1 : 0;
            ((Player *) (self->player))->setEmpForce(clamped);
        }
    }

    if (self->empActive == 0 && (unsigned int) (self->state - 3) >= 2) {
        int kind = self->kind;
        bool doMove = (kind != 0x37a3);
        if (doMove) doMove = (self->moving != 0);
        if (doMove) self->moveForward(dt);

        int cm = Status::gStatus->getCurrentCampaignMission();
        int k2 = self->kind;
        bool skip = (cm == 0x5b && k2 == 0x494e);
        if (!skip) {
            if (k2 == 0x494a) {
                if (Status::gStatus->getCurrentCampaignMission() == 0x91) goto afterMotion;
                k2 = self->kind;
            }
            if (k2 != 0x4220) {
                void *t = PaintCanvas::gCanvas->TransformGetTransform(
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
            ((Level *) ((int) (__INTPTR_TYPE__) self->level))->enemyDied(0, (bool) (unsigned char) self->kind);
        }
        if (self->kind == 0x37a3)
            ((Level *) self->level)->pirateStationAction((bool) (unsigned char) self->kind);

        self->moving = 0;
        self->state = 3;
        int cargo = ((KIPlayer *) (self))->cargoAvailable();
        self->hasCargo = (unsigned char) cargo;
        if (cargo != 0) ((KIPlayer *) (self))->createCrate(0);
        self->setExhaustVisible(false);

        AEGeometry *wreck = self->wreckGeometry;
        wreck->setMatrix(((AEGeometry *) (self->geometry))->getMatrix());
        wreck = self->wreckGeometry;

        void *expl;
        if (wreck != 0) {
            wreck->setMatrix(((AEGeometry *) (self->geometry))->getMatrix());
            void *t = PaintCanvas::gCanvas->TransformGetTransform(wreck->transform);
            ((AbyssEngine::Transform *) (t))->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
            if (self->faction == 3 && self->moving != 0 &&
                (int) ((AEGeometry *) self->geometry)->parentTransform != -1) {
                ((AEGeometry *) (self->geometry))->addChild((uint32_t)(uintptr_t)self->wreckGeometry);
            }
        }

        Level *lod = (Level *) self->level;
        ParticleSystemManager *mgr = lod->field_74;
        int sys = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
        const AbyssEngine::AEMath::Matrix *m = &((AEGeometry *) (self->geometry))->getMatrix();
        mgr->systemSetMatrix(sys, m);
        int sndHandle = sys;
        Vector *pos = 0;

        if (((PfoAudioSettings *) *g_pfo_audioFlag)->positionalAudioEnabled != 0)
            pos = &self->position;
        (*g_pfo_fmod)->play(0x14, pos, (Vector *) 0, (float) sndHandle);
        lod = (Level *) self->level;
        {
            int emitHandle = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
            lod->field_74->enableSystemEmit(emitHandle, true);
        }

        Explosion *explosion = new Explosion(0);
        self->explosion = explosion;
        explosion->addFireStreaks();
        expl = self->explosion;

        char posBuf[12];
        ((AEGeometry *) ((Vector *) posBuf))->getPosition();
        Vector zeroDir = {0.0f, 0.0f, 0.0f};
        ((Explosion *) (expl))->start(*(Vector *) posBuf, zeroDir);

        if (self->kind == 0xe) {
            Array<KIPlayer *> *enemies = ((Level *) self->level)->getEnemies();
            for (unsigned int i = 0; i < enemies->size(); i++) {
                KIPlayer *obj = (*enemies)[i];

                if ((char) obj->field_0x3e != 0) {
                    obj = (*enemies)[i];
                    ((Player *) (obj->player))->damage(g_pfo_dmgVal);
                }
            }
            if (self->kind == 0xe &&
                (char) ((Player *) self->player)->destroyed == 0) {
                PfoEgoCounters *egoObj = (PfoEgoCounters *) *g_pfo_egoA;

                egoObj->destroyedEnemyCount = egoObj->destroyedEnemyCount + 1;
                if (Achievements::gAchievements->hasMedal(0x27, 1) == 0) {
                    float cur = (float) egoObj->destroyedEnemyCount;
                    float val = (float) Achievements::gAchievements->getValue(0x27, 1);
                    if ((int) (cur / val) < 2) {
                        void *ego = (void *) (intptr_t)((Level *) self->level)->getPlayer();
                        void *hud = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getHUD();
                        cur = (float) egoObj->destroyedEnemyCount;
                        val = (float) Achievements::gAchievements->getValue(0x27, 1);
                        ((Hud *) (hud))->hudEventMedal(0x27, (int) ((cur / val) * 100.0f));
                    }
                }
            }
        }
    }

    int state = self->state;
    if (state == 3) {
        if (self->explosion != 0)
            self->explosion->update(dt, self->position);
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
        void *t = PaintCanvas::gCanvas->TransformGetTransform(
            self->wreckGeometry->transform);
        ((AbyssEngine::Transform *) (t))->Update((long long) dt, true);
        t = PaintCanvas::gCanvas->TransformGetTransform(
            self->wreckGeometry->transform);
        if (((AbyssEngine::Transform *) t)->animating == 0) {
            Level *lod = (Level *) self->level;
            self->state = 4;
            {
                int emitHandle = typeIsPirateOrE(self) ? lod->field_54 : lod->field_50;
                lod->field_74->enableSystemEmit(emitHandle, true);
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
                    char cp[12];
                    ((TargetFollowCamera *) ((Vector *) cp))->getPosition();
                    *(Vector *) cp -= self->position;
                    float len = AbyssEngine::AEMath::VectorLength(*(const Vector *) cp);
                    float maxd = 50.0f;
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
            self->explosion->update(dt, self->position);
        self->explosionTimer = self->explosionTimer + dt;

        bool spin = self->hasCargo != 0 && ((Player *) (self->player))->isActive() != 0 &&
                    self->secondaryGeometry != 0;
        if (spin) {
            float r = (float) (dt >> 1) * 0.001f;
            r = (float) (int) (r * 1.0f);
            self->secondaryGeometry->rotate(r, r, r);
            if (self->explosionTimer >= 0xea61) {
                self->finished = 1;
            }
        } else {
            if (self->explosion != 0 && self->explosion->isPlaying() == 0) {
                if (self->explosionTimer >= 0xea61)
                    self->finished = 1;
            }
        }

        if (self->wreckType >= 0) {
            if (self->wreckCollision != 0 && self->explosionElapsed >= 0x8d &&
                (unsigned int) self->wreckMaterial <= 0x7fffffff) {
                char posBuf[12];
                ((AEGeometry *) ((Vector *) posBuf))->getPosition();
                self->position = *(const Vector *) ((Vector *) posBuf);
                Array<BoundingVolume *> *bv = self->wreckCollision;
                for (unsigned int i = 0; i < bv->size(); i++) {
                    BoundingVolume *o = (*bv)[i];
                    o->update(self->position.x, self->position.y, self->position.z);
                }
                unsigned short mat;
                switch ((unsigned int) self->wreckType) {
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
                PaintCanvas::gCanvas->MaterialCreate(mat, matOut);
                PaintCanvas::gCanvas->MeshChangeMaterial(
                    self->wreckGeometry->meshId,
                    matOut);
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
                        self->rumblePercentage * ((float) v / 50.0f + 1.0f), 0x32);
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
                if (((Player *) (((Player *) (self->player))->getEnemy(i)))->isActive() != 0) {
                    self->targetPos = ((Player *) (((Player *) (self->player))->getEnemy(i)))->getPosition();
                    float dx = self->position.x - self->targetPos.x;
                    float dy = self->position.y - self->targetPos.y;
                    float dz = self->position.z - self->targetPos.z;
                    const float lo = 0.0f, hi = 50.0f;
                    if (dx < hi && dx > lo && dy < hi && dy > lo && dz < hi && dz > lo) {
                        self->targetEnemy = (int32_t)(__INTPTR_TYPE__)((Player *) (self->player))->getEnemy(i);
                        self->targetPos = ((Player *) (((Player *) (self->player))->getEnemy(i)))->getPosition();
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
        const float lo = 0.0f, hi = 50.0f;
        if (vx < hi && vx > lo && vz > lo && vy < hi && vy > lo && vz < hi) {
            self->state = 1;
            ((Player *) (self->player))->setActive((bool) (unsigned char) (long) self->player);
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
        return ((AbyssEngine::Transform *) (PaintCanvas::gCanvas->TransformGetTransform(geom->transform)))->SetVisible(v);
    }
}

int PlayerFixedObject::collide(float x, float y, float z) {
    PlayerFixedObject *self = this;
    Array<BoundingVolume *> *a = self->wreckCollision;
    if ((a != 0 || self->state != 4) && self->collisionEnabled != 0) {
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

typedef void (*VecAssignFn)(void *dst, void *src);


static VecAssignFn *g_pfo_vecAssignZero = nullptr;

void PlayerFixedObject::reset() {
    this->KIPlayer::reset();

    this->setPosition(this->spawnX, this->spawnY, this->spawnZ);

    this->targetEnemy = 0;

    Vector spawn = {this->spawnX, this->spawnY, this->spawnZ};
    this->respawnPos = spawn;
    this->position = this->respawnPos;

    this->deltaTime = 0;
    if (this->state != 5)
        this->state = 0;

    VecAssignFn fn = *g_pfo_vecAssignZero;
    char zero[12] = {0};
    fn(&this->targetPos, zero);
    fn(&this->homingTarget, zero);
    fn(&this->homingDir, zero);
}

void PlayerFixedObject::setWreckedMeshId(int meshId) {
    this->wreckMeshId = (uint16_t) meshId;
    AEGeometry *geom = new AEGeometry((uint16_t) meshId, PaintCanvas::gCanvas, true);
    this->wreckGeometry = geom;
    void *t = PaintCanvas::gCanvas->TransformGetTransform(geom->transform);
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
        this->wreckCollision = Globals::gGlobals->getWreckCollision(sel, this->wreckGeometry);
        return;
    }
    this->wreckType = sel;
    this->wreckCollision = Globals::gGlobals->getWreckCollision(sel, this->wreckGeometry);
}

V3 PlayerFixedObject::getProjectionVector(const Vector &vec) {
    (void) vec;
    PlayerFixedObject *self = this;
    Array<BoundingVolume *> *bv = self->wreckCollision;
    if (bv != 0 && self->state == 4) {
        int idx = self->collisionIndex;
        return BV_getProjectionVector((*bv)[idx]);
    }
    Array<BoundingVolume *> *bv2 = self->boundingVolumes;
    if (bv2 != 0) {
        int idx = self->collisionIndex;
        return BV_getProjectionVector((*bv2)[idx]);
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
        delete bvB;
    }
    self->boundingVolumes = 0;
    Array<BoundingVolume *> *bvA = self->wreckCollision;
    if (bvA != 0) {
        ArrayReleaseClasses(*bvA);
        delete bvA;
    }
    self->wreckCollision = 0;
    Explosion *expl = self->explosion;
    if (expl != 0) delete expl;
    self->explosion = 0;
    { if (self->name.data) delete[] self->name.data; self->name.data = nullptr; self->name.length = 0; }
}

static void render_thunk_state5(void *geom) {
    ((AEGeometry *) geom)->render();
}

static void render_thunk_other(void *expl) {
    ((Explosion *) expl)->render();
}

void PlayerFixedObject::render() {
    PlayerFixedObject *self = this;
    void *g78 = self->secondaryGeometry;
    if (g78 != 0 && self->shipHidden == 0) {
        ((AEGeometry *) (g78))->render();
    }
    int state = self->state;
    void *expl;
    if (state == 5) {
    LAB_538:
        if (self->shipHidden != 0) return;
        return render_thunk_state5(self->geometry);
    }
    if (state == 4) {
        if (self->shipHidden == 0) self->wreckGeometry->render();
        expl = self->explosion;
        if (expl == 0) return;
    } else {
        if (state != 3) {
            if (((Player *) (self->player))->isActive() == 0) return;
            goto LAB_538;
        }
        if (self->shipHidden == 0) self->wreckGeometry->render();
        expl = self->explosion;
    }
    return render_thunk_other(expl);
}


static const int g_pfo_stationIdx[4] = {0, 0, 0, 0};

static const int g_pfo_lootParams[8] = {0, 0, 0, 0, 0, 0, 0, 0};

PlayerFixedObject::PlayerFixedObject(int kind, int param2, Player *player, AEGeometry *geom,
                                     float x, float y, float z)
    : KIPlayer(kind, -1, player, geom, x, y, z, false) {
    PlayerFixedObject * self = this;

    Vector zeroVec = {0.0f, 0.0f, 0.0f};
    self->respawnPos = zeroVec;
    self->homingTarget = zeroVec;
    self->homingDir = zeroVec;
    self->field_0x15c = 0;
    self->field_0x160 = 0;

    { if (self->name.data) delete[] self->name.data; self->name.data = nullptr; self->name.length = 0; }
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

    void *mission = Status::gStatus->getMission();
    int campaign = ((Mission *) (mission))->isCampaignMission();
    bool special = false;
    if (campaign != 0) {
        int cm = Status::gStatus->getCurrentCampaignMission();
        if (cm == 0x28) special = true;
        else {
            cm = Status::gStatus->getCurrentCampaignMission();
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
            void *station = Status::gStatus->getStation();
            int sidx = ((Station *) station)->getIndex();
            for (uint32_t i = 0; i < 4; i++) {
                if (g_pfo_stationIdx[i] == sidx) {
                    self->lootList = gen->getLootList(g_pfo_lootParams[i * 2 + 1], 0);
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
                            int r = AERandom::gRandom->nextInt();
                            int &cell = (*self->lootList)[idx];
                            cell = cell * (r + 5);
                        } else {
                            int r = AERandom::gRandom->nextInt();
                            int &base = (*self->lootList)[idx];
                            base = base * (r + 2);
                            int r2 = AERandom::gRandom->nextInt();
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

    Vector posVec = {x, y, z};
    ((AEGeometry *) (this->geometry))->setPosition(posVec);
    void *m = (void *) &((AEGeometry *) (this->geometry))->getMatrix();
    *(Matrix *) ((Player *) this->player)->transform = *(const Matrix *) (m);

    char buf[12];
    ((AEGeometry *) ((Vector *) buf))->getPosition();
    this->position = *(const Vector *) ((Vector *) buf);

    Array<BoundingVolume *> *bv = this->boundingVolumes;
    if (bv != 0) {
        for (uint32_t i = 0; i < bv->size(); i++) {
            BoundingVolume *o = (*bv)[i];
            o->update(this->position.x, this->position.y, this->position.z);
            bv = this->boundingVolumes;
        }
    }

    if (this->wreckGeometry != 0) {
        this->wreckGeometry->setPosition(posVec);
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
    void *t = PaintCanvas::gCanvas->TransformGetTransform(newGeom->transform);
    ((AbyssEngine::Transform *) (t))->SetAnimationRangeInTime(((AbyssEngine::Transform *) t)->animationLength, 0);
}

int PlayerFixedObject::outerCollide(float x, float y, float z) {
    PlayerFixedObject * self = this;
    Array<BoundingVolume *> *a = self->wreckCollision;
    if ((a != 0 || self->state != 4) && self->collisionEnabled != 0) {
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
    char buf[12];
    ((AEGeometry *) ((Vector *) buf))->getPosition();
    this->position = *(const Vector *) ((Vector *) buf);
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
