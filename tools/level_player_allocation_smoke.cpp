#include "game/ship/Player.h"
#include "game/ship/PlayerEgo.h"

#include <cstddef>

static_assert(sizeof(Player) > 0x114, "host Player exceeds ARM allocation");
static_assert(sizeof(PlayerEgo) > 0x3a0, "host PlayerEgo exceeds ARM allocation");
static_assert(offsetof(Player, field_69) > 0x69, "host field follows widened pointer");
static_assert(offsetof(Player, field_69) == offsetof(Player, empDisabledByte) + 1,
              "source-backed byte stays paired with EMP byte");

int main() {}
