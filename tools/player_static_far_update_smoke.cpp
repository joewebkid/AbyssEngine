#include <cassert>
#include <cmath>

namespace AbyssEngine::AEMath {
struct Vector {
    float x, y, z;
    Vector &operator*=(float scale) { x *= scale; y *= scale; z *= scale; return *this; }
    Vector &operator+=(const Vector &other) {
        x += other.x; y += other.y; z += other.z; return *this;
    }
};
struct Matrix { Vector position; };
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
float VectorLength(const Vector &v) { return std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z); }
Vector VectorNormalize(const Vector &v) {
    float length = VectorLength(v);
    return {v.x / length, v.y / length, v.z / length};
}
Vector MatrixGetPosition(const Matrix &m) { return m.position; }
}
using namespace AbyssEngine::AEMath;

struct PaintCanvas {
    Matrix *local;
    unsigned calls = 0;
    unsigned CameraGetCurrent() { ++calls; return 3; }
    void *CameraGetLocal(unsigned camera) { assert(camera == 3); ++calls; return local; }
};
struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct AEGeometry {
    Vector position{};
    Vector scaling{};
    int scalarPositions = 0;
    int vectorPositions = 0;
    int scaleCalls = 0;
    void setPosition(float x, float y, float z) {
        position = {x, y, z}; ++scalarPositions;
    }
    void setPosition(const Vector &v) { position = v; ++vectorPositions; }
    void setScaling(float x, float y, float z) {
        scaling = {x, y, z}; ++scaleCalls;
    }
};

struct PlayerStaticFar {
    AEGeometry *geometry = nullptr;
    int positionX = 0, positionY = 0, positionZ = 0;
    float posX = 0, posY = 0, posZ = 0;
    Vector cameraPosition{}, objectPosition{}, viewDirection{};
    void update(int delta);
};

// PRODUCTION_METHOD

int main() {
    PlayerStaticFar far;
    far.update(1); // Null geometry returns before touching the canvas.

    Matrix matrix{{1000, 0, 0}};
    PaintCanvas canvas{&matrix};
    Globals::Canvas = &canvas;
    AEGeometry geometry;
    far.geometry = &geometry;
    far.positionX = 1100;
    far.positionY = 200;
    far.positionZ = 300;
    far.posX = 1000000;
    far.update(1);
    assert(canvas.calls == 2 && geometry.scalarPositions == 1 && geometry.vectorPositions == 0);
    assert(geometry.position.x == 1100 && geometry.position.y == 200 && geometry.position.z == 300);
    assert(geometry.scaling.x == 1 && geometry.scaling.y == 1 && geometry.scaling.z == 1);

    far.positionX = 601000;
    far.positionY = 0;
    far.positionZ = 0;
    far.update(1);
    assert(canvas.calls == 4 && geometry.scalarPositions == 1 && geometry.vectorPositions == 1);
    assert(geometry.position.x == 296000 && geometry.position.y == 0 && geometry.position.z == 0);
    const float expectedScale = 2013.0f / 4096.0f;
    assert(geometry.scaling.x == expectedScale && geometry.scaling.y == expectedScale);
    assert(geometry.scaling.z == expectedScale && geometry.scaleCalls == 2);
}
