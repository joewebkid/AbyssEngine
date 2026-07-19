#include "game/ui/WantedWindow.h"
#include "game/world/Galaxy.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/world/StarMap.h"
#include "game/mission/Status.h"
#include "game/mission/Mission.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "engine/render/ImagePart.h"
#include "engine/render/PaintCanvas.h"
#include "game/ui/Layout.h"
#include "game/world/SolarSystem.h"
#include "game/world/Station.h"
#include "game/ui/TouchButton.h"
#include "game/world/Wanted.h"
#include "game/core/String.h"
#include "game/menu/ModStation.h"

static Layout **g_WantedWindow_move_layout = nullptr;
static int *g_WantedWindow_move_screen_h = nullptr;
static int *g_WantedWindow_move_screen_w_a = nullptr;
static int *g_WantedWindow_move_force = nullptr;
static int *g_WantedWindow_move_screen_w_b = nullptr;

int WantedWindow::OnTouchMove(int x, int y) {
    if (this->showingMap != 0) {
        this->starMap->OnTouchMove(x, y);
        return 0;
    }

    Layout *layout = *g_WantedWindow_move_layout;
    if (((layout->field_0xc < y) &&
         (y < *g_WantedWindow_move_screen_h - layout->field_0x10) &&
         (x < *g_WantedWindow_move_screen_w_a / 2)) ||
        (*g_WantedWindow_move_force != 0)) {
        int delta = y - this->lastDragY;
        this->dragDelta = delta;
        this->scrollDamping = 1.0f;
        this->scrollOffset += delta;
        this->lastDragY = y;
    }

    if (*g_WantedWindow_move_screen_w_b / 2 < x) {
        this->scrollWindow->OnTouchMove(x, y);
    }

    for (uint32_t i = 0; i < this->buttons->size(); ++i) {
        (*this->buttons)[i]->OnTouchMove(x, y);
    }

    layout->OnTouchMove(x, y);
    if (this->detailButton != nullptr) {
        Wanted *wanted = (*this->wantedList)[this->selectedWanted];
        if (wanted->isActive() != 0) {
            this->detailButton->OnTouchMove(x, y);
        }
    }

    uint32_t selected = this->getWantedAtPosition(x, y);
    int moved = y - this->touchStartY;
    if (moved < 0) {
        moved = -moved;
    }
    if (moved > 5) {
        selected = 0xffffffffu;
    }
    this->highlightedWanted = selected;
    return 0;
}

void WantedWindow::update(int dt) {
    if (this->showingMap != 0) {
        this->starMap->update(dt);
        return;
    }

    (*this->buttons)[1]->setAlwaysPressed(true);
    this->scrollWindow->update(dt);

    if (this->dragging == 0) {
        float delta = this->scrollDamping * this->scrollVelocity;
        float mag = -delta;
        if (0.0f < delta) {
            mag = delta;
        }
        this->scrollVelocity = delta;
        if (mag > 1.0f) {
            this->scrollOffset = (int) (delta + (float) this->scrollOffset);
        }
    }

    int scroll = this->scrollOffset;
    if (scroll > 0) {
        this->scrollDamping = 1.0f;
        this->scrollVelocity = (float) -scroll * 0.5f;
    }

    int limit = this->visibleHeight - this->contentHeight;
    if (limit <= -1) {
        if (scroll < limit) {
            this->scrollDamping = 1.0f;
            this->scrollVelocity = (float) (limit - scroll) * 0.5f;
        }
    } else {
        this->scrollOffset = 0;
        this->scrollVelocity = 0;
    }
}

static Layout **g_WantedWindow_touch_layout = nullptr;

int WantedWindow::OnTouchBegin(int x, int y) {
    if (this->showingMap != 0) {
        this->starMap->OnTouchBegin(x, y);
        return 0;
    }

    this->lastDragY = y;
    this->touchStartY = y;
    this->dragDelta = 0;
    this->dragging = 1;
    this->scrollWindow->OnTouchBegin(x, y);

    for (uint32_t i = 0; i < this->buttons->size(); ++i) {
        (*this->buttons)[i]->OnTouchBegin(x, y);
    }

    (*g_WantedWindow_touch_layout)->OnTouchBegin(x, y);
    if (this->detailButton != nullptr) {
        Wanted *wanted = (*this->wantedList)[this->selectedWanted];
        if (wanted->isActive() != 0) {
            this->detailButton->OnTouchBegin(x, y);
        }
    }

    this->highlightedWanted = this->getWantedAtPosition(x, y);
    return 0;
}

void WantedWindow::render3D() {
    if (this->showingMap != 0) {
        this->starMap->render();
    }
}

float WantedWindow::getRelativeScrollStartPos() {
    int pos = this->scrollOffset;
    if (pos > 0) {
        union {
            uint32_t u;
            float f;
        } c;
        c.u = 0x4650a903u;
        return c.f;
    }
    return -(float) pos / (float) this->contentHeight;
}

static Layout **g_WantedWindow_hit_layout = nullptr;

uint32_t WantedWindow::getWantedAtPosition(int x, int y) {
    if (x >= this->windowX + (this->windowWidth >> 1)) {
        return 0xffffffffu;
    }

    Array<Wanted *> *list = this->wantedList;
    Layout *layout = *g_WantedWindow_hit_layout;
    int numerator = y - this->windowY;
    numerator -= layout->field_0xc;
    numerator -= layout->field_0x20;
    numerator -= layout->field_0x5c;
    numerator -= this->scrollOffset;
    int idx = numerator / (layout->field_0x70 + layout->field_0x34);
    if ((uint32_t) idx > (uint32_t)((int) list->size() - 1)) {
        return 0xffffffffu;
    }
    return (uint32_t) idx;
}

static uint8_t *g_WantedWindow_end_fullscreen = nullptr;
static uint8_t *g_WantedWindow_end_tablet = nullptr;
static uint8_t *g_WantedWindow_end_small = nullptr;
static int *g_WantedWindow_end_screen_w = nullptr;
static int *g_WantedWindow_end_screen_h = nullptr;
static int *g_WantedWindow_end_window_h = nullptr;
static int *g_WantedWindow_end_window_w = nullptr;
static Layout **g_WantedWindow_end_layout_a = nullptr;
static Layout **g_WantedWindow_end_layout_b = nullptr;
static GameText **g_WantedWindow_end_text = nullptr;

void WantedWindow::OnTouchEnd(int x, int y) {
    if (this->showingMap != 0) {
        this->starMap->OnTouchEnd(x, y);
        uint32_t h;
        uint32_t w;
        uint32_t halfW = 0;
        if (*g_WantedWindow_end_fullscreen == 0) {
            h = *g_WantedWindow_end_window_h;
            w = *g_WantedWindow_end_window_w;
            this->windowX = 0;
            this->windowY = 0;
        } else {
            if (*g_WantedWindow_end_tablet == 0) {
                h = 1000;
                w = 0x514;
                if (*g_WantedWindow_end_small == 0) {
                    h = 500;
                    w = 0x28a;
                }
                halfW = w >> 1;
            } else {
                h = 0x2bf;
                halfW = 0x1c9;
                w = 0x392;
            }
            this->windowX = (*g_WantedWindow_end_screen_w >> 1) - halfW;
            this->windowY = (*g_WantedWindow_end_screen_h >> 1) - (h >> 1);
        }
        this->windowWidth = w;
        this->windowHeight = h;
        this->showingMap = 0;
        return;
    }

    int delta = this->dragDelta;
    int pos = this->scrollOffset + delta;
    float velocity = 0.0f;
    int mag = delta < 0 ? -delta : delta;
    if (mag > 3) {
        velocity = (float) delta;
    }
    this->scrollDamping = 0.95f;
    this->dragging = 0;
    this->scrollOffset = pos;
    this->scrollOffsetSnapshot = pos;
    this->scrollVelocity = velocity;

    this->scrollWindow->OnTouchEnd(x, y);
    for (uint32_t i = 0; i < this->buttons->size(); ++i) {
        if ((*this->buttons)[i]->OnTouchEnd(x, y) != 0) {
            this->lastButtonHit = i;
        }
    }

    {
        uint32_t idx = this->getWantedAtPosition(x, y);
        this->selectedWanted = idx;
        this->highlightedWanted = idx;
        this->selectWanted((int) idx);
    }

    bool openMap = false;
    if (this->detailButton != nullptr) {
        Wanted *wanted = (*this->wantedList)[this->selectedWanted];
        if (wanted->isActive() != 0 &&
            this->detailButton->OnTouchEnd(x, y) != 0) {
            openMap = true;
        }
    }

    if (openMap) {
        ApplicationManager *app = ApplicationManager::gAppManager;
        ModStation *module = (ModStation *) app->GetApplicationModule(5);
        this->starMap = module->starMap;
        Wanted *wanted = (*this->wantedList)[this->selectedWanted];
        int lastSeen = wanted->getLastSeen();
        int stationIndex = Galaxy::gGalaxy->getStation(lastSeen);
        Station *station = (Station *) (long) stationIndex;
        delete this->mission;
        this->mission = nullptr;

        Mission *mission = new Mission(0, 0, wanted->getTravelsTo());
        this->mission = mission;

        StarMap *map = this->starMap;
        if (map == nullptr) {
            map = new StarMap(true, mission, false, -1);
            ((ModStation *) app->GetApplicationModule(5))->starMap = map;
            map = ((ModStation *) app->GetApplicationModule(5))->starMap;
            this->starMap = map;
        } else {
            map->init(true, mission, false, -1);
        }

        int system = station->getSystem();
        lastSeen = wanted->getLastSeen();
        map->setStart(system, lastSeen);
        delete station;
        this->showingMap = 1;
        (*g_WantedWindow_end_layout_a)->resetWindowDimensions();
    } else {
        Layout *layout = *g_WantedWindow_end_layout_b;
        if (layout->OnTouchEnd(x, y) != 0) {
            layout->resetWindowDimensions();
        } else if (layout->helpPressed() != 0) {
            String help;
            help.copy((*g_WantedWindow_end_text)->getText(0x27b), false);
            layout->initHelpWindow(help);
        }
    }
}

static Layout **g_WantedWindow_draw_layout = nullptr;
static unsigned int *g_WantedWindow_draw_font = nullptr;
static GameText **g_WantedWindow_draw_text = nullptr;
static ImageFactory **g_WantedWindow_draw_factory = nullptr;

void WantedWindow::draw() {
    if (this->showingMap != 0) {
        this->starMap->draw();
        return;
    }

    Layout *layout = *g_WantedWindow_draw_layout;
    PaintCanvas *canvas = PaintCanvas::gCanvas;
    unsigned int font = *g_WantedWindow_draw_font;

    canvas->EnableClip(this->windowX,
                       this->windowY + layout->field_0xc +
                       layout->field_0x20 + layout->field_0x5c,
                       this->windowWidth,
                       layout->field_0x2c + this->visibleHeight);

    float relStart = this->getRelativeScrollStartPos();
    float visf = (float) this->visibleHeight;
    float relHeight = this->getRelativeScrollHeight();
    int barSize = (int) (relHeight * visf);
    int barStart = (int) (relStart * visf);
    if (barSize >= 1 || barStart >= 0) {
        layout->drawScrollBar(((this->windowX + (this->windowWidth >> 1)) -
                               layout->field_0x2c) - layout->field_0x48,
                              this->windowY + layout->field_0x2c +
                              layout->field_0xc + layout->field_0x20 +
                              layout->field_0x5c,
                              this->visibleHeight, barStart, barSize);
    }

    int y = this->windowY + layout->field_0xc + layout->field_0x20 +
            layout->field_0x5c + layout->field_0x2c + this->scrollOffset;
    int inset = barSize < 1 ? 0 : layout->field_0x2c + layout->field_0x48;

    for (uint32_t i = 0; i < this->wantedList->size(); ++i) {
        int style = (i == this->selectedWanted || i == this->highlightedWanted) ? 4 : 3;
        String boxLabel("", false);
        layout->drawBox(style, layout->buttonInsetX + this->windowX, y,
                        (this->windowWidth >> 1) -
                        (inset + layout->buttonInsetX + layout->field_0x2c),
                        layout->field_0x70, boxLabel, 0);

        Wanted *wanted = (*this->wantedList)[i];
        canvas->SetColor(wanted->isActive() ? 0xffffffffu : 0xff808080u);
        String name = wanted->getName();
        int textY = y + layout->field_0x70 / 2 - canvas->GetTextHeight(font) / 2;
        canvas->DrawString(font, name,
                           this->windowX + layout->buttonInsetX +
                           layout->field_0x44,
                           textY, false);

        int campaign = Status::gStatus->getCurrentCampaignMission();
        if ((i == 0 && campaign == 0x80) ||
            (i == 1 && Status::gStatus->getCurrentCampaignMission() == 0x82)) {
            String marked = wanted->getName();
            marked += String(" *", false);
            int textW = canvas->GetTextWidth(font, marked);
            canvas->DrawImage2D(this->bgImage,
                                this->windowX + layout->buttonInsetX +
                                layout->field_0x44 + textW,
                                textY);
        }

        y += layout->field_0x34 + layout->field_0x70;
    }

    canvas->DisableClip();
    canvas->SetColor(0xffffffffu);
    String header;
    header.copy((*g_WantedWindow_draw_text)->getText(0xc93), false);
    layout->drawHeader(header);

    for (uint32_t i = 0; i < this->buttons->size(); ++i) {
        (*this->buttons)[i]->draw();
    }

    String leftHdr;
    leftHdr.copy((*g_WantedWindow_draw_text)->getText(0xc95), false);
    layout->drawBox(1, layout->buttonInsetX + this->windowX,
                    this->windowY + layout->field_0xc + layout->field_0x20,
                    (this->windowWidth >> 1) - (layout->field_0x2c + layout->buttonInsetX),
                    layout->field_0x5c, leftHdr, 0);

    String leftBody("", false);
    layout->drawBox(5, layout->buttonInsetX + this->windowX,
                    this->windowY + layout->field_0xc + layout->field_0x20 +
                    layout->field_0x5c + layout->field_0x2c,
                    (this->windowWidth >> 1) - (layout->field_0x2c + layout->buttonInsetX),
                    ((this->windowHeight -
                      (layout->field_0x20 + layout->field_0xc +
                       layout->field_0x5c + layout->field_0x2c * 2)) -
                     layout->field_0x10) -
                    layout->field_0x24,
                    leftBody, 0);

    String rightHdr;
    rightHdr.copy((*g_WantedWindow_draw_text)->getText(0xc95), false);
    layout->drawBox(1, this->windowX + (this->windowWidth >> 1) + layout->field_0x2c,
                    this->windowY + layout->field_0xc + layout->field_0x20,
                    ((this->windowWidth >> 1) - layout->field_0x2c) - layout->buttonInsetX,
                    layout->field_0x5c, rightHdr, 0);

    String rightBody("", false);
    layout->drawBox(5, this->windowX + (this->windowWidth >> 1) + layout->field_0x2c,
                    this->windowY + layout->field_0x2c + layout->field_0xc +
                    layout->field_0x20 + layout->field_0x5c,
                    ((this->windowWidth >> 1) - layout->field_0x2c) - layout->buttonInsetX,
                    ((this->windowHeight -
                      (layout->field_0xc + layout->field_0x2c * 2 +
                       layout->field_0x20 + layout->field_0x5c)) -
                     layout->field_0x10) -
                    layout->field_0x24,
                    rightBody, 0);

    if (this->imageParts != nullptr) {
        int charX = this->windowX + (this->windowWidth >> 1) + layout->field_0x2c;
        int charY = layout->field_0x5c + this->windowY + layout->field_0x2c +
                    layout->field_0xc + layout->field_0x20;
        (*g_WantedWindow_draw_factory)->drawChar(this->imageParts, charX, charY, false);
        int textX = layout->field_0x2d4 + charX + layout->field_0x2c;
        canvas->DrawString(font, this->nameText, textX, charY, false);

        String fromLine = String("from: ", false) +
                          *(*g_WantedWindow_draw_text)->getText(0xc93) + this->fromText;
        canvas->DrawString(font, fromLine, textX, charY + layout->field_0x4 * 2, false);

        String toLine = String("to: ", false) +
                        *(*g_WantedWindow_draw_text)->getText(0xc93) + this->toText;
        canvas->DrawString(font, toLine, textX, charY + layout->field_0x4 * 3, false);

        this->scrollWindow->draw();
    }

    if (this->detailButton != nullptr &&
        (*this->wantedList)[this->selectedWanted]->isActive() != 0) {
        this->detailButton->draw();
    }

    layout->drawFooter();
}

static Layout **g_WantedWindow_init_layout = nullptr;
static uint8_t *g_WantedWindow_init_fullscreen = nullptr;
static uint8_t *g_WantedWindow_init_tablet = nullptr;
static uint8_t *g_WantedWindow_init_small = nullptr;
static int *g_WantedWindow_init_screen_w = nullptr;
static int *g_WantedWindow_init_screen_h = nullptr;
static int *g_WantedWindow_init_window_w = nullptr;
static int *g_WantedWindow_init_window_h = nullptr;
static GameText **g_WantedWindow_init_text = nullptr;

int WantedWindow::init() {
    this->scrollOffset = 0;
    this->lastDragY = 0;
    this->scrollOffsetSnapshot = 0;
    this->dragDelta = 0;

    this->wantedList = new Array<Wanted *>();

    Status *status = Status::gStatus;
    Array<Wanted *> *allWanted = status->getWanted();
    Layout *layout = *g_WantedWindow_init_layout;

    for (uint32_t i = 0; i < allWanted->size(); ++i) {
        int race = ((SolarSystem *) (long) status->getSystem())->getRace();
        Wanted *wanted = (*allWanted)[i];
        if (race == wanted->getBoard()) {
            race = ((SolarSystem *) (long) status->getSystem())->getRace();
            if (race != 0 || status->getCurrentCampaignMission() < 0x80) {
                race = ((SolarSystem *) (long) status->getSystem())->getRace();
                if (race == 0 && status->getCurrentCampaignMission() >= 0xa2) {
                    ArrayAdd(wanted, *(this->wantedList));
                }
            } else {
                ArrayAdd(wanted, *(this->wantedList));
            }
        }
    }

    uint32_t h;
    uint32_t w;
    uint32_t halfW = 0;
    if (*g_WantedWindow_init_fullscreen == 0) {
        h = *g_WantedWindow_init_window_h;
        w = *g_WantedWindow_init_window_w;
        this->windowX = 0;
        this->windowY = 0;
    } else {
        if (*g_WantedWindow_init_tablet == 0) {
            h = 1000;
            w = 0x514;
            if (*g_WantedWindow_init_small == 0) {
                h = 500;
                w = 0x28a;
            }
            halfW = w >> 1;
        } else {
            h = 0x2bf;
            halfW = 0x1c9;
            w = 0x392;
        }
        this->windowX = (*g_WantedWindow_init_screen_w >> 1) - halfW;
        this->windowY = (*g_WantedWindow_init_screen_h >> 1) - (h >> 1);
    }

    this->windowWidth = w;
    this->windowHeight = h;
    this->selectedWanted = 0;

    uint32_t selected = 0;
    for (;;) {
        Array<Wanted *> *arr = this->wantedList;
        uint32_t count = arr->size();
        if (selected >= count) {
            selected = this->selectedWanted;
            if (selected == count - 1 && (*arr)[selected]->isActive() == 0) {
                selected = 0;
                this->selectedWanted = 0;
            } else {
                selected = this->selectedWanted;
            }
            break;
        }
        if ((*arr)[selected]->isActive() != 0) {
            this->selectedWanted = selected;
            arr = this->wantedList;
            count = arr->size();
            if (selected == count - 1 && (*arr)[selected]->isActive() == 0) {
                selected = 0;
                this->selectedWanted = 0;
            } else {
                selected = this->selectedWanted;
            }
            break;
        }
        ++selected;
    }

    this->highlightedWanted = selected;
    this->selectWanted((int) selected);

    Array<TouchButton *> *buttons = new Array<TouchButton *>();
    this->buttons = buttons;
    ArraySetLength(2, *buttons);

    GameText *text = *g_WantedWindow_init_text;
    {
        String *label = text->getText(0xc93);
        int helpOff = layout->getHelpButtonOffset();
        (*buttons)[1] = new TouchButton(this->windowX + (int) this->windowWidth - helpOff,
                                        this->windowY, *label, 3, 0x12, 0);
    }
    {
        String *label = text->getText(0x81);
        int helpOff = layout->getHelpButtonOffset();
        int width = (*buttons)[1]->getWidth();
        (*buttons)[0] = new TouchButton(this->windowX + (int) this->windowWidth - helpOff -
                                        width + layout->field_0x38,
                                        this->windowY, *label, 3, 0x12, 0);
    }

    (*buttons)[1]->setAlwaysPressed(true);
    layout->setWindowDimensions(this->windowX, this->windowY, this->windowWidth, this->windowHeight);

    layout = *g_WantedWindow_init_layout;
    this->contentHeight = (layout->field_0x34 + layout->field_0x70) *
                          (int) this->wantedList->size();
    this->visibleHeight =
            (((((this->windowHeight - layout->field_0x10) - layout->field_0xc) -
               layout->field_0x20) - layout->field_0x24) - layout->field_0x5c) +
            layout->field_0x2c;

    delete this->detailButton;
    this->starMap = nullptr;
    this->detailButton = nullptr;
    this->showingMap = 0;

    int activeWidth = layout->field_0xcc - layout->buttonInsetX - layout->field_0x4c;
    {
        String *label = text->getText(0x1a8);
        this->detailButton = new TouchButton(
            *label, 0,
            this->windowX + (this->windowWidth >> 1) + layout->field_0x2c,
            (((this->windowY - layout->field_0x2c) + this->windowHeight) -
             layout->field_0x10) - layout->field_0x24,
            activeWidth, 0x21, 4);
    }
    this->hangarUpdate = 0;
    this->lastButtonHit = 1;
    return 1;
}

static unsigned int *g_WantedWindow_ctor_font = nullptr;

WantedWindow::WantedWindow() {
    this->detailButton = nullptr;
    this->starMap = nullptr;
    this->imageParts = nullptr;
    PaintCanvas *canvas = PaintCanvas::gCanvas;
    int h = canvas->GetTextHeight(*g_WantedWindow_ctor_font);
    this->wantedList = nullptr;
    this->mission = nullptr;
    this->scrollWindow = nullptr;
    this->buttons = nullptr;
    this->halfTextHeight = h / 2 - 1;
    unsigned int bgImageHandle;
    canvas->Image2DCreate(0x454, bgImageHandle);
    this->bgImage = bgImageHandle;
    this->init();
}

WantedWindow::~WantedWindow() {
    if (this->imageParts != nullptr) {
        ArrayReleaseClasses(*this->imageParts); ArrayRemoveAll(*(this->imageParts));
        delete this->imageParts;
    }
    this->imageParts = nullptr;

    if (this->buttons != nullptr) {
        ArrayReleaseClasses(*this->buttons); ArrayRemoveAll(*(this->buttons));
        delete this->buttons;
    }
    this->buttons = nullptr;

    delete this->detailButton;
    this->detailButton = nullptr;

    delete this->wantedList;
    this->wantedList = nullptr;

    delete this->mission;
    this->mission = nullptr;

    delete this->scrollWindow;
    this->scrollWindow = nullptr;
}

void WantedWindow::setHangarUpdate(bool needsUpdate) {
    this->hangarUpdate = needsUpdate;
}

bool WantedWindow::hangarNeedsUpdate() {
    return this->hangarUpdate;
}

float WantedWindow::getRelativeScrollHeight() {
    int content = this->contentHeight;
    int visible = this->visibleHeight;
    if (content < visible) {
        union {
            uint32_t u;
            float f;
        } c;
        c.u = 0x4605e009u;
        return c.f;
    }
    int scroll = this->scrollOffset;
    int num;
    if (scroll >= 1) {
        num = visible - scroll;
    } else if (scroll >= visible - content) {
        num = visible;
    } else {
        num = scroll + content;
    }
    return (float) num / (float) content;
}

static ImageFactory **g_WantedWindow_select_factory = nullptr;
static GameText **g_WantedWindow_select_text_a = nullptr;
static Layout **g_WantedWindow_select_layout = nullptr;

void WantedWindow::selectWanted(int idx) {
    delete this->imageParts;
    this->imageParts = nullptr;

    delete this->scrollWindow;
    this->scrollWindow = nullptr;

    Wanted *wanted = (*this->wantedList)[idx];
    this->imageParts = (*g_WantedWindow_select_factory)->loadChar(wanted->getImageParts());

    this->nameText = wanted->getName();

    if (wanted->isActive() != 0) {
        Galaxy *galaxy = Galaxy::gGalaxy;
        Station *last = (Station *) (long) galaxy->getStation(wanted->getLastSeen());
        Station *travel = (Station *) (long) galaxy->getStation(wanted->getTravelsTo());
        Station *current = (Station *) (long) galaxy->getStation(wanted->getCurrentLocation());

        this->fromText = last->getName() + String("from ", false) + String(" ", false);
        this->toText = travel->getName() + String("to ", false) + String(" ", false);
        this->atText = current->getName() + String("at ", false) + String(" ", false);

        String reward;
        reward.Set((long long) (wanted->getReward()));
        this->rewardText = reward + String("reward ", false);

        delete last;
        delete travel;
        delete current;
    } else if (wanted->isTerminated() != 0) {
        this->fromText = String("terminated", false);
        this->toText = String("", false);
        this->atText = String("", false);
        this->rewardText = String("", false);
    } else {
        this->fromText = *(*g_WantedWindow_select_text_a)->getText(0xc9d);
        this->toText = *(*g_WantedWindow_select_text_a)->getText(0xc9d);
        String reward;
        reward.Set((long long) (wanted->getReward()));
        this->rewardText = reward + String("reward ", false);
        this->atText = *(*g_WantedWindow_select_text_a)->getText(0xc9d);
    }

    this->detailText = *(*g_WantedWindow_select_text_a)->getText(0xc9d);

    Layout *layout = *g_WantedWindow_select_layout;
    int y = this->windowY;
    int h = this->windowHeight;
    int top = y + layout->field_0xc + layout->field_0x20 +
              layout->field_0x5c + layout->field_0x2c;
    int height = (((((y - layout->field_0x2c) + h) - top) -
                   layout->field_0x10) - layout->field_0x2d8) -
                 layout->field_0x24;
    if (wanted->isActive() != 0) {
        height = (height - layout->field_0x4c) - layout->field_0x30;
    }

    int pad = layout->field_0x2c;
    this->scrollWindow = new ScrollTouchWindow(
        this->windowX + (this->windowWidth >> 1) + pad,
        layout->field_0x2d8 + pad + top,
        ((this->windowWidth >> 1) - pad) - layout->buttonInsetX,
        height, false);

    String body;
    body = *(*g_WantedWindow_select_text_a)->getText(0xc9d);
    body += String("\n", false) + this->fromText;
    body += String("\n", false) + this->toText;
    body += String("\n", false) + this->atText;
    body += String("\n", false) + this->rewardText;

    String title("", false);
    this->scrollWindow->setText(title, body);
}
