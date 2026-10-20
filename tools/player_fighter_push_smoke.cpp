#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

static std::vector<std::string> events;

namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
struct Matrix { float m[15]; };
static Vector operator*(const Vector &v, float scale) {
    events.push_back("scale");
    return {v.x * scale, v.y * scale, v.z * scale};
}
static Matrix MatrixSetRotation(Matrix &base, float x, float y, float z) {
    events.push_back("rotation");
    base.m[0] = x;
    base.m[5] = y;
    base.m[10] = z;
    return base;
}
static Matrix operator*(const Matrix &lhs, const Matrix &rhs) {
    events.push_back("multiply");
    Matrix result = lhs;
    result.m[0] = lhs.m[0] + rhs.m[0];
    result.m[5] = lhs.m[5] + rhs.m[5];
    result.m[10] = lhs.m[10] + rhs.m[10];
    return result;
}
}}
using AbyssEngine::AEMath::Vector;
using AbyssEngine::AEMath::Matrix;

struct AEGeometry {
    Matrix matrix{};
    Vector translation{};
    Matrix &getMatrix() { events.push_back("get"); return matrix; }
    void setMatrix(const Matrix &value) { events.push_back("set"); matrix = value; }
    void translate(const Vector &value) { events.push_back("translate"); translation = value; }
};
struct PlayerFighter {
    int pushTimer = 0, pushDuration = 100;
    int deltaTime = 0, deltaTimeHi = 0;
    Vector pushImpulse{1.0f, 2.0f, 3.0f};
    Vector pushNormal{1.0f, -2.0f, 3.0f};
    float currentSpeed = 4.0f;
    AEGeometry *geometry = nullptr;
    void push(int dt);
};

// PRODUCTION_METHOD

static bool near(float actual, float wanted) { return std::fabs(actual - wanted) < 0.0001f; }

int main() {
    AEGeometry geom;
    geom.matrix.m[0] = 10.0f;
    geom.matrix.m[5] = 20.0f;
    geom.matrix.m[10] = 30.0f;
    PlayerFighter fighter;
    fighter.geometry = &geom;

    fighter.push(10);
    assert(events.empty());

    fighter.pushTimer = 50;
    fighter.deltaTime = 16;
    fighter.push(10);
    assert(fighter.pushTimer == 40);
    assert((events == std::vector<std::string>{"rotation", "get", "multiply", "set",
                                                "scale", "scale", "scale", "translate"}));
    assert(near(geom.matrix.m[0], 10.4f));
    assert(near(geom.matrix.m[5], 20.8f));
    assert(near(geom.matrix.m[10], 31.2f));
    assert(near(geom.translation.x, 6.144f));
    assert(near(geom.translation.y, -12.288f));
    assert(near(geom.translation.z, 18.432f));

    events.clear();
    fighter.deltaTime = 0;
    fighter.push(50);
    assert(fighter.pushTimer == -10);
    assert((events == std::vector<std::string>{"rotation", "scale", "scale", "scale", "translate"}));
    assert(geom.translation.x == 0.0f && geom.translation.y == 0.0f);

    events.clear();
    fighter.pushTimer = 10;
    fighter.deltaTime = -1;
    fighter.deltaTimeHi = -1;
    fighter.push(1);
    assert((events == std::vector<std::string>{"rotation", "scale", "scale", "scale", "translate"}));
    assert(geom.translation.x < 0.0f);

    events.clear();
    fighter.pushTimer = 10;
    fighter.deltaTimeHi = 0;
    fighter.push(1);
    assert((events == std::vector<std::string>{"rotation", "get", "multiply", "set",
                                                "scale", "scale", "scale", "translate"}));
}
