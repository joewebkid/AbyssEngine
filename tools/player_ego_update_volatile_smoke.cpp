#include <cassert>
#include <cmath>
#include <cstdint>

struct Vector { float x, y, z; };
struct AEGeometry {
    Vector position = {10, 20, 30};
    Vector getPosition() { return position; }
};
struct Explosion {
    int type;
    float scale = 0;
    Vector position = {0, 0, 0}, direction = {1, 1, 1};
    explicit Explosion(int value) : type(value) {}
    void setScaling(float value) { scale = value; }
    void start(Vector p, Vector d) { position = p; direction = d; }
};
struct Sound {
    bool playing = false;
    int playCalls = 0, paramCalls = 0;
    float lastParam = -1;
    bool isPlaying(int id) { assert(id == 35); return playing; }
    void play(int id, Vector *, Vector *, float) {
        assert(id == 35); ++playCalls; playing = true;
    }
    void setParamValue(int index, int id, float value) {
        assert(index == 0 && id == 35); ++paramCalls; lastParam = value;
    }
};
struct Globals { static Sound *sound; };
Sound soundService;
Sound *Globals::sound = &soundService;
struct Player {
    float flShake = 0;
    int hitpoints = 100, gammaHP = 10, setHitpointCalls = 0;
    void setHitpoints(int value) { hitpoints = value; ++setHitpointCalls; }
    int getHitpoints() { return hitpoints; }
    int getGammaHP() { return gammaHP; }
};
struct AxisPair { float now = 0, previous = 0; };
struct PlayerEgo {
    Player *player;
    AEGeometry *geometry;
    bool volatileGoods = false, dockingToPlanet = false, dockedToStream = false;
    uint16_t docked = 0;
    void *explosion2 = nullptr;
    AxisPair rollAccum, yawAccumD;
    void updateVolatile(int dt) {
        Player *player = this->player;
        // PRODUCTION_BLOCK
    }
};

int main() {
    Player player;
    AEGeometry geometry;
    PlayerEgo ego = {&player, &geometry};

    player.flShake = 0.8f;
    ego.updateVolatile(100);
    assert(player.flShake == 0.0f && soundService.playCalls == 0 && soundService.paramCalls == 0);

    ego.volatileGoods = true;
    player.flShake = 0.2f;
    ego.rollAccum = {4, 2};
    ego.yawAccumD = {1, 4};
    ego.updateVolatile(100);
    assert(soundService.playCalls == 1 && soundService.paramCalls == 1);
    assert(std::fabs(player.flShake - 0.35f) < 0.0001f);
    assert(std::fabs(soundService.lastParam - 0.35f) < 0.0001f);
    assert(ego.rollAccum.previous == 4 && ego.yawAccumD.previous == 1);
    ego.updateVolatile(100);
    assert(soundService.playCalls == 1 && std::fabs(player.flShake - 0.35f) < 0.0001f);

    player.flShake = 0.96f;
    ego.rollAccum.now = 5;
    ego.updateVolatile(100);
    assert(player.hitpoints == 0 && player.setHitpointCalls == 1);
    auto *explosion = static_cast<Explosion *>(ego.explosion2);
    assert(explosion && explosion->type == 12 && explosion->scale == 0.7f);
    assert(explosion->position.x == 10 && explosion->position.y == 20 && explosion->position.z == 30);
    assert(explosion->direction.x == 0 && explosion->direction.y == 0 && explosion->direction.z == 0);
    assert(soundService.lastParam == 0.0f);

    player.hitpoints = 100;
    ego.dockingToPlanet = true;
    ego.updateVolatile(100);
    assert(player.hitpoints == 100 && ego.explosion2 == explosion && soundService.lastParam > 1.0f);
    ego.dockingToPlanet = false;
    ego.dockedToStream = true;
    ego.updateVolatile(100);
    assert(player.hitpoints == 100);

    player.gammaHP = 0;
    ego.updateVolatile(100);
    assert(player.hitpoints == 0); // Stream docking does not suppress gamma death.
    player.hitpoints = 100;
    ego.docked = 1;
    ego.updateVolatile(100);
    assert(player.hitpoints == 100);
    ego.docked = 0x100;
    ego.updateVolatile(100);
    assert(player.hitpoints == 0); // ARM tests the low docked byte.
    delete explosion;
}
