#include <cassert>
#include <cmath>
#include <limits>

namespace AbyssEngine { namespace AEMath {
struct Matrix {
    float m11_rightY = 0.0f;
    float m12_upY = 0.0f;
    float zRotation = 0.0f;
};
static int identities = 0, rotations = 0;
Matrix MatrixIdentity(Matrix &matrix) {
    ++identities;
    matrix = {};
    return matrix;
}
Matrix MatrixSetRotation(Matrix &matrix, float x, float y, float z) {
    assert(x == 0.0f && y == 0.0f);
    ++rotations;
    matrix.zRotation = z;
    return matrix;
}
}}
using AbyssEngine::AEMath::Matrix;

struct AEGeometry {
    Matrix matrix;
    int reads = 0;
    Matrix &getMatrix() { ++reads; return matrix; }
};
struct PlayerFighter {
    AEGeometry *geometry = nullptr;
    Matrix rollMatrix;
    unsigned char rollActive = 0, field_0x254 = 0, field_0x255 = 0;
    void roll(int angle);
};

// PRODUCTION_METHOD

static bool near(float a, float b) { return std::fabs(a - b) < 0.00001f; }

int main() {
    AEGeometry geom;
    PlayerFighter fighter;
    fighter.geometry = &geom;
    fighter.roll(10);
    assert(geom.reads == 0 && AbyssEngine::AEMath::rotations == 0);

    fighter.rollActive = 1;
    fighter.field_0x254 = 1;
    fighter.field_0x255 = 2;
    geom.matrix = {0.01f, 1.0f, 0.0f};
    fighter.roll(10);
    assert(AbyssEngine::AEMath::identities == 1);
    assert(!fighter.rollActive && !fighter.field_0x254 && !fighter.field_0x255);

    fighter.rollActive = 1;
    geom.matrix = {0.01f, 0.0f, 0.0f};
    fighter.roll(100);
    assert(near(fighter.rollMatrix.zRotation, 0.6f));
    assert(fighter.field_0x255 == 2);

    geom.matrix = {0.015f, 1.0f, 0.0f};
    fighter.roll(10);
    assert(fighter.rollActive && near(fighter.rollMatrix.zRotation, -0.0025f));

    geom.matrix = {0.2f, -1.0f, 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, -0.0075f));
    geom.matrix = {0.2f, 1.0f, 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, -0.0025f));
    geom.matrix = {0.4f, 1.0f, 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, -0.0035f));

    fighter.field_0x255 = 1;
    geom.matrix = {-0.2f, -1.0f, 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, 0.0075f));
    geom.matrix = {-0.2f, 0.0f, 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, 0.0025f));
    geom.matrix = {-0.4f, 1.0f, 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, 0.0035f));
    geom.matrix = {-0.3f, 0.0f, 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, 0.0035f));

    fighter.field_0x255 = 2;
    fighter.field_0x254 = 0;
    fighter.roll(10);
    assert(fighter.field_0x254 == 1 && fighter.field_0x255 == 1);
    assert(near(fighter.rollMatrix.zRotation, 0.0035f));
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, 0.002f));

    fighter.field_0x255 = 1;
    fighter.field_0x254 = 0;
    geom.matrix = {0.4f, 1.0f, 0.0f};
    fighter.roll(10);
    assert(fighter.field_0x254 == 1 && fighter.field_0x255 == 2);
    assert(near(fighter.rollMatrix.zRotation, -0.0035f));
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, -0.002f));

    fighter.field_0x254 = 1;
    geom.matrix = {-0.4f, std::numeric_limits<float>::quiet_NaN(), 0.0f};
    fighter.roll(10);
    assert(near(fighter.rollMatrix.zRotation, -0.002f));
    assert(fighter.field_0x255 == 1);

    fighter.field_0x254 = 0;
    geom.matrix = {std::numeric_limits<float>::quiet_NaN(), -1.0f, 0.0f};
    fighter.roll(10);
    assert(std::isnan(fighter.rollMatrix.zRotation));
    assert(fighter.field_0x255 == 1);
}
