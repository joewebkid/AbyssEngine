#include <cassert>
#include <cmath>
#include <cstdint>

struct Vector { float x, y, z; };
struct Matrix { union { float value[15]; float m[15]; }; };

int translationCalls = 0;
float translationArgs[3]{};
Matrix MatrixSetTranslation(Matrix &m, float x, float y, float z) {
    ++translationCalls;
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

Vector MatrixGetPosition(const Matrix &m) { return {m.value[0], m.value[1], m.value[2]}; }
Vector MatrixGetRight(const Matrix &m) { return {m.value[3], m.value[4], m.value[5]}; }
Vector MatrixGetUp(const Matrix &m) { return {m.value[6], m.value[7], m.value[8]}; }
Vector MatrixGetDir(const Matrix &m) { return {m.value[9], m.value[10], m.value[11]}; }

struct PaintCanvas {
    Matrix matrices[10]{};
    unsigned int nextTransform = 7;
    unsigned int lastGet = 0xffffffffu;
    int getCalls = 0;
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
        matrices[index] = matrix;
    }
};

struct AEGeometry {
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
    Vector cameraDelta{};
    Matrix referenceMatrix{};
    void *merger = nullptr;

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
    void setRotation(float, float, float);
    void rotate(float, float, float);
    void setScaling(float, float, float);
    void translate(float, float, float);
    void translate(const Vector &);
    void setDirection(const Vector &, const Vector &);
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
    canvas.matrices[9] = {{21, 22, 23}};
    AEGeometry geom;
    geom.canvas = &canvas;

    equal(geom.getPosition(), 1, 2, 3);
    equal(geom.getRightVector(), 4, 5, 6);
    equal(geom.getUpVector(), 7, 8, 9);
    equal(geom.getDirection(), 10, 11, 12);
    assert(&geom.getMatrix() == &canvas.matrices[5] && canvas.lastGet == 5);
    geom.updateReferenceMatrix();
    assert(geom.referenceMatrix.value[11] == 12 && canvas.lastGet == 5);

    equal(geom.getParentPosition(), 1, 2, 3);
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
