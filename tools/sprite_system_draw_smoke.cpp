#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "engine/core/Array.h"
#include "engine/math/Matrix.h"

namespace AbyssEngine {
namespace AEMath {
Matrix::Matrix() : m{} {}
Matrix::operator const float *() const { return m; }
Matrix &Matrix::operator=(const Matrix &other) {
    std::memcpy(m, other.m, sizeof(m));
    return *this;
}
Matrix operator*(const Matrix &lhs, const Matrix &rhs);
}
using AEMath::Matrix;

struct Material {
    Array<Matrix> arr_2c, arr_38, arr_5c;
    Array<struct Mesh *> meshes;
    Array<unsigned> arr_50;
};
struct Mesh { void *positions; void *material; };
struct SpriteSystem {
    unsigned short count;
    float *posCpu;
    short *sizeCpu;
    unsigned char sharedSize;
    Mesh *mesh;
};
struct Engine {};

static int products;
static int directDraws;
namespace AEMath {
Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
    assert(lhs.m[0] == 7 && rhs.m[3] == 10);
    ++products;
    return Matrix();
}
}
int MeshDraw(Engine *, Mesh *) { ++directDraws; return 0; }

// PRODUCTION_METHOD
}

static void near(float actual, float expected) {
    assert(std::fabs(actual - expected) < 0.00001f);
}

int main() {
    using namespace AbyssEngine;
    Matrix view;
    view.m[0] = 7;
    Matrix world;
    world.m[0] = world.m[5] = world.m[10] = 1;
    world.m[3] = 10; world.m[7] = 20; world.m[11] = 30;
    float positions[] = {2, 3, 4, -1, 0, 2};
    short sizes[] = {5, 9};
    float vertices[24] = {};
    Material material;
    Mesh mesh = {vertices, &material};
    SpriteSystem sys = {2, positions, sizes, 0, &mesh};
    Engine engine;

    SpriteSystemDraw(&engine, view, world, nullptr);
    assert(products == 0);
    SpriteSystemDraw(&engine, view, world, &sys);
    assert(products == 1 && directDraws == 0);
    near(vertices[0], 10); near(vertices[1], 21); near(vertices[2], 34);
    near(vertices[3], 14); near(vertices[7], 25);
    near(vertices[12], 5); near(vertices[13], 16); near(vertices[14], 32);
    near(vertices[15], 13); near(vertices[19], 24);
    assert(material.meshes.size_ == 1 && material.meshes[0] == &mesh);
    assert(material.arr_2c.size_ == 1 && material.arr_2c[0].m[0] == 7);
    assert(material.arr_38.size_ == 1 && material.arr_38[0].m[5] == -1);
    assert(material.arr_38[0].m[12] == 1);
    assert(material.arr_5c.size_ == 1 && material.arr_5c[0].m[3] == 10);
    assert(material.arr_50.size_ == 1 && material.arr_50[0] == 0xffffffff);

    sys.sharedSize = 1;
    SpriteSystemDraw(&engine, view, world, &sys);
    assert(products == 2 && directDraws == 0);
    near(vertices[12], 7); near(vertices[13], 18);
    near(vertices[15], 11); near(vertices[19], 22);
    assert(material.meshes.size_ == 2 && material.meshes.capacity_ == 2);
    assert(material.arr_2c.size_ == 2 && material.arr_2c[1].m[0] == 7);
    assert(material.arr_38.size_ == 2 && material.arr_38[1].m[5] == -1);
    assert(material.arr_5c.size_ == 2 && material.arr_5c[1].m[3] == 10);
    assert(material.arr_50.size_ == 2 && material.arr_50[1] == 0xffffffff);

    mesh.material = nullptr;
    SpriteSystemDraw(&engine, view, world, &sys);
    assert(products == 3 && directDraws == 1);
    std::puts("SpriteSystemDraw host smoke: OK");
}
