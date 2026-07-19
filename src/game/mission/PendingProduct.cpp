#include "game/mission/PendingProduct.h"
#include "game/mission/BluePrint.h"

PendingProduct::PendingProduct(int blueprintIndex, String stationName,
                               int stationIndex, int quantity)
    : stationName(stationName),
      stationIndex(stationIndex),
      quantity(quantity),
      blueprintIndex(blueprintIndex) {
}

PendingProduct::PendingProduct(BluePrint *bp)
    : stationName(bp->getStationName()),
      stationIndex(bp->getStationIndex()),
      quantity(bp->getQuantity()),
      blueprintIndex(bp->getIndex()) {
}

PendingProduct::~PendingProduct() {
}
