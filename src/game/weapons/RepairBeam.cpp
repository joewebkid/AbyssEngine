#include "game/weapons/RepairBeam.h"
#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"
#include "game/ship/Ship.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/world/Level.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/PlayerEgo.h"
#include "game/core/Globals.h"
#include "engine/math/Transform.h"
#include "engine/audio/FModSound.h"


namespace AbyssEngine {
    namespace AEMath {
        float VectorLength(const Vector &v);

        Vector VectorNormalize(const Vector &v);
    }
}

// Android 2.0.16 .rodata at 0x1fbf80, indexed by the ship id.
static constexpr int kRepairBeamEventByShip[64] = {
    52, 53, 54, 56, 55, 57, 58, 59,
    60, 50, 49, 50, 78, 75, 79, 77,
    72, 73, 74, 76, 80, 81, 66, 67,
    63, 64, 68, 65, 69, 70, 71, 82,
    83, 84, 85, 86, 87, 88, 89, 90,
    91, 6, 7, 8, 9, 10, 11, 92,
    93, 94, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 1098, 1100, 1099, -1,
};

void RepairBeam::render() {
    Array<int> &ids = *this->targetIds;
    for (unsigned i = 0; i < ids.size(); ++i) {
        if (ids[i] != -1)
            (*this->geometries)[i]->render();
    }
}

RepairBeam::RepairBeam(int shipIndex, int sort) {
    this->sort = sort;
    this->shipIndex = shipIndex;
    this->beamPosition.x = 0;
    this->beamPosition.y = 0;
    this->beamPosition.z = 0;

    Ship *ship = Status::gStatus->getShip();
    Item *equip = ship->getFirstEquipmentOfSort(sort);
    int count = equip->getAttribute(/*RepairBeamCount*/ 0x37);

    this->geometries = new Array<AEGeometry *>();
    ArraySetLength<AEGeometry *>(count, *this->geometries);

    uint16_t geoId = (sort == 0x25) ? 0x4a94 : 0x4a95;
    PaintCanvas *canvas = Globals::Canvas;
    for (int i = 0; i < count; ++i)
        (*this->geometries)[i] = new AEGeometry(geoId, canvas, false);

    this->targetIds = new Array<int>();
    ArraySetLength(count, *(this->targetIds));
    this->timer = 0x9c4;
    for (unsigned k = 0; k < this->targetIds->size(); ++k)
        (*this->targetIds)[k] = -1;

    this->charges = new Array<float>();
    ArraySetLength<float>(count, *this->charges);
    for (unsigned j = 0; j < this->charges->size(); ++j)
        (*this->charges)[j] = 0.0f;
}

RepairBeam::~RepairBeam() {
    if (this->geometries != nullptr) {
        ArrayReleaseClasses(*this->geometries); delete this->geometries;
        this->geometries = nullptr;
    }
    delete this->targetIds;
    this->targetIds = nullptr;
    delete this->charges;
    this->charges = nullptr;
}

void RepairBeam::update(int dt, Radar *radar, Level *level, Hud *hud) {
    (void) radar;
    (void) hud;

    Array<KIPlayer *> *enemies = level->getEnemies();
    this->timer -= dt;

    if (enemies != nullptr) {
        PlayerEgo *ego = level->getPlayer();
        if (!ego->isDead()) {
            Ship *ship = Globals::status->getShip();
            Item *equip = ship->getFirstEquipmentOfSort(this->sort);
            float attrF = static_cast<float>(equip->getAttribute(53));

            Array<int> &ids = *this->targetIds;
            Vector &beamPos = this->beamPosition;

            if (this->timer < 0) {
                for (unsigned i = 0; i < ids.size(); ++i) {
                    ids[i] = -1;
                    (*this->charges)[i] = 0.0f;
                }
                this->timer += 0x9c4;

                for (unsigned e = 0; e < enemies->size(); ++e) {
                    KIPlayer *kp = (*enemies)[e];
                    if (kp->isDead() || kp->isDying())
                        continue;

                    bool consider = false;
                    if (this->sort == 0x25) {
                        Player *pl = kp->player;
                        if (pl->carriesFriendCargoFlag != 0 &&
                            pl->getHitpoints() < pl->getMaxHitpoints())
                            consider = true;
                    } else if (this->sort == 0x29) {
                        if (kp->noTargetFlag == 0 &&
                            kp->player->enemyFlagsLo != 0) {
                            Player *egoPlayer = static_cast<Player *>(ego->player);
                            if (egoPlayer->getShieldDamageRate() < 100 &&
                                Globals::status->getShip()->getFirstEquipmentOfSort(9) != nullptr)
                                consider = true;
                        }
                    }
                    if (!consider)
                        continue;

                    beamPos = kp->player->getPosition();
                    beamPos -= ego->getPosition();
                    if (AbyssEngine::AEMath::VectorLength(beamPos) > attrF)
                        continue;

                    bool placed = false;
                    for (unsigned s = 0; !placed && s < ids.size(); ++s) {
                        if (ids[s] == -1) {
                            ids[s] = (int) e;
                            placed = true;
                        }
                    }
                    if (!placed) {
                        int srcHp = kp->player->getHitpoints();
                        unsigned best = 0xffffffffu;
                        int bestHp = 9999999;
                        for (unsigned s = 0; s < ids.size(); ++s) {
                            if (ids[s] != -1) {
                                int hp = (*enemies)[ids[s]]->player->getHitpoints();
                                if (hp < bestHp && srcHp < hp) {
                                    bestHp = hp;
                                    best = s;
                                }
                            }
                        }
                        if (best != 0xffffffffu)
                            ids[best] = (int) e;
                    }
                }
            }

            float scaleDiv = 100.0f;
            float dtF = (float) dt;
            PaintCanvas *canvas = Globals::Canvas;
            float healAmt = dtF * 0.03f;
            float shieldAmt = dtF * 0.01f;
            bool allInactive = true;

            for (unsigned i = 0; i < ids.size(); ++i) {
                if (ids[i] == -1)
                    continue;

                AEGeometry *geo = (*this->geometries)[i];
                auto *transform = static_cast<AbyssEngine::Transform *>(
                    canvas->TransformGetTransform(geo->transform));
                transform->Update(dt, false);

                KIPlayer *enemy = (*enemies)[ids[i]];
                beamPos = enemy->player->getPosition();
                int kind = enemy->shipGroupFlag;

                Vector contrib{0.0f, 0.0f, 0.0f};
                if (kind == 0x2c) {
                    contrib = AbyssEngine::AEMath::VectorNormalize(enemy->geometry->getDirection()) * -300.0f;
                } else if (kind == 0x31) {
                    contrib = AbyssEngine::AEMath::VectorNormalize(enemy->geometry->getDirection()) * -300.0f;
                    contrib += AbyssEngine::AEMath::VectorNormalize(enemy->geometry->getUpVector()) * 200.0f;
                }
                beamPos += contrib;
                beamPos -= ego->getPosition();

                Vector originOffset{0.0f, 0.0f, 0.0f};
                int ownShipIndex = Globals::status->getShip()->getIndex();
                if (ownShipIndex == 0x2c) {
                    originOffset = ego->GetDirVector() * -300.0f;
                } else if (ownShipIndex == 0x31) {
                    originOffset = ego->GetDirVector() * -300.0f;
                    originOffset += ego->GetUpVector() * 200.0f;
                }

                float len = AbyssEngine::AEMath::VectorLength(beamPos);
                geo->setScaling(0.5f, 0.5f, len);

                Vector ndir = AbyssEngine::AEMath::VectorNormalize(beamPos - originOffset);
                Vector beamUp;
                beamUp.x = 0.0f;
                beamUp.y = 1.0f;
                beamUp.z = 0.0f;
                geo->setDirection(ndir, beamUp);

                geo->setPosition(ego->getPosition() + originOffset);

                transform = static_cast<AbyssEngine::Transform *>(
                    canvas->TransformGetTransform(geo->transform));
                transform->Update(dt, false);

                if (this->sort == 0x29) {
                    Player *egoPlayer = static_cast<Player *>(level->getPlayer()->player);
                    if (egoPlayer->getShieldDamageRate() < 100) {
                        Item *equip = Globals::status->getShip()->getFirstEquipmentOfSort(this->sort);
                        float a = static_cast<float>(equip->getAttribute(54));
                        float &charge = (*this->charges)[i];
                        charge += (shieldAmt * a) / scaleDiv;
                        if (charge >= 1.0f) {
                            enemy->player->damage(1, false, -1);
                            charge -= 1.0f;
                        }
                        Player *egoPlayer2 = static_cast<Player *>(level->getPlayer()->player);
                        Item *equip2 = Globals::status->getShip()->getFirstEquipmentOfSort(this->sort);
                        float a2 = static_cast<float>(equip2->getAttribute(54));
                        egoPlayer2->regenerateShield((shieldAmt * a2) / scaleDiv);
                    }
                } else if (this->sort == 0x25) {
                    Item *equip = Globals::status->getShip()->getFirstEquipmentOfSort(this->sort);
                    float a = static_cast<float>(equip->getAttribute(54));
                    enemy->player->heal((healAmt * a) / scaleDiv);
                }

                int event = kRepairBeamEventByShip[this->shipIndex];
                if (Globals::sound->isPlaying(event) == 0)
                    Globals::sound->play(event, nullptr, nullptr, 0.0f);
                allInactive = false;
            }
            if (allInactive)
                Globals::sound->stop(kRepairBeamEventByShip[this->shipIndex]);
        }
    }

    PlayerEgo *ego2 = level->getPlayer();
    int event = kRepairBeamEventByShip[this->shipIndex];
    if (ego2->isDead() && Globals::sound->isPlaying(event) != 0) {
        Globals::sound->stop(event);
    }

    if (Globals::options[0x0f] != 0) {
        if (Globals::sound->isPlaying(event) != 0) {
            this->beamPosition = level->getPlayer()->getPosition();
            Globals::sound->updateEvent3DAttributes(event, &this->beamPosition, nullptr, false);
        }
    }
}

