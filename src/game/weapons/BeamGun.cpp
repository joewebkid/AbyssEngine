#include "game/weapons/BeamGun.h"
#include "game/world/Level.h"
#include "game/world/LevelScript.h"
#include "engine/math/Transform.h"
#include "game/weapons/Gun.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"
#include "engine/render/AEGeometry.h"

using ::AbyssEngine::Transform;


namespace AbyssEngine {
    namespace AEMath {
        Vector operator+(const Vector &lhs, const Vector &rhs);

        Matrix operator*(const Matrix &lhs, const Matrix &rhs);
    }
}

Transform *BeamGun_canvasTransform(PaintCanvas *canvas, uint32_t transformId);

void MatrixRotateVector(Vector *out, const Matrix *matrix, const Vector *vector);

void MatrixGetDir(Vector *out, const Matrix *matrix);


static void **BeamGun_canvas = nullptr;

static int32_t BeamGun_secondaryMeshes[256] = {};


BeamGun::BeamGun(int owner, Gun *gun, int meshKind, Level *level) {
    int type = meshKind;
    int kind = type - 9;
    if (type == 0xe4)
        kind = 2;

    this->field_0x4 = 0;
    this->gun = gun;
    this->level = level;
    this->owner = owner;
    this->meshKind = kind;

    PaintCanvas *canvas = (PaintCanvas *) *BeamGun_canvas;
    uint16_t primaryMesh = (uint16_t)(kind + 0x3795);
    if (type == 0xe4)
        primaryMesh = 0x4a92;
    this->primaryGeometry = new AEGeometry(primaryMesh, canvas, false);

    AEGeometry *secondary = nullptr;
    int mesh = gun->isPlayerGun();
    if (mesh != 0 && (mesh = BeamGun_secondaryMeshes[gun->itemIndex]) >= 0) {
        this->hasSecondary = gun->weaponType != ITEM_SORT_MINE;
        if (gun->weaponType != ITEM_SORT_MINE)
            secondary = new AEGeometry((uint16_t) mesh, canvas, false);
    } else {
        this->hasSecondary = 0;
    }

    this->secondaryVisible = 0;
    this->secondaryGeometry = secondary;
}

BeamGun::~BeamGun() {
    if (this->primaryGeometry != nullptr)
        delete this->primaryGeometry;
    this->primaryGeometry = nullptr;

    if (this->secondaryGeometry != nullptr)
        delete this->secondaryGeometry;
    this->secondaryGeometry = nullptr;
}

void BeamGun::render() {
    if (this->gun->active == 0)
        return;

    this->primaryGeometry->render();

    if (this->secondaryVisible != 0 && this->secondaryGeometry != nullptr)
        this->secondaryGeometry->render();
}

void BeamGun::update(int elapsed) {
    Vector back;
    Vector rotated;
    Matrix playerMatrix;
    Vector transformed;
    Vector position;

    this->gun->update(elapsed);

    Gun *gun = this->gun;
    if (gun->active == 0) {
        this->primaryGeometry->setVisible(false);
        return;
    }

    PaintCanvas *canvas = (PaintCanvas *) *BeamGun_canvas;

    if (gun->hitSmall != 0) {
        AEGeometry *geometry = this->primaryGeometry;
        Transform *transform = BeamGun_canvasTransform(canvas, geometry->transform);
        transform->SetAnimationState((AbyssEngine::AnimationMode) 3, 0);
        transform = BeamGun_canvasTransform(canvas, geometry->transform);
        transform->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
        this->gun->hitSmall = 0;
    }

    Transform *primaryTransform = BeamGun_canvasTransform(canvas, this->primaryGeometry->transform);
    primaryTransform->Update((long long) elapsed, false);

    gun = this->gun;
    this->primaryGeometry->setScaling(1.0f);

    Vector up;
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;
    this->primaryGeometry->setDirection(*(Vector *) &gun->field_0x90, up);

    PlayerEgo *player = (PlayerEgo *) this->level->getPlayer();
    position = player->getPosition();

    Vector zero;
    zero.x = 0.0f;
    zero.y = 0.0f;
    zero.z = 0.0f;
    if (memcmp(&this->gun->offset, &zero, sizeof(float) * 3) != 0) {
        player = (PlayerEgo *) this->level->getPlayer();
        Matrix &firstMatrix = player->geometry->getMatrix();
        player = (PlayerEgo *) this->level->getPlayer();
        Matrix &secondMatrix = player->field_0x4->getMatrix();
        playerMatrix = firstMatrix * secondMatrix;

        gun = this->gun;
        back.x = 0.0f;
        back.y = 0.0f;
        back.z = -100.0f;
        rotated = gun->offset + back;
        MatrixRotateVector(&transformed, &playerMatrix, &rotated);
        position = position + transformed;
    }

    this->primaryGeometry->setPosition(position);
    this->primaryGeometry->setVisible(true);

    if (this->hasSecondary != 0) {
        gun = this->gun;
        if (gun->delayActive == 0) {
            AEGeometry *secondary = this->secondaryGeometry;
            Transform *t = BeamGun_canvasTransform(canvas, secondary->transform);
            t->SetAnimationState((AbyssEngine::AnimationMode) 0, 0);
            t = BeamGun_canvasTransform(canvas, secondary->transform);
            t->SetAnimationState((AbyssEngine::AnimationMode) 3, 0);
            t = BeamGun_canvasTransform(canvas, secondary->transform);
            t->SetAnimationState((AbyssEngine::AnimationMode) 1, 0);
        } else {
            player = (PlayerEgo *) this->level->getPlayer();
            Vector playerPos = player->getPosition();
            playerMatrix = *(Matrix *) &playerPos;

            gun = this->gun;
            transformed = gun->offset;
            transformed.z = gun->offset.z + -100.0f;

            MatrixRotateVector(&rotated, (Matrix *) ((Player *) player->player)->transform, &transformed);
            *(Vector *) &playerMatrix = *(Vector *) &playerMatrix + rotated;
            this->secondaryGeometry->setPosition(*(Vector *) &playerMatrix);

            AEGeometry *secondary = this->secondaryGeometry;
            Transform *t = BeamGun_canvasTransform(canvas, secondary->transform);
            t->Update((long long) elapsed, false);

            MatrixGetDir(&rotated, (Matrix *) ((Player *) player->player)->transform);
            back.x = 0.0f;
            back.y = 1.0f;
            back.z = 0.0f;
            this->secondaryGeometry->setDirection(rotated, back);
        }
    }

    this->secondaryVisible = this->gun->delayActive;
}

void BeamGun::replaceGun(unsigned int /*mesh*/, int /*unused*/) {
}

void BeamGun::translate(const Vector & /*v*/) {
}

void BeamGun::setEnemies(Array<Player *> *enemies) {
    this->gun->setEnemies(enemies);
}

void BeamGun::setEnemy(Player *enemy) {
    this->gun->setEnemy(enemy);
}
