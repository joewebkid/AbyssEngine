#ifndef GOF2_MISSION_H
#define GOF2_MISSION_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
class Agent;


using AbyssEngine::String;

class Mission {
public:
    uint8_t failed;
    uint8_t won;
    Agent *agent;
    int id;
    String name;
    String targetName;
    int *clientImage;
    int clientRace;
    int costs;
    int costsValue;
    int bonus;
    int targetStation;
    String targetStationName;
    String targetSystemName;
    int reward;
    uint8_t instantAction;
    int distance;
    int campaign;
    int productionGoodsA;
    int productionGoodsB;
    int statusValue;
    uint8_t visible;

    Mission();

    Mission(int id);

    Mission(int id, int goods, int station);

    Mission(int id, AbyssEngine::String client, int *clientImage, int clientRace,
            int costs, int station, int reward);

    virtual ~Mission();

    void calcDistance();

    Mission *clone();

    String getClientName();

    String getDescription();

    String getName();

    String getTargetName();

    String getTargetStationName();

    String getTargetSystemName();

    bool isCampaignMission();

    bool isEmpty();

    uint8_t isInstantActionMission();

    uint8_t isVisible();

    void setInstantActionMission(bool v);

    void setProductionGoods(int a, int b);

    void *setTargetName(String rhs);

    void setTargetStation(int idx);

    void *setTargetSystemName(const String &rhs);

    void setVisible(bool v);

    int getType();

    void setType(int type);

    int getReward();

    void setReward(int reward);

    int getCosts();

    void setCosts(int costs);

    int getBonus();

    void setBonus(int bonus);

    int getDifficulty();

    void setDifficulty(int difficulty);

    int getDistance();

    int getStatusValue();

    void setStatusValue(int value);

    Agent *getAgent();

    void setAgent(Agent *agent);

    int *getClientImage();

    int getClientRace();

    int getProductionGoodIndex();

    int getProductionGoodAmount();

    int getTargetStation();

    bool hasFailed();

    void setFailed(bool failed);

    bool hasWon();

    void setWon(bool won);

    void setCampaignMission(bool flag);

    bool isOutsideMission();

    // Static data members present in the original binary (defined for symbol parity).
    static void *empty;
};
#endif
