// Focused host test: actual Player::update with audio boundary doubles.
// This does not validate the full native game or ARM ABI/runtime.
#include "game/ship/Player.h"
#include "game/core/Globals.h"
#include "engine/audio/FModSound.h"
#include "engine/math/AEMath.h"
#include "game/weapons/Gun.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>

unsigned char Globals::options[100] = {};
FModSound *Globals::sound = nullptr;
Status *Globals::status = nullptr;
FMOD::Event *FModSound::updateEvent3DAttributes(FMOD::Event *event, int, Vector *, Vector *, bool) { return event; }
FModSound *FModSound::stop(FMOD::Event *) { return nullptr; }
Gun::~Gun() noexcept(false) {} // All gun slots are empty in this test.

// Constructor boundary: production Matrix assignment/math come from AEMath.cpp.
AbyssEngine::AEMath::Matrix::Matrix() {
    for (float &f : m) f = 0.0f;
    m[0] = m[5] = m[10] = 1.0f;
}
void MatrixGetPosition(void *out, float *matrix) {
    *static_cast<Vector *>(out) = AbyssEngine::AEMath::MatrixGetPosition(*reinterpret_cast<Matrix *>(matrix));
}

int main() {
    Player player(10, 100, 0, 0, 0);
    player.maxArmorHP = player.maxShieldHP = player.maxEmpPoints = 100;
    player.empDisabledByte = 1;
    player.empData = 1000;
    player.empElapsed = 0;
    player.flShake = 0.1f;
    player.update(100, false);
    assert(player.empElapsed == 100 && player.empData == 1000);
    assert(player.empPoints == 10 && player.empDisabledByte == 1);
    assert(std::fabs(player.flShake - 0.0975f) < 0.000001f);
    player.update(900, false);
    // Original clears EMP only when regenerated points exceed the maximum.
    assert(player.empPoints == 100 && player.empDisabledByte == 1);
    player.empDisabledByte = 0;
    player.damaged = 1;
    player.damageTimer = 3000;
    player.flShake = 0.001f;
    player.update(100, false);
    assert(player.damaged == 0 && player.damageTimer == 0);
    assert(player.flShake == 0.0f);
    player.setHitVector(1, 2, 3);
    assert(player.hitVector[0] == 1 && player.hitVector[1] == 2 && player.hitVector[2] == 3);
    player.field_dc = 0x12345678;
    player.damageDoneByPlayer = 42;
    player.turnedEnemyFlag = 1;
    player.resetDamageDoneByPlayer();
    assert(player.field_dc == 0 && player.damageDoneByPlayer == 0 && player.turnedEnemyFlag == 0);
    player.damaged = 1;
    player.field_dc = 0x12345678;
    player.turnedEnemyFlag = 1;
    player.spawnedFlag = 7;
    player.reset();
    assert(player.damaged == 0 && player.field_dc == 0 && player.turnedEnemyFlag == 0);
    assert(player.spawnedFlag == 7); // reset only writes the destroyed byte.
    player.gammaHP = 1;
    player.damageGamma(2);
    assert(player.gammaHP == 0 && player.gammaHit == 1);
    player.gammaHP = std::numeric_limits<float>::quiet_NaN();
    player.damageGamma(1);
    assert(std::isnan(player.gammaHP)); // ARM's <= comparison preserves NaN.
    std::puts("Player EMP/shake update smoke: OK");
}
