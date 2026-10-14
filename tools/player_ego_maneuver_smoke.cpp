#include <cassert>
#include <cstdio>

namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
Vector operator*(float scale, const Vector &v) {
    return {v.x * scale, v.y * scale, v.z * scale};
}
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
}}
using AbyssEngine::AEMath::Vector;
using Vec3 = Vector;

struct Player { float flShake = 0.0f; };
struct AEGeometry {
    Vector direction{0.0f, 0.0f, 1.0f};
    int directionCalls = 0;
    Vector getDirection() { ++directionCalls; return direction; }
};
struct PlayerEgo {
    void *player = nullptr;
    AEGeometry *geometry = nullptr;
    bool volatileGoods = false;
    int maneuverType = 0;
    int maneuverTimer = 9;
    Vector strafeTargetVec{};
    Vector facingVec{};
    Vector position{1.0f, 2.0f, 3.0f};
    Vector getPosition() { return position; }
    void initManeuver(int type);
};

// PRODUCTION_METHOD

int main() {
    Player player;
    AEGeometry geometry;
    PlayerEgo ego;
    ego.player = &player;
    ego.geometry = &geometry;
    ego.initManeuver(3);
    assert(ego.maneuverType == 3 && ego.maneuverTimer == 0);
    assert(ego.strafeTargetVec.x == 1.0f && ego.strafeTargetVec.y == 2.0f);
    assert(ego.strafeTargetVec.z == -9997.0f);
    assert(ego.facingVec.z == 1.0f);
    assert(geometry.directionCalls == 2);
    ego.initManeuver(3);
    assert(ego.strafeTargetVec.z == -9997.0f && geometry.directionCalls == 2);

    PlayerEgo rolling;
    rolling.player = &player;
    rolling.geometry = &geometry;
    rolling.volatileGoods = true;
    rolling.initManeuver(1);
    assert(player.flShake == 0.17f && rolling.maneuverType == 1);
    std::puts("PlayerEgo maneuver host smoke: OK");
}
