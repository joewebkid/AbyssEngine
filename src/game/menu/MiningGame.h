#ifndef GOF2_MININGGAME_H
#define GOF2_MININGGAME_H
#include <cstdint>
#include <cstddef>

#include "engine/render/MarqueeImage.h"
#include "engine/render/Sprite.h"
#include "game/ui/Hud.h"


#include "game/menu/MiningHostObject.h"
class Hud;
class MarqueeImage;
class Sprite;



static_assert(offsetof(MiningHostObject, miningActiveFlag) == 0x37,
              "MiningHostObject::miningActiveFlag must stay at +0x37");
static_assert(offsetof(MiningHostObject, miningResultSlot) == 0x124,
              "MiningHostObject::miningResultSlot must stay at +0x124");

class MiningGame {
public:
    float inputX;
    float inputY;
    float driftX;
    float driftY;
    float posX;
    float posY;
    int layer;
    int station;
    int lossTimer;
    float oreAmount;
    float oreRate;
    float controlDivisor;
    int field_0x30;
    int oreImageHeight;
    int field_0x38;
    int field_0x3c;
    int oreIconOffsetX;
    int oreIconOffsetY;
    int progressBarWidth;
    int progressBarHeight;
    int progressBarX;
    int progressBarY;
    int centerX;
    int centerY;
    int coreImageId;
    float textAlpha;
    float animAccumulator;
    int driftTimer;
    int failThreshold;
    int layerTimer;
    int currentLayer;
    int targetLayer;
    uint8_t isCoreLayer;
    uint8_t gameWonFlag;
    uint8_t gameLostFlag;
    uint8_t gotCoreFlag;
    uint8_t campaignFlag;
    uint8_t _pad_85[3];
    MarqueeImage *oreMarquee;
    MarqueeImage *leftMarquee;
    MarqueeImage *rightMarquee;
    Sprite *drillSprite;
    int oreIconImageId;
    int oreLabelImageId;
    int oreTextImageId;
    int cornerImageId;
    int progressBarImageId;
    int ringEvenNear;
    int ringEvenFar;
    int ringEvenMid;
    int ringOddNear;
    int ringOddMid;
    int ringOddFar;
    int progressLabelImageId;
    int marqueeWidth;
    int promptPulseTimer;
    Hud *hud;

    MiningGame(int layer, int station, Hud *hud);

    ~MiningGame();

    float up(float amount);

    float down(float amount);

    float left(float amount);

    float right(float amount);

    int getOreAmount();

    int getAsteroidType();

    uint8_t gameWon();

    uint8_t gameLost();

    uint8_t gotCore();

    bool isInCurrentLayer();

    int update(int delta);

    void render2D();
};
#endif
