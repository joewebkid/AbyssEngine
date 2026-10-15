#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
Vector operator-(Vector a, Vector b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
float VectorLength(Vector v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
}}
using AbyssEngine::AEMath::Vector;

struct KIPlayer {
    Vector position;
    Vector getPosition() { return position; }
};

struct PlayerEgo {
    // ARM fields +0xec/+0xf0/+0xf4 form the waypoint Vector.
    float waypointY = 0.0f;
    uint32_t gunBaseGeo = 0;
    int pitchAccumDir = 0;
    uint8_t aboutToReachAutoTargetFlag = 0;
    Vector position{10.0f, 20.0f, 30.0f};
    Vector getPosition() { return position; }
    void checkWaypoint(void *wp);
    Vector storedTarget() const {
        Vector result;
        std::memcpy(&result, &waypointY, sizeof(result));
        return result;
    }
};

void PlayerEgo::checkWaypoint(void *wp) {
// PRODUCTION_FRAGMENT
}

int main() {
    PlayerEgo ego;
    KIPlayer waypoint{{110.0f, 120.0f, 30.0f}};
    ego.checkWaypoint(&waypoint);
    Vector stored = ego.storedTarget();
    assert(stored.x == 110.0f && stored.y == 120.0f && stored.z == 30.0f);
    assert(ego.aboutToReachAutoTargetFlag == 1); // Euclidean 141, squared 20000.

    ego.aboutToReachAutoTargetFlag = 0;
    waypoint.position = {20009.9f, 20.0f, 30.0f};
    ego.checkWaypoint(&waypoint);
    assert(ego.aboutToReachAutoTargetFlag == 1); // int truncation: 19999.

    ego.aboutToReachAutoTargetFlag = 0;
    waypoint.position.x = 20010.0f;
    ego.checkWaypoint(&waypoint);
    assert(ego.aboutToReachAutoTargetFlag == 0);
}
