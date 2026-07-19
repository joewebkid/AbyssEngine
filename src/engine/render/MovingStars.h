#ifndef GOF2_MOVINGSTARS_H
#define GOF2_MOVINGSTARS_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Matrix.h"
#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"


class MovingStars {
public:
    uint32_t *billboardIds;
    uint32_t *transformHandles;
    uint32_t textureHandle;
    int *lifeArray;
    int *velocityArray;
    uint8_t animResetFlag;
    uint8_t animActiveFlag;
    char pad[2];
    uint32_t tickAccumulator;

    MovingStars();

    ~MovingStars();

    void update(int param1, AbyssEngine::AEMath::Matrix m, bool flag, float param19);

    void translate(const AbyssEngine::AEMath::Vector &v);

    void render();
};
#endif
