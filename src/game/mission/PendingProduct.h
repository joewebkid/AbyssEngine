#ifndef GOF2_PENDINGPRODUCT_H
#define GOF2_PENDINGPRODUCT_H
#include "BluePrint.h"
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
class BluePrint;


class PendingProduct {
public:
    String stationName;
    int stationIndex;
    int quantity;
    int blueprintIndex;

    explicit PendingProduct(BluePrint *bp);

    PendingProduct(int blueprintIndex, String stationName,
                   int stationIndex, int quantity);

    ~PendingProduct();
};
#endif
