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
    int32_t player;
    int32_t geometry;
    int32_t subGeometry;
    uint8_t field_0x24;
    signed char field_0x25;
    int32_t wingmanCommand;
    Vector renderPosition;
    int32_t field_0x38;
    signed char field_0x43;
    uint8_t crateCaptured;
    Array<int> *lootList;
    int32_t level;
    float posX;
    float posY;
    float posZ;
    uint8_t missionCrateLost;
    uint8_t missionCrateCaptured;
    uint8_t crateLost;
    Route *routeClone;
    uint8_t exhaustHidden;
    AEGeometry *wreckGeometry;
    int32_t field_0x80;
    int32_t field_0x84;
    int32_t state;
    Vector resetVecA;
    uint8_t isMissionCrate;
    int32_t deathTimer;
    signed char field_0xdc;
    int32_t field_0xe4;
    int32_t field_0xf0;
    uint8_t field_0xf5;
    int32_t fov;
    signed char rollActive;
    int32_t pushTimer;
    int32_t pushDuration;
    Vector pushNormal;
    Vector pushImpulse;
    Explosion *explosion;
    int32_t field_0x128;
    uint8_t field_0x12c;
    short field_0x12d;
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
    int32_t engineTrailSystem;
    float rotate;
    float shootError;
    float speed;
    int32_t field_0x1b0;
    int32_t boostProb;
    int32_t maneuverTimer;
    int32_t field_0x1c0;
    int32_t field_0x1c4;
    int32_t field_0x1c8;
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
    AbyssEngine::Matrix rollMatrix;
    signed char field_0x254;
    signed char field_0x255;
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
#endif
