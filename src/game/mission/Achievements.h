#ifndef GOF2_ACHIEVEMENTS_H
#define GOF2_ACHIEVEMENTS_H
#include <cstdint>

#include "game/ship/PlayerEgo.h"

class PlayerEgo;


class Achievements {
public:
    int *medals;
    int *newMedals;
    int kills;
    int catches;
    int pirateKills;
    int weaponCount;
    uint8_t hasTurretAndWeapon;
    int credits;
    uint8_t gotAllMedals_;
    uint8_t gotAllGoldMedals_;
    uint8_t gotAllSupernovaMedals_;
    int medalCount;

    Achievements();

    ~Achievements();

    void applyNewMedals();

    void checkForNewMedal(PlayerEgo *ego);

    void countMedals();

    int getCatches();

    int getKills();

    int getPirateKills();

    int getValue(int index, int sub);

    uint8_t gotAllGoldMedals();

    uint8_t gotAllMedals();

    uint8_t gotAllSupernovaMedals();

    int *getMedals();

    int *getNewMedals();

    uint8_t hasMedal(int index, int value);

    void incCatches();

    void incKills();

    void incPirateKills();

    int init();

    void initCheckEquipmentAndWeapons();

    uint8_t isEliteMedal(int index);

    void resetNewMedals();

    void resetPirateKills();

    void setMedal(int index, int value);

    void setMedals(int *src, int count);

    void updateCredits(int value);

    static Achievements *gAchievements;
};

#endif
