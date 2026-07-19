#include "game/ui/DialogueWindow.h"
#include "game/ui/ChoiceWindow.h"
#include "engine/audio/FModSound.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/mission/Status.h"
#include "game/ship/Agent.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "engine/render/ImagePart.h"
#include "game/ui/Layout.h"
#include "engine/render/PaintCanvas.h"
#include "engine/core/AERandom.h"
#include "game/mission/Mission.h"
#include "game/world/Standing.h"
#include "game/world/Level.h"
#include "game/core/String.h"
#include "game/core/Globals.h"
#include "game/ui/TouchButton.h"


struct EngineSoundConfig {
    char field_0x0[0xe];
    uint8_t autoAdvanceEnabled;
};

int DialogueWindow::OnTouchBegin(int x, int y) {
    if (this->choiceActive != 0) {
        this->choiceWindow->OnTouchBegin(x, y);
    } else {
        this->scrollWindow->OnTouchBegin(x, y);
        this->prevButton->OnTouchBegin(x, y);
        this->nextButton->OnTouchBegin(x, y);
        this->moreButton->OnTouchBegin(x, y);
    }
    return 0;
}


static int g_dw_briefingCounts[0xa2];

static int g_dw_successCounts[0xa2];

int DialogueWindow::getMode() {
    return this->kind;
}

int DialogueWindow::length() {
    Mission *mission = this->mission;
    if (mission != 0 && mission->isCampaignMission() != 0) {
        int *counts;
        if (this->kind == 1) {
            counts = g_dw_successCounts;
        } else {
            if (this->kind != 0) return 1;
            counts = g_dw_briefingCounts;
        }
        return counts[this->campaignMission] / 2;
    }
    if (this->kind == 0 && this->mission != 0 &&
        this->mission->getTargetStation() == 0x6c) {
        int result = 6;
        if (Status::gStatus->field_114 == 2) result = 0x12;
        return result;
    }
    return 1;
}

int DialogueWindow::nextPage() {
    int len = this->length();
    if (this->page < len - 1) {
        this->page = this->page + 1;
        this->loadContent();
        return 1;
    }
    return 0;
}

bool DialogueWindow::hasLevel() {
    return this->level != 0;
}

DialogueWindow::~DialogueWindow() {
    if (this->faceParts != 0) {
        ArrayReleaseClasses(*this->faceParts); ArrayRemoveAll(*(this->faceParts));
        delete this->faceParts;
        this->faceParts = 0;
    }

    delete[] this->briefingOffsets;
    this->briefingOffsets = 0;
    delete[] this->successOffsets;
    this->successOffsets = 0;

    delete this->scrollWindow;
    this->scrollWindow = 0;

    delete this->prevButton;
    this->prevButton = 0;
    delete this->nextButton;
    this->nextButton = 0;
    delete this->moreButton;
    this->moreButton = 0;

    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
}

int DialogueWindow::OnTouchMove(int x, int y) {
    if (this->choiceActive != 0) {
        this->choiceWindow->OnTouchMove(x, y);
    } else {
        this->scrollWindow->OnTouchMove(x, y);
        this->prevButton->OnTouchMove(x, y);
        this->nextButton->OnTouchMove(x, y);
        this->moreButton->OnTouchMove(x, y);
    }
    return 0;
}

bool DialogueWindow::isFirstPage() {
    return this->page == 0;
}

int DialogueWindow::previousPage() {
    if (this->page <= 0) return 0;
    this->page = this->page - 1;
    this->loadContent();
    return 1;
}

bool DialogueWindow::isLastPage() {
    int len = this->length();
    return this->page == len - 1;
}


static int g_dw_briefingDialogueIds[0xa2];

bool DialogueWindow::hasBriefingDialogue(int id) {
    if (id > 0xa1) return false;
    return g_dw_briefingDialogueIds[id] > 0;
}


static int g_dw_successDialogueIds[0xa2];

bool DialogueWindow::hasSuccessDialogue(int id) {
    if (id > 0xa1) return false;
    return g_dw_successDialogueIds[id] > 0;
}

void DialogueWindow::set(Mission *mission, int kind, int campaign) {
    this->mission = mission;
    this->kind = kind;
    if (kind == 1) {
        mission->getAgent();
        mission->setWon(true);
    } else if (kind == 2) {
        Agent *agent = mission->getAgent();
        if (agent != 0 && agent->isGenericAgent() == 0) {
            agent->setOfferAccepted(false);
        }
        mission->setFailed(true);
    }

    this->page = 0;
    if (campaign == -1) {
        campaign = Status::gStatus->getCurrentCampaignMission();
    }
    this->campaignMission = campaign;
    this->loadContent();
}


static GameText *g_dw_gameTextLoad_storage = nullptr;
static GameText **g_dw_gameTextLoad = &g_dw_gameTextLoad_storage;

static FModSound *g_dw_soundLoad_storage = nullptr;
static FModSound **g_dw_soundLoad = &g_dw_soundLoad_storage;

static ImageFactory *g_dw_imageFactoryLoad_storage = nullptr;
static ImageFactory **g_dw_imageFactoryLoad = &g_dw_imageFactoryLoad_storage;

static const char g_dw_emptyLoad[] = "";

static int g_dw_campaignBriefingTextIds[0x1000];

static int g_dw_campaignSuccessTextIds[0x1000];

static void *g_dw_defaultClientImage = nullptr;

void DialogueWindow::loadContent() {
    GameText *gameText = *g_dw_gameTextLoad;
    FModSound *sound = *g_dw_soundLoad;

    this->nextButton->replaceTextKeepSize(*gameText->getText(0xb4));
    this->mirrorFace = 0;
    this->autoAdvanceTimer = 0;
    this->pauseLength = 0;
    sound->stop(this->voiceSound);
    this->voiceSound = -1;
    this->nextButton->setPressProgress(0);

    if (this->faceParts != 0) {
        ArrayReleaseClasses(*this->faceParts); ArrayRemoveAll(*(this->faceParts));
        delete this->faceParts;
        this->faceParts = 0;
    }

    Mission *mission = this->mission;
    int kind = this->kind;
    int page = this->page;
    int textId = -1;

    if (mission != 0 && mission->isCampaignMission() != 0) {
        int base;
        if (kind == 1) {
            base = this->successOffsets[this->campaignMission];
            textId = g_dw_campaignSuccessTextIds[base + page * 2 + 1];
        } else if (kind == 0) {
            base = this->briefingOffsets[this->campaignMission];
            textId = g_dw_campaignBriefingTextIds[base + page * 2 + 1];
        } else {
            textId = 0x10 + 0x63d;
        }
        this->clientImage = g_dw_defaultClientImage;
        this->agentName = *(gameText->getText(0x63d + (textId & 0xff)));
        this->bodyText = *(gameText->getText(textId));
    } else if (mission != 0) {
        if ((page & 1) != 0) {
            this->clientImage = g_dw_defaultClientImage;
            this->agentName = *(gameText->getText(0x63d));
            this->mirrorFace = 1;
        } else {
            this->clientImage = (void *) (intptr_t) mission->getClientImage();
            this->agentName = mission->getClientName();
            this->mirrorFace = 0;
        }

        if (kind == 1 || kind == 0 || kind == 2) {
            Agent *agent = mission->getAgent();
            if (GameText::getLanguage() == 1 && agent != 0) {
                textId = this->pickGermanGenericTextBecauseWeSaved100EurosWithThat(kind, agent);
            } else {
                textId = 0x188 + AERandom::gRandom->nextInt(5);
            }
        } else {
            textId = 0x20f;
        }
        this->bodyText = *(gameText->getText(textId));

        if (kind == 1) {
            int standing = Status::gStatus->getStanding();
            ((Standing *) (intptr_t) standing)->applyMissionCompleted(mission->getClientRace());
        }
        if (mission->getTargetStation() == 0x6c && kind == 0) {
            textId = 0x1ca;
            this->bodyText = *(gameText->getText(textId));
        }
        if (mission->getType() == 0x0c && kind == 0) {
            textId = 0x174;
            this->bodyText = *(gameText->getText(textId));
        }
    } else {
        this->clientImage = g_dw_defaultClientImage;
        textId = 0x10;
        this->bodyText = *(gameText->getText(textId));
        this->agentName = *(gameText->getText(0x63d));
    }

    String style(g_dw_emptyLoad, false);
    String body;
    body.Set((this->bodyText).data);
    this->scrollWindow->setText(style, body, 0);

    this->prevButton->setVisible(this->page != 0);
    this->moreButton->setVisible(this->length() > 1);
    this->faceParts = (*g_dw_imageFactoryLoad)->loadChar((int *) this->clientImage);

    if (this->isLastPage() != 0) {
        this->nextButton->replaceTextKeepSize(*gameText->getText(0xb5));
    }

    Agent *agent = mission == 0 ? (Agent *) 0 : mission->getAgent();
    int soundId = Globals::gGlobals->getDialogueSoundId(textId, agent);
    this->voiceSound = soundId;
    if (soundId >= 0) {
        sound->play(soundId, 0, 0, 0);
        this->pauseLength = sound->getEventPauseLength(soundId);
    }
}

DialogueWindow::DialogueWindow() {
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    this->init();
}


static EngineSoundConfig g_dw_soundConfig_storage;
static EngineSoundConfig *g_dw_soundConfig = &g_dw_soundConfig_storage;

static FModSound *g_dw_fmodSound_storage = nullptr;
static FModSound **g_dw_fmodSound = &g_dw_fmodSound_storage;

void DialogueWindow::update(int dt) {
    if (this->scrollWindow != 0) {
        this->scrollWindow->update(dt);
    }
    if (this->choiceActive != 0) {
        this->choiceWindow->update(dt);
    }
    if (g_dw_soundConfig->autoAdvanceEnabled != 0 && this->voiceSound != -1) {
        FModSound *sound = *g_dw_fmodSound;
        sound->getPlayingProgress(this->voiceSound);
        if (sound->isPlaying(this->voiceSound) == 0 && this->isLastPage() == 0) {
            if (this->autoAdvanceTimer >= this->pauseLength) {
                this->nextPage();
            }
            this->autoAdvanceTimer = this->autoAdvanceTimer + dt;
        }
    }
}


static FModSound *g_dw_soundChoice_storage = nullptr;
static FModSound **g_dw_soundChoice = &g_dw_soundChoice_storage;

static FModSound *g_dw_soundVoice_storage = nullptr;
static FModSound **g_dw_soundVoice = &g_dw_soundVoice_storage;

static FModSound *g_dw_soundPrev_storage = nullptr;
static FModSound **g_dw_soundPrev = &g_dw_soundPrev_storage;

static FModSound *g_dw_soundNext_storage = nullptr;
static FModSound **g_dw_soundNext = &g_dw_soundNext_storage;

static GameText *g_dw_gameTextTouchEnd_storage = nullptr;
static GameText **g_dw_gameTextTouchEnd = &g_dw_gameTextTouchEnd_storage;

int DialogueWindow::OnTouchEnd(int x, int y) {
    if (this->choiceActive != 0) {
        int r = this->choiceWindow->OnTouchEnd(x, y);
        if (r == 1) {
            this->choiceActive = 0;
            return 0;
        }
        if (r != 0) return 0;
        this->choiceActive = 0;
        if (Status::gStatus->getCurrentCampaignMission() == 0x0f) {
            FModSound *sound = *g_dw_soundChoice;
            sound->play(0xa2, 0, 0, 0);
            sound->stop(sound->currentMusicEvent);
            sound->play(0x88, 0, 0, 0);
        }
        if (this->voiceSound != -1) {
            (*g_dw_soundVoice)->stop(this->voiceSound);
        }
        return 1;
    }

    this->scrollWindow->OnTouchEnd(x, y);
    if (this->prevButton->OnTouchEnd(x, y) != 0) {
        (*g_dw_soundPrev)->stop(this->voiceSound);
        this->previousPage();
    }
    if (this->nextButton->OnTouchEnd(x, y) != 0) {
        (*g_dw_soundNext)->stop(this->voiceSound);
        if (this->nextPage() == 0) {
            return 1;
        }
    }
    if (this->moreButton->OnTouchEnd(x, y) != 0) {
        String *text = (*g_dw_gameTextTouchEnd)->getText(0x18c);
        this->choiceWindow->set(*text, true);
        this->choiceActive = 1;
    }
    return 0;
}


static int g_dw_briefingPartCounts[0xa2];

static int g_dw_successPartCounts[0xa2];

static const char g_dw_defaultAgentName[] = "";

static int g_dw_screenWidth_storage = 0;
static int *g_dw_screenWidth = &g_dw_screenWidth_storage;

static void *g_dw_layoutInit_storage = nullptr;
static void **g_dw_layoutInit = &g_dw_layoutInit_storage;

static int g_dw_screenHeight_storage = 0;
static int *g_dw_screenHeight = &g_dw_screenHeight_storage;

static GameText *g_dw_gameTextInit_storage = nullptr;
static GameText **g_dw_gameTextInit = &g_dw_gameTextInit_storage;

static inline int half_round_to_zero(int v) {
    return (v + ((unsigned) v >> 31)) >> 1;
}

int DialogueWindow::init() {
    this->briefingOffsets = new int[0xa2];
    this->successOffsets = new int[0xa2];

    int briefingSum = 0;
    int successSum = 0;
    for (int i = 0; i != 0xa2; ++i) {
        int briefingCount = g_dw_briefingPartCounts[i];
        int successCount = g_dw_successPartCounts[i];
        this->briefingOffsets[i] = briefingSum;
        this->successOffsets[i] = successSum;
        successSum += briefingCount;
        briefingSum += successCount;
    }

    this->agentName = String(g_dw_defaultAgentName, false);

    this->mission = 0;
    this->choiceWindow = 0;
    this->level = 0;
    this->clientImage = 0;
    this->faceParts = 0;
    this->voiceSound = -1;
    this->autoAdvanceTimer = 0;
    this->mirrorFace = 0;

    Layout *layout = (Layout *) *g_dw_layoutInit;
    int frameW = layout->field_0x54;
    int frameH = layout->field_0x58;
    this->frameWidth = frameW;
    this->frameHeight = frameH;
    this->frameX = half_round_to_zero(*g_dw_screenWidth) - half_round_to_zero(frameW);
    this->frameY = half_round_to_zero(*g_dw_screenHeight) - half_round_to_zero(frameH);

    int margin = layout->field_0x4c;
    this->scrollWindow = new ScrollTouchWindow(
        this->frameX + margin * 2 + layout->field_0x2d4,
        layout->field_0x8 + this->frameY,
        frameW - margin * 2 - layout->field_0x2d4,
        frameH - margin * 2 - layout->field_0x8 - layout->field_0x30,
        false);

    this->choiceWindow = new ChoiceWindow();

    GameText *gameText = *g_dw_gameTextInit;
    layout = (Layout *) *g_dw_layoutInit;
    margin = layout->field_0x4c;
    this->prevButton = new TouchButton(*gameText->getText(0xb3), 5,
                                       this->frameX + margin, this->frameY - margin + this->frameHeight,
                                       layout->field_0x50, 0x21, 4);

    layout = (Layout *) *g_dw_layoutInit;
    margin = layout->field_0x4c;
    this->nextButton = new TouchButton(*gameText->getText(0xb4), 6,
                                       this->frameX + this->frameWidth - margin,
                                       this->frameY - margin + this->frameHeight,
                                       layout->field_0x50, 0x22, 4);

    layout = (Layout *) *g_dw_layoutInit;
    margin = layout->field_0x4c;
    this->moreButton = new TouchButton(*gameText->getText(0x18b), 0,
                                       this->frameX + half_round_to_zero(this->frameWidth),
                                       this->frameY + this->frameHeight - margin,
                                       this->frameWidth - margin * 4 - layout->field_0x50 * 2, 0x24, 4);
    this->choiceActive = 0;
    return 0;
}


static const char g_dw_emptyString[] = "";

static ImageFactory *g_dw_imageFactoryCtor_storage = nullptr;
static ImageFactory **g_dw_imageFactoryCtor = &g_dw_imageFactoryCtor_storage;

static GameText *g_dw_gameTextCtor_storage = nullptr;
static GameText **g_dw_gameTextCtor = &g_dw_gameTextCtor_storage;

static void *g_dw_layoutCtor_storage = nullptr;
static void **g_dw_layoutCtor = &g_dw_layoutCtor_storage;

DialogueWindow::DialogueWindow(String *text, String *agentName, int *parts) {
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    this->init();

    String blank(g_dw_emptyString, false);
    String copy;
    copy.Set((text)->data);
    this->scrollWindow->setText(blank, copy);

    this->moreButton->setVisible(false);
    this->prevButton->setVisible(false);

    this->faceParts = (*g_dw_imageFactoryCtor)->loadChar(parts);
    delete this->nextButton;
    this->nextButton = 0;

    GameText *gameText = *g_dw_gameTextCtor;
    Layout *layout = (Layout *) *g_dw_layoutCtor;
    int margin = layout->field_0x4c;
    int x = this->frameX + this->frameWidth / 2;
    int y = this->frameY + this->frameHeight - margin;
    int width = this->frameWidth - margin * 4 - layout->field_0x50 * 2;
    this->nextButton = new TouchButton(*gameText->getText(0x20c), 0, x, y, width, 0x24, 4);

    this->agentName = *(agentName);
    this->voiceSound = -1;
    this->page = 0;
    this->pauseLength = 0;
}

DialogueWindow::DialogueWindow(Mission *mission, Level *level, int kind) {
    { if (this->bodyText.data) delete[] this->bodyText.data; this->bodyText.data = nullptr; this->bodyText.length = 0; }
    { if (this->agentName.data) delete[] this->agentName.data; this->agentName.data = nullptr; this->agentName.length = 0; }
    this->init();
    this->level = level;
    this->set(mission, kind, -1);
}


static int g_dw_germanBriefingTexts[64];

static int g_dw_germanSuccessTexts[64];

static int g_dw_germanOtherTexts[64];

int DialogueWindow::pickGermanGenericTextBecauseWeSaved100EurosWithThat(int kind, Agent *agent) {
    int race = agent->getRace();
    int male;
    if (race < 10) {
        race = agent->getRace();
        male = agent->isMale();
        if (race == 3) {
            if (agent->getImageParts() == 0) {
                race = 3;
            } else {
                int *parts = agent->getImageParts();
                race = 0;
                if (*parts == 2) race = 3;
            }
        }
    } else {
        male = agent->isMale();
        race = 10;
    }

    int index;
    DialogueWindowGermanTextTable *texts;
    if (kind == 2) {
        index = AERandom::gRandom->nextInt(2);
        texts = (DialogueWindowGermanTextTable *) g_dw_germanSuccessTexts;
    } else if (kind != 0) {
        index = AERandom::gRandom->nextInt(2);
        texts = (DialogueWindowGermanTextTable *) g_dw_germanOtherTexts;
    } else {
        index = AERandom::gRandom->nextInt(2);
        texts = (DialogueWindowGermanTextTable *) g_dw_germanBriefingTexts;
    }

    int *picked = &(&texts->femaleVariantBase)[index];
    if (male != 0) {
        picked = &(&texts->maleRaceRow)[race * 2 + index];
    }
    return *picked;
}


static int g_dw_drawGuard_storage = 0;
static int *g_dw_drawGuard = &g_dw_drawGuard_storage;

static Layout *g_dw_layoutDraw_storage = nullptr;
static Layout **g_dw_layoutDraw = &g_dw_layoutDraw_storage;

static ImageFactory *g_dw_imageFactoryDraw_storage = nullptr;
static ImageFactory **g_dw_imageFactoryDraw = &g_dw_imageFactoryDraw_storage;

static int g_dw_drawPositionsReady_storage = 0;
static int *g_dw_drawPositionsReady = &g_dw_drawPositionsReady_storage;

static int g_dw_moreButtonX_storage[4];
static int *g_dw_moreButtonX = g_dw_moreButtonX_storage;

static int g_dw_moreButtonY_storage[4];
static int *g_dw_moreButtonY = g_dw_moreButtonY_storage;

static int g_dw_nextButtonX_storage[4];
static int *g_dw_nextButtonX = g_dw_nextButtonX_storage;

static int g_dw_nextButtonY_storage[4];
static int *g_dw_nextButtonY = g_dw_nextButtonY_storage;

static int g_dw_drawReadyFlag_storage = 0;
static int *g_dw_drawReadyFlag = &g_dw_drawReadyFlag_storage;

void DialogueWindow::draw() {
    PaintCanvas::gCanvas->SetColor((unsigned int) -1);
    Layout *layout = *g_dw_layoutDraw;
    layout->drawMask();
    String title;
    title.Set((this->agentName).data);
    layout->drawBox(7, this->frameX, this->frameY, this->frameWidth, this->frameHeight, title, 1);
    { if (title.data) delete[] title.data; title.data = nullptr; title.length = 0; }

    this->scrollWindow->draw();

    layout = *g_dw_layoutDraw;
    int margin = layout->field_0x4c;
    (*g_dw_imageFactoryDraw)->drawChar(this->faceParts,
                                       this->frameX + margin, this->frameY + margin + layout->field_0x8,
                                       this->mirrorFace);

    this->prevButton->draw();
    this->nextButton->draw();
    this->moreButton->draw();

    if (this->choiceActive != 0) {
        this->choiceWindow->draw();
    }

    if (*g_dw_drawPositionsReady == 0) {
        Vector pos;
        if (this->moreButton != 0) {
            pos = this->moreButton->getPosition();
            g_dw_moreButtonX[2] = (int) pos.x;
            pos = this->moreButton->getPosition();
            g_dw_moreButtonY[2] = (int) pos.y;
        }
        if (this->nextButton != 0) {
            pos = this->nextButton->getPosition();
            g_dw_nextButtonX[3] = (int) pos.x;
            pos = this->nextButton->getPosition();
            g_dw_nextButtonY[3] = (int) pos.y;
        }
        *g_dw_drawReadyFlag = 1;
    }
}

void DialogueWindow::setLevel(Level *level) {
    this->level = level;
}
