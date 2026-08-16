# Hud Flight-Control Cluster Recovery

Date: 2026-08-16

## Evidence

This package is based on the Android HD ARM bodies and was cross-checked
against both available decompiler sources:

- `Hud::draw` at `0x163b90`;
- `Hud::init` at `0x1604e4`;
- `Hud::resetAnalogStick` and the analog/touch helpers;
- `Hud::checkIfQuickMenuIsEmpty` at `0x1613d0`;
- the `MGame::OnRender2D` call site that supplies elapsed time, current camera
  mode, and the next camera mode.

`drawControlsInterface` at `0x16600e` is not the missing implementation. Its
complete Android body is an intentional two-instruction no-op. The real flight
controls are branches inside `Hud::draw`.

## Recovered Image Roles

| Android slot | Resource | Confirmed role |
| --- | ---: | --- |
| `Hud+0x2e0/+0x2e4` | `0x04b5/0x04b4` | main fire pressed/idle |
| `Hud+0x2e8` | `0x0536` | target-context overlay |
| `Hud+0x2ec/+0x2f0` | `0x04bd/0x04bc` | secondary pressed/idle |
| `Hud+0x2fc/+0x300` | `0x04b3/0x04b2` | boost pressed/idle |
| `Hud+0x304/+0x308` | `0x04b7/0x04b6` | steering knob pressed/idle |
| `Hud+0x30c/+0x310` | `0x04b1/0x04b0` | dock/orbit pressed/idle |
| `Hud+0x314/+0x318` | `0x0546/0x0547` | auto-turret enabled/disabled |
| `Hud+0x31c` | `0x04c1` | steering base |
| `Hud+0x348` | `0x04c6` | phone reticle |
| `Hud+0x34c/+0x350` | `0x04bb/0x04ba` | quick menu pressed/idle |
| `Hud+0x354` | `0x04c3` | transient camera/event banner |
| `Hud+0x358` | `0x04c2` | secondary-weapon bottom banner |

The four camera-button pairs at `Hud+0x4f4..+0x510` are now represented as
idle and pressed arrays. Their resource rows are
`0x0528/0x04e9/0x04be/0x052a` and
`0x0527/0x04ea/0x04bf/0x0529`.

## Recovered Behavior

- `MGame::OnRender2D` now passes the real elapsed time, current camera mode,
  and `nextCamId(cameraMode)` to `Hud::draw`.
- Steering uses base, pressed/idle knob images, native touch bit `0x20`, and
  the recovered centre/current coordinate pairs. The previous lock-bracket
  interpretation of these fields was rejected.
- Main fire, secondary fire, boost, dock/orbit, camera, auto-turret, quick-menu,
  phone/iPad reticle, and camera-mode label branches now use their native image
  roles, touch bits, visibility gates, and alpha/pulse state.
- Camera labels use GameText IDs `217..220` and the native four-second fade.
- `checkIfQuickMenuIsEmpty` now has one source-shaped scan. It marks the menu
  non-empty for secondary equipment, jump drive, wingmen, or cloak, then
  updates the selected secondary-weapon string.
- `Hud+0x21d..+0x221` are preserved as the source capability flags for cloak,
  boost, shield, armor regeneration, and fire/autofire UI.

## Boundary

This is a source-backed behavioral recovery, not a whole-function ARM
byte-match claim. Cargo/passenger and mission-progress rows, the target-context
overlay consumer, and hit-direction overlays were completed by the follow-up
recorded in `HUD_MISSION_TARGET_OVERLAYS_2026-08-16.md`. A persistent settings
bridge for the Android touch-steering global remains queued. The C++ class
remains a host mirror and is not claimed to reproduce the original 32-bit
object ABI.

## Validation

The native UCRT64 build links `libgof2.a`. After rebuilding the ARM verifier
objects, the focused fuzzy source-shape baselines are:

| Function | Match |
| --- | ---: |
| `Hud::draw` | `1.8%` |
| `Hud::resetAnalogStick` | `33.3%` |
| `Hud::getAnalogX` | `47.6%` |
| `Hud::getAnalogY` | `47.6%` |
| `Hud::checkIfQuickMenuIsEmpty` | `87.1%` |

The lower whole-`draw` score is expected at this stage: the local function is
far shorter than the original `3223` instructions and still uses the host
class layout and reorganized control flow. None of these figures is a linked-
or raw-byte match. The incremental ARM build compiled `201/204` translation
units; its three unrelated `SolarSystem *`/`int` signature failures remain
pre-existing verifier debt.
