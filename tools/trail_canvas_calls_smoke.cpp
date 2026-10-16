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

struct PaintCanvas {
    int drawCalls = 0;
    int colorCalls = 0;
    unsigned lastTransform = 0xffffffffu;
    const void *lastView = reinterpret_cast<void *>(1);
    uint32_t lastColor = 0;
    unsigned lastLocal = 0xffffffffu;
    Matrix local{};
    std::vector<PointCall> pointCalls;

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
};

struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct Trail {
    unsigned transformId = 19;
    unsigned meshId = 42;
    int width = 2;
    int segments = 2;
    int pointCount = 18;
    int *points = nullptr;
    int *relativePoints = nullptr;
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
}
