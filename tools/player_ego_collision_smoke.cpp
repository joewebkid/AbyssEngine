#include <cassert>
#include <cstdint>
#include <cmath>
#include <vector>

namespace AbyssEngine { namespace AEMath {
struct Vector {
    float x, y, z;
    Vector &operator-=(Vector other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vector &operator*=(float factor) { x *= factor; y *= factor; z *= factor; return *this; }
};
struct Matrix { int value = 0; };
Vector operator-(Vector a, Vector b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
bool operator!=(Vector a, Vector b) { return a.x != b.x || a.y != b.y || a.z != b.z; }
float VectorLength(Vector v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
Vector VectorNormalize(Vector v) {
    float len = VectorLength(v);
    return len ? Vector{v.x / len, v.y / len, v.z / len} : Vector{0, 0, 0};
}
Matrix operator*(Matrix a, Matrix b) { return {a.value + b.value}; }
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::Matrix;
static const float g_PE_cc_alarmDist = 16000.0f;

template<class T> struct Array {
    std::vector<T> values;
    unsigned size() const { return static_cast<unsigned>(values.size()); }
    T operator[](unsigned index) const { return values[index]; }
};
struct Player {
    Matrix transformMatrix;
    Vector hit = {0, 0, 0};
    float bombForce = 0, flShake = 0;
    int damageTotal = 0, hitCalls = 0;
    void setHitVector(float x, float y, float z) { hit = {x, y, z}; ++hitCalls; }
    void setBombForce(float value) { bombForce = value; }
    void damage(int value) { damageTotal += value; }
};
struct AEGeometry {
    Vector position = {0, 0, 0};
    unsigned transform = 0;
    int setCalls = 0, translateCalls = 0;
    Vector getPosition() { return position; }
    void setPosition(const Vector &value) { position = value; ++setCalls; }
    void translate(const Vector &value) {
        position = {position.x + value.x, position.y + value.y, position.z + value.z};
        ++translateCalls;
    }
};
struct KIPlayer {
    Player *player = nullptr;
    unsigned char _pad_0x71 = 0;
    char stealFlagByte = 0;
    bool colliding = false, visible = true, dying = false, dead = false;
    int type = 0;
    Vector position = {0, 0, 0}, projected = {0, 0, 0}, hit = {0, 0, 0};
    Vector projectInput = {0, 0, 0}, hitInput = {0, 0, 0};
    AEGeometry *outerTarget = nullptr;
    Vector outerPosition = {0, 0, 0};
    virtual ~KIPlayer() = default;
    virtual int outerCollide(const Vector &) {
        if (outerTarget) outerTarget->position = outerPosition;
        return colliding;
    }
    int getType() { return type; }
    unsigned char isVisible() { return visible; }
    virtual Vector getPosition() { return position; }
    virtual Vector projectCollisionOnSurface(const Vector &input) {
        projectInput = input;
        return projected;
    }
    int isDying() { return dying; }
    int isDead() { return dead; }
    virtual Vector getProjectionVector(const Vector &input) {
        hitInput = input;
        return hit;
    }
};
struct PlayerWormHole : KIPlayer {
    bool shrinking = false;
    int isShrinking() { return shrinking; }
};
struct Status {
    bool alien = false;
    int inAlienOrbit() { return alien; }
};
struct FModSound {
    bool playing = false;
    int playCalls = 0, stopCalls = 0, updateCalls = 0;
    Vector lastPosition = {0, 0, 0}, lastVelocity = {1, 1, 1};
    unsigned isPlaying(int id) { assert(id == 34); return playing; }
    void play(int id, Vector *, Vector *, float) { assert(id == 34); ++playCalls; }
    void stop(int id) { assert(id == 34); ++stopCalls; }
    void updateEvent3DAttributes(int id, Vector *pos, Vector *vel, bool restart) {
        assert(id == 34 && !restart); ++updateCalls;
        lastPosition = *pos; lastVelocity = *vel;
    }
};
struct PaintCanvas {
    Matrix local = {2}, parent = {3};
    int calls = 0;
    void *TransformGetLocal(unsigned id) {
        ++calls;
        assert(id == 1 || id == 2);
        return id == 1 ? static_cast<void *>(&local) : static_cast<void *>(&parent);
    }
};
struct Globals {
    static Status *status;
    static FModSound *sound;
    static PaintCanvas *Canvas;
};
Status *Globals::status = nullptr;
FModSound *Globals::sound = nullptr;
PaintCanvas *Globals::Canvas = nullptr;
struct PlayerEgo {
    Player *player = nullptr;
    AEGeometry *geometry = nullptr, *field_0x4 = nullptr;
    Vector dockOffsetVec = {0, 0, 0};
    Vector waypointY = {0, 0, 0}, rotY = {0, 0, 0};
    int dockingPointIndex = 0, dockingState = 0, miningGame = 0;
    void *asteroidTarget = nullptr;
    unsigned char dockedFlag = 0, collidesWithStationFlag = 0;
    unsigned char inWormhole = 0, boostingFlag = 0, volatileGoods = 0;
    int hitCameraCalls = 0;
    void hitCamera() { ++hitCameraCalls; }
    void calcCollision(Array<KIPlayer *> *candidates);
};

// PRODUCTION_METHOD

int main() {
    Status status;
    FModSound sound;
    PaintCanvas canvas;
    Globals::status = &status;
    Globals::sound = &sound;
    Globals::Canvas = &canvas;
    Player egoPlayer, otherPlayer;
    AEGeometry body, parent;
    body.transform = 1;
    parent.transform = 2;
    PlayerEgo ego;
    ego.player = &egoPlayer;
    ego.geometry = &body;
    ego.field_0x4 = &parent;
    ego.calcCollision(nullptr);

    KIPlayer obstacle;
    obstacle.player = &otherPlayer;
    obstacle._pad_0x71 = 1;
    Array<KIPlayer *> list{{&obstacle}};
    ego.dockedFlag = 1;
    ego.dockingPointIndex = 1;
    ego.calcCollision(&list);
    assert(!ego.collidesWithStationFlag && ego.hitCameraCalls == 0);
    ego.dockedFlag = 0;
    ego.dockOffsetVec = {100, 0, 0};
    ego.calcCollision(&list);
    assert(ego.collidesWithStationFlag && ego.hitCameraCalls == 0);

    ego.collidesWithStationFlag = 0;
    status.alien = true;
    obstacle.colliding = true;
    obstacle.projected = {3, 4, 5};
    obstacle.outerTarget = &body;
    obstacle.outerPosition = {8, 0, 0};
    ego.dockingState = 1;
    ego.asteroidTarget = &obstacle;
    ego.calcCollision(&list);
    assert(body.setCalls == 1 && body.position.x == 3 && body.position.z == 5);
    assert(obstacle.projectInput.x == 8);
    assert(egoPlayer.transformMatrix.value == 5 && canvas.calls == 2);
    assert(ego.collidesWithStationFlag && ego.hitCameraCalls == 1);
    obstacle.visible = false;
    ego.calcCollision(&list);
    assert(body.setCalls == 1 && ego.hitCameraCalls == 1);

    KIPlayer hostile;
    hostile.player = &otherPlayer;
    hostile.stealFlagByte = 1;
    hostile.colliding = true;
    hostile.hit = {1, 2, 3};
    hostile.outerTarget = &body;
    hostile.outerPosition = {9, 0, 0};
    list.values = {&hostile};
    ego.dockingState = 0;
    ego.asteroidTarget = nullptr;
    ego.boostingFlag = 1;
    ego.volatileGoods = 1;
    ego.calcCollision(&list);
    assert(otherPlayer.hitCalls == 1 && otherPlayer.hit.x == 1 && otherPlayer.hit.z == 3);
    assert(hostile.hitInput.x == 9);
    assert(otherPlayer.bombForce == 0.2f && otherPlayer.damageTotal == 9999);
    assert(egoPlayer.damageTotal == 20 && egoPlayer.flShake == 0.2f && ego.hitCameraCalls == 2);
    ego.dockingState = 1;
    ego.asteroidTarget = &hostile;
    ego.calcCollision(&list);
    assert(otherPlayer.hitCalls == 1 && ego.hitCameraCalls == 2);
    ego.dockingState = 0x100;
    ego.calcCollision(&list);
    assert(otherPlayer.hitCalls == 2 && ego.hitCameraCalls == 3);

    PlayerWormHole hole;
    hole.player = &otherPlayer;
    hole.type = 0x4262;
    hole.colliding = true;
    hole.position = {500, 0, 0};
    list.values = {&hole};
    ego.dockingState = 0;
    ego.asteroidTarget = nullptr;
    ego.hitCameraCalls = 2;
    body.position = {0, 0, 0};
    ego.calcCollision(&list);
    assert(sound.playCalls == 1 && body.translateCalls == 1 && body.position.x == 154);
    assert(ego.waypointY.x == 1 && ego.rotY.x == 154);
    assert(ego.inWormhole && ego.hitCameraCalls == 3 && canvas.calls == 4);
    body.position = {0, 0, 0};
    sound.playing = true;
    ego.calcCollision(&list);
    assert(sound.updateCalls == 1 && sound.lastPosition.x == 500);
    assert(sound.lastVelocity.x == 0 && body.translateCalls == 2);
    hole.position = {50000, 0, 0};
    ego.calcCollision(&list);
    assert(sound.stopCalls == 1 && body.translateCalls == 2);
    ego.miningGame = 1;
    ego.calcCollision(&list);
    assert(sound.stopCalls == 1 && body.translateCalls == 2);
}
