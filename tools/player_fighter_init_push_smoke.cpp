#include <cassert>
#include <cmath>

struct Vector {
    float x, y, z;
};

namespace AbyssEngine {
namespace AEMath {
float VectorLength(const Vector &v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
Vector VectorNormalize(const Vector &v) {
    float length = VectorLength(v);
    return length == 0.0f ? Vector{0.0f, 1.0f, 0.0f}
                          : Vector{v.x / length, v.y / length, v.z / length};
}
}
}

Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
Vector operator*(float scale, const Vector &v) {
    return {scale * v.x, scale * v.y, scale * v.z};
}

struct AERandom {
    int calls = 0;
    int nextInt(int bound) {
        assert(bound == 200);
        const int values[] = {150, 100, 100};
        return values[(calls++) % 3];
    }
};
namespace Globals { void *rnd; }

struct PlayerFighter {
    Vector position{};
    int positionCalls = 0;
    int pushTimer = -1;
    int pushDuration = -1;
    Vector pushNormal{};
    Vector pushImpulse{};
    Vector getPosition() { ++positionCalls; return position; }
    void initPush(const Vector &target, int radius);
};

// PRODUCTION_METHOD

int main() {
    AERandom random;
    Globals::rnd = &random;
    PlayerFighter fighter;
    fighter.initPush({3.0f, 4.0f, 0.0f}, 10);
    assert(fighter.positionCalls == 2);
    assert(fighter.pushTimer == 2500 && fighter.pushDuration == 2500);
    assert(std::fabs(fighter.pushNormal.x + 0.6f) < 0.00001f);
    assert(std::fabs(fighter.pushNormal.y + 0.8f) < 0.00001f);
    assert(std::fabs(fighter.pushImpulse.x - 0.2f) < 0.00001f);
    assert(fighter.pushImpulse.y == 0.0f && fighter.pushImpulse.z == 0.0f);
    assert(random.calls == 3);

    fighter.initPush({20.0f, 0.0f, 0.0f}, 10);
    assert(fighter.pushTimer == 0 && fighter.pushDuration == 0);
    assert(random.calls == 6);
}
