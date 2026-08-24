# Hangar Store and Blueprint State Recovery

Date: 2026-08-24

## Scope

This pass replaces the remaining shim-based `HangarWindow::setSellMode(bool)`
body with the Android-HD control flow at `0x14bff0`. It also retains the
confirmed `OnTouchEnd` ship-swap cleanup at `0x14c740`: all Android
`LABEL_254` exits clear the two-byte pending-swap state.

## Confirmed Store Route

On Hangar tab `1`, the Android body:

1. Rejects invalid, absent, ship, slot, text-button, unselectable, and
   blueprint selections by clearing `buyMode`.
2. Stores the buy-side station amount, player amount, credits, and load, or
   schedules secondary-weapon auto-equip on the sell-side.
3. Commits the mixed list through `Item::extractItems` into the live ship
   cargo and station inventory.
4. Rebuilds the mixed item list, Store tab, Ship tab, selected item, and
   current-content height in that order.

The two introductory Store notices are Android GameText `588` and `589`.
Their original storage is an otherwise-untyped standalone global word/byte;
the host keeps source-equivalent one-shot local storage rather than following
the retired null `g_hw_itemFlags` pointer. This preserves the observed
behavior but does not claim the persistence owner has been recovered.

## Confirmed Blueprint Route

On tab `4`, `buyMode == 0` adds the selected ingredient using its blueprint
amount and current station index. When the blueprint completes:

- at the production station, Android formats GameText `211` with `#N`, creates
  the product from the global item table, adds it to cargo, and switches to
  tab `1`;
- at another station, it formats GameText `210` with `#N` and `#S`, queues a
  `PendingProduct`, and switches to tab `2`.

Both routes reset the blueprint, rebuild cargo/Store/Blueprint lists, and
then perform the source-confirmed secondary auto-equip scan.

For `buyMode != 0`, the native body zeroes the blueprint count, records the
saved item/credit/load state and opens GameText `212`. Blueprint IDs `210`
and `223` instead show GameText `528` with a non-affirmative dialog when the
current system has no routes; `routeWarningPending` is the typed owner of
that branch.

## Ship-Swap Cleanup

The second ship-purchase cancellation, the already-owned ship notice
(GameText `328`), and the insufficient-credits notice all converge on Android
`LABEL_254`. The recovered code clears `shipSwapPending` and
`swapConfirmFlag` before returning. The two fields are the 16-bit word at
`HangarWindow+0x90`.

## Verification

UCRT64 build:

```text
C:\msys64\ucrt64\bin\cmake.exe --build cmake-build-ucrt --target gof2 -- -k 0
```

completed successfully.

Focused ARM validation compiled `201/204` translation units. The three
skipped units retain known unrelated `SolarSystem *` type errors. The local
report `_work/hangar_set_sell_mode_after_20260824.json` is deliberately not
tracked.

| Method | ARM similarity | Target/base instructions |
| --- | ---: | ---: |
| `HangarWindow::setSellMode(bool)` | 13.5% | 708 / 643 |
| `HangarWindow::OnTouchEnd(int, int)` | 11.8% | 2154 / 1973 |
| `HangarWindow::OnTouchBegin(int, int)` | 27.2% | 448 / 390 |

These results are neither linked- nor raw-byte-exact. The remaining
`setSellMode` gap is chiefly ARM String temporary cleanup, one-shot-global
ownership, exact allocation/lifetime shape, and the host-side null guards.

## Evidence

- Android 2.0.16 `gof2_libgof2hdaa_full_ida.c`,
  `_ZN12HangarWindow11setSellModeEb` at `0x14bff0`;
- Android first-pass SDK extraction,
  `extracted/ui_input_hud/HangarWindow.c`;
- typed `BluePrint`, `Status`, `Ship`, `Station`, `Item`, and `HangarList`
  APIs in this repository.
