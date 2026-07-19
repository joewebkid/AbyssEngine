#ifndef GOF2_LISTITEM_H
#define GOF2_LISTITEM_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/mission/BluePrint.h"
#include "game/mission/Mission.h"
#include "game/mission/PendingProduct.h"
#include "game/ship/Agent.h"
#include "game/ship/Ship.h"

class Agent;
class BluePrint;
class Item;
class Mission;
class PendingProduct;
class Ship;


class ListItem {
public:
    Array<AbyssEngine::String *> *lines;
    Agent *agent;
    BluePrint *bluePrint;
    Ship *ship;

    union {
        Item *item;
        Item *field_0x10;
    };

    Mission *mission;
    PendingProduct *pendingProduct;
    AbyssEngine::String *name;
    AbyssEngine::String *name2;

    union {
        uint8_t selectable;
        uint8_t field_0x24;
    };

    union {
        int slot;
        int field_0x28;
    };

    union {
        int itemId;
        int field_0x2c;
    };

    union {
        int buttonKind;
        int field_0x30;
    };

    int imageIndex;
    uint8_t textButton;

    union {
        int inTabIndex;
        int field_0x3c;
    };

    union {
        int subTabIndex;
        int field_0x40;
    };

    uint8_t text;

    union {
        uint8_t craftable;
        uint8_t field_0x45;
    };

    ListItem(Agent *a);

    ListItem(Array<AbyssEngine::String *> *arr);

    ListItem(BluePrint *bp);

    ListItem(Item *it);

    ListItem(Mission *m);

    ListItem(PendingProduct *pp);

    ListItem(Ship *s);

    ListItem(AbyssEngine::String *src);

    ListItem(AbyssEngine::String *p1, AbyssEngine::String *p2);

    ListItem(AbyssEngine::String *p1, bool b);

    ListItem(AbyssEngine::String *src, bool b, int v);

    ListItem(AbyssEngine::String *src, int v);

    ListItem(ListItem *src);

    ListItem(int v);

    ListItem(int a, int b);

    ListItem(int a, int b, AbyssEngine::String *src);

    ~ListItem();

    bool checkCredits();

    int checkSlot();

    int checkSort();

    int getIndex();

    int getNumLines();

    int getPrice();

    void *init();

    bool isBluePrint();

    bool isCargo();

    bool isImage();

    bool isItem();

    bool isMission();

    bool isMoveToCargoButton();

    bool isPendingProduct();

    uint8_t isSelectable();

    bool isSellButton();

    bool isShip();

    bool isSlot();

    bool isTab();

    uint8_t isText();

    uint8_t isTextButton();
};
#endif
