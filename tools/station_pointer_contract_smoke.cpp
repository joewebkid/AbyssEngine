#include <type_traits>
#include <utility>

#include "engine/file/FileRead.h"
#include "game/mission/Status.h"
#include "game/world/Galaxy.h"

static_assert(std::is_same_v<decltype(std::declval<FileRead &>().loadStation(0)), Station *>);
static_assert(std::is_same_v<decltype(std::declval<Galaxy &>().getStation(0)), Station *>);
static_assert(std::is_same_v<decltype(std::declval<Galaxy &>().getSystem(0)), SolarSystem *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().system), SolarSystem *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().getSystem()), SolarSystem *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().standing), Standing *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().getStanding()), Standing *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().planetNames), Array<AbyssEngine::String *> *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().getPlanetNames()), Array<AbyssEngine::String *> *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().planetTextures), Array<int> *>);
static_assert(std::is_same_v<decltype(std::declval<Status &>().getPlanetTextures()), Array<int> *>);
static_assert(std::is_same_v<decltype(std::declval<FileRead &>().loadStationsBinary(std::declval<SolarSystem *>())), Array<Station *> *>);
