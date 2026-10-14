#include <cassert>
#include <cmath>
#include <cstdio>
#include <limits>

namespace AbyssEngine { namespace AEMath {
struct Matrix {
    float m11_rightY = 0.0f;
    float m12_upY = 0.0f;
    float zRotation = 0.0f;
};
static int identities = 0, rotations = 0;
Matrix MatrixIdentity(Matrix &) {
    ++identities;
    return {};
}
Matrix MatrixSetRotation(Matrix &base, float x, float y, float z) {
    assert(x == 0.0f && y == 0.0f);
    ++rotations;
    Matrix result = base;
    result.zRotation = z;
    return result;
}
}}
using AbyssEngine::AEMath::Matrix;
struct AEGeometry {
    Matrix matrix{};
    Matrix &getMatrix() { return matrix; }
};
static const float g_PE_rollLevelEps = 0.015f;
struct PlayerEgo {
    AEGeometry *geometry = nullptr;
    Matrix rollMatrix{};
    unsigned char rolling = 0, rollDirection = 0, autoLevel = 0;
    void roll(int amount);
};

// PRODUCTION_METHOD

static bool near(float a, float b) { return std::fabs(a - b) < 0.00001f; }

int main() {
    AEGeometry geometry;
    PlayerEgo ego;
    ego.geometry = &geometry;
    ego.roll(10);
    assert(AbyssEngine::AEMath::rotations == 0);

    ego.rolling = 1;
    ego.rollDirection = 2;
    ego.autoLevel = 1;
    geometry.matrix = {0.01f, 1.0f, 0.0f};
    ego.roll(10);
    assert(AbyssEngine::AEMath::identities == 1);
    assert(ego.rolling == 0 && ego.rollDirection == 0 && ego.autoLevel == 0);

    ego.rolling = 1;
    geometry.matrix = {0.01f, 0.0f, 0.0f};
    ego.roll(100);
    assert(near(ego.rollMatrix.zRotation, 0.6f));
    assert(ego.rollDirection == 2);

    geometry.matrix = {0.2f, -1.0f, 0.0f};
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, -0.0075f));
    geometry.matrix = {0.2f, 1.0f, 0.0f};
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, -0.0025f));
    geometry.matrix = {0.4f, 1.0f, 0.0f};
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, -0.0035f));

    ego.rollDirection = 1;
    geometry.matrix = {-0.2f, -1.0f, 0.0f};
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, 0.0075f));
    geometry.matrix = {-0.2f, 1.0f, 0.0f};
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, 0.0025f));
    geometry.matrix = {-0.4f, 1.0f, 0.0f};
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, 0.0035f));

    ego.rollDirection = 2;
    ego.autoLevel = 0;
    geometry.matrix = {-0.4f, 1.0f, 0.0f};
    ego.roll(10);
    assert(ego.autoLevel == 1 && ego.rollDirection == 1);
    assert(near(ego.rollMatrix.zRotation, 0.0035f));
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, 0.002f));

    ego.rollDirection = 1;
    ego.autoLevel = 0;
    geometry.matrix = {0.4f, 1.0f, 0.0f};
    ego.roll(10);
    assert(ego.autoLevel == 1 && ego.rollDirection == 2);
    assert(near(ego.rollMatrix.zRotation, -0.0035f));
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, -0.002f));

    ego.autoLevel = 0;
    ego.rollDirection = 2;
    geometry.matrix = {std::numeric_limits<float>::quiet_NaN(), -1.0f, 0.0f};
    ego.roll(10);
    assert(ego.rollDirection == 2);
    assert(std::isnan(ego.rollMatrix.zRotation));

    ego.autoLevel = 1;
    geometry.matrix = {-0.4f, std::numeric_limits<float>::quiet_NaN(), 0.0f};
    ego.roll(10);
    assert(near(ego.rollMatrix.zRotation, -0.002f));
    std::puts("PlayerEgo roll smoke: OK");
}
