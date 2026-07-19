#include "game/weapons/Radar.h"
#include "engine/core/GameText.h"
#include "engine/audio/FModSound.h"
#include "engine/render/Sprite.h"
#include "engine/render/PaintCanvas.h"

#include <cstdint>

#include "game/core/Globals.h"
#include "game/mission/Item.h"
#include "game/mission/Mission.h"
#include "game/mission/Status.h"
#include "game/ship/Agent.h"
#include "game/ship/Player.h"
#include "game/ship/PlayerAsteroid.h"
#include "game/ship/PlayerEgo.h"
#include "game/ship/PlayerGasCloud.h"
#include "game/ship/KIPlayer.h"
#include "game/ship/Ship.h"
#include "game/weapons/AbstractGun.h"
#include "game/weapons/BombGun.h"
#include "game/weapons/MineGun.h"
#include "game/weapons/RocketGun.h"
#include "game/ui/Hud.h"
#include "game/world/Level.h"
#include "game/world/Route.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"
#include "game/world/StarSystem.h"

#if defined(__GNUC__) || defined(__clang__)
#define GOF2_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define GOF2_ALWAYS_INLINE inline
#endif

static inline int layout_i32(void *layout, unsigned off) {
    return *reinterpret_cast<int *>(static_cast<char *>(layout) + off);
}

static inline uint8_t station_u8(void *station, unsigned off) {
    return *reinterpret_cast<uint8_t *>(static_cast<char *>(station) + off);
}

static inline uint8_t raw_u8(void *ptr, unsigned off) {
    return *reinterpret_cast<uint8_t *>(static_cast<char *>(ptr) + off);
}

static inline int raw_i32(void *ptr, unsigned off) {
    return *reinterpret_cast<int *>(static_cast<char *>(ptr) + off);
}

static inline int &radar_image_slot(Radar *self, unsigned offset) {
    return self->imageIds_0x5c[(offset - 0x5c) / 4];
}

static inline unsigned int radar_font() {
    return static_cast<unsigned int>(reinterpret_cast<uintptr_t>(Globals::font));
}

static inline String radar_get_text(int id) {
    return String(*static_cast<GameText *>(Globals::gameText)->getText(id), false);
}

static inline String *radar_alloc_string(const String &value) {
    return new String(value);
}

static inline String *radar_label_at(Array<String *> *labels, unsigned index) {
    if (labels == nullptr || labels->size() <= index) {
        return nullptr;
    }
    return (*labels)[index];
}

static inline KIPlayer *radar_target_at(Array<KIPlayer *> *targets, unsigned index) {
    if (targets == nullptr || targets->size() <= index) {
        return nullptr;
    }
    return (*targets)[index];
}

static GOF2_ALWAYS_INLINE PlayerEgo *radar_level_player(Radar *self) {
    return self->level->getPlayer();
}

static GOF2_ALWAYS_INLINE void radar_kiplayer_position_into(KIPlayer *player, Vector &out) {
#if defined(__arm__)
    using GetPositionFn = void (*)(Vector *, KIPlayer *);
    void **vtable = *reinterpret_cast<void ***>(player);
    reinterpret_cast<GetPositionFn>(vtable[10])(&out, player);
#else
    out = player->getPosition();
#endif
}

static inline void radar_draw_centered(PaintCanvas *canvas, const String &text, int y) {
    if (canvas == nullptr) {
        return;
    }
    unsigned int font = radar_font();
    int x = (Globals::w >> 1) - (canvas->GetTextWidth(font, text) >> 1);
    canvas->DrawString(font, text, x, y, false);
}

static inline void radar_create_image(AbyssEngine::PaintCanvas *canvas, unsigned short resId, int &slot) {
    unsigned int image;
    canvas->Image2DCreate(resId, image);
    slot = static_cast<int>(image);
}

static inline Mission *radar_campaign_mission(Status *status) {
    return status != nullptr
        ? reinterpret_cast<Mission *>(static_cast<intptr_t>(status->getCampaignMission()))
        : nullptr;
}

static inline Array<String *> *radar_planet_names(Status *status) {
    return status != nullptr
        ? reinterpret_cast<Array<String *> *>(static_cast<intptr_t>(status->getPlanetNames()))
        : nullptr;
}

static GOF2_ALWAYS_INLINE bool radar_crosshair_contains(Radar *self, int size) {
    int half = size >> 1;
    int left = static_cast<int>(PlayerEgo::crosshairPos.x - static_cast<float>(half));
    int top = static_cast<int>(PlayerEgo::crosshairPos.y - static_cast<float>(half));
    return self->screenX > left &&
           self->screenX < left + size &&
           self->screenY > top &&
           self->screenY < top + size;
}

static inline bool radar_campaign_forces_marker(Status *status, Mission *campaign, int stationId, int currentMission) {
    if (currentMission == 116 && static_cast<unsigned>(stationId - 90) <= 4u) {
        return ((1 << (stationId - 90)) & campaign->getStatusValue()) == 0;
    }
    if (currentMission == 120 && stationId == 93) {
        return true;
    }
    if (currentMission == 125 && status->isFreighterMissionStation(stationId)) {
        int bit = status->getFreighterMissionStationBit(stationId);
        return ((1 << bit) & campaign->getStatusValue()) == 0;
    }
    return false;
}

static inline bool radar_draw_landmark_station_markers(Radar *self,
                                                       Status *status,
                                                       bool turretMode,
                                                       bool alienOrbit,
                                                       int currentMission,
                                                       int elapsed) {
    if (self->landmarkTargets == nullptr) {
        return false;
    }

    Station *station = status->getStation();
    int stationIndex = station->getIndex();
    if (stationIndex == 109 || stationIndex == 110 || (stationIndex == 111 && currentMission > 93)) {
        return false;
    }

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    bool existingNonStationLock = self->lockedAsteroid != nullptr ||
                                  self->lockedGasCloud != nullptr ||
                                  self->field_0x1c != nullptr ||
                                  self->lockedPlanetTarget != nullptr;
    bool candidateFound = false;

    for (unsigned i = 0; i < self->landmarkTargets->size(); ++i) {
        if (i == 2) {
            continue;
        }

        KIPlayer *target = (*self->landmarkTargets)[i];
        if (target == nullptr || target->isVisible() == 0) {
            continue;
        }

        self->update(target);
        if (self->onScreen != 0 &&
            self->screenX > self->centerX - self->field_0x128 &&
            self->screenX < self->centerX + self->field_0x128 &&
            self->screenY > self->centerY - self->field_0x128 &&
            self->screenY < self->centerY + self->field_0x128) {
            if (!candidateFound && !existingNonStationLock && !turretMode && i != 3 &&
                radar_crosshair_contains(self, self->field_0x124)) {
                if (self->candidateStation != target) {
                    self->field_0x1a4 = 0;
                }
                self->candidateStation = target;
                candidateFound = true;
            }

            if (i != 3) {
                canvas->DrawImage2D(radar_image_slot(self, 0x88),
                                    self->screenX,
                                    self->screenY,
                                    0x11u,
                                    0x44u);
            }

            String *label = (*self->labelStrings)[i];
            int textOffset = i == 0 ? self->originX : self->originY;
            canvas->DrawString(radar_font(), *label, self->screenX + textOffset, self->screenY, false);

            if (i <= 1) {
                int row = layout_i32(Globals::layout, 4);
                self->lockLabel = self->calcDistance(target->posX,
                                                     target->posY,
                                                     target->posZ,
                                                     self->cameraPosX,
                                                     self->cameraPosY,
                                                     self->cameraPosZ);
                if (i != 0 || alienOrbit) {
                    canvas->DrawString(radar_font(),
                                       self->lockLabel,
                                       self->screenX + textOffset,
                                       self->screenY + row,
                                       false);
                } else {
                    String techText = radar_get_text(133);
                    techText += String(": ");
                    techText += String(station->getTecLevel());
                    canvas->DrawString(radar_font(),
                                       techText,
                                       self->screenX + self->halfScreenWidth,
                                       self->screenY + row,
                                       false);
                    canvas->DrawString(radar_font(),
                                       self->lockLabel,
                                       self->screenX + self->halfScreenWidth,
                                       self->screenY + 2 * row,
                                       false);
                }
            }
        } else {
            if (i == 1) {
                canvas->DrawImage2D(radar_image_slot(self, 0xb8),
                                    self->screenX,
                                    self->screenY,
                                    0x11u,
                                    0x44u);
            } else if (i == 3) {
                canvas->DrawImage2D(radar_image_slot(self, 0xb4),
                                    self->screenX,
                                    self->screenY,
                                    0x11u,
                                    0x44u);
            }
        }
    }

    if (turretMode) {
        return candidateFound;
    }

    if (self->lockedStation != nullptr) {
        KIPlayer *locked = self->lockedStation;
        if (locked->isDead()) {
            self->lockedStation = nullptr;
        } else if (locked == self->candidateStation) {
            self->field_0x218_byte = 1;
        }
        self->lockedStation = nullptr;
    }

    PlayerEgo *ego = radar_level_player(self);
    if (!candidateFound || ego->isDockingToAsteroid() || ego->isDockingToDockingPoint() || ego->isAutoPilot()) {
        self->field_0x1a4 = 0;
        if (self->lockedStation == nullptr) {
            self->candidateStation = nullptr;
        }
        return candidateFound;
    }

    self->field_0x1a4 += elapsed;
    if (self->field_0x1a4 > self->field_0x1b8) {
        if (self->field_0x218_byte == 0) {
            Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
        }
        self->lockedStation = self->candidateStation;
    }

    if (self->field_0x1a4 >= 1) {
        if (self->candidateStation == nullptr || self->candidateStation == self->lockedStation) {
            self->blipSprite->setFrame(self->blipSprite->getRawFrameCount() - 1);
        } else {
            int frame = static_cast<int>(
                (static_cast<float>(self->field_0x1a4) / static_cast<float>(self->field_0x1b8)) *
                static_cast<float>(self->blipSprite->getRawFrameCount() - 1));
            self->blipSprite->setFrame(frame);
        }
        self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                              static_cast<int>(PlayerEgo::crosshairPos.y));
        self->blipSprite->draw(1.0f, 1.0f);
    }

    return candidateFound;
}

static inline bool radar_draw_planet_station_markers(Radar *self,
                                                     Hud *hud,
                                                     Status *status,
                                                     bool missionTargetFilter,
                                                     bool turretMode,
                                                     bool stationCandidateActive,
                                                     int elapsed) {
    if (status->getCurrentCampaignMission() < 2 ||
        self->planetTargets == nullptr ||
        status->inAlienOrbit() ||
        radar_level_player(self)->isDockingToPlanet() != 0) {
        return false;
    }

    SolarSystem *system = status->getSystem();
    auto *stations = reinterpret_cast<Array<int> *>(system->getStations());

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    Station *currentStation = status->getStation();
    int currentStationId = currentStation->getIndex();
    int warpGateEnumIndex = system->getWarpGateEnumIndex();
    int currentMission = status->getCurrentCampaignMission();
    Mission *campaign = radar_campaign_mission(status);
    Mission *freelance = status->getFreelanceMission();
    bool anyExistingLock = self->lockedAsteroid != nullptr ||
                           self->lockedGasCloud != nullptr ||
                           self->field_0x1c != nullptr ||
                           self->lockedStation != nullptr ||
                           stationCandidateActive;
    bool candidateFound = false;

    unsigned count = self->planetTargets->size();

    for (unsigned i = 0; i < count; ++i) {
        KIPlayer *target = (*self->planetTargets)[i];
        if (target == nullptr) {
            continue;
        }

        self->update(target);
        if (self->onScreen == 0) {
            continue;
        }

        int stationId = (*stations)[i];
        int markerOffset = 10;
        if (stationId != currentStationId && static_cast<int>(i) == warpGateEnumIndex) {
            canvas->DrawImage2D(radar_image_slot(self, 0xbc), self->screenX + 10, self->screenY - 10);
            markerOffset = 24;
        }

        bool markerDrawn = false;
        if (campaign != nullptr && !campaign->isEmpty() && campaign->isVisible() &&
            (currentMission < 148 || currentMission >= 152)) {
            if (stationId != currentStationId && stationId == campaign->getTargetStation()) {
                if (currentMission != 120) {
                    canvas->DrawImage2D(radar_image_slot(self, 0xc8),
                                        self->screenX + markerOffset,
                                        self->screenY - 10);
                    markerDrawn = true;
                }
            }
            if (radar_campaign_forces_marker(status, campaign, stationId, currentMission)) {
                canvas->DrawImage2D(radar_image_slot(self, 0xc8),
                                    self->screenX + markerOffset,
                                    self->screenY - 10);
                markerDrawn = true;
            }
        }

        if (freelance != nullptr && !freelance->isEmpty() && freelance->isVisible()) {
            int targetStation = freelance->getTargetStation();
            if (freelance->getType() == 14 && freelance->getAgent() != nullptr) {
                targetStation = freelance->getAgent()->getStation();
            }
            if (stationId != currentStationId && stationId == targetStation) {
                canvas->DrawImage2D(radar_image_slot(self, 0xc4),
                                    self->screenX + markerOffset,
                                    self->screenY - 10);
                markerDrawn = true;
            }
        }

        if (!anyExistingLock && !candidateFound && !turretMode) {
            int targetSize = static_cast<int>(i) == StarSystem::orbitPlanetIndex
                ? self->field_0x128
                : self->field_0x124;
            if (radar_crosshair_contains(self, targetSize)) {
                if (static_cast<int>(i) != StarSystem::orbitPlanetIndex) {
                    if (self->candidatePlanetTarget != target) {
                        self->field_0x1a0 = 0;
                    }
                    self->candidatePlanetTarget = target;
                    self->planetDockIndex = static_cast<int>(i);
                    candidateFound = true;
                }

                if (stationId != currentStationId) {
                    Array<String *> *names = radar_planet_names(status);
                    int textOffset = markerDrawn ? markerOffset + 14 : markerOffset;
                    canvas->DrawString(radar_font(),
                                       *(*names)[i],
                                       self->screenX + textOffset,
                                       self->screenY - 10,
                                       false);
                }
            }
        }
    }

    if (turretMode) {
        return candidateFound;
    }

    if (self->lockedPlanetTarget != nullptr) {
        if (self->lockedPlanetTarget == self->candidatePlanetTarget) {
            self->field_0x218_byte = 1;
        }
        self->lockedPlanetTarget = nullptr;
    }

    PlayerEgo *player = radar_level_player(self);
    if (!candidateFound || player->isDockingToAsteroid() || player->isDockingToDockingPoint()) {
        self->field_0x1a0 = 0;
        self->candidatePlanetTarget = nullptr;
        return candidateFound;
    }

    self->field_0x1a0 += elapsed;
    if (self->field_0x1a0 > self->field_0x1b8) {
        if (missionTargetFilter) {
            if (hud != nullptr) {
                hud->hudEvent(21, radar_level_player(self), 0);
            }
        } else {
            if (self->field_0x218_byte == 0) {
                Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
            }
            self->lockedPlanetTarget = self->candidatePlanetTarget;
            if (self->field_0x1a8 != 0) {
                radar_level_player(self)->dockToPlanet();
            }
        }
    }

    if (!missionTargetFilter && self->field_0x1a0 >= 1) {
        if (self->candidatePlanetTarget == nullptr || self->candidatePlanetTarget == self->lockedPlanetTarget) {
            if (radar_level_player(self)->isDockingToPlanet() != 0) {
                return candidateFound;
            }
            self->blipSprite->setFrame(self->blipSprite->getRawFrameCount() - 1);
        } else {
            int frame = static_cast<int>(
                (static_cast<float>(self->field_0x1a0) / static_cast<float>(self->field_0x1b8)) *
                static_cast<float>(self->blipSprite->getRawFrameCount() - 1));
            self->blipSprite->setFrame(frame);
        }
        self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                              static_cast<int>(PlayerEgo::crosshairPos.y));
        self->blipSprite->draw(1.0f, 1.0f);
    }

    return candidateFound;
}

static GOF2_ALWAYS_INLINE bool radar_enemy_has_dead_cargo(KIPlayer *target) {
    return target != nullptr &&
           target->hasCargo != 0 &&
           (target->isDead() || target->isDying());
}

static inline void radar_draw_distance_label(Radar *self, KIPlayer *target, int x, int y);

static inline bool radar_enemy_relation_enemy(Player *player) {
    return player != nullptr && player->enemyFlagsLo != 0;
}

static inline bool radar_enemy_relation_friend(Player *player) {
    return player != nullptr && player->enemyFlagsLo == 0 && player->carriesFriendCargoFlag != 0;
}

static inline bool radar_enemy_count_excluded(KIPlayer *target) {
    return target != nullptr &&
           (target->stealFlagByte != 0 || target->countsAsEnemyExcludeFlag != 0);
}

static inline void radar_accumulate_enemy_ambient_threat(Radar *self,
                                                         KIPlayer *target,
                                                         bool deadCargo,
                                                         bool &type10Threat,
                                                         bool &alwaysEnemyThreat) {
    if (self == nullptr || target == nullptr || target->player == nullptr) {
        return;
    }
    if (deadCargo || radar_enemy_count_excluded(target)) {
        return;
    }
    if (!radar_enemy_relation_enemy(target->player)) {
        return;
    }

    ++self->field_0x1bc;
    if (target->getType() == 10) {
        type10Threat = true;
    }
    if (target->field_0x42 != 0 && target->player->isAlwaysEnemy() != 0) {
        alwaysEnemyThreat = true;
    }
}

static inline int radar_enemy_blip_offset(Radar *self, KIPlayer *target, bool deadCargo) {
    if (deadCargo) {
        return 0x9c;
    }
    Player *player = target != nullptr ? target->player : nullptr;
    if (player == nullptr) {
        return 0x78;
    }
    if (radar_enemy_relation_enemy(player)) {
        return target == self->lockedEnemy ? 0x90 : 0x60;
    }
    if (radar_enemy_relation_friend(player)) {
        return target == self->lockedEnemy ? 0x94 : 0x68;
    }
    return target == self->lockedEnemy ? 0x98 : 0x78;
}

static inline int radar_enemy_far_blip_offset(Radar *self, KIPlayer *target) {
    Player *player = target != nullptr ? target->player : nullptr;
    if (radar_enemy_relation_enemy(player)) {
        return target == self->lockedEnemy ? 0x7c : 0x60;
    }
    if (radar_enemy_relation_friend(player)) {
        return target == self->lockedEnemy ? 0x80 : 0x68;
    }
    return target == self->lockedEnemy ? 0x84 : 0x78;
}

static inline int radar_enemy_locked_overlay_offset(KIPlayer *target) {
    Player *player = target != nullptr ? target->player : nullptr;
    if (radar_enemy_relation_enemy(player)) {
        return 0x5c;
    }
    if (radar_enemy_relation_friend(player)) {
        return 0x64;
    }
    return 0x74;
}

static inline int radar_enemy_health_bg_offset(KIPlayer *target) {
    Player *player = target != nullptr ? target->player : nullptr;
    if (radar_enemy_relation_enemy(player)) {
        return 0xe0;
    }
    if (radar_enemy_relation_friend(player)) {
        return 0xe4;
    }
    return 0xe8;
}

static inline int radar_enemy_health_fill_offset(KIPlayer *target) {
    Player *player = target != nullptr ? target->player : nullptr;
    if (radar_enemy_relation_enemy(player)) {
        return 0xd0;
    }
    if (radar_enemy_relation_friend(player)) {
        return 0xd4;
    }
    return 0xd8;
}

static inline bool radar_enemy_is_far_or_special(Radar *self, KIPlayer *target) {
    Vector targetPosition;
    radar_kiplayer_position_into(target, targetPosition);
    Vector egoPosition = radar_level_player(self)->getPosition();
    float distance = targetPosition.x - egoPosition.x;
    if (distance > 24000.0f || distance < -24000.0f) {
        return true;
    }
    distance = targetPosition.y - egoPosition.y;
    if (distance > 24000.0f || distance < -24000.0f) {
        return true;
    }
    distance = targetPosition.z - egoPosition.z;
    return distance > 24000.0f || distance < -24000.0f || target->getType() == 16917;
}

static inline void radar_draw_enemy_health_emp_renderer(Radar *self,
                                                        KIPlayer *target,
                                                        bool deadCargo) {
    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    if (deadCargo) {
        canvas->DrawImage2D(radar_image_slot(self, 0x9c),
                            self->screenX,
                            self->screenY,
                            0x11u,
                            0x44u);
        return;
    }

    if (target->isDead()) {
        return;
    }

    if (radar_enemy_is_far_or_special(self, target)) {
        canvas->DrawImage2D(radar_image_slot(self, radar_enemy_far_blip_offset(self, target)),
                            self->screenX,
                            self->screenY,
                            0x11u,
                            0x44u);
        if (target == self->lockedEnemy) {
            radar_draw_distance_label(self,
                                      target,
                                      self->screenX - self->halfScreenHeight,
                                      self->screenY + self->halfScreenWidth);
        }
        return;
    }

    int barX = self->screenX + 2 - self->halfScreenHeight;
    int barY = self->screenY + self->halfScreenWidth + 2;
    canvas->DrawImage2D(radar_image_slot(self, radar_enemy_health_bg_offset(target)), barX, barY);

    int healthWidth = static_cast<int>((static_cast<float>(target->player->getDamageRate()) / 100.0f) *
                                       static_cast<float>(self->lockPanelWidth));
    self->field_0x184 = healthWidth;
    canvas->DrawRegion2D(radar_image_slot(self, radar_enemy_health_fill_offset(target)),
                         0,
                         0,
                         healthWidth,
                         self->lockPanelHeight,
                         0.0f,
                         0,
                         0,
                         self->screenX + 3 - self->halfScreenHeight,
                         self->screenY + self->halfScreenWidth + 3);

    if (target->player->getEmpPoints() < target->player->getMaxEmpPoints()) {
        canvas->DrawImage2D(radar_image_slot(self, 0xec),
                            self->screenX + 2 - self->halfScreenHeight,
                            self->screenY + self->halfScreenWidth + 8);
        int empWidth = static_cast<int>((static_cast<float>(target->player->getEmpDamageRate()) / 100.0f) *
                                        static_cast<float>(self->lockPanelWidth));
        self->field_0x184 = empWidth;
        canvas->DrawRegion2D(radar_image_slot(self, 0xdc),
                             0,
                             0,
                             empWidth,
                             self->lockPanelHeight,
                             0.0f,
                             0,
                             0,
                             self->screenX + 3 - self->halfScreenHeight,
                             self->screenY + self->halfScreenWidth + 9);
    }

    if (target == self->lockedEnemy) {
        canvas->DrawImage2D(radar_image_slot(self, radar_enemy_locked_overlay_offset(target)),
                            self->screenX,
                            self->screenY,
                            0x11u,
                            0x44u);
    }
}

static inline void radar_draw_enemy_lock_progress(Radar *self, KIPlayer *target, bool deadCargo) {
    bool fastLock = deadCargo || target->field_0x24 != 0;
    int drawDelay = fastLock ? 500 : 0;
    if (self->field_0x198 <= drawDelay) {
        return;
    }

    KIPlayer *resolvedLock = fastLock ? self->field_0x1c : self->lockedEnemy;
    if (target == resolvedLock) {
        return;
    }

    int denominator = self->field_0x1b8;
    int elapsed = self->field_0x198;
    if (fastLock) {
        elapsed -= 500;
        denominator = self->field_0x1b4 - 500;
    }

    int frameMax = self->blipSprite->getRawFrameCount() - 1;
    int frame = static_cast<int>((static_cast<float>(elapsed) / static_cast<float>(denominator)) *
                                 static_cast<float>(frameMax));
    self->blipSprite->setFrame(frame);
    self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                          static_cast<int>(PlayerEgo::crosshairPos.y));
    self->blipSprite->draw(1.0f, 1.0f);
}

static inline void radar_scan_enemy_cargo_for_hud(Radar *self, Hud *hud, KIPlayer *target) {
    Vector targetPosition;
    radar_kiplayer_position_into(target, targetPosition);
    Vector egoPosition = radar_level_player(self)->getPosition();
    float dx = targetPosition.x - egoPosition.x;
    if (dx < 24000.0f || dx > -24000.0f ||
        (dx = targetPosition.y - egoPosition.y, dx < 24000.0f) || dx > -24000.0f ||
        (dx = targetPosition.z - egoPosition.z, dx < 24000.0f) || dx > -24000.0f) {
        Array<int> *cargo = target->cargo;
        if (cargo != nullptr) {
            for (unsigned i = 0; i + 1 < cargo->size(); i += 2) {
                int amount = (*cargo)[i + 1];
                if (amount >= 1) {
                    hud->catchCargo((*cargo)[i], amount, false, false, false, true, false);
                    return;
                }
            }
        } else {
            hud->hudEvent(22, radar_level_player(self), 0);
        }
    }
}

static inline bool radar_draw_enemy_targets(Radar *self,
                                            Hud *hud,
                                            bool turretMode,
                                            bool suppressCandidate,
                                            int elapsed,
                                            bool &type10Threat,
                                            bool &alwaysEnemyThreat) {
    if (self->scannerAvailable == 0 || self->enemyTargets == nullptr) {
        return false;
    }

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    bool candidateFound = false;
    bool lockBlocked = suppressCandidate ||
                       self->lockedAsteroid != nullptr ||
                       self->lockedGasCloud != nullptr ||
                       self->field_0x1c != nullptr ||
                       self->lockedStation != nullptr ||
                       self->lockedPlanetTarget != nullptr;

    for (unsigned i = 0; i < self->enemyTargets->size(); ++i) {
        KIPlayer *target = (*self->enemyTargets)[i];
        bool hasDeadCargo = radar_enemy_has_dead_cargo(target);
        if (target->player->isActive() == 0 ||
            (target->isDying() && target->hasCargo == 0) ||
            target->field_0x74 != 0) {
            if (target->getType() == 10 && target->player->isActive() != 0 &&
                !target->isDead() && !target->isDying()) {
                ++self->field_0x1bc;
                type10Threat = true;
            }
            continue;
        }

        self->update(target);
        target->field_0x75 = 0;
        if ((target->isDead() || target->isDying()) && target->hasCargo == 0) {
            continue;
        }
        radar_accumulate_enemy_ambient_threat(self,
                                              target,
                                              hasDeadCargo,
                                              type10Threat,
                                              alwaysEnemyThreat);

        if (self->onScreen != 0) {
            target->field_0x76 = 1;
            radar_draw_enemy_health_emp_renderer(self, target, hasDeadCargo);

            PlayerEgo *player = radar_level_player(self);
            if (!player->isAutoPilot() && !player->isDockedToDockingPoint()) {
                bool candidateGate = (hasDeadCargo || target->countsAsEnemyExcludeFlag == 0) &&
                                     (!candidateFound || target->field_0x24 != 0);
                if (candidateGate) {
                    if (hasDeadCargo && target->isDead() && self->field_0x1c == nullptr && self->field_0x1ae != 0) {
                        self->candidateEnemy = target;
                        self->field_0x1c = target;
                        candidateFound = true;
                    } else if (radar_crosshair_contains(self, self->field_0x124)) {
                        target->field_0x75 = 1;
                        if ((hasDeadCargo && self->field_0x1ad != 0) ||
                            (self->field_0x1b0 != 0 && self->field_0x1a8 == 0)) {
                            if (self->candidateEnemy != target) {
                                self->field_0x198 = 0;
                            }
                            self->candidateEnemy = target;
                            candidateFound = true;
                        }
                    }
                }
            }
        } else {
            target->field_0x75 = 0;
            target->field_0x76 = 0;
            canvas->DrawImage2D(radar_image_slot(self, radar_enemy_blip_offset(self, target, hasDeadCargo)),
                                self->screenX,
                                self->screenY,
                                0x11u,
                                0x44u);
        }

        if (self->field_0x1af != 0 && self->field_0x1c == nullptr &&
            hasDeadCargo && target->isDead()) {
            self->candidateEnemy = target;
            self->field_0x1c = target;
            candidateFound = true;
        }
    }

    PlayerEgo *player = radar_level_player(self);
    if (player->isDockingToAsteroid() || player->isDockingToDockingPoint() || turretMode || lockBlocked) {
        return candidateFound;
    }

    KIPlayer *candidate = self->candidateEnemy;
    bool candidateDeadCargo = radar_enemy_has_dead_cargo(candidate);
    if (self->lockedEnemy != nullptr && (self->lockedEnemy->isDying() || self->lockedEnemy->isDead())) {
        self->lockedEnemy = nullptr;
    }
    if (!candidateFound || candidate == nullptr) {
        self->field_0x198 = 0;
        if ((candidateDeadCargo ? self->field_0x1c : self->lockedEnemy) == nullptr) {
            self->candidateEnemy = nullptr;
        }
        return false;
    }

    self->field_0x198 += elapsed;
    int threshold = (candidateDeadCargo || candidate->field_0x24 != 0) ? self->field_0x1b4 : self->field_0x1b8;
    if (self->field_0x198 > threshold) {
        if (candidateDeadCargo || candidate->field_0x24 != 0) {
            self->field_0x1c = candidate;
            if (self->field_0x1ad == 0) {
                hud->hudEvent(9, radar_level_player(self), 0);
                self->field_0x1c = nullptr;
            }
        } else if (self->lockedEnemy != candidate) {
            Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
            self->lockedEnemy = candidate;
            if (self->field_0x1a9 != 0) {
                radar_scan_enemy_cargo_for_hud(self, hud, candidate);
            }
        }
        self->field_0x198 = 0;
    }

    radar_draw_enemy_lock_progress(self, candidate, candidateDeadCargo);
    return true;
}

static GOF2_ALWAYS_INLINE bool radar_is_space_ambient_keep_event(int eventId) {
    unsigned normalDelta = static_cast<unsigned>(eventId - 127);
    if (normalDelta <= 0x19u && ((1u << normalDelta) & 0x23c1c8fu) != 0) {
        return true;
    }
    return static_cast<unsigned>(eventId - 2238) < 5u;
}

static GOF2_ALWAYS_INLINE bool radar_is_combat_ambient_keep_event(int eventId, bool alwaysEnemyThreat) {
    unsigned combatDelta = static_cast<unsigned>(eventId - 136);
    if (combatDelta > 0x0fu || ((1u << combatDelta) & 0xe071u) == 0) {
        return false;
    }
    return eventId == 151 || !alwaysEnemyThreat;
}

static GOF2_ALWAYS_INLINE int radar_race_space_ambient_track(int race) {
    static const int tracks[] = {0x86, 0x8b, 0x8a, 0x89};
    return tracks[race];
}

static GOF2_ALWAYS_INLINE int radar_space_ambient_track(Status *status) {
    Station *station = status->getStation();
    if (status->inAlienOrbit() || station->isAttackedByAliens()) {
        return 0x91;
    }
    if (status->getCurrentCampaignMission() == 1) {
        return 0x8f;
    }

    SolarSystem *system = status->getSystem();
    int race = system->getRace();
    int stationIndex = station->getIndex();
    if (stationIndex == 108) {
        return 0x92;
    }
    if (stationIndex == 101) {
        return 0x93;
    }
    if (status->inSupernovaSystem() != 0) {
        Mission *mission = status->getMission();
        if (mission == nullptr || mission->isEmpty() ||
            mission->getTargetStation() != status->getStation()->getIndex()) {
            return 0x94;
        }
        return status->getCurrentCampaignMission() < 106 ? 2241 : 2242;
    }
    if (status->inDeepScienceOrbit()) {
        return 0x98;
    }

    return radar_race_space_ambient_track(race);
}

static GOF2_ALWAYS_INLINE int radar_combat_ambient_track(Status *status,
                                             int threatCount,
                                             bool type10Threat,
                                             bool alwaysEnemyThreat) {
    Station *station = status->getStation();
    if (status->inAlienOrbit() ||
        station->isAttackedByAliens() ||
        status->getCurrentCampaignMission() == 16) {
        return 0x88;
    }

    if (alwaysEnemyThreat) {
        return 0x97;
    }
    if (threatCount <= 2) {
        return type10Threat ? 0x95 : 0x8c;
    }
    if (threatCount > 4) {
        return type10Threat ? 0x96 : 0x8e;
    }
    return type10Threat ? 0x95 : 0x8d;
}

static GOF2_ALWAYS_INLINE void radar_update_ambient_music(Radar *self,
                                               Status *status,
                                               bool type10Threat,
                                               bool alwaysEnemyThreat) {
    FModSound *sound = Globals::sound;
    int currentEvent = sound->currentMusicEvent;
    if (currentEvent == 143) {
        return;
    }

    if (self->field_0x1bc < 1) {
        self->field_0x54 = 0;
        if (status != nullptr && status->getCurrentCampaignMission() == 145) {
            self->field_0x1bc = 0;
            return;
        }
        int spaceEvent = sound->currentMusicEvent;
        if (radar_is_space_ambient_keep_event(spaceEvent)) {
            self->field_0x1bc = 0;
            return;
        }
        sound->stop(spaceEvent);
        int track = radar_space_ambient_track(status);
        sound->play(track, nullptr, nullptr, 0.0f);
        self->field_0x1bc = 0;
        return;
    }

    self->field_0x54 = 1;
    if (radar_is_combat_ambient_keep_event(currentEvent, alwaysEnemyThreat)) {
        self->field_0x1bc = 0;
        return;
    }
    sound->stop(currentEvent);
    int track = radar_combat_ambient_track(status, self->field_0x1bc, type10Threat, alwaysEnemyThreat);
    sound->play(track, nullptr, nullptr, 0.0f);
    self->field_0x1bc = 0;
}

static GOF2_ALWAYS_INLINE bool radar_target_has_dead_cargo(KIPlayer *target) {
    return target != nullptr &&
           target->hasCargo != 0 &&
           (target->isDead() || target->isDying());
}

static GOF2_ALWAYS_INLINE void radar_play_lock_cue(Radar *self) {
    if (self->field_0x218_byte == 0 && Globals::sound->isPlaying(0) == 0) {
        Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
    }
}

static GOF2_ALWAYS_INLINE void radar_draw_delayed_lock_blip(Radar *self,
                                                            int timer,
                                                            int lockTime,
                                                            KIPlayer *candidate,
                                                            KIPlayer *lockedTarget) {
    if (timer <= 500) {
        return;
    }

    int frameMax = self->blipSprite->getRawFrameCount() - 1;
    if (candidate == nullptr || candidate == lockedTarget) {
        self->blipSprite->setFrame(frameMax);
        self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                              static_cast<int>(PlayerEgo::crosshairPos.y));
        self->blipSprite->draw(1.0f, 1.0f);
        return;
    }

    int frame = static_cast<int>((static_cast<float>(timer - 500) / static_cast<float>(lockTime - 500)) *
                                 static_cast<float>(frameMax));
    if (self->blipSprite->getRawFrameCount() - 1 <= frame) {
        return;
    }
    self->blipSprite->setFrame(frame);
    self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                          static_cast<int>(PlayerEgo::crosshairPos.y));
    self->blipSprite->draw(1.0f, 1.0f);
}

static inline int radar_distance_to_player(Radar *self, KIPlayer *target) {
    if (self == nullptr || target == nullptr) {
        return 999999;
    }

    Vector delta = AbyssEngine::AEMath::operator-(target->getPosition(), radar_level_player(self)->getPosition());
    return static_cast<int>(AbyssEngine::AEMath::VectorLength(delta));
}

static GOF2_ALWAYS_INLINE void radar_draw_distance_label(Radar *self, KIPlayer *target, int x, int y) {
    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    if (self == nullptr || target == nullptr || canvas == nullptr) {
        return;
    }

    self->lockLabel = self->calcDistance(target->posX,
                                         target->posY,
                                         target->posZ,
                                         self->cameraPosX,
                                         self->cameraPosY,
                                         self->cameraPosZ);
    canvas->DrawString(radar_font(), self->lockLabel, x, y, false);
}

static inline void radar_update_gas_cloud_sparks(Radar *self,
                                                 Status *status,
                                                 PlayerGasCloud *cloud,
                                                 bool turretMode) {
    if (self == nullptr || status == nullptr || cloud == nullptr) {
        return;
    }

    auto *sparks = static_cast<Array<AEGeometry *> *>(cloud->getSparks());
    Ship *ship = status->getShip();
    Item *scope = ship != nullptr ? ship->getFirstEquipmentOfSort(35) : nullptr;
    if (sparks == nullptr || scope == nullptr) {
        return;
    }

    float maxDistance = static_cast<float>(scope->getAttribute(51));
    for (unsigned i = 0; i < sparks->size(); ++i) {
        AEGeometry *spark = (*sparks)[i];
        if (spark == nullptr) {
            cloud->setSparkInSight(static_cast<int>(i), false);
            continue;
        }

        Vector sparkPosition = spark->getPosition();
        self->update(sparkPosition);
        bool inSight = false;
        if (self->onScreen != 0 && radar_crosshair_contains(self, self->turretScopeHalfWidth)) {
            Vector delta = AbyssEngine::AEMath::operator-(sparkPosition, radar_level_player(self)->getPosition());
            float distance = AbyssEngine::AEMath::VectorLength(delta);
            inSight = distance <= maxDistance;
            if (inSight && cloud->isSparkAlive(static_cast<int>(i))) {
                self->plasmaInRange = 1;
            }
        }
        cloud->setSparkInSight(static_cast<int>(i), turretMode && inSight);
    }
}

static inline bool radar_draw_gas_cloud_targets(Radar *self,
                                                Status *status,
                                                bool turretMode,
                                                bool suppressCandidate,
                                                int elapsed) {
    if (self == nullptr || status == nullptr || self->gasCloudTargets == nullptr) {
        return false;
    }

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    Ship *ship = status->getShip();
    Item *gasScanner = ship != nullptr ? ship->getFirstEquipmentOfSort(33) : nullptr;
    bool hasGasScanner = gasScanner != nullptr && gasScanner->getAttribute(57) == 1;
    bool drawOffscreenGas = gasScanner != nullptr && gasScanner->getAttribute(58) == 1;
    bool lockBlocked = suppressCandidate ||
                       self->lockedEnemy != nullptr ||
                       self->lockedAsteroid != nullptr ||
                       self->field_0x1c != nullptr ||
                       self->lockedStation != nullptr ||
                       self->lockedPlanetTarget != nullptr;
    bool candidateFound = false;

    for (unsigned i = 0; i < self->gasCloudTargets->size(); ++i) {
        auto *cloud = static_cast<PlayerGasCloud *>((*self->gasCloudTargets)[i]);
        if (cloud == nullptr) {
            continue;
        }

        radar_update_gas_cloud_sparks(self, status, cloud, turretMode);

        if (cloud->player == nullptr || cloud->player->isActive() == 0 || cloud->isDying() || !hasGasScanner) {
            continue;
        }

        self->update(cloud);
        cloud->field_0x75 = 0;
        if (cloud->isDead() || cloud->isDying()) {
            continue;
        }

        if (self->onScreen != 0) {
            KIPlayer *previousLock = self->lockedGasCloud;
            cloud->field_0x76 = 1;
            if (canvas != nullptr) {
                canvas->DrawImage2D(radar_image_slot(self, 0x8c),
                                    self->screenX,
                                    self->screenY,
                                    0x11u,
                                    0x44u);
            }
            cloud->field_0x73 = 0;

            if (previousLock == cloud) {
                radar_draw_distance_label(self,
                                          cloud,
                                          self->screenX - self->halfScreenHeight,
                                          self->screenY + self->halfScreenWidth);
            }

            if (!radar_level_player(self)->isAutoPilot() && !lockBlocked && !turretMode && !candidateFound &&
                self->field_0x1b0 != 0 && self->field_0x1a8 == 0 &&
                radar_crosshair_contains(self, self->field_0x124)) {
                cloud->field_0x75 = 1;
                if (self->candidateGasCloud != cloud) {
                    self->gasLockTimer = 0;
                }
                self->candidateGasCloud = cloud;
                candidateFound = true;
            }
        } else if (drawOffscreenGas && canvas != nullptr) {
            cloud->field_0x73 = 0;
            cloud->field_0x76 = 0;
            canvas->DrawImage2D(radar_image_slot(self, 0xc0),
                                self->screenX,
                                self->screenY,
                                0x11u,
                                0x44u);
        }
    }

    if (turretMode || suppressCandidate) {
        return candidateFound;
    }

    KIPlayer *resolvedLock = nullptr;
    if (self->lockedGasCloud != nullptr) {
        if (self->lockedGasCloud->isDead()) {
            self->lockedGasCloud = nullptr;
        } else {
            resolvedLock = self->lockedGasCloud;
        }
        if (resolvedLock == self->candidateGasCloud) {
            self->field_0x218_byte = 1;
        }
        self->lockedGasCloud = nullptr;
    }

    if (!candidateFound || self->candidateGasCloud == nullptr) {
        self->gasLockTimer = 0;
        self->candidateGasCloud = nullptr;
        return false;
    }

    int lockThreshold = self->field_0x1b8 - 200;
    self->gasLockTimer += elapsed;
    if (self->gasLockTimer > lockThreshold) {
        radar_play_lock_cue(self);
        resolvedLock = self->candidateGasCloud;
        self->lockedGasCloud = self->candidateGasCloud;
    }

    radar_draw_delayed_lock_blip(self,
                                 self->gasLockTimer,
                                 self->field_0x1b8,
                                 self->candidateGasCloud,
                                 resolvedLock);
    return true;
}

static inline bool radar_asteroid_center_range_enabled(Radar *self) {
    if (self == nullptr || self->field_0x1aa == 0 || self->level == nullptr) {
        return false;
    }
    return self->level->isInAsteroidCenterRange(radar_level_player(self)->getPosition()) != 0;
}

static inline void radar_draw_asteroid_quality_indicator(Radar *self,
                                                         bool centerRange,
                                                         PlayerAsteroid *asteroid) {
    if (self == nullptr || asteroid == nullptr || self->qualitySprite == nullptr || !centerRange) {
        return;
    }
    if (asteroid->getQualityFrameIndex() != 0) {
        return;
    }
    self->qualitySprite->setFrame(0);
    self->qualitySprite->setPosition(self->screenX, self->screenY);
    self->qualitySprite->draw(1.0f, 1.0f);
}

static inline void radar_dead_cargo_directional_overlay_pos(Radar *self, int &x, int &y) {
    x = self->screenX;
    y = self->screenY;

    bool right = self->offscreenFlagRight != 0 || self->screenX > self->centerX;
    bool leftOrVertical = self->offscreenFlagLeftOrVertical != 0 || self->screenX < self->centerX;
    bool bottom = self->offscreenFlagBottom != 0 || self->screenY > self->centerY;
    bool top = self->offscreenFlagTop != 0 || self->screenY < self->centerY;

    if (right && !bottom && !top) {
        x += 11;
        return;
    }

    if (leftOrVertical) {
        if (bottom) {
            y += 11;
        } else {
            if (!top) {
                x -= 11;
            } else {
                y -= 11;
            }
        }
        return;
    }

    if (bottom) {
        y += 11;
    } else if (top) {
        y -= 11;
    }
}

static GOF2_ALWAYS_INLINE void radar_draw_asteroid_dead_cargo_marker(Radar *self, bool onScreen) {
    if (self == nullptr) {
        return;
    }
    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    if (canvas == nullptr) {
        return;
    }

    if (onScreen) {
        canvas->DrawImage2D(radar_image_slot(self, 0x9c),
                            self->screenX,
                            self->screenY,
                            0x11u,
                            0x44u);
        return;
    }

    canvas->DrawImage2D(radar_image_slot(self, 0x8c),
                        self->screenX,
                        self->screenY,
                        0x11u,
                        0x44u);

    int overlayX = 0;
    int overlayY = 0;
    radar_dead_cargo_directional_overlay_pos(self, overlayX, overlayY);
    canvas->DrawImage2D(radar_image_slot(self, 0xac),
                        overlayX,
                        overlayY,
                        0x11u,
                        0x44u);
}

static inline bool radar_draw_asteroid_targets(Radar *self,
                                               Hud *hud,
                                               bool turretMode,
                                               bool suppressCandidate,
                                               int elapsed) {
    if (self == nullptr) {
        return false;
    }

    if (self->radarSlots != nullptr) {
        for (int i = 0; i < 5; ++i) {
            self->radarSlots[i] = -1;
        }
    }

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    PlayerEgo *player = radar_level_player(self);
    if (player->isInDockingProcedure() || self->asteroidTargets == nullptr) {
        if (player->isDockingToAsteroid() && self->lockedAsteroid != nullptr) {
            radar_draw_distance_label(self,
                                      self->lockedAsteroid,
                                      static_cast<int>(PlayerEgo::crosshairPos.x) - self->halfScreenHeight,
                                      static_cast<int>(PlayerEgo::crosshairPos.y) + self->halfScreenWidth);
        }
        return false;
    }

    bool lockBlocked = suppressCandidate ||
                       self->lockedEnemy != nullptr ||
                       self->lockedGasCloud != nullptr ||
                       self->field_0x1c != nullptr ||
                       self->lockedStation != nullptr ||
                       self->lockedPlanetTarget != nullptr;
    bool centerRangeQuality = radar_asteroid_center_range_enabled(self);
    bool candidateFound = false;
    int slotCount = 0;

    for (unsigned i = 0; i < self->asteroidTargets->size(); ++i) {
        auto *asteroid = static_cast<PlayerAsteroid *>((*self->asteroidTargets)[i]);
        if (asteroid == nullptr) {
            continue;
        }

        bool deadCargo = radar_target_has_dead_cargo(asteroid);
        if ((asteroid->isDead() || asteroid->isDying()) && !deadCargo) {
            continue;
        }

        self->update(asteroid);

        if (self->onScreen != 0) {
            if (!radar_level_player(self)->isAutoPilot() && !lockBlocked && !turretMode && !candidateFound &&
                asteroid->isMinable() != 0 && self->field_0x1a8 == 0) {
                if (deadCargo && self->field_0x1c == nullptr && self->field_0x1ae != 0) {
                    self->candidateAsteroid = asteroid;
                    self->field_0x1c = asteroid;
                    candidateFound = true;
                } else if (radar_crosshair_contains(self, self->field_0x124)) {
                    asteroid->field_0x75 = 1;
                    if (asteroid->asteroidFlag == 0 && self->field_0x1b0 != 0 &&
                        self->radarSlots != nullptr && slotCount < 5) {
                        self->radarSlots[slotCount++] = static_cast<int>(i);
                    }
                }
            }

            radar_draw_asteroid_quality_indicator(self, centerRangeQuality, asteroid);
            if (deadCargo) {
                radar_draw_asteroid_dead_cargo_marker(self, true);
            }
        } else if (deadCargo) {
            radar_draw_asteroid_dead_cargo_marker(self, false);
        }

        if (self->field_0x1af != 0 && self->field_0x1c == nullptr && deadCargo && asteroid->isDead()) {
            self->candidateAsteroid = asteroid;
            self->field_0x1c = asteroid;
            candidateFound = true;
        }
    }

    int nearestIndex = -1;
    int nearestDistance = 999999;
    if (self->radarSlots != nullptr) {
        for (int i = 0; i < 5; ++i) {
            int targetIndex = self->radarSlots[i];
            if (targetIndex < 0 ||
                static_cast<unsigned>(targetIndex) >= self->asteroidTargets->size()) {
                continue;
            }
            KIPlayer *target = (*self->asteroidTargets)[static_cast<unsigned>(targetIndex)];
            int distance = radar_distance_to_player(self, target);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestIndex = targetIndex;
            }
        }
    }

    if (nearestIndex >= 0) {
        KIPlayer *nearest = (*self->asteroidTargets)[static_cast<unsigned>(nearestIndex)];
        if (self->candidateAsteroid != nearest) {
            self->asteroidLockTimer = 0;
        }
        self->candidateAsteroid = nearest;
        candidateFound = true;
    }

    if (turretMode || suppressCandidate) {
        return candidateFound;
    }

    KIPlayer *resolvedLock = nullptr;
    if (self->lockedAsteroid != nullptr) {
        if (self->lockedAsteroid->isDead()) {
            self->lockedAsteroid = nullptr;
        } else {
            resolvedLock = self->lockedAsteroid;
        }
        if (resolvedLock == self->candidateAsteroid) {
            self->field_0x218_byte = 1;
        }
        self->lockedAsteroid = nullptr;
    }

    KIPlayer *candidate = self->candidateAsteroid;
    bool candidateDeadCargo = radar_target_has_dead_cargo(candidate);
    if (!candidateFound || candidate == nullptr || radar_level_player(self)->isDockingToAsteroid()) {
        self->asteroidLockTimer = 0;
        if ((candidateDeadCargo ? self->field_0x1c : self->lockedAsteroid) == nullptr) {
            self->candidateAsteroid = nullptr;
        }
        return false;
    }

    int lockThreshold = self->field_0x1b8 - 200;
    self->asteroidLockTimer += elapsed;
    if (self->asteroidLockTimer > lockThreshold) {
        if (candidateDeadCargo) {
            if (self->field_0x1ad != 0) {
                self->field_0x1c = candidate;
            } else {
                hud->hudEvent(9, radar_level_player(self), 0);
            }
        } else if (self->field_0x1ac == 0) {
            hud->hudEvent(20, radar_level_player(self), 0);
        } else {
            radar_play_lock_cue(self);
            resolvedLock = candidate;
            self->lockedAsteroid = candidate;
        }
    }

    radar_draw_delayed_lock_blip(self,
                                 self->asteroidLockTimer,
                                 self->field_0x1b8,
                                 candidate,
                                 candidateDeadCargo ? self->field_0x1c : resolvedLock);
    return true;
}

static void radar_wire_guns(Radar *radar, Level *level, Array<AbstractGun *> *guns, bool playerOwned) {
    if (radar == nullptr || guns == nullptr) {
        return;
    }

    PlayerEgo *ego = playerOwned && level != nullptr ? level->getPlayer() : nullptr;
    for (unsigned int i = 0; i < guns->size(); ++i) {
        AbstractGun *gun = (*guns)[i];
        if (gun == nullptr) {
            continue;
        }
        if (gun->isRocketGun()) {
            static_cast<RocketGun *>(gun)->setRadar(radar);
        }
        if (playerOwned && gun->isBombGun()) {
            static_cast<BombGun *>(gun)->setPlayer(ego);
        }
        if (playerOwned && gun->isMineGun()) {
            static_cast<MineGun *>(gun)->setPlayer(ego);
        }
    }
}

int Radar::getTurretScopeWidth() {
    return this->turretScopeHalfWidth << 1;
}

Radar::Radar(Level *level) {
    this->field_0x28 = 0;
    this->field_0x2c = 0;
    this->field_0x30 = 0;
    this->field_0x44 = 0;
    this->field_0x134 = 0;
    this->field_0x138 = 0;
    this->field_0x13c = 0;
    this->field_0x140 = 0;
    this->field_0x144 = 0;
    this->field_0x148 = 0;
    this->field_0x14c = 0;
    this->field_0x150 = 0;
    this->field_0x174 = 0;
    this->field_0x178 = 0;
    this->field_0x17c = 0;
    this->field_0x180 = 0;
    this->field_0x184 = 0;
    this->field_0x164 = 0;
    this->field_0x168 = 0;
    this->field_0x16c = 0;
    this->field_0x170 = 0;
    this->field_0x198 = 0;
    this->field_0x19c = 0;
    this->field_0x1a0 = 0;
    this->field_0x1a4 = 0;
    this->radarPosX = 0;
    this->radarPosY = 0;
    this->radarPosZ = 0;
    this->field_0x160 = 0;

    this->lockedEnemy = nullptr;
    this->field_0x8 = nullptr;
    this->field_0x14 = 0;
    this->enabled = 1;
    this->field_0x58 = 0;
    this->labelStrings = nullptr;
    this->lockedAsteroid = nullptr;
    this->field_0x10 = 0;
    this->field_0x18 = 0;
    this->lockedGasCloud = nullptr;
    this->field_0x3c = 0;
    this->planetDockIndex = 0;
    this->field_0x1b0 = 0;
    this->field_0x20c = 0;
    this->field_0x1b4 = 0;
    this->field_0x1b8 = 0;
    this->field_0x1bc = 0;
    this->field_0x1c0 = 0;
    this->field_0x54 = 0;
    this->plasmaInRange = 0;
    this->field_0x11d = 0;
    this->field_0x11e = 0;
    this->field_0x11f = 0;
    this->field_0x120 = 0;
    this->onScreen = 0;
    this->field_0x1a8 = 0;
    this->level = level;

    void *layout = Globals::layout;
    int width = layout_i32(layout, 0xac);
    int height = layout_i32(layout, 0xa8);
    this->screenWidth = width;
    this->halfScreenWidth = width >> 1;
    this->screenHeight = height;
    this->halfScreenHeight = height >> 1;
    this->originX = layout_i32(layout, 0xa0);
    this->originY = layout_i32(layout, 0xa4);

    this->centerX = Globals::w / 2;
    this->centerY = Globals::h / 2;

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    radar_create_image(canvas, 0x4c7, this->radarImage);
    int imageWidth = canvas->GetImage2DWidth(this->radarImage);
    int imageHeight = canvas->GetImage2DHeight(this->radarImage);
    this->imageWidth = imageWidth;
    this->imageHeight = imageHeight;
    this->imageWidthSq = imageWidth * imageWidth;
    this->imageHeightSq = imageHeight * imageHeight;
    this->weightX = 1.0f / static_cast<float>(imageWidth * imageWidth);
    this->weightY = 1.0f / static_cast<float>(imageHeight * imageHeight);

    Array<String *> *strings = new Array<String *>();
    ArraySetLength<String *>(4, *strings);
    this->labelStrings = strings;

    Status *status = Globals::status;
    Station *station = status->getStation();
    String stationLabel;
    if (status->inAlienOrbit()) {
        stationLabel = radar_get_text(415);
    } else {
        stationLabel = station->getName();
    }

    String stationSuffix("");
    if (station->getIndex() != 101) {
        stationSuffix = String(" ") + radar_get_text(136);
    }
    (*strings)[0] = radar_alloc_string(stationLabel + stationSuffix);
    if (status->inAlienOrbit() && status->dlc1Won() && !status->inEmptyOrbit()) {
        KIPlayer *primaryLandmark = (*level->getLandmarks())[0];
        (*strings)[0]->Set(primaryLandmark->name.data);
    }
    (*strings)[1] = radar_alloc_string(radar_get_text(547));
    (*strings)[2] = radar_alloc_string(String(""));
    (*strings)[3] = radar_alloc_string(radar_get_text(545));

    radar_wire_guns(this, level, level->getPlayerGuns(), true);
    radar_wire_guns(this, level, level->getEnemyGuns(), false);

    radar_create_image(canvas, 0x4d4, radar_image_slot(this, 0xd4));
    radar_create_image(canvas, 0x4d9, radar_image_slot(this, 0xd0));
    radar_create_image(canvas, 0x4d6, radar_image_slot(this, 0xd8));
    radar_create_image(canvas, 0x4d7, radar_image_slot(this, 0xdc));
    radar_create_image(canvas, 0x4d3, radar_image_slot(this, 0xe4));
    radar_create_image(canvas, 0x4da, radar_image_slot(this, 0xe0));
    radar_create_image(canvas, 0x4d5, radar_image_slot(this, 0xe8));
    radar_create_image(canvas, 0x4d8, radar_image_slot(this, 0xec));
    this->lockPanelWidth = canvas->GetImage2DWidth(radar_image_slot(this, 0xd4));
    this->lockPanelHeight = canvas->GetImage2DHeight(radar_image_slot(this, 0xd4));

    radar_create_image(canvas, 0x454, radar_image_slot(this, 0xc8));
    radar_create_image(canvas, 0x455, radar_image_slot(this, 0xc4));
    radar_create_image(canvas, 0x4dc, radar_image_slot(this, 0x74));
    radar_create_image(canvas, 0x4cb, radar_image_slot(this, 0x78));
    radar_create_image(canvas, 0x4c9, radar_image_slot(this, 0x98));
    radar_create_image(canvas, 0x4db, radar_image_slot(this, 0x5c));
    radar_create_image(canvas, 0x4cc, radar_image_slot(this, 0x60));
    radar_create_image(canvas, 0x4c8, radar_image_slot(this, 0x90));
    radar_create_image(canvas, 0x4d2, radar_image_slot(this, 0x64));
    radar_create_image(canvas, 0x4cd, radar_image_slot(this, 0x68));
    radar_create_image(canvas, 0x4ca, radar_image_slot(this, 0x94));
    radar_create_image(canvas, 0x4f0, radar_image_slot(this, 0x6c));
    radar_create_image(canvas, 0x4ef, radar_image_slot(this, 0x70));
    radar_create_image(canvas, 0x4f2, radar_image_slot(this, 0x88));
    radar_create_image(canvas, 0x4f1, radar_image_slot(this, 0x8c));
    radar_create_image(canvas, 0x4c8, radar_image_slot(this, 0x7c));
    radar_create_image(canvas, 0x4ca, radar_image_slot(this, 0x80));
    radar_create_image(canvas, 0x4c9, radar_image_slot(this, 0x84));
    radar_create_image(canvas, 0x4f2, radar_image_slot(this, 0x9c));
    radar_create_image(canvas, 0x4f1, radar_image_slot(this, 0xa4));
    radar_image_slot(this, 0xa0) = radar_image_slot(this, 0xa4);
    radar_create_image(canvas, 0x451, radar_image_slot(this, 0xa8));
    radar_create_image(canvas, 0x44f, radar_image_slot(this, 0xb0));
    radar_create_image(canvas, 0x44c, radar_image_slot(this, 0xac));
    radar_create_image(canvas, 0x44d, radar_image_slot(this, 0xb4));
    radar_create_image(canvas, 0x450, radar_image_slot(this, 0xb8));
    radar_create_image(canvas, 0x453, radar_image_slot(this, 0xbc));
    radar_create_image(canvas, 0x1f62, radar_image_slot(this, 0xc0));
    radar_create_image(canvas, 0x4c4, radar_image_slot(this, 0xcc));

    this->lockPanelX = Globals::w / 2 - canvas->GetImage2DWidth(radar_image_slot(this, 0xcc)) / 2;
    this->lockPanelY = layout_i32(layout, 0xb0);

    uint32_t *raceFrames = new uint32_t[10]();
    radar_create_image(canvas, 0x4a1, reinterpret_cast<int &>(raceFrames[0]));
    radar_create_image(canvas, 0x49c, reinterpret_cast<int &>(raceFrames[1]));
    radar_create_image(canvas, 0x49e, reinterpret_cast<int &>(raceFrames[3]));
    radar_create_image(canvas, 0x49f, reinterpret_cast<int &>(raceFrames[2]));
    radar_create_image(canvas, 0x4a0, reinterpret_cast<int &>(raceFrames[8]));
    radar_create_image(canvas, 0x49d, reinterpret_cast<int &>(raceFrames[9]));
    int raceFrameHeight = canvas->GetImage2DHeight(raceFrames[0]);
    this->raceSprite = new Sprite(raceFrames, 10, raceFrameHeight, raceFrameHeight);

    unsigned int blipImage = 0;
    canvas->Image2DCreate(0x456, blipImage);
    int blipSize = canvas->GetImage2DHeight(blipImage);
    this->blipSprite = new Sprite(blipImage, blipSize, blipSize);
    this->blipSprite->defineReferencePixel(blipSize / 2, blipSize / 2);

    unsigned int qualityImage = 0;
    canvas->Image2DCreate(0x44e, qualityImage);
    this->qualitySprite = new Sprite(qualityImage,
                                     layout_i32(layout, 0xb8),
                                     layout_i32(layout, 0xb4));

    Ship *ship = Globals::status->getShip();
    Item *scanner = ship->getFirstEquipmentOfSort(19);
    Item *repairOrScanner = ship->getFirstEquipmentOfSort(17);
    Item *plasma = ship->getFirstEquipmentOfSort(13);
    this->field_0x1ac = scanner != nullptr;
    if (repairOrScanner != nullptr) {
        this->scannerAvailable = 1;
        this->field_0x1a9 = repairOrScanner->getAttribute(31) == 1;
        this->field_0x1aa = repairOrScanner->getAttribute(30) == 1;
        this->field_0x1b8 = repairOrScanner->getAttribute(29);
    } else {
        this->scannerAvailable = 0;
        this->field_0x1a9 = 0;
        this->field_0x1aa = 0;
        this->field_0x1b8 = 8000;
    }
    if (plasma != nullptr) {
        this->field_0x1ad = 1;
        this->field_0x1ae = plasma->getAttribute(23) == 1;
        this->field_0x1b4 = plasma->getAttribute(24);
        this->field_0x1af = plasma->getAttribute(23) == 2;
    } else {
        this->field_0x1ad = 0;
        this->field_0x1ae = 0;
        this->field_0x1af = 0;
        this->field_0x1b4 = 0;
    }
    this->field_0x1bc = 0;
    this->field_0x1b0 = 1;
    this->enabled = 1;

    int *radarSlots = new int[5];
    this->radarSlots = radarSlots;
    for (int i = 0; i < 5; ++i) {
        radarSlots[i] = -1;
    }

    this->field_0x124 = Globals::w / 16;
    this->field_0x128 = Globals::w / 6;
    this->turretScopeHalfWidth = Globals::w / 8;

    Item *scope = ship->getFirstEquipmentOfSort(35);
    if (scope != nullptr && scope->getAttribute(50) != 0) {
        this->turretScopeHalfWidth = static_cast<int>(
            (static_cast<float>(scope->getAttribute(50)) / 100.0f) *
            static_cast<float>(this->turretScopeHalfWidth));
    }

    Item *reservation = ship->getFirstEquipmentOfSort(37);
    if (reservation != nullptr) {
        this->players = new Array<KIPlayer *>();
        ArraySetLength<KIPlayer *>(reservation->getAttribute(55), *this->players);
    }
}

Radar::~Radar() {
    Array<KIPlayer *> *players = this->players;
    if (players != nullptr) {
        delete players;
    }
    this->players = nullptr;
}

int Radar::hasScanner() {
    return this->scannerAvailable;
}

int Radar::isPlasmaInRange() {
    return this->plasmaInRange;
}

bool Radar::stationLocked() {
    void *station = this->lockedStation;
    if (station != nullptr) {
        return station_u8(station, 0x71) != 0;
    }
    return false;
}

KIPlayer *Radar::getLockedEnemy() {
    return this->lockedEnemy;
}

KIPlayer *Radar::getLockedAsteroid() {
    return this->lockedAsteroid;
}

KIPlayer *Radar::getLockedGasCloud() {
    return this->lockedGasCloud;
}

int Radar::unlockAsteroid() {
    this->lockedAsteroid = nullptr;
    return static_cast<int>(reinterpret_cast<uintptr_t>(this));
}

int Radar::getPlanetDockIndex() {
    Status *status = Globals::status;
    SolarSystem *system = status->getSystem();
    auto *stations = reinterpret_cast<Array<int> *>(system->getStations());
    return (*stations)[this->planetDockIndex];
}

void Radar::update(KIPlayer *player) {
#if defined(__arm__)
    // Match the Android hidden-return shape: vtable +40 writes the Vector into the stack scratch.
    int positionWords[4];
    auto *positionPtr = reinterpret_cast<Vector *>(positionWords);
    using GetPositionFn = void (*)(Vector *, KIPlayer *);
    void **vtable = *reinterpret_cast<void ***>(player);
    reinterpret_cast<GetPositionFn>(vtable[10])(positionPtr, player);
    Vector &position = *positionPtr;
#else
    Vector position = player->getPosition();
#endif
    update(position);
}

void Radar::update(Vector value) {
    Vector screen = value;
    Vector transformed = AbyssEngine::AEMath::MatrixTransformVector(this->transform, screen);
    Vector *current = reinterpret_cast<Vector *>(&this->radarPosX);
    *current = transformed;

    this->radarPosY = -this->radarPosY;
    this->radarPosZ = -this->radarPosZ;

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    int visible = canvas->GetScreenPosition(screen, screen);
    this->onScreen = static_cast<uint8_t>(visible);

    int screenX = static_cast<int>(screen.x);
    this->screenX = screenX;
    int screenY = static_cast<int>(screen.y);
    this->screenY = screenY;

    if (visible == 0) {
        *current = this->elipsoidIntersect(screenX, screenY, *current);
        this->screenX = static_cast<int>(current->x);
        this->screenY = static_cast<int>(current->y);
    }
}

int Radar::draw(Player *, Hud *hud, int elapsed) {
    if (this->enabled == 0) {
        return 0;
    }

    this->field_0x218_byte = 0;
    this->plasmaInRange = 0;

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    canvas->SetColor(static_cast<unsigned int>(-1));

    Status *status = Globals::status;
    Mission *mission = status->getMission();
    bool missionTargetFilter = false;
    if (!mission->isEmpty()) {
        int type = mission->getType();
        missionTargetFilter = type != 11 && type != 0 && type != 13 && type != 171 && type != 172;
    }
    PlayerEgo *ego = this->level->getPlayer();
    bool turretMode = ego->isInTurretMode() != 0;
    bool alienOrbit = status->inAlienOrbit();
    int currentMission = status->getCurrentCampaignMission();

    this->enemyTargets = this->level->getEnemies();
    this->landmarkTargets = this->level->getLandmarks();
    this->playerRoute = this->level->getPlayerRoute();
    this->asteroidTargets = this->level->getAsteroids();
    this->gasCloudTargets = this->level->getGasClouds();
    StarSystem *starSystem = this->level->getStarSystem();
    this->planetTargets = static_cast<Array<KIPlayer *> *>(starSystem->getPlanetTargets());

    unsigned int camera = canvas->CameraGetCurrent();
    auto *local = static_cast<Matrix *>(canvas->CameraGetLocal(camera));
    this->transform = *local;
    this->cameraPosX = this->transform.m[3];
    this->cameraPosY = this->transform.m[7];
    this->cameraPosZ = this->transform.m[11];
    this->transform = AbyssEngine::AEMath::MatrixGetInverse(this->transform);

    bool planetApproachTarget = false;
    if (ego->goingToPlanet()) {
        if (alienOrbit) {
            planetApproachTarget = ego->isDockingToPlanet() == 0;
        } else if (ego->isDockingToPlanet() == 0) {
            SolarSystem *system = status->getSystem();
            auto *stations = reinterpret_cast<Array<int> *>(system->getStations());
            uint32_t stationEnum = system->getStationEnumIndex((*stations)[this->planetDockIndex]);
            planetApproachTarget = reinterpret_cast<KIPlayer *>(
                static_cast<intptr_t>(ego->getAutoPilotTarget())) == (*this->planetTargets)[stationEnum];
        }
    }
    this->field_0x1a8 = static_cast<uint8_t>(planetApproachTarget);

    canvas->DrawImage2D(this->radarImage,
                        this->centerX - this->imageWidth,
                        this->centerY - this->imageHeight);
    canvas->DrawImage2D(this->radarImage,
                        this->centerX,
                        this->centerY - this->imageHeight,
                        1u);
    canvas->DrawImage2D(this->radarImage,
                        this->centerX - this->imageWidth,
                        this->centerY,
                        2u);
    canvas->DrawImage2D(this->radarImage,
                        this->centerX,
                        this->centerY,
                        3u);

    Waypoint *waypoint = this->playerRoute != nullptr ? this->playerRoute->getWaypoint() : nullptr;
    if (waypoint != nullptr) {
        this->update(waypoint);
        this->lockLabel = this->calcDistance(static_cast<float>(waypoint->x),
                                             static_cast<float>(waypoint->y),
                                             static_cast<float>(waypoint->z),
                                             this->cameraPosX,
                                             this->cameraPosY,
                                             this->cameraPosZ);
        if (this->onScreen != 0) {
            canvas->DrawImage2D(radar_image_slot(this, 0x6c),
                                this->screenX,
                                this->screenY,
                                0x11u,
                                0x44u);
            canvas->DrawString(radar_font(),
                               this->lockLabel,
                               this->screenX - this->halfScreenHeight,
                               this->screenY + this->halfScreenWidth,
                               false);
        } else if (this->scannerAvailable != 0) {
            canvas->DrawImage2D(radar_image_slot(this, 0x70),
                                this->screenX,
                                this->screenY,
                                0x11u,
                                0x44u);
        }
    }

    bool stationCandidateActive = radar_draw_landmark_station_markers(this,
                                                                      status,
                                                                      turretMode,
                                                                      alienOrbit,
                                                                      currentMission,
                                                                      elapsed);

    bool planetCandidateActive = radar_draw_planet_station_markers(this,
                                                                   hud,
                                                                   status,
                                                                   missionTargetFilter,
                                                                   turretMode,
                                                                   stationCandidateActive,
                                                                   elapsed);

    // Keep the enemy pass in draw: the recovered ARM body owns its candidate,
    // timer, position-scratch and HUD work in one local control-flow region.
    bool type10Threat = false;
    bool alwaysEnemyThreat = false;
    bool enemyCandidateActive = false;
    bool enemyLockBlocked = stationCandidateActive || planetCandidateActive ||
                            this->lockedAsteroid != nullptr ||
                            this->lockedGasCloud != nullptr ||
                            this->field_0x1c != nullptr ||
                            this->lockedStation != nullptr ||
                            this->lockedPlanetTarget != nullptr;

    if (this->scannerAvailable != 0 && this->enemyTargets != nullptr) {
        Vector &enemyPositionScratch = *reinterpret_cast<Vector *>(&this->field_0x16c);
        Vector &egoPositionScratch = *reinterpret_cast<Vector *>(&this->field_0x178);

        for (unsigned i = 0; i < this->enemyTargets->size(); ++i) {
            KIPlayer *target = (*this->enemyTargets)[i];
            Player *targetPlayer = target->player;
            bool deadCargo = target->hasCargo != 0 && (target->isDead() || target->isDying());

            if (targetPlayer->isActive() == 0 ||
                (target->isDying() && target->hasCargo == 0) ||
                target->field_0x74 != 0) {
                if (target->getType() == 10 && targetPlayer->isActive() != 0 &&
                    !target->isDead() && !target->isDying()) {
                    ++this->field_0x1bc;
                    type10Threat = true;
                }
                continue;
            }

            this->update(target);
            target->field_0x75 = 0;
            if ((target->isDead() || target->isDying()) && target->hasCargo == 0) {
                continue;
            }

            if (!deadCargo && target->stealFlagByte == 0 &&
                target->countsAsEnemyExcludeFlag == 0 && targetPlayer->enemyFlagsLo != 0) {
                ++this->field_0x1bc;
                if (target->getType() == 10) {
                    type10Threat = true;
                }
                if (target->field_0x42 != 0 && targetPlayer->isAlwaysEnemy() != 0) {
                    alwaysEnemyThreat = true;
                }
            }

            if (this->onScreen == 0) {
                target->field_0x75 = 0;
                target->field_0x76 = 0;

                int imageOffset;
                if (deadCargo) {
                    imageOffset = 0xb0;
                    if (target->countsAsEnemyExcludeFlag == 0) {
                        imageOffset = target->getType() == 9 ? 0xb4 : 0xa8;
                    }
                } else if (targetPlayer->enemyFlagsLo != 0) {
                    imageOffset = target == this->lockedEnemy ? 0x90 : 0x60;
                } else if (targetPlayer->carriesFriendCargoFlag != 0) {
                    imageOffset = target == this->lockedEnemy ? 0x94 : 0x68;
                } else {
                    imageOffset = target == this->lockedEnemy ? 0x98 : 0x78;
                }

                canvas->DrawImage2D(radar_image_slot(this, imageOffset),
                                    this->screenX,
                                    this->screenY,
                                    0x11u,
                                    0x44u);
            } else {
                target->field_0x76 = 1;

                radar_kiplayer_position_into(target, enemyPositionScratch);
                egoPositionScratch = radar_level_player(this)->getPosition();
                float dx = enemyPositionScratch.x - egoPositionScratch.x;
                bool farOrSpecial = dx > 24000.0f || dx < -24000.0f;
                dx = enemyPositionScratch.y - egoPositionScratch.y;
                farOrSpecial = farOrSpecial || dx > 24000.0f || dx < -24000.0f;
                dx = enemyPositionScratch.z - egoPositionScratch.z;
                farOrSpecial = farOrSpecial || dx > 24000.0f || dx < -24000.0f ||
                               target->getType() == 16917;

                if (farOrSpecial) {
                    int imageOffset = 0xa0;
                    if (!deadCargo) {
                        if (targetPlayer->enemyFlagsLo != 0) {
                            imageOffset = target == this->lockedEnemy ? 0x7c : 0x60;
                        } else if (targetPlayer->carriesFriendCargoFlag != 0) {
                            imageOffset = target == this->lockedEnemy ? 0x80 : 0x68;
                        } else {
                            imageOffset = target == this->lockedEnemy ? 0x84 : 0x78;
                        }
                    }
                    canvas->DrawImage2D(radar_image_slot(this, imageOffset),
                                        this->screenX,
                                        this->screenY,
                                        0x11u,
                                        0x44u);

                    float crosshairX = PlayerEgo::crosshairPos.x - static_cast<float>(this->screenX);
                    float crosshairY = PlayerEgo::crosshairPos.y - static_cast<float>(this->screenY);
                    if (crosshairX * crosshairX + crosshairY * crosshairY < 60000.0f &&
                        radar_crosshair_contains(this, this->field_0x124)) {
                        target->field_0x75 = 1;
                    }

                    if (target == this->lockedEnemy) {
                        this->lockLabel = this->calcDistance(enemyPositionScratch.x,
                                                            enemyPositionScratch.y,
                                                            enemyPositionScratch.z,
                                                            egoPositionScratch.x,
                                                            egoPositionScratch.y,
                                                            egoPositionScratch.z);
                        canvas->DrawString(radar_font(),
                                           this->lockLabel,
                                           this->screenX - this->halfScreenHeight,
                                           this->screenY + this->halfScreenWidth,
                                           false);
                    }
                } else if (deadCargo) {
                    canvas->DrawImage2D(radar_image_slot(this, 0x9c),
                                        this->screenX,
                                        this->screenY,
                                        0x11u,
                                        0x44u);
                } else {
                    if (!target->isDead()) {
                        target->field_0x75 = static_cast<uint8_t>(
                            radar_crosshair_contains(this, this->field_0x124));
                    }

                    int healthBackground;
                    int healthFill;
                    int lockedOverlay;
                    if (targetPlayer->enemyFlagsLo != 0) {
                        healthBackground = 0xe0;
                        healthFill = 0xd0;
                        lockedOverlay = 0x5c;
                    } else if (targetPlayer->carriesFriendCargoFlag != 0) {
                        healthBackground = 0xe4;
                        healthFill = 0xd4;
                        lockedOverlay = 0x64;
                    } else {
                        healthBackground = 0xe8;
                        healthFill = 0xd8;
                        lockedOverlay = 0x74;
                    }

                    int barX = this->screenX + 2 - this->halfScreenHeight;
                    int barY = this->screenY + this->halfScreenWidth + 2;
                    canvas->DrawImage2D(radar_image_slot(this, healthBackground), barX, barY);

                    int healthWidth = static_cast<int>(
                        (static_cast<float>(targetPlayer->getDamageRate()) / 100.0f) *
                        static_cast<float>(this->lockPanelWidth));
                    this->field_0x184 = healthWidth;
                    canvas->DrawRegion2D(radar_image_slot(this, healthFill),
                                         0,
                                         0,
                                         healthWidth,
                                         this->lockPanelHeight,
                                         0.0f,
                                         0,
                                         0,
                                         this->screenX + 3 - this->halfScreenHeight,
                                         this->screenY + this->halfScreenWidth + 3);

                    if (targetPlayer->getEmpPoints() < targetPlayer->getMaxEmpPoints()) {
                        canvas->DrawImage2D(radar_image_slot(this, 0xec),
                                            this->screenX + 2 - this->halfScreenHeight,
                                            this->screenY + this->halfScreenWidth + 8);
                        int empWidth = static_cast<int>(
                            (static_cast<float>(targetPlayer->getEmpDamageRate()) / 100.0f) *
                            static_cast<float>(this->lockPanelWidth));
                        this->field_0x184 = empWidth;
                        canvas->DrawRegion2D(radar_image_slot(this, 0xdc),
                                             0,
                                             0,
                                             empWidth,
                                             this->lockPanelHeight,
                                             0.0f,
                                             0,
                                             0,
                                             this->screenX + 3 - this->halfScreenHeight,
                                             this->screenY + this->halfScreenWidth + 9);
                    }

                    if (target == this->lockedEnemy) {
                        canvas->DrawImage2D(radar_image_slot(this, lockedOverlay),
                                            this->screenX,
                                            this->screenY,
                                            0x11u,
                                            0x44u);
                    }
                }

                PlayerEgo *enemyEgo = radar_level_player(this);
                if (!enemyEgo->isAutoPilot() && !enemyEgo->isDockedToDockingPoint()) {
                    bool candidateGate = (deadCargo || target->countsAsEnemyExcludeFlag == 0) &&
                                         (!enemyCandidateActive || target->field_0x24 != 0);
                    if (candidateGate) {
                        if (deadCargo && target->isDead() && this->field_0x1c == nullptr &&
                            this->field_0x1ae != 0) {
                            this->candidateEnemy = target;
                            this->field_0x1c = target;
                            enemyCandidateActive = true;
                        } else if (radar_crosshair_contains(this, this->field_0x124)) {
                            target->field_0x75 = 1;
                            if ((deadCargo && this->field_0x1ad != 0) ||
                                (this->field_0x1b0 != 0 && this->field_0x1a8 == 0)) {
                                if (this->candidateEnemy != target) {
                                    this->field_0x198 = 0;
                                }
                                this->candidateEnemy = target;
                                enemyCandidateActive = true;
                            }
                        }
                    }
                }
            }

            if (this->field_0x1af != 0 && this->field_0x1c == nullptr &&
                deadCargo && target->isDead()) {
                this->candidateEnemy = target;
                this->field_0x1c = target;
                enemyCandidateActive = true;
            }
        }

        PlayerEgo *enemyEgo = radar_level_player(this);
        if (!enemyEgo->isDockingToAsteroid() && !enemyEgo->isDockingToDockingPoint() &&
            !turretMode && !enemyLockBlocked) {
            KIPlayer *candidate = this->candidateEnemy;
            bool candidateDeadCargo = candidate != nullptr && candidate->hasCargo != 0 &&
                                      (candidate->isDead() || candidate->isDying());

            if (this->lockedEnemy != nullptr &&
                (this->lockedEnemy->isDying() || this->lockedEnemy->isDead())) {
                this->lockedEnemy = nullptr;
            }

            if (!enemyCandidateActive || candidate == nullptr) {
                this->field_0x198 = 0;
                if ((candidateDeadCargo ? this->field_0x1c : this->lockedEnemy) == nullptr) {
                    this->candidateEnemy = nullptr;
                }
            } else {
                this->field_0x198 += elapsed;
                bool fastLock = candidateDeadCargo || candidate->field_0x24 != 0;
                int lockThreshold = fastLock ? this->field_0x1b4 : this->field_0x1b8;
                if (this->field_0x198 > lockThreshold) {
                    if (fastLock) {
                        this->field_0x1c = candidate;
                        if (this->field_0x1ad == 0) {
                            hud->hudEvent(9, radar_level_player(this), 0);
                            this->field_0x1c = nullptr;
                        }
                    } else if (this->lockedEnemy != candidate) {
                        Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
                        this->lockedEnemy = candidate;

                        if (this->field_0x1a9 != 0) {
                            radar_kiplayer_position_into(candidate, enemyPositionScratch);
                            egoPositionScratch = radar_level_player(this)->getPosition();
                            float dx = enemyPositionScratch.x - egoPositionScratch.x;
                            if (dx < 24000.0f || dx > -24000.0f ||
                                (dx = enemyPositionScratch.y - egoPositionScratch.y, dx < 24000.0f) ||
                                dx > -24000.0f ||
                                (dx = enemyPositionScratch.z - egoPositionScratch.z, dx < 24000.0f) ||
                                dx > -24000.0f) {
                                Array<int> *cargo = candidate->cargo;
                                if (cargo != nullptr) {
                                    for (unsigned cargoIndex = 0;
                                         cargoIndex + 1 < cargo->size();
                                         cargoIndex += 2) {
                                        int amount = (*cargo)[cargoIndex + 1];
                                        if (amount >= 1) {
                                            hud->catchCargo((*cargo)[cargoIndex],
                                                            amount,
                                                            false,
                                                            false,
                                                            false,
                                                            true,
                                                            false);
                                            break;
                                        }
                                    }
                                } else {
                                    hud->hudEvent(22, radar_level_player(this), 0);
                                }
                            }
                        }
                    }
                    this->field_0x198 = 0;
                }

                int drawDelay = fastLock ? 500 : 0;
                if (this->field_0x198 > drawDelay) {
                    KIPlayer *resolvedLock = fastLock ? this->field_0x1c : this->lockedEnemy;
                    if (candidate != resolvedLock) {
                        int progressElapsed = this->field_0x198;
                        int progressThreshold = this->field_0x1b8;
                        if (fastLock) {
                            progressElapsed -= 500;
                            progressThreshold = this->field_0x1b4 - 500;
                        }
                        int frame = static_cast<int>(
                            (static_cast<float>(progressElapsed) / static_cast<float>(progressThreshold)) *
                            static_cast<float>(this->blipSprite->getRawFrameCount() - 1));
                        this->blipSprite->setFrame(frame);
                        this->blipSprite->setRefPixelPosition(
                            static_cast<int>(PlayerEgo::crosshairPos.x),
                            static_cast<int>(PlayerEgo::crosshairPos.y));
                        this->blipSprite->draw(1.0f, 1.0f);
                    }
                }
            }
        }
    }

    bool gasCandidateActive = radar_draw_gas_cloud_targets(this,
                                                           status,
                                                           turretMode,
                                                           stationCandidateActive || planetCandidateActive ||
                                                               enemyCandidateActive,
                                                           elapsed);

    radar_draw_asteroid_targets(this,
                                hud,
                                turretMode,
                                stationCandidateActive || planetCandidateActive ||
                                    enemyCandidateActive || gasCandidateActive,
                                elapsed);

    radar_update_ambient_music(this, status, type10Threat, alwaysEnemyThreat);
    return 0;
}

Vector Radar::elipsoidIntersect(int y, int x, Vector value) {
    int centerY = this->centerY;
    float dy = static_cast<float>(centerY - x);
    float dy2 = dy * dy;
    int centerX = this->centerX;
    float dx = static_cast<float>(centerX - y);
    float dx2 = dx * dx;
    float weightY = this->weightY;
    float weightX = this->weightX;
    float distance = weightY * dy2 + weightX * dx2;

    if (distance >= 0.0f) {
        float scale = (distance - Globals::gGlobals->sqrt(distance)) / distance;
        if (scale >= 0.0f && scale <= 1.0f) {
            value.y = static_cast<float>(static_cast<int>(static_cast<float>(x) + scale * dy));
            value.x = static_cast<float>(static_cast<int>(static_cast<float>(y) + scale * dx));
        }
    }

    return value;
}

int Radar::drawCurrentLock(Hud *) {
    if (this->enabled == 0) {
        return 0;
    }

    Radar::drawTarget = 1;

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    void *layout = Globals::layout;
    const int titleY = this->lockPanelY + layout_i32(layout, 0xbc);
    const int qualityY = this->lockPanelY + layout_i32(layout, 0xc0);
    const int detailY = this->lockPanelY + layout_i32(layout, 0xc4);
    const int raceY = this->lockPanelY + layout_i32(layout, 0xc8);
    const int iconGap = layout_i32(layout, 0x2c);

    if (this->lockedPlanetTarget != nullptr) {
        canvas->DrawImage2D(radar_image_slot(this, 0xcc), this->lockPanelX, this->lockPanelY);
        Status *status = Globals::status;
        auto *planetNames = reinterpret_cast<Array<String *> *>(
            static_cast<intptr_t>(status->getPlanetNames()));
        String planetName(*(*planetNames)[this->planetDockIndex], false);
        int textWidth = canvas->GetTextWidth(radar_font(), planetName);
        canvas->DrawString(radar_font(), planetName, (Globals::w >> 1) - textWidth / 2,
                           titleY, false);
        return 0;
    }

    KIPlayer *target = this->lockedAsteroid;
    if (target == nullptr) {
        target = this->lockedGasCloud;
        if (target == nullptr) {
            target = this->lockedEnemy;
            if (target == nullptr) {
                target = this->lockedStation;
            }
        }
    }

    if (target == nullptr) {
        Radar::drawTarget = 0;
        return 0;
    }

    canvas->DrawImage2D(radar_image_slot(this, 0xcc), this->lockPanelX, this->lockPanelY);

    KIPlayer *stationLock = this->lockedStation;
    KIPlayer **landmarkData = this->landmarkTargets->data_;
    String **labelData = this->labelStrings->data_;
    String **panelLabelSlot;
    if (stationLock == landmarkData[0]) {
        panelLabelSlot = labelData;
    } else {
        panelLabelSlot = stationLock == landmarkData[3] ? labelData + 3 : labelData + 1;
    }
    String panelLabel(**panelLabelSlot, false);

    if (target == this->lockedGasCloud) {
        String gasText(*static_cast<GameText *>(Globals::gameText)->getText(3236), false);
        int textWidth = canvas->GetTextWidth(radar_font(), gasText);
        canvas->DrawString(radar_font(), gasText, (Globals::w >> 1) - (textWidth >> 1),
                           detailY, false);
        return 0;
    }

    if (target == this->lockedAsteroid) {
        String asteroidText(*static_cast<GameText *>(Globals::gameText)->getText(raw_i32(target, 0x128) + 1274), false);
        int textWidth = canvas->GetTextWidth(radar_font(), asteroidText);
        int textX = (Globals::w >> 1) - (textWidth >> 1);
        this->qualitySprite->setFrame(static_cast<PlayerAsteroid *>(target)->getQualityFrameIndex());
        this->qualitySprite->setPosition(textX - iconGap - this->qualitySprite->getFrameWidth(),
                                         qualityY);
        this->qualitySprite->draw(1.0f, 1.0f);
        canvas->DrawString(radar_font(), asteroidText, textX, detailY, false);
        return 0;
    }

    String line;
    bool normalLabel = target->name.size() == 0 ||
        (target->name.Compare(*static_cast<GameText *>(Globals::gameText)->getText(1611)) != 0 &&
         target->name.Compare(*static_cast<GameText *>(Globals::gameText)->getText(1663)) != 0 &&
         raw_u8(target, 0x42) == 0);

    int textX;
    if (normalLabel) {
        const String *lineSource;
        if (target != stationLock && target->name.size() == 0) {
            lineSource = static_cast<GameText *>(Globals::gameText)->getText(target->shipGroup + 406);
        } else if (target == stationLock) {
            lineSource = &panelLabel;
        } else {
            lineSource = &target->name;
        }
        line = *lineSource;

        if (target == stationLock) {
            Status *status = Globals::status;
            if (status->inAlienOrbit()) {
                if (status->dlc1Won()) {
                    this->raceSprite->setFrame(8);
                } else {
                    this->raceSprite->setFrame(9);
                }
            } else {
                SolarSystem *system = status->getSystem();
                this->raceSprite->setFrame(system->getRace());
            }
            int textWidth = canvas->GetTextWidth(radar_font(), line);
            textX = (Globals::w >> 1) - (textWidth >> 1);
            this->raceSprite->setPosition(textX - iconGap - this->raceSprite->getFrameWidth(),
                                          raceY);
            this->raceSprite->draw(1.0f, 1.0f);
        } else {
            line += String(" ") + String(target->player->getDamageRate()) + String("%");
            int textWidth = canvas->GetTextWidth(radar_font(), line);
            textX = (Globals::w >> 1) - (textWidth >> 1);
        }

        canvas->DrawString(radar_font(), line, textX, detailY, false);
    } else {
        canvas->SetColor(0xff2a00ffu);
        line = target->name;
        if (raw_u8(target, 0x42) != 0) {
            line += String(" ") + String(target->player->getDamageRate()) + String("%");
        }
        int textWidth = canvas->GetTextWidth(radar_font(), line);
        textX = (Globals::w >> 1) - (textWidth >> 1);
        canvas->DrawString(radar_font(), line, textX, detailY, false);
        canvas->SetColor(0xffffffffu);
    }

    bool suppressRaceIcon = target == stationLock;
    int raceFrame;
    if (target != stationLock) {
        raceFrame = target->shipGroup;
        suppressRaceIcon = raceFrame == 10;
    }
    if (!suppressRaceIcon) {
        this->raceSprite->setFrame(raceFrame);
        this->raceSprite->setPosition(textX - iconGap - this->raceSprite->getFrameWidth(),
                                      raceY);
        this->raceSprite->draw(1.0f, 1.0f);
    }

    return 0;
}

String Radar::calcDistance(float x, float y, float z, float originX, float originY, float originZ) {
    long long dx = static_cast<long long>(x * 0.5f - originX * 0.5f);
    long long dy = static_cast<long long>(y * 0.5f - originY * 0.5f);
    long long dz = static_cast<long long>(z * 0.5f - originZ * 0.5f);
    long long total = dx * dx + dy * dy + dz * dz;
    int distance = static_cast<int>(Globals::gGlobals->sqrt(static_cast<float>(total) * 0.00024414f));
    int meters = 8 * distance;

    if (distance < 125) {
        return String(meters) + String("m");
    }

    String decimal;
    if (meters % 1000 < 100) {
        decimal = String("0");
    } else {
        decimal = String(meters % 1000);
    }
    decimal = decimal.SubString(0, 1);

    return String(static_cast<int>(static_cast<unsigned int>(distance) / 125u)) +
           String(".") + decimal + String("km");
}

// Static data members present in the original binary (defined for symbol parity).
unsigned char Radar::drawTarget;
