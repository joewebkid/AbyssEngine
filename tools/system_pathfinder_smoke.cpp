#include "game/world/SystemPathFinder.h"
#include "game/core/Globals.h"

#include <cassert>

using AbyssEngine::String;

// Only the services read by the production path-finder body are needed here.
String::String() : data(nullptr), length(0) {}
String::~String() = default;
Status::Status() : systemVisibilities(nullptr) {}
Status::~Status() = default;
Array<bool> *Status::getSystemVisibilities() { return systemVisibilities; }
Status *Globals::status = nullptr;

SolarSystem::SolarSystem(int id, String, int, bool, int, int, int, int, int,
                         int, int *, Array<int> *, Array<int> *routes, Array<int> *)
    : systemId(id), linkedSystemIds(routes) {}
SolarSystem::~SolarSystem() = default;
int SolarSystem::getIndex() { return systemId; }
uint32_t *SolarSystem::getRoutes() { return reinterpret_cast<uint32_t *>(linkedSystemIds); }

int main() {
    Array<bool> visibility;
    ArraySetLength(3, visibility);
    visibility[0] = visibility[1] = visibility[2] = true;
    Status status;
    status.systemVisibilities = &visibility;
    Globals::status = &status;

    Array<int> route0, route1, route2;
    ArrayAdd(1, route0);
    ArrayAdd(2, route1);
    int colors[3] = {};
    SolarSystem a(0, String(), 0, false, 0, 0, 0, 0, 0, 0, colors,
                  nullptr, &route0, nullptr);
    SolarSystem b(1, String(), 0, false, 0, 0, 0, 0, 0, 0, colors,
                  nullptr, &route1, nullptr);
    SolarSystem c(2, String(), 0, false, 0, 0, 0, 0, 0, 0, colors,
                  nullptr, &route2, nullptr);
    Array<SolarSystem *> systems;
    ArrayAdd(&a, systems);
    ArrayAdd(&b, systems);
    ArrayAdd(&c, systems);

    SystemPathFinder finder;
    Array<int> *path = finder.getSystemPath(&systems, 0, 2);
    assert(path && path->size() == 3 && (*path)[0] == 0 &&
           (*path)[1] == 1 && (*path)[2] == 2);
    delete path;
    assert(finder.getJumpDistance(&systems, 0, 2) == 2);

    visibility[2] = false;
    assert(finder.getSystemPath(&systems, 0, 2) == nullptr);
}
