#include <cassert>
#include <cstdint>

struct Vector { float x, y, z; };
struct Matrix { float value[15]; };

namespace AbyssEngine { namespace AEMath {
enum RotationOrder { ROTATION_ORDER_XYZ = 0 };
int rotationCalls = 0, scalingCalls = 0;
Matrix *rotationMatrix = nullptr, *scalingMatrix = nullptr;
float angles[3]{}, scales[3]{};
int lastOrder = -1;
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
