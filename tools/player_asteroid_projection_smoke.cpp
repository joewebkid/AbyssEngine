#include <cassert>
#include <cmath>

namespace AbyssEngine::AEMath {
struct Vector {
    float x, y, z;
    Vector &operator=(const Vector &other) {
        x = other.x; y = other.y; z = other.z;
        return *this;
    }
    Vector &operator-=(const Vector &other) {
        x -= other.x; y -= other.y; z -= other.z;
        return *this;
    }
};
Vector VectorNormalize(const Vector &value) {
    float length = std::sqrt(value.x * value.x + value.y * value.y + value.z * value.z);
    return {value.x / length, value.y / length, value.z / length};
}
}
using namespace AbyssEngine::AEMath;

struct AEGeometry {
    Vector position;
    int queries = 0;
    Vector getPosition() { ++queries; return position; }
};

struct PlayerAsteroid {
    static Vector pos;
    AEGeometry *geometry = nullptr;
    Vector getProjectionVector(const Vector &value);
};
Vector PlayerAsteroid::pos{};

// PRODUCTION_METHOD

int main() {
    AEGeometry geometry{{10, 4, 0}};
    PlayerAsteroid asteroid;
    asteroid.geometry = &geometry;

    const Vector input{4, 1, 0};
    Vector result = asteroid.getProjectionVector(input);
    assert(geometry.queries == 1 && input.x == 4 && input.y == 1);
    assert(PlayerAsteroid::pos.x == 6 && PlayerAsteroid::pos.y == 3);
    assert(std::fabs(result.x - 2.0f / std::sqrt(5.0f)) < 0.00001f);
    assert(std::fabs(result.y - 1.0f / std::sqrt(5.0f)) < 0.00001f);

    PlayerAsteroid::pos = {2, 3, 0};
    geometry.position = {5, 7, 0};
    result = asteroid.getProjectionVector(PlayerAsteroid::pos);
    assert(geometry.queries == 2);
    assert(PlayerAsteroid::pos.x == 3 && PlayerAsteroid::pos.y == 4);
    assert(std::fabs(result.x - 0.6f) < 0.00001f);
    assert(std::fabs(result.y - 0.8f) < 0.00001f);
}
