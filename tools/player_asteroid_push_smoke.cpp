#include <cassert>
#include <cmath>

namespace AbyssEngine::AEMath {
struct Vector { float x, y, z; };
struct Matrix {
    float m[15];
    constexpr Matrix(float a0, float a1, float a2, float a3, float a4,
                     float a5, float a6, float a7, float a8, float a9,
                     float a10, float a11, float a12, float a13, float a14)
        : m{a0, a1, a2, a3, a4, a5, a6, a7, a8, a9,
            a10, a11, a12, a13, a14} {}
};

int rotations = 0;
int products = 0;
float lastX = 0, lastY = 0, lastZ = 0;

Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z) {
    ++rotations;
    for (int i = 0; i < 15; ++i)
        assert(matrix.m[i] == ((i == 0 || i == 5 || i == 10 || i >= 12) ? 1.0f : 0.0f));
    lastX = x; lastY = y; lastZ = z;
    matrix.m[3] = x;
    return matrix;
}

Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
    ++products;
    assert(lhs.m[0] == 7.0f && rhs.m[0] == 1.0f);
    Matrix result = rhs;
    result.m[0] = lhs.m[0] + rhs.m[0];
    return result;
}

Vector operator*(const Vector &v, float scale) {
    return {v.x * scale, v.y * scale, v.z * scale};
}
}
using namespace AbyssEngine::AEMath;

struct AEGeometry {
    Matrix matrix{7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    Vector moved{};
    int reads = 0, writes = 0, translations = 0;
    Matrix &getMatrix() { ++reads; return matrix; }
    void setMatrix(const Matrix &value) { ++writes; matrix = value; }
    void translate(const Vector &value) { ++translations; moved = value; }
};

struct PlayerAsteroid {
    int timer = 0, duration = 5000, lastDelta = 0;
    Vector direction{2, 0, -1}, spin{1, 2, 3};
    AEGeometry *geometry = nullptr;
    int &pushTimer() { return timer; }
    int &pushDuration() { return duration; }
    Vector &pushDirection() { return direction; }
    Vector &pushSpin() { return spin; }
    void push(int delta);
};

// PRODUCTION_METHOD

int main() {
    AEGeometry geometry;
    PlayerAsteroid asteroid;
    asteroid.geometry = &geometry;
    asteroid.push(100);
    assert(rotations == 0 && geometry.translations == 0);

    asteroid.timer = 1000;
    asteroid.lastDelta = 10;
    asteroid.push(100);
    assert(asteroid.timer == 900 && rotations == 1 && products == 1);
    assert(geometry.reads == 1 && geometry.writes == 1 && geometry.translations == 1);
    assert(std::fabs(lastX - 0.18f) < 0.00001f);
    assert(std::fabs(lastY - 0.36f) < 0.00001f);
    assert(std::fabs(lastZ - 0.54f) < 0.00001f);
    assert(geometry.matrix.m[0] == 8.0f);
    assert(std::fabs(geometry.moved.x - 109.2f) < 0.0001f);
    assert(std::fabs(geometry.moved.z + 54.6f) < 0.0001f);

    AEGeometry still;
    asteroid.geometry = &still;
    asteroid.timer = 1000;
    asteroid.lastDelta = 0;
    asteroid.push(100);
    assert(still.reads == 0 && still.writes == 0 && still.translations == 1);
    assert(still.moved.x == 0.0f && still.moved.z == 0.0f);

    AEGeometry backwards;
    asteroid.geometry = &backwards;
    asteroid.timer = 1000;
    asteroid.lastDelta = -2;
    asteroid.push(100);
    assert(backwards.reads == 0 && backwards.writes == 0 && backwards.translations == 1);
    assert(std::fabs(backwards.moved.x + 21.84f) < 0.0001f);
}
