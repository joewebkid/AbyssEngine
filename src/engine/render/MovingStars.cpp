#include "engine/render/MovingStars.h"
#include "engine/core/AERandom.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/Globals.h"


MovingStars::~MovingStars() {
    if (this->billboardIds) delete[] this->billboardIds;
    this->billboardIds = 0;
    if (this->transformHandles) delete[] this->transformHandles;
    this->transformHandles = 0;
    if (this->lifeArray) delete[] this->lifeArray;
    this->lifeArray = 0;
    if (this->velocityArray) delete[] this->velocityArray;
    this->velocityArray = 0;
}

MovingStars::MovingStars() {
    this->billboardIds = 0;
    this->transformHandles = 0;
    this->textureHandle = 0;
    this->lifeArray = 0;
    this->velocityArray = 0;

    this->billboardIds = new uint32_t[50];
    this->transformHandles = new uint32_t[50];
    this->velocityArray = new int[50];
    this->lifeArray = new int[50];
    for (int i = 0; i != 0x32; i = i + 1)
        this->lifeArray[i] = -1;

    AERandom *rng = (AERandom *) Globals::rnd;
    Globals *globals = (Globals *) Globals::globals;
    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;

    for (int j = 0; j != 50; j = j + 1) {
        rng->nextInt(4);
        uint32_t bb = globals->createBillBoard(70, 500, 0.87695f, 0.24805f, 0.93555f, 0.18945f, 0x4e54);
        this->billboardIds[j] = bb;
        canvas->TransformCreate(this->transformHandles[j]);
        canvas->TransformAddMeshId(this->transformHandles[j], this->billboardIds[j]);
        Matrix *local = (Matrix *) canvas->TransformGetLocal(this->transformHandles[j]);
        AbyssEngine::AEMath::MatrixSetTranslation(*local, 100000.0f, 100000.0f, 100000.0f);
    }

    canvas->TextureCreate(0x2711, this->textureHandle, 0);
    this->tickAccumulator = 0;
    this->animResetFlag = 0;
    this->animActiveFlag = 0;
}

static void MovingStars_setPoints(PaintCanvas *canvas, uint32_t mesh,
                                  float neg, float pos, float zPos, float zNeg) {
    canvas->MeshSetPoint(mesh, 0, neg, 0.0f, zPos);
    canvas->MeshSetPoint(mesh, 1, neg, 0.0f, zNeg);
    canvas->MeshSetPoint(mesh, 2, pos, 0.0f, zPos);
    canvas->MeshSetPoint(mesh, 3, pos, 0.0f, zNeg);
    canvas->MeshSetPoint(mesh, 4, 0.0f, neg, zPos);
    canvas->MeshSetPoint(mesh, 5, 0.0f, neg, zNeg);
    canvas->MeshSetPoint(mesh, 6, 0.0f, pos, zPos);
    canvas->MeshSetPoint(mesh, 7, 0.0f, pos, zNeg);
    canvas->MeshSetPoint(mesh, 8, neg, pos, 0.0f);
    canvas->MeshSetPoint(mesh, 9, neg, neg, 0.0f);
    canvas->MeshSetPoint(mesh, 10, pos, pos, 0.0f);
    canvas->MeshSetPoint(mesh, 11, pos, neg, 0.0f);
}

void MovingStars::update(int delta, Matrix m, bool flag, float speedFactor) {
    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
    AERandom *rng = (AERandom *) Globals::rnd;

    this->tickAccumulator += delta;

    if (flag) {
        int zStretch = (int) (speedFactor * 4500.0f);
        float zNeg = (float) (-500 - zStretch);
        float zPos = (float) (zStretch + 500);
        int widthStretch = (int) (speedFactor * -20.0f);
        float pos = (float) (widthStretch + 70);
        float neg = (float) (-70 - widthStretch);
        int velocity = (int) (speedFactor * 1000.0f) + 1000;

        for (int i = 0; i != 50; i += 1) {
            MovingStars_setPoints(canvas, this->billboardIds[i], neg, pos, zPos, zNeg);
            this->velocityArray[i] = velocity;
        }
        this->animResetFlag = 1;
        this->animActiveFlag = 1;
    } else {
        this->animResetFlag = 0;
        if (this->animActiveFlag != 0) {
            this->animActiveFlag = 0;
            for (int i = 0; i != 50; i += 1) {
                MovingStars_setPoints(canvas, this->billboardIds[i], -70.0f, 70.0f, 500.0f, -500.0f);
            }
        }
    }

    bool respawned = false;
    for (int i = 0; i != 50; i += 1) {
        int life = this->lifeArray[i];
        if ((life > 0 || respawned) || (!flag && (int) this->tickAccumulator < 41)) {
            this->lifeArray[i] = life - delta;

            Matrix local = *(Matrix *) canvas->TransformGetLocal(this->transformHandles[i]);
            float velocity = (float) this->velocityArray[i];
            local.m[11] -= local.m[10] * velocity;
            local.m[7] -= local.m[6] * velocity;
            local.m[3] -= local.m[2] * velocity;
            canvas->TransformSetLocal(this->transformHandles[i], local);
        } else {
            this->tickAccumulator = 0;

            Vector spawn;
            spawn.x = (float) (rng->nextInt(20000) - 10000);
            spawn.y = (float) (rng->nextInt(18000) - 9000);
            spawn.z = 20000.0f;
            spawn = AbyssEngine::AEMath::MatrixTransformVector(m, spawn);

            canvas->TransformSetLocal(this->transformHandles[i], m);
            Matrix *local = (Matrix *) canvas->TransformGetLocal(this->transformHandles[i]);
            AbyssEngine::AEMath::MatrixSetTranslation(*local, spawn.x, spawn.y, spawn.z);

            if (flag) {
                this->velocityArray[i] = (int) (speedFactor * 1000.0f) + 1000;
                this->lifeArray[i] = 500;
            } else {
                this->velocityArray[i] = rng->nextInt(500) + 500;
                this->lifeArray[i] = 2000;
            }
            respawned = true;
        }
    }
}

void MovingStars::translate(const Vector &v) {
    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
    for (int i = 0; i != 50; i += 1) {
        Matrix *local = (Matrix *) canvas->TransformGetLocal(this->transformHandles[i]);
        Vector pos = AbyssEngine::AEMath::MatrixGetPosition(*local);
        pos += v;
        AbyssEngine::AEMath::MatrixSetTranslation(*local, pos.x, pos.y, pos.z);
    }
}

void MovingStars::render() {
    PaintCanvas *canvas = (PaintCanvas *) Globals::Canvas;
    canvas->SetTexture(this->textureHandle, 0xffffffff);
    canvas->SetBlendMode(AbyssEngine::BlendMode_1);
    for (int i = 0; i != 50; i += 1) {
        canvas->DrawTransform(this->transformHandles[i], nullptr);
    }
}
