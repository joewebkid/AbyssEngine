# Hangar Touch-Return ABI ARM Pass

Date: 2026-08-23

## Scope

This pass recovers the return protocol between the station owner and the
Hangar child window. It does not claim to finish the large
`HangarWindow::OnTouchEnd` behavior body.

| Method | Android address | Recovered signature |
| --- | ---: | --- |
| `HangarWindow::OnTouchEnd` | `0x14c740` | `int (HangarWindow *, int, int)` |
| `ModStation::OnTouchEnd` | `0x0d7600` | `void (ModStation *, int, int, void *)` |

IDA's Android thunk and full body both declare the Hangar method as `int`.
The Android `ModStation` caller consumes that value before it runs the station
close cleanup. The automatic Ghidra output's `void` prototype is therefore a
decompiler inference error, not an ABI fact.

## Confirmed Return Protocol

`HangarWindow::OnTouchEnd` returns `0` for internal Hangar handling:

- suppressed touch-end, list/detail interaction, scrolling and tab actions;
- blueprint, mount/demount, buy/sell, ship swap and DLC dialogs;
- paid-credit and free-credit menus;
- closing the `ListItemWindow` detail view;
- Layout close while the selected tab is the blueprint tab (`4`) or the
  store-specific tab (`3`).

It returns `1` on the top-level Layout close path when all of these hold:

1. no detail view owns the close;
2. the active tab is neither `4` nor `3`;
3. `HangarWindow::readyToClose()` accepts the close.

Before that Layout-close `1`, the Android body calls `setSellMode(false)`, clears the
selected item and resets the Hangar list's current item index. The local
source already performs the equivalent cleanup through `setSellMode(false)`
and `resetSelection()`.

The later `HANGAR_MODAL_CLOSE_CREDITS_ARM_2026-08-24.md` audit found one
additional, independently confirmed owner-close route: the station-launched
paid-credit flow. `ModStation+0x18` marks that temporary Hangar route; its
dialog close clears the byte and returns `1` to the station owner. Internal
Hangar credit screens still return `0`.

## Source-Backed Changes

- Changed the public `HangarWindow::OnTouchEnd(int, int)` declaration and its
  active definition from `void` to `int`.
- Made every existing active internal exit return explicit `0`; the confirmed
  `readyToClose()` close path returns `1`.
- Replaced `HangarWindow_OnTouchEnd_ote` in `ModStation` with the direct
  typed `HangarWindow::OnTouchEnd(x, y)` call.
- Removed the local fake `HangarWindow` declaration from `ModStation.cpp` in
  favor of the canonical header.

## Boundary

The current `HangarWindow::OnTouchEnd` remains a mixed-recovery body. The
return ABI is source-backed, but this pass does not certify every purchase,
blueprint, social-credit, ship-swap, String-lifetime or stack-local detail as
identical to Android. `readyToClose()` is used as the confirmed existing
predicate; its full predicate body is a separate audit target.

## Verification

The UCRT64 `gof2` static library links successfully.

Focused ARM validation compiles `201/204` translation units; the three known
failures are unrelated `SolarSystem *` type errors. The checkpoint is:

| Method | Match | Target/base instructions |
| --- | ---: | ---: |
| `HangarWindow::OnTouchEnd` | 5.5% | 2154 / 1487 |
| `ModStation::OnTouchEnd(int, int, void *)` | 6.4% | 2022 / 906 |

Neither function is linked- or raw-byte-exact. These figures are not evidence
against the recovered ABI; whole-body control flow, host-side object layout
and local lifetime remain substantial matching work.

## Evidence

- Android 2.0.16 `gof2_libgof2hdaa_full_ida.c`,
  `_ZN12HangarWindow10OnTouchEndEii` at `0x14c740`;
- its Android thunk at `0x651c0`, which returns the same integer result;
- the direct `ModStation::OnTouchEnd` Hangar owner branch, which closes the
  station subwindow only after a nonzero child return;
- Ghidra retained only to record the conflicting automatic `void` inference.
