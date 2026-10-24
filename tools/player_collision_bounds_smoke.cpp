#include <cassert>
#include <limits>

struct Vector { float x, y, z; };

struct Player {
    int radius;
    int hitpoints;
    int getHitpoints() { return hitpoints; }
};

struct AEGeometry {
    Vector positions[3];
    int calls = 0;
    Vector getPosition() { return positions[(calls++) % 3]; }
};

struct PlayerAsteroid {
    Player *player;
    AEGeometry *geometry;
    int collide(float x, float y, float z);
};

struct PlayerStaticFar {
    Player *player;
    int positionX, positionY, positionZ;
    float posX, posY, posZ;
    int collide(float x, float y, float z);
};

// PRODUCTION_METHODS

int main() {
    Player player{10, 1};
    AEGeometry geometry{{{100, 0, 0}, {0, 200, 0}, {0, 0, 300}}};
    PlayerAsteroid asteroid{&player, &geometry};
    assert(asteroid.collide(100, 200, 300));
    assert(geometry.calls == 3);

    for (Vector &position : geometry.positions) position = {0, 0, 0};
    const float nan = std::numeric_limits<float>::quiet_NaN();
    assert(!asteroid.collide(nan, 0, 0));
    assert(!asteroid.collide(0, nan, 0));
    assert(!asteroid.collide(0, 0, nan));
    assert(!asteroid.collide(10, 0, 0));
    assert(asteroid.collide(9, 0, 0));
    player.hitpoints = 0;
    const int calls = geometry.calls;
    assert(!asteroid.collide(0, 0, 0) && geometry.calls == calls);

    PlayerStaticFar far{&player, 100, 200, 300, 1000000, 0, 0};
    assert(far.collide(100, 200, 300));
    assert(!far.collide(1000000, 200, 300));
    assert(!far.collide(nan, 200, 300));
    assert(!far.collide(100, nan, 300));
    assert(!far.collide(100, 200, nan));
    assert(!far.collide(110, 200, 300));
}
