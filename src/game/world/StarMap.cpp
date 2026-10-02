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
    String tmp;

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
        this->alpha = (int) (t * 255.0f);
    }

    if (mode != 3 || this->transitionIn != 0 || this->transitionOut != 0) {
        Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff), (unsigned char) (0xff),
                          (unsigned char) (this->alpha < 34 ? this->alpha : 34));
        Array<Vector *> *positions = this->systemPositions;
        Array<bool> *visibility = Globals::status->getSystemVisibilities();
        for (uint32_t i = 0; i < positions->size(); i++) {
            if (visibility == nullptr || i >= visibility->size() || visibility->data()[i] == 0 ||
                (this->autoMode != 0 && this->pulseSystem == (int)i && this->autoTimer < 4000)) {
                continue;
            }
            this->scratchVector = *positions->data()[i];
            Array<int> *routes = (Array<int> *) this->systems->data()[i]->getRoutes();
            if (routes != 0) {
                for (uint32_t j = 0; j < routes->size(); j++) {
                    uint32_t to = (uint32_t) routes->data()[j];
                    if (to < positions->size() && to < visibility->size() && visibility->data()[to] != 0 &&
                        (this->autoMode == 0 || this->pulseSystem != (int)to || this->autoTimer >= 4000)) {
                        this->scratchVector2 = *positions->data()[to];
                        if (this->scratchVector.z >= 0.0f || this->scratchVector2.z >= 0.0f) {
                            Globals::Canvas->DrawLine((int) this->scratchVector.x, (int) this->scratchVector.y,
                                              (int) this->scratchVector2.x, (int) this->scratchVector2.y);
                        }
                    }
                }
            }
        }
        Globals::Canvas->SetColor(255, 255, 255, (unsigned char)this->alpha);
        this->scratchVector = *positions->data()[Globals::status->getSystem()->getIndex()];
        Array<int> *currentRoutes = reinterpret_cast<Array<int> *>(Globals::status->getSystem()->getRoutes());
        for (uint32_t i = 0; i < this->systems->size(); i++) {
            visibility = Globals::status->getSystemVisibilities();
            if (visibility != nullptr && i < positions->size() && i < visibility->size() && visibility->data()[i]) {
                this->scratchVector2 = *positions->data()[i];
                if (currentRoutes && (this->scratchVector.z >= 0.0f || this->scratchVector2.z >= 0.0f) && !this->jumpMapModeB) {
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
                this->scratchVector = *positions->data()[this->systemPath->data()[i]];
                this->scratchVector2 = *positions->data()[this->systemPath->data()[i + 1]];
                Globals::Canvas->DrawLine((int)this->scratchVector2.x, (int)this->scratchVector2.y,
                                             (int)this->scratchVector.x, (int)this->scratchVector.y);
            }
            if ((uint32_t)this->field_0xe8 < this->systemPath->size() - 1) {
                this->scratchVector = *positions->data()[this->systemPath->data()[this->field_0xe8]];
                this->scratchVector2 = *positions->data()[this->systemPath->data()[this->field_0xe8 + 1]];
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
                                 drawLayout->field_0x2c_rowHeight, (unsigned char) (0));
            tmp = system->getName();
            Globals::Canvas->SetColor(255, 128, 0, (unsigned char)(this->alpha ^ 255));
            Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), tmp,
                                Globals::Canvas->GetImage2DWidth((unsigned int) (this->systemNameImage)) +
                                drawLayout->field_0x2c_rowHeight * 2,
                                drawLayout->field_0xc_leftMargin +
                                drawLayout->field_0x2c_rowHeight + 2, false);
            Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(this->alpha ^ 255));
            int textX = Globals::Canvas->GetImage2DWidth(this->systemNameImage) + drawLayout->field_0x2c_rowHeight * 2;
            Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(system->getRace() + 406), textX,
                drawLayout->field_0x2c_rowHeight + drawLayout->field_0xc_leftMargin + drawLayout->field_0x4 + 2, false);
            static const int securityColors[4][3] = {{255,42,0},{255,108,0},{237,237,0},{0,237,0}};
            int security = system->getSecurityLevel();
            Globals::Canvas->SetColor(securityColors[security][0], securityColors[security][1], securityColors[security][2], (unsigned char)(this->alpha ^ 255));
            Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(security + 402), textX,
                drawLayout->field_0x2c_rowHeight + drawLayout->field_0xc_leftMargin + drawLayout->field_0x4 * 2 + 2, false);
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
    tmp.copy((String *) ((GameText *) (Globals::gameText))->getText(177), false);
    ((Layout *) (Globals::layout))->drawHeader(tmp);
    ((Layout *) (Globals::layout))->drawEmptyFooter(1);
    if ((Globals::status->getShip()->hasJumpDriveIntegrated() || this->jumpMapModeB) && this->jumpCost > 0 &&
        this->selectedSystem >= 0 && this->selectedSystem != Globals::status->getSystem()->getIndex()) {
        String cost = *Globals::gameText->getText(578) + String(" ", false) +
                      (String(this->jumpCost) + String(" / ", false) + String(this->cargoAmount));
        if (this->jumpCost > this->cargoAmount) Globals::Canvas->SetColor(0xff2a00ffu);
        int *layoutWords = reinterpret_cast<int *>(Globals::layout);
        Globals::Canvas->DrawString(Globals::font, cost,
            layoutWords[183] + layoutWords[185] / 2 - Globals::Canvas->GetTextWidth(Globals::font, cost) / 2,
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
        Array<Station *> *stations = this->stations;
        Globals::status->departStation(stations->data()[selected]);
        Level::programmedStation = nullptr;
        Level::setInitStreamOut();
        Globals::status->jumpgateUsed();
        int used = Globals::status->getJumpgateUsed();
        if (jump) {
            used = this->jumpMapModeB;
        }
        if (jump && used != 0) {
            int toSystem = stations->data()[selected]->getSystem();
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
            Level::programmedStation = target;
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
        Achievements::gAchievements->resetNewMedals();
    }

cleanup: {
        Array<Station *> *stations = this->stations;
        for (uint32_t i = 0; i < stations->size(); i++) {
            if (i != (uint32_t) this->selectedStation) {
                delete (*stations)[i];
                (*stations)[i] = 0;
            }
        }
        delete stations;
        this->stations = (Array<Station *> *) 0;
    }
    ((FModSound *) (Globals::sound))->stop(0x66);
    Globals::switch_to_target_setting = 1;

    ApplicationManager::gAppManager->SetCurrentApplicationModule(2);
}

static inline float absf_end(float v) {
    return v < 0.0f ? -v : v;
}

int StarMap::OnTouchEnd(int x, int y) {
    String help;

    if (this->choiceVisible != 0) {
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
            Level::programmedStation = Globals::status->getStation();
            this->alienJumpPending = 0;
            this->exitRequested = 1;
            Level::doInstantJump = 1;
            Level::energyCellsForNextJump = this->jumpCost;
            Globals::Canvas->CameraSetCurrent(this->prevCamera);
            return 1;
        }
        if (!this->pad_0xa8_a && this->mode == 3) {
            if (this->jumpMapModeB && this->cargoAmount < this->jumpCost) {
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
                if (this->jumpMapModeB) {
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
            this->easeX->SetRange(this->easeX->GetMaxValue(), this->easeX->GetMinValue());
            this->easeY->SetRange(this->easeY->GetMaxValue(), this->easeY->GetMinValue());
            this->easeZ->SetRange(this->easeZ->GetMaxValue(), this->easeZ->GetMinValue());
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
        if (this->mode == 0) {
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
                Vector p;
                p = this->planetGeom->getPosition();
                this->scratchVector = p;
                this->scratchVector.z -= 500.0f;
                p = AbyssEngine::AEMath::MatrixGetPosition(*static_cast<Matrix *>(Globals::Canvas->CameraGetLocal(Globals::Canvas->CameraGetCurrent())));
                this->scratchVector2 = p;
                this->easeX->SetRange(this->scratchVector2.x, this->scratchVector.x);
                this->easeY->SetRange(this->scratchVector2.y, this->scratchVector.y);
                this->easeZ->SetRange(this->scratchVector2.z, this->scratchVector.z);
                this->momentumFactor = 0.0f;
                this->velocityX = 0.0f;
                this->velocityY = 0.0f;
            } else if (this->selectedSystem >= 0) {
                this->lastSelectedSystem = -1;
                this->pathAnim = 1;
            }
        } else if (this->selectedStation >= 0) {
            if (this->lastSelectedStation == this->selectedStation) {
                if (this->choiceWindow == 0) {
                    this->choiceWindow = new ChoiceWindow();
                }
                Station *station = this->stations->data()[this->selectedStation];
                Status *status = Globals::status;
                int stationIndex = station->getIndex();
                if (stationIndex == status->getStation()->getIndex()) {
                    this->choiceWindow->set(*Globals::gameText->getText(419));
                    this->choiceVisible = 1;
                    return 0;
                }

                Mission *campaign = (Mission *)(intptr_t)status->getCampaignMission();
                int campaignId = status->getCurrentCampaignMission();
                Ship *ship = status->getShip();
                int warningText = -1;
                if (campaign != nullptr && campaign->getTargetStation() == stationIndex) {
                    if (campaignId == 24 &&
                        (ship->getFirstEquipmentOfSort(13) == nullptr ||
                         ship->getFirstEquipmentOfSort(17) == nullptr)) {
                        warningText = 532;
                    } else if (campaignId == 135 && ship->getFirstEquipmentOfSort(19) == nullptr) {
                        warningText = 3213;
                    } else if ((campaignId == 91 && ship->getMaxPassengers() <= 9) ||
                               (campaignId == 94 && ship->getMaxPassengers() == 0)) {
                        warningText = 3214;
                    } else if (campaignId == 105 && !ship->hasEquipment(206, 1)) {
                        warningText = 3217;
                    } else if (campaignId == 139 && ship->getIndex() != 42) {
                        // Android rodata 0x1fde40: ship classes used by this campaign gate.
                        static const int shipClasses[64] = {3,0,8,3,2,0,3,0,9,1,0,8,2,0,0,0,2,0,2,3,3,2,0,8,8,8,0,0,0,8,3,2,8,0,0,2,0,0,0,1,0,1,1,2,1,3,3,3,3,1,1,0,8,1,1,0,3,2,0,0,8,1,3,1};
                        if (shipClasses[ship->getIndex()] != 1 || !ship->hasEquipment(190, 1)) warningText = 3215;
                    } else if (campaignId == 142 &&
                               (!ship->getFirstEquipmentOfSort(33) || !ship->hasEquipment(197, 15) ||
                                !ship->getFirstEquipmentOfSort(35))) {
                        warningText = 3216;
                    } else if (campaignId == 142 && ship->getFreeSpace() == 0) {
                        warningText = 3218;
                    }
                }
                if (warningText >= 0) {
                    this->choiceWindow->set(*Globals::gameText->getText(warningText));
                    this->suppressNextClose = 1;
                    this->choiceVisible = 1;
                    return 0;
                }

                if (this->jumpMapModeB) {
                    if (this->cargoAmount < this->jumpCost && !ship->hasVolatileGoods()) {
                        if (this->jumpCost != 1) this->choiceWindow->set(*Globals::gameText->getText(579));
                        else this->choiceWindow->set(*Globals::gameText->getText(582), true);
                        this->choiceVisible = 1;
                        return 0;
                    }
                    if (this->noRoute && this->cargoAmount < this->jumpCost * 2 && !ship->hasVolatileGoods()) {
                        this->choiceWindow->set(*Globals::gameText->getText(581), true);
                        this->choiceVisible = 1;
                        return 0;
                    }
                }
                String prompt;
                prompt.copy((String *) ((GameText *) (Globals::gameText))->getText(574), false);
                String separator(": ");
                prompt += separator;
                prompt += station->getName();
                String newline("\n");
                prompt += newline;
                prompt += *(String *) ((GameText *) (Globals::gameText))->getText(421);
                this->choiceWindow->set(prompt, true);
                this->choiceVisible = 1;
            } else {
                ((FModSound *) (Globals::sound))->play(0x69, 0, 0, 0.0f);
                this->stationCenterAnim = 1;
            }
            this->velocityX = absf_end(this->touchDeltaX) > 150.0f ? this->touchDeltaX : 0.0f;
            this->velocityY = absf_end(this->touchDeltaY) > 150.0f ? this->touchDeltaY : 0.0f;
        }
    }
    if (((Layout *) (layout))->helpPressed() != 0) {
        help.copy((String *) ((GameText *) (Globals::gameText))->getText(this->mode == 0 ? 628 : 631), false);
        ((Layout *) (layout))->initHelpWindow(help);
    }
    return 0;
}

void StarMap::initLights() {
    void *engine = ApplicationManager::gAppManager->GetEngine();
    ((Engine *) (engine))->LightSetMaterialColorAmbient(0.5f, 0.5f, 0.5f);
    Globals::Canvas->FogEnable(0, static_cast<AbyssEngine::FogMode>(1));
}

static inline float absf_update(float v) {
    return v < 0.0f ? -v : v;
}

void StarMap::update(int dt) {
    Matrix matrix;
    Vector tmp;

    this->lastDt = dt;
    if (this->mode == 0 || this->transitionIn != 0 || this->transitionOut != 0) {
        Array<Vector *> *positions = this->systemPositions;
        for (uint32_t i = 0; i < positions->size(); i++) {
            tmp = this->systemGeoms->data()[i]->getPosition();
            int visible = Globals::Canvas->GetScreenPosition(tmp, *positions->data()[i]);
            positions->data()[i]->z = visible != 0 ? 1.0f : -1.0f;
        }
    }
    if (this->mode == 3 || this->transitionIn != 0 || this->transitionOut != 0) {
        Array<Vector *> *positions = this->stationPositions;
        if (positions != 0) {
            for (uint32_t i = 0; i < positions->size(); i++) {
                tmp = AbyssEngine::AEMath::MatrixGetPosition(this->starSystemRoot->getMatrix() * this->stationGeoms->data()[i + 1]->getMatrix());
                int visible = Globals::Canvas->GetScreenPosition(tmp, *positions->data()[i]);
                positions->data()[i]->z = visible != 0 ? 1.0f : -1.0f;
            }
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
        float scale = t * -0.014f + 0.02f;
        this->markerGeom->setScaling(scale, scale, scale);
        if (this->starSystemRoot != 0 && this->centeredStation >= 0) {
            Vector start = this->systemGeoms->data()[Globals::status->field_7c]->getPosition();
            tmp = AbyssEngine::AEMath::MatrixGetPosition(this->starSystemRoot->getMatrix() *
                                                      this->stationGeoms->data()[this->centeredStation]->getMatrix());
            this->markerGeom->setPosition(start + (tmp - start) * t);
        }
        PaintCanvas *canvas = Globals::Canvas;
        canvas->CameraGetCurrent();
        tmp = AbyssEngine::AEMath::MatrixGetPosition(*static_cast<Matrix *>(
            canvas->CameraGetLocal(canvas->CameraGetCurrent())));
        this->scratchVector = tmp;
        tmp = this->markerGeom->getPosition();
        this->scratchVector -= tmp;
        tmp = AbyssEngine::AEMath::VectorNormalize(this->scratchVector);
        this->scratchVector = tmp;
        this->scratchVector.x += 0.5f;
        Vector worldUp = {0.0f, 1.0f, 0.0f};
        this->markerGeom->setDirection(this->scratchVector, worldUp);
        ((AbyssEngine::Transform *) (canvas->TransformGetTransform(this->markerGeom->transform)))->Update(dt, false);
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
        __builtin_memcpy(
            &matrix, canvas->CameraGetLocal(
                canvas->CameraGetCurrent()), 0x3c);
        AbyssEngine::AEMath::MatrixSetTranslation(matrix, this->scratchVector.x, this->scratchVector.y, this->scratchVector.z);
        canvas->CameraSetLocal(canvas->CameraGetCurrent(),
                                                          *(const AbyssEngine::AEMath::Matrix *) (&matrix));
        if (absf_update(this->scratchVector.x - this->easeX->GetMaxValue()) <= 1.0f &&
            absf_update(this->scratchVector.y - this->easeY->GetMaxValue()) <= 1.0f &&
            absf_update(this->scratchVector.z - this->easeZ->GetMaxValue()) <= 1.0f) {
            if (this->transitionIn == 0) {
                if (this->stationGeoms != 0) {
                    ArrayReleaseClasses(*this->stationGeoms); delete this->stationGeoms;
                    this->stationGeoms = 0;
                }
                if (this->ringGeoms != 0) {
                    ArrayReleaseClasses(*this->ringGeoms); delete this->ringGeoms;
                    this->ringGeoms = 0;
                }
                delete[] this->stationAngles;
                this->stationAngles = (int *) 0;
                delete[] this->stationDistances;
                this->stationDistances = (int *) 0;
                if (this->usedFlags != 0) {
                    ArrayRelease(*this->usedFlags);
                    delete this->usedFlags;
                    this->usedFlags = 0;
                }
                delete this->starSystemRoot;
                this->mode = 0;
                this->starSystemRoot = (AEGeometry *) 0;
                this->systemGeoms->data()[this->selectedSystem]->setVisible(true);
            } else {
                this->mode = 3;
            }
            this->transitionIn = 0;
            this->transitionOut = 0;
        }
        return;
    }

    ((FModSound *) (Globals::sound))->setParamValue(0, 0x66, this->spin);
    if (this->mode == 3) {
        if (this->stationCenterAnim == 0 && this->dragging == 0) {
            float vx = this->momentumFactor * this->velocityX;
            float vy = this->momentumFactor * this->velocityY;
            this->velocityX = vx;
            this->velocityY = vy;
            if (absf_update(vx) > 0.5f) {
                this->yaw += vx;
            }
            if (absf_update(vy) > 0.5f) {
                this->pitch += vy;
            }
        } else if (this->stationCenterAnim != 0 && this->selectedStation >= 0) {
            int target = 0x8000 - ((int *) this->stationAngles)[this->selectedStation];
            int diff = (int) this->yaw - target;
            this->yaw += (float) (diff < 0 ? -diff : diff) * (diff < 0 ? 0.25f : -0.25f);
            if (absf_update((float) diff) < 11.0f) {
                this->stationCenterAnim = 0;
            }
            int pitchDiff = (int)this->pitch + 3096;
            this->pitch += (float)(pitchDiff < 0 ? -pitchDiff : pitchDiff) * (pitchDiff < 0 ? 0.25f : -0.25f);
            if (abs(diff) <= 4 * dt && abs(pitchDiff) <= 4 * dt) {
                this->lastSelectedStation = this->selectedStation;
                this->stationCenterAnim = 0;
            }
        }
        float speed = absf_update(this->velocityX + this->velocityY) * 0.01f;
        if (speed > 10.0f) {
            speed = 10.0f;
        }
        this->spin += speed;
        if (this->spin > 100.0f) this->spin = 0.0f;
        if (this->stationGeoms != 0) {
            Array<AEGeometry *> *geoms = this->stationGeoms;
            for (uint32_t i = 0; i < geoms->size(); i++) {
                if (geoms->data()[i] != 0) {
                    geoms->data()[i]->rotate(0.0f, (float) dt * 0.0002f, 0.0f);
                }
            }
        }
        this->yaw = (float) ((int) this->yaw % 0x10000);
        if (this->pitch < -8192.0f) {
            this->pitch = -8192.0f;
        }
        if (this->pitch > 8192.0f) {
            this->pitch = 8192.0f;
        }
        this->starSystemRoot->setRotation(this->pitch * 0.000015259f * 6.2832f,
                                         this->yaw * 0.000015259f * 6.2832f, -0.098175f);
        return;
    }

    if (this->mode == 0) {
        if (this->pulseSystem >= 0 && this->autoMode != 0 &&
            this->autoTimer < 4000) {
            this->autoTimer += dt;
            if (this->autoTimer > 3999) {
                OnTouchBegin(Globals::w >> 1, Globals::h >> 1);
            }
            float scale = ((float) this->autoTimer / 4000.0f) * 0.002f;
            this->systemGeoms->data()[this->pulseSystem]->setScaling(scale, scale, scale);
        }
        if (this->dragging == 0) {
            float vx = this->momentumFactor * this->velocityX;
            float vy = this->momentumFactor * this->velocityY;
            this->velocityX = vx;
            this->velocityY = vy;
            if (absf_update(vx) > 0.5f) {
                this->panX = (int) ((float) this->panX + vx);
            }
            if (absf_update(vy) > 0.5f) {
                this->panY = (int) ((float) this->panY + vy);
            }
        }
        int combinedX = (int)(this->cameraBaseX + (float)this->panX);
        int combinedY = (int)(this->cameraBaseZ + (float)this->panY);
        if (combinedX > 120) this->velocityX = (float)(120 - combinedX) * 0.1f;
        else if (combinedX <= -501) this->velocityX = (float)(-500 - combinedX) * 0.1f;
        if (combinedY >= 141) this->velocityY = (float)(140 - combinedY) * 0.1f;
        else if (combinedY < -400) this->velocityY = (float)(-400 - combinedY) * 0.1f;
        PaintCanvas *canvas = Globals::Canvas;
        __builtin_memcpy(
            &matrix, canvas->CameraGetLocal(
                canvas->CameraGetCurrent()), 0x3c);
        AbyssEngine::AEMath::MatrixSetTranslation(matrix,
                             (this->cameraBaseX + (float) this->panX) * 20.0f,
                             (this->cameraBaseZ + (float) this->panY) * 20.0f,
                             0.0f);
        canvas->CameraSetLocal(canvas->CameraGetCurrent(),
                                                          *(const AbyssEngine::AEMath::Matrix *) (&matrix));
        float speed = absf_update(this->velocityX + this->velocityY);
        this->spin += speed < 10.0f ? speed : 10.0f;
        if (this->spin > 100.0f) this->spin = 0.0f;
        if (this->pathAnim != 0) {
            Array<uint8_t> *vis = (Array<uint8_t> *) Globals::status->getSystemVisibilities();
            uint32_t selected = this->selectedSystem;
            if (vis != 0 && selected < vis->size() && vis->data()[selected] != 0) {
                this->scratchVector = *this->systemPositions->data()[selected];
                float targetX = (float) (Globals::w >> 1);
                float targetY = (float) (Globals::h >> 1);
                float sx = (this->scratchVector.x - targetX) / -30.0f;
                float sy = (this->scratchVector.y - targetY) / -30.0f;
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
    }
}

StarMap::StarMap(bool jumpMapMode, Mission *mission, bool param3, int param4) {
    Vector zero = {0.0f, 0.0f, 0.0f};
    this->scratchVector = zero;
    this->scratchVector2.y = 0.0f;
    this->scratchVector2.z = 0.0f;
    this->touchDeltaX = 0.0f;
    this->touchDeltaY = 0.0f;
    this->field_0x158 = 0.0f;
    this->touchStartX = 0.0f;
    this->touchStartY = 0.0f;
    this->field_0x14c = 0.0f;
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

    this->iconBuffer = new int[5];
    this->systems = (Array<SolarSystem *> *) Galaxy::gGalaxy->getSystems();
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

    for (uint32_t i = 0; i < systemsGeom->size(); i++) {
        SolarSystem *sys = this->systems->data()[i];
        int tex = sys->getTextureIndex();
        uint16_t image = (uint16_t)(tex + 0x4696);
        if (Globals::status->getCurrentCampaignMission() > 0x9d) image = 0x469b;
        if (i != 0x1b) image = (uint16_t)(tex + 0x4696);
        AEGeometry *geom = new AEGeometry(image, Globals::Canvas, false);
        this->systemGeoms->data()[i] = geom;
        float scale = 0.012f;
        if (sys->getTextureIndex() == 15 && Globals::status->getCurrentCampaignMission() <= 157) {
            scale = Globals::status->getCurrentCampaignMission() <= 105 ? 0.018f : 0.025f;
        }
        this->systemGeoms->data()[i]->setScaling(scale, scale, scale);
        float px = (float) ((int) (((100 - sys->getX()) / 100.0f) * 14000.0f) - 10000);
        float py = (float) ((int) (((100 - sys->getY()) / 100.0f) * 13000.0f) - 9000);
        float pz = (float) ((int) (((100 - sys->getZ()) / 100.0f) * 6000.0f) + 1000);
        Vector posVec = {px, py, pz};
        this->systemGeoms->data()[i]->setPosition(posVec);
        this->systemRoot->addChild(this->systemGeoms->data()[i]->transform);
        Vector *pos = new Vector(zero);
        this->systemPositions->data()[i] = pos;
    }

    AERandom::gRandom->reset();
    this->markerGeom = (AEGeometry *) 0;
    this->choiceVisible = 0;
    if (Globals::status->getCurrentCampaignMission() > 0x1f &&
        Globals::status->field_7c >= 0) {
        AEGeometry *marker = new AEGeometry((uint16_t) 0x4262, Globals::Canvas, false);
        this->markerGeom = marker;
        Vector p;
        p = this->systemGeoms->data()[Globals::status->field_7c]->getPosition();
        marker->setPosition(p);
        ((AbyssEngine::Transform *) (Globals::Canvas->TransformGetTransform(marker->transform)))->SetAnimationState(
            (AbyssEngine::AnimationMode) 2, 0);
        marker->setRotation(0.0f, 3.1416f, 0.0f);
    }

    init(jumpMapMode, mission, param3, param4);
}

static inline float absf_local(float v) {
    return v < 0.0f ? -v : v;
}

uint32_t StarMap::OnTouchBegin(int x, int y) {
    if (this->choiceVisible != 0) {
        this->choiceWindow->OnTouchBegin(x, y);
        return 0;
    }
    if (this->transitionIn != 0 || this->transitionOut != 0) {
        return 0;
    }

    void *layout = Globals::layout;
    ((Layout *) (layout))->OnTouchBegin(x, y);
    if ((this->pad_0xa8_a != 0 && this->pathAnim != 0) ||
        this->stationCenterAnim != 0) {
        return 0;
    }
    this->backButton->OnTouchBegin(x, y);
    if (((Layout *) layout)->field_0xc_leftMargin >= y || y >= Globals::h - ((Layout *) layout)->
        field_0x10_rightMargin) {
        return 0;
    }
    if (this->autoMode != 0 && this->autoTimer < 4000) {
        return 0;
    }

    void *sound = Globals::sound;
    ((FModSound *) (sound))->stop(0x66);
    ((FModSound *) (sound))->play(0x66, 0, 0, 0.0f);

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
                        ((FModSound *) (sound))->play(0x67, 0, 0, 0.0f);
                    }
                    int current = Globals::status->getSystem()->getIndex();
                    int dist = this->pathFinder->getJumpDistance(this->systems, current, this->selectedSystem);
                    this->jumpCost = dist;
                    if (dist == 0 && current != this->selectedSystem) {
                        this->jumpCost = 4;
                        if (this->systems->data()[i]->getRoutes() == 0) {
                            this->noRoute = 1;
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
        Array<Vector *> *positions = this->stationPositions;
        for (uint32_t i = 0; i < positions->size(); i++) {
            this->scratchVector = *positions->data()[i];
            if (this->scratchVector.z > 0.0f &&
                absf_local(this->scratchVector.x - fx) < (float) this->hitRadius &&
                absf_local(this->scratchVector.y - fy) < (float) this->hitRadius) {
                this->selectedStation = (int) i;
                ((FModSound *) (sound))->play(0x68, 0, 0, 0.0f);
                return 0;
            }
        }
    }
    return 0;
}

void StarMap::OnTouchMove(int x, int y) {
    Matrix matrix;

    if (this->choiceVisible != 0) {
        this->choiceWindow->OnTouchMove(x, y);
        return;
    }
    if (this->transitionIn != 0 || this->transitionOut != 0) {
        return;
    }
    void *layout = Globals::layout;
    ((Layout *) (layout))->OnTouchMove(x, y);
    if ((this->pad_0xa8_a != 0 && this->pathAnim != 0) ||
        this->stationCenterAnim != 0) {
        return;
    }
    this->backButton->OnTouchMove(x, y);
    if (this->dragging == 0) {
        return;
    }

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

    if (this->mode == 0) {
        float speed = absf_local(dx + dy);
        if (speed > 10.0f) {
            speed = 10.0f;
        }
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
        Globals::Canvas->CameraGetCurrent();
        __builtin_memcpy(&matrix, Globals::Canvas->CameraGetLocal(Globals::Canvas->CameraGetCurrent()), 0x3c);
        Globals::Canvas->CameraSetLocal(Globals::Canvas->CameraGetCurrent(), *(const AbyssEngine::AEMath::Matrix *) (&matrix));
        return;
    }

    if (absf_local(this->touchStartX - fx) > 3.0f ||
        absf_local(this->touchStartY - fy) > 3.0f) {
        this->lastSelectedStation = -1;
        this->selectedStation = -1;
    }

    float rotZ = dx * 50.0f;
    float rotX = dy * -50.0f;
    float pitch = this->pitch + rotX;
    if (pitch < -8192.0f) {
        pitch = -8192.0f;
    }
    if (pitch > 8192.0f) {
        pitch = 8192.0f;
    }
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
    static const uint16_t planetTextures[19] = {
            0x272f, 0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735,
            0x2736, 0x2737, 0x2738, 0x2739, 0x2730, 0x2737, 0x2733,
            0x272f, 0x2ddd, 0x2736, 0x2731, 0x2735};
    static const uint16_t systemNameImages[4] = {0x4a6, 0x4a3, 0x4a5, 0x4a4};
    SolarSystem *system = this->systems->data()[this->selectedSystem];
    uint32_t count = ((Array<void *> *) system->getStations())->size();

    Array<Station *> *stations = new Array<Station *>();
    this->stations = stations;
    ArraySetLength(count, *stations);
    FileRead *reader = new FileRead();
    this->stations = ((FileRead *)reader)->loadStationsBinary(system);
    delete reader;

    this->stationAngles = new int[count];
    this->stationDistances = new int[count];
    this->centeredStation = -1;
    AERandom::gRandom->setSeed((long long) system->getIndex() * 1000);

    Array<AEGeometry *> *stationGeoms = new Array<AEGeometry *>();
    this->stationGeoms = stationGeoms;
    ArraySetLength(count + 1, *stationGeoms);

    AEGeometry *root = new AEGeometry(Globals::Canvas);
    this->starSystemRoot = root;

    Array<uint8_t> *used = new Array<uint8_t>();
    this->usedFlags = used;
    ArraySetLength(stationGeoms->size(), *used);
    for (uint32_t i = 0; i < used->size(); i++) {
        (*used)[i] = 0;
    }

    for (uint32_t i = 1; i < stationGeoms->size(); i++) {
        uint32_t stationIndex = i - 1;
        int tex = this->stations->data()[stationIndex]->getTextureIndex();
        AEGeometry *geom = new AEGeometry((uint16_t)(tex + 0x4704), Globals::Canvas, false);
        stationGeoms->data()[i] = geom;
        uint32_t angleSlot;
        do {
            angleSlot = AERandom::gRandom->nextInt(used->size());
        } while ((*used)[angleSlot] != 0);
        (*used)[angleSlot] = 1;
        this->stationAngles[stationIndex] = (int)angleSlot * (0x10000 / (int)used->size());
        int dist = (i == 1) ? 0x1900 : this->stationDistances[i - 2];
        dist += AERandom::gRandom->nextInt(0x15e0) + 0x640;
        this->stationDistances[stationIndex] = dist;
        float angle = (float)this->stationAngles[stationIndex] * 0.000015259f * 6.2832f;
        Matrix rotation;
        AbyssEngine::AEMath::MatrixSetRotation(rotation, 0.0f, angle, 0.0f);
        Vector radial = {0.0f, 0.0f, (float)dist};
        geom->translate(AbyssEngine::AEMath::MatrixTransformVector(rotation, radial));
        float scale = (float)(16 * stationScale[tex]) * 0.000015259f;
        geom->setScaling(scale, scale, scale);
        this->scratchVector = AbyssEngine::AEMath::VectorNormalize(geom->getPosition());
        root->addChild(geom->transform);
        // Android ARM 0xc7580 + 0x3e8 literal is 0x00210200 (not a function pointer).
        if ((tex <= 21 && (0x00210200u & (1u << tex))) ||
            Globals::status->orbitHasPlanetRing(this->stations->data()[stationIndex]->getIndex())) {
            unsigned int ringTransform = 0xffffffffu;
            Globals::Canvas->TransformCreate(ringTransform);
            Globals::Canvas->TransformAddMesh(ringTransform, 0x41d5, false);
            Globals::Canvas->TransformSetLocal(ringTransform, geom->getMatrix());
            root->addChild(ringTransform);
        }
        if (this->markerGeom != 0 && this->selectedSystem == Globals::status->field_7c &&
            this->stations->data()[stationIndex]->getIndex() == Globals::status->field_80) {
            this->centeredStation = i;
        }
        if (geom) root->addChild(geom->transform);
    }

    stationGeoms->data()[1]->setVisible(false);

    Array<AEGeometry *> *rings = new Array<AEGeometry *>();
    this->ringGeoms = rings;
    ArraySetLength(count, *rings);
    for (uint32_t i = 0; i < rings->size(); i++) {
        AEGeometry *ring = new AEGeometry((uint16_t) 0x1a7b, Globals::Canvas, false);
        (*rings)[i] = ring;
        root->addChild(ring->transform);
        ring->setRotation(0.0f, (float)AERandom::gRandom->nextInt(0x0c45) / 1000.0f, 0.0f);
        float scale = (float)(this->stationDistances[i] << 1) * 0.000015259f;
        ring->setScaling(scale, scale, scale);
    }

    Vector selected;
    selected = this->systemGeoms->data()[this->selectedSystem]->getPosition();
    root->setPosition(selected);
    root->setScaling(0.0078125f, 0.0078125f, 0.0078125f);
    root->setRotation(0.0f, 90.0f, 0.0f);
    root->setRotation(-0.3927f, 0.3927f, -0.098175f);
    this->yaw = 4096.0f;
    this->pitch = -4096.0f;
    this->selectedStation = -1;
    Globals::Canvas->Image2DCreate(systemNameImages[system->getRace()], this->systemNameImage);

    if (this->stationPositions != 0) {
        ArrayReleaseClasses(*this->stationPositions); delete this->stationPositions;
        this->stationPositions = 0;
    }
    Array<Vector *> *stationPositions = new Array<Vector *>();
    this->stationPositions = stationPositions;
    ArraySetLength(this->stations->size(), *stationPositions);
    for (uint32_t i = 0; i < stationPositions->size(); i++) {
        Vector *p = new Vector{0.0f, 0.0f, 0.0f};
        (*stationPositions)[i] = p;
    }

    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetLightDirection(0.0f, -1.0f, -1.0f, 0x4000);
    if (this->planetGeom != 0) {
        delete this->planetGeom;
    }
    this->planetTexture = 0xffffffffu;
    this->planetGeom = (AEGeometry *) 0;
    uint16_t texture = planetTextures[system->getTextureIndex()];
    if (this->selectedSystem == 0x1b) {
        texture = 0x2734;
    }
    Globals::Canvas->TextureCreate(texture, this->planetTexture, false);
    AEGeometry *planet = new AEGeometry((uint16_t) 0x1a70, Globals::Canvas, false);
    this->planetGeom = planet;
    planet->setPosition(selected);
    planet->setRotation(0.0f, 0.0f, 0.0f);
    if (system->getTextureIndex() == 15) {
        int campaign = Globals::status->getCurrentCampaignMission();
        float scale = campaign > 157 ? 0.004f : (campaign > 105 ? 0.01f : 0.007f);
        planet->setScaling(scale, scale, scale);
    } else {
        planet->setScaling(0.004f, 0.004f, 0.004f);
    }
    this->systemGeoms->data()[this->selectedSystem]->setVisible(false);

    Vector lightPosition = planet->getPosition();
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetLightPosition(lightPosition.x, lightPosition.y, lightPosition.z, 0x4000);
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetLightColorAmbient(0.2f, 0.2f, 0.2f, 0x4000);
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetLightColorDiffuse(2.0f, 2.0f, 2.0f, 0x4000);
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetMaterialColorAmbient(1.0f, 1.0f, 1.0f);
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetMaterialColorDiffuse(1.0f, 1.0f, 1.0f);
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetMaterialColorSpecular(0.0f, 0.0f, 0.0f);
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->LightSetMaterialColorShininess(10.0f);
    static_cast<Engine *>(ApplicationManager::gAppManager->GetEngine())->field_0x32c = 1;
    AERandom::gRandom->setSeed((long long)Globals::status->getPlayingTime());
}

void StarMap::drawOnScreenInfo(int index, bool stationMode) {
    Array<Vector *> *positions =
            stationMode ? this->stationPositions : this->systemPositions;
    this->scratchVector = *positions->data()[index];
    float x = this->scratchVector.x;
    float y = this->scratchVector.y;
    if (x < -50.0f || x > (float) (Globals::w + 0x32) ||
        y < -50.0f || y > (float) (Globals::h + 0x32)) {
        return;
    }

    Globals::Canvas->SetColor((unsigned int) (0xffffffffu));
    int *icons = this->iconBuffer;
    for (int i = 0; i != 5; i++) {
        icons[i] = -1;
    }

    Mission *campaign = (Mission *)(intptr_t)Globals::status->getCampaignMission();
    Mission *freelance = Globals::status->getFreelanceMission();

    if (stationMode) {
        int stationId = this->stations->data()[index]->getIndex();
        if (this->stations->data()[index]->isDiscovered() != 0) {
            icons[0] = this->keyImageDiscovered;
        }
        if (Globals::status->getCurrentCampaignMission() == 52 && stationId == 74) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && Globals::status->getCurrentCampaignMission() == 116 && stationId >= 90 && stationId <= 94 &&
            (campaign->getStatusValue() & (1 << (stationId - 90))) == 0) {
            icons[1] = this->keyImageWanted;
        }
        if (Globals::status->getCurrentCampaignMission() == 120 && stationId == 93) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && Globals::status->getCurrentCampaignMission() == 125 && Globals::status->isFreighterMissionStation(stationId) != 0 &&
            (campaign->getStatusValue() & (1 << Globals::status->getFreighterMissionStationBit(stationId))) == 0) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && campaign->isEmpty() == 0) {
            bool missionTarget = false;
            if (campaign->getType() == 163 && Globals::status->field_90 != nullptr) {
                for (uint32_t i = 0; i < Globals::status->field_90->size(); ++i) {
                    if ((*Globals::status->field_90)[i] == stationId) {
                        missionTarget = true;
                        break;
                    }
                }
            }
            bool excluded = Globals::status->getCurrentCampaignMission() == 52 || Globals::status->getCurrentCampaignMission() == 120 || Globals::status->getCurrentCampaignMission() == 128 ||
                            Globals::status->getCurrentCampaignMission() == 130 || (Globals::status->getCurrentCampaignMission() >= 148 && Globals::status->getCurrentCampaignMission() < 152);
            if (excluded) {
                missionTarget = false;
            } else {
                if (Globals::status->getCurrentCampaignMission() == 59 && stationId == 101) {
                    missionTarget = true;
                }
                if (campaign->getTargetStation() == stationId ||
                    (campaign->getTargetStation() < 0 && Globals::status->getCurrentCampaignMission() >= 33 && Globals::status->field_80 == stationId)) {
                    missionTarget = true;
                }
            }
            if (missionTarget) {
                icons[1] = this->keyImageWanted;
            }
        }
        if (freelance != nullptr && freelance->isEmpty() == 0) {
            int target = freelance->getTargetStation();
            if (freelance->getType() == 14 && freelance->getAgent() != nullptr) {
                target = freelance->getAgent()->getStation();
            }
            if (stationId == target) {
                icons[2] = this->keyImageMission;
            }
        }
        int current = Globals::status->getStation()->getIndex();
        if (this->selectedSystem >= 0 &&
            this->systems->data()[this->selectedSystem]->getWarpGateIndex() == stationId) {
            icons[3] = this->keyImageCurrent;
        }
        Array<PendingProduct *> *pendingProducts = Globals::status->pendingProducts;
        if (pendingProducts != nullptr) {
            for (uint32_t i = 0; i < pendingProducts->size(); ++i) {
                PendingProduct *product = (*pendingProducts)[i];
                if (product != nullptr && product->stationIndex == stationId) {
                    icons[4] = this->keyImageRetreat;
                    break;
                }
            }
        }
        int textW = Globals::Canvas->GetTextWidth(Globals::font, this->stations->data()[index]->getName());
        int drawX = (int) (x - (float) (textW / 2));
        int drawY = (int) (y + (float) (this->iconWidth >> 1) - 3.0f);
        if (current == stationId) {
            int pulse = (int)(((Layout *) Globals::layout)->getPulseValue(0.005f) * (255 - this->alpha));
            Globals::Canvas->SetColor(0xff, 0xff, 0xff, pulse);
            Globals::Canvas->DrawImage2D(this->currentMarkerIcon, (int)x, (int)y, 0x11, 0x44);
        }
        if (this->selectedStation == index) {
            Globals::Canvas->SetColor(0xff, 0x80, 0, (unsigned char)~this->alpha);
            Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), this->stations->data()[index]->getName(), drawX,
                                                          drawY, false);
            Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                        (unsigned char) (0xff), (unsigned char)~this->alpha);
            if (this->stations->data()[index]->getTecLevel() > 0) {
                String line = *Globals::gameText->getText(133) + String(": ", false) +
                              String(String(this->stations->data()[index]->getTecLevel()), false);
                Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), line, drawX,
                                                              drawY + ((Layout *) Globals::layout)->field_0x4,
                                                              false);
            }
            Globals::Canvas->DrawImage2D((unsigned int) (this->selectIcon), (int) x, (int) y, 0x11, 0x44);
        } else {
            Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                        (unsigned char) (0xff), (unsigned char)~this->alpha);
            Globals::Canvas->DrawImage2D((unsigned int) (this->cursorIcon), (int) x, (int) y, 0x11, 0x44);
            Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), this->stations->data()[index]->getName(), drawX,
                                                          drawY, false);
        }
    } else {
        int systemId = this->systems->data()[index]->getIndex();
        if (this->systems->data()[index]->isFullyDiscovered() != 0) {
            icons[0] = this->keyImageDiscovered;
        }
        if (Globals::status->getCurrentCampaignMission() == 52 && this->systems->data()[index]->getStationEnumIndex(74) >= 0) {
            icons[1] = this->keyImageWanted;
        }
        if (Globals::status->getCurrentCampaignMission() == 120 && this->systems->data()[index]->getStationEnumIndex(93) >= 0) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && Globals::status->getCurrentCampaignMission() == 125) {
            int warpGate = this->systems->data()[index]->getWarpGateIndex();
            if (Globals::status->isFreighterMissionStation(warpGate) != 0 &&
                (campaign->getStatusValue() & (1 << Globals::status->getFreighterMissionStationBit(warpGate))) == 0) {
                icons[1] = this->keyImageWanted;
            }
        }
        if (campaign != nullptr && campaign->isEmpty() == 0) {
            bool missionTarget = false;
            if (campaign->getType() == 163 && Globals::status->field_90 != nullptr) {
                for (uint32_t i = 0; i < Globals::status->field_90->size(); ++i) {
                    if (this->systems->data()[index]->getStationEnumIndex((*Globals::status->field_90)[i]) >= 0) {
                        missionTarget = true;
                        break;
                    }
                }
            }
            bool excluded = Globals::status->getCurrentCampaignMission() == 52 || Globals::status->getCurrentCampaignMission() == 120 || Globals::status->getCurrentCampaignMission() == 128 ||
                            Globals::status->getCurrentCampaignMission() == 130 || (Globals::status->getCurrentCampaignMission() >= 148 && Globals::status->getCurrentCampaignMission() < 152);
            if (excluded) {
                missionTarget = false;
            } else {
                if (Globals::status->getCurrentCampaignMission() == 59 && systemId == 23) {
                    missionTarget = true;
                }
                if (this->systems->data()[index]->getStationEnumIndex(campaign->getTargetStation()) >= 0 ||
                    (campaign->getTargetStation() < 0 && Globals::status->getCurrentCampaignMission() >= 33 &&
                     this->systems->data()[index]->getStationEnumIndex(Globals::status->field_80) >= 0)) {
                    missionTarget = true;
                }
            }
            if (missionTarget) {
                icons[1] = this->keyImageWanted;
            }
        }
        if (freelance != nullptr && freelance->isEmpty() == 0) {
            int target = freelance->getTargetStation();
            if (freelance->getType() == 14 && Globals::status->getShip()->hasCargo(115, 1) &&
                freelance->getAgent() != nullptr) {
                target = freelance->getAgent()->getStation();
            }
            if (this->systems->data()[index]->getStationEnumIndex(target) >= 0) {
                icons[2] = this->keyImageMission;
            }
        }
        Array<PendingProduct *> *pendingProducts = Globals::status->pendingProducts;
        if (pendingProducts != nullptr) {
            for (uint32_t i = 0; i < pendingProducts->size(); ++i) {
                PendingProduct *product = (*pendingProducts)[i];
                if (product != nullptr && this->systems->data()[index]->getStationEnumIndex(product->stationIndex) >= 0) {
                    icons[4] = this->keyImageRetreat;
                    break;
                }
            }
        }
        int textW = Globals::Canvas->GetTextWidth(Globals::font, this->systems->data()[index]->getName());
        int drawX = (int) (x - (float) (textW / 2));
        int drawY = (int) (y + (float) (this->iconWidth >> 1) - 3.0f);
        int currentSystem = Globals::status->getSystem()->getIndex();
        if (currentSystem == systemId) {
            int pulse = (int)(((Layout *) Globals::layout)->getPulseValue(0.005f) * this->alpha);
            Globals::Canvas->SetColor(0xff, 0xff, 0xff, pulse);
            Globals::Canvas->DrawImage2D(this->currentMarkerIcon, (int)x, (int)y, 0x11, 0x44);
        }
        if (this->selectedSystem == index) {
            Globals::Canvas->SetColor((unsigned char) (0xff), (unsigned char) (0x80),
                                                        (unsigned char) (0), (unsigned char) (this->alpha));
        } else if (this->selectedSystem >= 0) {
            Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(this->alpha < 64 ? this->alpha : 64));
        }
        Globals::Canvas->DrawString((unsigned int) (long) (Globals::font), this->systems->data()[index]->getName(), drawX, drawY,
                                                      false);
        int objectAlpha = (this->selectedSystem == index || this->selectedSystem < 0) ? this->alpha : (this->alpha < 64 ? this->alpha : 64);
        Globals::Canvas->SetColor(255, 255, 255, (unsigned char)objectAlpha);
        if (this->systems->data()[index]->hasNoOwner() == 0) {
            uint32_t image = this->raceImageDefault;
            int race = this->systems->data()[index]->getRace();
            if (race == 2) {
                image = this->raceImageB;
            } else if (race == 1) {
                image = this->raceImageA;
            } else if (race == 0) {
                image = this->raceImageNeutral;
            }
            Globals::Canvas->DrawImage2D((unsigned int) (image), drawX, drawY - 3, 0x11, 0x12);
        }
        if (index == 26 && Globals::status->field_114 == 3) {
            Globals::Canvas->DrawImage2D(this->image_0x134, drawX,
                (int)((float)drawY + (float)Globals::layout->field_0x2c_rowHeight * 0.5f), 0x11, 0x12);
        }
        if (this->selectedSystem == index) {
            if (this->systems->data()[index]->hasNoOwner() == 0) {
                Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(this->systems->data()[index]->getRace() + 406),
                                   drawX, drawY + ((Layout *)Globals::layout)->field_0x4, false);
            }
            int security = this->systems->data()[index]->getSecurityLevel();
            if (systemId == 26 && Globals::status->field_114 > 1) {
                security = 3;
            }
            static const unsigned char securityColor[4][3] = {
                    {0xff, 0x2a, 0x00}, {0xff, 0x6c, 0x00},
                    {0xed, 0xed, 0x00}, {0x00, 0xed, 0x00}};
            Globals::Canvas->SetColor(securityColor[security][0], securityColor[security][1],
                             securityColor[security][2], this->alpha);
            int securityY = drawY + ((this->systems->data()[index]->hasNoOwner() == 0) ?
                    2 * ((Layout *)Globals::layout)->field_0x4 : ((Layout *)Globals::layout)->field_0x4);
            Globals::Canvas->DrawString(Globals::font, *Globals::gameText->getText(security + 402),
                               drawX, securityY, false);
            Globals::Canvas->SetColor(0xff, 0xff, 0xff, this->alpha);
        }
        if (this->selectedSystem == index) {
            Globals::Canvas->DrawImage2D((unsigned int) (this->selectIcon), (int) x, (int) y, 0x11, 0x44);
        } else {
            Globals::Canvas->DrawImage2D((unsigned int) (this->cursorIcon), (int) x, (int) y, 0x11, 0x44);
            if (this->selectedSystem >= 0) Globals::Canvas->SetColor(255, 255, 255, (unsigned char)(this->alpha < 64 ? this->alpha : 64));
        }
    }

    float iconY = (float) (int) ((this->scratchVector.y - (float) (this->iconWidth >> 1)) + 10.0f);
    float iconX = (float) (int) (this->scratchVector.x + (float) (this->iconWidth >> 1) - 7.0f);
    int iconRow = this->missionIconWidth;
    for (int i = 0; i != 5; i++) {
        int image = icons[i];
        if (image != -1) {
            if (i == 0) {
                int dx = Globals::iPadHD != 0 ? 12 : 18;
                int dy = Globals::iPadHD != 0 ? 24 : 35;
                Globals::Canvas->DrawImage2D((unsigned int) ((uint32_t) image), (int) iconX - dx,
                                    (int) iconY + this->iconWidth - dy);
            } else {
                Globals::Canvas->DrawImage2D((unsigned int) ((uint32_t) image), (int) iconX, (int) iconY);
                iconY += iconRow;
            }
        }
    }
}

int StarMap::init(bool jumpMapMode, Mission *mission, bool param3, int param4) {
    Matrix matrix;
    Vector pos;

    PaintCanvas *canvas = Globals::Canvas;
    canvas->FogEnable(0, AbyssEngine::FogMode_dummy);
    this->autoMode = (uint8_t) param3;
    this->pad_0xa8_a = (uint8_t) jumpMapMode;
    this->pulseSystem = param4;

    canvas->Image2DCreate(0x4a1, this->raceImageNeutral);
    canvas->Image2DCreate(0x49c, this->raceImageA);
    canvas->Image2DCreate(0x49f, this->raceImageB);
    canvas->Image2DCreate(0x49e, this->raceImageDefault);
    canvas->Image2DCreate(0x452, this->keyImageRetreat);
    canvas->Image2DCreate(0x4a2, this->keyImageDiscovered);
    canvas->Image2DCreate(0x453, this->keyImageCurrent);
    canvas->Image2DCreate(0x455, this->keyImageMission);
    canvas->Image2DCreate(0x454, this->keyImageWanted);
    canvas->Image2DCreate(0x48c, this->selectIcon);
    canvas->Image2DCreate(0x48a, this->cursorIcon);
    canvas->Image2DCreate(0x4fd, this->currentMarkerIcon);
    canvas->Image2DCreate(0x545, this->image_0x134);

    this->iconWidth = canvas->GetImage2DWidth((unsigned int) (this->selectIcon));
    this->missionIconWidth = canvas->GetImage2DWidth((unsigned int) (this->keyImageMission));
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
    this->dragScale = ((Layout *) Globals::layout)->field_0x90;
    this->prevCamera = (uint32_t)canvas->CameraGetCurrent();
    canvas->CameraCreate(this->camera);
    canvas->CameraSetPerspective(this->camera, 1.1504f, 200.0f, 64000.0f);
    AbyssEngine::AEMath::MatrixSetTranslation(matrix, 0.0f, 0.0f, -2500.0f);
    AbyssEngine::AEMath::MatrixSetRotation(matrix, 0.0f, 3.1416f, 0.0f);
    AbyssEngine::AEMath::MatrixSetTranslation(matrix, (float)(20 * (int)this->cameraBaseX),
                                            (float)(20 * (int)this->cameraBaseZ), 0.0f);
    canvas->CameraSetLocal(this->camera, matrix);
    canvas->CameraSetCurrent((unsigned int) (this->camera));

    int campaign = Globals::status->getCurrentCampaignMission();
    this->isGalaxyMode = campaign > 0xf;
    this->mode = campaign > 0xf ? 0 : 3;
    this->selectedSystem = Globals::status->getSystem()->getIndex();

    if (param3 != 0) {
        pos = this->systemGeoms->data()[param4]->getPosition();
        this->scratchVector = pos;
    } else if (!jumpMapMode || this->isGalaxyMode == 0) {
        pos = this->systemGeoms->data()[this->selectedSystem]->getPosition();
        this->scratchVector = pos;
    } else {
        Station *target = reinterpret_cast<Station *>(static_cast<intptr_t>(Galaxy::gGalaxy->getStation(mission->getTargetStation())));
        if (target && target->getSystem() >= 0) {
            this->scratchVector = this->systemGeoms->data()[target->getSystem()]->getPosition();
        } else if (Globals::status->field_7c >= 0) {
            this->scratchVector = this->systemGeoms->data()[Globals::status->field_7c]->getPosition();
        }
        if (target && target != Globals::status->getStation()) delete target;
    }

    __builtin_memcpy(&matrix, canvas->CameraGetLocal(canvas->CameraGetCurrent()), 0x3c);
    AbyssEngine::AEMath::MatrixSetTranslation(matrix, this->scratchVector.x, this->scratchVector.y, 0.0f);
    canvas->CameraSetLocal(this->camera, matrix);
    this->cameraBaseX = this->scratchVector.x / 20.0f;
    this->cameraBaseZ = this->scratchVector.y / 20.0f;
    if (this->mode == 3) {
        this->selectedStation = -1;
        initStarSystem();
        this->scratchVector = this->planetGeom->getPosition();
        this->scratchVector.z -= 500.0f;
        AbyssEngine::AEMath::MatrixSetTranslation(matrix, this->scratchVector.x, this->scratchVector.y, this->scratchVector.z);
        canvas->CameraSetLocal(this->camera, matrix);
    } else {
        this->lastSelectedSystem = this->selectedSystem;
        Array<Station *> *stations = this->stations;
        if (stations != 0) {
            ArrayReleaseClasses(*stations);
            delete stations;
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
    String *back = (String *) ((GameText *) (Globals::gameText))->getText(0x190);
    this->backButton = new TouchButton(
        *back, 0, Globals::w - ((Layout *) Globals::layout)->field_0x2c_rowHeight,
        Globals::h - ((Layout *) Globals::layout)->field_0x2c_rowHeight, 0x22);
    this->systemPath = (Array<int> *) 0;
    this->choiceWindow = new ChoiceWindow();
    this->pathFinder = new SystemPathFinder();

    if (jumpMapMode && this->mode == 0) {
        this->targetSystem = -1;
        this->selectedSystem = -1;
        bool usable = !mission->isEmpty() && (mission->isVisible() || mission->getType() == 14);
        bool hasRoutes = this->mode == 3 || Globals::status->getSystem()->getRoutes() != nullptr;
        if (usable) {
            for (uint32_t i = 0; i < this->systems->size(); ++i) {
                Array<int> *ids = reinterpret_cast<Array<int> *>(this->systems->data()[i]->getStations());
                if (ids) {
                    for (uint32_t j = 0; j < ids->size(); ++j) {
                        if (mission->getTargetStation() == ids->data()[j]) {
                            this->targetSystem = (int)i;
                            if (hasRoutes && this->mode == 3) {
                                this->lastSelectedStation = (int)j;
                                this->selectedStation = (int)j;
                                this->stationCenterAnim = 1;
                            }
                            break;
                        }
                    }
                }
            }
        }
        if (usable && hasRoutes && this->targetSystem == -1 && mission->getTargetStation() == -1 && Globals::status->field_7c >= 0)
            this->targetSystem = Globals::status->field_7c;
        if (usable && hasRoutes && this->targetSystem >= 0) {
            this->systemPath = this->pathFinder->getSystemPath(this->systems,
                Globals::status->getSystem()->getIndex(), this->targetSystem);
        }
        this->pathAnim = 1;
        this->momentumFactor = 0.9f;
        this->selectedSystem = this->targetSystem;
    }

    this->keyBoxWidth = 0;
    static const int keyTextIds[6] = {0x190, 0x191, 0x223, 0x22c, 0x22b, 0x112};
    for (int i = 0; i < 6; i++) {
        int width = canvas->GetTextWidth((unsigned int) (long) (Globals::font),
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

    this->bgLayer0 = new AEGeometry((uint16_t) 0x41d2, canvas, false);
    this->bgLayer0->setRotation(0.0f, 3.1416f, 0.0f);
    this->bgLayer0->setPosition(-3000.0f, -2500.0f, 0.0f);
    this->bgLayer1 = new AEGeometry((uint16_t) 0x41d3, canvas, false);
    this->bgLayer1->setRotation(0.0f, 3.1416f, 0.0f);
    this->bgLayer1->setPosition(-3000.0f, -2500.0f, 0.0f);
    this->bgLayer2 = new AEGeometry((uint16_t) 0x41d4, canvas, false);
    this->bgLayer2->setRotation(0.0f, 3.1416f, 0.0f);
    this->bgLayer2->setPosition(-3000.0f, -2500.0f, 0.0f);
    return 0;
}
