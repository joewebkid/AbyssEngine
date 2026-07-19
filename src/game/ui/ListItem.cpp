#include "game/ui/ListItem.h"
#include "game/mission/BluePrint.h"
#include "game/mission/Item.h"
#include "game/mission/Mission.h"
#include "game/mission/PendingProduct.h"
#include "game/mission/Status.h"
#include "game/ship/Agent.h"
#include "game/ship/Ship.h"

bool ListItem::isMission() {
    return this->mission != 0;
}

bool ListItem::isItem() {
    return this->item != 0;
}

ListItem::ListItem(BluePrint *bp) {
    this->init();
    this->selectable = 1;
    this->bluePrint = bp;
}

int ListItem::checkSort() {
    if (this->item == 0)
        return 0;
    Ship *ship = Status::gStatus->getShip();
    int sort = this->item->getSort();
    return ship->slotAvailable(sort);
}

ListItem::ListItem(int a, int b) {
    this->init();
    this->slot = b;
    this->itemId = a;
    this->selectable = 1;
}

ListItem::~ListItem() {
    delete this->name;
    this->name = 0;
    delete this->name2;
    this->name2 = 0;
}

bool ListItem::isCargo() {
    return this->item != 0;
}

int ListItem::getNumLines() {
    if (this->text == 0)
        return 0;
    return (int) this->lines->size();
}

bool ListItem::isBluePrint() {
    return this->bluePrint != 0;
}

uint8_t ListItem::isSelectable() {
    return this->selectable;
}

ListItem::ListItem(Ship *s) {
    this->init();
    this->selectable = 1;
    this->ship = s;
}

int ListItem::checkSlot() {
    int r = 0;
    if (this->item != 0) {
        Ship *ship = Status::gStatus->getShip();
        int type = this->item->getType();
        if (ship->getFreeSlots(type) > 0)
            r = 1;
    }
    return r;
}

ListItem::ListItem(AbyssEngine::String *src, int v) {
    this->init();
    this->name = new AbyssEngine::String(*src);
    this->buttonKind = v;
    this->selectable = 0;
}

ListItem::ListItem(int v) {
    this->init();
    this->selectable = 1;
    this->slot = v;
}

ListItem::ListItem(AbyssEngine::String *p1, AbyssEngine::String *p2) {
    this->init();
    this->name = new AbyssEngine::String(*p1);
    this->name2 = new AbyssEngine::String(*p2);
    this->selectable = 0;
}

bool ListItem::isImage() {
    return this->imageIndex >= 0;
}

ListItem::ListItem(int a, int b, AbyssEngine::String *src) {
    this->init();
    this->imageIndex = a;
    this->name = new AbyssEngine::String(*src);
    this->slot = b;
    this->selectable = 1;
}

int ListItem::getIndex() {
    if (this->ship)
        return this->ship->getIndex();
    if (this->item)
        return this->item->getIndex();
    if (this->bluePrint)
        return this->bluePrint->getIndex();
    if (this->pendingProduct == 0)
        return 999999;
    return this->pendingProduct->blueprintIndex;
}

ListItem::ListItem(Item *it) {
    this->init();
    this->selectable = 1;
    this->item = it;
}

ListItem::ListItem(Array<AbyssEngine::String *> *arr) {
    this->init();
    this->selectable = 0;
    this->lines = arr;
}

ListItem::ListItem(Mission *m) {
    this->init();
    this->selectable = 1;
    this->mission = m;
}

uint8_t ListItem::isTextButton() {
    return this->textButton;
}

uint8_t ListItem::isText() {
    return this->text;
}

bool ListItem::checkCredits() {
    int price;
    if (this->ship == 0) {
        if (this->item == 0)
            return false;
        price = this->item->getSinglePrice();
    } else {
        price = this->ship->getPrice();
    }
    return price <= Status::gStatus->getCredits();
}

ListItem::ListItem(AbyssEngine::String *p1, bool b) {
    this->init();
    this->name = new AbyssEngine::String(*p1);
    this->text = b;
    this->selectable = 0;
}

int ListItem::getPrice() {
    if (this->ship)
        return this->ship->getPrice();
    if (this->item == 0)
        return 0;
    return this->item->getSinglePrice();
}

bool ListItem::isSlot() {
    return this->slot >= 0;
}

bool ListItem::isMoveToCargoButton() {
    if (this->textButton == 0)
        return false;
    return this->buttonKind == 1;
}

bool ListItem::isPendingProduct() {
    return this->pendingProduct != 0;
}

bool ListItem::isTab() {
    if (this->selectable == 0 && this->name->size() != 0)
        return this->textButton == 0;
    return false;
}

bool ListItem::isShip() {
    return this->ship != 0;
}

bool ListItem::isSellButton() {
    if (this->textButton == 0)
        return false;
    return this->buttonKind == 0;
}

ListItem::ListItem(AbyssEngine::String *src) {
    this->init();
    this->name = new AbyssEngine::String(*src);
    this->selectable = 0;
}

ListItem::ListItem(PendingProduct *pp) {
    this->init();
    this->selectable = 1;
    this->pendingProduct = pp;
}

ListItem::ListItem(Agent *a) {
    this->init();
    this->selectable = 1;
    this->agent = a;
}

ListItem::ListItem(AbyssEngine::String *src, bool b, int v) {
    this->init();
    this->name = new AbyssEngine::String(*src);
    this->textButton = 1;
    this->buttonKind = v;
    this->selectable = b;
}

void *ListItem::init() {
    this->lines = 0;
    this->agent = 0;
    this->bluePrint = 0;
    this->ship = 0;
    this->item = 0;
    this->mission = 0;
    this->pendingProduct = 0;
    this->name = 0;
    this->name2 = 0;
    this->selectable = 0;
    this->slot = -1;
    this->itemId = -1;
    this->buttonKind = -1;
    this->imageIndex = -1;
    this->textButton = 0;
    this->inTabIndex = -1;
    this->subTabIndex = -1;
    this->text = 0;
    return &this->agent;
}

ListItem::ListItem(ListItem *src) {
    this->init();
    this->agent = src->agent;
    this->bluePrint = src->bluePrint;
    this->ship = src->ship;
    this->item = src->item;
    this->mission = src->mission;
    this->selectable = src->selectable;

    this->name = src->name ? new AbyssEngine::String(*src->name) : 0;
    this->name2 = src->name2 ? new AbyssEngine::String(*src->name2) : 0;

    this->slot = src->slot;
    this->itemId = src->itemId;
    this->buttonKind = src->buttonKind;
    this->imageIndex = src->imageIndex;
    this->textButton = src->textButton;
    this->inTabIndex = src->inTabIndex;
    this->subTabIndex = src->subTabIndex;
    this->pendingProduct = src->pendingProduct;
}
