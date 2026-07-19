#ifndef GOF2_AGENT_H
#define GOF2_AGENT_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/core/String.h"


#include "game/ship/Triple.h"
class Mission;



class Agent {
public:
    AbyssEngine::String name;
    AbyssEngine::String *wingman1;
    AbyssEngine::String *wingman2;
    int wingmanCount;
    AbyssEngine::String systemName;

    union {
        uint8_t wasAskedForDifficulty;
        uint8_t field_0x24;
    };

    union {
        uint8_t wasAskedForLocation;
        uint8_t field_0x25;
    };

    int field_0x28;
    int field_0x2c;
    int field_0x30;
    int sellItemIndex;
    int sellItemQuantity;
    int sellItemPrice;
    unsigned type;
    int station;
    int system;
    int race;
    uint8_t male;
    int eventCount;
    int category;
    int offer;
    int costs;
    int sellSystemIndex;
    int sellBlueprintIndex;
    AbyssEngine::String missionString;
    AbyssEngine::String stationName;
    uint8_t offerAccepted;
    uint8_t rewardAtNextChat;
    int *imageParts;
    Mission *mission;
    Array<AbyssEngine::String *> *wingmanNames;
    int sellModIndex;

    Agent(int kind, AbyssEngine::String name, int station, int system, int race,
          bool male, int sellSystemIndex, int sellBlueprintIndex, int sellModIndex,
          int sellItemPrice);

    ~Agent() noexcept(false);

    int *getImageParts();

    Mission *getMission();

    AbyssEngine::String getMissionString();

    int getModPricePercentage();

    AbyssEngine::String getName();

    int getSellModIndex();

    AbyssEngine::String getStationName();

    AbyssEngine::String getSystemName();

    AbyssEngine::String getWingmanName(int idx);

    Array<AbyssEngine::String *> *getWingmanNames();

    void giveRewardAtNextChat(bool v);

    uint8_t hasAcceptedOffer();

    uint8_t hasReward();

    bool isGenericAgent();

    bool isKnown();

    uint8_t isMale();

    bool isStoryAgent();

    void nextEvent();

    void setImageParts(int *parts);

    void setMission(Mission *mission);

    void setMissionString(AbyssEngine::String src);

    void setOfferAccepted(bool v);

    Triple *setSellItemData(int index, int quantity, int price);

    void setStationName(AbyssEngine::String src);

    void setSystemName(AbyssEngine::String src);

    void setWingmanFriendNames(Array<AbyssEngine::String *> *param);

    int getStation();

    int getSystem();

    int getRace();

    int getType();

    int getIndex();

    int getCosts();

    void setCosts(int costs);

    int getOffer();

    void setOffer(int offer);

    int getEvent();

    void setEvent(int event);

    int getSellItemIndex();

    int getSellItemQuantity();

    int getSellItemPrice();

    void setSellItemPrice(int price);

    int getSellSystemIndex();

    int getSellBlueprintIndex();

    int getWingmanFriendsCount();
};
#endif
