#include <cassert>
#include <utility>
#include <vector>

struct AERandom {
    std::vector<std::pair<int, int>> draws;
    unsigned pos = 0;
    int nextInt(int bound) {
        assert(pos < draws.size());
        const auto draw = draws[pos++];
        assert(draw.first == bound && draw.second >= 0 && draw.second < bound);
        return draw.second;
    }
};

struct Globals { static void *rnd; };
void *Globals::rnd = nullptr;

struct Route {
    std::vector<int> coords;
    bool loop = false;
    Route(const int *pts, int count) : coords(pts, pts + count) {}
    void setLoop(bool value) { loop = value; }
};

struct Vector { float x, y, z; };
struct PlayerFighter {
    static Route *stationRouteAliens;
    Route *route = nullptr;
    Vector workingPosition{};
};
Route *PlayerFighter::stationRouteAliens = nullptr;

void run_route(PlayerFighter *self, float x, float y, float z) {
    Route *&stationRouteAliens = PlayerFighter::stationRouteAliens;
    // PRODUCTION_ROUTE_SETUP
    // PRODUCTION_ROUTE_EFFECTS
}

int main() {
    const std::vector<int> waypoints = {
        -30000, -10000, 20000,
        5000, -10000, 20000,
        5000, -10000, 55000,
        -30000, -10000, 55000,
    };
    const std::vector<int> station = {
        40000, 0, 40000, 40000, 0, -40000,
        -40000, 0, -40000, -40000, 0, 40000,
    };
    Route *firstStation = nullptr;
    for (int countChoice = 0; countChoice != 3; ++countChoice) {
        AERandom rng;
        for (int i = 0; i != 12; ++i)
            rng.draws.push_back({i % 3 == 1 ? 10000 : 25000, 0});
        rng.draws.push_back({3, countChoice});
        rng.draws.push_back({4, 0});
        if (countChoice == 2) rng.draws.push_back({4, 0}); // duplicate retry
        for (int i = 1; i != countChoice + 2; ++i)
            rng.draws.push_back({4, i});
        Globals::rnd = &rng;
        PlayerFighter fighter;
        run_route(&fighter, 1.25f, -2.5f, 3.75f);
        assert(rng.pos == rng.draws.size());
        assert(fighter.route->coords == std::vector<int>(waypoints.begin(),
                                                         waypoints.begin() + (countChoice + 2) * 3));
        assert(fighter.route->loop);
        assert(fighter.workingPosition.x == 1.25f);
        assert(fighter.workingPosition.y == -2.5f);
        assert(fighter.workingPosition.z == 3.75f);
        assert(PlayerFighter::stationRouteAliens->coords == station);
        assert(PlayerFighter::stationRouteAliens->loop);
        if (firstStation == nullptr) firstStation = PlayerFighter::stationRouteAliens;
        assert(PlayerFighter::stationRouteAliens == firstStation);
        delete fighter.route;
    }
    delete PlayerFighter::stationRouteAliens;
}
