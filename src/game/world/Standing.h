#ifndef GOF2_STANDING_H
#define GOF2_STANDING_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
class Standing {
public:
    int *standings;
    int currentRace;

    Standing();

    ~Standing();

    int *getStandings();

    void setStandings(int *arr);

    void applyDelict(int kind, int severity);

    void applyDisable(int race);

    void applyKill(int kind);

    void applyMissionCompleted(int race);

    void applyPoints(int race, int delta);

    void applyStealCargo(int race);

    uint32_t getEnemyRace(int idx);

    float getMissionBonus(int race);

    int getStanding(int race);

    float getStandingRate(int race);

    bool isEnemy(int race);

    bool isEnemyWithAnyone();

    bool isFriend(int race);

    unsigned isNeutral(int race);

    void rehabilitate(int race);

    void setPlayerSignatureRace(int race);

    void setStanding(int race, int value);
};
#endif
