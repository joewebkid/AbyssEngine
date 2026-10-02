#include "game/weapons/Gun.h"
#include <new>
#include <cstdint>
#include "game/ship/Player.h"
#include "game/mission/Item.h"
#include "engine/render/Sparks.h"
#include "engine/render/AEGeometry.h"
#include "engine/core/Array.h"
#include "engine/math/Vector.h"
#include "engine/math/AEMath.h"
#include "engine/core/AERandom.h"
#include "engine/math/Transform.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/TargetFollowCamera.h"
#include "game/world/Level.h"
#include "game/ship/PlayerGasCloud.h"
#include "game/mission/Achievements.h"
#include "game/ui/Hud.h"
#include "engine/render/ParticleSystemManager.h"

using VecArray = Array<Vector>;

Gun::~Gun() noexcept(false) {
    delete[] this->lifetimes;
    this->lifetimes = 0;

    delete[] this->hitFlags;
    this->hitFlags = 0;

    delete[] this->geometries;
    this->geometries = 0;

    delete[] this->randomFlags;
    this->randomFlags = 0;

    Array<Vector *> *arr = reinterpret_cast<Array<Vector *> *>(this->wobbleOffsets);
    if (arr != 0) {
        ArrayReleaseClasses(*arr);
        Array<Vector *> *arr2 = reinterpret_cast<Array<Vector *> *>(this->wobbleOffsets);
        if (arr2 != 0) {
            delete arr2;
        }
    }
    this->wobbleOffsets = 0;

    this->hitArray.~VecArray();
    this->upArray.~VecArray();
    this->directionArray.~VecArray();
    this->positionArray.~VecArray();
}

void Gun::setFriendGun(bool v) {
    this->friendGun = v;
}

int Gun::getMagnitude() {
    return this->magnitude;
}

void *Gun::getEnemies() {
    return this->enemies;
}

void Gun::setEnemies(Array<Player *> *enemies) {
    this->enemies = enemies;
}

void Gun::setMagnitude(int v) {
    this->magnitude = v;
}

void Gun::setErrorMagnitudePercentage(int v) {
    this->errorMagnitudePercentage = (float) v;
}

void Gun::setImpact(Sparks *impact) {
    this->impact = impact;
}

void Gun::setPlayerGun(bool v) {
    this->playerGun = v;
}

uint8_t Gun::isPlayerGun() {
    return this->playerGun;
}

void Gun::setLevelCollision(bool v) {
    this->levelCollision = v;
}

void Gun::setLevel(Level *lvl) {
    this->level = (int) (intptr_t) lvl;
}

void Gun::removeAllEnemies() {
    this->enemies = 0;
}

// Android 2.0.16 rodata @0x202b04: animation geometry by item index.
static const int gunAnimationResources[233] = {
    14600, 14600, 14600, 14601, 14601, 14602, 14602, 14603, 14603, 14600, 14603, 14602,
    14601, 14601, 14601, 14601, 14604, 14604, 14604, 14605, 14605, 14605, 14606, 14606,
    14606, 14606, 14606, 14606, 14606, 14605, 14602, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 14603,
    14605, 14601, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    14600, 14600, 14600, 14603, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 14600, -1, -1, -1, 14600, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 14600, -1, -1, 14600, -1, -1, -1,
    14601, 14601, 14606, 14602, -1,
};

void Gun::setIndex(int index) {
    this->itemIndex = index;
    this->homing = (index == 0xe4) || ((unsigned) (index - 9) < 3);
    this->empDamage = (*static_cast<Array<Item *> *>(Globals::items))[index]->getAttribute(10);
    int g = gunAnimationResources[index];
    if (g >= 0) {
        unsigned count = this->count;
        this->geometries = new int[count];
        this->randomFlags = new uint8_t[count];
        for (unsigned i = 0; i < count; i = i + 1) {
            AEGeometry *geom = new AEGeometry((uint16_t) g, Globals::Canvas, false);
            this->geometries[i] = geom->transform;
            int r = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
            this->randomFlags[i] = (r == 0);
            auto *tf = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(this->geometries[i]));
            tf->SetAnimationState(AbyssEngine::AnimationMode_0, nullptr);
            delete geom;
            count = this->count;
        }
    }
}

// Android rodata @0x202ea8..0x203100: ten mounting patterns (one to ten shots).
static const short gunMountPatterns[10][30] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {270, 0, 100, -270, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {320, 0, 100, 0, 0, 100, -320, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {370, 0, 100, 170, 100, 150, -170, 100, 150, -370, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {350, 100, 200, 270, -100, 150, 0, 0, 100, -270, -100, 150, -350, 100, 200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {500, 100, 150, 500, -100, 150, 200, 0, 100, -200, 0, 100, -500, -100, 150, -500, 100, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {500, -100, 150, 400, 100, 150, 200, -50, 100, 0, 0, 150, -200, -50, 100, -400, 100, 150, -500, -100, 150, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {550, -100, 100, 550, 100, 100, 300, 0, 100, 100, 0, 150, -100, 0, 150, -300, 0, 100, -550, 100, 100, -550, -100, 100, 0, 0, 0, 0, 0, 0},
    {550, -100, 100, 550, 100, 100, 300, 0, 100, 100, 100, 150, 0, -100, 100, -100, 100, 150, -300, 0, 100, -550, 100, 100, -550, -100, 100, 0, 0, 0},
    {600, -120, 100, 600, 120, 100, 400, 0, 100, 200, 100, 150, 200, -100, 100, -200, -100, 100, -200, 100, 150, -400, 0, 100, -600, 120, 100, -600, -120, 100},
};

void Gun::setOffset(int a, int b) {
    const short *row = &gunMountPatterns[b - 1][a * 3];
    Vector local;
    local.x = (float) (int) row[0];
    local.y = (float) (int) row[1];
    local.z = (float) (int) row[2];
    local.x = this->offset.x + local.x;
    local.y = this->offset.y + local.y;
    local.z = this->offset.z + local.z;
    this->offset = local;
}

// Android 2.0.16 @0x152b7c: radial EMP/damage and hit-vector contract.
void Gun::ignite() {
    using namespace AbyssEngine::AEMath;
    if (weaponType == ITEM_SORT_NUKE)
        ++*reinterpret_cast<int *>(reinterpret_cast<char *>(Globals::status) + 0xc8);
    if (weaponType == ITEM_SORT_EMP_BOMB || weaponType == ITEM_SORT_NUKE)
        reinterpret_cast<Level *>(static_cast<intptr_t>(level))->manualSecondaryActive = 0;
    ignited = 1;
    if (enemies) {
        lastHitKIPlayer = 0;
        for (unsigned enemyIndex = 0; enemyIndex < enemies->size(); ++enemyIndex) {
            target = (*enemies)[enemyIndex];
            if (weaponType == ITEM_SORT_EMP_BOMB && target->isAsteroid()) continue;
            if (!target->isActive()) continue;
            for (unsigned i = 0; i < count; ++i) {
                Vector difference = reinterpret_cast<Vector *>(positions)[i];
                basePos = difference;
                Vector targetPosition = target->getPosition();
                targetDir = targetPosition;
                targetDir -= basePos;
                int distance = static_cast<int>(VectorLength(targetDir));
                if (magnitude > distance) {
                    hitFlags[i] = 1;
                    reinterpret_cast<Vector *>(hitPositions)[i] = basePos;
                    float force;
                    if (weaponType == ITEM_SORT_MINE)
                        force = static_cast<float>(10000 - distance) / 10000.0f;
                    else force = static_cast<float>(magnitude - distance) / static_cast<float>(magnitude);
                    float scale = 1.0f;
                    if (!(force > 1.0f)) {
                        scale = force;
                        if (force < 0.0f) scale = 0.0f;
                    }
                    int emp = (*static_cast<Array<Item *> *>(Globals::items))[itemIndex]->getAttribute(10);
                    unsigned kind = weaponType - 11;
                    if ((kind <= 31 && ((1u << kind) & 0x80800001u)) || weaponType == ITEM_SORT_NUKE) {
                        if (emp != -979797979) target->setEmpForce(scale * emp);
                        target->setBombForce(scale);
                        if (target->isAsteroid()) scale *= 0.6f;
                        if (damage >= 1) {
                            target->damage(static_cast<int>(scale * damage), friendGun, itemIndex);
                            if (target->isGasCloud() && weaponType == ITEM_SORT_IONIZING_MISSILE)
                                static_cast<PlayerGasCloud *>(target->getKIPlayer())->explode(itemIndex,
                                    reinterpret_cast<Vector *>(positions)[i], static_cast<float>(magnitude));
                            if (itemIndex == 179 && target->isAsteroid()) {
                                if (scale * damage >= target->getHitpoints() && !static_cast<Achievements *>(Globals::achievements)->hasMedal(44, 1)) {
                                    auto *statusBytes = reinterpret_cast<char *>(Globals::status);
                                    int &progress = *reinterpret_cast<int *>(statusBytes + 0x144);
                                    ++progress;
                                    if (progress >= static_cast<Achievements *>(Globals::achievements)->getValue(44, 1)) {
                                        auto *ego = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer();
                                        auto *hud = reinterpret_cast<Hud *>(static_cast<intptr_t>(ego->getHUD()));
                                        hud->hudEventMedal(44, 100);
                                        reinterpret_cast<char *>(Globals::status)[0x148] = 1;
                                    }
                                }
                            }
                        }
                        if (emp >= 1) target->damageEmp(static_cast<int>(scale * emp), friendGun);
                        if (weaponType == ITEM_SORT_SHOCK_BLAST && target->getKIPlayer())
                            target->getKIPlayer()->initPush(basePos, magnitude);
                    } else {
                        target->damageEmp(static_cast<int>(scale * emp), friendGun);
                    }
                    Vector normalized = VectorNormalize(targetDir);
                    targetDir = normalized;
                    target->setHitVector(targetDir.x, targetDir.y, targetDir.z);
                    Vector projectileSpeed = reinterpret_cast<Vector *>(velocities)[i];
                    targetDir = projectileSpeed;
                    Vector normalizedSpeed = VectorNormalize(targetDir);
                    targetDir = normalizedSpeed;
                    lastHitKIPlayer = static_cast<int>(reinterpret_cast<intptr_t>(target->getKIPlayer()));
                    lifetimes[i] = -1;
                }
                if (weaponType != ITEM_SORT_MINE) lifetimes[i] = -1;
            }
        }
        if (weaponType == ITEM_SORT_EMP_BOMB) lifetimes[0] = -1;
    }
    if (impact) impact->explode(basePos);
}

// Android 2.0.16 Gun::render @0x15395c: camera-facing animation transforms.
void Gun::render() {
    if (impact) impact->render();
    if (geometries) {
        for (unsigned i = 0; i < count; ++i) {
            auto *transform = static_cast<AbyssEngine::Transform *>(
                Globals::Canvas->TransformGetTransform(geometries[i]));
            if (transform->animating) {
                auto *canvas = Globals::Canvas;
                unsigned current = canvas->CameraGetCurrent();
                // Native copies all 60 bytes without constructing another Matrix.
                alignas(Matrix) unsigned char cameraBytes[sizeof(Matrix)];
                memcpy(cameraBytes, canvas->CameraGetLocal(current), sizeof(Matrix));
                auto &camera = *reinterpret_cast<Matrix *>(cameraBytes);
                const auto *local = static_cast<const Matrix *>(
                    Globals::Canvas->TransformGetLocal(geometries[i]));
                targetDir = AbyssEngine::AEMath::MatrixGetPosition(*local);
                AbyssEngine::AEMath::MatrixSetTranslation(camera, targetDir.x, targetDir.y, targetDir.z);
                Globals::Canvas->TransformSetLocal(geometries[i], camera);
                Globals::Canvas->DrawTransform(geometries[i], nullptr);
            }
        }
    }
}

Gun::Gun(int kind, int p2, int count, int p4, int p5, int p6, float p7, Vector dir, Vector vel)
    : positionArray(), directionArray(), upArray(), hitArray() {
    this->offset.x = 0;
    this->offset.y = 0;
    this->offset.z = 0;
    this->field_0x90 = 0;
    this->field_0x94 = 0;
    this->field_0x98 = 0;
    this->level = 0;
    this->lifetimes = 0;
    this->enemies = 0;
    this->impact = 0;
    this->geometries = 0;
    this->randomFlags = 0;
    this->slotIndex = kind;
    this->damage = p2;
    this->field_0x50 = p7;
    this->targetDir.z = 0;
    this->velocity.x = 0;
    this->velocity.y = 0;
    this->velocity.z = 0;
    this->field_0xd0 = 0;
    this->field_0xd4 = 0;
    this->targetDir.x = 0;
    this->targetDir.y = 0;
    this->basePos.x = 0;
    this->basePos.y = 0;
    this->basePos.z = 0;
    this->field_0xcc = 0;
    this->ammoCount = 0;
    this->field_0x78 = 0;
    this->playerGun = 0;
    this->field_0xa8 = 0;

    this->offset = dir;
    // ARM literal @0x152124, independent of the projectile speed parameter.
    vel *= 0.15339000523090363f;
    this->velocity = vel;
    this->initialLifetime = p5;
    this->fireDelay = p6;
    this->timer = p6;
    *reinterpret_cast<unsigned *>(&_pad_0xa0) = 0;
    this->ignited = 0;
    this->delayActive = 0;
    this->ammoCount = p4;
    this->field_0x78 = p4 << 1;
    this->lifetimes = new int[static_cast<unsigned>(count)];
    this->hitFlags = new uint8_t[count];
    Array<Vector *> *arr = new Array<Vector *>();
    this->wobbleOffsets = arr;
    ArraySetLength(count, this->positionArray);
    ArraySetLength(count, this->directionArray);
    ArraySetLength(count, this->upArray);
    ArraySetLength(count, this->hitArray);
    ArraySetLength(count, *this->wobbleOffsets);
    for (int i = 0; i < count; i = i + 1) {
        reinterpret_cast<Vector *>(this->positions)[i].x = 50000.0f;
        this->lifetimes[i] = -1000000;
        this->hitFlags[i] = 0;
        this->wobbleOffsets->data_[i] = 0;
    }
    this->impact = 0;
    this->field_0x54 = 0;
    this->active = 0;
    this->hitSmall = 0;
    this->enemies = 0;
    this->itemIndex = -1;
    this->weaponType = static_cast<ItemSort>(-1);
    this->useCustomRadius = 0;
    this->levelCollision = 1;
    this->friendGun = 0;
    this->errorMagnitudePercentage = 0;
    this->field_0x89 = 0;
    this->owner = 0;
    this->field_0xb0 = 0;
    this->empDamage = 0;
    this->field_0xa4 = 0;
}

bool Gun::shoot(Matrix m, int n, bool b) {
    return this->shootAt(m, n, 0, b);
}

void Gun::setEnemy(Player *enemy) {
    // Android @0x1523c2 destroys the previous container, not its Player objects.
    delete enemies;
    enemies = nullptr;
    enemies = new Array<Player *>();
    ArrayAdd(enemy, *enemies);
}

static const float kZOffset = 100.0f;

void Gun::setOffset(Vector *v) {
    float z = v->z;
    Vector local;
    local.x = v->x;
    local.y = v->y;
    local.z = z + kZOffset;
    this->offset = local;
}

// Android 2.0.16 @0x153754; expired projectiles park at 50000 in each axis.
void Gun::update(int dt) {
    timer += dt;
    if (delayActive) {
        int t = delayTimer + dt;
        delayTimer = t;
        if (t >= fireDelay) delayActive = 0;
    }
    if (impact) impact->update(dt);
    if (geometries) {
        for (unsigned i = 0; i < count; ++i) {
            auto *transform = static_cast<AbyssEngine::Transform *>(
                Globals::Canvas->TransformGetTransform(geometries[i]));
            transform->Update(dt, false);
        }
    }
    if (active && weaponType != ITEM_SORT_SENTRY_GUN) {
        calcCharacterCollision();
        float fdt = static_cast<float>(dt);
        for (unsigned i = 0; i < count; ++i) {
            int remaining = lifetimes[i];
            int threshold = 0;
            if (static_cast<unsigned>(weaponType - ITEM_SORT_ROCKET) < 2 ||
                weaponType == ITEM_SORT_CLUSTER_MISSILE) threshold = -2000;
            if (remaining > threshold) {
                lifetimes[i] = remaining - dt;
                if (weaponType == ITEM_SORT_MINE) {
                    Vector step = reinterpret_cast<Vector *>(velocities)[i] * fdt;
                    float fade = static_cast<float>(initialLifetime - lifetimes[i]) / -500.0f + 1.0f;
                    if (fade < 0.0f) fade = 0.0f;
                    Vector scaled = step * fade;
                    reinterpret_cast<Vector *>(positions)[i] += scaled;
                } else {
                    Vector scaled = reinterpret_cast<Vector *>(velocities)[i] * fdt;
                    reinterpret_cast<Vector *>(positions)[i] += scaled;
                }
                remaining = lifetimes[i];
                if (remaining <= 0) {
                    unsigned kind = weaponType - 6;
                    if (kind <= 0x1c && ((1u << kind) & 0x10000003u)) {
                        ignite();
                        remaining = lifetimes[i];
                    }
                    if (remaining <= -2000 &&
                        (static_cast<unsigned>(weaponType - ITEM_SORT_ROCKET) < 2 ||
                         weaponType == ITEM_SORT_CLUSTER_MISSILE))
                        *reinterpret_cast<int *>(reinterpret_cast<char *>(Globals::status) + 0x12c) = 0;
                }
                if (weaponType == ITEM_SORT_SHOCK_BLAST) ignite();
            } else {
                auto &position = reinterpret_cast<Vector *>(positions)[i];
                position.x = position.y = position.z = 50000.0f;
                auto &speed = reinterpret_cast<Vector *>(velocities)[i];
                speed.x = speed.y = speed.z = 0.0f;
            }
        }
    }
}

void Gun::translate(const Vector &v) {
    for (unsigned i = 0; i < this->count; i = i + 1) {
        reinterpret_cast<Vector *>(this->positions)[i] += v;
    }
}

// Android 2.0.16 @0x152428: bool result is consumed by Player::shoot.
// The integer and final bool arguments are forwarded by shoot but unused here.
bool Gun::shootAt(Matrix m, int, Player *p, bool) {
    using namespace AbyssEngine::AEMath;
    owner = p;
    Item *equipment = nullptr;
    if (!friendGun) {
        equipment = (*Globals::status->getShip()->getEquipment())[slotIndex];
        if (!equipment || !equipment->getAmount()) return false;
    }
    if (homing) {
        if (lifetimes[0] >= 1) return weaponType == ITEM_SORT_CLUSTER_MISSILE;
        timer = 0;
        delayTimer = 0;
        active = 1;
        hitSmall = 1;
        if (playerGun) delayActive = 1;
        fireIndex = -1;
        int nearest = 99999999;
        for (unsigned i = 0; i < enemies->size(); ++i) {
            Player *enemy = (*enemies)[i];
            if (enemy->getKIPlayer() &&
                enemy->getKIPlayer()->field_0x73 &&
                enemy->isActive() && !enemy->isDead()) {
                Vector difference = enemy->getPosition() - reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->getPosition();
                int distance = static_cast<int>(VectorLength(difference));
                if (nearest > distance) { fireIndex = i; nearest = distance; }
            }
        }
        lifetimes[0] = initialLifetime;
        if (fireIndex <= -1) {
            *reinterpret_cast<Vector *>(&field_0x90) = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->geometry->getDirection();
            Vector pos = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->getPosition();
            Vector dir = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->geometry->getDirection();
            *reinterpret_cast<Vector *>(positions) = pos + dir * 30000.0f;
            *reinterpret_cast<Vector *>(velocities) = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->geometry->getDirection();
            field_0x8c = 30000;
        } else {
            Player *enemy = (*enemies)[fireIndex];
            Vector difference = enemy->getPosition() - reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->getPosition();
            *reinterpret_cast<Vector *>(&field_0x90) = VectorNormalize(difference);
            *reinterpret_cast<Vector *>(positions) = enemy->getPosition();
            *reinterpret_cast<Vector *>(velocities) = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->geometry->getDirection();
            field_0x8c = nearest;
        }
        return true;
    }
    bool fired = false;
    for (unsigned i = 0; i < count; ++i) {
        int threshold = 0;
        if (static_cast<unsigned>(weaponType - ITEM_SORT_ROCKET) < 2 ||
            weaponType == ITEM_SORT_CLUSTER_MISSILE) threshold = -2000;
        if (lifetimes[i] > threshold ||
            (weaponType == ITEM_SORT_SENTRY_GUN && reinterpret_cast<Level *>(static_cast<intptr_t>(level))->field_6c > 2)) continue;
        timer = 0;
        delayTimer = 0;
        active = 1;
        hitSmall = 1;
        *reinterpret_cast<unsigned *>(&_pad_0xa0) = i;
        reinterpret_cast<Vector *>(positions)[i] = MatrixGetPosition(m);
        Vector zero{0, 0, 0};
        if (offset != zero) {
            if (weaponType != ITEM_SORT_SHOCK_BLAST) {
                if (!field_0xa5 || field_0xa4_b0) {
                    targetDir = offset;
                    if (field_0xa4_b0) {
                        if (field_0xa6) targetDir.x = -targetDir.x;
                        field_0xa6 ^= 1;
                    }
                    Vector displacement = MatrixRotateVector(m, targetDir);
                    reinterpret_cast<Vector *>(positions)[i] += displacement;
                }
                if (field_0xa5 && field_0xa6) {
                    targetDir = offset;
                    if (field_0xa7) targetDir.y = -targetDir.y;
                    field_0xa7 ^= 1;
                    Vector displacement = MatrixRotateVector(m, targetDir);
                    reinterpret_cast<Vector *>(positions)[i] += displacement;
                }
            }
        }
        Vector launch{0, 0, 0};
        if (weaponType == ITEM_SORT_MINE) {
            Vector direction = MatrixGetDir(m);
            Vector up = MatrixGetUp(m);
            launch = direction + up;
            if (!(*wobbleOffsets)[i]) (*wobbleOffsets)[i] = new Vector{0, 0, 0};
            (*wobbleOffsets)[i]->x = static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100) / 50.0f;
            (*wobbleOffsets)[i]->y = static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100) / 50.0f;
            (*wobbleOffsets)[i]->z = static_cast<float>(static_cast<AERandom *>(Globals::rnd)->nextInt(200) - 100) / 50.0f;
        } else {
            launch = MatrixGetDir(m);
        }
        Vector rotatedLaunch = MatrixRotateVector(m, velocity);
        launch += rotatedLaunch;
        reinterpret_cast<Vector *>(velocities)[i] = launch;
        reinterpret_cast<Vector *>(upVectors)[i] = MatrixGetUp(m);
        float error = errorMagnitudePercentage;
        if (error > 0.0f) {
            int random = static_cast<AERandom *>(Globals::rnd)->nextInt(static_cast<int>(error));
            float half = error * 0.005f;
            reinterpret_cast<Vector *>(velocities)[i].x += random * 0.01f - half;
            reinterpret_cast<Vector *>(velocities)[i].y += static_cast<AERandom *>(Globals::rnd)->nextInt(static_cast<int>(errorMagnitudePercentage)) * 0.01f - half;
            reinterpret_cast<Vector *>(velocities)[i].z += static_cast<AERandom *>(Globals::rnd)->nextInt(static_cast<int>(errorMagnitudePercentage)) * 0.01f - half;
        }
        Vector normalized = VectorNormalize(reinterpret_cast<Vector *>(velocities)[i]);
        reinterpret_cast<Vector *>(velocities)[i] = normalized;
        reinterpret_cast<Vector *>(velocities)[i] *= pitchRate;
        lifetimes[i] = weaponType == ITEM_SORT_SENTRY_GUN ? 0 : initialLifetime;
        bool keepAmmo = true;
        if (!friendGun && equipment && ammoCount >= 1 && equipment->getType() == 1) {
            keepAmmo = false;
            if (itemIndex == 179)
                *reinterpret_cast<int *>(reinterpret_cast<char *>(Globals::status) + 0x144) = 0;
        }
        delayActive = 1;
        if (playerGun && hitSmall)
            reinterpret_cast<TargetFollowCamera *>(static_cast<intptr_t>(reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->getTargetFollowCamera()))->hitSmall();
        if (weaponType != ITEM_SORT_CLUSTER_MISSILE && !keepAmmo) {
            --ammoCount;
            equipment->changeAmount(-1);
            if (!ammoCount) Globals::status->getShip()->freeSlot(equipment);
            return true;
        }
        fired = true;
        if (weaponType != ITEM_SORT_CLUSTER_MISSILE) return true;
    }
    if (weaponType != ITEM_SORT_CLUSTER_MISSILE) return false;
    if (fired) {
        --ammoCount;
        if (equipment) {
            equipment->changeAmount(-1);
            if (!ammoCount) Globals::status->getShip()->freeSlot(equipment);
        }
    }
    return true;
}

// Android 2.0.16 @0x152f90. Native collision is an axis predicate plus optional
// KIPlayer::outerCollide dispatch, not a replacement spherical collision model.
void Gun::calcCharacterCollision() {
    using namespace AbyssEngine::AEMath;
    if (!enemies) return;
    int kind = weaponType;
    unsigned radial = static_cast<unsigned>(kind - 6) < 2 || kind == 42 || kind == 34;
    unsigned mine = kind == 11;
    unsigned scatter = kind == 25;
    unsigned particle = radial || static_cast<unsigned>(kind - 4) < 2 || kind == 40;
    unsigned detonate = radial | mine;
    lastHitKIPlayer = 0;
    for (unsigned enemyIndex = 0; enemyIndex < enemies->size(); ++enemyIndex) {
        Player *enemy = (*enemies)[enemyIndex];
        if (mine && (!enemy->enemyFlagsLo ||
            (enemy->getKIPlayer() && enemy->getKIPlayer()->field_0x41))) continue;
        if (!enemy->isActive() || enemy->getHitpoints() < 1) continue;
        for (unsigned i = 0; i < count; ++i) {
            Vector position = reinterpret_cast<Vector *>(positions)[i];
            Vector speed = reinterpret_cast<Vector *>(velocities)[i];
            Vector center = MatrixGetPosition(enemy->transformMatrix);
            int radius = useCustomRadius ? customRadius : enemy->radius;
            if (mine) radius *= 5;
            else if (scatter) {
                Vector difference = center - reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer()->getPosition();
                int distance = static_cast<int>(VectorLength(difference));
                float factor;
                if (enemy->getKIPlayer() && enemy->getKIPlayer()->_pad_0x71)
                    factor = radius * 0.5f;
                else if (distance < 20001) {
                    factor = radius * 1.5f;
                    if (distance > 10000) factor = static_cast<float>(radius * 2);
                } else factor = static_cast<float>(radius * 3);
                radius = static_cast<int>(factor);
            }
            KIPlayer *ki = enemy->getKIPlayer();
            if (!reinterpret_cast<unsigned char *>(&enemy->empDisabled)[1] && (!ki || !reinterpret_cast<unsigned char *>(&ki->field_0x8c)[0])) continue;
            bool rejectBox = false;
            if (ki && ki->collisionEnabled) {
                if (ki->outerCollide(position)) goto collided;
                rejectBox = true;
            }
            {
                float extent = static_cast<float>(radius);
                float negativeExtent = static_cast<float>(-radius);
                float x = (center.x - position.x) + speed.x;
                if (!(x < extent) || !(x > negativeExtent)) continue;
                float y = (center.y - position.y) + speed.y;
                if (!(y < extent) || !(y > negativeExtent)) continue;
                float z = (center.z - position.z) + speed.z;
                if (rejectBox | !(z < extent) | !(z > negativeExtent)) continue;
            }
collided:
            if (enemy->isAsteroid()) {
                if (mine) continue;
                enemy->setBombForce(0.0f);
                if (particle) {
                    enemy->damage(9999, false, itemIndex);
                    bool otherMedal = true;
                    if (!static_cast<Achievements *>(Globals::achievements)->hasMedal(41, 1) &&
                        (static_cast<unsigned>(weaponType - 4) < 2 || weaponType == 40 || weaponType == 34)) {
                        otherMedal = false;
                        char *status = reinterpret_cast<char *>(Globals::status);
                        int &progress = *reinterpret_cast<int *>(status + 0x12c);
                        ++progress;
                        if (progress >= static_cast<Achievements *>(Globals::achievements)->getValue(41, 1)) {
                            auto *egoPlayer = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer();
                            reinterpret_cast<Hud *>(static_cast<intptr_t>(egoPlayer->getHUD()))->hudEventMedal(41, 100);
                            reinterpret_cast<char *>(Globals::status)[0x130] = 1;
                        }
                    }
                    if (otherMedal && itemIndex == 179 && !static_cast<Achievements *>(Globals::achievements)->hasMedal(44, 1)) {
                        char *status = reinterpret_cast<char *>(Globals::status);
                        int &progress = *reinterpret_cast<int *>(status + 0x144);
                        ++progress;
                        if (progress >= static_cast<Achievements *>(Globals::achievements)->getValue(44, 1)) {
                            auto *egoPlayer = reinterpret_cast<Level *>(static_cast<intptr_t>(level))->getPlayer();
                            reinterpret_cast<Hud *>(static_cast<intptr_t>(egoPlayer->getHUD()))->hudEventMedal(44, 100);
                            reinterpret_cast<char *>(Globals::status)[0x148] = 1;
                        }
                    }
                    return;
                }
            } else if (mine) {
                float extent = static_cast<float>(enemy->radius);
                float negativeExtent = static_cast<float>(-enemy->radius);
                float x = (center.x - position.x) + speed.x;
                float y = (center.y - position.y) + speed.y;
                float z = (center.z - position.z) + speed.z;
                if (x < extent && x > negativeExtent && y < extent && y > negativeExtent && z < extent && z > negativeExtent) {
                    ignite();
                    return;
                }
                Vector difference = center - position;
                Vector normalized = VectorNormalize(difference);
                reinterpret_cast<Vector *>(velocities)[i] = normalized;
                Vector movement = reinterpret_cast<Vector *>(velocities)[i] * pitchRate;
                Vector scaledMovement = movement * 100.0f;
                reinterpret_cast<Vector *>(positions)[i] += scaledMovement;
                return;
            }
            if (detonate) { ignite(); return; }
            if (empDamage != -979797979) enemy->damageEmp(empDamage, friendGun);
            int amount = damage;
            if (owner && !owner->enemyFlagsLo) {
                if (reinterpret_cast<unsigned char *>(&enemy->empDisabled)[1]) amount = static_cast<int>(damage * 0.2f);
            } else if (reinterpret_cast<unsigned char *>(&enemy->empDisabled)[1]) {
                auto *lvl = reinterpret_cast<Level *>(static_cast<intptr_t>(level));
                if (lvl->getPlayer() && lvl->getPlayer()->isDockedToDockingPoint())
                    amount = static_cast<int>((damage * 3.0f) * 0.25f);
            }
            enemy->damage(amount, friendGun, itemIndex);
            reinterpret_cast<unsigned char *>(&enemy->field_54)[0] = 1;
            if (playerGun) reinterpret_cast<unsigned char *>(&reinterpret_cast<Level *>(static_cast<intptr_t>(level))->field_30)[0] = 1;
            enemy->setHitVector(-speed.x, -speed.y, -speed.z);
            lifetimes[i] = -1000000;
            lastHitKIPlayer = static_cast<int>(reinterpret_cast<intptr_t>(enemy->getKIPlayer()));
            if (particle) {
                auto *particleLevel = reinterpret_cast<Level *>(static_cast<intptr_t>(level));
                particleLevel->field_74->emitManual(particleLevel->field_3c, position, 0, speed, -1.0f);
            } else if (geometries) {
                auto *transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(geometries[i]));
                transform->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(3), nullptr);
                transform = static_cast<AbyssEngine::Transform *>(Globals::Canvas->TransformGetTransform(geometries[i]));
                transform->SetAnimationState(static_cast<AbyssEngine::AnimationMode>(1), nullptr);
                auto *canvas = Globals::Canvas;
                unsigned current = canvas->CameraGetCurrent();
                alignas(Matrix) unsigned char cameraBytes[sizeof(Matrix)];
                memcpy(cameraBytes, canvas->CameraGetLocal(current), sizeof(Matrix));
                auto &camera = *reinterpret_cast<Matrix *>(cameraBytes);
                MatrixSetTranslation(camera, position.x, position.y, position.z);
                Globals::Canvas->TransformSetLocal(geometries[i], camera);
            }
            if (impact) {
                if (impact->isRocket()) reinterpret_cast<unsigned char *>(&enemy->field_54)[1] = 1;
                if (impact) impact->explode(position);
            }
            if (scatter) {
                hitFlags[i] = 1;
                reinterpret_cast<Vector *>(hitPositions)[i] = position;
                for (unsigned k = 0; k < enemies->size(); ++k) {
                    Player *nearby = (*enemies)[k];
                    Vector difference = nearby->getPosition();
                    targetDir = difference;
                    targetDir -= position;
                    float distance = VectorLength(targetDir);
                    if (magnitude > static_cast<int>(distance)) {
                        float scale = 1.0f;
                        float force = static_cast<float>(magnitude - static_cast<int>(distance)) / magnitude;
                        if (!(force > 1.0f)) {
                            scale = force;
                            if (force < 0.0f) scale = 0.0f;
                        }
                        nearby->damage(static_cast<int>(scale * damage), friendGun, itemIndex);
                        if (empDamage >= 1) nearby->damageEmp(static_cast<int>(scale * empDamage), friendGun);
                    }
                }
            }
        }
    }
}

void Gun::calcLevelCollision() {
}
