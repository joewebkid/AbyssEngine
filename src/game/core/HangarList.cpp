#include "game/core/HangarList.h"
#include "game/mission/BluePrint.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/world/Station.h"
#include "engine/core/GameText.h"
#include "game/mission/PendingProduct.h"
#include "game/ui/ListItem.h"
#include "game/ship/Ship.h"

HangarList::HangarList() {
    this->tabs = nullptr;
    this->currentTab = 0;
    this->currentItemIndex = 0;
}

HangarList::~HangarList() {
    release();
}

void HangarList::setCurrentTab(int tab, bool blueprintIngredients) {
    (void)blueprintIngredients;
    this->currentTab = static_cast<uint32_t>(tab);
}

void HangarList::setCurrentItemIndex(int index) {
    this->currentItemIndex = index;
}

uint32_t HangarList::getCurrentItemIndex() {
    return static_cast<uint32_t>(this->currentItemIndex);
}

uint32_t HangarList::getCurrentTab() {
    return this->currentTab;
}

Array<Array<ListItem *> *> *HangarList::getItems() {
    return this->tabs;
}

uint32_t HangarList::getCurrentLength() {
    Array<ListItem *> *items = (*this->tabs)[this->currentTab];
    return items != nullptr ? items->size() : 0;
}

Array<ListItem *> *HangarList::getCurrentTabItems() {
    return (*this->tabs)[this->currentTab];
}

void HangarList::release() {
    Array<Array<ListItem *> *> *tabs = this->tabs;
    if (tabs != nullptr) {
        for (uint32_t i = 0; i < tabs->size(); ++i) {
            Array<ListItem *> *items = (*tabs)[i];
            if (items != nullptr) {
                ArrayReleaseClasses(*items);
                delete items;
            }
            (*tabs)[i] = nullptr;
        }
        ArrayRemoveAll(*tabs);
        delete tabs;
    }
    this->tabs = nullptr;
}

ListItem *HangarList::getCurrentItem() {
    return getCurrentItemAt(this->currentItemIndex);
}

ListItem *HangarList::getCurrentItemAt(int index) {
    Array<Array<ListItem *> *> *tabs = this->tabs;
    if (tabs != nullptr) {
        Array<ListItem *> *items = (*tabs)[this->currentTab];
        if (items != nullptr) {
            if (index < 0) {
                return nullptr;
            }
            if (static_cast<int>(items->size()) > index) {
                return (*items)[index];
            }
            return nullptr;
        }
    }
    return nullptr;
}

void HangarList::initBlueprintTab(Array<BluePrint *> *blueprints) {
    Array<ListItem *> *old = (*this->tabs)[2];
    if (old != nullptr) {
        ArrayReleaseClasses(*old);
        delete old;
    }
    (*this->tabs)[2] = nullptr;

    Array<ListItem *> *list = new Array<ListItem *>();

    uint32_t unlocked = 1;
    for (uint32_t i = 0; i < blueprints->size(); ++i) {
        unlocked += (*blueprints)[i]->isUnlocked();
    }

    Array<PendingProduct *> *pending = Status::gStatus->pendingProducts;
    uint32_t length = unlocked;
    bool noPending = true;
    if (pending != nullptr) {
        for (uint32_t i = 0; i < pending->size(); ++i) {
            if ((*pending)[i] != nullptr) {
                ++length;
            }
        }
        noPending = length <= unlocked;
        if (unlocked < length) {
            ++length;
        }
    }

    ArraySetLength(length, *list);

    (*list)[0] = new ListItem(GameText::gGameText->getText(0x111));

    uint32_t out = 1;
    for (uint32_t i = 0; i < blueprints->size(); ++i) {
        if ((*blueprints)[i]->isUnlocked()) {
            (*list)[out] = new ListItem((*blueprints)[i]);
            ++out;
        }
    }

    if (!noPending) {
        (*list)[out] = new ListItem(GameText::gGameText->getText(0x112));
        uint32_t pendingOut = out + 1;
        for (uint32_t i = 0; i < pending->size(); ++i) {
            if ((*pending)[i] != nullptr) {
                (*list)[pendingOut] = new ListItem((*pending)[i]);
                ++pendingOut;
            }
        }
    }

    (*this->tabs)[2] = list;
}

void HangarList::fillIngredientsList(BluePrint *blueprint, bool flag) {
    (void) flag;
    Ship *ship = Status::gStatus->getShip();
    Array<Item *> *cargo = ship->getCargo();
    Array<Item *> *allItems = Item::g_items;
    Array<int> *ingredients = blueprint->getIngredientList();
    uint32_t count = ingredients->size();

    Array<ListItem *> *list = new Array<ListItem *>();
    ArraySetLength(count + 1, *list);

    (*list)[0] = new ListItem(GameText::gGameText->getText(blueprint->getIndex() + 0x4fa));

    uint32_t out = 1;
    for (uint32_t i = 0; i < ingredients->size(); ++i) {
        Item *shown = nullptr;
        bool found = false;
        if (cargo != nullptr) {
            for (uint32_t j = 0; j < cargo->size(); ++j) {
                if ((*cargo)[j]->getIndex() == (*ingredients)[i]) {
                    (*list)[out] = new ListItem((*cargo)[j]);
                    shown = (*cargo)[j];
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            shown = (*allItems)[(*ingredients)[i]];
            (*list)[out] = new ListItem(shown);
        }
        shown->setBlueprintAmount(0);
        ++out;
    }

    (*this->tabs)[4] = list;
    ListItem *terminator = new ListItem(0);
    terminator->field_0x24 = 0;
    ArrayAdd(terminator, *(*this->tabs)[4]);
}

void HangarList::fillBuyList(ListItem *item) {
    Array<ListItem *> *old = (*this->tabs)[3];
    if (old != nullptr) {
        ArrayReleaseClasses(*old);
        delete old;
    }
    (*this->tabs)[3] = nullptr;

    uint32_t isShip = item->isShip();
    int type = item->field_0x28;
    int count = 0;
    if (isShip == 0) {
        if (!item->isSlot()) {
            type = item->item->getType();
        }
        Ship *ship = Status::gStatus->getShip();
        Array<Item *> *cargo = ship->getCargo();
        if (cargo != nullptr) {
            for (uint32_t i = 0; i < cargo->size(); ++i) {
                if ((*cargo)[i]->getType() == type) {
                    ++count;
                }
            }
        }
    } else {
        Station *station = Status::gStatus->getStation();
        Array<Ship *> *ships = station->getShips();
        if (ships != nullptr) {
            count = ships->size();
        }
    }

    uint32_t isSlot = item->isSlot();
    Array<ListItem *> *list = new Array<ListItem *>();
    int base = 3;
    uint32_t special = isShip | isSlot;
    if (count == 0) {
        base = 4;
    }
    uint32_t length = base + count + (special ^ 1);
    if (special == 0) {
        length += 2;
    }
    ArraySetLength(length, *list);

    (*list)[0] = new ListItem(GameText::gGameText->getText(0x115));
    (*list)[1] = new ListItem(item);

    uint32_t out;
    if (special == 0) {
        (*list)[2] = new ListItem(GameText::gGameText->getText(0x116));
        (*list)[3] = new ListItem(GameText::gGameText->getText(0x117), true, 0);
        (*list)[4] = new ListItem(GameText::gGameText->getText(0x11a), true, 1);
        out = 5;
    } else {
        out = 2;
    }

    (*list)[out] = new ListItem(GameText::gGameText->getText(isShip == 0 ? 0x11c : 0x11d));
    uint32_t next = out + 1;

    if (count < 1) {
        (*list)[next] = new ListItem(static_cast<int>(next));
    } else if (isShip == 0) {
        Ship *ship = Status::gStatus->getShip();
        Array<Item *> *cargo = ship->getCargo();
        for (uint32_t i = 0; i < cargo->size(); ++i) {
            if ((*cargo)[i]->getType() == type) {
                (*list)[next] = new ListItem((*cargo)[i]);
                ++next;
            }
        }
    } else {
        Station *station = Status::gStatus->getStation();
        Array<Ship *> *ships = station->getShips();
        for (uint32_t i = 0; i < ships->size(); ++i) {
            (*list)[out + 1 + i] = new ListItem((*ships)[i]);
        }
    }

    (*this->tabs)[3] = list;
}

void HangarList::initShipTab(Ship *ship) {
    Array<ListItem *> *old = (*this->tabs)[0];
    if (old != nullptr) {
        ArrayReleaseClasses(*old);
        delete old;
    }
    (*this->tabs)[0] = nullptr;

    Array<ListItem *> *items = new Array<ListItem *>();
    Array<Item *> *equipment = ship->getEquipment();
    Array<Item *> *cargo = ship->getCargo();
    Array<Item *> *available;
    int baseLength;
    bool cargoEmpty;
    if (cargo == nullptr) {
        baseLength = 2;
        available = nullptr;
        cargoEmpty = true;
    } else {
        available = new Array<Item *>();
        for (uint32_t i = 0; i < cargo->size(); ++i) {
            Item *item = (*cargo)[i];
            int type = item->getType();
            if (type != 4) {
                if (ship->getSlots(type) > 0) {
                    ArrayAdd(item, *available);
                }
            }
        }
        cargoEmpty = false;
        baseLength = available->size() + 2;
    }

    uint32_t equipmentLength = equipment->size();
    int slotTypes = ship->getSlotTypes();
    ArraySetLength(slotTypes + baseLength + equipmentLength, *items);

    (*items)[0] = new ListItem(GameText::gGameText->getText(0xb7));
    (*items)[1] = new ListItem(ship);
    uint32_t out = 2;

    for (uint32_t type = 0; type < 4; ++type) {
        if (ship->getSlots(type) > 0) {
            int textId = 0x10d;
            if (type < 3) {
                textId = type + 0x109;
            }
            AbyssEngine::String *slotLabel = GameText::gGameText->getText(textId);
            (*items)[out] = new ListItem(slotLabel, static_cast<int>(type));
            Array<Item *> *slotItems = ship->getEquipment(type);
            for (uint32_t j = 0; j < slotItems->size(); ++j) {
                ++out;
                Item *item = (*slotItems)[j];
                ListItem *li;
                if (item == nullptr) {
                    li = new ListItem(static_cast<int>(type));
                } else {
                    li = new ListItem(item);
                }
                (*items)[out] = li;
                for (uint32_t k = 0; k < equipment->size(); ++k) {
                    if ((*equipment)[k] == li->item) {
                        li->field_0x40 = k;
                        break;
                    }
                }
                li->field_0x3c = j;
            }
            delete slotItems;
            if (!cargoEmpty) {
                ++out;
                for (uint32_t j = 0; j < available->size(); ++j) {
                    if (static_cast<uint32_t>((*available)[j]->getType()) == type) {
                        (*items)[out] = new ListItem((*available)[j]);
                        ++out;
                    }
                }
            }
        }
    }
    (*this->tabs)[0] = items;
}

static int shopTypeTextId(uint32_t type) {
    int textId = 0x10e;
    if (type == 3) {
        textId = 0x10d;
    }
    if (type < 3) {
        textId = type + 0x109;
    }
    return textId;
}

void HangarList::initShopTab(Array<Item *> *shopItems, Array<Ship *> *ships) {
    int counts[5] = {};

    Array<ListItem *> *old = (*this->tabs)[1];
    if (old != nullptr) {
        ArrayReleaseClasses(*old);
        delete old;
    }
    (*this->tabs)[1] = nullptr;

    if ((shopItems == nullptr || shopItems->size() == 0) &&
        (ships == nullptr || ships->size() == 0)) {
        return;
    }

    Array<ListItem *> *list = new Array<ListItem *>();
    if (shopItems != nullptr) {
        for (uint32_t i = 0; i < shopItems->size(); ++i) {
            int type = (*shopItems)[i]->getType();
            counts[type] = counts[type] + 1;
        }
    }

    int length = 0;
    for (uint32_t i = 0; i < 5; ++i) {
        if (counts[i] > 0) {
            ++length;
        }
    }
    if (ships != nullptr && ships->size() != 0) {
        length += ships->size() + 1;
    }
    length += shopItems == nullptr ? 0 : shopItems->size();
    ArraySetLength(length, *list);

    uint32_t out;
    if (ships != nullptr && ships->size() != 0) {
        AbyssEngine::String *shipsLabel = GameText::gGameText->getText(0xad);
        (*list)[0] = new ListItem(shipsLabel, -1);
        for (uint32_t i = 0; i < ships->size(); ++i) {
            (*ships)[i]->adjustPrice();
            (*list)[i + 1] = new ListItem((*ships)[i]);
        }
        out = ships->size() + 1;
    } else {
        out = 0;
    }

    for (uint32_t type = 0; type < 5; ++type) {
        if (counts[type] > 0) {
            AbyssEngine::String *shopLabel = GameText::gGameText->getText(shopTypeTextId(type));
            (*list)[out] = new ListItem(shopLabel, static_cast<int>(type));
            ++out;
            if (shopItems != nullptr) {
                for (uint32_t i = 0; i < shopItems->size(); ++i) {
                    if (static_cast<uint32_t>((*shopItems)[i]->getType()) == type) {
                        (*list)[out] = new ListItem((*shopItems)[i]);
                        ++out;
                    }
                }
            }
        }
    }
    (*this->tabs)[1] = list;
}

int HangarList::init(Ship *ship, Array<Item *> *items, Array<Ship *> *ships,
                     Array<BluePrint *> *blueprints) {
    release();
    Array<Array<ListItem *> *> *tabs = new Array<Array<ListItem *> *>();
    this->tabs = tabs;
    ArraySetLength(5, *tabs);
    initShipTab(ship);
    initShopTab(items, ships);
    initBlueprintTab(blueprints);
    return 0;
}
