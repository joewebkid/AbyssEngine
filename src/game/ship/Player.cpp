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
#include "engine/math/AEMath.h"
#include <algorithm>
#include <cstring>
#if defined(__ARM_NEON)
#include <arm_neon.h>
#endif

namespace AbyssEngine {
    namespace AEMath {
        Vector operator-(const Vector &, const Vector &);

        Vector operator*(float, const Vector &);

        float VectorLength(const Vector &);
    }
}

// Android 2.0.16 rodata: item sound IDs @ 0x1fbb50, ship-group IDs @ 0x1fbf00.
static const int kItemSoundIds[] = {
    52, 53, 54, 56, 55, 57, 58, 59, 60, 50, 49, 50,
    78, 75, 79, 77, 72, 73, 74, 76, 80, 81, 66, 67,
    63, 64, 68, 65, 69, 70, 71, 82, 83, 84, 85, 86,
    87, 88, 89, 90, 91, 6, 7, 8, 9, 10, 11, 92,
    93, 94, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    1098, 1100, 1099, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, 1111, 1113, 1112, 1117,
    1095, 1096, 1097, 1101, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 1118, -1, -1, -1, 2254, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 2271, 2272, -1, -1, 2263, 2263, 2263, 2265, 2265,
    2265, -1, -1, -1, -1, 2254, 2267, 2268, 2252, -1, 2269, -1,
    2275, 2276, 2278, 2277, 2280,
};
static const int kGroupSoundIds[] = {52, 55, 54, 53, 61, 61, 61, 61, 61, 62, 2276};

// Wingman/slave link record referenced by KIPlayer::field_0x10: a small
// linkage struct whose +4 slot holds the linked Player.
struct SlaveLink {
    int field_0x0;
    Player *player;
};
#if __SIZEOF_POINTER__ == 4
static_assert(__builtin_offsetof(SlaveLink, player) == 4, "SlaveLink.player must be at +4");
#endif

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

void Player::replaceGuns(int, int, int, int, int, bool) {
}

int Player::getShieldHP() {
    return (int) this->shieldHP;
}

void Player::removeAllGuns() {
    if (this->guns != 0) {
        ArrayReleaseClasses(*this->guns);
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
        result = static_cast<uint8_t>(ki->stealFlag) != 0;
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
    if (static_cast<unsigned int>(slot) < 4) {
        Array<Gun *> *slotArray = this->guns->data()[slot];
        if (slotArray != 0 && slotArray->size() != 0) {
            return slotArray->data()[0] != nullptr;
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
        result = static_cast<uint8_t>(ki->field_0x44) != 0;
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
        for (int i = 0; i < (int) f; i++) {
            ((Player *) (this))->regenerateHull();
        }
        this->healAccumulator = this->healAccumulator - (float) (int) f;
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

Player::Player(int radius, int hitpoints, int numPrimary, int numSecondary, int numTertiary) : transformMatrix() {
    Player * self = this;

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
    self->damaged = 0;
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

    *reinterpret_cast<Vector *>(self->position) = AbyssEngine::AEMath::MatrixGetPosition(self->transformMatrix);
    self->enginePositionVec = (void *) (__INTPTR_TYPE__) -1;
}

Vector Player::getPosition() {
    return AbyssEngine::AEMath::MatrixGetPosition(this->transformMatrix);
}

void Player::damageGamma(float amount) {
    if (this->vulnerable) {
        if (this->active) {
            float remaining = this->gammaHP - amount;
            this->gammaHit = 1;
            this->gammaHP = remaining;
            if (remaining <= 0.0f) {
                std::memset(&this->gammaHP, 0, sizeof(this->gammaHP));
            }
        }
    }
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
            SolarSystem *sys = Status::gStatus->getSystem();
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
            if (race == ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getRace()) {
                int prev = self->field_dc;
                self->field_dc = prev + amount;
                if (self->maxEmpPoints / 3 < prev + amount) {
                    self->turnedEnemyFlag = 1;
                    ((Level *) (self->kiPlayer->level))->friendTurnedEnemy(self->kiPlayer->shipGroup);
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
            if (race == ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getRace()) {
                ((Level *) (self->kiPlayer->level))->alarmAllFriends(self->kiPlayer->shipGroup, false);
            }
        }
        KIPlayer *ki = self->kiPlayer;
        if (static_cast<uint8_t>(ki->stealFlag) != 0) {
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
        if (self->empDisabledByte != 0) {
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
                    *reinterpret_cast<uint8_t *>(&Status::gStatus->field_138) = 1;
                }
            }
        }
    }

lab_3164:

    float f = (float) self->empData;
    self->empDisabledByte = 1;
    self->empPoints = 0;
    self->empElapsed = 0;
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
    if (value > 100 && value != 9999999) value = 100;
    this->gammaHP = static_cast<float>(value);
    this->updateDamageRate();
}

void Player::stopShootSound(int index, int channel) {
    if ((unsigned int) channel <= 8 && ((1u << channel) & 0x10c) != 0) {
        if (this->kiPlayer != 0 && this->kiPlayer->shipGroup == 9) {
            Globals::sound->stop(0x3e);
            return;
        } else {
            Globals::sound->stop(kItemSoundIds[index]);
            return;
        }
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
    self->damaged = 0;
    self->field_54 = 0;
    self->destroyedByte = 0;
    self->damageDoneByPlayer = 0;
    self->field_5e = 0;
    self->damageTimer = 0;
    self->empDisabledByte = 0;
    self->shieldHit = 0;
    self->armorHit = 0;
    self->hullHit = 0;
    self->gammaHit = 0;
    self->bombForce = 0;
    self->empForce = 0;
    self->field_dc = 0;
    self->turnedEnemyFlag = 0;
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
        do {
            for (; i < (int) n; i++) {
                Item **dataArr = static_cast<Array<Item *> *>(Globals::items)->data_;
                int a = dataArr[order[i - 1]]->getSinglePrice();
                dataArr = static_cast<Array<Item *> *>(Globals::items)->data_;
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

        for (int x = 0; x < (int) n; x++) {
            for (int y = 0; y != (int) n; y++) {
                if (x != y && order[x] == order[y]) {
                    order[y] = -1;
                }
            }
        }

        int idx = 0;
        do {
            if ((int) n <= idx) break;
            if (order[idx] >= 0) {
                Gun *selected = this->guns->data()[0]->data()[idx];
                int itemIndex = selected->itemIndex;
                selected->field_0x89 = 1;
                Globals::gGlobals->addSoundResourceToList(kItemSoundIds[itemIndex]);
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
                int sid = kItemSoundIds[g->itemIndex];
                g->field_0x89 = 1;
                Globals::gGlobals->addSoundResourceToList(sid);
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
        ArrayAdd(*enemies, *copy);
    }
    Array<Array<Gun *> *> *guns = this->guns;
    if (guns != 0) {
        for (unsigned int i = 0; i < guns->size(); i++) {
            Array<Gun *> *slot = guns->data()[i];
            if (slot != 0) {
                for (unsigned int j = 0; j < slot->size(); j++) {
                    Gun *gun = slot->data()[j];
                    if (gun != 0) {
                        gun->setEnemies(this->enemies);
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
        soundId = kItemSoundIds[type];
    } else {
        unsigned int kind = static_cast<unsigned int>(this->kiPlayer->shipGroup);
        if (kind < 0xb) {
            soundId = kGroupSoundIds[kind];
        } else {
            soundId = 0x3d;
        }
    }

    if (static_cast<unsigned int>(channel) <= 8 && ((1u << channel) & 0x10c) != 0) {
        if (Globals::sound->isPlaying(soundId)) {
            if (Globals::options[0x0f]) {
                Globals::sound->updateEvent3DAttributes(soundId, pos, nullptr, false);
            }
            return;
        }
        if (!Globals::options[0x0f]) pos = nullptr;
    } else if (!Globals::options[0x0f]) {
        pos = nullptr;
    }
    Globals::sound->play(soundId, pos, nullptr, volume);
}

static const float k_shootAt_inc = 0.008f;

static const float k_damage_full = 0.33f;
static const float k_damage_hc = 0.1f;
static const float k_damage_full2 = 0.66f;
static const float k_damage_hc2 = 0.4f;
static const float k_damage_regen = 0.065f;

void Player::damage(int amount, bool flag, int missionId) {
    Player * self = this;
    if (self->vulnerable == 0) return;
    if (self->active == 0) return;
    if (self->hitpoints < 1) return;

    // Android a31f4 skips reputation/black-market effects for this damage flag.
    if (flag) goto LAB_3488;
    if (self->kiPlayer != 0) {
        KIPlayer *ki = self->kiPlayer;
        if (self->alwaysEnemy == 0 &&
            (unsigned int) (ki->shipGroup - 9) > 1 &&
            Status::gStatus->getSystem() != 0 &&
            ((self->enemyFlagsLo == 0) || (self->turnedEnemyFlag != 0))) {
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
            ((self->enemyFlagsLo == 0) || (self->turnedEnemyFlag != 0))) {
            int race = self->kiPlayer->shipGroup;
            bool sameRace = (race == ((SolarSystem *) (intptr_t) Status::gStatus->getSystem())->getRace());
            if (!sameRace) {
                int race2 = self->kiPlayer->shipGroup;
                SolarSystem *sys = Status::gStatus->getSystem();
                if (race2 != sys->getAttackRace()) {
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
                ((Level *) (self->kiPlayer->level))->friendTurnedEnemy(self->kiPlayer->shipGroup);
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
                // Android damage body writes both wanted-state bytes at Status +0x110.
                uint16_t wantedState = 0x100;
                std::memcpy(&Status::gStatus->field_110, &wantedState, sizeof(wantedState));
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
        if (ki != 0 && static_cast<uint8_t>(ki->stealFlag) == 0 && ki->countsAsEnemyExcludeFlag == 0 &&

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
                self->destroyedByte = 1;
            } else {
                KIPlayer *ki2 = self->kiPlayer;
                if (ki2 != 0 && static_cast<uint8_t>(ki2->stealFlag) == 0 && ki2->countsAsEnemyExcludeFlag == 0 &&

                    Status::gStatus->inBlackMarketSystem() == 0) {
                    if (self->kiPlayer->field_0x42 == 0) {
                        void *st = (void *) (long) Status::gStatus->getStanding();
                        ((Standing *) (st))->applyKill(self->kiPlayer->shipGroup);
                    }
                    int mission = Status::gStatus->getCampaignMission();
                    KIPlayer *ki3 = self->kiPlayer;

                    void *txt = Globals::gameText->getText(1664);
                    int cmp = (&ki3->name)->Compare_str((String *) txt);
                    if (missionId == 0xb3 && cmp == 0) {
                        ((Mission *) (intptr_t) mission)->setStatusValue(
                            ((Mission *) (intptr_t) mission)->getStatusValue() + 1);
                    }
                    if (self->kiPlayer->field_0x42 != 0) {
                        ((Level *) (self->kiPlayer->level))->killWanted(self->kiPlayer->field_0x48);
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
        ArrayAdd(*this->enemies, *tmp);
    }
    ArrayAdd(enemy, *tmp);
    ((Player *) (this))->setEnemies(tmp);
    delete tmp;
}

void Player::damage(int amount) {
    this->damage(amount, false, -1);
}

void Player::stopShooting(int slot, int channel) {
    if ((unsigned int) (channel - 0x16) < 9) {
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

static const float k_shoot_inc = 0.008f;

// Android 0xa4110: +0xe4 duration, +0xe8 elapsed; options +0x0f gates 3D audio.
void Player::update(int dt, bool doSound) {
    damageTimer += dt;
    if (damageTimer > 3000) {
        damaged = 0;
        damageTimer = 0;
    }
    if (empDisabledByte) {
        int elapsed = empElapsed + dt;
        float points = (static_cast<float>(elapsed) / static_cast<float>(empData)) * static_cast<float>(maxEmpPoints);
        empElapsed = elapsed;
        empPoints = static_cast<int>(points);
        if (maxEmpPoints < static_cast<int>(points)) {
            empDisabledByte = 0;
            empPoints = maxEmpPoints;
            --Globals::status->field_134;
            empElapsed = 0;
        }
        updateDamageRate();
    }
    if (Globals::options[0x0f] && doSound && enginePositionVec != reinterpret_cast<void *>(static_cast<intptr_t>(-1))) {
        float audioSpeed = static_cast<float>(Player::velocity);
        Vector velocity = (audioSpeed * (AbyssEngine::AEMath::MatrixGetPosition(transformMatrix) -
                                        *reinterpret_cast<Vector *>(position))) / static_cast<float>(dt);
        Vector eventPosition = AbyssEngine::AEMath::MatrixGetPosition(transformMatrix);
        engineEvent = Globals::sound->updateEvent3DAttributes(engineEvent, static_cast<int>(reinterpret_cast<intptr_t>(enginePositionVec)), &eventPosition, &velocity, false);
        engineSoundPlaying = 1;
        *reinterpret_cast<Vector *>(position) = AbyssEngine::AEMath::MatrixGetPosition(transformMatrix);
    } else if (engineSoundPlaying) {
        StopEngineSound();
    }
    float shake = flShake + static_cast<float>(dt) * -0.001f * 0.025f;
#if defined(__ARM_NEON)
    flShake = vget_lane_f32(vmax_f32(vset_lane_f32(shake, vdup_n_f32(0.0f), 0), vdup_n_f32(0.0f)), 0);
#else
    flShake = std::max(shake, 0.0f);
#endif
}

void Player::shoot(int a, int b, long long pos, bool flag) {
    this->shoot(a, b, pos, flag, this->transformMatrix);
}

void Player::shoot(int a, int b, long long pos, bool flag, Matrix mat) {
    Player * self = this;
    unsigned int mask = 0x10000003;

    Array<Array<Gun *> *> *guns = self->guns;
    if (guns != 0 && self->shootingEnabled != 0 && a >= 0 &&
        (unsigned int) a < guns->size()) {
        Array<Gun *> *arr = guns->data()[a];
        if (arr != 0) {
            for (unsigned int i = 0; i < arr->size(); i++) {
                Gun *g = self->guns->data()[a]->data()[i];
                unsigned int sortIdx = g->weaponType - 6;
                if (sortIdx < 0x1d && ((1u << sortIdx) & mask) != 0 &&
                    *(int *) g->lifetimes >= 0) {
                    ((Gun *) (g))->ignite();
                } else if (g->itemIndex == b && g->timer > g->fireDelay) {
                    if (sortIdx < 0x1d && ((1u << sortIdx) & mask) != 0) {
                        ((Level *) (intptr_t) g->level)->field_69 = 1;
                    }
                    ((Gun *) (g))->shoot(mat, (int) pos, flag);
                    self->flShake = self->flShake + k_shoot_inc;
                    if (self->playShootSoundFlag != 0) {
                        Vector shotPosition = AbyssEngine::AEMath::MatrixGetPosition(mat);
                        Gun *g2 = self->guns->data()[a]->data()[i];
                        self->playShootSound(g2->itemIndex, g2->weaponType,
                                             &shotPosition,
                                             g2->field_0xb0);
                    }
                    g->timer = 0;
                    break;
                }
                arr = self->guns->data()[a];
            }
        }
    }
}

void Player::shoot(int a, long long pos, bool flag) {
    this->shoot(a, pos, flag, this->transformMatrix);
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
                if (g->timer > g->fireDelay) {
                    ((Gun *) (g))->shootAt(mat, (int) pos, self, flag);
                    self->flShake = self->flShake + k_shootAt_inc;
                    Gun *g2 = self->guns->data()[a]->data()[i];
                    g2->timer = 0;
                    if (self->playShootSoundFlag != 0 && g2->field_0x89 != 0) {
                        Vector shotPosition = AbyssEngine::AEMath::MatrixGetPosition(mat);
                        Gun *g3 = self->guns->data()[a]->data()[i];
                        self->playShootSound(g3->itemIndex, g3->weaponType,
                                             &shotPosition,
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

void Player::setHitVector(float x, float y, float z) {
    this->hitVector[0] = x;
    this->hitVector[1] = y;
    this->hitVector[2] = z;
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
    this->enginePositionVec = reinterpret_cast<void *>(static_cast<intptr_t>(unused));
    if (Globals::options[0x0f] != 0) {
        Vector pos = AbyssEngine::AEMath::MatrixGetPosition(this->transformMatrix);
        FMOD::Event *ev = ((FModSound *) Globals::sound)->updateEvent3DAttributes(
            this->engineEvent, static_cast<int>(reinterpret_cast<intptr_t>(this->enginePositionVec)), &pos, nullptr, true);
        this->engineEvent = ev;
        this->engineSoundPlaying = 1;
    }
}

void Player::PauseEngineSound() {
    FMOD::Event *event = this->engineEvent;
    if (event != 0) {
        this->enginePaused = ((FModSound *) Globals::sound)->pause(event);
    }
}

void Player::ResumeEngineSound(bool force) {
    FMOD::Event *event = this->engineEvent;
    if (event != 0 && (this->enginePaused != 0 || force)) {
        this->enginePaused = ((FModSound *) Globals::sound)->resume(event) ^ 1;
    }
}

void Player::StopEngineSound() {
    FMOD::Event *event = this->engineEvent;
    if (event != 0) {
        ((FModSound *) Globals::sound)->stop(event);
        this->engineSoundPlaying = 0;
        this->engineEvent = 0;
    }
}

// Static data members present in the original binary (defined for symbol parity).
int Player::velocity = 100;
