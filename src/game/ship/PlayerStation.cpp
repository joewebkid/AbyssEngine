#include "game/ship/PlayerStation.h"

#include "game/world/SolarSystem.h"
#include "engine/render/AEGeometry.h"
#include "engine/math/BoundingVolume.h"
#include "engine/math/BoundingAAB.h"
#include "engine/math/BoundingSphere.h"
#include "engine/file/FileRead.h"
#include "game/mission/Status.h"
#include "engine/math/Transform.h"
#include "game/ship/Player.h"
#include "game/world/Station.h"
#include "engine/render/PaintCanvas.h"

static inline PaintCanvas *paintCanvas() {
    return PaintCanvas::gCanvas;
}

typedef Array<BoundingVolume *> BoundingVolumeList;

void PlayerStation::setVisible(bool visible) {
    this->visibleFlag = visible;
}

PlayerStation::~PlayerStation() {
    if (this->rootGeometry != nullptr) {
        delete this->rootGeometry;
    }
    this->rootGeometry = nullptr;

    if (this->secondGeometry != nullptr) {
        delete this->secondGeometry;
    }
    this->secondGeometry = nullptr;

    BoundingVolumeList *volumes = (BoundingVolumeList *) this->boundingVolumes;
    if (volumes != nullptr) {
        ArrayReleaseClasses<BoundingVolume *>(*volumes);
        delete volumes;
    }
    this->boundingVolumes = nullptr;
}

void PlayerStation::setPosition(const Vector &position) {
    this->posX = position.x;
    this->posY = position.y;
    this->posZ = position.z;
    this->rootGeometry->setPosition(position);

    BoundingVolumeList *volumes = (BoundingVolumeList *) this->boundingVolumes;
    if (volumes != nullptr) {
        for (uint32_t i = 0; i < volumes->size(); ++i) {
            (*volumes)[i]->update(position.x, position.y, position.z);
            volumes = (BoundingVolumeList *) this->boundingVolumes;
        }
    }
}

Vector PlayerStation::projectCollisionOnSurface(const Vector &position) {
    Vector result;
    reinterpret_cast<BoundingVolume *>(&result)->staticProjectCollisionOnSurface(
        position, (BoundingVolumeList *) this->boundingVolumes);
    return result;
}

void *PlayerStation::getRoot() {
    return this->rootGeometry;
}

Vector PlayerStation::getProjectionVector(const Vector &position) {
    BoundingVolumeList *volumes = (BoundingVolumeList *) this->boundingVolumes;
    if (volumes != nullptr) {
        BoundingVolume *volume = (*volumes)[this->collisionIndex];
        return volume->getProjectionVector(position);
    }

    Vector result = {0.0f, 0.0f, 0.0f};
    return result;
}

void PlayerStation::render() {
    if (!this->visibleFlag) {
        return;
    }
    this->rootGeometry->render();
}

int PlayerStation::outerCollide(const Vector &position) {
    return this->outerCollide(position.x, position.y, position.z);
}

Vector PlayerStation::getPosition() {
    return this->rootGeometry->getPosition();
}

int PlayerStation::collide(float x, float y, float z) {
    return this->outerCollide(x, y, z);
}

void PlayerStation::update(int delta) {
    bool active = (int32_t) this->rootGeometry->childTransform != -1;
    int type = active ? this->stationIndex : 0;
    if (!active || type == 0x65) {
        return;
    }

    if (Status::gStatus->inAlienOrbit()) {
        return;
    }

    PaintCanvas *canvas = paintCanvas();
    long long delta64 = (long long) delta;
    AEGeometry *root = this->rootGeometry;

    ((AbyssEngine::Transform *) canvas->TransformGetTransform(root->childTransform))->Update(delta64, false);

    type = this->stationIndex;
    if (type == 100) {
        ((AbyssEngine::Transform *) canvas->TransformGetTransform(this->meshTransform))->Update(delta64, false);
    } else if (type == 0x6c) {
        ((AbyssEngine::Transform *) canvas->TransformGetTransform(this->animTransform0))->Update(delta64, false);
        ((AbyssEngine::Transform *) canvas->TransformGetTransform(this->animTransform1))->Update(delta64, false);
        ((AbyssEngine::Transform *) canvas->TransformGetTransform(this->animTransform2))->Update(delta64, false);
        ((AbyssEngine::Transform *) canvas->TransformGetTransform(this->animTransform3))->Update(delta64, false);
    }
}

PlayerStation::PlayerStation(Station *station)
    : PlayerStaticFar(-1, nullptr, 0.0f, 0.0f, 0.0f) {
    this->field_0x158 = 0;
    this->field_0x15c = 0;
    this->field_0x160 = 0;
    this->player->setRadius(15000);
    this->posX = 0;
    this->posY = 0;
    this->posZ = 0;
    this->boundingVolumes = nullptr;
    this->field_0x25 = 0;
    this->player->setMaxHitpoints(0x0161eb02);
    this->rootGeometry = nullptr;
    this->secondGeometry = nullptr;
    this->collisionRadius = 0;

    int stationIndex = station->getIndex();
    this->stationIndex = stationIndex;

    PaintCanvas *canvas = paintCanvas();
    Status *status = Status::gStatus;

    Array<int> *collision;
    {
        FileRead reader;
        collision = reader.loadStationCollision(stationIndex);
    }

    bool alienOrbit = status->inAlienOrbit();

    if ((uint32_t)(stationIndex - 0x6d) > 2 && collision == nullptr) {
        if (!alienOrbit) {
            this->rootGeometry = new AEGeometry((uint16_t) 0x4034, canvas, false);
            AEGeometry child(0x4037, canvas, false);
            this->rootGeometry->addChild(child.transform);
            AEGeometry child2(0x403a, canvas, false);
            this->rootGeometry->addChild(child2.transform);
            FileRead reader;
            collision = reader.loadStationCollision(0x3e8);
        } else if (status->dlc1Won()) {
            this->rootGeometry = new AEGeometry((uint16_t) 0x4220, canvas, false);
            uint32_t t0, t1;
            canvas->TransformCreate(t0);
            canvas->TransformAddMesh(t0, 0x4221, true);
            this->rootGeometry->addChild(t0);
            t1 = 0xffffffff;
            canvas->TransformCreate(t1);
            canvas->TransformAddMesh(t1, 0x4222, true);
            this->rootGeometry->addChild(t1);
            FileRead reader;
            collision = reader.loadStationCollision(0x3eb);
            AEGeometry *root = this->rootGeometry;
            AbyssEngine::Transform *t;
            t = (AbyssEngine::Transform *) canvas->TransformGetTransform(root->transform);
            t->Update(t->animationLength, false);
            t = (AbyssEngine::Transform *) canvas->TransformGetTransform(root->childTransform);
            t->Update(t->animationLength, false);
            t = (AbyssEngine::Transform *) canvas->TransformGetTransform(root->parentTransform);
            t->Update(t->animationLength, false);
        } else {
            this->rootGeometry = new AEGeometry((uint16_t) 0x403b, canvas, false);
            AEGeometry child(0x403e, canvas, false);
            this->rootGeometry->addChild(child.transform);
            AEGeometry child2(0x4041, canvas, false);
            this->rootGeometry->addChild(child2.transform);
            FileRead reader;
            collision = reader.loadStationCollision(0x3e9);
        }
    }

    if (this->rootGeometry == nullptr) {
        if ((uint32_t)(stationIndex - 0x6d) < 3) {
            this->rootGeometry = new AEGeometry((uint16_t) 0x5254, canvas, false);
        } else {
            this->rootGeometry =
                    new AEGeometry((uint16_t)(stationIndex + 21000), canvas, false);
        }

        SolarSystem *system = (SolarSystem *) (long) status->getSystem();
        uint32_t race = system == nullptr ? 9u : (uint32_t) system->getRace();
        stationIndex = this->stationIndex;
        if (stationIndex == 0x65) {
            this->rootGeometry = new AEGeometry((uint16_t) 0x4220, canvas, false);
            uint32_t t0, t1;
            canvas->TransformCreate(t0);
            canvas->TransformAddMesh(t0, 0x4221, true);
            this->rootGeometry->addChild(t0);
            t1 = 0xffffffff;
            canvas->TransformCreate(t1);
            canvas->TransformAddMesh(t1, 0x4222, true);
            this->rootGeometry->addChild(t1);
            if (status->getCurrentCampaignMission() == 0x9d &&
                status->getStation()->getIndex() == 0x70) {
                uint32_t m0 = 0xffffffff, m1, m2;
                canvas->TransformCreate(m0);
                canvas->TransformAddMesh(m0, 0x4950, true);
                this->rootGeometry->addChild(m0);
                m1 = 0xffffffff;
                canvas->TransformCreate(m1);
                canvas->TransformAddMesh(m1, 0x4952, true);
                this->rootGeometry->addChild(m1);
                m2 = 0xffffffff;
                canvas->TransformCreate(m2);
                canvas->TransformAddMesh(m2, 0x4951, true);
                this->rootGeometry->addChild(m2);
            }
        } else if ((uint32_t)(stationIndex - 0x6d) < 2 ||
                   (stationIndex == 0x6f && status->getCurrentCampaignMission() > 0x5e)) {
            this->rootGeometry = new AEGeometry((uint16_t) 0x4953, canvas, false);
            AEGeometry child(0x4954, canvas, false);
            this->rootGeometry->addChild(child.transform);
            AEGeometry child2(0x4955, canvas, false);
            this->rootGeometry->addChild(child2.transform);
            AEGeometry child3(0x4956, canvas, false);
            this->rootGeometry->addChild(child3.transform);
            if (collision != nullptr) {
                delete collision;
            }
            FileRead reader;
            collision = reader.loadStaticCollision(stationIndex);
        } else if (stationIndex == 100) {
            int mission = status->getCurrentCampaignMission();
            bool dlcWon = status->dlc1Won();
            uint16_t rootMesh = dlcWon ? 0x3823 : 0x4223;
            if (mission == 0x50) {
                rootMesh = 0x381f;
            }
            uint16_t mesh1 = (mission == 0x50 || dlcWon) ? 0x422a : 0x4228;
            uint16_t mesh2 = mission == 0x50 ? 0x3820 : (dlcWon ? 0x3824 : 0x4224);
            uint16_t mesh3 = mission == 0x50 ? 0x3821 : (dlcWon ? 0x3825 : 0x4225);
            this->rootGeometry = new AEGeometry(rootMesh, canvas, false);
            uint32_t t0, t1, t2;
            canvas->TransformCreate(t0);
            canvas->TransformAddMesh(t0, mesh1, false);
            this->rootGeometry->addChild(t0);
            this->meshTransform = t0;
            t1 = 0xffffffff;
            canvas->TransformCreate(t1);
            canvas->TransformAddMesh(t1, mesh2, false);
            this->rootGeometry->addChild(t1);
            t2 = 0xffffffff;
            canvas->TransformCreate(t2);
            canvas->TransformAddMesh(t2, mesh3, false);
            this->rootGeometry->addChild(t2);
            if (mission == 0x50 || dlcWon) {
                ((AbyssEngine::Transform *) canvas->TransformGetTransform(this->rootGeometry->transform))
                        ->SetAnimationState(AbyssEngine::AnimationMode_0, nullptr);
                ((AbyssEngine::Transform *) canvas->TransformGetTransform(t1))
                        ->SetAnimationState(AbyssEngine::AnimationMode_0, nullptr);
                ((AbyssEngine::Transform *) canvas->TransformGetTransform(t2))
                        ->SetAnimationState(AbyssEngine::AnimationMode_0, nullptr);
                if (collision != nullptr) {
                    delete collision;
                }
                FileRead reader;
                collision = reader.loadStationCollision(0x3ed);
            } else {
                uint16_t lodMesh = 0x4226;
                int lodDist = (int) 0xf72c2200;
                this->rootGeometry->setLodMeshes(&lodMesh, &lodDist, 1);
                uint16_t lodChildMesh = 0x4227;
                this->rootGeometry->setLodChildMeshes(&lodChildMesh);
            }
        } else if (race == 0 || race == 2 || race == 3) {
            AEGeometry child((uint16_t)(stationIndex + 0x5528), canvas, false);
            this->rootGeometry->addChild(child.transform);
            AEGeometry child2((uint16_t)(this->stationIndex + 22000), canvas, false);
            this->rootGeometry->addChild(child2.transform);
            if (race == 3 && stationIndex == 0x6c) {
                AEGeometry e0(0x5974, canvas, false);
                this->rootGeometry->addChild(e0.transform);
                this->animTransform0 = e0.transform;
                AEGeometry e1(0x5975, canvas, false);
                this->rootGeometry->addChild(e1.transform);
                this->animTransform1 = e1.transform;
                AEGeometry e2(0x5976, canvas, false);
                this->rootGeometry->addChild(e2.transform);
                this->animTransform2 = e2.transform;
                AEGeometry e3(0x5977, canvas, false);
                this->rootGeometry->addChild(e3.transform);
                this->animTransform3 = e3.transform;
            }
        }
    }

    if (collision != nullptr) {
        int *data = collision->data();
        uint32_t count = (uint32_t) data[0];
        BoundingVolumeList *volumes = new BoundingVolumeList();
        this->boundingVolumes = volumes;
        ArraySetLength(count, *volumes);
        int cursor = 1;
        for (uint32_t i = 0; i < count; ++i) {
            data = collision->data();
            int next = cursor + 1;
            int type = data[cursor];
            if (type == 1) {
                int *entry = data + cursor;
                Vector c = {(float) entry[4], 0.0f, (float) entry[6]};
                c.y = (float) entry[5];
                c *= c.z;
                (*volumes)[i] = new BoundingAAB(c.x + c.x, (float) entry[3], c.z + c.z,
                                                (float) entry[2], c.y + c.y, (float) -data[next],
                                                0.0f, 0.0f, 0.0f);
                next = cursor + 7;
            } else if (type == 0) {
                int *entry = data + cursor;
                float radius = (float) -data[next];
                Vector c = {(float) entry[4], 0.0f, 0.0f};
                c *= radius;
                if (c.x < 0.0f) {
                    c *= c.x;
                }
                (*volumes)[i] = new BoundingSphere(c.x, (float) entry[3], (float) entry[2],
                                                   radius, 0.0f, 0.0f, 0.0f);
                next = cursor + 5;
            }
            cursor = next;
        }
        ArrayRemoveAll(*collision);
        delete collision;
    }

    this->rootGeometry->setRotation(0.0f, 0.0f, 0.0f);
    AbyssEngine::Transform *transform =
            (AbyssEngine::Transform *) canvas->TransformGetTransform(this->rootGeometry->transform);
    this->collisionRadius = (int) (transform->boundingRadius + 10.0f);
}

int PlayerStation::outerCollide(float x, float y, float z) {
    float radius = (float) this->collisionRadius;

    if (x < this->posX + radius && this->posX - radius < x &&
        y < this->posY + radius && this->posY - radius < y &&
        z < this->posZ + radius && this->posZ - radius < z) {
        BoundingVolumeList *volumes = (BoundingVolumeList *) this->boundingVolumes;
        if (volumes != nullptr) {
            for (uint32_t i = 0; i < volumes->size(); ++i) {
                BoundingVolume *volume = (*volumes)[i];
                if (volume->collide(x, y, z)) {
                    this->collisionIndex = i;
                    return true;
                }
                volumes = (BoundingVolumeList *) this->boundingVolumes;
            }
        }
    }

    return false;
}

void PlayerStation::translate(float x, float y, float z) {
    this->rootGeometry->translate(x, y, z);
}
