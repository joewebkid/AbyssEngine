# Hud init ARM recovery (2026-08-20)

## Evidence

- Android 2.0.16 `libgof2hdaa.so` ARM body and exported symbols.
- IDA/Ghidra-backed `Hud::init` body in the recovered SDK first-pass source.
- `Hud` ARM32 offset assertions and focused object comparisons after each
  accepted source-shape change.

## Recovered initialization contract

- The 75 static `Image2DCreate` call sites now execute directly in native
  resource/slot order. The iPad branch creates resources `0x04c6` and
  `0x06aa` and aliases the reticle; the phone branch creates `0x04c6` directly
  in the reticle slot.
- The temporary descriptor array was removed. It was not present in Android
  and inflated the ARM stack frame from 188 to 752 bytes.
- `Layout+0x12c..0x14b` is copied into `Hud+0x4d4..0x4f3` as the native
  contiguous 32-byte span before coordinate calculations.
- Coordinate setup follows the Android image-width/height and layout-field
  order, including the unrounded hacking-panel width, iPad steer/fire remap,
  quick-menu frame geometry, hit-bar coordinates and pause placement.
- The fixed 20-entry event queue is allocated before the 25-entry touch-key
  array and its parallel 100-byte element-mask buffer.
- Ship capability bytes are populated through fresh native-order
  `Status::getShip()` calls: boost, shield, armor regeneration, fire-power UI,
  then cloak.
- Event, progress, flash, time-extender, camera-label and touch fields now use
  the confirmed Android values and write order. The faction logo uses the
  four-entry, four-byte-stride resource table.
- `Hud::init` and `checkIfQuickMenuIsEmpty` use their native `void` call ABI.
  `Globals::pause_x/pause_y` are integer screen coordinates; the prior float
  declarations introduced non-native VFP conversions.
- Non-native Canvas, Layout, Status, System and Ship null guards were removed
  only where the original body dereferences those initialized globals
  unconditionally.

## ARM verification

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `Hud::init` | 24.1% | 64.7% | 1077 / 1021 |
| `Hud::checkIfQuickMenuIsEmpty` | not re-baselined in this pass | 96.4% | 42 / 41 |

Intermediate checkpoints for `Hud::init` were 35.0% after removing the local
image descriptor table and 54.3% after restoring the post-coordinate field,
queue and touch-array order. Correcting the return/global ABI and contiguous
layout copy raised it to the final 64.7%.

No artificial stack scratch, volatile register forcing, inline assembly or
padding was retained. The 16-byte vector type models the two 128-bit copies
visible in the original body and preserves the same 32-byte field semantics.

## Build status and remaining work

- UCRT64 `libgof2.a` builds successfully.
- The ARM pass remains 201/204 with the same three unrelated
  `SolarSystem *` versus integer failures.
- `Hud::init` is source-backed but not byte-exact. Most remaining differences
  are in coordinate/iPad local lifetimes, stack reservation (196 versus 188
  bytes), compiler register allocation and literal-pool placement.
- A future pass should compare the two `Globals::setCoordsSteer/setCoordsFire`
  call frames and hidden temporaries as a focused unit. The recovered runtime
  order and values should not be replaced with speculative scratch locals.
