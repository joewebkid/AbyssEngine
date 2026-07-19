#include "game/ship/PlayerTurret.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/world/Level.h"
#include "engine/render/ParticleSystemManager.h"
#include "engine/math/Transform.h"
#include "game/mission/Explosion.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Player.h"
#include "game/world/Standing.h"
#include "engine/render/PaintCanvas.h"
#include "game/mission/Status.h"


namespace AbyssEngine {
    namespace AEMath {
        Vector MatrixGetDir(const Matrix &matrix);

        Vector VectorNormalize(const Vector &v);

        float VectorLength(const Vector &v);

        Vector MatrixRotateVector(const Matrix &matrix, const Vector &vector);

        Vector MatrixInverseTransformVector(const Matrix &matrix, const Vector &vector);

        Vector operator+(const Vector &, const Vector &);

        Vector operator-(const Vector &, const Vector &);

        Vector operator*(const Vector &, float);

        Matrix operator*(const Matrix &, const Matrix &);
    }

    namespace AERandom {
        int nextInt(int rng, int max);
    }
}

static FModSound **g_turretSound = nullptr;
static int g_turretRandom = 0;

void PlayerTurret::setTurretRange(int range) {
    this->turretRange = range;
}

void PlayerTurret::handleSentryGun(int delta) {
    this->pickEnemyTimer += delta;
    this->pickEnemy();
    Player *enemy = this->currentEnemy;
    if (enemy != nullptr && enemy->field_5e == 0) {
        AEGeometry *geometry = this->geometry;
        this->handleRotation(delta, geometry, geometry);
    }
}

void PlayerTurret::setHost(KIPlayer *host, const Vector &offset) {
    this->turretHost = host;
    this->hostOffset = offset;
}

void PlayerTurret::render() {
    AEGeometry *visible = this->crateGeometry;
    if (visible != nullptr) {
        visible->render();
    }
    int state = this->state;
    if (state == 3) {
        this->explosion->render();
        state = this->state;
    }

    if ((uint32_t)(state - 3) >= 2) {
        this->KIPlayer::render();
    }
}

int PlayerTurret::collide(float x, float y, float z) {
    return 0;
}

int PlayerTurret::outerCollide(float x, float y, float z) {
    return 0;
}

void PlayerTurret::handleTurret(int delta) {
    this->pickEnemyTimer += delta;
    this->pickEnemy();
    Player *enemy = this->currentEnemy;
    if (enemy != nullptr && enemy->field_5e == 0) {
        this->handleRotation(delta, this->helperGeometry, this->turretGeometry);
    }
}

void PlayerTurret::revive() {
    this->player->reset();
    this->crateGeometry = nullptr;
    this->state = 1;
    this->reviveFlag = 0;
    this->explosion->reset();
    AEGeometry *geometry = this->parentGeometry;
    this->spawnInvulnTimer = 0;
    this->visibleFlag = 1;
    if (geometry == nullptr) {
        geometry = this->geometry;
    }
    geometry->setVisible(true);
}

void PlayerTurret::setPosition(const Vector &position) {
    this->geometry->setPosition(position);
    this->posX = position.x;
    this->posY = position.y;
    this->posZ = position.z;
}

void PlayerTurret::reset() {
    this->KIPlayer::reset();
    this->state = 0;
}

void PlayerTurret::setLevel(Level *level) {
    this->KIPlayer::setLevel(level);
    ParticleSystemManager *manager = (ParticleSystemManager *) this->level->field_74;
    int system = manager->addSystem(&this->geometry->getReferenceMatrix(), ParticleSettings::ParticleSet_9, false);
    this->particleSystemId = system;
    manager->enableSystemEmit(system, false);
}

KIPlayer *PlayerTurret::getHost() {
    return this->turretHost;
}

void PlayerTurret::setScaling(float scale) {
    this->helperGeometry->setScaling(scale);
}

void PlayerTurret::handleRotation(int delta, AEGeometry *mainGeometry, AEGeometry *turretGeometry) {
    Matrix matrix;
    matrix = *(const Matrix *) this->currentEnemy->transform;
    Vector enemyPos = this->currentEnemy->getPosition();

    Vector dir = MatrixGetDir(matrix);
    Vector normal = VectorNormalize(dir);
    Vector scaled = normal * 3000.0f;
    this->aimPoint = enemyPos + scaled;

    if (!this->isSentryGun) {
        Matrix base = this->geometry->getMatrix();
        Matrix turret = turretGeometry->getMatrix();
        Matrix main = mainGeometry->getMatrix();
        matrix = (base * turret) * main;
    } else {
        matrix = this->geometry->getMatrix();
    }

    Vector local = MatrixInverseTransformVector(matrix, this->aimPoint);
    Vector aim = VectorNormalize(local);

    bool ready = false;
    float yaw;
    if (aim.x > 0.0f) {
        yaw = (float) delta;
        turretGeometry->rotate(0.0f, yaw * 0.001f * 0.25f, 0.0f);
    } else if (aim.x < -0.05f) {
        yaw = (float) -delta;
        turretGeometry->rotate(0.0f, yaw * 0.001f * 0.25f, 0.0f);
    } else {
        ready = true;
    }

    if (aim.y > 0.0f) {
        if (!this->isSentryGun && this->rotationAccum < 100) {
            this->previousEnemy = this->currentEnemy;
            this->pickEnemyTimer += delta;
            return;
        }
        float step = (float) this->frameDelta;
        float next = (float) this->rotationAccum - step;
        this->rotationAccum = (int) next;
        mainGeometry->rotate(next, 0.0f, step * 0.001f * 0.25f);
        this->previousEnemy = nullptr;
        return;
    }
    if (aim.y < -0.05f) {
        if (!this->isSentryGun && this->rotationAccum > 99) {
            this->previousEnemy = this->currentEnemy;
            this->pickEnemyTimer += delta;
            return;
        }
        float step = (float) this->frameDelta;
        float next = (float) this->rotationAccum + step;
        this->rotationAccum = (int) next;
        mainGeometry->rotate(next, 0.0f, step * 0.001f * -0.25f);
        this->previousEnemy = nullptr;
        return;
    }

    if (ready) {
        this->player->shoot(0, delta, delta >> 31, 0);
        AbyssEngine::Transform *transform =
                (AbyssEngine::Transform *) PaintCanvas::gCanvas->TransformGetTransform(turretGeometry->transform);
        transform->Update(delta, delta >> 31);
    }
}

void PlayerTurret::update(int delta) {
    this->frameDelta = delta;

    Player *player = this->player;
    if (!player->isActive()) {
        return;
    }

    if (this->isSentryGun && this->spawnInvulnTimer < 3000) {
        player->setVulnerable(false);
        int time = this->spawnInvulnTimer + delta;
        this->spawnInvulnTimer = time;
        if (time > 2999) {
            player->setVulnerable(true);
        }
    }

    if (this->turretHost != nullptr) {
        const Matrix &hostMatrix = *(const Matrix *) this->turretHost->player->transform;
        this->hostWorldOffset = MatrixRotateVector(hostMatrix, this->hostOffset);
        this->geometry->setMatrix(hostMatrix);
        this->geometry->translate(this->hostWorldOffset);
    }

    Matrix geomMatrix = this->geometry->getMatrix();
    *(Matrix *) player->transform = geomMatrix;
    this->cachedPosition = this->geometry->getPosition();

    int hp = player->getHitpoints();
    int state = this->state;
    if (hp < 1 && (uint32_t)(state - 3) >= 2) {
        this->explosionTimer = 0;
        this->state = 3;
        (*g_turretSound)->play(0x16, nullptr, nullptr, 0.0f);
        Vector zero = {0.0f, 0.0f, 0.0f};
        ParticleSystemManager *manager = (ParticleSystemManager *) this->level->field_74;
        manager->emitManual(this->level->field_3c, this->cachedPosition, 0, 0.0f);
        manager->enableSystemEmit(this->particleSystemId, true);
        this->explosion->start(this->cachedPosition, zero);

        int random = AbyssEngine::AERandom::nextInt(g_turretRandom, 100);
        if (random < 0) {
            this->cargo = new Array<int>();
            ArrayAdd(99, *(this->cargo));
            ArrayAdd(AbyssEngine::AERandom::nextInt(g_turretRandom, 10) + 1, *(this->cargo));
            this->createCrate(3);
        } else {
            Player *levelPlayer = (Player *) (intptr_t) this->level->getPlayer();
            if (levelPlayer != nullptr && levelPlayer->kiPlayer != nullptr &&
                levelPlayer->kiPlayer->wingmanTarget == (KIPlayer *) this) {
                levelPlayer->kiPlayer->wingmanTarget = nullptr;
            }
        }
        state = this->state;
    }

    if (state == 4) {
        return;
    }
    if (state == 3) {
        this->explosionTimer += delta;
        this->explosion->update(delta, static_cast<TargetFollowCamera *>(nullptr));
        if (this->explosionTimer > 4500) {
            ((ParticleSystemManager *) this->level->field_74)->enableSystemEmit(this->particleSystemId, false);
            this->explosionTimer = 0;
            this->state = 4;
            player->setActive(false);
            if (this->isSentryGun) {
                this->level->field_6c -= 1;
            }
        }
        return;
    }

    int faction = this->shipGroup;
    if ((faction & 0xfffffffeU) == 8) {
        player->enemyFlags = 1;
    } else {
        Standing *standing = (Standing *) (intptr_t) Status::gStatus->getStanding();
        bool enemy = standing->isEnemy(faction);
        bool friendly = false;
        if ((faction & 0xfffffffeU) != 8) {
            friendly = ((Standing *) (intptr_t) Status::gStatus->getStanding())->isFriend(faction);
        }
        player->enemyFlags = (uint16_t)((friendly ? 0x100 : 0) | (enemy ? 1 : 0));
    }
    if (player->turnedEnemy() != 0) {
        player->enemyFlags = 1;
    }
    if (player->isAlwaysFriend() != 0) {
        player->enemyFlags = 0x100;
    }

    if (this->isSentryGun) {
        handleSentryGun(delta);
    } else if (this->turretEnabled && this->turretGeometry != nullptr) {
        handleTurret(delta);
    }
}

void PlayerTurret::pickEnemy() {
    if (this->pickEnemyTimer <= 3000) {
        return;
    }

    int bestRange = this->turretRange;
    this->pickEnemyTimer = 0;
    this->currentEnemy = nullptr;

    Array<Player *> *enemies = this->player->getEnemies();
    if (enemies == nullptr) {
        return;
    }

    const Vector &position = this->cachedPosition;
    for (uint32_t i = 0; i < enemies->size(); ++i) {
        Player *enemy = (*enemies)[i];
        if (enemy->isDead() || !enemy->isActive()) {
            continue;
        }

        bool accepted = false;
        if ((enemy->empDisabled >> 8) != 0 && (this->player->enemyFlags & 0xff) != 0) {
            accepted = true;
        } else if (!this->isSentryGun) {
            if (enemy->getKIPlayer() != nullptr) {
                accepted = true;
            }
        } else if ((enemy->enemyFlags & 0xff) != 0) {
            accepted = true;
        }

        if (!accepted) {
            continue;
        }

        Vector enemyPos = enemy->getPosition();
        Vector diff = position - enemyPos;
        int distance = (int) VectorLength(diff);
        if (distance < bestRange) {
            Player *current = this->currentEnemy;
            if (current == nullptr || current != this->previousEnemy) {
                bestRange = distance;
                this->currentEnemy = enemy;
            }
        }
    }
}

PlayerTurret::~PlayerTurret() {
    if (this->explosion != nullptr) {
        delete this->explosion;
    }
    this->explosion = nullptr;

    if (this->baseGeometry != nullptr) {
        delete this->baseGeometry;
    }
    this->baseGeometry = nullptr;

    if (this->turretGeometry != nullptr) {
        delete this->turretGeometry;
    }
    this->turretGeometry = nullptr;

    if (this->helperGeometry != nullptr) {
        delete this->helperGeometry;
    }
    this->helperGeometry = nullptr;
}

PlayerTurret::PlayerTurret(int mesh, Player *player, AEGeometry *geometry, float x, float y, float z)
    : KIPlayer(0, 0, player, geometry, x, y, z, false) {
    this->turretEnabled = true;
    this->field_0x3e = 1;
    this->isSentryGun = false;
    this->baseGeometry = nullptr;
    this->turretGeometry = nullptr;
    this->spawnInvulnTimer = 0;
    this->turretHost = nullptr;
    this->hostOffset = Vector{0.0f, 0.0f, 0.0f};
    this->turretRange = 50000;

    this->baseGeometry = new AEGeometry((uint16_t) mesh, PaintCanvas::gCanvas, false);

    if (mesh == 0x381b) {
        AEGeometry *turret = new AEGeometry((uint16_t) 0x381c, PaintCanvas::gCanvas, false);
        this->turretGeometry = turret;
        turret->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
        turret->setPosition(Vector{0.0f, 0.0f, 0.0f});
    } else if (mesh == 0x1a76) {
        AEGeometry *turret = new AEGeometry((uint16_t) 0x1a77, PaintCanvas::gCanvas, false);
        this->turretGeometry = turret;
        turret->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
        turret->setPosition(Vector{0.0f, 0.0f, 0.0f});
    } else if (mesh == 0x1a74) {
        AEGeometry *turret = new AEGeometry((uint16_t) 0x1a75, PaintCanvas::gCanvas, false);
        this->turretGeometry = turret;
        turret->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
        turret->setPosition(Vector{0.0f, 0.0f, 0.0f});
    }

    this->helperGeometry = new AEGeometry(PaintCanvas::gCanvas);
    AbyssEngine::Transform *helperTransform =
            (AbyssEngine::Transform *) PaintCanvas::gCanvas->TransformGetTransform(this->helperGeometry->transform);
    helperTransform->flags = 0;

    this->setPosition(Vector{x, y, z});

    if (mesh == 0x381b) {
        this->baseGeometry->rotate(0.0f, 0.0f, 0.0f);
        this->turretGeometry->rotate(0.0f, 0.0f, 0.0f);
        this->helperGeometry->addChild(this->turretGeometry->transform);
    } else if ((mesh | 2U) == 0x1a76) {
        this->helperGeometry->addChild(this->turretGeometry->transform);
    } else if ((uint32_t)(mesh - 0x49c0) < 3) {
        this->isSentryGun = true;
        uint16_t childMesh = 0x49c8;
        if (mesh == 0x49c1) {
            childMesh = 0x49c7;
        }
        if (mesh == 0x49c0) {
            childMesh = 0x49c6;
        }
        AEGeometry *child = new AEGeometry(childMesh, PaintCanvas::gCanvas, false);
        geometry->addChild(child->transform);
        delete child;
        geometry->setScaling(0.5f);
    }

    this->helperGeometry->addChild(this->baseGeometry->transform);
    if (!this->isSentryGun) {
        geometry->addChild(this->helperGeometry->transform);
    }

    this->explosion = new Explosion(0);
    this->explosion->addFireStreaks();

    this->currentEnemy = nullptr;
    this->previousEnemy = nullptr;
    this->explosionTimer = 0;
    this->pickEnemyTimer = 0;
    this->rotationAccum = 0;
    this->particleSystemId = 0;
}
