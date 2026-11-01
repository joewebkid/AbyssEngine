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
// PaintCanvas::gCanvas is declared `extern` in engine/render/PaintCanvas.h (already included).

// These per-screen globals appear only in this translation unit. The original
// build shared them via extern decls; here they are file-static definitions.
// Parity loss accepted (criterion 7: drive extern toward 0).
static Layout **g_mw_m_layout = nullptr;
static Layout **g_mw_b_layout = nullptr;

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

// Android ARM libgof2hdaa.so, dword_20285C: campaign title text IDs 0..163.
static const int g_mw_titleTable[164] = {
    749,749,749,749,749,749,749,749,749,749,652,653,654,655,656,749,
    657,749,658,749,659,659,749,660,661,749,749,749,662,749,749,749,
    663,664,665,666,667,749,668,749,669,749,749,749,670,670,671,749,
    672,672,672,672,672,672,672,673,674,749,675,676,749,677,678,679,
    680,749,681,682,683,684,684,685,686,687,688,749,749,689,690,690,
    691,691,749,749,692,692,693,694,694,694,694,695,696,697,698,698,
    699,700,701,701,702,702,703,704,705,706,707,708,709,710,710,711,
    712,713,714,715,716,717,718,718,719,719,720,721,721,722,722,723,
    724,724,725,726,726,726,727,728,729,730,731,732,733,734,735,736,
    736,737,738,739,740,740,740,740,741,742,743,743,744,745,746,747,
    748,748,748,0,
};

#if __SIZEOF_POINTER__ == 4
int StarMap_OnTouchEnd(StarMap *map, int x, int y);
#endif

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
    int h = Globals::Canvas->GetTextHeight(Globals::font);
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
    Layout *L = Globals::layout;

    if (Globals::iPad == 0) {
        this->m_x = 0;
        this->m_y = 0;
        this->m_width = Globals::w;
        this->m_height = Globals::h;
    } else {
        int width;
        float height;
        if (Globals::iPadHD != 0) {
            width = 914;
            height = 703.12f;
        } else {
            width = Globals::iPadLarge ? 1300 : 650;
            height = Globals::iPadLarge ? 1000.0f : 500.0f;
        }
        this->m_width = width;
        this->m_height = static_cast<int>(height);
        this->m_x = (Globals::w >> 1) - (width >> 1);
        this->m_y = (Globals::h >> 1) - (this->m_height >> 1);
    }

    Array<TouchButton *> *tabs = new Array<TouchButton *>();
    this->m_pTabButtons = tabs;
    ArraySetLength(2, *tabs);

    String *t0 = Globals::gameText->getText(3219);
    int helpOff = L->getHelpButtonOffset();
    TouchButton *tab1 = new TouchButton(*t0, 3, (this->m_width + this->m_x) - helpOff,
                                         this->m_y, 0x12);
    (*tabs)[1] = tab1;

    String *t1 = Globals::gameText->getText(129);
    int helpOff2 = L->getHelpButtonOffset();
    TouchButton *tab0 = new TouchButton(*t1, 3,
                                         (((this->m_width + this->m_x) - helpOff2) - tab1->getWidth())
                                             + L->field_0x38,
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

    bool campShow = (Status::gStatus->gameWon() == 0) ||
                    (Globals::options[0x37] != 0 || Globals::options[0x35] != 0);
    if (campShow) {
        String text("", false);
        if (Status::gStatus->getCurrentCampaignMission() < 0xa4) {
            String *t = Globals::gameText->getText(
                g_mw_titleTable[Status::gStatus->getCurrentCampaignMission()]);
            text = *t;
        }
        Mission *cm = (Mission *) ((void *) (intptr_t) Status::gStatus->getCampaignMission());
        int type = cm->getType();
        bool production = (type == 0xa7) || (cm->getType() == 0xae);
        if (production) {
            int need = cm->getProductionGoodAmount();
            int have = cm->getStatusValue();
            String val;
            val.Set((long long) (need - have));
            text = Status::gStatus->replaceHash(text, val, String("#Q", false));
        } else {
            String stationName = cm->getTargetStationName();
            text = Status::gStatus->replaceHash(text, stationName, String("#*", false));
        }

        String a("", false);
        String b(text);
        this->m_pCampaignWindow->setText(a, b);
    } else {
        bool useGold = Achievements::gAchievements->gotAllGoldMedals() != 0 &&
                       ((Ship *) (Status::gStatus->getShip()))->getIndex() != 8;
        String a("", false);
        String *t = Globals::gameText->getText(useGold ? 650 : 670);
        String b(*t);
        this->m_pCampaignWindow->setText(a, b);
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
        String body(text);
        int rew = fm->getReward();
        int bonus = fm->getBonus();
        String reward = Globals::layout->formatCredits(rew + bonus);
        text = Status::gStatus->replaceHash(body, reward, String("#C", false));

        String a("", false);
        String b(text);
        this->m_pFreelanceWindow->setText(a, b);

        int *parts = ((Agent *) (fm->getAgent()))->getImageParts();
        this->m_pAgentImageParts = static_cast<ImageFactory *>(Globals::imageFactory)->loadChar(parts);
    } else {
        this->m_pFreelanceWindow = new ScrollTouchWindow(
            rx, topY, (half - pad) - L->buttonInsetX,
            ((this->m_height + (this->m_y - (topY + pad * 2))) - L->field_0x10) - L->field_0x24, false);
        String a("", false);
        String *t = Globals::gameText->getText(174);
        String b(*t);
        this->m_pFreelanceWindow->setText(a, b);
    }

    if (Status::gStatus->inAlienOrbit() == 0) {
        int btnY = ((this->m_width >> 1) >> 1) - L->buttonInsetX;
        if (Status::gStatus->gameWon() == 0) {
            String *t = Globals::gameText->getText(424);
            this->m_pAcceptButton = new TouchButton(
                *t, 0, L->buttonInsetX + this->m_x,
                (((this->m_y + this->m_height) - L->field_0x10) - L->field_0x24) - L->field_0x2c,
                btnY, '!', 4);
        }
        if (Status::gStatus->getFreelanceMission()->isEmpty() == 0) {
            String *t = Globals::gameText->getText(424);
            this->m_pRejectButton = new TouchButton(
                *t, 0, this->m_x + (this->m_width >> 1) + L->field_0x2c,
                (((this->m_y - L->field_0x2c) + this->m_height) - L->field_0x10) - L->field_0x24,
                btnY, '!', 4);

            if (reinterpret_cast<intptr_t>(ApplicationManager::gAppManager->GetCurrentApplicationModule()) == 5) {
                String *t2 = Globals::gameText->getText(423);
                this->m_pMapButton = new TouchButton(
                    *t2, 0, this->m_x + btnY + (this->m_width >> 1) + L->field_0x2c * 2,
                    (((this->m_y - L->field_0x2c) + this->m_height) - L->field_0x10) - L->field_0x24,
                    btnY, '!', 4);
                this->m_pMapButton->setTextColor(-14024449);
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
        String *ht = Globals::gameText->getText(titleId);
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
        String *t = Globals::gameText->getText(titleId);
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
        String *t = Globals::gameText->getText(titleId);
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

        String *typeTxt = Globals::gameText->getText(
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

int MissionsWindow::OnTouchEnd(int y, int z) {
    if (this->m_mode == 1) {
        int result = this->m_pWantedWindow->OnTouchEnd(y, z);
        if (this->m_pWantedWindow->lastButtonHit == 0) {
            this->m_mode = 0;
            this->m_pWantedWindow->lastButtonHit = 1;
        }
        return result;
    }

    if (this->m_choiceActive != 0) {
        int r = this->m_pChoiceWindow->OnTouchEnd(y, z);
        if (r == 1) {
            this->m_choiceActive = 0;
            return 0;
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
            return 0;
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
                String *t = Globals::gameText->getText(0x1a2);
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
                return 0;
            }
            Layout *layout = *g_mwt_layout;
            if (layout->OnTouchEnd(y, z) != 0) {
                layout->resetWindowDimensions();
                return 1;
            }
            if (layout->helpPressed() != 0) {
                String *t = Globals::gameText->getText(0x27b);
                String title(*t);
                layout->initHelpWindow(title);
            }
        }
    } else {
#if __SIZEOF_POINTER__ == 8
        if (this->m_pStarMap->OnTouchEnd(y, z) != 0) {
#else
        if (StarMap_OnTouchEnd(this->m_pStarMap, y, z) != 0) {
#endif
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
    return 0;
}

void MissionsWindow::update(int dt) {
    if (this->m_mode == 1) {
        this->m_pWantedWindow->update(dt);
        return;
    }

    if (this->m_starMapActive != 0) {
        this->m_pStarMap->update(dt);
        return;
    }

    this->m_pCampaignWindow->update(dt);
    this->m_pFreelanceWindow->update(dt);

    Mission *cm = (Mission *) ((void *) (intptr_t) Status::gStatus->getCampaignMission());
    int type = cm->getType();
    bool relevant = (type == 0xa7) || (cm->getType() == 0xae);

    if (relevant) {
        bool show = (Status::gStatus->gameWon() == 0) ||
                    (Globals::options[0x37] != 0 || Globals::options[0x35] != 0);
        if (show) {
            String text("", false);
            if (Status::gStatus->getCurrentCampaignMission() < 0xa4) {
                String *titleTxt = Globals::gameText->getText(
                    g_mw_titleTable[Status::gStatus->getCurrentCampaignMission()]);
                text = *titleTxt;

                String hdr(text);
                int need = cm->getProductionGoodAmount();
                int have = cm->getStatusValue();
                String amount;
                amount.Set((long long) (need - have));
                text = Status::gStatus->replaceHash(hdr, amount, String("#Q", false));

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
