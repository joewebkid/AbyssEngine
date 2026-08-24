# Hangar Touch-Begin Source-Shape ARM Pass

Date: 2026-08-24

## Scope

This pass recovers the Android input ownership and control flow for
`HangarWindow::OnTouchBegin`. It is deliberately limited to the child window;
`ModStation` still owns the outer station touch dispatch and ignores the
child's return value on touch begin.

| Method | Android address | Recovered signature |
| --- | ---: | --- |
| `HangarWindow::OnTouchBegin` | `0x14b740` | `int (HangarWindow *, int, int)` |
| `j__ZN12HangarWindow12OnTouchBeginEii` | `0x65088` | same integer return |

The return type does not participate in the C++ symbol mangling, but both the
Android thunk and the full body return zero. The local declaration is now
`int`, with every exit returning that confirmed zero result.

## Confirmed Android Flow

1. Reset the repeat/hold counters, then retain the exact integer result of
   `Layout::OnTouchBegin`.
2. When the Hangar dialog is active, route paid-credit buttons `12..16` plus
   slot `17`, or free-credit buttons `18..22`, before routing the dialog.
3. Otherwise initialise the scroll gesture, delegate the detail mode to
   `ListItemWindow`, and pick the current row from the Android Layout row
   recurrence.
4. In ship-upgrade mode on store tab `1`, commit the mixed item list back to
   ship cargo and station stock after selecting a ship row.
5. For an unfinished blueprint on another station, show the native GameText
   `288`/`289` prompt, substitute `#S` and `#C` for non-local blueprints, and
   set the dialog, pending-purchase and touch-end suppression bytes.
6. In the ordinary route, touch all tab and action buttons and use the retained
   Layout/tab result in the confirmed `autoEquipSecondaryWeapons` gate.

The involved offsets agree with the Android body and the existing typed
members: dialog active `+0x3c`, selected item `+0x68`, blueprint `+0x80`,
blueprint item `+0x84`, buy mode `+0x88`, blueprint count `+0x94`, scroll
offset `+0xb4`, touch Y values `+0xb8/+0xcc`, dragging `+0xd0`, paid/free
credit flags `+0xae/+0xb0`, and auto-equip state `+0xf8/+0xfc`.

## Intentional Local Boundary

Android relies on its touch-coordinate invariant and only tests the computed
row against the upper list bound. The recovered host code also rejects a
negative row before calling `HangarList::setCurrentItemIndex`; this is a local
runtime-safety guard for the mixed 64-bit build, not a claimed Android branch.

No new economy, blueprint, or auto-equip policy was added. The GameText IDs,
button ranges, station comparison and flag writes come directly from the
Android body.

## Verification

UCRT64 `gof2` links successfully.

Focused ARM validation compiles `201/204` translation units. The three skipped
units have known unrelated `SolarSystem *` type errors. The changed function
improved as follows:

| Method | Before | After | Target/base instructions after |
| --- | ---: | ---: | ---: |
| `HangarWindow::OnTouchBegin` | 8.8% | 27.2% | 448 / 390 |

It is neither linked- nor raw-byte-exact. The remaining gap is principally
the native String temporary/destructor graph and ARM stack/local lifetime,
not an untyped input-owner route. `OnTouchMove` and `highlightItem` were
measured in the same run and remain unchanged at 11.6% and 79.0% respectively.

## Evidence

- Android 2.0.16 `gof2_libgof2hdaa_full_ida.c`,
  `_ZN12HangarWindow12OnTouchBeginEii` at `0x14b740`;
- Android thunk at `0x65088`;
- typed button/row evidence in `HANGAR_LAYOUT_BUTTON_SLOTS_2026-08-16.md`;
- direct local owner call in `ModStation::OnTouchBegin`.
