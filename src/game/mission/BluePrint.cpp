#include "game/mission/BluePrint.h"
#include "game/world/Galaxy.h"
#include "game/mission/Item.h"
#include "game/mission/Status.h"
#include "game/world/Station.h"
#include "game/core/String.h"

int BluePrint::getAutoCompletionPrice() {
    if (itemIndex == 0xd2)
        return getIngredientsValue() + 0x1e8480;
    int maxPrice = (*Item::g_items)[itemIndex]->getMaxPrice();
    return (int) ((float) (batchMultiplier * maxPrice) * 1.25f);
}

int BluePrint::getRemainingAmount(int item) {
    Array<int> *il = getIngredientList();
    for (uint32_t i = 0; i < ingredientCounters->size(); i++) {
        if ((*il)[i] == item)
            return (*ingredientCounters)[i];
    }
    return 0;
}

bool BluePrint::isEmpty() {
    return spentValue == 0;
}

String BluePrint::getStationName() {
    return stationName;
}

BluePrint::~BluePrint() {
    delete ingredientCounters;
    ingredientCounters = nullptr;
}

void BluePrint::complete() {
    for (uint32_t i = 0; i < ingredientCounters->size(); i++)
        (*ingredientCounters)[i] = 0;
}

int BluePrint::getCurrentAmount(int item) {
    return getTotalAmount(item) - getRemainingAmount(item);
}

bool BluePrint::isCompleted() {
    for (uint32_t i = 0; i < ingredientCounters->size(); i++)
        if ((*ingredientCounters)[i] >= 1)
            return false;
    return true;
}

int BluePrint::getIndex() { return itemIndex; }

int BluePrint::getStationIndex() { return stationIndex; }

int BluePrint::getQuantity() { return remainingBatch; }

bool BluePrint::isUnlocked() { return locked != 0; }

void BluePrint::unlock() {
    locked = 1;
}

void BluePrint::reset() {
    productionCount += 1;
    Status::gStatus->incGoodsProduced(1);
    Array<int> *ql = getQuantityList();
    for (uint32_t i = 0; i < ingredientCounters->size(); i++)
        (*ingredientCounters)[i] = (*ql)[i];
    stationIndex = -1;
    remainingBatch = batchMultiplier;
    spentValue = 0;
}

void BluePrint::lock() {
    locked = 1;
}

int BluePrint::getTotalAmount(int item) {
    Array<int> *il = getIngredientList();
    Array<int> *ql = getQuantityList();
    for (uint32_t i = 0; i < ql->size(); i++) {
        if ((*il)[i] == item)
            return (*ql)[i];
    }
    return 0;
}

Array<int> *BluePrint::getIngredientList() {
    return (Array<int> *) (*Item::g_items)[itemIndex]->getIngredients();
}

Array<int> *BluePrint::getQuantityList() {
    return (Array<int> *) (*Item::g_items)[itemIndex]->getQuantities();
}

int BluePrint::getIngredientsValue() {
    Array<int> *il = getIngredientList();
    int total = 0;
    if (il != nullptr) {
        for (uint32_t i = 0; i < il->size(); i++) {
            int price = (*Item::g_items)[(*il)[i]]->getSinglePrice();
            total += (*ingredientCounters)[i] * price;
        }
    }
    return total;
}

BluePrint::BluePrint(int item) {
    itemIndex = item;
    Item *it = (*Item::g_items)[item];
    int type = it->getType();
    stationIndex = -1;
    batchMultiplier = (type == 1) ? 10 : 1;
    Array<int> *quantities = (Array<int> *) it->getQuantities();
    ingredientCounters = nullptr;
    if (it->getIngredients() != nullptr) {
        ingredientCounters = new Array<int>();
        ArraySetLength(it->getIngredients()->size(), *ingredientCounters);
        for (uint32_t i = 0; i < ingredientCounters->size(); i++)
            (*ingredientCounters)[i] = (*quantities)[i];
    }
    productionCount = 0;
    locked = 0;
    spentValue = 0;
    remainingBatch = batchMultiplier;
}

int BluePrint::getBaseQuantity() { return batchMultiplier; }

int BluePrint::getMoneySpent() { return spentValue; }
void BluePrint::setMoneySpent(int value) { spentValue = value; }

void BluePrint::addItem(Item *item, int amount, int station) {
    if (amount == 0)
        return;
    item->setBlueprintAmount(0);
    Array<int> *il = getIngredientList();
    if (il == nullptr)
        return;
    for (uint32_t i = 0; i < il->size(); i++) {
        if ((*il)[i] != item->getIndex())
            continue;
        (*ingredientCounters)[i] -= amount;
        spentValue += item->getSinglePrice() * amount;
        if (station >= 0 && stationIndex < 0) {
            stationIndex = station;
            Station *current = Status::gStatus->getStation();
            if (current->getIndex() == station) {
                stationName = current->getName();
            } else {
                Station *st = (Station *) (intptr_t) Galaxy::gGalaxy->getStation(station);
                stationName = st->getName();
                if (st != nullptr)
                    delete st;
            }
        }
        break;
    }
}

float BluePrint::getCompletionRate() {
    Array<int> *quantity = getQuantityList();
    float rate = 0.0f;
    for (uint32_t i = 0; i < quantity->size(); i++) {
        int target = (*quantity)[i];
        int remaining = (*ingredientCounters)[i];
        float frac = (float) (target - remaining) / (float) target;
        rate += frac / (float) ingredientCounters->size();
    }
    return rate;
}
