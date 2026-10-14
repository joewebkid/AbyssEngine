#include <cassert>
#include <initializer_list>
#include <vector>

struct Radar {};
struct Level {};
struct Hud {};

static Radar radar;
static Level level;
static Hud hud;
static std::vector<int> calls;

struct TractorBeam {
    void update(int dt, Radar *r, Level *l, Hud *h) {
        assert(dt == 17 && r == &radar && l == &level && h == &hud);
        calls.push_back(1);
    }
};
struct RepairBeam {
    int id;
    void update(int dt, Radar *r, Level *l, Hud *h) {
        assert(dt == 17 && r == &radar && l == &level && h == &hud);
        calls.push_back(id);
    }
};
template <typename T> struct Array {
    unsigned count;
    T *data;
    unsigned size() const { return count; }
    T operator[](unsigned index) const { return data[index]; }
};
struct PlayerEgo {
    void *tractorBeam = nullptr;
    Array<RepairBeam *> *repairBeams = nullptr;
    Level *level = &::level;
    void updateBeams(int dt, Radar *radar, Hud *hud);
};

void PlayerEgo::updateBeams(int dt, Radar *radar, Hud *hud) {
// PRODUCTION_FRAGMENT
}

static void expect(std::initializer_list<int> wanted) {
    assert(calls == std::vector<int>(wanted));
    calls.clear();
}

int main() {
    TractorBeam tractor;
    RepairBeam first{2}, second{3};
    RepairBeam *data[] = {&first, &second};
    Array<RepairBeam *> beams{2, data};
    PlayerEgo ego;

    ego.updateBeams(17, &radar, &hud);
    expect({});
    ego.tractorBeam = &tractor;
    ego.updateBeams(17, &radar, &hud);
    expect({1});
    ego.tractorBeam = nullptr;
    ego.repairBeams = &beams;
    ego.updateBeams(17, &radar, &hud);
    expect({2, 3});
    ego.tractorBeam = &tractor;
    ego.updateBeams(17, &radar, &hud);
    expect({1, 2, 3});
    beams.count = 0;
    ego.updateBeams(17, &radar, &hud);
    expect({1});
}
