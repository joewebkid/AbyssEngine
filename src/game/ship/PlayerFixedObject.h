#ifndef GOF2_PLAYERFIXEDOBJECT_H
#define GOF2_PLAYERFIXEDOBJECT_H
#include "engine/core/Array.h"
#include "../../engine/core/AEString.h"
#include "game/mission/Explosion.h"

#include "game/ship/KIPlayer.h"

#include "engine/math/Vector.h"

#include "engine/math/AEMath.h"


class Player;

class AEGeometry;
class BoundingVolume;
class Explosion;


typedef AbyssEngine::AEMath::Vector V3;

// Original ARM allocation is 0x1bc; core state belongs to KIPlayer.
#pragma pack(push, 4)
class PlayerFixedObject : public KIPlayer {
public:
    AEGeometry *wreckGeometry;
    Array<BoundingVolume *> *boundingVolumes;
    Array<BoundingVolume *> *wreckCollision;
    int32_t deltaTime;
    unsigned char moving;
    Vector respawnPos;
    Vector homingTarget;
    Vector homingDir;
    uint32_t field_0x15c;
    uint64_t field_0x160;
    int32_t targetEnemy;
    int collisionIndex;
    uint8_t field_0x170;
    int32_t field_0x174;
    int32_t intPosX;
    int32_t intPosY;
    int32_t intPosZ;
    uint16_t wreckMeshId;
    int field_188; // Android gap between wreckMeshId and explosion.
    Explosion *explosion;
    int explosionElapsed;
    int wreckType;
    int rumbleTimer;
    float rumblePercentage;
    int wreckMaterial;
    int dockingType;
    int transportID;
    String name;
    uint8_t shipHidden;

    PlayerFixedObject(int kind, int param2, Player *player, AEGeometry *geom,
                      float x, float y, float z);

    ~PlayerFixedObject() override;

    int collide(float x, float y, float z) override;

    int getDockingType();

    String getName();

    V3 getPosition() override;

    V3 getProjectionVector(const Vector &vec) override;

    int getTransportID();

    void hideShip();

    void moveForward(int amount);

    int outerCollide(float x, float y, float z) override;

    void outerCollide(Vector v);

    V3 projectCollisionOnSurface(const Vector &vec) override;

    void render() override;

    void reset();

    void setBV(BoundingVolume *bv);

    void setBV(Array<BoundingVolume *> *bv);

    void setDeadButSelectable();

    void setDockingType(int v);

    void setExhaustVisible(bool v);

    void setMoving(bool v);

    void *setName(String name);

    void setPosition(float x, float y, float z) override;

    void setPosition(const Vector &v) override;

    void setTransportID(int v);

    void setWreckedMeshId(int meshId);

    void translate(const Vector &d) override;

    void update(int dt) override;
};
#pragma pack(pop)

#if __SIZEOF_POINTER__ == 4
static_assert(sizeof(PlayerFixedObject) == 0x1bc, "PlayerFixedObject ARM size");
static_assert(__builtin_offsetof(PlayerFixedObject, hasCargo) == 0x4c, "PFO cargo byte storage offset");
static_assert(__builtin_offsetof(PlayerFixedObject, field_0x8c) == 0x8c, "PFO collision gate storage offset");
static_assert(__builtin_offsetof(PlayerFixedObject, wreckGeometry) == 0x124, "PFO wreckGeometry offset");
static_assert(__builtin_offsetof(PlayerFixedObject, boundingVolumes) == 0x128, "PFO bounds offset");
static_assert(__builtin_offsetof(PlayerFixedObject, wreckCollision) == 0x12c, "PFO wreck bounds offset");
static_assert(__builtin_offsetof(PlayerFixedObject, moving) == 0x134, "PFO moving offset");
static_assert(__builtin_offsetof(PlayerFixedObject, homingTarget) == 0x144, "PFO homing target offset");
static_assert(__builtin_offsetof(PlayerFixedObject, homingDir) == 0x150, "PFO homing direction offset");
static_assert(__builtin_offsetof(PlayerFixedObject, intPosX) == 0x178, "PFO intPosX offset");
static_assert(__builtin_offsetof(PlayerFixedObject, explosion) == 0x18c, "PFO explosion offset");
static_assert(__builtin_offsetof(PlayerFixedObject, name) == 0x1ac, "PFO name offset");
#endif
#endif
