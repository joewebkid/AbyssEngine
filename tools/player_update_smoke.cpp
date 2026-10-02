// Focused host test: actual recovered Player bodies with game/audio boundary doubles.
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
#include <cstring>
#include <new>
#include <initializer_list>

unsigned char Globals::options[100] = {};
FModSound *Globals::sound = nullptr;
Status *Globals::status = nullptr;
static FModSound *replacementSound;
static FModSound *usedSound;
static Vector *usedPosition;
static unsigned alreadyPlaying;
static int soundId, played, updated;
static Vector enginePosition, engineVelocity;
static int engineIndex;
FModSound::FModSound() {}
FModSound::~FModSound() {}
unsigned FModSound::isPlaying(int index) {
    soundId = index;
    Globals::sound = replacementSound;
    return alreadyPlaying;
}
void FModSound::play(int index, Vector *position, Vector *velocity, float) {
    assert(velocity == nullptr);
    soundId = index;
    usedSound = this;
    usedPosition = position;
    ++played;
}
void FModSound::updateEvent3DAttributes(int index, Vector *position, Vector *velocity, bool restart) {
    assert(velocity == nullptr && !restart && index == soundId);
    usedSound = this;
    usedPosition = position;
    ++updated;
}
FMOD::Event *FModSound::updateEvent3DAttributes(FMOD::Event *event, int index, Vector *position, Vector *velocity, bool restart) {
    assert(position != nullptr && velocity != nullptr && !restart);
    engineIndex = index;
    enginePosition = *position;
    engineVelocity = *velocity;
    return event;
}
FModSound *FModSound::stop(FMOD::Event *) { return nullptr; }
Gun::~Gun() noexcept(false) {}
Gun::Gun(int, int, int, int, int, int, float, Vector, Vector) : positionArray(), directionArray(), upArray(), hitArray(), enemies(nullptr) {}
void Gun::setEnemies(Array<Player *> *value) { enemies = value; } // Gun boundary double.

static Gun *failureGun;
static bool allFail;
static int shotCalls;
void Gun::ignite() { assert(false); }
bool Gun::shoot(Matrix, int, bool) { ++shotCalls; return !allFail && this != failureGun; }
bool Gun::shootAt(Matrix, int, Player *, bool) { ++shotCalls; return !allFail && this != failureGun; }

// Constructor boundary: production Matrix assignment/math come from AEMath.cpp.
AbyssEngine::AEMath::Matrix::Matrix() {
    for (float &f : m) f = 0.0f;
    m[0] = m[5] = m[10] = 1.0f;
}
int main() {
    alignas(Player) unsigned char storage[sizeof(Player)];
    std::memset(storage, 0xa5, sizeof(storage));
    Player *constructed = new (storage) Player(10, 100, 0, 0, 0);
    assert(constructed->active == 1 && constructed->damaged == 0);
    constructed->~Player();
    {
        Player slots(10, 100, 2, -1, 1);
        assert(slots.guns->data()[0]->size() == 2 && slots.guns->data()[1] == nullptr);
        assert(slots.guns->data()[2]->size() == 1);
        assert(slots.guns->data()[0]->data()[0] == nullptr && slots.guns->data()[0]->data()[1] == nullptr);
        assert(slots.guns->data()[2]->data()[0] == nullptr);
    }
    Player player(10, 100, 0, 0, 0);
    player.transformMatrix.m[3] = 1.25f;
    player.transformMatrix.m[7] = -2.5f;
    player.transformMatrix.m[11] = 3.75f;
    Vector position = player.getPosition();
    assert(position.x == 1.25f && position.y == -2.5f && position.z == 3.75f);
    assert(!player.gunAvailable(-1));
    assert(!player.gunAvailable(4));
    Gun *gun = new Gun(0, 0, 0, 0, 0, 0, 0, Vector{}, Vector{});
    Array<Gun *> *slot = new Array<Gun *>();
    ArrayAdd(gun, *slot);
    player.guns->data()[0] = slot;
    assert(player.gunAvailable(0));
    Array<Player *> incoming;
    ArrayAdd(&player, incoming);
    player.setEnemies(&incoming);
    assert(player.enemies != &incoming && player.enemies->data() != incoming.data());
    assert(gun->enemies == player.enemies && player.enemies->data()[0] == &player);
    incoming.data()[0] = nullptr;
    assert(player.enemies->data()[0] == &player);
    player.addEnemy(nullptr);
    assert(player.enemies->size() == 2 && player.enemies->data()[0] == &player);
    assert(player.enemies->data()[1] == nullptr && gun->enemies == player.enemies);
    player.setEnemies(nullptr);
    assert(player.enemies == nullptr && gun->enemies == nullptr);
    player.addEnemy(&player); // Empty-list path calls setEnemy.
    assert(player.enemies->size() == 1 && gun->enemies == player.enemies);
    FModSound firstSound, nextSound;
    replacementSound = &nextSound;
    Vector shotPosition{};
    Globals::options[0x0f] = 1;
    Globals::sound = &firstSound;
    alreadyPlaying = 0;
    player.playShootSound(0, 2, &shotPosition, 1);
    assert(played == 1 && usedSound == &nextSound && usedPosition == &shotPosition);
    Globals::sound = &firstSound;
    alreadyPlaying = 1;
    player.playShootSound(0, 2, &shotPosition, 1);
    assert(updated == 1 && usedSound == &nextSound && usedPosition == &shotPosition);
    Globals::options[0x0f] = 0;
    player.playShootSound(0, 2, &shotPosition, 1);
    assert(played == 1 && updated == 1); // Playing loop stays untouched with 3D disabled.
    player.playShootSound(0, -1, &shotPosition, 1);
    assert(played == 2 && usedPosition == nullptr); // Out-of-mask channel plays unconditionally.
    Globals::options[0x0f] = 1;
    player.enginePositionVec = reinterpret_cast<void *>(12);
    player.update(100, true);
    assert(engineIndex == 12 && enginePosition.x == 1.25f && enginePosition.y == -2.5f && enginePosition.z == 3.75f);
    assert(engineVelocity.x == 1.25f && engineVelocity.y == -2.5f && engineVelocity.z == 3.75f);
    player.transformMatrix.m[3] = 2.25f;
    player.transformMatrix.m[7] = -4.5f;
    player.transformMatrix.m[11] = 6.75f;
    player.update(50, true);
    assert(engineVelocity.x == 2 && engineVelocity.y == -4 && engineVelocity.z == 6);
    assert(player.position[0] == 2.25f && player.position[1] == -4.5f && player.position[2] == 6.75f);
    Globals::options[0x0f] = 0;
    player.enginePositionVec = reinterpret_cast<void *>(static_cast<intptr_t>(-1));
    player.engineSoundPlaying = 0;
    player.damageTimer = 0;
    Globals::sound = nullptr;
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
    assert(!std::signbit(player.gammaHP));
    player.gammaHP = -0.0f;
    player.damageGamma(0);
    assert(player.gammaHP == 0 && !std::signbit(player.gammaHP));
    player.gammaHP = 1;
    player.damageGamma(std::numeric_limits<float>::infinity());
    assert(player.gammaHP == 0 && !std::signbit(player.gammaHP));
    player.gammaHP = std::numeric_limits<float>::quiet_NaN();
    player.damageGamma(1);
    assert(std::isnan(player.gammaHP)); // ARM's <= comparison preserves NaN.
    for (int value : {std::numeric_limits<int>::min(), -1, 0, 99, 100, 101,
                      9999998, 9999999, 10000000, std::numeric_limits<int>::max()}) {
        player.setGammaHP(value);
        float expected = static_cast<float>(value);
        if (value > 100 && value != 9999999) expected = 100.0f;
        assert(player.gammaHP == expected);
    }
    {
        Player shooter(10, 100, 0, 0, 0);
        shooter.playShootSoundFlag = 0;
        Array<Gun *> *slot = new Array<Gun *>();
        Gun *first = new Gun(0, 0, 0, 0, 0, 0, 0, Vector{}, Vector{});
        Gun *second = new Gun(0, 0, 0, 0, 0, 0, 0, Vector{}, Vector{});
        for (Gun *g : {first, second}) {
            g->weaponType = static_cast<ItemSort>(1);
            g->itemIndex = 99;
            g->timer = 10;
            g->fireDelay = 1;
            g->ammoCount = 0;
            g->field_0x89 = 0;
        }
        ArrayAdd(first, *slot);
        shooter.guns->data()[0] = slot;
        allFail = true;
        assert(!shooter.shoot(0, 99, 42LL, false));
        assert(first->timer == 10 && shooter.flShake == 0 && shotCalls == 1);
        first->ammoCount = 1;
        assert(shooter.shoot(0, 99, 42LL, false));
        assert(first->timer == 10 && shooter.flShake == 0);
        first->ammoCount = 0;
        ArrayAdd(second, *slot);
        failureGun = first;
        allFail = false;
        assert(shooter.shoot(0, 99, 42LL, false));
        assert(first->timer == 10 && second->timer == 0);
        assert(std::fabs(shooter.flShake - 0.008f) < 0.000001f);
        second->timer = 10;
        shooter.flShake = 0;
        allFail = true;
        assert(shooter.shoot(0, 42LL, false));
        assert(first->timer == 10 && second->timer == 10 && shooter.flShake == 0);
        allFail = false;
        assert(shooter.shoot(0, 42LL, false));
        assert(first->timer == 10 && second->timer == 0);
        assert(std::fabs(shooter.flShake - 0.008f) < 0.000001f);
    }
    std::puts("Player recovery host smoke: OK");
}
