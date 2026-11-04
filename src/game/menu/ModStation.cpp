#include "game/menu/ModStation.h"
#include <cstddef>
#include "game/core/Globals.h"
#include "engine/core/GameText.h"
#include "engine/core/AERandom.h"
#include "game/ship/Ship.h"
#include "game/ship/Agent.h"
#include "engine/audio/FModSound.h"
#include "engine/render/PaintCanvas.h"
#include "engine/render/AEGeometry.h"
#include <new>
#include "game/ui/ChoiceWindow.h"
#include "game/core/CutScene.h"
#include "engine/math/EaseInOut.h"
#include "engine/math/EaseInOutMatrix.h"
#include "game/mission/Item.h"
#include "game/ui/HangarWindow.h"
#include "game/ui/MissionsWindow.h"
#include "game/ui/StatusWindow.h"
#include "game/world/NewsTicker.h"
#include "game/world/StarMap.h"
#include "game/mission/Achievements.h"
#include "engine/core/ApplicationManager.h"
#include "engine/core/GameData.h"
#include "game/ui/DialogueWindow.h"
#include "engine/render/ImageFactory.h"
#include "game/core/Radio.h"
#include "game/core/RadioMessage.h"
#include "game/ui/Layout.h"
#include "game/mission/RecordHandler.h"
#include "game/menu/SpaceLounge.h"
#include "game/mission/Generator.h"
#include "game/mission/Mission.h"
#include "game/mission/PendingProduct.h"
#include "game/world/Station.h"
#include "game/world/Standing.h"
#include "game/mission/Status.h"
#include "game/world/SolarSystem.h"
#include "game/ui/TouchButton.h"
#include "game/ui/MenuTouchWindow.h"
#include "game/ui/ScrollTouchBox.h"
#include "game/world/Wanted.h"
#include "engine/math/AEMath.h"

using AbyssEngine::AEMath::VectorSignedToFloat;
using AbyssEngine::AEMath::Matrix;

namespace {

// Android libgof2hdaa.so rodata: 0x1fdf40 (iPad) and 0x1fdfb8 (phone).
constexpr int kStationCameraCoordinatesIPad[9][3] = {
    {725, 899, -2271}, {1630, 1199, -1933}, {-2934, 1027, -1890},
    {1759, 909, -2232}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {1200, 800, -2978}, {1759, 909, -2232},
};

constexpr int kStationCameraCoordinatesPhone[9][3] = {
    {1076, 900, -2273}, {1787, 1086, -1632}, {-2247, 1010, -1845},
    {1800, 800, -1778}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {1200, 800, -2678}, {2200, 800, -1678},
};

// Android libgof2hdaa.so rodata: 0x1fe030, 0x1fe058, and 0x1fe080.
constexpr float kStationCameraPitch[9] = {
    -0.27f, -0.30f, -0.18f, -0.20f, 0.0f, 0.0f, 0.0f, -0.20f, -0.20f,
};

constexpr float kStationCameraYawIPad[9] = {
    -3.36f, -3.75f, -2.05f, 2.57f, 0.0f, 0.0f, 0.0f, 3.00f, 2.35f,
};

constexpr float kStationCameraYawPhone[9] = {
    -3.36f, -3.75f, -2.05f, 2.57f, 0.0f, 0.0f, 0.0f, 3.00f, 2.50f,
};

// Android rodata 0x1fe0e0: credit overrides for campaign steps 55..61.
constexpr int kNonCampaignRewardCredits[7] = {
    200000, 6666, 6666, 100000, 6666, 6666, 50000,
};

const int *modStationCameraCoordinates() {
    return Globals::iPad ? &kStationCameraCoordinatesIPad[0][0]
                         : &kStationCameraCoordinatesPhone[0][0];
}

const float *modStationCameraYaw() {
    return Globals::iPad ? kStationCameraYawIPad : kStationCameraYawPhone;
}

} // namespace

// ---- Models for untyped runtime handles touched via byte offsets ----

// First bytes of a HangarWindow object (only the +0 dirty flag is poked here).
struct HangarWindowHead {
    char shipChanged;         // 0x00  set when the player's ship was swapped
};

// Hint/medal record: a run of per-hint "shown" flag bytes.
struct HintRecord {
    char flags[0x40];         // byte flags indexed by hint id (0x1a,0x1b,0x33..0x36,0x34,0x3a,...)
};

#if __SIZEOF_POINTER__ == 4
static_assert(offsetof(Layout, rewardMessageActive) == 0x2ec, "");
#endif



int FModSound_tryToStopMusicForBGMusic();





void *ArrayReleaseClasses_TouchButton(void *a);

void *Array_TB_dtor(void *p);


void ModStation::autosave() {
    if (Status::gStatus->getPlayingTime() - 1LL < 0)
        return;
    RecordHandler *rh = new RecordHandler();
    rh->recordStoreWrite(0);
    rh->recordStoreWritePreview(0);
    delete rh;
    this->cameraFlags.bytes[1] = 1;
    if ((int) (intptr_t) this->dlcMenu != 0) {
    }
}


void ModStation::enterStation() {
    Status *status = Globals::status;
    status->departStation(status->getStation());
    Globals::status->getStation()->visit();
    static_cast<Achievements *>(Globals::achievements)->applyNewMedals();

    Item *e10 = Globals::status->getShip()->getFirstEquipmentOfSort(10);
    Item *e9 = Globals::status->getShip()->getFirstEquipmentOfSort(9);
    Globals::status->field_150 = Globals::status->getShip()->getIndex();
    Globals::status->field_154 = e10 ? e10->getIndex() : -1;
    Globals::status->field_158 = e9 ? e9->getIndex() : -1;
    Globals::status->field_124 = 0;
    Globals::status->field_11c = 0;
}


extern int g_android_current_achievements[3];

void ModStation::addAchievement(int medalId, int kind) {
    if (static_cast<Achievements *>(Globals::achievements)->isEliteMedal(medalId) != 0)
        return;
    if ((unsigned) (kind - 1) < 2) {
        if (static_cast<Achievements *>(Globals::achievements)->getValue(medalId, 3) != -1)
            g_android_current_achievements[0] = medalId * 3;
        if (kind == 1 &&
            static_cast<Achievements *>(Globals::achievements)->getValue(medalId, 2) != -1)
            g_android_current_achievements[1] = medalId * 3 + 1;
    }
    g_android_current_achievements[2] = (medalId * 3 + 3) - kind;
}

void ModStation::showMapWindow() {
    this->subWindowFlags.bytes[0] = 0;
    this->subWindowFlags.bytes[3] = 1;
}

void ModStation::setGameLoaded() {
    this->cameraFlags.bytes[0] = 1;
    this->cameraFlags.bytes[3] = 1;
}


void ModStation::OnSuspend() {
    this->activeTouch = 0;
    RecordHandler *recordHandler = static_cast<RecordHandler *>(Globals::recordHandler);
    if (recordHandler != nullptr)
        recordHandler->saveOptions();
}


void ModStation::OnResume() {
    FModSound *&sound = Globals::sound;
    if (sound == nullptr)
        return;
    if (sound->tryToStopMusicForBGMusic() != 0)
        return;
    sound->setVolume(1, *reinterpret_cast<float *>(Globals::options));
}


void ModStation::checkPendingProducts() {
    int campaignStep = Globals::status->getCurrentCampaignMission();
    String *header = Globals::gameText->getText(0xd5);
    String message = *header + String("\n", false);
    Array<PendingProduct *> *products = Globals::status->getPendingProducts();
    if (products) {
        for (unsigned i = 0; i < products->size(); ++i) {
            PendingProduct *product = (*products)[i];
            if (!product || product->stationIndex !=
                Globals::status->getStation()->getIndex())
                continue;

            if (campaignStep == 0x92 && product->blueprintIndex == 0xd2) {
                delete (*products)[i];
                (*products)[i] = nullptr;
                return;
            }

            Array<Item *> *items = static_cast<Array<Item *> *>(Globals::items);
            Item *item = (*items)[product->blueprintIndex]->makeItem(product->quantity);
            Globals::status->getShip()->addCargo(item);
            {
                String newline("\n", false);
                String withNewline = message + newline;
                String amount(item->getAmount());
                String withAmount = withNewline + amount;
                String marker("x ", false);
                String withMarker = withAmount + marker;
                String withName = withMarker +
                    *Globals::gameText->getText(item->getIndex() + 0x4fa);
                message = withName;
            }

            delete (*products)[i];
            (*products)[i] = nullptr;
            this->m_nStarMapWindowOpen.bytes[3] = 1;
        }
    }

    if (this->m_nStarMapWindowOpen.bytes[3])
        static_cast<ChoiceWindow *>(this->choiceWindow)->set(message);
}


static bool g_stationMissionsHelpShown = false;
static bool g_stationHangarHelpShown = false;
static bool g_stationStatusHelpShown = false;
static bool g_stationLoungeHelpShown = false;
static bool g_stationMapHelpShown = false;
static bool g_stationEarlyMapHelpShown = false;
// Android .data 0x215690, 0x2156a4, and zero-initialized .bss 0x217bf8.
static int g_stationPirateHangarFace[5] = {2, 0, 0, 0, 0};
static int g_stationRewardFace[5] = {4, 2, 0, 2, 2};
static int g_stationPirateLoungeFace[5] = {};
// Android .data 0x21562c and 0x215640, selected by the mission-116 dialogue.
static int g_stationSupernovaFace1[5] = {0, 6, 0, 7, 4};
static int g_stationSupernovaFace3[5] = {2, 3, 1, 1, 4};

void ModStation::OnKeyPress(long long key, long long) {
    if (this->stationActive == 0)
        return;

    Globals::status->checkForLevelUp();

    int t34 = this->accumTimeHi.word;
    bool banner = 999 < (uint32_t) this->accumTime;
    if (!((-(int) banner - t34) < 0)) {
        if (key == 0x10000) {
            this->accumTimeLo.word = 0x3e9;
            this->accumTimeHi.word = 0;
        }
        return;
    }

    if (this->m_nStarMapWindowOpen.bytes[3] != 0 || this->modalFlags.bytes[1] != 0 ||
        this->subWindowFlags.bytes[1] != 0 || this->subWindowFlags.bytes[2] != 0 ||
        this->subWindowFlags.bytes[3] != 0 || this->subWindowFlags.bytes[0] != 0 ||
        this->modalFlags.bytes[0] != 0)
        return;

    if (this->modalFlags.bytes[2] != 0) {
        if (key == 0x10000)
            this->checkMedals();
        return;
    }

    unsigned char held = this->m_nStarMapWindowOpen.bytes[1];
    char sub = this->m_nStarMapWindowOpen.bytes[2];
    if (held == 0) {
        if (key == 0x40000 && sub == 0)
            this->leaveStation();
        return;
    }

    if (sub != 0) {
        return;
    }

    if (key == 0x4000) {
        int sel = this->selectedButton;
        int *btn = this->buttonState;
        btn[sel] = 0;
        sel = (0 < sel) ? (sel - 1) : 4;
        this->selectedButton = sel;
        btn[sel] = 1;
        return;
    }
    if (key == 0x8000) {
        int sel = this->selectedButton;
        int *btn = this->buttonState;
        btn[sel] = 0;
        sel = (sel < 4) ? (sel + 1) : 0;
        this->selectedButton = sel;
        btn[sel] = 1;
        return;
    }
    if (key == 0x20000)
        return;
    if (key != 0x10000)
        return;

    int which = this->selectedButton;
    if (which == 0) {
        const int campaign = Globals::status->getCurrentCampaignMission();
        if (campaign < 5 || campaign == 48 || campaign == 49 || campaign == 56) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(528));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            return;
        }
        Station *station = Globals::status->getStation();
        if (station->getIndex() == 4 && station->stationHasPirateBase()) {
            if (this->dialogueWindow == nullptr) {
                this->dialogueWindow = new DialogueWindow(
                    Globals::gameText->getText(432),
                    Globals::gameText->getText(1609),
                    g_stationPirateHangarFace);
                this->modalFlags.bytes[1] = 1;
                Agent *agent = new Agent(0, String("", false), 0, 0, 2,
                                         true, 0, 0, 0, 0);
                int soundId = static_cast<Globals *>(Globals::globals)
                                  ->getDialogueSoundId(432, agent);
                Globals::sound->play(soundId, nullptr, nullptr, 0.0f);
                delete agent;
            }
            return;
        }

        HangarWindow *window = static_cast<HangarWindow *>(this->hangarWindow);
        if (window == nullptr) {
            window = new HangarWindow();
            this->hangarWindow = window;
            window->initialize();
        }
        SpaceLounge *lounge = static_cast<SpaceLounge *>(this->spaceLounge);
        if (lounge != nullptr && lounge->hangarNeedsUpdate()) {
            delete window;
            window = new HangarWindow();
            this->hangarWindow = window;
            window->initialize();
            lounge->setHangarUpdate(false);
        } else if (this->missionsWindow != nullptr && this->missionsWindow->hangarNeedsUpdate()) {
            delete window;
            window = new HangarWindow();
            this->hangarWindow = window;
            window->initialize();
            this->missionsWindow->setHangarUpdate(false);
        }

        for (unsigned int i = 0; i < window->tabButtons->size(); ++i) {
            if (i < 10) {
                Globals::sub_menu_buttons_x[i] =
                    static_cast<int>((*window->tabButtons)[i]->getPosition().x);
                Globals::sub_menu_buttons_y[i] =
                    static_cast<int>((*window->tabButtons)[i]->getPosition().y);
            }
        }
        Globals::sub_menu_button_count = static_cast<int>(window->tabButtons->size());

        this->field_0xd0 = 0;
        Array<Item *> *cargo = Globals::status->getShip()->getCargo();
        if (cargo != nullptr) {
            for (unsigned int i = 0; i < cargo->size(); ++i) {
                Item *item = (*cargo)[i];
                if (item->getIndex() >= 132 && item->getIndex() <= 153)
                    this->field_0xd0 += item->getAmount();
            }
        }
        Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 1.0f);
        Globals::sound->stop(122);
        Globals::sound->play(95, nullptr, nullptr, 0.0f);
        auto *engine = static_cast<AbyssEngine::Engine *>(
            static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->GetEngine());
        this->field_0x12c.bytes[0] = engine->IsPostEffectActivated();
        this->subWindowFlags.bytes[2] = 1;
        if (!g_stationHangarHelpShown) {
            g_stationHangarHelpShown = true;
            Globals::layout->initHelpWindow(
                String(*Globals::gameText->getText(622), false));
        }
    } else if (which == 1) {
        const int campaign = Globals::status->getCurrentCampaignMission();
        if (campaign < 13 || campaign == 49) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(528));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            return;
        }
        Station *station = Globals::status->getStation();
        if (station->getIndex() == 88 && station->stationHasPirateBase()) {
            if (this->dialogueWindow == nullptr) {
                this->dialogueWindow = new DialogueWindow(
                    Globals::gameText->getText(433),
                    Globals::gameText->getText(1597),
                    g_stationPirateLoungeFace);
                this->modalFlags.bytes[1] = 1;
                Agent *agent = new Agent(0, String("", false), 0, 0, 0,
                                         true, 0, 0, 0, 0);
                int soundId = static_cast<Globals *>(Globals::globals)
                                  ->getDialogueSoundId(433, agent);
                Globals::sound->play(soundId, nullptr, nullptr, 0.0f);
                delete agent;
            }
            return;
        }
        if (this->spaceLounge != nullptr) {
            static_cast<SpaceLounge *>(this->spaceLounge)->init();
        } else {
            this->spaceLounge = new SpaceLounge();
        }
        Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 2.0f);
        Globals::sound->stop(122);
        Globals::sound->play(108, nullptr, nullptr, 0.0f);
        this->subWindowFlags.bytes[1] = 1;
        if (!g_stationLoungeHelpShown) {
            g_stationLoungeHelpShown = true;
            Globals::layout->initHelpWindow(
                String(*Globals::gameText->getText(627), false));
        }
    } else if (which == 2) {
        const int campaign = Globals::status->getCurrentCampaignMission();
        if (campaign < 9 || campaign == 48 || campaign == 49) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(528));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            return;
        }
        Ship *ship = Globals::status->getShip();
        if (ship->getCurrentLoad() > ship->getMaxLoad()) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(204));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            return;
        }
        if (campaign == 77 && ship->getIndex() != 37) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(326));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            return;
        }
        if (this->starMap != nullptr) {
            this->starMap->init(false, nullptr, false, -1);
        } else {
            this->starMap = new StarMap(false, nullptr, false, -1);
        }
        if (ship->hasEquipment(85, 1) || ship->hasJumpDriveIntegrated()) {
            this->starMap->setJumpMapMode(false, true);
        }
        this->subWindowFlags.bytes[3] = 1;
        Globals::sound->stop(122);
        Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.6f);
        if (campaign >= 16 && !g_stationMapHelpShown) {
            g_stationMapHelpShown = true;
            Globals::layout->initHelpWindow(
                String(*Globals::gameText->getText(628), false));
        } else if (campaign <= 15 && !g_stationEarlyMapHelpShown) {
            g_stationEarlyMapHelpShown = true;
            Globals::layout->initHelpWindow(
                String(*Globals::gameText->getText(631), false));
        }
    } else if (which == 3) {
        if (Globals::status->getCurrentCampaignMission() <= 8) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(528));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            return;
        }
        if (this->missionsWindow != nullptr) {
            this->missionsWindow->init();
        } else {
            this->missionsWindow = new MissionsWindow();
        }
        this->subWindowFlags.bytes[0] = 1;
        Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.6f);
        if (!g_stationMissionsHelpShown) {
            g_stationMissionsHelpShown = true;
            Globals::layout->initHelpWindow(
                String(*Globals::gameText->getText(635), false));
        }
    } else if (which == 4) {
        StatusWindow *window = static_cast<StatusWindow *>(this->statusWindow);
        if (window == nullptr) {
            window = new StatusWindow();
            this->statusWindow = window;
        }
        window->reInit();
        this->modalFlags.bytes[0] = 1;
        Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.6f);
        if (!Globals::iPad && !g_stationStatusHelpShown) {
            g_stationStatusHelpShown = true;
            Globals::layout->initHelpWindow(
                String(*Globals::gameText->getText(640), false));
        }
    }
}

long long ModStation::OnKeyRelease(long long, long long key) {
    return key;
}

int ModStation::ShowLoadingScreen() {
    return 1;
}


static int *g_msc_stack = 0;

ModStation::ModStation() {
    this->dt = 0;
    this->cameraTweenFlags.bytes[3] = 0;
    this->cameraTweenFlags.halfword = 0;
    this->stationActive = 0;
    this->modalFlags.bytes[1] = 0;
    this->state = 100;
    this->starMap = 0;
    this->cutScene = 0;
    this->newsTicker = 0;
    this->cameraTweenFlags.word = 0;
    this->dlcMenu = 0;
    this->activeMission = 0;
    this->radioMessages = 0;
    this->screenFlags.bytes[2] = 0;
    this->closeHangarAfterCredits = 0;
    this->missionsWindow = 0;
    this->choiceWindow = 0;

    Station *st = Status::gStatus->getStation();
    int race;
    if (st->getIndex() == 0x65) {
        race = 8;
    } else {
        st = Status::gStatus->getStation();
        if (st->getIndex() == 100) {
            race = 7;
        } else {
            race = Status::gStatus->getSystem()->getRace();
        }
    }

    AbyssEngine::EaseInOutMatrix *cam;
    {
        const int *coord = modStationCameraCoordinates();
        const float *yawTable = modStationCameraYaw();

        int ix = race * 3, iy = race * 3 + 1, iz = race * 3 + 2;
        float kx = VectorSignedToFloat(coord[ix], 0);
        float ky = VectorSignedToFloat(coord[iy], 0);
        float kz = VectorSignedToFloat(coord[iz], 0);
        float pitch = kStationCameraPitch[race];
        float yaw = yawTable[race];

        this->camKeyX = kx;
        this->camKeyY = ky;
        this->camKeyZ = kz;

        Matrix nearKey, farKey;
        AbyssEngine::AEMath::MatrixSetTranslation(nearKey, kx, ky, kz);
        AbyssEngine::AEMath::MatrixSetRotation(
            nearKey, pitch, yaw, 0.0f, AbyssEngine::AEMath::ROTATION_ORDER_YXZ);

        AbyssEngine::AEMath::MatrixSetTranslation(farKey, kx, ky, kz);
        AbyssEngine::AEMath::MatrixSetRotation(
            farKey, pitch, yaw, 0.0f, AbyssEngine::AEMath::ROTATION_ORDER_YXZ);

        cam = new AbyssEngine::EaseInOutMatrix(nearKey, farKey, 3000);
    }
    this->cameraTween = cam;

    unsigned camHandle = *(unsigned *) PaintCanvas::gCanvas;
    AbyssEngine::Matrix *cur = (AbyssEngine::Matrix *) PaintCanvas::gCanvas->CameraGetCurrent();
    PaintCanvas::gCanvas->CameraSetLocal(camHandle, *cur);

    this->easeX = new AbyssEngine::EaseInOut();
    this->easeY = new AbyssEngine::EaseInOut();
    this->easeZ = new AbyssEngine::EaseInOut();
}

ModStation::~ModStation() {
    this->OnRelease();
}

void ModStation::leaveStation() {
    int noticeTextId;
    bool checkCargo;

    if (Globals::status->getShip()->getCurrentLoad() >
        Globals::status->getShip()->getMaxLoad()) {
        noticeTextId = 0xcc;
        goto showNotice;
    }

    checkCargo = Globals::status->getCurrentCampaignMission() == 6;
    if (!checkCargo && Globals::status->getCurrentCampaignMission() == 7) {
        // The getter returns the float's raw bits in r0 on Android ARM.
        union { int bits; float value; } firePower = {
            Globals::status->getShip()->getFirePower()
        };
        checkCargo = firePower.value == 0.0f ||
            Globals::status->getShip()->getCombinedHP() ==
            Globals::status->getShip()->getBaseHP();
    }
    if (checkCargo) {
        if (Globals::status->getShip()->hasCargoType(0) ||
            Globals::status->getShip()->hasCargoType(3))
            noticeTextId = 0x212;
        else
            noticeTextId = 0x211;
        goto showNotice;
    }

    if (Globals::status->getCurrentCampaignMission() == 20 &&
        Globals::status->getStation()->getIndex() ==
        reinterpret_cast<Mission *>(Globals::status->getCampaignMission())->getTargetStation()) {
        noticeTextId = 0x213;
        goto showNotice;
    }

    if (Globals::status->getCurrentCampaignMission() == 21 &&
        Globals::status->getStation()->getIndex() ==
        reinterpret_cast<Mission *>(Globals::status->getCampaignMission())->getTargetStation()) {
        if (!Globals::status->getShip()->hasEquipment(41, 1) &&
            !Globals::status->getShip()->hasEquipment(42, 1) &&
            !Globals::status->getShip()->hasEquipment(43, 1)) {
            noticeTextId = 0x213;
            goto showNotice;
        }
    } else if (Globals::status->getCurrentCampaignMission() == 77 &&
               Globals::status->getShip()->getIndex() != 37) {
        noticeTextId = 0x146;
        goto showNotice;
    }

    if (this->m_nStarMapWindowOpen.bytes[3] != 0 ||
        this->modalFlags.bytes[1] != 0 ||
        this->modalFlags.bytes[2] != 0)
        return;

    static_cast<ChoiceWindow *>(this->choiceWindow)->set(
        *Globals::gameText->getText(0x18d), true);
    static_cast<ChoiceWindow *>(this->choiceWindow)->left();
    this->cameraFlags.bytes[2] = 1;
    this->m_nStarMapWindowOpen.bytes[3] = 1;
    return;

showNotice:
    static_cast<ChoiceWindow *>(this->choiceWindow)->set(
        *Globals::gameText->getText(noticeTextId));
    this->m_nStarMapWindowOpen.bytes[3] = 1;
}


// Android 2.0.16 .rodata 0x1fb848, shared with ChoiceWindow's credit label.
static const int g_ModStation_medalCredits[4] = {0, 5000, 2500, 1000};

void ModStation::checkMedals() {
    Globals::gGlobals->reportLeaderboards();

    if (this->modalFlags.bytes[2] != 0 || this->m_nStarMapWindowOpen.bytes[3] != 0) {
        int idx = this->medalIndex + 1;
        this->medalIndex = idx;
        if (idx >= this->medalCount) {
            this->modalFlags.bytes[2] = 0;
            return;
        }
        Array<int *> *medalArr = this->medalArray;
        int *medal = (*medalArr)[idx];
        ((ChoiceWindow *) (this->medalChoiceWindow))->setMedal(medal[0], medal[1]);
        int delta = g_ModStation_medalCredits[medal[1]];
        if (Globals::status->hardCoreMode() == 0)
            Globals::status->changeCredits(delta);
        int *p = (*medalArr)[this->medalIndex];
        this->addAchievement(p[0], p[1]);
        return;
    }

    if (Globals::status->getCurrentCampaignMission() == 1)
        this->addAchievement(0, 1);
    this->medalArray = nullptr;
    int *medals = static_cast<Achievements *>(Globals::achievements)->getNewMedals();
    int count = 0;
    this->medalCount = 0;
    for (int i = 0; i != 0x2d; i++) {
        if (medals[i] >= 1) {
            count++;
            this->medalCount = count;
        }
    }
    if (count < 1)
        return;

    Array<int *> *arr = new Array<int *>();
    this->medalArray = arr;
    ArraySetLength(this->medalCount, *arr);
    int j = 0;
    this->medalCount = 0;
    for (int i = 0; i != 0x2d; i++) {
        if (medals[i] >= 1) {
            int *cell = new int[2];
            (*arr)[j] = cell;
            cell[0] = i;
            (*arr)[this->medalCount][1] = medals[i];
            j = this->medalCount + 1;
            this->medalCount = j;
        }
    }
    this->medalIndex = 0;
    this->modalFlags.bytes[2] = 1;
    ChoiceWindow *cw = new ChoiceWindow();
    this->medalChoiceWindow = cw;
    int *medal = (*arr)[0];
    cw->setMedal(medal[0], medal[1]);
    if (Globals::status->hardCoreMode() == 0)
        Globals::status->changeCredits(g_ModStation_medalCredits[medal[1]]);
    int *p = (*arr)[0];
    this->addAchievement(p[0], p[1]);
}

void ModStation::OnRender3D() {
    if (this->stationActive == 0)
        return;
    PaintCanvas::gCanvas->ClearBuffer(0);

    uint8_t *p65 = &this->subWindowFlags.bytes[1];
    if (this->cutScene == 0 || this->subWindowFlags.bytes[2] != 0 || this->subWindowFlags.bytes[0] != 0 ||
        this->modalFlags.bytes[0] != 0 || this->subWindowFlags.bytes[3] != 0 || *p65 != 0 ||
        this->m_nStarMapWindowOpen.bytes[2] != 0) {
        if (this->subWindowFlags.bytes[3] != 0) {
            ((StarMap *) ((void *&) this->starMap))->renderBG();
        } else if (*p65 != 0) {
            ((SpaceLounge *) (this->spaceLounge))->OnRenderBG();
        }
    } else {
        ((CutScene *) (this->cutScene))->renderBG();
    }
    PaintCanvas::gCanvas->Begin3d();
    if (*p65 != 0)
        ((SpaceLounge *) (this->spaceLounge))->OnRender3D();
    else if (this->subWindowFlags.bytes[3] != 0)
        ((StarMap *) ((void *&) this->starMap))->render();
    else if (this->subWindowFlags.bytes[0] != 0)
        this->missionsWindow->render3D();
    else if (this->subWindowFlags.bytes[2] == 0 && this->cutScene != 0)
        ((CutScene *) (this->cutScene))->render3D();
    ((PaintCanvas *) PaintCanvas::gCanvas)->End3d();
}


void ModStation::resetLight() {
    static_cast<Engine *>(static_cast<ApplicationManager *>(
        Globals::appManager)->GetEngine())->LightSetMaterialColorAmbient(
        0.7f, 0.7f, 0.7f);
    static_cast<Engine *>(static_cast<ApplicationManager *>(
        Globals::appManager)->GetEngine())->LightSetLightDirection(
        -this->scrollTargetf, -this->field_0x120, -this->field_0x124, 0x4000);
    static_cast<Engine *>(static_cast<ApplicationManager *>(
        Globals::appManager)->GetEngine())->LightSetLightColorDiffuse(
        1.0f, 1.0f, 1.0f, 0x4000);

    switch (Globals::status->getSystem()->getRace()) {
        case 1:
            static_cast<Engine *>(static_cast<ApplicationManager *>(
                Globals::appManager)->GetEngine())->LightSetLightColorAmbient(
                    0.25f, 0.55f, 0.25f, 0x4000);
            break;
        case 2:
            static_cast<Engine *>(static_cast<ApplicationManager *>(
                Globals::appManager)->GetEngine())->LightSetLightColorAmbient(
                    0.15f, 0.15f, 0.35f, 0x4000);
            break;
        case 3:
            static_cast<Engine *>(static_cast<ApplicationManager *>(
                Globals::appManager)->GetEngine())->LightSetLightColorAmbient(
                    0.45f, 0.25f, 0.25f, 0x4000);
            break;
        case 8:
            static_cast<Engine *>(static_cast<ApplicationManager *>(
                Globals::appManager)->GetEngine())->LightSetLightColorAmbient(
                    0.83f, 0.28f, 0.12f, 0x4000);
            break;
        default:
            static_cast<Engine *>(static_cast<ApplicationManager *>(
                Globals::appManager)->GetEngine())->LightSetLightColorAmbient(
                    0.25f, 0.25f, 0.25f, 0x4000);
            break;
    }
    static_cast<Engine *>(static_cast<ApplicationManager *>(
        Globals::appManager)->GetEngine())->LightSetLightColorSpecular(
        0.5f, 0.5f, 0.5f, 0x4000);
    static_cast<Engine *>(static_cast<ApplicationManager *>(
        Globals::appManager)->GetEngine())->LightSetMaterialColorShininess(96.0f);
}


void ModStation::OnUpdate() {
    Status *status = Globals::status;
    auto *manager = this->manager;

    int elapsed;
    int e = static_cast<int>(manager->GetElapsedTimeMillis());
    if (e < 0x97 && static_cast<int>(manager->GetElapsedTimeMillis()) < 0) {
        elapsed = 0;
    } else {
        if (static_cast<int>(manager->GetElapsedTimeMillis()) < 0x97)
            elapsed = static_cast<int>(manager->GetElapsedTimeMillis());
        else
            elapsed = 0x96;
    }

    this->dt = elapsed;
    this->accumTime += (long long) elapsed;

    Globals::sound->updateAll(nullptr, nullptr, nullptr, nullptr);
    Globals::layout->update(elapsed);
    if (this->m_nStarMapWindowOpen.bytes[2] == 0)
        Globals::status->incPlayingTime(elapsed);

    String credits = Globals::layout->formatCredits(Globals::status->getCredits());
    static_cast<TouchButton *>(this->buttonCredits)->setText(credits);
    static_cast<Achievements *>(Globals::achievements)->updateCredits(
        Globals::status->getCredits());

    if (this->stationActive != 0) {
        if (Globals::enterSpaceLounge != 0) {
            this->alarmFlags.bytes[0] = 1;
            Globals::enterSpaceLounge = 0;
            if (this->spaceLounge == 0) {
                this->spaceLounge = new SpaceLounge();
            } else {
                static_cast<SpaceLounge *>(this->spaceLounge)->init();
            }
            Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 2.0f);
            Globals::sound->stop(0x7a);
            Globals::sound->play(0x6c, nullptr, nullptr, 0.0f);
            this->subWindowFlags.bytes[1] = 1;
            return;
        }
    }

    {
        GameData *appData = static_cast<GameData *>(
            static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->GetApplicationData());
        if (appData->purchaseErrorFlag != 0) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(100));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            appData->purchaseErrorFlag = 0;
        }
        if (appData->purchaseResultFlag != 0 && appData->purchaseCount != nullptr &&
            *appData->purchaseCount != 0 &&
            (unsigned) (appData->purchaseCode - 0x32) < 5) {
            int idx = Globals::gGlobals->getInAppPurchaseArrayIndex(appData->purchaseCode,
                                                                     appData->productIdList);
            if ((*appData->purchaseCreditResults)[idx] >= 0) {
                Globals::status->changeCredits((*appData->purchaseCreditResults)[idx]);
                this->autosave();
                static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                    *Globals::gameText->getText(125));
                this->m_nStarMapWindowOpen.bytes[3] = 1;
                appData->purchaseResultFlag = 0;
                if (this->hangarWindow != nullptr)
                    static_cast<HangarWindow *>(this->hangarWindow)->hideMessage();
                if (this->dlcMenu != nullptr) {
                    delete static_cast<MenuTouchWindow *>(this->dlcMenu);
                }
                this->dlcMenu = 0;
            }
        }
    }

    if (this->subWindowFlags.bytes[0] == 0 && this->subWindowFlags.bytes[1] == 0 &&
        this->subWindowFlags.bytes[2] == 0 && this->subWindowFlags.bytes[3] == 0 &&
        this->modalFlags.bytes[0] == 0 && this->m_nStarMapWindowOpen.bytes[0] == 0) {
        if (this->newsTicker != nullptr)
            this->newsTicker->update(this->dt);
        if (this->cutScene != nullptr)
            this->cutScene->process(0);

        AERandom *random = static_cast<AERandom *>(Globals::rnd);
        auto *easeX = static_cast<AbyssEngine::EaseInOut *>(this->easeX);
        auto *easeY = static_cast<AbyssEngine::EaseInOut *>(this->easeY);
        auto *easeZ = static_cast<AbyssEngine::EaseInOut *>(this->easeZ);

        const bool xIncreasing = this->field_0x12c.bytes[2] == 0;
        easeX->Increase(static_cast<float>(xIncreasing ? elapsed : -elapsed));
        const float x = easeX->GetValue();
        float xDistance = x - easeX->GetMaxValue();
        if (xDistance < 0.0f) xDistance = -xDistance;
        if (xDistance <= 5.0f) {
            this->field_0x12c.bytes[2] ^= 1;
            const int shift = random->nextInt(131) + 18;
            easeX->SetRange(x, this->camKeyX +
                               static_cast<float>(xIncreasing ? -shift : shift));
        }

        const bool yIncreasing = this->field_0x12c.bytes[1] != 0;
        easeY->Increase(static_cast<float>(yIncreasing ? elapsed : -elapsed));
        const float y = easeY->GetValue();
        float yDistance = y - easeY->GetMaxValue();
        if (yDistance < 0.0f) yDistance = -yDistance;
        if (yDistance <= 5.0f) {
            this->field_0x12c.bytes[1] ^= 1;
            const int shift = random->nextInt(120) + 30;
            easeY->SetRange(y, this->camKeyY +
                               static_cast<float>(yIncreasing ? shift : -shift));
        }

        const bool zIncreasing = this->field_0x12c.bytes[3] != 0;
        easeZ->Increase(static_cast<float>(zIncreasing ? elapsed : -elapsed));
        const float z = easeZ->GetValue();
        float zDistance = z - easeZ->GetMaxValue();
        if (zDistance < 0.0f) zDistance = -zDistance;
        if (zDistance <= 5.0f) {
            this->field_0x12c.bytes[3] ^= 1;
            const int shift = random->nextInt(100) + 50;
            easeZ->SetRange(z, this->camKeyZ +
                               static_cast<float>(zIncreasing ? shift : -shift));
        }

        const unsigned camera = Globals::Canvas->CameraGetCurrent();
        Matrix *local = static_cast<Matrix *>(Globals::Canvas->CameraGetLocal(camera));
        AbyssEngine::AEMath::MatrixSetTranslation(*local, x, y, z);
    }

    if (this->accumTime < 1000)
        return;

    if (this->m_nStarMapWindowOpen.bytes[0] != 0) {
        int t = this->field_0xb4;
        if (t < -6000 && t + this->dt > -6001) {
            delete this->cutScene;
            this->cutScene = new CutScene(2);
            this->cutScene->initialize();
            this->cutScene->rotationSpeed = 0.00003f;
            this->m_nStarMapWindowOpen.bytes[0] = 1;
        }
        this->field_0xb4 = t + this->dt;
        this->cutScene->update(this->dt);

        if (reinterpret_cast<Radio *>(this->activeMission)->lastMessageShown() != 0) {
            float halfHeight = static_cast<float>(Globals::h / 2);
            float scrollY = this->radioScrollAnchor.y;
            if (scrollY > halfHeight) {
                float nextY = scrollY + static_cast<float>(this->dt) * -0.03f;
                if (nextY <= halfHeight) {
                    this->radioScrollAnchor.y = nextY;
                    this->field_0xb8 = 4000;
                    scrollY = nextY;
                }
            }
            if (this->field_0xb8 > 0) {
                this->field_0xb8 -= this->dt;
            } else {
                scrollY += static_cast<float>(this->dt) * -0.03f;
                this->radioScrollAnchor.y = scrollY;
                int imageHeight = Globals::Canvas->GetImage2DHeight(this->field_0x5c);
                static_cast<ScrollTouchBox *>(this->scrollBox)->setPosition(
                    static_cast<int>(this->radioScrollAnchor.x),
                    static_cast<int>(scrollY + imageHeight + halfHeight));
            }
        }

        if (this->field_0xb4 >= 136001) {
            this->stationActive = 0;
            Globals::status->nextCampaignMission(true);
            Globals::switch_to_target_setting = 0;
            manager->SetCurrentApplicationModule(5);
            this->m_nStarMapWindowOpen.bytes[0] = 0;
            return;
        }
    } else if (this->modalFlags.bytes[1] != 0) {
        this->dialogueWindow->update(this->dt);
    } else if (this->subWindowFlags.bytes[3] != 0) {
        static_cast<StarMap *>(this->starMap)->update(this->dt);
    } else if (this->subWindowFlags.bytes[2] != 0) {
        static_cast<HangarWindow *>(this->hangarWindow)->update(this->dt);
    } else if (this->subWindowFlags.bytes[0] != 0) {
        this->missionsWindow->update(this->dt);
    } else if (this->subWindowFlags.bytes[1] != 0) {
        static_cast<SpaceLounge *>(this->spaceLounge)->update(this->dt);
    } else if (this->modalFlags.bytes[0] != 0) {
        static_cast<StatusWindow *>(this->statusWindow)->update(this->dt);
    }

    if (this->m_nStarMapWindowOpen.bytes[3] != 0)
        static_cast<ChoiceWindow *>(this->choiceWindow)->update(this->dt);
    if (this->m_nStarMapWindowOpen.bytes[2] != 0)
        static_cast<MenuTouchWindow *>(this->dlcMenu)->update(this->dt);

    if (this->m_nStarMapWindowOpen.bytes[3] == 0 && this->modalFlags.bytes[1] == 0 &&
        this->screenFlags.bytes[0] == 0) {
        if (this->buttonCreditsFlags.bytes[2] == 0) {
            this->checkPendingProducts();
            this->buttonCreditsFlags.bytes[2] = 1;
        }
        if (this->buttonCreditsFlags.bytes[1] == 0) {
            this->checkMedals();
            this->buttonCreditsFlags.bytes[1] = 1;
        }

        if (this->modalFlags.bytes[2] == 0 && this->screenFlags.bytes[2] != 0 &&
            Globals::hints[0x2a] == 0) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(601));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            Globals::hints[0x2a] = 1;
            this->screenFlags.bytes[2] = 0;
        }

        bool special = false;
        const bool intro = this->subWindowFlags.bytes[1] != 0 &&
            static_cast<SpaceLounge *>(this->spaceLounge)->introFinished();
        Mission *completed = status->missionCompleted(true, intro, 0);

        if (completed == nullptr && status->getCurrentCampaignMission() == 116 &&
            this->subWindowFlags.bytes[1] != 0 &&
            static_cast<SpaceLounge *>(this->spaceLounge)->introFinished() &&
            status->getSystem()->getIndex() == 18) {
            Mission *campaign = reinterpret_cast<Mission *>(status->getCampaignMission());
            const int mask = campaign->getStatusValue();
            const int stationIndex = status->getStation()->getIndex();
            const unsigned shift = static_cast<unsigned>(stationIndex - 90);
            const int bit = shift < 32 ? static_cast<int>(1u << shift) : 0;
            if ((mask & bit) == 0) {
                campaign->setStatusValue(mask | bit);
                special = true;
            }
        }

        const int campaignStep = status->getCurrentCampaignMission();
        if (campaignStep >= 148 && campaignStep <= 151) {
            const int stationIndex = status->getStation()->getIndex();
            const int bit = stationIndex == 66 ? 2 :
                            stationIndex == 55 ? 1 :
                            stationIndex == 9 ? 4 : 0;
            const bool introFinished = this->subWindowFlags.bytes[1] != 0 &&
                static_cast<SpaceLounge *>(this->spaceLounge)->introFinished();
            if (completed != nullptr && introFinished && campaignStep <= 150 &&
                stationIndex == 96) {
                status->setCurrentCampaignMission(151);
                this->dialogueMission = completed;
                this->dialogueWindow = new DialogueWindow();
                this->dialogueWindow->set(completed, 1, 151);
                this->modalFlags.bytes[1] = 1;
                goto afterDialogue;
            }
            if (completed == nullptr && introFinished && campaignStep <= 150 &&
                bit != 0) {
                Mission *campaign = reinterpret_cast<Mission *>(status->getCampaignMission());
                if ((campaign->getStatusValue() & bit) == 0) {
                    campaign->setStatusValue(campaign->getStatusValue() | bit);
                    const int dialogueStep = stationIndex == 66 ? 149 :
                                             stationIndex == 9 ? 150 : 148;
                    this->dialogueWindow = new DialogueWindow();
                    Mission *noticeMission = new Mission(160, 0, -1);
                    noticeMission->setCampaignMission(true);
                    this->dialogueWindow->set(noticeMission, 1, dialogueStep);
                    this->modalFlags.bytes[1] = 1;
                    goto afterDialogue;
                }
            }
        }

        if (special) {
            const int stationIndex = status->getStation()->getIndex();
            const int slot = stationIndex == 94 ? 3 : stationIndex - 90;
            Globals::sound->play(slot + 1561, nullptr, nullptr, 0.0f);
            int *face = slot == 0 || slot == 2 ? g_stationPirateLoungeFace :
                        slot == 1 ? g_stationSupernovaFace1 : g_stationSupernovaFace3;
            const int speakerText = slot == 0 || slot == 2 ? 1597 : 1516;
            this->dialogueWindow = new DialogueWindow(
                Globals::gameText->getText(slot + 2716),
                Globals::gameText->getText(speakerText), face);
            this->modalFlags.bytes[1] = 1;
        } else if (completed != nullptr) {
            this->dialogueMission = completed;
            this->dialogueWindow = new DialogueWindow(completed, nullptr, 1);
            this->modalFlags.bytes[1] = 1;
            if (completed->getType() == 13) {
                status->field_0xf0 = 0;
                status->field_0xf1 = 0;
                this->autosave();
            }
            if (this->spaceLounge != nullptr)
                static_cast<SpaceLounge *>(this->spaceLounge)->setHangarUpdate(true);
        } else {
            Mission *failed = status->missionFailed(true, 0);
            if (failed != nullptr) {
                this->dialogueWindow = new DialogueWindow(failed, nullptr, 2);
                this->modalFlags.bytes[1] = 1;
                const bool failedIsType13 = failed->getType() == 13;
                status->removeMission(failed);
                if (failedIsType13) {
                    status->field_0xf0 = 0;
                    status->field_0xf1 = 0;
                    this->autosave();
                }
            }
        }
    afterDialogue:
        this->checkHints();
    }

    if (this->dragFlags.bytes[0] == 0) {
        float decay = static_cast<float>(this->dt) * 0.05f;
        if (decay > 1.0f) decay = 1.0f;
        const float velocity = this->camDragVelocity * (this->camDragScale * decay);
        this->camDragVelocity = velocity;
        const float magnitude = velocity > 0.0f ? velocity : -velocity;
        if (magnitude > 1.0f) {
            this->camAngle = static_cast<int>(velocity + static_cast<float>(this->camAngle));
        }
    }

    CutScene *scene = this->cutScene;
    if (scene != nullptr && this->m_nStarMapWindowOpen.bytes[0] == 0)
        scene->cameraRotX = static_cast<float>(this->camAngle) / 120.0f;

    if (this->scrollFlags.bytes[0] != 0) {
        const int x = this->touchX;
        const int y = this->touchY;
        AEGeometry *geometry = scene->rootGeom;
        const float frame = static_cast<float>(this->dt);

        if (y >= Globals::layout->field_0xc) {
            if (y >= Globals::h - 2 * Globals::layout->field_0x10) {
                if (x > 69) {
                    if (x <= Globals::w - 70) return;
                    geometry->translate(frame, 0.0f, 0.0f);
                    scene->vec8.x += frame;
                } else {
                    geometry->translate(-frame, 0.0f, 0.0f);
                    scene->vec8.x -= frame;
                }
                return;
            }
            if (x > 69) {
                if (x <= Globals::w - 70) {
                    if (x >= 101 && x < Globals::w - 100) {
                        if (y >= Globals::h / 2) {
                            geometry->moveForward(frame);
                            scene->vec8.z += frame;
                        } else {
                            geometry->moveForward(-frame);
                            scene->vec8.z -= frame;
                        }
                    }
                    return;
                }
                geometry->rotate(0.0f, -0.01f, 0.0f);
                scene->stationCameraAngles.y -= 0.01f;
            } else {
                geometry->rotate(0.0f, 0.01f, 0.0f);
                scene->stationCameraAngles.y += 0.01f;
            }
            return;
        }

        if (x > 99) {
            if (x > 199) {
                if (x >= 400) return;
                if (x >= 300) {
                    geometry->rotate(-0.01f, 0.0f, 0.0f);
                    scene->stationCameraAngles.x -= 0.01f;
                } else {
                    geometry->rotate(0.01f, 0.0f, 0.0f);
                    scene->stationCameraAngles.x += 0.01f;
                }
                return;
            }
            geometry->translate(0.0f, 10.0f, 0.0f);
            scene->vec8.y += 10.0f;
        } else {
            geometry->translate(0.0f, -10.0f, 0.0f);
            scene->vec8.y -= 10.0f;
        }
    }
}


void ModStation::resetIdleCamForHangar() {
    if (this->cutScene != 0)
        ((CutScene *) (this->cutScene))->resetCamera();

    if (this->easeX != 0)
        ((AbyssEngine::EaseInOut *) (this->easeX))->SetRange(
            this->camKeyX, this->camKeyX);
    else
        this->easeX = new AbyssEngine::EaseInOut(this->camKeyX,
                                                                        this->camKeyX);

    if (this->easeY != 0)
        ((AbyssEngine::EaseInOut *) (this->easeY))->SetRange(
            this->camKeyY, this->camKeyY);
    else
        this->easeY = new AbyssEngine::EaseInOut(this->camKeyY,
                                                                        this->camKeyY);

    if (this->easeZ != 0)
        ((AbyssEngine::EaseInOut *) (this->easeZ))->SetRange(
            this->camKeyZ, this->camKeyZ);
    else
        this->easeZ = new AbyssEngine::EaseInOut(this->camKeyZ,
                                                                        this->camKeyZ);

    int race;
    Station *st = Status::gStatus->getStation();
    if (st->getIndex() == 0x65) {
        race = 8;
    } else {
        st = Status::gStatus->getStation();
        if (st->getIndex() == 100) {
            race = 7;
        } else {
            race = Status::gStatus->getSystem()->getRace();
        }
    }

    const unsigned int camera = PaintCanvas::gCanvas->CameraGetCurrent();
    Matrix *local = static_cast<Matrix *>(PaintCanvas::gCanvas->CameraGetLocal(camera));
    AbyssEngine::AEMath::MatrixSetTranslation(*local, this->camKeyX, this->camKeyY, this->camKeyZ);
    // The Android reset path always reads the phone yaw table at 0x1fe080.
    AbyssEngine::AEMath::MatrixSetRotation(
        *local, kStationCameraPitch[race], kStationCameraYawPhone[race], 0.0f,
        AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
}


void ModStation::checkHints() {
    if (this->m_nStarMapWindowOpen.bytes[0] != 0)
        return;
    Achievements *achievements = static_cast<Achievements *>(Globals::achievements);

    if (this->modalFlags.bytes[2] == 0 && this->m_nStarMapWindowOpen.bytes[3] == 0 &&
        Globals::options[0x34] == 0 &&
        Globals::status->getCurrentCampaignMission() >= 19) {
        static_cast<ChoiceWindow *>(this->choiceWindow)->set(
            *Globals::gameText->getText(62), *Globals::gameText->getText(73), true,
            *Globals::gameText->getText(126), *Globals::gameText->getText(127),
            *Globals::gameText->getText(524), -1, -1);
        Globals::options[0x34] = 1;
        this->m_nStarMapWindowOpen.bytes[3] = 1;
        this->hintFlags.bytes[0] = 1;
    }

    if (this->modalFlags.bytes[1] == 0) {
        static const unsigned wantedIndex[4] = {6, 12, 18, 24};
        for (int k = 0; k < 4; ++k) {
            if (this->modalFlags.bytes[2] != 0)
                return;
            if (this->m_nStarMapWindowOpen.bytes[3] == 0 &&
                Globals::hints[0x33 + k] == 0) {
                Wanted *wanted = (*Globals::status->wanted)[wantedIndex[k]];
                if (wanted->isTerminated()) {
                    String notice = *Globals::gameText->getText(3232);
                    notice = Globals::status->replaceHash(
                        notice, *Globals::gameText->getText(wanted->getShip() + 913),
                        String("#SHIP_NAME"));
                    notice = Globals::status->replaceHash(
                        notice, wanted->getName(), String("#WANTED_NAME"));
                    static_cast<ChoiceWindow *>(this->choiceWindow)->set(notice);
                    this->m_nStarMapWindowOpen.bytes[3] = 1;
                    Globals::hints[0x33 + k] = 1;
                }
            }
        }
    }

    if (this->modalFlags.bytes[2] == 0) {
        if (this->m_nStarMapWindowOpen.bytes[3] == 0 && Globals::hints[0x1a] == 0 &&
            achievements->gotAllMedals()) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(649));
            Globals::hints[0x1a] = 1;
            this->m_nStarMapWindowOpen.bytes[3] = 1;
        }
        if (this->modalFlags.bytes[2] == 0 && this->m_nStarMapWindowOpen.bytes[3] == 0 &&
            Globals::hints[0x1b] == 0 &&
            achievements->gotAllGoldMedals()) {
            static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                *Globals::gameText->getText(650));
            Globals::hints[0x1b] = 1;
            this->m_nStarMapWindowOpen.bytes[3] = 1;
        }

        if (this->modalFlags.bytes[2] == 0 && this->m_nStarMapWindowOpen.bytes[3] == 0) {
            if (!Globals::status->isBlueprintUnlocked(232) &&
                achievements->gotAllGoldMedals() &&
                achievements->gotAllSupernovaMedals()) {
                static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                    *Globals::gameText->getText(651));
                Globals::status->unlockBluePrint(232);
                this->autosave();
                this->m_nStarMapWindowOpen.bytes[3] = 1;
            }
        }

        if (this->modalFlags.bytes[2] == 0 && this->m_nStarMapWindowOpen.bytes[3] == 0 &&
            Globals::hints[0x3a] == 0 &&
            Globals::status->getCurrentCampaignMission() >= 162) {
            bool ok = (achievements->gotAllGoldMedals() &&
                       achievements->gotAllSupernovaMedals() &&
                       !Globals::status->hardCoreMode()) ||
                      Globals::status->hardCoreMode();
            if (ok) {
                Globals::hints[0x3a] = 1;
                static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                    *Globals::gameText->getText(3233));
                this->autosave();
                this->m_nStarMapWindowOpen.bytes[3] = 1;
            }
        }
    }

    if (this->modalFlags.bytes[1] == 0 && this->modalFlags.bytes[2] == 0 &&
        this->m_nStarMapWindowOpen.bytes[3] == 0) {
        Status *status = Globals::status;
        if (status->wingmen == nullptr || this->alarmFlags.bytes[3] != 0 ||
            status->field_0x30 > 0 || Globals::layout->rewardMessageActive != 0) {
            if (status->field_0xf9 != 0 &&
                Globals::layout->rewardMessageActive == 0) {
                delete this->dialogueWindow;
                this->dialogueWindow = new DialogueWindow(
                    Globals::gameText->getText(442),
                    Globals::gameText->getText(1662), g_stationPirateHangarFace);
                Globals::sound->play(0x24d, nullptr, nullptr, 0.0f);
                this->modalFlags.bytes[1] = 1;
                status->field_0xf9 = 0;
                status->changeCredits(20000);
            }
        } else {
            delete this->dialogueWindow;
            this->dialogueWindow = new DialogueWindow(
                Globals::gameText->getText(313), (*status->wingmen)[0],
                status->getWingmenStats());
            Agent *agent = new Agent(0, String("", false), 0, 0,
                                     status->field_0x2c, true, 0, 0, 0, 0);
            const int soundId = static_cast<Globals *>(Globals::globals)
                                    ->getDialogueSoundId(313, agent);
            Globals::sound->play(soundId, nullptr, nullptr, 0.0f);
            delete agent;
            this->alarmFlags.bytes[3] = 1;
            this->modalFlags.bytes[1] = 1;
            status->setWingmen(nullptr);
        }
    }
}


void ModStation::OnTouchMove(int x, int y, void *touch) {
    if (this->activeTouch != touch)
        return;
    this->touchX = x;
    this->touchY = y;
    if (this->m_nStarMapWindowOpen.bytes[0] != 0)
        return;

    Layout *&layoutObj = Globals::layout;
    if (layoutObj->choiceWindowOpen != 0) {
        layoutObj->OnTouchMove(x, y);
        return;
    }
    if (this->modalFlags.bytes[1] != 0) {
        ((DialogueWindow *) (this->dialogueWindow))->OnTouchMove(x, y);
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[3] != 0) {
        ((ChoiceWindow *) (this->choiceWindow))->OnTouchMove(x, y);
        return;
    }
    if (this->modalFlags.bytes[2] != 0) {
        ((ChoiceWindow *) (this->medalChoiceWindow))->OnTouchMove(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[2] != 0) {
        ((HangarWindow *) (this->hangarWindow))->OnTouchMove(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[3] != 0) {
        ((StarMap *) ((void *&) this->starMap))->OnTouchMove(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[1] != 0) {
        ((SpaceLounge *) (this->spaceLounge))->OnTouchMove(x, y);
        return;
    }
    if (this->modalFlags.bytes[0] != 0) {
        ((StatusWindow *) (this->statusWindow))->OnTouchMove(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[0] != 0) {
        this->missionsWindow->OnTouchMove(x, y);
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[2] != 0) {
        ((MenuTouchWindow *) (this->dlcMenu))->OnTouchMove(x, y, touch);
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[1] == 0)
        return;
    ((TouchButton *) (this->buttonLaunch))->OnTouchMove(x, y);
    ((TouchButton *) (this->buttonCredits))->OnTouchMove(x, y);
    auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
    for (int i = 0; i != 5; i++)
        (*row)[i]->OnTouchMove(x, y);
    layoutObj->OnTouchMove(x, y);
    if (((NewsTicker *) (this->newsTicker))->OnTouchMove(x, y) != 0)
        return;
    Layout *layout = layoutObj;
    if (y <= layout->field_0xc)
        return;
    if (Globals::h - layout->field_0x10 <= y)
        return;
    if (x <= static_cast<int>(this->hangarLeftX))
        return;
    int delta = x - this->camScrollPos;
    this->camScrollVel = delta;
    this->field_0xf4 = 0x3f800000;
    this->camAngle = this->camAngle + delta;
    this->camScrollPos = x;
}


void ModStation::OnRelease() {
    FModSound *&sound = Globals::sound;
    if (sound != nullptr) {
        sound->disableReverb();
        sound->stopAllSoundFXEvents();
    }
    PaintCanvas::gCanvas->FogEnable(0, AbyssEngine::FogMode_dummy);

    if (this->buttonRow != 0) {
        auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
        ArrayReleaseClasses<TouchButton *>(*row);
        delete row;
    }
    this->buttonRow = 0;

    delete static_cast<HangarWindow *>(this->hangarWindow);
    this->hangarWindow = 0;

    delete this->starMap;
    (void *&) this->starMap = 0;

    delete (SpaceLounge *) this->spaceLounge;
    this->spaceLounge = 0;

    delete this->missionsWindow;
    this->missionsWindow = 0;

    delete this->dialogueWindow;
    this->dialogueWindow = 0;

    delete static_cast<ChoiceWindow *>(this->medalChoiceWindow);
    this->medalChoiceWindow = 0;

    delete static_cast<StatusWindow *>(this->statusWindow);
    this->statusWindow = 0;

    delete this->cutScene;
    this->cutScene = 0;

    delete (MenuTouchWindow *) this->dlcMenu;
    this->dlcMenu = 0;

    delete reinterpret_cast<Radio *>(this->activeMission);
    this->activeMission = 0;

    delete static_cast<Array<RadioMessage *> *>(this->radioMessages);
    this->radioMessages = 0;

    delete this->newsTicker;
    this->newsTicker = 0;

    delete static_cast<ChoiceWindow *>(this->choiceWindow);
    this->choiceWindow = 0;

    PaintCanvas::gCanvas->ReleaseAllResources();
    Globals::gGlobals->loadFont(GameText::getLanguage());

    Layout *&layout = Globals::layout;
    if (layout != nullptr) {
        layout->reload();
        static_cast<ImageFactory *>(Globals::imageFactory)->reload();
        layout->initTip();
    }

    delete this->cameraTween;
    this->cameraTween = 0;

    delete (AbyssEngine::EaseInOut *) this->easeX;
    this->easeX = 0;

    delete (AbyssEngine::EaseInOut *) this->easeY;
    this->easeY = 0;

    delete (AbyssEngine::EaseInOut *) this->easeZ;
    this->easeZ = 0;

    delete reinterpret_cast<Radio *>(this->activeMission);
    this->activeMission = 0;

    delete static_cast<ScrollTouchBox *>(this->scrollBox);
    this->scrollBox = 0;

    this->cameraFlags.halfword = 0;
    if (sound != nullptr)
        sound->freeAllEvents();
}


static int *g_ote_stack = 0;

static int **g_ote_helpLayout = 0;

static int **g_ote_status = 0;

static int **g_ote_achievements = 0;

static int **g_ote_module = 0;

static int **g_ote_canvas = 0;

static int **g_ote_sound = 0;

static int **g_ote_textRoot = 0;

static int **g_ote_galaxy = 0;

static int **g_ote_itemTable = 0;

static int **g_ote_shipTable = 0;




void ApplicationManager_SetCurrentApplicationModule_ote(int module);


int Status_getStation_ote();

int Station_getIndex_ote(Station * s);

int Station_stationHasPirateBase_ote();

int Station_hasShip_ote(void *station);

int Station_getAgents_ote(Station * s);
void Station_addShip_ote(Ship * s);
void Station_departStation_ote(Station * s);

void Station_setAttackedFriends_ote(Station *s, int flag);

void Station_setItems_ote(Station *s, void *arr, int flag);

int Status_getShip_ote();

int Status_getCredits_ote();

void Status_changeCredits_ote(int status);

int Status_getCurrentCampaignMission_ote();

void Status_nextCampaignMission_ote(int status);

void Status_departStation_ote(Station * s);
void Status_setStation_ote(Station * s);

void Status_setMission_ote(int status);

void Status_removeMission_ote(int status);

void Status_incMissionCount_ote(int status);

void Status_setPassengers_ote(int status, int n);


void Status_activateNewWanted_ote();

int Ship_getCargo_ote();

int Ship_getIndex_ote();

void Ship_getRace_ote(Ship * s);

void Ship_removeCargo_ote(int ship, int item);

void Ship_removeCargo1_ote(Item * it);
void Ship_addCargo_ote(Item * it);

void Ship_makeShip_ote(int desc);

int Item_isUnsaleable_ote(Item * it);
int Item_getIndex_ote(Item * it);

int Item_getAmount_ote();

void Item_makeItem_ote(int desc);

int Mission_getType_ote();

int Mission_isCampaignMission_ote(Mission * m);
int Mission_getProductionGoodIndex_ote(Mission * m);

int Mission_getProductionGoodAmount_ote();

int Mission_getReward_ote();

int Mission_getBonus_ote();

int Agent_getOffer_ote(Agent * a);
int Agent_getSellItemIndex_ote(Agent * a);

void Agent_setEvent_ote(Agent *a, int e);

void Agent_setOfferAccepted_ote(Agent *a, int flag);

void Achievements_resetNewMedals_ote(void *ach);

int GameText_getText_ote(int id);

void Galaxy_getStation_ote(int index);

void Layout_OnTouchEnd_ote(Layout *l, int p1, int p2);

void Layout_showMissionRewardMessage_ote(int l, int flag);

void ChoiceWindow_set_ote(int cw, int textStr, int flag);

void ChoiceWindow_setNotice_ote(int cw, int textStr);

void HangarWindow_ctor_ote(HangarWindow *w);

void HangarWindow_dtor_ote(HangarWindow *w);

void HangarWindow_initialize_ote();

void HangarWindow_showCreditsBuyWindow_ote(HangarWindow *w);

void HangarWindow_setSellMode_ote(int w);

int SpaceLounge_hangarNeedsUpdate_ote();

void SpaceLounge_refresh_ote();

void FModSound_play_ote(int sound, int id, void *p, float vol);

void FModSound_stop_ote(int sound);

void FModSound_setParamValue_ote(int sound, int a, int b, float v);

void RecordHandler_saveOptions_ote(RecordHandler * rh);

void CutScene_replacePlayerShip_ote(int cs, int shipIndex);

void CutScene_checkForTurret_ote(int cs);

void ChoiceWindow_setFee_frag(void *cw, int credits, int templateId);

void *ModStation_ote_buttonRow(ModStation * self);
int TouchButton_getX_ote(void *btn);
int TouchButton_getY_ote(void *btn);

void Radio_ctor_ote(void *radio);
void Radio_addMessage_ote(void *radio, int textId);
void ScrollTouchBox_initRadio_ote(void *box, void *radio);

void ModStation::OnTouchEnd(int x, int y, void *touch) {
    if (this->activeTouch != touch)
        return;
    int starMapOpen = this->m_nStarMapWindowOpen.bytes[0];
    this->scrollFlags.bytes[0] = 0;
    this->activeTouch = 0;
    if (starMapOpen != 0)
        return;

    Layout *&layout = Globals::layout;
    if (layout->choiceWindowOpen != 0) {
        if (layout->OnTouchEnd(x, y) != 0)
            layout->choiceWindowOpen = 0;
        return;
    }

    int statusHandle = static_cast<int>(reinterpret_cast<intptr_t>(Globals::status));
    int *status = &statusHandle;
    if (this->modalFlags.bytes[1] != 0) {
        if (this->dialogueWindow->OnTouchEnd(x, y) != 0) {
            if (this->dialogueMission != 0) {
                {
                        Mission *mission = this->dialogueMission;

                        if (mission != 0) {
                            int type = mission->getType();
                            int campaign = mission->isCampaignMission();
                            if (type == 8) {
                                if (campaign == 0) {
                                    Ship *ship = Globals::status->getShip();
                                    int good = mission->getProductionGoodIndex();
                                    int amount = mission->getProductionGoodAmount();
                                    ship->removeCargo(good, amount);
                                    if (this->hangarWindow != nullptr)
                                        static_cast<HangarWindow *>(this->hangarWindow)->initialize();
                                }
                            } else if (campaign == 0 && mission->getType() == 0xb) {
                                Status *statusObj = Globals::status;
                                statusObj->setPassengers(0);
                                if (!mission->isCampaignMission())
                                    statusObj->field_b8 += mission->getProductionGoodAmount();
                                Array<Item *> *cargo = statusObj->getShip()->getCargo();
                                if (cargo != nullptr) {
                                    for (unsigned i = 0; i < cargo->size(); i = i + 1) {
                                        if ((*cargo)[i]->isUnsaleable() &&
                                            ((*cargo)[i]->getIndex() == 0x74 ||
                                             (*cargo)[i]->getIndex() == 0x75)) {
                                            statusObj->getShip()->removeCargo((*cargo)[i]);
                                            if (this->hangarWindow != nullptr)
                                                static_cast<HangarWindow *>(this->hangarWindow)->initialize();
                                            break;
                                        }
                                    }
                                }
                            } else if (mission->getType() != 3 && mission->getType() != 5 &&
                                       mission->getType() != 0xb) {
                                if (mission->getType() == 0) {
                                    Array<Item *> *cargo = Globals::status->getShip()->getCargo();
                                    if (cargo != nullptr) {
                                        for (unsigned i = 0; i < cargo->size(); i = i + 1) {
                                            if ((*cargo)[i]->isUnsaleable() &&
                                                ((*cargo)[i]->getIndex() == 0x74 ||
                                                 (*cargo)[i]->getIndex() == 0x75)) {
                                                Globals::status->getShip()->removeCargo((*cargo)[i]);
                                                if (this->hangarWindow != nullptr)
                                                    static_cast<HangarWindow *>(this->hangarWindow)->initialize();
                                                break;
                                            }
                                        }
                                    }
                                    Globals::status->field_9c += mission->getProductionGoodAmount();
                                } else if (mission->getType() == 0xe) {
                                    Array<Item *> *cargo = Globals::status->getShip()->getCargo();
                                    if (cargo != nullptr) {
                                        for (unsigned i = 0; i < cargo->size(); i = i + 1) {
                                            if ((*cargo)[i]->getIndex() == 0x73) {
                                                Globals::status->getShip()->removeCargo((*cargo)[i]);
                                                if (this->hangarWindow != nullptr)
                                                    static_cast<HangarWindow *>(this->hangarWindow)->initialize();
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        if (!mission->isCampaignMission()) {
                            Status *statusObj = Globals::status;
                            statusObj->incMissionCount();
                            unsigned shown = static_cast<unsigned>(mission->getReward()) +
                                             static_cast<unsigned>(mission->getBonus());
                            Globals::layout->showMissionRewardMessage(static_cast<int>(shown), false);

                            unsigned credited = static_cast<unsigned>(mission->getReward()) +
                                                static_cast<unsigned>(mission->getBonus());
                            if (credited >= 1000001u) {
                                unsigned step = static_cast<unsigned>(
                                    statusObj->getCurrentCampaignMission() - 55);
                                credited = step < 7 ? kNonCampaignRewardCredits[step] : 6666;
                            }
                            statusObj->changeCredits(static_cast<int>(credited));
                            statusObj->removeMission(mission);
                            this->dialogueMission = nullptr;
                            if (this->spaceLounge != nullptr)
                                static_cast<SpaceLounge *>(this->spaceLounge)->refresh();
                            this->modalFlags.bytes[1] = 0;
                            this->checkHints();
                            return;
                        }

                        int camp = Globals::status->getCurrentCampaignMission();
                        if (camp == 0x2b) {
                            Globals::status->removeMission(this->dialogueMission);
                            Globals::status->setMission(static_cast<Mission *>(Mission::empty));
                            this->modalFlags.bytes[1] = 0;
                            Globals::sound->stop(Globals::sound->currentMusicEvent);
                            Globals::sound->play(0x90, nullptr, nullptr, 0.0f);

                            Radio *radio = new Radio();
                            this->activeMission = reinterpret_cast<intptr_t>(radio);
                            auto *messages = new Array<RadioMessage *>();
                            this->radioMessages = messages;
                            ArraySetLength(4, *messages);
                            (*messages)[0] = new RadioMessage(0x817, 1, 5, 4000);
                            (*messages)[1] = new RadioMessage(0x818, 1, 6, 0);
                            (*messages)[2] = new RadioMessage(0x819, 1, 6, 1);
                            (*messages)[3] = new RadioMessage(0x81a, 1, 6, 2);
                            radio->setMessages(messages);

                            Globals::Canvas->Image2DCreate(0x1b5a,
                                                           reinterpret_cast<unsigned int &>(this->field_0x5c));
                            int imageHeight = Globals::Canvas->GetImage2DHeight(this->field_0x5c);
                            AbyssEngine::AEMath::Vector anchor = {
                                50.0f, static_cast<float>(imageHeight + Globals::h + Globals::h / 2 + 10), 0.0f};
                            this->radioScrollAnchor = anchor;
                            auto *box = new ScrollTouchBox(
                                static_cast<int>(anchor.x), static_cast<int>(anchor.y),
                                static_cast<int>(static_cast<float>(Globals::w) - anchor.x * 2.0f),
                                5000);
                            this->scrollBox = box;
                            box->setText(*Globals::gameText->getText(0x30));
                            box->setTextCentered(true);
                            this->field_0xb4 = -12000;
                            this->field_0xb8 = 0;
                            this->m_nStarMapWindowOpen.bytes[0] = 1;
                            return;
                        }

                        if (Globals::status->getCurrentCampaignMission() != 0x94)
                            Globals::status->nextCampaignMission(true);

                        int cm = Globals::status->getCurrentCampaignMission();
                        unsigned d = (unsigned) (cm - 0x2c);
                        bool handled = false;
                        if (d < 0xf) {
                            if (((1u << d) & 0x5830) != 0) {
                                Ship *ship = Globals::status->getShip();
                                this->cutScene->replacePlayerShip(ship->getIndex(), ship->getRace());
                                unsigned credited = static_cast<unsigned>(this->dialogueMission->getReward()) +
                                                    static_cast<unsigned>(this->dialogueMission->getBonus());
                                if (credited >= 1000001u) {
                                    unsigned step = static_cast<unsigned>(
                                        Globals::status->getCurrentCampaignMission() - 55);
                                    credited = step < 7 ? kNonCampaignRewardCredits[step] : 6666;
                                }
                                Globals::status->changeCredits(static_cast<int>(credited));
                                Globals::status->removeMission(this->dialogueMission);
                                this->dialogueMission = nullptr;
                                if (this->spaceLounge != nullptr)
                                    static_cast<SpaceLounge *>(this->spaceLounge)->refresh();
                                this->modalFlags.bytes[1] = 0;
                                this->checkHints();
                                return;
                            }
                            if (d == 1) {
                                Globals::status->removeMission(this->dialogueMission);
                                this->dialogueMission = nullptr;
                                Globals::status->setMission(static_cast<Mission *>(Mission::empty));
                                this->selectedButton = 1;
                                this->modalFlags.bytes[1] = 0;

                                {
                                    int module = **(int **) g_ote_module;
                                    if (module != 0)
                                        ((void (*)(int, int)) module)(module, 0x10000);
                                }
                                return;
                            }
                            if (d != 0)
                                handled = false;
                            else
                                handled = true;
                        }

                        if (!handled) {
                            if ((unsigned) (cm - 0x4b) > 8 || (1 << ((cm - 0x4b) & 0xff) & 0x103) == 0) {
                                if (cm == 0x12) {
                                    Status_removeMission_ote(*status);
                                    this->modalFlags.bytes[1] = 0;
                                    this->dialogueMission = 0;
                                    return;
                                }

                                Station *here = (Station *) Status_getStation_ote();
                                int hereIdx = Station_getIndex_ote(here);

                                if (cm == 0x4d && hereIdx == 100) {
                                    Ship *sh = (Ship *) Status_getStation_ote();
                                    Ship_makeShip_ote((*(Array<int> *) (intptr_t) **g_ote_shipTable)[(0x94) / 4]);
                                    Station_addShip_ote(sh);
                                    {
                                        Mission_getReward_ote();
                                        int bonus = Mission_getBonus_ote();
                                        if ((unsigned) bonus >= 0)
                                            Status_getCurrentCampaignMission_ote();
                                        Status_changeCredits_ote(*status);
                                        Status_removeMission_ote(*status);
                                        this->dialogueMission = 0;
                                        if ((int) (intptr_t) this->spaceLounge != 0)
                                            SpaceLounge_refresh_ote();
                                        this->modalFlags.bytes[1] = 0;
                                        this->checkHints();
                                    }
                                    return;
                                }
                                if (cm == 0x4e) {
                                    ((Status *) (intptr_t) *status)->field_5c = -1;
                                    ((Status *) (intptr_t) *status)->field_60 = -1;
                                    ((Status *) (intptr_t) *status)->field_64 = -1;
                                    ((Status *) (intptr_t) *status)->field_68 = -1;
                                    Achievements_resetNewMedals_ote((void *) **(int **) g_ote_achievements);
                                    Station *home = (Station *) *status;
                                    Status_getStation_ote();
                                    Status_departStation_ote(home);
                                    **(int **) g_ote_module = 1;
                                    ApplicationManager_SetCurrentApplicationModule_ote(**(int **) g_ote_module);
                                    this->stationActive = 0;
                                    {
                                        Mission_getReward_ote();
                                        int bonus = Mission_getBonus_ote();
                                        if ((unsigned) bonus >= 0)
                                            Status_getCurrentCampaignMission_ote();
                                        Status_changeCredits_ote(*status);
                                        Status_removeMission_ote(*status);
                                        this->dialogueMission = 0;
                                        if ((int) (intptr_t) this->spaceLounge != 0)
                                            SpaceLounge_refresh_ote();
                                        this->modalFlags.bytes[1] = 0;
                                        this->checkHints();
                                    }
                                    return;
                                }
                                Station *st = (Station *) Status_getStation_ote();
                                if (cm == 0x54 && Station_getIndex_ote(st) == 100) {
                                    Ship *sh = (Ship *) Status_getStation_ote();
                                    if (Station_hasShip_ote(sh) == 0) {
                                        Ship_makeShip_ote((*(Array<int> *) (intptr_t) **g_ote_shipTable)[(0x98) / 4]);
                                        Station_addShip_ote(sh);
                                    }
                                    if (Station_hasShip_ote(sh) == 0) {
                                        Ship_makeShip_ote((*(Array<int> *) (intptr_t) **g_ote_shipTable)[(0xa0) / 4]);
                                        Station_addShip_ote(sh);
                                    }
                                    Item_makeItem_ote((*(Array<int> *) (intptr_t) **g_ote_itemTable)[(0x224) / 4]);
                                    Ship_addCargo_ote((Item *) Status_getShip_ote());
                                    {
                                        Mission_getReward_ote();
                                        int bonus = Mission_getBonus_ote();
                                        if ((unsigned) bonus >= 0)
                                            Status_getCurrentCampaignMission_ote();
                                        Status_changeCredits_ote(*status);
                                        Status_removeMission_ote(*status);
                                        this->dialogueMission = 0;
                                        if ((int) (intptr_t) this->spaceLounge != 0)
                                            SpaceLounge_refresh_ote();
                                        this->modalFlags.bytes[1] = 0;
                                        this->checkHints();
                                    }
                                    return;
                                }

                                if (cm == 0xa0 || cm == 0x90 || cm == 99 || cm == 0x77 || cm == 0x85 || cm == 0x6d ||
                                    cm == 0x59) {
                                    Station *home = (Station *) *status;
                                    **(char **) g_ote_module = 1;
                                    Galaxy_getStation_ote(**(int **) g_ote_galaxy);
                                    Status_departStation_ote(home);
                                    ApplicationManager_SetCurrentApplicationModule_ote(**(int **) g_ote_module);
                                    this->stationActive = 0;
                                    return;
                                }
                                if (cm == 0x68) {
                                    if (this->hangarWindow != 0) {
                                        HangarWindow_dtor_ote((HangarWindow *) this->hangarWindow);
                                        ::operator delete(this->hangarWindow);
                                    }
                                    this->subWindowFlags.bytes[2] = 0;
                                    this->hangarWindow = 0;
                                    {
                                        Mission_getReward_ote();
                                        int bonus = Mission_getBonus_ote();
                                        if ((unsigned) bonus >= 0)
                                            Status_getCurrentCampaignMission_ote();
                                        Status_changeCredits_ote(*status);
                                        Status_removeMission_ote(*status);
                                        this->dialogueMission = 0;
                                        if ((int) (intptr_t) this->spaceLounge != 0)
                                            SpaceLounge_refresh_ote();
                                        this->modalFlags.bytes[1] = 0;
                                        this->checkHints();
                                    }
                                    return;
                                }
                                if (cm == 0x80) {
                                    Status_activateNewWanted_ote();
                                    if (((Status *) (intptr_t) *(int *) g_ote_status)->byte_0x2a == 0)
                                        this->screenFlags.bytes[2] = 1;
                                    {
                                        Mission_getReward_ote();
                                        int bonus = Mission_getBonus_ote();
                                        if ((unsigned) bonus >= 0)
                                            Status_getCurrentCampaignMission_ote();
                                        Status_changeCredits_ote(*status);
                                        Status_removeMission_ote(*status);
                                        this->dialogueMission = 0;
                                        if ((int) (intptr_t) this->spaceLounge != 0)
                                            SpaceLounge_refresh_ote();
                                        this->modalFlags.bytes[1] = 0;
                                        this->checkHints();
                                    }
                                    return;
                                }

                                {
                                    Mission_getReward_ote();
                                    int bonus = Mission_getBonus_ote();
                                    if ((unsigned) bonus >= 0)
                                        Status_getCurrentCampaignMission_ote();
                                    Status_changeCredits_ote(*status);
                                    Status_removeMission_ote(*status);
                                    this->dialogueMission = 0;
                                    if ((int) (intptr_t) this->spaceLounge != 0)
                                        SpaceLounge_refresh_ote();
                                    this->modalFlags.bytes[1] = 0;
                                    this->checkHints();
                                }
                                return;
                            }
                        }

                        Status_removeMission_ote(*status);
                        this->dialogueMission = 0;
                        Status_setMission_ote(*status);
                        this->modalFlags.bytes[1] = 0;
                        ((Status *) (intptr_t) *status)->wanted = 0; /* cutscene slot (+0x0) */
                        {
                            int module = **(int **) g_ote_module;
                            if (module != 0)
                                ((void (*)(int, int)) module)(module, 5);
                        }
                }
                return;
            }

            DialogueWindow *dw = this->dialogueWindow;
            this->modalFlags.bytes[1] = 0;
            if (dw != 0) {
                delete dw;
            }
            char touchActive = this->dragFlags.bytes[1];
            this->dialogueWindow = 0;
            if (touchActive == 0) {
                Station *station = Globals::status->getStation();
                if (station->getIndex() != 4 &&
                    Globals::status->getStation()->getIndex() != 0x58 &&
                    Globals::status->getStation()->stationHasPirateBase() != 0) {
                    this->cameraFlags.bytes[2] = 0;
                    Globals::status->departStation(Globals::status->getStation());
                    static_cast<Achievements *>(Globals::achievements)->resetNewMedals();
                    Globals::switch_to_target_setting =
                        Globals::status->getCurrentCampaignMission() == 0x10 ? -1 : 1;
                    static_cast<ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(2);
                    this->stationActive = 0;
                }
            }
            return;
        }
        if (this->subWindowFlags.bytes[1] != 0)
            return;
    }

    if (this->m_nStarMapWindowOpen.bytes[3] != 0) {
        int r = static_cast<ChoiceWindow *>(this->choiceWindow)->OnTouchEnd(x, y);
        if (r == 1) {
            char departed = this->modalFlags.bytes[3];
            this->departPendingFlags.bytes[2] = 0;
            if (departed == 0) {
                this->m_nStarMapWindowOpen.bytes[3] = 0;
                this->scrollBoxFlags.bytes[0] = 0;
            } else {
                this->departPendingFlags.bytes[2] = 0;
                Station *home = (Station *) **(int **) g_ote_status;
                Status_getStation_ote();
                Status_departStation_ote(home);
                Achievements_resetNewMedals_ote((void *) **(int **) g_ote_achievements);
                **(int **) g_ote_module = 1;
                ApplicationManager_SetCurrentApplicationModule_ote(**(int **) g_ote_module);
                this->stationActive = 0;
            }
        } else if (r == 0) {
            {
                    (void) x;
                    (void) y;

                    if (this->departPendingFlags.bytes[2] != 0) {
                        this->departPendingFlags.bytes[2] = 0;
                        int camp = Status_getCurrentCampaignMission_ote();
                        if (camp == 0x18) {
                            Station *st = (Station *) Status_getStation_ote();
                            if (Station_getIndex_ote(st) == 10) {
                                unsigned i = 0;
                                for (;;) {
                                    Station *st1 = (Station *) Status_getStation_ote();
                                    Array<Agent *> *ag = (Array<Agent *> *) (intptr_t) Station_getAgents_ote(st1);
                                    if (ag->size() <= i) break;
                                    Station *st2 = (Station *) Status_getStation_ote();
                                    Array<Agent *> *agents = (Array<Agent *> *) (intptr_t) Station_getAgents_ote(st2);
                                    Agent *a = (*agents)[i];
                                    if (Agent_getOffer_ote(a) == 2 && Agent_getSellItemIndex_ote(a) == 0x44) {
                                        Agent_setEvent_ote(a, 1);
                                        Agent_setOfferAccepted_ote(a, 1);
                                    }
                                    i = i + 1;
                                }
                            }
                        }
                        Station *home = (Station *) *status;
                        if (Status_getCurrentCampaignMission_ote() == 0x30) {
                            Galaxy_getStation_ote(**(int **) g_ote_galaxy);
                            Status_departStation_ote(home);
                            **(char **) g_ote_galaxy = 1;
                        } else {
                            Status_getStation_ote();
                            Status_departStation_ote(home);
                        }
                        ((Status *) (intptr_t) *status)->field_5c = -1;
                        ((Status *) (intptr_t) *status)->field_60 = -1;
                        ((Status *) (intptr_t) *status)->field_64 = -1;
                        ((Status *) (intptr_t) *status)->field_68 = -1;
                        Achievements_resetNewMedals_ote((void *) **(int **) g_ote_achievements);
                        **(int **) g_ote_module = 1;
                        ApplicationManager_SetCurrentApplicationModule_ote(**(int **) g_ote_module);
                        this->stationActive = 0;
                        return;
                    }

                    if (this->modalFlags.bytes[3] != 0) {
                        int credits = Status_getCredits_ote();
                        if ((int) (intptr_t) this->buttonCredits <= credits) {
                            Status_changeCredits_ote(*status);
                            this->modalFlags.bytes[3] = 0;
                            this->buttonCreditsFlags.bytes[0] = 1;
                            Station *st = (Station *) Status_getStation_ote();
                            Station_setAttackedFriends_ote(st, 0);
                            this->choiceWindowFlags.bytes[1] = 1;
                            this->enterStation();
                            this->autosave();
                            {
                                if (this->scrollBoxFlags.bytes[0] != 0) {
                                    this->m_nStarMapWindowOpen.bytes[3] = 0;
                                    this->scrollBoxFlags.bytes[0] = 0;
                                    GameData *appData = static_cast<GameData *>(
                                        static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->GetApplicationData());
                                    appData->radioHiddenFlag = 1;
                                    return;
                                }
                                if (this->screenFlags.bytes[0] == 0) {
                                    if (this->screenFlags.bytes[1] != 0) {
                                        int feeCredits = Status_getCredits_ote();
                                        if (feeCredits < 25000) {
                                            ChoiceWindow_setFee_frag(this->choiceWindow, feeCredits, 1);
                                            this->screenFlags.bytes[1] = 0;
                                            return;
                                        }
                                        Status_changeCredits_ote(*status);
                                        this->m_nStarMapWindowOpen.bytes[3] = 0;
                                        this->screenFlags.bytes[1] = 0;
                                        Station *home = (Station *) *status;
                                        Galaxy_getStation_ote(**(int **) g_ote_galaxy);
                                        Status_setStation_ote(home);
                                        **(int **) g_ote_module = 0;
                                        ApplicationManager_SetCurrentApplicationModule_ote(**(int **) g_ote_module);
                                    }
                                    if (this->closeHangarAfterCredits != 0 && this->subWindowFlags.bytes[2] != 0) {
                                        this->closeHangarAfterCredits = 0;
                                        this->subWindowFlags.bytes[2] = 0;
                                    }
                                    this->m_nStarMapWindowOpen.bytes[3] = 0;
                                } else {
                                    Status_changeCredits_ote(*status);
                                    Ship_removeCargo_ote(Status_getShip_ote(), 0x6d);
                                    ((Status *) (intptr_t) *status)->field_114 = 3;
                                    RecordHandler_saveOptions_ote((RecordHandler *) **(int **) g_ote_status);
                                    ChoiceWindow_setNotice_ote((int) (intptr_t) this->choiceWindow, GameText_getText_ote(**g_ote_textRoot));
                                    Station *st2 = (Station *) Status_getStation_ote();
                                    Station_setItems_ote(st2, 0, 0);
                                    Station_setItems_ote(((Status *) (intptr_t) *status)->voidStation, 0, 0);
                                    this->screenFlags.bytes[0] = 0;
                                }
                            }
                            return;
                        }
                        ChoiceWindow_setFee_frag(this->choiceWindow, credits, 0);
                        this->modalFlags.bytes[3] = 1;
                        this->departPendingFlags.bytes[2] = 1;
                        this->m_nStarMapWindowOpen.bytes[3] = 1;
                        return;
                    }

                    {
                        if (this->scrollBoxFlags.bytes[0] != 0) {
                            this->m_nStarMapWindowOpen.bytes[3] = 0;
                            this->scrollBoxFlags.bytes[0] = 0;
                            GameData *appData = static_cast<GameData *>(
                                static_cast<AbyssEngine::ApplicationManager *>(Globals::appManager)->GetApplicationData());
                            appData->radioHiddenFlag = 1;
                            return;
                        }
                        if (this->screenFlags.bytes[0] == 0) {
                            if (this->screenFlags.bytes[1] != 0) {
                                int credits = Status_getCredits_ote();
                                if (credits < 25000) {
                                    ChoiceWindow_setFee_frag(this->choiceWindow, credits, 1);
                                    this->screenFlags.bytes[1] = 0;
                                    return;
                                }
                                Status_changeCredits_ote(*status);
                                this->m_nStarMapWindowOpen.bytes[3] = 0;
                                this->screenFlags.bytes[1] = 0;
                                Station *home = (Station *) *status;
                                Galaxy_getStation_ote(**(int **) g_ote_galaxy);
                                Status_setStation_ote(home);
                                **(int **) g_ote_module = 0;
                                ApplicationManager_SetCurrentApplicationModule_ote(**(int **) g_ote_module);
                            }
                            if (this->closeHangarAfterCredits != 0 && this->subWindowFlags.bytes[2] != 0) {
                                this->closeHangarAfterCredits = 0;
                                this->subWindowFlags.bytes[2] = 0;
                            }
                            this->m_nStarMapWindowOpen.bytes[3] = 0;
                        } else {
                            Status_changeCredits_ote(*status);
                            Ship_removeCargo_ote(Status_getShip_ote(), 0x6d);
                            ((Status *) (intptr_t) *status)->field_114 = 3;
                            RecordHandler_saveOptions_ote((RecordHandler *) **(int **) g_ote_status);
                            ChoiceWindow_setNotice_ote((int) (intptr_t) this->choiceWindow, GameText_getText_ote(**g_ote_textRoot));
                            Station *st = (Station *) Status_getStation_ote();
                            Station_setItems_ote(st, 0, 0);
                            Station_setItems_ote(((Status *) (intptr_t) *status)->voidStation, 0, 0);
                            this->screenFlags.bytes[0] = 0;
                        }
                    }
            }
        }
        return;
    }

    if (this->modalFlags.bytes[0] != 0) {
        if (static_cast<StatusWindow *>(this->statusWindow)->OnTouchEnd(x, y) != 0) {
            this->modalFlags.bytes[0] = 0;
            Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.0f);
        }
        return;
    }
    if (this->subWindowFlags.bytes[3] != 0) {
        if (this->starMap->OnTouchEnd(x, y) != 0) {
            this->subWindowFlags.bytes[3] = 0;
            this->resetLight();
            Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.0f);
        }
        return;
    }
    if (this->subWindowFlags.bytes[2] != 0) {
        if (static_cast<HangarWindow *>(this->hangarWindow)->OnTouchEnd(x, y) != 0) {
            Array<Item *> *cargo = Globals::status->getShip()->getCargo();
            int sold = 0;
            if (cargo != 0) {
                for (unsigned i = 0; i < cargo->size(); i = i + 1) {
                    int idx = (*cargo)[i]->getIndex();
                    if (idx >= 0x84 && (*cargo)[i]->getIndex() <= 0x99)
                        sold += (*cargo)[i]->getAmount();
                }
            }
            if (sold > this->field_0xd0)
                Globals::status->field_a8 += sold - this->field_0xd0;
            HangarWindow *hangar = static_cast<HangarWindow *>(this->hangarWindow);
            hangar->setSellMode(false);
            this->resetIdleCamForHangar();
            this->subWindowFlags.bytes[2] = 0;
            if (hangar->field_0x0 != 0) {
                hangar->field_0x0 = 0;
                int shipIndex = Globals::status->getShip()->getIndex();
                int shipRace = Globals::status->getShip()->getRace();
                this->cutScene->replacePlayerShip(shipIndex, shipRace);
            }
            if (this->cutScene != nullptr)
                this->cutScene->checkForTurret();
            Globals::sound->stop(0x5f);
            Globals::sound->play(0x7a, nullptr, nullptr, 0.0f);
            Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.0f);
            auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
            for (unsigned i = 0; i < row->size(); ++i) {
                if (i < 10) {
                    Globals::sub_menu_buttons_x[i] = static_cast<int>((*row)[i]->getPosition().x);
                    Globals::sub_menu_buttons_y[i] = static_cast<int>((*row)[i]->getPosition().y);
                }
            }
        }
        return;
    }
    if (this->subWindowFlags.bytes[1] != 0) {
        if (static_cast<SpaceLounge *>(this->spaceLounge)->OnTouchEnd(x, y) != 0) {
            this->subWindowFlags.bytes[1] = 0;
            this->resetIdleCamForHangar();
            this->resetLight();
            Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.0f);
            Globals::sound->stop(0x6c);
            Globals::sound->play(0x7a, nullptr, nullptr, 0.0f);
            this->cutScene->checkForTurret();
            SpaceLounge *lounge = static_cast<SpaceLounge *>(this->spaceLounge);
            if (lounge != nullptr && lounge->hangarNeedsUpdate() != 0) {
                delete static_cast<HangarWindow *>(this->hangarWindow);
                this->hangarWindow = nullptr;
            }
        }
        return;
    }
    if (this->subWindowFlags.bytes[0] != 0) {
        if (this->missionsWindow->OnTouchEnd(x, y) != 0) {
            this->subWindowFlags.bytes[0] = 0;
            Globals::sound->setParamValue(0, Globals::sound->currentMusicEvent, 0.0f);
            this->resetLight();
        }
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[2] != 0) {
        if (static_cast<MenuTouchWindow *>(this->dlcMenu)->OnTouchEnd(x, y, touch) != 0) {
            this->m_nStarMapWindowOpen.bytes[2] = 0;
            auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
            for (unsigned i = 0; i < row->size(); ++i) {
                if (i < 10) {
                    Globals::sub_menu_buttons_x[i] = static_cast<int>((*row)[i]->getPosition().x);
                    Globals::sub_menu_buttons_y[i] = static_cast<int>((*row)[i]->getPosition().y);
                }
            }
        }
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[1] != 0) {
        {
                Layout *help = Globals::layout;

                if (static_cast<TouchButton *>(this->buttonLaunch)->OnTouchEnd(x, y) != 0) {
                    this->leaveStation();
                    return;
                }

                if (static_cast<TouchButton *>(this->buttonCredits)->OnTouchEnd(x, y) != 0) {
                    Globals::options[0x4e] = 1;
                    static_cast<RecordHandler *>(Globals::recordHandler)->saveOptions();
                    HangarWindow *hangar = static_cast<HangarWindow *>(this->hangarWindow);
                    if (hangar == nullptr) {
                        hangar = new HangarWindow();
                        this->hangarWindow = hangar;
                    }
                    hangar->initialize();
                    this->closeHangarAfterCredits = 1;
                    this->subWindowFlags.bytes[2] = 1;
                    hangar->showCreditsBuyWindow();
                }

                this->selectedButton = -1;
                auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
                for (unsigned i = 0; i < 5; i = i + 1) {
                    if ((*row)[i]->OnTouchEnd(x, y) != 0) {
                        this->selectedButton = (int) i;
                        this->OnKeyPress(0x10000LL, 0);
                        return;
                    }
                }

                if (help->OnTouchEnd(x, y) != 0) {
                    MenuTouchWindow *win = static_cast<MenuTouchWindow *>(this->dlcMenu);
                    if (win == nullptr) {
                        win = new MenuTouchWindow(2);
                        this->dlcMenu = win;
                    }
                    Globals::status->checkForLevelUp();
                    this->m_nStarMapWindowOpen.bytes[2] = 1;
                    for (unsigned i = 0; i < win->buttons->size(); ++i) {
                        if (i < 10) {
                            Globals::sub_menu_buttons_x[i] = static_cast<int>(
                                static_cast<TouchButton *>((*win->buttons)[i])->getPosition().x);
                            win = static_cast<MenuTouchWindow *>(this->dlcMenu);
                            Globals::sub_menu_buttons_y[i] = static_cast<int>(
                                static_cast<TouchButton *>((*win->buttons)[i])->getPosition().y);
                        }
                    }
                    return;
                }

                if (help->helpPressed() != 0)
                    help->initHelpWindow(*Globals::gameText->getText(0x284));

                if (this->newsTicker->OnTouchEnd(x, y) == 0) {
                    const int lastMove = this->camScrollVel;
                    const int magnitude = lastMove < 0 ? -lastMove : lastMove;
                    const int angle = this->camAngle + lastMove;
                    this->camDragScale = 0.9f;
                    this->dragFlags.bytes[0] = 0;
                    this->camAngle = angle;
                    this->field_0xec = angle;
                    this->camDragVelocity = magnitude > 3 ?
                        static_cast<float>(lastMove) : 0.0f;
                }
        }
    }
}





void ModStation::OnTouchBegin(int x, int y, void *touch) {
    if (this->activeTouch != 0)
        return;
    this->scrollFlags.bytes[0] = 1;
    this->activeTouch = touch;
    const int flag = this->m_nStarMapWindowOpen.bytes[0];
    this->touchX = x;
    this->touchY = y;
    if (flag != 0) {
        if (((Radio *) (intptr_t) this->activeMission)->lastMessageShown() != 0) {
            (unsigned char &) this->stationActive = 0;
            Globals::status->nextCampaignMission(true);
            Globals::switch_to_target_setting = 0;
            static_cast<ApplicationManager *>(Globals::appManager)->SetCurrentApplicationModule(5);
            this->m_nStarMapWindowOpen.bytes[0] = 0;
        }
        return;
    }
    Layout *&layoutObj = Globals::layout;
    if (layoutObj->choiceWindowOpen != 0) {
        layoutObj->OnTouchBegin(x, y);
        return;
    }
    if (this->modalFlags.bytes[1] != 0) {
        ((DialogueWindow *) (this->dialogueWindow))->OnTouchBegin(x, y);
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[3] != 0) {
        ((ChoiceWindow *) (this->choiceWindow))->OnTouchBegin(x, y);
        return;
    }
    if (this->modalFlags.bytes[2] != 0) {
        ((ChoiceWindow *) (this->medalChoiceWindow))->OnTouchBegin(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[2] != 0) {
        ((HangarWindow *) (this->hangarWindow))->OnTouchBegin(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[1] != 0) {
        ((SpaceLounge *) (this->spaceLounge))->OnTouchBegin(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[3] != 0) {
        ((StarMap *) ((void *&) this->starMap))->OnTouchBegin(x, y);
        return;
    }
    if (this->modalFlags.bytes[0] != 0) {
        ((StatusWindow *) (this->statusWindow))->OnTouchBegin(x, y);
        return;
    }
    if (this->subWindowFlags.bytes[0] != 0) {
        this->missionsWindow->OnTouchBegin(x, y);
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[2] != 0) {
        ((MenuTouchWindow *) (this->dlcMenu))->OnTouchBegin(x, y, touch);
        return;
    }
    if (this->m_nStarMapWindowOpen.bytes[1] == 0)
        return;
    ((TouchButton *) (this->buttonLaunch))->OnTouchBegin(x, y);
    ((TouchButton *) (this->buttonCredits))->OnTouchBegin(x, y);
    auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
    for (int i = 0; i != 5; i++)
        (*row)[i]->OnTouchBegin(x, y);
    layoutObj->OnTouchBegin(x, y);
    if (((NewsTicker *) (this->newsTicker))->OnTouchBegin(x, y) != 0)
        return;
    Layout *layout = layoutObj;
    if (y <= layout->field_0xc)
        return;
    if (Globals::h - layout->field_0x10 <= y)
        return;
    if (x <= static_cast<int>(this->hangarLeftX))
        return;
    this->field_0xfc = x;
    this->camScrollPos = x;
    this->camScrollVel = 0;
    this->dragFlags.bytes[0] = 1;
}

int ModStation::OnTouchBegin(int x, int y) { return x; }
int ModStation::OnTouchEnd(int x, int y) { return x; }
int ModStation::OnTouchMove(int x, int y) { return x; }


void ModStation::OnRender2D() {
    PaintCanvas *canvas = this->canvas;
    canvas->Begin2d();
    canvas->SetColor(0xffffffffu);

    if (this->stationActive == 0) {
        return;
    }

    if (this->subWindowFlags.bytes[2] != 0) {
        static_cast<HangarWindow *>(this->hangarWindow)->render();
    } else if (this->subWindowFlags.bytes[1] != 0) {
        static_cast<SpaceLounge *>(this->spaceLounge)->draw();
    } else if (this->subWindowFlags.bytes[0] != 0) {
        this->missionsWindow->draw();
    } else if (this->subWindowFlags.bytes[3] != 0) {
        this->starMap->draw();
    } else if (this->modalFlags.bytes[0] != 0) {
        static_cast<StatusWindow *>(this->statusWindow)->draw();
    } else if (this->m_nStarMapWindowOpen.bytes[2] != 0) {
        static_cast<MenuTouchWindow *>(this->dlcMenu)->draw();
    } else if (this->m_nStarMapWindowOpen.bytes[0] != 0) {
        auto *radio = reinterpret_cast<Radio *>(this->activeMission);
        radio->draw(static_cast<int64_t>(this->field_0xb4), nullptr, nullptr);
        if (radio->lastMessageShown() != 0) {
            canvas->DrawImage2D(this->field_0x5c, 0,
                                static_cast<int>(this->radioScrollAnchor.y), 20, 68);
            static_cast<ScrollTouchBox *>(this->scrollBox)->draw();
        }

        int timer = this->field_0xb4;
        bool fill = true;
        float alpha;
        if (timer >= 130001) {
            alpha = static_cast<float>(timer - 130000) / 6000.0f;
        } else if (timer < -6000) {
            alpha = static_cast<float>(timer + 12000) / 6000.0f;
        } else if (timer <= -1) {
            alpha = static_cast<float>(timer + 6000) / -6000.0f + 1.0f;
        } else {
            fill = false;
        }
        if (fill) {
            canvas->SetColor(static_cast<unsigned int>(static_cast<int>(alpha * 255.0f)));
            canvas->FillRectangle(0, 0, Globals::w, Globals::h);
        }
        if (timer > -6000)
            this->cutScene->render2D();
    } else {
        Layout *layout = Globals::layout;
        Status *status = Globals::status;
        layout->drawFooterStation();

        String stationName = status->getStation()->getName();
        if (status->getStation()->getIndex() != 101)
            stationName = stationName + (String(" ") + *Globals::gameText->getText(136));
        layout->drawHeader(stationName);

        if (this->m_nStarMapWindowOpen.bytes[1] != 0) {
            layout->drawBox(2, static_cast<int>(this->stationBoxX),
                            this->stationBoxY, static_cast<int>(this->hangarLeftX),
                            this->stationBoxHeight, String(""));

            SolarSystem *system = status->getSystem();
            int badge = this->departPending;
            int textX = canvas->GetImage2DWidth(badge) + layout->field_0x28 + layout->field_0x4c;
            int rowY = layout->field_0xc + layout->field_0x20 + 2 * layout->field_0x2c;
            if (system->hasNoOwner() == 0)
                canvas->DrawImage2D(badge, layout->field_0x28, rowY);

            String systemName = system->getName();
            String systemLine = systemName + String(" ") + *Globals::gameText->getText(137);
            int availableWidth = static_cast<int>(this->hangarLeftX) -
                                 2 * layout->field_0x28 - layout->field_0x4c -
                                 canvas->GetImage2DWidth(badge);
            if (canvas->GetTextWidth(Globals::font, systemLine) > availableWidth)
                systemLine = system->getName();
            canvas->DrawString(Globals::font, systemLine, textX, rowY, false);

            String techLevel = *Globals::gameText->getText(133) + String(": ") +
                               String(status->getStation()->getTecLevel());
            int techY = layout->field_0xc + layout->field_0x20 +
                        layout->field_0x4 + 4 * layout->field_0x2c;
            canvas->DrawString(Globals::font, techLevel, textX, techY, false);

            if (layout->field_0x286 != 0 && system->hasNoOwner() == 0) {
                int factionY = layout->field_0xc + layout->field_0x20 +
                               2 * layout->field_0x4 + 6 * layout->field_0x2c;
                canvas->DrawString(Globals::font,
                                   *Globals::gameText->getText(system->getRace() + 406),
                                   textX, factionY, false);
            }

            auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
            for (unsigned i = 0; i < 5; ++i)
                (*row)[i]->draw();
            static_cast<TouchButton *>(this->buttonLaunch)->draw();
            static_cast<TouchButton *>(this->buttonCredits)->draw();

            int stationIndex = status->getStation()->getIndex();
            if (stationIndex != 101 && stationIndex != 108 &&
                status->getSystem()->getIndex() != 25 && this->newsTicker != nullptr)
                this->newsTicker->draw();
            layout->drawMissionRewardMessage(true);
        }

        if (this->accumTime > 2999) {
            this->cameraFlags.bytes[0] = 0;
        } else {
            int textId = this->cameraFlags.bytes[0] ? 54 : 50;
            canvas->DrawString(Globals::font, *Globals::gameText->getText(textId),
                               layout->field_0x28 + static_cast<int>(this->hangarLeftX),
                               layout->field_0x18, false);
        }
    }

    if (this->modalFlags.bytes[2] != 0)
        static_cast<ChoiceWindow *>(this->medalChoiceWindow)->draw();
    else if (this->m_nStarMapWindowOpen.bytes[3] != 0)
        static_cast<ChoiceWindow *>(this->choiceWindow)->draw();
    if (this->modalFlags.bytes[1] != 0) {
        if (this->dialogueWindow != 0)
            this->dialogueWindow->draw();
    }

    Layout *help = Globals::layout;
    if (help->choiceWindowOpen != 0)
        help->drawHelpWindow();

    canvas->End2d();

    if (help->choiceWindowOpen == 0) {
        if (this->subWindowFlags.bytes[2] != 0) {
            static_cast<HangarWindow *>(this->hangarWindow)->render3D();
        }
        if (help->choiceWindowOpen == 0 && this->subWindowFlags.bytes[1] != 0)
            static_cast<SpaceLounge *>(this->spaceLounge)->draw3DShip();
    }

    canvas->SwapBuffer();
}


// Station-entry state is held by Globals::hints and Globals::options.








int Status_getShip_oi();

int Status_getLevel_oi();

int Status_hardCoreMode_oi();









void Ship_addCargo_oi(int ship, Item *it);




int Standing_getStanding_oi(int standing);





void TouchButton_setHalfTransparent_oi(int flag);

void Globals_playMusicAndFadeOutCurrent_oi(int id);

void FModSound_play_oi(int sound, int id, void *pos, float vol);

void FModSound_enableReverb_oi(int sound);

void FModSound_setDownPitch_oi(int sound);


void ModStation::OnInitialize() {
    this->dialogueMission = 0;

    if (this->cutScene == nullptr) {
        const size_t bytes = sizeof(CutScene) > 0xa0 ? sizeof(CutScene) : 0xa0;
        CutScene *cs = new (::operator new(bytes)) CutScene(23);
        this->cutScene = cs;
        cs->initialize();
    }

    int state = this->state;
    int next;

    if (state == 0x14) {
        if (this->cameraFlags.bytes[1] == 0 && this->cameraFlags.bytes[0] == 0 &&
            this->dragFlags.bytes[1] != 0) {
            bool skip = false;
            if (Globals::status->getCurrentCampaignMission() == 0x4d) {
                Station *st = Globals::status->getStation();
                if (st->getIndex() == 0x65)
                    skip = true;
            }
            if (!skip)
                this->autosave();
        }

        if (Globals::status->getCurrentCampaignMission() == 1) {
            Array<Ship *> *shipTable = static_cast<Array<Ship *> *>(Globals::ships);
            Globals::status->setShip((*shipTable)[0]->makeShip(-1));
            Ship *ship = Globals::status->getShip();
            ship->setRace(8);
            Array<Item *> *itemTable = static_cast<Array<Item *> *>(Globals::items);
            Item *firstItem = (*itemTable)[90]->makeItem();
            firstItem->setUnsaleable(true);
            ship->setEquipment(firstItem, 0);
            Item *secondItem = (*itemTable)[81]->makeItem();
            ship->setEquipment(secondItem, 1);
            secondItem->setUnsaleable(true);
            this->cutScene->replacePlayerShip(ship->getIndex(), 3);
        }

        auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
        int camp = Globals::status->getCurrentCampaignMission();
        bool special = false;
        if (camp < 5) {
            (*row)[0]->setHalfTransparent(true);
            (*row)[2]->setHalfTransparent(true);
            (*row)[3]->setHalfTransparent(true);
        } else if (camp < 9 || camp == 0xf) {
            special = camp == 0xf;
            (*row)[2]->setHalfTransparent(true);
            (*row)[3]->setHalfTransparent(true);
        }
        bool showShop = camp < 0xc || special;
        if (!showShop) {
            Station *st = Globals::status->getStation();
            if (st->getIndex() == 100) showShop = true;
            else {
                st = Globals::status->getStation();
                if (st->getIndex() == 0x65) showShop = true;
            }
        }
        if (showShop)
            (*row)[1]->setHalfTransparent(true);

        AbyssEngine::PaintCanvas *canvas = Globals::Canvas;
        unsigned int camera = canvas->CameraGetCurrent();
        const Matrix *current = static_cast<const Matrix *>(canvas->CameraGetLocal(camera));
        AbyssEngine::AEMath::Vector lightDirection =
            AbyssEngine::AEMath::MatrixGetDir(*current);
        this->scrollTargetf = lightDirection.x - 0.2f;
        this->field_0x120 = lightDirection.y - 0.3f;
        this->field_0x124 = lightDirection.z;
        this->resetLight();
        if (this->dragFlags.bytes[1] != 0)
            this->enterStation();

        if (Globals::status->getCurrentCampaignMission() == 0x4d) {
            Station *st2 = Globals::status->getStation();
            if (st2->getIndex() == 100) {
                Array<Ship *> *ships = Globals::status->getStation()->getShips();
                if (ships != 0) {
                    for (unsigned i = 0; i < ships->size(); i = i + 1) {
                        Ship *ship = (*ships)[i];
                        if (ship->getIndex() == 0x25)
                            ship->setPrice(0);
                    }
                }
            }
        }

        int race;
        Station *st = Globals::status->getStation();
        if (st->getIndex() == 0x65) {
            race = 8;
        } else {
            st = Globals::status->getStation();
            race = st->getIndex() == 100 ? 7 : Globals::status->getSystem()->getRace();
        }
        // Android OnInitialize uses 0x1fdfb8 for both layouts; yaw still varies by iPad.
        const int *coord = &kStationCameraCoordinatesPhone[0][0];
        const float *yawTable = modStationCameraYaw();
        float kx = static_cast<float>(coord[race * 3]);
        float ky = static_cast<float>(coord[race * 3 + 1]);
        float kz = static_cast<float>(coord[race * 3 + 2]);
        float pitch = kStationCameraPitch[race];
        float yaw = yawTable[race];
        this->camKeyX = kx;
        this->camKeyY = ky;
        this->camKeyZ = kz;
        if (this->easeX != nullptr)
            static_cast<AbyssEngine::EaseInOut *>(this->easeX)->SetRange(kx, kx);
        else
            this->easeX = new AbyssEngine::EaseInOut(kx, kx);
        if (this->easeY != nullptr)
            static_cast<AbyssEngine::EaseInOut *>(this->easeY)->SetRange(ky, ky);
        else
            this->easeY = new AbyssEngine::EaseInOut(ky, ky);
        if (this->easeZ != nullptr)
            static_cast<AbyssEngine::EaseInOut *>(this->easeZ)->SetRange(kz, kz);
        else
            this->easeZ = new AbyssEngine::EaseInOut(kz, kz);

        if (this->cutScene != nullptr) {
            Matrix key;
            AbyssEngine::AEMath::MatrixSetTranslation(key, kx, ky, kz);
            AbyssEngine::AEMath::MatrixSetRotation(
                key, pitch, yaw, -0.03f, AbyssEngine::AEMath::ROTATION_ORDER_YXZ);
            canvas->CameraSetLocal(canvas->CameraGetCurrent(), key);
            this->cutScene->vec8 = AbyssEngine::AEMath::Vector{kx, ky, kz};
            this->cutScene->stationCameraAngles =
                AbyssEngine::AEMath::Vector{pitch, yaw, -0.03f};
            this->cutScene->rootGeom->setMatrix(key);

            AbyssEngine::AERandom *random =
                static_cast<AbyssEngine::AERandom *>(Globals::rnd);
            this->field_0x12c.bytes[1] = random->nextInt(20) < 10;
            this->field_0x12c.bytes[2] = random->nextInt(20) > 10;
            this->field_0x12c.bytes[3] = random->nextInt(20) < 10;
            int dx = random->nextInt(150);
            if (this->field_0x12c.bytes[2] != 0) dx = -dx;
            this->cutScene->vec8.x += static_cast<float>(dx);
            int dy = random->nextInt(150);
            if (this->field_0x12c.bytes[1] != 0) dy = -dy;
            this->cutScene->vec8.y += static_cast<float>(dy);
            int dz = random->nextInt(150);
            if (this->field_0x12c.bytes[3] != 0) dz = -dz;
            this->cutScene->vec8.z += static_cast<float>(dz);
        }

        if (this->modalFlags.bytes[1] == 0 && this->modalFlags.bytes[2] == 0 &&
            this->m_nStarMapWindowOpen.bytes[3] == 0 &&
            Globals::layout->rewardMessageActive == 0) {
            Station *st2 = Globals::status->getStation();
            if (st2->getIndex() != 0x65) {
                Ship *ship = Globals::status->getShip();
                bool hasJump = ship->hasJumpDrive() != 0;
                bool hasFuel = Globals::status->getShip()->hasCargo(85, 1);
                uint32_t *routes = Globals::status->getSystem()->getRoutes();
                if (routes == 0 && hasJump == 0 && hasFuel == 0 &&
                    0x10 < Globals::status->getCurrentCampaignMission()) {
                    static_cast<ChoiceWindow *>(this->choiceWindow)->set(
                        *Globals::gameText->getText(335), true);
                    this->screenFlags.bytes[1] = 1;
                    this->m_nStarMapWindowOpen.bytes[3] = 1;
                }
            }
        }

        Globals::status->field_f8 = 1;
        this->scrollFlags.bytes[0] = 0;
        this->activeTouch = 0;
        this->cameraFlags.bytes[3] = 0;
        this->loadTick = 0;
        this->field_0x148.bytes[0] = 0;
        next = 1;
    } else if (state == 0x28) {
        if (Globals::status->inAlienOrbit() == 0) {
            Station *st = Globals::status->getStation();
            if (st->getIndex() == 0x78) {
                int camp = Globals::status->getCurrentCampaignMission();
                if (camp == 0x62 || Globals::status->getCurrentCampaignMission() == 100)
                    this->alarmFlags.bytes[0] = 1;
            }
        }
        if (Globals::status->inAlienOrbit() == 0) {
            Station *st = Globals::status->getStation();
            if (st->getIndex() == 0x3a && Globals::status->getCurrentCampaignMission() == 0x8a)
                this->alarmFlags.bytes[0] = 1;
        }
        if (Globals::status->inAlienOrbit() == 0) {
            Station *st = Globals::status->getStation();
            if (st->getIndex() == 0x7e && Globals::status->getCurrentCampaignMission() == 0x78)
                this->alarmFlags.bytes[0] = 1;
        }
        if (Globals::status->inAlienOrbit() == 0) {
            Station *st = Globals::status->getStation();
            if (st->getIndex() == 0x4e && Globals::status->getCurrentCampaignMission() == 0x8d)
                this->alarmFlags.bytes[0] = 1;
        }
        if (Globals::status->inSupernovaSystem() != 0)
            this->alarmFlags.bytes[0] = 1;
        if (Globals::enterSpaceLounge != 0)
            this->alarmFlags.halfword = 0x101;

        if (this->cameraFlags.bytes[0] == 0) {
            bool gate = false;
            Station *st = Globals::status->getStation();
            if (st->getIndex() == 4) gate = true;
            else {
                st = Globals::status->getStation();
                if (st->getIndex() == 0x58) gate = true;
                else {
                    Station *pirateBaseStation = Globals::status->getStation();
                    if (pirateBaseStation->stationHasPirateBase() != 0) {
                        this->alarmFlags.bytes[1] = 1;
                        goto afterGate;
                    }
                    gate = true;
                }
            }
            (void) gate;
            if (Globals::status->getCurrentCampaignMission() != 0x30 &&
                Globals::status->inBlackMarketSystem() == 0) {
                Station *s1 = Globals::status->getStation();
                if (s1->getIndex() != 0x6c) {
                    Station *s2 = Globals::status->getStation();
                    if (s2->getIndex() != 100) {
                        Station *s3 = Globals::status->getStation();
                        if (s3->getIndex() != 0x65 && this->modalFlags.bytes[2] == 0 &&
                            this->m_nStarMapWindowOpen.bytes[3] == 0 &&
                            this->alarmFlags.bytes[0] == 0 /* RAWREAD: alarmActive (+0xd8) */ &&
                            Globals::status->field_0x108 == 0) {
                            Station *currentStation = Globals::status->getStation();
                            bool enemy = currentStation->hasAttackedFriends() != 0;
                            if (!enemy) {
                                Standing *standing = Globals::status->getStanding();
                                SolarSystem *system = Globals::status->getSystem();
                                enemy = standing->isEnemy(system->getRace());
                            }
                            if (enemy) {
                                String notice;
                                Standing *standing = Globals::status->getStanding();
                                SolarSystem *system = Globals::status->getSystem();
                                int race = system->getRace();
                                if (standing->isEnemy(race)) {
                                    int standingRace = (unsigned) race > 1;
                                    int value = standing->getStanding(standingRace);
                                    if (value < 1)
                                        value = -standing->getStanding(standingRace);
                                    else
                                        value = standing->getStanding(standingRace);
                                    this->field_0xd4 = (int) ((float) value / 100.0f * 2800.0f);
                                    this->field_0xd4 +=
                                        static_cast<AbyssEngine::AERandom *>(Globals::rnd)->nextInt(200) - 100;
                                    if (Globals::status->hardCoreMode())
                                        this->field_0xd4 *= 10;
                                    notice = *Globals::gameText->getText(205);
                                } else {
                                    this->field_0xd4 = 150 * Globals::status->getLevel() + 1000;
                                    if (Globals::status->hardCoreMode())
                                        this->field_0xd4 *= 10;
                                    notice = *Globals::gameText->getText(206);
                                }
                                String credits = Globals::layout->formatCredits(this->field_0xd4);
                                notice = Globals::status->replaceHash(notice, credits, String("#C"));
                                static_cast<ChoiceWindow *>(this->choiceWindow)->set(notice, true);
                                this->m_nStarMapWindowOpen.bytes[3] = 1;
                                this->modalFlags.bytes[3] = 1;
                                this->dragFlags.bytes[1] = 0;
                            }
                        }
                    }
                }
            }
        afterGate:;
        }

        Station *here = Globals::status->getStation();
        int hereIdx = here->getIndex();
        Mission *campaignMission =
            reinterpret_cast<Mission *>(Globals::status->getCampaignMission());

        if (Globals::status->getCurrentCampaignMission() == 0x14 &&
            campaignMission->getTargetStation() == hereIdx) {
            Station *station = Globals::status->getStation();
            Array<Item *> *items = station->getItems();
            if (items != 0) {
                for (unsigned i = 0; i < items->size(); i = i + 1) {
                    if ((*items)[i]->getIndex() == 0x29)
                        (*items)[i]->setPrice(0);
                }
            }
            Station *rewardStation = Globals::status->getStation();
            Array<Item *> *itemTable = static_cast<Array<Item *> *>(Globals::items);
            Item *reward = (*itemTable)[0x29]->makeItem(10, 0);
            rewardStation->addItem(reward);
        }
        if (Globals::status->getCurrentCampaignMission() == 0x1b &&
            campaignMission->getTargetStation() == hereIdx) {
            Globals::status->getShip()->removeCargo(0x83);
        }

        if (Globals::status->getCurrentCampaignMission() == 0x2b) {
            if (hereIdx == 10) this->alarmFlags.bytes[1] = 1;
        } else if (hereIdx == 10 && Globals::status->getCurrentCampaignMission() == 0x2c) {
            this->alarmFlags.bytes[1] = 1;
        }

        bool isS100;
        if (hereIdx == 10 && Globals::status->gameWon() != 0) {
            Ship *ship = Globals::status->getShip();
            if (!ship->hasEquipment(0x55, 1)) {
                ship = Globals::status->getShip();
                if (!ship->hasCargo(0x55, 1)) {
                    Station *station = Globals::status->getStation();
                    if (!station->hasItem(0x55)) {
                        station = Globals::status->getStation();
                        if (!station->hasItem(0xa4)) {
                            Station *rewardStation = Globals::status->getStation();
                            Array<Item *> *itemTable = static_cast<Array<Item *> *>(Globals::items);
                            rewardStation->addItem((*itemTable)[0xa4]->makeItem(50));
                        }
                    }
                }
            }
            isS100 = false;
            goto fuelTop;
        } else {
            isS100 = hereIdx == 100;
            if ((unsigned) (hereIdx - 100) < 2 || hereIdx == 10) {
            fuelTop:
                Ship *playerShip = Globals::status->getShip();
                Item *cargo = playerShip->getCargo(0x7a);
                int amt = cargo != nullptr ? cargo->getAmount() : 0;
                Station *station = Globals::status->getStation();
                if (amt <= 5 && !station->hasItem(0x7a)) {
                    Station *fuelStation = Globals::status->getStation();
                    Array<Item *> *itemTable = static_cast<Array<Item *> *>(Globals::items);
                    fuelStation->addItem((*itemTable)[0x7a]->makeItem(10));
                }
            }
        }

        Station *st10 = Globals::status->getStation();
        if (st10->getIndex() == 10 &&
            static_cast<Achievements *>(Globals::achievements)->gotAllGoldMedals()) {
            Station *medalStation = Globals::status->getStation();
            Array<Ship *> *ships = medalStation->getShips();
            bool needNew = ships == 0;
            if (ships != 0) {
                unsigned i = 0;
                bool found = false;
                while (i < ships->size()) {
                    if ((*ships)[i]->getIndex() == 8) {
                        found = true;
                        break;
                    }
                    i = i + 1;
                }
                needNew = !found;
            }
            if (needNew) {
                Generator *generator = new Generator();
                Station *station = Globals::status->getStation();
                Station *buyStation = Globals::status->getStation();
                Array<Ship *> *list = generator->getShipBuyList(buyStation);
                station->setShips(list, false);
                delete generator;
            }
        }

        if (isS100 &&
            (Globals::status->dlc1Won() != 0 ||
             Globals::status->getCurrentCampaignMission() == 0x50 ||
             Globals::status->getCurrentCampaignMission() == 0x51 ||
             Globals::status->getCurrentCampaignMission() == 0x52 ||
             Globals::status->getCurrentCampaignMission() == 0x53 ||
             Globals::status->getCurrentCampaignMission() == 0x54)) {
            Station *station = Globals::status->getStation();
            if (station->hasShip(0x25)) {
                Array<Ship *> *ships = station->getShips();
                Ship *oldShip = nullptr;
                for (unsigned i = 0; i < ships->size(); ++i) {
                    if ((*ships)[i]->getIndex() == 0x25) {
                        oldShip = (*ships)[i];
                        break;
                    }
                }
                station->removeShip(oldShip);
            }
            Array<Ship *> *shipTable = static_cast<Array<Ship *> *>(Globals::ships);
            if (!station->hasShip(0x26)) {
                station->addShip((*shipTable)[0x26]->makeShip(-1));
            }
            if (!station->hasShip(0x28)) {
                station->addShip((*shipTable)[0x28]->makeShip(-1));
            }
        }

        Station *st6c = Globals::status->getStation();
        bool atReward = st6c->getIndex() == 0x6c;
        int rewardState = atReward ? Globals::status->field_114 : 0;
        if (atReward && rewardState == 1) {
            delete this->dialogueWindow;
            this->dialogueWindow = nullptr;
            Mission *rewardMission = new Mission(
                -1, AbyssEngine::String(*Globals::gameText->getText(1601), false),
                g_stationRewardFace, 4, 0, 0x6c, 0);
            rewardMission->setWon(true);
            Globals::status->field_114 = 2;
            this->dialogueWindow = new DialogueWindow(rewardMission, nullptr, 0);
            this->modalFlags.bytes[1] = 1;
        } else {
            Station *st = Globals::status->getStation();
            if (st->getIndex() == 0x6c && Globals::status->field_114 == 2) {
                ChoiceWindow *choice = static_cast<ChoiceWindow *>(this->choiceWindow);
                if (Globals::status->getCredits() >= 30000001 &&
                    Globals::status->getShip()->hasCargo(0x6d, 50)) {
                    choice->set(*Globals::gameText->getText(477), true);
                    this->screenFlags.bytes[0] = 1;
                } else {
                    choice->set(*Globals::gameText->getText(476));
                }
                this->m_nStarMapWindowOpen.bytes[3] = 1;
            }
        }

        Mission *freelanceMission = Globals::status->getFreelanceMission();
        if (freelanceMission != nullptr && freelanceMission->getType() == 0xe) {
            int targetStation = freelanceMission->getTargetStation();
            Station *currentStation = Globals::status->getStation();
            if (targetStation == currentStation->getIndex()) {
                Station *itemStation = Globals::status->getStation();
                if (!itemStation->hasItem(0x73)) {
                    Array<Item *> *itemTable = static_cast<Array<Item *> *>(Globals::items);
                    Item *reward = (*itemTable)[0x73]->makeItem(1);
                    Station *rewardStation = Globals::status->getStation();
                    rewardStation->addItem(reward);
                }
            }
        }

        ChoiceWindow *choice = static_cast<ChoiceWindow *>(this->choiceWindow);
        if (Globals::status->getCurrentCampaignMission() == 0xa2 &&
            Globals::hints[0x2b] == 0) {
            choice->set(*Globals::gameText->getText(613));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            Globals::hints[0x2b] = 1;
        }
        if (Globals::status->gameWon() != 0 && Globals::options[0x35] == 0 &&
            Globals::hints[0x38] == 0) {
            choice->set(*Globals::gameText->getText(110));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            Globals::hints[0x38] = 1;
        }
        if (Globals::status->dlc1Won() != 0 && Globals::options[0x37] == 0 &&
            Globals::hints[0x39] == 0) {
            choice->set(*Globals::gameText->getText(111));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
            Globals::hints[0x39] = 1;
        }

        int wanted = Globals::status->activateNewWanted();
        if (wanted > 0 && this->m_nStarMapWindowOpen.bytes[3] == 0) {
            if (wanted == 1)
                choice->set(*Globals::gameText->getText(3230));
            else
                choice->set(Globals::status->replaceHash(
                    String(*Globals::gameText->getText(3231), false),
                    String(wanted), String("#N")));
            this->m_nStarMapWindowOpen.bytes[3] = 1;
        }
        next = 0x14;
    } else if (state == 0x3c) {
        this->dragFlags.bytes[0] = 0;
        this->camScrollPos = 0;
        this->field_0xe0 = 0;
        this->camScrollVel = 0;
        this->field_0xf4 = 0;
        this->field_0xf8 = 0;
        this->field_0xfc = 0;
        int angle = (Globals::status->getSystem()->getRace() == 2) ? -200 : 270;
        this->camAngle = angle;
        if (this->cutScene != 0)
            this->cutScene->cameraRotX = (float) angle / 120.0f;
        this->field_0xec = 0;
        this->cameraFlags.bytes[0] =
                this->cameraFlags.bytes[3];
        const int tickerX = static_cast<int>(this->hangarLeftX);
        const int tickerY = Globals::h - Globals::layout->field_0x10 -
                            Globals::Canvas->GetTextHeight(Globals::font);
        this->newsTicker = new NewsTicker(
            tickerX, tickerY, Globals::w - tickerX,
            Globals::status->getSystem()->getRace(),
            Globals::status->getCurrentCampaignMission());
        this->accumTime = 0;
        this->cameraFlags.bytes[1] = 0;
        this->cameraFlags.bytes[2] = 0;
        this->field_0xd0 = 0;
        this->field_0xd4 = 0;
        this->alarmFlags.word = 0;
        this->choiceWindow = new ChoiceWindow();
        static const unsigned short factionImages[4] = {0x4a6, 0x4a3, 0x4a5, 0x4a4};
        Globals::Canvas->Image2DCreate(
            factionImages[Globals::status->getSystem()->getRace()],
            reinterpret_cast<unsigned int &>(this->departPending));
        next = 0x28;
    } else if (state == 0x50) {
        this->m_nStarMapWindowOpen.halfwords[1] = 0;   // upper half of 0x5c..0x5f word
        this->subWindowFlags.halfwords[0] = 0;          // lower half of 0x60..0x63 word (was 32-bit write @+2)
        this->screenFlags.bytes[2] = 0;
        this->screenFlags.halfword = 0;
        this->modalFlags.word = 0;
        this->subWindowFlags.halfwords[1] = 0;
        this->m_nStarMapWindowOpen.halfword = 0x100;
        this->hintFlags.bytes[0] = 0;
        this->hintFlags.halfwords[1] = 0;
        this->field_0x12c.bytes[0] = 0;
        this->closeHangarAfterCredits = 0;
        this->m_nStarMapWindowOpen.bytes[3] = 0;
        Layout *layout = Globals::layout;
        this->hangarLeftX = layout->field_0xcc;
        this->stationBoxX = 0;
        this->stationBoxY = layout->field_0xc;
        this->stationBoxHeight = Globals::h - layout->field_0xc - layout->field_0x10;

        this->buttonLaunch = new TouchButton(String(""), 12, Globals::w, Globals::h, 0x22);
        String credits = layout->formatCredits(Globals::status->getCredits());
        this->buttonCredits = new TouchButton(
            credits, 11,
            Globals::w - static_cast<TouchButton *>(this->buttonLaunch)->getWidth(),
            Globals::h, 0x22);

        auto *row = new Array<TouchButton *>();
        this->buttonRow = reinterpret_cast<int *>(row);
        ArraySetLength<TouchButton *>(5, *row);
        const int rowY = Globals::h - layout->field_0x10 - layout->field_0x24
                         - 5 * layout->field_0x30 - 4 * layout->field_0x34;
        const int rowWidth = static_cast<int>(this->hangarLeftX)
                             - layout->field_0x28 - layout->field_0x4c;
        const int rowStep = layout->field_0x30 + layout->field_0x34;
        (*row)[0] = new TouchButton(*Globals::gameText->getText(167), 0,
                                    layout->field_0x28, rowY, rowWidth, 0x11, 1);
        (*row)[1] = new TouchButton(*Globals::gameText->getText(398), 0,
                                    layout->field_0x28, rowY + rowStep, rowWidth, 0x11, 1);
        (*row)[2] = new TouchButton(*Globals::gameText->getText(177), 0,
                                    layout->field_0x28, rowY + 2 * rowStep, rowWidth, 0x11, 1);
        (*row)[3] = new TouchButton(*Globals::gameText->getText(129), 0,
                                    layout->field_0x28, rowY + 3 * rowStep, rowWidth, 0x11, 1);
        (*row)[4] = new TouchButton(*Globals::gameText->getText(169), 0,
                                    layout->field_0x28, rowY + 4 * rowStep, rowWidth, 0x11, 1);
        next = 0x3c;
    } else if (state == 100) {
        Globals *globals = static_cast<Globals *>(Globals::globals);
        globals->startNewSoundResourceList();
        static const int snd[] = {
            0x5f, 0x7a, 0x6c, 0x60, 0x61, 0x62, 99, 0x65, 100,
            0x66, 0x68, 0x69, 0x6a, 0x6b, 0x67, 0x7e
        };
        for (unsigned i = 0; i < sizeof(snd) / sizeof(snd[0]); i = i + 1)
            globals->addSoundResourceToList(snd[i]);

        this->introCountdown = 0x32;
        this->dragFlags.bytes[1] = 1;
        Status *status = Globals::status;
        this->stationName = status->getStation()->getName();
        int64_t enterTime = static_cast<int64_t>(
            static_cast<uint64_t>(static_cast<uint32_t>(status->field_70)) |
            (static_cast<uint64_t>(static_cast<uint32_t>(status->field_74)) << 32));
        if (status->getPlayingTime() - enterTime >= 0x7531) {
            if (status->getStation()->getIndex() != 0x6c) {
                Generator *g = new Generator();
                g->computerTradeGoods(status->getStation());
            }
        }
        next = 0x50;
    } else {
        Globals::sound->play(0x7a, nullptr, nullptr, 0.0f);
        Globals::sound->enableReverb(0);
        Globals::sound->setDownPitch(false);
        int music = Globals::switch_to_target_setting;
        if (music != -1)
            static_cast<Globals *>(Globals::globals)->playMusicAndFadeOutCurrent(music);
        Globals::switch_to_target_setting = -1;
        this->stationActive = 1;
        this->state = 100;

        auto *row = reinterpret_cast<Array<TouchButton *> *>(this->buttonRow);
        if (row != nullptr) {
            for (unsigned i = 0; i < row->size(); i = i + 1) {
                if (i < 10) {
                    Globals::sub_menu_buttons_x[i] = static_cast<int>((*row)[i]->getPosition().x);
                    Globals::sub_menu_buttons_y[i] = static_cast<int>((*row)[i]->getPosition().y);
                }
            }
            Globals::sub_menu_button_count = static_cast<int>(row->size());
        }
        return;
    }

    this->state = next;
}


static int *g_dlc_stack = 0;

void ModStation::showDlcMenu() {
    MenuTouchWindow *win = (MenuTouchWindow *) this->dlcMenu;
    if (win == 0) {
        win = new MenuTouchWindow(2);
        this->dlcMenu = win;
    }
    this->m_nStarMapWindowOpen.bytes[2] = 1;

    for (unsigned i = 0; i < win->buttons->size(); i = i + 1) {
        if (i < 10) {
            Globals::sub_menu_buttons_x[i] = (int) ((TouchButton *) (*win->buttons)[i])->getPosition().x;
            win = (MenuTouchWindow *) this->dlcMenu;
            Globals::sub_menu_buttons_y[i] = (int) ((TouchButton *) (*win->buttons)[i])->getPosition().y;
        }
    }
    Globals::sub_menu_button_count = win->buttons->size();
    this->subWindowFlags.bytes[2] = 0;
    win->callDlcMenu();
}


static int *g_cbs_stack = 0;

static int **g_cbs_textId = 0;

int GameText_getText_cbs(int id);

void ChoiceWindow_set_cbs(ChoiceWindow *cw, String *title, String *ok, int modal,
                          String *a, String *b, String *c, int d, int e);

void ModStation::showCBSMessage() {
    String emptyA, emptyB, ok;

    ChoiceWindow *cw = (ChoiceWindow *) this->choiceWindow;
    String *title = (String *) GameText_getText_cbs(**g_cbs_textId);
    ChoiceWindow_set_cbs(cw, title, &ok, 1, &emptyA, &emptyB, &emptyA, -1, -1);

    this->hintFlags.bytes[1] = 1;
    this->modalFlags.bytes[3] = 1;
}

int GameText_getText_frag(int id);

void ChoiceWindow_set1_frag(void *cw, int textStr);

void ChoiceWindow_setNotice_frag(void *cw, int textStr);

void ChoiceWindow_set6_frag(void *cw, int a, int b, int yesNo, int c, int d, int e);

int Status_holder_frag();

int GameText_root_frag();

int Status_getCredits_frag();
