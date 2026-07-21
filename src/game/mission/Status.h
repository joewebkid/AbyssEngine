#ifndef GOF2_STATUS_H
#define GOF2_STATUS_H
#include <cstddef>
#include "BluePrint.h"
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Mission.h"
#include "PendingProduct.h"
#include "game/ship/Agent.h"
#include "game/ship/Ship.h"
#include "game/world/Standing.h"
#include "game/world/Station.h"
#include "game/world/Wanted.h"

class Agent;
class BluePrint;
class Mission;
class PendingProduct;
class Ship;
class SolarSystem;
class Standing;
class Station;
class Wanted;


using AbyssEngine::String;

// Layout is byte-faithful to the original 32-bit binary (492 bytes). Each
// `field_0xNN` sits at struct offset 0xNN; gaps that the binary leaves between
// observed fields are filled with `field_0xNN` filler slots so later fields land
// at their true offsets. Locked in by the static_asserts at the end of this file.
// pack(4): the original struct has no trailing padding; without this the 8-byte
// alignment of `playingTime` would round sizeof up to 496. Every member already
// lands on a >=4-byte boundary (playingTime stays 8-aligned at 0x1b8), so pack(4)
// changes no field offset -- it only drops the tail padding.
#pragma pack(push, 4)
class Status {
public:
    Array<Wanted *> *wanted;                 // 0x00
    int32_t collectedBounties[4];            // 0x04 (0x04..0x14)
    Standing *standing;                      // 0x14
    Array<BluePrint *> *bluePrints;          // 0x18
    Array<PendingProduct *> *pendingProducts;// 0x1c
    Array<Agent *> *agents;                  // 0x20
    int32_t wingmen;                         // 0x24
    union {                                   // 0x28
        int32_t field_0x28;
        struct {
            uint8_t _byte_0x28;
            uint8_t _byte_0x29;
            uint8_t byte_0x2a;               // 0x2a
            uint8_t _byte_0x2b;
        };
    };
    int32_t field_0x2c;                      // 0x2c (binary gap)

    union {                                  // 0x30
        int32_t field_0x30;
        uint32_t fadeValue;
    };

    union {                                   // 0x34
        int32_t passengers;
        struct {
            uint8_t _byte_0x34;
            uint8_t byte_0x35;               // 0x35 (gameWon flag)
            uint8_t _byte_0x36;
            uint8_t byte_0x37;               // 0x37 (dlc1Won flag)
        };
    };

    union {                                  // 0x38
        Array<bool> *systemVisibilities;

        struct {
            uint8_t field_0x34_b0;

            union {
                uint8_t dlcOverrideFlag;
                uint8_t flag_0x35;
            };

            uint8_t field_0x36_b2;

            union {
                uint8_t versionOverrideFlag;
                uint8_t flag_0x37;
            };
        };
    };

    Array<int> *field_0x3c;                  // 0x3c
    Array<int> *field_0x40;                  // 0x40
    Array<int> *field_0x44;                  // 0x44
    Array<int> *field_0x48;                  // 0x48
    union {                                   // 0x4c
        Array<bool> *field_4c;
        struct {
            uint8_t _byte_0x4c;
            uint8_t _byte_0x4d;
            uint8_t byte_0x4e;               // 0x4e
            uint8_t _byte_0x4f;
        };
    };
    Array<bool> *field_50;                   // 0x50
    Array<bool> *field_54;                   // 0x54
    Array<bool> *field_58;                   // 0x58
    int32_t field_5c;                        // 0x5c (shieldHp)
    int32_t field_60;                        // 0x60 (armorHp)
    int32_t field_64;                        // 0x64 (hullHp)
    int32_t field_68;                        // 0x68 (energy)
    int32_t field_6c;                        // 0x6c
    int32_t field_70;                        // 0x70
    int32_t field_74;                        // 0x74
    Station *playerStation;                  // 0x78
    int32_t field_7c;                        // 0x7c
    int32_t field_80;                        // 0x80
    int32_t field_84;                        // 0x84
    int32_t field_88;                        // 0x88 (binary gap)

    union {                                  // 0x8c (previousShip)
        int32_t field_8c;
        int32_t preSetField0x84;
    };

    Array<int> *field_90;                    // 0x90
    Array<bool> *field_94;                   // 0x94
    Array<bool> *field_98;                   // 0x98
    int32_t field_9c;                        // 0x9c
    int32_t field_a0;                        // 0xa0
    int32_t field_a4;                        // 0xa4
    int32_t field_a8;                        // 0xa8
    Array<bool> *field_ac;                   // 0xac
    int32_t field_b0;                        // 0xb0
    Array<bool> *field_b4;                   // 0xb4
    int32_t field_b8;                        // 0xb8
    int32_t field_bc;                        // 0xbc
    int32_t field_c0;                        // 0xc0
    int32_t field_c4;                        // 0xc4
    union {
        struct {
            int32_t field_c8;                // 0xc8
            int32_t field_cc;                // 0xcc
        };
        int64_t field_c8_q;                  // 0xc8  8-byte value (serialised via WriteLong)
    };
    int32_t field_d0;                        // 0xd0
    int32_t field_d4;                        // 0xd4
    int32_t field_d8;                        // 0xd8
    int32_t field_dc;                        // 0xdc
    int32_t field_e0;                        // 0xe0
    int32_t field_e4;                        // 0xe4
    int32_t field_e8;                        // 0xe8
    int32_t field_ec;                        // 0xec
    uint8_t field_0xf0;                      // 0xf0
    uint8_t field_0xf1;                      // 0xf1
    uint8_t _pad_0xf2[2];                    // 0xf2 (binary gap)
    int32_t field_f4;                        // 0xf4
    int16_t field_f8;                        // 0xf8
    uint8_t _pad_0xfa[6];                    // 0xfa (binary gap)
    int32_t field_0x100;                     // 0x100
    int32_t field_0x104;                     // 0x104
    uint8_t field_0x108;                     // 0x108
    uint8_t _pad_0x109[3];                   // 0x109 (binary gap)
    union {
        struct {
            int32_t field_10c;               // 0x10c
            uint8_t field_110;               // 0x110
            uint8_t field_0x111;             // 0x111
            uint8_t _pad_0x112[2];           // 0x112 (binary gap)
        };
        int64_t field_10c_q;                 // 0x10c  8-byte value (serialised via WriteLong)
    };
    int32_t field_114;                       // 0x114
    int32_t field_118;                       // 0x118
    int32_t field_11c;                       // 0x11c

    union {                                  // 0x120
        uint8_t field_120;
        int32_t mode_0x114;
    };

    int32_t field_124;                       // 0x124
    int32_t field_128;                       // 0x128
    int32_t field_12c;                       // 0x12c
    int32_t field_130;                       // 0x130
    int32_t field_134;                       // 0x134
    int32_t field_138;                       // 0x138
    int32_t field_13c;                       // 0x13c
    int32_t field_140;                       // 0x140
    int32_t field_144;                       // 0x144
    int32_t field_148;                       // 0x148

    union {                                  // 0x14c (generatedStation)
        Station *voidStation;
        int32_t field_14c;
    };

    int32_t field_150;                       // 0x150
    int32_t field_154;                       // 0x154
    int32_t field_158;                       // 0x158
    int32_t field_0x15c;                     // 0x15c (binary gap)
    int32_t field_160;                       // 0x160
    int32_t field_164;                       // 0x164
    String string_168;                       // 0x168 (0x168..0x174)
    int32_t field_174;                       // 0x174
    int32_t field_178;                       // 0x178 (missionCounter)
    uint8_t field_0x17c;                     // 0x17c
    uint8_t _pad_0x17d[7];                    // 0x17d (binary gap)
    int32_t field_0x184;                     // 0x184
    int32_t field_0x188;                     // 0x188
    int32_t field_0x18c;                     // 0x18c
    Ship *ship;                              // 0x190
    Mission *mission;                        // 0x194 (campaignMission)
    Array<Mission *> *missions;              // 0x198
    Station *station;                        // 0x19c (currentStation)
    Array<Station *> *stationStack;          // 0x1a0
    int32_t system;                          // 0x1a4
    int32_t planetNames;                     // 0x1a8
    int32_t planetTextures;                  // 0x1ac
    int32_t credits;                         // 0x1b0 (money)
    int32_t rating;                          // 0x1b4
    int64_t playingTime;                     // 0x1b8 (0x1b8..0x1c0)
    int32_t kills;                           // 0x1c0
    int32_t missionCount;                    // 0x1c4
    int32_t level;                           // 0x1c8
    int32_t lastXP;                          // 0x1cc
    int32_t stationsVisited;                 // 0x1d0
    int32_t goodsProduced;                   // 0x1d4
    int32_t pirateKills;                     // 0x1d8 (binary gap)
    int32_t jumpgatesUsed;                   // 0x1dc
    int32_t capturedCrates;                  // 0x1e0
    int32_t boughtEquipment;                 // 0x1e4
    int32_t currentCampaignMission;          // 0x1e8 (campaignStep)

    Status();

    ~Status();

    void addPendingProduct(BluePrint *product);

    int getPendingProducts();

    Array<bool> *getSystemVisibilities();

    void setSystemVisibility(int index, bool value);

    Array<Station *> *getStationStack();

    void setStationStack(Array<Station *> *stack);

    int addStationToStack(Station *station);

    bool isOnStack(Station *station);

    void setStation(Station *station);

    void departStation(Station *dest);

    bool inAlienOrbit();

    Station *getStation();

    int64_t getPlayingTime();

    void setMission(Mission *mission);

    Ship *getShip();

    bool gameWon();

    int getCurrentCampaignMission();

    int getCampaignMission();

    Mission *getMission();

    void moveWanted();

    int getPassengers();

    void setPassengers(int passengers);

    Array<Mission *> *getMissions();

    Mission *getFreelanceMission();

    void setFreelanceMission(Mission *mission);

    void setCampaignMission(Mission *mission);

    int getNumberOfMissions();

    int getMaxMissions();

    void incMissionCount();

    void setCurrentCampaignMission(int value);

    void nextCampaignMission(bool advance);

    void changeCredits(int delta);

    void setShip(Ship *ship);

    void setStationsVisited(int value);

    int getStationsVisited();

    bool dlc1Won();

    bool inEmptyOrbit();

    uint32_t inPlanetRingOrbit();

    uint32_t orbitHasPlanetRing(int index);

    bool inStormOrbit();

    int inSupernovaSystem();

    SolarSystem *getSystem();

    bool inFogSkyboxOrbit();

    bool inSupernovaOrbit();

    bool inDeepScienceOrbit();

    bool inBlackMarketSystem();

    bool inPirateLootOrbit();

    bool hardCoreMode();

    Wanted *getWantedInCurrentOrbit();

    int missionFailed(bool docked, int64_t time);

    Mission *missionCompleted(bool atStation, bool docked, long long extra);

    void setJumpgateUsed(int value);

    void jumpgateUsed();

    int getJumpgateUsed();

    void crateCaptured(int delta);

    void setCapturedCrates(int value);

    int getCapturedCrates();

    void incEquipmentBought();

    void setBoughtEquipment(int value);

    int getBoughtEquipment();

    void removeMission(Mission *mission);

    void visitStation();

    int getPlanetNames();

    int getPlanetTextures();

    int getCredits();

    void setRating(int value);

    void setPlayingTime(int64_t value);

    void setKills(int value);

    void setMissionCount(int value);

    void setLevel(int value);

    void setLastXP(int value);

    void setGoodsProduced(int value);

    int getGoodsProduced();

    void incGoodsProduced(int delta);

    void setCredits(int value);

    void checkForLevelUp();

    int getRating();

    int getLastXP();

    void changeRating(int delta);

    int getKills();

    void incKills();

    void setPirateKills(int value);

    int getPirateKills();

    void incPirateKills();

    void addKills(int delta);

    int getMissionCount();

    int getLevel();

    int getStanding();

    Array<BluePrint *> *getBluePrints();

    void unlockBluePrint(int index);

    int isBlueprintUnlocked(int index);

    int getAgents();

    void incPlayingTime(int64_t delta);

    int getWingmen();

    void setWingmen(Array<String *> *list);

    bool stringHasToken(String haystack, String needle);

    String replaceHash(String haystack, String needle);

    String replaceHash(String haystack, String needle, String replacement);

    void calcCargoPrices();

    void loadAgents();

    Array<int> *loadAgents(Array<int> *agents);

    int getCollectedBounties(int index);

    void incCollectedBounties(int index);

    int getGammaRayDamagePerSecond(int station, int system);

    void loadWanted();

    Array<Wanted *> *getWanted();

    bool isStorylineWanted(int index);

    int wantedBoardAccessible();

    int activateNewWanted();

    void resetGame();

    int isFreighterMissionStation(int station);

    int getFreighterMissionStationBit(int station);

    static Status *gStatus;
};
#pragma pack(pop)

// Byte-faithful layout lock-in (32-bit MATCH build / original binary = 492 bytes).
// Guarded: offsets only hold for the 32-bit parity target; native (64-bit) pointers differ.
#if __SIZEOF_POINTER__ == 4  // live in 32-bit MATCH build (was #ifdef GOF2_MATCH, which is never defined)
static_assert(sizeof(Status) == 492, "Status must be 492 bytes");
static_assert(offsetof(Status, wanted) == 0x0, "");
static_assert(offsetof(Status, standing) == 0x14, "");
static_assert(offsetof(Status, bluePrints) == 0x18, "");
static_assert(offsetof(Status, pendingProducts) == 0x1c, "");
static_assert(offsetof(Status, agents) == 0x20, "");
static_assert(offsetof(Status, field_0x28) == 0x28, "");
static_assert(offsetof(Status, field_0x30) == 0x30, "");
static_assert(offsetof(Status, passengers) == 0x34, "");
static_assert(offsetof(Status, systemVisibilities) == 0x38, "");
static_assert(offsetof(Status, field_5c) == 0x5c, "shieldHp");
static_assert(offsetof(Status, field_60) == 0x60, "armorHp");
static_assert(offsetof(Status, field_64) == 0x64, "hullHp");
static_assert(offsetof(Status, field_68) == 0x68, "energy");
static_assert(offsetof(Status, playerStation) == 0x78, "");
static_assert(offsetof(Status, field_8c) == 0x8c, "previousShip");
static_assert(offsetof(Status, field_0xf0) == 0xf0, "");
static_assert(offsetof(Status, field_f4) == 0xf4, "");
static_assert(offsetof(Status, field_0x100) == 0x100, "");
static_assert(offsetof(Status, field_110) == 0x110, "");
static_assert(offsetof(Status, field_0x111) == 0x111, "");
static_assert(offsetof(Status, field_114) == 0x114, "");
static_assert(offsetof(Status, field_11c) == 0x11c, "");
static_assert(offsetof(Status, field_124) == 0x124, "");
static_assert(offsetof(Status, field_14c) == 0x14c, "generatedStation");
static_assert(offsetof(Status, field_150) == 0x150, "");
static_assert(offsetof(Status, field_158) == 0x158, "");
static_assert(offsetof(Status, field_178) == 0x178, "missionCounter");
static_assert(offsetof(Status, field_0x17c) == 0x17c, "");
static_assert(offsetof(Status, ship) == 0x190, "");
static_assert(offsetof(Status, mission) == 0x194, "campaignMission");
static_assert(offsetof(Status, missions) == 0x198, "");
static_assert(offsetof(Status, station) == 0x19c, "currentStation");
static_assert(offsetof(Status, stationStack) == 0x1a0, "");
static_assert(offsetof(Status, system) == 0x1a4, "");
static_assert(offsetof(Status, credits) == 0x1b0, "money");
static_assert(offsetof(Status, rating) == 0x1b4, "");
static_assert(offsetof(Status, kills) == 0x1c0, "");
static_assert(offsetof(Status, missionCount) == 0x1c4, "");
static_assert(offsetof(Status, level) == 0x1c8, "");
static_assert(offsetof(Status, lastXP) == 0x1cc, "");
static_assert(offsetof(Status, stationsVisited) == 0x1d0, "");
static_assert(offsetof(Status, goodsProduced) == 0x1d4, "");
static_assert(offsetof(Status, jumpgatesUsed) == 0x1dc, "");
static_assert(offsetof(Status, capturedCrates) == 0x1e0, "");
static_assert(offsetof(Status, boughtEquipment) == 0x1e4, "");
static_assert(offsetof(Status, currentCampaignMission) == 0x1e8, "campaignStep");
#endif // GOF2_MATCH

#endif
