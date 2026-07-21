#ifndef GOF2_HANGARLIST_H
#define GOF2_HANGARLIST_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/core/GameText.h"
#include "game/mission/BluePrint.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/ship/Ship.h"
#include "game/ui/ListItem.h"

class BluePrint;
class GameText;
class Item;
class ListItem;
class Ship;
class Status;


class HangarList {
public:
    Array<Array<ListItem *> *> *tabs;
    uint32_t currentTab;
    // Android HD stores the selected row as an int at HangarList+0x08, not as
    // a ListItem pointer. Keeping the native representation prevents tab and
    // selection state from aliasing pointer bits on 64-bit host builds.
    int currentItemIndex;

    HangarList();

    ~HangarList();

    void release();

    void setCurrentTab(int tab, bool blueprintIngredients);

    void setCurrentItemIndex(int index);

    uint32_t getCurrentItemIndex();

    uint32_t getCurrentTab();

    Array<Array<ListItem *> *> *getItems();

    uint32_t getCurrentLength();

    Array<ListItem *> *getCurrentTabItems();

    ListItem *getCurrentItem();

    ListItem *getCurrentItemAt(int index);

    void initBlueprintTab(Array<BluePrint *> *blueprints);

    void fillIngredientsList(BluePrint *blueprint, bool flag);

    void fillBuyList(ListItem *item);

    void initShipTab(Ship *ship);

    void initShopTab(Array<Item *> *shopItems, Array<Ship *> *ships);

    int init(Ship *ship, Array<Item *> *items, Array<Ship *> *ships,
             Array<BluePrint *> *blueprints);
};
#endif
