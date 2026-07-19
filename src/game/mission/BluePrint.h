#ifndef GOF2_BLUEPRINT_H
#define GOF2_BLUEPRINT_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "Item.h"

#include "game/core/String.h"

class Item;


class BluePrint {
public:
    Array<int> *ingredientCounters;
    int32_t spentValue;
    uint8_t locked;
    int32_t productionCount;
    int32_t stationIndex;
    AbyssEngine::String stationName;
    int32_t itemIndex;
    int32_t batchMultiplier;
    int32_t remainingBatch;

    explicit BluePrint(int item);

    ~BluePrint();

    void addItem(Item *item, int amount, int station);

    void complete();

    int getAutoCompletionPrice();

    float getCompletionRate();

    int getCurrentAmount(int item);

    int getIndex();

    int getStationIndex();

    int getQuantity();

    int getBaseQuantity();

    int getMoneySpent();

    void setMoneySpent(int value);

    bool isUnlocked();

    int getIngredientsValue();

    int getRemainingAmount(int item);

    AbyssEngine::String getStationName();

    int getTotalAmount(int item);

    bool isCompleted();

    bool isEmpty();

    void lock();

    void reset();

    void unlock();

private:
    Array<int> *getIngredientList();

    Array<int> *getQuantityList();
};
#endif
