#ifndef GOF2_RADIO_H
#define GOF2_RADIO_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/render/ImagePart.h"
#include "game/ship/PlayerEgo.h"
#include "game/world/LevelScript.h"

class ImagePart;
class LevelScript;
class PlayerEgo;
class RadioMessage;


class Radio {
public:
    Array<RadioMessage *> *messages;
    RadioMessage *currentMessage;
    Array<String *> *textLines;
    Array<ImagePart *> *imageParts;
    int *imagePartBuffer;
    int64_t startTime;
    int displayDuration;
    uint8_t lastMessageShownFlag;
    uint8_t soundPending;
    int soundId;
    String *font;
    int boxWidth;
    int boxX;
    int boxY;

    Radio();

    ~Radio();

    bool isShowingMessage();

    uint8_t lastMessageShown();

    RadioMessage *getMessage(int index);

    void setCurrentMessage(RadioMessage *message);

    void setMessages(Array<RadioMessage *> *messages);

    void update(long time, PlayerEgo *ego, LevelScript *script);

    void draw(int64_t time, PlayerEgo *ego, LevelScript *script);
};
#endif
