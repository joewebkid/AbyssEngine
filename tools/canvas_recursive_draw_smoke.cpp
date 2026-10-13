#include <cassert>
#include <cstdio>

template <typename T> struct Array {
    unsigned count = 0;
    T *data_ = nullptr;
    unsigned size() const { return count; }
    T &operator[](unsigned index) { return data_[index]; }
};

namespace AbyssEngine {
namespace AEMath {
struct Matrix {
    float m[15] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1};
    Matrix &operator*=(const Matrix &rhs) {
        m[3] += rhs.m[3];
        return *this;
    }
};
Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
    Matrix result = lhs;
    result *= rhs;
    return result;
}
}
using AEMath::Matrix;
struct Camera {};
struct Mesh { int marker; };
static int frustumCalls = 0;
static float lastFrustumWorldX = 0.0f;
struct Transform {
    Matrix worldMatrix;
    Array<Mesh *> meshes;
    unsigned id = 0;
    Array<Transform *> children;
    Matrix rotationMatrix;
    Matrix localMatrix;
    Array<int> keyFrames;
    bool visible = true;
    bool inFrustum = true;
    int InCameraVF(Matrix *world, Camera *) {
        ++frustumCalls;
        lastFrustumWorldX = world->m[3];
        return inFrustum;
    }
};
static int meshCalls = 0;
static int meshMarkers[6] = {};
static float worldXs[6] = {};
static float viewXs[6] = {};
static unsigned colors[6] = {};
static Matrix *uvMatrices[6] = {};
struct PaintCanvas {
    Array<Camera *> cameras;
    unsigned currentCamera = 0;
    int culledCount = 0;
    void DrawMesh(Mesh *mesh, Matrix &world, Matrix &view,
                  unsigned color, Matrix &uv) {
        assert(meshCalls < 6);
        meshMarkers[meshCalls] = mesh->marker;
        worldXs[meshCalls] = world.m[3];
        viewXs[meshCalls] = view.m[3];
        colors[meshCalls] = color;
        uvMatrices[meshCalls] = &uv;
        ++meshCalls;
    }
    void DrawTransform(Transform *tf, const Matrix &view, Matrix &world);
};

// PRODUCTION_METHOD
}

int main() {
    using namespace AbyssEngine;
    Matrix parent, world;
    parent.m[3] = 10.0f;
    world.m[3] = 20.0f;
    Camera camera;
    Camera *cameras[] = {&camera};
    Mesh rootMesh{1}, childMesh{2};
    Mesh *rootMeshes[] = {&rootMesh};
    Mesh *childMeshes[] = {&childMesh};
    Transform root, child, culled;
    root.worldMatrix.m[3] = 2.0f;
    root.rotationMatrix.m[3] = 4.0f;
    root.meshes = {1, rootMeshes};
    root.id = 42;
    child.worldMatrix.m[3] = 3.0f;
    child.meshes = {1, childMeshes};
    child.id = 17;
    culled.inFrustum = false;
    Transform *children[] = {&child, &culled};
    root.children = {2, children};
    PaintCanvas canvas;
    canvas.cameras = {1, cameras};

    canvas.DrawTransform(nullptr, parent, world);
    root.visible = false;
    canvas.DrawTransform(&root, parent, world);
    assert(meshCalls == 0 && frustumCalls == 0 && canvas.culledCount == 0);
    root.visible = true;
    canvas.DrawTransform(&root, parent, world);
    assert(meshCalls == 2 && meshMarkers[0] == 1 && meshMarkers[1] == 2);
    assert(worldXs[0] == 12.0f && worldXs[1] == 15.0f);
    assert(viewXs[0] == 20.0f && viewXs[1] == 20.0f);
    assert(colors[0] == 42 && colors[1] == 17);
    assert(uvMatrices[0] == &root.localMatrix && uvMatrices[1] == &child.localMatrix);
    assert(frustumCalls == 2 && lastFrustumWorldX == 12.0f);
    assert(canvas.culledCount == 1);

    canvas.cameras.count = 0;
    canvas.DrawTransform(&root, parent, world);
    assert(meshCalls == 3 && frustumCalls == 2 && canvas.culledCount == 3);
    int keyframe = 0;
    root.keyFrames = {1, &keyframe};
    canvas.DrawTransform(&root, parent, world);
    assert(meshCalls == 4 && worldXs[3] == 16.0f && canvas.culledCount == 5);
    std::puts("PaintCanvas recursive DrawTransform smoke: OK");
}
