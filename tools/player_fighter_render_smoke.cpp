#include <cassert>
#include <functional>
#include <string>
#include <vector>

static std::vector<std::string> events;

struct Matrix { int cells[15]{}; };

struct PaintCanvas {
    Matrix local;
    void *TransformGetLocal(unsigned id) {
        assert(id == 7);
        events.push_back("get");
        return &local;
    }
    void TransformSetLocal(unsigned id, const Matrix &matrix) {
        assert(id == 7);
        events.push_back(matrix.cells[0] == 99 ? "set-player" : "restore");
        local = matrix;
    }
};

struct Globals { static PaintCanvas *Canvas; };
PaintCanvas *Globals::Canvas = nullptr;

struct AEGeometry {
    const char *name;
    unsigned transform;
    void render() {
        events.push_back(name);
        if (std::string(name) == "sub") {
            for (int i = 0; i < 15; ++i)
                assert(Globals::Canvas->local.cells[i] == 99 + i);
        }
    }
};

struct Player {
    Matrix transformMatrix;
    bool active = true;
    int isActive() { return active; }
};

struct Explosion {
    std::function<void()> onRender;
    void render() {
        events.push_back("explosion");
        if (onRender) onRender();
    }
};

struct Trail { void render() { events.push_back("trail"); } };

struct PlayerFighter {
    AEGeometry *wreckGeometry = nullptr;
    AEGeometry *geometry = nullptr;
    AEGeometry *subGeometry = nullptr;
    Player *player = nullptr;
    Explosion *explosion = nullptr;
    Trail *trail = nullptr;
    int state = 1;
    int field_0x1f8 = 0;
    void render();
};

// PRODUCTION_RENDER

static void expect(std::initializer_list<std::string> sequence) {
    assert(events == std::vector<std::string>(sequence));
    events.clear();
}

int main() {
    PaintCanvas canvas;
    Globals::Canvas = &canvas;
    AEGeometry wreck{"wreck", 1}, main{"main", 2}, sub{"sub", 7};
    Player player;
    for (int i = 0; i < 15; ++i) {
        canvas.local.cells[i] = 11 + i;
        player.transformMatrix.cells[i] = 99 + i;
    }
    Explosion explosion;
    Trail trail;
    PlayerFighter fighter;
    fighter.wreckGeometry = &wreck;
    fighter.geometry = &main;
    fighter.subGeometry = &sub;
    fighter.player = &player;
    fighter.explosion = &explosion;
    fighter.trail = &trail;

    fighter.render();
    expect({"wreck", "get", "set-player", "sub", "restore", "trail"});
    for (int i = 0; i < 15; ++i) assert(canvas.local.cells[i] == 11 + i);

    player.active = false;
    fighter.render();
    expect({"wreck"});

    fighter.state = 5;
    fighter.subGeometry = nullptr;
    fighter.render();
    expect({"wreck", "main", "trail"});

    player.active = true;
    fighter.state = 3;
    fighter.render();
    expect({"wreck", "explosion", "main"});

    fighter.subGeometry = &sub;
    fighter.render();
    expect({"wreck", "explosion", "get", "set-player", "sub", "restore"});
    for (int i = 0; i < 15; ++i) assert(canvas.local.cells[i] == 11 + i);

    fighter.state = 4;
    fighter.field_0x1f8 = 299;
    fighter.render();
    expect({"wreck", "explosion", "get", "set-player", "sub", "restore"});
    fighter.field_0x1f8 = 300;
    fighter.render();
    expect({"wreck", "explosion"});

    fighter.state = 3;
    explosion.onRender = [&] { fighter.state = 1; };
    fighter.render();
    expect({"wreck", "explosion"});
}
