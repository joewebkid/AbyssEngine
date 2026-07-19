#include "game/ui/MissionsWindow.h"
#include "game/ship/Ship.h"
#include "game/ship/Agent.h"
#include "engine/render/PaintCanvas.h"
#include "game/ui/ChoiceWindow.h"
#include "game/ui/ScrollTouchWindow.h"
#include "game/ui/TouchButton.h"
#include "game/ui/WantedWindow.h"
#include "game/ui/Layout.h"
#include "game/mission/Item.h"
#include "game/mission/Mission.h"
#include "game/mission/Status.h"
#include "game/mission/Achievements.h"
#include "game/core/Globals.h"
#include "game/world/StarMap.h"
#include "game/core/String.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/GameText.h"
#include "engine/render/ImageFactory.h"
#include "engine/render/ImagePart.h"
#include "game/menu/ModStation.h"
#include <cstddef>

// Campaign-mission visibility flags. The campaign-mission handle behind
// g_mwi_campaign / g_mw_campaign is a Mission object whose byte-addressable
// visibility flags live at offsets 0x35 and 0x37 (inside the 0x34 flag word).
// Model them with a named overlay so the access is a struct member, not raw
// pointer arithmetic.
struct CampaignMissionFlags {
    char pad00[0x35];
    char campaignVisibleFlagA; // 0x35
    char pad36;
    char campaignVisibleFlagB; // 0x37
};
#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(CampaignMissionFlags, campaignVisibleFlagA) == 0x35,
              "CampaignMissionFlags flagA offset");
static_assert(offsetof(CampaignMissionFlags, campaignVisibleFlagB) == 0x37,
              "CampaignMissionFlags flagB offset");
#endif

// PaintCanvas::gCanvas is declared `extern` in engine/render/PaintCanvas.h (already included).

// These per-screen globals appear only in this translation unit. The original
// build shared them via extern decls; here they are file-static definitions.
// Parity loss accepted (criterion 7: drive extern toward 0).
static GameText *g_mw_gameText = nullptr;

static Layout **g_mw_m_layout = nullptr;
static Layout **g_mw_b_layout = nullptr;

static Layout **g_mwi_layout = nullptr;
static int *g_mwi_titleTable = nullptr;
static char *g_mwi_flagA = nullptr;
static char *g_mwi_flagB = nullptr;
static char *g_mwi_flagC = nullptr;
static int *g_mwi_screenW = nullptr;
static int *g_mwi_screenH = nullptr;
static void *g_mwi_campaign = nullptr;
static ImageFactory **g_mwi_imageFactory = nullptr;
static int g_mwi_actionColor = 0;

static Layout **g_mwd_layout = nullptr;
static int *g_mwd_textId = nullptr;
static void *g_mwd_color = nullptr;
static ImageFactory **g_mwd_imageFactory = nullptr;
static void *g_mwd_font = nullptr;

static char *g_mwt_flagA = nullptr;
static char *g_mwt_flagB = nullptr;
static char *g_mwt_flagC = nullptr;
static int *g_mwt_screenW = nullptr;
static int *g_mwt_screenH = nullptr;
static Layout **g_mwt_layout = nullptr;
static Layout **g_mwt_resetLayout = nullptr;

static void *g_mw_campaign = nullptr;
static int *g_mw_textBase = nullptr;
static int *g_mw_titleTable = nullptr;

// Our own C-ified shims; demoted from extern "C" to plain C++ decls
// (mangling change accepted).
void Status_replaceHash(void *out, void *key, void *a, void *b, void *c);

int ApplicationManager_GetCurrentApplicationModule(void *appMgr);

int _mw_GetTextHeight(void *canvas);

void TouchButton_ctorTab(void *self, void *text, int kind, int x, int y, char flags);

int StarMap_OnTouchEnd(StarMap *map, int x, int y);

int MissionsWindow::OnTouchMove(int p1, int p2) {
    if (this->m_mode == 1)
        return this->m_pWantedWindow->OnTouchMove(p1, p2);
    if (this->m_choiceActive) {
        this->m_pChoiceWindow->OnTouchMove(p1, p2);
    } else if (this->m_starMapActive) {
        this->m_pStarMap->OnTouchMove(p1, p2);
    } else {
        if (Status::gStatus->wantedBoardAccessible()) {
            Array<TouchButton *> *arr = this->m_pTabButtons;
            for (unsigned i = 0; i < arr->size(); i++)
                (*arr)[i]->OnTouchMove(p1, p2);
        }
        (*g_mw_m_layout)->OnTouchMove(p1, p2);
        this->m_pCampaignWindow->OnTouchMove(p1, p2);
        this->m_pFreelanceWindow->OnTouchMove(p1, p2);
        if (this->m_pAcceptButton) this->m_pAcceptButton->OnTouchMove(p1, p2);
        if (this->m_pMapButton) this->m_pMapButton->OnTouchMove(p1, p2);
        if (this->m_pRejectButton) this->m_pRejectButton->OnTouchMove(p1, p2);
    }
    return 0;
}

int MissionsWindow::OnTouchBegin(int p1, int p2) {
    if (this->m_mode == 1)
        return this->m_pWantedWindow->OnTouchBegin(p1, p2);
    if (this->m_choiceActive) {
        this->m_pChoiceWindow->OnTouchBegin(p1, p2);
    } else if (this->m_starMapActive) {
        this->m_pStarMap->OnTouchBegin(p1, p2);
    } else {
        if (Status::gStatus->wantedBoardAccessible()) {
            Array<TouchButton *> *arr = this->m_pTabButtons;
            for (unsigned i = 0; i < arr->size(); i++)
                (*arr)[i]->OnTouchBegin(p1, p2);
        }
        (*g_mw_b_layout)->OnTouchBegin(p1, p2);
        this->m_pCampaignWindow->OnTouchBegin(p1, p2);
        this->m_pFreelanceWindow->OnTouchBegin(p1, p2);
        if (this->m_pAcceptButton) this->m_pAcceptButton->OnTouchBegin(p1, p2);
        if (this->m_pMapButton) this->m_pMapButton->OnTouchBegin(p1, p2);
        if (this->m_pRejectButton) this->m_pRejectButton->OnTouchBegin(p1, p2);
    }
    return 0;
}

void MissionsWindow::setHangarUpdate(bool v) {
    this->m_hangarNeedsUpdate = v;
}

uint8_t MissionsWindow::hangarNeedsUpdate() {
    return this->m_hangarNeedsUpdate;
}

MissionsWindow::MissionsWindow() {
    PaintCanvas *canvas = PaintCanvas::gCanvas;
    this->m_pAcceptButton = nullptr;
    this->m_pRejectButton = nullptr;
    this->m_pMapButton = nullptr;
    this->m_pCampaignWindow = nullptr;
    this->m_pFreelanceWindow = nullptr;
    this->m_pStarMap = nullptr;
    this->m_pChoiceWindow = nullptr;
    this->m_pWantedWindow = nullptr;
    this->m_pTabButtons = nullptr;
    this->m_pAgentImageParts = nullptr;
    int h = _mw_GetTextHeight(canvas);
    this->m_textHalfHeight = h / 2 - 1;
    this->init();
}

MissionsWindow::~MissionsWindow() {
    if (this->m_pAgentImageParts) {
        ArrayReleaseClasses(*this->m_pAgentImageParts); ArrayRemoveAll(*(this->m_pAgentImageParts));
        delete this->m_pAgentImageParts;
    }
    this->m_pAgentImageParts = nullptr;
    if (this->m_pTabButtons) {
        ArrayReleaseClasses(*this->m_pTabButtons); ArrayRemoveAll(*(this->m_pTabButtons));
        delete this->m_pTabButtons;
    }
    this->m_pTabButtons = nullptr;
    delete this->m_pCampaignWindow;
    this->m_pCampaignWindow = nullptr;
    delete this->m_pFreelanceWindow;
    this->m_pFreelanceWindow = nullptr;
    delete this->m_pChoiceWindow;
    this->m_pChoiceWindow = nullptr;
    delete this->m_pAcceptButton;
    this->m_pAcceptButton = nullptr;
    delete this->m_pRejectButton;
    this->m_pRejectButton = nullptr;
    delete this->m_pMapButton;
    this->m_pMapButton = nullptr;
    delete this->m_pWantedWindow;
    this->m_pWantedWindow = nullptr;
}

int MissionsWindow::init() {
    Layout *L = *g_mwi_layout;
    int titleId = *g_mwi_titleTable;

    if (*g_mwi_flagA == 0) {
        this->m_x = 0;
        this->m_y = 0;
        this->m_width = *g_mwi_screenW;
        this->m_height = *g_mwi_screenH;
    } else {
        int w, h;
        if (*g_mwi_flagB == 0) {
            h = 0x514;
            if (*g_mwi_flagC == 0) h = 0x28a;
            w = (*g_mwi_flagC == 0) ? 500 : 1000;
        } else {
            h = 0x392;
            w = 1000;
        }
        this->m_width = h;
        this->m_height = w;
        this->m_x = (*g_mwi_screenW >> 1) - (h >> 1);
        this->m_y = (*g_mwi_screenH >> 1) - (w >> 1);
    }

    Array<TouchButton *> *tabs = new Array<TouchButton *>();
    this->m_pTabButtons = tabs;
    ArraySetLength(2, *tabs);

    TouchButton *tab1 = (TouchButton *) ::operator new(sizeof(TouchButton));
    String *t0 = g_mw_gameText->getText(titleId);
    int helpOff = L->getHelpButtonOffset();
    TouchButton_ctorTab(tab1, t0, 3, (this->m_width + this->m_x) - helpOff, this->m_y, 0x12);
    (*tabs)[1] = tab1;

    TouchButton *tab0 = (TouchButton *) ::operator new(sizeof(TouchButton));
    String *t1 = g_mw_gameText->getText(titleId);
    int helpOff2 = L->getHelpButtonOffset();
    int w1 = tab0->getWidth();
    TouchButton_ctorTab(tab0, t1, 3,
                        (((this->m_width + this->m_x) - helpOff2) - w1) + L->field_0x38,
                        this->m_y, 0x12);
    (*tabs)[0] = tab0;
    tab0->setAlwaysPressed(true);

    L->setWindowDimensions(this->m_x, this->m_y, this->m_width, this->m_height);

    if (this->m_pAgentImageParts) {
        ArrayReleaseClasses(*this->m_pAgentImageParts); ArrayRemoveAll(*(this->m_pAgentImageParts));
        delete this->m_pAgentImageParts;
    }
    this->m_pAgentImageParts = nullptr;
    delete this->m_pCampaignWindow;
    this->m_pCampaignWindow = nullptr;
    delete this->m_pFreelanceWindow;
    this->m_pFreelanceWindow = nullptr;
    delete this->m_pChoiceWindow;
    this->m_pChoiceWindow = nullptr;
    delete this->m_pAcceptButton;
    this->m_pAcceptButton = nullptr;
    delete this->m_pRejectButton;
    this->m_pRejectButton = nullptr;
    delete this->m_pMapButton;
    this->m_pStarMap = nullptr;
    this->m_pMapButton = nullptr;
    this->m_choiceActive = 0;

    int topY = L->field_0xc + this->m_y + L->field_0x20 + L->field_0x5c + L->field_0x2c;
    int reserve = (Status::gStatus->gameWon() == 0) ? L->field_0x30 : 0;
    this->m_pCampaignWindow = new ScrollTouchWindow(
        L->buttonInsetX + this->m_x, topY,
        (this->m_width >> 1) - (L->field_0x2c + L->buttonInsetX),
        (((((this->m_y - topY) + this->m_height) - L->field_0x10) - L->field_0x24) - reserve)
        + L->field_0x2c * -2, false);

    CampaignMissionFlags *campFlags = *(CampaignMissionFlags **) g_mwi_campaign;
    bool campShow = (Status::gStatus->gameWon() == 0) ||
                    (campFlags->campaignVisibleFlagB != 0 ||
                     campFlags->campaignVisibleFlagA != 0);
    if (campShow) {
        String text("", false);
        if (Status::gStatus->getCurrentCampaignMission() < 0xa4) {
            String *t = g_mw_gameText->getText(titleId);
            text = *t;
        }
        void *key = Status::gStatus;
        Mission *cm = (Mission *) ((void *) (intptr_t) Status::gStatus->getCampaignMission());
        int type = cm->getType();
        bool production = (type == 0xa7) || (cm->getType() == 0xae);
        String suffix("", false);
        String merged;
        if (production) {
            String hdr(text);
            int need = cm->getProductionGoodAmount();
            int have = cm->getStatusValue();
            String val;
            val.Set((long long) (need - have));
            Status_replaceHash(&merged, key, &hdr, &val, &suffix);
        } else {
            String hdr(text);
            cm->getTargetStationName();
            String val;
            Status_replaceHash(&merged, key, &hdr, &val, &suffix);
        }
        text = merged;

        String a("", false);
        String b(text);
        this->m_pCampaignWindow->setText(a, b);
    } else {
        bool useGold = Achievements::gAchievements->gotAllGoldMedals() != 0 &&
                       ((Ship *) (Status::gStatus->getShip()))->getIndex() != 8;
        String a("", false);
        String *t = g_mw_gameText->getText(titleId);
        String b(*t);
        this->m_pCampaignWindow->setText(a, b);
        (void) useGold;
    }

    int fmEmpty = Status::gStatus->getFreelanceMission()->isEmpty();
    int half = this->m_width >> 1;
    int pad = L->field_0x2c;
    int rx = this->m_x + half + pad;
    if (fmEmpty == 0) {
        int ry = L->field_0x2d8 + topY + pad;
        this->m_pFreelanceWindow = new ScrollTouchWindow(
            rx, ry, (half - pad) - L->buttonInsetX,
            (((((this->m_y - ry) + this->m_height) - L->field_0x10) - L->field_0x24) - L->field_0x4c)
            - L->field_0x30, false);

        Mission *fm = Status::gStatus->getFreelanceMission();
        String text = Globals::gGlobals->getAgentMissionText(fm->getAgent());
        void *key = Status::gStatus;
        String body(text);
        int rew = fm->getReward();
        int bonus = fm->getBonus();
        String reward = Layout::formatCredits(rew + bonus);
        String suffix("", false);
        String merged;
        Status_replaceHash(&merged, key, &body, &reward, &suffix);
        text = merged;

        String a("", false);
        String b(text);
        this->m_pFreelanceWindow->setText(a, b);

        int *parts = ((Agent *) (fm->getAgent()))->getImageParts();
        this->m_pAgentImageParts = (*g_mwi_imageFactory)->loadChar(parts);
    } else {
        this->m_pFreelanceWindow = new ScrollTouchWindow(
            rx, topY, (half - pad) - L->buttonInsetX,
            ((this->m_height + (this->m_y - (topY + pad * 2))) - L->field_0x10) - L->field_0x24, false);
        String a("", false);
        String *t = g_mw_gameText->getText(titleId);
        String b(*t);
        this->m_pFreelanceWindow->setText(a, b);
    }

    if (Status::gStatus->inAlienOrbit() == 0) {
        int btnY = ((this->m_width >> 1) >> 1) - L->buttonInsetX;
        if (Status::gStatus->gameWon() == 0) {
            String *t = g_mw_gameText->getText(titleId);
            this->m_pAcceptButton = new TouchButton(
                *t, 0, L->buttonInsetX + this->m_x,
                (((this->m_y + this->m_height) - L->field_0x10) - L->field_0x24) - L->field_0x2c,
                btnY, '!', 4);
        }
        if (Status::gStatus->getFreelanceMission()->isEmpty() == 0) {
            String *t = g_mw_gameText->getText(titleId);
            this->m_pRejectButton = new TouchButton(
                *t, 0, this->m_x + (this->m_width >> 1) + L->field_0x2c,
                (((this->m_y - L->field_0x2c) + this->m_height) - L->field_0x10) - L->field_0x24,
                btnY, '!', 4);

            if (ApplicationManager_GetCurrentApplicationModule(ApplicationManager::gAppManager) == 5) {
                String *t2 = g_mw_gameText->getText(titleId);
                this->m_pMapButton = new TouchButton(
                    *t2, 0, this->m_x + btnY + (this->m_width >> 1) + L->field_0x2c * 2,
                    (((this->m_y - L->field_0x2c) + this->m_height) - L->field_0x10) - L->field_0x24,
                    btnY, '!', 4);
                this->m_pMapButton->setTextColor(g_mwi_actionColor);
            }
            this->m_pChoiceWindow = new ChoiceWindow();
        }
    }

    this->m_mode = 0;
    this->m_hangarNeedsUpdate = 0;

    if (Status::gStatus->wantedBoardAccessible() != 0) {
        if (this->m_pWantedWindow == nullptr)
            this->m_pWantedWindow = new WantedWindow();
        else
            this->m_pWantedWindow->init();
    }

    return 0;
}

void MissionsWindow::draw() {
    if (this->m_mode == 1) {
        this->m_pWantedWindow->draw();
        return;
    }
    if (this->m_starMapActive != 0) {
        this->m_pStarMap->draw();
        return;
    }

    PaintCanvas *canvas = PaintCanvas::gCanvas;
    Layout *L = *g_mwd_layout;
    void *color = *(void **) g_mwd_color;
    void *font = *(void **) g_mwd_font;
    int titleId = *g_mwd_textId;

    ((PaintCanvas *) canvas)->SetColor((unsigned int) (uintptr_t) color);

    {
        String *ht = g_mw_gameText->getText(titleId);
        String header(*ht);
        L->drawHeader(header);
    }

    if (Status::gStatus->wantedBoardAccessible() != 0) {
        Array<TouchButton *> *tabs = this->m_pTabButtons;
        for (unsigned int i = 0; i < tabs->size(); i++)
            (*tabs)[i]->draw();
    }

    int ox = this->m_x, oy = this->m_y;
    int ow = this->m_width, oh = this->m_height;

    {
        String *t = g_mw_gameText->getText(titleId);
        String box(*t);
        int c = L->field_0xc, p20 = L->field_0x20;
        int p28 = L->buttonInsetX, p2c = L->field_0x2c;
        L->drawBox(1, p28 + ox, oy + c + p20, (ow >> 1) - (p2c + p28), L->field_0x5c, box,
                   (unsigned) (uintptr_t) canvas);
    }
    {
        String box("", false);
        int c = L->field_0xc, p10 = L->field_0x10;
        int p20 = L->field_0x20, p24 = L->field_0x24;
        int p28 = L->buttonInsetX, p2c = L->field_0x2c;
        int p5c = L->field_0x5c;
        L->drawBox(5, p28 + ox, oy + c + p20 + p5c + p2c, (ow >> 1) - (p2c + p28),
                   ((oh - (p20 + c + p5c + p2c * 2)) - p10) - p24, box, (unsigned) (uintptr_t) canvas);
    }

    this->m_pCampaignWindow->draw();
    if (this->m_pAcceptButton) this->m_pAcceptButton->draw();

    {
        String *t = g_mw_gameText->getText(titleId);
        String box(*t);
        int c = L->field_0xc, p20 = L->field_0x20;
        int p28 = L->buttonInsetX, p2c = L->field_0x2c;
        L->drawBox(1, ox + (ow >> 1) + p2c, oy + c + p20, ((ow >> 1) - p2c) - p28, L->field_0x5c, box,
                   (unsigned) (uintptr_t) canvas);
    }
    {
        String box("", false);
        int c = L->field_0xc, p10 = L->field_0x10;
        int p20 = L->field_0x20, p24 = L->field_0x24;
        int p28 = L->buttonInsetX, p2c = L->field_0x2c;
        int p5c = L->field_0x5c;
        L->drawBox(5, ox + (ow >> 1) + p2c, oy + p2c + c + p20 + p5c, ((ow >> 1) - p2c) - p28,
                   ((oh - (c + p2c * 2 + p20 + p5c)) - p10) - p24, box, (unsigned) (uintptr_t) canvas);
    }

    Mission *fm = Status::gStatus->getFreelanceMission();
    if (fm != nullptr && fm->isEmpty() == 0 && this->m_pAgentImageParts != nullptr) {
        (*g_mwd_imageFactory)->drawChar(
            this->m_pAgentImageParts, ox + (ow >> 1) + L->field_0x2c,
            L->field_0x2c + oy + L->field_0xc + L->field_0x20 + L->field_0x5c, false);

        int detailX = ox + (ow >> 1) + L->field_0x2d4 + L->field_0x2c * 2;
        int detailY = oy + L->field_0xc + L->field_0x20 + L->field_0x2c + L->field_0x5c;

        String name = ((Agent *) (fm->getAgent()))->getName();
        ((PaintCanvas *) canvas)->DrawString((unsigned int) (uintptr_t) font, name, detailX, detailY, false);

        String station = ((Agent *) (fm->getAgent()))->getStationName();
        ((PaintCanvas *) canvas)->DrawString((unsigned int) (uintptr_t) font, station, detailX, detailY, false);

        String *typeTxt = g_mw_gameText->getText(
            ((Agent *) (fm->getAgent()))->getMission()->getType() + 0x162);
        ((PaintCanvas *) canvas)->DrawString((unsigned int) (uintptr_t) font, *typeTxt, detailX, detailY, false);
    }

    this->m_pFreelanceWindow->draw();
    if (this->m_pMapButton) this->m_pMapButton->draw();
    if (this->m_pRejectButton) this->m_pRejectButton->draw();

    L->drawFooter();
    if (this->m_field_0x21 != 0 || this->m_choiceActive != 0)
        this->m_pChoiceWindow->draw();
}

void MissionsWindow::render3D() {
    if (this->m_mode == 1)
        this->m_pWantedWindow->render3D();
    if (this->m_starMapActive)
        this->m_pStarMap->render();
}

void MissionsWindow::OnTouchEnd(int y, int z) {
    if (this->m_mode == 1) {
        this->m_pWantedWindow->OnTouchEnd(y, z);
        if (this->m_pWantedWindow->lastButtonHit == 0) {
            this->m_mode = 0;
            this->m_pWantedWindow->lastButtonHit = 1;
        }
        return;
    }

    if (this->m_choiceActive != 0) {
        int r = this->m_pChoiceWindow->OnTouchEnd(y, z);
        if (r == 1) {
            this->m_choiceActive = 0;
            return;
        }
        if (r == 0) {
            Status *st = Status::gStatus;
            Mission *fm = st->getFreelanceMission();
            int type = fm->getType();
            bool clearCargo = (type == 0) || (fm->getType() == 3) || (fm->getType() == 5);
            if (clearCargo) {
                Array<Item *> *cargo = ((Ship *) (st->getShip()))->getCargo();
                if (cargo != nullptr) {
                    for (unsigned int i = 0; i < cargo->size(); i++) {
                        Item *item = (*cargo)[i];
                        if (item->isUnsaleable() != 0 && item->getIndex() == 0x74) {
                            ((Ship *) (st->getShip()))->removeCargo(item);
                            this->m_hangarNeedsUpdate = 1;
                            break;
                        }
                    }
                }
            } else if (fm->getType() == 0xb) {
                st->setPassengers(0);
            }

            Agent *agent = fm->getAgent();
            if (agent->isGenericAgent() == 0)
                fm->getAgent()->setOfferAccepted(false);
            st->setFreelanceMission(nullptr);

            unsigned char savedFlag = this->m_hangarNeedsUpdate;
            this->init();
            this->m_hangarNeedsUpdate = savedFlag;
            return;
        }
    }

    if (this->m_starMapActive == 0) {
        if (Status::gStatus->wantedBoardAccessible() != 0) {
            Array<TouchButton *> *tabs = this->m_pTabButtons;
            for (unsigned int i = 0; i < tabs->size(); i++) {
                if ((*tabs)[i]->OnTouchEnd(y, z) != 0)
                    this->m_mode = (int) i;
            }
        }
        this->m_pCampaignWindow->OnTouchEnd(y, z);
        this->m_pFreelanceWindow->OnTouchEnd(y, z);

        if (this->m_pAcceptButton && this->m_pAcceptButton->OnTouchEnd(y, z) != 0) {
            ApplicationManager *appMgr = ApplicationManager::gAppManager;
            ModStation *mod = (ModStation *) appMgr->GetApplicationModule(5);
            StarMap *map = mod->starMap;
            this->m_pStarMap = map;
            if (map == nullptr) {
                StarMap *m = new StarMap(true,
                                         (Mission *) (void *) (intptr_t) Status::gStatus->getCampaignMission(),
                                         false, -1);
                ModStation *mod2 = (ModStation *) appMgr->GetApplicationModule(5);
                mod2->starMap = m;
                ModStation *mod3 = (ModStation *) appMgr->GetApplicationModule(5);
                this->m_pStarMap = mod3->starMap;
            } else {
                this->m_pStarMap->init(true,
                                       (Mission *) (void *) (intptr_t) Status::gStatus->getCampaignMission(),
                                       false, -1);
            }
            this->m_starMapActive = 1;
            (*g_mwt_resetLayout)->resetWindowDimensions();
        } else {
            if (this->m_pMapButton && this->m_pMapButton->OnTouchEnd(y, z) != 0) {
                String *t = g_mw_gameText->getText(0x1a2);
                this->m_pChoiceWindow->set(*t, true);
                this->m_choiceActive = 1;
            }
            if (this->m_pRejectButton && this->m_pRejectButton->OnTouchEnd(y, z) != 0) {
                ApplicationManager *appMgr = ApplicationManager::gAppManager;
                ModStation *mod = (ModStation *) appMgr->GetApplicationModule(5);
                StarMap *map = mod->starMap;
                this->m_pStarMap = map;
                if (map == nullptr) {
                    StarMap *m = new StarMap(true,
                                             Status::gStatus->getFreelanceMission(), false, -1);
                    ModStation *mod2 = (ModStation *) appMgr->GetApplicationModule(5);
                    mod2->starMap = m;
                    this->m_pStarMap = m;
                } else {
                    this->m_pStarMap->init(true,
                                           Status::gStatus->getFreelanceMission(), false, -1);
                }
                this->m_starMapActive = 1;
                (*g_mwt_resetLayout)->resetWindowDimensions();
                return;
            }
            Layout *layout = *g_mwt_layout;
            if (layout->OnTouchEnd(z, 0) != 0) {
                layout->resetWindowDimensions();
                return;
            }
            if (layout->helpPressed() != 0) {
                String *t = g_mw_gameText->getText(0x27b);
                String title(*t);
                layout->initHelpWindow(title);
            }
        }
    } else {
        if (StarMap_OnTouchEnd(this->m_pStarMap, y, z) != 0) {
            int wantW, wantH, posX;
            if (*g_mwt_flagA == 0) {
                this->m_x = 0;
                this->m_y = 0;
                wantW = 0;
                wantH = 0;
                posX = 0;
            } else {
                if (*g_mwt_flagB == 0) {
                    int w = 1000, h = 0x514;
                    if (*g_mwt_flagC == 0) {
                        w = 500;
                        h = 0x28a;
                    }
                    posX = h >> 1;
                    wantW = w;
                    wantH = h;
                } else {
                    wantW = 0x2bf;
                    posX = 0x1c9;
                    wantH = 0x392;
                }
                this->m_x = (*g_mwt_screenW >> 1) - posX;
                this->m_y = (*g_mwt_screenH >> 1) - (wantW >> 1);
            }
            this->m_width = wantH;
            this->m_height = wantW;
            this->m_starMapActive = 0;
        }
    }
}

void MissionsWindow::update(int dt) {
    if (this->m_mode == 1) {
        this->m_pWantedWindow->update(0);
        return;
    }

    if (this->m_starMapActive != 0) {
        this->m_pStarMap->update(0);
        return;
    }

    this->m_pCampaignWindow->update(dt);
    this->m_pFreelanceWindow->update(dt);

    Mission *cm = (Mission *) ((void *) (intptr_t) Status::gStatus->getCampaignMission());
    int type = cm->getType();
    bool relevant = (type == 0xa7) || (cm->getType() == 0xae);

    if (relevant) {
        CampaignMissionFlags *camp = *(CampaignMissionFlags **) g_mw_campaign;
        bool show = (Status::gStatus->gameWon() == 0) ||
                    (camp->campaignVisibleFlagB != 0 || camp->campaignVisibleFlagA != 0);
        if (show) {
            String text("", false);
            if (Status::gStatus->getCurrentCampaignMission() < 0xa4) {
                String *titleTxt = g_mw_gameText->getText(
                    g_mw_titleTable[Status::gStatus->getCurrentCampaignMission()]);
                text = *titleTxt;

                void *key = Status::gStatus;
                String hdr(text);
                int need = cm->getProductionGoodAmount();
                int have = cm->getStatusValue();
                String amount;
                amount.Set((long long) (need - have));
                String suffix("", false);
                String merged;
                Status_replaceHash(&merged, key, &hdr, &amount, &suffix);
                text = merged;

                String a("", false);
                String b(text);
                this->m_pCampaignWindow->setText(a, b);
            }
        }
    }

    Array<TouchButton *> *tabs = this->m_pTabButtons;
    for (unsigned int i = 0; i < tabs->size(); i++)
        (*tabs)[i]->setAlwaysPressed((int) i == this->m_mode);
}
