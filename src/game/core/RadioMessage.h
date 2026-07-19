#ifndef GOF2_RADIOMESSAGE_H
#define GOF2_RADIOMESSAGE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/mission/Objective.h"

class LevelScript;
class Objective;
class PlayerEgo;
class Radio;


class RadioMessage {
public:
    Radio *radio;
    Objective *objective;
    int textID;
    int imageID;
    int conditionType;
    int conditionValue;
    int targetCount;
    int *targetIndices;
    uint8_t triggeredFlag;
    uint8_t over;
    int lastRouteIndex;

    RadioMessage(int textID, int imageID, int conditionType, int conditionValue, int targetCount);

    RadioMessage(int textID, int imageID, int conditionType, int conditionValue);

    RadioMessage(int textID, int imageID, Objective *objective);

    ~RadioMessage();

    void setRadio(Radio *radio);

    int getTextID();

    int getImageID();

    int getSoundID();

    void finish();

    uint8_t isOver();

    void reset();

    void trigger();

    uint8_t isTriggered();

    int triggered(int64_t time, PlayerEgo *ego, LevelScript *script);
};
#endif
