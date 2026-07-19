#include "game/weapons/RepairBeam.h"
#include "engine/render/AEGeometry.h"
#include "engine/render/PaintCanvas.h"
#include "game/ship/Ship.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/world/Level.h"
#include "game/ship/KIPlayer.h"


namespace AbyssEngine {
    namespace AEMath {
        float VectorLength(const Vector &v);

        Vector VectorNormalize(const Vector &v);
    }
}

static PaintCanvas **g_RepairBeam_canvas;
static PaintCanvas **g_RB_canvas;
static int *g_RB_dmgThresh;
static float g_RB_scaleDiv;
static float g_RB_healMul;
static float g_RB_shieldMul;
static int *g_RB_sndPlay;
static int *g_RB_sndPlayEv;
static int **g_RB_sndStop;
static int *g_RB_sndDead;
static int **g_RB_sndUpd;
static int *g_RB_sndUpdEv;
static char **g_RB_sndFlag;

void *RB_Level_getPlayer(void *level);

int RB_PlayerEgo_isDead(void *ego);

int RB_Status_getShip();

int RB_Ship_getFirstEquipmentOfSort(int ship);

int RB_Ship_getIndex();

int RB_Item_getAttribute(int item);

int RB_KIPlayer_isDead(void *kp);

int RB_KIPlayer_isDying(void *kp);

int RB_Player_getHitpoints();

int RB_Player_getMaxHitpoints();

int RB_Player_getShieldDamageRate(void *pl);

void RB_Player_getPosition(Vector * out);
void RB_PlayerEgo_getPosition(Vector * out);

void RB_Player_damage(int pl, bool b, int z);

void RB_Player_heal(int pl, float amt);

void RB_Player_regenerateShield(void *pl, float amt);

float RB_PlayerEgo_GetDirVector();

float RB_PlayerEgo_GetUpVector();

void RB_Transform_Update(long long t, bool b);

int RB_FModSound_isPlaying(int snd);

void RB_FModSound_play(int snd, void *ev, void *p, float f);

void RB_FModSound_stop(int snd);

void RB_FModSound_updateEvent3DAttributes(void *snd, int ev, Vector *pos, void *p, bool b);

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
    PaintCanvas *canvas = *g_RepairBeam_canvas;
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
        void *ego = RB_Level_getPlayer(level);
        if (RB_PlayerEgo_isDead(ego) == 0) {
            int ship = RB_Status_getShip();
            int equip = RB_Ship_getFirstEquipmentOfSort(ship);
            float attrF = (float) RB_Item_getAttribute(equip);

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
                    if (RB_KIPlayer_isDead(kp) != 0 || RB_KIPlayer_isDying(kp) != 0)
                        continue;

                    bool consider = false;
                    if (this->sort == 0x25) {
                        Player *pl = kp->player;
                        if (pl->carriesFriendCargoFlag != 0 && RB_Player_getHitpoints() < RB_Player_getMaxHitpoints())
                            consider = true;
                    } else if (this->sort == 0x29) {
                        if (kp->noTargetFlag == 0 &&
                            kp->player->enemyFlagsLo != 0) {
                            void **plp = (void **) RB_Level_getPlayer(level);
                            if (RB_Player_getShieldDamageRate(*plp) < 100 &&
                                RB_Ship_getFirstEquipmentOfSort(RB_Status_getShip()) != 0)
                                consider = true;
                        }
                    }
                    if (!consider)
                        continue;

                    Vector tmp;
                    RB_Player_getPosition(&tmp);
                    beamPos = tmp;
                    RB_PlayerEgo_getPosition(&tmp);
                    beamPos -= tmp;
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
                        int srcHp = RB_Player_getHitpoints();
                        unsigned best = 0xffffffffu;
                        int bestHp = *g_RB_dmgThresh;
                        for (unsigned s = 0; s < ids.size(); ++s) {
                            if (ids[s] != -1) {
                                int hp = RB_Player_getHitpoints();
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

            float scaleDiv = g_RB_scaleDiv;
            float dtF = (float) dt;
            PaintCanvas *canvas = *g_RB_canvas;
            float healAmt = dtF * g_RB_healMul;
            float shieldAmt = dtF * g_RB_shieldMul;
            bool allInactive = true;

            for (unsigned i = 0; i < ids.size(); ++i) {
                if (ids[i] == -1)
                    continue;

                AEGeometry *geo = (*this->geometries)[i];
                long long t = (long long) canvas->TransformGetTransform(geo->transform);
                RB_Transform_Update(t, dt != 0);

                Vector tmp;
                RB_Player_getPosition(&tmp);
                beamPos = tmp;

                KIPlayer *enemy = (*enemies)[ids[i]];
                int kind = enemy->field_0x72;

                Vector contrib;
                if (kind == 0x2c) {
                    Vector dir = AbyssEngine::AEMath::VectorNormalize(((AEGeometry *) enemy)->getDirection());
                    dir *= RB_PlayerEgo_GetDirVector();
                    contrib = dir;
                } else if (kind == 0x31) {
                    Vector dir = AbyssEngine::AEMath::VectorNormalize(((AEGeometry *) enemy)->getDirection());
                    dir *= RB_PlayerEgo_GetDirVector();
                    Vector up = AbyssEngine::AEMath::VectorNormalize(((AEGeometry *) enemy)->getUpVector());
                    up *= RB_PlayerEgo_GetUpVector();
                    dir += up;
                    contrib = dir;
                } else {
                    contrib.x = 0;
                    contrib.y = 0;
                    contrib.z = 0;
                }
                beamPos += contrib;
                RB_PlayerEgo_getPosition(&tmp);
                beamPos -= tmp;

                float len = AbyssEngine::AEMath::VectorLength(beamPos);
                geo->setScaling(len, 0.5f, 0.5f);

                Vector ndir = AbyssEngine::AEMath::VectorNormalize(beamPos);
                Vector beamUp;
                beamUp.x = 0.0f;
                beamUp.y = 1.0f;
                beamUp.z = 0.0f;
                geo->setDirection(ndir, beamUp);

                RB_PlayerEgo_getPosition(&ndir);
                geo->setPosition(ndir);

                long long t2 = (long long) canvas->TransformGetTransform(geo->transform);
                RB_Transform_Update(t2, dt != 0);

                if (this->sort == 0x29) {
                    void **plp = (void **) RB_Level_getPlayer(level);
                    if (RB_Player_getShieldDamageRate(*plp) < 100) {
                        int eq = RB_Ship_getFirstEquipmentOfSort(RB_Status_getShip());
                        float a = (float) RB_Item_getAttribute(eq);
                        float &charge = (*this->charges)[i];
                        charge += (shieldAmt * a) / scaleDiv;
                        if (charge < 1.0f) {
                            RB_Player_damage((int) (intptr_t) enemy->player, true, 0);
                            charge -= 1.0f;
                        }
                        void **plp2 = (void **) RB_Level_getPlayer(level);
                        int eq2 = RB_Ship_getFirstEquipmentOfSort(RB_Status_getShip());
                        float a2 = (float) RB_Item_getAttribute(eq2);
                        RB_Player_regenerateShield(*plp2, (shieldAmt * a2) / scaleDiv);
                    }
                } else if (this->sort == 0x25) {
                    int pl = (int) (intptr_t) enemy->player;
                    int eq = RB_Ship_getFirstEquipmentOfSort(RB_Status_getShip());
                    float a = (float) RB_Item_getAttribute(eq);
                    RB_Player_heal(pl, (healAmt * a) / scaleDiv);
                }

                int snd = *g_RB_sndPlay;
                if (RB_FModSound_isPlaying(snd) == 0)
                    RB_FModSound_play(snd, (void *) (intptr_t) g_RB_sndPlayEv[this->shipIndex], nullptr, 0.0f);
                allInactive = false;
            }
            if (allInactive)
                RB_FModSound_stop(**g_RB_sndStop);
        }
    }

    void *ego2 = RB_Level_getPlayer(level);
    if (RB_PlayerEgo_isDead(ego2) != 0) {
        int snd = *g_RB_sndDead;
        if (RB_FModSound_isPlaying(snd) != 0)
            RB_FModSound_stop(snd);
    }

    if ((*g_RB_sndFlag)[0xf] != 0) {
        int *evArr = g_RB_sndUpdEv;
        int *snd = *g_RB_sndUpd;
        if (RB_FModSound_isPlaying(*snd) != 0) {
            int shipIdx = this->shipIndex;
            Vector tmp;
            RB_PlayerEgo_getPosition(&tmp);
            this->beamPosition = tmp;
            RB_FModSound_updateEvent3DAttributes(snd, evArr[shipIdx], &this->beamPosition, nullptr, false);
        }
    }
}

