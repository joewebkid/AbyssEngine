# Hud Runtime Image Span Migration

Date: 2026-08-17

## Scope

This package moves the complete Android ARMv7 `Hud` Image2D region into the
live C++ object. All 71 consecutive 32-bit handles now occupy
`Hud+0x298..+0x3b0`, and the secondary-weapon label String immediately after
them occupies its native `Hud+0x3b4` slot.

The old `HudInitImageSlots` structure was compact host storage: its names
recorded source offsets, but its physical placement did not. It has been
removed. Every migrated draw and coordinate consumer now reads the live native
slot directly. Only the eight camera-mode images remain in a small temporary
helper until the tail is migrated to `Hud+0x4f4..+0x510`.

## Confirmed Resource Map

The following map is transcribed from Android `Hud::init` and its direct
consumers. A dash means that the slot is structurally confirmed but its exact
producer or role is still unnamed.

| Native offset | Role | Resource ID |
| --- | --- | ---: |
| `+0x298/+0x29c/+0x2a0` | quick-menu top/bottom/middle | `0x4cf/0x4d0/0x4d1` |
| `+0x2a4..+0x2b0` | shield frame/hit/background/fill | `0x4ac..0x4af` |
| `+0x2b4..+0x2c4` | armor frame/low/background/regen/fill | `0x4aa/0x4ab/0x4a7/0x4a8/0x524` |
| `+0x2c8/+0x2cc/+0x2d0/+0x2d4` | gamma fill/background/frame and divider | `0x1f5b/0x1f5a/0x1f59/0x4a9` |
| `+0x2d8/+0x2dc` | unnamed image handles | - |
| `+0x2e0/+0x2e4` | main action pressed/idle | `0x4b5/0x4b4` |
| `+0x2e8` | target-context overlay | `0x536` |
| `+0x2ec/+0x2f0` | secondary action pressed/idle | `0x4bd/0x4bc` |
| `+0x2f4/+0x2f8` | pause pressed/idle | `0x4b9/0x4b8` |
| `+0x2fc/+0x300` | boost pressed/idle | `0x4b3/0x4b2` |
| `+0x304/+0x308` | steering knob pressed/idle | `0x4b7/0x4b6` |
| `+0x30c/+0x310` | dock action pressed/idle | `0x4b1/0x4b0` |
| `+0x314/+0x318/+0x31c` | auto-turret enabled/disabled and steering base | `0x546/0x547/0x4c1` |
| `+0x320/+0x324` | mission timer and cargo panels | `0x4c5/0x520` |
| `+0x328/+0x32c/+0x330` | unnamed image handles | - |
| `+0x334..+0x344` | passenger, production, volatile and mission-status panels | `0x1f43/0x1f61/0x1f60/0x1f5c/0x1f42` |
| `+0x348` | reticle / iPad fire image | `0x4c6` |
| `+0x34c/+0x350` | quick-menu pressed/idle | `0x4bb/0x4ba` |
| `+0x354/+0x358` | event and secondary-weapon banners | `0x4c3/0x4c2` |
| `+0x35c` | quick-menu header, created by `initHudMenu` modes 0/1/2/3 | `0x4f5/0x4f6/0x4f3/0x4f4` |
| `+0x360..+0x36c` | armor/shield hit-direction images | `0x525/0x526/0x52b/0x52c` |
| `+0x370/+0x374` | fuel icon/bar | `0x537/0x538` |
| `+0x378/+0x37c` | progress panel and charge fill | `0x53a/0x539` |
| `+0x380/+0x384` | mission/production transfer markers | `0x1f40/0x1f5f` |
| `+0x388/+0x38c` | duplicate progress panel and transfer fill | `0x53a/0x1f41` |
| `+0x390..+0x3a0` | five unnamed native images | `0x540/0x541/0x53f/0x542/0x543` |
| `+0x3a4/+0x3a8` | two unnamed native images | `0x1f58/0x1f57` |
| `+0x3ac/+0x3b0` | second dock-image pair | `0x4b1/0x4b0` |

The repeated `0x53a` and `0x4b1/0x4b0` resources are intentional duplicate
native handles. They were not folded together merely because their resource
IDs match.

## Runtime Checks

ARM-only `offsetof` assertions lock representative boundaries and consumers
across the region, including `+0x298`, `+0x2c8`, `+0x2e0`, `+0x2f4`,
`+0x30c`, `+0x348`, `+0x354`, `+0x35c`, `+0x370`, `+0x38c`, `+0x3b0` and the
String at `+0x3b4`.

At the image-span stage, the constructor is `98.8%` similar with the exact
`82/82` instruction count. Its only immediate-offset difference is the camera-mode
String: local `+0x4b8`, Android `+0x51c`. The destructor is `90.7%`
(`80/81`). Across 48 verified `Hud` functions the current average is `59.7%`;
nine are linked-exact and eight are raw-byte-exact.

`Hud::init` is `9.8%` (`1077/1118`). Removing a temporary aggregate zero of
the compact camera helper reduced its fuzzy score, but this is retained because
Android writes all eight camera handles directly with `Image2DCreate` and does
not perform that extra zeroing. Source evidence takes precedence over a
coincidental score improvement in a still-misaligned tail.

## Remaining Boundary

At this stage, the coordinate and timer/camera regions are not yet exact. A
dedicated ARM size probe measures the object at `0x50c` bytes versus the Android
allocation size `0x53c`.
The next migration must restore the omitted 16-bit coordinate holes in
`+0x3c4..+0x463`, then move timers, camera image arrays, camera String and the
final fields through `+0x53b`. No whole-class or `Hud::init` byte match is
claimed by this package.

Follow-up: `HUD_RUNTIME_TAIL_2026-08-17.md` completes this migration. The live
ARM object now has the exact `0x53c` size and the constructor is linked-exact.

## Validation

- UCRT64 native build links `libgof2.a` successfully.
- ARM corpus builds `201/204` translation units; the same three unrelated
  `SolarSystem *` versus integer system-index failures remain.
- `tools/verify/verify.py --only '^_ZN3Hud'` compares 48 functions.
