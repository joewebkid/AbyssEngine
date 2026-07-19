#include "game/ship/Player.h"
#include "game/ship/Ship.h"
#include "game/mission/Mission.h"
#include "engine/audio/FModSound.h"
#include "game/mission/Item.h"
#include "game/world/Level.h"
#include "game/mission/Achievements.h"
#include "game/ship/KIPlayer.h"
#include "engine/core/GameText.h"
#include "game/ui/Hud.h"
#include "game/world/SolarSystem.h"
#include "game/world/Standing.h"
#include "game/core/Globals.h"
#include "game/weapons/Gun.h"
#include "game/mission/Status.h"
#include "engine/render/Engine.h"
#include "engine/core/ApplicationManager.h"
#include <new>

namespace AbyssEngine {
    namespace AEMath {
        Vector operator-(const Vector &, const Vector &);

        Vector operator*(float, const Vector &);

        float VectorLength(const Vector &);
    }
}

static int *g_cws_items = nullptr;
static int *g_cws_sound = nullptr;
static int *g_cws_sound2 = nullptr;
static int *g_cws_sound3 = nullptr;
static int **g_damage_text = nullptr;
static unsigned int g_shoot_mask = 0;
static void **g_update_sound = nullptr;
static float **g_update_speed = nullptr;

// Wingman/slave link record referenced by KIPlayer::field_0x10: a small
// linkage struct whose +4 slot holds the linked Player.
struct SlaveLink {
    int field_0x0;
    Player *player;
};
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(SlaveLink, player) == 4, "SlaveLink.player must be at +4");
#endif

// Overlay view of the global audio-state object (the engine / application
// manager) whose byte at +0xf gates positional 3D sound updates.
struct AudioStateView {
    char pad_0[0xf];
    char soundEnabledFlag;  // +0xf
};
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(AudioStateView, soundEnabledFlag) == 0xf,
              "AudioStateView.soundEnabledFlag must be at +0xf");
#endif

void MatrixGetPosition(void *out, float *matrix);

static int gStopSoundIds[256];
static void *gFModSound = nullptr;
static void *gFModSoundAlt = nullptr;
static void **gFModSoundPtr = nullptr;

void Gun_setEnemies(void *gun);

static int gShootSoundsByType[256];
static int gShootSoundsByIndex[256];
static void *gAppManagerA = nullptr;
static void *gAppManagerB = nullptr;
static void *gAppManagerC = nullptr;

void FloatVectorMax(void *out, float a, float b, int c, int d);

void Player::pitchAllPrimaryGuns(float pitch) {
    if (this->guns != 0) {
        Array<Gun *> *prim = this->guns->data()[0];
        if (prim != 0) {
            int n = prim->size();
            for (int i = 0; n != i; i++) {
                prim->data()[i]->field_0xb0 = pitch;
            }
        }
    }
}

unsigned char Player::isAlwaysEnemy() {
    return this->alwaysEnemy;
}

void Player::setKIPlayer(KIPlayer *value) {
    this->kiPlayer = value;
}

void Player::damageHull(int damage) {
    if (!this->vulnerable) {
        return;
    }
    if (!this->active) {
        return;
    }
    int armor = this->armorHP;
    if (armor <= 0) {
        int h = this->hitpoints - damage;
        h &= ~(h >> 31);
        this->hitpoints = h;
    } else {
        armor -= damage;
        this->armorHP = armor;
    }
    if (armor <= -1) {
        this->armorHP = 0;
    }
    this->damaged = 1;
    this->updateDamageRate();
}

int Player::getShieldDamageRate() {
    return this->shieldDamageRate;
}

int Player::replaceGuns(int a, int b, int c, int d, int e, bool f) {
    return a;
}

int Player::getShieldHP() {
    return (int) this->shieldHP;
}

void Player::removeAllGuns() {
    if (this->guns != 0) {
        ArrayReleaseClasses(*this->guns); ArrayRemoveAll(*(this->guns));
        delete this->guns;
    }
    this->guns = 0;
}

int Player::getArmorDamageRate() {
    return this->armorDamageRate;
}

int Player::getArmorHP() {
    return this->armorHP;
}

void Player::setRadius(int value) {
    this->radius = value;
}

void Player::resetDamageDoneByPlayer() {
    this->field_dc = 0;
    this->damageDoneByPlayer = 0;
    this->turnedEnemyFlag = 0;
}

KIPlayer *Player::getKIPlayer() {
    return this->kiPlayer;
}

bool Player::isDead() {
    return this->hitpoints < 1;
}

int Player::getRadius() {
    return this->radius;
}

int Player::getEmpDamageRate() {
    return this->empDamageRate;
}

unsigned char Player::isAlwaysFriend() {
    return this->alwaysFriend;
}

int Player::getHitpoints() {
    return this->hitpoints;
}

void Player::damageShield(int damage) {
    if (!this->vulnerable) {
        return;
    }
    if (!this->active) {
        return;
    }
    float s = this->shieldHP;
    if (s <= 0.0f) {
        int h = this->hitpoints - damage;
        h &= ~(h >> 31);
        this->hitpoints = h;
    } else {
        s = s - (float) damage;
        this->shieldHP = s;
    }
    if (s < 0.0f) {
        this->shieldHP = 0;
    }
    this->damaged = 1;
    this->updateDamageRate();
}

void Player::regenerateArmor() {
    int v = this->armorHP + 2;
    if (v > this->maxArmorHP) {
        v = this->maxArmorHP;
    }
    this->armorHP = v;
    this->updateDamageRate();
}

void Player::damageShip(int damage) {
    int v = this->hitpoints - damage;
    v &= ~(v >> 31);
    this->hitpoints = v;
}

int Player::getMaxHitpoints() {
    return this->maxHitpoints;
}

int Player::getGammaHP() {
    return (int) this->gammaHP;
}

float Player::getBombForce() {
    return this->bombForce;
}

int Player::getMaxArmorHP() {
    return this->maxArmorHP;
}

Player *Player::getEnemy(int index) {
    return this->enemies->data()[index];
}

void Player::turnEnemy() {
    this->turnedEnemyFlag = 1;
}

void Player::setEmpData(int points, int data) {
    this->empPoints = points;
    if (this->maxEmpPoints < points) {
        this->maxEmpPoints = points;
    }
    ((Player *) (this))->updateDamageRate();
    this->empData = data;
}

void Player::setVulnerable(bool value) {
    this->vulnerable = value;
}

void Player::setActive(bool value) {
    this->active = value;
}

bool Player::isAsteroid() {
    KIPlayer *ki = this->kiPlayer;
    bool result = false;
    if (ki != 0) {
        result = ki->stealFlag != 0;
    }
    return result;
}

void Player::updateDamageRate() {
    float maxHp = (float) this->maxHitpoints;
    float hp = (float) this->hitpoints;
    float maxArmor = (float) this->maxArmorHP;
    float armor = (float) this->armorHP;
    float maxEmp = (float) this->maxEmpPoints;
    float maxShield = (float) this->maxShieldHP;
    float emp = (float) this->empPoints;

    float shieldRate = (this->shieldHP / maxShield) * 100.0f;
    float armorRate = (armor / maxArmor) * 100.0f;
    float empRate = (emp / maxEmp) * 100.0f;

    this->damageRate = (int) ((hp / maxHp) * 100.0f);
    this->shieldDamageRate = (int) shieldRate;
    this->armorDamageRate = (int) armorRate;
    this->empDamageRate = (int) empRate;
}

void Player::setBombForce(float value) {
    this->bombForce = value;
}

void Player::setMaxHitpoints(int value) {
    this->maxHitpoints = value;
    this->hitpoints = value;
    this->updateDamageRate();
}

int Player::getGunRegenRate(int slot) {
    (void) slot;
    return 0;
}

int Player::getMaxEmpPoints() {
    return this->maxEmpPoints;
}

void Player::regenerateShield(float amount) {
    float f = this->shieldHP + amount;
    float maxF = (float) this->maxShieldHP;
    if (f < maxF) {
        maxF = f;
    }
    this->shieldHP = maxF;
    this->updateDamageRate();
}

unsigned char Player::doesNeverAttack() {
    return this->neverAttack;
}

int Player::getMaxShieldHP() {
    return this->maxShieldHP;
}

unsigned char Player::isDamaged() {
    return this->damaged;
}

unsigned char Player::isActive() {
    return this->active;
}

int Player::getEmpPoints() {
    return this->empPoints;
}

int Player::GetEngineEvent() {
    return (int) (__INTPTR_TYPE__) this->engineEvent;
}

void Player::setEmpForce(float value) {
    this->empForce = value;
}

void Player::setShootingEnabled(bool value) {
    this->shootingEnabled = value;
}

int Player::getDamageRate() {
    return this->damageRate;
}

float Player::getEmpForce() {
    return this->empForce;
}

unsigned char Player::turnedEnemy() {
    return this->turnedEnemyFlag;
}

bool Player::gunAvailable(int slot) {
    if (slot < 4) {
        Array<Gun *> *slotArray = this->guns->data()[slot];
        if (slotArray != 0 && slotArray->size() != 0) {
            return *(int *) slotArray->data() != 0;
        }
    }
    return false;
}

int Player::getCombinedHP() {
    return (int) (this->shieldHP + (float) this->armorHP + (float) this->hitpoints);
}

bool Player::isGasCloud() {
    KIPlayer *ki = this->kiPlayer;
    bool result = false;
    if (ki != 0) {
        result = ki->field_0x44 != 0;
    }
    return result;
}

void Player::setArmorHP(int value) {
    if (this->maxArmorHP < value) {
        value = this->maxArmorHP;
    }
    this->armorHP = value;
    this->updateDamageRate();
}

void Player::setHitpoints(int value) {
    this->hitpoints = value;
    if (this->maxHitpoints < value) {
        this->maxHitpoints = value;
    }
    this->updateDamageRate();
}

void Player::setNeverAttack(bool value) {
    this->neverAttack = value;
}

void Player::setMaxShieldHP(int value) {
    this->maxShieldHP = value;
    this->shieldHP = (float) value;
    this->updateDamageRate();
}

void Player::setMaxEmpPoints(int value) {
    this->empPoints = value;
    this->maxEmpPoints = value;
    this->updateDamageRate();
}

int Player::getGunSlots() {
    return 3;
}

struct HitVec3 {
    double xy;
    float z;
};

Vector Player::getHitVector() {
    Vector out;
    double xy = *(double *) this->hitVector;
    out.z = this->hitVector[2];
    *(double *) &out.x = xy;
    return out;
}

void Player::setPlayShootSound(bool play, int id) {
    this->playShootSoundFlag = play;
    this->playShootSoundId = id;
}

void Player::regenerateShield() {
    float f = this->shieldHP + 1.0f;
    float maxF = (float) this->maxShieldHP;
    if (f < maxF) {
        maxF = f;
    }
    this->shieldHP = maxF;
    this->updateDamageRate();
}

void Player::heal(float amount) {
    float f = this->healAccumulator + amount;
    this->healAccumulator = f;
    if (f > 1.0f) {
        int count = (int) f;
        for (int i = 0; i < count; i++) {
            ((Player *) (this))->regenerateHull();
        }
        this->healAccumulator = this->healAccumulator - (float) count;
    }
}

void Player::setShieldHP(int value) {
    float maxF = (float) this->maxShieldHP;
    this->shieldHP = (float) value;
    if ((float) value > maxF) {
        this->shieldHP = maxF;
    }
    this->updateDamageRate();
}

void Player::refillGunDelay(int slot) {
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns != 0 && slot >= 0 && (unsigned int) slot < guns->size()) {
        Array<Gun *> *arr = guns->data()[slot];
        if (arr != 0) {
            int n = arr->size();
            for (int i = 0; n != i; i++) {
                Gun *gun = arr->data()[i];
                gun->timer = gun->fireDelay;
            }
        }
    }
}

void Player::addEnemies(Array<Player *> *enemies) {
    if (this->enemies == 0) {
        this->setEnemies(enemies);
        return;
    }
    Array<Player *> *tmp = new Array<Player *>();
    for (unsigned int i = 0; i < this->enemies->size(); i++) {
        ArrayAdd(this->enemies->data()[i], *tmp);
    }
    for (unsigned int i = 0; i < enemies->size(); i++) {
        ArrayAdd(enemies->data()[i], *tmp);
    }
    ((Player *) (this))->setEnemies(tmp);
    delete tmp;
}

Player::Player(int radius, int hitpoints, int numPrimary, int numSecondary, int numTertiary) {
    Player * self = this;

    *reinterpret_cast<AbyssEngine::AEMath::Matrix *>(self->transform) = AbyssEngine::AEMath::Matrix();
    self->shieldHP = 0.0f;
    self->armorHP = 0;
    self->maxArmorHP = 0;
    self->maxShieldHP = 0;
    self->hitVector[0] = 0.0f;
    self->hitVector[1] = 0.0f;
    self->hitVector[2] = 0.0f;
    self->position[1] = 0.0f;
    self->position[2] = 0.0f;
    self->radius = radius;
    self->numPrimaryGuns = numPrimary;
    self->numSecondaryGuns = numSecondary;
    self->numTertiaryGuns = numTertiary;
    self->gammaHP = 100.0f;
    self->hitpoints = hitpoints;
    self->empPoints = 0;
    self->maxEmpPoints = 0;
    self->maxHitpoints = hitpoints;
    self->damageDoneByPlayer = 0;
    self->field_5e = 0;
    self->shootingEnabled = 1;
    self->position[0] = 0.0f;
    self->flShake = 0.0f;
    self->bombForce = 0.0f;
    self->empForce = 0.0f;
    ((Player *) (self))->updateDamageRate();
    self->field_58 = -1;

    Array<Array<Gun *> *> *gunArr = new Array<Array<Gun *> *>();
    self->guns = gunArr;
    ArraySetLength(3, *gunArr);

    if (numPrimary < 1) {
        self->guns->data()[0] = 0;
    } else {
        Array<Gun *> *a = new Array<Gun *>();
        self->guns->data()[0] = a;
        ArraySetLength(numPrimary, *self->guns->data()[0]);
    }
    if (numSecondary < 1) {
        self->guns->data()[1] = 0;
    } else {
        Array<Gun *> *a = new Array<Gun *>();
        self->guns->data()[1] = a;
        ArraySetLength(numSecondary, *self->guns->data()[1]);
    }
    if (numTertiary < 1) {
        self->guns->data()[2] = 0;
    } else {
        Array<Gun *> *a = new Array<Gun *>();
        self->guns->data()[2] = a;
        ArraySetLength(numTertiary, *self->guns->data()[2]);
    }

    self->playShootSoundFlag = 1;
    self->playShootSoundId = 1;
    self->destroyed = 0;
    self->active = 1;
    self->damageTimer = 0;
    self->vulnerable = 1;
    self->kiPlayer = 0;
    self->bombForce = 0.0f;
    self->enemyFlags = 0;
    self->turnedEnemyFlag = 0;
    self->alwaysEnemy = 0;
    self->alwaysFriend = 0;
    self->enemies = 0;
    self->field_54 = 0;
    self->empDisabled = 0;
    self->neverAttack = 0;
    self->healAccumulator = 0;
    self->engineEvent = 0;
    self->engineSoundPlaying = 0;
    self->enginePaused = 0;
    self->shieldHit = 0;
    self->armorHit = 0;
    self->hullHit = 0;
    self->gammaHit = 0;

    float tmp[3];
    MatrixGetPosition(tmp, self->transform);
    *(Vector *) self->position = *(Vector *) tmp;
    self->enginePositionVec = (void *) (__INTPTR_TYPE__) -1;
}

Vector Player::getPosition() {
    Vector out;
    MatrixGetPosition(&out, this->transform);
    return out;
}

float Player::damageGamma(float amount) {
    if (this->vulnerable) {
        if (this->active) {
            amount = this->gammaHP - amount;
            this->gammaHit = 1;
            this->gammaHP = amount;
            if (!(amount > 0.0f)) {
                this->gammaHP = 0;
            }
        }
    }
    return amount;
}

void Player::damageEmp(int amount, bool flag) {
    Player * self = this;
    if (self->vulnerable == 0 || self->active == 0) {
        return;
    }
    int hp = self->empPoints;
    if (hp > 0) {
        hp = self->hitpoints;
    }
    if (hp < 1) {
        return;
    }

    if (!flag && self->kiPlayer != 0 && self->alwaysEnemy == 0) {
        KIPlayer *ki = self->kiPlayer;
        bool runLab = true;
        if ((unsigned int) (ki->shipGroup - 9) >= 2) {
            int sys = Status::gStatus->getSystem();
            ki = self->kiPlayer;
            if (sys != 0 && ki->field_0x42 != 0) {
                if (amount > 0) {
                    ((Level *) (ki->level))->attackWanted(ki->field_0x48);
                }
                runLab = false;
            } else if (self->kiPlayer == 0) {
                runLab = false;
            }
        }

        if (runLab && self->alwaysEnemy == 0 && self->kiPlayer->isWingMan() == 0 &&
            (unsigned int) (self->kiPlayer->shipGroup - 9) > 1 &&
            Status::gStatus->getSystem() != 0) {
            int race = self->kiPlayer->shipGroup;
            Status::gStatus->getSystem();
            if (race == ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getRace()) {
                int prev = self->field_dc;
                self->field_dc = prev + amount;
                if (self->maxEmpPoints / 3 < prev + amount) {
                    self->turnedEnemyFlag = 1;
                    ((Level *) (self->kiPlayer->level))->friendTurnedEnemy(0);
                }
            }
        }
    }

    {
        int ep = self->empPoints - amount;
        self->empPoints = ep;
        if (ep > 0) {
            self->updateDamageRate();
            return;
        }
    }
    if (!flag && self->kiPlayer != 0) {
        if (self->alwaysEnemy == 0 &&
            (unsigned int) (self->kiPlayer->shipGroup - 9) > 1 &&
            Status::gStatus->getSystem() != 0) {
            int race = self->kiPlayer->shipGroup;
            Status::gStatus->getSystem();
            if (race == ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getRace()) {
                ((Level *) (self->kiPlayer->level))->alarmAllFriends(self->kiPlayer->shipGroup, false);
            }
        }
        KIPlayer *ki = self->kiPlayer;
        if (ki->stealFlag != 0) {
            goto lab_30e2;
        }
        if (ki->countsAsEnemyExcludeFlag != 0) {
            goto lab_30f4;
        }
        if (ki->field_0x42 != 0) {
            goto lab_30f4;
        }
        {
            void *st = (void *) (long) Status::gStatus->getStanding();
            ((Standing *) (st))->applyDisable(self->kiPlayer->shipGroup);
            ki = self->kiPlayer;
        }
    lab_30e2:
        if (ki == 0) {
            goto lab_3164;
        }
    lab_30f4:
        if (self->empDisabled != 0) {
            goto lab_3164;
        }
        {
            if ((unsigned int) Status::gStatus->field_134 > 0x7fffffff) {
                Status::gStatus->field_134 = 0;
            }
            if (Achievements::gAchievements->hasMedal(0x2a, 1) == 0) {
                int cnt = Status::gStatus->field_134 + 1;
                Status::gStatus->field_134 = cnt;
                if (Achievements::gAchievements->getValue(0x2a, 1) <= cnt) {
                    void *ego = (void *) (__INTPTR_TYPE__) ((Level *) (self->kiPlayer->level))->getPlayer();
                    void *hud = (void *) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getHUD();
                    ((Hud *) (hud))->hudEventMedal(0x2a, 100);
                    Status::gStatus->field_138 = 1;
                }
            }
        }
    }

lab_3164:

    float f = (float) self->empData;
    self->empDisabled = 1;
    self->empPoints = 0;
    (self->pad_e8[0]) = 0;
    self->empForce = f;
    self->updateDamageRate();
    return;
}

void Player::addGun(Array<Gun *> *gunsIn, int slot) {
    if (this->guns != 0) {
        if ((unsigned int) slot < 4) {
            Array<Gun *> *arr = new Array<Gun *>();
            this->guns->data()[slot] = arr;
            for (unsigned int i = 0; i < gunsIn->size(); i++) {
                ArrayAdd(gunsIn->data()[i], *this->guns->data()[slot]);
            }
        }
        if (this->playShootSoundFlag) {
            this->calcWeaponSounds(this->playShootSoundId);
            return;
        }
    }
}

void Player::setAlwaysEnemy(bool value) {
    this->alwaysEnemy = value;
    this->enemyFlags = 1;
    this->turnedEnemyFlag = 1;
}

void Player::regenerateHull() {
    int v = this->maxHitpoints;
    if (this->hitpoints + 1 < this->maxHitpoints) {
        v = this->hitpoints + 1;
    }
    this->hitpoints = v;
    this->updateDamageRate();
}

Player::~Player() {
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns != 0) {
        for (unsigned int i = 0; i < guns->size(); i++) {
            Array<Gun *> *slot = guns->data()[i];
            if (slot != 0) {
                ArrayReleaseClasses(*slot);
                Array<Gun *> *s2 = this->guns->data()[i];
                if (s2 == 0) {
                    this->guns->data()[i] = 0;
                } else {
                    delete s2;
                    this->guns->data()[i] = 0;
                }
                guns = this->guns;
            }
        }
        ArrayReleaseClasses(*guns);
        delete this->guns;
        this->guns = 0;
    }
    if (this->enemies != 0) {
        delete this->enemies;
    }
    this->enemies = 0;
}

void Player::setGammaHP(int value) {
    float f = (float) value;
    float sel = f;
    if (value != 9999999) {
        sel = 100.0f;
    }
    if (value > 100) {
        f = sel;
    }
    this->gammaHP = f;
    this->updateDamageRate();
}

void Player::stopShootSound(int index, int channel) {
    if ((unsigned int) channel > 8) {
        return;
    }
    if (((1 << channel) & 0x10c) != 0) {
        void *sound;
        int id;
        if (this->kiPlayer != 0 && this->kiPlayer->shipGroup == 9) {
            id = 0x3e;
            sound = gFModSoundAlt;
        } else {
            id = gStopSoundIds[index];
            sound = gFModSound;
        }
        ((FModSound *) sound)->stop(id);
    }
}

void Player::reset() {
    Player * self = this;
    float shield = (float) self->maxShieldHP;
    int maxHp = self->maxHitpoints;
    int maxEmp = self->maxEmpPoints;
    self->gammaHP = 100.0f;
    self->hitpoints = maxHp;
    self->empPoints = maxEmp;
    self->shieldHP = shield;
    ((Player *) (self))->updateDamageRate();
    self->vulnerable = 1;
    self->active = 1;
    self->field_54 = 0;
    self->destroyed = 0;
    self->damageDoneByPlayer = 0;
    self->field_5e = 0;
    self->damageTimer = 0;
    self->empDisabled = 0;
    self->shieldHit = 0;
    self->armorHit = 0;
    self->hullHit = 0;
    self->gammaHit = 0;
    self->bombForce = 0;
    self->empForce = 0;
    (self->pad_dd[0]) = 0;
    (((uint8_t *) &self->empForce)[1]) = 0;
}

void Player::addGun(Gun *gun, int slot) {
    if (this->guns != 0) {
        if ((unsigned int) slot < 4) {
            Array<Gun *> *arr = new Array<Gun *>();
            this->guns->data()[slot] = arr;
            ArrayAdd(gun, *this->guns->data()[slot]);
        }
        if (this->playShootSoundFlag) {
            this->calcWeaponSounds(this->playShootSoundId);
            return;
        }
    }
}

void Player::calcWeaponSounds(int count) {
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns == 0) {
        return;
    }
    if (guns->data()[0] != 0) {
        unsigned int n = ((Array<Gun *> *) guns->data()[0])->size();
        int *order = new int[n];
        for (int i = 0; i < (int) n; i++) {
            order[i] = this->guns->data()[0]->data()[i]->itemIndex;
        }

        bool sorted = true;
        int i = 1;
        Array<Item *> **itemTablePtr = reinterpret_cast<Array<Item *> **>(g_cws_items);
        do {
            for (; i < (int) n; i++) {
                Item **dataArr = (*itemTablePtr)->data_;
                int a = dataArr[order[i - 1]]->getSinglePrice();
                dataArr = (*itemTablePtr)->data_;
                int b = dataArr[order[i]]->getSinglePrice();
                if (a < b) {
                    sorted = false;
                    int t = order[i - 1];
                    order[i - 1] = order[i];
                    order[i] = t;
                }
            }
            bool again = !sorted;
            sorted = true;
            i = 1;
            if (!again) break;
        } while (true);

        for (unsigned int x = 0; x < n; x++) {
            for (unsigned int y = 0; y != n; y++) {
                if (x != y && order[x] == order[y]) {
                    order[y] = -1;
                }
            }
        }

        int idx = 0;
        int *sound = g_cws_sound;
        do {
            if ((int) n <= idx) break;
            if (order[idx] >= 0) {
                this->guns->data()[0]->data()[idx]->field_0x89 = 1;
                Globals::gGlobals->addSoundResourceToList(*sound);
                count--;
            }
            idx++;
        } while (count != 0);

        delete[] order;
        guns = this->guns;
    }

    if (guns->size() > 2) {
        Array<Gun *> *slot2 = guns->data()[2];
        if (slot2 != 0 && slot2->size() != 0) {
            Gun *g = slot2->data()[0];
            if (g != 0) {
                int sid = g_cws_sound3[g->itemIndex];
                g->field_0x89 = 1;
                (void) sid;
                Globals::gGlobals->addSoundResourceToList(*g_cws_sound2);
                return;
            }
        }
    }
}

void Player::setEnemies(Array<Player *> *enemies) {
    if (this->enemies != 0) {
        delete this->enemies;
    }
    this->enemies = 0;
    if (enemies != 0) {
        Array<Player *> *copy = new Array<Player *>();
        this->enemies = copy;
        for (unsigned int i = 0; i < enemies->size(); i++) {
            ArrayAdd(enemies->data()[i], *copy);
        }
    }
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns != 0) {
        for (unsigned int i = 0; i < guns->size(); i++) {
            Array<Gun *> *slot = guns->data()[i];
            if (slot != 0) {
                for (unsigned int j = 0; j < slot->size(); j++) {
                    Gun *gun = slot->data()[j];
                    if (gun != 0) {
                        Gun_setEnemies(gun);
                        guns = this->guns;
                        slot = guns->data()[i];
                    }
                }
            }
        }
    }
}

void Player::playShootSound(int type, int channel, Vector *pos, float volume) {
    int soundId;
    if (this->kiPlayer == 0) {
        soundId = gShootSoundsByIndex[type];
    } else {
        unsigned int kind = static_cast<unsigned int>(this->kiPlayer->shipGroup);
        if (kind < 0xb) {
            soundId = gShootSoundsByType[kind];
        } else {
            soundId = 0x3d;
        }
    }

    FModSound *sound = reinterpret_cast<FModSound *>(gFModSoundPtr[0]);
    if (static_cast<unsigned int>(channel) < 9 && ((1 << (channel & 0xff)) & 0x10c) != 0) {
        if (sound->isPlaying(soundId) != 0) {
            if (reinterpret_cast<AudioStateView *>(gAppManagerA)->soundEnabledFlag != 0) {
                sound->updateEvent3DAttributes(soundId, pos, 0, false);
            }
            return;
        }
        if (reinterpret_cast<AudioStateView *>(gAppManagerB)->soundEnabledFlag == 0) {
            pos = 0;
        }
    } else if (reinterpret_cast<AudioStateView *>(gAppManagerC)->soundEnabledFlag == 0) {
        pos = 0;
    }
    sound->play(soundId, pos, 0, volume);
}

static const float k_shootAt_inc = 0.0f;

static const float k_damage_full = 0.0f;
static const float k_damage_hc = 0.0f;
static const float k_damage_full2 = 0.0f;
static const float k_damage_hc2 = 0.0f;
static const float k_damage_regen = 0.0f;

void Player::damage(int amount, bool flag, int missionId) {
    Player * self = this;
    if (self->vulnerable == 0) return;
    if (self->active == 0) return;
    if (self->hitpoints < 1) return;

    if (flag == 0 && self->kiPlayer != 0) {
        KIPlayer *ki = self->kiPlayer;
        if (self->alwaysEnemy == 0 &&
            (unsigned int) (ki->shipGroup - 9) > 1 &&
            Status::gStatus->getSystem() != 0 &&
            ((self->enemyFlags == 0) || (self->turnedEnemyFlag != 0))) {
            ki = self->kiPlayer;
            if (ki->field_0x42 != 0) {
                if (amount > 0) {
                    self->damageDoneByPlayer = self->damageDoneByPlayer + amount;
                    ((Level *) (ki->level))->attackWanted(ki->field_0x48);
                }
                goto LAB_3488;
            }
        } else {
            ki = self->kiPlayer;
        }

        if (ki != 0 && self->alwaysEnemy == 0 &&
            (unsigned int) (ki->shipGroup - 9) > 1 &&
            self->kiPlayer->isWingMan() == 0 && Status::gStatus->getSystem() != 0 &&
            ((self->enemyFlags == 0) || (self->turnedEnemyFlag != 0))) {
            int race = self->kiPlayer->shipGroup;
            Status::gStatus->getSystem();
            bool sameRace = (race == ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getRace());
            if (!sameRace) {
                int race2 = self->kiPlayer->shipGroup;
                void *sys = (void *) (long) Status::gStatus->getSystem();
                if (race2 != ((SolarSystem *) (sys))->getAttackRace()) {
                    goto LAB_342a;
                }
            }
            self->damageDoneByPlayer = self->damageDoneByPlayer + amount;
            int hc = Status::gStatus->hardCoreMode();
            float thr1 = (hc != 0) ? k_damage_hc : k_damage_full;
            float f1 = thr1 * (float) self->maxHitpoints;
            float dmgF = (float) self->damageDoneByPlayer;
            float frac2 = (hc != 0) ? 0.25f : 0.5f;
            float thr3 = (hc != 0) ? k_damage_hc2 : k_damage_full2;

            if (f1 < dmgF) {
                ((Level *) (self->kiPlayer->level))->friendTurnedEnemy(0);
                void *ship = (void *) Status::gStatus->getShip();
                void *standing = (void *) (long) Status::gStatus->getStanding();
                if (((Ship *) (ship))->getSignatureRace() >= 0) {
                    bool match = ((unsigned int) ((Ship *) (ship))->getSignatureRace() ==
                                  (unsigned int) self->kiPlayer->shipGroup);
                    unsigned int dis = 0;
                    if (match) dis = (unsigned char) self->kiPlayer->field_0x42;
                    if (match && dis == 0) {
                        Item *item = ((Ship *) (ship))->getFirstEquipmentOfSort(0x1d);
                        ((Ship *) (ship))->removeEquipment(item);
                        void *st2 = (void *) (long) Status::gStatus->getStanding();
                        ((Standing *) (st2))->applyDelict(((Ship *) (ship))->getSignatureRace(), 100);
                        ((Standing *) (standing))->setPlayerSignatureRace(-1);
                        void *ego = (void *) (__INTPTR_TYPE__) ((Level *) (self->kiPlayer->level))->getPlayer();
                        int hud = (int) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getHUD();
                        PlayerEgo *p = ((Level *) (self->kiPlayer->level))->getPlayer();
                        ((Hud *) (hud))->hudEvent(0x1f, p, 0);
                    }
                }
            }

            float f3 = (float) self->maxHitpoints;
            float dmgF3 = (float) self->damageDoneByPlayer;
            if (frac2 * f3 < dmgF3) {
                void *ship = (void *) Status::gStatus->getShip();
                void *standing = (void *) (long) Status::gStatus->getStanding();
                if (((Ship *) (ship))->getSignatureRace() >= 0 &&
                    self->kiPlayer->shipGroup < 4 &&
                    self->kiPlayer->field_0x42 == 0) {
                    Item *item = ((Ship *) (ship))->getFirstEquipmentOfSort(0x1d);
                    ((Ship *) (ship))->removeEquipment(item);
                    void *st2 = (void *) (long) Status::gStatus->getStanding();
                    ((Standing *) (st2))->applyDelict(((Ship *) (ship))->getSignatureRace(), 100);
                    ((Standing *) (standing))->setPlayerSignatureRace(-1);
                    void *ego = (void *) (__INTPTR_TYPE__) ((Level *) (self->kiPlayer->level))->getPlayer();
                    int hud = (int) (__INTPTR_TYPE__) ((PlayerEgo *) (ego))->getHUD();
                    PlayerEgo *p = ((Level *) (self->kiPlayer->level))->getPlayer();
                    ((Hud *) (hud))->hudEvent(0x1f, p, 0);
                }
                self->turnedEnemyFlag = 1;
            }

            float f4a = (float) self->maxHitpoints;
            float dmgF4 = (float) self->damageDoneByPlayer;
            if (thr3 * f4a < dmgF4) {
                ((Level *) (self->kiPlayer->level))->alarmAllFriends(self->kiPlayer->shipGroup, true);
            }
            goto LAB_3488;
        }
    }

LAB_342a: {
        if (Status::gStatus->inBlackMarketSystem() != 0) {
            KIPlayer *ki = self->kiPlayer;
            if (ki != 0 && ki->shipGroup == 8) {
                self->turnedEnemyFlag = 1;
                ((Level *) (ki->level))->alarmAllFriends(8, true);
                Array<KIPlayer *> *enemies = ((Level *) (ki->level))->getEnemies();
                if (enemies != nullptr) {
                    unsigned count = enemies->size();
                    unsigned i = 0;
                    while (count != i) {
                        KIPlayer *e = (*enemies)[i];
                        i++;
                        if (e->shipGroup == 8) {
                            e->field_0x25 = 1;
                        }
                    }
                }
                Status::gStatus->field_110 = 0x100;
            }
        }
    }

LAB_3488: {
        int shieldI = (int) self->shieldHP - amount;
        if (shieldI < 0) {
            self->shieldHP = 0.0f;
            shieldI = shieldI + self->armorHP;
            if (shieldI < 0) {
                self->armorHP = 0;
                self->hullHit = 1;
                self->hitpoints = shieldI + self->hitpoints;
            } else {
                self->armorHit = 1;
                self->armorHP = shieldI;
            }
        } else {
            self->shieldHit = 1;
            self->shieldHP = (float) shieldI;
        }
    }

    {
        int hp;
        KIPlayer *ki = self->kiPlayer;
        if (ki != 0 && ki->stealFlag == 0 && ki->countsAsEnemyExcludeFlag == 0 &&

            ki->field_0x42 != 0) {
            hp = self->hitpoints;
            if (self->maxHitpoints / 3 > hp) {
                ((Level *) (ki->level))->almostKillWanted(ki->field_0x48);
            } else {
                goto LAB_34f8_hp;
            }
        }
        hp = self->hitpoints;
    LAB_34f8_hp:
        if (hp < 1) {
            self->hitpoints = 0;
            if (flag != 0) {
                self->destroyed = 1;
            } else {
                KIPlayer *ki2 = self->kiPlayer;
                if (ki2 != 0 && ki2->stealFlag == 0 && ki2->countsAsEnemyExcludeFlag == 0 &&

                    Status::gStatus->inBlackMarketSystem() == 0) {
                    if (self->kiPlayer->field_0x42 == 0) {
                        void *st = (void *) (long) Status::gStatus->getStanding();
                        ((Standing *) (st))->applyKill(self->kiPlayer->shipGroup);
                    }
                    int mission = Status::gStatus->getCampaignMission();
                    KIPlayer *ki3 = self->kiPlayer;

                    void *txt = ((GameText *) (*g_damage_text[0]))->getText(missionId);
                    int cmp = (&ki3->name)->Compare_str((String *) txt);
                    if (missionId == 0xb3 && cmp == 0) {
                        ((Mission *) (intptr_t) mission)->setStatusValue(
                            ((Mission *) (intptr_t) mission)->getStatusValue() + 1);
                    }
                    if (self->kiPlayer->field_0x42 != 0) {
                        ((Level *) (self->kiPlayer->level))->killWanted(0);
                    }
                }
            }
        }
    }

    self->damaged = 1;
    self->flShake = self->flShake + k_damage_regen;
    ((Player *) (self))->updateDamageRate();
    if (self->kiPlayer != 0) {
        SlaveLink *slave = reinterpret_cast<SlaveLink *>(self->kiPlayer->field_0x10);
        if (slave != 0) {
            Player * other = slave->player;

            other->vulnerable = 1;
            other->damage(amount);
            other->vulnerable = 0;
        }
    }
}

void Player::setEnemy(Player *enemy) {
    Array<Player *> *tmp = new Array<Player *>();
    ArrayAdd(enemy, *tmp);
    ((Player *) (this))->setEnemies(tmp);
    delete tmp;
}

void Player::addEnemy(Player *enemy) {
    if (this->enemies == 0) {
        this->setEnemy(enemy);
        return;
    }
    Array<Player *> *tmp = new Array<Player *>();
    if (this->enemies->size() != 0) {
        for (unsigned int i = 0; i < this->enemies->size(); i++) {
            ArrayAdd(this->enemies->data()[i], *tmp);
        }
    }
    ArrayAdd(enemy, *tmp);
    ((Player *) (this))->setEnemies(tmp);
    delete tmp;
}

void Player::damage(int amount) {
    this->damage(amount, false, -1);
}

void Player::stopShooting(int slot, int channel) {
    if ((unsigned int) (channel - 0x16) >= 9) {
        return;
    }
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns == 0) {
        return;
    }
    if (slot < 0) {
        return;
    }
    if ((unsigned int) slot >= guns->size()) {
        return;
    }
    Array<Gun *> *arr = guns->data()[slot];
    if (arr == 0) {
        return;
    }
    for (unsigned int i = 0; i < arr->size(); i++) {
        Gun *gun = arr->data()[i];
        ((Player *) (this))->stopShootSound(gun->itemIndex, gun->weaponType);
        arr = this->guns->data()[slot];
    }
}

void Player::setAlwaysFriend(bool value) {
    this->alwaysFriend = value;
    this->enemyFlags = 0x100;
    this->turnedEnemyFlag = 0;
}

void Player::setMaxArmorHP(int value) {
    this->armorHP = value;
    this->maxArmorHP = value;
    this->updateDamageRate();
}

static const float k_shoot_inc = 0.0f;

static void (**g_update_transform)(void *, void *, int) = nullptr;

static const float k_update_a = 0.0f;
static const float k_update_b = 0.0f;
static const float k_update_c = 0.0f;

Vector *Player::update(int dt, bool doSound) {
    Player * self = this;

    int b4 = self->damageTimer + dt;
    self->damageTimer = b4;
    if (b4 > 3000) {
        self->damaged = 0;
        self->damageTimer = 0;
    }

    Vector *result = 0;

    if (self->empDisabled != 0) {
        int e = self->empData + dt;
        float ef = (float) self->empData;
        float ef2 = (float) e;
        int maxEmp = self->maxEmpPoints;
        float mf = (float) maxEmp;
        float v = (ef2 / ef) * mf;
        self->empData = e;
        self->empPoints = (int) v;
        if (maxEmp < (int) v) {
            self->empDisabled = 0;
            self->empPoints = maxEmp;
            Status::gStatus->field_134 = Status::gStatus->field_134 - 1;
            self->empData = 0;
        }
        ((Player *) (self))->updateDamageRate();
    }

    AudioStateView *flagObj = reinterpret_cast<AudioStateView *>(gEngine);
    if (flagObj->soundEnabledFlag == 0 || doSound == 0 || self->enginePositionVec == (void *) (__INTPTR_TYPE__) -1) {
        if (self->engineSoundPlaying != 0) {
            self->StopEngineSound();
        }
    } else {
        float *transform = self->transform;
        void (*fn)(void *, void *, int) = *g_update_transform;
        float local[3];
        float spd = (float) (**g_update_speed);
        fn(*g_update_speed, local, (int) (long) transform);
        float tmpA[3], tmpB[3], tmpC[3];
        float d = (*(Vector *) tmpA = *(const Vector *) self->position - *(const Vector *) local,
                   AbyssEngine::AEMath::VectorLength(*(const Vector *) tmpA));
        *(Vector *) tmpB = d * *(const Vector *) tmpA;
        *(Vector *) tmpC = *(const Vector *) tmpB;
        *(Vector *) tmpC /= (float) dt;
        (void) spd;
        fn(tmpB, local, (int) (long) transform);
        FMOD::Event *ev = ((FModSound *) (*g_update_sound))->updateEvent3DAttributes(
            self->engineEvent, 0, (Vector *) tmpB, (Vector *) tmpC, false);
        self->engineEvent = ev;
        self->engineSoundPlaying = 1;
        fn(tmpA, local, (int) (long) transform);
        *(Vector *) self->position = *(Vector *) tmpA;
    }

    float speed = (float) dt;
    float nf = self->flShake + speed * k_update_a * k_update_b;
    self->flShake = nf;
    FloatVectorMax(&result, nf, k_update_c, 2, 0x20);

    return result;
}

void Player::shoot(int a, int b, long long pos, bool flag) {
    Matrix mat;
    float *src = this->transform;
    for (int k = 0; k < 15; ++k) mat.m[k] = src[k];
    mat.m[15] = 1.0f;
    this->shoot(a, b, pos, flag, mat);
}

void Player::shoot(int a, int b, long long pos, bool flag, Matrix mat) {
    Player * self = this;
    unsigned int mask = g_shoot_mask;

    Array<Array<Gun *> *> *guns = self->guns;
    if (guns != 0 && self->shootingEnabled != 0 && b >= 0 &&
        (unsigned int) b < guns->size()) {
        Array<Gun *> *arr = guns->data()[b];
        if (arr != 0) {
            for (unsigned int i = 0; i < arr->size(); i++) {
                Gun *g = self->guns->data()[b]->data()[i];
                unsigned int sortIdx = g->weaponType - 6;
                if (sortIdx < 0x1d && ((1u << (sortIdx & 0xff)) & mask) != 0 &&
                    *(int *) g->lifetimes >= 0) {
                    ((Gun *) (g))->ignite();
                } else if (g->itemIndex == (int) pos && g->fireDelay < g->timer) {
                    if (sortIdx < 0x1d && ((1u << (sortIdx & 0xff)) & mask) != 0) {
                        ((Level *) (intptr_t) g->level)->field_69 = 1;
                    }
                    ((Gun *) (g))->shoot(mat, flag, false);
                    self->flShake = self->flShake + k_shoot_inc;
                    if (self->playShootSoundFlag != 0) {
                        float tmp[3];
                        MatrixGetPosition(tmp, self->transform);
                        Gun *g2 = self->guns->data()[b]->data()[i];
                        self->playShootSound(g2->itemIndex, g2->weaponType,
                                             reinterpret_cast<Vector *>(tmp),
                                             g2->field_0xb0);
                    }
                    g->timer = 0;
                    break;
                }
            }
        }
    }
    (void) a;
}

void Player::shoot(int a, long long pos, bool flag) {
    Matrix mat;
    float *src = this->transform;
    for (int k = 0; k < 15; ++k) mat.m[k] = src[k];
    mat.m[15] = 1.0f;
    this->shoot(a, pos, flag, mat);
}

void Player::shoot(int a, long long pos, bool flag, Matrix mat) {
    Player * self = this;

    Array<Array<Gun *> *> *guns = self->guns;
    if (guns != 0 && self->shootingEnabled != 0 && a >= 0 &&
        (unsigned int) a < guns->size()) {
        Array<Gun *> *arr = guns->data()[a];
        if (arr != 0) {
            for (unsigned int i = 0; i < arr->size(); i++) {
                Gun *g = self->guns->data()[a]->data()[i];
                if (g->fireDelay < g->timer) {
                    ((Gun *) (g))->shootAt(mat, (int) pos, self, flag);
                    self->flShake = self->flShake + k_shootAt_inc;
                    Gun *g2 = self->guns->data()[a]->data()[i];
                    g2->timer = 0;
                    if (self->playShootSoundFlag != 0 && g2->field_0x89 != 0) {
                        float tmp[3];
                        MatrixGetPosition(tmp, self->transform);
                        Gun *g3 = self->guns->data()[a]->data()[i];
                        self->playShootSound(g3->itemIndex, g3->weaponType,
                                             reinterpret_cast<Vector *>(tmp),
                                             g3->field_0xb0);
                    }
                }
            }
        }
    }
}

void Player::stopShooting(int slot) {
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns == 0) {
        return;
    }
    if (slot < 0) {
        return;
    }
    if ((unsigned int) slot >= guns->size()) {
        return;
    }
    Array<Gun *> *arr = guns->data()[slot];
    if (arr == 0) {
        return;
    }
    for (unsigned int i = 0; i < arr->size(); i++) {
        Gun *gun = arr->data()[i];
        ((Player *) (this))->stopShootSound(gun->itemIndex, gun->weaponType);
        arr = this->guns->data()[slot];
    }
}

float *Player::setHitVector(float x, float y, float z) {
    float *p = this->hitVector;
    *p++ = x;
    *p++ = y;
    *p++ = z;
    return p;
}

void Player::resetGunDelay(int slot) {
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns == 0) {
        return;
    }
    if (slot < 0) {
        return;
    }
    if ((unsigned int) slot >= guns->size()) {
        return;
    }
    Array<Gun *> *arr = guns->data()[slot];
    if (arr != 0) {
        int n = arr->size();
        for (int i = 0; n != i; i++) {
            arr->data()[i]->timer = 0;
        }
    }
}

Array<Player *> *Player::getEnemies() {
    return this->enemies;
}

void Player::PlayEngineSound(int unused, Vector *vec) {
    (void) unused;
    this->enginePositionVec = vec;
    if (reinterpret_cast<AudioStateView *>(ApplicationManager::gAppManager)->soundEnabledFlag != 0) {
        float pos[12];
        MatrixGetPosition(pos, this->transform);
        FMOD::Event *ev = ((FModSound *) gFModSoundPtr[0])->updateEvent3DAttributes(
            this->engineEvent, 0, (Vector *) this->enginePositionVec, (Vector *) pos, false);
        this->engineEvent = ev;
        this->engineSoundPlaying = 1;
    }
}

void Player::PauseEngineSound() {
    FMOD::Event *event = this->engineEvent;
    if (event != 0) {
        this->enginePaused = ((FModSound *) gFModSound)->pause(event);
    }
}

void Player::ResumeEngineSound(bool force) {
    FMOD::Event *event = this->engineEvent;
    if (event != 0 && (this->enginePaused != 0 || force)) {
        this->enginePaused = ((FModSound *) gFModSound)->resume(event) ^ 1;
    }
}

void Player::StopEngineSound() {
    FMOD::Event *event = this->engineEvent;
    if (event != 0) {
        ((FModSound *) gFModSound)->stop(event);
        this->engineSoundPlaying = 0;
        this->engineEvent = 0;
    }
}

// Static data members present in the original binary (defined for symbol parity).
void *Player::velocity;
