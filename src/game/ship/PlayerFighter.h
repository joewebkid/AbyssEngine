#ifndef GOF2_PLAYERFIGHTER_H
#define GOF2_PLAYERFIGHTER_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "engine/math/EaseInOutMatrix.h"

#include "engine/math/Matrix.h"
#include "engine/render/Trail.h"
#include "game/mission/Explosion.h"
#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"

class Player;

class AEGeometry;
class BoundingVolume;
class Explosion;
class KIPlayer;
class Level;
class Route;
class Trail;
namespace AbyssEngine { 
    class EaseInOutMatrix;
 }



class PlayerFighter : public KIPlayer {
public:
    Explosion *explosion;
    int32_t field_0x128;
    uint8_t field_0x12c;
    uint8_t field_0x12d;
    uint8_t field_0x12e;
    signed char field_0x12f;
    int32_t field_0x130;
    int32_t field_0x134;
    int32_t field_0x138;
    signed char field_0x13c;
    uint8_t field_0x13d;
    signed char field_0x13e;
    int32_t field_0x140;
    Route *route;
    int32_t field_0x148;
    Route *commandRoute;
    Array<BoundingVolume *> *boundingVolumes;
    Trail *trail;
    Vector workingPosition;
    Vector resetVecB;
    Vector resetVecC;
    uint8_t _pad_0x17c[0x24];
    int32_t engineTrailSystem;
    float rotate;
    float shootError;
    float speed;
    int32_t field_0x1b0;
    int32_t boostProb;
    int32_t maneuverTimer;
    uint8_t _pad_0x1bc[4];
    int32_t field_0x1c0;
    int32_t field_0x1c4;
    int32_t field_0x1c8;
    uint8_t _pad_0x1cc[4];
    int32_t deltaTime;
    int32_t deltaTimeHi;
    int32_t hitpoints;
    int32_t field_0x1dc;
    signed char field_0x1e0;
    int32_t field_0x1e4;
    float currentSpeed;
    int32_t field_0x1ec;
    float currentRotate;
    short field_0x1f4;
    int32_t field_0x1f8;
    signed char field_0x1fc;
    int32_t field_0x200;
    int32_t field_0x204;
    int32_t field_0x208;
    int32_t targetRoll;
    int32_t smoothRoll;
    int32_t field_0x214;
    AbyssEngine::Matrix easeBaseMatrix;
    signed char field_0x254;
    signed char field_0x255;
    uint8_t _pad_0x256[2];
    AbyssEngine::Matrix rollMatrix;
    int32_t field_0x294;
    int32_t field_0x298;
    int32_t rollSamples;
    int32_t field_0x2a0;
    int32_t field_0x2a4;
    int32_t field_0x2a8;
    int32_t field_0x2ac;
    int32_t rollSampleIndex;
    uint8_t rollBufferFilled;
    AbyssEngine::EaseInOutMatrix *easeMatrix;
    int32_t spacePoint;
    int32_t cloakTimer;
    int32_t field_0x2c9;
    int32_t cloakDuration;
    int32_t field_0x2cd;
    uint8_t cloakActive;
    int32_t cloakCooldown;
    uint8_t cloakingPossible;
    signed char field_0x2d9;
    uint32_t cloakMaterial;
    int32_t cloakSavedMode;
    uint8_t aiDisabled;
    int32_t gunSwitchTimer;
    uint32_t field_0x2ec; // Original allocation includes this unclassified tail word.

    PlayerFighter(int faction, int wingmanCmd, Player *player, AEGeometry *geom,
                  float x, float y, float z, bool active);

    ~PlayerFighter();

    void awake() override;

    void reset();

    void revive() override;

    void cloak(int dur, bool b);

    int collide(float x, float y, float z) override;

    void handleCloaking();

    uint8_t hasCrateCaptured();

    uint8_t hasCrateLost();

    uint8_t hasMissionCrateCaptured();

    uint8_t hasMissionCrateLost();

    void initPush(const Vector &target, int radius) override;

    int outerCollide(float x, float y, float z) override;

    void push(int dt) override;

    void removeTrail();

    void render() override;

    void roll(int angle);

    void setAIDisabled(bool v);

    void setBV(Array<BoundingVolume *> *v);

    void setBV(BoundingVolume *bv);

    void setBoostProb(int v);

    void setCloakingPossible(bool v);

    void setExhaustVisible(bool vis);

    void setLevel(Level *lvl);

    void setMissionCrate(bool on);

    void setPosition(float x, float y, float z) override;

    void setPosition(const Vector &v);

    void setRotate(int v);

    void setShootError(int v);

    void setShipGroup(AEGeometry *geom, int group, bool flag) override;

    void setSpeed(float v) override;

    void setWingmanCommand(int cmd, KIPlayer *target) override;

    void update(int dt) override;

    // Static data members present in the original binary (defined for symbol parity).
    static int stationRouteAliens;
};
#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(PlayerFighter) == 0x2f0, "PlayerFighter ARM allocation must be 0x2f0 bytes");
static_assert(offsetof(PlayerFighter, explosion) == 0x124, "PlayerFighter::explosion ABI offset");
static_assert(offsetof(PlayerFighter, field_0x130) == 0x130, "PlayerFighter particle handle ABI offset");
static_assert(offsetof(PlayerFighter, field_0x134) == 0x134, "PlayerFighter particle handle ABI offset");
static_assert(offsetof(PlayerFighter, field_0x138) == 0x138, "PlayerFighter particle handle ABI offset");
static_assert(offsetof(PlayerFighter, route) == 0x144, "PlayerFighter::route ABI offset");
static_assert(offsetof(PlayerFighter, engineTrailSystem) == 0x1a0, "PlayerFighter engine trail ABI offset");
static_assert(offsetof(PlayerFighter, deltaTime) == 0x1d0, "PlayerFighter::deltaTime ABI offset");
static_assert(offsetof(PlayerFighter, easeBaseMatrix) == 0x218, "PlayerFighter first matrix ABI offset");
static_assert(offsetof(PlayerFighter, rollMatrix) == 0x258, "PlayerFighter roll matrix ABI offset");
#endif
#endif
