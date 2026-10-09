#include <type_traits>
#include <utility>

#include "engine/file/FileRead.h"
#include "game/world/Galaxy.h"

static_assert(std::is_same_v<decltype(std::declval<FileRead &>().loadStation(0)), Station *>);
static_assert(std::is_same_v<decltype(std::declval<Galaxy &>().getStation(0)), Station *>);
