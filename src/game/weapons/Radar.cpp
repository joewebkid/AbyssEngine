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
    return Globals::font;
}

static GOF2_ALWAYS_INLINE PlayerEgo *radar_level_player(Radar *self) {
    return self->level->getPlayer();
}

static GOF2_ALWAYS_INLINE Mission *radar_campaign_mission(Status *status) {
    return reinterpret_cast<Mission *>(static_cast<intptr_t>(status->getCampaignMission()));
}

static GOF2_ALWAYS_INLINE Array<String *> *radar_planet_names(Status *status) {
    return reinterpret_cast<Array<String *> *>(static_cast<intptr_t>(status->getPlanetNames()));
}

static GOF2_ALWAYS_INLINE bool radar_crosshair_contains(Radar *self, int size) {
    // Radar+0x124/+0x12c already contain half extents (Android draw).
    int left = static_cast<int>(PlayerEgo::crosshairPos.x - static_cast<float>(size));
    int top = static_cast<int>(PlayerEgo::crosshairPos.y - static_cast<float>(size));
    return self->screenX > left && self->screenX < left + 2 * size && self->screenY > top &&
           self->screenY < top + 2 * size;
}

static inline bool radar_campaign_forces_marker(Status *status, Mission *campaign, int stationId,
                                                int currentMission) {
    if (Globals::status->getCurrentCampaignMission() == 116 && static_cast<unsigned>(stationId - 90) <= 4u &&
        ((1 << (stationId - 90)) & radar_campaign_mission(Globals::status)->getStatusValue()) == 0) {
        return true;
    }
    if (Globals::status->getCurrentCampaignMission() == 120 && stationId == 93) {
        return true;
    }
    if (Globals::status->getCurrentCampaignMission() == 125 &&
        Globals::status->isFreighterMissionStation(stationId)) {
        int value = radar_campaign_mission(Globals::status)->getStatusValue();
        int bit = Globals::status->getFreighterMissionStationBit(stationId);
        return ((1 << bit) & value) == 0;
    }
    return false;
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

static GOF2_ALWAYS_INLINE void radar_play_lock_cue(Radar *self) {
    if (self->field_0x218_byte == 0 && Globals::sound->isPlaying(0) == 0) {
        Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
    }
}

static GOF2_ALWAYS_INLINE void radar_draw_delayed_lock_blip(Radar *self, int timer, int lockTime,
                                                            KIPlayer *candidate, KIPlayer *lockedTarget) {
    if (timer <= 500) {
        return;
    }

    Sprite *blip;
    int frame;
    if (candidate == nullptr || candidate == lockedTarget) {
        blip = self->blipSprite;
        frame = blip->getRawFrameCount() - 1;
    } else {
        frame = static_cast<int>((static_cast<float>(timer - 500) / static_cast<float>(lockTime - 500)) *
                                 static_cast<float>(self->blipSprite->getRawFrameCount() - 1));
        if (self->blipSprite->getRawFrameCount() - 1 <= frame) {
            return;
        }
        blip = self->blipSprite;
    }
    blip->setFrame(frame);
    self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                          static_cast<int>(PlayerEgo::crosshairPos.y));
    self->blipSprite->draw(1.0f, 1.0f);
}

static inline void radar_update_gas_cloud_sparks(Radar *self, Status *status, PlayerGasCloud *cloud,
                                                 bool turretMode) {
    auto *sparks = static_cast<Array<AEGeometry *> *>(cloud->getSparks());
    if (sparks == nullptr || Globals::status->getShip()->getFirstEquipmentOfSort(35) == nullptr) {
        return;
    }

    for (unsigned i = 0; i < sparks->size(); ++i) {
        AEGeometry *spark = (*sparks)[i];
        self->update(spark->getPosition());
        if (self->onScreen != 0 && radar_crosshair_contains(self, self->turretScopeHalfWidth)) {
            Vector sparkPosition = spark->getPosition();
            Vector playerPosition = radar_level_player(self)->getPosition();
            Vector delta = sparkPosition - playerPosition;
            float distance = AbyssEngine::AEMath::VectorLength(delta);
            float maxDistance =
                static_cast<float>(Globals::status->getShip()->getFirstEquipmentOfSort(35)->getAttribute(51));
            bool inSight = distance <= maxDistance;
            if (inSight && cloud->isSparkAlive(static_cast<int>(i))) {
                self->plasmaInRange = 1;
            }
            cloud->setSparkInSight(static_cast<int>(i), turretMode && inSight);
        } else {
            cloud->setSparkInSight(static_cast<int>(i), false);
        }
    }
}


static inline bool radar_asteroid_center_range_enabled(Radar *self) {
    if (self->field_0x1aa == 0) {
        return false;
    }
    return self->level->isInAsteroidCenterRange(radar_level_player(self)->getPosition()) != 0;
}

static inline void radar_draw_asteroid_quality_indicator(Radar *self, bool centerRange,
                                                         PlayerAsteroid *asteroid) {
    if (!centerRange) {
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
    if (self->offscreenFlagRight != 0 && self->offscreenFlagBottom == 0 && self->offscreenFlagTop == 0) {
        x = self->screenX + 11;
        goto horizontalOverlay;
    }
    x = self->screenX;
    if (self->offscreenFlagLeftOrVertical != 0) {
        if (self->offscreenFlagBottom != 0) {
            goto bottomOverlay;
        }
        if (self->offscreenFlagTop == 0) {
            x -= 11;
        }
    } else if (self->offscreenFlagBottom != 0) {
        goto bottomOverlay;
    }

horizontalOverlay:
    y = self->screenY;
    if (self->offscreenFlagTop != 0) {
        y -= 11;
    }
    return;

bottomOverlay:
    y = self->screenY + 11;
}

static GOF2_ALWAYS_INLINE void radar_draw_asteroid_dead_cargo_marker(Radar *self, bool onScreen) {
    if (onScreen) {
        Globals::Canvas->DrawImage2D(radar_image_slot(self, 0x9c), self->screenX, self->screenY, 0x11u,
                                     0x44u);
        return;
    }

    Globals::Canvas->DrawImage2D(radar_image_slot(self, 0x8c), self->screenX, self->screenY, 0x11u, 0x44u);

    unsigned int overlayImage = radar_image_slot(self, 0xac);
    PaintCanvas *overlayCanvas = Globals::Canvas;
    int overlayX;
    int overlayY;
    radar_dead_cargo_directional_overlay_pos(self, overlayX, overlayY);
    overlayCanvas->DrawImage2D(overlayImage, overlayX, overlayY, 0x11u, 0x44u);
}


int Radar::getTurretScopeWidth() {
    return this->turretScopeHalfWidth << 1;
}

Radar::Radar(Level *level)
    : radarPosX(0), radarPosY(0), radarPosZ(0), field_0x160(0), field_0x164(0), field_0x168(0),
      field_0x16c(0), field_0x170(0), field_0x174(0), field_0x178(0), field_0x17c(0), field_0x180(0) {
    this->lockedEnemy = nullptr;
    this->candidateEnemy = nullptr;
    this->lockedPlanetTarget = nullptr;
    this->enabled = 1;
    this->field_0x58 = 0;
    this->labelStrings = nullptr;
    this->lockedAsteroid = nullptr;
    this->candidateAsteroid = nullptr;
    this->candidatePlanetTarget = nullptr;
    this->lockedGasCloud = nullptr;
    this->candidateGasCloud = nullptr;
    this->planetDockIndex = 0;
    this->field_0x20c = 0;
    this->field_0x1b4 = 0;
    this->field_0x1b8 = 0;
    this->field_0x1bc = 0;
    this->candidateStation = nullptr;
    this->field_0x2c = 0;
    this->field_0x30 = 0;
    this->players = nullptr;
    this->field_0x1c = nullptr;
    this->field_0x20 = 0;
    this->lockedStation = nullptr;
    this->field_0x144 = 0;
    this->field_0x148 = 0;
    this->field_0x14c = 0;
    this->field_0x150 = 0;
    this->field_0x134 = 0;
    this->field_0x138 = 0;
    this->field_0x13c = 0;
    this->field_0x140 = 0;
    this->field_0x198 = 0;
    this->field_0x19c = 0;
    this->field_0x1a0 = 0;
    this->field_0x1a4 = 0;
    this->field_0x54 = 0;
    this->plasmaInRange = 0;
    this->onScreen = 0;
    this->field_0x11d = 0;
    this->field_0x11e = 0;
    this->field_0x11f = 0;
    this->field_0x120 = 0;
    this->field_0x1a8 = 0;
    this->level = level;

    void *layout = Globals::layout;
    int width = layout_i32(layout, 0xac);
    this->screenWidth = width;
    this->halfScreenWidth = width >> 1;
    int height = layout_i32(layout, 0xa8);
    this->screenHeight = height;
    this->halfScreenHeight = height >> 1;
    this->originX = layout_i32(layout, 0xa0);
    this->originY = layout_i32(layout, 0xa4);

    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4c7, reinterpret_cast<unsigned int &>(this->radarImage));
    this->imageWidth = static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DWidth(this->radarImage);
    int imageHeight = static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DHeight(this->radarImage);
    int imageWidth = this->imageWidth;
    this->imageHeight = imageHeight;
    this->imageWidthSq = imageWidth * imageWidth;
    this->imageHeightSq = imageHeight * imageHeight;
    this->centerX = Globals::w / 2;
    this->centerY = Globals::h / 2;
    this->weightX = 1.0f / static_cast<float>(imageWidth * imageWidth);
    this->weightY = 1.0f / static_cast<float>(imageHeight * imageHeight);

    Array<String *> *strings = new Array<String *>();
    this->labelStrings = strings;
    ArraySetLength<String *>(4, *strings);

    (*this->labelStrings)[0] = new String((Globals::status->inAlienOrbit()
                                               ? String(*GameText::gGameText->getText(415), false)
                                               : String(Globals::status->getStation()->getName(), false)) +
                                          (Globals::status->getStation()->getIndex() == 101
                                               ? String("", false)
                                               : String(" ", false) + *GameText::gGameText->getText(136)));
    if (Globals::status->inAlienOrbit() && Globals::status->dlc1Won() && !Globals::status->inEmptyOrbit()) {
        String *label = (*this->labelStrings)[0];
        KIPlayer *primaryLandmark = (*level->getLandmarks())[0];
        label->Set(static_cast<unsigned short *>(primaryLandmark->name));
    }
    (*this->labelStrings)[1] = new String(*GameText::gGameText->getText(547), false);
    (*this->labelStrings)[2] = new String("", false);
    (*this->labelStrings)[3] = new String(*GameText::gGameText->getText(545), false);

    Array<AbstractGun *> *playerGuns = level->getPlayerGuns();
    if (playerGuns != nullptr) {
        for (unsigned i = 0; i < playerGuns->size(); ++i) {
            if ((*playerGuns)[i]->isRocketGun())
                static_cast<RocketGun *>((*playerGuns)[i])->setRadar(this);
            if ((*playerGuns)[i]->isBombGun())
                static_cast<BombGun *>((*playerGuns)[i])->setPlayer(level->getPlayer());
            if ((*playerGuns)[i]->isMineGun())
                static_cast<MineGun *>((*playerGuns)[i])->setPlayer(level->getPlayer());
        }
    }
    Array<AbstractGun *> *enemyGuns = level->getEnemyGuns();
    if (enemyGuns != nullptr) {
        for (unsigned i = 0; i < enemyGuns->size(); ++i)
            if ((*enemyGuns)[i]->isRocketGun())
                static_cast<RocketGun *>((*enemyGuns)[i])->setRadar(this);
    }

    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d4, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xd4)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d9, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xd0)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d6, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xd8)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d7, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xdc)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d3, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xe4)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4da, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xe0)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d5, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xe8)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d8, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xec)));
    this->lockPanelWidth =
        static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DWidth(radar_image_slot(this, 0xd4));
    this->lockPanelHeight =
        static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DHeight(radar_image_slot(this, 0xd4));

    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x454, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xc8)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x455, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xc4)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4dc, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x74)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4cb, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x78)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4c9, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x98)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4db, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x5c)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4cc, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x60)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4c8, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x90)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4d2, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x64)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4cd, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x68)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4ca, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x94)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4f0, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x6c)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4ef, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x70)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4f2, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x88)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4f1, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x8c)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4c8, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x7c)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4ca, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x80)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4c9, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x84)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4f2, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0x9c)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4f1, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xa4)));
    radar_image_slot(this, 0xa0) = radar_image_slot(this, 0xa4);
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x451, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xa8)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x44f, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xb0)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x44c, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xac)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x44d, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xb4)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x450, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xb8)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x453, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xbc)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x1f62, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xc0)));
    static_cast<PaintCanvas *>(Globals::Canvas)
        ->Image2DCreate(0x4c4, reinterpret_cast<unsigned int &>(radar_image_slot(this, 0xcc)));

    int halfWidth = Globals::w / 2;
    this->lockPanelX =
        halfWidth -
        static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DWidth(radar_image_slot(this, 0xcc)) / 2;
    this->lockPanelY = layout_i32(Globals::layout, 0xb0);

    uint32_t *raceFrames = new uint32_t[10];
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x4a1, raceFrames[0]);
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x49c, raceFrames[1]);
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x49e, raceFrames[3]);
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x49f, raceFrames[2]);
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x4a0, raceFrames[8]);
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x49d, raceFrames[9]);
    this->raceSprite = new Sprite(
        raceFrames, 10, static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DHeight(raceFrames[0]),
        static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DHeight(raceFrames[0]));

    unsigned int blipImage = 0;
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x456, blipImage);
    int blipSize = static_cast<PaintCanvas *>(Globals::Canvas)->GetImage2DHeight(blipImage);
    this->blipSprite = new Sprite(blipImage, blipSize, blipSize);
    this->blipSprite->defineReferencePixel(blipSize / 2, blipSize / 2);

    unsigned int qualityImage = 0;
    static_cast<PaintCanvas *>(Globals::Canvas)->Image2DCreate(0x44e, qualityImage);
    this->qualitySprite =
        new Sprite(qualityImage, layout_i32(Globals::layout, 0xb8), layout_i32(Globals::layout, 0xb4));

    Item *scanner = Globals::status->getShip()->getFirstEquipmentOfSort(19);
    Item *repairOrScanner = Globals::status->getShip()->getFirstEquipmentOfSort(17);
    Item *plasma = Globals::status->getShip()->getFirstEquipmentOfSort(13);
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
    *reinterpret_cast<uint8_t *>(&this->field_0x1b0) = 1;
    this->enabled = 1;

    int *radarSlots = new int[5];
    this->radarSlots = radarSlots;
    for (int i = 0; i < 5; ++i) {
        radarSlots[i] = -1;
    }

    this->field_0x124 = Globals::w / 16;
    this->field_0x128 = Globals::w / 6;
    this->turretScopeHalfWidth = Globals::w / 8;

    Item *scope = Globals::status->getShip()->getFirstEquipmentOfSort(35);
    if (scope != nullptr && scope->getAttribute(50) != 0) {
        scope = Globals::status->getShip()->getFirstEquipmentOfSort(35);
        this->turretScopeHalfWidth = static_cast<int>((static_cast<float>(scope->getAttribute(50)) / 100.0f) *
                                                      static_cast<float>(this->turretScopeHalfWidth));
    }

    Item *reservation = Globals::status->getShip()->getFirstEquipmentOfSort(37);
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
    Vector position = player->getPosition();
    update(position);
}

void Radar::update(Vector value) {
    Vector *current = reinterpret_cast<Vector *>(&this->radarPosX);
    *current = AbyssEngine::AEMath::MatrixTransformVector(this->transform, value);

    this->radarPosY = -this->radarPosY;
    this->radarPosZ = -this->radarPosZ;

    PaintCanvas *canvas = static_cast<PaintCanvas *>(Globals::Canvas);
    int visible = canvas->GetScreenPosition(value, value);
    this->onScreen = static_cast<uint8_t>(visible);

    int screenX = static_cast<int>(value.x);
    this->screenX = screenX;
    int screenY = static_cast<int>(value.y);
    this->screenY = screenY;

    if (visible == 0) {
        *current = this->elipsoidIntersect(screenX, screenY, *current);
        this->screenX = static_cast<int>(current->x);
        this->screenY = static_cast<int>(current->y);
    }
}

void Radar::draw(Player *player, Hud *hud, int elapsed) {
    if (this->enabled == 0) {
        return;
    }

    this->field_0x218_byte = 0;
    this->plasmaInRange = 0;

    Globals::Canvas->SetColor(static_cast<unsigned int>(-1));

    Status *status = Globals::status;
    Mission *mission = status->getMission();
    bool missionTargetFilter = false;
    if (!mission->isEmpty()) {
        missionTargetFilter = mission->getType() != 11 && mission->getType() != 0 &&
                              mission->getType() != 13 && mission->getType() != 171 &&
                              mission->getType() != 172;
    }
    PlayerEgo *ego = this->level->getPlayer();
    bool turretMode = ego->isInTurretMode() != 0;
    bool alienOrbit = Globals::status->inAlienOrbit();
    int currentMission = Globals::status->getCurrentCampaignMission();

    this->enemyTargets = this->level->getEnemies();
    this->landmarkTargets = this->level->getLandmarks();
    this->playerRoute = this->level->getPlayerRoute();
    this->asteroidTargets = this->level->getAsteroids();
    this->gasCloudTargets = this->level->getGasClouds();
    StarSystem *starSystem = this->level->getStarSystem();
    this->planetTargets = static_cast<Array<KIPlayer *> *>(starSystem->getPlanetTargets());

    PaintCanvas *cameraCanvas = Globals::Canvas;
    unsigned int camera = Globals::Canvas->CameraGetCurrent();
    auto *local = static_cast<Matrix *>(cameraCanvas->CameraGetLocal(camera));
    this->transform = *local;
    this->cameraPosX = this->transform.m[3];
    this->cameraPosY = this->transform.m[7];
    this->cameraPosZ = this->transform.m[11];
    this->transform = AbyssEngine::AEMath::MatrixGetInverse(this->transform);

    uint8_t planetApproachTarget = 0;
    bool goingToPlanet = radar_level_player(this)->goingToPlanet();
    if (!alienOrbit) {
        if (goingToPlanet && radar_level_player(this)->isDockingToPlanet() == 0) {
            KIPlayer *autopilotTarget = reinterpret_cast<KIPlayer *>(
                static_cast<intptr_t>(radar_level_player(this)->getAutoPilotTarget()));
            auto *planets =
                static_cast<Array<KIPlayer *> *>(this->level->getStarSystem()->getPlanetTargets());
            SolarSystem *system = Globals::status->getSystem();
            auto *stations = reinterpret_cast<Array<int> *>(Globals::status->getSystem()->getStations());
            uint32_t stationEnum = system->getStationEnumIndex((*stations)[this->planetDockIndex]);
            planetApproachTarget = autopilotTarget == (*planets)[stationEnum];
        }
    } else if (goingToPlanet) {
        planetApproachTarget = radar_level_player(this)->isDockingToPlanet() ^ 1;
    }
    this->field_0x1a8 = planetApproachTarget;

    Globals::Canvas->DrawImage2D(this->radarImage, this->centerX - this->imageWidth,
                                 this->centerY - this->imageHeight);
    Globals::Canvas->DrawImage2D(this->radarImage, this->centerX, this->centerY - this->imageHeight, 1u);
    Globals::Canvas->DrawImage2D(this->radarImage, this->centerX - this->imageWidth, this->centerY, 2u);
    Globals::Canvas->DrawImage2D(this->radarImage, this->centerX, this->centerY, 3u);

    Waypoint *waypoint = this->playerRoute != nullptr ? this->playerRoute->getWaypoint() : nullptr;
    if (waypoint != nullptr) {
        this->update(waypoint);
        this->lockLabel = this->calcDistance(static_cast<float>(waypoint->x), static_cast<float>(waypoint->y),
                                             static_cast<float>(waypoint->z), this->cameraPosX,
                                             this->cameraPosY, this->cameraPosZ);
        if (this->onScreen != 0) {
            Globals::Canvas->DrawImage2D(radar_image_slot(this, 0x6c), this->screenX, this->screenY, 0x11u,
                                         0x44u);
            Globals::Canvas->DrawString(radar_font(), this->lockLabel, this->screenX - this->halfScreenHeight,
                                        this->screenY + this->halfScreenWidth, false);
        } else if (this->scannerAvailable != 0) {
            Globals::Canvas->DrawImage2D(radar_image_slot(this, 0x70), this->screenX, this->screenY, 0x11u,
                                         0x44u);
        }
    }

    // These phases share draw's native lock state and join at their exit labels.
    bool stationCandidateActive = false;
    {
        bool existingNonStationLock = this->lockedAsteroid != nullptr || this->lockedGasCloud != nullptr ||
                                      this->field_0x1c != nullptr || this->lockedPlanetTarget != nullptr;
        if (this->landmarkTargets == nullptr) {
            goto stationMarkerDone;
        }

        if (Globals::status->getStation()->getIndex() == 109 ||
            Globals::status->getStation()->getIndex() == 110 ||
            (Globals::status->getStation()->getIndex() == 111 &&
             Globals::status->getCurrentCampaignMission() > 93)) {
            goto stationMarkerDone;
        }

        bool candidateFound = false;

        for (unsigned i = 0; i < this->landmarkTargets->size(); ++i) {
            if (i == 2) {
                continue;
            }

            KIPlayer *target = (*this->landmarkTargets)[i];
            if (target == nullptr || target->isVisible() == 0) {
                continue;
            }

            this->update((*this->landmarkTargets)[i]);
            if (this->onScreen != 0 && this->screenX > this->centerX - this->field_0x128 &&
                this->screenX < this->centerX + this->field_0x128 &&
                this->screenY > this->centerY - this->field_0x128 &&
                this->screenY < this->centerY + this->field_0x128) {
                if (!candidateFound && !existingNonStationLock && !turretMode && i != 3) {
                    int radius = this->field_0x124;
                    int left = static_cast<int>(PlayerEgo::crosshairPos.x - static_cast<float>(radius));
                    int top = static_cast<int>(PlayerEgo::crosshairPos.y - static_cast<float>(radius));
                    if (this->screenX > left && this->screenY < top + 2 * radius && this->screenY > top &&
                        this->screenX < left + 2 * radius) {
                        if (this->candidateStation != (*this->landmarkTargets)[i]) {
                            this->field_0x1a4 = 0;
                        }
                        this->candidateStation = (*this->landmarkTargets)[i];
                        candidateFound = true;
                    }
                }

                *reinterpret_cast<Vector *>(&this->field_0x16c) = player->getPosition();
                *reinterpret_cast<Vector *>(&this->field_0x178) =
                    (*this->landmarkTargets)[i]->player->getPosition();
                if (i != 3) {
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, 0x88), this->screenX, this->screenY,
                                                 0x11u, 0x44u);
                }

                String *label = (*this->labelStrings)[i];
                int *textOffset = i == 0 ? &this->originX : &this->originY;
                Globals::Canvas->DrawString(radar_font(), *label, this->screenX + *textOffset, this->screenY,
                                            false);

                if (i <= 1) {
                    if (i != 0 || alienOrbit) {
                        target = (*this->landmarkTargets)[i];
                        this->lockLabel =
                            this->calcDistance(target->posX, target->posY, target->posZ, this->cameraPosX,
                                               this->cameraPosY, this->cameraPosZ);
                        Globals::Canvas->DrawString(radar_font(), this->lockLabel,
                                                    this->screenX + *textOffset,
                                                    this->screenY + layout_i32(Globals::layout, 4), false);
                    } else {
                        unsigned int techFont = Globals::font;
                        PaintCanvas *techCanvas = Globals::Canvas;
                        techCanvas->DrawString(
                            techFont,
                            *static_cast<GameText *>(Globals::gameText)->getText(133) + String(": ", false) +
                                String(String(Globals::status->getStation()->getTecLevel()), false),
                            this->screenX + this->originX, this->screenY + layout_i32(Globals::layout, 4),
                            false);
                        target = (*this->landmarkTargets)[i];
                        this->lockLabel =
                            this->calcDistance(target->posX, target->posY, target->posZ, this->cameraPosX,
                                               this->cameraPosY, this->cameraPosZ);
                        Globals::Canvas->DrawString(
                            radar_font(), this->lockLabel, this->screenX + this->originX,
                            this->screenY + 2 * layout_i32(Globals::layout, 4), false);
                    }
                }
            } else {
                if (i == 1) {
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xbc), this->screenX, this->screenY,
                                                 0x11u, 0x44u);
                } else if (i == 3) {
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xb8), this->screenX, this->screenY,
                                                 0x11u, 0x44u);
                }
            }
        }

        if (turretMode) {
            stationCandidateActive = candidateFound;
            goto stationMarkerDone;
        }

        if (this->lockedStation != nullptr) {
            if (this->lockedStation->isDead()) {
                this->lockedStation = nullptr;
            }
            if (this->lockedStation == this->candidateStation) {
                this->field_0x218_byte = 1;
            }
            this->lockedStation = nullptr;
        }

        if (!candidateFound || radar_level_player(this)->isDockingToAsteroid() ||
            radar_level_player(this)->isDockingToDockingPoint() || radar_level_player(this)->isAutoPilot()) {
            this->field_0x1a4 = 0;
            if (this->lockedStation == nullptr) {
                this->candidateStation = nullptr;
            }
            stationCandidateActive = candidateFound;
            goto stationMarkerDone;
        }

        this->field_0x1a4 += elapsed;
        if (this->field_0x1a4 > this->field_0x1b8) {
            if (this->field_0x218_byte == 0) {
                Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
            }
            this->lockedStation = this->candidateStation;
        }

        if (this->field_0x1a4 >= 1) {
            Sprite *blip = this->blipSprite;
            int frame;
            if (this->candidateStation == nullptr || this->candidateStation == this->lockedStation) {
                frame = blip->getRawFrameCount() - 1;
            } else {
                int frameMax = blip->getRawFrameCount() - 1;
                frame = static_cast<int>(
                    (static_cast<float>(this->field_0x1a4) / static_cast<float>(this->field_0x1b8)) *
                    static_cast<float>(frameMax));
            }
            blip->setFrame(frame);
            this->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                                  static_cast<int>(PlayerEgo::crosshairPos.y));
            this->blipSprite->draw(1.0f, 1.0f);
        }

        stationCandidateActive = candidateFound;
        goto stationMarkerDone;
    }
stationMarkerDone:

    bool enemyLockBlocked = this->lockedAsteroid != nullptr || this->lockedGasCloud != nullptr ||
                            this->field_0x1c != nullptr || this->lockedStation != nullptr;
    {
        if (currentMission < 2 || this->planetTargets == nullptr || alienOrbit ||
            radar_level_player(this)->isDockingToPlanet() != 0) {
            goto planetMarkerDone;
        }

        SolarSystem *system = Globals::status->getSystem();
        auto *stations = reinterpret_cast<Array<int> *>(system->getStations());

        bool candidateFound = false;

        for (unsigned i = 0; i < this->planetTargets->size(); ++i) {
            KIPlayer *target = (*this->planetTargets)[i];
            if (target == nullptr) {
                continue;
            }

            this->update(target);
            if (this->onScreen == 0) {
                continue;
            }

            int stationId = (*stations)[i];
            int currentStationId = Globals::status->getStation()->getIndex();
            int warpGateEnumIndex = Globals::status->getSystem()->getWarpGateEnumIndex();
            int markerOffset = 10;
            if (stationId != currentStationId && static_cast<int>(i) == warpGateEnumIndex) {
                Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xbc), this->screenX + 10,
                                             this->screenY - 10);
                markerOffset = 24;
            }

            bool markerDrawn = false;
            Mission *campaign = radar_campaign_mission(Globals::status);
            Mission *freelance = Globals::status->getFreelanceMission();
            if (campaign != nullptr && !campaign->isEmpty() && campaign->isVisible() &&
                (Globals::status->getCurrentCampaignMission() < 148 ||
                 Globals::status->getCurrentCampaignMission() >= 152)) {
                int campaignTargetStation = campaign->getTargetStation();
                if (stationId != currentStationId && stationId == campaignTargetStation) {
                    if (Globals::status->getCurrentCampaignMission() != 120) {
                        Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xc8),
                                                     this->screenX + markerOffset, this->screenY - 10);
                        markerDrawn = true;
                    }
                }
                if (radar_campaign_forces_marker(Globals::status, campaign, stationId, currentMission)) {
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xc8), this->screenX + markerOffset,
                                                 this->screenY - 10);
                    markerDrawn = true;
                }
            }

            if (freelance != nullptr && !freelance->isEmpty() && freelance->isVisible()) {
                int targetStation;
                if (freelance->getType() == 14) {
                    targetStation = freelance->getAgent()->getStation();
                } else {
                    targetStation = freelance->getTargetStation();
                }
                if (stationId != currentStationId && stationId == targetStation) {
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xc4), this->screenX + markerOffset,
                                                 this->screenY - 10);
                    markerDrawn = true;
                }
            }

            if (!enemyLockBlocked && !candidateFound && !turretMode) {
                int targetSize = static_cast<int>(i) == StarSystem::orbitPlanetIndex ? this->field_0x128
                                                                                     : this->field_0x124;
                // Planet hover uses a full extent; enemy/station tests use radii.
                int left = static_cast<int>(PlayerEgo::crosshairPos.x - static_cast<float>(targetSize >> 1));
                int top = static_cast<int>(PlayerEgo::crosshairPos.y - static_cast<float>(targetSize >> 1));
                if (this->screenX > left && this->screenX < left + targetSize && this->screenY > top &&
                    this->screenY < top + targetSize) {
                    if (static_cast<int>(i) != StarSystem::orbitPlanetIndex) {
                        if (this->candidatePlanetTarget != target) {
                            this->field_0x1a0 = 0;
                        }
                        this->candidatePlanetTarget = target;
                        this->planetDockIndex = static_cast<int>(i);
                        candidateFound = true;
                    }

                    if (stationId != currentStationId) {
                        Array<String *> *names = radar_planet_names(Globals::status);
                        int textOffset = markerDrawn ? markerOffset + 14 : markerOffset;
                        Globals::Canvas->DrawString(radar_font(), *(*names)[i], this->screenX + textOffset,
                                                    this->screenY - 10, false);
                    }
                }
            }
        }

        if (turretMode || stationCandidateActive) {
            goto planetMarkerDone;
        }

        if (this->lockedPlanetTarget != nullptr) {
            if (this->lockedPlanetTarget == this->candidatePlanetTarget) {
                this->field_0x218_byte = 1;
            }
            this->lockedPlanetTarget = nullptr;
        }

        if (!candidateFound || radar_level_player(this)->isDockingToAsteroid() ||
            radar_level_player(this)->isDockingToDockingPoint()) {
            this->field_0x1a0 = 0;
            this->candidatePlanetTarget = nullptr;
            goto planetMarkerDone;
        }

        this->field_0x1a0 += elapsed;
        if (this->field_0x1a0 > this->field_0x1b8) {
            if (missionTargetFilter) {
                hud->hudEvent(21, radar_level_player(this), 0);
                goto planetMarkerDone;
            } else {
                if (this->field_0x218_byte == 0) {
                    Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
                }
                this->lockedPlanetTarget = this->candidatePlanetTarget;
                if (this->field_0x1a8 != 0) {
                    radar_level_player(this)->dockToPlanet();
                }
            }
        }

        if (!missionTargetFilter && this->field_0x1a0 >= 1) {
            Sprite *blip;
            int frame;
            if (this->candidatePlanetTarget == nullptr ||
                this->candidatePlanetTarget == this->lockedPlanetTarget) {
                if (radar_level_player(this)->isDockingToPlanet() != 0) {
                    goto planetMarkerDone;
                }
                blip = this->blipSprite;
                frame = blip->getRawFrameCount() - 1;
            } else {
                blip = this->blipSprite;
                int frameMax = blip->getRawFrameCount() - 1;
                frame = static_cast<int>(
                    (static_cast<float>(this->field_0x1a0) / static_cast<float>(this->field_0x1b8)) *
                    static_cast<float>(frameMax));
            }
            blip->setFrame(frame);
            this->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                                  static_cast<int>(PlayerEgo::crosshairPos.y));
            this->blipSprite->draw(1.0f, 1.0f);
        }

        goto planetMarkerDone;
    }
planetMarkerDone:

    // Keep the enemy pass in draw: the recovered ARM body owns its candidate,
    // timer, position-scratch and HUD work in one local control-flow region.
    bool type10Threat = false;
    bool alwaysEnemyThreat = false;
    bool enemyCandidateActive = false;
    if (this->lockedEnemy != nullptr && (this->lockedEnemy->isDying() || this->lockedEnemy->isDead())) {
        this->lockedEnemy = nullptr;
        this->candidateEnemy = nullptr;
        enemyLockBlocked = false;
    }

    if (this->scannerAvailable != 0 && this->enemyTargets != nullptr) {
        Vector &enemyPositionScratch = *reinterpret_cast<Vector *>(&this->field_0x16c);
        Vector &egoPositionScratch = *reinterpret_cast<Vector *>(&this->field_0x178);

        for (unsigned i = 0; i < this->enemyTargets->size(); ++i) {
            KIPlayer *target = (*this->enemyTargets)[i];
            if (target->player->isActive() == 0 || (target->isDying() && raw_u8(target, 0x4c) == 0) ||
                target->field_0x74 != 0) {
                if (target->shipGroup == 10 && target->player->isActive() != 0 && !target->isDead() &&
                    !target->isDying()) {
                    ++this->field_0x1bc;
                    if (target->shipGroup == 10) {
                        type10Threat = true;
                    }
                }
                continue;
            }

            this->update(target);
            bool deadCargo = raw_u8(target, 0x4c) != 0 && (target->isDead() || target->isDying());
            target->field_0x75 = 0;
            if ((target->isDead() || target->isDying()) && raw_u8(target, 0x4c) == 0) {
                continue;
            }
            KIPlayer *enemyLock = this->lockedEnemy;

            if (target->stealFlagByte == 0 && !((target->countsAsEnemyExcludeFlag != 0) | deadCargo) &&
                target->player->enemyFlagsLo != 0) {
                ++this->field_0x1bc;
                if (target->shipGroup == 10) {
                    type10Threat = true;
                }
                if (target->field_0x42 != 0 && target->player->isAlwaysEnemy() != 0) {
                    alwaysEnemyThreat = true;
                }
            }

            if (this->onScreen == 0) {
                target->field_0x73 = 0;
                target->field_0x76 = 0;

                int imageOffset;
                if (deadCargo) {
                    KIPlayer *blipTarget = (*this->enemyTargets)[i];
                    imageOffset = 0xb0;
                    if (blipTarget->countsAsEnemyExcludeFlag == 0) {
                        imageOffset = blipTarget->shipGroup == 9 ? 0xb4 : 0xa8;
                    }
                } else if (target->player->enemyFlagsLo != 0) {
                    imageOffset = target == enemyLock ? 0x90 : 0x60;
                } else if (target->player->carriesFriendCargoFlag != 0) {
                    imageOffset = target == enemyLock ? 0x94 : 0x68;
                } else {
                    imageOffset = target == enemyLock ? 0x98 : 0x78;
                }

                Globals::Canvas->DrawImage2D(radar_image_slot(this, imageOffset), this->screenX,
                                             this->screenY, 0x11u, 0x44u);
            } else {
                target->field_0x76 = 1;

                egoPositionScratch = player->getPosition();
                enemyPositionScratch = target->player->getPosition();
                float dx = enemyPositionScratch.x - egoPositionScratch.x;
                float dy;
                float dz;
                bool farOrSpecial = dx > 24000.0f || dx < -24000.0f ||
                                    (dy = enemyPositionScratch.y - egoPositionScratch.y, dy > 24000.0f) ||
                                    dy < -24000.0f ||
                                    (dz = enemyPositionScratch.z - egoPositionScratch.z, dz > 24000.0f) ||
                                    dz < -24000.0f || target->getType() == 16917;

                if (farOrSpecial) {
                    int imageOffset = 0xa0;
                    if (!deadCargo) {
                        if (target == enemyLock) {
                            if (target->player->enemyFlagsLo != 0) {
                                imageOffset = 0x7c;
                            } else {
                                imageOffset = target->player->carriesFriendCargoFlag != 0 ? 0x80 : 0x84;
                            }
                        } else if (target->player->enemyFlagsLo != 0) {
                            imageOffset = 0x60;
                        } else {
                            imageOffset = target->player->carriesFriendCargoFlag != 0 ? 0x68 : 0x78;
                        }
                    }
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, imageOffset), this->screenX,
                                                 this->screenY, 0x11u, 0x44u);

                    target->field_0x73 = 0;
                    Vector crosshair = PlayerEgo::crosshairPos;
                    Vector screen = {static_cast<float>(this->screenX), static_cast<float>(this->screenY),
                                     0.0f};
                    if (AbyssEngine::AEMath::VectorLength(crosshair - screen) < 60000.0f) {
                        int x = static_cast<int>(crosshair.x);
                        int y = static_cast<int>(crosshair.y);
                        target->field_0x73 =
                            this->screenX > x - this->field_0x124 && this->screenX < x + this->field_0x124 &&
                            this->screenY > y - this->field_0x124 && this->screenY < y + this->field_0x124;
                    }

                    if (target == enemyLock) {
                        this->lockLabel = this->calcDistance(egoPositionScratch.x, egoPositionScratch.y,
                                                             egoPositionScratch.z, this->cameraPosX,
                                                             this->cameraPosY, this->cameraPosZ);
                        Globals::Canvas->DrawString(radar_font(), this->lockLabel,
                                                    this->screenX - this->halfScreenHeight,
                                                    this->screenY + this->halfScreenWidth, false);
                    }
                } else if (deadCargo) {
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, 0x9c), this->screenX, this->screenY,
                                                 0x11u, 0x44u);
                } else {
                    if (!target->isDead()) {
                        int x = static_cast<int>(PlayerEgo::crosshairPos.x);
                        int y = static_cast<int>(PlayerEgo::crosshairPos.y);
                        target->field_0x73 =
                            this->screenX > x - this->field_0x124 && this->screenX < x + this->field_0x124 &&
                            this->screenY > y - this->field_0x124 && this->screenY < y + this->field_0x124;
                    }

                    int healthBackground;
                    int healthFill;
                    if (target->player->enemyFlagsLo != 0) {
                        healthBackground = 0xe0;
                        healthFill = 0xd0;
                    } else if (target->player->carriesFriendCargoFlag != 0) {
                        healthBackground = 0xe4;
                        healthFill = 0xd4;
                    } else {
                        healthBackground = 0xe8;
                        healthFill = 0xd8;
                    }

                    unsigned int healthFillImage = radar_image_slot(this, healthFill);
                    int barX = this->screenX + 2 - this->halfScreenHeight;
                    int barY = this->screenY + this->halfScreenWidth + 2;
                    Globals::Canvas->DrawImage2D(radar_image_slot(this, healthBackground), barX, barY);

                    int healthWidth =
                        static_cast<int>((static_cast<float>(target->player->getDamageRate()) / 100.0f) *
                                         static_cast<float>(this->lockPanelWidth));
                    this->field_0x184 = healthWidth;
                    Globals::Canvas->DrawRegion2D(healthFillImage, 0, 0, healthWidth, this->lockPanelHeight,
                                                  0.0f, 0, 0, this->screenX + 3 - this->halfScreenHeight,
                                                  this->screenY + this->halfScreenWidth + 3);

                    if (target->player->getEmpPoints() < target->player->getMaxEmpPoints()) {
                        Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xec),
                                                     this->screenX + 2 - this->halfScreenHeight,
                                                     this->screenY + this->halfScreenWidth + 8);
                        int empWidth = static_cast<int>(
                            (static_cast<float>(target->player->getEmpDamageRate()) / 100.0f) *
                            static_cast<float>(this->lockPanelWidth));
                        this->field_0x184 = empWidth;
                        Globals::Canvas->DrawRegion2D(radar_image_slot(this, 0xdc), 0, 0, empWidth,
                                                      this->lockPanelHeight, 0.0f, 0, 0,
                                                      this->screenX + 3 - this->halfScreenHeight,
                                                      this->screenY + this->halfScreenWidth + 9);
                    }

                    if (target == enemyLock) {
                        int lockedOverlay = 0x5c;
                        if (target->player->enemyFlagsLo == 0) {
                            lockedOverlay = target->player->carriesFriendCargoFlag != 0 ? 0x64 : 0x74;
                        }
                        Globals::Canvas->DrawImage2D(radar_image_slot(this, lockedOverlay), this->screenX,
                                                     this->screenY, 0x11u, 0x44u);
                    }
                }

                if (!radar_level_player(this)->isAutoPilot() &&
                    !radar_level_player(this)->isDockedToDockingPoint()) {
                    bool candidateGate = (deadCargo || target->countsAsEnemyExcludeFlag == 0) &&
                                         (!enemyCandidateActive || target->field_0x24 != 0);
                    if (candidateGate) {
                        if (deadCargo) {
                            if (!target->isDead()) {
                                goto enemyAfterCandidate;
                            }
                            if (this->field_0x1c == nullptr && this->field_0x1ae != 0) {
                                this->candidateEnemy = target;
                                this->field_0x1c = target;
                                enemyCandidateActive = true;
                                goto enemyAfterCandidate;
                            }
                        }
                        if (radar_crosshair_contains(this, this->field_0x124)) {
                            target->field_0x75 = 1;
                            if ((deadCargo && this->field_0x1ad != 0) ||
                                (raw_u8(this, 0x1b0) != 0 && this->field_0x1a8 == 0)) {
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

        enemyAfterCandidate:
            if (this->field_0x1af != 0 && this->field_0x1c == nullptr && deadCargo && target->isDead()) {
                this->candidateEnemy = target;
                this->field_0x1c = target;
                enemyCandidateActive = true;
            }
        }

        if (!radar_level_player(this)->isDockingToAsteroid() &&
            !(radar_level_player(this)->isDockingToDockingPoint() | turretMode | enemyLockBlocked)) {
            KIPlayer *candidate = this->candidateEnemy;
            bool candidateDeadCargo = candidate != nullptr && raw_u8(candidate, 0x4c) != 0 &&
                                      (candidate->isDead() || this->candidateEnemy->isDying());

            if (this->lockedEnemy != nullptr &&
                (this->lockedEnemy->isDying() || this->lockedEnemy->isDead())) {
                this->lockedEnemy = nullptr;
            }

            if (!enemyCandidateActive) {
                KIPlayer **lockSlot = candidateDeadCargo ? &this->field_0x1c : &this->lockedEnemy;
                this->field_0x198 = 0;
                if (*lockSlot == nullptr) {
                    this->candidateEnemy = nullptr;
                }
            } else {
                this->field_0x198 += elapsed;
                bool fastLock = candidateDeadCargo || this->candidateEnemy->field_0x24 != 0;
                int *lockThreshold = fastLock ? &this->field_0x1b4 : &this->field_0x1b8;
                if (this->field_0x198 > *lockThreshold) {
                    if (candidateDeadCargo || this->candidateEnemy->field_0x24 != 0) {
                        this->field_0x1c = this->candidateEnemy;
                        if (this->field_0x1ad == 0) {
                            hud->hudEvent(9, radar_level_player(this), 0);
                            this->field_0x1c = nullptr;
                        }
                    } else if (this->lockedEnemy != this->candidateEnemy) {
                        Globals::sound->play(0x1a, nullptr, nullptr, 0.0f);
                        this->lockedEnemy = this->candidateEnemy;

                        if (this->field_0x1a9 != 0) {
                            egoPositionScratch = player->getPosition();
                            enemyPositionScratch = this->lockedEnemy->player->getPosition();
                            float dx = egoPositionScratch.x - enemyPositionScratch.x;
                            float dy;
                            float dz;
                            if (dx < 24000.0f || dx > -24000.0f ||
                                (dy = enemyPositionScratch.y - egoPositionScratch.y, dy < 24000.0f) ||
                                dy > -24000.0f ||
                                (dz = enemyPositionScratch.z - egoPositionScratch.z, dz < 24000.0f) ||
                                dz > -24000.0f) {
                                Array<int> *cargo = this->lockedEnemy->cargo;
                                if (cargo != nullptr) {
                                    for (unsigned cargoIndex = 0; cargoIndex < cargo->size();
                                         cargoIndex += 2) {
                                        int amount = (*cargo)[cargoIndex + 1];
                                        if (amount >= 1) {
                                            hud->catchCargo((*cargo)[0], (*cargo)[1], false, false, false,
                                                            true, false);
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

                int drawDelay = candidateDeadCargo || this->candidateEnemy->field_0x24 != 0 ? 500 : 0;
                if (this->field_0x198 > drawDelay) {
                    candidate = this->candidateEnemy;
                    if (candidate != nullptr) {
                        KIPlayer **resolvedLock = candidateDeadCargo || candidate->field_0x24 != 0
                                                      ? &this->field_0x1c
                                                      : &this->lockedEnemy;
                        if (candidate != *resolvedLock) {
                            Sprite *blip = this->blipSprite;
                            float frameMax = static_cast<float>(blip->getRawFrameCount() - 1);
                            int progressElapsed = this->field_0x198;
                            int progressThreshold;
                            if (candidateDeadCargo) {
                                progressElapsed -= 500;
                                progressThreshold = this->field_0x1b4 - 500;
                            } else {
                                if (this->candidateEnemy->field_0x24 != 0) {
                                    progressElapsed -= 500;
                                }
                                progressThreshold = this->candidateEnemy->field_0x24 != 0
                                                        ? this->field_0x1b4 - 500
                                                        : this->field_0x1b8;
                            }
                            int frame = static_cast<int>((static_cast<float>(progressElapsed) /
                                                          static_cast<float>(progressThreshold)) *
                                                         frameMax);
                            blip->setFrame(frame);
                            this->blipSprite->setRefPixelPosition(
                                static_cast<int>(PlayerEgo::crosshairPos.x),
                                static_cast<int>(PlayerEgo::crosshairPos.y));
                            this->blipSprite->draw(1.0f, 1.0f);
                        }
                    }
                }
            }
        }
    }

    {
        Radar *self = this;
        bool suppressCandidate = enemyCandidateActive;
    if (self->gasCloudTargets == nullptr) {
            goto gasMarkerDone;
        }

        bool lockBlocked = (self->candidateEnemy != nullptr && self->lockedEnemy == nullptr) ||
                           self->field_0x1c != nullptr || self->lockedStation != nullptr ||
                           self->lockedPlanetTarget != nullptr;
        bool candidateFound = false;

        for (unsigned i = 0; i < self->gasCloudTargets->size(); ++i) {
            auto *cloud = static_cast<PlayerGasCloud *>((*self->gasCloudTargets)[i]);
            radar_update_gas_cloud_sparks(self, status, cloud, turretMode);

            if (cloud->player->isActive() == 0 || cloud->isDying() ||
                Globals::status->getShip()->getFirstEquipmentOfSort(33)->getAttribute(57) != 1) {
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
                *reinterpret_cast<Vector *>(&self->field_0x16c) = player->getPosition();
                *reinterpret_cast<Vector *>(&self->field_0x178) = cloud->player->getPosition();
                // Android 0x12f64c..0x12f6cc observes this byte before and after drawing.
                Vector &playerPosition = *reinterpret_cast<Vector *>(&self->field_0x16c);
                Vector &cloudPosition = *reinterpret_cast<Vector *>(&self->field_0x178);
                float dx = playerPosition.x - cloudPosition.x;
                float dy;
                float dz;
                if (dx > 24000.0f || dx < -24000.0f ||
                    (dy = playerPosition.y - cloudPosition.y, dy > 24000.0f) || dy < -24000.0f ||
                    (dz = playerPosition.z - cloudPosition.z, dz > 24000.0f) || dz < -24000.0f) {
                    cloud->field_0x73 = 0;
                }
                Globals::Canvas->DrawImage2D(radar_image_slot(self, 0x8c), self->screenX, self->screenY, 0x11u,
                                             0x44u);
                cloud->field_0x73 = 0;

                if (previousLock == cloud) {
                    Vector &cloudPosition = *reinterpret_cast<Vector *>(&self->field_0x178);
                    self->lockLabel = self->calcDistance(cloudPosition.x, cloudPosition.y, cloudPosition.z,
                                                         self->cameraPosX, self->cameraPosY, self->cameraPosZ);
                    Globals::Canvas->DrawString(radar_font(), self->lockLabel,
                                                self->screenX - self->halfScreenHeight,
                                                self->screenY + self->halfScreenWidth, false);
                }

                if (!radar_level_player(self)->isAutoPilot() && !lockBlocked && !suppressCandidate &&
                    radar_crosshair_contains(self, self->field_0x124)) {
                    cloud->field_0x75 = 1;
                    if (raw_u8(self, 0x1b0) != 0 && self->field_0x1a8 == 0) {
                        if (self->candidateGasCloud != cloud) {
                            self->gasLockTimer = 0;
                        }
                        self->candidateGasCloud = cloud;
                        candidateFound = true;
                    }
                }
            } else if (Globals::status->getShip()->getFirstEquipmentOfSort(33)->getAttribute(58) == 1) {
                cloud->field_0x73 = 0;
                cloud->field_0x76 = 0;
                Globals::Canvas->DrawImage2D(radar_image_slot(self, 0xc0), self->screenX, self->screenY, 0x11u,
                                             0x44u);
            }
        }

        if (turretMode || suppressCandidate) {
            goto gasMarkerDone;
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
        resolvedLock = nullptr;

        if (!candidateFound) {
            self->gasLockTimer = 0;
            self->candidateGasCloud = nullptr;
            goto gasMarkerDone;
        }

        int lockThreshold = self->field_0x1b8 - 200;
        self->gasLockTimer += elapsed;
        if (self->gasLockTimer > lockThreshold) {
            radar_play_lock_cue(self);
            resolvedLock = self->candidateGasCloud;
            self->lockedGasCloud = self->candidateGasCloud;
        }

        if (self->gasLockTimer > 500) {
            Sprite *blip = self->blipSprite;
            KIPlayer *candidate = self->candidateGasCloud;
            int frame = blip->getRawFrameCount() - 1;
            if (candidate != nullptr && candidate != resolvedLock) {
                frame = static_cast<int>(
                    (static_cast<float>(self->gasLockTimer - 500) / static_cast<float>(self->field_0x1b8 - 500)) *
                    static_cast<float>(frame));
                if (self->blipSprite->getRawFrameCount() - 1 <= frame) {
                    goto gasMarkerDone;
                }
                blip = self->blipSprite;
            }
            blip->setFrame(frame);
            self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                                  static_cast<int>(PlayerEgo::crosshairPos.y));
            self->blipSprite->draw(1.0f, 1.0f);
        }
        goto gasMarkerDone;
    }
gasMarkerDone:

    {
        Radar *self = this;
        Player *ego = player;
        bool suppressCandidate = enemyCandidateActive;
    for (int i = 0; i < 5; ++i) {
            self->radarSlots[i] = -1;
        }

        bool lockBlocked = (self->candidateEnemy != nullptr && self->lockedEnemy == nullptr) ||
                           self->field_0x1c != nullptr || self->lockedStation != nullptr ||
                           self->lockedPlanetTarget != nullptr;

        Vector &targetPosition = *reinterpret_cast<Vector *>(&self->field_0x16c);
        Vector &egoPosition = *reinterpret_cast<Vector *>(&self->field_0x178);
        if (radar_level_player(self)->isInDockingProcedure() || self->asteroidTargets == nullptr) {
            if (radar_level_player(self)->isDockingToAsteroid() && self->lockedAsteroid != nullptr) {
                targetPosition = self->lockedAsteroid->getPosition();
                self->lockLabel = self->calcDistance(targetPosition.x, targetPosition.y, targetPosition.z,
                                                     self->cameraPosX, self->cameraPosY, self->cameraPosZ);
                Globals::Canvas->DrawString(
                    radar_font(), self->lockLabel,
                    static_cast<int>(PlayerEgo::crosshairPos.x - static_cast<float>(self->halfScreenHeight)),
                    static_cast<int>(PlayerEgo::crosshairPos.y + static_cast<float>(self->halfScreenWidth)),
                    false);
            }
            goto asteroidMarkerDone;
        }

        bool centerRangeQuality = radar_asteroid_center_range_enabled(self);
        bool candidateFound = false;
        int slotCount = 0;

        for (unsigned i = 0; i < self->asteroidTargets->size(); ++i) {
            auto *asteroid = static_cast<PlayerAsteroid *>((*self->asteroidTargets)[i]);
            if ((raw_u8(asteroid, 0x4c) == 0 || (!asteroid->isDead() && !asteroid->isDying())) &&
                (asteroid->isDead() || asteroid->isDying())) {
                continue;
            }

            self->update(asteroid);
            bool deadCargo = raw_u8(asteroid, 0x4c) != 0 && (asteroid->isDead() || asteroid->isDying());

            if (self->onScreen != 0) {
                if (((radar_level_player(self)->isAutoPilot() | lockBlocked | suppressCandidate |
                      candidateFound | turretMode) & 1) == 0) {
                    if (asteroid->isMinable() != 0 && self->field_0x1a8 == 0) {
                        if (deadCargo && self->field_0x1c == nullptr && self->field_0x1ae != 0) {
                            self->candidateEnemy = asteroid;
                            self->field_0x1c = asteroid;
                            candidateFound = true;
                            goto asteroidAfterCandidate;
                        }
                        int radius = self->field_0x124;
                        int left = static_cast<int>(PlayerEgo::crosshairPos.x - static_cast<float>(radius));
                        int top = static_cast<int>(PlayerEgo::crosshairPos.y - static_cast<float>(radius));
                        if (self->screenX > left && self->screenX < left + 2 * radius) {
                            // Android 0x12fba4/0x12fbd6 clears the per-row flag, not the slot count.
                            candidateFound = false;
                            if (self->screenY <= top || self->screenY >= top + 2 * radius) {
                                goto asteroidAfterCandidate;
                            }
                            if (asteroid->asteroidFlag == 0) {
                                candidateFound = false;
                                if (slotCount > 3 || raw_u8(self, 0x1b0) == 0) {
                                    goto asteroidAfterCandidate;
                                }
                                self->radarSlots[slotCount++] = static_cast<int>(i);
                            }
                        }
                    }
                    candidateFound = false;
                }

            asteroidAfterCandidate:
                radar_draw_asteroid_quality_indicator(self, centerRangeQuality, asteroid);
                if (deadCargo) {
                    radar_draw_asteroid_dead_cargo_marker(self, true);
                }
            } else if (deadCargo) {
                radar_draw_asteroid_dead_cargo_marker(self, false);
            }

            if (self->field_0x1af != 0 && self->field_0x1c == nullptr && deadCargo) {
                self->candidateEnemy = asteroid;
                self->field_0x1c = asteroid;
                candidateFound = true;
            }
        }

        int nearestIndex = -1;
        int nearestDistance = 999999;
        for (int i = 0; i < 5; ++i) {
            int targetIndex = self->radarSlots[i];
            if (targetIndex < 0) {
                continue;
            }
            KIPlayer *target = (*self->asteroidTargets)[static_cast<unsigned>(targetIndex)];
            targetPosition = target->getPosition();
            egoPosition = ego->getPosition();
            targetPosition -= egoPosition;
            int distance = static_cast<int>(AbyssEngine::AEMath::VectorLength(targetPosition));
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearestIndex = targetIndex;
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
            goto asteroidMarkerDone;
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
        resolvedLock = nullptr;

        KIPlayer **candidateSlot = &self->candidateAsteroid;
        KIPlayer *candidate = *candidateSlot;
        bool candidateDeadCargo = candidate != nullptr && raw_u8(candidate, 0x4c) != 0 &&
                                  (candidate->isDead() || (*candidateSlot)->isDying());
        if (!candidateFound || radar_level_player(self)->isDockingToAsteroid()) {
            KIPlayer **lockSlot = candidateDeadCargo ? &self->field_0x1c : &self->lockedAsteroid;
            self->asteroidLockTimer = 0;
            if (*lockSlot == nullptr) {
                *candidateSlot = nullptr;
            }
            goto asteroidMarkerDone;
        }

        int lockThreshold = self->field_0x1b8 - 200;
        self->asteroidLockTimer += elapsed;
        if (self->asteroidLockTimer > lockThreshold) {
            PlayerEgo *eventPlayer;
            int eventId;
            if (candidateDeadCargo) {
                if (self->field_0x1ad != 0) {
                    self->field_0x1c = *candidateSlot;
                    goto asteroidLockResolved;
                } else {
                    eventPlayer = radar_level_player(self);
                    eventId = 9;
                }
            } else if (self->field_0x1ac == 0) {
                eventPlayer = radar_level_player(self);
                eventId = 20;
            } else {
                radar_play_lock_cue(self);
                self->lockedAsteroid = *candidateSlot;
                goto asteroidLockResolved;
            }
            hud->hudEvent(eventId, eventPlayer, 0);
        }

    asteroidLockResolved:
        if (self->asteroidLockTimer > 500) {
            KIPlayer **lockSlot;
            Sprite *blip;
            int frame;
            int frameMax;
            if (*candidateSlot == nullptr) {
                goto asteroidLastFrame;
            }
            lockSlot = candidateDeadCargo ? &self->field_0x1c : &self->lockedAsteroid;
            if (*candidateSlot == *lockSlot) {
                goto asteroidLastFrame;
            }

            // Android 0x1301c6..0x1301d0 reloads timer/threshold after the Sprite query.
            frameMax = self->blipSprite->getRawFrameCount() - 1;
            frame = static_cast<int>(
                (static_cast<float>(self->asteroidLockTimer - 500) /
                 static_cast<float>(self->field_0x1b8 - 500)) *
                static_cast<float>(frameMax));
            if (self->blipSprite->getRawFrameCount() - 1 <= frame) {
                goto asteroidMarkerDone;
            }
            blip = self->blipSprite;
            goto asteroidDrawBlip;

        asteroidLastFrame:
            blip = self->blipSprite;
            frame = blip->getRawFrameCount() - 1;
        asteroidDrawBlip:
            blip->setFrame(frame);
            self->blipSprite->setRefPixelPosition(static_cast<int>(PlayerEgo::crosshairPos.x),
                                                  static_cast<int>(PlayerEgo::crosshairPos.y));
            self->blipSprite->draw(1.0f, 1.0f);
        }
        goto asteroidMarkerDone;
    }
asteroidMarkerDone:

    FModSound *sound = Globals::sound;
    int currentEvent = sound->currentMusicEvent;
    if (currentEvent == 143) {
        return;
    }

    if (this->field_0x1bc < 1) {
        this->field_0x54 = 0;
        if (Globals::status->getCurrentCampaignMission() == 145) {
            goto resetThreatCounter;
        }
        int spaceEvent = Globals::sound->currentMusicEvent;
        if (radar_is_space_ambient_keep_event(spaceEvent)) {
            goto resetThreatCounter;
        }
        Globals::sound->stop(spaceEvent);
        if (Globals::status->inAlienOrbit() || Globals::status->getStation()->isAttackedByAliens()) {
            Globals::sound->play(0x91, nullptr, nullptr, 0.0f);
            goto resetThreatCounter;
        }
        if (Globals::status->getCurrentCampaignMission() == 1) {
            Globals::sound->play(0x8f, nullptr, nullptr, 0.0f);
            goto resetThreatCounter;
        }
        int race = Globals::status->getSystem()->getRace();
        if (Globals::status->getStation()->getIndex() == 108) {
            Globals::sound->play(0x92, nullptr, nullptr, 0.0f);
            goto resetThreatCounter;
        }
        if (Globals::status->getStation()->getIndex() == 101) {
            Globals::sound->play(0x93, nullptr, nullptr, 0.0f);
            goto resetThreatCounter;
        }
        if (Globals::status->inSupernovaSystem()) {
            if (Globals::status->getMission() == nullptr || Globals::status->getMission()->isEmpty() ||
                Globals::status->getMission()->getTargetStation() !=
                    Globals::status->getStation()->getIndex()) {
                Globals::sound->play(0x94, nullptr, nullptr, 0.0f);
                goto resetThreatCounter;
            }
            int track = Globals::status->getCurrentCampaignMission() < 106 ? 2241 : 2242;
            Globals::sound->play(track, nullptr, nullptr, 0.0f);
        } else if (Globals::status->inDeepScienceOrbit()) {
            Globals::sound->play(0x98, nullptr, nullptr, 0.0f);
        } else {
            Globals::sound->play(radar_race_space_ambient_track(race), nullptr, nullptr, 0.0f);
        }
    } else {
        this->field_0x54 = 1;
        if (radar_is_combat_ambient_keep_event(currentEvent, alwaysEnemyThreat)) {
            goto resetThreatCounter;
        }
        sound->stop(currentEvent);
        if (Globals::status->inAlienOrbit() || Globals::status->getStation()->isAttackedByAliens() ||
            Globals::status->getCurrentCampaignMission() == 16) {
            Globals::sound->play(0x88, nullptr, nullptr, 0.0f);
        } else if (this->field_0x1bc <= 2) {
            if (alwaysEnemyThreat) {
                Globals::sound->play(0x97, nullptr, nullptr, 0.0f);
            } else if (type10Threat) {
                Globals::sound->play(0x95, nullptr, nullptr, 0.0f);
            } else {
                Globals::sound->play(0x8c, nullptr, nullptr, 0.0f);
            }
        } else if (this->field_0x1bc > 4) {
            if (alwaysEnemyThreat) {
                Globals::sound->play(0x97, nullptr, nullptr, 0.0f);
            } else {
                Globals::sound->play(type10Threat ? 0x96 : 0x8e, nullptr, nullptr, 0.0f);
            }
        } else if (alwaysEnemyThreat) {
            Globals::sound->play(0x97, nullptr, nullptr, 0.0f);
        } else if (type10Threat) {
            Globals::sound->play(0x95, nullptr, nullptr, 0.0f);
        } else {
            Globals::sound->play(0x8d, nullptr, nullptr, 0.0f);
        }
    }

resetThreatCounter:
    this->field_0x1bc = 0;
    return;
}

Vector Radar::elipsoidIntersect(int y, int x, Vector value) {
    float dy = static_cast<float>(this->centerY - x);
    float dx = static_cast<float>(this->centerX - y);
    float distance = this->weightY * (dy * dy) + this->weightX * (dx * dx);

    if (distance >= 0.0f) {
        float scale = (distance - static_cast<Globals *>(Globals::globals)->sqrt(distance)) / distance;
        if (scale >= 0.0f && scale <= 1.0f) {
            value.y = static_cast<float>(static_cast<int>(static_cast<float>(x) + scale * dy));
            value.x = static_cast<float>(static_cast<int>(static_cast<float>(y) + scale * dx));
        }
    }

    return value;
}

void Radar::drawCurrentLock(Hud *) {
    if (this->enabled != 0) {
        Radar::drawTarget = 1;

        if (this->lockedPlanetTarget != nullptr) {
            Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xcc), this->lockPanelX, this->lockPanelY);
            Status *status = Globals::status;
            auto *planetNames =
                reinterpret_cast<Array<String *> *>(static_cast<intptr_t>(status->getPlanetNames()));
            String planetName(*(*planetNames)[this->planetDockIndex], false);
            PaintCanvas *drawCanvas = Globals::Canvas;
            int width = Globals::w;
            unsigned int font = Globals::font;
            int textWidth = drawCanvas->GetTextWidth(font, planetName);
            drawCanvas->DrawString(font, planetName, (width >> 1) - textWidth / 2,
                                   this->lockPanelY + layout_i32(Globals::layout, 0xbc), false);
        } else {
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

            if (target != nullptr) {
                Globals::Canvas->DrawImage2D(radar_image_slot(this, 0xcc), this->lockPanelX,
                                             this->lockPanelY);

                KIPlayer *asteroidLock = this->lockedAsteroid;
                KIPlayer *stationLock = this->lockedStation;
                KIPlayer **landmarkData = this->landmarkTargets->data_;
                String panelLabel(stationLock == landmarkData[0]
                                      ? *this->labelStrings->data_[0]
                                      : stationLock == landmarkData[3] ? *this->labelStrings->data_[3]
                                                                       : *this->labelStrings->data_[1],
                                  false);

                bool gasLabel = target == this->lockedGasCloud;
                bool asteroidLabel = !gasLabel && target == asteroidLock;
                String line = gasLabel
                                  ? String(*static_cast<GameText *>(Globals::gameText)->getText(3236), false)
                                  : asteroidLabel ? String(*static_cast<GameText *>(Globals::gameText)
                                                                ->getText(raw_i32(target, 0x128) + 1274),
                                                           false)
                                                  : String();
                if (gasLabel) {
                    int width = Globals::w;
                    int textWidth = Globals::Canvas->GetTextWidth(radar_font(), line);
                    Globals::Canvas->DrawString(radar_font(), line, (width >> 1) - (textWidth >> 1),
                                                this->lockPanelY + layout_i32(Globals::layout, 0xc4), false);
                } else if (asteroidLabel) {
                    int width = Globals::w;
                    int textWidth = Globals::Canvas->GetTextWidth(radar_font(), line);
                    int textX = (width >> 1) - (textWidth >> 1);
                    Sprite *quality = this->qualitySprite;
                    int qualityFrame = static_cast<PlayerAsteroid *>(target)->getQualityFrameIndex();
                    quality->setFrame(qualityFrame);
                    quality = this->qualitySprite;
                    int margin = layout_i32(Globals::layout, 0x2c);
                    int frameWidth = quality->getFrameWidth();
                    quality->setPosition(textX - margin - frameWidth,
                                         this->lockPanelY + layout_i32(Globals::layout, 0xc0));
                    this->qualitySprite->draw(1.0f, 1.0f);
                    Globals::Canvas->DrawString(radar_font(), line, textX,
                                                this->lockPanelY + layout_i32(Globals::layout, 0xc4), false);
                } else {
                    bool normalLabel =
                        target->name.size() == 0 ||
                        (target->name.Compare(*static_cast<GameText *>(Globals::gameText)->getText(1611)) !=
                             0 &&
                         target->name.Compare(*static_cast<GameText *>(Globals::gameText)->getText(1663)) !=
                             0 &&
                         raw_u8(target, 0x42) == 0);

                    int textX;
                    if (normalLabel) {
                        const String *lineSource;
                        if (target != stationLock && target->name.size() == 0) {
                            lineSource =
                                static_cast<GameText *>(Globals::gameText)->getText(target->shipGroup + 406);
                        } else if (target == stationLock) {
                            lineSource = &panelLabel;
                        } else {
                            lineSource = &target->name;
                        }
                        line = *lineSource;

                        if (target == stationLock) {
                            if (Globals::status->inAlienOrbit()) {
                                if (Globals::status->dlc1Won()) {
                                    this->raceSprite->setFrame(8);
                                } else {
                                    this->raceSprite->setFrame(9);
                                }
                            } else {
                                Sprite *raceSprite = this->raceSprite;
                                SolarSystem *system = Globals::status->getSystem();
                                int race = system->getRace();
                                raceSprite->setFrame(race);
                            }
                            int width = Globals::w;
                            int textWidth = Globals::Canvas->GetTextWidth(radar_font(), line);
                            textX = (width >> 1) - (textWidth >> 1);
                            Sprite *raceSprite = this->raceSprite;
                            int margin = layout_i32(Globals::layout, 0x2c);
                            int frameWidth = raceSprite->getFrameWidth();
                            raceSprite->setPosition(textX - margin - frameWidth,
                                                    this->lockPanelY + layout_i32(Globals::layout, 0xc8));
                            this->raceSprite->draw(1.0f, 1.0f);
                        } else {
                            line += String(
                                String(" ") + String(target->player->getDamageRate()) + String("%"), false);
                            int width = Globals::w;
                            int textWidth = Globals::Canvas->GetTextWidth(radar_font(), line);
                            textX = (width >> 1) - (textWidth >> 1);
                        }

                        Globals::Canvas->DrawString(radar_font(), line, textX,
                                                    this->lockPanelY + layout_i32(Globals::layout, 0xc4),
                                                    false);
                    } else {
                        Globals::Canvas->SetColor(0xff2a00ffu);
                        line = target->name;
                        if (raw_u8(target, 0x42) != 0) {
                            line += String(
                                String(" ") + String(target->player->getDamageRate()) + String("%"), false);
                        }
                        int width = Globals::w;
                        int textWidth = Globals::Canvas->GetTextWidth(radar_font(), line);
                        textX = (width >> 1) - (textWidth >> 1);
                        Globals::Canvas->DrawString(radar_font(), line, textX,
                                                    this->lockPanelY + layout_i32(Globals::layout, 0xc4),
                                                    false);
                        Globals::Canvas->SetColor(0xffffffffu);
                    }

                    bool suppressRaceIcon = target == stationLock;
                    int raceFrame;
                    if (target != stationLock) {
                        raceFrame = target->shipGroup;
                        suppressRaceIcon = raceFrame == 10;
                    }
                    if (!suppressRaceIcon) {
                        this->raceSprite->setFrame(raceFrame);
                        Sprite *raceSprite = this->raceSprite;
                        int margin = layout_i32(Globals::layout, 0x2c);
                        int frameWidth = raceSprite->getFrameWidth();
                        raceSprite->setPosition(textX - margin - frameWidth,
                                                this->lockPanelY + layout_i32(Globals::layout, 0xc8));
                        this->raceSprite->draw(1.0f, 1.0f);
                    }
                }
            } else {
                Radar::drawTarget = 0;
            }
        }
    }
    return;
}

String Radar::calcDistance(float x, float y, float z, float originX, float originY, float originZ) {
    float diffX = x * 0.5f - originX * 0.5f;
    float diffY = y * 0.5f - originY * 0.5f;
    float diffZ = z * 0.5f - originZ * 0.5f;
    long long total = static_cast<long long>(diffY) * static_cast<long long>(diffY) +
                      static_cast<long long>(diffX) * static_cast<long long>(diffX) +
                      static_cast<long long>(diffZ) * static_cast<long long>(diffZ);
    int distance = static_cast<int>(
        static_cast<Globals *>(Globals::globals)->sqrt(static_cast<float>(total) * 0.000244140625f));
    int meters = 8 * distance;
    String result = String(meters) + String("m");

    if (distance >= 125) {
        if (meters % 1000 >= 100) {
            result = String(meters % 1000);
        } else {
            result = String("0");
        }
        result = result.SubString(0, 1);

        result = String(static_cast<int>(static_cast<unsigned int>(distance) / 125u)) + String(".") + result +
                 String("km");
    }

    return result;
}

// Static data members present in the original binary (defined for symbol parity).
unsigned char Radar::drawTarget;
