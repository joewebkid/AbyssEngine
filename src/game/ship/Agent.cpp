#include "game/ship/Agent.h"
#include "game/core/String.h"
#include "game/mission/Mission.h"

String Agent::getStationName() {
    return this->stationName;
}

uint8_t Agent::hasAcceptedOffer() {
    return this->offerAccepted;
}

bool Agent::isStoryAgent() {
    return this->category == 0;
}

Array<AbyssEngine::String *> *Agent::getWingmanNames() {
    return this->wingmanNames;
}

int *Agent::getImageParts() {
    return this->imageParts;
}

Mission *Agent::getMission() {
    return this->mission;
}

void Agent::setMission(Mission *mission) {
    this->mission = mission;
}

int Agent::getSellModIndex() {
    return this->sellModIndex;
}

void Agent::setStationName(String src) {
    this->stationName = src;
}

String Agent::getMissionString() {
    return this->missionString;
}

uint8_t Agent::isMale() {
    return this->male;
}

String Agent::getName() {
    return this->name;
}

static const int kModPriceTable[4] = {0, 0, 0, 0};

int Agent::getModPricePercentage() {
    uint32_t i = this->sellModIndex;
    if (i < 4)
        return kModPriceTable[i];
    return 0x28;
}

void Agent::setOfferAccepted(bool v) {
    this->offerAccepted = v;
}

void Agent::nextEvent() {
    this->eventCount = this->eventCount + 1;
}

uint8_t Agent::hasReward() {
    return this->rewardAtNextChat;
}

void Agent::setImageParts(int *parts) {
    this->imageParts = parts;
}

bool Agent::isKnown() {
    return this->eventCount > 0;
}

bool Agent::isGenericAgent() {
    return this->category == 1;
}

int Agent::getType() {
    return this->category;
}

String Agent::getSystemName() {
    return this->systemName;
}

void Agent::giveRewardAtNextChat(bool v) {
    this->rewardAtNextChat = v;
}

Triple *Agent::setSellItemData(int index, int quantity, int price) {
    this->sellItemIndex = index;
    this->sellItemQuantity = quantity;
    this->sellItemPrice = price;
    return reinterpret_cast<Triple *>(&this->sellItemIndex);
}

void Agent::setWingmanFriendNames(Array<AbyssEngine::String *> *param) {
    delete this->wingman1;
    this->wingman1 = nullptr;
    delete this->wingman2;
    this->wingman2 = nullptr;
    delete this->wingmanNames;

    this->wingmanNames = new Array<AbyssEngine::String *>();
    ArrayAdd(new String(this->name), *(this->wingmanNames));
    this->wingmanCount = 0;
    if (param == nullptr)
        return;

    if (param->size() != 0) {
        String *w0 = (*param)[0];
        if (w0 != nullptr) {
            this->wingmanCount = 1;
            this->wingman1 = w0;
            ArrayAdd(w0, *(this->wingmanNames));
        }
        if (param->size() >= 2) {
            String *w1 = (*param)[1];
            if (w1 != nullptr) {
                this->wingman2 = w1;
                this->wingmanCount += 1;
                ArrayAdd(w1, *(this->wingmanNames));
            }
        }
    }

    delete param;
}

String Agent::getWingmanName(int idx) {
    if (idx == 1)
        return *this->wingman1;
    if (idx == 0)
        return this->name;
    return *this->wingman2;
}

void Agent::setSystemName(String src) {
    this->systemName = src;
}

Agent::~Agent() noexcept(false) {
    delete[] this->imageParts;
    this->imageParts = nullptr;
    delete this->wingman1;
    this->wingman1 = nullptr;
}

void Agent::setMissionString(String src) {
    this->missionString = src;
}

int Agent::getStation() {
    return this->station;
}

int Agent::getSystem() {
    return this->system;
}

int Agent::getRace() {
    return this->race;
}

int Agent::getIndex() {
    return (int) this->type;
}

int Agent::getCosts() {
    return this->costs;
}

void Agent::setCosts(int costs) {
    this->costs = costs;
}

int Agent::getOffer() {
    return this->offer;
}

void Agent::setOffer(int offer) {
    this->offer = offer;
}

int Agent::getEvent() {
    return this->eventCount;
}

void Agent::setEvent(int event) {
    this->eventCount = event;
}

int Agent::getSellItemIndex() {
    return this->sellItemIndex;
}

int Agent::getSellItemQuantity() {
    return this->sellItemQuantity;
}

int Agent::getSellItemPrice() {
    return this->sellItemPrice;
}

void Agent::setSellItemPrice(int price) {
    this->sellItemPrice = price;
}

int Agent::getSellSystemIndex() {
    return this->sellSystemIndex;
}

int Agent::getSellBlueprintIndex() {
    return this->sellBlueprintIndex;
}

int Agent::getWingmanFriendsCount() {
    return this->wingmanCount;
}

Agent::Agent(int kind, String name, int station, int system, int race,
             bool male, int sellSystemIndex, int sellBlueprintIndex, int sellModIndex,
             int sellItemPrice) {
    this->type = kind;
    this->name = name;
    this->station = station;
    this->system = system;
    this->race = race;
    this->male = male;
    this->eventCount = 0;
    this->field_0x30 = -1;
    this->offer = -1;
    this->sellSystemIndex = sellSystemIndex;
    if (sellSystemIndex >= 0)
        this->offer = 4;
    this->sellBlueprintIndex = sellBlueprintIndex;
    if (sellBlueprintIndex >= 0)
        this->offer = 3;
    this->offerAccepted = 0;
    this->wasAskedForDifficulty = 0;
    this->wingman1 = nullptr;
    this->wingman2 = nullptr;
    this->wingmanCount = 0;
    this->sellItemIndex = 0;
    this->sellItemQuantity = 0;
    this->sellItemPrice = sellItemPrice;
    this->costs = 0;
    this->imageParts = nullptr;
    this->mission = nullptr;
    this->wingmanNames = nullptr;
    this->field_0x28 = -1;
    this->field_0x2c = -1;
    this->category = (unsigned) kind >> 31;
    if (sellModIndex >= 0)
        this->offer = 8;
    if (kind == 0x19)
        this->offer = 9;
    else if (kind == 0x1a)
        this->offer = 10;
    this->sellModIndex = sellModIndex;
}
