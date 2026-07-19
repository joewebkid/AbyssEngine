#include "game/mission/Mission.h"
#include "game/world/Galaxy.h"
#include "game/world/SolarSystem.h"
#include "game/mission/Status.h"
#include "engine/core/GameText.h"
#include "game/world/Station.h"


static GameText **g_gameText;

uint8_t Mission::isInstantActionMission() {
    return this->instantAction;
}

String Mission::getTargetStationName() {
    return this->targetStationName;
}

bool Mission::isCampaignMission() {
    return this->campaign != 0;
}

String Mission::getDescription() {
    return String("");
}

String Mission::getTargetSystemName() {
    return this->targetSystemName;
}

String Mission::getName() {
    if (this->campaign != 0) {
        return String("");
    }
    return *(*g_gameText)->getText(this->id + 0x162);
}

void Mission::setProductionGoods(int a, int b) {
    this->productionGoodsA = a;
    this->productionGoodsB = b;
}

void *Mission::setTargetName(String rhs) {
    this->targetName = rhs;
    return &this->targetName;
}

String Mission::getClientName() {
    return this->name;
}

uint8_t Mission::isVisible() {
    return this->visible;
}

bool Mission::isEmpty() {
    return this->id == -1;
}

String Mission::getTargetName() {
    return this->targetName;
}

void Mission::setInstantActionMission(bool v) {
    this->instantAction = v;
}

void *Mission::setTargetSystemName(const String &rhs) {
    this->targetSystemName = rhs;
    return &this->targetSystemName;
}

void Mission::setVisible(bool v) {
    this->visible = v;
}

Mission *Mission::clone() {
    Mission *m = new Mission(this->id, this->name, this->clientImage,
                             this->clientRace, this->costs, this->targetStation,
                             this->reward);
    m->instantAction = this->instantAction;
    return m;
}

void Mission::setTargetStation(int idx) {
    this->targetStation = idx;
    Station *st = (Station *) (long) Galaxy::gGalaxy->getStation(idx);
    this->targetStationName = st->getName();
}

Mission::Mission(int id, AbyssEngine::String client, int *clientImage, int clientRace,
                 int costs, int station, int reward) {
    this->id = id;
    this->name = client;
    this->clientImage = clientImage;
    this->clientRace = clientRace;
    this->costs = costs;
    this->targetStation = station;
    Station *st = (Station *) (long) Galaxy::gGalaxy->getStation(station);
    this->targetStationName = st->getName();
    this->reward = reward;
    this->targetSystemName = String("");
    this->visible = 1;
    this->campaign = 0;
    this->instantAction = 0;
    this->failed = 0;
    this->won = 0;
    this->agent = 0;
    this->bonus = 0;
}

Mission::Mission(int id) {
    this->name = String("");
    this->campaign = 0;
    this->visible = 0;
    this->agent = 0;
    this->id = id;
    this->clientImage = 0;
    this->clientRace = 0;
    this->costs = 0;
    this->reward = 0;
    this->instantAction = 0;
    this->statusValue = 0;
    this->failed = 0;
    this->won = 0;
    this->bonus = 0;
    this->targetStation = 0;
}

void Mission::calcDistance() {
    Galaxy *g = Galaxy::gGalaxy;
    Station *st = (Station *) (long) g->getStation(this->targetStation);
    Array<SolarSystem *> *sys = g->getSystems();
    SolarSystem *a = (*sys)[Status::gStatus->getStation()->getSystem()];
    SolarSystem *b = (*sys)[st->getSystem()];
    this->distance = (int) g->distance(a, b);
    delete st;
}

Mission::Mission(int id, int goods, int station) {
    this->costs = goods;
    this->targetStation = station;
    this->clientImage = 0;
    this->agent = 0;
    this->id = id;
    if (station < 0) {
        this->targetStationName = String("");
    } else {
        Station *st = (Station *) (long) Galaxy::gGalaxy->getStation(station);
        this->targetStationName = st->getName();
    }
    this->targetSystemName = String("");
    this->visible = 1;
    this->campaign = 1;
    this->failed = 0;
    this->won = 0;
    this->instantAction = 0;
    this->statusValue = 0;
    this->bonus = 0;
}

Mission::~Mission() {
}

Mission::Mission() {
    this->name = String("");
    this->campaign = 0;
    this->visible = 0;
    this->agent = 0;
    this->id = -1;
    this->clientImage = 0;
    this->clientRace = 0;
    this->costs = 0;
    this->reward = 0;
    this->instantAction = 0;
    this->statusValue = 0;
    this->failed = 0;
    this->won = 0;
    this->bonus = 0;
    this->targetStation = 0;
}

int Mission::getType() {
    return this->id;
}

int Mission::getReward() {
    return this->costs;
}

void Mission::setReward(int reward) {
    this->costs = reward;
}

int Mission::getCosts() {
    return this->costs;
}

void Mission::setCosts(int costs) {
    this->costsValue = costs;
}

int Mission::getBonus() {
    return this->bonus;
}

void Mission::setBonus(int bonus) {
    this->bonus = bonus;
}

int Mission::getDifficulty() {
    return this->reward;
}

int Mission::getDistance() {
    return this->distance;
}

int Mission::getStatusValue() {
    return this->statusValue;
}

void Mission::setStatusValue(int value) {
    this->statusValue = value;
}

Agent *Mission::getAgent() {
    return this->agent;
}

void Mission::setAgent(Agent *agent) {
    this->agent = agent;
}

int *Mission::getClientImage() {
    return this->clientImage;
}

int Mission::getClientRace() {
    return this->clientRace;
}

int Mission::getProductionGoodIndex() {
    return this->productionGoodsA;
}

int Mission::getProductionGoodAmount() {
    return this->productionGoodsB;
}

int Mission::getTargetStation() {
    return this->targetStation;
}

bool Mission::hasFailed() {
    return this->failed != 0;
}

void Mission::setFailed(bool failed) {
    this->failed = failed ? 1 : 0;
}

bool Mission::hasWon() {
    return this->won != 0;
}

void Mission::setWon(bool won) {
    this->won = won ? 1 : 0;
}

void Mission::setCampaignMission(bool flag) {
    this->campaign = flag ? 1 : 0;
}

bool Mission::isOutsideMission() {
    return true;
}

void Mission::setType(int type) {
    this->id = type;
}

void Mission::setDifficulty(int difficulty) {
    this->reward = difficulty;
}

// Static data members present in the original binary (defined for symbol parity).
void *Mission::empty;
