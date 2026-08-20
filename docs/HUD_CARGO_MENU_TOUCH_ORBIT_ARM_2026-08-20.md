# Hud cargo, menu, touch and orbit ARM pass (2026-08-20)

## Evidence

- Android 2.0.16 `libgof2hdaa.so` ARM bodies and symbol map.
- IDA/Ghidra-backed first-pass bodies in
  `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/Hud.c`.
- Focused ARM object comparisons after every accepted source-shape change.
- UCRT64 native build after each behavior package.

## Accepted recovery

- `Hud::catchCargo` now calls the typed `Status::replaceHash` member instead
  of a `void *` ABI shim. The returned `String` is constructed in the native
  hidden-return slot. Mission `#N`/`#Q` substitution, cargo-full messages,
  aggregate replacement and `ListItem` flags retain the Android branch order.
- Cargo labels use the native temporary shape
  `String(String(amount) + String("t "), false) + GameText[item + 0x4fa]`.
  This also removes the extra default-constructed result and copy temporaries.
- `Hud::drawMenu` reads Canvas, Layout and coordinate fields at the original
  draw sites. The top/middle/bottom frame loop, button loop and cloak/jump
  fuel gauge are behavior-identical, but no longer use non-native cached
  frame/header coordinates.
- `Hud::touchBegin` restores the native nonzero-element-first control flow.
  Its first two scans share the cached `keyArray->data_` pointer, while the
  release path reloads the key storage in the original loop shape.
- `Hud::drawOrbitInformation` restores source-order Canvas/font acquisition,
  station/system name lifetimes and direct global reads. The confirmed
  12-byte security-color stride and campaign/system override remain intact.

## ARM verification

| Function | Before | After | Target/base instructions |
| --- | ---: | ---: | ---: |
| `Hud::catchCargo` | 18.5% | 71.5% | 470 / 453 |
| `Hud::drawMenu` | 32.5% | 63.1% | 225 / 219 |
| `Hud::touchBegin` | 36.6% | 59.4% | 65 / 63 |
| `Hud::drawOrbitInformation` | 38.6% | 50.3% | 225 / 216 |

The complete 48-function Hud set now averages 84.8%, up from 82.4%, with 26
linked-exact and 19 byte-exact functions unchanged.

## Rejected experiment

An iPad call-frame rewrite for `Hud::init` moved fire-anchor/width evaluation
after `setCoordsSteer`, matching the decompiler statement order. Clang changed
the whole-function register allocation and reduced the ARM score from 64.7%
to 58.4%. The experiment was reverted. No stack scratch, inline assembly,
volatile register forcing or padding was retained.

## Build status and remaining work

- UCRT64 `libgof2.a` builds successfully.
- ARM compilation remains 201/204 with the same three unrelated
  `SolarSystem *` versus integer migration failures.
- The next dense Hud targets are `initHudMenu` (27.0%), `hudEvent` (24.9%),
  `touchedElement` (45.4%) and the large `draw` body (47.4%).
- The accepted functions are source-backed but not byte-exact. Remaining
  differences are register allocation, EH unwind tails, literal pools and a
  few local lifetimes; they must not be replaced by artificial scratch code.
