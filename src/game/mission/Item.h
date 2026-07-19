#ifndef GOF2_ITEM_H
#define GOF2_ITEM_H
#include "engine/core/Array.h"


#include "game/mission/ItemTable.h"
class Station;



class Item {
public:
    int index;
    int type;
    int sort;
    int tecLevel;
    int minPriceSystem;
    int maxPriceSystem;
    int price;
    int occurence;
    int minPrice;
    int maxPrice;
    Array<int> *ingredients;
    Array<int> *quantities;
    Array<int> *attributes;
    int amount;
    int stationAmount;
    int blueprintAmount;
    int missingIngredients;
    bool unsaleable;

    Item(Array<int> *ingredients, Array<int> *quantities, Array<int> *attributes);

    ~Item();

    void init();

    void setUnsaleable(bool value);

    bool canBeInstalledMultipleTimes();

    int getIndex();

    int getType();

    int getTecLevel();

    int getSort();

    int getSinglePrice();

    int getTotalPrice();

    int getMaxPrice();

    int getMinPrice();

    int getMaxPriceSystem();

    int getMinPriceSystem();

    void setPrice(int value);

    void setMinPrice(int value);

    void setMaxPrice(int value);

    float getPriceRate();

    void setAmount(int value);

    int getOccurence();

    int getAmount();

    void changeAmount(int delta);

    int getMissingIngredients();

    void setMissingIngredients(int value);

    void setStationAmount(int value);

    int getStationAmount();

    void changeStationAmount(int delta);

    void setBlueprintAmount(int value);

    int getBlueprintAmount();

    void changeBlueprintAmount(int delta);

    Array<int> *getIngredients();

    Array<int> *getQuantities();

    Array<int> *getAttributes();

    int getAttribute(int attribute);

    int transaction(bool buy, int priceAdjustment, bool useCredits);

    int transactionBlueprint(bool fabricate, int mode);

    bool equals(Item *other);

    bool isWeapon();

    Item *makeItem();

    Item *makeItem(int amount);

    Item *makeItem(int amount, int price);

    Item *clone();

    bool checkCredits();

    void adjustPrice(Station *station);

    bool checkCargoSpace();

    bool isUnsaleable();

    static bool isInList(int index, int amount, Array<Item*> *items);

    static bool isInList(int index, Array<Item*> *items);

    static bool isInList(Item *item, Array<Item*> *items);

    static void fabricate(Item *item, Array<Item*> *items, int amount);

    static Array<Item*> *combineItems(Array<Item*> *items, Array<Item*> *stationItems);

    static Array<Item*> *extractItems(Array<Item*> *items, bool station);

    static void combineDuplicates(Array<Item*> *items);

    static Array<Item*> *mixItems(Array<Item*> *items, Array<Item*> *stationItems);

    static Array<Item *> *g_items;
};

typedef Array<int> IntArray;
typedef Array<Item*> ItemArray;

#endif
