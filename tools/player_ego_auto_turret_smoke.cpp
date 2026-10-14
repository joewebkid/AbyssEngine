#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>

namespace AbyssEngine { namespace AEMath {
struct Vector { float x, y, z; };
Vector operator-(const Vector &a, const Vector &b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
float VectorLength(const Vector &v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
}}
using AbyssEngine::AEMath::Vector;
template<class T> using Array = std::vector<T>;

struct Player {
    bool active = true;
    int stops = 0;
    bool isActive() { return active; }
    void stopShooting(int weapon) { assert(weapon == 2); ++stops; }
};
struct KIPlayer {
    Player *player;
    explicit KIPlayer(Player *owner) : player(owner) {}
    void *geometry = nullptr;
    bool dead = false, dying = false, enemy = true;
    unsigned char noTargetFlag = 0;
    Vector position{};
    int positionCalls = 0;
    bool isDead() { return dead; }
    bool isDying() { return dying; }
    bool isEnemy() { return enemy; }
    virtual Vector getPosition() { ++positionCalls; return position; }
};
struct Level {
    Array<KIPlayer *> enemies;
    Array<KIPlayer *> *getEnemies() { return &enemies; }
};
struct PlayerEgo {
    int autoTurretTimer = 0;
    void *autoTurretTarget = nullptr, *autoTurretPrevTarget = nullptr;
    void *level = nullptr, *player = nullptr;
    Vector dockOffsetVec{};
    int autoTurretFireTimer = 0;
    void handleAutoTurret(int dt);
};
static int aimCalls;
int PE_hat_aimAndFire(PlayerEgo *, int) { ++aimCalls; return 0; }

// PRODUCTION_METHOD

int main() {
    Player self, other;
    Level level;
    KIPlayer dead{&other}, inactive{&other}, friendly{&other};
    KIPlayer blocked{&other}, far{&other}, nearest{&other};
    dead.dead = true;
    inactive.player->active = false;
    // Each candidate needs its own Player to test the active guard.
    Player active;
    friendly.player = &active;
    blocked.player = &active;
    far.player = &active;
    nearest.player = &active;
    friendly.enemy = false;
    blocked.noTargetFlag = 1;
    far.position = {0.0f, 0.0f, 50.0f};
    nearest.position = {0.0f, 0.0f, 25.0f};
    level.enemies = {&dead, &inactive, &friendly, &blocked, &far, &nearest};

    PlayerEgo ego;
    ego.level = &level;
    ego.player = &self;
    ego.dockOffsetVec = {0.0f, 0.0f, 0.0f};
    ego.handleAutoTurret(3001);
    assert(ego.autoTurretTarget == &nearest && ego.autoTurretTimer == 0);
    assert(far.positionCalls == 1 && nearest.positionCalls == 1);
    assert(dead.positionCalls == 0 && inactive.positionCalls == 0);
    assert(friendly.positionCalls == 0 && blocked.positionCalls == 0);
    assert(self.stops == 1 && aimCalls == 0);

    nearest.position = {0.0f, 0.0f, 60000.0f};
    far.position = {0.0f, 0.0f, 60001.0f};
    ego.handleAutoTurret(3001);
    assert(ego.autoTurretTarget == nullptr && self.stops == 2);

    nearest.position = {0.0f, 0.0f, 25.0f};
    nearest.geometry = &nearest;
    ego.handleAutoTurret(3001);
    assert(ego.autoTurretTarget == &nearest && aimCalls == 1);
    std::puts("PlayerEgo auto-turret selection smoke: OK");
}
