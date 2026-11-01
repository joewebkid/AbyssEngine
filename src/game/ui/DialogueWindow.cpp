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


static const int g_dw_briefingCounts[0xa2] = {
    4, 0, 10, 0, 2, 0, 0, 8, 0, 0, 0, 2,
    0, 0, 2, 0, 4, 0, 0, 0, 0, 4, 0, 2,
    8, 0, 6, 0, 0, 2, 0, 0, 2, 0, 0, 0,
    4, 0, 2, 0, 0, 6, 0, 0, 0, 0, 0, 0,
    14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 22, 0, 8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0
};

static const int g_dw_successCounts[0xa2] = {
    0, 38, 6, 10, 4, 12, 2, 10, 18, 22, 24, 26,
    16, 4, 0, 22, 6, 34, 16, 10, 2, 2, 20, 28,
    0, 6, 4, 22, 0, 2, 10, 20, 22, 18, 30, 20,
    4, 16, 4, 20, 0, 10, 8, 4, 8, 0, 20, 32,
    16, 0, 0, 0, 0, 0, 16, 28, 4, 20, 24, 2,
    16, 16, 26, 4, 10, 34, 24, 16, 20, 0, 2, 14,
    30, 18, 4, 26, 20, 40, 0, 0, 12, 0, 16, 18,
    0, 4, 8, 0, 16, 0, 18, 0, 14, 22, 14, 32,
    20, 6, 24, 24, 0, 20, 12, 20, 14, 0, 0, 0,
    30, 8, 6, 10, 6, 10, 2, 8, 36, 12, 2, 22,
    0, 18, 36, 0, 30, 12, 12, 22, 8, 0, 18, 0,
    14, 6, 16, 4, 12, 0, 48, 2, 24, 20, 10, 12,
    0, 2, 24, 16, 12, 10, 6, 16, 16, 22, 32, 8,
    10, 0, 10, 22, 6, 12
};

int DialogueWindow::getMode() {
    return this->kind;
}

int DialogueWindow::length() {
    Mission *mission = this->mission;
    if (mission != 0 && mission->isCampaignMission() != 0) {
        const int *counts;
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
        if (Globals::status->field_114 == 2) result = 0x12;
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


bool DialogueWindow::hasBriefingDialogue(int id) {
    if (id > 0xa1) return false;
    return g_dw_briefingCounts[id] > 0;
}


bool DialogueWindow::hasSuccessDialogue(int id) {
    if (id > 0xa1) return false;
    return g_dw_successCounts[id] > 0;
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


static const char g_dw_emptyLoad[] = "";

static const int g_dw_genericTextIds[15] = {
    379, 380, 381, 382, 383,
    373, 374, 375, 376, 377,
    384, 385, 386, 387, 388
};

static const int g_dw_specialStationStatus2[36] = {
    4, 459, 0, 460, 4, 461, 0, 462, 4, 463, 0, 464,
    4, 465, 0, 466, 4, 467, 0, 468, 4, 469, 0, 470,
    4, 471, 0, 472, 4, 473, 0, 474, 4, 475, 16, 476
};

static const int g_dw_specialStationDefault[12] = {
    4, 479, 0, 480, 4, 481, 0, 482, 4, 483, 0, 484
};

static const int g_dw_campaignBriefingRows[120] = {
    16, 1677, 16, 1680, 0, 1721, 2, 1722,
    0, 1723, 2, 1724, 16, 1725, 2, 1738,
    2, 1748, 16, 1749, 16, 1750, 16, 1753,
    0, 1795, 0, 1819, 1, 1836, 0, 1837,
    1, 1877, 16, 1878, 5, 1897, 6, 1913,
    0, 1914, 6, 1915, 0, 1916, 6, 1928,
    0, 1929, 6, 1930, 0, 1947, 0, 1970,
    7, 2016, 0, 2017, 0, 2028, 0, 2049,
    7, 2050, 0, 2051, 0, 2105, 24, 2106,
    0, 2107, 24, 2108, 0, 2109, 24, 2110,
    0, 2111, 0, 2240, 20, 2241, 0, 2242,
    20, 2243, 0, 2244, 20, 2245, 0, 2246,
    26, 2247, 0, 2248, 26, 2249, 0, 2250,
    31, 2283, 0, 2284, 31, 2285, 0, 2286,
    49, 2879, 0, 2880, 49, 2881, 0, 2882
};

static const int g_dw_campaignSuccessMission1Rows[38] = {
    0, 1700, 2, 1701, 0, 1702, 2, 1703,
    0, 1704, 2, 1705, 0, 1706, 2, 1707,
    2, 1708, 0, 1709, 2, 1710, 2, 1711,
    0, 1712, 2, 1713, 0, 1714, 2, 1715,
    2, 1716, 0, 1717, 16, 1718
};

static int g_dw_campaignSuccessTextIds[0x1000];

static int g_dw_defaultClientImage[5];
static int g_dw_stationPortrait1[5] = {0, 8, 8, 1, 8};
static int g_dw_stationPortrait2[5] = {2, 1, 1, 1, 1};
static int g_dw_stationPortrait5[5] = {0, 2, 3, 3, 10};
static int g_dw_stationPortrait6[5] = {10, 0, 0, 1, 0};
static int g_dw_stationPortrait7[5] = {1, 1, 0, 4, 0};
static int g_dw_stationPortrait20[5] = {7, 1, 1, 0, 0};
static int g_dw_stationPortrait24[5] = {1, 2, 4, 3, 1};
static int g_dw_stationPortrait26[5] = {10, 3, 3, 4, 6};
static int g_dw_stationPortrait31[5] = {0, 2, 10, 9, 2};
static int g_dw_stationPortrait49[5] = {2, 1, 0, 4, 0};
static int g_dw_stationPortrait4[5] = {4, 2, 0, 2, 2};
static int g_dw_stationPortrait16[5] = {11, 1, 0, 0, 2};
// Portrait indices used by the station and campaign briefing tables.
static int *const g_dw_stationPortraits[50] = {
    g_dw_defaultClientImage, g_dw_stationPortrait1, g_dw_stationPortrait2, nullptr, g_dw_stationPortrait4,
    g_dw_stationPortrait5, g_dw_stationPortrait6, g_dw_stationPortrait7, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, g_dw_stationPortrait16, nullptr, nullptr, nullptr,
    g_dw_stationPortrait20, nullptr, nullptr, nullptr, g_dw_stationPortrait24,
    nullptr, g_dw_stationPortrait26, nullptr, nullptr, nullptr,
    nullptr, g_dw_stationPortrait31, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr, g_dw_stationPortrait49
};

void DialogueWindow::loadContent() {
    GameText *gameText = Globals::gameText;
    FModSound *sound = Globals::sound;

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
        if (kind == 0) {
            base = this->briefingOffsets[this->campaignMission];
            int row = base + page * 2;
            int portraitId = g_dw_campaignBriefingRows[row];
            textId = g_dw_campaignBriefingRows[row + 1];
            this->clientImage = g_dw_stationPortraits[portraitId];
            this->agentName = *gameText->getText(0x63d + portraitId);
            this->mirrorFace = portraitId == 0;
            this->bodyText = *gameText->getText(textId);
            unsigned textIndex = static_cast<unsigned>(row + 1);
            // ARM 0x16759c: only these briefing entries pass through the binding helper.
            if ((textIndex <= 23 && (0xA0200Au & (1u << textIndex)) != 0) || textIndex == 35) {
                this->bodyText = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(this->bodyText);
            }
        } else if (kind == 1) {
            base = this->successOffsets[this->campaignMission];
            if (this->campaignMission == 1) {
                int row = page * 2;
                int portraitId = g_dw_campaignSuccessMission1Rows[row];
                textId = g_dw_campaignSuccessMission1Rows[row + 1];
                this->clientImage = g_dw_stationPortraits[portraitId];
                this->agentName = *gameText->getText(0x63d + portraitId);
                this->mirrorFace = portraitId == 0;
            } else {
                textId = g_dw_campaignSuccessTextIds[base + page * 2 + 1];
                this->clientImage = g_dw_defaultClientImage;
                this->agentName = *gameText->getText(0x63d + (textId & 0xff));
            }
            this->bodyText = *gameText->getText(textId);
            if (this->campaignMission == 1 && textId == 1718) {
                this->bodyText = static_cast<Globals *>(Globals::globals)->replaceKeyBindingTokens(this->bodyText);
            }
        } else {
            this->clientImage = g_dw_stationPortraits[16];
            this->agentName = *gameText->getText(0x63d + 16);
            String extra("");
            if (this->campaignMission >= 38 && this->campaignMission <= 41 &&
                this->campaignMission != 39) {
                extra = *gameText->getText(527);
            }
            this->bodyText = *gameText->getText(392) + String("\n", false) +
                             extra + String("\n\n", false) + *gameText->getText(319);
            textId = -1;
        }
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

        bool fixedIntroText = false;
        if (kind == 0) {
            if (mission->getType() == 12) {
                textId = 372;
                fixedIntroText = true;
            } else if (mission->getType() == 7) {
                textId = 378;
                fixedIntroText = true;
            }
        }
        bool specialStation = false;
        if (!fixedIntroText && kind == 0 && mission->getTargetStation() == 0x6c) {
            const int *rows = Globals::status->field_114 == 2
                                  ? g_dw_specialStationStatus2 : g_dw_specialStationDefault;
            int portraitId = rows[page * 2];
            textId = rows[page * 2 + 1];
            this->bodyText = *gameText->getText(textId);
            this->clientImage = g_dw_stationPortraits[portraitId];
            this->agentName = *gameText->getText(0x63d + portraitId);
            specialStation = true;
        }
        bool failureQuantityText = false;
        if (kind != 0 && kind != 1 && mission->getType() == 12) {
            textId = 371;
            String first = Globals::status->replaceHash(
                String(*gameText->getText(textId), false),
                String(this->level->killCountB), String("#Q1", false));
            this->bodyText = Globals::status->replaceHash(
                first, String(this->level->killCountA), String("#Q2", false));
            failureQuantityText = true;
        }
        bool fixedSuccessText = false;
        if (kind == 1) {
            int missionType = mission->getType();
            if (missionType == 5 || missionType == 3) {
                textId = 389;
                this->bodyText = Globals::status->replaceHash(
                    String(*gameText->getText(textId), false),
                    mission->getTargetStationName(), String("#S", false));
                fixedSuccessText = true;
            } else if (missionType == 12) {
                textId = 370;
                String suffix("");
                Agent *agent = mission->getAgent();
                if (agent != 0 && agent->isStoryAgent()) {
                    suffix = String("\n\n", false) + Globals::status->replaceHash(
                        String(*gameText->getText(389), false),
                        agent->getStationName(), String("#S", false));
                }
                String body = String(*gameText->getText(textId), false) + suffix;
                String first = Globals::status->replaceHash(
                    body, String(this->level->killCountB), String("#Q1", false));
                this->bodyText = Globals::status->replaceHash(
                    first, String(this->level->killCountA), String("#Q2", false));
                fixedSuccessText = true;
            } else if (missionType == 0xb7) {
                int variant = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
                textId = g_dw_genericTextIds[5 + variant];
                this->bodyText = *gameText->getText(textId);
                String suffix = String("\n\n", false) + *gameText->getText(176) +
                                String(": ", false) + String(this->level->killCountB);
                this->bodyText = this->bodyText + suffix;
                fixedSuccessText = true;
            }
        }
        if (!fixedIntroText && !specialStation && !failureQuantityText && !fixedSuccessText &&
            (kind == 1 || kind == 0 || kind == 2)) {
            Agent *agent = mission->getAgent();
            if (GameText::getLanguage() == 1 && agent != 0) {
                textId = this->pickGermanGenericTextBecauseWeSaved100EurosWithThat(kind, agent);
            } else {
                int variant = static_cast<AERandom *>(Globals::rnd)->nextInt(5);
                if (kind == 0) textId = g_dw_genericTextIds[variant];
                else if (kind == 1) textId = g_dw_genericTextIds[5 + variant];
                else textId = g_dw_genericTextIds[10 + variant];
            }
        } else if (!fixedIntroText && !specialStation && !failureQuantityText && !fixedSuccessText) {
            textId = 0x20f;
        }
        if (!specialStation && !failureQuantityText && !fixedSuccessText)
            this->bodyText = *gameText->getText(textId);

        if (kind == 1) {
            if (!fixedSuccessText) {
                String suffix("\n\n", false);
                Agent *agent = mission->getAgent();
                if (agent != 0 && agent->isStoryAgent()) {
                    suffix = suffix + Globals::status->replaceHash(
                        String(*gameText->getText(389), false),
                        agent->getStationName(), String("#S", false));
                } else {
                    suffix = suffix + *gameText->getText(216);
                }
                this->bodyText = this->bodyText + suffix;
            }
            Standing *standing = Status::gStatus->getStanding();
            standing->applyMissionCompleted(mission->getClientRace());
            if (mission->getTargetStation() == 0x6c) {
                textId = 458;
                this->bodyText = *gameText->getText(textId);
            }
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
    this->faceParts = ((ImageFactory *) Globals::imageFactory)->loadChar((int *) this->clientImage);

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


void DialogueWindow::update(int dt) {
    if (this->scrollWindow != 0) {
        this->scrollWindow->update(dt);
    }
    if (this->choiceActive != 0) {
        this->choiceWindow->update(dt);
    }
    if (Globals::options[0x0e] != 0 && this->voiceSound != -1) {
        Globals::sound->getPlayingProgress(this->voiceSound);
        if (Globals::sound->isPlaying(this->voiceSound) == 0 && this->isLastPage() == 0) {
            if (this->autoAdvanceTimer >= this->pauseLength) {
                this->nextPage();
            }
            this->autoAdvanceTimer = this->autoAdvanceTimer + dt;
        }
    }
}


int DialogueWindow::OnTouchEnd(int x, int y) {
    if (this->choiceActive != 0) {
        int r = this->choiceWindow->OnTouchEnd(x, y);
        if (r != 1) {
            if (r != 0) return 0;
            this->choiceActive = 0;
            if (Status::gStatus->getCurrentCampaignMission() == 0x0f) {
                Globals::sound->play(0xa2, 0, 0, 0);
                Globals::sound->stop(Globals::sound->currentMusicEvent);
                Globals::sound->play(0x88, 0, 0, 0);
            }
            if (this->voiceSound != -1) {
                Globals::sound->stop(this->voiceSound);
            }
            return 1;
        }
        this->choiceActive = 0;
        return 0;
    }

    this->scrollWindow->OnTouchEnd(x, y);
    if (this->prevButton->OnTouchEnd(x, y) != 0) {
        Globals::sound->stop(this->voiceSound);
        this->previousPage();
    }
    if (this->nextButton->OnTouchEnd(x, y) != 0) {
        Globals::sound->stop(this->voiceSound);
        if (this->nextPage() == 0) {
            return 1;
        }
    }
    if (this->moreButton->OnTouchEnd(x, y) != 0) {
        ChoiceWindow *choiceWindow = this->choiceWindow;
        String *text = Globals::gameText->getText(0x18c);
        choiceWindow->set(*text, true);
        this->choiceActive = 1;
    }
    return 0;
}


static const char g_dw_defaultAgentName[] = "";

static inline int half_round_to_zero(int v) {
    return (v + ((unsigned) v >> 31)) >> 1;
}

int DialogueWindow::init() {
    this->briefingOffsets = new int[0xa2];
    this->successOffsets = new int[0xa2];

    int briefingSum = 0;
    int successSum = 0;
    for (int i = 0; i != 0xa2; ++i) {
        int briefingCount = g_dw_briefingCounts[i];
        int successCount = g_dw_successCounts[i];
        this->briefingOffsets[i] = briefingSum;
        this->successOffsets[i] = successSum;
        briefingSum += briefingCount;
        successSum += successCount;
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

    Layout *layout = Globals::layout;
    int frameW = layout->field_0x54;
    int frameH = layout->field_0x58;
    this->frameWidth = frameW;
    this->frameHeight = frameH;
    this->frameX = half_round_to_zero(Globals::w) - half_round_to_zero(frameW);
    this->frameY = half_round_to_zero(Globals::h) - half_round_to_zero(frameH);

    int margin = layout->field_0x4c;
    this->scrollWindow = new ScrollTouchWindow(
        this->frameX + margin * 2 + layout->field_0x2d4,
        layout->field_0x8 + this->frameY,
        frameW - margin * 2 - layout->field_0x2d4,
        frameH - margin * 2 - layout->field_0x8 - layout->field_0x30,
        false);

    this->choiceWindow = new ChoiceWindow();

    GameText *gameText = Globals::gameText;
    layout = Globals::layout;
    margin = layout->field_0x4c;
    this->prevButton = new TouchButton(*gameText->getText(0xb3), 5,
                                       this->frameX + margin, this->frameY - margin + this->frameHeight,
                                       layout->field_0x50, 0x21, 4);

    layout = Globals::layout;
    margin = layout->field_0x4c;
    this->nextButton = new TouchButton(*gameText->getText(0xb4), 6,
                                       this->frameX + this->frameWidth - margin,
                                       this->frameY - margin + this->frameHeight,
                                       layout->field_0x50, 0x22, 4);

    layout = Globals::layout;
    margin = layout->field_0x4c;
    this->moreButton = new TouchButton(*gameText->getText(0x18b), 0,
                                       this->frameX + half_round_to_zero(this->frameWidth),
                                       this->frameY + this->frameHeight - margin,
                                       this->frameWidth - margin * 4 - layout->field_0x50 * 2, 0x24, 4);
    this->choiceActive = 0;
    return 0;
}


static const char g_dw_emptyString[] = "";

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

    this->faceParts = ((ImageFactory *) Globals::imageFactory)->loadChar(parts);
    delete this->nextButton;
    this->nextButton = 0;

    GameText *gameText = Globals::gameText;
    Layout *layout = Globals::layout;
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


static const int g_dw_germanTexts[62] = {
    379, 380, 379, 380, 380, 383, 380, 383, 379, 380,
    379, 380, 379, 381, 380, 382, 379, 380, 379, 380,
    387, 388, 384, 385, 384, 385, 384, 385, 385, 387,
    384, 385, 384, 386, 385, 386, 384, 385, 384, 385,
    373, 376, 373, 374, 374, 376, 374, 376, 373, 374,
    373, 374, 373, 375, 373, 375, 373, 374, 373, 374,
    2376, 2377
};

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
    const int *texts;
    if (kind == 2) {
        index = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        texts = g_dw_germanTexts + 20;
    } else if (kind != 0) {
        index = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        texts = g_dw_germanTexts + 40;
    } else {
        index = static_cast<AERandom *>(Globals::rnd)->nextInt(2);
        texts = g_dw_germanTexts;
    }

    const int *picked = &texts[18 + index];
    if (male != 0) {
        picked = &texts[race * 2 + index];
    }
    return *picked;
}


void DialogueWindow::draw() {
    Globals::Canvas->SetColor((unsigned int) -1);
    Layout *layout = Globals::layout;
    layout->drawMask();
    String title(this->agentName, false);
    layout->drawBox(7, this->frameX, this->frameY, this->frameWidth, this->frameHeight, title);

    this->scrollWindow->draw();

    layout = Globals::layout;
    int margin = layout->field_0x4c;
    ((ImageFactory *) Globals::imageFactory)->drawChar(this->faceParts,
                                                       this->frameX + margin, this->frameY + margin + layout->field_0x8,
                                                       this->mirrorFace);

    this->prevButton->draw();
    this->nextButton->draw();
    this->moreButton->draw();

    if (this->choiceActive != 0) {
        this->choiceWindow->draw();
    }

    if (Globals::is_choice_window_visible == 0) {
        Vector pos;
        if (this->moreButton != 0) {
            pos = this->moreButton->getPosition();
            Globals::other_buttons_x[2] = (int) pos.x;
            pos = this->moreButton->getPosition();
            Globals::other_buttons_y[2] = (int) pos.y;
        }
        if (this->nextButton != 0) {
            pos = this->nextButton->getPosition();
            Globals::other_buttons_x[3] = (int) pos.x;
            pos = this->nextButton->getPosition();
            Globals::other_buttons_y[3] = (int) pos.y;
        }
        Globals::is_dialogue_window_visible = 1;
    }
}

void DialogueWindow::setLevel(Level *level) {
    this->level = level;
}
