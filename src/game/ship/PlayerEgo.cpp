#include "game/ship/PlayerEgo.h"
#include "engine/render/Camera.h"
#include "engine/core/AERandom.h"
#include "game/ship/TargetFollowCamera.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/menu/HackingGame.h"
#include "game/mission/Item.h"
#include "game/world/Level.h"
#include "game/world/LevelScript.h"
#include "game/menu/MiningGame.h"
#include "engine/render/ParticleSystemManager.h"
#include "game/ship/PlayerAsteroid.h"
#include "game/ship/PlayerWormHole.h"
#include "game/weapons/Gun.h"
#include "engine/render/Mesh.h"
#include "engine/render/Material.h"
#include "game/world/Waypoint.h"
#include "engine/render/Engine.h"

#include "game/mission/Status.h"
#include "engine/math/Transform.h"
#include "engine/math/EaseInOutMatrix.h"
#include "engine/core/ApplicationManager.h"
#include "game/mission/Explosion.h"

#include "game/ship/KIPlayer.h"

#include "game/mission/Mission.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerFixedObject.h"
#include "game/weapons/RepairBeam.h"
#include "game/world/Route.h"
#include "game/world/SpacePoint.h"
#include "game/core/String.h"
#include "game/weapons/TractorBeam.h"
#include "game/ship/Ship.h"
#include "game/core/Globals.h"

namespace AbyssEngine {
    namespace AEMath {
        Matrix operator*(const Matrix &lhs, const Matrix &rhs);
    }
}

namespace {
    void *g_boost_fmod;
    void *g_FMod_singleton;
    void *g_setRotation_transform;
    void *g_dockToPlanet_fmod;
    void *g_rotate_transform;
    static void *g_explode_obj = nullptr;
    static void (*g_explode_fn)(void *, int) = nullptr;
    static PlayerEgo *g_PlayerEgo_singleton = nullptr;
    static void *g_emerg_fmod = nullptr;
    static const float g_PE_rollNudge = 0.17f;
    static const float g_PE_strafeDist = 0.0f; // TODO: extract exact literal from PlayerEgo::initManeuver(type==3) asm/data-ref.
    static const float g_PE_cft_transformVal = 1000000.0f; // IDA: checkForTurret writes 0x49742400 to Transform+0xE0.
    static const float g_PE_shakeDiv = 40000.0f;
    static const float g_PE_camFov = 1.22f;
    static const float g_PE_camNear = 20.0f;
    static void *g_engine_fmod = nullptr;
    static char **g_PE_d_miningGate = nullptr;
    static const float g_PE_d_eps = 70.0f;
    static const float g_PE_d_lookK1 = 0.00024414f;
    static const float g_PE_d_lookK2 = 6.2832f;
    static const float g_PE_d_manK = 0.003f;
    static const float g_PE_d_loadK = 0.4f;
    static const float g_PE_d_loadB = 0.6f;
    static const float g_PE_d_rateK = 750.0f;
    static int *g_PE_adp_dockRadius = nullptr;
    static PlayerFixedObject **g_PE_adp_fixedObj = nullptr;
    static const float g_PE_dc_farNormal = 300000.0f;
    static const float g_PE_dc_farAlien = 450000.0f;
    static const unsigned int g_PE_dc_defY = 0x43160000u;
    static const unsigned int g_PE_dc_defZ = 0x43fa0000u;
    static const float g_PE_r_loadK = 0.4f;
    static const float g_PE_r_loadB = 0.6f;
    static const float g_PE_r_rateK = -750.0f;
    static const float g_PE_r_turK1 = 200.0f;
    static const float g_PE_r_turK2 = 0.00024414f;
    static const float g_PE_r_turK3 = -6.2832f;
    static const float g_PE_r_manK1 = -0.003f;
    static const float g_PE_r_manK2 = 0.01f;
    static const float g_PE_l_loadK = 0.4f;
    static const float g_PE_l_loadB = 0.6f;
    static const float g_PE_l_rateK = 750.0f;
    static const float g_PE_l_turK1 = 200.0f;
    static const float g_PE_l_turK2 = 0.00024414f;
    static const float g_PE_l_turK3 = 6.2832f;
    static const float g_PE_l_manK1 = 0.003f;
    static const float g_PE_l_manK2 = -0.01f;
    static const float g_PE_mtp_strafeEps = 0.01f;
    static const float g_PE_mtp_strafeReset = 0.1f;
    static const float g_PE_mtp_strafeK = 0.7f;
    static const float g_PE_astApproach = 2500.0f;
    static const float g_PE_rollLevelEps = 0.015f;
    static const float g_PE_cc_alarmDist = 16000.0f;
    static char **g_PE_u_miningGate = nullptr;
    static const float g_PE_u_eps = -500.0f;
    static const float g_PE_u_eps2 = -250.0f;
    static const float g_PE_u_lookK1 = 0.00024414f;
    static const float g_PE_u_lookK2 = -6.2832f;
    static const float g_PE_u_manK = -0.003f;
    static const float g_PE_u_loadK = 0.4f;
    static const float g_PE_u_loadB = 0.6f;
    static const float g_PE_u_rateK = -750.0f;
    static int *g_PE_tm_hum = nullptr;
    static const float g_PE_tm_farNormal = 300000.0f;
    static const float g_PE_tm_farAlien = 450000.0f;
    static const float g_PE_strafeLoadK = 0.4f;
    static const float g_PE_strafeLoadB = 0.6f;
    static const float g_PE_strafeAccelK = 0.002f;
    static float *g_PE_dr_posLock = nullptr;
    static float *g_PE_dr_posNoLock = nullptr;
    static float *g_PE_dr_posBlink = nullptr;
    static float *g_PE_dr_posNormal = nullptr;
    static const float g_PE_upd_handlingBias = 2.7f;
    static void **g_PE_reviveSound = nullptr;
    static float *g_PE_t_anchor = nullptr;
    static String **g_PE_t_pctStr = nullptr;
    static const float g_PE_t_timerDiv = 500.0f;
    static const float g_PE_t_pctScale = 100.0f;
    static const float g_PE_t_textDiv = 1.6f;
    static void **g_PE_aa_levelHolder = nullptr;
    static int *g_PE_aa_mineSound = nullptr;
    static void **g_PE_aa_winHolder1 = nullptr;
    static void **g_PE_aa_winHolder2 = nullptr;
    static const float g_PE_aa_settleEps = -1024.0f;
    static FModSound **g_PE_hs_sound = nullptr;
    static const float g_PE_hs_throttleBias = 0.2f;
    static const float g_PE_ss_emDiv = 500.0f;
    static const float g_PE_ss_emBias = 0.1f;
    static int *g_PE_tc_sound = nullptr;
}

// Local 4-wide row-major matrix buffer used to build a transform on the stack.
// 0x30 bytes; the named diagonal members live at offsets 0x0/0x14/0x28.
struct PE_MatrixBuf {
    float m00;        // 0x00
    float pad0[4];    // 0x04..0x13
    float m11;        // 0x14
    float pad1[4];    // 0x18..0x27
    float m22;        // 0x28
    float pad2[1];    // 0x2c
};
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(PE_MatrixBuf) == 0x30, "PE_MatrixBuf size");
static_assert(__builtin_offsetof(PE_MatrixBuf, m00) == 0x00, "m00");
static_assert(__builtin_offsetof(PE_MatrixBuf, m11) == 0x14, "m11");
static_assert(__builtin_offsetof(PE_MatrixBuf, m22) == 0x28, "m22");
#endif

// Mesh material block: only the pointer at offset 0x20 is referenced here.
struct PE_MaterialBlock {
    char pad0[0x20];      // 0x00..0x1f
    void *materialList;   // 0x20
};
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PE_MaterialBlock, materialList) == 0x20, "materialList");
#endif

// PlayerAsteroid mining-target view: the int flag at object offset 0x44 is
// cleared when mining starts. asteroidTarget points at the base; the original
// code addressed it as (base + 4) + 0x40.
struct PE_AsteroidMineTarget {
    char pad0[0x44];   // 0x00..0x43
    int miningFlag;    // 0x44
};
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(PE_AsteroidMineTarget, miningFlag) == 0x44, "miningFlag");
#endif


static inline Status *PE_status() { return Status::gStatus; }





















int Station_getIndex(void *);






void stopShooting_extA(void *, int);

void stopShooting_extB(void *, int, int);

void *TransformGetLocal(void *, int);

void MatrixSetRotation(void *, void *, float, float, float);



int aeabi_idiv_(int a, int b);

float PE_pitchRampDelta(PlayerEgo *self, float rate, int frameTime);

int PE_adp_approach(PlayerEgo *self, void *station);

int PE_adp_glide(PlayerEgo * self);
void PE_adp_apply(PlayerEgo * self);


float PE_yawRampDelta(float rate, int frameTime);


void Mat_assign(void *dst, const void *src);

void Vec_assign(void *dst, const void *src);

void PE_mtp_steer(PlayerEgo *self, const float *target, int steer, float speed);



void Vec_sub(void *out, const void *a, const void *b);

float Vec_length(const void *v);

int PE_hat_aimAndFire(PlayerEgo *self, int dt);

float PE_roll_bankFactor(PlayerEgo *self, float rx, float ry, float *outZ);


void hitCamera_(PlayerEgo * self);

void PE_cc_wormhole(PlayerEgo *self, void *obj);

void PE_cc_obstacle(PlayerEgo *self, void *obj, unsigned idx);

void PE_cc_destructible(PlayerEgo *self, void *obj);


void PlayEngineSound_(PlayerEgo * self);

void *PE_dtdp_makeEase(const void *fromMatrix, const void *navPoint);

void PE_upd_boost(PlayerEgo *self, int dt);

void PE_upd_docksFinishDelivery(PlayerEgo *self, void *radio);

void PE_um_dodgeStep(PlayerEgo * self);

void PE_um_strafeTarget(PlayerEgo *self, float *out);

void PE_um_strafeGlide(PlayerEgo * self);


void Mat_getPosition(void *out, const void *m);

void Mat_getUp(void *out, const void *m);

void Mat_getDir(void *out, const void *m);

void Mat_getLookAt(void *out, const void *eye, const void *dir, const void *up);

void PE_htv_applyShake(PlayerEgo *self, int dt, void *eye, void *dir);

int PE_aa_approachStep(PlayerEgo *self, int hud2, void *radar);

void Mat_mul(void *out, const void *a, const void *b);

void PE_handleShip_orient(PlayerEgo *self, int dt, unsigned int tfHandle);

void *TractorBeam_new(void *geo, int kind);



void MatrixGetPosition(void *, void *);

void *MovingStars_ctor(void *self);

void PlayerEgo::setVisible(bool v) {
    this->visible = v;
    this->field_0x309 = v;
}

// IDA 0x9aea4: _ZN9PlayerEgo5boostEv
void PlayerEgo::boost() {
    if (this->boostingFlag != 0) return;
    if (this->field_0x146 == 0) return;
    if (this->rocketControlGun != 0) return;
    if (this->boostTimer < 0) return;
    float v = (float) this->boostSpeed;
    this->boostTimer = 0;
    this->boostingFlag = 1;
    void *snd = *(void **) g_boost_fmod;
    ((float &) this->speed) = v;
    ((FModSound *) (*(void **) snd))->play((int) (intptr_t) this->boostSoundId, (Vector *) 0, (Vector *) 0, v);
}

float PlayerEgo::getDriveChargeRate() {
    float d = (float) this->driveCharge / (float) this->driveChargeMax;
    return d >= 0.0f ? d : 1.0f;
}

unsigned char PlayerEgo::isAutoPilot() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->autoPilot);
}

bool PlayerEgo::goingToWormhole() {
    void *m = this->level;
    void *r4 = ((void *&) this->autoPilotTarget);
    Array<KIPlayer *> *lm = ((Level *) (m))->getLandmarks();
    return r4 == (void *) (*lm)[3];
}

int PlayerEgo::getCurrentSecondaryWeaponIndex() {
    PlayerEgo *self = this;
    return this->currentSecondaryWeaponIndex;
}

int PlayerEgo::getHitpoints() {
    return ((Player *) this->player)->getHitpoints();
}

unsigned char PlayerEgo::isChargingDrive() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->chargingDrive);
}

int PlayerEgo::shouldSwitchToFreeLookCam() {
    if (this->switchToStandardCam == 0) return 0;
    this->switchToStandardCam = 0;
    return 1;
}

int PlayerEgo::getHandling() {
    PlayerEgo *self = this;
    return this->handling;
}

void PlayerEgo::setComputerControlled(bool v) {
    PlayerEgo *self = this;
    this->computerControlled = v;
}

int PlayerEgo::getRocketBanking() {
    PlayerEgo *self = this;
    return this->rocketBanking;
}

bool PlayerEgo::isDockingToDockingPoint() {
    if (this->dockedFlag != 0) return this->dockingPointIndex != 1;
    return false;
}

void PlayerEgo::forceBoost() {
    this->boostTimer = 0;
    this->boostingFlag = 1;
    this->speed = 0x41000000;
    this->field_0xcc = 0x2710;
    this->field_0xd0 = 0;
}

int PlayerEgo::isDead() {
    PlayerEgo *self = this;
    return ((PlayerEgo *) (self))->getHitpoints() < 1;
}

int PlayerEgo::getAutoPilotTarget() {
    PlayerEgo *self = this;
    return this->autoPilotTarget;
}

bool PlayerEgo::goingToPlanet() {
    if (this->autoPilot != 0 && ((PlayerEgo *) (this))->goingToStream() == 0 && ((PlayerEgo *) (this))->goingToStation()
        == 0)
        return this->goingToWaypointFlag == 0;
    return false;
}

float PlayerEgo::getCloakRechargeRate() {
    return 1.0f - (float) this->cloakRechargeTimer / (float) this->cloakRechargeMax;
}

unsigned char PlayerEgo::isDockedToStream() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->dockedToStream);
}

int PlayerEgo::getTargetFollowCamera() {
    PlayerEgo *self = this;
    return this->targetFollowCamera;
}

unsigned char PlayerEgo::isCloaked() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->cloaked);
}

void PlayerEgo::setFreeLookMode(bool v) {
    PlayerEgo *self = this;
    this->freeLookMode = v;
}

void PlayerEgo::setThrust(float v) {
    PlayerEgo *self = this;
    this->thrust = v;
}

bool PlayerEgo::readyForCloak() {
    if (this->chargingCloak == 0) return false;
    return this->cloakCharge >= this->cloakDischargeMax;
}

int PlayerEgo::getHackingGameDockIndex() {
    int v = this->hackingGame;
    if (v == 0) return -1;
    return ((HackingGame *) v)->getDockingIndex();
}

void PlayerEgo::setPosition(float x, float y, float z) {
    void *g = this->geometry;
    char v[12];
    *(float *) (v + 0) = x;
    *(float *) (v + 4) = y;
    *(float *) (v + 8) = z;
    ((AEGeometry *) (g))->setPosition(*(Vector *) v);
}

void PlayerEgo::dockToStream(bool param) {
    if (param) {
        this->docked = 0x100;
        return;
    }
    this->speed = 0x40000000;
    ((PlayerEgo *) (this))->setPosition(this->rotX, this->rotY, this->rotZ);
    this->freeze = 0;
    this->field_0x145 = 0;
    this->docked = 0;
    if (this->geometry != 0)
        ((AEGeometry *) this->geometry)->setVisible(this->field_0x309 != 0);
}

void PlayerEgo::hackingShuffle() {
    int v = this->hackingGame;
    if (v != 0) ((HackingGame *) v)->reInit();
}

int PlayerEgo::getDockTransferedAmount() {
    PlayerEgo *self = this;
    return this->dockTransferedAmount;
}

unsigned char PlayerEgo::goingToWaypoint() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->goingToWaypointFlag);
}

bool PlayerEgo::isDockedToDockingPoint() {
    if (this->dockedFlag == 0) return false;
    return this->dockingPointIndex == 1;
}

int PlayerEgo::getDockTotalAmount() {
    PlayerEgo *self = this;
    return this->dockTotalAmount;
}

bool PlayerEgo::isInWormhole() {
    if (((PlayerEgo *) (this))->getHitpoints() > 0) return this->inWormhole != 0;
    return false;
}

void PlayerEgo::setRocketControl(Gun *gun, AEGeometry *geo) {
    Level *lvl = this->level;
    this->rocketControlGun = (int) (intptr_t) gun;
    int psm_arg = lvl->movingStarsIndex;
    void *psm = (void *) (intptr_t) lvl->skybox2Mesh;
    if (gun == 0) {
        ((ParticleSystemManager *) (psm))->systemSetMatrix(psm_arg, (Matrix *) ((Player *) this->player)->transform);
        this->rocketBanking = 0;
        return;
    }
    Matrix *m = &geo->getReferenceMatrix();
    ((ParticleSystemManager *) (psm))->systemSetMatrix(psm_arg, m);
    if (this->geometry != 0)
        ((AEGeometry *) this->geometry)->setVisible(this->field_0x309 != 0);
}

bool PlayerEgo::isMining() {
    PlayerEgo *self = this;
    return this->miningGame != 0;
}

void PlayerEgo::turnHorizontal(int a, float v) {
    if (v < 0.0f) {
        this->yawAccumulator -= this->yawRate;
        return;
    }
    if (v > 0.0f) {
        this->yawAccumulator += this->yawRate;
        return;
    }
}

int PlayerEgo::getThrust() {
    PlayerEgo *self = this;
    return ((int &) this->thrust);
}

float PlayerEgo::getCloakRate() {
    float a = (float) this->cloakCharge;
    float b = (float) (this->cloaked == 0 ? this->cloakDischargeMax : this->cloakChargeMax);
    float d = a / b;
    return d >= 0.0f ? d : 1.0f;
}

void PlayerEgo::resetLastHP() {
    this->lastHP = ((Player *) (this->player))->getCombinedHP();
}

// IDA 0x9a50c: _ZN9PlayerEgo17setExhaustVisibleEb
void PlayerEgo::setExhaustVisible(bool param) {
    Level *lvl = this->level;
    this->exhaustVisible = param;
    lvl->field_80->flagsLow = param;
    Array<int> *arr = lvl->field_a8;
    if (arr != nullptr) {
        for (unsigned i = 0; i < arr->size(); i++) {
            ((ParticleSystemManager *) (this->level->field_80))->enableSystemEmit((*arr)[i], param);
        }
    }
}

int PlayerEgo::shouldSwitchToStandardCam() {
    if (((char &) this->field_0xb0) == 0) return 0;
    ((char &) this->field_0xb0) = 0;
    return 1;
}

void PlayerEgo::resetMovement() {
    this->pitchAccumD = 0;
    this->rollAccum = 0;
    this->yawAccumD = 0;
}

bool PlayerEgo::isHacking() {
    PlayerEgo *self = this;
    return this->hackingGame != 0;
}

void PlayerEgo::setPosition(Vector v) {
    this->geometry->setPosition(v);
}

void PlayerEgo::resetChargingDrive() {
    PlayerEgo *self = this;
    ((uint8_t &) this->chargingDrive) = 0;
}

void PlayerEgo::PauseEngineSound() {
    ((Player *) this->player)->PauseEngineSound();
}

unsigned char PlayerEgo::isInFreeLookMode() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->freeLookMode);
}

void PlayerEgo::setActive(bool) {
    ((Player *) this->player)->setActive(false);
}

void PlayerEgo::alignToHorizon() {
    PlayerEgo *self = this;
    ((uint8_t &) this->rolling) = 1;
}

void PlayerEgo::setAutoTurret(bool on) {
    this->autoTurretEnabled = on;
    if (!on) ((Player *) this->player)->stopShooting(2);
}

void PlayerEgo::hitCamera() {
    this->hitShake = 1;
    int cam = this->targetFollowCamera;
    this->hitShakeTimer = 0;
    ((TargetFollowCamera *) cam)->setRumblePercentage(1.0f, 250);
}

int PlayerEgo::hackingWon() {
    int v = this->hackingGame;
    if (v == 0) return 0;
    return ((HackingGame *) v)->gameWon();
}

unsigned char PlayerEgo::lostMiningGame() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->lostMiningGameFlag);
}

int PlayerEgo::getCurrentMiningAmount() {
    int v = this->miningGame;
    if (v == 0) return 0;
    return ((MiningGame *) v)->getOreAmount();
}

void PlayerEgo::setDockingState(int s) {
    if (s == 2 && this->dockingPointIndex == 1) this->field_0xb2 = 1;
    this->dockingPointIndex = s;
}

void PlayerEgo::hackingRotateLCW() {
    if (this->hackingGame != 0 && ((HackingGame *) (this->hackingGame))->isRotating() == 0
        && ((HackingGame *) (this->hackingGame))->gameWon() == 0)
        ((HackingGame *) this->hackingGame)->rotateLeftCW(true);
}

bool PlayerEgo::isInDockingProcedure() {
    if (((char &) this->dockingState) != 0) return true;
    return this->dockedFlag != 0;
}

void PlayerEgo::setSpeed(float v) {
    PlayerEgo *self = this;
    ((float &) this->speed) = v;
}

void PlayerEgo::ResumeEngineSound() {
    ((Player *) this->player)->ResumeEngineSound(false);
}

void PlayerEgo::addNukeVolatileForce(float v) {
    Player *p = (Player *) this->player;
    p->flShake = p->flShake + v * 3.0f;
}



void PlayerEgo::explode() {
    ((ParticleSystemManager *) (this->level->field_74))->enableSystemEmit(this->currentSystem, 1);
    if (((int &) this->explosion) != 0) return;
    ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setActive(0);
    Explosion *e = new Explosion(0);
    ((int &) this->explosion) = (int) (intptr_t) e;
    static_cast<Player *>(this->player)->setActive(false);
    void *o = g_explode_obj;
    void (*fn)(void *, int) = g_explode_fn;
    fn(*(void **) o, ((ExplosionEmitterHolder *) (*(void **) o))->emitterFirstId);
    fn(*(void **) o, this->field_0x1c);
    fn(*(void **) o, 0x1b);
    fn(*(void **) o, 0x23);
    fn(*(void **) o, 0x8d5);
    fn(*(void **) o, 0x8d4);
    fn(*(void **) o, 0x8cc);
    fn(*(void **) o, 0x447);
    fn(*(void **) o, 0x448);
    fn(*(void **) o, 0x449);
    Explosion *exp = (Explosion *) this->explosion;
    if (exp != 0) {
        Vector pos = ((PlayerEgo *) this)->getPosition();
        exp->update(0, pos);
    }
}

unsigned char PlayerEgo::isDockingToStream() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->docked);
}

int PlayerEgo::goingToStream() {
    void *m = this->level;
    void *r4 = ((void *&) this->autoPilotTarget);
    Array<KIPlayer *> *lm = ((Level *) (m))->getLandmarks();
    return r4 == (void *) (*lm)[1];
}

Vec3 PlayerEgo::GetDirVector() {
    PlayerEgo *self = this;
    return ((AEGeometry *) self->geometry)->getDirection();
}

void PlayerEgo::hideShipForFirstPersonCameraView(bool param) {
    unsigned char r1 = param;
    this->field_0x32d = r1;
    unsigned char nr = r1 ^ 1;
    this->field_0x309 = (this->visible != 0) & nr;
    this->level->field_80->flagsLow = nr & (this->exhaustVisible != 0);
}

void PlayerEgo::changeThrust(float v) {
    float n = this->thrust + v;
    float p3 = 1.0f;
    if (n < 1.0f) p3 = 0.0f;
    if (n < 0.0f) p3 = 0.0f;
    float p2 = p3;
    if (n < 1.0f) p2 = n;
    if (n < 0.0f) p2 = p3;
    this->thrust = p2;
}

void PlayerEgo::deleteHackingGame() {
    HackingGame *g = (HackingGame *) ((void *&) this->hackingGame);
    if (g != 0) delete g;
    ((void *&) this->hackingGame) = 0;
}

bool PlayerEgo::explosionEnded() {
    if (((int &) this->explosion) == 0) return true;
    return 8000 < this->explosionTimer;
}

float PlayerEgo::getBoostPercentage() {
    float den = (float) (this->field_0xcc / 6);
    float d = (float) this->boostTimer / den;
    float r;
    if (d < 1.0f) {
        r = d;
    } else {
        float sub = 6.0f - d;
        r = 1.0f;
        if (5.0f < d) r = sub;
    }
    return r;
}

Vec3 PlayerEgo::GetUpVector() {
    PlayerEgo *self = this;
    return ((AEGeometry *) (self->geometry))->getUpVector();
}

bool PlayerEgo::isDockedToPlanet() {
    PlayerEgo *self = this;
    return 3000 < this->planetDockTimer;
}

bool PlayerEgo::emergencySystemActive() {
    PlayerEgo *self = this;
    return 0 < this->emergencySystemTimer;
}

bool PlayerEgo::isDockingToAsteroid() {
    if (((char &) this->dockingState) != 0) return this->dockingPointIndex != 1;
    return false;
}

float PlayerEgo::getBoostRate() {
    float d = (float) this->boostTimer / (float) this->field_0xd0;
    float r = d + 1.0f;
    if (0.0f < d) r = 1.0f;
    return r;
}

bool PlayerEgo::driveReady() {
    PlayerEgo *self = this;
    return this->driveCharge >= this->driveChargeMax;
}

void PlayerEgo::turnVertical(int a, float v) {
    if (v < -0.0f) {
        return;
    }
    if (v > 0.0f) {
        this->pitchAccumulator += this->pitchRate;
        return;
    }
}

unsigned char PlayerEgo::isInTurretMode() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->turretActive);
}

void PlayerEgo::startJumpDrive() {
    if (this->chargingDrive != 0) return;
    ((FModSound *) (*(void **) g_FMod_singleton))->play(0x21, (Vector *) 0, (Vector *) 0, 0);
    ((Hud *) (this->hud))->hudEvent(0x19, this, 0);
    this->driveCharge = 0;
    this->chargingDrive = 1;
}

bool PlayerEgo::isInRocketControl() {
    PlayerEgo *self = this;
    return this->rocketControlGun != 0;
}

int PlayerEgo::getSpeed() {
    PlayerEgo *self = this;
    return this->speed;
}

float PlayerEgo::getVolatileForce() {
    float f = ((Player *) this->player)->flShake;
    float r = 0.0f;
    if (!(f < 0.0f)) {
        r = 1.0f;
        if (!(f > 1.0f)) r = f;
    }
    return r;
}

unsigned char PlayerEgo::isDockingToPlanet() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->dockingToPlanet);
}

unsigned char PlayerEgo::isChargingCloak() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->chargingCloak);
}

bool PlayerEgo::isDockedToMiningPlant() {
    if (this->dockedFlag != 0 && this->dockingPointIndex == 1) {
        if (((Mission *) (Status::gStatus->getMission()))->isEmpty() != 0
            && Status::gStatus->inAlienOrbit() == 0) {
            return Station_getIndex(Status::gStatus->getStation()) == 0x67;
        }
    }
    return false;
}


void PlayerEgo::setCurrentSecondaryWeaponIndex(int idx) {
    this->currentSecondaryWeaponIndex = idx;
    (int &) g_PlayerEgo_singleton->gunBaseGeo = idx;
}

void PlayerEgo::removeRoute() {
    Route *r = (Route *) ((void *&) this->route);
    if (r != 0) delete r;
    ((void *&) this->route) = 0;
}

void PlayerEgo::setRoute(Route *v) {
    PlayerEgo *self = this;
    this->route = (int) (intptr_t) v;
}

unsigned char PlayerEgo::boosting() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->boostingFlag);
}

bool PlayerEgo::isDockedToAsteroid() {
    if (((char &) this->dockingState) == 0) return false;
    return this->dockingPointIndex == 1;
}

int PlayerEgo::goingToStation() {
    Array<KIPlayer *> *lm = ((Level *) (this->level))->getLandmarks();
    if ((*lm)[0] == 0) return false;
    void *r4 = ((void *&) this->autoPilotTarget);
    lm = ((Level *) (this->level))->getLandmarks();
    return r4 == (void *) (*lm)[0];
}

float PlayerEgo::getCloakingPercentage() {
    float r = 0.0f;
    if (this->cloaked != 0) {
        int v = this->cloakCharge;
        if (v >= 0) {
            if (v < 2000) {
                r = (float) v * 100.0f / 2000.0f;
            } else {
                int lo = this->cloakChargeMax - 2000;
                if (v > lo) {
                    r = (((float) v - (float) lo) / -2000.0f + 1.0f) * 100.0f;
                } else {
                    r = 100.0f;
                }
            }
        }
    }
    return r;
}

int PlayerEgo::getBoostSpeed() {
    PlayerEgo *self = this;
    return this->boostSpeed;
}

void PlayerEgo::addGun(Gun *gun, int x) {
    ((Player *) (this->player))->addGun(gun, x);
    ((Player *) this->player)->resetGunDelay(0);
}

unsigned char PlayerEgo::aboutToReachAutoTarget() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->aboutToReachAutoTargetFlag);
}

unsigned char PlayerEgo::autoTurretIsEnabled() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->autoTurretEnabled);
}

bool PlayerEgo::readyToBoost() {
    PlayerEgo *self = this;
    return -1 < this->boostTimer;
}

void PlayerEgo::endExplosion() {
    int v = ((int &) this->explosion);
    if (v != 0) ((Explosion *) v)->update(0, (TargetFollowCamera *) 0);
}

bool PlayerEgo::isLandingOrTakingOff() {
    if (this->dockedFlag == 0) return false;
    return (((uint32_t &) this->dockingPointIndex) | 1) == 3;
}

void PlayerEgo::setFreeze(bool v) {
    PlayerEgo *self = this;
    this->freeze = v;
}

int PlayerEgo::getHUD() {
    PlayerEgo *self = this;
    return this->hud;
}

int PlayerEgo::getRoute() {
    PlayerEgo *self = this;
    return this->route;
}

void PlayerEgo::setCollide(bool v) {
    PlayerEgo *self = this;
    this->collide = v;
}


int PlayerEgo::tryToStartEmergencySystem() {
    if (this->field_0xac == 0 || this->emergencySystemTimer != 0) return 0;
    if (((Player *) (this->player))->getHitpoints() > 1) return 0;
    ((Player *) (this->player))->setHitpoints(1);
    this->emergencySystemTimer = this->emergencyVal1;
    ((Player *) (this->player))->setVulnerable(0);
    void *s1 = Status::gStatus->getShip();
    Item *eq = ((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(0x1b);
    ((Ship *) (s1))->removeEquipment(eq);
    ((FModSound *) (*(void **) g_emerg_fmod))->play(0x45b, (Vector *) 0, (Vector *) 0, 1.0f);
    return 1;
}

unsigned char PlayerEgo::hasAutoTurret() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->autoTurretEquipped);
}

unsigned char PlayerEgo::collidesWithStation() {
    PlayerEgo *self = this;
    return ((uint8_t &) this->collidesWithStationFlag);
}

void PlayerEgo::stopPlanetDock() {
    ((TargetFollowCamera *) (this->targetFollowCamera))->setLookAtCam(0);
    this->collide = 1;
    this->dockingToPlanet = 0;
    ((PlayerEgo *) (this))->stopBoost();
    this->speed = 0x40000000;
}

bool PlayerEgo::isRechargingCloak() {
    PlayerEgo *self = this;
    return 0 < this->cloakRechargeTimer;
}

unsigned char PlayerEgo::hasVolatileGoods() {
    return ((uint8_t &) this->volatileGoods);
}

void PlayerEgo::hackingRotateRCW() {
    if (this->hackingGame != 0 && ((HackingGame *) (this->hackingGame))->isRotating() == 0
        && ((HackingGame *) (this->hackingGame))->gameWon() == 0)
        ((HackingGame *) this->hackingGame)->rotateRightCW(true);
}

bool PlayerEgo::hasCloak() {
    PlayerEgo *self = this;
    return this->cloak != 0;
}

int PlayerEgo::isBoostRefreshed() {
    if (this->boostingFlag != 0) return 0;
    if (this->field_0x146 == 0) return 0;
    if (this->boostTimer > -1) return 1;
    return 0;
}

Vec3 PlayerEgo::getPosition() {
    PlayerEgo *self = this;
    return ((AEGeometry *) (self->geometry))->getPosition();
}

Vec3 PlayerEgo::getTurretPosition() {
    Matrix &turret = ((AEGeometry *) (this->turretGeometry))->getMatrix();
    Matrix &ship = ((AEGeometry *) (this->geometry))->getMatrix();
    Matrix world = turret * ship;
    return MatrixGetPosition(world);
}

void Vec_scale(void *out, const void *v, float s);

void Vec_sub(void *out, const void *a, const void *b);

void Vec_assign(void *dst, const void *src);


void PlayerEgo::initManeuver(int type) {
    if ((unsigned) (type - 1) < 2 && this->volatileGoods != 0) {
        Player *player = (Player *) this->player;
        player->flShake = player->flShake + g_PE_rollNudge;
    }

    if (this->maneuverType == 0) {
        this->maneuverType = type;
        this->navPoint = 0;
        if (type == 3) {
            float pos[3];
            *(AbyssEngine::AEMath::Vector *) pos = ((PlayerEgo *) this)->getPosition();

            float dir[3];
            *(AbyssEngine::AEMath::Vector *) dir = ((AEGeometry *) this->geometry)->getDirection();

            float scaled[3];
            Vec_scale(scaled, dir, g_PE_strafeDist);

            float target[3];
            Vec_sub(target, pos, scaled);
            Vec_assign(&this->strafeTargetVec, target);

            Vec_assign(&this->facingVec, dir);
        }
    }
}

void PlayerEgo::refillGunDelay() {
    ((Player *) this->player)->refillGunDelay(0);
}

void PE_cft_finishMaterials(void *canvas, int mesh, void *out);

void PE_cft_place(PlayerEgo *self, int turretIdx);


void PlayerEgo::checkForTurret() {
    if (this->turretMode != 0)
        return;
    int avail = ((Player *) (this->player))->gunAvailable(0);
    this->turretMode = (unsigned char) avail;
    if (avail == 0)
        return;

    this->autoTurretEquipped = 0;
    Array<Item *> *equip = ((Ship *) (PE_status()->getShip()))->getEquipment(2);
    Item **item = equip->data_;
    this->turretPitch = (int) ((double) item[0]->getAttribute(0) * 1.5);

    int idx = item[0]->getIndex();
    unsigned short base = 0xffff, barrel = 0xffff;
    int muzzle = -1, child = -1, extra = -1, extra2 = -1;

    if (idx == 0xe0) {
        base = 0x499a;
        barrel = 0x499b;
        muzzle = 0x499c;
        child = 0x499d;
    } else if (idx == 0x30) {
        base = 0x1a74;
        barrel = 0x1a75;
    } else if (idx == 0x31) {
        base = 0x1a76;
        barrel = 0x1a77;
    } else if (idx == 0xb4) {
        this->autoTurretEquipped = 1;
        base = 0x1a95;
        barrel = 0x1a96;
    } else if (idx == 0xb5) {
        this->autoTurretEquipped = 1;
        base = 0x1a97;
        barrel = 0x1a98;
    } else if (idx == 0xb6) {
        this->autoTurretEquipped = 1;
        base = 0x1a99;
        barrel = 0x1a9a;
    } else if (idx == 0xc6) {
        base = 0x4963;
        barrel = 0x4964;
        muzzle = 0x4966;
        child = 0x4967;
        extra = -1;
        extra2 = 0x4a7f;
    } else if (idx == 0xc7) {
        base = 0x4968;
        barrel = 0x4969;
        muzzle = 0x496a;
        child = 0x496b;
        extra = -1;
        extra2 = 0x4a7f;
    } else if (idx == 0xc8) {
        base = 0x496c;
        barrel = 0x496d;
        muzzle = 0x496e;
        child = 0x496f;
        extra = 0x4970;
        extra2 = 0x4a7f;
    } else if (idx == 0x2f) {
        base = 0x1a72;
        barrel = 0x1a73;
    }

    void *canvas = (void *) PaintCanvas::gCanvas;

    void *baseGeo = (void *) new AEGeometry((uint16_t) base, (PaintCanvas *) canvas, false);
    this->gunBaseGeo = baseGeo;
    void *yawGeo = (void *) new AEGeometry((uint16_t) barrel, (PaintCanvas *) canvas, false);
    this->gunYawGeo = yawGeo;
    ((AEGeometry *) yawGeo)->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
    void *muzzleRoot = (void *) new AEGeometry((PaintCanvas *) canvas);
    this->gunMuzzleRoot = muzzleRoot;

    if (muzzle != -1) {
        void *g = (void *) new AEGeometry((uint16_t)(unsigned short)muzzle, (PaintCanvas *) canvas, false);
        ((AEGeometry *) (this->gunBaseGeo))->addChild((uint32_t)(uintptr_t)g);
        delete (AEGeometry *) g;
    }
    if (child != -1) {
        void *g = (void *) new AEGeometry((uint16_t)(unsigned short)child, (PaintCanvas *) canvas, false);
        ((AEGeometry *) (this->gunYawGeo))->addChild((uint32_t)(uintptr_t)g);
        delete (AEGeometry *) g;
    }
    if (extra != -1) {
        void *g = (void *) new AEGeometry((uint16_t)(unsigned short)extra, (PaintCanvas *) canvas, false);
        ((AEGeometry *) (this->gunYawGeo))->addChild((uint32_t)(uintptr_t)g);
        delete (AEGeometry *) g;
    }
    if (extra2 != -1) {
        void *g = (void *) new AEGeometry((uint16_t)(unsigned short)extra2, (PaintCanvas *) canvas, false);
        this->gunExtraGeo = g;
        ((AEGeometry *) (this->gunYawGeo))->addChild((uint32_t)(uintptr_t)g);
        void *tf = PaintCanvas::gCanvas->TransformGetTransform((unsigned int) ((unsigned int) (unsigned long) PaintCanvas::gCanvas));
        ((AbyssEngine::Transform *) (tf))->SetVisible(this->gunExtraVisible != 0);
    }

    void *tf = PaintCanvas::gCanvas->TransformGetTransform((unsigned int) ((unsigned int) (unsigned long) PaintCanvas::gCanvas));
    ((AbyssEngine::Transform *) tf)->boundingRadius = g_PE_cft_transformVal;
    tf = PaintCanvas::gCanvas->TransformGetTransform((unsigned int) ((unsigned int) (unsigned long) PaintCanvas::gCanvas));
    ((AbyssEngine::Transform *) (tf))->SetAnimationState((AbyssEngine::AnimationMode) 2, (void *) 0);

    ((AEGeometry *) (this->gunBaseGeo))->setPosition(*(Vector *) &this->dockArrowImage);
    ((AEGeometry *) (this->gunYawGeo))->setPosition(*(Vector *) &this->dockArrowImage);

    PE_cft_place(this, idx);

    ((AEGeometry *) (this->gunMuzzleRoot))->addChild((uint32_t)(uintptr_t)this->gunBaseGeo);
    ((AEGeometry *) (this->gunMuzzleRoot))->addChild((uint32_t)(uintptr_t)this->gunYawGeo);

    if (this->field_0x2c0 != 0 && this->turretMode != 0) {
        ((PaintCanvas *) (long) (canvas))->MeshCloneMaterial((unsigned int) (((AEGeometry *) this->gunBaseGeo)->meshId),
                                                             (uint32_t &) this->cloakMaterial1);
        PE_cft_finishMaterials(canvas, ((AEGeometry *) this->gunYawGeo)->meshId, &this->cloakMaterial2);
    }
}

void PlayerEgo::pitchAllPrimaryGuns(float) {
    ((Player *) this->player)->pitchAllPrimaryGuns(0.0f);
}

void PlayerEgo::stopShooting(int param) {
    if (this->turretActive != 0) {
        stopShooting_extA(this->player, 2);
        return;
    }
    if (((PlayerEgo *) (this))->isDead() != 0) return;
    void *p = this->player;
    if (param == 1) {
        stopShooting_extB(p, 1, this->currentSecondaryWeaponIndex);
        return;
    }
    stopShooting_extA(p, param);
}


void PlayerEgo::shake(int amount) {
    void *cam = this->geometry;
    float a = (float) amount / g_PE_shakeDiv;
    float intensity = (float) (this->shakeIntensity << 1);

    int range = (int) (a * intensity);
    if (range < 2)
        range = 1;
    int span = range << 1;

    float dx = (float) (AERandom::gRandom->next(span) - range);
    float dy = (float) (AERandom::gRandom->next(span) - range);
    float dz = (float) (AERandom::gRandom->next(span) - range);
    ((AEGeometry *) (cam))->translate(dx, dy, dz);
}

void PlayerEgo::setRotation(float rx, float ry, float rz) {
    char local[60];
    this->rotateX = rx;
    this->rotateY = ry;
    this->rotateZ = rz;
    void *t = *(void **) g_setRotation_transform;
    void *m = TransformGetLocal(t, this->field_0x4->transform);
    MatrixSetRotation(local, m, this->rotateX, this->rotateY, this->rotateZ);
}


void PlayerEgo::StopEngineSound() {
    if (this->dockedFlag == 0 || this->dockingPointIndex != 1) {
        if (((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(0x26) != 0
            && Status::gStatus->inAlienOrbit() == 0) {
            int idx = Station_getIndex(Status::gStatus->getStation());
            int cm = Status::gStatus->getCurrentCampaignMission();
            float g = Status::gStatus->getGammaRayDamagePerSecond(idx, cm);
            if (0.0f < g && this->engineSoundId != -1) {
                ((FModSound *) (*(void **) g_engine_fmod))->play((int) (intptr_t)((void *&) this->engineSoundId),
                                                                 (Vector *) 0, (Vector *) 0, g);
            }
        }
    }
    ((Player *) this->player)->StopEngineSound();
}

void PlayerEgo::startSmokeEmission() {
    int v = this->smokeSystem;
    if (v < 0) return;
    ((ParticleSystemManager *) (this->level->particleEmitBoolPtr))->enableSystemEmit(v, 1);
    ((ParticleSystemManager *) (intptr_t) this->level->particleRenderBoolPtr)->enableSystemEmit(this->explosionSmoke, true);
}



// IDA 0xa0b90: _ZN9PlayerEgo4downEif
float PlayerEgo::down(int frameTime, float delta) {
    if (((void *&) this->miningGame) != 0) {
        if (((MiningInputFlags *) *g_PE_d_miningGate)->invertAxisFlag == 0)
            return ((MiningGame *) (((void *&) this->miningGame)))->down(delta);
        return ((MiningGame *) (((void *&) this->miningGame)))->up(-delta);
    }

    if (this->turretActive != 0) {
        float ft = (float) frameTime;
        if (this->lookYaw < g_PE_d_eps) {
            float ang = ft * delta + this->lookYaw;
            this->lookYaw = ang;
            ((AEGeometry *) this->turretGeometry)->rotate((float) (ang * g_PE_d_lookK1 * g_PE_d_lookK2), 0.0f, 0.0f);
        }
        float p = this->lookPitch;
        if (p < g_PE_d_eps) {
            float half = ft * delta * 0.5f;
            float ang = half * g_PE_d_lookK1 * g_PE_d_lookK2;
            this->lookPitch = half + p;
            return (((AEGeometry *) this->dockCameraMid)->rotate((float) ang, 0.0f, 0.0f), 0.0f);
        }
        return p;
    }

    if (((void *&) this->rocketControlGun) != 0) {
        float v = (float) frameTime * g_PE_d_manK * ((Gun *) (intptr_t) this->rocketControlGun)->pitchRate;
        this->maneuverParam = v;
        return v;
    }

    if (this->autoPilot != 0)
        return delta;
    if (this->dockedFlag != 0 && this->dockingPointIndex != 1)
        return delta;

    this->pitchAccumDir = -1;
    float rate;
    if (this->hardCoreMode == 0) {
        rate = ((float &) this->handling);
    } else {
        float cur = (float) ((Ship *) (PE_status()->getShip()))->getCurrentLoad();
        float max = (float) ((Ship *) (PE_status()->getShip()))->getMaxLoad();
        rate = ((float &) this->handling) * (1.0f - cur / max) * g_PE_d_loadK + ((float &) this->handling) *
               g_PE_d_loadB;
    }

    float target = (float) aeabi_idiv_((int) (delta * g_PE_d_rateK * rate), 0x3f);
    ((float &) this->yawAccumD) = delta;
    this->pitchRamp = -rate;
    if (((float &) this->pitchAccumD) < target) {
        float v = ((float &) this->pitchAccumD) + PE_pitchRampDelta(this, rate, frameTime);
        if (target < v) v = target;
        ((float &) this->pitchAccumD) = v;
    }
    return target;
}




static inline void adp_clearDockVector(PlayerEgo *self) {
    self->dockApproachDist = 0;
    self->dockApproachThreshold = 0;
    self->field_0x1d0 = 0;
}

int PlayerEgo::approachDockingPoint(Hud *hud, int /*hud2*/, Radar *radar) {
    if (((KIPlayer *) (this))->isDying() != 0)
        return 0;

    int state = ((int &) this->emergencyVec.x);

    if (state == 0) {
        void *station = this->dockStation;
        Vector pos = ((PlayerEgo *) (this))->getPosition();
        void *nav = ((KIPlayer *) (station))->getNearestNavigationPoint(pos, (SpacePoint *) &this->navPoint);
        if (nav != 0) {
            if (this->strafeNavPoint != nav) {
                if (this->strafeNavPoint != 0)
                    ((SpacePoint *) (this->strafeNavPoint))->giveFree();
                this->strafeNavPoint = nav;
                ((SpacePoint *) (nav))->take();
            }
            int dist = PE_adp_approach(this, station);
            if (dist < this->dockApproachThreshold) {
                ((int &) this->emergencyVec.x) = 2;
                ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setLookAtCam(false);
                ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->useTargetsUpVector(false);
            }
        }
        PE_adp_apply(this);
        return 0;
    }

    if (state == 2) {
        if (this->gunExtraVisible != 0)
            ((PlayerEgo *) (this))->setTurretMode(0);
        int dist = PE_adp_glide(this);
        int radius = g_PE_adp_dockRadius[((Ship *) (PE_status()->getShip()))->getIndex()];
        if (dist < radius) {
            int ev = 0;
            {
                void *mission = PE_status()->getMission();
                void *campaign = (void *) PE_status()->getCampaignMission();
                PlayerFixedObject *fixed = *g_PE_adp_fixedObj;

                if (((Mission *) (mission))->isEmpty() == 0 && ((Mission *) (mission))->getType() == 0xf
                    && fixed->getDockingType() == 1) {
                    if (((Ship *) (PE_status()->getShip()))->hasCargo(((Mission *) (mission))->getProductionGoodIndex(), 1) != 0) {
                        int amount = ((Item *) (((Ship *) (PE_status()->getShip()))->getCargo(
                            ((Mission *) (mission))->getProductionGoodIndex())))->getAmount();
                        ((int &) this->autoTurretEquipped) = amount;
                        int need = ((Mission *) (mission))->getProductionGoodAmount()
                                   - ((Level *) (this->level))->getNumDeliveredOre();
                        if (need < amount) ((int &) this->autoTurretEquipped) = need;
                        this->autoTurretTimer = 0;
                        if (((int &) this->autoTurretEquipped) > 0) ev = 0x29;
                    }
                } else if (((Mission *) (mission))->isEmpty() == 0
                    && (((Mission *) (mission))->getType() == 0xb8 || ((Mission *) (mission))->getType() == 0xa8)
                    && fixed->getDockingType() == 2) {
                    int carried = fixed->intPosX;
                    int maxPax = ((Ship *) (PE_status()->getShip()))->getMaxPassengers();
                    bool handled = false;
                    if (maxPax > 0 && carried < maxPax) {
                        int avail = (((Mission *) (mission))->getType() == 0xa8)
                                        ? ((Mission *) (mission))->getStatusValue()
                                        : maxPax;
                        avail -= carried;
                        ((int &) this->autoTurretEquipped) = avail;
                        int status = ((Mission *) (mission))->getStatusValue() - carried;
                        if (status < avail) ((int &) this->autoTurretEquipped) = status;
                        this->autoTurretTimer = 0;
                        if (((int &) this->autoTurretEquipped) > 0) { ev = 0x23; handled = true; }
                    }
                    if (!handled && ((Mission *) (mission))->getType() != 0xa8
                        && ((Ship *) (PE_status()->getShip()))->getMaxPassengers() == 0)
                        ev = 0x2b;
                } else if (((Mission *) (mission))->isEmpty() == 0 && ((Mission *) (mission))->getType() == 0xb8
                    && fixed->getDockingType() == 1) {
                    int n = fixed->intPosX;
                    if (n > 0) {
                        this->autoTurretTimer = 0;
                        ((int &) this->autoTurretEquipped) = n;
                        ev = 0x25;
                    }
                } else if (campaign != 0 && ((Mission *) (campaign))->isEmpty() == 0
                    && (((Mission *) (campaign))->getType() == 0xa7 || ((Mission *) (campaign))->getType() == 0xae)
                    && fixed->getDockingType() == 1) {
                    if (((Ship *) (PE_status()->getShip()))->hasCargo(((Mission *) (campaign))->getProductionGoodIndex(), 1) != 0) {
                        int amount = ((Item *) (((Ship *) (PE_status()->getShip()))->getCargo(
                            ((Mission *) (campaign))->getProductionGoodIndex())))->getAmount();
                        ((int &) this->autoTurretEquipped) = amount;
                        int need = ((Mission *) (campaign))->getProductionGoodAmount() - ((Mission *) (campaign))->getStatusValue();
                        if (need < amount) ((int &) this->autoTurretEquipped) = need;
                        this->autoTurretTimer = 0;
                        if (((int &) this->autoTurretEquipped) > 0) ev = 0x29;
                    }
                }
            }
            if (ev != 0)
                hud->hudEvent(ev, this, 0);
            ((int &) this->emergencyVec.x) = 1;
        }
        PE_adp_apply(this);
        return 0;
    }

    if (state == 3) {
        int dist = PE_adp_glide(this);
        if (dist < 200) {
            this->dockStation = 0;
            radar->dockTargetPtr = 0;
            radar->dockNavPtr = 0;
            adp_clearDockVector(this);
            ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setActive(true);
            ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setLookAtCam(false);
            ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->useTargetsUpVector(false);
            ((LevelScript *) (this->levelScript))->resetCamera(this->levelScript->m_pLevel);
            ((Player *) (this->player))->resetGunDelay(0);
            ((int &) this->emergencyVec.x) = 0;
            if (this->strafeNavPoint != 0) {
                ((SpacePoint *) (this->strafeNavPoint))->giveFree();
                this->strafeNavPoint = 0;
            }
            if (((void *&) this->hackingGame) != 0) {
                delete (HackingGame *) ((void *&) this->hackingGame);
                ((void *&) this->hackingGame) = 0;
                hud->setHackingGameActive(false);
            }
            return 1;
        }
        PE_adp_apply(this);
    }
    return 0;
}

void PlayerEgo::setLevel(Level *level) {
    ((int &) this->level) = (int) (intptr_t) level;
    void *src = (void *) (intptr_t) level->field_74;
    Matrix *gm = &((AEGeometry *) this->geometry)->getMatrix();
    void *sys = (void *) ((ParticleSystemManager *) (src))->addSystem(gm, ParticleSettings::ParticleSet_9, 0);
    this->currentSystem = (int) (intptr_t) sys;
    ((ParticleSystemManager *) (this->level->field_74))->enableSystemEmit((int) (intptr_t) sys, 0);
    if (Status::gStatus->getCurrentCampaignMission() > 1) return;
    void *src2 = (void *) (intptr_t) this->level->particleEmitBoolPtr;
    Matrix *gm2 = &((AEGeometry *) this->geometry)->getMatrix();
    void *sys2 = (void *) ((ParticleSystemManager *) (src2))->addSystem(gm2, ParticleSettings::ParticleSet_0xf, 0);
    this->smokeSystem = (int) (intptr_t) sys2;
    ((ParticleSystemManager *) (this->level->particleEmitBoolPtr))->enableSystemEmit((int) (intptr_t) sys2, 0);
    void *src3 = (void *) (intptr_t) this->level->particleRenderBoolPtr;
    Matrix *gm3 = &((AEGeometry *) this->geometry)->getMatrix();
    void *sys3 = (void *) ((ParticleSystemManager *) (src3))->addSystem(gm3, ParticleSettings::ParticleSet_0x2a, 0);
    this->explosionSmoke = (int) (intptr_t) sys3;
    ((ParticleSystemManager *) (intptr_t) this->level->particleRenderBoolPtr)->enableSystemEmit((int) (intptr_t) sys3, false);
}


void PlayerEgo::setDockingCamera() {
    if (this->dockCameraNode == 0) {
        PaintCanvas::gCanvas->CameraCreate(this->turretCamera);
        float farPlane = (PE_status()->inAlienOrbit() != 0) ? g_PE_dc_farAlien : g_PE_dc_farNormal;
        PaintCanvas::gCanvas->CameraSetPerspective(this->turretCamera, g_PE_camFov, g_PE_camNear, farPlane);

        void *node = (void *) new AEGeometry(PaintCanvas::gCanvas);
        this->dockCameraNode = node;
        ((AEGeometry *) node)->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);

        if (this->turretOffsetVec.x == 0.0f && this->turretOffsetVec.y == 0.0f && this->turretOffsetVec.z == 0.0f) {
            ((uint32_t &) this->turretOffsetVec.y) = g_PE_dc_defY;
            ((uint32_t &) this->turretOffsetVec.z) = g_PE_dc_defZ;
        }
        ((AEGeometry *) (this->dockCameraNode))->translate(this->turretOffsetVec);

        void *mid = (void *) new AEGeometry(PaintCanvas::gCanvas);
        this->dockCameraMid = mid;
        ((AEGeometry *) (mid))->translate(*(Vector *) &((AEGeometry *) mid)->transform);
        ((AEGeometry *) (this->dockCameraNode))->addChild((uint32_t)((AEGeometry *) this->dockCameraMid)->transform);

        void *leaf = (void *) new AEGeometry(PaintCanvas::gCanvas);
        this->dockCameraLeaf = leaf;
        ((AEGeometry *) (this->dockCameraNode))->addChild((uint32_t)((AEGeometry *) this->dockCameraNode)->transform);
    }

    ((AEGeometry *) (this->dockCameraLeaf))->setPosition(this->dockOffsetVec);
    void *leaf = this->dockCameraLeaf;
    ((AEGeometry *) leaf)->setMatrix(((AEGeometry *) (this->geometry))->getMatrix());

    PaintCanvas::gCanvas->CameraSetCurrent((unsigned int) (this->turretCamera));
}


// IDA 0xa0704: _ZN9PlayerEgo5rightEif
float PlayerEgo::right(int frameTime, float delta) {
    if (((void *&) this->miningGame) != 0)
        return (((MiningGame *) (((void *&) this->miningGame)))->right(delta), ((MiningGame *) (((void *&) this->
                    miningGame)))->inputX);

    if (this->turretActive != 0) {
        float pitch = (float) this->turretPitch;
        float ft = (float) frameTime;
        float ang = ((ft * delta) / (g_PE_r_turK1 / pitch)) * g_PE_r_turK2 * g_PE_r_turK3;
        ((AEGeometry *) this->dockCameraNode)->rotate(0.0f, (float) ang, 0.0f);
        ((AEGeometry *) this->rollGeometry)->rotate(0.0f, (float) ang, 0.0f);
        ((AEGeometry *) this->turretGeometry)->rotate(0.0f, (float) ang, 0.0f);
        return ang;
    }

    if (((void *&) this->rocketControlGun) != 0) {
        float ft = (float) frameTime;
        this->field_0x80 = delta * g_PE_r_manK1 * ((Gun *) (intptr_t) this->rocketControlGun)->pitchRate;
        ((float &) this->rocketBanking) = ((float &) this->rocketBanking) + (ft * delta) * g_PE_r_manK2;
        return ft * delta;
    }

    if (this->autoPilot != 0)
        return delta;
    if (this->dockedFlag != 0 && this->dockingPointIndex != 1)
        return delta;

    this->yawAccumDir = -1;
    float rate;
    if (this->hardCoreMode == 0) {
        rate = ((float &) this->handling);
    } else {
        float cur = (float) ((Ship *) (PE_status()->getShip()))->getCurrentLoad();
        float max = (float) ((Ship *) (PE_status()->getShip()))->getMaxLoad();
        rate = ((float &) this->handling) * (1.0f - cur / max) * g_PE_r_loadK + ((float &) this->handling) *
               g_PE_r_loadB;
    }

    float target = (float) aeabi_idiv_((int) (delta * g_PE_r_rateK * rate), 0x3f);
    ((float &) this->rollAccum) = delta;
    this->yawRamp = -rate;
    if (this->yawAccumF > target) {
        float step = PE_yawRampDelta(rate, frameTime);
        float v = this->yawAccumF - step;
        if (v < target) v = target;
        this->yawAccumF = v;
    }
    return target;
}


// IDA 0xa094c: _ZN9PlayerEgo4leftEif
float PlayerEgo::left(int frameTime, float delta) {
    if (((void *&) this->miningGame) != 0)
        return (((MiningGame *) (((void *&) this->miningGame)))->left(-delta), ((MiningGame *) (((void *&) this->
                    miningGame)))->inputX);

    if (this->turretActive != 0) {
        float pitch = (float) this->turretPitch;
        float ft = (float) frameTime;
        float ang = ((ft * delta) / (g_PE_l_turK1 / pitch)) * g_PE_l_turK2 * g_PE_l_turK3;
        ((AEGeometry *) this->dockCameraNode)->rotate(0.0f, (float) ang, 0.0f);
        ((AEGeometry *) this->rollGeometry)->rotate(0.0f, (float) ang, 0.0f);
        ((AEGeometry *) this->turretGeometry)->rotate(0.0f, (float) ang, 0.0f);
        return ang;
    }

    if (((void *&) this->rocketControlGun) != 0) {
        float ft = (float) frameTime;
        this->field_0x80 = delta * g_PE_l_manK1 * ((Gun *) (intptr_t) this->rocketControlGun)->pitchRate;
        ((float &) this->rocketBanking) = ((float &) this->rocketBanking) + (ft * delta) * g_PE_l_manK2;
        return ft * delta;
    }

    if (this->autoPilot != 0)
        return delta;
    if (this->dockedFlag != 0 && this->dockingPointIndex != 1)
        return delta;

    this->yawAccumDir = 1;
    float rate;
    if (this->hardCoreMode == 0) {
        rate = ((float &) this->handling);
    } else {
        float cur = (float) ((Ship *) (PE_status()->getShip()))->getCurrentLoad();
        float max = (float) ((Ship *) (PE_status()->getShip()))->getMaxLoad();
        rate = ((float &) this->handling) * (1.0f - cur / max) * g_PE_l_loadK + ((float &) this->handling) *
               g_PE_l_loadB;
    }

    float target = (float) aeabi_idiv_((int) (delta * g_PE_l_rateK * rate), 0x3f);
    this->yawRamp = rate;
    ((float &) this->rollAccum) = -delta;
    if (this->yawAccumF < target) {
        float step = PE_yawRampDelta(rate, frameTime);
        float v = this->yawAccumF + step;
        if (target < v) v = target;
        this->yawAccumF = v;
    }
    return target;
}

PlayerEgo::~PlayerEgo() noexcept(false) {
    if (this->player) delete (Player *) this->player;
    this->player = 0;
    if (this->field_0x4) delete (AEGeometry *) this->field_0x4;
    this->field_0x4 = 0;
    if (this->geometry) delete (AEGeometry *) this->geometry;
    this->geometry = 0;
    if (this->rollGeometry) delete (AEGeometry *) this->rollGeometry;
    this->rollGeometry = 0;
    if (this->turretGeometry) delete (AEGeometry *) this->turretGeometry;
    this->turretGeometry = 0;
    if (this->route) delete (Route *) this->route;
    this->route = 0;
    if (this->dockCameraNode) delete (AEGeometry *) this->dockCameraNode;
    this->dockCameraNode = 0;
    if (this->dockCameraLeaf) delete (AEGeometry *) this->dockCameraLeaf;
    this->dockCameraLeaf = 0;
    if (this->field_0x2c) delete (AEGeometry *) this->field_0x2c;
    this->field_0x2c = 0;
    if (this->field_0x30) delete (AEGeometry *) this->field_0x30;
    this->field_0x30 = 0;
    if (this->gunYawGeo) delete (AEGeometry *) this->gunYawGeo;
    this->gunYawGeo = 0;
    if (this->dockCameraMid) delete (AEGeometry *) this->dockCameraMid;
    this->dockCameraMid = 0;
    if (this->tractorBeam) delete (TractorBeam *) this->tractorBeam;
    this->tractorBeam = 0;
    if (this->miningGame) delete (MiningGame *) (intptr_t) this->miningGame;
    this->miningGame = 0;
    if (this->explosion) delete (Explosion *) (intptr_t) this->explosion;
    this->explosion = 0;
    if (this->explosion2) delete (Explosion *) (intptr_t) this->explosion2;
    this->explosion2 = 0;
    if (this->easeMatrix) {
        ((AbyssEngine::EaseInOutMatrix *) this->easeMatrix)->~EaseInOutMatrix();
        ::operator delete(this->easeMatrix);
    }
    this->easeMatrix = 0;
    if (Array<RepairBeam *> *beams = this->repairBeams) {
        ArrayReleaseClasses(*beams);
        delete beams;
    }
    this->repairBeams = 0;
}

void PlayerEgo::throttleChanged() {
    int v;
    if (this->throttleStarted == 0) {
        this->throttleStarted = 1;
        v = 0;
    } else {
        v = this->throttle;
        if ((unsigned) (v - 0x1f5) < 999) {
            v = 500;
        } else {
            if (v < 0x5dc) return;
            v = 2000 - v;
        }
    }
    this->throttle = v;
}

PlayerEgo::PlayerEgo(Player *player) {
    this->rollMatrix = AbyssEngine::AEMath::Matrix();
    this->turretHudMatrix = AbyssEngine::AEMath::Matrix();

    this->player = (void *) player;
    player->setPlayShootSound(true, 2);

    Ship *ship = (Ship *) PE_status()->getShip();
    this->boostTimer = 0;
    this->boostingFlag = 0;
    if (ship != 0) {
        this->boostDelay = ship->getBoostDelay();
        this->handling = ship->getBoostTime();
        this->boostSpeed = ship->getBoostSpeed();
        this->field_0xb0 = ship->getHandling();
    }

    ((int &) this->explosion) = 0;
    ((int &) this->explosion2) = 0;
    this->autoTurretEnabled = 0;
    this->dockedFlag = 0;
    this->miningGame = 0;
    this->hackingGame = 0;
    this->visible = 1;
    this->field_0x309 = 1;
    this->exhaustVisible = 1;
    this->dockingPointIndex = -1;

    void *stars = ::operator new(0x1c);
    MovingStars_ctor(stars);
    this->dockCameraNode = stars;
}


void PlayerEgo::PlayEngineSound() {
    if (((Ship *) (Status::gStatus->getShip()))->getFirstEquipmentOfSort(0x26) != 0
        && Status::gStatus->inAlienOrbit() == 0) {
        int idx = Station_getIndex(Status::gStatus->getStation());
        int cm = Status::gStatus->getCurrentCampaignMission();
        float g = Status::gStatus->getGammaRayDamagePerSecond(idx, cm);
        if (0.0f < g && this->engineSoundId != -1) {
            ((FModSound *) (*(void **) g_engine_fmod))->play((int) (intptr_t)((void *&) this->engineSoundId),
                                                             (Vector *) 0, (Vector *) 0, g);
        }
    }
    ((Player *) this->player)->PlayEngineSound(0, (Vector *) 0);
}


void PlayerEgo::moveToPosition(Vector target, bool steer, float speed) {
    float t[3] = {target.x, target.y, target.z};
    PE_mtp_steer(this, t, steer ? 1 : 0, speed);

    float up[3] = {0.0f, 1.0f, 0.0f};
    ((AEGeometry *) (this->geometry))->setDirection(this->headingVec, *(Vector *) up);

    float dt = (float) this->shakeIntensity;
    ((AEGeometry *) (this->geometry))->moveForward(this->thrust * dt * ((float &) this->speed));
    ((PlayerEgo *) (this))->roll(this->shakeIntensity);

    float slide = this->strafeAccel;
    float mag = slide > 0.0f ? slide : -slide;
    if (mag <= g_PE_mtp_strafeEps) {
        ((float &) this->strafeNavPoint) = g_PE_mtp_strafeReset;
    } else {
        unsigned char m[0x30];
        Mat_assign(m, ((AEGeometry *) (this->geometry))->getMatrix());

        float v = slide * dt;
        ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->translateNoUpdate(v, 0.0f, 0.0f);
        this->strafeAccel = this->strafeAccel * g_PE_mtp_strafeK;
        ((AEGeometry *) this->geometry)->setMatrix(*(const AbyssEngine::AEMath::Matrix *) m);
    }

    Mat_assign(((Player *) this->player)->transform, ((AEGeometry *) (this->geometry))->getMatrix());
    ((AEGeometry *) (this->geometry))->getPosition();
}

void PlayerEgo::resetGunDelay() {
    ((Player *) this->player)->resetGunDelay(0);
}

int PlayerEgo::getShieldDamageRate() {
    return ((Player *) this->player)->getShieldDamageRate();
}

void PlayerEgo::handleAutoTurret(int dt) {
    int t = this->autoTurretTimer + dt;
    this->autoTurretTimer = t;
    if (t >= 0xbb9) {
        this->autoTurretTimer = 0;
        this->autoTurretTarget = 0;
        Array<KIPlayer *> *enemies = ((Level *) (this->level))->getEnemies();
        if (enemies != nullptr) {
            int best = 60000;
            for (unsigned i = 0; i < enemies->size(); i++) {
                void *e = (void *) (*enemies)[i];
                if (((KIPlayer *) (e))->isDead() != 0) continue;
                if (((KIPlayer *) (e))->isDying() != 0) continue;
                if (((Player *) (((KIPlayer *) e)->player))->isActive() == 0) continue;
                if (((KIPlayer *) (e))->isEnemy() == 0) continue;
                if (((KIPlayer *) e)->noTargetFlag != 0) continue;

                float epos[3];
                ((KIPlayer *) (e))->getPosition();
                float rel[3];
                Vec_sub(rel, epos, &this->dockOffsetVec);
                int dist = (int) Vec_length(rel);
                if (dist < best
                    && (this->autoTurretTarget == 0 || this->autoTurretTarget != this->autoTurretPrevTarget)) {
                    this->autoTurretTarget = e;
                    best = dist;
                }
            }
        }
    }

    void *target = this->autoTurretTarget;
    if (target != 0 && ((KIPlayer *) target)->geometry != 0) {
        int fireTimer = PE_hat_aimAndFire(this, dt);
        this->autoTurretFireTimer = fireTimer;
        if (fireTimer + dt > 0x1f4) {
        } else {
            return;
        }
    }

    ((Player *) (this->player))->stopShooting(0);
}

void Vec_assign(void *dst, const void *src);


void PlayerEgo::dockToAsteroid(KIPlayer *kip, Radar *radar) {
    (void) kip;
    if (((char &) this->dockingState) != 0) {
        ((PlayerAsteroid *) (((void *&) this->asteroidTarget)))->setRotationEnabled(true);
        this->field_0x145 = 0;
        ((char &) this->dockingState) = 0;
        ((void *&) this->asteroidTarget) = 0;

        int zero[3] = {0, 0, 0};
        Vec_assign(&this->dockApproachDist, zero);

        ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setActive(true);
        ((Player *) (this->player))->resetGunDelay(0);
        if (((void *&) this->miningGame) != 0)
            delete (MiningGame *) ((void *&) this->miningGame);
        ((void *&) this->miningGame) = 0;
        ((Radar *) (radar))->unlockAsteroid();
        this->dockingPointIndex = 0;
        ((PlayerEgo *) (this))->setExhaustVisible(true);
    } else if (radar != 0) {
        ((char &) this->dockingState) = 1;
        this->miningSettleTimer = 0;
        ((void *&) this->asteroidTarget) = radar;
        float dist = ((PlayerAsteroid *) (radar))->getScaling() * g_PE_astApproach;
        this->field_0x145 = 1;
        this->dockingPointIndex = 0;
        this->dockScaling = (int) dist;
    }
}

int PlayerEgo::levelCollision() {
    return 0;
}

void PlayerEgo::killLiberator() {
    char sv[12];
    Array<Array<Gun *> *> *p = ((Player *) this->player)->guns;
    if (p == 0) return;
    Array<Gun *> *arr = p->data_[1];
    if (arr == 0) return;
    if (p->count == 0) return;
    if (arr->count == 0) return;
    if (this->currentSecondaryWeaponIndex != 0xb3) return;
    *(int *) (sv + 0) = -1;
    *(int *) (sv + 4) = -1;
    *(int *) (sv + 8) = -1;
    unsigned count = arr->count;
    for (unsigned i = 0; i < count; i++) {
        int *e = (int *) arr->data_[i];
        if (e[0x16] == 0xb3) {
            *(int *) e[0xf] = -1;
            *(Vector *) ((void *) e[3]) = *(const Vector *) (sv);
            Array<Gun *> *arr2 = p->data_[1];
            Gun *e2 = arr2->data_[i];
            e2->active = 0;
            count = arr2->count;
        }
    }
}

void Mat_identity(void *out, const void *src);

void Mat_assign(void *dst, const void *src);

void Mat_setRotation(void *out, float x, float y, float z);


void PlayerEgo::roll(int amount) {
    if (this->rolling == 0)
        return;

    AbyssEngine::AEMath::Matrix &m = ((AEGeometry *) (this->geometry))->getMatrix();
    float rx = m.m11_rightY;
    float ry = m.m12_upY;
    float mag = rx > 0.0f ? rx : -rx;

    if (amount > 0x3b)
        amount = 0x3c;

    unsigned char rollMat[0x30];

    if (ry >= 0.0f && mag < g_PE_rollLevelEps) {
        Mat_identity(rollMat, &this->rollMatrix);
        Mat_assign(&this->rollMatrix, rollMat);
        this->rollDirection = 0;
        this->rolling = 0;
        this->autoLevel = 0;
        return;
    }

    float zAxis = 0.0f;
    float xScale = PE_roll_bankFactor(this, rx, ry, &zAxis);

    if (rx != 0.0f)
        this->rollDirection = (rx < 0.0f) ? 1 : 2;

    this->rolling = 1;
    Mat_setRotation(rollMat, xScale * (float) amount, 0.0f, zAxis);
    Mat_assign(&this->rollMatrix, rollMat);
}

void PlayerEgo::setTargetFollowCamera(TargetFollowCamera *cam) {
    ((void *&) this->targetFollowCamera) = cam;
    cam->resetShipHandling();
}


void PlayerEgo::calcCollision(Array<KIPlayer *> *candidates) {
    if (candidates == 0)
        return;
    if (this->dockedFlag != 0 && (unsigned) (this->dockingPointIndex - 1) < 3)
        return;

    for (unsigned i = 0; i < candidates->size(); i++) {
        void *obj = (*candidates)[i];
        if (obj == 0)
            continue;

        if (i == 0 && PE_status()->inAlienOrbit() == 0) {
            if (Vec_length(&this->dockOffsetVec) < g_PE_cc_alarmDist
                && ((KIPlayer *) obj)->proximityAlarmFlag != 0)
                this->collidesWithStationFlag = 1;
        }

        AbyssEngine::AEMath::Vector pos = ((AEGeometry *) (this->geometry))->getPosition();
        if (((KIPlayer *) obj)->outerCollide(pos) == 0)
            continue;

        if (((KIPlayer *) (obj))->getType() == 0x4262 && ((KIPlayer *) (obj))->isVisible() != 0) {
            if (((PlayerWormHole *) (obj))->isShrinking() == 0 && ((void *&) this->miningGame) == 0)
                PE_cc_wormhole(this, obj);
        } else if (((KIPlayer *) obj)->stealFlag == 0) {
            if (((KIPlayer *) (obj))->isVisible() != 0) {
                bool docking = (((char &) this->dockingState) != 0 || this->dockedFlag != 0)
                               && ((void *&) this->asteroidTarget) == obj;
                if (!docking)
                    PE_cc_obstacle(this, obj, i);
                hitCamera_(this);
            }
        } else {
            if (((KIPlayer *) (obj))->isDying() == 0 && ((KIPlayer *) (obj))->isDead() == 0) {
                bool skipDockTarget = (((char &) this->dockingState) != 0 || this->dockedFlag != 0)
                                      && obj == ((void *&) this->asteroidTarget);
                if (!skipDockTarget) {
                    PE_cc_destructible(this, obj);
                    hitCamera_(this);
                }
            }
        }
    }
}

void PlayerEgo::dockToPlanet() {
    ((TargetFollowCamera *) (this->targetFollowCamera))->setLookAtCam((bool) (unsigned char) this->targetFollowCamera);
    this->boostingFlag = 1;
    this->boostTimer = 0;
    this->speed = 0x41000000;
    this->field_0xcc = 10000;
    this->field_0xd0 = 0;
    this->collide = 0;
    ((Player *) this->player)->resetGunDelay(0);
    float f = 1.0f;
    void *snd = *(void **) g_dockToPlanet_fmod;
    ((char &) this->gunMuzzleRoot) = 0;
    this->dockingToPlanet = 1;
    this->speed = 0x41000000;
    this->planetDockTimer = 0;
    ((FModSound *) (*(void **) snd))->play(5, (Vector *) 0, (Vector *) 0, f);
}



// IDA 0xa0dfc: _ZN9PlayerEgo2upEif
float PlayerEgo::up(int frameTime, float delta) {
    if (((void *&) this->miningGame) != 0) {
        if (((MiningInputFlags *) *g_PE_u_miningGate)->invertAxisFlag == 0)
            return ((MiningGame *) (((void *&) this->miningGame)))->up(-delta);
        return ((MiningGame *) (((void *&) this->miningGame)))->down(delta);
    }

    if (this->turretActive != 0) {
        float ft = (float) frameTime;
        if (this->lookYaw > g_PE_u_eps) {
            float ang = this->lookYaw - ft * delta;
            this->lookYaw = ang;
            ((AEGeometry *) this->turretGeometry)->rotate((float) (ang * g_PE_u_lookK1 * g_PE_u_lookK2), 0.0f, 0.0f);
        }
        float p = this->lookPitch;
        if (p > g_PE_u_eps2) {
            float half = ft * delta * 0.5f;
            float ang = half * g_PE_u_lookK1 * g_PE_u_lookK2;
            this->lookPitch = p - half;
            return (((AEGeometry *) this->dockCameraMid)->rotate((float) ang, 0.0f, 0.0f), 0.0f);
        }
        return p;
    }

    if (((void *&) this->rocketControlGun) != 0) {
        float v = (float) frameTime * g_PE_u_manK * ((Gun *) (intptr_t) this->rocketControlGun)->pitchRate;
        this->maneuverParam = v;
        return v;
    }

    if (this->autoPilot != 0)
        return delta;
    if (this->dockedFlag != 0 && this->dockingPointIndex != 1)
        return delta;

    this->pitchAccumDir = 1;
    float rate;
    if (this->hardCoreMode == 0) {
        rate = ((float &) this->handling);
    } else {
        float cur = (float) ((Ship *) (PE_status()->getShip()))->getCurrentLoad();
        float max = (float) ((Ship *) (PE_status()->getShip()))->getMaxLoad();
        rate = ((float &) this->handling) * (1.0f - cur / max) * g_PE_u_loadK + ((float &) this->handling) *
               g_PE_u_loadB;
    }

    float target = (float) aeabi_idiv_((int) (delta * g_PE_u_rateK * rate), 0x3f);
    this->pitchRamp = rate;
    ((float &) this->yawAccumD) = -delta;
    if (((float &) this->pitchAccumD) > target) {
        float v = ((float &) this->pitchAccumD) - PE_pitchRampDelta(this, rate, frameTime);
        if (v < target) v = target;
        ((float &) this->pitchAccumD) = v;
    }
    return target;
}

void Mat_mul(void *out, const void *a, const void *b);

void PlayerEgo::shoot(int weapon, int type) {
    int hi = weapon >> 31;
    if (this->turretActive != 0) {
        void *m1 = ((AEGeometry *) (this->geometry))->getMatrix();
        void *m2 = ((AEGeometry *) (this->turretGeometry))->getMatrix();
        float combined[0x30 / sizeof(float)];
        Mat_mul(combined, m1, m2);
        Matrix mat;
        for (int k = 0; k < 15; ++k) mat.m[k] = combined[k];
        mat.m[15] = 1.0f;
        long long pos = ((long long) hi << 32) | (unsigned int) weapon;
        static_cast<Player *>(this->player)->shoot(2, pos, hi < 0, mat);
        return;
    }

    if (((PlayerEgo *) (this))->isDead() != 0)
        return;

    Player *player = static_cast<Player *>(this->player);
    if (type == 1) {
        int idx = this->currentSecondaryWeaponIndex;
        long long pos = ((long long) hi << 32) | (unsigned int) idx;
        player->shoot(1, idx, pos, hi < 0);
    } else {
        long long pos = ((long long) hi << 32) | (unsigned int) weapon;
        player->shoot(type, weapon, pos, hi < 0);
    }
}

void PlayerEgo::stopMining() {
    void *mg = ((void *&) this->miningGame);
    if (mg != 0) {
        delete (MiningGame *) mg;
        ((void *&) this->miningGame) = 0;
    }
}



void PlayerEgo::setTurretMode(bool enable) {
    if (this->turretMode == 0 || ((void *&) this->miningGame) != 0 || this->autoTurretEquipped != 0) {
        if (((void *&) this->rocketControlGun) != 0) {
            PaintCanvas::gCanvas->CameraSetCurrent(
                (unsigned int) (((TargetFollowCamera *) (intptr_t) this->targetFollowCamera)->id));
            ((LevelScript *) (this->level))->resetCamera(((LevelScript *) this->level)->m_pLevel);
        }
        return;
    }

    this->turretActive = (unsigned char) enable;
    if (enable == 0) {
        ((PlayerEgo *) (this))->stopShooting(0);
        PaintCanvas::gCanvas->CameraSetCurrent((unsigned int) (((TargetFollowCamera *) (intptr_t) this->targetFollowCamera)->id));
        ((LevelScript *) (this->level))->resetCamera(((LevelScript *) this->level)->m_pLevel);
    } else {
        if (((void *&) this->rocketControlGun) != 0)
            return;
        if (this->dockCameraNode == 0) {
            PaintCanvas::gCanvas->CameraCreate(this->turretCamera);
            float farPlane = (PE_status()->inAlienOrbit() != 0) ? g_PE_tm_farAlien : g_PE_tm_farNormal;
            PaintCanvas::gCanvas->CameraSetPerspective(this->turretCamera, g_PE_camFov, g_PE_camNear, farPlane);

            void *node = (void *) new AEGeometry(PaintCanvas::gCanvas);
            this->dockCameraNode = node;
            ((AEGeometry *) node)->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
            ((AEGeometry *) (node))->translate(this->turretOffsetVec);

            void *mid = (void *) new AEGeometry(PaintCanvas::gCanvas);
            this->dockCameraMid = mid;
            ((AEGeometry *) (mid))->translate(*(Vector *) &((AEGeometry *) mid)->transform);
            ((AEGeometry *) (this->dockCameraNode))->
                    addChild((uint32_t)((AEGeometry *) this->dockCameraMid)->transform);

            void *leaf = (void *) new AEGeometry(PaintCanvas::gCanvas);
            this->dockCameraLeaf = leaf;
            ((AEGeometry *) (this->dockCameraNode))->addChild(
                (uint32_t)((AEGeometry *) this->dockCameraNode)->transform);

            if (((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0x23) != 0)
                ((AEGeometry *) (this->dockCameraNode))->rotate(this->turretOffsetVec);
        }
        ((AEGeometry *) (this->dockCameraLeaf))->setPosition(this->dockOffsetVec);
        void *leaf = this->dockCameraLeaf;
        ((AEGeometry *) leaf)->setMatrix(((AEGeometry *) (this->geometry))->getMatrix());
        PaintCanvas::gCanvas->CameraSetCurrent((unsigned int) (this->turretCamera));
        ((Player *) (this->player))->stopShooting(0);
    }

    if (this->field_0x30 != 0) {
        void *tf = PaintCanvas::gCanvas->TransformGetTransform(
            reinterpret_cast<unsigned int &>(PaintCanvas::gCanvas->initialized));
        ((AbyssEngine::Transform *) (tf))->SetVisible(enable != 0);
        int v = (enable != 0);
        if (enable == 0)
            ((FModSound *) (*g_PE_tm_hum))->stop(0);
        else
            ((FModSound *) (*g_PE_tm_hum))->play(0x8cf, (Vector *) 0, (Vector *) 0, (float) v);
    }
}

void PlayerEgo::rotate(float rx, float ry, float rz) {
    char local[60];
    this->rotateX = this->rotateX + rx;
    this->rotateY = this->rotateY + ry;
    this->rotateZ = this->rotateZ + rz;
    int ido = this->field_0x4->transform;
    void *t = *(void **) g_rotate_transform;
    void *m = TransformGetLocal(t, ido);
    MatrixSetRotation(local, m, this->rotateX, this->rotateY, this->rotateZ);
}


static inline float fmin_(float a, float b) { return a < b ? a : b; }

// IDA 0xa11b4: _ZN9PlayerEgo6strafeEib
void PlayerEgo::strafe(int /*dir*/, bool positive) {
    if (this->rocketControlGun != 0)
        return;

    float base;
    if (this->hardCoreMode != 0) {
        float cur = (float) ((Ship *) (PE_status()->getShip()))->getCurrentLoad();
        float max = (float) ((Ship *) (PE_status()->getShip()))->getMaxLoad();
        float rate = ((float &) this->handling);
        base = rate * (1.0f - cur / max) * g_PE_strafeLoadK + rate * g_PE_strafeLoadB;
    } else {
        base = ((float &) this->handling);
    }

    float sign = positive ? 1.0f : -1.0f;
    float accel = fmin_(base * 30.0f * g_PE_strafeAccelK, 2.0f);
    float target = fmin_(((float &) this->strafeNavPoint) * 1.5f, 1.0f);

    this->strafeAccel = ((float &) this->strafeNavPoint) * sign * accel;
    ((float &) this->strafeNavPoint) = target;
}

void PlayerEgo::dockToDockingPoint(KIPlayer *kip, Radar *radar) {
    (void) radar;
    if (((PlayerEgo *) (this))->isDead() != 0)
        return;

    if (this->dockedFlag == 0) {
        if (kip != 0) {
            this->dockedFlag = 1;
            ((void *&) this->asteroidTarget) = kip;
            this->dockScaling = 0x578;
            this->miningSettleTimer = 0;
            this->field_0x145 = 1;
            this->dockingPointIndex = 0;
        }
        return;
    }

    bool undock = (kip == 0);
    if (!undock) {
        uint8_t *kipBytes = reinterpret_cast<uint8_t *>(kip);
        Vector pos = ((PlayerEgo *) (this))->getPosition();
        SpacePoint *sp = (SpacePoint *) (unsigned long) this->spacePoint;
        void *nav = kip->getNearestNavigationPoint(pos, sp);
        if (nav == 0) {
            if (kipBytes[0x70] != 0)
                kipBytes[0x8c] = 1;
            undock = true;
        } else {
            if (kipBytes[0x70] != 0)
                kipBytes[0x8c] = 1;

            ((PlayerEgo *) (this))->setTurretMode(0);
            this->field_0x1a1 = 0;
            PaintCanvas::gCanvas->CameraSetCurrent(
                (unsigned int) (((TargetFollowCamera *) (intptr_t) this->targetFollowCamera)->id));
            ((LevelScript *) (this->levelScript))->resetCamera(this->levelScript->m_pLevel);
            PlayEngineSound_(this);
            this->dockingPointIndex = 3;
            ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setLookAtCam(false);
            ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->useTargetsUpVector(false);

            if (this->easeMatrix != 0) {
                ((AbyssEngine::EaseInOutMatrix *) this->easeMatrix)->~EaseInOutMatrix();
                ::operator delete(this->easeMatrix);
            }
            this->easeMatrix = 0;

            pos = ((PlayerEgo *) (this))->getPosition();
            void *nav2 = kip->getNearestNavigationPoint(pos, sp);
            void *from = ((AEGeometry *) (this->geometry))->getMatrix();
            this->easeMatrix = PE_dtdp_makeEase(from, nav2);
            ((PlayerEgo *) (this))->setExhaustVisible(true);
        }
    }

    if (undock) {
        PlayEngineSound_(this);
        this->dockedFlag = 0;
        this->field_0x1a1 = 0;
        this->field_0x145 = 0;
        ((void *&) this->asteroidTarget) = 0;
        int zero[3] = {0, 0, 0};
        Vec_assign(&this->dockApproachDist, zero);
        ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setActive(true);
        ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setLookAtCam(false);
        ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->useTargetsUpVector(false);
        ((Player *) (this->player))->resetGunDelay(0);
        this->dockingPointIndex = 0;
        ((PlayerEgo *) (this))->setExhaustVisible(true);
        if (this->spacePoint != 0) {
            ((SpacePoint *) ((void *) (unsigned long) this->spacePoint))->giveFree();
            this->spacePoint = 0;
        }
    }

    if (((void *&) this->hackingGame) != 0) {
        delete (HackingGame *) ((void *&) this->hackingGame);
        ((void *&) this->hackingGame) = 0;
        ((Hud *) (((void *&) this->hud)))->setHackingGameActive(false);
    }
}






void PlayerEgo::draw(bool allowHud) {
    if (((void *&) this->rocketControlGun) != 0)
        return;

    if (((void *&) this->hackingGame) != 0 && this->turretActive == 0) {
        ((HackingGame *) (void *) (intptr_t) this->hackingGame)->render2D();
        return;
    }

    if (((void *&) this->miningGame) != 0) {
        ((MiningGame *) (void *) (intptr_t) this->miningGame)->render2D();
        return;
    }

    bool full;
    if (this->computerControlled != 0 || ((PlayerEgo *) (this))->isDead() != 0 || this->freeze != 0
        || allowHud == 0 || ((char &) this->dockingState) != 0 || this->dockingToPlanet != 0 || this->dockedToStream !=
        0) {
        full = false;
    } else {
        full = true;
    }

    if (!full) {
        if (this->autoPilot != 0) {
            this->drawThrottle();
            return;
        }
        if (this->dockedFlag == 0)
            return;
        if ((unsigned) (this->dockingPointIndex - 1) < 3)
            return;
        this->drawThrottle();
        return;
    }

    PE_MatrixBuf m;
    for (unsigned i = 0; i < sizeof(m); i++) ((unsigned char *) &m)[i] = 0;
    m.m00 = 1.0f;
    m.m11 = 1.0f;
    m.m22 = 1.0f;

    bool aligned = (this->autoPilot != 0 && this->turretActive != 0);
    if (!aligned)
        Mat_assign(&m, ((AEGeometry *) (this->geometry))->getMatrix());

    void *canvas = (void *) PaintCanvas::gCanvas;
    PaintCanvas::gCanvas->SetColor((unsigned int) (0xffffffff));

    if (this->turretActive != 0
        && ((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0x23) != 0) {
        if (((Radar *) (this->field_0x14))->isPlasmaInRange() != 0) {
            float *p = g_PE_dr_posLock;
            ((PaintCanvas *) (long) (canvas))->DrawImage2D((unsigned int) (this->radarBlipImage3), (int) p[0],
                                                           (int) p[1], (unsigned char) (0x11), (unsigned char) (0x44));
        } else {
            float *p = g_PE_dr_posNoLock;
            ((PaintCanvas *) (long) (canvas))->DrawImage2D((unsigned int) (this->radarBlipImage2), (int) p[0],
                                                           (int) p[1], (unsigned char) (0x11), (unsigned char) (0x44));
        }
    } else {
        if ((char &) this->level->field_30 != 0) {
            float *p = g_PE_dr_posBlink;
            ((PaintCanvas *) (long) (canvas))->DrawImage2D((unsigned int) (this->radarBlipImage1), (int) p[0],
                                                           (int) p[1], (unsigned char) (0x11), (unsigned char) (0x44));
            int t = this->shakeAccum + this->shakeIntensity;
            this->shakeAccum = t;
            if (t >= 0xc9)
                (char &) this->level->field_30 = 0;
        } else {
            float *p = g_PE_dr_posNormal;
            ((PaintCanvas *) (long) (canvas))->DrawImage2D((unsigned int) (this->dockArrowImage), (int) p[0],
                                                           (int) p[1], (unsigned char) (0x11), (unsigned char) (0x44));
            this->shakeAccum = 0;
        }
    }

    ((PlayerEgo *) (this))->drawThrottle();
}

void PE_upd_subsystems(PlayerEgo *self, int dt, void *radar, void *hud,
                                  void *radio, void *script);

void PE_upd_post(PlayerEgo *self, int dt, void *radar, void *hud,
                            void *radio, int arg5);


// IDA 0x9ce38: _ZN9PlayerEgo6updateEiP5RadarP3HudP5RadioP11LevelScriptibi
void PlayerEgo::update(int dt, Radar *radar, Hud *hud, Radio *radio, LevelScript *script, int arg5, bool arg6,
                       int arg7) {
    (void) arg6;
    (void) arg7;
    (void) script;

    if (((void *&) this->hud) == 0)
        return;
    if (this->freeze != 0)
        return;

    float pos[3];
    ((PlayerEgo *) (this))->getPosition();
    ((int &) this->dockOffsetVec.x) = *(int *) &pos[0];
    ((int &) this->dockOffsetVec.y) = *(int *) &pos[1];
    this->boostDelay = *(int *) &pos[2];

    PE_upd_subsystems(this, dt, radar, hud, radio, script);

    PE_upd_boost(this, dt);

    if (((char &) this->dockingState) != 0) {
        this->field_0x145 = 1;
        if (((void *&) this->asteroidTarget) == 0 || ((PlayerEgo *) (this))->isDead() != 0) {
            ((PlayerEgo *) (this))->dockToAsteroid(nullptr, nullptr);
            PE_upd_post(this, dt, radar, hud, radio, arg5);
            return;
        }
        ((PlayerEgo *) (this))->approachAsteroid(hud, (int) (intptr_t) hud, radar);
    }

    if (this->dockedFlag != 0 && ((Player *) (this->player))->getHitpoints() > 0) {
        this->field_0x145 = 1;
        if (((void *&) this->asteroidTarget) == 0 || ((PlayerEgo *) (this))->isDead() != 0) {
            ((PlayerEgo *) (this))->dockToDockingPoint(nullptr, nullptr);
            PE_upd_post(this, dt, radar, hud, radio, arg5);
            return;
        }
        if (((PlayerEgo *) (this))->approachDockingPoint(hud, 0, radar) != 0)
            PE_upd_docksFinishDelivery(this, radio);
    }

    if (this->turretActive != 0 || this->field_0x1a1 != 0)
        ((PlayerEgo *) (this))->handleTurretView(dt);

    bool autopilot = (this->autoPilot != 0 && this->autoPilotTarget != 0);
    if (!autopilot) {
        if (this->field_0x145 == 0) {
            if (this->turretActive == 0 && this->field_0x1a1 == 0) {
                ((PlayerEgo *) (this))->roll(this->shakeIntensity);
                if (((PlayerEgo *) (this))->updateManeuver() == 0)
                    ((PlayerEgo *) (this))->handleShip(dt);
            } else {
                ((PlayerEgo *) (this))->handleTurretView(dt);
            }
        }
    } else if (this->field_0x145 == 0 && ((PlayerEgo *) (this))->updateManeuver() == 0) {
        void *wp = (void *) (intptr_t) this->autoPilotTarget;
        if (this->goingToWaypointFlag != 0 && wp != 0 && ((Waypoint *) wp)->route != 0) {
            wp = ((Route *) (this->autoPilotTarget))->getWaypoint();
            ((void *&) this->autoPilotTarget) = wp;
        }
        if (wp == 0 || this->dockedToStream != 0) {
            this->setAutoPilot(nullptr);
        } else {
            AbyssEngine::AEMath::Vector wpVec = ((KIPlayer *) wp)->getPosition();
            float wpPos[3] = {wpVec.x, wpVec.y, wpVec.z};
            ((int &) this->waypointX) = *(int *) &wpPos[0];
            ((int &) this->waypointY) = *(int *) &wpPos[1];
            ((int &) this->gunBaseGeo) = *(int *) &wpPos[2];

            float me[3];
            ((PlayerEgo *) (this))->getPosition();
            float dx = me[0] - wpPos[0], dy = me[1] - wpPos[1], dz = me[2] - wpPos[2];
            if ((int) (dx * dx + dy * dy + dz * dz) < 20000)
                this->aboutToReachAutoTargetFlag = 1;

            float speed = 4.0f;
            float h = ((Ship *) (PE_status()->getShip()))->getHandling();
            if (h + g_PE_upd_handlingBias < 4.0f)
                speed = ((Ship *) (PE_status()->getShip()))->getHandling() + g_PE_upd_handlingBias;
            ((PlayerEgo *) (this))->moveToPosition(Vector{
                                                       this->waypointX, this->waypointY, ((float &) this->gunBaseGeo)
                                                   }, true, speed);
            if (this->turretActive != 0)
                ((PlayerEgo *) (this))->handleTurretView(dt);
        }
    }

    if (this->dockedFlag != 0 && ((uint32_t &) this->dockingPointIndex) != 1
        && this->turretActive == 0 && (((uint32_t &) this->dockingPointIndex) | 1) != 3)
        ((PlayerEgo *) (this))->updateManeuver();

    if (this->autoTurretEquipped != 0 && this->autoTurretEnabled != 0) {
        if (((PlayerEgo *) (this))->isDead() == 0) {
            ((PlayerEgo *) (this))->handleAutoTurret(dt);
        } else {
            this->autoTurretEnabled = 0;
            ((Player *) (this->player))->stopShooting(0);
        }
    }

    PE_upd_post(this, dt, radar, hud, radio, arg5);
}

void PlayerEgo::setTurretPosition(Vector v) {
    this->turretOffsetVec = v;
}



void PlayerEgo::revive() {
    ParticleSystemManager *psm = this->level->field_74;
    bool en = ((char &) this->currentSystem) != 0;
    psm->enableSystemEmit(this->currentSystem, en);
    psm->enableSystemRender(this->currentSystem, en);

    if (this->explosion != 0)
        delete (Explosion *) (intptr_t) this->explosion;
    this->explosion = 0;

    ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setActive(true);
    ((Player *) (this->player))->setActive(true);
    int v = 0;

    void *snd = *g_PE_reviveSound;
    ((FModSound *) (*(void **) snd))->play((int) (intptr_t) * (void **) snd, (Vector *) 0, (Vector *) 0, (float) v);
    ((FModSound *) (snd))->play((int) (intptr_t)((void *&) this->field_0x1c), (Vector *) 0, (Vector *) 0, (float) v);

    ((PlayerEgo *) (this))->setExhaustVisible(true);

    void *player = this->player;
    ((Player *) (player))->setHitpoints(((Player *) (player))->getMaxHitpoints());
    player = this->player;
    ((Player *) (player))->setArmorHP(((Player *) (player))->getMaxArmorHP());

    if (this->geometry != nullptr) {
        Vector pos = this->getPosition();
        ((AEGeometry *) (this->geometry))->setPosition(pos);
    }

    float fwd[3] = {0.0f, 0.0f, 1.0f};
    float fwdUp[3] = {0.0f, 1.0f, 0.0f};
    ((AEGeometry *) (this->geometry))->setDirection(*(Vector *) fwd, *(Vector *) fwdUp);
    this->explosionTimer = 0;
}




// IDA 0xa1f04: _ZN9PlayerEgo12drawThrottleEv
void PlayerEgo::drawThrottle() {
    if (this->throttleStarted == 0)
        return;

    int t = this->throttle;
    if (t > 500)
        t = 2000 - t;
    float frac = (float) t / g_PE_t_timerDiv;
    if (frac > 1.0f)
        frac = 1.0f;

    void *canvas = (void *) PaintCanvas::gCanvas;
    PaintCanvas::gCanvas->SetColor((unsigned int) (0xffffff00 | (unsigned int) (int) (frac * 255.0f) - 0x100));

    int img = this->hpGaugeImage;
    int w = ((PaintCanvas *) (long) (canvas))->GetImage2DWidth((unsigned int) (img));
    int h = ((PaintCanvas *) (long) (canvas))->GetImage2DHeight((unsigned int) (img));

    float thrust = this->thrust;
    int fillH = (int) (thrust * (float) h);
    float *anchor = g_PE_t_anchor;

    ((PaintCanvas *) (long) (canvas))->DrawRegion2D((unsigned int) (img), 0, h - fillH, w, fillH,
                                                    (float) ((int) ((anchor[1] + (float) h) - (float) fillH)), 0, 0, 0,
                                                    (int) (anchor[0] - (float) (w / 2)));

    unsigned char pct[12];
    ((String *) (pct))->Set((long long) (int) (thrust * g_PE_t_pctScale));

    int th = ((PaintCanvas *) (long) (canvas))->GetImage2DHeight((unsigned int) (img));
    String *pctStr = *g_PE_t_pctStr;
    int tw = ((PaintCanvas *) (long) (canvas))->GetTextWidth((unsigned int) (long) (canvas), *pctStr);
    ((PaintCanvas *) (long) (canvas))->DrawString((unsigned int) (long) (canvas), *pctStr, (int) (long) (pct),
                                                  (int) ((anchor[0] - (float) (tw / 2)) - 1.0f),
                                                  (bool) (int) (anchor[1] + (float) th / g_PE_t_textDiv));
    PaintCanvas::gCanvas->SetColor((unsigned int) (0xffffffff));

    { String *_s = ((String *) (pct)); if (_s->data) delete[] _s->data; _s->data = nullptr; _s->length = 0; }
}

void PlayerEgo::setAutoPilot(KIPlayer *kip) {
    this->goingToWaypointFlag = 0;
    int v = (int) (intptr_t) kip;
    this->autoPilotTarget = v;
    unsigned char old = this->autoPilot;
    this->autoPilot = (v != 0) ? 1 : 0;
    if (v == 0) {
        ((KIPlayer *) this->field_0x14)->autoPilotState = 0;
        if (old != 0) {
            this->field_0x2a8 = 0;
            this->field_0x2a4 = 0;
        }
        return;
    }
    if (kip->field_0x72 != 0) this->goingToWaypointFlag = 1;
    void *eng = ApplicationManager::gAppManager->GetEngine();
    ((AbyssEngine::Engine *) eng)->autoPilotEngaged = 0;
    ((int &) this->thrust) = 0x3f800000;
}

// IDA 0x9c1f8: _ZN9PlayerEgo14updateManeuverEv
int PlayerEgo::updateManeuver() {
    unsigned int type = ((uint32_t &) this->maneuverType);

    if ((type - 1) < 2) {
        ((int &) this->navPoint) = ((int &) this->explosion) + ((int &) this->navPoint);
        PE_um_dodgeStep(this);
        if (((int &) this->navPoint) > 0x4af) {
            void *level = this->levelScript;
            this->maneuverType = 0;
            ((LevelScript *) (level))->resetCamera(((LevelScript *) level)->m_pLevel);
        }
        return 1;
    }

    if (type != 3)
        return 0;

    ((int &) this->navPoint) = ((int &) this->explosion) + ((int &) this->navPoint);

    float target[3];
    PE_um_strafeTarget(this, target);
    if (((int &) this->navPoint) > 900) {
        target[0] = this->strafeTargetVec.x;
        target[1] = this->strafeTargetVec.y;
        target[2] = this->strafeTargetVec.z;
    }
    ((PlayerEgo *) (this))->moveToPosition(Vector{target[0], target[1], target[2]}, true, 0.0f);

    PE_um_strafeGlide(this);
    ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setLookAtCam(false);
    ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->useTargetsUpVector(false);

    if (((int &) this->navPoint) > 2999) {
        this->maneuverType = 0;
        ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->setLookAtCam(false);
        ((TargetFollowCamera *) (((void *&) this->targetFollowCamera)))->useTargetsUpVector(false);
    }
    return 1;
}

int PlayerEgo::getHullDamageRate() {
    return ((Player *) this->player)->getArmorDamageRate();
}

void Mat_mul(void *out, const void *a, const void *b);

void Mat_mulEq(void *acc, const void *b);

void PlayerEgo::handleTurretView(int dt) {
    bool move = true;
    if (((void *&) this->autoPilotTarget) != 0 && this->dockedToStream == 0)
        move = true;
    if (this->dockedFlag != 0 && this->dockingPointIndex == 1)
        move = false;
    if (((void *&) this->autoPilotTarget) == 0)
        move = true;
    if (move) {
        float d = (float) dt;
        ((AEGeometry *) (this->geometry))->moveForward(this->thrust * d * ((float &) this->speed));
    }

    unsigned char look[0x30];
    Mat_mul(look, ((AEGeometry *) (this->geometry))->getMatrix(), ((AEGeometry *) (this->dockCameraNode))->getMatrix());
    Mat_mulEq(look, ((AEGeometry *) (this->dockCameraMid))->getMatrix());

    if (this->hitShake != 0 || this->boostingFlag != 0) {
        unsigned char eye[12], up[12], dir[12], rel[12];
        Mat_getPosition(eye, look);
        Mat_getUp(up, look);
        Mat_getDir(dir, look);
        Vec_sub(rel, dir, eye);

        if (this->hitShake != 0) {
            int t = this->hitShakeTimer + dt;
            this->hitShakeTimer = t;
            if (t > 1000)
                this->hitShake = 0;
        }
        PE_htv_applyShake(this, dt, eye, rel);

        unsigned char lookAt[0x30];
        Mat_getLookAt(lookAt, eye, rel, up);
        Mat_assign(look, lookAt);
    }

    unsigned int cam = (unsigned int) (unsigned long) PaintCanvas::gCanvas;
    PaintCanvas::gCanvas->CameraSetLocal((unsigned int) (cam),
                            *(const AbyssEngine::AEMath::Matrix *) &this->turretCamera);

    this->pitchAccumDir = 0;
    this->yawAccumDir = 0;
    ((PlayerEgo *) (this))->roll(this->shakeIntensity);

    unsigned int hull = (unsigned int) (long) PaintCanvas::gCanvas->TransformGetLocal((unsigned int) (this->geometry->transform));
    unsigned int ret = (unsigned int) (long) PaintCanvas::gCanvas->TransformGetLocal((unsigned int) (this->field_0x4->transform));
    unsigned char tmp[0x30];
    Mat_mul(tmp, (void *) (unsigned long) hull, (void *) (unsigned long) ret);
    Mat_assign(((Player *) this->player)->transform, tmp);
}






void PlayerEgo::approachAsteroid(Hud *hud, int hud2, Radar *radar) {
    (void) hud;
    if (((KIPlayer *) (this))->isDying() != 0)
        return;

    if (this->dockingPointIndex == 1) {
        float settle = ((float &) this->miningSettleTimer);
        if (settle > g_PE_aa_settleEps) {
            ((float &) this->miningSettleTimer) = settle + (float) (-(this->shakeIntensity) >> 1);
            return;
        }

        if (((void *&) this->miningGame) == 0) {
            this->field_0x2f5 = 0;
            this->lostMiningGameFlag = 0;
            ((MiningHostObject *) *g_PE_aa_levelHolder)->miningActiveFlag = 0;
            MiningGame *mg = new MiningGame(((PlayerAsteroid *) (((void *&) this->asteroidTarget)))->getQuality(),
                                            ((PlayerAsteroid *) ((void *&) this->asteroidTarget))->asteroidIndex,
                                            (Hud *) (void *) (unsigned long) hud2);
            ((void *&) this->miningGame) = mg;
            ((PE_AsteroidMineTarget *) (intptr_t) this->asteroidTarget)->miningFlag = 0;

            int snd = *g_PE_aa_mineSound;
            ((FModSound *) (snd))->play(1, (Vector *) 0, (Vector *) 0, 0);
            ((FModSound *) ((void *) (unsigned long) snd))->pause(0);
            return;
        }

        int running = ((MiningGame *) (((void *&) this->miningGame)))->update(this->shakeIntensity);
        if (running == 0) {
            if (((MiningGame *) (((void *&) this->miningGame)))->gameLost() == 0
                && ((MiningGame *) (((void *&) this->miningGame)))->getOreAmount() > 0) {
                ((PlayerEgo *) (this))->stopMining();
            } else if (((MiningGame *) (((void *&) this->miningGame)))->gameLost() != 0) {
                this->lostMiningGameFlag = 1;
                ((MiningHostObject *) *g_PE_aa_winHolder1)->miningResultSlot = 0;
                ((PlayerEgo *) (this))->stopMining();
                ((Hud *) ((void *) (unsigned long) hud2))->hudEvent(8, this, 0);
            }
        } else if (((KIPlayer *) (this))->isDying() != 0 || ((KIPlayer *) (this))->isDead() != 0) {
            ((MiningHostObject *) *g_PE_aa_winHolder2)->miningResultSlot = 0;
            ((PlayerEgo *) (this))->stopMining();
            ((Hud *) ((void *) (unsigned long) hud2))->hudEvent(8, this, 0);
        }
        return;
    }

    if (this->dockingPointIndex == 0) {
        this->dockingPointIndex = PE_aa_approachStep(this, hud2, radar);
    }
}



// IDA 0x9bdf0: _ZN9PlayerEgo10handleShipEi
void PlayerEgo::handleShip(int dt) {
    FModSound *snd = *g_PE_hs_sound;

    snd->setParamValue((FMOD::Event *) (long) ((Player *) (this->player))->GetEngineEvent(), 0,
                       ((float &) this->yawAccumD));
    snd->setParamValue((FMOD::Event *) (long) ((Player *) (this->player))->GetEngineEvent(), 1,
                       ((float &) this->rollAccum) * g_PE_hs_throttleBias + 0.5f);

    unsigned int tf = reinterpret_cast<unsigned int &>(PaintCanvas::gCanvas->initialized);

    PE_handleShip_orient(this, dt, tf);

    ((int &) this->pitchRamp) = 0;
    ((int &) this->yawRamp) = 0;
    this->pitchAccumDir = 0;
    this->yawAccumDir = 0;
    ((int &) this->yawAccumD) = 0;
    ((int &) this->rollAccum) = 0;

    unsigned int hull = (unsigned int) (long) PaintCanvas::gCanvas->TransformGetLocal((unsigned int) (this->geometry->transform));
    unsigned int ret = (unsigned int) (long) PaintCanvas::gCanvas->TransformGetLocal((unsigned int) (this->field_0x4->transform));
    unsigned char tmp[0x30];
    Mat_mul(tmp, (void *) (unsigned long) hull, (void *) (unsigned long) ret);
    Mat_assign(((Player *) this->player)->transform, tmp);
}

// IDA 0x9ae34: _ZN9PlayerEgo9stopBoostEv
void PlayerEgo::stopBoost() {
    this->boostingFlag = 0;
    this->speed = 0x40000000;
    Globals::sound->stop(0x27);
    Globals::sound->stop(0x26);
    Globals::sound->stop(0x29);
    Globals::sound->stop(0x28);
    Globals::sound->stop(0x44e);
}


void PlayerEgo::setShip(int race, int group) {
    AEGeometry *grp = Globals::gGlobals->getShipGroup(race, group, true);
    this->field_0x4 = grp;

    void *canvas = (void *) PaintCanvas::gCanvas;
    AbyssEngine::Mesh *mesh = ((PaintCanvas *) (long) (canvas))->MeshGetPointer(
        (unsigned int) (((AEGeometry *) grp)->meshId));
    this->field_0x394 = ((PE_MaterialBlock *) mesh->materialBlock)->materialList;

    void *hull = (void *) new AEGeometry(PaintCanvas::gCanvas);
    this->geometry = (AEGeometry *) hull;
    ((AEGeometry *) (hull))->addChild((uint32_t) this->field_0x4->transform);

    if (((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0xd) != 0) {
        void *it = (void *) ((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0xd);
        int idx = ((Item *) (it))->getIndex();
        int kind = (idx < 0x48) ? idx - 0x44 : 3;
        void *tb = TractorBeam_new(this->geometry, kind);
        this->tractorBeam = tb;
        Globals::gGlobals->addSoundResourceToList(0x0);
        Globals::gGlobals->addSoundResourceToList(0x4);
    }

    for (unsigned i = 0; i < 2; i++) {
        int sort = (i == 0) ? 0x25 : 0x29;
        void *it = (void *) ((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(sort);
        if (it != 0) {
            if (this->repairBeams == 0)
                this->repairBeams = new Array<RepairBeam *>();
            RepairBeam *rb = new RepairBeam(((Item *) (it))->getIndex(), ((Item *) (it))->getSort());
            int idx = ((Item *) (it))->getIndex();
            if (idx == 0xde)
                Globals::gGlobals->addSoundResourceToList(0x8db);
            else if (((Item *) (it))->getIndex() == 0xdf)
                Globals::gGlobals->addSoundResourceToList(0x8dc);
            ArrayAdd(rb, *(this->repairBeams));
        }
    }

    if (((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0x1b) != 0
        && ((Ship *) (PE_status()->getShip()))->hasEmergencySystem() != 0) {
        void *geo = (void *) new AEGeometry((uint16_t) 0x3826, PaintCanvas::gCanvas, false);
        ((void *&) this->field_0xac) = geo;
        ((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0x1b);
        this->emergencyVal1 = ((Item *) ((void *) ((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0x29)))->
                getAttribute(0);
        void *tf = PaintCanvas::gCanvas->TransformGetTransform((unsigned int) (this->field_0x4->transform));
        Vec_assign(&this->emergencyVec, &((AbyssEngine::Transform *) tf)->boundingCenter);
        tf = PaintCanvas::gCanvas->TransformGetTransform((unsigned int) (this->field_0x4->transform));
        this->emergencyVal2 = ((AbyssEngine::Transform *) tf)->boundingRadius / g_PE_ss_emDiv + g_PE_ss_emBias;
    }

    if (PE_status()->inSupernovaSystem() != 0 || PE_status()->inSupernovaOrbit() != 0) {
        void *tf = PaintCanvas::gCanvas->TransformGetTransform((unsigned int) (this->field_0x4->transform));
        ((float &) this->gunExtraGeo) = ((AbyssEngine::Transform *) tf)->boundingRadius * 1.75f;
    }

    if (this->cloak != 0) {
        AbyssEngine::Mesh *mesh = PaintCanvas::gCanvas->MeshGetPointer(
            (unsigned int) this->field_0x4->meshId);
        if (mesh != 0)
            Vec_assign(&this->cloakMaterial1, &mesh->localOffset);
    }
}

void PlayerEgo::addGun(Array<Gun *> *arr, int x) {
    static_cast<Player *>(this->player)->addGun(arr, x);
    ((Player *) this->player)->resetGunDelay(0);
}

void PlayerEgo::render(bool allowHud) {
    Level *level = this->level;

    if (((PlayerEgo *) (this))->isDead() == 0) {
        if (this->field_0x309 == 0)
            return;

        if (this->explosion != 0) {
            ((Explosion *) (this->explosion))->render();
            if (this->explosionTimer <= 0xbb7)
                ((AEGeometry *) (this->geometry))->render();
        }
        if (this->explosion2 != 0)
            ((Explosion *) (this->explosion2))->render();

        ((AEGeometry *) (this->geometry))->render();

        if (((void *&) this->field_0xac) != 0 && this->emergencySystemTimer >= 1)
            ((AEGeometry *) (this->geometry))->render();

        if (((char &) this->gunMuzzleRoot) != 0)
            ((AEGeometry *) (this->gunYawGeo))->render();

        if (this->turretMode != 0) {
            if (this->field_0x30 != 0)
                ((AEGeometry *) (this->field_0x30))->setVisible(this->turretActive != 0);
            ((AEGeometry *) (this->field_0x2c))->render();
        }

        if (this->tractorBeam != 0)
            ((TractorBeam *) (this->tractorBeam))->render();

        Array<RepairBeam *> *beams = this->repairBeams;
        if (beams != 0) {
            for (unsigned int i = 0; i < beams->size(); i++) {
                (*beams)[i]->render();
            }
        }

        int flag = 1;
        if (this->freeze == 0 && allowHud != 0)
            flag = (this->dockingPointIndex - 1) != 0 ? 1 : 0;
        else
            flag = 1;
        level->enableMovingStars(flag != 0);
    } else {
        if (this->explosion != 0) {
            ((Explosion *) (this->explosion))->render();
            if (this->explosionTimer < 3000)
                ((AEGeometry *) (this->geometry))->render();
        }
        if (((void *&) this->field_0xac) != 0)
            ((Explosion *) (this->explosion))->render();
        level->enableMovingStars(true);
    }
}



void PlayerEgo::toggleCloaking() {
    if (this->chargingCloak == 0) {
        if (this->cloaked != 0 || this->cloakRechargeTimer > 0)
            return;
        int need = ((Item *) (this->cloak))->getAttribute(0);
        void *cargo = ((Ship *) (PE_status()->getShip()))->getCargo(0x7a);
        int have = (cargo == 0) ? 0 : ((Item *) (cargo))->getAmount();
        if (need <= have) {
            ((Ship *) (PE_status()->getShip()))->removeCargo(0x7a);
            this->chargingCloak = 1;
            ((Hud *) (((void *&) this->hud)))->hudEvent(0x1e, this, 0);
            ((Hud *) (((void *&) this->hud)))->hudEvent(0x1c, this, 0);
        }
        return;
    }

    if (this->cloakDischargeMax > this->cloakCharge)
        return;

    ((FModSound *) (*g_PE_tc_sound))->play(0x1e, (Vector *) 0, (Vector *) 0, 0);
    void *canvas = (void *) PaintCanvas::gCanvas;
    ((Player *) this->player)->field_5e = 1;
    this->cloakCharge = 0;
    this->cloaked = 1;

    ((PaintCanvas *) (long) (canvas))->MaterialGetMaterial((unsigned int) (this->cloakMaterial1));

    ((AbyssEngine::Material *) ((PaintCanvas *) (long) (canvas))->MaterialGetMaterial(
        (unsigned int) (this->cloakMaterial1)))->materialMode = 0xe;
    ((PaintCanvas *) (long) (canvas))->MeshChangeMaterial((unsigned int) (this->field_0x4->meshId),
                                                          (unsigned int) (this->cloakMaterial1));
    ((PaintCanvas *) (long) (canvas))->MeshChangeShaderAnimValue(
        ((PaintCanvas *) (long) (canvas))->MeshGetPointer((unsigned int) (this->field_0x4->meshId)), 0.0f,
        (unsigned int) (0));
    ((PaintCanvas *) (long) (canvas))->MeshChangeShaderAnimValue(
        ((PaintCanvas *) (long) (canvas))->MeshGetPointer((unsigned int) (this->field_0x4->meshId)), 0.0f,
        (unsigned int) (0));

    if (this->turretMode != 0) {
        ((AbyssEngine::Material *) ((PaintCanvas *) (long) (canvas))->MaterialGetMaterial(
            (unsigned int) (this->cloakMaterial2)))->materialMode = 0xe;
        ((AbyssEngine::Material *) ((PaintCanvas *) (long) (canvas))->MaterialGetMaterial(
            (unsigned int) (this->cloakMaterial3)))->materialMode = 0xe;
        ((PaintCanvas *) (long) (canvas))->MeshChangeMaterial(
            (unsigned int) (((AEGeometry *) this->rollGeometry)->meshId), (unsigned int) (this->cloakMaterial2));
        ((PaintCanvas *) (long) (canvas))->MeshChangeMaterial(
            (unsigned int) (((AEGeometry *) this->turretGeometry)->meshId), (unsigned int) (this->cloakMaterial3));

        AbyssEngine::Mesh *m;
        m = ((PaintCanvas *) (long) (canvas))->MeshGetPointer(
            (unsigned int) (((AEGeometry *) this->rollGeometry)->meshId));
        ((PaintCanvas *) (long) (canvas))->MeshChangeShaderAnimValue(m, 0.0f, (unsigned int) (1));
        m = ((PaintCanvas *) (long) (canvas))->MeshGetPointer(
            (unsigned int) (((AEGeometry *) this->rollGeometry)->meshId));
        ((PaintCanvas *) (long) (canvas))->MeshChangeShaderAnimValue(m, 0.0f, (unsigned int) (2));
        m = ((PaintCanvas *) (long) (canvas))->MeshGetPointer(
            (unsigned int) (((AEGeometry *) this->turretGeometry)->meshId));
        ((PaintCanvas *) (long) (canvas))->MeshChangeShaderAnimValue(m, 0.0f, (unsigned int) (1));
        m = ((PaintCanvas *) (long) (canvas))->MeshGetPointer(
            (unsigned int) (((AEGeometry *) this->turretGeometry)->meshId));
        ((PaintCanvas *) (long) (canvas))->MeshChangeShaderAnimValue(m, 0.0f, (unsigned int) (2));

        if (this->turretMode != 0) {
            unsigned short mat = 0x4e8e;
            void *it = (void *) ((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(8);
            if (it != 0 && ((Item *) (it))->getIndex() == 0xe0)
                mat = 0x5e17;
            it = (void *) ((Ship *) (PE_status()->getShip()))->getFirstEquipmentOfSort(0x23);
            if (it != 0) {
                int idx = ((Item *) (it))->getIndex();
                mat = 0x716d;
                if (idx == 0xc7) mat = 0x7167;
                if (idx == 0xc6) mat = 0x7161;
            }
            unsigned int out;
            ((PaintCanvas *) (long) (canvas))->MaterialCreate((unsigned short) (mat), out);
            ((PaintCanvas *) (long) (canvas))->MeshChangeResourceMaterial(
                (unsigned int) (((AEGeometry *) this->rollGeometry)->meshId), (unsigned int) (mat));
            ((PaintCanvas *) (long) (canvas))->MeshChangeResourceMaterial(
                (unsigned int) (((AEGeometry *) this->turretGeometry)->meshId), (unsigned int) (mat));
        }
    }
}

// Static data members present in the original binary (defined for symbol parity).
AbyssEngine::AEMath::Vector PlayerEgo::crosshairPos;
AbyssEngine::AEMath::Vector PlayerEgo::crosshairShootPos;
AbyssEngine::AEMath::Vector PlayerEgo::vec_up;
