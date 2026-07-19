
#include "game/menu/SpaceLounge.h"
#include "game/world/SolarSystem.h"
#include "engine/render/AEGeometry.h"
#include "game/ui/ChoiceWindow.h"
#include "game/core/CutScene.h"
#include "engine/math/EaseInOut.h"
#include "engine/math/EaseInOutMatrix.h"
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
#include "game/mission/Status.h"
#include "game/ui/TouchButton.h"

#include "engine/render/PaintCanvas.h"

static inline int pc_GetWidth(PaintCanvas *self) { return self->GetWidth(); }
static inline int pc_GetHeight(PaintCanvas *self) { return self->GetHeight(); }

static inline int &layoutMetric(Layout *layout, int off) {
    return *(int *) ((char *) layout + off);
}

namespace AbyssEngine {
    namespace AEMath {
        Vector operator+(const Vector &, const Vector &);

        Vector operator-(const Vector &, const Vector &);

        Vector operator*(const Vector &, float);

        void MatrixMultiply(Matrix &, const Matrix &);
    }
}

static void *SpaceLounge_layout_move;
static void *SpaceLounge_layout_begin;

void MatrixSetTranslation(void *matrix, float x, float y, float z);

void MatrixSetRotation(void *matrix, float x, float y, float z);

static void *SpaceLounge_touch_layout_slot;
static void *SpaceLounge_touch_help_text_slot;
static void *SpaceLounge_touch_list_help_text_slot;
static void *SpaceLounge_touch_camera_slot;
static int SpaceLounge_touch_race_vectors[64];

namespace AbyssEngine {
    namespace AERandom {
        int nextInt(void *random, int limit);
    }
}

static int *SpaceLounge_getSoundId_missionText;
static void *SpaceLounge_getSoundId_offerText;
static void *SpaceLounge_getSoundId_offer2358910;
static void *SpaceLounge_getSoundId_offer5;
static void *SpaceLounge_getSoundId_offer7;
static void *SpaceLounge_getSoundId_offer4;
static void *SpaceLounge_getSoundId_offer6;
static void *SpaceLounge_getSoundId_offer0_11;
static void *SpaceLounge_getSoundId_offer0_else;
static void *SpaceLounge_getSoundId_chance;
static void *SpaceLounge_getSoundId_offer1;
static void *SpaceLounge_getSoundId_accepted;
static void *SpaceLounge_getSoundId_specialText;
static void *SpaceLounge_getSoundId_specialRandom;

void MatrixGetRight(void *out, void *matrix);

void MatrixGetPosition(void *out, void *matrix);

void MatrixGetUp(void *out, void *matrix);

void MatrixGetLookAt(void *out, void *pos, void *target, void *up);

void MatrixGetDir(void *out, void *matrix);

static void *SpaceLounge_screen_level_slot;
static void *SpaceLounge_screen_canvas_slot;
static void *SpaceLounge_screen_projector;
static void *SpaceLounge_lounge_canvas_slot;
static void *SpaceLounge_lounge_layout_slot;
static void *SpaceLounge_lounge_image_factory_slot;
static void *SpaceLounge_lounge_text_slot;
static void *SpaceLounge_lounge_font_slot;

void *Station_getAgents(void *station);

static void *SpaceLounge_init_layout_slot;
static void *SpaceLounge_init_text_slot;
static void *SpaceLounge_init_camera_slot;

void ArrayRemove_AgentPtr(void *agent, void *array);

static void *SpaceLounge_ctor_camera_slot;
static void *SpaceLounge_start_text_slot;
static void *SpaceLounge_draw_layout_slot;
static void *SpaceLounge_draw_canvas_slot;
static void *SpaceLounge_draw_text_slot;

float Sinf(float value);

static void *SpaceLounge_update_camera_slot_a;
static void *SpaceLounge_update_camera_slot_b;
static void *SpaceLounge_update_camera_slot_c;
static void *SpaceLounge_update_random_slot;
static int *SpaceLounge_update_width_slot;
static int *SpaceLounge_update_height_slot;
static int *SpaceLounge_update_top_slot;

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

    ((Layout *) (*(void **) SpaceLounge_layout_move))->OnTouchMove(x, y);
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
        ((Layout *) (*(void **) SpaceLounge_layout_begin))->OnTouchBegin(x, y);
        this->scrollWindow->OnTouchBegin(x, y);
        return 0;
    }

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

    ((Layout *) (*(void **) SpaceLounge_layout_begin))->OnTouchBegin(x, y);
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

void SpaceLounge::OnTouchEnd(int x, int y) {
    String helpBig;
    String helpSmall;
    char matrix[60];

    this->touchDown = 0;

    if (this->choiceVisible != 0 || this->chatActive != 0) {
        int result = this->choiceWindow->OnTouchEnd(x, y);
        if (result == 1) {
            this->chatActive = 0;
        } else if (result == 0) {
            ((SpaceLounge *) (this))->onKeyPress(0x10000);
        }
        return;
    }

    if (this->mapVisible != 0) {
        if (this->starMap->OnTouchEnd(x, y) != 0) {
            this->cutScene->resetCamera();
            this->mapVisible = 0;
        }
        return;
    }

    void *layoutSlot = *(void **) &SpaceLounge_touch_layout_slot;
    void *layout = *(void **) layoutSlot;
    if (((Layout *) (layout))->OnTouchEnd(x, y) != 0) {
        if (this->listVisible != 0) {
            ((Layout *) (layout))->resetWindowDimensions();
            this->listVisible = 0;
        } else if (this->mode != 0) {
            if (this->selectedAgent >= 0) {
                void *agent = selected_agent(this);
                if (((Agent *) (agent))->isGenericAgent() != 0) {
                    ((Agent *) (agent))->setEvent(1);
                }
            }
            this->mode = 0;
            this->singleOffer = 0;
        }
        return;
    }

    if (this->listVisible != 0) {
        this->listWindow->OnTouchEnd(x, y);
        if (((Layout *) (layout))->helpPressed() != 0) {
            void *texts = *(void **) &SpaceLounge_touch_list_help_text_slot;
            void *text = ((GameText *) (*(void **) texts))->getText(0x283);
            helpSmall.Set(((String *) text)->data);
            ((Layout *) (layout))->initHelpWindow(helpSmall);
        }
        return;
    }

    switch (this->mode) {
        case 0:
            if (this->introDone == 0) {
                void *system = (void *) (long) Status::gStatus->getSystem();
                int race = ((SolarSystem *) (system))->getRace();
                int *v = &SpaceLounge_touch_race_vectors[race * 3];
                MatrixSetTranslation(matrix, (float) v[2], (float) v[0], (float) v[1]);
                MatrixSetRotation(matrix, 0.0f, 0.0f, 0.0f);
                void *cameraSlot = *(void **) &SpaceLounge_touch_camera_slot;
                void *camera = *(void **) cameraSlot;
                void *current = (void *) (long) ((PaintCanvas *) camera)->CameraGetCurrent();
                ((PaintCanvas *) camera)->CameraSetLocal((unsigned int) (long) camera,
                                                         *(const AbyssEngine::AEMath::Matrix *) current);
                if (this->cameraEase != 0) {
                    this->cameraEase->SetRange(*(AbyssEngine::AEMath::Matrix *) matrix, *(AbyssEngine::AEMath::Matrix *) matrix);
                }
                this->introDone = 1;
                this->headBobPhase = 0;
                return;
            }
            if (this->agents != 0) {
                this->hoverAgent = -1;
                this->selectedAgent = -1;
                unsigned count = (unsigned) this->agents->size();
                for (unsigned i = 0; i < count; ++i) {
                    if (hit_agent(this, x, y, i)) {
                        this->selectedAgent = i;
                        ((SpaceLounge *) (this))->onKeyPress(0x10000);
                        return;
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
                    void *agent = selected_agent(this);
                    if (i >= 5 && ((Agent *) (agent))->isGenericAgent() != 0) {
                        ((Agent *) (agent))->setEvent(1);
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
    if (((Layout *) (layout))->helpPressed() != 0) {
        void *texts = *(void **) &SpaceLounge_touch_help_text_slot;
        void *text = ((GameText *) (*(void **) texts))->getText(0x273);
        helpBig.Set(((String *) text)->data);
        ((Layout *) (layout))->initHelpWindow(helpBig);
    }
}

int SpaceLounge::getSoundId(Agent *agent) {
    String missionText;

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
                soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer0_11, 4) + 0x301;
            } else if (missionType == 0xc) {
                soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer0_else, 4) + 0x2fa;
            } else {
                void *random = *(void **) &SpaceLounge_getSoundId_offer0_else;
                int first = AbyssEngine::AERandom::nextInt(random, 2);
                if (first == 0) {
                    soundId = AbyssEngine::AERandom::nextInt(random, 4) + 0x31f;
                } else {
                    soundId = AbyssEngine::AERandom::nextInt(random, 4) + 0x309;
                }
            }
            break;
        case 1:
            soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer1, 2) + 0x30d;
            break;
        case 2:
        case 3:
        case 8:
        case 9:
        case 10:
            soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer2358910, 2) + 0x2f7;
            break;
        case 4:
            soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer4, 2) + 0x2fe;
            break;
        case 5:
            soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer5, 4) + 0x31b;
            break;
        case 6:
            soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer6, 4) + 0x323;
            break;
        case 7:
            soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer7, 4) + 0x305;
            break;
        default:
            soundId = -1;
            break;
    }

    if (offer != 1) {
        checkSpecialText = false;
        if (AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_chance, 100) < 30) {
            soundId = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_offer1, 2) + 0x30d;
            checkSpecialText = true;
        }
    }

    int dummy = 0;
    if (((Agent *) (agent))->hasAcceptedOffer() != 0) {
        dummy = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_accepted, 2);
        soundId = dummy + 0x30d;
    }

    if (checkSpecialText) {
        void *texts = *(void **) &SpaceLounge_getSoundId_specialText;
        void *text = ((GameText *) (*(void **) texts))->getText(0x334);
        if (missionText.Compare_str((String *) text) != 0) {
            text = ((GameText *) (*(void **) texts))->getText(0x338);
            if (missionText.Compare_str((String *) text) != 0) {
                text = ((GameText *) (*(void **) texts))->getText(0x33b);
                if (missionText.Compare_str((String *) text) != 0) {
                    text = ((GameText *) (*(void **) texts))->getText(0x341);
                    dummy = missionText.Compare_str((String *) text);
                    if (dummy != 0) {
                        goto special_done;
                    }
                }
            }
        }
        dummy = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_getSoundId_specialRandom, 2);
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
        void *agent = key_agent(this);
        if (key == 0x40000) {
            this->mode = 1;
        } else if (key == 0x10000 || key == 0x20000) {
            if (((Agent *) (agent))->getMission() == 0 && ((Agent *) (agent))->isGenericAgent() != 0) {
                ((Agent *) (agent))->setEvent(1);
            }
            this->mode = 0;
        } else if (key == 0x8000) {
            if (ScrollTouchBox *box = this->scrollWindow->scrollBox) {
                if (box->contentHeight - box->height > 0) {
                    box->scrollOffset -= box->height;
                    box->update(0);
                }
            }
        } else if (key == 0x4000) {
            if (ScrollTouchBox *box = this->scrollWindow->scrollBox) {
                if (box->contentHeight - box->height > 0) {
                    box->scrollOffset += box->height;
                    box->update(0);
                }
            }
        }
    }
}

void SpaceLounge::updateScreenPositions() {
    char camera[60];
    char look[60];
    char pos[12];
    char up[12];
    char target[12];
    char halfRight[12];
    char screen[12];

    if (this->agents == 0) {
        return;
    }

    void *level = *(void **) &SpaceLounge_screen_level_slot;
    Array<KIPlayer *> *enemies = ((Level *) (level))->getEnemies();
    void *canvasSlot = *(void **) &SpaceLounge_screen_canvas_slot;
    void *canvas = *(void **) canvasSlot;
    void *project = *(void **) &SpaceLounge_screen_projector;

    void *current = (void *) (long) ((PaintCanvas *) canvas)->CameraGetCurrent();
    void *local = ((PaintCanvas *) canvas)->CameraGetLocal((unsigned int) (long) canvas);
    MatrixGetRight(pos, local);
    *(Vector *) (halfRight) = *(const Vector *) (pos) * (0.5f);

    unsigned count = (unsigned) this->agents->size();
    for (unsigned i = 0; i < count; ++i) {
        KIPlayer *enemy = (*enemies)[i];
        *(Vector *) target = enemy->getPosition();

        ((void (*)(void *, void *)) project)(screen, target);
        *(Vector *) (pos) = *(const Vector *) (target) - *(const Vector *) (halfRight);
        ((void (*)(void *, void *)) project)(screen, pos);
        ((PaintCanvas *) canvas)->GetScreenPosition(*(AbyssEngine::AEMath::Vector *) canvas,
                                                    *(AbyssEngine::AEMath::Vector *) screen);

        *(Vector *) (pos) = *(const Vector *) (target) + *(const Vector *) (halfRight);
        ((void (*)(void *, void *)) project)(screen, pos);
        ((PaintCanvas *) canvas)->GetScreenPosition(*(AbyssEngine::AEMath::Vector *) canvas,
                                                    *(AbyssEngine::AEMath::Vector *) screen);

        current = (void *) (long) ((PaintCanvas *) canvas)->CameraGetCurrent();
        local = ((PaintCanvas *) canvas)->CameraGetLocal((unsigned int) (long) canvas);
        *(Matrix *) (camera) = *(const Matrix *) (local);
        MatrixGetPosition(pos, camera);
        MatrixGetUp(up, camera);
        MatrixGetLookAt(look, pos, target, up);
        *(Matrix *) (camera) = *(const Matrix *) (look);

        KIPlayer *mapped = (*enemies)[count + i];
        mapped->parentGeometry->setMatrix(*(const AbyssEngine::AEMath::Matrix *) (camera));

        ((void (*)(void *, void *)) project)(&this->silhouettePos, target);
        MatrixGetDir(pos, look);
        this->silhouettePos.x -= (*(float *) pos) * 20.0f;
        MatrixGetDir(pos, look);
        this->silhouettePos.z -= (*(float *) (pos + 8)) * 20.0f;

        mapped->setPosition(this->silhouettePos.x, this->silhouettePos.y, this->silhouettePos.z);

        if (((SolarSystem *) ((void *) (long) Status::gStatus->getSystem()))->getRace() == 0) {
            MatrixSetRotation(look, 0.0f, 0.0f, 0.0f);
            AbyssEngine::AEMath::MatrixMultiply(*(Matrix *) (camera), *(const Matrix *) (look));
        }

        enemy->parentGeometry->setMatrix(*(const AbyssEngine::AEMath::Matrix *) (camera));

        enemy->setPosition(((Vector *) target)->x, ((Vector *) target)->y, ((Vector *) target)->z);
    }
}

static inline void *button_at(SpaceLounge *self, unsigned i) {
    return (*self->buttons)[i];
}

void SpaceLounge::drawLounge() {
    String s0, s1, s2, s3, s4, s5, s6;

    void *canvasSlot = *(void **) &SpaceLounge_lounge_canvas_slot;
    void *canvas = *(void **) canvasSlot;
    void *layoutSlot = *(void **) &SpaceLounge_lounge_layout_slot;
    Layout *layout = (Layout *) *(void **) layoutSlot;
    void *factorySlot = *(void **) &SpaceLounge_lounge_image_factory_slot;
    void *factory = *(void **) factorySlot;

    if (this->mode == 0) {
        int hover = this->hoverAgent;
        if (hover >= 0) {
            void *agent = (*this->agents)[hover];
            Array<Vector *> *rects = this->agentRects;
            float *left = (float *) (*rects)[hover * 2];
            float *right = (float *) (*rects)[hover * 2 + 1];
            int pad = layoutMetric(layout, 0x94);
            int cx = (int) (left[0] + (right[0] - left[0]) * 0.5f);
            int y = (int) (right[1] - (float) (pad * 2));

            ((PaintCanvas *) canvas)->SetColor((unsigned int) (long) canvas);
            if (((Agent *) (agent))->isKnown() != 0 || ((Agent *) (agent))->isStoryAgent() != 0) {
                s0 = ((Agent *) (agent))->getName();
            } else {
                void *texts = *(void **) &SpaceLounge_lounge_text_slot;
                void *text = ((GameText *) (*(void **) texts))->getText(((Agent *) (agent))->getRace() + 0x196);
                s0.Set(((String *) text)->data);
            }

            void *mission = ((Agent *) (agent))->getMission();
            if (mission != 0) {
                void *texts = *(void **) &SpaceLounge_lounge_text_slot;
                void *text = ((GameText *) (*(void **) texts))->getText(((Mission *) (mission))->getType() + 0x162);
                s1.Set(((String *) text)->data);
            } else {
                int offer = ((Agent *) (agent))->getOffer();
                void *texts = *(void **) &SpaceLounge_lounge_text_slot;
                int id = 0;
                if (offer == 6) {
                    id = 0x132;
                } else if (offer == 2) {
                    id = 0x131;
                } else if (offer == 7) {
                    id = 0x130;
                }
                if (id != 0) {
                    s1.Set(((String *) ((GameText *) (*(void **) texts))->getText(id))->data);
                } else {
                    s1 = "";
                }
            }

            void *font = *(void **) *(void **) &SpaceLounge_lounge_font_slot;
            int textWidth = ((PaintCanvas *) canvas)->GetTextWidth((unsigned int) (long) canvas, *(String *) font);
            int boxX = cx - pad;
            int boxY = y - pad;
            int width = pad * 2 + textWidth;
            s2 = "";
            layout->drawBox(2, boxX, boxY, width, layout->field_0x30, s2, 1u);
            ((PaintCanvas *) canvas)->DrawRectangle(boxX, boxY, width, layout->field_0x30);

            s3 = ((Agent *) (agent))->isKnown() == 0 ? "?" : "";
            s4 = s3 + s0;
            ((PaintCanvas *) canvas)->DrawString((unsigned int) (long) font, s4, cx, y + layoutMetric(layout, 0x2c0),
                                                 false);

            if (((Agent *) (agent))->isKnown() != 0) {
                s5 = " ";
                s6 = s5 + s0;
                int nameWidth = ((PaintCanvas *) canvas)->GetTextWidth((unsigned int) (long) canvas, *(String *) font);
                ((PaintCanvas *) canvas)->DrawString((unsigned int) (long) font, s1, cx + nameWidth,
                                                     y + layoutMetric(layout, 0x2c0), false);
            }
        }
        return;
    }

    ((PaintCanvas *) canvas)->SetColor((unsigned int) (long) canvas);
    s0 = "";
    layout->drawBox(2, this->panelX, this->panelY, layoutMetric(layout, 0x68), layoutMetric(layout, 0x6c), s0, 1u);
    ((PaintCanvas *) canvas)->DrawRectangle(this->panelX, this->panelY, layoutMetric(layout, 0x68),
                                            layoutMetric(layout, 0x6c));
    ((ImageFactory *) (factory))->drawChar((*this->silhouetteGrid)[this->selectedAgent],
                                           layout->field_0x4c + this->panelX, layout->field_0x4c + this->panelY, false);
    this->scrollWindow->draw();

    if ((this->mode & 0xfffffffe) != 2) {
        return;
    }

    ((TouchButton *) (button_at(this, 0)))->setTextColor(-1);
    int offer = ((Agent *) ((*this->agents)[this->selectedAgent]))->getOffer();
    if (this->mode == 2) {
        ((TouchButton *) (button_at(this, 0)))->setPosition(this->buttonX, this->buttonY0);
        ((TouchButton *) (button_at(this, 1)))->setPosition(this->panelWidth + this->buttonX, this->buttonY0, 0x12);
        this->visibleButtonCount = 0;
        if (offer < 11 && ((1 << (offer & 0xff)) & 0x60c) != 0) {
            this->visibleButtonCount = 3;
        } else if (offer == 1) {
            this->visibleButtonCount = 1;
            ((TouchButton *) (button_at(this, 0)))->setPosition(this->buttonX, this->buttonY1);
        } else {
            this->visibleButtonCount = 2;
            ((TouchButton *) (button_at(this, 0)))->setPosition(this->buttonX, this->buttonY1);
            ((TouchButton *) (button_at(this, 1)))->setPosition(this->panelWidth + this->buttonX, this->buttonY1, 0x12);
        }
    } else {
        this->visibleButtonCount = 1;
        ((TouchButton *) (button_at(this, 0)))->setTextColor(-1);
        ((TouchButton *) (button_at(this, 0)))->setPosition(this->buttonX, this->buttonY1);
    }

    for (unsigned i = 0; i < this->buttons->size(); ++i) {
        ((TouchButton *) (button_at(this, i)))->setVisible(false);
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
    s0 = "";
    layout->drawBox(2, this->panelX, this->buttonPanelY, layoutMetric(layout, 0x68), panelHeight, s0, 1u);
    ((PaintCanvas *) canvas)->DrawRectangle(this->panelX, this->buttonPanelY, layoutMetric(layout, 0x68), panelHeight);
    ((ImageFactory *) (factory))->drawChar(this->agentImageParts, layout->field_0x4c + this->panelX,
                                           this->buttonPanelY + layout->field_0x4c, true);

    ((TouchButton *) (button_at(this, 0)))->setVisible(true);
    if (!(offer == 1 || this->singleOffer != 0 || this->mode == 3)) {
        ((TouchButton *) (button_at(this, 1)))->setVisible(true);
        ((TouchButton *) (button_at(this, 0)))->draw();
        ((TouchButton *) (button_at(this, 1)))->draw();
        if (offer <= 10 && ((1 << (offer & 0xff)) & 0x60c) != 0) {
            ((TouchButton *) (button_at(this, 2)))->setVisible(true);
        }
    }
    ((TouchButton *) (button_at(this, 0)))->draw();
}

int SpaceLounge::init() {
    char matrix[60];

    this->touchDown = 0;
    this->initialized = 0;
    this->cameraAnimating = 0;
    this->agents = (Array<Agent *> *) Station_getAgents(Status::gStatus->getStation());

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

    void *layoutSlot = *(void **) &SpaceLounge_init_layout_slot;
    Layout *layout = (Layout *) *(void **) layoutSlot;
    int panelW = layoutMetric(layout, 0x68);
    this->panelX = panelW / 2;
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
        ArrayRemoveAll(*oldButtons);
        delete oldButtons;
    }
    Array<TouchButton *> *buttons = new Array<TouchButton *>();
    this->buttons = buttons;
    ArraySetLength(5, *buttons);

    void *textsSlot = *(void **) &SpaceLounge_init_text_slot;
    void *text = ((GameText *) (*(void **) textsSlot))->getText(0);
    int baseY = this->buttonPanelY + layout->field_0x4c;
    this->buttonX = this->panelX + layoutMetric(layout, 0x68) - layout->field_0x4c - this->panelWidth;
    this->buttonY1 = (baseY + layout->field_0x2d8 / 2) - layout->field_0x30 / 2;
    this->buttonY0 = baseY;

    for (unsigned i = 0; i < 5; ++i) {
        TouchButton *button = new TouchButton(*static_cast<String *>(text), 0, this->buttonX,
                                              baseY + (int) i * (layout->field_0x30 + layout->field_0x34),
                                              this->panelWidth, 0x11, 4);
        (*buttons)[i] = button;
        button->setTextColor(-1);
    }

    this->hoverAgent = -1;
    MatrixSetTranslation(matrix, 0.0f, 0.0f, 0.0f);
    MatrixSetRotation(matrix, 0.0f, 0.0f, 0.0f);
    void *cameraSlot = *(void **) &SpaceLounge_init_camera_slot;
    void *camera = *(void **) cameraSlot;
    void *current = (void *) (long) ((PaintCanvas *) camera)->CameraGetCurrent();
    ((PaintCanvas *) camera)->CameraSetLocal((unsigned int) (long) camera,
                                             *(const AbyssEngine::AEMath::Matrix *) current);
    if (this->cameraEase != 0) {
        this->cameraEase->SetRange(*(AbyssEngine::AEMath::Matrix *) matrix, *(AbyssEngine::AEMath::Matrix *) matrix);
    }
    this->introDone = 1;
    this->headBobPhase = 0;
    ((SpaceLounge *) (this))->updateScreenPositions();
    return 0;
}

SpaceLounge::SpaceLounge() {
    char from[60];
    char to[60];

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
            void *agent = (*agents)[i];
            int offer = ((Agent *) (agent))->getOffer();
            if ((offer == 6 || offer == 0) && ((Agent *) (agent))->getMission() != 0) {
                void *mission = ((Agent *) (agent))->getMission();
                if (((Mission *) (mission))->getType() == 0xc && ((Agent *) (agent))->hasAcceptedOffer() != 0) {
                    ArrayRemove_AgentPtr(agent, this->agents);
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

    int race = ((SolarSystem *) ((void *) (long) Status::gStatus->getSystem()))->getRace();
    MatrixSetTranslation(from, (float) race, 0.0f, 0.0f);
    MatrixSetRotation(from, 0.0f, 0.0f, 0.0f);
    MatrixSetTranslation(to, (float) race, 0.0f, 0.0f);
    MatrixSetRotation(to, 0.0f, 0.0f, 0.0f);
    this->cameraEase = new AbyssEngine::EaseInOutMatrix(*(Matrix *) from, *(Matrix *) to, 3000);

    void *cameraSlot = *(void **) &SpaceLounge_ctor_camera_slot;
    void *camera = *(void **) cameraSlot;
    void *current = (void *) (long) ((PaintCanvas *) camera)->CameraGetCurrent();
    ((PaintCanvas *) camera)->CameraSetLocal((unsigned int) (long) camera,
                                             *(const AbyssEngine::AEMath::Matrix *) current);
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
            ArrayRemoveAll(*strings);
            delete strings;
        }
    }
    this->agentTexts = 0;

    {
        Array<TouchButton *> *buttons = this->buttons;
        if (buttons != 0) {
            ArrayRemoveAll(*buttons);
            delete buttons;
        }
    }
    this->buttons = 0;

    if (this->agentImageParts != 0) {
        return;
    }
    this->agentImageParts = 0;

    {
        Array<Array<ImagePart *> *> *grid = this->silhouetteGrid;
        if (grid != 0) {
            for (unsigned i = 0; i < grid->size(); ++i) {
                Array<ImagePart *> *inner = (*grid)[i];
                if (inner != 0) {
                    ArrayRemoveAll(*inner);
                    delete inner;
                }
                (*grid)[i] = 0;
            }
            ArrayRemoveAll(*grid);
            delete grid;
        }
    }
    this->silhouetteGrid = 0;

    {
        Array<Vector *> *rects = this->agentRects;
        if (rects != 0) {
            ArrayRemoveAll(*rects);
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
    String title;

    if (this->listVisible != 0) {
        if (this->listWindow->shows3DShip() != 0) {
            void *canvasSlot = *(void **) &SpaceLounge_draw_canvas_slot;
            void *canvas = *(void **) canvasSlot;
            ((PaintCanvas *) canvas)->SetColor((unsigned char) (long) canvas, 0, 0, 0);
            int width = pc_GetWidth((PaintCanvas *) canvas);
            int height = pc_GetHeight((PaintCanvas *) canvas);
            (void) height;
            ((PaintCanvas *) canvas)->FillRectangle((int) (long) canvas, 0, 0, width);
            ((PaintCanvas *) canvas)->SetColor((unsigned int) (long) canvas);
        }
        this->listWindow->draw();
        void *layout = *(void **) (*(void **) &SpaceLounge_draw_layout_slot);
        ((Layout *) layout)->drawFooter();
        return;
    }

    if (this->mapVisible != 0) {
        this->starMap->draw();
        return;
    }

    void *layoutSlot = *(void **) &SpaceLounge_draw_layout_slot;
    void *layout = *(void **) layoutSlot;
    void *textsSlot = *(void **) &SpaceLounge_draw_text_slot;
    void *text = ((GameText *) (*(void **) textsSlot))->getText(0x18e);
    title.Set(((String *) text)->data);
    ((Layout *) (layout))->drawHeader(title, false);

    ((SpaceLounge *) (this))->drawLounge();

    layout = *(void **) layoutSlot;
    if ((this->mode & 0xfffffffe) == 2) {
        ((Layout *) layout)->drawFooterNoBackButton();
    } else {
        ((Layout *) layout)->drawFooter();
    }

    if (this->chatActive != 0 || this->popupActive != 0 || this->choiceVisible != 0) {
        this->choiceWindow->draw();
    }
}

void SpaceLounge::update(int dt) {
    char maxMatrix[60];
    char valueMatrix[60];

    if (dt > 50) {
        dt = 50;
    }
    if (this->initialized == 0) {
        return;
    }

    if (this->mapVisible != 0) {
        this->starMap->update(dt);
        return;
    }
    if (this->listVisible != 0) {
        this->listWindow->update(dt);
        return;
    }

    if (this->cameraAnimating == 0) {
        this->cameraEase->Increase((float) dt);
    }

    void *cameraSlot;
    void *camera;
    void *current;
    if (this->introDone == 0) {
        ((AbyssEngine::EaseInOutMatrix *) (maxMatrix))->GetMaxValue();
        ((AbyssEngine::EaseInOutMatrix *) (valueMatrix))->GetValue();
        if ((memcmp((maxMatrix), (valueMatrix), sizeof(float) * 16) == 0) == 0) {
            this->cameraAnimating = 0;
            cameraSlot = *(void **) &SpaceLounge_update_camera_slot_c;
            camera = *(void **) cameraSlot;
            current = (void *) (long) ((PaintCanvas *) camera)->CameraGetCurrent();
            ((AbyssEngine::EaseInOutMatrix *) (valueMatrix))->GetValue();
            ((PaintCanvas *) camera)->CameraSetLocal((unsigned int) (long) camera,
                                                     *(const AbyssEngine::AEMath::Matrix *) valueMatrix);
        } else {
            goto idle_camera;
        }
    } else {
    idle_camera:
        float step = (float) dt * 0.001f;
        if (step > 1.0f) {
            step = 1.0f;
        }
        if (step < 0.0f) {
            step = 0.0f;
        }
        this->introDone = 1;
        this->headBobPhase = this->headBobPhase + step;
        float wave = Sinf(this->headBobPhase);
        float maxRot = (float) this->headBobSteps * 0.5f;

        if (this->cameraAnimating == 0) {
            cameraSlot = *(void **) &SpaceLounge_update_camera_slot_a;
            camera = *(void **) cameraSlot;
            current = ((PaintCanvas *) camera)->CameraGetLocal((unsigned int) (long) camera);
            (void) current;
            this->cameraAnimating = 1;
            int amount = AbyssEngine::AERandom::nextInt(*(void **) &SpaceLounge_update_random_slot, 10);
            this->headBobReverse = 0;
            if (this->headEase == 0) {
                this->headEase = new AbyssEngine::EaseInOut(0.0f, (float) amount);
            } else {
                this->headEase->SetRange(0.0f, (float) amount);
            }
            this->headBobSteps = 2;
        } else {
            float value = this->headEase->GetValue();
            float maxValue = this->headEase->GetMaxValue();
            float distance = value - maxValue;
            if (distance < 0.0f) {
                distance = -distance;
            }
            int amount = this->headBobSteps;
            if (distance < 0.25f) {
                void *random = *(void **) &SpaceLounge_update_random_slot;
                amount = AbyssEngine::AERandom::nextInt(random, 10);
                float next = (float) (5 - amount);
                this->headBobReverse = value > next;
                this->headEase->SetRange(value, next);
                this->headBobSteps = AbyssEngine::AERandom::nextInt(random, 4) + 1;
                amount = this->headBobSteps;
            }
            if (this->headBobReverse != 0) {
                amount = -amount;
            }
            this->headEase->Increase((float) (dt * amount));
        }

        float value = this->headEase->GetValue();
        MatrixSetRotation(valueMatrix, value / 90.0f, 0.0f, value);
        MatrixSetTranslation(maxMatrix, 0.0f, 0.0f, wave * maxRot);
        AbyssEngine::AEMath::MatrixMultiply(*(Matrix *) (valueMatrix), this->baseMatrix);
        cameraSlot = *(void **) &SpaceLounge_update_camera_slot_b;
        camera = *(void **) cameraSlot;
        current = (void *) (long) ((PaintCanvas *) camera)->CameraGetCurrent();
        ((PaintCanvas *) camera)->CameraSetLocal((unsigned int) (long) camera,
                                                 *(const AbyssEngine::AEMath::Matrix *) valueMatrix);
    }

    ((SpaceLounge *) (this))->updateScreenPositions();
    if (this->mode != 0) {
        this->scrollWindow->update(dt);
    }
    this->cutScene->update();

    if (this->touchDown != 0) {
        int x = this->touchX;
        int y = this->touchY;
        int top = *(int *) SpaceLounge_update_top_slot;
        int height = *(int *) SpaceLounge_update_height_slot;
        int width = *(int *) SpaceLounge_update_width_slot;
        CutScene *cutscene = this->cutScene;
        if (y < top) {
            if (x < 150) {
                cutscene->vec8.y -= 10.0f;
            } else if (x > width - 150) {
                cutscene->vec8.y += 10.0f;
            }
        } else if (y < height - top * 2) {
            if (x < 70) {
                cutscene->cameraRotX -= 7.5f;
            } else if (x > width - 70) {
                cutscene->cameraRotX += 7.5f;
            } else if (x > 100 && x < width - 100) {
                if (y < height / 2) {
                    cutscene->vec8.z -= (float) dt;
                } else {
                    cutscene->vec8.z += (float) dt;
                }
            }
        } else {
            if (x < 70) {
                cutscene->vec8.x -= (float) dt;
            } else if (x > width - 70) {
                cutscene->vec8.x += (float) dt;
            }
        }
    }
}

void SpaceLounge::refresh() {
}
