# MenuTouchWindow Main Dispatcher And Free-Camera ARM Pass

Date: 2026-08-23

## Scope

This package restores the Android main-state body of
`MenuTouchWindow::OnTouchEnd(int, int, void *)` and closes the free-camera
register-lifetime question left by the preceding `MGame` touch pass.

Primary evidence:

- Android 2.0.16 `MenuTouchWindow::OnTouchEnd` at `0x12aabc`;
- Android 2.0.16 `MenuTouchWindow::isShowingMessage` at `0x12cffa`;
- Android 2.0.16 `MenuTouchWindow::isMakingScreenshot` at `0x12d00e`;
- Android 2.0.16 `MGame::freeCamTouchEnd` at `0x178d30`;
- Android 2.0.16 `MGame::OnTouchEnd` at `0x17a144`;
- independent IDA/Ghidra output and local GNU ARM disassembly;
- the matching NDK r18b object produced by `tools/verify/build_objs.sh`.

## Coordinate Contract

The native method receives X in `r1` and Y in `r2` and forwards that order to
`Layout`, `ChoiceWindow`, `TouchButton`, `TouchSlider`, `MissionsWindow`, and
`ScrollTouchWindow`. The recovered C++ previously named the formal parameters
as Y/X and then reversed them again in every state helper. Calls from `MGame`
already used X/Y, so that representation changed runtime hit testing.

The full `OnTouchEnd` helper family now uses the native X/Y order. This is a
behavior correction backed by the call sites and ARM register flow; it does
not change the mangled ABI.

## Main Button Dispatcher

The primary button array is the pointer at `MenuTouchWindow+0x04`. A successful
`TouchButton::OnTouchEnd` reads the 64-bit `(id, secondary)` pair and dispatches
the following confirmed IDs when `secondary == 0`:

| ID | Native result |
| ---: | --- |
| `0` | enter state `17` |
| `1` | reload previews, select row zero, build load rows, enter state `1` |
| `2` | reload previews, select row zero, build save rows, enter state `2` |
| `3` | enter options state `3` |
| `4` | enter scroll/info state `4` |
| `6` | show GameText `523` and arm the module-exit confirmation |
| `10` | enter missions state `9`; initialize or allocate `MissionsWindow` |
| `11` | load `Globals::lastRecordWritten` |
| `12` | enter state `10` and build the cargo/load summary dialog |
| `18` | set the first-byte cutscene skip request and return nonzero |
| `19` | enter state `13` |
| `25` | enter language state `14` |
| `107` | enter state `18` |

The cargo dialog uses GameText `index + 1274` for each item, appends the amount
when it is at least two, uses GameText `286` for an empty cargo hold, and builds
the title from GameText `166` plus `current / maximum` load in tonnes.

## Auxiliary Button Array

The independently allocated array at `MenuTouchWindow+0xc0` is shared by
social, policy, restore, and scroll-related controls. The earlier constructor
translation incorrectly inserted Leaderboards and Achievements into the main
array at `+0x04`. Constructor and touch routing now agree with the native
layout:

- ID `23`: open Google Play leaderboards when linked;
- ID `24`: open Google Play achievements when linked;
- IDs `23/24` while unlinked: show the native `No link with Google+` dialog
  with GameText `3398` and arm the link request;
- ID `53`: set application-data bytes `+0x4c = 0`, `+0x3d = 1`, then show the
  GameText `71/425` dialog;
- ID `17`: show GameText `53` and arm the quit/return confirmation.

The five DLC selector buttons remain in the separate array at
`MenuTouchWindow+0xf8`; this pass does not conflate that array with `+0xc0`.

## Free-Camera Register Audit

`isMakingScreenshot` is six instructions and is linked- and byte-exact. It
loads signed `MenuTouchWindow+0x184` into `r1` and returns whether that value is
greater than `-1` in `r0`.

The call at `MGame::OnTouchEnd+0xb28` reloads only `r0` (`this`) and `r3`
(touch ID) before `freeCamTouchEnd`. This is intentional: the recovered native
body of `freeCamTouchEnd` never consumes its two coordinate parameters. It
uses only `this`, the touch ID, and the stored drag deltas at `MGame+0x134`.
Therefore the apparent stale `r1/r2` values are compiler dead-argument
elimination, not missing gameplay logic. The C++ call may keep real X/Y values
without changing behavior.

## Verification

- UCRT64 `gof2` links successfully as `libgof2.a`.
- ARM corpus remains `201/204`; the three failures are the pre-existing
  unrelated `SolarSystem *` versus integer type errors.
- `MenuTouchWindow::OnTouchEnd`: `7.4%`, `3001/1856` target/base instructions;
  the preceding checkpoint was `5.4%`, `3001/1274`.
- `MGame::OnTouchEnd`: unchanged at `8.5%`, `2863/2358`.
- `MGame::freeCamTouchEnd`: `20.0%`, `56/64`.
- `MenuTouchWindow::isMakingScreenshot`: `100%` linked- and byte-exact,
  `6/6` instructions.

The larger main-state body improves both semantic coverage and fuzzy
similarity, but `MenuTouchWindow::OnTouchEnd` is not byte-matched. Its native
frame saves `d8-d13` and reserves 280 bytes, while the current object still
has different helper inlining, String temporary placement, state block order,
and exception cleanup shape.

## Remaining Work

1. Fold the remaining state helpers into native switch order and lifetimes.
2. Restore the exact cargo-summary String construction/cleanup graph.
3. Audit the modal-dialog label joins before attempting stack-frame tuning.
4. Match the `d8-d13` save set, 280-byte frame, and hidden-return slots only
   after the complete behavior body is present.
