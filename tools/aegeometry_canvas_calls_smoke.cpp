#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

struct Vector { float x, y, z; };
struct Matrix { union { float value[15]; float m[15]; }; };
using longlong = long long;

Vector operator-(const Vector &lhs, const Vector &rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z};
}

int translationCalls = 0;
float translationArgs[3]{};
Matrix *translationMatrix = nullptr;
Matrix MatrixSetTranslation(Matrix &m, float x, float y, float z) {
    ++translationCalls;
    translationMatrix = &m;
    translationArgs[0] = x;
    translationArgs[1] = y;
    translationArgs[2] = z;
    m.value[3] = x; m.value[7] = y; m.value[11] = z;
    return m;
}

namespace AbyssEngine { namespace AEMath {
enum RotationOrder { ROTATION_ORDER_XYZ = 0 };
int rotationCalls = 0, scalingCalls = 0;
int crossCalls = 0, normalizeCalls = 0;
Matrix *rotationMatrix = nullptr, *scalingMatrix = nullptr;
float angles[3]{}, scales[3]{};
int lastOrder = -1;
Vector VectorCross(const Vector &lhs, const Vector &rhs) {
    ++crossCalls;
    return {lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x};
}
Vector VectorNormalize(const Vector &value) {
    ++normalizeCalls;
    float length = std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
    if (length == 0.0f)
        return {0.0f, 1.0f, 0.0f};
    return {value.x / length, value.y / length, value.z / length};
}
Matrix MatrixSetRotation(Matrix &m, float x, float y, float z, RotationOrder order) {
    rotationMatrix = &m;
    angles[0] = x; angles[1] = y; angles[2] = z;
    lastOrder = order;
    ++rotationCalls;
    m.value[0] = x;
    return m;
}
Matrix MatrixSetScaling(Matrix &m, float x, float y, float z) {
    scalingMatrix = &m;
    scales[0] = x; scales[1] = y; scales[2] = z;
    ++scalingCalls;
    m.value[0] *= x; m.value[4] *= x; m.value[8] *= x;
    m.value[1] *= y; m.value[5] *= y; m.value[9] *= y;
    m.value[2] *= z; m.value[6] *= z; m.value[10] *= z;
    m.value[12] = x; m.value[13] = y; m.value[14] = z;
    return m;
}
}}

Vector MatrixGetPosition(const Matrix &m) { return {m.m[3], m.m[7], m.m[11]}; }
Vector MatrixGetRight(const Matrix &m) { return {m.m[0], m.m[4], m.m[8]}; }
Vector MatrixGetUp(const Matrix &m) { return {m.m[1], m.m[5], m.m[9]}; }
Vector MatrixGetDir(const Matrix &m) { return {m.m[2], m.m[6], m.m[10]}; }

namespace AbyssEngine {
struct Transform {
    longlong currentTime = 0;
    int setCalls = 0;
    void SetCurrentAnimationTime(longlong time) {
        currentTime = time;
        ++setCalls;
    }
};
}

struct LodMeshMerger {
    int calls = 0, lastIndex = -1, lastLod = -1;
    void setLod(int index, signed char lod) {
        ++calls;
        lastIndex = index;
        lastLod = lod;
    }
};

struct PaintCanvas {
    Matrix matrices[10]{};
    AbyssEngine::Transform transforms[10]{};
    unsigned int nextTransform = 7;
    unsigned int lastGet = 0xffffffffu;
    int getCalls = 0;
    unsigned int lastTransform = 0xffffffffu;
    int getTransformCalls = 0, setCalls = 0;
    unsigned int lastParent = 0xffffffffu, lastChild = 0xffffffffu;
    unsigned int lastSet = 0xffffffffu, lastMeshIdTransform = 0xffffffffu;
    unsigned int lastMeshId = 0xffffffffu;
    int addChildCalls = 0, removeChildCalls = 0, addMeshCalls = 0;
    int createCalls = 0, meshCreateCalls = 0;
    void TransformCreate(unsigned int &out) {
        out = nextTransform++;
        ++createCalls;
    }
    void *TransformGetLocal(unsigned int index) {
        assert(index < 10);
        lastGet = index;
        ++getCalls;
        return &matrices[index];
    }
    void *TransformGetTransform(unsigned int index) {
        assert(index < 10);
        lastTransform = index;
        ++getTransformCalls;
        return &transforms[index];
    }
    void TransformAddChild(unsigned int parent, unsigned int child) {
        lastParent = parent;
        lastChild = child;
        ++addChildCalls;
    }
    void TransformRemoveChild(unsigned int parent, unsigned int child) {
        assert(parent < 10 && child < 10);
        ++removeChildCalls;
    }
    void TransformAddMesh(unsigned int transform, unsigned short mesh, bool flag) {
        assert(transform < 10 && mesh != 0 && !flag);
        ++addMeshCalls;
    }
    void MeshCreate(unsigned short mesh, unsigned int &out, bool flag) {
        assert(mesh == 42 && flag);
        out = 55;
        ++meshCreateCalls;
    }
    void TransformAddMeshId(unsigned int transform, unsigned int meshId) {
        lastMeshIdTransform = transform;
        lastMeshId = meshId;
    }
    void TransformSetLocal(unsigned int index, const Matrix &matrix) {
        assert(index < 10);
        lastSet = index;
        ++setCalls;
        matrices[index] = matrix;
    }
};

struct AEGeometry {
    unsigned int mergerIndex = 0;
    unsigned short mesh = 0;
    unsigned int transform = 5;
    unsigned int parentTransform = 77;
    unsigned int childTransform = 0xffffffffu;
    unsigned int baseTransform = 0;
    unsigned int meshId = 0xffffffffu, meshHandle = 0xffffffffu;
    unsigned int altTransform = 0xffffffffu;
    int currentLod = 0;
    PaintCanvas *canvas = nullptr;
    Vector rotation{};
    float scalingX = 1, scalingY = 1, scalingZ = 1;
    unsigned short visibility = 0;
    int rotationOrder = 0, lodCount = 0;
    unsigned int *lodTransforms = nullptr, *lodChildTransforms = nullptr;
    unsigned short *lodMeshes = nullptr, *lodChildMeshes = nullptr;
    unsigned long long *lodDistancesSq = nullptr;
    unsigned long long distSq = 0, lastVisibleDistSq = 0;
    Vector cameraDelta{};
    Matrix referenceMatrix{};
    LodMeshMerger *merger = nullptr;

    AEGeometry() = default;
    explicit AEGeometry(PaintCanvas *);
    AEGeometry(uint16_t, PaintCanvas *, bool);

    Vector getPosition();
    Vector getRightVector();
    Vector getUpVector();
    Vector getDirection();
    Vector getParentPosition();
    Matrix &getMatrix();
    void updateReferenceMatrix();
    void addChild(uint32_t child);
    void setMatrix(const Matrix &);
    void setPosition(float, float, float);
    void setPosition(const Vector &);
    void setRotation(float, float, float);
    void rotate(float, float, float);
    void setScaling(float, float, float);
    void translate(float, float, float);
    void translate(const Vector &);
    void setDirection(const Vector &, const Vector &);
    void updateLod(const Vector &, float);
    void setLodChildMeshes(uint16_t *);
    void setLodChildTransform(uint32_t);
};

// PRODUCTION_METHODS

static void equal(Vector actual, float a, float b, float c) {
    assert(actual.x == a && actual.y == b && actual.z == c);
}

int main() {
    PaintCanvas canvas;
    canvas.matrices[5] = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}};
    canvas.matrices[9] = {{0, 0, 0, 21, 0, 0, 0, 22, 0, 0, 0, 23}};
    AEGeometry geom;
    geom.canvas = &canvas;

    equal(geom.getPosition(), 4, 8, 12);
    equal(geom.getRightVector(), 1, 5, 9);
    equal(geom.getUpVector(), 2, 6, 10);
    equal(geom.getDirection(), 3, 7, 11);
    assert(&geom.getMatrix() == &canvas.matrices[5] && canvas.lastGet == 5);
    geom.updateReferenceMatrix();
    assert(geom.referenceMatrix.value[11] == 12 && canvas.lastGet == 5);

    equal(geom.getParentPosition(), 4, 8, 12);
    assert(canvas.lastGet == 5);
    geom.altTransform = 9;
    equal(geom.getParentPosition(), 21, 22, 23);
    assert(canvas.lastGet == 9);

    geom.addChild(12);
    assert(canvas.lastParent == 5 && canvas.lastChild == 12);
    assert(geom.parentTransform == 77 && geom.childTransform == 12);
    geom.addChild(13);
    assert(canvas.lastParent == 5 && canvas.lastChild == 13);
    assert(geom.parentTransform == 12 && geom.childTransform == 13);

    Matrix replacement{{31, 32, 33}};
    geom.setMatrix(replacement);
    assert(canvas.lastSet == 5 && canvas.matrices[5].value[0] == 31);

    int initialGets = canvas.getCalls;
    geom.rotationOrder = 0;
    geom.setRotation(0.5f, 1.0f, 1.5f);
    assert(canvas.getCalls == initialGets + 2 && canvas.lastGet == 5);
    assert(AbyssEngine::AEMath::rotationCalls == 1 &&
           AbyssEngine::AEMath::scalingCalls == 1);
    assert(AbyssEngine::AEMath::rotationMatrix == &canvas.matrices[5] &&
           AbyssEngine::AEMath::scalingMatrix == &canvas.matrices[5]);
    assert(AbyssEngine::AEMath::angles[0] == 0.5f &&
           AbyssEngine::AEMath::angles[1] == 1.0f &&
           AbyssEngine::AEMath::angles[2] == 1.5f);
    assert(AbyssEngine::AEMath::lastOrder == 0 &&
           canvas.matrices[5].value[12] == 1.0f);

    geom.rotate(0.25f, 0.5f, 0.75f);
    assert(geom.rotation.x == 0.75f && geom.rotation.y == 1.5f &&
           geom.rotation.z == 2.25f);
    assert(canvas.getCalls == initialGets + 4 &&
           AbyssEngine::AEMath::rotationCalls == 2 &&
           AbyssEngine::AEMath::scalingCalls == 2);
    assert(AbyssEngine::AEMath::angles[0] == 0.75f &&
           AbyssEngine::AEMath::angles[1] == 1.5f &&
           AbyssEngine::AEMath::angles[2] == 2.25f);

    geom.setScaling(2.0f, 3.0f, 4.0f);
    assert(canvas.getCalls == initialGets + 6 &&
           AbyssEngine::AEMath::rotationCalls == 3 &&
           AbyssEngine::AEMath::scalingCalls == 3);
    assert(AbyssEngine::AEMath::angles[0] == 0.75f &&
           AbyssEngine::AEMath::scales[0] == 2.0f &&
           AbyssEngine::AEMath::scales[1] == 3.0f &&
           AbyssEngine::AEMath::scales[2] == 4.0f);
    assert(geom.scalingX == 2.0f && geom.scalingY == 3.0f &&
           geom.scalingZ == 4.0f);

    canvas.matrices[5].m[3] = 10.0f;
    canvas.matrices[5].m[7] = 20.0f;
    canvas.matrices[5].m[11] = 30.0f;
    geom.translate(1.0f, 2.0f, 3.0f);
    assert(translationCalls == 1 && canvas.lastGet == 5 && canvas.lastSet == 5);
    assert(translationArgs[0] == 11.0f && translationArgs[1] == 22.0f &&
           translationArgs[2] == 33.0f);
    assert(canvas.matrices[5].m[3] == 11.0f &&
           canvas.matrices[5].m[7] == 22.0f && canvas.matrices[5].m[11] == 33.0f);
    geom.translate(Vector{2.0f, 4.0f, 6.0f});
    assert(translationCalls == 2 && translationArgs[0] == 13.0f &&
           translationArgs[1] == 26.0f && translationArgs[2] == 39.0f);

    int getsBeforePosition = canvas.getCalls;
    int setsBeforePosition = canvas.setCalls;
    geom.setPosition(41.0f, 42.0f, 43.0f);
    assert(canvas.getCalls == getsBeforePosition + 1 && canvas.lastGet == 5);
    assert(canvas.setCalls == setsBeforePosition);
    assert(translationCalls == 3 && translationMatrix == &canvas.matrices[5]);
    assert(translationArgs[0] == 41.0f && translationArgs[1] == 42.0f &&
           translationArgs[2] == 43.0f);
    equal(geom.getPosition(), 41.0f, 42.0f, 43.0f);

    getsBeforePosition = canvas.getCalls;
    geom.setPosition(Vector{7.0f, 8.0f, 9.0f});
    assert(canvas.getCalls == getsBeforePosition + 1 && canvas.lastGet == 5);
    assert(canvas.setCalls == setsBeforePosition);
    assert(translationCalls == 4 && translationMatrix == &canvas.matrices[5]);
    assert(translationArgs[0] == 7.0f && translationArgs[1] == 8.0f &&
           translationArgs[2] == 9.0f);
    equal(geom.getPosition(), 7.0f, 8.0f, 9.0f);

    PaintCanvas directionCanvas;
    directionCanvas.matrices[5].m[3] = 11.0f;
    directionCanvas.matrices[5].m[7] = 22.0f;
    directionCanvas.matrices[5].m[11] = 33.0f;
    AEGeometry directionGeom;
    directionGeom.canvas = &directionCanvas;
    directionGeom.scalingX = 2.0f;
    directionGeom.scalingY = 3.0f;
    directionGeom.scalingZ = 4.0f;
    int initialCrosses = AbyssEngine::AEMath::crossCalls;
    int initialNormalizes = AbyssEngine::AEMath::normalizeCalls;
    int initialScalings = AbyssEngine::AEMath::scalingCalls;
    directionGeom.setDirection(Vector{2.0f, 0.0f, 0.0f}, Vector{0.0f, 3.0f, 0.0f});
    assert(directionCanvas.getCalls == 2 && directionCanvas.lastSet == 5);
    assert(AbyssEngine::AEMath::crossCalls == initialCrosses + 2 &&
           AbyssEngine::AEMath::normalizeCalls == initialNormalizes + 2 &&
           AbyssEngine::AEMath::scalingCalls == initialScalings + 1);
    assert(AbyssEngine::AEMath::scalingMatrix == &directionCanvas.matrices[5]);
    const Matrix &facing = directionCanvas.matrices[5];
    assert(facing.m[0] == 0.0f && facing.m[4] == 0.0f && facing.m[8] == -2.0f);
    assert(facing.m[1] == 0.0f && facing.m[5] == 3.0f && facing.m[9] == 0.0f);
    assert(facing.m[2] == 8.0f && facing.m[6] == 0.0f && facing.m[10] == 0.0f);
    assert(facing.m[3] == 11.0f && facing.m[7] == 22.0f && facing.m[11] == 33.0f);
    assert(facing.m[12] == 2.0f && facing.m[13] == 3.0f && facing.m[14] == 4.0f);
    directionGeom.setDirection(Vector{0.0f, 0.0f, 1.0f}, Vector{0.0f, 0.0f, 0.0f});
    assert(directionCanvas.getCalls == 4 && directionCanvas.lastSet == 5);
    assert(AbyssEngine::AEMath::crossCalls == initialCrosses + 4 &&
           AbyssEngine::AEMath::normalizeCalls == initialNormalizes + 4 &&
           AbyssEngine::AEMath::scalingCalls == initialScalings + 2);
    assert(facing.m[0] == 0.0f && facing.m[4] == 2.0f && facing.m[8] == 0.0f);
    assert(facing.m[1] == -3.0f && facing.m[5] == 0.0f && facing.m[9] == 0.0f);
    assert(facing.m[2] == 0.0f && facing.m[6] == 0.0f && facing.m[10] == 4.0f);
    assert(facing.m[3] == 11.0f && facing.m[7] == 22.0f && facing.m[11] == 33.0f);

    PaintCanvas lodCanvas;
    lodCanvas.matrices[5].m[0] = 9.0f;
    lodCanvas.matrices[5].m[3] = 50.0f;
    lodCanvas.matrices[5].m[7] = 60.0f;
    lodCanvas.matrices[5].m[11] = 70.0f;
    lodCanvas.matrices[7].m[3] = 16.0f;
    lodCanvas.transforms[5].currentTime = 1234;
    unsigned int lodHandles[] = {6, 8};
    unsigned long long lodDistances[] = {100, 400};
    LodMeshMerger lodMerger;
    AEGeometry lodGeom;
    lodGeom.canvas = &lodCanvas;
    lodGeom.transform = 5;
    lodGeom.baseTransform = 3;
    lodGeom.altTransform = 7;
    lodGeom.visibility = 0x101;
    lodGeom.lodCount = 2;
    lodGeom.lodTransforms = lodHandles;
    lodGeom.lodDistancesSq = lodDistances;
    lodGeom.mergerIndex = 23;
    lodGeom.merger = &lodMerger;
    Vector cameraPosition{1.0f, 0.0f, 0.0f};

    lodGeom.updateLod(cameraPosition, 0.33f);
    assert(lodGeom.cameraDelta.x == 15.0f && lodGeom.cameraDelta.y == 0.0f &&
           lodGeom.distSq == 225 && (lodGeom.visibility & 0xff) == 1);
    assert(lodCanvas.lastGet == 7 && lodCanvas.lastSet == 8 && lodCanvas.setCalls == 1);
    assert(lodGeom.transform == 8 && lodGeom.currentLod == 2);
    assert(lodCanvas.matrices[8].m[0] == 9.0f && lodCanvas.matrices[8].m[3] == 50.0f);
    assert(lodGeom.referenceMatrix.m[3] == 50.0f);
    assert(lodCanvas.getTransformCalls == 3 && lodCanvas.lastTransform == 3);
    assert(lodCanvas.transforms[8].currentTime == 1234 &&
           lodCanvas.transforms[3].currentTime == 1234);
    assert(lodMerger.calls == 1 && lodMerger.lastIndex == 23 && lodMerger.lastLod == 2);

    lodGeom.updateLod(cameraPosition, 0.33f);
    assert(lodGeom.transform == 8 && lodGeom.currentLod == 2);
    assert(lodCanvas.setCalls == 1 && lodMerger.calls == 1 &&
           lodCanvas.transforms[8].setCalls == 1);

    lodGeom.updateLod(cameraPosition, 0.34f);
    assert(lodGeom.transform == 6 && lodGeom.currentLod == 1 &&
           lodCanvas.lastSet == 6 && lodCanvas.setCalls == 2);
    assert(lodCanvas.transforms[6].currentTime == 1234 &&
           lodCanvas.transforms[3].setCalls == 2);
    assert(lodMerger.calls == 2 && lodMerger.lastLod == 1);

    lodCanvas.matrices[7].m[3] = 1.0f;
    int baseTimeCalls = lodCanvas.transforms[3].setCalls;
    lodGeom.updateLod(cameraPosition, 0.8f);
    assert(lodGeom.transform == 3 && lodGeom.currentLod == 0 && lodGeom.distSq == 0);
    assert(lodCanvas.lastSet == 3 && lodCanvas.matrices[3].m[3] == 50.0f);
    assert(lodCanvas.transforms[3].setCalls == baseTimeCalls);
    assert(lodMerger.calls == 3 && lodMerger.lastLod == 0);

    lodCanvas.matrices[7].m[3] = 16.0f;
    lodGeom.lastVisibleDistSq = 25;
    int priorTransformCalls = lodCanvas.getTransformCalls;
    int priorSetCalls = lodCanvas.setCalls;
    lodGeom.updateLod(cameraPosition, 0.8f);
    assert(lodGeom.currentLod == -1 && (lodGeom.visibility & 0xff) == 0);
    assert(lodCanvas.getTransformCalls == priorTransformCalls &&
           lodCanvas.setCalls == priorSetCalls && lodMerger.calls == 3);

    lodGeom.altTransform = 0xffffffffu;
    lodGeom.transform = 5;
    lodGeom.updateLod(cameraPosition, 0.8f);
    assert(lodCanvas.lastGet == 5 && lodGeom.cameraDelta.x == 49.0f &&
           lodGeom.cameraDelta.y == 60.0f && lodGeom.cameraDelta.z == 70.0f);
    assert(lodGeom.distSq == 10901 && lodCanvas.getTransformCalls == priorTransformCalls);

    lodGeom.altTransform = 7;
    lodGeom.lastVisibleDistSq = 0;
    lodGeom.visibility = 0x101;
    lodCanvas.matrices[7].m[3] = 20.0f;
    lodCanvas.transforms[5].currentTime = 3333;
    lodGeom.updateLod(cameraPosition, 0.66f);
    assert(lodGeom.distSq == 361 && lodGeom.transform == 8 && lodGeom.currentLod == 2);
    assert(lodCanvas.transforms[8].currentTime == 3333 && lodMerger.calls == 4);
    lodGeom.updateLod(cameraPosition, 0.67f);
    assert(lodGeom.transform == 6 && lodGeom.currentLod == 1 &&
           lodCanvas.transforms[6].currentTime == 3333 && lodMerger.calls == 5);
    lodGeom.updateLod(cameraPosition, std::numeric_limits<float>::quiet_NaN());
    assert(lodGeom.transform == 6 && lodGeom.currentLod == 1 && lodMerger.calls == 5);
    lodGeom.lastVisibleDistSq = 400;
    lodGeom.updateLod(cameraPosition, 0.67f);
    assert((lodGeom.visibility & 0xff) == 1 && lodGeom.transform == 6 &&
           lodGeom.currentLod == 1 && lodMerger.calls == 5);

    PaintCanvas ctorCanvas;
    ctorCanvas.matrices[7].value[0] = 71;
    AEGeometry plain(&ctorCanvas);
    assert(plain.canvas == &ctorCanvas && plain.baseTransform == 7);
    assert(plain.transform == 7 && ctorCanvas.createCalls == 1);

    PaintCanvas meshCanvas;
    meshCanvas.nextTransform = 8;
    meshCanvas.matrices[8].value[0] = 88;
    AEGeometry withMesh(42, &meshCanvas, true);
    assert(withMesh.baseTransform == 8 && withMesh.transform == 8);
    assert(withMesh.meshId == 55 && meshCanvas.meshCreateCalls == 1);
    assert(meshCanvas.lastMeshIdTransform == 8 && meshCanvas.lastMeshId == 55);
    assert(meshCanvas.lastGet == 8 && withMesh.referenceMatrix.value[0] == 88);

    unsigned int lodTransforms[] = {3, 4};
    unsigned short lodMeshes[] = {21, 22};
    geom.lodCount = 2;
    geom.lodTransforms = lodTransforms;
    geom.childTransform = 6;
    canvas.nextTransform = 7;
    geom.setLodChildMeshes(lodMeshes);
    assert(geom.lodChildMeshes[0] == 21 && geom.lodChildMeshes[1] == 22);
    assert(geom.lodChildTransforms[0] == 7 && geom.lodChildTransforms[1] == 8);
    assert(canvas.createCalls == 2 && canvas.addMeshCalls == 2);
    assert(canvas.removeChildCalls == 2 && canvas.lastParent == 4 && canvas.lastChild == 6);
    delete[] geom.lodChildMeshes;
    delete[] geom.lodChildTransforms;

    AEGeometry childGeom;
    childGeom.canvas = &canvas;
    childGeom.lodCount = 2;
    childGeom.lodTransforms = lodTransforms;
    childGeom.setLodChildTransform(9);
    assert(canvas.lastParent == 4 && canvas.lastChild == 9);
    delete[] childGeom.lodChildTransforms;
}
