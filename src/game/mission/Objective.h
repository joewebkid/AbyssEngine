#ifndef GOF2_OBJECTIVE_H
#define GOF2_OBJECTIVE_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
class Level;


class Objective {
public:
    int type;
    int value;
    int calcValue;
    Level *level;
    Array<Objective *> *children;
    AbyssEngine::String *achievedText;
    int storedValue;

    Objective(int type, int value, Level *level);

    Objective(int type, int value, int calcValue, Level *level);

    ~Objective();

    Objective *addObjective(Objective *objective);

    void setAchievedText(AbyssEngine::String *text);

    AbyssEngine::String *getAchievedText();

    bool isSurvivalObjective();

    bool getCalcValue();

    unsigned int achieved(int value);
};
#endif
