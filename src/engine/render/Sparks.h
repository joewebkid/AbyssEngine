#ifndef GOF2_SPARKS_H
#define GOF2_SPARKS_H
#include "engine/core/Array.h"
#include "../core/AEString.h"
#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"


class Sparks {
public:
    int *lifetimeThresholds;
    uint32_t spriteSystem;
    uint32_t texture;
    int elapsed;
    uint8_t active;
    int kind;
    uint32_t count;
    int lifetime;
    int totalThreshold;

    Sparks(int kind);

    ~Sparks();

    void translate(AbyssEngine::AEMath::Vector const &v);

    void explode(AbyssEngine::AEMath::Vector const &v);

    void explode(int x, int y, int z);

    bool isRocket();

    void update(int step);

    void render();
};
#endif
