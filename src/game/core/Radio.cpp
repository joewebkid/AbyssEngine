#include "game/core/Radio.h"
#include "game/core/RadioMessage.h"
#include "game/world/Wanted.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/ImageFactory.h"
#include "engine/render/ImagePart.h"
#include "engine/audio/FModSound.h"
#include "engine/core/GameText.h"
#include "game/ui/Layout.h"
#include "game/ship/Agent.h"
#include "game/core/Globals.h"

void Globals_drawLines(void *globals, String *font, Array<String *> *lines,
                       int x, int y);

static Array<Wanted *> **g_Radio_wantedRoot;
static ImageFactory **g_Radio_imageFactoryCreate;
static ImageFactory **g_Radio_imageFactoryLoad;
static int * g_Radio_imagePartTable[0x3f];
static GameText **g_Radio_gameText;
static String **g_Radio_fontNormal;
static String **g_Radio_fontWide;
static Layout **g_Radio_layoutForText;
static void **g_Radio_globals;
static char g_Radio_agentName[1];
static Layout **g_Radio_layout;
static int **g_Radio_screenWidth;
static FModSound **g_Radio_drawSound;
static Layout **g_Radio_drawLayout;
static Array<Wanted *> **g_Radio_drawWantedRoot;
static GameText **g_Radio_drawGameText;
static ImageFactory **g_Radio_drawImageFactory;
static void **g_Radio_drawGlobals;

static String radio_string_from_cstr(const char *c) {
    String r;
    for (const char *p = c; p && *p; ++p)
        { int _nl = r.length + 1; unsigned short *_nd = new unsigned short[_nl + 1]; for (int _i = 0; _i < r.length; _i++) _nd[_i] = r.data[_i]; _nd[r.length] = (unsigned short) ((char16_t) (unsigned char) *p); _nd[_nl] = 0; if (r.data) delete[] r.data; r.data = _nd; r.length = _nl; }
    return r;
}

Radio::Radio() {
    this->startTime = 0;
    this->displayDuration = 0;
    this->lastMessageShownFlag = 0;
    this->imagePartBuffer = 0;
    this->soundId = -1;
    this->messages = 0;

    Layout *layout = *g_Radio_layout;
    int width = layout->field_0x98;
    int screenWidth = **g_Radio_screenWidth;
    this->boxWidth = width;
    this->boxX = (screenWidth - width) >> 1;
    this->boxY = layout->field_0x9c;
}

Radio::~Radio() {
    if (this->imageParts != 0) {
        ArrayReleaseClasses(*this->imageParts); ArrayRemoveAll(*(this->imageParts));
        delete this->imageParts;
    }
    this->imageParts = 0;

    delete[] this->imagePartBuffer;
    this->imagePartBuffer = 0;

    if (this->textLines != 0) {
        ArrayReleaseClasses(*this->textLines); ArrayRemoveAll(*(this->textLines));
        delete this->textLines;
    }
    this->textLines = 0;
}

bool Radio::isShowingMessage() {
    return this->currentMessage != 0;
}

uint8_t Radio::lastMessageShown() {
    return this->lastMessageShownFlag;
}

RadioMessage *Radio::getMessage(int index) {
    return (*this->messages)[index];
}

void Radio::setMessages(Array<RadioMessage *> *messages) {
    this->messages = messages;
    if (messages != 0) {
        for (uint32_t i = 0; i < messages->size(); ++i)
            (*messages)[i]->radio = this;
    }
}

void Radio::setCurrentMessage(RadioMessage *message) {
    this->currentMessage = message;
}

void Radio::update(long time, PlayerEgo *ego, LevelScript *script) {
    Array<RadioMessage *> *messages = this->messages;
    if (messages == 0)
        return;

    for (uint32_t i = 0; i < messages->size(); ++i) {
        RadioMessage *message = (*messages)[i];
        if (message->triggered((int64_t) time, ego, script) == 0) {
            messages = this->messages;
            continue;
        }

        int imageId = message->imageID;
        int *parts;
        int agentIndex;
        bool generated;

        if (imageId >= 10000) {
            delete[] this->imagePartBuffer;
            this->imagePartBuffer = new int[5];
            int wantedIndex = imageId - 10000;
            Wanted *wanted = (**g_Radio_wantedRoot)[wantedIndex];
            int *source = wanted->getImageParts();
            for (int j = 0; j != 5; ++j)
                this->imagePartBuffer[j] = source[j];
            parts = this->imagePartBuffer;
            agentIndex = 0;
            generated = true;
        } else if (imageId < 0x3f && imageId != 0x15) {
            delete[] this->imagePartBuffer;
            int *source = g_Radio_imagePartTable[imageId];
            this->imagePartBuffer = new int[5];
            for (int j = 0; j != 5; ++j)
                this->imagePartBuffer[j] = source[j];
            parts = this->imagePartBuffer;
            generated = parts[0] != 10;
            agentIndex = generated ? parts[0] : 0;
            if (imageId == 9)
                agentIndex = 8;
        } else {
            if (imageId == 0x40)
                agentIndex = 0;
            else if (imageId == 0x41)
                agentIndex = 2;
            else
                agentIndex = (imageId == 0x15) ? 3 : 1;
            delete[] this->imagePartBuffer;
            generated = true;
            parts = (*g_Radio_imageFactoryCreate)->createChar(1, agentIndex);
            this->imagePartBuffer = parts;
        }

        this->imageParts = (*g_Radio_imageFactoryLoad)->loadChar(parts);

        if (this->textLines != 0) {
            ArrayReleaseClasses(*this->textLines); ArrayRemoveAll(*(this->textLines));
            delete this->textLines;
        }
        this->textLines = new Array<String *>();

        int textId = message->textID;
        String text = *(*g_Radio_gameText)->getText(textId);

        String **fontHolder = g_Radio_fontWide;
        if (imageId != 0x38)
            fontHolder = g_Radio_fontNormal;
        if (imageId == 0x13)
            fontHolder = g_Radio_fontWide;
        this->font = *fontHolder;

        Layout *layout = *g_Radio_layoutForText;
        static_cast<Globals *>(*g_Radio_globals)->getLineArray(
            static_cast<unsigned int>(reinterpret_cast<std::size_t>(this->font)),
            text, (this->boxWidth - 10) - layout->field_0x2d4,
            this->textLines);

        this->startTime = (int64_t) time;
        this->soundPending = 1;
        this->displayDuration = (int) this->textLines->size() * 2000 + 1500;

        String agentName = radio_string_from_cstr(g_Radio_agentName);
        Agent *agent = new Agent(0, agentName, 0, 0, agentIndex, generated,
                                 0, 0, 0, 0);
        this->soundId = static_cast<Globals *>(*g_Radio_globals)
                ->getDialogueSoundId(message->textID, agent);
        delete agent;
        break;
    }
}

void Radio::draw(int64_t time, PlayerEgo *ego, LevelScript *script) {
    (void) ego;
    (void) script;

    if (this->currentMessage == 0)
        return;
    if (this->startTime + 2000 >= time)
        return;

    if (this->soundPending != 0 && this->soundId >= 0) {
        (*g_Radio_drawSound)->play(this->soundId, 0, 0, 0.0f);
        this->soundPending = 0;
    }

    PaintCanvas::gCanvas->SetColor(0xffffffffu);
    int imageId = this->currentMessage->imageID;
    Layout *layout = *g_Radio_drawLayout;
    layout->setDrawColor(-0xd1);

    int width = this->boxWidth;
    int x = this->boxX;
    int y = this->boxY;
    uint32_t imageHeight = layout->field_0x4 * (uint32_t) this->textLines->size();
    uint32_t minHeight = layout->field_0x2d8;
    if (minHeight > imageHeight)
        imageHeight = minHeight;
    int boxHeight = imageHeight + layout->field_0x8 + 10;

    if (imageId >= 10000) {
        Wanted *wanted = (**g_Radio_drawWantedRoot)[imageId - 10000];
        String title = wanted->getName();
        layout->drawBox(7, x, y, width, boxHeight, title, 0u);
    } else {
        String title = *(*g_Radio_drawGameText)->getText(imageId + 0x63d);
        layout->drawBox(7, x, y, width, boxHeight, title, 0u);
    }

    layout->setDrawColor(-1);
    (*g_Radio_drawImageFactory)->drawChar(this->imageParts,
                                          this->boxX + 5,
                                          layout->field_0x8 + this->boxY + 5,
                                          false);
    static_cast<Globals *>(*g_Radio_drawGlobals)->drawLines((unsigned int) (uintptr_t) this->font, this->textLines,
                                                            layout->field_0x2d4 + this->boxX + 7,
                                                            layout->field_0x8 + this->boxY + 7);

    if (this->soundPending != 0)
        this->soundPending = 0;

    Array<RadioMessage *> *messages = this->messages;
    if (messages != 0 && messages->size() != 0) {
        if (this->startTime + (int64_t) this->displayDuration + 2000 < time) {
            if (this->currentMessage == (*messages)[messages->size() - 1])
                this->lastMessageShownFlag = 1;
            this->startTime = 0;
            this->currentMessage->finish();
            this->currentMessage = 0;
        }
    }
}
