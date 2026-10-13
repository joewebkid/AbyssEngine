#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

namespace AbyssEngine {
namespace AEMath {
struct Matrix { float m[15]; };
struct Vector { float x, y, z; };
}
using AEMath::Matrix;
using AEMath::Vector;

template <typename T> struct Array { unsigned size_; T *data_; };
struct Mesh { int id; };
struct LightColor { float r, g, b, a; };
struct Material {
    unsigned textures[8];
    void *addData;
    unsigned addDataSize;
    Array<Matrix> arr_2c, arr_38;
    Array<Mesh *> meshes;
    Array<unsigned> arr_50;
    Array<Matrix> arr_5c;
    Vector ambientColor;
};

static std::vector<int> calls;
static Matrix *expectedWorld;
static Matrix *expectedModel;
static Matrix *expectedUV;
static Mesh **expectedMeshes;
static unsigned productIndex;
static unsigned meshIndex;
static float seenColor[4];

namespace AEMath {
Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
    assert(&lhs == &expectedWorld[productIndex]);
    assert(&rhs == &expectedModel[productIndex]);
    Matrix result = {};
    result.m[0] = lhs.m[0] + rhs.m[0];
    ++productIndex;
    calls.push_back(4);
    return result;
}
}

struct Engine {
    LightColor lightAmbient;
    void SetTexturesExt(unsigned a, unsigned b, unsigned c, unsigned slot) {
        assert(a == 11 && b == 12 && c == 13 && slot == 0xffffffff);
        calls.push_back(1);
    }
    void SetAddData(void *data, int size) {
        assert(data == this && size == 7);
        calls.push_back(2);
    }
    void LightSetLightColorAmbient(float r, float g, float b, unsigned light) {
        assert(light == 0x4000);
        lightAmbient.r = r;
        lightAmbient.g = g;
        lightAmbient.b = b;
        calls.push_back(3);
    }
    void SetModelMatrix(const Matrix &matrix) {
        assert(&matrix == &expectedModel[meshIndex]);
        calls.push_back(6);
    }
    void SetUVMatrix(const Matrix &matrix) {
        assert(&matrix == &expectedUV[meshIndex]);
        calls.push_back(7);
    }
    void SetColor(float a, float r, float g, float b) {
        seenColor[0] = a;
        seenColor[1] = r;
        seenColor[2] = g;
        seenColor[3] = b;
        calls.push_back(8);
    }
};
struct PaintCanvas {
    void SetWorldViewMatrix(const Matrix &matrix) {
        assert(matrix.m[0] == expectedWorld[meshIndex].m[0] + expectedModel[meshIndex].m[0]);
        calls.push_back(5);
    }
};
int MeshDraw(Engine *, Mesh *mesh) {
    assert(mesh == expectedMeshes[meshIndex]);
    ++meshIndex;
    calls.push_back(9);
    return 0;
}

// PRODUCTION_METHOD
}

static void near(float actual, float expected) {
    assert(std::fabs(actual - expected) < 0.00001f);
}

int main() {
    using namespace AbyssEngine;
    Matrix models[2] = {}, uvs[2] = {}, worlds[2] = {};
    models[0].m[0] = 3; models[1].m[0] = 4;
    worlds[0].m[0] = 7; worlds[1].m[0] = 9;
    Mesh meshesData[2] = {{1}, {2}};
    Mesh *meshes[2] = {&meshesData[0], &meshesData[1]};
    unsigned colors[2] = {0x80ff4000, 0xff004080};
    Material mat = {};
    mat.textures[0] = 11; mat.textures[1] = 12; mat.textures[2] = 13;
    mat.arr_2c = {2, models}; mat.arr_38 = {2, uvs};
    mat.meshes = {2, meshes}; mat.arr_50 = {2, colors}; mat.arr_5c = {2, worlds};
    mat.ambientColor = {0.4f, 0.5f, 0.6f};
    Engine engine = {{0.1f, 0.2f, 0.3f, 1.0f}};
    mat.addData = &engine; mat.addDataSize = 7;
    PaintCanvas canvas;
    expectedWorld = worlds; expectedModel = models; expectedUV = uvs;
    expectedMeshes = meshes;

    MaterialDraw(nullptr, &engine, &mat, true);
    MaterialDraw(&canvas, &engine, nullptr, true);
    assert(calls.empty() && mat.meshes.size_ == 2);

    MaterialDraw(&canvas, &engine, &mat, true);
    assert((calls == std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9,
                                      4, 5, 6, 7, 8, 9, 3}));
    assert(productIndex == 2 && meshIndex == 2);
    near(seenColor[0], 1.0f); near(seenColor[1], 0.0f);
    near(seenColor[2], 64.0f / 255.0f); near(seenColor[3], 128.0f / 255.0f);
    near(engine.lightAmbient.r, 0.1f);
    near(engine.lightAmbient.g, 0.2f);
    near(engine.lightAmbient.b, 0.3f);
    assert(mat.arr_2c.size_ == 0 && mat.arr_38.size_ == 0);
    assert(mat.arr_5c.size_ == 0 && mat.arr_50.size_ == 0 && mat.meshes.size_ == 0);

    calls.clear(); productIndex = 0; meshIndex = 0;
    mat.arr_2c.size_ = mat.arr_38.size_ = mat.arr_5c.size_ = 1;
    mat.arr_50.size_ = mat.meshes.size_ = 1;
    mat.ambientColor.x = -10.0f;
    MaterialDraw(&canvas, &engine, &mat, false);
    assert((calls == std::vector<int>{2, 4, 5, 6, 7, 8, 9}));
    near(engine.lightAmbient.r, 0.1f);
    std::puts("MaterialDraw host smoke: OK");
}
