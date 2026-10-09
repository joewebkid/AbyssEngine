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
