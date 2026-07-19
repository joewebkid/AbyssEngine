#include "game/weapons/TractorBeam.h"
#include "game/ship/Ship.h"
#include "engine/render/AEGeometry.h"
#include "engine/audio/FModSound.h"
#include "game/world/Level.h"
#include "engine/math/Transform.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/PlayerEgo.h"
#include "game/mission/Status.h"
#include "engine/render/PaintCanvas.h"
#include "game/weapons/Radar.h"
#include "game/ship/Player.h"

static PaintCanvas *gCanvasRoot = nullptr;
static FModSound *gPullSound = nullptr;
static FModSound *gCaptureSound = nullptr;
static const float gCaptureDistance = 0.0f;

void TractorBeam::render() {
    if (!this->active)
        return;
    this->beamGeometry->render();
}

void TractorBeam::update(int frameTime, Radar *radar, Level *level, Hud *hud) {
    KIPlayer *crate = this->grabbedCrate;
    KIPlayer *radarCrate = (KIPlayer *) radar->field_0x1c;

    if (radarCrate == nullptr && crate == nullptr)
        return;

    PlayerEgo *player = (PlayerEgo *) (intptr_t) level->getPlayer();

    if (crate == nullptr) {
        if (!player->isInTurretMode()) {
            if (radarCrate->cargoAvailable() == 0) {
                this->active = 0;
                radar->field_0x1c = nullptr;
            } else {
                if (radarCrate->crateGeometry == nullptr)
                    radarCrate->createCrate(0);
                this->grabbedCrate = radarCrate;
                this->storedHitpoints = ((Player *) radarCrate->player)->getHitpoints();
                this->active = 1;
            }
            return;
        }
        crate = this->grabbedCrate;
        if (crate == nullptr)
            goto detach;
    }

    if (crate->crateGeometry == nullptr ||
        (crate->stealFlag == 0 && ((Player *) crate)->isActive() == 0)) {
    detach:
        radar->field_0x8 = nullptr;
        radar->field_0x1c = nullptr;
        this->grabbedCrate = nullptr;
        this->active = 0;
        return;
    }

    PaintCanvas *canvas = gCanvasRoot;
    AbyssEngine::Transform *tf = (AbyssEngine::Transform *) canvas->TransformGetTransform(0);
    tf->Update(frameTime, false);

    Vector cratePos = ((AEGeometry *) this->grabbedCrate->crateGeometry)->getPosition();
    Vector playerPos = player->getPosition();
    Vector working = cratePos - playerPos;
    this->dirX = working.x;
    this->dirY = working.y;
    this->dirZ = working.z;
    float dist = VectorLength(working);

    int shipIndex = Status::gStatus->getShip()->getIndex();
    Vector offset = {0.0f, 0.0f, 0.0f};
    if (shipIndex == 0x2c) {
        offset = player->GetDirVector() * 0.5f;
    } else if (shipIndex == 0x31) {
        Vector dir = player->GetDirVector();
        Vector tmp = dir * 0.5f;
        offset.x = tmp.x + dir.x;
        offset.y = tmp.y + dir.y;
        offset.z = tmp.z + dir.z;
    }
    this->beamGeometry->setScaling(offset.x);

    AEGeometry *beam = this->beamGeometry;
    Vector dirN = VectorNormalize(offset - working);
    beam->setDirection(dirN, dirN);
    beam->setPosition(player->getPosition());

    if (dist > gCaptureDistance) {
        radar->field_0x1c = nullptr;
        dirN = VectorNormalize(working);
        working = dirN * (float) (frameTime * 10);
        Vector pull = dirN - working;
        ((AEGeometry *) this->grabbedCrate->crateGeometry)->translate(pull);

        if (this->soundPlaying == 0) {
            gPullSound->play(0, nullptr, nullptr, 0.0f);
            this->soundPlaying = 1;
        }
    } else {
        this->grabbedCrate->captureCrate(hud);
        this->grabbedCrate = nullptr;
        this->active = 0;
        radar->field_0x8 = nullptr;
        radar->field_0x1c = nullptr;
        this->soundPlaying = 0;
        gCaptureSound->stop(nullptr);
        gCaptureSound->play(4, nullptr, nullptr, 0.0f);
    }
}

TractorBeam::TractorBeam(AEGeometry * /*unused*/, int kind) {
    this->dirX = 0.0f;
    this->dirY = 0.0f;
    this->dirZ = 0.0f;
    this->grabbedCrate = nullptr;
    this->active = 0;
    this->soundPlaying = 0;

    uint16_t meshId = (uint16_t)((short) kind + 0x3798);
    this->beamGeometry = new AEGeometry(meshId, gCanvasRoot, false);
    this->storedHitpoints = 0;
}

TractorBeam::~TractorBeam() {
    delete this->beamGeometry;
    this->beamGeometry = nullptr;
}
