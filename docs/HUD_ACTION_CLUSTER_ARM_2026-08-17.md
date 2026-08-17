# Hud Action Cluster ARM Pass

Date: 2026-08-17

## Scope

This package restores the Android-backed runtime prelude and the right-side
action cluster inside `Hud::draw` at `0x163b90`. The primary evidence is the
Android 2.0.16 body in IDA/Hex-Rays, cross-checked against Ghidra and the live
ARM field layout.

The recovered slice covers:

- shield-hit and auxiliary countdown timers;
- boost and cloak readiness transitions;
- quick-menu, secondary, boost and primary-action image selection;
- disabled/highlight colors and flash-timer ordering;
- the low byte of the `Hud+0x284` touch mask;
- the delayed secondary-projectile state at `Level+0x69`.

## Confirmed Runtime Behavior

The shield-hit flag at `Hud+0x244` remains active for 500 ms. Its timer at
`+0x46c` is advanced before the status bars are drawn and reset together with
the flag. The independent dword at `+0x470` counts down while positive.

Boost readiness is edge-triggered. A transition to `getBoostRate() == 1.0`
sets `Hud+0x474`, starts the `+0x484/+0x488` flash pair at `2000/80`, and writes
the native placement value at `Hud+0x47c`. The semantic role of `+0x47c` is not
yet named because the recovered body proves its value but not its complete set
of consumers.

Cloak readiness uses `Hud+0x476` and starts the quick-menu flash pair at
`+0x498/+0x49c`. Android decrements this pair after the secondary-action branch.
The former local decrement of `+0x48c/+0x490` was removed: those secondary
flash fields are selected and reset here, but are not decremented by this
portion of the original Android body.

## Action Map

| Action | Low-byte mask | Pressed / idle Image2D | Coordinates |
| --- | ---: | --- | --- |
| boost | `0x02` | `0x04b3 / 0x04b2` | `Hud+0x410/+0x412` |
| quick menu | `0x04` | `0x04bb / 0x04ba` | `Hud+0x416/+0x418` |
| secondary | `0x08` | `0x04bd / 0x04bc` | `Hud+0x3ec/+0x3ee` |
| primary | `0x10` | `0x04b5 / 0x04b4` | `Hud+0x3e4/+0x3e6` |
| steering | `0x20` | `0x04b7 / 0x04b6` | live/base steering pairs |
| dock | `0x40` | `0x04b1 / 0x04b0` | `Hud+0x3f8/+0x3fa` |
| camera | `0x80` | camera-mode arrays | `Hud+0x3f2/+0x3f4` |

These branches read `Hud+0x284` as an unsigned byte. The high auto-turret bit
`0x20000000` still uses the complete 32-bit `touchFlags` field.

The yellow action tint is selected when the mouse cursor or quick menu is
active. The secondary banner temporarily restores white for mouse input, then
restores the action tint. Boost is disabled by mouse input, mining, the hacking
game, or docking. The previous `letterbox` test in that branch was incorrect.

## Level Secondary State

Android `Hud::draw` keeps the secondary control visible when `Level+0x69` is
non-zero, even when the normal item/amount predicate is false. Android
`Player::shoot` sets the byte for EMP bomb, nuke and ionizing-missile paths;
`Gun::ignite` clears it for the delayed EMP/nuke detonation. The live `Level`
layout now exposes the byte as `manualSecondaryActive`, with the old
`field_69` name retained as an ABI alias.

This naming is source-backed by the producer/consumer lifecycle. It does not
claim that every weapon-specific use of the byte has been recovered.

## ARM Result

| Function | Before | After | Result |
| --- | ---: | ---: | --- |
| `Hud::draw` | `8.1%` | `13.9%` | fuzzy, `3223/2003` instructions |
| `Hud::firePressed` | `66.7%` | `100.0%` | linked- and raw-byte-exact |

Across all 48 verified Hud functions:

- average similarity rises from `68.5%` to `69.4%`;
- linked-exact functions rise from 21 to 22;
- raw-byte-exact functions rise from 15 to 16.

The full `Hud::draw` function is not byte-matched. Its original body is much
larger than the current source, and several early platform/control, menu,
target, event and string-lifetime regions remain separate recovery packages.

## Validation

- UCRT64 links `cmake-build-ucrt/libgof2.a` successfully.
- ARM corpus builds `201/204` translation units.
- The three existing `SolarSystem *` versus integer failures remain unrelated.
- `Hud::firePressed` is identical after normalization.
- `Hud`, `HudArm32Layout` and `Level` ARM offset assertions compile.
