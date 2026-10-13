#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

template <typename T> struct Array {
    std::vector<T> values;
    unsigned count = 0;
    T *data_ = nullptr;
    unsigned size() const { return static_cast<unsigned>(values.size()); }
    T &operator[](unsigned i) { return values[i]; }
};
template <typename T> void ArrayAddCached(T item, Array<T> &array) {
    array.values.push_back(item);
}

namespace AbyssEngine { struct PaintCanvas; }
int paintcanvas_ext_transform_tricount(AbyssEngine::PaintCanvas *, void *);

namespace AbyssEngine {
namespace AEMath {
struct Vector { float x, y, z; };
struct Matrix {
    float m[15] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1};
};
Matrix operator*(const Matrix &a, const Matrix &b) {
    Matrix out;
    out.m[0] = a.m[0] * b.m[0];
    out.m[3] = a.m[3] + a.m[0] * b.m[3];
    out.m[7] = a.m[7] + b.m[7];
    out.m[11] = a.m[11] + b.m[11];
    return out;
}
Matrix MatrixSetTranslation(Matrix &m, float x, float y, float z) {
    m.m[3] = x; m.m[7] = y; m.m[11] = z; return m;
}
Matrix MatrixSetTranslation(Matrix &m, const Vector &v) {
    return MatrixSetTranslation(m, v.x, v.y, v.z);
}
Vector MatrixGetPosition(const Matrix &m) { return {m.m[3], m.m[7], m.m[11]}; }
Vector MatrixTransformVector(const Matrix &m, const Vector &v) {
    return {m.m[0] * v.x + m.m[3], v.y + m.m[7], v.z + m.m[11]};
}
Vector MatrixRotateVector(const Matrix &m, const Vector &v) {
    return {m.m[0] * v.x, v.y, v.z};
}
}
using AEMath::Matrix;
using AEMath::Vector;
struct Camera {};
struct Mesh;
struct Transform {
    Matrix rotationMatrix, localMatrix;
    unsigned id = 0xffffffff;
    Array<Mesh *> meshes;
    Array<Transform *> children;
    float checkedWorldX = -999;
    bool visible = true;
    int InCameraVF(Matrix *world, Camera *) {
        checkedWorldX = world->m[3];
        return visible;
    }
};
struct Material {
    Array<Mesh *> meshes;
    Array<Matrix> arr_2c, arr_38, arr_5c;
    Array<unsigned> arr_50;
};
struct Mesh {
    int marker = 0;
    unsigned vertexCount = 1;
    unsigned indexCount = 0;
    unsigned materialId = 0;
    float shaderAnimValue0 = 0, shaderAnimValue1 = 0;
    Transform *animation = nullptr;
    void *material = nullptr;
    float pivotX = 0, pivotY = 0, pivotZ = 0;
    float boundsCenterX = 1, boundsCenterY = 0, boundsCenterZ = 0;
    float boundsRadius = 2, boundsRadiusSq = 3;
};
struct Engine {
    float rgba[4] = {};
    Matrix model, uv;
    int setModel = 0, setUV = 0, resetUV = 0, draws = 0;
    void SetColor(float r, float g, float b, float a) {
        rgba[0] = r; rgba[1] = g; rgba[2] = b; rgba[3] = a;
    }
    void SetModelMatrix(const Matrix &m) { model = m; ++setModel; }
    void SetUVMatrix(const Matrix &m) { uv = m; ++setUV; }
    void ResetUVMatrix() { ++resetUV; }
};
static std::vector<int> meshOrder;
static std::vector<int> transformOrder;
void MeshDraw(Engine *engine, Mesh *mesh) {
    ++engine->draws;
    meshOrder.push_back(mesh->marker);
}
struct PaintCanvas {
    Engine *engine = nullptr;
    float colorR = 1, colorG = 1, colorB = 1, colorA = 1;
    int culledCount = 0;
    bool inFrustum = true;
    Vector checkedCenter{};
    float checkedRadius = -1;
    int frustumChecks = 0;
    Matrix worldView;
    Array<Camera *> cameras;
    unsigned currentCamera = 0;
    unsigned transformCount = 0;
    char **transforms = nullptr;
    int CameraIsSphereinViewFrustum(const Vector &center, float radius) {
        checkedCenter = center; checkedRadius = radius; ++frustumChecks;
        return inFrustum;
    }
    void SetWorldViewMatrix(const Matrix &m) { worldView = m; }
    void DrawTransform(Transform *tf, const Matrix &world, Matrix &view) {
        transformOrder.push_back(static_cast<int>(tf->id));
        assert(world.m[3] == 13);
        assert(view.m[3] == 7);
    }
    void DrawMesh(Mesh *, Matrix &, Matrix &, unsigned, Matrix &);
    int MeshGetTriCount(Mesh *);
    int TransformGetTriCount(unsigned);
    int TransformGetTriCount(Transform *);
    void MeshChangeShaderAnimValue(Mesh *, float, unsigned);
    void MeshChangeShaderAnimValue(Transform *, float, unsigned);
};

// PRODUCTION_COLOR
// PRODUCTION_METHOD
// PRODUCTION_TRI_METHODS
// PRODUCTION_SHADER_METHODS
}
// PRODUCTION_TRI_THUNK

int main() {
    using namespace AbyssEngine;
    Engine engine;
    PaintCanvas canvas;
    canvas.engine = &engine;
    canvas.colorR = 0.5f;
    Matrix world, view, uv;
    world.m[0] = 2; world.m[3] = 5;
    view.m[0] = 3; view.m[3] = 7;
    uv.m[3] = 9;
    Mesh mesh;
    mesh.marker = 1;
    canvas.DrawMesh(&mesh, world, view, 0x80402010, uv);
    assert(engine.draws == 1 && engine.setModel == 1 && engine.setUV == 1 && engine.resetUV == 1);
    assert(canvas.checkedCenter.x == 7 && canvas.checkedRadius == 12);
    assert(canvas.worldView.m[3] == 22 && engine.model.m[3] == 5 && engine.uv.m[3] == 9);
    assert(std::fabs(engine.rgba[0] - 0.5f * 128 / 255) < 0.0001f);
    assert(std::fabs(engine.rgba[1] - 64.0f / 255) < 0.0001f);

    canvas.inFrustum = false;
    canvas.DrawMesh(&mesh, world, view, 0xffffffff, uv);
    assert(canvas.culledCount == 1 && engine.draws == 1);
    canvas.inFrustum = true;
    Material material;
    mesh.material = &material;
    canvas.DrawMesh(&mesh, world, view, 0x12345678, uv);
    assert(material.meshes.values[0] == &mesh && material.arr_2c.values[0].m[3] == 5);
    assert(material.arr_38.values[0].m[3] == 9 && material.arr_5c.values[0].m[3] == 7);
    assert(material.arr_50.values[0] == 0x12345678 && engine.draws == 1);

    mesh.material = nullptr;
    mesh.vertexCount = 0;
    Transform animation, child;
    Mesh first, second;
    first.marker = 2; second.marker = 3;
    first.vertexCount = second.vertexCount = 0;
    animation.rotationMatrix.m[3] = 4;
    animation.localMatrix.m[3] = 11;
    animation.id = 0xffffffff;
    child.id = 42;
    animation.meshes.values = {&first, &second};
    animation.children.values = {&child};
    mesh.animation = &animation;
    mesh.pivotX = 1;
    Camera camera;
    Camera *cameraPtr = &camera;
    canvas.cameras.count = 1;
    canvas.cameras.data_ = &cameraPtr;
    int checks = canvas.frustumChecks;
    canvas.DrawMesh(&mesh, world, view, 0xffffffff, uv);
    assert(canvas.frustumChecks == checks);
    assert(child.checkedWorldX == 5);
    assert(transformOrder.size() == 1 && transformOrder[0] == 42);
    assert(engine.draws == 1);

    first.vertexCount = second.vertexCount = 1;
    child.visible = false;
    canvas.DrawMesh(&mesh, world, view, 0xffffffff, uv);
    assert(meshOrder.size() == 3 && meshOrder[1] == 3 && meshOrder[2] == 2);
    assert(engine.model.m[3] == 13 && canvas.worldView.m[3] == 46);
    assert(engine.uv.m[3] == 11 && engine.resetUV == 3);
    assert(canvas.frustumChecks == checks + 2);

    animation.meshes.values.clear();
    animation.children.values.clear();
    animation.id = 0x80808080;
    mesh.vertexCount = 1;
    canvas.DrawMesh(&mesh, world, view, 0xffffffff, uv);
    assert(canvas.checkedCenter.x == 15 && canvas.checkedRadius == 12);
    assert(std::fabs(engine.rgba[0] - 0.5f * 127 / 255) < 0.0001f);
    assert(std::fabs(engine.rgba[3] - 127.0f / 255) < 0.0001f);

    Mesh outer, inner, leaf;
    Transform root, branch;
    outer.indexCount = 12;
    inner.indexCount = 6;
    leaf.indexCount = 9;
    outer.animation = &root;
    root.meshes.values = {&inner};
    root.children.values = {&branch};
    branch.meshes.values = {&leaf};
    char *transforms[] = {reinterpret_cast<char *>(&root)};
    canvas.transforms = transforms;
    canvas.transformCount = 1;
    assert(canvas.MeshGetTriCount(nullptr) == 0);
    assert(canvas.TransformGetTriCount(static_cast<Transform *>(nullptr)) == 0);
    assert(canvas.MeshGetTriCount(&outer) == 9);
    assert(canvas.TransformGetTriCount(0u) == 5);
    assert(canvas.TransformGetTriCount(1u) == 0);

    canvas.MeshChangeShaderAnimValue(&outer, 2.25f, 2);
    assert(outer.shaderAnimValue0 == 2.25f && inner.shaderAnimValue0 == 2.25f);
    assert(leaf.shaderAnimValue0 == 2.25f && outer.shaderAnimValue1 == 0);
    canvas.MeshChangeShaderAnimValue(&outer, 3.5f, 4);
    assert(outer.shaderAnimValue1 == 3.5f && inner.shaderAnimValue1 == 3.5f);
    assert(leaf.shaderAnimValue1 == 3.5f);
    float encoded = 1.75f;
    unsigned bits = 0;
    std::memcpy(&bits, &encoded, sizeof(bits));
    canvas.MeshChangeShaderAnimValue(&outer, encoded, 1);
    assert(outer.materialId == bits && inner.materialId == bits && leaf.materialId == bits);
    canvas.MeshChangeShaderAnimValue(static_cast<Mesh *>(nullptr), encoded, 1);
    canvas.MeshChangeShaderAnimValue(static_cast<Transform *>(nullptr), encoded, 1);
    canvas.MeshChangeShaderAnimValue(&outer, 9.0f, 7);
    assert(outer.shaderAnimValue0 == 2.25f && leaf.shaderAnimValue1 == 3.5f);
    std::puts("PaintCanvas mesh draw, tri-count, and shader-animation smoke: OK");
}
