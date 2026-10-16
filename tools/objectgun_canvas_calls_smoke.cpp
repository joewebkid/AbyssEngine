#include <cassert>
#include <cstdint>

struct Matrix {
    int marker = 42;
    float m[15] = {};
    bool scaled = false;
    bool rotated = false;
    bool multiplied = false;
    float x = 0, y = 0, z = 0;
    Matrix &operator*=(const Matrix &other) {
        multiplied = other.rotated;
        return *this;
    }
};
struct Vector { int marker = 0; float x = 0, y = 0, z = 0; };
namespace AbyssEngine { namespace AEMath {
Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
    Matrix result;
    result.marker = lhs.marker * 100 + rhs.marker;
    return result;
}
Vector MatrixGetDir(const Matrix &matrix) { return {matrix.marker * 10 + 1}; }
Vector MatrixGetUp(const Matrix &matrix) { return {matrix.marker * 10 + 2}; }
Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z) {
    matrix.rotated = true;
    matrix.x = x;
    matrix.y = y;
    matrix.z = z;
    return matrix;
}
Vector VectorCross(const Vector &a, const Vector &b) {
    return {0, a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
Vector VectorNormalize(const Vector &vector) { return vector; }
Matrix MatrixSetScaling(Matrix &matrix, float x, float y, float z) {
    matrix.scaled = true;
    matrix.x = x;
    matrix.y = y;
    matrix.z = z;
    return matrix;
}
} }

struct PaintCanvas;
struct Globals {
    static PaintCanvas *Canvas;
    static unsigned char iPad;
};
namespace AbyssEngine {
    enum AnimationMode : int { AnimationMode_0 = 0 };
    struct Transform {
        int calls = 0;
        int mode = -1;
        void *argument = this;
        void SetAnimationState(AnimationMode value, void *arg) {
            ++calls;
            mode = static_cast<int>(value);
            argument = arg;
        }
    };
}

struct PaintCanvas {
    int removeCalls = 0, addCalls = 0;
    int getCalls = 0;
    int setLocalCalls = 0, drawCalls = 0;
    unsigned removedTransform = 0, addedTransform = 0;
    unsigned retrievedTransform = 0;
    unsigned localTransform = 0, drawnTransform = 0;
    int localMarker = 0;
    const Matrix *drawnMatrix = nullptr;
    unsigned short removedMesh = 0, addedMesh = 0;
    bool addedFlag = true;
    PaintCanvas *replacementOnRemove = nullptr;
    PaintCanvas *replacementOnGet = nullptr;
    PaintCanvas *replacementOnSetLocal = nullptr;
    AbyssEngine::Transform transform;

    void TransformRemoveMesh(unsigned transform, unsigned short mesh) {
        ++removeCalls;
        removedTransform = transform;
        removedMesh = mesh;
        if (replacementOnRemove) Globals::Canvas = replacementOnRemove;
    }
    void TransformAddMesh(unsigned transform, unsigned short mesh, bool flag) {
        ++addCalls;
        addedTransform = transform;
        addedMesh = mesh;
        addedFlag = flag;
    }
    void *TransformGetTransform(unsigned index) {
        ++getCalls;
        retrievedTransform = index;
        if (replacementOnGet) Globals::Canvas = replacementOnGet;
        return &transform;
    }
    void TransformSetLocal(unsigned index, const Matrix &matrix) {
        ++setLocalCalls;
        localTransform = index;
        localMarker = matrix.marker;
        if (replacementOnSetLocal) Globals::Canvas = replacementOnSetLocal;
    }
    void DrawTransform(unsigned index, const Matrix *matrix) {
        ++drawCalls;
        drawnTransform = index;
        drawnMatrix = matrix;
    }
};

PaintCanvas *Globals::Canvas = nullptr;
unsigned char Globals::iPad = 0;

template <typename T> struct Array {
    T *items = nullptr;
    T *data() { return items; }
};
struct Player;
struct TargetFollowCamera;
constexpr int ITEM_SORT_SCATTER_GUN = 25;
struct Explosion {
    int startCalls = 0, updateCalls = 0, resetCalls = 0, renderCalls = 0;
    int hitMarker = 0, directionMarker = -1, lastDt = 0;
    float directionX = 1, directionY = 1, directionZ = 1;
    TargetFollowCamera *lastCamera = reinterpret_cast<TargetFollowCamera *>(1);
    uint8_t playing = 1;
    void start(const Vector &hit, const Vector &direction) {
        ++startCalls;
        hitMarker = hit.marker;
        directionMarker = direction.marker;
        directionX = direction.x;
        directionY = direction.y;
        directionZ = direction.z;
    }
    void update(int dt, TargetFollowCamera *camera) {
        ++updateCalls;
        lastDt = dt;
        lastCamera = camera;
    }
    uint8_t isPlaying() { return playing; }
    void reset() { ++resetCalls; }
    void render() { ++renderCalls; }
};
struct Gun {
    int delayActive = 0;
    int renderCalls = 0;
    int weaponType = 0;
    uint32_t count = 0;
    uint8_t *hitFlags = nullptr;
    Vector *hitPositions = nullptr;
    uint8_t ignited = 0;
    int *lifetimes = nullptr;
    uint8_t hitSmall = 0;
    Array<Player *> *enemies = nullptr;
    void setEnemies(Array<Player *> *value) { enemies = value; }
    void render() { ++renderCalls; }
};
struct AEGeometry {
    unsigned transform = 77;
    Matrix matrix;
    int directionMarker = 0, upMarker = 0;
    Matrix &getMatrix() { return matrix; }
    void setDirection(const Vector &dir, const Vector &up) {
        directionMarker = dir.marker;
        upMarker = up.marker;
    }
};
struct Player { float empPointsF = 0.2f, maxEmpPointsF = 0.3f; };
struct PlayerEgo { void *turretGeometry = nullptr, *field_0x2c = nullptr; };
struct EgoPlayerView { Matrix orientation; };

struct ObjectGun {
    unsigned transform = 17;
    int meshId = 0x12345;
    Gun *gun = nullptr;
    AEGeometry *geometry = nullptr;
    Array<Explosion *> *explosions = nullptr;
    uint8_t *explosionReady = nullptr;
    uint8_t wasFiring = 0;
    int deltaTime = 0;
    float spinAngle = 0;
    bool rocketGun = false;
    Vector dir, up, side;
    Matrix orientation;
    float scaleX = 0.6f, scaleY = 0.7f, scaleZ = 0.8f;
    void setEnemies(Array<Player *> *enemies);
    void replaceGun(unsigned mesh, int unused);
    void applyInactive();
    void applyCameraDirection(Matrix &playerMatrix, Matrix &cameraMatrix);
    void applyTurretMatrix(PlayerEgo *player, Matrix &workMatrix);
    void applyExplosionUpdate(int dt);
    void applyExplosionRender();
    void applyVisibleOrientation(Matrix &cameraLocal, Vector &muzzle);
    bool isRenderable(const Vector *gunPos);
    void applyTurretSpin(Matrix &rotate, Matrix &scaleMatrix);
    void applyVisibleLifetimeScaling(unsigned i);
    void applyWorldLifetimeScaling(unsigned i);
    void applyBasisUp();
    void applyWobbleStep(Vector *spin);
    bool isRocketGun() const { return rocketGun; }
    void applyRenderScale();
    void applyEgoRotation(EgoPlayerView *playerView, Player *player,
                          Matrix &local, Matrix &scaleMatrix);
    void renderFinal();
};

// PRODUCTION_METHODS

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    ObjectGun gun;
    gun.replaceGun(0x23456, 71);
    assert(canvas.removeCalls == 1 && canvas.addCalls == 1);
    assert(canvas.removedTransform == 17 && canvas.removedMesh == 0x2345);
    assert(gun.meshId == 0x23456);
    assert(canvas.addedTransform == 17 && canvas.addedMesh == 0x3456 &&
           !canvas.addedFlag);

    PaintCanvas replacement;
    PaintCanvas afterRemove;
    replacement.replacementOnRemove = &afterRemove;
    Globals::Canvas = &replacement;
    gun.replaceGun(23, -1);
    assert(replacement.removeCalls == 1 && replacement.addCalls == 0);
    assert(replacement.removedMesh == 0x3456);
    assert(afterRemove.addCalls == 1 && afterRemove.addedMesh == 23 &&
           afterRemove.addedTransform == 17 && !afterRemove.addedFlag);
    assert(gun.meshId == 23 && canvas.addCalls == 1);

    PaintCanvas first, second, third;
    first.replacementOnGet = &second;
    second.replacementOnGet = &third;
    Gun inactive;
    AEGeometry geometry;
    gun.gun = &inactive;
    gun.geometry = &geometry;
    Array<Player *> enemies;
    gun.setEnemies(&enemies);
    assert(inactive.enemies == &enemies);
    gun.setEnemies(nullptr);
    assert(inactive.enemies == nullptr);
    Globals::Canvas = &first;
    gun.applyInactive();
    assert(first.getCalls == 1 && second.getCalls == 1 && third.getCalls == 1);
    assert(first.retrievedTransform == 77 && second.retrievedTransform == 77 &&
           third.retrievedTransform == 77);
    assert(first.transform.calls == 1 && first.transform.mode == 0 &&
           first.transform.argument == nullptr);
    assert(second.transform.calls == 1 && second.transform.mode == 3 &&
           second.transform.argument == nullptr);
    assert(third.transform.calls == 1 && third.transform.mode == 1 &&
           third.transform.argument == nullptr);

    Matrix playerMatrix, cameraMatrix;
    playerMatrix.marker = 3;
    cameraMatrix.marker = 5;
    gun.applyCameraDirection(playerMatrix, cameraMatrix);
    assert(geometry.directionMarker == 31 && geometry.upMarker == 52);

    AEGeometry turretRoot, turretView;
    turretRoot.matrix.marker = 7;
    turretView.matrix.marker = 9;
    PlayerEgo ego;
    ego.field_0x2c = &turretRoot;
    ego.turretGeometry = &turretView;
    Matrix combined;
    gun.applyTurretMatrix(&ego, combined);
    assert(combined.marker == 709);

    Gun scatter;
    scatter.weaponType = ITEM_SORT_SCATTER_GUN;
    scatter.count = 2;
    scatter.delayActive = 1;
    scatter.ignited = 1;
    uint8_t hits[2] = {0, 1}, ready[2] = {1, 1};
    Vector hitPositions[2];
    hitPositions[1].marker = 77;
    scatter.hitFlags = hits;
    scatter.hitPositions = hitPositions;
    Explosion skipped, active;
    Explosion *explosionItems[2] = {&skipped, &active};
    Array<Explosion *> explosionArray;
    explosionArray.items = explosionItems;
    gun.gun = &scatter;
    gun.explosions = &explosionArray;
    gun.explosionReady = ready;
    gun.applyExplosionUpdate(16);
    assert(gun.wasFiring == 1 && skipped.startCalls == 0 &&
           skipped.updateCalls == 0 && active.startCalls == 1 &&
           active.updateCalls == 1 && active.resetCalls == 0);
    assert(active.hitMarker == 77 && active.directionMarker == 0 &&
           active.directionX == 0 && active.directionY == 0 &&
           active.directionZ == 0 && active.lastDt == 16 &&
           active.lastCamera == nullptr);
    assert(hits[1] == 1 && ready[1] == 0 && scatter.ignited == 1);
    active.playing = 0;
    gun.applyExplosionUpdate(17);
    assert(active.startCalls == 1 && active.updateCalls == 2 &&
           active.resetCalls == 1 && active.lastDt == 17);
    assert(hits[1] == 0 && ready[1] == 1 && scatter.ignited == 0);
    gun.applyExplosionUpdate(18);
    assert(active.updateCalls == 2 && active.resetCalls == 1);
    gun.applyExplosionRender();
    assert(scatter.renderCalls == 1 && skipped.renderCalls == 0 &&
           active.renderCalls == 0);
    hits[1] = 1;
    gun.applyExplosionRender();
    assert(scatter.renderCalls == 2 && skipped.renderCalls == 0 &&
           active.renderCalls == 1);

    Matrix visibleCamera;
    visibleCamera.m[0] = 10;
    visibleCamera.m[1] = 11;
    visibleCamera.m[4] = 14;
    visibleCamera.m[5] = 15;
    visibleCamera.m[8] = 18;
    visibleCamera.m[9] = 19;
    visibleCamera.m[10] = 110;
    visibleCamera.m[11] = 111;
    Vector muzzle;
    muzzle.x = 2;
    muzzle.y = 3;
    muzzle.z = 4;
    gun.applyVisibleOrientation(visibleCamera, muzzle);
    assert(gun.orientation.m[0] == 10 && gun.orientation.m[1] == 11 &&
           gun.orientation.m[4] == 14 && gun.orientation.m[5] == 15 &&
           gun.orientation.m[8] == 18 && gun.orientation.m[9] == 19);
    assert(gun.orientation.m[2] == -2 && gun.orientation.m[6] == -3 &&
           gun.orientation.m[10] == -4);

    Vector sentinel, ordinary;
    sentinel.x = 50000;
    ordinary.x = -1000;
    assert(!gun.isRenderable(&sentinel) && gun.isRenderable(&ordinary));

    Matrix turretRotate, turretCopy;
    gun.spinAngle = 0.75f;
    gun.applyTurretSpin(turretRotate, turretCopy);
    assert(turretRotate.rotated && turretCopy.rotated &&
           turretRotate.x == 0.0f && turretRotate.y == 3.1415927f &&
           turretRotate.z == 0.75f);

    int lifetimes[1] = {250};
    Gun lifetimeGun;
    lifetimeGun.lifetimes = lifetimes;
    gun.gun = &lifetimeGun;
    gun.applyVisibleLifetimeScaling(0);
    assert(gun.orientation.x == 0.25f && gun.orientation.y == 0.25f &&
           gun.orientation.z == 0.25f);
    gun.applyWorldLifetimeScaling(0);
    assert(gun.orientation.x == 0.25f);
    lifetimes[0] = 1000;
    lifetimeGun.hitSmall = 1;
    gun.applyWorldLifetimeScaling(0);
    assert(gun.orientation.x == 1.0f && lifetimeGun.hitSmall == 0);
    lifetimeGun.hitSmall = 1;
    gun.rocketGun = true;
    gun.applyWorldLifetimeScaling(0);
    assert(lifetimeGun.hitSmall == 1);

    gun.dir = {0, 0, 0, 1};
    gun.side = {0, 1, 0, 0};
    gun.applyBasisUp();
    assert(gun.up.x == 0 && gun.up.y == 1 && gun.up.z == 0);

    Vector wobble{0, -1.0f, 2.0f, -3.0f};
    gun.deltaTime = 100;
    gun.applyWobbleStep(&wobble);
    assert(wobble.x == -1.05f && wobble.y == 2.05f && wobble.z == -3.05f);

    gun.orientation.scaled = false;
    Globals::iPad = 0;
    gun.applyRenderScale();
    assert(!gun.orientation.scaled);
    Globals::iPad = 1;
    gun.applyRenderScale();
    assert(gun.orientation.scaled && gun.orientation.x == 0.6f &&
           gun.orientation.y == 0.7f && gun.orientation.z == 0.8f);

    EgoPlayerView playerView;
    Player player;
    Matrix egoLocal, rotatedCopy;
    gun.applyEgoRotation(&playerView, &player, egoLocal, rotatedCopy);
    assert(egoLocal.rotated && rotatedCopy.rotated);
    assert(playerView.orientation.multiplied && gun.orientation.multiplied);

    PaintCanvas beforeDraw, afterSetLocal;
    beforeDraw.replacementOnSetLocal = &afterSetLocal;
    Globals::Canvas = &beforeDraw;
    gun.renderFinal();
    assert(beforeDraw.setLocalCalls == 1 && beforeDraw.drawCalls == 0);
    assert(beforeDraw.localTransform == 17 && beforeDraw.localMarker == 42);
    assert(afterSetLocal.drawCalls == 1 && afterSetLocal.drawnTransform == 17 &&
           afterSetLocal.drawnMatrix == nullptr);
}
