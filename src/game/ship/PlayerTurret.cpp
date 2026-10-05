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
#include "game/core/Globals.h"
#include "engine/core/AERandom.h"


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

}


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
    this->reviveFlag() = 0;
    this->setActive(true);
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
    this->setActive(true);
}

void PlayerTurret::setLevel(Level *level) {
    this->KIPlayer::setLevel(level);
    ParticleSystemManager *manager = (ParticleSystemManager *) this->level->field_74;
    int system = manager->addSystem(&this->geometry->getReferenceMatrix(), ParticleSettings::ParticleSet_9, false);
    this->particleSystemId = system;
    this->level->field_74->enableSystemEmit(system, false);
}

KIPlayer *PlayerTurret::getHost() {
    return this->turretHost;
}

void PlayerTurret::setScaling(float scale) {
    this->helperGeometry->setScaling(scale, scale, scale);
}

void PlayerTurret::handleRotation(int delta, AEGeometry *mainGeometry, AEGeometry *turretGeometry) {
    Matrix matrix = this->currentEnemy->transformMatrix;
    Vector enemyPos = this->currentEnemy->getPosition();

    Vector dir = MatrixGetDir(matrix);
    Vector normal = VectorNormalize(dir);
    // Android 2.0.16 literal at 0x157ba4, not a decompiler float guess.
    Vector scaled = normal * 1500.0f;
    this->aimPoint() = enemyPos + scaled;

    if (!this->isSentryGun()) {
        matrix = (this->geometry->getMatrix() * mainGeometry->getMatrix()) * turretGeometry->getMatrix();
    } else {
        matrix = this->geometry->getMatrix();
    }

    Vector local = MatrixInverseTransformVector(matrix, this->aimPoint());
    Vector aim = VectorNormalize(local);

    bool ready = false;
    float yaw;
    if (aim.x > 0.05f) {
        yaw = (float) delta;
        mainGeometry->rotate(0.0f, yaw * 0.000244140625f * 6.2831854820251465f, 0.0f);
    } else if (aim.x < -0.05f) {
        yaw = (float) -delta;
        mainGeometry->rotate(0.0f, yaw * 0.000244140625f * 6.2831854820251465f, 0.0f);
    } else {
        ready = true;
    }

    if (aim.y > 0.05f) {
        if (!this->isSentryGun() && this->rotationAccum < -599) {
            this->previousEnemy = this->currentEnemy;
            this->pickEnemyTimer += delta;
            return;
        }
        float step = (float) this->frameDelta();
        float next = (float) this->rotationAccum - step;
        this->rotationAccum = (int) next;
        turretGeometry->rotate(step * 0.000244140625f * -6.2831854820251465f, 0.0f, 0.0f);
        this->previousEnemy = nullptr;
        return;
    }
    if (aim.y < -0.05f) {
        if (!this->isSentryGun() && this->rotationAccum > 99) {
            this->previousEnemy = this->currentEnemy;
            this->pickEnemyTimer += delta;
            return;
        }
        float step = (float) this->frameDelta();
        float next = (float) this->rotationAccum + step;
        this->rotationAccum = (int) next;
        turretGeometry->rotate(step * 0.000244140625f * 6.2831854820251465f, 0.0f, 0.0f);
        this->previousEnemy = nullptr;
        return;
    }

    if (ready) {
        this->player->shoot(0, static_cast<long long>(delta), false, matrix);
        AbyssEngine::Transform *transform =
                (AbyssEngine::Transform *) Globals::Canvas->TransformGetTransform(turretGeometry->transform);
        transform->Update(delta, false);
    }
}

void PlayerTurret::update(int delta) {
    this->frameDelta() = delta;

    if (!this->player->isActive()) {
        return;
    }

    if (this->isSentryGun() && this->spawnInvulnTimer < 3000) {
        this->player->setVulnerable(false);
        int time = this->spawnInvulnTimer + delta;
        this->spawnInvulnTimer = time;
        if (time > 2999) {
            this->player->setVulnerable(true);
        }
    }

    if (this->turretHost != nullptr) {
        Matrix hostMatrix = this->turretHost->player->transformMatrix;
        this->hostWorldOffset() = MatrixRotateVector(hostMatrix, this->hostOffset);
        this->geometry->setMatrix(hostMatrix);
        this->geometry->translate(this->hostWorldOffset());
    }

    this->player->transformMatrix = this->geometry->getMatrix();
    this->cachedPosition() = this->geometry->getPosition();

    int state;
    if (this->player->getHitpoints() > 0) {
        goto check_state;
    }
    state = this->state;
    if ((uint32_t)(state - 3) >= 2) {
        this->explosionTimer = 0;
        this->state = 3;
        Globals::sound->play(0x16, nullptr, nullptr, 0.0f);
        Vector zero = {0.0f, 0.0f, 0.0f};
        ParticleSystemManager *manager = (ParticleSystemManager *) this->level->field_74;
        manager->emitManual(this->level->field_3c, this->cachedPosition(), 0, zero, -1.0f);
        this->level->field_74->enableSystemEmit(this->particleSystemId, true);
        this->explosion->start(this->cachedPosition(), Vector{0.0f, 0.0f, 0.0f});

        int random = static_cast<AERandom *>(Globals::rnd)->nextInt(100);
        if (random <= -1) {
            reinterpret_cast<uint8_t *>(&this->hasCargo)[0] = 1;
            this->cargo = new Array<int>();
            ArrayAdd(99, *(this->cargo));
            ArrayAdd(static_cast<AERandom *>(Globals::rnd)->nextInt(10) + 1, *(this->cargo));
            this->createCrate(3);
            reinterpret_cast<uint8_t *>(&this->hasCargo)[0] = 1;
        } else {
            // Android Level::getPlayer returns PlayerEgo; +0x14 points to a
            // descriptor whose +0x1c target is cleared (not KIPlayer+0xe8).
            struct TargetDescriptor { uint8_t prefix[0x1c]; KIPlayer *target; };
            if (this->level->getPlayer() != nullptr
                    && this->level->getPlayer()->field_0x14 != nullptr
                    && static_cast<TargetDescriptor *>(this->level->getPlayer()->field_0x14)->target == this) {
                static_cast<TargetDescriptor *>(this->level->getPlayer()->field_0x14)->target = nullptr;
            }
        }
    }
check_state:
    state = this->state;
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
            this->player->setActive(false);
            if (this->isSentryGun()) {
                this->level->field_6c -= 1;
            }
        }
        return;
    }

    Player *body;
    uint8_t friendly;
    if ((this->shipGroup & 0xfffffffeU) == 8) {
        body = this->player;
        body->enemyFlagsLo = 1;
    } else {
        auto standing = reinterpret_cast<Standing *>(static_cast<intptr_t>(Globals::status->getStanding()));
        uint8_t enemy = standing->isEnemy(this->shipGroup);
        body = this->player;
        int faction = this->shipGroup;
        body->enemyFlagsLo = enemy;
        if ((faction & 0xfffffffeU) != 8) {
            auto currentStanding = reinterpret_cast<Standing *>(static_cast<intptr_t>(Globals::status->getStanding()));
            friendly = currentStanding->isFriend(this->shipGroup);
            body = this->player;
            goto set_friend;
        }
    }
    friendly = 0;
set_friend:
    body->carriesFriendCargoFlag = friendly;
    int turned = body->turnedEnemy();
    body = this->player;
    if (turned != 0) {
        body->enemyFlags = 1;
    }
    if (body->isAlwaysFriend() != 0) {
        this->player->enemyFlags = 0x100;
    }

    if (this->isSentryGun()) {
        handleSentryGun(delta);
    } else if (this->turretEnabled() && this->turretGeometry != nullptr) {
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

    const Vector &position = this->cachedPosition();
    for (uint32_t i = 0; i < enemies->size(); ++i) {
        Player *enemy = (*enemies)[i];
        if (enemy->isDead() || !enemy->isActive()) {
            continue;
        }

        bool accepted = false;
        if ((reinterpret_cast<const uint8_t *>(&enemy->empDisabled)[1]) != 0 && (this->player->enemyFlags & 0xff) != 0) {
            accepted = true;
        } else if (!this->isSentryGun()) {
            if (enemy->getKIPlayer() != nullptr) {
                int other = enemy->getKIPlayer()->shipGroup;
                int own = this->shipGroup;
                accepted = (other == 8 && own != 8) || (other != 8 && own == 8)
                        || (other == 10 && own != 10) || (other != 10 && own == 10)
                        || (other == 9 && own != 9) || (other != 9 && own == 9)
                        || (other == 0 && own == 1) || (other == 1 && own == 0)
                        || (other == 3 && own == 2) || (other == 2 && own == 3);
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
    : KIPlayer(mesh, -1, player, geometry, x, y, z, false), hostOffset{0.0f, 0.0f, 0.0f} {
    this->turretEnabled() = true;
    this->field_0x3e = 1;
    this->baseGeometry = nullptr;
    this->turretGeometry = nullptr;
    this->spawnInvulnTimer = 0;
    this->turretHost = nullptr;
    this->turretRange = 50000;

    this->baseGeometry = new AEGeometry((uint16_t) mesh, Globals::Canvas, false);

    if (mesh == 0x381b) {
        AEGeometry *turret = new AEGeometry((uint16_t) 0x381c, Globals::Canvas, false);
        this->turretGeometry = turret;
        turret->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
        this->turretGeometry->setPosition(Vector{0.0f, 565.0f, -528.0f});
    } else if (mesh == 0x1a76) {
        AEGeometry *turret = new AEGeometry((uint16_t) 0x1a77, Globals::Canvas, false);
        this->turretGeometry = turret;
        turret->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
        this->turretGeometry->setPosition(Vector{0.0f, 60.0f, 0.0f});
    } else if (mesh == 0x1a74) {
        AEGeometry *turret = new AEGeometry((uint16_t) 0x1a75, Globals::Canvas, false);
        this->turretGeometry = turret;
        turret->setRotationOrder(AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
        this->turretGeometry->setPosition(Vector{0.0f, 60.0f, 0.0f});
    }

    this->helperGeometry = new AEGeometry(Globals::Canvas);
    AbyssEngine::Transform *helperTransform =
            (AbyssEngine::Transform *) Globals::Canvas->TransformGetTransform(this->helperGeometry->transform);
    helperTransform->boundingRadius = 1000000.0f;

    this->setPosition(Vector{x, y, z});

    if (mesh == 0x381b) {
        this->baseGeometry->rotate(0.0f, 3.1415927410125732f, 0.0f);
        this->turretGeometry->rotate(0.0f, 3.1415927410125732f, 0.0f);
        this->helperGeometry->addChild(this->turretGeometry->transform);
    } else if ((mesh | 2U) == 0x1a76) {
        this->helperGeometry->addChild(this->turretGeometry->transform);
    } else if ((uint32_t)(mesh - 0x49c0) < 3) {
        this->isSentryGun() = true;
        uint16_t childMesh = 0x49c8;
        if (mesh == 0x49c1) {
            childMesh = 0x49c7;
        }
        if (mesh == 0x49c0) {
            childMesh = 0x49c6;
        }
        AEGeometry *child = new AEGeometry(childMesh, Globals::Canvas, false);
        geometry->addChild(child->transform);
        delete child;
        geometry->setScaling(0.5f);
    }

    this->helperGeometry->addChild(this->baseGeometry->transform);
    if (!this->isSentryGun()) {
        geometry->addChild(this->helperGeometry->transform);
    }

    this->explosion = new Explosion(0);
    this->explosion->addFireStreaks();

    this->currentEnemy = nullptr;
    this->previousEnemy = nullptr;
    this->explosionTimer = 0;
    this->pickEnemyTimer = 0;
    this->rotationAccum = 0;
    this->particleSystemId = -1;
}
