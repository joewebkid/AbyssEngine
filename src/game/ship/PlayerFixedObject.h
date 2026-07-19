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

class PlayerFixedObject : public KIPlayer {
public:
    unsigned char empActive;
    int faction;
    Vector position;
    uint8_t field_0x40;
    uint8_t field_0x41;
    unsigned char hasCargo;
    Array<int> *lootList;
    float spawnX;
    float spawnY;
    float spawnZ;
    AEGeometry *secondaryGeometry;
    uint8_t collisionEnabled;
    Vector targetPos;
    int kind;
    int explosionTimer;
    int32_t aiActiveCounter;
    unsigned char finished;
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

    ~PlayerFixedObject();

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

    void setPosition(const Vector &v);

    void setTransportID(int v);

    void setWreckedMeshId(int meshId);

    void translate(const Vector &d) override;

    void update(int dt) override;
};
#endif
