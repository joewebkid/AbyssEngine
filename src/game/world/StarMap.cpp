#include "game/world/StarMap.h"
#include "game/world/SystemPathFinder.h"
#include "game/world/Level.h"
#include "engine/file/FileRead.h"
#include "game/ship/Ship.h"
#include "engine/core/AERandom.h"
#include "game/world/Galaxy.h"
#include "engine/render/AEGeometry.h"
#include "game/ui/ChoiceWindow.h"
#include "engine/math/EaseInOut.h"
#include "engine/audio/FModSound.h"


#include "game/mission/Item.h"
#include "game/core/Globals.h"



#include "engine/math/Transform.h"
#include "engine/render/PaintCanvas.h"

#include "game/mission/Achievements.h"

#include "engine/core/ApplicationManager.h"

#include "engine/render/Engine.h"
#include "engine/core/GameText.h"
#include "game/ui/Layout.h"
#include "game/mission/Mission.h"
#include "game/world/Station.h"
#include "game/world/SolarSystem.h"
#include "game/world/Wanted.h"
#include "game/mission/Status.h"
#include "game/core/String.h"
#include "game/ui/TouchButton.h"



uint8_t StarMap::missionChanged() {
    return this->missionChangedFlag;
}

void StarMap::render() {
    Globals::Canvas->SetColor((unsigned int) (0xffffffffu));
    this->bgLayer0->render();
    this->bgLayer1->render();
    this->bgLayer2->render();
    this->systemRoot->render();
    Globals::Canvas->End3d();
    Globals::Canvas->Begin3d();
    if (this->starSystemRoot != 0) {
        Globals::Canvas->SetTexture((unsigned int) (this->planetTexture), (unsigned int) (0xffffffffu));
        Globals::Canvas->SetBlendMode(AbyssEngine::BlendMode_dummy);
        this->planetGeom->render();
        this->starSystemRoot->render();
    }
    if (this->markerGeom != 0) {
        this->markerGeom->render();
    }
}

void StarMap::renderBG() {
}

bool StarMap::isInPlanetMode() {
    return this->mode == 3;
}

void StarMap::askForJumpIntoAlienWorld() {
    void *window = (void *) this->choiceWindow;
    this->alienJumpPending = 1;
    String *text = (String *) ((GameText *) (Globals::gameText))->getText(0x1a6);
    ((ChoiceWindow *) (window))->set(*text, true);
    this->choiceVisible = 1;
}

void StarMap::setJumpMapMode(bool enabled, bool value) {
    this->jumpMapModeB = (uint8_t) value;
    this->jumpMapModeA = (uint8_t) enabled;
}

void StarMap::setStart(int start, int target) {
    this->routeStart = target;
    this->routeTarget = start;
    delete this->systemPath;
    this->systemPath = nullptr;
    this->systemPath =
            this->pathFinder->getSystemPath(this->systems, start, this->targetSystem);
}

StarMap::~StarMap() {
    if (this->systemPositions != 0) {
        ArrayReleaseClasses(*this->systemPositions); delete this->systemPositions;
        this->systemPositions = 0;
    }

    if (this->stationPositions != 0) {
        ArrayReleaseClasses(*this->stationPositions); delete this->stationPositions;
        this->stationPositions = 0;
    }

    delete this->planetGeom;
    this->planetGeom = (AEGeometry *) 0;

    delete this->bgLayer0;
    this->bgLayer0 = (AEGeometry *) 0;

    delete this->bgLayer1;
    this->bgLayer1 = (AEGeometry *) 0;

    delete this->bgLayer2;
    this->bgLayer2 = (AEGeometry *) 0;

    delete this->easeX;
    this->easeX = (AbyssEngine::EaseInOut *) 0;

    delete this->easeY;
    this->easeY = (AbyssEngine::EaseInOut *) 0;

    delete this->easeZ;
    this->easeZ = (AbyssEngine::EaseInOut *) 0;

    delete this->markerGeom;
    this->markerGeom = (AEGeometry *) 0;

    if (this->pathFinder != 0) {
        delete this->pathFinder;
    }
    this->pathFinder = (SystemPathFinder *) 0;
}

void StarMap::draw() {
    int mode = this->mode;
    int alpha = mode == 0 ? 0xff : 0;
    this->alpha = alpha;
    if (this->transitionIn != 0 || this->transitionOut != 0) {
        float v = this->easeZ->GetValue();
        float min = this->easeZ->GetMinValue();
        float max = this->easeZ->GetMaxValue();
        float t = (v - min) / (max - this->easeZ->GetMinValue());
        if (this->transitionIn != 0) {
            t = 1.0f - t;
        }
        mode = this->mode;
        alpha = (int)(t * 255.0f);
        this->alpha = alpha;
    }

    if (mode != 3 || this->transitionIn != 0 || this->transitionOut != 0) {
        Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff), (unsigned char) (0xff),
                          (unsigned char) (alpha < 34 ? alpha : 34));
        Array<bool> *visibility;
        for (uint32_t i = 0; i < this->systemGeoms->size(); i++) {
            visibility = Globals::status->getSystemVisibilities();
            if (visibility == nullptr || i >= visibility->size() || visibility->data()[i] == 0 ||
                (this->autoMode != 0 && this->pulseSystem == (int)i && this->autoTimer < 4000)) {
                continue;
            }
            Array<int> *routes = (Array<int> *) this->systems->data()[i]->getRoutes();
            if (routes != 0) {
                this->scratchVector = Vector(*this->systemPositions->data()[i]);
                for (uint32_t j = 0; j < routes->size(); j++) {
                    uint32_t to = (uint32_t) routes->data()[j];
                    if (to < visibility->size() && visibility->data()[to] != 0 && to < this->systemPositions->size() &&
                        (this->autoMode == 0 || this->pulseSystem != (int)to || this->autoTimer >= 4000)) {
                        this->scratchVector2 = Vector(*this->systemPositions->data()[to]);
                        if (!(this->scratchVector.z < 0.0f && this->scratchVector2.z < 0.0f)) {
                            Globals::Canvas->DrawLine((int) this->scratchVector.x, (int) this->scratchVector.y,
                                              (int) this->scratchVector2.x, (int) this->scratchVector2.y);
                        }
                    }
                }
            }
        }
        Globals::Canvas->SetColor(255, 255, 255, (unsigned char)this->alpha);
        this->scratchVector = Vector(*this->systemPositions->data()[Globals::status->getSystem()->getIndex()]);
        Array<int> *currentRoutes = reinterpret_cast<Array<int> *>(Globals::status->getSystem()->getRoutes());
        for (uint32_t i = 0; i < this->systems->size(); i++) {
            visibility = Globals::status->getSystemVisibilities();
            if (visibility != nullptr && i < this->systemPositions->size() && i < visibility->size() && visibility->data()[i]) {
                this->scratchVector2 = Vector(*this->systemPositions->data()[i]);
                if (currentRoutes && !(this->scratchVector.z < 0.0f && this->scratchVector2.z < 0.0f) && !this->jumpMapModeB) {
                    for (uint32_t j = 0; j < currentRoutes->size(); ++j) {
                        if (currentRoutes->data()[j] == (int)i) {
                            float phase = (float)(101 - (Globals::status->getPlayingTime() / 10) % 99) / 100.0f;
                            int routeAlpha = (int)(phase * 255.0f);
                            Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(routeAlpha < this->alpha ? routeAlpha : this->alpha));
                            Globals::Canvas->DrawLine((int)this->scratchVector2.x, (int)this->scratchVector2.y,
                                (int)(this->scratchVector.x + phase * (this->scratchVector2.x - this->scratchVector.x)),
                                (int)(this->scratchVector.y + phase * (this->scratchVector2.y - this->scratchVector.y)));
                            break;
                        }
                    }
                }
                if (!this->transitionIn && !this->transitionOut) drawOnScreenInfo((int)i, false);
            }
        }
        // Android 2.0.16, 0xc90f4: animate the recovered path one edge at a time.
        if (!this->mode && !this->transitionIn && !this->transitionOut && this->systemPath) {
            float phase = (float)(101 - (Globals::status->getPlayingTime() / 10) % 99) / 100.0f;
            if (phase > this->field_0xec) {
                ++this->field_0xe8;
                if ((uint32_t)this->field_0xe8 >= this->systemPath->size()) this->field_0xe8 = 0;
            }
            this->field_0xec = phase;
            if ((uint32_t)this->field_0xe8 == this->systemPath->size() - 1)
                Globals::Canvas->SetColor(255, 255, 0, (unsigned char)(int)(phase * 255.0f));
            else Globals::Canvas->SetColor(0xffff00ffu);
            for (int i = 0; i < this->field_0xe8; ++i) {
                this->scratchVector = Vector(*this->systemPositions->data()[this->systemPath->data()[i]]);
                this->scratchVector2 = Vector(*this->systemPositions->data()[this->systemPath->data()[i + 1]]);
                Globals::Canvas->DrawLine((int)this->scratchVector2.x, (int)this->scratchVector2.y,
                                             (int)this->scratchVector.x, (int)this->scratchVector.y);
            }
            if ((uint32_t)this->field_0xe8 < this->systemPath->size() - 1) {
                this->scratchVector = Vector(*this->systemPositions->data()[this->systemPath->data()[this->field_0xe8]]);
                this->scratchVector2 = Vector(*this->systemPositions->data()[this->systemPath->data()[this->field_0xe8 + 1]]);
                Globals::Canvas->DrawLine((int)this->scratchVector2.x, (int)this->scratchVector2.y,
                    (int)(this->scratchVector.x + phase * (this->scratchVector2.x - this->scratchVector.x)),
                    (int)(this->scratchVector.y + phase * (this->scratchVector2.y - this->scratchVector.y)));
            }
        }
    }

    if (this->selectedSystem >= 0 &&
        (this->autoMode == 0 || this->autoTimer > 3999)) {
        drawOnScreenInfo(this->selectedSystem, false);
    }

    if (this->starSystemRoot != 0 && this->stations != 0) {
        SolarSystem *system = this->systems->data()[this->selectedSystem];
        if (system->hasNoOwner() == 0) {
            Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff), (unsigned char) (0xff),
                              (unsigned char) (this->alpha ^ 0xff));
            Layout *drawLayout = (Layout *) Globals::layout;
            Globals::Canvas->DrawImage2D((unsigned int) (this->systemNameImage), drawLayout->field_0x2c_rowHeight,
                                 drawLayout->field_0xc_leftMargin +
                                 drawLayout->field_0x2c_rowHeight);
            int rowHeight = Globals::layout->field_0x2c_rowHeight;
            int imageWidth = Globals::Canvas->GetImage2DWidth(this->systemNameImage);
            Globals::Canvas->SetColor(255, 128, 0, (unsigned char)(this->alpha ^ 255));
            int textX = imageWidth + rowHeight * 2;
            Globals::Canvas->DrawString(Globals::font,
                this->systems->data()[this->selectedSystem]->getName(), textX,
                Globals::layout->field_0x2c_rowHeight + Globals::layout->field_0xc_leftMargin + 2, false);
            Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(this->alpha ^ 255));
            Globals::Canvas->DrawString(Globals::font,
                *Globals::gameText->getText(this->systems->data()[this->selectedSystem]->getRace() + 406), textX,
                Globals::layout->field_0x2c_rowHeight + Globals::layout->field_0xc_leftMargin + Globals::layout->field_0x4 + 2, false);
            static const int securityColors[4][3] = {{255,42,0},{255,108,0},{237,237,0},{0,237,0}};
            int security = this->systems->data()[this->selectedSystem]->getSecurityLevel();
            Globals::Canvas->SetColor(securityColors[security][0], securityColors[security][1], securityColors[security][2], (unsigned char)(this->alpha ^ 255));
            Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(security + 402), textX,
                Globals::layout->field_0x2c_rowHeight + Globals::layout->field_0xc_leftMargin + Globals::layout->field_0x4 * 2 + 2, false);
        }
        for (uint32_t i = 0; i < this->stations->size(); i++) {
            if (i != (uint32_t) this->selectedStation) {
                drawOnScreenInfo((int) i, true);
            }
        }
        if (this->selectedStation >= 0) {
            drawOnScreenInfo(this->selectedStation, true);
        }
    }

    if (this->showKey != 0) {
        drawKey();
    }
    Globals::layout->drawHeader(String(*Globals::gameText->getText(177), false));
    ((Layout *) (Globals::layout))->drawEmptyFooter(1);
    if ((Globals::status->getShip()->hasJumpDriveIntegrated() || this->jumpMapModeB) && this->jumpCost > 0 &&
        this->selectedSystem >= 0 && this->selectedSystem != Globals::status->getSystem()->getIndex()) {
        String cost = *Globals::gameText->getText(578) + String(" ", false) +
                      String(String(this->jumpCost) + String(" / ", false) + String(this->cargoAmount), false);
        if (this->jumpCost > this->cargoAmount) Globals::Canvas->SetColor(0xff2a00ffu);
        unsigned int font = Globals::font;
        PaintCanvas *canvas = Globals::Canvas;
        int *layoutWords = reinterpret_cast<int *>(Globals::layout);
        int footerX = layoutWords[183];
        int footerWidth = layoutWords[185];
        int textWidth = Globals::Canvas->GetTextWidth(Globals::font, cost);
        layoutWords = reinterpret_cast<int *>(Globals::layout);
        canvas->DrawString(font, cost, footerX + footerWidth / 2 - textWidth / 2,
            layoutWords[184] + layoutWords[186] - layoutWords[5], false);
        Globals::Canvas->SetColor(0xffffffffu);
    }
    this->backButton->draw();
    if (this->choiceVisible != 0) {
        this->choiceWindow->draw();
    }
}

void StarMap::depart(bool jump) {
    int selected = this->selectedStation;
    if (selected < 0) {
        return;
    }

    if (this->jumpMapModeA != 0) {
        Globals::status->departStation(this->stations->data()[selected]);
        Level::programmedStation = nullptr;
        Level::setInitStreamOut();
        Globals::status->jumpgateUsed();
        if (jump && this->jumpMapModeB != 0) {
            int toSystem = this->stations->data()[this->selectedStation]->getSystem();
            int current = Globals::status->getSystem()->getIndex();
            Level::doInstantJump = static_cast<unsigned char>(toSystem != current);
            if (toSystem != current) {
                Level::energyCellsForNextJump = this->jumpCost;
            }
        } else {
            Level::doInstantJump = 0;
        }
    } else {
        if (Globals::status->getCurrentCampaignMission() == 3) {
            goto cleanup;
        }

        Globals::status->field_5c = -1;
        Globals::status->field_60 = -1;
        Globals::status->field_64 = -1;
        Globals::status->field_68 = -1;
        Globals::status->departStation((Station *) Globals::status->getStation());

        Station *target = this->stations->data()[this->selectedStation];
        if (target->equals((Station *) Globals::status->getStation()) == 0) {
            Level::programmedStation = this->stations->data()[this->selectedStation];
        }

        if (jump) {
            void *ship = Globals::status->getShip();
            if (((Ship *) (ship))->hasVolatileGoods() != 0) {
                goto no_jump;
            }
            if (((Ship *) (Globals::status->getShip()))->hasJumpDriveIntegrated() == 0 && this->jumpMapModeB == 0) {
                goto no_jump;
            }
            int toSystem = static_cast<Station *>(Level::programmedStation)->getSystem();
            int current = Globals::status->getSystem()->getIndex();
            Level::doInstantJump = static_cast<unsigned char>(toSystem != current);
            if (toSystem != current) {
                Level::energyCellsForNextJump = this->jumpCost;
            }
        } else {
        no_jump:
            Level::doInstantJump = 0;
        }
        static_cast<Achievements *>(Globals::achievements)->resetNewMedals();
    }

cleanup: {
        for (uint32_t i = 0; i < this->stations->size(); i++) {
            if (i != (uint32_t) this->selectedStation) {
                delete this->stations->data()[i];
                this->stations->data()[i] = 0;
            }
        }
        delete this->stations;
        this->stations = (Array<Station *> *) 0;
    }
    ((FModSound *) (Globals::sound))->stop(0x66);
    Globals::switch_to_target_setting = 1;

    static_cast<ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
}

static inline float absf_end(float v) {
    return v > 0.0f ? v : -v;
}

int StarMap::OnTouchEnd(int x, int y) {

    if (this->choiceVisible == 0) {
        if (this->transitionIn != 0 || this->transitionOut != 0) {
            return 0;
        }
        void *layout = Globals::layout;
        if (((Layout *) layout)->layoutVisibleFlag == 0 && ((Layout *) (layout))->OnTouchEnd(x, y) != 0) {
            if (this->mode == 3 && this->isGalaxyMode != 0) {
                this->transitionOut = 1;
                this->momentumFactor = 0.0f;
                this->velocityX = 0.0f;
                this->velocityY = 0.0f;
                AbyssEngine::EaseInOut *ease = this->easeX;
                float maximum = ease->GetMaxValue();
                float minimum = this->easeX->GetMinValue();
                ease->SetRange(maximum, minimum);
                ease = this->easeY;
                maximum = ease->GetMaxValue();
                minimum = this->easeY->GetMinValue();
                ease->SetRange(maximum, minimum);
                ease = this->easeZ;
                maximum = ease->GetMaxValue();
                minimum = this->easeZ->GetMinValue();
                ease->SetRange(maximum, minimum);
                ((FModSound *) (Globals::sound))->play(0x6b, 0, 0, 0.0f);
                return 0;
            }

            Globals::Canvas->CameraSetCurrent((unsigned int) (this->prevCamera));
            ((FModSound *) (Globals::sound))->stop(0x66);
            return 1;
        }
        if (this->pad_0xa8_a != 0 && this->pathAnim != 0) {
            return 0;
        }
        if (this->backButton->OnTouchEnd(x, y) != 0) {
            this->showKey ^= 1;
        }
        if (this->dragging != 0) {
            float dx = this->touchDeltaX;
            float dy = this->touchDeltaY;
            this->momentumFactor = 0.9f;
            this->dragging = 0;
            this->velocityX = absf_end(dx) > 3.0f ? dx : 0.0f;
            this->velocityY = absf_end(dy) > 3.0f ? dy : 0.0f;
            if (this->mode != 0) {
                if (this->selectedStation >= 0) {
                    if (this->lastSelectedStation == this->selectedStation) {
                        if (!this->pad_0xa8_a) {
                            int stationIndex = this->stations->data()[this->selectedStation]->getIndex();
                            int currentIndex = Globals::status->getStation()->getIndex();
                            ChoiceWindow *window = this->choiceWindow;
                            if (stationIndex == currentIndex) {
                                if (!window) {
                                    window = new ChoiceWindow();
                                    this->choiceWindow = window;
                                }
                                window->set(*Globals::gameText->getText(419));
                            } else {
                                if (!window) this->choiceWindow = new ChoiceWindow();
                                int warningText = -1;
                                if (Globals::status->getCurrentCampaignMission() == 24 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    (!Globals::status->getShip()->getFirstEquipmentOfSort(13) ||
                                     !Globals::status->getShip()->getFirstEquipmentOfSort(17))) {
                                    warningText = 532;
                                } else if (Globals::status->getCurrentCampaignMission() == 135 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    !Globals::status->getShip()->getFirstEquipmentOfSort(19)) {
                                    warningText = 3213;
                                } else if ((Globals::status->getCurrentCampaignMission() == 91 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    Globals::status->getShip()->getMaxPassengers() <= 9) ||
                                    (Globals::status->getCurrentCampaignMission() == 94 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    !Globals::status->getShip()->getMaxPassengers())) {
                                    warningText = 3214;
                                } else if (Globals::status->getCurrentCampaignMission() == 105 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    !Globals::status->getShip()->hasEquipment(206, 1)) {
                                    warningText = 3217;
                                } else if (Globals::status->getCurrentCampaignMission() == 139 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    Globals::status->getShip()->getIndex() != 42) {
                                    // Android rodata 0x1fde40, not a PC ship classification.
                                    static const int shipClasses[64] = {3,0,8,3,2,0,3,0,9,1,0,8,2,0,0,0,2,0,2,3,3,2,0,8,8,8,0,0,0,8,3,2,8,0,0,2,0,0,0,1,0,1,1,2,1,3,3,3,3,1,1,0,8,1,1,0,3,2,0,0,8,1,3,1};
                                    if (shipClasses[Globals::status->getShip()->getIndex()] != 1 ||
                                        !Globals::status->getShip()->hasEquipment(190, 1)) warningText = 3215;
                                }
                                if (warningText < 0 && Globals::status->getCurrentCampaignMission() == 142 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    (!Globals::status->getShip()->getFirstEquipmentOfSort(33) ||
                                     !Globals::status->getShip()->hasEquipment(197, 15) ||
                                     !Globals::status->getShip()->getFirstEquipmentOfSort(35))) {
                                    warningText = 3216;
                                }
                                if (warningText < 0 && Globals::status->getCurrentCampaignMission() == 142 &&
                                    this->stations->data()[this->selectedStation]->getIndex() ==
                                        reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getTargetStation() &&
                                    !Globals::status->getShip()->getFreeSpace()) {
                                    warningText = 3218;
                                }
                                if (warningText >= 0) {
                                    this->choiceWindow->set(*Globals::gameText->getText(warningText));
                                    this->suppressNextClose = 1;
                                } else if (this->jumpMapModeB && this->cargoAmount < this->jumpCost &&
                                           !Globals::status->getShip()->hasVolatileGoods()) {
                                    if (this->jumpCost != 1) this->choiceWindow->set(*Globals::gameText->getText(579));
                                    else this->choiceWindow->set(*Globals::gameText->getText(582), true);
                                } else if (this->jumpMapModeB && this->noRoute &&
                                           this->cargoAmount < 2 * this->jumpCost &&
                                           !Globals::status->getShip()->hasVolatileGoods()) {
                                    this->choiceWindow->set(*Globals::gameText->getText(581), true);
                                } else {
                                    this->choiceWindow->set(*Globals::gameText->getText(574) + String(": ", false) +
                                        this->stations->data()[this->selectedStation]->getName() + String("\n", false) +
                                        *Globals::gameText->getText(421), true);
                                }
                            }
                            this->choiceVisible = 1;
                        }
                    } else {
                        Globals::sound->play(0x69, 0, 0, 0.0f);
                        this->stationCenterAnim = 1;
                    }
                }
                this->velocityX = absf_end(this->touchDeltaX) > 150.0f ? this->touchDeltaX : 0.0f;
                this->velocityY = absf_end(this->touchDeltaY) > 150.0f ? this->touchDeltaY : 0.0f;
            } else {
                if (this->selectedSystem >= 0 &&
                    this->autoMode == 0 &&
                    this->pad_0xa8_a == 0 &&
                    this->lastSelectedSystem == this->selectedSystem) {
                    if (this->jumpMapModeB == 0 &&
                        Globals::status->getSystem()->systemIsInSystemRoutes(
                                this->systems->data()[this->selectedSystem]->getIndex()) ==
                        0) {
                        this->choiceWindow->set(*(String *) ((GameText *) (Globals::gameText))->getText(0x1a4), false);
                        this->choiceVisible = 1;
                        return 0;
                    }
                    if (this->jumpMapModeB && Globals::status->getShip()->hasVolatileGoods() &&
                        !Globals::status->getSystem()->systemIsInSystemRoutes(this->systems->data()[this->selectedSystem]->getIndex())) {
                        this->choiceWindow->set(*Globals::gameText->getText(612), false);
                        this->choiceVisible = 1;
                        return 0;
                    }
                    ((FModSound *) (Globals::sound))->play(0x6a, 0, 0, 0.0f);
                    initStarSystem();
                    this->transitionIn = 1;
                    this->scratchVector = this->systemGeoms->data()[this->selectedSystem]->getPosition();
                    this->scratchVector.z -= 500.0f;
                    PaintCanvas *canvas = Globals::Canvas;
                    this->scratchVector2 = AbyssEngine::AEMath::MatrixGetPosition(*static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent())));
                    this->easeX->SetRange(this->scratchVector2.x, this->scratchVector.x);
                    this->easeY->SetRange(this->scratchVector2.y, this->scratchVector.y);
                    this->easeZ->SetRange(this->scratchVector2.z, this->scratchVector.z);
                    this->momentumFactor = 0.0f;
                    this->velocityX = 0.0f;
                    this->velocityY = 0.0f;
                    return 0;
                } else if (this->selectedSystem >= 0) {
                    this->lastSelectedSystem = -1;
                    this->pathAnim = 1;
                }
            }
        }
        if (Globals::layout->helpPressed()) {
            Globals::layout->initHelpWindow(String(*Globals::gameText->getText(this->mode == 0 ? 628 : 631), false));
        }
        return 0;
    }

    int result = this->choiceWindow->OnTouchEnd(x, y);
    if (result == 1) {
        this->choiceVisible = 0;
        this->alienJumpPending = 0;
        return 0;
    }
    if (result != 0) {
        return 0;
    }
    this->choiceVisible = 0;
    if (this->suppressNextClose != 0) {
        this->suppressNextClose = 0;
        return 0;
    }
    if (this->mode == 3) {
        Station *station = this->stations->data()[this->selectedStation];
        if (station->getIndex() == Globals::status->getStation()->getIndex()) {
            this->suppressNextClose = 0;
            return 0;
        }
    }
    if (this->alienJumpPending && this->mode == 0 && !this->pad_0xa8_a && !this->autoMode) {
        Level::programmedStation = Globals::status->playerStation;
        this->alienJumpPending = 0;
        this->exitRequested = 1;
        Level::doInstantJump = 1;
        Level::energyCellsForNextJump = this->jumpCost;
        Globals::Canvas->CameraSetCurrent(this->prevCamera);
        return 1;
    }
    if (!this->pad_0xa8_a && this->mode == 3) {
        uint8_t jumpMapMode = this->jumpMapModeB;
        if (jumpMapMode && this->cargoAmount < this->jumpCost) {
            if (this->jumpCost != 1) return 0;
            if (!this->jumpMapModeA) {
                depart(false);
                return 0;
            }
            Level::doInstantJump = 0;
            Level::programmedStation = this->stations->data()[this->selectedStation];
        } else {
            if (!this->jumpMapModeA) {
                depart(true);
                return 0;
            }
            Station *station = this->stations->data()[this->selectedStation];
            Level::programmedStation = station;
            if (jumpMapMode) {
                int difference = station->getSystem() - Globals::status->getSystem()->getIndex();
                Level::doInstantJump = difference != 0;
                if (difference) Level::energyCellsForNextJump = this->jumpCost;
            } else Level::doInstantJump = 0;
        }
        this->exitRequested = 1;
        Globals::Canvas->CameraSetCurrent(this->prevCamera);
        return 1;
    }
    return 0;
}

void StarMap::initLights() {
    void *engine = ApplicationManager::gAppManager->GetEngine();
    ((Engine *) (engine))->LightSetMaterialColorAmbient(0.5f, 0.5f, 0.5f);
    Globals::Canvas->FogEnable(0, static_cast<AbyssEngine::FogMode>(1));
}

static inline float absf_update(float v) {
    return v > 0.0f ? v : -v;
}

void StarMap::update(int dt) {
    Vector tmp;

    this->lastDt = dt;
    if (this->mode == 0 || this->transitionIn != 0 || this->transitionOut != 0) {
        for (uint32_t i = 0; i < this->systemPositions->size(); i++) {
            Vector position = this->systemGeoms->data()[i]->getPosition();
            int visible = Globals::Canvas->GetScreenPosition(position, *this->systemPositions->data()[i]);
            this->systemPositions->data()[i]->z = visible != 0 ? 1.0f : -1.0f;
        }
    }
    if (this->mode == 3 || this->transitionIn != 0 || this->transitionOut != 0) {
        for (uint32_t i = 0; i < this->stationPositions->size(); i++) {
            tmp = AbyssEngine::AEMath::MatrixGetPosition(this->starSystemRoot->getMatrix() * this->stationGeoms->data()[i + 1]->getMatrix());
            int visible = Globals::Canvas->GetScreenPosition(tmp, *this->stationPositions->data()[i]);
            this->stationPositions->data()[i]->z = visible != 0 ? 1.0f : -1.0f;
        }
    }

    if (this->markerGeom != 0) {
        float v = this->easeZ->GetValue();
        float min = this->easeZ->GetMinValue();
        float max = this->easeZ->GetMaxValue();
        float t = (v - min) / (max - this->easeZ->GetMinValue());
        if (this->transitionOut != 0) {
            t = 1.0f - t;
        } else if (this->transitionIn == 0 && this->mode == 0) {
            t = 0.0f;
        }
        float scale = (float)(t * -0.014 + 0.02);
        this->markerGeom->setScaling(scale, scale, scale);
        if (this->starSystemRoot != 0 && this->centeredStation >= 0) {
            Vector start = this->systemGeoms->data()[Globals::status->field_7c]->getPosition();
            Vector end = AbyssEngine::AEMath::MatrixGetPosition(this->starSystemRoot->getMatrix() *
                                                      this->stationGeoms->data()[this->centeredStation]->getMatrix());
            this->markerGeom->setPosition(start + t * (end - start));
        }
        PaintCanvas *canvas = Globals::Canvas;
        this->scratchVector = AbyssEngine::AEMath::MatrixGetPosition(*static_cast<Matrix *>(
            canvas->CameraGetLocal(canvas->CameraGetCurrent())));
        this->scratchVector -= this->markerGeom->getPosition();
        this->scratchVector = AbyssEngine::AEMath::VectorNormalize(this->scratchVector);
        this->scratchVector.x += 0.5f;
        Vector worldUp = {0.0f, 1.0f, 0.0f};
        this->markerGeom->setDirection(this->scratchVector, worldUp);
        ((AbyssEngine::Transform *) (Globals::Canvas->TransformGetTransform(this->markerGeom->transform)))->Update(dt, false);
    }

    if (this->transitionIn != 0 || this->transitionOut != 0) {
        float step = (float) (dt * 15);
        this->easeX->Increase(step);
        this->easeY->Increase(step);
        this->easeZ->Increase(step);
        tmp.x = this->easeX->GetValue();
        tmp.y = this->easeY->GetValue();
        tmp.z = this->easeZ->GetValue();
        this->scratchVector = tmp;
        PaintCanvas *canvas = Globals::Canvas;
        Matrix matrix = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
        AbyssEngine::AEMath::MatrixSetTranslation(matrix, this->scratchVector.x, this->scratchVector.y, this->scratchVector.z);
        canvas = Globals::Canvas;
        canvas->CameraSetLocal(canvas->CameraGetCurrent(),
                                                          *(const AbyssEngine::AEMath::Matrix *) (&matrix));
        float side = this->scratchVector.x - this->easeX->GetMaxValue();
        float delta = this->scratchVector.x - this->easeX->GetMaxValue();
        if ((side > 0.0f ? delta : -delta) <= 1.0f) {
            side = this->scratchVector.y - this->easeY->GetMaxValue();
            delta = this->scratchVector.y - this->easeY->GetMaxValue();
            if ((side > 0.0f ? delta : -delta) <= 1.0f) {
                side = this->scratchVector.z - this->easeZ->GetMaxValue();
                delta = this->scratchVector.z - this->easeZ->GetMaxValue();
                if ((side > 0.0f ? delta : -delta) <= 1.0f) {
                    if (this->transitionIn != 0) {
                        this->mode = 3;
                    } else {
                        ArrayReleaseClasses(*this->stationGeoms);
                        delete this->stationGeoms;
                        this->stationGeoms = 0;
                        ArrayReleaseClasses(*this->ringGeoms);
                        delete this->ringGeoms;
                        this->ringGeoms = 0;
                        delete[] this->stationAngles;
                        this->stationAngles = (int *) 0;
                        delete[] this->stationDistances;
                        this->stationDistances = (int *) 0;
                        ArrayRelease(*this->usedFlags);
                        delete this->usedFlags;
                        this->usedFlags = 0;
                        delete this->starSystemRoot;
                        this->mode = 0;
                        this->starSystemRoot = (AEGeometry *) 0;
                        this->systemGeoms->data()[this->selectedSystem]->setVisible(true);
                    }
                    this->transitionIn = 0;
                    this->transitionOut = 0;
                }
            }
        }
        return;
    }

    ((FModSound *) (Globals::sound))->setParamValue(0, 0x66, this->spin);
    if (this->mode != 3) {
        if (this->mode != 0) return;
        if (this->pulseSystem >= 0 && this->autoMode != 0 &&
            this->autoTimer < 4000) {
            this->autoTimer += dt;
            if (this->autoTimer > 3999) {
                OnTouchBegin(Globals::w >> 1, Globals::h >> 1);
            }
            float scale = (float)(((float) this->autoTimer / 4000.0f) * 0.002);
            this->systemGeoms->data()[this->pulseSystem]->setScaling(scale, scale, scale);
        }
        if (this->dragging == 0) {
            float vx = this->momentumFactor * this->velocityX;
            float vy = this->momentumFactor * this->velocityY;
            this->velocityX = vx;
            this->velocityY = vy;
            if (absf_update(vx) > 0.5f) {
                this->panX = (int) (vx + (float) this->panX);
            }
            if (absf_update(vy) > 0.5f) {
                this->panY = (int) (vy + (float) this->panY);
            }
        }
        int combinedX = (int)(this->cameraBaseX + (float)this->panX);
        int combinedY = (int)(this->cameraBaseZ + (float)this->panY);
        if (combinedX > 120) this->velocityX = (float)(120 - combinedX) * 0.1f;
        else if (combinedX <= -501) this->velocityX = (float)(-500 - combinedX) * 0.1f;
        if (combinedY >= 141) this->velocityY = (float)(140 - combinedY) * 0.1f;
        else if (combinedY < -400) this->velocityY = (float)(-400 - combinedY) * 0.1f;
        PaintCanvas *canvas = Globals::Canvas;
        Matrix matrix = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
        AbyssEngine::AEMath::MatrixSetTranslation(matrix,
                             (this->cameraBaseX + (float) this->panX) * 20.0f,
                             (this->cameraBaseZ + (float) this->panY) * 20.0f,
                             0.0f);
        canvas = Globals::Canvas;
        canvas->CameraSetLocal(canvas->CameraGetCurrent(),
                                                          *(const AbyssEngine::AEMath::Matrix *) (&matrix));
        float speed = absf_update(this->velocityX + this->velocityY);
        this->spin += speed < 10.0f ? speed : 10.0f;
        if (this->spin > 100.0f) this->spin = 0.0f;
        if (this->pathAnim != 0) {
            Array<uint8_t> *vis = (Array<uint8_t> *) Globals::status->getSystemVisibilities();
            int selected = this->selectedSystem;
            if (vis != 0 && vis->size() > (uint32_t)selected && selected >= 0 && vis->data()[selected] != 0) {
                this->scratchVector = *this->systemPositions->data()[selected];
                float targetX = (float) (Globals::w >> 1);
                float targetY = (float) (Globals::h >> 1);
                float currentX = this->scratchVector.x;
                float currentY = this->scratchVector.y;
                float sx = 0.0f;
                if (!(currentX < targetX)) {
                    if (currentX > targetX) sx = (currentX - targetX) / -30.0f;
                } else {
                    sx = (currentX - targetX) / -30.0f;
                }
                float sy = (currentY - targetY) / -30.0f;
                if (!(currentY > targetY) && !(currentY < targetY)) sy = 0.0f;
                if (abs((int)sx) > abs((int)(this->scratchVector.x - targetX))) sx *= 0.5f;
                if (abs((int)sy) > abs((int)(this->scratchVector.y - targetY))) sy *= 0.5f;
                this->velocityX = sx;
                this->velocityY = sy;
                if (absf_update(sx) <= 2.0f && absf_update(sy) <= 2.0f) {
                    this->pathAnim = 0;
                    this->lastSelectedSystem = this->selectedSystem;
                }
            }
        }
        return;
    }

    {
        if (this->stationCenterAnim != 0) {
            int selectedStation = this->selectedStation;
            float oldPitch = this->pitch;
            float oldYaw = this->yaw;
            int target = 0x8000 - this->stationAngles[selectedStation];
            int yaw = (int)oldYaw;
            int pitch = (int)oldPitch;
            int yawDistance = yaw - target;
            if (yawDistance < 0) yawDistance = -yawDistance;
            int pitchDistance = -3096 - pitch;
            if (pitch > -3096) pitchDistance = pitch + 3096;
            float yawFactor;
            int yawStep;
            if (target <= yaw) {
                yawFactor = -0.25f;
                yawStep = yaw - target;
            } else {
                yawFactor = 0.25f;
                yawStep = target - yaw;
            }
            this->yaw = oldYaw + (float)yawStep * yawFactor;
            if (yawDistance <= 10) this->stationCenterAnim = 0;
            float pitchFactor;
            int pitchStep;
            if (pitch > -3097) {
                pitchFactor = -0.25f;
                pitchStep = pitch + 3096;
            } else {
                pitchFactor = 0.25f;
                pitchStep = -3096 - pitch;
            }
            this->pitch = oldPitch + (float)pitchStep * pitchFactor;
            if (yawDistance <= 4 * dt && abs(pitchDistance) <= 4 * dt) {
                this->lastSelectedStation = selectedStation;
                this->stationCenterAnim = 0;
            }
        } else if (this->dragging == 0) {
            float vx = this->momentumFactor * this->velocityX;
            float vy = this->momentumFactor * this->velocityY;
            this->velocityX = vx;
            this->velocityY = vy;
            if (absf_update(vx) > 0.5f) {
                this->yaw = vx + this->yaw;
            }
            if (absf_update(vy) > 0.5f) {
                this->pitch = vy + this->pitch;
            }
        }
        float magnitude = absf_update(this->velocityX + this->velocityY) * 0.01f;
        float speed = magnitude < 10.0f ? magnitude : 10.0f;
        this->spin += speed;
        if (this->spin > 100.0f) this->spin = 0.0f;
        if (this->stationGeoms != 0) {
            for (uint32_t i = 0; i < this->stationGeoms->size(); i++) {
                if (this->stationGeoms->data()[i] != 0) {
                    this->stationGeoms->data()[i]->rotate(0.0f, (float) dt * 0.0002f, 0.0f);
                }
            }
        }
        this->yaw = (float) ((int) this->yaw % 0x10000);
        float nextPitch = this->pitch;
        float pitch = nextPitch > -8192.0f ? nextPitch : -8192.0f;
        if (nextPitch > 8192.0f) pitch = 8192.0f;
        this->pitch = pitch;
        this->starSystemRoot->setRotation(this->pitch * 0.000015259f * 6.2832f,
                                         this->yaw * 0.000015259f * 6.2832f, -0.098175f);
    }
}

StarMap::StarMap(bool jumpMapMode, Mission *mission, bool param3, int param4)
    : scratchVector{0.0f, 0.0f, 0.0f}, scratchVector2{0.0f, 0.0f, 0.0f} {
    this->touchDeltaX = 0.0f;
    this->touchDeltaY = 0.0f;
    this->field_0x158 = 0.0f;
    this->touchStartX = 0.0f;
    this->touchStartY = 0.0f;
    this->field_0x14c = 0.0f;
    this->lastTouchX = 0.0f;
    this->lastTouchY = 0.0f;
    this->field_0x164 = 0;
    this->yaw = 0.0f;
    this->pitch = 0.0f;
    this->field_0x190 = 0;

    this->mode = 0;
    this->hitRadius = *reinterpret_cast<int *>(reinterpret_cast<char *>(Globals::layout) + 0x88);
    this->selectedSystem = -1;
    this->starSystemRoot = (AEGeometry *) 0;
    this->stations = (Array<Station *> *) 0;
    this->systemPositions = 0;
    this->stationPositions = 0;
    this->suppressNextClose = 0;
    this->field_0x01 = 0;
    this->alienJumpPending = 0;
    this->easeX = (AbyssEngine::EaseInOut *) 0;
    this->easeY = (AbyssEngine::EaseInOut *) 0;
    this->easeZ = (AbyssEngine::EaseInOut *) 0;
    this->momentumFactor = 0.0f;
    this->velocityX = 0.0f;
    this->velocityY = 0.0f;
    this->dragging = 0;

    this->iconBuffer = new int[5];
    this->systems = (Array<SolarSystem *> *) static_cast<Galaxy *>(Globals::galaxy)->getSystems();
    this->field_0x10 = 500;
    this->field_0x14 = 500;

    AEGeometry *root = new AEGeometry(Globals::Canvas);
    this->systemRoot = root;

    Array<AEGeometry *> *systemsGeom = new Array<AEGeometry *>();
    this->systemGeoms = systemsGeom;
    Array<Vector *> *systemPositions = new Array<Vector *>();
    this->systemPositions = systemPositions;
    ArraySetLength(0x22, *systemsGeom);
    ArraySetLength(0x22, *systemPositions);

    for (uint32_t i = 0; i < this->systemGeoms->size(); i++) {
        int tex = this->systems->data()[i]->getTextureIndex();
        uint16_t image = (uint16_t)(tex + 0x4696);
        if (Globals::status->getCurrentCampaignMission() > 0x9d) image = 0x469b;
        if (i != 0x1b) image = (uint16_t)(tex + 0x4696);
        AEGeometry *geom = new AEGeometry(image, Globals::Canvas, false);
        this->systemGeoms->data()[i] = geom;
        float scale = 0.012f;
        if (this->systems->data()[i]->getTextureIndex() == 15 && Globals::status->getCurrentCampaignMission() <= 157) {
            scale = Globals::status->getCurrentCampaignMission() <= 105 ? 0.018f : 0.025f;
        }
        this->systemGeoms->data()[i]->setScaling(scale, scale, scale);
        AEGeometry *positionGeom = this->systemGeoms->data()[i];
        float px = (float) ((int) (((100 - this->systems->data()[i]->getX()) / 100.0f) * 14000.0f) - 10000);
        float py = (float) ((int) (((100 - this->systems->data()[i]->getY()) / 100.0f) * 13000.0f) - 9000);
        float pz = (float) ((int) (((100 - this->systems->data()[i]->getZ()) / 100.0f) * 6000.0f) + 1000);
        positionGeom->setPosition(px, py, pz);
        this->systemRoot->addChild(this->systemGeoms->data()[i]->transform);
        Vector *pos = new Vector{0.0f, 0.0f, 0.0f};
        this->systemPositions->data()[i] = pos;
    }

    static_cast<AERandom *>(Globals::rnd)->reset();
    this->markerGeom = (AEGeometry *) 0;
    this->choiceVisible = 0;
    if (Globals::status->getCurrentCampaignMission() > 0x1f &&
        Globals::status->field_7c >= 0) {
        AEGeometry *marker = new AEGeometry((uint16_t) 0x4262, Globals::Canvas, false);
        this->markerGeom = marker;
        this->markerGeom->setPosition(this->systemGeoms->data()[Globals::status->field_7c]->getPosition());
        ((AbyssEngine::Transform *) (Globals::Canvas->TransformGetTransform(this->markerGeom->transform)))->SetAnimationState(
            (AbyssEngine::AnimationMode) 2, 0);
        this->markerGeom->setRotation(0.0f, 3.1416f, 0.0f);
    }

    init(jumpMapMode, mission, param3, param4);
}

static inline float absf_local(float v) {
    return v > 0.0f ? v : -v;
}

uint32_t StarMap::OnTouchBegin(int x, int y) {
    if (this->choiceVisible != 0) {
        this->choiceWindow->OnTouchBegin(x, y);
        return 0;
    }
    if (this->transitionIn != 0 || this->transitionOut != 0) {
        return 0;
    }

    Globals::layout->OnTouchBegin(x, y);
    if ((this->pad_0xa8_a != 0 && this->pathAnim != 0) ||
        this->stationCenterAnim != 0) {
        return 0;
    }
    this->backButton->OnTouchBegin(x, y);
    if (Globals::layout->field_0xc_leftMargin >= y || y >= Globals::h - Globals::layout->
        field_0x10_rightMargin) {
        return 0;
    }
    if (this->autoMode != 0 && this->autoTimer < 4000) {
        return 0;
    }

    Globals::sound->stop(0x66);
    Globals::sound->play(0x66, 0, 0, 0.0f);

    float fx = (float) x;
    float fy = (float) y;
    this->lastTouchX = fx;
    this->lastTouchY = fy;
    this->touchStartX = fx;
    this->touchStartY = fy;
    this->touchDeltaX = 0.0f;
    this->touchDeltaY = 0.0f;
    this->dragging = 1;
    this->pathAnim = 0;

    int oldSystem = this->selectedSystem;
    if (this->mode == 0) {
        this->jumpCost = 0;
        this->selectedSystem = -1;
        for (uint32_t i = 0; i < this->systemGeoms->size(); i++) {
            Array<uint8_t> *vis = (Array<uint8_t> *) Globals::status->getSystemVisibilities();
            if (vis != 0 && i < vis->size() && vis->data()[i] != 0) {
                this->scratchVector = *this->systemPositions->data()[i];
                if (this->scratchVector.z > 0.0f &&
                    absf_local(this->scratchVector.x - fx) < (float) this->hitRadius &&
                    absf_local(this->scratchVector.y - fy) < (float) this->hitRadius) {
                    this->selectedSystem = (int) i;
                    if (this->stations != 0) {
                        ArrayReleaseClasses(*this->stations);
                        delete this->stations;
                        this->stations = (Array<Station *> *) 0;
                    }
                    this->stations = new Array<Station *>();
                    FileRead *reader = new FileRead();
                    this->stations =
                            ((FileRead *) (reader))->loadStationsBinary(this->systems->data()[this->selectedSystem]);
                    delete reader;
                    if (oldSystem != this->selectedSystem) {
                        Globals::sound->play(0x67, 0, 0, 0.0f);
                    }
                    int current = Globals::status->getSystem()->getIndex();
                    int dist = this->pathFinder->getJumpDistance(this->systems, current, this->selectedSystem);
                    this->jumpCost = dist;
                    if (dist == 0) {
                        int currentIndex = Globals::status->getSystem()->getIndex();
                        int selected = this->selectedSystem;
                        if (currentIndex != selected) {
                            this->jumpCost = 4;
                            if (!this->systems->data()[selected]->getRoutes()) this->noRoute = 1;
                        }
                    }
                    if (Globals::status->hardCoreMode() != 0) {
                        this->jumpCost <<= 1;
                    }
                    return 0;
                }
            }
        }
    } else if (this->mode == 3) {
        for (uint32_t i = 0; i < this->stationPositions->size(); i++) {
            this->scratchVector = *this->stationPositions->data()[i];
            if (this->scratchVector.z > 0.0f &&
                absf_local(this->scratchVector.x - fx) < (float) this->hitRadius &&
                absf_local(this->scratchVector.y - fy) < (float) this->hitRadius) {
                this->selectedStation = (int) i;
                Globals::sound->play(0x68, 0, 0, 0.0f);
                return 0;
            }
        }
    }
    return 0;
}

int StarMap::OnTouchMove(int x, int y) {
    if (this->choiceVisible) {
        this->choiceWindow->OnTouchMove(x, y);
    } else if (!this->transitionIn && !this->transitionOut) {
        Globals::layout->OnTouchMove(x, y);
        if ((!this->pad_0xa8_a || !this->pathAnim) && !this->stationCenterAnim) {
            this->backButton->OnTouchMove(x, y);
            if (this->dragging) {
                float fx = (float) x;
                float fy = (float) y;
                float lastX = this->lastTouchX;
                float lastY = this->lastTouchY;
                this->momentumFactor = 1.0f;
                this->lastTouchX = fx;
                this->lastTouchY = fy;
                float dx = (fx - lastX) * this->dragScale;
                float dy = this->dragScale * (fy - lastY);
                this->touchDeltaX = dx;
                this->touchDeltaY = dy;
                if (this->mode) {
                    if (absf_local(this->touchStartX - fx) > 3.0f ||
                        absf_local(this->touchStartY - fy) > 3.0f) {
                        this->lastSelectedStation = -1;
                        this->selectedStation = -1;
                    }

                    float rotZ = dx * 50.0f;
                    float rotX = dy * -50.0f;
                    float nextPitch = rotX + this->pitch;
                    float pitch = nextPitch > -8192.0f ? nextPitch : -8192.0f;
                    if (nextPitch > 8192.0f) pitch = 8192.0f;
                    float yaw = (float) ((int) (this->yaw + rotZ) % 0x10000);
                    float absZ = absf_local(rotZ);
                    float absX = absf_local(rotX);
                    this->touchDeltaX = rotZ;
                    this->touchDeltaY = rotX;
                    this->velocityX = absZ > 150.0f ? rotZ : 0.0f;
                    this->velocityY = absX > 150.0f ? rotX : 0.0f;
                    this->yaw = yaw;
                    this->pitch = pitch;
                    this->starSystemRoot->setRotation(pitch * 0.000015259f * 6.2832f, yaw * 0.000015259f * 6.2832f, -0.098175f);
                } else {
                    float magnitude = absf_local(dx + dy);
                    float speed = 10.0f;
                    if (magnitude < 10.0f) speed = magnitude;
                    float targetX = (float) this->panX;
                    float targetY = (float) this->panY;
                    this->panX = (int) (dx + targetX);
                    this->panY = (int) (dy + targetY);
                    this->spin = this->spin + speed;
                    if (this->spin > 100.0f) this->spin = 0.0f;
                    if (absf_local(this->touchStartX - fx) > 3.0f ||
                        absf_local(this->touchStartY - fy) > 3.0f) {
                        this->lastSelectedSystem = -1;
                        this->selectedSystem = -1;
                        this->jumpCost = 0;
                    }
                    PaintCanvas *canvas = Globals::Canvas;
                    Matrix matrix = *static_cast<Matrix *>(canvas->CameraGetLocal(canvas->CameraGetCurrent()));
                    canvas = Globals::Canvas;
                    unsigned int camera = canvas->CameraGetCurrent();
                    canvas->CameraSetLocal(camera, matrix);
                }
            }
        }
    }
    return 0;
}

void StarMap::drawKey() {
    int imageWidth = Globals::Canvas->GetImage2DWidth(this->keyImageDiscovered);
    int height = Globals::h;
    int lineGap = Globals::layout->field_0x4;
    int boxWidth = this->keyBoxWidth;
    int rightPad = Globals::layout->field_0x10_rightMargin;
    int lineHeight = Globals::layout->field_0x2c_rowHeight;
    int x = Globals::w - boxWidth;
    Globals::layout->drawBox(7, x, height - rightPad - this->keyBoxHeight - Globals::layout->windowTopInset,
        boxWidth, Globals::layout->windowTopInset + this->keyBoxHeight, String("", false));
    int imageX = x + lineHeight;
    int textX = imageWidth + lineHeight + imageX;
    int y = height - lineHeight - rightPad - lineGap;
    Globals::Canvas->DrawImage2D(this->keyImageRetreat, imageX, y);
    Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(274), textX, y, false);
    y -= Globals::layout->field_0x4;
    Globals::Canvas->DrawImage2D(this->keyImageDiscovered, imageX, y);
    Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(401), textX, y, false);
    y -= Globals::layout->field_0x4;
    Globals::Canvas->DrawImage2D(this->keyImageCurrent, imageX, y);
    Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(547), textX, y, false);
    y -= Globals::layout->field_0x4;
    Globals::Canvas->DrawImage2D(this->keyImageMission, imageX, y);
    Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(556), textX, y, false);
    y -= Globals::layout->field_0x4;
    Globals::Canvas->DrawImage2D(this->keyImageWanted, imageX, y);
    Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(555), textX, y, false);
}

void StarMap::initStarSystem() {
    static const int stationScale[22] = {
            0x140, 0x0c0, 0x100, 0x100, 0x0c0, 0x100, 0x0c0, 0x0c0,
            0x140, 0x100, 0x0c0, 0x0c0, 0x140, 0x100, 0x140, 0x100,
            0x100, 0x100, 0x140, 0x0c0, 0x140, 0x100};
    static const uint32_t planetTextures[19] = {
            0x272f, 0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735,
            0x2736, 0x2737, 0x2738, 0x2739, 0x2730, 0x2737, 0x2733,
            0x272f, 0x2ddd, 0x2736, 0x2731, 0x2735};
    static const uint32_t systemNameImages[4] = {0x4a6, 0x4a3, 0x4a5, 0x4a4};
    uint32_t count = ((Array<void *> *) this->systems->data()[this->selectedSystem]->getStations())->size();

    Array<Station *> *stations = new Array<Station *>();
    this->stations = stations;
    ArraySetLength(count, *stations);
    FileRead *reader = new FileRead();
    this->stations = ((FileRead *)reader)->loadStationsBinary(this->systems->data()[this->selectedSystem]);
    delete reader;

    this->stationAngles = new int[count];
    this->stationDistances = new int[count];
    this->centeredStation = -1;
    static_cast<AERandom *>(Globals::rnd)->setSeed(static_cast<int32_t>(static_cast<uint32_t>(this->systems->data()[this->selectedSystem]->getIndex()) * 1000u));

    Array<AEGeometry *> *stationGeoms = new Array<AEGeometry *>();
    this->stationGeoms = stationGeoms;
    ArraySetLength(count + 1, *stationGeoms);

    AEGeometry *root = new AEGeometry(Globals::Canvas);
    this->starSystemRoot = root;

    Array<uint8_t> *used = new Array<uint8_t>();
    this->usedFlags = used;
    ArraySetLength(this->stationGeoms->size(), *used);
    Array<uint8_t> *flags = this->usedFlags;
    uint32_t flagCount = flags->size();
    for (uint32_t i = 0; i != flagCount; i++) {
        flags->data()[i] = 0;
    }

    for (uint32_t i = 0; i < this->stationGeoms->size(); i++) {
        if (i != 0) {
            uint32_t stationIndex = i - 1;
            AEGeometry *geom = new AEGeometry((uint16_t)(this->stations->data()[stationIndex]->getTextureIndex() + 0x4704), Globals::Canvas, false);
            this->stationGeoms->data()[i] = geom;
            Matrix rotation;
            rotation.m[10] = 1.0f;
            rotation.m[13] = 1.0f;
            uint32_t angleSlot;
            do {
                angleSlot = static_cast<AERandom *>(Globals::rnd)->nextInt(this->usedFlags->size());
            } while (this->usedFlags->data()[angleSlot] != 0);
            this->usedFlags->data()[angleSlot] = 1;
            this->stationAngles[stationIndex] = (int)(angleSlot * (0x10000u / this->usedFlags->size()));
            float angle = (float)this->stationAngles[stationIndex] * 0.000015259f * 6.2832f;
            AbyssEngine::AEMath::MatrixSetRotation(rotation, 0.0f, angle, 0.0f);
            int dist = (i == 1) ? 0x1900 : this->stationDistances[i - 2];
            dist += static_cast<AERandom *>(Globals::rnd)->nextInt(0x15e0) + 0x640;
            this->stationDistances[stationIndex] = dist;
            Vector radial = {0.0f, 0.0f, (float)dist};
            Vector translated = {0.0f, 0.0f, 0.0f};
            translated = AbyssEngine::AEMath::MatrixTransformVector(rotation, radial);
            this->stationGeoms->data()[i]->translate(translated);
            int tex = this->stations->data()[stationIndex]->getTextureIndex();
            float scale = (float)(16 * stationScale[tex]) * 0.000015259f;
            this->stationGeoms->data()[i]->setScaling(scale, scale, scale);
            Vector position = this->stationGeoms->data()[i]->getPosition();
            position = AbyssEngine::AEMath::VectorNormalize(position);
            this->starSystemRoot->addChild(this->stationGeoms->data()[i]->transform);
            // Android ARM 0xc7580 + 0x3e8 literal is 0x00210200 (not a function pointer).
            if ((static_cast<uint32_t>(tex) <= 21 && (0x00210200u & (1u << tex))) ||
                Globals::status->orbitHasPlanetRing(this->stations->data()[stationIndex]->getIndex())) {
                unsigned int ringTransform = 0xffffffffu;
                Globals::Canvas->TransformCreate(ringTransform);
                Globals::Canvas->TransformAddMesh(ringTransform, 0x41d5, false);
                Globals::Canvas->TransformSetLocal(ringTransform, this->stationGeoms->data()[i]->getMatrix());
                this->starSystemRoot->addChild(ringTransform);
            }
            if (this->markerGeom != 0 && this->selectedSystem == Globals::status->field_7c &&
                this->stations->data()[stationIndex]->getIndex() == Globals::status->field_80) {
                this->centeredStation = i;
            }
        }
        if (this->stationGeoms->data()[i]) this->starSystemRoot->addChild(this->stationGeoms->data()[i]->transform);
    }

    this->stationGeoms->data()[1]->setVisible(false);

    Array<AEGeometry *> *rings = new Array<AEGeometry *>();
    this->ringGeoms = rings;
    ArraySetLength(count, *rings);
    for (uint32_t i = 0; i < this->ringGeoms->size(); i++) {
        AEGeometry *ring = new AEGeometry((uint16_t) 0x1a7b, Globals::Canvas, false);
        this->ringGeoms->data()[i] = ring;
        this->starSystemRoot->addChild(this->ringGeoms->data()[i]->transform);
        AEGeometry *ringGeom = this->ringGeoms->data()[i];
        float distance = (float)this->stationDistances[i];
        ringGeom->setRotation(0.0f, (float)static_cast<AERandom *>(Globals::rnd)->nextInt(0x0c45) / 1000.0f, 0.0f);
        float scale = (float)(2 * (int)distance) * 0.000015259f;
        this->ringGeoms->data()[i]->setScaling(scale, scale, scale);
    }

    this->starSystemRoot->setPosition(this->systemGeoms->data()[this->selectedSystem]->getPosition());
    this->starSystemRoot->setScaling(0.0078125f, 0.0078125f, 0.0078125f);
    this->starSystemRoot->setRotation(0.0f, 90.0f, 0.0f);
    AEGeometry *systemRoot = this->starSystemRoot;
    this->yaw = 4096.0f;
    this->pitch = -4096.0f;
    systemRoot->setRotation(-0.3927f, 0.3927f, -0.098175f);
    this->selectedStation = -1;
    Globals::Canvas->Image2DCreate(systemNameImages[this->systems->data()[this->selectedSystem]->getRace()], this->systemNameImage);

    if (this->stationPositions != 0) {
        ArrayReleaseClasses(*this->stationPositions);
        delete this->stationPositions;
    }
    this->stationPositions = 0;
    Array<Vector *> *stationPositions = new Array<Vector *>();
    this->stationPositions = stationPositions;
    ArraySetLength(this->stations->size(), *stationPositions);
    for (uint32_t i = 0; i < this->stationPositions->size(); i++) {
        Array<Vector *> *positions = this->stationPositions;
        Vector *p = new Vector{0.0f, 0.0f, 0.0f};
        positions->data()[i] = p;
    }

    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetLightDirection(0.0f, -1.0f, -1.0f, 0x4000);
    if (this->planetGeom != 0) {
        delete this->planetGeom;
    }
    this->planetTexture = 0xffffffffu;
    this->planetGeom = (AEGeometry *) 0;
    uint16_t texture = planetTextures[this->systems->data()[this->selectedSystem]->getTextureIndex()];
    if (this->selectedSystem == 0x1b) {
        texture = 0x2734;
    }
    Globals::Canvas->TextureCreate(texture, this->planetTexture, false);
    AEGeometry *planet = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
    this->planetGeom = planet;
    this->planetGeom->setPosition(this->systemGeoms->data()[this->selectedSystem]->getPosition());
    this->planetGeom->setRotation(0.0f, 0.0f, 0.0f);
    if (this->systems->data()[this->selectedSystem]->getTextureIndex() == 15) {
        float scale = 0.007f;
        if (Globals::status->getCurrentCampaignMission() > 105)
            scale = Globals::status->getCurrentCampaignMission() > 157 ? 0.004f : 0.01f;
        this->planetGeom->setScaling(scale, scale, scale);
    } else {
        this->planetGeom->setScaling(0.004f, 0.004f, 0.004f);
    }
    this->systemGeoms->data()[this->selectedSystem]->setVisible(false);

    Vector lightPosition = this->planetGeom->getPosition();
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetLightPosition(lightPosition.x, lightPosition.y, lightPosition.z, 0x4000);
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetLightColorAmbient(0.2f, 0.2f, 0.2f, 0x4000);
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetLightColorDiffuse(2.0f, 2.0f, 2.0f, 0x4000);
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetMaterialColorAmbient(1.0f, 1.0f, 1.0f);
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetMaterialColorDiffuse(1.0f, 1.0f, 1.0f);
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetMaterialColorSpecular(0.0f, 0.0f, 0.0f);
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->LightSetMaterialColorShininess(10.0f);
    static_cast<Engine *>(static_cast<ApplicationManager *>(Globals::appManager)->GetEngine())->field_0x32c = 1;
    static_cast<AERandom *>(Globals::rnd)->setSeed((long long)Globals::status->getPlayingTime());
}

void StarMap::drawOnScreenInfo(int index, bool stationMode) {
    Array<Vector *> *positions = stationMode ? this->stationPositions : this->systemPositions;
    this->scratchVector = Vector(*positions->data()[index]);
    float x = this->scratchVector.x;
    float y = this->scratchVector.y;
    if (x < -50.0f || x > (float)(Globals::w + 50) ||
        y < -50.0f || y > (float)(Globals::h + 50)) return;
    Globals::Canvas->SetColor(0xffffffffu);
    int *icons = this->iconBuffer;
    for (int i = 0; i != 5; ++i) icons[i] = -1;
    int discovered = stationMode ? this->stations->data()[index]->isDiscovered() :
                                  this->systems->data()[index]->isFullyDiscovered();
    if (discovered) this->iconBuffer[0] = this->keyImageDiscovered;
    Mission *campaign = reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()));
    Mission *freelance = Globals::status->getFreelanceMission();
    if (Globals::status->getCurrentCampaignMission() == 52 &&
        (stationMode ? this->stations->data()[index]->getIndex() == 74 :
                       static_cast<int32_t>(this->systems->data()[index]->getStationEnumIndex(74)) >= 0)) {
        this->iconBuffer[1] = this->keyImageWanted;
    }
    if (Globals::status->getCurrentCampaignMission() == 116 && stationMode &&
        this->stations->data()[index]->getIndex() >= 90 && this->stations->data()[index]->getIndex() <= 94) {
        int statusValue = reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getStatusValue();
        if (!(statusValue & (1 << (this->stations->data()[index]->getIndex() - 90))))
            goto special_campaign_marker;
    }
    if (Globals::status->getCurrentCampaignMission() == 120 &&
        (stationMode ? this->stations->data()[index]->getIndex() == 93 :
                       static_cast<int32_t>(this->systems->data()[index]->getStationEnumIndex(93)) >= 0))
        goto special_campaign_marker;
    if (Globals::status->getCurrentCampaignMission() == 125 && stationMode &&
        Globals::status->isFreighterMissionStation(this->stations->data()[index]->getIndex())) {
        int statusValue = reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getStatusValue();
        if (!(statusValue & (1 << Globals::status->getFreighterMissionStationBit(this->stations->data()[index]->getIndex()))))
            goto special_campaign_marker;
    }
    if (Globals::status->getCurrentCampaignMission() == 125 && !stationMode &&
        Globals::status->isFreighterMissionStation(this->systems->data()[index]->getWarpGateIndex())) {
        int statusValue = reinterpret_cast<Mission *>(intptr_t(Globals::status->getCampaignMission()))->getStatusValue();
        if (!(statusValue & (1 << Globals::status->getFreighterMissionStationBit(this->systems->data()[index]->getWarpGateIndex()))))
            goto special_campaign_marker;
    }
    goto campaign_targets;
special_campaign_marker:
    this->iconBuffer[1] = this->keyImageWanted;
campaign_targets:
    if (campaign) {
        bool missionTarget = false;
        if (campaign->getType() == 163) {
            Array<int> *targets = Globals::status->field_90;
            if (targets) {
                for (uint32_t i = 0; i < targets->size(); ++i) {
                    if (stationMode ? targets->data()[i] == this->stations->data()[index]->getIndex() :
                        static_cast<int32_t>(this->systems->data()[index]->getStationEnumIndex(targets->data()[i])) >= 0) {
                        missionTarget = true;
                        break;
                    }
                    targets = Globals::status->field_90;
                }
            }
        }
        if (!campaign->isEmpty() && Globals::status->getCurrentCampaignMission() != 52 &&
            Globals::status->getCurrentCampaignMission() != 120 && Globals::status->getCurrentCampaignMission() != 128 &&
            Globals::status->getCurrentCampaignMission() != 130 &&
            (Globals::status->getCurrentCampaignMission() < 148 || Globals::status->getCurrentCampaignMission() >= 152)) {
            bool suppressed = Globals::status->getCurrentCampaignMission() == 59 &&
                (stationMode ? this->stations->data()[index]->getIndex() == 101 :
                               this->systems->data()[index]->getIndex() == 23);
            if (!suppressed) {
                bool targetMatch;
                if (stationMode) {
                    int stationId = this->stations->data()[index]->getIndex();
                    targetMatch = (stationId == campaign->getTargetStation()) | missionTarget;
                } else {
                    SolarSystem *system = this->systems->data()[index];
                    targetMatch = (static_cast<int32_t>(system->getStationEnumIndex(campaign->getTargetStation())) >= 0) | missionTarget;
                }
                if (targetMatch || (Globals::status->getCurrentCampaignMission() >= 33 &&
                    (stationMode ? this->stations->data()[index]->getIndex() == Globals::status->field_80 :
                        static_cast<int32_t>(this->systems->data()[index]->getStationEnumIndex(Globals::status->field_80)) >= 0) &&
                    campaign->getTargetStation() == -1)) this->iconBuffer[1] = this->keyImageWanted;
            }
        }
    }
    if (freelance && !freelance->isEmpty()) {
        if (stationMode) {
            int stationId = this->stations->data()[index]->getIndex();
            int target = freelance->getType() == 14 ? freelance->getAgent()->getStation() : freelance->getTargetStation();
            if (stationId == target) this->iconBuffer[2] = this->keyImageMission;
        } else {
            SolarSystem *system = this->systems->data()[index];
            int target = freelance->getType() == 14 && Globals::status->getShip()->hasCargo(115, 1) ?
                freelance->getAgent()->getStation() : freelance->getTargetStation();
            if (static_cast<int32_t>(system->getStationEnumIndex(target)) >= 0) this->iconBuffer[2] = this->keyImageMission;
        }
    }
    if (stationMode && this->selectedSystem >= 0) {
        int warpGate = this->systems->data()[this->selectedSystem]->getWarpGateIndex();
        if (warpGate == this->stations->data()[index]->getIndex()) this->iconBuffer[3] = this->keyImageCurrent;
    }
    Array<PendingProduct *> *pendingProducts = reinterpret_cast<Array<PendingProduct *> *>(intptr_t(Globals::status->getPendingProducts()));
    if (pendingProducts) {
        for (uint32_t i = 0; i < pendingProducts->size(); ++i) {
            PendingProduct *product = pendingProducts->data()[i];
            if (product && (stationMode ? product->stationIndex == this->stations->data()[index]->getIndex() :
                           static_cast<int32_t>(this->systems->data()[index]->getStationEnumIndex(product->stationIndex)) >= 0)) {
                this->iconBuffer[4] = this->keyImageRetreat;
                break;
            }
        }
    }
    float textX = this->scratchVector.x;
    unsigned int textFont = Globals::font;
    PaintCanvas *textCanvas = Globals::Canvas;
    int drawY = (int)(this->scratchVector.y + (float)(this->iconWidth >> 1) - 3.0f);
    if (stationMode) {
        int textW = textCanvas->GetTextWidth(textFont, this->stations->data()[index]->getName());
        float drawX = textX - (float)(textW / 2);
        int current = Globals::status->getStation()->getIndex();
        if (current == this->stations->data()[index]->getIndex()) {
            int alpha = this->alpha;
            PaintCanvas *canvas = Globals::Canvas;
            unsigned int pulse = (unsigned int)(Globals::layout->getPulseValue(0.005f) * (float)(255 - alpha));
            canvas->SetColor(0xff, 0xff, 0xff, (unsigned char)pulse);
            Globals::Canvas->DrawImage2D(this->currentMarkerIcon, (int)this->scratchVector.x, (int)this->scratchVector.y, 0x11, 0x44);
        }
        if (this->selectedStation == index) {
            Globals::Canvas->SetColor(0xff, 0x80, 0, (unsigned char)~this->alpha);
            Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), this->stations->data()[index]->getName(), (int)drawX,
                                                          drawY, false);
            Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                        (unsigned char) (0xff), (unsigned char)~this->alpha);
            if (this->stations->data()[index]->getTecLevel() > 0) {
                Globals::Canvas->DrawString(Globals::font,
                    *Globals::gameText->getText(133) + String(": ", false) +
                    String(String(this->stations->data()[index]->getTecLevel()), false), (int)drawX,
                                                              drawY + ((Layout *) Globals::layout)->field_0x4,
                                                              false);
            }
            Globals::Canvas->DrawImage2D((unsigned int) (this->selectIcon), (int)this->scratchVector.x, (int)this->scratchVector.y, 0x11, 0x44);
        } else {
            Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                        (unsigned char) (0xff), (unsigned char)~this->alpha);
            Globals::Canvas->DrawImage2D((unsigned int) (this->cursorIcon), (int)this->scratchVector.x, (int)this->scratchVector.y, 0x11, 0x44);
            if (this->selectedStation >= 0) {
                int objectAlpha = 255 - this->alpha;
                Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(objectAlpha < 64 ? objectAlpha : 64));
            }
            Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), this->stations->data()[index]->getName(), (int)drawX,
                                                          drawY, false);
        }
    } else {
        int textW = textCanvas->GetTextWidth(textFont, this->systems->data()[index]->getName());
        int drawX = (int)(textX - (float)(textW / 2));
        if (this->pad_0xa8_a) this->systems->data()[index]->getIndex();
        int currentSystem = Globals::status->getSystem()->getIndex();
        if (currentSystem == this->systems->data()[index]->getIndex()) {
            float alpha = (float)this->alpha;
            PaintCanvas *canvas = Globals::Canvas;
            unsigned int pulse = (unsigned int)(Globals::layout->getPulseValue(0.005f) * alpha);
            canvas->SetColor(0xff, 0xff, 0xff, (unsigned char)pulse);
            Globals::Canvas->DrawImage2D(this->currentMarkerIcon, (int)this->scratchVector.x, (int)this->scratchVector.y, 0x11, 0x44);
        }
        Globals::Canvas->SetColor(255, 255, 255, (unsigned char)this->alpha);
        if (this->selectedSystem == index) {
            Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0x80),
                                                        (unsigned char) (0), (unsigned char) (this->alpha));
        } else if (this->selectedSystem >= 0) {
            Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(this->alpha < 64 ? this->alpha : 64));
        }
        Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), this->systems->data()[index]->getName(), (int)drawX, drawY,
                                                      false);
        int objectAlpha = (this->selectedSystem == index || this->selectedSystem < 0) ? this->alpha : (this->alpha < 64 ? this->alpha : 64);
        Globals::Canvas->SetColor(255, 255, 255, (unsigned char)objectAlpha);
        if (this->systems->data()[index]->hasNoOwner() == 0) {
            int race = this->systems->data()[index]->getRace();
            uint32_t *image;
            if (race == 2) {
                image = &this->raceImageB;
            } else if (race == 1) {
                image = &this->raceImageA;
            } else if (race == 0) {
                image = &this->raceImageNeutral;
            } else {
                image = &this->raceImageDefault;
            }
            Globals::Canvas->DrawImage2D(*image, drawX, drawY - 3, 0x11, 0x12);
        }
        if (index == 26 && Globals::status->field_114 == 3) {
            Globals::Canvas->DrawImage2D(this->image_0x134, drawX,
                (int)((float)drawY + (float)Globals::layout->field_0x2c_rowHeight * 0.5f), 0x11, 0x12);
        }
        int selectedSystem = this->selectedSystem;
        Globals::Canvas->SetColor(255, 255, 255, (unsigned char)this->alpha);
        if (selectedSystem == index) {
            Globals::Canvas->DrawImage2D(this->selectIcon, (int)this->scratchVector.x, (int)this->scratchVector.y, 0x11, 0x44);
            int securityLine = Globals::layout->field_0x4;
            if (!this->systems->data()[index]->hasNoOwner()) {
                Globals::Canvas->DrawString(Globals::font,
                    *Globals::gameText->getText(this->systems->data()[this->selectedSystem]->getRace() + 406),
                    drawX, drawY + Globals::layout->field_0x4, false);
                securityLine = 2 * Globals::layout->field_0x4;
            }
            int security = this->systems->data()[this->selectedSystem]->getSecurityLevel();
            if (index == 26 && Globals::status->field_114 > 1) security = 3;
            // Android rodata 0x1fde10: three 32-bit channel slots per level.
            static const int securityColor[4][3] = {{255,42,0},{255,108,0},{237,237,0},{0,237,0}};
            Globals::Canvas->SetColor(securityColor[security][0], securityColor[security][1],
                                     securityColor[security][2], (unsigned char)this->alpha);
            Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(security + 402),
                                       drawX, drawY + securityLine, false);
            Globals::Canvas->SetColor(255, 255, 255, (unsigned char)this->alpha);
        } else {
            Globals::Canvas->DrawImage2D(this->cursorIcon, (int)this->scratchVector.x, (int)this->scratchVector.y, 0x11, 0x44);
            if (this->selectedSystem >= 0)
                Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(this->alpha < 64 ? this->alpha : 64));
        }
    }

    int halfWidth = this->iconWidth >> 1;
    int iconX = (int)(this->scratchVector.x + (float)halfWidth - 7.0f);
    int iconY = (int)(this->scratchVector.y - (float)halfWidth + 10.0f);
    for (int i = 0; i != 5; i++) {
        int image = this->iconBuffer[i];
        if (image != -1) {
            if (i == 0) {
                int dx = Globals::iPadHD != 0 ? 12 : 18;
                int dy = Globals::iPadHD != 0 ? 24 : 35;
                Globals::Canvas->DrawImage2D((unsigned int) ((uint32_t) image), (int) iconX - dx,
                                    (int) iconY + this->iconWidth - dy);
            } else {
                Globals::Canvas->DrawImage2D((unsigned int) ((uint32_t) image), (int) iconX, (int) iconY);
                iconY += this->missionIconWidth;
            }
        }
    }
}

int StarMap::init(bool jumpMapMode, Mission *mission, bool param3, int param4) {

    Globals::Canvas->FogEnable(0, AbyssEngine::FogMode_dummy);
    this->autoMode = (uint8_t) param3;
    this->pad_0xa8_a = (uint8_t) jumpMapMode;
    this->pulseSystem = param4;

    Globals::Canvas->Image2DCreate(0x4a1, this->raceImageNeutral);
    Globals::Canvas->Image2DCreate(0x49c, this->raceImageA);
    Globals::Canvas->Image2DCreate(0x49f, this->raceImageB);
    Globals::Canvas->Image2DCreate(0x49e, this->raceImageDefault);
    Globals::Canvas->Image2DCreate(0x452, this->keyImageRetreat);
    Globals::Canvas->Image2DCreate(0x4a2, this->keyImageDiscovered);
    Globals::Canvas->Image2DCreate(0x453, this->keyImageCurrent);
    Globals::Canvas->Image2DCreate(0x455, this->keyImageMission);
    Globals::Canvas->Image2DCreate(0x454, this->keyImageWanted);
    Globals::Canvas->Image2DCreate(0x48c, this->selectIcon);
    Globals::Canvas->Image2DCreate(0x48a, this->cursorIcon);
    Globals::Canvas->Image2DCreate(0x4fd, this->currentMarkerIcon);
    Globals::Canvas->Image2DCreate(0x545, this->image_0x134);

    this->iconWidth = Globals::Canvas->GetImage2DWidth((unsigned int) (this->selectIcon));
    this->missionIconWidth = Globals::Canvas->GetImage2DWidth((unsigned int) (this->keyImageMission));
    this->jumpCost = 0;
    this->noRoute = 0;
    this->planetGeom = (AEGeometry *) 0;
    this->spin = 0.0f;
    this->centeredStation = -1;
    this->lastSelectedSystem = -1;
    this->lastSelectedStation = -1;
    this->routeStart = -1;
    this->routeTarget = -1;
    this->field_0xe8 = 0;
    this->field_0xec = 0;
    this->cameraBaseX = 0.0f;
    this->cameraBaseZ = 0.0f;
    this->field_0x01 = 0;
    this->dragging = 0;
    this->transitionIn = 0;
    this->transitionOut = 0;
    this->pathAnim = 0;
    this->stationCenterAnim = 0;
    this->panX = 0;
    this->panY = 0;

    this->easeX = new AbyssEngine::EaseInOut();
    this->easeY = new AbyssEngine::EaseInOut();
    this->easeZ = new AbyssEngine::EaseInOut();
    // Android c6d7e/c6d82 copies this float's bits; Layout currently declares the slot as int.
    __builtin_memcpy(&this->dragScale, &Globals::layout->field_0x90, sizeof(this->dragScale));
    this->prevCamera = (uint32_t)Globals::Canvas->CameraGetCurrent();
    Globals::Canvas->CameraCreate(this->camera);
    Globals::Canvas->CameraSetPerspective(this->camera, 1.1504f, 200.0f, 64000.0f);
    Matrix matrix;
    matrix.m[10] = 1.0f;
    matrix.m[13] = 1.0f;
    AbyssEngine::AEMath::MatrixSetTranslation(matrix, 0.0f, 0.0f, -2500.0f);
    AbyssEngine::AEMath::MatrixSetRotation(matrix, 0.0f, 3.1416f, 0.0f);
    AbyssEngine::AEMath::MatrixSetTranslation(matrix, (float)(20 * (int)this->cameraBaseX),
                                            (float)(20 * (int)this->cameraBaseZ), 0.0f);
    Globals::Canvas->CameraSetLocal(this->camera, matrix);
    Globals::Canvas->CameraSetCurrent((unsigned int) (this->camera));

    int campaign = Globals::status->getCurrentCampaignMission();
    this->isGalaxyMode = campaign > 0xf;
    this->mode = campaign > 0xf ? 0 : 3;
    this->selectedSystem = Globals::status->getSystem()->getIndex();

    if (param3 != 0) {
        this->scratchVector = this->systemGeoms->data()[param4]->getPosition();
    } else if (!jumpMapMode || this->isGalaxyMode == 0) {
        this->scratchVector = this->systemGeoms->data()[this->selectedSystem]->getPosition();
    } else {
        Station *target = reinterpret_cast<Station *>(static_cast<intptr_t>(static_cast<Galaxy *>(Globals::galaxy)->getStation(mission->getTargetStation())));
        if (target && target->getSystem() >= 0) {
            this->scratchVector = this->systemGeoms->data()[target->getSystem()]->getPosition();
        } else if (Globals::status->field_7c >= 0) {
            this->scratchVector = this->systemGeoms->data()[Globals::status->field_7c]->getPosition();
        }
        if (target != Globals::status->playerStation && target) delete target;
    }

    PaintCanvas *cameraCanvas = Globals::Canvas;
    unsigned int currentCamera = Globals::Canvas->CameraGetCurrent();
    __builtin_memcpy(&matrix, cameraCanvas->CameraGetLocal(currentCamera), 0x3c);
    AbyssEngine::AEMath::MatrixSetTranslation(matrix, this->scratchVector.x, this->scratchVector.y, 0.0f);
    Globals::Canvas->CameraSetLocal(this->camera, matrix);
    this->cameraBaseX = this->scratchVector.x / 20.0f;
    this->cameraBaseZ = this->scratchVector.y / 20.0f;
    if (this->mode == 3) {
        this->selectedStation = -1;
        initStarSystem();
        this->scratchVector = this->systemGeoms->data()[this->selectedSystem]->getPosition();
        this->scratchVector.z -= 500.0f;
        cameraCanvas = Globals::Canvas;
        currentCamera = Globals::Canvas->CameraGetCurrent();
        __builtin_memcpy(&matrix, cameraCanvas->CameraGetLocal(currentCamera), 0x3c);
        matrix.m[11] = this->scratchVector.z;
        Globals::Canvas->CameraSetLocal(this->camera, matrix);
    } else {
        this->lastSelectedSystem = this->selectedSystem;
        Array<Station *> *stations = this->stations;
        if (stations != 0) {
            ArrayReleaseClasses(*stations);
            if (this->stations != 0) delete this->stations;
            this->stations = (Array<Station *> *) 0;
        }
        this->stations = new Array<Station *>();
        FileRead *reader = new FileRead();
        this->stations =
                ((FileRead *) (reader))->loadStationsBinary(this->systems->data()[this->selectedSystem]);
        delete reader;
    }
    if (param3 != 0) {
        this->lastSelectedSystem = -1;
        this->selectedSystem = -1;
    }

    this->showKey = 0;
    this->jumpMapModeA = 0;
    this->jumpMapModeB = 0;
    this->exitRequested = 0;
    this->backButton = new TouchButton(
        *Globals::gameText->getText(0x190), 0, Globals::w - ((Layout *) Globals::layout)->field_0x2c_rowHeight,
        Globals::h - ((Layout *) Globals::layout)->field_0x2c_rowHeight, 0x22);
    ChoiceWindow *choice = new ChoiceWindow();
    this->systemPath = (Array<int> *) 0;
    this->choiceWindow = choice;
    this->pathFinder = new SystemPathFinder();

    if (jumpMapMode && this->mode == 0) {
        this->targetSystem = -1;
        this->selectedSystem = -1;
        if (mission->isEmpty() || (!mission->isVisible() && mission->getType() != 14) ||
            (this->mode != 3 && !Globals::status->getSystem()->getRoutes())) {
            if (!mission->isEmpty() && (mission->isVisible() || mission->getType() == 14) &&
                !Globals::status->getSystem()->getRoutes()) {
                for (uint32_t i = 0; i < this->systems->size(); ++i) {
                    if (this->systems->data()[i]->getStations()) {
                        uint32_t j = 0;
                        while (j < reinterpret_cast<Array<int> *>(this->systems->data()[i]->getStations())->size()) {
                            int stationIndex = mission->getTargetStation();
                            int candidate = reinterpret_cast<Array<int> *>(this->systems->data()[i]->getStations())->data()[j++];
                            if (stationIndex == candidate) {
                                this->targetSystem = (int)i;
                                break;
                            }
                        }
                    }
                }
            }
        } else {
            for (uint32_t i = 0; i < this->systems->size(); ++i) {
                if (this->systems->data()[i]->getStations()) {
                    for (uint32_t j = 0;
                         j < reinterpret_cast<Array<int> *>(this->systems->data()[i]->getStations())->size(); ++j) {
                        int stationIndex = mission->getTargetStation();
                        if (stationIndex == reinterpret_cast<Array<int> *>(this->systems->data()[i]->getStations())->data()[j]) {
                            this->targetSystem = (int)i;
                            if (this->mode == 3) {
                                this->lastSelectedStation = (int)j;
                                this->selectedStation = (int)j;
                                this->stationCenterAnim = 1;
                            }
                            break;
                        }
                    }
                }
            }
            if (this->targetSystem == -1 && mission->getTargetStation() == -1 && Globals::status->field_7c >= 0)
                this->targetSystem = Globals::status->field_7c;
            if (this->targetSystem >= 0) {
                this->systemPath = this->pathFinder->getSystemPath(this->systems,
                    Globals::status->getSystem()->getIndex(), this->targetSystem);
            }
        }
        this->pathAnim = 1;
        this->momentumFactor = 0.9f;
        this->selectedSystem = this->targetSystem;
    }

    this->keyBoxWidth = 0;
    static const int keyTextIds[6] = {0x190, 0x191, 0x223, 0x22c, 0x22b, 0x112};
    for (int i = 0; i < 6; i++) {
        unsigned int keyFont = Globals::font;
        PaintCanvas *keyCanvas = Globals::Canvas;
        int width = keyCanvas->GetTextWidth(keyFont,
                                                                    *((GameText *) (Globals::gameText))->getText(
                                                                        keyTextIds[i]));
        if (this->keyBoxWidth < width) {
            this->keyBoxWidth = width;
        }
    }
    this->keyBoxWidth += ((Layout *) Globals::layout)->field_0x8c;
    this->keyBoxHeight =
            ((Layout *) Globals::layout)->field_0x4 * 5 + ((Layout *) Globals::layout)->
            field_0x2c_rowHeight * 2;
    this->autoTimer = 0;
    void *cargo = (void *) ((Ship *) (Globals::status->getShip()))->getCargo(122);
    this->cargoAmount = cargo != 0 ? ((Item *) (cargo))->getAmount() : 0;

    this->bgLayer0 = new AEGeometry((uint16_t) 0x41d2, Globals::Canvas, false);
    this->bgLayer0->setRotation(0.0f, 3.1416f, 0.0f);
    this->bgLayer0->setPosition(-3000.0f, -2500.0f, 0.0f);
    this->bgLayer1 = new AEGeometry((uint16_t) 0x41d3, Globals::Canvas, false);
    this->bgLayer1->setRotation(0.0f, 3.1416f, 0.0f);
    this->bgLayer1->setPosition(-3000.0f, -2500.0f, 0.0f);
    this->bgLayer2 = new AEGeometry((uint16_t) 0x41d4, Globals::Canvas, false);
    this->bgLayer2->setRotation(0.0f, 3.1416f, 0.0f);
    this->bgLayer2->setPosition(-3000.0f, -2500.0f, 0.0f);
    return 0;
}
