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



void *SystemPathFinder_dtor(void *finder);


float EaseInOut_GetValue(void *ease);

float EaseInOut_GetMinValue(void *ease);


int Station_getSystem(void *station);


int Station_getIndex(void *station);

void MatrixGetPosition(Vector *out, void *matrix);


void MatrixSetTranslation(void *matrix, float x, float y, float z);

void VectorNormalize(Vector * out, Vector * value);

void EaseInOut_Update(void *ease, float dt);

float EaseInOut_GetCurrentValue(void *ease);



void FileRead_ctor(void *reader);

void *FileRead_dtor(void *reader);




int Station_getTextureIndex(void *station);


int Station_getTecLevel(void *station);



void MatrixSetRotation(void *matrix, float x, float y, float z, float w);

void *SystemPathFinder_ctor(void *finder);

uint8_t StarMap::missionChanged() {
    return this->missionChangedFlag;
}

void StarMap::render() {
    PaintCanvas::gCanvas->SetColor((unsigned int) (0xffffffffu));
    this->bgLayer0->render();
    this->bgLayer1->render();
    this->bgLayer2->render();
    this->systemRoot->render();
    PaintCanvas::gCanvas->End3d();
    PaintCanvas::gCanvas->Begin3d();
    if (this->starSystemRoot != 0) {
        PaintCanvas::gCanvas->SetTexture((unsigned int) (this->planetTexture), (unsigned int) (0xffffffffu));
        PaintCanvas::gCanvas->SetBlendMode(AbyssEngine::BlendMode_dummy);
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
        operator delete(SystemPathFinder_dtor(this->pathFinder));
    }
    this->pathFinder = (SystemPathFinder *) 0;
}

void StarMap::draw() {
    String tmp;

    int mode = this->mode;
    int alpha = mode == 0 ? 0xff : 0;
    this->alpha = alpha;
    if (this->transitionIn != 0 || this->transitionOut != 0) {
        float v = EaseInOut_GetValue(this->easeZ);
        float min = EaseInOut_GetMinValue(this->easeZ);
        float max = this->easeZ->GetMaxValue();
        float t = (v - min) / (max - min);
        if (this->transitionIn != 0) {
            t = 1.0f - t;
        }
        this->alpha = (int) (t * 255.0f);
    }

    if (mode != 3 || this->transitionIn != 0 || this->transitionOut != 0) {
        PaintCanvas::gCanvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff), (unsigned char) (0xff),
                          (unsigned char) (this->alpha));
        Array<Vector *> *positions = this->systemPositions;
        Array<bool> *visibility = Status::gStatus->getSystemVisibilities();
        for (uint32_t i = 0; i < positions->size(); i++) {
            if (visibility == nullptr || i >= visibility->size() || visibility->data()[i] == 0 ||
                (this->autoMode != 0 && this->selectedSystem == (int)i && this->autoTimer < 4000)) {
                continue;
            }
            this->scratchVector = *positions->data()[i];
            Array<int> *routes = (Array<int> *) this->systems->data()[i]->getRoutes();
            if (routes != 0) {
                for (uint32_t j = 0; j < routes->size(); j++) {
                    uint32_t to = (uint32_t) routes->data()[j];
                    if (to < positions->size() && to < visibility->size() && visibility->data()[to] != 0 &&
                        (this->autoMode == 0 || this->selectedSystem != (int)to || this->autoTimer >= 4000)) {
                        this->scratchVector2 = *positions->data()[to];
                        if (this->scratchVector.z >= 0.0f || this->scratchVector2.z >= 0.0f) {
                            PaintCanvas::gCanvas->DrawLine((int) this->scratchVector.x, (int) this->scratchVector.y,
                                              (int) this->scratchVector2.x, (int) this->scratchVector2.y);
                        }
                    }
                }
            }
        }
        for (uint32_t i = 0; i < this->systems->size(); i++) {
            if (visibility != nullptr && i < visibility->size() && visibility->data()[i] != 0 &&
                (this->autoMode == 0 || this->selectedSystem != (int)i || this->autoTimer >= 4000)) {
                drawOnScreenInfo((int) i, false);
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
            PaintCanvas::gCanvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff), (unsigned char) (0xff),
                              (unsigned char) (this->alpha ^ 0xff));
            Layout *drawLayout = (Layout *) Globals::layout;
            PaintCanvas::gCanvas->DrawImage2D((unsigned int) (this->systemNameImage), drawLayout->field_0x2c_rowHeight,
                                 drawLayout->field_0xc_leftMargin +
                                 drawLayout->field_0x2c_rowHeight, (unsigned char) (0));
            ((SolarSystem *) (&tmp))->getName();
            PaintCanvas::gCanvas->DrawString((unsigned int) (long) (Globals::font), tmp,
                                PaintCanvas::gCanvas->GetImage2DWidth((unsigned int) (this->systemNameImage)) +
                                drawLayout->field_0x2c_rowHeight * 2,
                                drawLayout->field_0xc_leftMargin +
                                drawLayout->field_0x2c_rowHeight + 2, false);
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
    tmp.copy((String *) ((GameText *) (Globals::gameText))->getText(0x190), false);
    ((Layout *) (Globals::layout))->drawHeader(tmp);
    ((Layout *) (Globals::layout))->drawEmptyFooter(1);
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
        Status::gStatus->departStation(stations->data()[selected]);
        Level::programmedStation = nullptr;
        Level::setInitStreamOut();
        Status::gStatus->jumpgateUsed();
        int used = Status::gStatus->getJumpgateUsed();
        if (jump) {
            used = this->jumpMapModeB;
        }
        if (jump && used != 0) {
            int toSystem = Station_getSystem(stations->data()[selected]);
            int current = Status::gStatus->getSystem()->getIndex();
            Level::doInstantJump = static_cast<unsigned char>(toSystem != current);
            if (toSystem != current) {
                Level::energyCellsForNextJump = this->jumpCost;
            }
        } else {
            Level::doInstantJump = 0;
        }
    } else {
        if (Status::gStatus->getCurrentCampaignMission() == 3) {
            goto cleanup;
        }

        Status::gStatus->field_5c = -1;
        Status::gStatus->field_60 = -1;
        Status::gStatus->field_64 = -1;
        Status::gStatus->field_68 = -1;
        Status::gStatus->departStation((Station *) Status::gStatus->getStation());

        Station *target = this->stations->data()[this->selectedStation];
        if (target->equals((Station *) Status::gStatus->getStation()) == 0) {
            Level::programmedStation = target;
        }

        if (jump) {
            void *ship = Status::gStatus->getShip();
            if (((Ship *) (ship))->hasVolatileGoods() != 0) {
                goto no_jump;
            }
            if (((Ship *) (Status::gStatus->getShip()))->hasJumpDriveIntegrated() == 0 && this->jumpMapModeB == 0) {
                goto no_jump;
            }
            int toSystem = Station_getSystem(Level::programmedStation);
            int current = Status::gStatus->getSystem()->getIndex();
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
            if (Station_getIndex(station) == Station_getIndex(Status::gStatus->getStation())) {
                this->suppressNextClose = 0;
                return 0;
            }
        }
        if (this->alienJumpPending != 0 && this->mode == 0 &&
            this->pad_0xa8_a == 0 && this->autoMode == 0) {
            this->alienJumpPending = 0;
            this->exitRequested = 1;
            PaintCanvas::gCanvas->CameraSetCurrent((unsigned int) (this->prevCamera));
            return 0;
        }
        if (this->pad_0xa8_a == 0 && this->mode == 3) {
            if (this->jumpMapModeB == 0 ||
                this->jumpCost <= this->cargoAmount) {
                if (this->jumpMapModeA == 0) {
                    depart(true);
                    return 0;
                }
            } else if (this->jumpCost == 1 && this->jumpMapModeA == 0) {
                depart(false);
                return 0;
            }
            this->exitRequested = 1;
            PaintCanvas::gCanvas->CameraSetCurrent((unsigned int) (this->prevCamera));
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
            this->easeX->SetRange(this->scratchVector.x, this->scratchVector.x);
            this->easeY->SetRange(this->scratchVector.y, this->scratchVector.y);
            this->easeZ->SetRange(this->scratchVector.z, this->scratchVector.z);
            ((FModSound *) (Globals::sound))->play(0x6b, 0, 0, 0.0f);
            return 0;
        }

        PaintCanvas::gCanvas->CameraSetCurrent((unsigned int) (this->prevCamera));
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
                    Status::gStatus->getSystem()->systemIsInSystemRoutes(
                            this->systems->data()[this->selectedSystem]->getIndex()) ==
                    0) {
                    this->choiceWindow->set(*(String *) ((GameText *) (Globals::gameText))->getText(0x1a4), false);
                    this->choiceVisible = 1;
                    return 0;
                }
                ((FModSound *) (Globals::sound))->play(0x6a, 0, 0, 0.0f);
                initStarSystem();
                this->transitionIn = 1;
                Vector p;
                ((AEGeometry *) (&p))->getPosition();
                this->scratchVector = p;
                this->scratchVector.z += 20.0f;
                MatrixGetPosition(&p, PaintCanvas::gCanvas->CameraGetLocal(PaintCanvas::gCanvas->CameraGetCurrent()));
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
                Status *status = Status::gStatus;
                int stationIndex = station->getIndex();
                if (stationIndex == Station_getIndex(status->getStation())) {
                    this->choiceWindow->set(*(String *) ((GameText *) (Globals::gameText))->getText(419), true);
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
                    } else if (campaignId == 142 && ship->getFreeSpace() == 0) {
                        warningText = 3218;
                    }
                }
                if (warningText >= 0) {
                    this->choiceWindow->set(*(String *) ((GameText *) (Globals::gameText))->getText(warningText),
                                            true);
                    this->choiceVisible = 1;
                    return 0;
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
        }
    }
    if (((Layout *) (layout))->helpPressed() != 0) {
        help.copy((String *) ((GameText *) (Globals::gameText))->getText(0x1a5), false);
        ((Layout *) (layout))->initHelpWindow(help);
    }
    return 0;
}

void StarMap::initLights() {
    void *engine = ApplicationManager::gAppManager->GetEngine();
    ((Engine *) (engine))->LightSetMaterialColorAmbient(0.5f, 0.5f, 0.5f);
    ((Engine *) (engine))->LightEnable(true);
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
            ((AEGeometry *) (&tmp))->getPosition();
            int visible = PaintCanvas::gCanvas->GetScreenPosition(tmp, *positions->data()[i]);
            positions->data()[i]->z = visible != 0 ? 1.0f : -1.0f;
        }
    }
    if (this->mode == 3 || this->transitionIn != 0 || this->transitionOut != 0) {
        Array<Vector *> *positions = this->stationPositions;
        if (positions != 0) {
            for (uint32_t i = 0; i < positions->size(); i++) {
                ((AEGeometry *) (&tmp))->getPosition();
                int visible = PaintCanvas::gCanvas->GetScreenPosition(tmp, *positions->data()[i]);
                positions->data()[i]->z = visible != 0 ? 1.0f : -1.0f;
            }
        }
    }

    if (this->markerGeom != 0) {
        float v = EaseInOut_GetValue(this->easeZ);
        float min = EaseInOut_GetMinValue(this->easeZ);
        float max = this->easeZ->GetMaxValue();
        float t = (v - min) / (max - min);
        if (this->transitionOut != 0) {
            t = 1.0f - t;
        } else if (this->transitionIn == 0 && this->mode == 0) {
            t = 1.0f;
        }
        float scale = (float) (0.5 + (double) t * 0.5);
        this->markerGeom->setScaling(scale);
        if (this->starSystemRoot != 0 && this->centeredStation >= 0) {
            ((AEGeometry *) (&tmp))->getPosition();
            this->markerGeom->setPosition(tmp);
        }
        PaintCanvas *canvas = PaintCanvas::gCanvas;
        canvas->CameraGetCurrent();
        MatrixGetPosition(
            &tmp, canvas->CameraGetLocal(
                canvas->CameraGetCurrent()));
        this->scratchVector = tmp;
        ((AEGeometry *) (&tmp))->getPosition();
        this->scratchVector -= tmp;
        VectorNormalize(&tmp, &this->scratchVector);
        this->scratchVector = tmp;
        this->scratchVector.x += 0.5f;
        Vector worldUp = {0.0f, 1.0f, 0.0f};
        this->markerGeom->setDirection(this->scratchVector, worldUp);
        ((AbyssEngine::Transform *) (canvas->TransformGetTransform(0)))->Update(dt, false);
    }

    if (this->transitionIn != 0 || this->transitionOut != 0) {
        float step = (float) (dt * 15);
        EaseInOut_Update(this->easeX, step);
        EaseInOut_Update(this->easeY, step);
        EaseInOut_Update(this->easeZ, step);
        tmp.x = EaseInOut_GetCurrentValue(this->easeX);
        tmp.y = EaseInOut_GetCurrentValue(this->easeY);
        tmp.z = EaseInOut_GetCurrentValue(this->easeZ);
        this->scratchVector = tmp;
        PaintCanvas *canvas = PaintCanvas::gCanvas;
        __builtin_memcpy(
            &matrix, canvas->CameraGetLocal(
                canvas->CameraGetCurrent()), 0x3c);
        MatrixSetTranslation(&matrix, this->scratchVector.x, this->scratchVector.y, this->scratchVector.z);
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
                    ArrayRemoveAll(*(this->usedFlags));
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

    ((FModSound *) (Globals::sound))->setParamValue(0, 0x66, 0.0f);
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
        }
        float speed = absf_update(this->velocityX + this->velocityY);
        if (speed > 10.0f) {
            speed = 10.0f;
        }
        this->spin += speed;
        if (this->stationGeoms != 0) {
            Array<AEGeometry *> *geoms = this->stationGeoms;
            for (uint32_t i = 0; i < geoms->size(); i++) {
                if (geoms->data()[i] != 0) {
                    geoms->data()[i]->rotate((float) dt * 0.001f, 0.0f, 0.001f);
                }
            }
        }
        this->yaw = (float) ((int) this->yaw & 0xffff);
        if (this->pitch < -90.0f) {
            this->pitch = -90.0f;
        }
        if (this->pitch > 90.0f) {
            this->pitch = 90.0f;
        }
        this->starSystemRoot->setRotation(this->pitch, 0.0f, this->yaw);
        return;
    }

    if (this->mode == 0) {
        if (this->pulseSystem >= 0 && this->autoMode != 0 &&
            this->autoTimer < 4000) {
            this->autoTimer += dt;
            if (this->autoTimer > 3999) {
                OnTouchBegin(Globals::w >> 1, Globals::h >> 1);
            }
            float scale = ((float) this->autoTimer / 4000.0f) * 1.5f;
            this->systemGeoms->data()[this->pulseSystem]->setScaling(scale);
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
        PaintCanvas *canvas = PaintCanvas::gCanvas;
        __builtin_memcpy(
            &matrix, canvas->CameraGetLocal(
                canvas->CameraGetCurrent()), 0x3c);
        MatrixSetTranslation(&matrix,
                             (this->cameraBaseX + (float) this->panX) * 20.0f,
                             0.0f,
                             (this->cameraBaseZ + (float) this->panY) * 20.0f);
        canvas->CameraSetLocal(canvas->CameraGetCurrent(),
                                                          *(const AbyssEngine::AEMath::Matrix *) (&matrix));
        if (this->pathAnim != 0) {
            Array<uint8_t> *vis = (Array<uint8_t> *) Status::gStatus->getSystemVisibilities();
            uint32_t selected = this->selectedSystem;
            if (vis != 0 && selected < vis->size() && vis->data()[selected] != 0) {
                this->scratchVector = *this->systemPositions->data()[selected];
                float targetX = (float) (Globals::w >> 1);
                float targetY = (float) (Globals::h >> 1);
                float sx = (this->scratchVector.x - targetX) / -30.0f;
                float sy = (this->scratchVector.y - targetY) / -30.0f;
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
    this->field_0xac.x = this->field_0xac.y = this->field_0xac.z = 0.0f;
    this->field_0xbc.x = this->field_0xbc.y = this->field_0xbc.z = 0.0f;
    this->field_0xcc.x = this->field_0xcc.y = this->field_0xcc.z = 0.0f;
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
    this->hitRadius = Status::gStatus->field_8c;
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

    AEGeometry *root = new AEGeometry(PaintCanvas::gCanvas);
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
        if (i == 0x1b && Status::gStatus->getCurrentCampaignMission() > 0x9d) {
            image = 0x469b;
        }
        AEGeometry *geom = new AEGeometry(image, PaintCanvas::gCanvas, false);
        systemsGeom->data()[i] = geom;
        geom->setScaling(1.0f);
        float px = (float) ((int) (((100 - sys->getX()) / 100.0f) * 20000.0f) - 10000);
        float py = (float) ((int) (((100 - sys->getY()) / 100.0f) * 18000.0f) - 9000);
        float pz = (float) ((int) (((100 - sys->getZ()) / 100.0f) * 9000.0f) + 1000);
        Vector posVec = {px, py, pz};
        geom->setPosition(posVec);
        root->addChild(geom->transform);
        Vector *pos = new Vector(zero);
        systemPositions->data()[i] = pos;
    }

    AERandom::gRandom->reset();
    this->markerGeom = (AEGeometry *) 0;
    this->choiceVisible = 0;
    if (Status::gStatus->getCurrentCampaignMission() > 0x1f &&
        Status::gStatus->field_7c >= 0) {
        AEGeometry *marker = new AEGeometry((uint16_t) 0x4262, PaintCanvas::gCanvas, false);
        this->markerGeom = marker;
        Vector p;
        ((AEGeometry *) (&p))->getPosition();
        marker->setPosition(p);
        ((AbyssEngine::Transform *) (PaintCanvas::gCanvas->TransformGetTransform(0)))->SetAnimationState(
            (AbyssEngine::AnimationMode) 2, 0);
        marker->setRotation(0.0f, 0.0f, 0.0f);
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
            Array<uint8_t> *vis = (Array<uint8_t> *) Status::gStatus->getSystemVisibilities();
            if (vis != 0 && i < vis->size() && vis->data()[i] != 0) {
                this->scratchVector = *this->systemPositions->data()[i];
                if (this->scratchVector.z > 0.0f &&
                    absf_local(this->scratchVector.x - fx) < (float) this->hitRadius &&
                    absf_local(this->scratchVector.y - fy) < (float) this->hitRadius) {
                    this->selectedSystem = (int) i;
                    if (this->stations != 0) {
                        ArrayReleaseClasses(*this->stations); ArrayRemoveAll(*(this->stations));
                        delete this->stations;
                        this->stations = (Array<Station *> *) 0;
                    }
                    this->stations = new Array<Station *>();
                    void *reader = operator new(1);
                    FileRead_ctor(reader);
                    this->stations =
                            (Array<Station *> *) ((FileRead *) (reader))->loadStationsBinary();
                    operator delete(FileRead_dtor(reader));
                    if (oldSystem != this->selectedSystem) {
                        ((FModSound *) (sound))->play(0x67, 0, 0, 0.0f);
                    }
                    int current = Status::gStatus->getSystem()->getIndex();
                    int dist = this->pathFinder->getJumpDistance(this->systems, current, this->selectedSystem);
                    this->jumpCost = dist;
                    if (dist == 0 && current != this->selectedSystem) {
                        this->jumpCost = 4;
                        if (this->systems->data()[i]->getRoutes() == 0) {
                            this->noRoute = 1;
                        }
                    }
                    if (Status::gStatus->hardCoreMode() != 0) {
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
        if (absf_local(this->touchStartX - fx) > 3.0f ||
            absf_local(this->touchStartY - fy) > 3.0f) {
            this->lastSelectedSystem = -1;
            this->selectedSystem = -1;
            this->jumpCost = 0;
        }
        PaintCanvas::gCanvas->CameraGetCurrent();
        __builtin_memcpy(&matrix, PaintCanvas::gCanvas->CameraGetLocal(PaintCanvas::gCanvas->CameraGetCurrent()), 0x3c);
        PaintCanvas::gCanvas->CameraSetLocal(PaintCanvas::gCanvas->CameraGetCurrent(), *(const AbyssEngine::AEMath::Matrix *) (&matrix));
        return;
    }

    if (absf_local(this->touchStartX - fx) <= 3.0f ||
        absf_local(this->touchStartY - fy) <= 3.0f) {
        this->lastSelectedStation = -1;
        this->selectedStation = -1;
    }

    float rotZ = dx * 16.0f;
    float rotX = dy * 16.0f;
    float pitch = this->pitch + rotX;
    if (pitch < -90.0f) {
        pitch = -90.0f;
    }
    if (pitch > 90.0f) {
        pitch = 90.0f;
    }
    float yaw = (float) ((int) (this->yaw + rotZ) & 0xffff);
    float absZ = absf_local(rotZ);
    float absX = absf_local(rotX);
    this->touchDeltaX = rotZ;
    this->touchDeltaY = rotX;
    this->velocityX = absZ > 3.0f ? rotZ : 0.0f;
    this->velocityY = absX > 3.0f ? rotX : 0.0f;
    this->yaw = yaw;
    this->pitch = pitch;
    this->starSystemRoot->setRotation(this->velocityY, yaw, this->velocityX);
}

void StarMap::drawKey() {
    PaintCanvas *canvas = Globals::Canvas;
    int imageWidth = canvas->GetImage2DWidth(this->keyImageDiscovered);
    Layout *layout = Globals::layout;
    int boxW = this->keyBoxWidth;
    int boxH = this->keyBoxHeight;
    int marginY = layout->field_0x4;
    int padY = layout->windowTopInset;
    int rightPad = layout->field_0x10_rightMargin;
    int lineH = layout->field_0x2c_rowHeight;

    String empty;
    int x = Globals::w - boxW;
    layout->drawBox(7, x, ((Globals::h - rightPad) - boxH) - padY, boxW, padY + boxH, empty, 0);
    int drawX = x + lineH;
    int textX = imageWidth + lineH + drawX;
    int y = ((Globals::h - lineH) - rightPad) - marginY;

    canvas->DrawImage2D(this->keyImageRetreat, drawX, y);
    canvas->DrawString(Globals::font, *Globals::gameText->getText(0x112), textX, y, false);
    y -= layout->field_0x4;
    canvas->DrawImage2D(this->keyImageDiscovered, drawX, y);
    canvas->DrawString(Globals::font, *Globals::gameText->getText(0x191), textX, y, false);
    y -= layout->field_0x4;
    canvas->DrawImage2D(this->keyImageCurrent, drawX, y);
    canvas->DrawString(Globals::font, *Globals::gameText->getText(0x223), textX, y, false);
    y -= layout->field_0x4;
    canvas->DrawImage2D(this->keyImageMission, drawX, y);
    canvas->DrawString(Globals::font, *Globals::gameText->getText(0x22c), textX, y, false);
    y -= layout->field_0x4;
    canvas->DrawImage2D(this->keyImageWanted, drawX, y);
    canvas->DrawString(Globals::font, *Globals::gameText->getText(0x22b), textX, y, false);
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
    void *reader = operator new(1);
    FileRead_ctor(reader);
    this->stations = (Array<Station *> *) ((FileRead *) (reader))->loadStationsBinary();
    operator delete(FileRead_dtor(reader));

    this->stationAngles = new int[count];
    this->stationDistances = new int[count];
    this->centeredStation = -1;
    AERandom::gRandom->setSeed((long long) system->getIndex() * 1000);

    Array<AEGeometry *> *stationGeoms = new Array<AEGeometry *>();
    this->stationGeoms = stationGeoms;
    ArraySetLength(count + 1, *stationGeoms);

    AEGeometry *root = new AEGeometry(PaintCanvas::gCanvas);
    this->starSystemRoot = root;

    Array<uint8_t> *used = new Array<uint8_t>();
    this->usedFlags = used;
    ArraySetLength(stationGeoms->size(), *used);
    for (uint32_t i = 0; i < used->size(); i++) {
        (*used)[i] = 0;
    }

    for (uint32_t i = 1; i < stationGeoms->size(); i++) {
        uint32_t stationIndex = i - 1;
        int tex = Station_getTextureIndex(this->stations->data()[stationIndex]);
        AEGeometry *geom = new AEGeometry((uint16_t)(tex + 0x4704), PaintCanvas::gCanvas, false);
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
        MatrixSetRotation(&rotation, 0.0f, angle, 0.0f, 0.0f);
        Vector radial = {0.0f, 0.0f, (float)dist};
        geom->translate(AbyssEngine::AEMath::MatrixTransformVector(rotation, radial));
        float scale = (float)(16 * stationScale[tex]) * 0.000015259f;
        geom->setScaling(scale);
        root->addChild(geom->transform);
        if (this->markerGeom != 0 && this->selectedSystem == Status::gStatus->field_7c &&
            Station_getIndex(this->stations->data()[stationIndex]) == Status::gStatus->field_80) {
            this->centeredStation = i;
        }
    }

    stationGeoms->data()[1]->setVisible(false);

    Array<AEGeometry *> *rings = new Array<AEGeometry *>();
    this->ringGeoms = rings;
    ArraySetLength(count, *rings);
    for (uint32_t i = 0; i < rings->size(); i++) {
        AEGeometry *ring = new AEGeometry((uint16_t) 0x1a7b, PaintCanvas::gCanvas, false);
        (*rings)[i] = ring;
        root->addChild(ring->transform);
        ring->setRotation(0.0f, (float)AERandom::gRandom->nextInt(0x0c45) / 1000.0f, 0.0f);
        float scale = (float)(this->stationDistances[i] << 1) * 0.000015259f;
        ring->setScaling(scale);
    }

    Vector selected;
    ((AEGeometry *) (&selected))->getPosition();
    root->setPosition(selected);
    root->setScaling(0.0078125f);
    root->setRotation(-0.3927f, 0.3927f, -0.098175f);
    this->yaw = 4096.0f;
    this->pitch = 0.0f;
    this->selectedStation = -1;
    PaintCanvas::gCanvas->Image2DCreate(systemNameImages[system->getRace()], this->systemNameImage);

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

    gEngine->LightSetLightDirection(0.0f, -1.0f, -1.0f, 0x4000);
    if (this->planetGeom != 0) {
        delete this->planetGeom;
    }
    this->planetTexture = 0xffffffffu;
    this->planetGeom = (AEGeometry *) 0;
    uint16_t texture = planetTextures[system->getTextureIndex()];
    if (this->selectedSystem == 0x1b) {
        texture = 0x2734;
    }
    PaintCanvas::gCanvas->TextureCreate((unsigned short) (texture), nullptr, nullptr, this->planetTexture, false);
    AEGeometry *planet = new AEGeometry((uint16_t) 0x1a70, PaintCanvas::gCanvas, false);
    this->planetGeom = planet;
    planet->setPosition(selected);
    planet->setRotation(0.0f, 0.0f, 0.0f);
    if (system->getTextureIndex() == 15) {
        int campaign = Status::gStatus->getCurrentCampaignMission();
        float scale = campaign > 157 ? 0.004f : (campaign > 105 ? 0.01f : 0.007f);
        planet->setScaling(scale);
    } else {
        planet->setScaling(0.004f);
    }
    this->systemGeoms->data()[this->selectedSystem]->setVisible(false);

    Vector lightPosition = planet->getPosition();
    gEngine->LightSetLightPosition(lightPosition.x, lightPosition.y, lightPosition.z, 0x4000);
    gEngine->LightSetLightColorAmbient(0.2f, 0.2f, 0.2f, 0x4000);
    gEngine->LightSetLightColorDiffuse(2.0f, 2.0f, 2.0f, 0x4000);
    gEngine->LightSetMaterialColorAmbient(1.0f, 1.0f, 1.0f);
    gEngine->LightSetMaterialColorDiffuse(1.0f, 1.0f, 1.0f);
    gEngine->LightSetMaterialColorSpecular(0.0f, 0.0f, 0.0f);
    gEngine->LightSetMaterialColorShininess(10.0f);
    AERandom::gRandom->setSeed((long long)Status::gStatus->getPlayingTime());
}

void StarMap::drawOnScreenInfo(int index, bool stationMode) {
    String name;
    String line;
    String value;

    Array<Vector *> *positions =
            stationMode ? this->stationPositions : this->systemPositions;
    this->scratchVector = *positions->data()[index];
    float x = this->scratchVector.x;
    float y = this->scratchVector.y;
    if (x < 0.0f || x > (float) (Globals::w + 0x32) ||
        y < 0.0f || y > (float) (Globals::h + 0x32)) {
        return;
    }

    PaintCanvas *canvas = PaintCanvas::gCanvas;
    canvas->SetColor((unsigned int) (0xffffffffu));
    int *icons = this->iconBuffer;
    for (int i = 0; i != 5; i++) {
        icons[i] = -1;
    }

    Status *status = Status::gStatus;
    Mission *campaign = (Mission *)(intptr_t)status->getCampaignMission();
    Mission *freelance = status->getFreelanceMission();
    int campaignId = status->getCurrentCampaignMission();

    if (stationMode) {
        Station *station = this->stations->data()[index];
        int stationId = station->getIndex();
        if (station->isDiscovered() != 0) {
            icons[0] = this->keyImageDiscovered;
        }
        if (campaignId == 52 && stationId == 74) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && campaignId == 116 && stationId >= 90 && stationId <= 94 &&
            (campaign->getStatusValue() & (1 << (stationId - 90))) == 0) {
            icons[1] = this->keyImageWanted;
        }
        if (campaignId == 120 && stationId == 93) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && campaignId == 125 && status->isFreighterMissionStation(stationId) != 0 &&
            (campaign->getStatusValue() & (1 << status->getFreighterMissionStationBit(stationId))) == 0) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && campaign->isEmpty() == 0) {
            bool missionTarget = false;
            if (campaign->getType() == 163 && status->field_90 != nullptr) {
                for (uint32_t i = 0; i < status->field_90->size(); ++i) {
                    if ((*status->field_90)[i] == stationId) {
                        missionTarget = true;
                        break;
                    }
                }
            }
            bool excluded = campaignId == 52 || campaignId == 120 || campaignId == 128 ||
                            campaignId == 130 || (campaignId >= 148 && campaignId < 152);
            if (excluded) {
                missionTarget = false;
            } else {
                if (campaignId == 59 && stationId == 101) {
                    missionTarget = true;
                }
                if (campaign->getTargetStation() == stationId ||
                    (campaign->getTargetStation() < 0 && campaignId >= 33 && status->field_80 == stationId)) {
                    missionTarget = true;
                }
            }
            if (missionTarget) {
                icons[2] = this->keyImageMission;
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
        int current = Station_getIndex(status->getStation());
        if (current == stationId) {
            icons[3] = this->keyImageCurrent;
        }
        if (status->field_7c >= 0) {
            SolarSystem *targetSystem = this->systems->data()[status->field_7c];
            if (stationId == (int)targetSystem->getWarpGateIndex()) {
                icons[4] = this->keyImageRetreat;
            }
        }
        Array<PendingProduct *> *pendingProducts = status->pendingProducts;
        if (pendingProducts != nullptr) {
            for (uint32_t i = 0; i < pendingProducts->size(); ++i) {
                PendingProduct *product = (*pendingProducts)[i];
                if (product != nullptr && product->stationIndex == stationId) {
                    icons[4] = this->keyImageRetreat;
                    break;
                }
            }
        }
        ((Station *) (&name))->getName();
        int textW = canvas->GetTextWidth((unsigned int) (long) (Globals::font), name);
        int drawX = (int) (x - (float) (textW / 2));
        int drawY = (int) (y + (float) (this->iconWidth >> 1) - 3.0f);
        if (current == stationId) {
            int pulse = (int)(((Layout *) Globals::layout)->getPulseValue(0.005f) * (255 - this->alpha));
            canvas->SetColor(0xff, 0xff, 0xff, pulse);
            canvas->DrawImage2D(this->currentMarkerIcon, (int)x, (int)y, 0x11, 0x44);
        }
        if (this->selectedStation == index) {
            canvas->SetColor(0xff, 0x80, 0, (unsigned char)~this->alpha);
            canvas->DrawString((unsigned int) (long) (Globals::font), name, drawX,
                                                          drawY, false);
            canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                        (unsigned char) (0xff), (unsigned char) (this->alpha));
            if (Station_getTecLevel(station) > 0) {
                line.copy((String *) ((GameText *) (Globals::gameText))->getText(133), false);
                value.Set(": ");
                value += (int)Station_getTecLevel(station);
                line += value;
                canvas->DrawString((unsigned int) (long) (Globals::font), line, drawX,
                                                              drawY + ((Layout *) Globals::layout)->field_0x4,
                                                              false);
            }
            canvas->DrawImage2D((unsigned int) (this->selectIcon), (int) x, (int) y, 0x11, 0x44);
        } else {
            canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                        (unsigned char) (0xff), (unsigned char) (this->alpha));
            canvas->DrawImage2D((unsigned int) (this->cursorIcon), (int) x, (int) y, 0x11, 0x44);
            canvas->DrawString((unsigned int) (long) (Globals::font), name, drawX,
                                                          drawY, false);
        }
    } else {
        SolarSystem *system = this->systems->data()[index];
        int systemId = system->getIndex();
        if (system->isFullyDiscovered() != 0) {
            icons[0] = this->keyImageDiscovered;
        }
        if (campaignId == 52 && system->getStationEnumIndex(74) >= 0) {
            icons[1] = this->keyImageWanted;
        }
        if (campaignId == 120 && system->getStationEnumIndex(93) >= 0) {
            icons[1] = this->keyImageWanted;
        }
        if (campaign != nullptr && campaignId == 125) {
            int warpGate = system->getWarpGateIndex();
            if (status->isFreighterMissionStation(warpGate) != 0 &&
                (campaign->getStatusValue() & (1 << status->getFreighterMissionStationBit(warpGate))) == 0) {
                icons[1] = this->keyImageWanted;
            }
        }
        if (campaign != nullptr && campaign->isEmpty() == 0) {
            bool missionTarget = false;
            if (campaign->getType() == 163 && status->field_90 != nullptr) {
                for (uint32_t i = 0; i < status->field_90->size(); ++i) {
                    if (system->getStationEnumIndex((*status->field_90)[i]) >= 0) {
                        missionTarget = true;
                        break;
                    }
                }
            }
            bool excluded = campaignId == 52 || campaignId == 120 || campaignId == 128 ||
                            campaignId == 130 || (campaignId >= 148 && campaignId < 152);
            if (excluded) {
                missionTarget = false;
            } else {
                if (campaignId == 59 && systemId == 23) {
                    missionTarget = true;
                }
                if (system->getStationEnumIndex(campaign->getTargetStation()) >= 0 ||
                    (campaign->getTargetStation() < 0 && campaignId >= 33 &&
                     system->getStationEnumIndex(status->field_80) >= 0)) {
                    missionTarget = true;
                }
            }
            if (missionTarget) {
                icons[2] = this->keyImageMission;
            }
        }
        if (freelance != nullptr && freelance->isEmpty() == 0) {
            int target = freelance->getTargetStation();
            if (freelance->getType() == 14 && status->getShip()->hasCargo(115, 1) &&
                freelance->getAgent() != nullptr) {
                target = freelance->getAgent()->getStation();
            }
            if (system->getStationEnumIndex(target) >= 0) {
                icons[2] = this->keyImageMission;
            }
        }
        if (status->field_7c >= 0 && system->getWarpGateIndex() == status->field_80) {
            icons[4] = this->keyImageRetreat;
        }
        Array<PendingProduct *> *pendingProducts = status->pendingProducts;
        if (pendingProducts != nullptr) {
            for (uint32_t i = 0; i < pendingProducts->size(); ++i) {
                PendingProduct *product = (*pendingProducts)[i];
                if (product != nullptr && system->getStationEnumIndex(product->stationIndex) >= 0) {
                    icons[4] = this->keyImageRetreat;
                    break;
                }
            }
        }
        ((SolarSystem *) (&name))->getName();
        int textW = canvas->GetTextWidth((unsigned int) (long) (Globals::font), name);
        int drawX = (int) (x - (float) (textW / 2));
        int drawY = (int) (y + (float) (this->iconWidth >> 1) - 3.0f);
        int currentSystem = status->getSystem()->getIndex();
        if (currentSystem == systemId) {
            int pulse = (int)(((Layout *) Globals::layout)->getPulseValue(0.005f) * this->alpha);
            canvas->SetColor(0xff, 0xff, 0xff, pulse);
            canvas->DrawImage2D(this->currentMarkerIcon, (int)x, (int)y, 0x11, 0x44);
        }
        if (this->selectedSystem == index) {
            canvas->SetColor((unsigned char) (0xff), (unsigned char) (0x80),
                                                        (unsigned char) (0), (unsigned char) (this->alpha));
        } else if (this->selectedSystem >= 0) {
            canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                        (unsigned char) (0xff), (unsigned char) (this->alpha));
        }
        canvas->DrawString((unsigned int) (long) (Globals::font), name, drawX, drawY,
                                                      false);
        canvas->SetColor((unsigned char) (0xff), (unsigned char) (0xff),
                                                    (unsigned char) (0xff), (unsigned char) (this->alpha));
        if (system->hasNoOwner() == 0) {
            uint32_t image = this->raceImageDefault;
            int race = system->getRace();
            if (race == 2) {
                image = this->raceImageB;
            } else if (race == 1) {
                image = this->raceImageA;
            } else if (race == 0) {
                image = this->raceImageNeutral;
            }
            canvas->DrawImage2D((unsigned int) (image), drawX, drawY - 3, 0x11, 0x12);
        }
        if (this->selectedSystem == index) {
            if (index == 26 && status->field_114 == 3) {
                canvas->DrawImage2D(this->image_0x134, drawX,
                                    drawY + (int)(((Layout *)Globals::layout)->field_0x2c_rowHeight * 0.5f), 0x11, 0x12);
            }
            if (system->hasNoOwner() == 0) {
                canvas->DrawString(Globals::font, *Globals::gameText->getText(system->getRace() + 406),
                                   drawX, drawY + ((Layout *)Globals::layout)->field_0x4, false);
            }
            int security = system->getSecurityLevel();
            if (systemId == 26 && status->field_114 > 1) {
                security = 3;
            }
            static const unsigned char securityColor[4][3] = {
                    {0xff, 0x2a, 0x00}, {0xff, 0x6c, 0x00},
                    {0xed, 0xed, 0x00}, {0xed, 0x00, 0x00}};
            if (security < 0) security = 0;
            if (security > 3) security = 3;
            canvas->SetColor(securityColor[security][0], securityColor[security][1],
                             securityColor[security][2], this->alpha);
            int securityY = drawY + ((system->hasNoOwner() == 0) ?
                    2 * ((Layout *)Globals::layout)->field_0x4 : ((Layout *)Globals::layout)->field_0x4);
            canvas->DrawString(Globals::font, *Globals::gameText->getText(security + 402),
                               drawX, securityY, false);
            canvas->SetColor(0xff, 0xff, 0xff, this->alpha);
        }
        if (this->selectedSystem == index) {
            canvas->DrawImage2D((unsigned int) (this->selectIcon), (int) x, (int) y, 0x11, 0x44);
        } else {
            canvas->DrawImage2D((unsigned int) (this->cursorIcon), (int) x, (int) y, 0x11, 0x44);
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
                canvas->DrawImage2D((unsigned int) ((uint32_t) image), (int) iconX - dx,
                                    (int) iconY + this->iconWidth - dy);
            } else {
                canvas->DrawImage2D((unsigned int) ((uint32_t) image), (int) iconX, (int) iconY);
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
    MatrixSetTranslation(&matrix, 0.0f, 0.0f, -2500.0f);
    MatrixSetRotation(&matrix, 0.0f, 3.1416f, 0.0f, 0.0f);
    canvas->CameraSetLocal(this->camera, *(const AbyssEngine::AEMath::Matrix *) (&matrix));
    canvas->CameraSetCurrent((unsigned int) (this->camera));

    int campaign = Status::gStatus->getCurrentCampaignMission();
    this->isGalaxyMode = campaign > 0xf;
    this->mode = campaign > 0xf ? 0 : 3;
    this->selectedSystem = Status::gStatus->getSystem()->getIndex();

    if (param3 != 0) {
        ((AEGeometry *) (&pos))->getPosition();
        this->scratchVector = pos;
    } else if (!jumpMapMode || this->isGalaxyMode == 0) {
        ((AEGeometry *) (&pos))->getPosition();
        this->scratchVector = pos;
    }

    MatrixSetTranslation(&matrix, this->scratchVector.x, this->scratchVector.y, 0.0f);
    canvas->CameraSetLocal(this->camera, matrix);
    this->cameraBaseX = this->scratchVector.x / 20.0f;
    this->cameraBaseZ = this->scratchVector.y / 20.0f;
    if (this->mode == 3) {
        this->selectedStation = -1;
        initStarSystem();
        this->planetGeom->getPosition();
        this->scratchVector.z -= 500.0f;
        MatrixSetTranslation(&matrix, this->scratchVector.x, this->scratchVector.y, this->scratchVector.z);
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
        void *reader = operator new(1);
        FileRead_ctor(reader);
        this->stations =
                (Array<Station *> *) ((FileRead *) (reader))->loadStationsBinary();
        operator delete(FileRead_dtor(reader));
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
    this->pathFinder = (SystemPathFinder *) SystemPathFinder_ctor(operator new(1));

    if (jumpMapMode && this->mode == 0 && mission != 0 && mission->isEmpty() == 0 &&
        (mission->isVisible() != 0 || mission->getType() == 0xe)) {
        this->targetSystem = -1;
        this->selectedSystem = -1;
        int target = mission->getTargetStation();
        if (target >= 0) {
            for (uint32_t i = 0; i < this->systems->size(); i++) {
                if (this->systems->data()[i]->getStationEnumIndex(target) >= 0) {
                    this->targetSystem = (int) i;
                    break;
                }
            }
        } else if (Status::gStatus->field_7c >= 0) {
            this->targetSystem = Status::gStatus->field_7c;
        }
        if (this->targetSystem >= 0) {
            int current = Status::gStatus->getSystem()->getIndex();
            this->systemPath =
                    this->pathFinder->getSystemPath(this->systems, current, this->targetSystem);
            this->pathAnim = 1;
            this->momentumFactor = 1.0f;
            this->selectedSystem = this->targetSystem;
        } else {
            this->pathAnim = 1;
            this->momentumFactor = 0.9f;
        }
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
    void *cargo = (void *) ((Ship *) (Status::gStatus->getShip()))->getCargo(122);
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
