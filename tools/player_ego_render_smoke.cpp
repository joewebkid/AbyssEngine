#include <cassert>
#include <cstdint>
#include <vector>

static std::vector<int> calls;

struct AEGeometry {
    int id;
    void render() { calls.push_back(id); }
    void setVisible(bool visible) { calls.push_back(visible ? 91 : 90); }
};
struct Explosion {
    int id;
    void render() { calls.push_back(id); }
};
struct TractorBeam {
    void render() { calls.push_back(70); }
};
struct RepairBeam {
    int id;
    void render() { calls.push_back(id); }
};
template <typename T> struct Array {
    unsigned count;
    T *data_;
    unsigned size() const { return count; }
    T operator[](unsigned index) const { return data_[index]; }
};
struct Level {
    void enableMovingStars(bool enabled) { calls.push_back(enabled ? 81 : 80); }
};
struct PlayerEgo {
    Level *level = nullptr;
    bool dead = false;
    uint8_t requestedVisible = 0;
    void *explosion = nullptr;
    void *explosion2 = nullptr;
    int explosionTimer = 0;
    AEGeometry *geometry = nullptr;
    intptr_t field_0xac = 0;
    int emergencySystemTimer = 0;
    void *gunMuzzleRoot = nullptr;
    void *gunYawGeo = nullptr;
    uint8_t turretMode = 0;
    void *field_0x30 = nullptr;
    void *field_0x2c = nullptr;
    uint8_t turretActive = 0;
    void *tractorBeam = nullptr;
    Array<RepairBeam *> *repairBeams = nullptr;
    uint8_t freeze = 0;
    int dockingPointIndex = 0;
    bool isDead() { return dead; }
    void render(bool allowHud);
};

// PRODUCTION_METHOD

static void expect(std::initializer_list<int> wanted) {
    assert(calls == std::vector<int>(wanted));
    calls.clear();
}

int main() {
    Level level;
    AEGeometry ship{10}, emergency{11}, yaw{12}, extra{13}, root{14};
    Explosion first{20}, second{21};
    TractorBeam tractor;
    RepairBeam beam1{30}, beam2{31};
    RepairBeam *beamData[] = {&beam1, &beam2};
    Array<RepairBeam *> beams{2, beamData};
    PlayerEgo ego;
    ego.level = &level;
    ego.geometry = &ship;
    ego.explosion = &first;
    ego.explosion2 = &second;

    ego.dead = true;
    ego.explosionTimer = 2999;
    ego.render(false);
    expect({20, 10, 21, 81});
    ego.explosionTimer = 3000;
    ego.render(false);
    expect({20, 21, 81});

    ego.dead = false;
    ego.render(true);
    expect({});

    ego.requestedVisible = 1;
    ego.field_0xac = reinterpret_cast<intptr_t>(&emergency);
    ego.emergencySystemTimer = 1;
    ego.gunMuzzleRoot = reinterpret_cast<void *>(1);
    ego.gunYawGeo = &yaw;
    ego.turretMode = 1;
    ego.field_0x30 = &extra;
    ego.field_0x2c = &root;
    ego.turretActive = 1;
    ego.tractorBeam = &tractor;
    ego.repairBeams = &beams;
    ego.dockingPointIndex = 2;
    ego.render(true);
    expect({20, 10, 11, 12, 91, 14, 70, 30, 31, 81});

    ego.explosion = nullptr;
    ego.explosion2 = nullptr;
    ego.emergencySystemTimer = 0;
    ego.gunMuzzleRoot = nullptr;
    ego.turretActive = 0;
    ego.tractorBeam = nullptr;
    ego.repairBeams = nullptr;
    ego.freeze = 1;
    ego.render(true);
    expect({10, 90, 14, 80});
    ego.freeze = 0;
    ego.dockingPointIndex = 1;
    ego.render(true);
    expect({10, 90, 14, 80});
    ego.render(false);
    expect({10, 90, 14, 80});
}
