#include <cassert>
#include <vector>

struct Matrix { int value; };
Matrix operator*(Matrix lhs, Matrix rhs) { return {lhs.value + rhs.value}; }

struct Canvas {
    Matrix locals[2] = {{10}, {20}};
    void *TransformGetLocal(unsigned transform) { return &locals[transform]; }
};
struct Globals { static Canvas *Canvas; };
Canvas canvas;
Canvas *Globals::Canvas = &canvas;

struct AEGeometry {
    unsigned transform = 0;
    float forward = 0;
    void moveForward(float distance) { forward += distance; }
};
struct Player {
    Matrix transformMatrix = {0};
    bool active = true;
    bool isActive() { return active; }
};
struct KIPlayer {};
template<class T> struct Array { int id; };
struct Level {
    Array<KIPlayer *> landmarks = {1}, enemies = {2}, asteroids = {3};
    std::vector<int> *events;
    Array<KIPlayer *> *getLandmarks() { events->push_back(11); return &landmarks; }
    Array<KIPlayer *> *getEnemies() { events->push_back(12); return &enemies; }
    Array<KIPlayer *> *getAsteroids() { events->push_back(13); return &asteroids; }
};

struct PlayerEgo {
    Player *player;
    Level *level;
    AEGeometry *geometry;
    AEGeometry *field_0x4;
    bool aboutToReachAutoTargetFlag = true;
    bool field_0x145 = true;
    bool computerControlled = false;
    bool dockingToPlanet = false;
    bool dockedToStream = false;
    bool collide = false;
    bool collidesWithStationFlag = true;
    bool turretActive = false;
    int planetDockTimer = 0;
    float speedValue = 2.0f;
    std::vector<int> events;
    bool triggerStation = false;

    void calcCollision(Array<KIPlayer *> *candidates) {
        events.push_back(20 + candidates->id);
        if (triggerStation && candidates->id == 1)
            collidesWithStationFlag = true;
    }
    void handleTurretView(int dt) { events.push_back(30 + dt); }
    void collisionFrame(int dt) {
        Player *player = this->player;
        // PRODUCTION_BLOCK
    }
};

int main() {
    Player player;
    AEGeometry geometry, parent;
    parent.transform = 1;
    Level level;
    PlayerEgo ego = {&player, &level, &geometry, &parent};
    level.events = &ego.events;

    ego.collisionFrame(5);
    assert(!ego.aboutToReachAutoTargetFlag && !ego.field_0x145);
    assert(ego.collidesWithStationFlag && ego.events.empty() && geometry.forward == 0);

    ego.collide = true;
    player.active = false;
    ego.collisionFrame(5);
    assert(ego.events.empty() && ego.collidesWithStationFlag);

    player.active = true;
    ego.turretActive = true;
    ego.collisionFrame(5);
    assert((ego.events == std::vector<int>{11, 21, 12, 22, 13, 23}));
    assert(!ego.collidesWithStationFlag);

    ego.events.clear();
    ego.triggerStation = true;
    ego.computerControlled = true;
    ego.collisionFrame(5);
    assert(ego.field_0x145 && geometry.forward == 10.0f);
    assert(player.transformMatrix.value == 30 && ego.planetDockTimer == 0);
    assert((ego.events == std::vector<int>{11, 21, 12, 22, 13, 23, 35}));

    ego.events.clear();
    ego.computerControlled = false;
    ego.dockingToPlanet = true;
    ego.collisionFrame(7);
    assert(ego.field_0x145 && ego.planetDockTimer == 7 && geometry.forward == 24.0f);
    ego.dockingToPlanet = false;
    ego.dockedToStream = true;
    ego.collisionFrame(3);
    assert(ego.field_0x145 && ego.planetDockTimer == 7 && geometry.forward == 30.0f);
}
