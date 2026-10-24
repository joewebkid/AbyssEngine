#include "game/mission/PendingProduct.h"
#include "game/mission/BluePrint.h"

PendingProduct::PendingProduct(int blueprintIndex, String stationName,
                               int stationIndex, int quantity) {
    this->blueprintIndex = blueprintIndex;
    this->stationName = stationName;
    this->stationIndex = stationIndex;
    this->quantity = quantity;
}

PendingProduct::PendingProduct(BluePrint *bp) {
    blueprintIndex = bp->getIndex();
    stationName = bp->getStationName();
    stationIndex = bp->getStationIndex();
    quantity = bp->getQuantity();
}

PendingProduct::~PendingProduct() {
}
