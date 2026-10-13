#include "game/menu/MiningGame.h"

#include "game/core/Globals.h"
#include "engine/core/AERandom.h"
#include "engine/core/GameText.h"
#include "engine/audio/FModSound.h"
#include "engine/render/PaintCanvas.h"
#include "game/core/String.h"
#include "game/ui/Hud.h"
#include "game/ui/Layout.h"
#include "game/mission/Status.h"
#include "game/mission/Achievements.h"
#include "game/ship/Ship.h"
#include "engine/render/MarqueeImage.h"
#include "engine/render/Sprite.h"

extern "C" {
int LAYER_DIAMETERS[49] = {
    250, 210, 170, 140, 110, 80, 50,
    250, 210, 170, 140, 110, 80, 0,
    250, 200, 150, 100, 70, 0, 0,
    250, 170, 120, 80, 0, 0, 0,
    250, 160, 80, 0, 0, 0, 0,
    250, 120, 0, 0, 0, 0, 0,
    250, 0, 0, 0, 0, 0, 0,
};
float LAYER_SPEEDS[7] = {5.0f, 8.0f, 12.0f, 17.0f, 23.0f, 30.0f, 38.0f};
}

int MiningGame::getOreAmount() {
    return (int) this->oreAmount;
}

int MiningGame::getAsteroidType() {
    return this->station;
}


bool MiningGame::isInCurrentLayer() {
    float dx = this->posX - (float) this->centerX;
    float dy = this->posY - (float) this->centerY;
    int size = LAYER_DIAMETERS[7 * (7 - this->targetLayer) + this->currentLayer];
    float scale = Globals::layout->ringRadiusScale;
    float distance = static_cast<Globals *>(Globals::globals)->sqrt(dx * dx + dy * dy);
    return distance < scale * (float) (size / 2);
}

float MiningGame::up(float amount) {
    return this->inputY = amount * 3.0f;
}

uint8_t MiningGame::gameWon() {
    return this->gameWonFlag;
}

float MiningGame::down(float amount) {
    return this->inputY = amount * 3.0f;
}

uint8_t MiningGame::gotCore() {
    return this->gotCoreFlag;
}

float MiningGame::left(float amount) {
    return this->inputX = amount * 3.0f;
}

uint8_t MiningGame::gameLost() {
    return this->gameLostFlag;
}

float MiningGame::right(float amount) {
    return this->inputX = amount * 3.0f;
}

MiningGame::~MiningGame() {
    delete this->drillSprite;
    this->drillSprite = nullptr;
}


int MiningGame::update(int delta) {
    int pulse = this->promptPulseTimer + delta;
    if (pulse >= 2000) {
        pulse = 0;
    }
    this->promptPulseTimer = pulse;

    int wasInLayer = isInCurrentLayer();
    int driftTimer = this->driftTimer + delta;
    this->driftTimer = driftTimer;
    if (driftTimer >= 2501) {
        this->driftTimer = static_cast<AERandom *>(Globals::rnd)->nextInt(2000) + 500;

        int value = static_cast<AERandom *>(Globals::rnd)->nextInt(7);
        int sign = -1;
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0) {
            sign = 1;
        }
        this->driftX = ((float) ((value + 5) * sign) / 10.0f) / this->controlDivisor;

        value = static_cast<AERandom *>(Globals::rnd)->nextInt(7);
        sign = -1;
        if (static_cast<AERandom *>(Globals::rnd)->nextInt(2) == 0) {
            sign = 1;
        }
        this->driftY = ((float) (sign * (value + 5)) / 10.0f) / this->controlDivisor;

        if (this->isCoreLayer != 0 && this->currentLayer == this->targetLayer - 1) {
            this->driftX *= 0.3f;
            this->driftY *= 0.3f;
        }
    }

    if (this->campaignFlag == 0 && !isInCurrentLayer()) {
        this->driftX = ((float) this->centerX - this->posX) * 0.03f;
        this->driftY = ((float) this->centerY - this->posY) * 0.03f;
    }

    float driftDivisor = Globals::layout->driftSpeedDivisor;
    float frameScale = (float) delta;
    this->posX += ((this->inputX + this->driftX) / driftDivisor) * frameScale;
    this->posY += ((this->inputY + this->driftY) / driftDivisor) * frameScale;

    this->leftMarquee->update(delta);
    this->rightMarquee->update(delta);

    float *layerSpeed = LAYER_SPEEDS;
    Globals::sound->setParamValue(0, 1,
                                  ((layerSpeed[this->currentLayer] - 5.0f) / 33.0f) * 3.0f);

    if (!isInCurrentLayer()) {
        if (wasInLayer != 0) {
            Globals::sound->stop(1);
            Globals::sound->play(3, nullptr, nullptr, 0.0f);
        }
        int lossTimer = this->lossTimer + delta;
        this->lossTimer = lossTimer;
        if (lossTimer >= 2501) {
            this->lossTimer = 0x9c4;
            this->oreAmount = 0.0f;
            this->gameLostFlag = 1;
            Globals::status->field_124 = 0;
            return 0;
        }
        this->textAlpha = 1.0f;
        return 1;
    }

    if (wasInLayer == 0) {
        Globals::sound->play(1, nullptr, nullptr, 0.0f);
        Globals::sound->stop(3);
    }

    float anim = this->animAccumulator +
                 (frameScale / 1000.0f) * Globals::layout->animSpeedScale * layerSpeed[this->currentLayer] * 3.0f;
    this->animAccumulator = anim;
    if (anim >= 1.0f) {
        this->drillSprite->nextFrame();
        this->animAccumulator = 0.0f;
    }

    this->oreMarquee->update(delta);
    int nextLayer = this->currentLayer + 1;
    float oldOre = this->oreAmount;
    float layerFactor = 0.15f + ((float) nextLayer / 7.0f) * 2.35f;
    float newOre = oldOre + ((this->oreRate * layerFactor) / 1000.0f) * frameScale;
    this->oreAmount = newOre;
    float alpha = (int) oldOre < (int) newOre ? 0.0f : this->textAlpha;
    int layerTimer = this->layerTimer + delta;
    this->layerTimer = layerTimer;
    alpha += frameScale / 500.0f;
    if (alpha > 1.0f) {
        alpha = 1.0f;
    }
    this->textAlpha = alpha;

    if (layerTimer > 6000) {
        this->layerTimer = 0;
        this->currentLayer = nextLayer;
        if (nextLayer >= this->targetLayer) {
            this->gameWonFlag = 1;
            this->gotCoreFlag = this->targetLayer == 7;
            Achievements *achievements = static_cast<Achievements *>(Globals::achievements);
            if (achievements->hasMedal(0x26, 1) == 0) {
                Status *status = Globals::status;
                int count = status->field_124 + 1;
                status->field_124 = count;
                int goal = achievements->getValue(0x26, 1);
                int percent = (int) (((float) count / (float) goal) * 100.0f);
                if (percent % 10 == 0) {
                    int shown = (int) (((float) status->field_124 /
                                        (float) achievements->getValue(0x26, 1)) *
                                       100.0f);
                    if (shown > 29) {
                        this->hud->hudEventMedal(0x26, shown);
                    }
                }
                if (achievements->getValue(0x26, 1) <= status->field_124) {
                    *reinterpret_cast<uint8_t *>(&status->field_128) = 1;
                }
            }
            return 0;
        }
        this->oreMarquee->setSpeed(Globals::layout->animSpeedScale * layerSpeed[nextLayer]);
    }

    return 1;
}


MiningGame::MiningGame(int layer, int station, Hud *hud) {
    this->layer = layer;
    this->station = station;
    this->hud = hud;

    int centerX = Globals::w >> 1;
    this->centerX = centerX;
    int centerY = Globals::layout->centerYBase + (Globals::h >> 1);
    this->centerY = centerY;
    this->marqueeWidth = Globals::layout->marqueeWidth;
    this->isCoreLayer = layer == 7;
    this->lossTimer = 0;
    this->oreAmount = 0.0f;
    this->oreRate = 0.0f;
    this->driftTimer = 0;
    this->failThreshold = 0x9c4;
    this->layerTimer = 0;
    this->currentLayer = 0;
    this->targetLayer = layer;
    this->gameWonFlag = 0;
    this->gameLostFlag = 0;
    this->gotCoreFlag = 0;
    this->posX = (float) centerX;
    this->inputX = 0.0f;
    this->inputY = 0.0f;
    this->driftX = 0.0f;
    this->driftY = 0.0f;
    this->posY = (float) centerY;

    Ship *ship = Globals::status->getShip();
    Item *equipment = ship->getFirstEquipmentOfSort(0x13);
    if (equipment != 0) {
        int value = equipment->getAttribute(0x20);
        this->controlDivisor = 0.3f + ((float) value / 100.0f) * 1.5f;
        value = equipment->getAttribute(0x21);
        this->oreRate = (float) value / 100.0f;
    }

    unsigned int drillImageId = static_cast<unsigned int>(-1);
    Globals::Canvas->Image2DCreate(0x4e6, drillImageId);
    int imageHeight = Globals::Canvas->GetImage2DHeight(drillImageId);
    Sprite *sprite = new Sprite(drillImageId, imageHeight, imageHeight);
    this->drillSprite = sprite;
    sprite->defineReferencePixel(imageHeight >> 1, imageHeight >> 1);

    this->animAccumulator = 0.0f;
    Globals::Canvas->Image2DCreate(0x4e2, this->ringEvenNear);
    Globals::Canvas->Image2DCreate(0x4dd, this->ringEvenFar);
    Globals::Canvas->Image2DCreate(0x4de, this->ringEvenMid);
    Globals::Canvas->Image2DCreate(0x4e1, this->ringOddNear);
    Globals::Canvas->Image2DCreate(0x4df, this->ringOddMid);
    Globals::Canvas->Image2DCreate(0x4e0, this->ringOddFar);
    Globals::Canvas->Image2DCreate(0x4e5, this->oreLabelImageId);
    Globals::Canvas->Image2DCreate(0x4e4, this->oreTextImageId);
    Globals::Canvas->Image2DCreate(0x4e7, this->oreIconImageId);
    Globals::Canvas->Image2DCreate(0x4e3, this->cornerImageId);
    Globals::Canvas->Image2DCreate(0x4e8, this->progressBarImageId);
    Globals::Canvas->Image2DCreate(0x4ed, this->progressLabelImageId);

    if (this->isCoreLayer != 0) {
        int coreImage = 0x523;
        if (station == 0xa4) {
            coreImage = 0x522;
        }
        Globals::Canvas->Image2DCreate(coreImage, this->coreImageId);
    }

    this->progressBarWidth = Globals::Canvas->GetImage2DWidth(this->progressBarImageId);
    this->progressBarHeight = Globals::Canvas->GetImage2DHeight(this->progressBarImageId);
    int x = Globals::w / 2 - this->progressBarWidth / 2;
    this->progressBarX = x;
    int y = Globals::layout->progressBarYBase;
    this->progressBarY = y;

    this->leftMarquee = new MarqueeImage(0x4eb, this->marqueeWidth, x, this->progressBarHeight + y + 5, 20.0f);

    this->rightMarquee = new MarqueeImage(0x4ec, this->marqueeWidth,
                                          (this->progressBarX - this->marqueeWidth) + this->progressBarWidth,
                                          this->progressBarHeight + this->progressBarY + 5, 32.0f);

    this->oreIconOffsetX = Globals::Canvas->GetImage2DWidth(this->oreIconImageId) / 2 + 5;
    this->oreIconOffsetY = Globals::Canvas->GetImage2DWidth(this->oreLabelImageId) >> 1;
    this->oreImageHeight = Globals::Canvas->GetImage2DHeight(this->oreTextImageId);

    MarqueeImage *oreMarquee =
            new MarqueeImage(0x4e4, Globals::Canvas->GetImage2DWidth(this->oreLabelImageId) - 8, 0, 0,
                             Globals::layout->field_0xdc);
    this->oreMarquee = oreMarquee;
    oreMarquee->setSpeed(Globals::layout->animSpeedScale * LAYER_SPEEDS[this->currentLayer]);

    this->promptPulseTimer = 0;
    this->textAlpha = 1.0f;
    this->campaignFlag = Globals::status->getCurrentCampaignMission() > 4;
}


void MiningGame::render2D() {
    Globals::Canvas->SetColor((unsigned int) -1);

    for (int layerIndex = this->currentLayer; layerIndex < this->targetLayer; layerIndex++) {
        int raw = LAYER_DIAMETERS[(layerIndex - this->targetLayer * 7) + 0x31];
        int radius = (int) (Globals::layout->ringRadiusScale * (float) raw);
        uint32_t *imageSlot;
        if ((layerIndex & 1) == 0) {
            imageSlot = &this->ringEvenNear;
            if (Globals::layout->ringNearMidThreshold < radius) {
                imageSlot = &this->ringEvenMid;
                if (radius < Globals::layout->ringMidFarThreshold) {
                    imageSlot = &this->ringEvenFar;
                }
            }
        } else {
            imageSlot = &this->ringOddNear;
            if (Globals::layout->ringNearMidThreshold < radius) {
                imageSlot = &this->ringOddFar;
                if (radius < Globals::layout->ringMidFarThreshold) {
                    imageSlot = &this->ringOddMid;
                }
            }
        }
        uint32_t image = *imageSlot;
        int half = radius / 2;
        Globals::Canvas->DrawImage2D(image, this->centerX, this->centerY, half, half, 0x11, 0x22, 0);
        Globals::Canvas->DrawImage2D(image, this->centerX, this->centerY, half, half, 0x11, 0x21, 1);
        Globals::Canvas->DrawImage2D(image, this->centerX, this->centerY, half, half, 0x11, 0x12, 2);
        Globals::Canvas->DrawImage2D(image, this->centerX, this->centerY, half, half, 0x11, 0x11, 3);
    }

    if (this->isCoreLayer != 0) {
        Globals::Canvas->DrawImage2D(this->coreImageId, this->centerX, this->centerY, 0x11, 0x44);
    }

    Globals::Canvas->DrawImage2D(this->oreIconImageId, (int) this->posX, (int) this->posY, 0x11, 0x44);
    this->drillSprite->setRefPixelPosition((int) this->posX, (int) this->posY);
    this->drillSprite->draw(1.0f, 1.0f);

    Globals::Canvas->DrawImage2D(this->cornerImageId, this->progressBarX - Globals::layout->hudInset,
                                 this->progressBarY - Globals::layout->hudInset);

    int lossTimer = this->lossTimer;
    if (lossTimer > 0x341) {
        uint32_t red = (uint32_t) (Globals::layout->getPulseValue(10.0f) * 255.0f);
        uint32_t green = (uint32_t) (Globals::layout->getPulseValue(10.0f) * 255.0f);
        Globals::Canvas->SetColor(0xff, red, green, 0xff);
    }

    int width = (int) (((2500.0f - (float) lossTimer) / 2500.0f) * (float) this->progressBarWidth);
    Globals::Canvas->DrawRegion2D(this->progressBarImageId, 0, 0, width, this->progressBarHeight, 0.0f, 0, 0,
                                  this->progressBarX, this->progressBarY);
    Globals::Canvas->SetColor((unsigned int) -1);
    Globals::Canvas->DrawImage2D(this->progressLabelImageId, this->centerX, this->progressBarY - 3, 0x11, 0x24);

    this->leftMarquee->draw();
    this->rightMarquee->draw();
    this->oreMarquee->draw((int) (this->posX + (float) this->oreIconOffsetX),
                           (int) (this->posY - (float) this->oreImageHeight));

    Globals::Canvas->DrawImage2D(this->oreLabelImageId,
                                 (int) ((this->posX + (float) this->oreIconOffsetX) - (float) Globals::layout->hudInset),
                                 (int) (this->posY - (float) Globals::layout->oreLabelYOffset));

    String oreText = String((int) this->oreAmount) + String("t", false);

    int freeSpace = Globals::status->getShip()->getFreeSpace();
    uint32_t alpha = (uint32_t) (this->textAlpha * 255.0f);
    if (freeSpace < (int) this->oreAmount) {
        Globals::Canvas->SetColor(0xff, 0x2a, 0, alpha);
    } else {
        Globals::Canvas->SetColor(0xff, 0xff, 0xff, alpha);
    }

    int textWidth = Globals::Canvas->GetTextWidth(Globals::font, oreText);
    int textX = (int) (((this->posX + (float) this->oreIconOffsetX + (float) this->oreIconOffsetY) -
                        (float) Globals::layout->hudInset) -
                       (float) (textWidth >> 1));
    int textY = (int) (this->posY + (float) Globals::layout->oreTextYOffset);
    Globals::Canvas->DrawString(Globals::font, oreText, textX, textY, false);
    Globals::Canvas->SetColor((unsigned int) -1);

    if (Globals::status->getCurrentCampaignMission() < 5) {
        int promptAlpha = (int) (((float) this->promptPulseTimer / 1000.0f) * 255.0f);
        if (promptAlpha > 255) {
            promptAlpha = 255 - promptAlpha;
        }
        Globals::Canvas->SetColor(0xff, 0xff, 0xff, (uint8_t) promptAlpha);
        String prompt(*Globals::gameText->getText(0x268), false);
        int promptWidth = Globals::Canvas->GetTextWidth(Globals::font, prompt);
        Globals::Canvas->DrawString(Globals::font, prompt,
                                    Globals::w / 2 - promptWidth / 2,
                                    Globals::layout->promptYOffset + this->progressBarY, false);
        Globals::Canvas->SetColor((unsigned int) -1);
    }
}
