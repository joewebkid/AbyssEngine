#include <cassert>
#include <cstdint>

struct Vector { float x, y, z; };
struct Matrix { float value[12]; };

Vector MatrixGetPosition(const Matrix &m) { return {m.value[0], m.value[1], m.value[2]}; }
Vector MatrixGetRight(const Matrix &m) { return {m.value[3], m.value[4], m.value[5]}; }
Vector MatrixGetUp(const Matrix &m) { return {m.value[6], m.value[7], m.value[8]}; }
Vector MatrixGetDir(const Matrix &m) { return {m.value[9], m.value[10], m.value[11]}; }

struct PaintCanvas {
    Matrix matrices[10]{};
    unsigned int lastGet = 0xffffffffu;
    unsigned int lastParent = 0xffffffffu, lastChild = 0xffffffffu;
    void *TransformGetLocal(unsigned int index) {
        assert(index < 10);
        lastGet = index;
        return &matrices[index];
    }
    void TransformAddChild(unsigned int parent, unsigned int child) {
        lastParent = parent;
        lastChild = child;
    }
};

struct AEGeometry {
    unsigned int transform = 5;
    unsigned int parentTransform = 77;
    unsigned int childTransform = 0xffffffffu;
    unsigned int altTransform = 0xffffffffu;
    PaintCanvas *canvas;
    Matrix referenceMatrix{};

    Vector getPosition();
    Vector getRightVector();
    Vector getUpVector();
    Vector getDirection();
    Vector getParentPosition();
    Matrix &getMatrix();
    void updateReferenceMatrix();
    void addChild(uint32_t child);
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
}
