#include <cassert>
#include <cstdint>

struct Vector { float x, y, z; };
struct AERandom {
    int calls = 0;
    int value = 17;
    int nextInt(int bound) { assert(bound == 5000); ++calls; return value; }
};
struct Globals { static void *rnd; };
void *Globals::rnd = nullptr;

struct KIPlayer {
    int baseResets = 0;
    void reset() { ++baseResets; }
};
struct PlayerFighter : KIPlayer {
    float posX = 1.25f, posY = -2.5f, posZ = 3.75f;
    Vector workingPosition{}, renderPosition{}, resetVecA{}, resetVecB{}, resetVecC{};
    uint8_t crateCaptured = 0, field_0x12e = 1, field_0x12c = 1;
    uint8_t isMissionCrate = 1, missionCrateLost = 1;
    uint8_t missionCrateCaptured = 1, crateLost = 1;
    uint8_t field_0x1fc = 1, field_0x13c = 1;
    uint8_t cloakActive = 1, cloakingPossible = 0, aiDisabled = 1;
    int deltaTime = 3, deltaTimeHi = 4, field_0x38 = 5, field_0x148 = 6;
    int maneuverTimer = 7, field_0x1c0 = 8, field_0x1c4 = 9, field_0x1c8 = 10;
    int state = 5, field_0x140 = 11;
    int field_0x2c0 = 123, cloakTimer = 12, cloakDuration = 13, cloakCooldown = 14;
    int field_0x2d9 = 0, handleCalls = 0;
    void handleCloaking() { ++handleCalls; }
    void reset();
    void cloak(int dur, bool b);
    void setCloakingPossible(bool value);
};

// PRODUCTION_METHODS

int main() {
    AERandom rng;
    Globals::rnd = &rng;
    PlayerFighter fighter;
    fighter.resetVecA = {1, 2, 3};
    fighter.resetVecB = {4, 5, 6};
    fighter.resetVecC = {7, 8, 9};
    fighter.reset();
    assert(fighter.baseResets == 1 && fighter.state == 5);
    assert(fighter.workingPosition.x == fighter.posX);
    assert(fighter.renderPosition.y == fighter.posY);
    assert(fighter.renderPosition.z == fighter.posZ);
    assert(fighter.resetVecA.x == 0 && fighter.resetVecB.y == 0 && fighter.resetVecC.z == 0);
    assert(fighter.deltaTime == 0 && fighter.deltaTimeHi == 0);
    assert(fighter.maneuverTimer == 0 && fighter.field_0x1c0 == 0 && fighter.field_0x1c8 == 0);
    assert(fighter.isMissionCrate == 0 && fighter.missionCrateLost == 0);
    assert(fighter.missionCrateCaptured == 0 && fighter.crateLost == 0);
    assert(fighter.crateCaptured == 1 && fighter.cloakActive == 0);
    assert(fighter.cloakTimer == 0 && fighter.cloakDuration == 0);
    assert(fighter.cloakCooldown == 0 && fighter.cloakingPossible == 1);
    assert(fighter.field_0x2c0 == 123);
    fighter.state = 4;
    fighter.reset();
    assert(fighter.state == 0 && fighter.baseResets == 2);

    fighter.cloak(2000, true);
    assert(rng.calls == 0 && fighter.cloakActive == 1);
    assert(fighter.cloakDuration == 6000 && fighter.field_0x2d9 == 1);
    fighter.cloak(0, false);
    assert(rng.calls == 1 && fighter.cloakDuration == 9017);
    assert(fighter.field_0x2d9 == 0);

    fighter.field_0x13c = 1;
    fighter.setCloakingPossible(false);
    assert(fighter.cloakingPossible == 0 && fighter.cloakTimer == 9018);
    assert(fighter.handleCalls == 1);
    fighter.field_0x13c = 0;
    fighter.setCloakingPossible(false);
    assert(fighter.handleCalls == 1);
    fighter.setCloakingPossible(true);
    assert(fighter.cloakingPossible == 1 && fighter.handleCalls == 1);
}
