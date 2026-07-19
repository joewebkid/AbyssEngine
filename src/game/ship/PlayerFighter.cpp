#include "game/ship/PlayerFighter.h"
#include "engine/core/AERandom.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/Mesh.h"
#include "engine/render/Material.h"
#include "game/mission/Mission.h"
#include "engine/render/AEGeometry.h"
#include "game/mission/Generator.h"
#include "game/world/Level.h"
#include "game/mission/Status.h"
#include "engine/render/Trail.h"
#include "game/mission/Explosion.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "game/world/Route.h"
#include "game/world/Standing.h"
#include "engine/math/Transform.h"
#include "engine/math/EaseInOutMatrix.h"



void AEMath_Matrix_ctor(void *m);
// PaintCanvas::gCanvas is declared extern in engine/render/PaintCanvas.h (included above).

int AERandom_nextInt_nobound(int rng);

static inline int PF_nextInt(int rng) { return AERandom_nextInt_nobound(rng); }

float VectorSignedToFloat(int v, int mode);

int *RH_op_new_arr(unsigned int n);

void RH_op_delete_arr(void *p);

void PF_update_dead(PlayerFighter * self);

void PF_update_body(PlayerFighter *self, int dt);

void AEMath_MatrixAssign(void *dst, void *src);

void AEMath_MatrixIdentity(void *out, void *m);

void AEMath_MatrixSetRotation(void *m, float rx, float ry, float rz);

namespace AbyssEngine {
    namespace AEMath {
        float VectorLength(const Vector &value);

        Vector operator-(const Vector &lhs, const Vector &rhs);

        Vector operator*(float lhs, const Vector &rhs);
    }
}

int AERandom_nextIntB(int rng, int bound);

void PF_vscale(void *out, void *vec, float scalar);

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
    int geom = this->subGeometry;
    this->field_0xf5 = 1;
    if (geom == 0) {
        geom = this->geometry;
    }

    return ((AEGeometry *) (intptr_t) geom)->setVisible(true);
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

static void *const gSL_f1 = nullptr;
static void *const gSL_f2 = nullptr;
static void *const gSL_f3 = nullptr;

typedef int (*F1)(int geom);

typedef int (*F2)(int base, int v, int kind, int z);

typedef void (*F3)(int base, int v, int z);

void PlayerFighter::setLevel(Level *lvl) {
    ((KIPlayer *) (this))->setLevel(lvl);
    F1 f1 = (F1) gSL_f1;
    F2 f2 = (F2) gSL_f2;
    F3 f3 = (F3) gSL_f3;
    Level *lev = (Level *) (intptr_t) this->level;

    int v;
    v = f2((int) (intptr_t) lev->field_74, f1(this->geometry), 9, 0);
    this->engineTrailSystem = v;
    f3((int) (intptr_t) lev->field_74, v, 0);

    v = f2((int) (intptr_t) lev->particleEmitBoolPtr, f1(this->geometry), 0xf, 0);
    this->field_0x80 = v;
    f3((int) (intptr_t) lev->particleEmitBoolPtr, v, 0);

    v = f2((int) (intptr_t) lev->particleRenderBoolPtr, f1(this->geometry), 0x2a, 0);
    this->field_0x84 = v;
    f3((int) (intptr_t) lev->particleRenderBoolPtr, v, 0);

    v = f2((int) (intptr_t) lev->field_8c, f1(this->geometry), 0x11, 0);
    this->field_0x134 = v;
    f3((int) (intptr_t) lev->field_8c, v, 0);

    v = f2((int) (intptr_t) lev->field_8c, f1(this->geometry), 0x12, 0);
    this->field_0x138 = v;
    return f3((int) (intptr_t) lev->field_8c, v, 0);
}

static void *const gPFC_guard = nullptr;
static int *const gPFC_sharedRoute = nullptr;
static const int gPFC_defaultRoute = 0;

PlayerFighter::PlayerFighter(int faction, int wingmanCmd, Player *player, AEGeometry *geom,
                             float x, float y, float z, bool flag)
    : KIPlayer(faction, wingmanCmd, player, geom, x, y, z, flag) {
    PlayerFighter * self = this;
    int *guardP = *(int **) gPFC_guard;
    volatile int saved = *guardP;

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
    AEMath_Matrix_ctor(&self->easeBaseMatrix);
    AEMath_Matrix_ctor(&self->rollMatrix);

    int rng = (int) (intptr_t) AERandom::gRandom;

    float wp[27];
    int r;
    r = PF_nextInt(rng);
    wp[0] = VectorSignedToFloat(r - 30000, 0);
    int r1 = PF_nextInt(rng);
    int r2 = PF_nextInt(rng);
    wp[2] = VectorSignedToFloat(r2 + 20000, 0);
    wp[1] = VectorSignedToFloat(r1 - 10000, 0);
    r = PF_nextInt(rng);
    wp[3] = VectorSignedToFloat(r + 5000, 0);
    r1 = PF_nextInt(rng);
    r2 = PF_nextInt(rng);
    wp[5] = VectorSignedToFloat(r2 + 20000, 0);
    wp[4] = VectorSignedToFloat(r1 - 10000, 0);
    r = PF_nextInt(rng);
    wp[6] = VectorSignedToFloat(r + 5000, 0);
    r1 = PF_nextInt(rng);
    r2 = PF_nextInt(rng);
    wp[8] = VectorSignedToFloat(r2 + 55000, 0);
    wp[7] = VectorSignedToFloat(r1 - 10000, 0);
    r = PF_nextInt(rng);
    wp[9] = VectorSignedToFloat(r - 30000, 0);
    r1 = PF_nextInt(rng);
    r2 = PF_nextInt(rng);
    wp[10] = VectorSignedToFloat(r1 - 10000, 0);
    wp[11] = VectorSignedToFloat(r2 + 55000, 0);

    int count = PF_nextInt(rng) * 3 + 6;
    char used[16];
    for (int i = 0; i < 13; i++) used[i] = 0;
    unsigned long long bytes = (unsigned long long) (unsigned) count * 4;
    unsigned sz = (int) (bytes >> 32) != 0 ? 0xffffffff : (unsigned) bytes;
    int *pts = RH_op_new_arr(sz);
    for (int i = 0; i < count; i += 3) {
        int idx;
        do {
            idx = PF_nextInt(rng);
        } while (used[idx] != 0);
        used[idx] = 1;
        pts[i] = (int) wp[idx * 3];
        pts[i + 1] = (int) wp[idx * 3 + 1];
        pts[i + 2] = (int) wp[idx * 3 + 2];
    }
    self->route = new Route(pts, (unsigned) count);
    RH_op_delete_arr(pts);

    int *shared = gPFC_sharedRoute;
    if (*shared == 0) {
        int defPts[12];
        memcpy(defPts, &gPFC_defaultRoute, 0x30);
        *shared = (int) (intptr_t)(new Route(defPts, 0xc));
    }

    self->field_0x130 = -1;
    self->field_0x134 = -1;
    self->field_0x138 = -1;
    self->field_0x128 = 50000;
    self->rotate = 2.0f;
    self->shootError = 0;
    self->speed = 2.0f;
    self->field_0x1b0 = 0x5dc;
    self->boostProb = 5;
    self->field_0x38 = 0;
    self->field_0x140 = 0;
    self->field_0x12d = 0;
    self->field_0x13d = 0;
    self->field_0x1f8 = 0;
    self->deathTimer = 0;
    self->field_0x12f = 0;
    self->field_0x1fc = 0;
    self->field_0x1c8 = 0;
    self->field_0x1e0 = 0;
    self->field_0x1f4 = 0;
    self->wingmanCommand = wingmanCmd;

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

    self->workingPosition.x = *(float *) &flag;
    self->workingPosition.y = 0;
    self->workingPosition.z = 0;
    self->field_0x13d = 1;
    self->crateCaptured = 1;
    self->currentSpeed = self->speed;
    self->currentRotate = self->rotate;
    self->route->setLoop(0);
    ((Route *) ((void *) (long) *shared))->setLoop(0);
    self->routeClone = 0;

    if (Status::gStatus->getCurrentCampaignMission() != 0x29) {
        if (wingmanCmd == 9) {
            self->routeClone = ((Route *) ((void *) (long) *shared))->clone();
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
    self->field_0x294 = 0;
    self->field_0x298 = 0;
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
    self->cloakCooldown = 0;
    self->field_0x2cd = 0;
    self->field_0x2c9 = 0;
    self->cloakingPossible = 1;
    self->cloakMaterial = -1;
    self->aiDisabled = 0;
    self->gunSwitchTimer = 0;

    return;
}

static void *const gUpd_guard = nullptr;

void PlayerFighter::update(int dt) {
    int *guardP = *(int **) gUpd_guard;
    volatile int saved = *guardP;

    if (this->state == 4 && this->explosion->isPlaying() == 0 &&
        (this->crateCaptured == 0 || 60000 < this->deathTimer)) {
        PF_update_dead(this);
        return;
    }

    this->field_0x1c0 += dt;
    this->maneuverTimer += dt;
    if (this->wingmanCommand == 1) {
        this->wingmanCommand = 1;
    }
    this->deltaTime = dt;
    this->deltaTimeHi = dt >> 31;

    float pos[3];
    ((AEGeometry *) (pos))->getPosition();
    this->renderPosition = *(Vector *) pos;

    if (this->field_0x43 == 0) {
        unsigned char enemy;
        if ((this->wingmanCommand & 0xfffffffe) == 8) {
            enemy = 1;
        } else if (((KIPlayer *) (this))->isWingMan() != 0) {
            enemy = 0;
        } else {
            enemy = (unsigned char) ((Standing *) (Status::gStatus->getStanding()))->isEnemy(this->wingmanCommand);
        }
        ((Player *) (intptr_t) this->player)->enemyFlagsLo = enemy;
    }

    PF_update_body(this, dt);

    return;
}

void PlayerFighter::setPosition(float x, float y, float z) {
    this->posX = x;
    this->posY = y;
    this->posZ = z;

    ((AEGeometry *) (intptr_t) this->geometry)->setPosition(x, y, z);
    this->workingPosition = Vector{x, y, z};
    if (this->trail != 0) {
        this->trail->reset(this->workingPosition);
    }
    int m = (int) (intptr_t) & ((AEGeometry *) (intptr_t) this->geometry)->getMatrix();
    AEMath_MatrixAssign(((Player *) (intptr_t) this->player)->transform, (void *) m);

    return;
}

static void *const gRoll_guard = nullptr;
static const float gRoll_threshold = 0;
static const float gRoll_f18 = 0;
static const float gRoll_f1c = 0;
static const float gRoll_f20 = 0;
static const float gRoll_f24 = 0;
static const float gRoll_f28 = 0;
static const float gRoll_f2c = 0;
static const float gRoll_f30 = 0;
static const float gRoll_f34 = 0;
static const float gRoll_f38 = 0;
static const float gRoll_f3c = 0;
static const float gRoll_f40 = 0;
static const float gRoll_f44 = 0;
static const float gRoll_f48 = 0;

void PlayerFighter::roll(int angle) {
    PlayerFighter * self = this;
    int *guardP = *(int **) gRoll_guard;
    volatile int saved = *guardP;

    if (self->rollActive == 0) {
        goto done;
    }

    {
        AbyssEngine::AEMath::Matrix &m =
                ((AEGeometry *) (intptr_t) self->geometry)->getMatrix();
        float fwdX = m.m[4];
        float fwdY = m.m[5];
        float absX = (0.0f < fwdX) ? fwdX : -fwdX;
        if (0x3b < angle) angle = 0x3c;

        if (fwdY >= 0.0f && absX < gRoll_threshold) {
            unsigned char tmp[60];
            AEMath_MatrixIdentity(tmp, &self->rollMatrix);
            AEMath_MatrixAssign(&self->rollMatrix, tmp);
            self->rollActive = 0;
            self->field_0x254 = 0;
            goto done;
        }

        float bank;
        float roll;
        if (self->field_0x254 != 0) {
            roll = (fwdY == fwdY) ? gRoll_f20 : gRoll_f1c;
            bank = (fwdX < 0.0f) ? roll : gRoll_f18;
        } else {
            bank = 0.0f;
            roll = 0.0f;
            if (fwdX > 0.0f && self->field_0x255 == 1) {
                self->field_0x254 = 1;
                bank = gRoll_f48;
            } else if (fwdX < 0.0f && self->field_0x255 == 2) {
                self->field_0x254 = 1;
                bank = gRoll_f44;
            } else if (fwdX >= 0.0f) {
                if (fwdY >= 0.0f) {
                    bank = absX;
                    const float *p = (gRoll_f38 < fwdX) ? &gRoll_f40 : &gRoll_f3c;
                    bank = *p;
                } else {
                    bank = gRoll_f24;
                }
            } else {
                bank = gRoll_f28;
                if (fwdY >= 0.0f) {
                    const float *p = &gRoll_f30;
                    if (fwdX != gRoll_f2c && fwdX < gRoll_f2c) p = &gRoll_f34;
                    bank = *p;
                }
            }
        }

        if (fwdX != 0.0f) {
            self->field_0x255 = (fwdX < 0.0f) ? 1 : 2;
        }
        float fa = VectorSignedToFloat(angle, 0);
        self->rollActive = 1;
        unsigned char tmp[60];
        AEMath_MatrixSetRotation(tmp, bank * fa, 0.0f, roll);
        AEMath_MatrixAssign(&self->rollMatrix, tmp);
    }

done:
    return;
}

void PlayerFighter::cloak(int dur, bool b) {
    unsigned v;
    if (dur > 0) {
        v = (unsigned) dur;
    } else {
        v = PF_nextInt((int) (intptr_t) AERandom::gRandom) + 5000;
    }
    this->cloakActive = 1;
    this->cloakDuration = v + 4000;
    this->field_0x2d9 = b;
}

void PlayerFighter::setMissionCrate(bool on) {
    this->isMissionCrate = on;
    if (on) {
        this->lootList = new Array<int>();
        int mission = (int) (intptr_t) Status::gStatus->getMission();
        int type = ((Mission *) (mission))->getType();
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

static inline void AEMath_VectorScale(void *out, float s, void *v) {
    *(AbyssEngine::AEMath::Vector *) out =
            s * *(const AbyssEngine::AEMath::Vector *) v;
}

static void *const gIP_guard = nullptr;
static const float gIP_strength = 0;
static void *const gIP_rngFn = nullptr;

typedef int (*RngFn)(int rng, int bound);

void PlayerFighter::initPush(const Vector &target, int radius) {
    int *guardP = *(int **) gIP_guard;
    volatile int saved = *guardP;

    AbyssEngine::AEMath::Vector gp = this->getPosition();
    float pos[3] = {gp.x, gp.y, gp.z};

    float diff[3];
    *reinterpret_cast<Vector *>(diff) = target - *reinterpret_cast<const Vector *>(pos);
    float len = AbyssEngine::AEMath::VectorLength(*reinterpret_cast<const AbyssEngine::AEMath::Vector *>(diff));
    float r = VectorSignedToFloat(radius, 0);
    float t = 1.0f;
    if (len / r < 1.0f) t = len / r;
    int strength = (int) ((1.0f - t) * gIP_strength);
    this->pushTimer = strength;
    this->pushDuration = strength;

    AbyssEngine::AEMath::Vector gp2 = this->getPosition();
    float pos2[3] = {gp2.x, gp2.y, gp2.z};
    float dir[3];
    *reinterpret_cast<Vector *>(dir) =
            *reinterpret_cast<const Vector *>(pos) - *reinterpret_cast<const Vector *>(pos2);
    float norm[3];
    AbyssEngine::AEMath::VectorNormalize((Vector *) norm, (Vector *) dir);
    this->pushNormal = *(Vector *) norm;

    RngFn rng = (RngFn) gIP_rngFn;
    int rngObj = (int) (intptr_t) AERandom::gRandom;
    float rx = VectorSignedToFloat(rng(rngObj, 200) - 100, 0);
    float ry = VectorSignedToFloat(rng(rngObj, 200) - 100, 0);
    float rz = VectorSignedToFloat(rng(rngObj, 200) - 100, 0);
    float rvec[3] = {rx, ry, rz};
    float rnorm[3];
    AbyssEngine::AEMath::VectorNormalize((Vector *) rnorm, (Vector *) rvec);
    float scaled[3];
    AEMath_VectorScale(scaled, (float) strength, rnorm);
    this->pushImpulse = *(Vector *) scaled;

    return;
}

static void *const gExhaustCanvas = nullptr;

void PlayerFighter::setExhaustVisible(bool vis) {
    int geom = this->geometry;
    if (geom != 0) {
        int sub = this->subGeometry;
        int id = (sub != 0)
                     ? (int) ((AEGeometry *) (intptr_t) sub)->childTransform
                     : (int) ((AEGeometry *) (intptr_t) geom)->childTransform;
        if (id != -1) {
            unsigned t = (unsigned) (unsigned long) PaintCanvas::gCanvas->TransformGetTransform(*(unsigned *) gExhaustCanvas);

            return ((AbyssEngine::Transform *) (unsigned long) t)->SetVisible(vis);
        }
    }
}

static void *const gR_g3 = nullptr;
static void *const gR_g4 = nullptr;
static void *const gR_g5 = nullptr;

void PlayerFighter::render() {
    if (this->wreckGeometry != 0) {
        ((AEGeometry *) (0))->render();
    }
    int active = ((Player *) (this->player))->isActive();
    int st = this->state;
    unsigned *idp;
    if (active != 0 && (unsigned) (st - 3) < 2) {
        if (this->explosion != 0) {
            ((Explosion *) (0))->render();
            st = this->state;
        }
        if (st == 4) {
            if (this->field_0x1f8 >= 300) {
                goto done;
            }
            idp = *(unsigned **) gR_g4;
        } else {
            if (st != 3) {
                goto done;
            }
            if (this->subGeometry == 0) {
                return ((AEGeometry *) (intptr_t) this->geometry)->render();
            }
            idp = *(unsigned **) gR_g3;
        }
    } else {
        if (active != 0 || st != 5) {
            goto done;
        }
        if (this->subGeometry == 0) {
            ((AEGeometry *) (0))->render();
        } else {
            idp = *(unsigned **) gR_g5;
            unsigned char tmp[60];
            unsigned id = *idp;
            void *src = PaintCanvas::gCanvas->TransformGetLocal(id);
            memcpy(tmp, src, 0x3c);
            PaintCanvas::gCanvas->TransformSetLocal(
                *idp, *(const AbyssEngine::AEMath::Matrix *) (intptr_t) ((AEGeometry *) (intptr_t) this->subGeometry)->transform);
            ((AEGeometry *) (0))->render();
            PaintCanvas::gCanvas->TransformSetLocal(
                *idp, *(const AbyssEngine::AEMath::Matrix *) (intptr_t) ((AEGeometry *) (intptr_t) this->subGeometry)->transform);
        }
        if (this->trail != 0) {
            ((AbyssEngine::Trail *) (0))->render();
        }
        goto done;
    }
    {
        unsigned char tmp[60];
        unsigned id = *idp;
        void *src = PaintCanvas::gCanvas->TransformGetLocal(id);
        memcpy(tmp, src, 0x3c);
        PaintCanvas::gCanvas->TransformSetLocal(*idp, *(const AbyssEngine::AEMath::Matrix *) (intptr_t) ((AEGeometry *) (intptr_t) this->subGeometry)->transform);
        ((AEGeometry *) (0))->render();
        PaintCanvas::gCanvas->TransformSetLocal(*idp, *(const AbyssEngine::AEMath::Matrix *) (intptr_t) ((AEGeometry *) (intptr_t) this->subGeometry)->transform);
    }
done:
    ;
}

void AEMath_MatrixMul(void *out, void *m);

static void *const gPush_guard = nullptr;
static const float gPush_div = 0;

void PlayerFighter::push(int dt) {
    int *guardP = *(int **) gPush_guard;
    volatile int saved = *guardP;

    if (0 < this->pushTimer) {
        int remaining = this->pushTimer - dt;
        float fr = VectorSignedToFloat(remaining, 0);
        float ftotal = VectorSignedToFloat(this->pushDuration, 0);
        this->pushTimer = remaining;
        float frac = fr / ftotal;

        unsigned char rot[60];
        AEMath_MatrixSetRotation(rot, frac * this->pushImpulse.z, 0.0f,
                                 frac * this->pushImpulse.y);

        int lo = this->deltaTime;
        int hi = this->deltaTimeHi;
        if ((int) (unsigned) (lo == 0) <= hi) {
            void *geom = (void *) (intptr_t) this->geometry;
            void *m = (void *) &((AEGeometry *) ((int) (long) geom))->getMatrix();
            AEMath_MatrixMul(rot, m);

            ((AEGeometry *) ((int) (long) geom))->setMatrix(*(const Matrix *) rot);
            lo = this->deltaTime;
            hi = this->deltaTimeHi;
        }
        float speed = (float) (((long long) hi << 32) | (unsigned) lo);
        void *geom = (void *) (intptr_t) this->geometry;
        float ftotal2 = VectorSignedToFloat(this->pushDuration, 0);

        unsigned char a[12], b[12], c[60];
        PF_vscale(a, &this->pushNormal, speed);
        PF_vscale(b, a, this->currentSpeed);
        PF_vscale(c, b, (2.0f - frac) * 3.0f * (ftotal2 / gPush_div));
        ((AEGeometry *) (geom))->translate(*(Vector *) c);
    }

    return;
}

static void *const gReset_vfn = nullptr;

typedef void (*VFn)(void *dst, void *zeroVec);

void PlayerFighter::reset() {
    ((KIPlayer *) (this))->reset();
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

    VFn vfn = (VFn) gReset_vfn;
    int z[3] = {0, 0, 0};
    vfn(&this->resetVecA, z);
    vfn(&this->resetVecB, z);
    vfn(&this->resetVecC, z);

    this->isMissionCrate = 0;
    this->missionCrateLost = 0;
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

void PF_cloakStart(PlayerFighter * self);

void PF_cloakStop(PlayerFighter *self, int on);

void PF_cloakApply(void *meshPtr, int arg, float alpha, int flag);

static const float gHC_divIn = 0;
static const float gHC_divOut = 0;

void PlayerFighter::handleCloaking() {
    if (this->wingmanCommand != 10) return;
    if (this->geometry == 0) return;
    if (((KIPlayer *) (this))->isDead() != 0) return;
    if (((Player *) (intptr_t) this->player)->empDisabledByte != 0) return;
    if (this->cloakingPossible == 0) return;

    if (this->cloakActive != 0) {
        int delta = this->cloakTimer;
        if (delta == 0) {
            unsigned matId = this->cloakMaterial;
            this->field_0x13c = 1;
            if (matId == 0xffffffff) {
                PaintCanvas::gCanvas->MeshCloneMaterial(
                    ((AEGeometry *) (intptr_t) this->subGeometry)->meshId,
                    this->cloakMaterial);
                AbyssEngine::Mesh *mp = (AbyssEngine::Mesh *) PaintCanvas::gCanvas->MeshGetPointer(
                    ((AEGeometry *) (intptr_t) this->subGeometry)->meshId);
                matId = this->cloakMaterial;
                this->cloakSavedMode =
                    ((AbyssEngine::Material *) mp->material)->materialMode;
            }
            AbyssEngine::Material *mat =
                (AbyssEngine::Material *) PaintCanvas::gCanvas->MaterialGetMaterial(matId);
            mat->materialMode = 0xe;
            PaintCanvas::gCanvas->MeshChangeMaterial(
                ((AEGeometry *) (intptr_t) this->subGeometry)->meshId,
                this->cloakMaterial);
            if (this->field_0x2d9 == 0) {
                delta = this->cloakTimer;
            } else {
                this->field_0x2d9 = 0;
                delta = 2000;
                this->cloakTimer = 2000;
            }
        }
        long long acc = (long long) delta +
                        (((long long) this->deltaTimeHi << 32) | (unsigned) this->deltaTime);
        int total = (int) acc;
        this->cloakTimer = total;

        if (total - this->deltaTime <= 2000) {
            if (1999 < total) {
                ((PlayerFighter *) (this))->setExhaustVisible(false);
                this->exhaustHidden = 1;
            }
            void *cv = (void *) PaintCanvas::gCanvas;
            int mp = (int) (long) PaintCanvas::gCanvas->MeshGetPointer(((AEGeometry *) (intptr_t) this->subGeometry)->meshId);
            float a = VectorSignedToFloat(this->cloakTimer, 0) / gHC_divIn;
            PF_cloakApply((void *) (long) mp, (int) (long) cv, a, 1);
            return;
        } else {
            if (this->cloakDuration < total) {
                int restore = this->cloakSavedMode;
                this->cloakTimer = 0;
                this->cloakActive = 0;
                this->field_0x13c = 0;
                AbyssEngine::Material *mat =
                    (AbyssEngine::Material *) PaintCanvas::gCanvas->MaterialGetMaterial(this->cloakMaterial);
                mat->materialMode = restore;
                PF_cloakStop(this, 1);
                return;
            }
            if (total <= this->cloakDuration - 2000) {
                return;
            }
            this->exhaustHidden = 0;
            void *cv = (void *) PaintCanvas::gCanvas;
            int mp = (int) (long) PaintCanvas::gCanvas->MeshGetPointer(((AEGeometry *) (intptr_t) this->subGeometry)->meshId);
            float a = VectorSignedToFloat(this->cloakTimer, 0);
            float b = VectorSignedToFloat(this->cloakDuration - 2000, 0);
            float alpha = (a - b) / gHC_divOut + 1.0f;
            PF_cloakApply((void *) (long) mp, (int) (long) cv, alpha, 1);
            return;
        }
    }

    if (this->field_0x1e0 != 0 && PF_nextInt((int) (intptr_t) AERandom::gRandom) < 0x32) {
        PF_cloakStart(this);
        return;
    }
    int acc = this->deltaTime + this->cloakCooldown;
    this->cloakCooldown = acc;
    if (8000 < acc) {
        this->cloakCooldown = 0;
        if (PF_nextInt((int) (intptr_t) AERandom::gRandom) < 0x1e) {
            PF_cloakStart(this);
        }
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

    int geom = this->subGeometry;
    this->field_0xf5 = 1;
    if (geom == 0) {
        geom = this->geometry;
    }
    ((AEGeometry *) (geom))->setVisible(1);

    if ((unsigned) (this->wingmanCommand - 9) < 2) {
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
int PlayerFighter::stationRouteAliens;
