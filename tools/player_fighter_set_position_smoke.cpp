#include <cassert>
#include <string>
#include <vector>

static std::vector<std::string> events;

struct Vector { float x, y, z; };
struct Matrix { float m[15]{}; };
struct AEGeometry {
    Matrix matrix;
    Vector position{};
    void setPosition(float x, float y, float z) {
        events.push_back("geometry");
        position = {x, y, z};
    }
    Matrix &getMatrix() {
        events.push_back("matrix");
        return matrix;
    }
};
struct Trail {
    Vector last{};
    void reset(const Vector &value) {
        events.push_back("trail");
        last = value;
    }
};
struct Player { Matrix transformMatrix; };
struct PlayerFighter {
    float posX = 0, posY = 0, posZ = 0;
    Vector workingPosition{};
    AEGeometry *geometry = nullptr;
    Player *player = nullptr;
    Trail *trail = nullptr;
    void setPosition(float x, float y, float z);
};

// PRODUCTION_METHOD

static void check(const Vector &actual, float x, float y, float z) {
    assert(actual.x == x && actual.y == y && actual.z == z);
}

int main() {
    AEGeometry geometry;
    for (int i = 0; i < 15; ++i) geometry.matrix.m[i] = float(20 + i);
    Player player;
    Trail trail;
    PlayerFighter fighter;
    fighter.geometry = &geometry;
    fighter.player = &player;
    fighter.trail = &trail;

    fighter.setPosition(12.5f, -3.0f, 70.25f);
    assert((events == std::vector<std::string>{"geometry", "trail", "matrix"}));
    check(geometry.position, 12.5f, -3.0f, 70.25f);
    check(fighter.workingPosition, 12.5f, -3.0f, 70.25f);
    check(trail.last, 12.5f, -3.0f, 70.25f);
    assert(fighter.posX == 12.5f && fighter.posY == -3.0f && fighter.posZ == 70.25f);
    for (int i = 0; i < 15; ++i) assert(player.transformMatrix.m[i] == geometry.matrix.m[i]);

    events.clear();
    fighter.trail = nullptr;
    geometry.matrix.m[14] = 999.0f;
    fighter.setPosition(-40.0f, 0.0f, 1.0f);
    assert((events == std::vector<std::string>{"geometry", "matrix"}));
    check(fighter.workingPosition, -40.0f, 0.0f, 1.0f);
    assert(player.transformMatrix.m[14] == 999.0f);
}
