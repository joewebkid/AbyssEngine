
#include "game/menu/SpaceLounge.h"
#include "game/world/SolarSystem.h"
#include "game/world/Galaxy.h"
#include "game/world/Station.h"
#include "engine/render/AEGeometry.h"
#include "game/ui/ChoiceWindow.h"
#include "game/core/CutScene.h"
#include "engine/math/EaseInOut.h"
#include "engine/math/EaseInOutMatrix.h"
#include "engine/core/AERandom.h"
#include "engine/core/ApplicationManager.h"
#include "engine/render/Engine.h"
#include "game/world/Level.h"
#include "game/ship/KIPlayer.h"
#include "game/ui/ListItemWindow.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/ui/ScrollTouchBox.h"
#include "game/world/StarMap.h"
#include "game/ship/Agent.h"
#include "game/core/Globals.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "game/ui/Layout.h"
#include "game/mission/Mission.h"
#include "game/mission/Achievements.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/ui/TouchButton.h"

#include "engine/render/PaintCanvas.h"
#include <cstddef>

#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(SpaceLounge, mode) == 0x14, "SpaceLounge mode offset");
static_assert(offsetof(SpaceLounge, chatActive) == 0x19, "SpaceLounge chat offset");
static_assert(offsetof(SpaceLounge, listVisible) == 0x1c, "SpaceLounge list offset");
static_assert(offsetof(SpaceLounge, title) == 0xa4, "SpaceLounge title offset");
static_assert(offsetof(SpaceLounge, initialized) == 0xb0, "SpaceLounge initialized offset");
static_assert(offsetof(SpaceLounge, savedPostEffect) == 0xb1, "SpaceLounge post-effect offset");
static_assert(offsetof(SpaceLounge, touchDown) == 0xb2, "SpaceLounge touch offset");
static_assert(offsetof(SpaceLounge, touchX) == 0xb4, "SpaceLounge touch-X offset");
#endif

// Application module 5 owns the StarMap pointer at ARM offset 0x10.
struct SpaceLoungeStarMapModule {
    char padding[0x10];
    StarMap *starMap;
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(SpaceLoungeStarMapModule, starMap) == 0x10,
              "StarMap module pointer offset");
#endif

static inline int &layoutMetric(Layout *layout, int off) {
    return *(int *) ((char *) layout + off);
}

namespace AbyssEngine {
    namespace AEMath {
        Vector operator+(const Vector &, const Vector &);

        Vector operator-(const Vector &, const Vector &);

        Vector operator*(const Vector &, float);

    }
}

// Android .bss at 0x2183d4: five zero-initialized words passed to loadChar.
static int SpaceLounge_defaultAgentParts[5] = {};

static void *SpaceLounge_start_text_slot;

int SpaceLounge::OnTouchMove(int x, int y) {
    this->touchX = x;
    this->touchY = y;

    if (this->choiceVisible != 0 || this->chatActive != 0) {
        this->choiceWindow->OnTouchMove(x, y);
        return 0;
    }

    if (this->mapVisible != 0) {
        this->starMap->OnTouchMove(x, y);
        return 0;
    }

    int mode = this->mode;
    if (mode == 0) {
        Array<Agent *> *agents = this->agents;
        if (agents != 0) {
            Array<Vector *> *rects = this->agentRects;
            float fy = (float) y;
            float fx = (float) x;
            this->hoverAgent = -1;
            this->selectedAgent = -1;
            int count = (int) agents->size();
            for (int i = 0; count != i; ++i) {
                float *a = (float *) (*rects)[i * 2];
                if (a[0] < fx) {
                    float *b = (float *) (*rects)[i * 2 + 1];
                    if (fx < b[0] && fy < a[1] && b[1] < fy) {
                        this->hoverAgent = i;
                    }
                }
            }
        }
    } else if (mode == 3) {
        Array<TouchButton *> *buttons = this->buttons;
        (*buttons)[0]->OnTouchMove(x, y);
    } else if (mode == 2) {
        Array<TouchButton *> *buttons = this->buttons;
        for (unsigned i = 0; i < buttons->size(); ++i) {
            (*buttons)[i]->OnTouchMove(x, y);
        }
    }

    Globals::layout->OnTouchMove(x, y);
    if (this->listVisible != 0) {
        this->listWindow->OnTouchMove(x, y);
    } else {
        this->scrollWindow->OnTouchMove(x, y);
    }
    return 0;
}

void SpaceLounge::OnRender3D() {
    if (this->mapVisible != 0) {
        this->starMap->render();
        return;
    }

    CutScene *cutscene = this->cutScene;
    if (cutscene == 0) {
        return;
    }

    if (this->listVisible != 0) {
        if (this->listWindow->shows3DShip() != 0) {
            return;
        }
        cutscene = this->cutScene;
    }

    cutscene->render3D();
}

void SpaceLounge::OnRenderBG() {
    if (this->cutScene != 0) {
        this->cutScene->renderBG();
    }
}

unsigned char SpaceLounge::introFinished() {
    return this->introDone;
}

bool SpaceLounge::listMode() {
    return this->mapVisible == 0;
}

unsigned char SpaceLounge::mapMode() {
    return this->mapVisible;
}

unsigned char SpaceLounge::hangarNeedsUpdate() {
    return this->hangarUpdate;
}

bool SpaceLounge::checkLocationMode() {
    return false;
}

void SpaceLounge::draw3DShip() {
    if (this->listVisible != 0) {
        this->listWindow->render();
    }
}

void SpaceLounge::setHangarUpdate(bool value) {
    this->hangarUpdate = value;
}

int SpaceLounge::OnTouchBegin(int x, int y) {
    this->touchDown = 1;
    this->touchX = x;
    this->touchY = y;
    this->hoverAgent = -1;

    if (this->choiceVisible != 0 || this->chatActive != 0) {
        this->choiceWindow->OnTouchBegin(x, y);
        return 0;
    }

    if (this->mapVisible != 0) {
        this->starMap->OnTouchBegin(x, y);
        return 0;
    }

    if (this->listVisible != 0) {
        this->listWindow->OnTouchBegin(x, y);
    } else {
        int mode = this->mode;
        if (mode == 0) {
            if (this->introDone == 0) {
                return 0;
            }
            Array<Agent *> *agents = this->agents;
            if (agents != 0) {
                Array<Vector *> *rects = this->agentRects;
                float fy = (float) y;
                float fx = (float) x;
                int count = (int) agents->size();
                for (int i = 0; count != i; ++i) {
                    float *a = (float *) (*rects)[i * 2];
                    if (a[0] < fx) {
                        float *b = (float *) (*rects)[i * 2 + 1];
                        if (fx < b[0] && fy < a[1] && b[1] < fy) {
                            this->hoverAgent = i;
                        }
                    }
                }
            }
        } else if (mode == 3) {
            Array<TouchButton *> *buttons = this->buttons;
            (*buttons)[0]->OnTouchBegin(x, y);
        } else if (mode == 2) {
            Array<TouchButton *> *buttons = this->buttons;
            for (unsigned i = 0; i < buttons->size(); ++i) {
                (*buttons)[i]->OnTouchBegin(x, y);
            }
        }
    }

    Globals::layout->OnTouchBegin(x, y);
    this->scrollWindow->OnTouchBegin(x, y);
    return 0;
}

int SpaceLounge::getSpecificSoundForRace(int soundId, int race, bool alternate) {
    unsigned delta;

    switch (race) {
        case 0:
        case 5:
            delta = soundId - 0x2f7;
            if (alternate) {
                if ((unsigned) delta < 0x30) {
                    return soundId + 0xf0;
                }
            } else if ((unsigned) delta < 0x30) {
                return soundId + 0xc0;
            }
            break;
        case 1:
            delta = soundId - 0x2f7;
            if ((unsigned) delta < 0x30) {
                return soundId + 0x120;
            }
            break;
        case 2:
        case 3:
            delta = soundId - 0x2f7;
            if ((unsigned) delta < 0x30) {
                return soundId + 0x90;
            }
            break;
        case 4:
            delta = soundId - 0x2f7;
            if ((unsigned) delta < 0x30) {
                return soundId + 0x60;
            }
            break;
        case 6:
            goto done;
        case 7:
            delta = soundId - 0x2f7;
            if ((unsigned) delta < 0x30) {
                return soundId + 0x30;
            }
            break;
    }

    soundId = -1;
done:
    return soundId;
}

static inline void *selected_agent(SpaceLounge *self) {
    Array<Agent *> *agents = self->agents;
    return (*agents)[self->selectedAgent];
}

static inline bool hit_agent(SpaceLounge *self, int x, int y, int i) {
    Array<Vector *> *rects = self->agentRects;
    float *a = (float *) (*rects)[i * 2];
    if (!(a[0] < (float) x)) {
        return false;
    }
    float *b = (float *) (*rects)[i * 2 + 1];
    return (float) x < b[0] && (float) y < a[1] && b[1] < (float) y;
}

int SpaceLounge::OnTouchEnd(int x, int y) {
    this->touchDown = 0;

    if (this->choiceVisible != 0 || this->chatActive != 0) {
        int result = this->choiceWindow->OnTouchEnd(x, y);
        if (result == 1) {
            this->chatActive = 0;
        } else if (result == 0) {
            ((SpaceLounge *) (this))->onKeyPress(0x10000);
        }
        return 0;
    }

    if (this->mapVisible != 0) {
        if (this->starMap->OnTouchEnd(x, y) != 0) {
            this->cutScene->resetCamera();
            this->mapVisible = 0;
        }
        return 0;
    }

    Layout *layout = Globals::layout;
    if (layout->OnTouchEnd(x, y) != 0) {
        if (this->listVisible != 0) {
            layout->resetWindowDimensions();
            this->listVisible = 0;
            return 0;
        } else if (this->mode != 0) {
            if (this->selectedAgent >= 0) {
                void *agent = selected_agent(this);
                if (((Agent *) (agent))->isGenericAgent() != 0) {
                    ((Agent *) (agent))->setEvent(1);
                }
            }
            this->mode = 0;
            this->singleOffer = 0;
            return 0;
        }
        return 1;
    }

    if (this->listVisible != 0) {
        this->listWindow->OnTouchEnd(x, y);
        if (layout->helpPressed() != 0) {
            layout->initHelpWindow(String(*Globals::gameText->getText(643), false));
        }
        return 0;
    }

    switch (this->mode) {
        case 0:
            if (this->introDone == 0) {
                // Android ARM camera tables at 0x206794 and 0x1fb8d8.
                static const int cameraPosition[4][3] = {
                    {-1800, 620, 1805}, {607, 610, 2362},
                    {136, 444, 1122}, {2222, 450, 1227},
                };
                static const float cameraYaw[4] = {0.04f, 0.43f, 0.60f, 0.46f};
                const int race = Globals::status->getSystem()->getRace();
                Matrix matrix;
                AbyssEngine::AEMath::MatrixSetTranslation(matrix,
                    static_cast<float>(cameraPosition[race][0]),
                    static_cast<float>(cameraPosition[race][1]),
                    static_cast<float>(cameraPosition[race][2]));
                AbyssEngine::AEMath::MatrixSetRotation(matrix, 0.0f, cameraYaw[race], 0.0f);
                PaintCanvas *canvas = Globals::Canvas;
                canvas->CameraSetLocal(canvas->CameraGetCurrent(), matrix);
                if (this->cameraEase != 0) {
                    this->cameraEase->SetRange(matrix, matrix);
                }
                this->introDone = 1;
                this->headBobPhase = 0;
                return 0;
            }
            if (this->agents != 0) {
                this->hoverAgent = -1;
                this->selectedAgent = -1;
                unsigned count = (unsigned) this->agents->size();
                for (unsigned i = 0; i < count; ++i) {
                    if (hit_agent(this, x, y, i)) {
                        this->selectedAgent = i;
                        ((SpaceLounge *) (this))->onKeyPress(0x10000);
                        return 0;
                    }
                }
            }
            break;
        case 1:
            ((SpaceLounge *) (this))->onKeyPress(0x10000);
            break;
        case 2: {
            Array<TouchButton *> *buttons = this->buttons;
            for (unsigned i = 0; i < buttons->size(); ++i) {
                TouchButton *button = (*buttons)[i];
                if (button->OnTouchEnd(x, y) != 0) {
                    Agent *agent = static_cast<Agent *>(selected_agent(this));
                    if (i == 0) {
                        const int offer = agent->getOffer();
                        if (offer == 1) {
                            this->mode = 0;
                            return 0;
                        }
                        if (offer == 0 || offer == 5) {
                            if (this->singleOffer != 0) {
                                this->mode = 2;
                                this->singleOffer = 0;
                                String missionText = Globals::gGlobals->getAgentMissionText(agent);
                                Mission *mission = agent->getMission();
                                if (mission != nullptr && !mission->isEmpty()) {
                                    const int reward = mission->getReward() + mission->getBonus();
                                    missionText = Globals::status->replaceHash(
                                        missionText,
                                        Globals::layout->formatCredits(reward) + this->title,
                                        String("#C", false));
                                }
                                this->scrollWindow->setText(agent->getName(), missionText);
                                this->chatScroll = 0;
                                return 0;
                            }
                            Mission *mission = agent->getMission();
                            const int missionType = mission->getType();
                            int amount = 0;
                            int textId = 0;
                            if (missionType == 0) {
                                amount = mission->getProductionGoodAmount();
                                if (!Globals::status->getShip()->spaceAvailable(amount)) {
                                    textId = 337;
                                }
                            } else if (missionType == 11) {
                                amount = mission->getProductionGoodAmount();
                                if (amount > Globals::status->getShip()->getMaxPassengers()) {
                                    textId = 338;
                                }
                            }
                            if (textId != 0) {
                                String prompt = Globals::status->replaceHash(
                                    String(*Globals::gameText->getText(textId), false),
                                    String(amount), String("#Q", false));
                                this->choiceWindow->set(prompt, false);
                                this->choiceVisible = 1;
                            } else {
                                String prompt;
                                if (agent->isGenericAgent() != 0) {
                                    prompt = *Globals::gameText->getText(865);
                                    const int reward = mission->getReward() + mission->getBonus();
                                    prompt = Globals::status->replaceHash(
                                        prompt, Globals::layout->formatCredits(reward),
                                        String("#C", false));
                                    prompt = Globals::status->replaceHash(
                                        prompt,
                                        String(*Globals::gameText->getText(
                                            mission->getType() + 354), false),
                                        String("#M", false));
                                    if (Globals::status->hardCoreMode()) {
                                        const int hardCoreReward =
                                            (mission->getReward() + mission->getBonus()) / 10;
                                        prompt += String("\n", false) +
                                            *Globals::gameText->getText(27) +
                                            String(": ", false) +
                                            Globals::layout->formatCredits(hardCoreReward);
                                    }
                                } else {
                                    prompt = *Globals::gameText->getText(863);
                                }
                                Mission *freelance = Globals::status->getFreelanceMission();
                                if (freelance != nullptr && !freelance->isEmpty()) {
                                    prompt += String(" ", false) +
                                        *Globals::gameText->getText(864);
                                }
                                this->choiceWindow->set(prompt, true);
                                this->chatActive = 1;
                            }
                        }
                        if (offer == 2 || offer == 3 || offer == 4 ||
                            offer == 8 || offer == 9 || offer == 10) {
                            const float priceFactor =
                                offer >= 8 && Globals::options[0x38] == 0 ? 2.0f : 1.0f;
                            const int credits = Globals::status->getCredits();
                            if (priceFactor * static_cast<float>(agent->getSellItemPrice()) >
                                static_cast<float>(credits)) {
                                const int shortfall = static_cast<int>(
                                    priceFactor * static_cast<float>(agent->getSellItemPrice()) -
                                    static_cast<float>(Globals::status->getCredits()));
                                String prompt = Globals::status->replaceHash(
                                    String(*Globals::gameText->getText(203), false),
                                    Globals::layout->formatCredits(shortfall),
                                    String("#C", false));
                                this->choiceWindow->set(prompt, false);
                                this->choiceVisible = 1;
                                return 0;
                            }
                            if (offer == 2 || offer == 9) {
                                String prompt(*Globals::gameText->getText(868), false);
                                prompt = Globals::status->replaceHash(
                                    prompt, String(agent->getSellItemQuantity()),
                                    String("#Q", false));
                                prompt = Globals::status->replaceHash(
                                    prompt,
                                    String(*Globals::gameText->getText(
                                        agent->getSellItemIndex() + 1274), false),
                                    String("#P", false));
                                const int cost = static_cast<int>(
                                    priceFactor * static_cast<float>(agent->getSellItemPrice()));
                                prompt = Globals::status->replaceHash(
                                    prompt, Globals::layout->formatCredits(cost),
                                    String("#C", false));
                                this->choiceWindow->set(prompt, true);
                                this->chatActive = 1;
                            } else if (offer == 3) {
                                String prompt(*Globals::gameText->getText(869), false);
                                Item *item = (*static_cast<Array<Item *> *>(Globals::items))[
                                    agent->getSellBlueprintIndex()];
                                prompt = Globals::status->replaceHash(
                                    prompt,
                                    String(*Globals::gameText->getText(
                                        item->getIndex() + 1274), false),
                                    String("#P", false));
                                prompt = Globals::status->replaceHash(
                                    prompt, Globals::layout->formatCredits(agent->getSellItemPrice()),
                                    String("#C", false));
                                this->choiceWindow->set(prompt, true);
                                this->chatActive = 1;
                            } else if (offer == 4) {
                                String prompt(*Globals::gameText->getText(870), false);
                                auto *galaxy = static_cast<Galaxy *>(Globals::galaxy);
                                SolarSystem *system = galaxy->getSystem(
                                    agent->getSellSystemIndex());
                                prompt = Globals::status->replaceHash(
                                    prompt, system->getName(), String("#S", false));
                                prompt = Globals::status->replaceHash(
                                    prompt, Globals::layout->formatCredits(agent->getSellItemPrice()),
                                    String("#C", false));
                                this->choiceWindow->set(prompt, true);
                                this->chatActive = 1;
                            } else if (offer == 8) {
                                String prompt(*Globals::gameText->getText(871), false);
                                const int cost = static_cast<int>(
                                    priceFactor * static_cast<float>(agent->getSellItemPrice()));
                                prompt = Globals::status->replaceHash(
                                    prompt, Globals::layout->formatCredits(cost),
                                    String("#C", false));
                                this->choiceWindow->set(prompt, true);
                                this->chatActive = 1;
                            } else if (offer == 10) {
                                String prompt(*Globals::gameText->getText(873), false);
                                const int cost = static_cast<int>(
                                    priceFactor * static_cast<float>(agent->getSellItemPrice()));
                                prompt = Globals::status->replaceHash(
                                    prompt, Globals::layout->formatCredits(cost),
                                    String("#C", false));
                                this->choiceWindow->set(prompt, true);
                                this->chatActive = 1;
                            }
                        }
                        if (offer == 6) {
                            if (Globals::status->getWingmen() != nullptr) {
                                this->choiceWindow->set(*Globals::gameText->getText(785), false);
                                this->choiceVisible = 1;
                                return 0;
                            }
                            const int credits = Globals::status->getCredits();
                            if (credits < agent->getCosts()) {
                                const int shortfall = agent->getCosts() -
                                    Globals::status->getCredits();
                                String prompt = Globals::status->replaceHash(
                                    String(*Globals::gameText->getText(203), false),
                                    Globals::layout->formatCredits(shortfall),
                                    String("#C", false));
                                this->choiceWindow->set(prompt, false);
                                this->choiceVisible = 1;
                                return 0;
                            }
                            auto *achievements = static_cast<Achievements *>(Globals::achievements);
                            const int textId = achievements->gotAllMedals() ? 867 : 866;
                            String prompt(*Globals::gameText->getText(textId), false);
                            prompt = Globals::status->replaceHash(
                                prompt, String(agent->getWingmanFriendsCount() + 1),
                                String("#Q", false));
                            prompt = Globals::status->replaceHash(
                                prompt, Globals::layout->formatCredits(agent->getCosts()),
                                String("#C", false));
                            this->choiceWindow->set(prompt, true);
                            this->chatActive = 1;
                        }
                        if (offer == 7) {
                            const int cost = agent->getCosts();
                            const int credits = Globals::status->getCredits();
                            const bool canAfford = credits >= cost;
                            const int textId = canAfford ? 885 : 203;
                            const int shownCredits = canAfford ? cost : cost - credits;
                            String prompt = Globals::status->replaceHash(
                                String(*Globals::gameText->getText(textId), false),
                                Globals::layout->formatCredits(shownCredits),
                                String("#C", false));
                            this->choiceWindow->set(prompt, canAfford);
                            if (!canAfford) {
                                this->choiceVisible = 1;
                                return 0;
                            }
                            this->chatActive = 1;
                        }
                    } else if (i == 1) {
                        AbyssEngine::AERandom *random =
                            static_cast<AbyssEngine::AERandom *>(Globals::rnd);
                        this->scrollWindow->setText(agent->getName(),
                            String(*Globals::gameText->getText(random->nextInt(5) + 845), false));
                        this->chatScroll = 0;
                        this->mode = 0;
                        ++Globals::status->field_e0;
                    } else if (i == 2) {
                        if (this->listWindow == nullptr) {
                            this->listWindow = new ListItemWindow();
                        }
                        const int offer = agent->getOffer();
                        if (offer == 2 || offer == 9) {
                            Item *item = (*static_cast<Array<Item *> *>(Globals::items))[
                                agent->getSellItemIndex()];
                            this->listWindow->set(new ListItem(item), 0, 0, 0, 0, false);
                        } else if (offer == 10) {
                            AbyssEngine::Engine *engine = static_cast<AbyssEngine::Engine *>(
                                static_cast<AbyssEngine::ApplicationManager *>(
                                    Globals::appManager)->GetEngine());
                            this->savedPostEffect = engine->IsPostEffectActivated();
                            engine->SetPostEffect(0x1400000u, false);
                            Ship *ship = (*static_cast<Array<Ship *> *>(Globals::ships))[
                                agent->getSellItemIndex()];
                            this->listWindow->set(new ListItem(ship), 0, 0, 0, 0, false);
                        } else {
                            Array<BluePrint *> *blueprints = Globals::status->getBluePrints();
                            for (unsigned j = 0; j < blueprints->size(); ++j) {
                                BluePrint *blueprint = (*blueprints)[j];
                                if (blueprint->getIndex() == agent->getSellBlueprintIndex()) {
                                    Item *item = (*static_cast<Array<Item *> *>(Globals::items))[
                                        blueprint->getIndex()];
                                    this->listWindow->set(new ListItem(item), 0, 0, 0, 0, false);
                                    break;
                                }
                            }
                        }
                        this->listVisible = 1;
                    } else if (i == 3) {
                        agent->wasAskedForLocation = 1;
                        Mission *mission = agent->getMission();
                        const int targetStation = mission->getTargetStation();
                        int textId = 0;
                        if (targetStation == Globals::status->getStation()->getIndex()) {
                            textId = 805;
                        } else if (Globals::status->getSystem()->stationIsInSystem(
                                       targetStation)) {
                            textId = 806;
                        }
                        if (textId != 0) {
                            this->scrollWindow->setText(agent->getName(),
                                String(*Globals::gameText->getText(textId), false));
                            this->chatScroll = 0;
                            this->mode = 2;
                        } else {
                            auto *manager = static_cast<AbyssEngine::ApplicationManager *>(
                                Globals::appManager);
                            auto *module = static_cast<SpaceLoungeStarMapModule *>(
                                manager->GetApplicationModule(5));
                            this->starMap = module->starMap;
                            if (this->starMap != nullptr) {
                                this->starMap->init(true, mission, false, -1);
                            } else {
                                module->starMap = new StarMap(true, mission, false, -1);
                                this->starMap = module->starMap;
                            }
                            this->mapVisible = 1;
                        }
                    } else if (i == 4) {
                        agent->wasAskedForDifficulty = 1;
                        const int difficulty = agent->getMission()->getDifficulty();
                        const int textId = static_cast<int>(
                            (static_cast<float>(difficulty) / 10.0f) * 5.0f) + 808;
                        this->scrollWindow->setText(agent->getName(),
                            String(*Globals::gameText->getText(textId), false));
                        this->singleOffer = 1;
                        this->mode = 2;
                        this->chatScroll = 0;
                    }
                    if (agent->isGenericAgent() != 0) {
                        agent->setEvent(1);
                    }
                }
            }
            break;
        }
        case 3:
            if ((*this->buttons)[0]->OnTouchEnd(x, y) != 0) {
                ((SpaceLounge *) (this))->onKeyPress(0x10000);
            }
            break;
    }

    this->scrollWindow->OnTouchEnd(x, y);
    if (layout->helpPressed() != 0) {
        layout->initHelpWindow(String(*Globals::gameText->getText(627), false));
    }
    return 0;
}

int SpaceLounge::getSoundId(Agent *agent) {
    String missionText;
    AbyssEngine::AERandom *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);

    int race = ((Agent *) (agent))->getRace();
    bool male = ((Agent *) (agent))->isMale();
    int offer = ((Agent *) (agent))->getOffer();
    void *mission = ((Agent *) (agent))->getMission();
    int missionType;
    if (mission == 0) {
        missionType = -1;
    } else {
        mission = ((Agent *) (agent))->getMission();
        if (((Mission *) (mission))->isEmpty() != 0) {
            missionType = -1;
        } else {
            mission = ((Agent *) (agent))->getMission();
            missionType = ((Mission *) (mission))->getType();
        }
    }

    missionText = Globals::gGlobals->getAgentMissionText(static_cast<Agent *>(agent));

    bool checkSpecialText = true;
    int soundId;
    switch (offer) {
        case 0:
            if (missionType == 0 || missionType == 0xb) {
                soundId = random->nextInt(4) + 0x301;
            } else if (missionType == 0xc) {
                soundId = random->nextInt(4) + 0x2fa;
            } else {
                int first = random->nextInt(2);
                if (first == 0) {
                    soundId = random->nextInt(4) + 0x31f;
                } else {
                    soundId = random->nextInt(4) + 0x309;
                }
            }
            break;
        case 1:
            soundId = random->nextInt(2) + 0x30d;
            break;
        case 2:
        case 3:
        case 8:
        case 9:
        case 10:
            soundId = random->nextInt(2) + 0x2f7;
            break;
        case 4:
            soundId = random->nextInt(2) + 0x2fe;
            break;
        case 5:
            soundId = random->nextInt(4) + 0x31b;
            break;
        case 6:
            soundId = random->nextInt(4) + 0x323;
            break;
        case 7:
            soundId = random->nextInt(4) + 0x305;
            break;
        default:
            soundId = -1;
            break;
    }

    if (offer != 1) {
        checkSpecialText = false;
        if (random->nextInt(100) < 30) {
            soundId = random->nextInt(2) + 0x30d;
            checkSpecialText = true;
        }
    }

    int dummy = 0;
    if (((Agent *) (agent))->hasAcceptedOffer() != 0) {
        dummy = random->nextInt(2);
        soundId = dummy + 0x30d;
    }

    if (checkSpecialText) {
        GameText *texts = Globals::gameText;
        void *text = texts->getText(0x334);
        if (missionText.Compare_str((String *) text) != 0) {
            text = texts->getText(0x338);
            if (missionText.Compare_str((String *) text) != 0) {
                text = texts->getText(0x33b);
                if (missionText.Compare_str((String *) text) != 0) {
                    text = texts->getText(0x341);
                    dummy = missionText.Compare_str((String *) text);
                    if (dummy != 0) {
                        goto special_done;
                    }
                }
            }
        }
        dummy = random->nextInt(2);
        soundId = dummy + 0x314;
    }

special_done:
    if (race == 3) {
        int *parts = ((Agent *) (agent))->getImageParts();
        if (parts == 0) {
            race = 3;
            dummy = 0;
        } else {
            parts = ((Agent *) (agent))->getImageParts();
            dummy = parts[0];
            race = 0;
            if (dummy == 2) {
                race = 3;
            }
        }
    }

    int result = this->getSpecificSoundForRace(soundId, race, male);
    return result;
}

static inline void *key_agent(SpaceLounge *self) {
    return (*self->agents)[self->selectedAgent];
}

void SpaceLounge::onKeyPress(int key) {
    char scratch[960];
    (void) scratch;

    if (this->choiceVisible != 0) {
        if (key == 0x1000) {
            this->choiceWindow->left();
        } else if (key == 0x2000) {
            this->choiceWindow->right();
        } else if (key == 0x10000) {
            this->choiceVisible = 0;
        }
        return;
    }

    int mode = this->mode;
    if (mode == 0) {
        if (key == 0x1000 || key == 0x8000) {
            unsigned next = this->selectedAgent + 1;
            if (this->agents != 0 && next >= this->agents->size()) {
                next = 0;
            }
            this->selectedAgent = next;
        } else if (key == 0x2000 || key == 0x4000) {
            int current = this->selectedAgent;
            if (current < 1 && this->agents != 0) {
                current = (int) this->agents->size();
            }
            this->selectedAgent = current - 1;
        } else if (key == 0x10000 || key == 0x20000) {
            ((SpaceLounge *) (this))->startChat();
        }
        return;
    }

    if (mode == 1 || mode == 3) {
        if (key == 0x10000 || key == 0x20000) {
            this->chatAnswer = 0;
            void *agent = key_agent(this);
            if (((Agent *) (agent))->getOffer() == 1) {
                this->mode = 2;
            } else {
                this->mode = mode == 1 ? 2 : 0;
                if (mode == 3 && this->field_0x18_byte != 0) {
                    auto *manager = static_cast<AbyssEngine::ApplicationManager *>(
                        Globals::appManager);
                    auto *module = static_cast<SpaceLoungeStarMapModule *>(
                        manager->GetApplicationModule(5));
                    this->starMap = module->starMap;
                    const int systemIndex = ((Agent *) (agent))->getSellSystemIndex();
                    if (this->starMap != nullptr) {
                        this->starMap->init(false, nullptr, true, systemIndex);
                    } else {
                        module->starMap = new StarMap(false, nullptr, true, systemIndex);
                        this->starMap = module->starMap;
                    }
                    this->field_0x18_byte = 0;
                    this->mapVisible = 1;
                }
            }
        } else if (key == 0x40000) {
            if (this->chatScroll < 3) {
                if (mode == 1) {
                    this->chatScroll = 0;
                    this->mode = 0;
                }
            } else {
                this->chatScroll -= 0x28;
            }
        }
        return;
    }

    if (mode == 2) {
        Agent *agent = static_cast<Agent *>(key_agent(this));
        if (this->listVisible != 0) {
            if (key == 0x40000) {
                this->listVisible = 0;
                auto *manager = static_cast<AbyssEngine::ApplicationManager *>(
                    Globals::appManager);
                auto *engine = static_cast<AbyssEngine::Engine *>(manager->GetEngine());
                engine->SetPostEffect(0x1400000u, this->savedPostEffect != 0);
            }
            return;
        }
        if (this->chatActive != 0) {
            if (key == 0x1000) {
                this->choiceWindow->left();
                return;
            }
            if (key == 0x2000) {
                this->choiceWindow->right();
                return;
            }
            if (key == 0x10000 && agent->getOffer() == 4) {
                auto *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);
                String reply(*Globals::gameText->getText(random->nextInt(3) + 850), false);
                Status *status = Globals::status;
                status->changeCredits(-agent->getSellItemPrice());
                status->setSystemVisibility(agent->getSellSystemIndex(), true);
                this->field_0x18_byte = 1;
                this->hangarUpdate = 1;
                Array<Agent *> *allAgents = status->getAgents();
                const unsigned index = static_cast<unsigned>(agent->getIndex());
                if (index < allAgents->size()) {
                    (*allAgents)[index]->setOfferAccepted(true);
                }
                agent->setOfferAccepted(true);
                this->scrollWindow->setText(agent->getName(), reply);
                this->chatScroll = 0;
                this->mode = 3;
                this->chatActive = 0;
                return;
            }
            return;
        }
        Mission *mission = agent->getMission();
        int optionCount = 3;
        if (mission != nullptr && mission->isOutsideMission()) {
            optionCount = 5;
        } else {
            const int offer = agent->getOffer();
            if (offer == 2 || offer == 3 || offer == 8 || offer == 9 || offer == 10) {
                optionCount = 4;
            }
        }
        if (key == 0x4000) {
            const int current = this->chatAnswer < 1 ? optionCount : this->chatAnswer;
            this->chatAnswer = current - 1;
            return;
        }
        if (key == 0x8000) {
            const int next = this->chatAnswer + 1;
            this->chatAnswer = next >= optionCount ? 0 : next;
            return;
        }
        if (key != 0x10000 && key != 0x20000) {
            return;
        }
        switch (this->chatAnswer) {
            case 1: {
                auto *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);
                this->scrollWindow->setText(agent->getName(),
                    String(*Globals::gameText->getText(random->nextInt(5) + 845), false));
                this->chatScroll = 0;
                this->mode = 3;
                ++Globals::status->field_e0;
                break;
            }
            case 2:
                this->mode = 2;
                this->scrollWindow->setText(agent->getName(),
                    Globals::gGlobals->getAgentMissionText(agent));
                this->chatScroll = 0;
                ++Globals::status->field_e4;
                break;
            case 4: {
                agent->wasAskedForDifficulty = 1;
                const int difficulty = agent->getMission()->getDifficulty();
                const int textId = static_cast<int>(
                    (static_cast<float>(difficulty) / 10.0f) * 5.0f) + 808;
                this->scrollWindow->setText(agent->getName(),
                    String(*Globals::gameText->getText(textId), false));
                this->chatScroll = 0;
                this->mode = 2;
                break;
            }
            default:
                return;
        }
        if (agent->isGenericAgent() != 0) {
            agent->setEvent(1);
        }
        return;
    }
}

void SpaceLounge::updateScreenPositions() {
    if (this->agents == nullptr) {
        return;
    }

    Array<KIPlayer *> *enemies = this->cutScene->level->getEnemies();
    PaintCanvas *canvas = Globals::Canvas;
    const Matrix *local = static_cast<const Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
    const Vector halfRight = 150.0f * AbyssEngine::AEMath::MatrixGetRight(*local);
    const unsigned count = this->agents->size();
    for (unsigned i = 0; i < count; ++i) {
        KIPlayer *enemy = (*enemies)[i];
        const Vector target = enemy->getPosition();
        const Vector left = target - halfRight;
        canvas->GetScreenPosition(left, *(*this->agentRects)[2 * i]);
        Vector right = target + halfRight;
        right.y += 450.0f;
        canvas->GetScreenPosition(right, *(*this->agentRects)[2 * i + 1]);

        const Matrix *current = static_cast<const Matrix *>(
            canvas->CameraGetLocal(canvas->CameraGetCurrent()));
        Matrix camera = *current;
        camera = AbyssEngine::AEMath::MatrixGetLookAt(
            AbyssEngine::AEMath::MatrixGetPosition(camera), target,
            AbyssEngine::AEMath::MatrixGetUp(camera));

        KIPlayer *mapped = (*enemies)[count + i];
        mapped->geometry->setMatrix(camera);
        this->silhouettePos = target;
        const Vector direction = AbyssEngine::AEMath::MatrixGetDir(camera);
        this->silhouettePos.x -= direction.x * 100.0f;
        this->silhouettePos.z -= direction.z * 100.0f;
        mapped->setPosition(this->silhouettePos);

        if (Status::gStatus->getSystem()->getRace() == 0) {
            Matrix rotation;
            AbyssEngine::AEMath::MatrixSetRotation(rotation, 0.0f, 3.1416f, 0.0f);
            camera *= rotation;
        }
        enemy->geometry->setMatrix(camera);
        enemy->setPosition(target);
    }
}

void SpaceLounge::drawLounge() {
    PaintCanvas *canvas = Globals::Canvas;
    Layout *layout = Globals::layout;
    ImageFactory *factory = static_cast<ImageFactory *>(Globals::imageFactory);

    if (this->mode == 0) {
        const int hover = this->hoverAgent;
        if (hover >= 0) {
            Agent *agent = (*this->agents)[hover];
            const Vector &left = *(*this->agentRects)[2 * hover];
            const Vector &right = *(*this->agentRects)[2 * hover + 1];
            const int pad = layout->field_0x94;
            const int centerX = static_cast<int>(left.x + (right.x - left.x) * 0.5f);
            const int textY = static_cast<int>(right.y - static_cast<float>(2 * pad));

            canvas->SetColor(0xFFFFFFFF);
            String name = (agent->isKnown() || agent->isStoryAgent())
                ? agent->getName() : *Globals::gameText->getText(agent->getRace() + 406);
            String offerText;
            if (Mission *mission = agent->getMission()) {
                offerText = *Globals::gameText->getText(mission->getType() + 354);
            } else if (agent->getOffer() == 6) {
                offerText = *Globals::gameText->getText(306);
            } else if (agent->getOffer() == 2) {
                offerText = *Globals::gameText->getText(305);
            } else if (agent->getOffer() == 7) {
                offerText = *Globals::gameText->getText(884);
            } else {
                offerText = "";
            }

            int textWidth = canvas->GetTextWidth(Globals::font, name);
            if (agent->isKnown()) {
                textWidth += canvas->GetTextWidth(Globals::font, offerText + String("  "));
            }
            int textX = centerX;
            const int edge = layout->field_0x28;
            if (centerX - pad < edge) {
                textX = edge + pad;
            } else if (centerX + pad + textWidth > Globals::w - edge) {
                textX = Globals::w - edge - textWidth - 2 * pad;
            }
            const int boxX = textX - pad;
            const int boxY = textY - pad;
            const int boxWidth = textWidth + 2 * pad;
            layout->drawBox(2, boxX, boxY, boxWidth, layout->field_0x30, String(""));
            canvas->SetColor(0x193043FF);
            canvas->DrawRectangle(boxX, boxY, boxWidth, layout->field_0x30);
            canvas->SetColor(0xFFFFFFFF);
            const String spacer(agent->isKnown() ? "  " : "");
            canvas->DrawString(Globals::font, name + spacer,
                               textX, textY + layout->field_0x2c0, false);
            if (agent->isKnown()) {
                canvas->SetColor(0xFF8000FF);
                const int nameWidth = canvas->GetTextWidth(Globals::font, name + String("  "));
                canvas->DrawString(Globals::font, offerText,
                                   textX + nameWidth, textY + layout->field_0x2c0, false);
            }
        }
        return;
    }

    canvas->SetColor(0xFFFFFFFF);
    layout->drawBox(2, this->panelX, this->panelY,
                    layout->field_0x68, layout->field_0x6c, String(""));
    canvas->SetColor(0x193043FF);
    canvas->DrawRectangle(this->panelX, this->panelY,
                          layout->field_0x68, layout->field_0x6c);
    canvas->SetColor(0xFFFFFFFF);
    factory->drawChar((*this->silhouetteGrid)[this->selectedAgent],
                      layout->field_0x4c + this->panelX,
                      layout->field_0x4c + this->panelY, false);
    this->scrollWindow->draw();

    if ((this->mode & 0xfffffffe) != 2) {
        return;
    }

    TouchButton *first = (*this->buttons)[0];
    TouchButton *second = (*this->buttons)[1];
    first->setTextColor(15532287);
    Agent *agent = (*this->agents)[this->selectedAgent];
    const unsigned offer = agent->getOffer();
    if (this->mode == 2) {
        first->setPosition(this->buttonX, this->buttonY0);
        second->setPosition(this->panelWidth + this->buttonX, this->buttonY0, 0x12);
        this->visibleButtonCount = 0;
        if (offer <= 10 && ((1 << offer) & 0x60c) != 0) {
            this->visibleButtonCount = 3;
        } else if (offer == 0 && agent->getMission() != nullptr &&
                   agent->getMission()->isOutsideMission() &&
                   agent->getMission()->getType() != 12) {
            this->visibleButtonCount = this->singleOffer ? 1 : 4;
            if (this->singleOffer) {
                first->setTextColor(-1);
                first->setPosition(this->buttonX, this->buttonY1);
            }
        } else if (offer == 1) {
            this->visibleButtonCount = 1;
            first->setPosition(this->buttonX, this->buttonY1);
        } else {
            this->visibleButtonCount = 2;
            first->setPosition(this->buttonX, this->buttonY1);
            second->setPosition(this->panelWidth + this->buttonX, this->buttonY1, 0x12);
        }
    } else {
        this->visibleButtonCount = 1;
        first->setTextColor(-1);
        first->setPosition(this->buttonX, this->buttonY1);
    }

    for (unsigned i = 0; i < this->buttons->size(); ++i) {
        (*this->buttons)[i]->setVisible(false);
    }

    int buttonHeight = layout->field_0x2d8;
    if (this->visibleButtonCount > 2) {
        int needed = layout->field_0x30 * (this->visibleButtonCount - 1) + layout->field_0x34 * (
                         this->visibleButtonCount - 2);
        if (needed > buttonHeight) {
            buttonHeight = needed;
        }
    }
    int panelHeight = layout->field_0x4c * 2 + buttonHeight;
    layout->drawBox(2, this->panelX, this->buttonPanelY,
                    layout->field_0x68, panelHeight, String(""));
    canvas->SetColor(0x193043FF);
    canvas->DrawRectangle(this->panelX, this->buttonPanelY,
                          layout->field_0x68, panelHeight);
    canvas->SetColor(0xFFFFFFFF);
    factory->drawChar(this->agentImageParts,
                      layout->field_0x4c + this->panelX,
                      this->buttonPanelY + layout->field_0x4c, true);

    first->setVisible(true);
    if (offer == 1 || this->singleOffer || this->mode == 3) {
        first->draw();
        return;
    }
    second->setVisible(true);
    first->draw();
    second->draw();
    if (offer <= 10 && ((1 << offer) & 0x60c) != 0) {
        TouchButton *third = (*this->buttons)[2];
        third->setVisible(true);
        third->draw();
    } else if (offer == 0 && agent->getMission() != nullptr &&
               agent->getMission()->isOutsideMission() &&
               agent->getMission()->getType() != 12) {
        TouchButton *third = (*this->buttons)[3];
        TouchButton *fourth = (*this->buttons)[4];
        third->setVisible(true);
        fourth->setVisible(true);
        third->draw();
        fourth->draw();
    }
}

int SpaceLounge::init() {
    this->touchDown = 0;
    this->initialized = 0;
    this->cameraAnimating = 0;
    this->agents = Status::gStatus->getStation()->getAgents();

    if (this->silhouetteGrid == nullptr && this->agents != nullptr && this->agentRects == nullptr) {
        const unsigned count = this->agents->size();
        this->silhouetteGrid = new Array<Array<ImagePart *> *>();
        ArraySetLength(count, *this->silhouetteGrid);
        this->agentRects = new Array<Vector *>();
        ArraySetLength(count * 2, *this->agentRects);
        ImageFactory *factory = static_cast<ImageFactory *>(Globals::imageFactory);
        for (unsigned i = 0; i < count; ++i) {
            (*this->silhouetteGrid)[i] = factory->loadChar((*this->agents)[i]->getImageParts());
            (*this->agentRects)[i] = new Vector{0.0f, 0.0f, 0.0f};
            (*this->agentRects)[count + i] = new Vector{0.0f, 0.0f, 0.0f};
        }
        this->agentImageParts = factory->loadChar(SpaceLounge_defaultAgentParts);
    }

    if (this->choiceWindow != 0) {
        delete this->choiceWindow;
        this->choiceWindow = 0;
    }
    this->choiceWindow = new ChoiceWindow();
    this->hangarUpdate = 0;

    if (this->cutScene != 0) {
        this->cutScene->resetCamera();
    }

    this->field_0x18 = 0;
    this->agentVisited = new char[0x15];
    for (int i = 0; i != 0x15; ++i) {
        this->agentVisited[i] = 0;
    }

    Layout *layout = Globals::layout;
    int panelW = layoutMetric(layout, 0x68);
    this->panelX = Globals::w / 2 - panelW / 2;
    this->panelY = layout->field_0x20 + layout->field_0xc;
    this->panelWidth = panelW - layout->field_0x4c * 3 - layout->field_0x2d4;

    this->scrollWindow = new ScrollTouchWindow(
        this->panelX + layout->field_0x4c * 2 + layout->field_0x2d4,
        this->panelY, this->panelWidth, layoutMetric(layout, 0x6c), false);

    this->mode = 0;
    this->buttonPanelY = this->panelY + layoutMetric(layout, 0x6c) + layout->field_0x2c;
    this->buttonsHeight = (layout->field_0x34 + layout->field_0x30) * 5;

    if (this->buttons != 0) {
        Array<TouchButton *> *oldButtons = this->buttons;
        ArrayReleaseClasses(*oldButtons);
        delete oldButtons;
    }
    Array<TouchButton *> *buttons = new Array<TouchButton *>();
    this->buttons = buttons;
    ArraySetLength(5, *buttons);

    int baseY = this->buttonPanelY + layout->field_0x4c;
    this->buttonX = this->panelX + layoutMetric(layout, 0x68) - layout->field_0x4c - this->panelWidth;
    this->buttonY1 = (baseY + layout->field_0x2d8 / 2) - layout->field_0x30 / 2;
    this->buttonY0 = baseY;

    (*buttons)[0] = new TouchButton(*Globals::gameText->getText(860), 0,
                                    this->buttonX, baseY,
                                    this->panelWidth / 2 - layout->field_0x4c / 2, 0x11, 4);
    (*buttons)[0]->setTextColor(15532287);
    (*buttons)[1] = new TouchButton(*Globals::gameText->getText(861), 0,
                                    this->panelX + panelW - layout->field_0x4c, baseY,
                                    this->panelWidth / 2 - layout->field_0x4c, 0x12, 4);
    (*buttons)[1]->setTextColor(-14024449);
    int secondY = baseY + layout->field_0x30 + layout->field_0x34;
    (*buttons)[2] = new TouchButton(*Globals::gameText->getText(776), 0,
                                    this->buttonX, secondY, this->panelWidth, 0x11, 4);
    (*buttons)[3] = new TouchButton(*Globals::gameText->getText(804), 0,
                                    this->buttonX, secondY, this->panelWidth, 0x11, 4);
    (*buttons)[4] = new TouchButton(*Globals::gameText->getText(807), 0,
                                    this->buttonX, secondY + layout->field_0x30 + layout->field_0x34,
                                    this->panelWidth, 0x11, 4);

    this->hoverAgent = -1;
    // ARM tables at 0x206794 and 0x1fb8d8: camera position and yaw by race.
    static const int cameraPosition[4][3] = {
        {-1800, 620, 1805}, {607, 610, 2362},
        {136, 444, 1122}, {2222, 450, 1227},
    };
    static const float cameraYaw[4] = {0.04f, 0.43f, 0.60f, 0.46f};
    int race = Status::gStatus->getSystem()->getRace();
    Matrix matrix;
    AbyssEngine::AEMath::MatrixSetTranslation(matrix,
        static_cast<float>(cameraPosition[race][0]),
        static_cast<float>(cameraPosition[race][1]),
        static_cast<float>(cameraPosition[race][2]));
    AbyssEngine::AEMath::MatrixSetRotation(matrix, 0.0f, cameraYaw[race], 0.0f);
    Globals::Canvas->CameraSetLocal(Globals::Canvas->CameraGetCurrent(), matrix);
    if (this->cameraEase != 0) {
        this->cameraEase->SetRange(matrix, matrix);
    }
    this->introDone = 1;
    this->headBobPhase = 0;
    this->savedPostEffect = static_cast<AbyssEngine::Engine *>(
        static_cast<AbyssEngine::ApplicationManager *>(
            Globals::appManager)->GetEngine())->IsPostEffectActivated();
    this->initialized = 1;
    return 0;
}

SpaceLounge::SpaceLounge() {
    this->field_0x8c = 0;
    this->field_0x90 = 0;
    this->field_0x94 = 0;
    this->field_0x98 = 0;
    this->silhouettePos.x = 0;
    this->silhouettePos.y = 0;
    this->silhouettePos.z = 0;
    this->field_0x9c = 0;
    this->field_0xa0 = 0;
    this->baseMatrix = AbyssEngine::AEMath::Matrix();

    this->initialized = 0;
    this->starMap = 0;
    this->choiceWindow = 0;
    this->listWindow = 0;
    this->mapVisible = 0;
    this->singleOffer = 0;
    this->agentVisited = 0;
    this->buttons = 0;
    this->headEase = 0;
    this->listVisible = 0;
    this->mode = 0;
    this->field_0x18 = 0;
    this->cameraEase = 0;
    this->selectedAgent = 0;
    this->agents = 0;
    this->agentTexts = 0;
    this->chatScroll = 0;
    this->silhouetteGrid = 0;
    this->agentImageParts = 0;
    this->agentRects = 0;
    this->cutScene = 0;

    this->init();

    Array<Agent *> *agents = this->agents;
    if (agents != 0) {
        for (unsigned i = 0; i < agents->size(); ++i) {
            Agent *agent = (*agents)[i];
            int offer = agent->getOffer();
            if ((offer == 6 || offer == 0) && agent->getMission() != nullptr) {
                Mission *mission = agent->getMission();
                if (mission->getType() == 12 && agent->hasAcceptedOffer()) {
                    ArrayRemove(agent, *this->agents);
                    this->init();
                    break;
                }
            }
            agents = this->agents;
        }
    }

    if (this->cutScene == 0) {
        this->cutScene = new CutScene(4);
    }
    while (this->cutScene->isInitialized() == 0) {
        this->cutScene->initialize();
    }

    Array<KIPlayer *> *enemies = this->cutScene->level->getEnemies();
    if (this->agents != nullptr) {
        for (unsigned i = 0; i < this->agents->size(); ++i) {
            KIPlayer *agentObject = (*enemies)[i];
            Vector position = agentObject->getPosition();
            PaintCanvas *canvas = Globals::Canvas;
            const Matrix *local = static_cast<const Matrix *>(
                canvas->CameraGetLocal(canvas->CameraGetCurrent()));
            Matrix facing = AbyssEngine::AEMath::MatrixGetLookAt(
                AbyssEngine::AEMath::MatrixGetPosition(*local), position,
                AbyssEngine::AEMath::MatrixGetUp(*local));
            agentObject->geometry->setMatrix(facing);
            agentObject->setPosition(position);

            KIPlayer *silhouetteObject = (*enemies)[this->agents->size() + i];
            silhouetteObject->geometry->setMatrix(facing);
            const Vector direction = AbyssEngine::AEMath::MatrixGetDir(facing);
            position.x -= direction.x * 100.0f;
            position.z -= direction.z * 100.0f;
            silhouetteObject->setPosition(position);
        }
    }

    // ARM camera position/yaw tables at 0x206764/0x1fb8c8 and 0x206794/0x1fb8d8.
    static const int startPosition[4][3] = {
        {-1315, 1160, -2728}, {1484, 2980, -1626},
        {-2472, 734, -3000}, {-2598, 400, 1813},
    };
    static const float startYaw[4] = {0.03f, 0.03f, 0.21f, 1.87f};
    static const int endPosition[4][3] = {
        {-1800, 620, 1805}, {607, 610, 2362},
        {136, 444, 1122}, {2222, 450, 1227},
    };
    static const float endYaw[4] = {0.04f, 0.43f, 0.60f, 0.46f};
    const int race = Status::gStatus->getSystem()->getRace();
    Matrix from;
    AbyssEngine::AEMath::MatrixSetTranslation(from,
        static_cast<float>(startPosition[race][0]),
        static_cast<float>(startPosition[race][1]),
        static_cast<float>(startPosition[race][2]));
    AbyssEngine::AEMath::MatrixSetRotation(from, 0.0f, startYaw[race], 0.0f);
    Matrix to;
    AbyssEngine::AEMath::MatrixSetTranslation(to,
        static_cast<float>(endPosition[race][0]),
        static_cast<float>(endPosition[race][1]),
        static_cast<float>(endPosition[race][2]));
    AbyssEngine::AEMath::MatrixSetRotation(to, 0.0f, endYaw[race], 0.0f);
    this->cameraEase = new AbyssEngine::EaseInOutMatrix(from, to, 3000);
    Globals::Canvas->CameraSetLocal(Globals::Canvas->CameraGetCurrent(), from);
    this->initialized = 1;
    this->introDone = 0;
}

void SpaceLounge::startChat() {
    String title;
    String body;
    String left;
    String right;

    if (this->agents == 0 || this->selectedAgent < 0) {
        return;
    }

    void *agent = (*this->agents)[this->selectedAgent];
    int offer = ((Agent *) (agent))->getOffer();
    void *mission = ((Agent *) (agent))->getMission();
    void *texts = *(void **) &SpaceLounge_start_text_slot;

    int titleId = 0x100;
    int bodyId = 0x101;
    if (mission != 0) {
        titleId = 0x120;
        bodyId = 0x121;
    } else if (offer == 1) {
        titleId = 0x130;
        bodyId = 0x131;
    } else if (offer == 6) {
        titleId = 0x132;
        bodyId = 0x133;
    }

    title.Set(((String *) ((GameText *) (*(void **) texts))->getText(titleId))->data);
    body.Set(((String *) ((GameText *) (*(void **) texts))->getText(bodyId))->data);
    left.Set(((String *) ((GameText *) (*(void **) texts))->getText(0x10))->data);
    right.Set(((String *) ((GameText *) (*(void **) texts))->getText(0x11))->data);

    this->choiceWindow->set(title, body, true);
    if (this->choiceWindow->leftButton != nullptr)
        this->choiceWindow->leftButton->setText(left);
    if (this->choiceWindow->rightButton != nullptr)
        this->choiceWindow->rightButton->setText(right);

    if (((Agent *) (agent))->isKnown() == 0 && ((Agent *) (agent))->isStoryAgent() == 0) {
        if (((Agent *) (agent))->eventCount <= 0)
            ((Agent *) (agent))->eventCount = 1;
    }
    this->getSoundId(static_cast<Agent *>(agent));

    this->choiceVisible = 1;
    this->chatActive = 1;
    this->mode = 1;

    if (offer == 1) {
        this->mode = 3;
    } else if (((Agent *) (agent))->isGenericAgent() != 0) {
        ((Agent *) (agent))->setEvent(1);
    }
}

SpaceLounge::~SpaceLounge() {
    delete this->choiceWindow;
    this->choiceWindow = 0;

    delete this->cutScene;
    this->cutScene = 0;

    delete[] this->agentVisited;
    this->agentVisited = 0;

    {
        Array<String *> *strings = this->agentTexts;
        if (strings != 0) {
            ArrayReleaseClasses(*strings);
            delete strings;
        }
    }
    this->agentTexts = 0;

    {
        Array<TouchButton *> *buttons = this->buttons;
        if (buttons != 0) {
            ArrayReleaseClasses(*buttons);
            delete buttons;
        }
    }
    this->buttons = 0;

    if (this->agentImageParts != nullptr) {
        ArrayReleaseClasses(*this->agentImageParts);
        delete this->agentImageParts;
    }
    this->agentImageParts = 0;

    {
        Array<Array<ImagePart *> *> *grid = this->silhouetteGrid;
        if (grid != 0) {
            for (unsigned i = 0; i < grid->size(); ++i) {
                Array<ImagePart *> *inner = (*grid)[i];
                if (inner != 0) {
                    ArrayReleaseClasses(*inner);
                    delete inner;
                }
                (*grid)[i] = 0;
            }
            ArrayReleaseClasses(*grid);
            delete grid;
        }
    }
    this->silhouetteGrid = 0;

    {
        Array<Vector *> *rects = this->agentRects;
        if (rects != 0) {
            ArrayReleaseClasses(*rects);
            delete rects;
        }
    }
    this->agentRects = 0;

    delete this->cameraEase;
    this->cameraEase = 0;

    delete this->headEase;
    this->headEase = 0;
}

void SpaceLounge::draw() {
    if (this->listVisible != 0) {
        if (this->listWindow->shows3DShip() != 0) {
            Globals::Canvas->SetColor(0, 0, 0, 255);
            PaintCanvas *canvas = Globals::Canvas;
            int width = canvas->GetWidth();
            int height = canvas->GetHeight();
            canvas->FillRectangle(0, 0, width, height);
            Globals::Canvas->SetColor(0xffffffffu);
        }
        this->listWindow->draw();
        Globals::layout->drawFooter();
        return;
    }

    if (this->mapVisible != 0) {
        this->starMap->draw();
        return;
    }

    Globals::layout->drawHeader(String(*Globals::gameText->getText(398), false));
    drawLounge();

    if ((this->mode & 0xfffffffe) == 2) {
        Globals::layout->drawFooterNoBackButton();
    } else {
        Globals::layout->drawFooter();
    }

    if (this->chatActive != 0 || this->popupActive != 0 || this->choiceVisible != 0) {
        this->choiceWindow->draw();
    }
}

void SpaceLounge::update(int dt) {
    if (dt > 50) {
        dt = 50;
    }
    if (!this->initialized) {
        return;
    }
    if (this->mapVisible) {
        this->starMap->update(dt);
        return;
    }
    if (this->listVisible) {
        this->listWindow->update(dt);
        return;
    }

    AbyssEngine::AERandom *random = static_cast<AbyssEngine::AERandom *>(Globals::rnd);
    if (!this->cameraAnimating) {
        this->cameraEase->Increase(static_cast<float>(dt));
    }

    Matrix camera;
    if (this->introDone || this->cameraEase->GetMaxValue() == this->cameraEase->GetValue()) {
        float step = static_cast<float>(dt) * 0.0025f;
        if (step > 0.12f) step = 0.12f;
        if (step < 0.05f) step = 0.05f;
        this->introDone = 1;
        this->headBobPhase += step;
        const float wave = AbyssEngine::AEMath::Sinf(this->headBobPhase);
        float amplitude = static_cast<float>(this->headBobSteps) * 0.85f;
        if (amplitude > 3.5f) amplitude = 3.5f;

        if (this->cameraAnimating) {
            const float value = this->headEase->GetValue();
            const float maxValue = this->headEase->GetMaxValue();
            float distance = value - maxValue;
            if (distance < 0.0f) distance = -distance;
            int amount = this->headBobSteps;
            if (distance < 0.25f) {
                const float next = static_cast<float>(
                    5 - random->nextInt(10));
                this->headBobReverse = value > next;
                this->headEase->SetRange(value, next);
                amount = random->nextInt(4) + 1;
                this->headBobSteps = amount;
            }
            if (this->headBobReverse) amount = -amount;
            this->headEase->Increase(static_cast<float>(amount * dt));
        } else {
            PaintCanvas *canvas = Globals::Canvas;
            this->baseMatrix = *static_cast<const Matrix *>(
                canvas->CameraGetLocal(canvas->CameraGetCurrent()));
            this->cameraAnimating = 1;
            (void) random->nextInt(10);
            this->headBobReverse = 0;
            if (this->headEase) {
                this->headEase->SetRange(0.0f, 5.0f);
            } else {
                this->headEase = new AbyssEngine::EaseInOut(0.0f, 5.0f);
            }
            this->headBobSteps = 2;
        }

        camera = Matrix();
        AbyssEngine::AEMath::MatrixSetRotation(
            camera, 0.0f, this->headEase->GetValue() / 35.0f, 0.0f);
        AbyssEngine::AEMath::MatrixSetTranslation(
            camera, 0.0f, wave * amplitude, 0.0f);
        camera *= this->baseMatrix;
    } else {
        this->cameraAnimating = 0;
        camera = this->cameraEase->GetValue();
    }

    Globals::Canvas->CameraSetLocal(Globals::Canvas->CameraGetCurrent(), camera);
    this->updateScreenPositions();
    if (this->mode != 0) {
        this->scrollWindow->update(dt);
    }
    this->cutScene->update();

    if (!this->touchDown) {
        return;
    }
    const int x = this->touchX;
    const int y = this->touchY;
    CutScene *cutScene = this->cutScene;
    if (y < Globals::layout->field_0xc) {
        if (x <= 149) {
            cutScene->vec8.y -= 10.0f;
        } else if (x > Globals::w - 150) {
            cutScene->vec8.y += 10.0f;
        }
    } else if (y >= Globals::h - 2 * Globals::layout->field_0x10) {
        if (x <= 69) {
            cutScene->vec8.x -= static_cast<float>(dt);
        } else if (x > Globals::w - 70) {
            cutScene->vec8.x += static_cast<float>(dt);
        }
    } else if (x <= 69) {
        cutScene->cameraRotX += 0.01f;
    } else if (x > Globals::w - 70) {
        cutScene->cameraRotX -= 0.01f;
    } else if (x >= 101 && x < Globals::w - 100) {
        if (y < Globals::h / 2) {
            cutScene->vec8.z -= static_cast<float>(dt);
        } else {
            cutScene->vec8.z += static_cast<float>(dt);
        }
    }
}

void SpaceLounge::refresh() {
}
