#include "game/mission/GameRecord.h"

#include "game/world/Galaxy.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/mission/Achievements.h"
#include "game/mission/BluePrint.h"
#include "game/world/Station.h"
#include "game/mission/Mission.h"
#include "game/world/SolarSystem.h"
#include "game/ship/Ship.h"

GameRecord::~GameRecord() {
}

GameRecord::GameRecord() {
    this->data = new uint8_t[0x87];
    this->rank = 0;

    this->field_0x08 = 0;
    this->field_0x0c = 0;
    this->playTimeObj = nullptr;
    this->field_0x14 = 0;
    this->field_0x18 = 0;
    this->field_0x1c = 0;
    this->killsText = 0;
    this->field_0x24 = 0;
    this->field_0x28 = 0;
    this->field_0x2c = 0;
    this->field_0x30 = 0;
    this->field_0x34 = 0;
    this->field_0x38 = 0;
    this->field_0x3c = 0;
    this->field_0x40 = 0;
    this->field_0x44 = 0;
    this->field_0x48 = 0;
    this->field_0x4c = 0;
    this->field_0x50 = 0;
    this->field_0x54 = 0;
    this->field_0x58 = 0;
    this->field_0x5c = 0;
    this->field_0x60 = 0;
    this->field_0x64 = 0;
    this->field_0x68 = 0;
    this->field_0x6c = 0;
    this->field_0x70 = 0;
    this->field_0x74 = 0;
    this->field_0x78 = 0;
    this->field_0x7c = 0;
    this->field_0x80 = 0;
    this->field_0x84 = 0;
    this->field_0x88 = 0;
    this->field_0x8c = 0;
    this->field_0x90 = 0;

    this->field_0xb8 = 0;
    this->field_0xbc = 0;
    this->field_0xc0 = 0;
    this->field_0xc4 = 0;
    this->field_0xa8 = 0;
    this->field_0xac = 0;
    this->field_0xb0 = 0;
    this->field_0xb4 = 0;
    this->field_0x98 = 0;
    this->field_0x9c = 0;
    this->field_0xa0 = 0;
    this->field_0xa4 = 0;
    this->field_0x10b = 0;
    this->field_0x10c = 0;
    this->field_0x10d = 0;
    this->field_0x10e = 0;
    this->field_0x10f = 0;
    this->field_0x110 = 0;
    this->field_0x111 = 0;
    this->field_0x112 = 0;
    this->field_0x113 = 0;
    this->field_0x114 = 0;
    this->dlcRequiredFlag = 0;
    this->field_0x116 = 0;
    this->versionMismatchFlag = 0;
    this->field_0x118 = 0;
    this->field_0x119 = 0;
    this->field_0x11a = 0;
    this->field_0x100 = 0;
    this->field_0x101 = 0;
    this->field_0x102 = 0;
    this->field_0x103 = 0;
    this->field_0x104 = 0;
    this->field_0x105 = 0;
    this->field_0x106 = 0;
    this->field_0x107 = 0;
    this->field_0x108 = 0;
    this->field_0x109 = 0;
    this->field_0x10a = 0;
    this->field_0xf0 = 0;
    this->field_0xf4 = 0;
    this->field_0xf8 = 0;
    this->field_0xfc = 0;
    this->field_0xe0 = 0;
    this->field_0xe4 = 0;
    this->field_0xe8 = 0;
    this->field_0xec = 0;

    this->shipId = 0xffffffff;
    this->field_0xc8 = 0;
    this->field_0xcc = 0;
    this->field_0xd0 = 0;
    this->field_0xd4 = 0;
    this->field_0xd8 = 0;
    this->field_0xdc = 0;
    this->field_0x1b8 = 0;
    this->field_0x1c0 = 0;

    this->field_0x130 = 0;
    this->field_0x134 = 0;
    this->field_0x138 = 0;
    this->field_0x13c = 0;
    this->field_0x140 = 0;
    this->field_0x144 = 0;
    this->field_0x148 = 0;
    this->field_0x14c = 0;
    this->field_0x150 = 0;
    this->field_0x154 = 0;
    this->field_0x158 = 0;
    this->field_0x15c = 0;
    this->field_0x160 = 0;
    this->field_0x164 = 0;
    this->field_0x168 = 0;
    this->field_0x16c = 0;
    this->field_0x170 = 0;
    this->field_0x174 = 0;
    this->field_0x178 = 0;
    this->field_0x17c = 0;
    this->field_0x180 = 0;
    this->field_0x184 = 0;

    this->field_0x1a4 = 0;
    this->field_0x1a8 = 0;
    this->field_0x1ac = 0;
    this->field_0x1b0 = 0;

    this->field_0x1b4 = 0;
}

typedef uint32_t uint;

long Array_dtor(void *self);

long Station_getIndex(...);

long Station_getSystem(...);

// The player-ego object lives behind a global pointer slot at 0x220290.
// GameRecord::load() restores a packed block of its fields from the save record.
// This overlay names exactly the byte offsets that load() touches (0x08..0x3a);
// the leading 8 bytes (vtable/owner ptrs) are left as opaque padding.
struct PlayerEgoSaveBlock {
    uint8_t  _pad_0x00[0x8];    // 0x00
    uint32_t restored_0x08;     // 0x08
    uint32_t restored_0x0c;     // 0x0c
    uint32_t restored_0x10;     // 0x10
    uint32_t restored_0x14;     // 0x14
    uint32_t restored_0x18;     // 0x18
    uint32_t restored_0x1c;     // 0x1c
    uint32_t restored_0x20;     // 0x20
    uint32_t restored_0x24;     // 0x24
    uint8_t  flag_0x28;         // 0x28
    uint8_t  flag_0x29;         // 0x29
    uint8_t  flag_0x2a;         // 0x2a
    uint8_t  flag_0x2b;         // 0x2b
    uint8_t  flag_0x2c;         // 0x2c
    uint8_t  flag_0x2d;         // 0x2d
    uint8_t  flag_0x2e;         // 0x2e
    uint8_t  flag_0x2f;         // 0x2f
    uint8_t  flag_0x30;         // 0x30
    uint8_t  flag_0x31;         // 0x31
    uint8_t  flag_0x32;         // 0x32
    uint8_t  flag_0x33;         // 0x33
    uint8_t  flag_0x34;         // 0x34
    uint8_t  flag_0x35;         // 0x35
    uint8_t  flag_0x36;         // 0x36
    uint8_t  flag_0x37;         // 0x37
    uint8_t  flag_0x38;         // 0x38
    uint8_t  flag_0x39;         // 0x39
    uint8_t  flag_0x3a;         // 0x3a
};
static PlayerEgoSaveBlock *const *const kPlayerEgoSlot =
        (PlayerEgoSaveBlock *const *const) 0x220290;
// A separate global slot at 0x2202bc holds a pointer to a controller object whose
// first field is the active campaign Mission pointer (read into Status). The slot is
// dereferenced twice: load the controller, then load its Mission* at offset 0.
static Mission *const *const *const kCampaignMissionSlot =
        (Mission *const *const *const) 0x2202bc;

// The mission-flags object behind the global slot at 0x22016c. Only three of its
// fields are touched by load(): two read-only DLC/version flags and a writable
// scalar; model them at their exact byte offsets so access is by name.
struct MissionFlags {
    uint8_t  _pad_0x00[0x2c];   // 0x00
    uint32_t field_0x2c;        // 0x2c  (written from rec[0x47])
    uint8_t  _pad_0x30[0x5];    // 0x30
    uint8_t  dlcSkyboxFlag;     // 0x35  (read)
    uint8_t  versionGuardFlag;  // 0x36  (read)
};
static MissionFlags *const *const kMissionFlagsSlot = (MissionFlags *const *const) 0x22016c;

static const uint32_t kRecordExtendedTag = 0x6e6a78;

void GameRecord::load() {
    bool campaignLocked;
    int campaignStage;
    void *deadArray;
    Array<uint8_t> *srcByteArray;
    Array<uint32_t> *srcVec;
    BluePrint *blueprint;
    Station *station;
    int targetStation;
    uint32_t scalar;
    int srcCount;
    int copyIndex;
    Array<uint8_t> *srcByteArray2;
    uint32_t i;
    uint32_t j;
    Status *st;

    Status::gStatus->resetGame();
    Galaxy::gGalaxy->setVisited((bool *) this->data, this->field_0x04);
    Status::gStatus->setLastXP(this->field_0x24);
    Status::gStatus->setCredits(this->field_0x08);
    Status::gStatus->setRating(this->field_0x0c);
    Status::gStatus->setPlayingTime(*(int64_t *) &this->playTimeObj);
    Status::gStatus->setKills(this->field_0x18);
    Status::gStatus->setMissionCount(this->field_0x1c);
    Status::gStatus->setLevel(this->killsText);
    Status::gStatus->setLastXP(this->field_0x24);
    Status::gStatus->setGoodsProduced(this->field_0x28);
    Status::gStatus->setPirateKills(this->field_0x2c);
    Status::gStatus->setJumpgateUsed(this->field_0x30);
    Status::gStatus->setCapturedCrates(this->field_0x34);
    Status::gStatus->setBoughtEquipment(this->field_0x38);
    Status::gStatus->setStationsVisited(this->field_0x3c);
    st = Status::gStatus;
    st->field_80 = this->field_0x44;
    st->field_7c = this->field_0x48;
    st->field_84 = this->field_0x4c;
    st->field_88 = this->field_0x50;
    if (this->dlcRequiredFlag == '\0') {
        if (this->versionMismatchFlag == '\0') {
            if (0x2d < (long) this->field_0x40) {
                this->field_0x40 = 0x2d;
            }
            goto applyMissionCap;
        }
    LAB_dlcUnlocked:
        campaignLocked = true;
    } else {
        if (this->versionMismatchFlag != '\0') goto LAB_dlcUnlocked;
    applyMissionCap:
        if (0x54 < (long) this->field_0x40) {
            this->field_0x40 = 0x54;
        }
        campaignLocked = false;
    }
    if (((this->dlcRequiredFlag != '\0' || campaignLocked) && (this->field_0x40 == 0x2e)) &&
        (campaignStage = ((Mission *) this->field_0x58)->isEmpty(), campaignStage != 0)) {
        this->field_0x40 = 0x2d;
    }
    Status::gStatus->setCurrentCampaignMission(this->field_0x40);
    this->field_0x102 = 0x2d < (long) this->field_0x40;
    this->field_0x119 = 0x55 < (long) this->field_0x40;
    Status::gStatus->setFreelanceMission((Mission *) this->field_0x54);
    Status::gStatus->setCampaignMission((Mission *) this->field_0x58);
    Status::gStatus->setStationStack((Array<Station *> *) this->field_0x5c);
    campaignStage = this->field_0x40;
    if (campaignStage == 0x23) {
        targetStation = ((Mission *) this->field_0x58)->getTargetStation();
        if (targetStation != 0x1d) {
            Mission *m = new Mission(0xb, 0, 0x1d);
            Status::gStatus->setCampaignMission(m);
        }
        campaignStage = this->field_0x40;
    }
    if (campaignStage == 0x1d) {
        this->field_0x44 = 0x5b;
        this->field_0x48 = 0x12;
        Status::gStatus->setCurrentCampaignMission(0x1c);
        Mission *m = new Mission(4, 0, 0x5b);
        Status::gStatus->setCampaignMission(m);
        campaignStage = this->field_0x40;
    }
    if (campaignStage == 0x19) {
        this->field_0x44 = 0x30;
        this->field_0x48 = 9;
        Status::gStatus->setCurrentCampaignMission(0x18);
        Mission *m = new Mission(4, 0, 0x30);
        Status::gStatus->setCampaignMission(m);
        campaignStage = this->field_0x40;
    }
    if (campaignStage == 0x29) {
        Status::gStatus->setCurrentCampaignMission(0x27);
        Mission *m = new Mission(0xb, 0, 0x1e);
        Status::gStatus->setCampaignMission(m);
    }
    if (((this->versionMismatchFlag != '\0') || (this->field_0x118 != '\0')) &&
        ((this->field_0x40 == 0x56 && (targetStation = ((Mission *) this->field_0x58)->getTargetStation(), targetStation != 100)))) {
        (*kPlayerEgoSlot)->flag_0x31 = 1;
        Mission *m = new Mission(0xb, 0, 100);
        Status::gStatus->setCampaignMission(m);
    }
    if (((this->versionMismatchFlag != '\0') || (this->field_0x118 != '\0')) &&
        ((this->field_0x40 == 0x57 && (targetStation = ((Mission *) this->field_0x58)->getTargetStation(), targetStation != 10)))) {
        (*kPlayerEgoSlot)->flag_0x31 = 1;
        Mission *m = new Mission(4, 0, 10);
        Status::gStatus->setCampaignMission(m);
    }
    if ((((this->versionMismatchFlag != '\0') || (this->field_0x118 != '\0')) &&
         (this->field_0x40 == 0x58)) && (targetStation = ((Mission *) this->field_0x58)->getTargetStation(), targetStation != 10)) {
        (*kPlayerEgoSlot)->flag_0x31 = 1;
        Mission *m = new Mission(0xb, 0, 10);
        Status::gStatus->setCampaignMission(m);
    }
    if (((this->versionMismatchFlag != '\0') || (this->field_0x118 != '\0')) &&
        ((this->field_0x40 == 0x59 && (targetStation = ((Mission *) this->field_0x58)->getTargetStation(), targetStation != 10)))) {
        (*kPlayerEgoSlot)->flag_0x31 = 1;
        Status::gStatus->setCurrentCampaignMission(0x56);
        Mission *m = new Mission(0xb, 0, 100);
        Status::gStatus->setCampaignMission(m);
    }
    if ((this->versionMismatchFlag != '\0') || (this->field_0x118 != '\0')) {
        campaignStage = this->field_0x40;
        if (campaignStage == 0x5b) {
            targetStation = ((Mission *) this->field_0x58)->getTargetStation();
            if (targetStation == 0x6e) {
                campaignStage = this->field_0x40;
                goto LAB_checkStageRange;
            }
        } else {
        LAB_checkStageRange:
            if ((0xc < campaignStage - 0x5bU) ||
                (((Array<uint8_t> *) this->field_0x160)->data_[0x1b] != '\0'))
                goto stationStackLoaded;
        }
        (*kPlayerEgoSlot)->flag_0x31 = 1;
        this->field_0x40 = 0x56;
        Status::gStatus->setCurrentCampaignMission(this->field_0x40);
        Mission *m = new Mission(0xb, 0, 100);
        Status::gStatus->setCampaignMission(m);
    }
stationStackLoaded:
    ((Station *) Status::gStatus->voidStation)->setItems((Array<Item *> *) this->field_0x180, true);
    ((Station *) Status::gStatus->voidStation)->setShips((Array<Ship *> *) this->field_0x184, true);
    st = Status::gStatus;
    if (st->field_94 != (Array<bool> *) 0x0) {
        deadArray = (void *) Array_dtor(st->field_94);
        ::operator delete(deadArray);
        st = Status::gStatus;
    }
    st->field_94 = (Array<bool> *) this->field_0x68;
    if (st->field_98 != (Array<bool> *) 0x0) {
        deadArray = (void *) Array_dtor(st->field_98);
        ::operator delete(deadArray);
        st = Status::gStatus;
    }
    st->field_98 = (Array<bool> *) this->field_0x6c;
    if (st->field_90 != (Array<int> *) 0x0) {
        deadArray = (void *) Array_dtor(st->field_90);
        ::operator delete(deadArray);
        st = Status::gStatus;
    }
    st->field_90 = (Array<int> *) this->field_0x70;
    st->field_10c = this->field_0xd0;

    MissionFlags *missionFlags = *kMissionFlagsSlot;
    st->field_110 = this->field_0xd4;
    st->field_0x111 = this->field_0xdc;
    if (missionFlags->versionGuardFlag == '\0') {
        scalar = this->field_0xd8;
    } else {
        scalar = 3;
    }
    st->field_114 = scalar;
    // qword copy: Status 0x9c..0xa8 <- record 0x74..0x83 (rec[0x1d..0x20])
    st->field_9c = this->field_0x74;
    st->field_a0 = this->field_0x78;
    st->field_a4 = this->field_0x7c;
    st->field_a8 = this->field_0x80;
    if (st->field_ac != (Array<bool> *) 0x0) {
        deadArray = (void *) Array_dtor(st->field_ac);
        ::operator delete(deadArray);
        st = Status::gStatus;
    }
    st->field_ac = (Array<bool> *) this->field_0x84;
    st->field_b0 = this->field_0x88;
    srcByteArray = (Array<uint8_t> *) this->field_0x8c;
    srcCount = srcByteArray->size_;
    for (copyIndex = 0; srcCount != copyIndex; copyIndex = copyIndex + 1) {
        st->field_b4->data_[copyIndex] = srcByteArray->data_[copyIndex];
    }
    st->field_b8 = this->field_0x90;
    scalar = this->field_0x9c;
    st->field_c0 = this->field_0x98;
    st->field_c4 = scalar;
    // qword copies: Status 0xc8..0xe7 <- record 0xa0..0xbf (rec[0x28..0x2f])
    st->field_c8 = this->field_0xa0;
    st->field_cc = this->field_0xa4;
    st->field_d0 = this->field_0xa8;
    st->field_d4 = this->field_0xac;
    st->field_d8 = this->field_0xb0;
    st->field_dc = this->field_0xb4;
    st->field_e0 = this->field_0xb8;
    st->field_e4 = this->field_0xbc;
    st->field_e8 = this->field_0xc0;
    st->field_ec = this->field_0xc4;
    Achievements::gAchievements->init();
    if ((int *) this->field_0x60 != (int *) 0x0) {
        Achievements::gAchievements->setMedals((int *) this->field_0x60, this->field_0x64);
    }
    Status::gStatus->setShip((Ship *) this->field_0x130);
    if (Status::gStatus->dlc1Won() != 0) {
        Item *dlcItem = (Item *) Status::gStatus->getShip()->getFirstEquipmentOfSort(0x12);
        if (dlcItem == (Item *) 0x0) {
            dlcItem = (Item *) Status::gStatus->getShip()->getCargo(0x55);
            if (dlcItem == (Item *) 0x0) goto afterDlcUnsaleable;
        }
        dlcItem->setUnsaleable(false);
    }
afterDlcUnsaleable:
    if (Status::gStatus->gameWon() != 0) {
        Array<Item *> *cargo = ((Ship *) (Status::gStatus->getShip()))->getCargo();
        if (cargo != (Array<Item *> *) 0x0) {
            for (i = 0; i < cargo->size(); i = i + 1) {
                Item *item = (*cargo)[i];
                if ((item != (Item *) 0x0) && (item->getIndex() == 0x83)) {
                    item->setUnsaleable(false);
                }
            }
        }
    }
    if (0x79 < (long) this->field_0x40) {
        Array<Item *> *cargo = ((Ship *) (Status::gStatus->getShip()))->getCargo();
        if (cargo != (Array<Item *> *) 0x0) {
            for (i = 0; i < cargo->size(); i = i + 1) {
                Item *item = (*cargo)[i];
                if ((item != (Item *) 0x0) && (item->getIndex() == 0xd1)) {
                    item->setUnsaleable(false);
                }
            }
        }
    }
    Array<BluePrint *> *bluePrintArray = (Array<BluePrint *> *) this->field_0x140;
    if (bluePrintArray != (Array<BluePrint *> *) 0x0) {
        for (i = 0; i < bluePrintArray->size_; i = i + 1) {
            blueprint = bluePrintArray->data_[i];
            if ((blueprint != (BluePrint *) 0x0) &&
                (((blueprint->getIndex() == 0xdf ||
                   (bluePrintArray->data_[i]->getIndex() == 0xd2)) &&
                  (bluePrintArray->data_[i]->isEmpty() == 0)))) {
                station = (Station *) Galaxy::gGalaxy->getStation(
                    (int) bluePrintArray->data_[i]->getStationIndex());
                if (station != (Station *) 0x0) {
                    int sysIndex = Station_getSystem(station);
                    SolarSystem *sys = (SolarSystem *) Galaxy::gGalaxy->getSystem(sysIndex);
                    if (sys->getRoutes() == (void *) 0x0) {
                        ((BluePrint *) bluePrintArray->data_[i])->stationIndex = 10;
                        delete station;
                        station = (Station *) Galaxy::gGalaxy->getStation(
                            (int) bluePrintArray->data_[i]->getStationIndex());
                        bluePrintArray->data_[i]->stationName =
                                ((Station *) (station))->getName();
                        if (station == (Station *) 0x0) goto LAB_blueprintLoopTail;
                    }
                    delete station;
                }
            }
        LAB_blueprintLoopTail:
            bluePrintArray = (Array<BluePrint *> *) this->field_0x140;
        }
    }
    st = Status::gStatus;
    st->field_8c = this->field_0x134;
    st->setStation((Station *) this->field_0x138);
    Status::gStatus->setMission(**kCampaignMissionSlot);
    st->standing = (Standing *) this->field_0x13c;
    {
        Array<uint32_t> *bpVec = (Array<uint32_t> *) this->field_0x140;
        for (i = 0; i < bpVec->size_; i = i + 1) {
            ((Array<uint32_t> *) Status::gStatus->bluePrints)->data_[i] = bpVec->data_[i];
        }
    }
    Status::gStatus->pendingProducts = (Array<PendingProduct *> *) this->field_0x144;
    {
        Array<uint32_t> *agVec = (Array<uint32_t> *) this->field_0x148;
        for (i = 0; i < agVec->size_; i = i + 1) {
            ((Array<uint32_t> *) Status::gStatus->agents)->data_[i] = agVec->data_[i];
        }
    }
    st = Status::gStatus;
    st->wingmen = this->field_0x14c;
    st->field_0x2c = this->field_0x150;
    st->field_0x30 = this->field_0x154;
    st->field_0x28 = this->field_0x158;
    st->passengers = this->field_0x15c;
    srcByteArray2 = (Array<uint8_t> *) this->field_0x160;
    srcCount = srcByteArray2->size_;
    for (copyIndex = 0; srcCount != copyIndex; copyIndex = copyIndex + 1) {
        ((Array<uint8_t> *) st->systemVisibilities)->data_[copyIndex] =
                srcByteArray2->data_[copyIndex];
    }
    uint8_t *visBuf = ((Array<uint8_t> *) st->systemVisibilities)->data_;
    if (missionFlags->dlcSkyboxFlag != '\0') {
        visBuf[0x19] = 1;
    }
    visBuf[0x1a] = 1;
    srcVec = (Array<uint32_t> *) this->field_0x164;
    for (i = 0; i < srcVec->size_; i = i + 1) {
        ((Array<uint32_t> *) st->field_0x3c)->data_[i] = srcVec->data_[i];
    }
    srcVec = (Array<uint32_t> *) this->field_0x168;
    for (i = 0; i < srcVec->size_; i = i + 1) {
        ((Array<uint32_t> *) st->field_0x40)->data_[i] = srcVec->data_[i];
    }
    srcVec = (Array<uint32_t> *) this->field_0x16c;
    for (i = 0; i < srcVec->size_; i = i + 1) {
        ((Array<uint32_t> *) st->field_0x44)->data_[i] = srcVec->data_[i];
    }
    srcVec = (Array<uint32_t> *) this->field_0x170;
    for (i = 0; i < srcVec->size_; i = i + 1) {
        ((Array<uint32_t> *) st->field_0x48)->data_[i] = srcVec->data_[i];
    }
    {
        Array<uint8_t> *dstByteVec = (Array<uint8_t> *) st->field_54;
        st->field_4c = (Array<bool> *) this->field_0x174;
        srcByteArray2 = (Array<uint8_t> *) this->field_0x178;
        i = srcByteArray2->size_;
        if (dstByteVec->size_ <= i) {
            for (j = 0; i != j; j = j + 1) {
                dstByteVec->data_[j] = srcByteArray2->data_[j];
            }
        }
    }

    PlayerEgoSaveBlock *ego = *kPlayerEgoSlot;
    // qword copies: ego 0x08..0x17 <- record 0xe4..0xf3 (rec[0x39..0x3c])
    ego->restored_0x08 = this->field_0xe4;
    ego->restored_0x0c = this->field_0xe8;
    ego->restored_0x10 = this->field_0xec;
    ego->restored_0x14 = this->field_0xf0;

    missionFlags->field_0x2c = reinterpret_cast<uint32_t &>(this->rank);
    scalar = this->field_0xcc;
    st->field_0x100 = this->field_0xc8;
    st->field_0x104 = scalar;
    // qword copies: ego 0x18..0x27 <- record 0xf4..0x103 (rec[0x3d..0x40])
    ego->restored_0x18 = this->field_0xf4;
    ego->restored_0x1c = this->field_0xf8;
    ego->restored_0x20 = this->field_0xfc;
    ego->restored_0x24 = this->field_0x100;
    if (this->field_0x1bc == kRecordExtendedTag) {
        ego->flag_0x29 = this->field_0x105;
        ego->flag_0x28 = this->field_0x104;
        ego->flag_0x2a = this->field_0x106;
        ego->flag_0x2b = this->field_0x107;
        ego->flag_0x2c = this->field_0x108;
        ego->flag_0x2d = this->field_0x109;
        ego->flag_0x2e = this->field_0x10a;
        ego->flag_0x2f = this->field_0x10b;
        ego->flag_0x30 = this->field_0x10c;
        st->field_178 = this->field_0x1b8;
        ego->flag_0x31 = this->field_0x119;
        ego->flag_0x32 = this->field_0x11a;
        ego->flag_0x33 = this->field_0x10d;
        ego->flag_0x34 = this->field_0x10e;
        ego->flag_0x35 = this->field_0x10f;
        ego->flag_0x36 = this->field_0x110;
        ego->flag_0x37 = this->field_0x111;
        ego->flag_0x38 = this->field_0x113;
        ego->flag_0x39 = this->field_0x112;
        ego->flag_0x3a = this->field_0x114;
        st->field_58 = (Array<bool> *) this->field_0x17c;
        srcVec = (Array<uint32_t> *) this->field_0x1b4;
        if ((srcVec != (Array<uint32_t> *) 0x0) && (i = srcVec->size_, i != 0)) {
            for (j = 0; j < i; j = j + 1) {
                ((Array<uint32_t> *) Status::gStatus->wanted)->data_[j] = srcVec->data_[j];
                srcVec = (Array<uint32_t> *) this->field_0x1b4;
                i = srcVec->size_;
            }
        }
        st = Status::gStatus;
        st->collectedBounties[0] = this->field_0x1a4;
        st->collectedBounties[1] = this->field_0x1a8;
        st->collectedBounties[2] = this->field_0x1ac;
        st->collectedBounties[3] = this->field_0x1b0;
        station = Status::gStatus->getStation();
        if (Station_getIndex(station) == 0x6c) {
            Station *stStation = Status::gStatus->getStation();
            Array<Ship *> *stationShips = (Array<Ship *> *) stStation->getShips();
            if (stationShips != (Array<Ship *> *) 0x0) {
                Array<Ship *> *recShips = (Array<Ship *> *) this->field_0x184;
                for (i = 0; i < stationShips->size(); i = i + 1) {
                    Array<int> *mods = recShips->data_[i]->getMods();
                    if (mods != (Array<int> *) 0x0) {
                        for (j = 0; j < mods->size(); j = j + 1) {
                            (*stationShips)[i]->addMod((*mods)[j]);
                        }
                    }
                }
            }
        }
        st = Status::gStatus;
        st->field_118 = this->field_0xe0;
        st->field_0x17c = this->field_0x1c0;
    }
}
