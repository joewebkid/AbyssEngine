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
    int64_t field_0x20;
    int displayDuration;
    uint8_t lastMessageShownFlag;
    uint8_t soundPending;
    int soundId;
    String *font;
    int boxWidth;
    int boxX;
    int boxY;
    uint8_t _unknown_tail_0x44[4];

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

#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(Radio) == 0x48, "Android Radio allocation is 0x48 bytes");
static_assert(__builtin_offsetof(Radio, lastMessageShownFlag) == 0x2c,
              "Android Radio::lastMessageShownFlag @ 0x2c");
static_assert(__builtin_offsetof(Radio, boxY) == 0x40,
              "Android Radio::boxY @ 0x40");
#endif
#endif
