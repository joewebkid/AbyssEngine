# Hud Runtime Event And Progress Migration

Date: 2026-08-17

## Scope

This package moves the proven Android ARMv7 `Hud` event, queue, touch and
progress region into the live C++ class. Runtime member offsets are now locked
from `Hud+0x000` through the image-span boundary at `Hud+0x298`.

The migrated region includes:

- the unknown prefix tail and Image2D slot through `+0x15c`;
- event origin, faction image and scroll state;
- event Strings at `+0x1e0`, `+0x1f4`, `+0x200` and `+0x228`;
- availability, cargo, quick-menu and shield-hit flags;
- current secondary weapon, equipment and event queue pointers;
- jump, cloak and docking-transfer progress bytes;
- fuel, quick-menu, touch and key-array state;
- the reserved dword at `+0x294` and image-span start at `+0x298`.

Unknown storage remains explicitly named by offset. No semantics were inferred
from padding or adjacency.

## Auto-Turret Correction

The previous host layout contained a separate `autoTurretFlags` byte after
`touchFlags`. Android `Hud::draw` reads `Hud+0x287 & 0x20`: this is byte 3 of
the 32-bit `touchFlags` member at `+0x284`, equivalent to mask `0x20000000`.

The false member was removed. Auto-turret pressed-state rendering now tests the
native bit directly, which also prevents every later member from being shifted
by an invented byte.

## Runtime Assertions

ARM-only `offsetof` assertions now cover the event origin, four Strings,
secondary-label X coordinate, all known flags and pointers, the touch/key
state, reserved `+0x294` dword and `quickMenuTopImage` at `+0x298`.

This means the live class, rather than only `HudArm32Layout`, now enforces the
complete pre-image region during every ARM verification build.

## ARM Result

| Function | Previous | Current | Instructions original/local |
| --- | ---: | ---: | ---: |
| `Hud::Hud` | `92.7%` | `97.6%` | `82/82` |
| `Hud::~Hud` | `73.3%` | `88.2%` | `80/81` |
| `Hud::init` | `9.8%` | `10.3%` | `1077/1125` |
| `Hud::hudEvent` | `9.7%` | `9.7%` | `1088/885` |
| `Hud::touchEnd` | not baselined | `92.3%` | `45/46` |

The following small accessors are linked- and raw-byte-exact in the current
ARM build:

- `Hud::releaseAllKeys`;
- `Hud::jumpMapSelected`;
- `Hud::setJumpMapSelected`;
- `Hud::cargoFull`.

Across all 48 verified `Hud` functions, fuzzy average is `58.6%`; nine are
linked-exact and eight are raw-byte-exact. Whole-class byte equality is not
claimed.

The constructor has the exact instruction count. Its only remaining immediate
offset differences are the secondary-weapon label String, currently `+0x304`
instead of `+0x3b4`, and camera-mode label String, currently `+0x408` instead
of `+0x51c`.

## Validation

- UCRT64 native build links `libgof2.a` successfully.
- ARM corpus compiles `201/204` translation units; the same three unrelated
  `SolarSystem *` versus numeric system-index failures remain.
- `tools/lint_hacks.py` reports no byte-match definition hacks.
