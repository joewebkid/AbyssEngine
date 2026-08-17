# Hud Runtime Coordinate And Tail Migration

Date: 2026-08-17

## Scope

This package completes the live Android ARMv7 `Hud` object layout. The runtime
class now preserves every byte from `Hud+0x000` through `Hud+0x53b`, and an
ARM-only assertion locks `sizeof(Hud) == 0x53c`.

The migration adds the missing `+0x3c0` dword, restores the packed coordinate
holes at `+0x3e8`, `+0x428/+0x42a` and `+0x432`, and moves the timer, flash,
touch, camera and final ownership fields to their native positions.

## Confirmed Tail Map

| Native range / offset | Live role |
| --- | --- |
| `+0x3c0` | previously omitted dword between the two String/geometry regions |
| `+0x3c4..+0x3dc` | quick-menu frame and row geometry |
| `+0x3e0..+0x463` | complete 66-element packed `uint16_t` coordinate span |
| `+0x464/+0x468/+0x46c` | charge, docking-transfer and hit-flash timers |
| `+0x474/+0x476` | boost/cloak ready latches |
| `+0x484/+0x488` | boost flash remaining/pulse |
| `+0x48c/+0x490` | secondary-action flash remaining/pulse |
| `+0x498/+0x49c` | quick-menu flash remaining/pulse |
| `+0x4a0/+0x4a5` | autofire and tutorial-fire bytes |
| `+0x4ac..+0x4b8` | left/right/top/bottom hit-direction timers |
| `+0x4bc/+0x4c0/+0x4c4` | time-extender timer/duration and mining-hint pulse |
| `+0x4c8` | message-active byte |
| `+0x4d4..+0x4f0` | menu/touch/layout integers and event margins |
| `+0x4f4..+0x510` | four idle and four pressed camera-mode images |
| `+0x514/+0x518/+0x51c` | previous camera mode, label timer and camera String |
| `+0x528/+0x52c` | hacking-active byte and cargo aggregate |
| `+0x530/+0x534/+0x538` | uint array, digit sprite and multiplier image |

The secondary and quick-menu flash pairs and all four hit-direction timers are
not host-only additions. Android `Hud::draw` addresses them directly as dword
indices 291/292, 294/295 and 299..302. `HudArm32Layout` now records these
source-backed roles instead of leaving the same bytes as broad unknown spans.

The temporary `HudCameraImageSlots` helper has been removed. Camera resources
are created and consumed directly through the native `cameraIdleImages` and
`cameraPressedImages` arrays.

## ARM Result

The constructor reaches `100.0%` with `82/82` instructions and is
linked-exact. It is not raw-byte-exact because relocation bytes differ before
link normalization.

The exact tail offsets also improve several consumers:

| Function | Previous | Current |
| --- | ---: | ---: |
| `Hud::init` | `9.8%` | `27.2%` |
| `Hud::touchedElement` | `8.3%` | `14.0%` |
| `Hud::getAnalogX/Y` | `72.7%` | `90.9%` |
| `Hud::setTimeExtender` | `83.3%` | byte-exact |
| `Hud::resetAnalogStick` | `33.3%` | byte-exact |

Across all 48 verified `Hud` functions, average similarity is `68.5%`;
21 are linked-exact and 15 are raw-byte-exact. The immediately preceding
image-span snapshot was `59.7%`, nine linked-exact and eight raw-byte-exact.

The follow-up Array ABI package restores the out-of-line
`ArrayReleaseClasses<TouchButton *>` specialization. `Hud::~Hud` is now `97.5%`
linked-exact with matching `80/80` instruction counts; only two unwind/LSDA
tail instructions differ. The specialization itself is `100.0%` linked-exact
at `29/29`. See `ARRAY_TOUCHBUTTON_ARM_ABI_2026-08-17.md`.

The subsequent pause-button pass adds a low-byte view of the full touch mask
at `+0x284` and raises `Hud::drawPauseButton` from `61.9%` to `92.7%`
linked-exact. See `HUD_PAUSE_BUTTON_ARM_2026-08-17.md`.

## Validation

- UCRT64 native build links `libgof2.a` successfully.
- ARM corpus builds `201/204` translation units; the same three unrelated
  `SolarSystem *` versus integer system-index failures remain.
- ARM compilation passes all live `offsetof` checks and
  `sizeof(Hud) == 0x53c`.
- `tools/verify/verify.py --only '^_ZN3Hud'` compares 48 functions.
- ARM `nm` reports a strong specialization definition in
  `ArrayInstantiations.o` and an undefined out-of-line reference in `Hud.o`.

This package establishes the object ABI. It does not claim that large bodies
such as `Hud::draw`, `Hud::init` or `Hud::hudEvent` are byte-matched.
