#include <cassert>
#include <cstdint>
#include <vector>

struct Matrix { float m[15]{}; };
namespace AEMath {
Matrix MatrixSetTranslation(Matrix &matrix, float x, float y, float z) {
    matrix.m[3] = x;
    matrix.m[7] = y;
    matrix.m[11] = z;
    return matrix;
}
}

struct PointCall {
    unsigned mesh;
    unsigned short vertex;
    float x, y, z;
};
struct TriangleCall {
    unsigned mesh;
    unsigned short index, a, b, c;
};
struct UvCall {
    unsigned mesh;
    unsigned short vertex;
    float u, v;
};

struct PaintCanvas {
    int drawCalls = 0;
    int colorCalls = 0;
    unsigned lastTransform = 0xffffffffu;
    const void *lastView = reinterpret_cast<void *>(1);
    uint32_t lastColor = 0;
    unsigned lastLocal = 0xffffffffu;
    Matrix local{};
    std::vector<PointCall> pointCalls;
    std::vector<TriangleCall> triangleCalls;
    std::vector<UvCall> uvCalls;
    unsigned short createdVertices = 0, createdTriangles = 0;
    signed char createdType = 0;
    unsigned short createdMaterial = 0;
    unsigned addedTransform = 0xffffffffu, addedMesh = 0xffffffffu;

    void DrawTransform(unsigned transform, const void *view) {
        ++drawCalls;
        lastTransform = transform;
        lastView = view;
    }
    void TransformSetColor(unsigned transform, unsigned color) {
        ++colorCalls;
        lastTransform = transform;
        lastColor = color;
    }
    float MeshSetPoint(unsigned mesh, unsigned short vertex, float x, float y, float z) {
        pointCalls.push_back({mesh, vertex, x, y, z});
        return 0.0f;
    }
    void *TransformGetLocal(unsigned transform) {
        lastLocal = transform;
        return &local;
    }
    void MeshCreate(unsigned short vertices, unsigned short triangles,
                    signed char type, unsigned short material, unsigned &out) {
        createdVertices = vertices;
        createdTriangles = triangles;
        createdType = type;
        createdMaterial = material;
        out = 42;
    }
    void MeshSetTriangle(unsigned mesh, unsigned short index, unsigned short a,
                         unsigned short b, unsigned short c) {
        triangleCalls.push_back({mesh, index, a, b, c});
    }
    void MeshSetUv(unsigned mesh, unsigned short vertex, float u, float v) {
        uvCalls.push_back({mesh, vertex, u, v});
    }
    void TransformCreate(unsigned &out) { out = 19; }
    void TransformAddMeshId(unsigned transform, unsigned mesh) {
        addedTransform = transform;
        addedMesh = mesh;
    }
};

struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct Trail {
    int sourceX = -1, sourceY = -1, sourceZ = -1;
    unsigned transformId = 19;
    unsigned meshId = 42;
    int width = 2;
    int segments = 2;
    int pointCount = 18;
    int *points = nullptr;
    int *relativePoints = nullptr;
    Trail() = default;
    Trail(int type, int segments);
    void render();
    void changeType(int type);
    void setWidth(int width);
    void update(float ax, float ay, float az, float bx, float by, float bz);
};

// PRODUCTION_METHODS

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    Trail trail;
    trail.render();
    assert(canvas.drawCalls == 1 && canvas.lastTransform == 19 &&
           canvas.lastView == nullptr);

    const struct { int type; uint32_t color; } cases[] = {
        {0, 0xffffffffu}, {1, 0xff0000ffu}, {2, 0x00ff00ffu},
        {3, 0xffff00ffu}, {4, 0xffffffffu}, {5, 0x00ff0000u},
        {6, 0xffff00ffu}, {7, 0xff0000ffu}, {8, 0xff4000ffu},
        {9, 0x00ff00ffu}, {10, 0xffff00ffu}, {11, 0xff0000ffu},
        {12, 0xffffffffu}
    };
    for (const auto &caseValue : cases) {
        trail.changeType(caseValue.type);
        assert(canvas.lastTransform == 19 && canvas.lastColor == caseValue.color);
    }
    assert(canvas.colorCalls == 13);

    PaintCanvas replacement;
    Globals::Canvas = &replacement;
    trail.render();
    trail.changeType(1);
    assert(replacement.drawCalls == 1 && replacement.colorCalls == 1 &&
           replacement.lastTransform == 19 && replacement.lastColor == 0xff0000ffu);
    assert(canvas.drawCalls == 1 && canvas.colorCalls == 13);

    PaintCanvas widthCanvas;
    Globals::Canvas = &widthCanvas;
    int widthPoints[18]{};
    Trail widthTrail;
    widthTrail.width = 60;
    widthTrail.points = widthPoints;
    for (int i = 0; i < 6; ++i) {
        widthPoints[3 * i] = 100 + 10 * i;
        widthPoints[3 * i + 1] = i + 1;
        widthPoints[3 * i + 2] = -i - 1;
    }
    widthTrail.setWidth(50);
    assert(widthTrail.width == 50 && widthCanvas.pointCalls.size() == 6);
    for (int i = 0; i < 6; ++i) {
        const auto &call = widthCanvas.pointCalls[i];
        assert(widthPoints[3 * i] == 110 + 10 * i);
        assert(call.mesh == 42 && call.vertex == i &&
               call.x == float(110 + 10 * i) &&
               call.y == float(i + 1) && call.z == float(-i - 1));
    }
    widthTrail.setWidth(70);
    assert(widthTrail.width == 70 && widthCanvas.pointCalls.size() == 12);
    for (int i = 0; i < 6; ++i)
        assert(widthPoints[3 * i] == 90 + 10 * i &&
               widthCanvas.pointCalls[6 + i].x == float(90 + 10 * i));

    PaintCanvas updateCanvas;
    Globals::Canvas = &updateCanvas;
    int points[18]{}, relative[18]{};
    for (int i = 0; i < 18; ++i) points[i] = 1000 + i;
    Trail updateTrail;
    updateTrail.points = points;
    updateTrail.relativePoints = relative;
    updateTrail.update(10.25f, 20.75f, -30.5f, 40.75f, 50.25f, -60.5f);
    // ARM 0x15c07c shifts the pairs after storing both new endpoints.
    const int expected[18] = {8, 20, -30, 12, 20, -30,
                              8, 20, -30, 12, 20, -30,
                              38, 50, -60, 42, 50, -60};
    for (int i = 0; i < 18; ++i) assert(points[i] == expected[i]);
    for (int i = 0; i < 18; i += 3) {
        assert(relative[i] == int(float(points[i]) - 10.25f));
        assert(relative[i + 1] == int(float(points[i + 1]) - 20.75f));
        assert(relative[i + 2] == int(float(points[i + 2]) + 30.5f));
    }
    assert(updateCanvas.pointCalls.size() == 6 && updateCanvas.lastLocal == 19);
    for (int i = 0; i < 6; ++i) {
        const auto &call = updateCanvas.pointCalls[i];
        assert(call.mesh == 42 && call.vertex == i &&
               call.x == float(relative[3 * i]) &&
               call.y == float(relative[3 * i + 1]) &&
               call.z == float(relative[3 * i + 2]));
    }
    assert(updateCanvas.local.m[3] == 10.25f &&
           updateCanvas.local.m[7] == 20.75f &&
           updateCanvas.local.m[11] == -30.5f);

    PaintCanvas constructedCanvas;
    Globals::Canvas = &constructedCanvas;
    Trail constructed(9, 2);
    assert(constructed.sourceX == 0 && constructed.sourceY == 0 &&
           constructed.sourceZ == 0 && constructed.width == 60);
    assert(constructed.segments == 2 && constructed.pointCount == 18 &&
           constructed.points != nullptr && constructed.relativePoints != nullptr);
    assert(constructed.meshId == 42 && constructed.transformId == 19);
    assert(constructedCanvas.createdVertices == 6 &&
           constructedCanvas.createdTriangles == 4 &&
           constructedCanvas.createdType == 19 &&
           constructedCanvas.createdMaterial == 0x4e77);
    assert(constructedCanvas.triangleCalls.size() == 2);
    const auto &first = constructedCanvas.triangleCalls[0];
    const auto &second = constructedCanvas.triangleCalls[1];
    assert(first.mesh == 42 && first.index == 0 && first.a == 0 &&
           first.b == 1 && first.c == 2);
    assert(second.mesh == 42 && second.index == 1 && second.a == 3 &&
           second.b == 2 && second.c == 1);
    assert(constructedCanvas.uvCalls.size() == 6);
    for (int i = 0; i < 6; ++i) {
        const auto &uv = constructedCanvas.uvCalls[i];
        assert(uv.mesh == 42 && uv.vertex == i && uv.u == 1.0f && uv.v == 0.0f);
    }
    assert(constructedCanvas.addedTransform == 19 && constructedCanvas.addedMesh == 42);
    assert(constructedCanvas.colorCalls == 1 && constructedCanvas.lastColor == 0x00ff00ffu);
    delete[] constructed.points;
    delete[] constructed.relativePoints;

    PaintCanvas emptyCanvas;
    Globals::Canvas = &emptyCanvas;
    Trail empty(4, 0);
    assert(empty.pointCount == 6 && emptyCanvas.createdVertices == 2 &&
           emptyCanvas.createdTriangles == 0 && emptyCanvas.triangleCalls.empty());
    assert(emptyCanvas.uvCalls.size() == 2 && emptyCanvas.colorCalls == 1);
    for (const auto &uv : emptyCanvas.uvCalls)
        assert(uv.u == 0.0f && uv.v == 0.0f);
    assert(emptyCanvas.lastColor == 0xffffffffu);
    delete[] empty.points;
    delete[] empty.relativePoints;
}
