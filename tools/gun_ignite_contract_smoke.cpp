#include "game/weapons/Gun.h"
#include <cstdio>

// Test double records the exact Vector operand; no particle renderer is exercised.
static const Vector *explosionOperand;
static int explosions;
Sparks::Sparks(int type)
    : lifetimeThresholds(nullptr), spriteSystem(0), texture(0), elapsed(0), active(0),
      kind(type), count(0), lifetime(0), totalThreshold(0) {}
Sparks::~Sparks() {}
void Sparks::explode(const Vector &position) {
    explosionOperand = &position;
    ++explosions;
}
void exerciseGunIgniteImpact(Gun &);

int checkGunIgniteImpactContract() {
    Gun gun(0, 1, 2, 0, 10, 1, 1.0f, Vector{0,0,0}, Vector{0,0,0});
    gun.positionArray[0] = Vector{1,2,3};
    gun.positionArray[1] = Vector{4,5,6};
    gun.basePos = Vector{101,102,103};
    exerciseGunIgniteImpact(gun);
    if (explosions) return 1;
    Sparks sparks(0);
    gun.impact = &sparks;
    exerciseGunIgniteImpact(gun);
    gun.impact = nullptr;
    if (explosions != 1 || explosionOperand != &gun.positionArray[0] ||
        explosionOperand->x != 1 || explosionOperand->y != 2 || explosionOperand->z != 3) {
        std::fputs("Ignite impact must use the first projectile position, not cached basePos\n", stderr);
        return 1;
    }
    std::puts("Gun extracted ignite impact operand smoke: OK");
    return 0;
}
