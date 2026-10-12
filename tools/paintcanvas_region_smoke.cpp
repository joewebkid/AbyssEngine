#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

struct PCMeshView {
    char *positions;
    char *uvs;
    char *indexBuffer;
};
struct PCImage2DView {
    void *mesh;
    unsigned textureId;
    unsigned short regionA, regionB, u0, v0;
    unsigned char restoreFlag;
};
struct ImageList { unsigned count; void **data_; };
struct Engine {};
struct Mesh {};
struct Matrix {
    float m[15];
    Matrix(float a0, float a1, float a2, float a3,
           float a4, float a5, float a6, float a7,
           float a8, float a9, float a10, float a11,
           float a12, float a13, float a14)
        : m{a0, a1, a2, a3, a4, a5, a6, a7,
            a8, a9, a10, a11, a12, a13, a14} {}
};
namespace AEMath {
float Sinf(float angle) { return std::sin(angle); }
float Cosf(float angle) { return std::cos(angle); }
}
struct PaintCanvas {
    ImageList images;
    Engine *engine;
    void SetTexture(unsigned tex, unsigned slot);
    void SetWorldViewMatrix(const Matrix &matrix);
    void DrawRegion2D(unsigned int index, int srcX, int srcY,
                      int destW, int destH, float angle, int pivotX, int pivotY, int x, int y);
};

static std::vector<int> calls;
static float world[15];
static Engine *expectedEngine;
static Mesh *expectedMesh;

void PaintCanvas::SetTexture(unsigned tex, unsigned slot) {
    assert(tex == 42 && slot == 0xffffffff);
    calls.push_back(1);
}
void PaintCanvas::SetWorldViewMatrix(const Matrix &matrix) {
    std::memcpy(world, matrix.m, sizeof(world));
    calls.push_back(2);
}
void glDisable(unsigned cap) {
    assert(cap == 0xb44);
    calls.push_back(3);
}
int MeshDraw(Engine *engine, Mesh *mesh) {
    assert(engine == expectedEngine && mesh == expectedMesh);
    calls.push_back(4);
    return 0;
}
void glEnable(unsigned cap) {
    assert(cap == 0xb44);
    calls.push_back(5);
}

// PRODUCTION_METHOD

static void near(float actual, float expected) {
    assert(std::fabs(actual - expected) < 0.001f);
}

int main() {
    float positions[12] = {};
    float uvs[8] = {};
    unsigned indices[3] = {};
    PCMeshView mesh = {(char *)positions, (char *)uvs, (char *)indices};
    PCImage2DView image = {&mesh, 42, 100, 200, 10, 20, 0};
    void *entries[] = {&image};
    Engine engine;
    PaintCanvas canvas = {{1, entries}, &engine};
    expectedEngine = canvas.engine;
    expectedMesh = (Mesh *) image.mesh;

    canvas.DrawRegion2D(1, 4, 6, 30, 40, 0.0f, 5, 7, 100, 200);
    assert(calls.empty());

    canvas.DrawRegion2D(0, 4, 6, 30, 40, 0.0f, 5, 7, 100, 200);
    assert((calls == std::vector<int>{1, 2, 3, 4, 5}));
    assert(image.restoreFlag == 1);
    assert(indices[0] == 0x20000 && indices[1] == 1 && indices[2] == 0x20003);
    near(positions[3], 30.0f);
    near(positions[7], 40.0f);
    near(uvs[0], 0.14f);
    near(uvs[1], 0.13f);
    near(uvs[4], 0.44f);
    near(uvs[5], 0.33f);
    near(world[0], 1.0f);
    near(world[5], 1.0f);
    near(world[3], 100.0f);
    near(world[7], 200.0f);

    calls.clear();
    canvas.DrawRegion2D(0, 4, 6, 30, 40, 1.57079632679f, 5, 7, 100, 200);
    assert((calls == std::vector<int>{1, 2, 3, 4, 5}));
    near(world[0], 0.0f);
    near(world[1], -1.0f);
    near(world[4], 1.0f);
    near(world[5], 0.0f);
    near(world[3], 112.0f);
    near(world[7], 202.0f);
    near(world[10], 1.0f);
    near(world[12], 1.0f);
    near(world[13], 1.0f);
    near(world[14], 1.0f);
    std::puts("PaintCanvas region host smoke: OK");
}
