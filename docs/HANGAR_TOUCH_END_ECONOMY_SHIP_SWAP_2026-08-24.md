# Hangar Touch-End Economy And Ship-Swap Pass

Date: 2026-08-24

## Scope

This pass extends the source-backed recovery of
`HangarWindow::OnTouchEnd` (`0x14c740`). It covers direct Android evidence for
the social-credit payload, blueprint confirmation/cancel behavior, and the
ship purchase/trade-in state machine. It does not claim to finish the whole
2,154-instruction ARM body.

## Confirmed Social-Credit Data

The Android code reads `dword_202844[n]` after the social actions. The source
loop uses offer indexes `0..4`; index zero is a RecordHandler/store action and
does not change credits.

The original `libgof2hdaa.so` `.rodata` bytes at virtual address `0x202844`
decode as:

| Offer index | Action | Credits |
| ---: | --- | ---: |
| `0` | record-store route | `0` |
| `1` | Like GOF2 on Facebook | `7,500` |
| `2` | Like Fishlabs on Facebook | `5,000` |
| `3` | Subscribe to YouTube | `5,000` |
| `4` | Follow on Twitter | `7,500` |

`Status::changeCredits` now receives these values after the corresponding NFC
call. The prior host body had no amount and only recorded a local claimed
state. It also wrote claimed state into `0..3` while the renderer reads the
button-derived indexes `1..4`; that off-by-one is corrected.

The original persistence bytes are still not bound to the recovered
`RecordHandler` object. The local array remains a clearly bounded host-side
representation of that persistent state; the reward values and action order
are native data, not an approximation.

## Blueprint Dialog Behavior

Android cancels/restores a pending blueprint transaction when the dialog
returns `1`, including when funds would otherwise be sufficient. A successful
`0` action is the only route that spends `200 * blueprintAmount`, clears the
pending state, exits sell mode and resets the selected row. The local body now
uses the same three-way outcome instead of treating a cancel as an accepted
purchase state.

## Ship Swap State Machine

The following field/state protocol is present in the Android body:

| ARM field | Recovered member | Role |
| ---: | --- | --- |
| `HangarWindow+0x90` | `shipSwapPending` | current ship-purchase dialog |
| `HangarWindow+0x91` | `swapConfirmFlag` | second confirmation stage |
| `HangarWindow+0x92` | `dlcMenuPending` | DLC diversion branch |
| `HangarWindow+0x11d` | `upgradeMode` | suppresses ordinary trade-in cost/message |
| `Status+0x114` | `field_114` | Android comparison against `3`; semantic name remains open |

The restored flow now includes:

1. GameText `327/330/331` second confirmation, and the GameText `328` guard
   when the station already has the outgoing hull.
2. The native low-credit route, DLC menu diversion, and the difference between
   purchasing a ship without a trade-in and a normal old-price/new-price swap.
3. Construction of a fresh incoming hull, then transfer of cargo, equipment
   and mods from the active ship.
4. Construction of the station-returned hull with mods only. Cargo and mounted
   equipment do not duplicate into the station inventory.
5. The native GameText `303`/ship-name confirmation after an ordinary purchase.

The exact native factory comes through the global ship table before
`Ship::makeShip(-1)`. The existing typed project route uses the selected
ship's equivalent factory method. That is source-backed for its outcome, but
the global table load/lifetime remains a byte-match item.

## Verification

UCRT64 `gof2` links successfully.

Focused ARM verification compiles `201/204` translation units; the three
remaining failures are unrelated `SolarSystem *` type errors.

| Method | Before this pass | After | Target/base instructions after |
| --- | ---: | ---: | ---: |
| `HangarWindow::OnTouchEnd` | 6.0% | 6.4% | 2154 / 1776 |
| `HangarWindow::OnTouchBegin` | 27.2% | 27.2% | 448 / 390 |
| `HangarWindow::OnTouchMove` | 78.1% | 78.1% | 141 / 138 |
| `HangarWindow::update` | 48.3% | 48.3% | 175 / 173 |

`OnTouchEnd` is not linked- or raw-byte-exact. Its body is still missing the
native shared ChoiceWindow tail, exact String temporary/destructor placement,
global ship-table load lifetime and final ARM register allocation. The larger
base span is evidence that recovered behavior is now represented; it is not a
claim of byte equivalence.

## Evidence

- Android 2.0.16 `gof2_libgof2hdaa_full_ida.c`,
  `_ZN12HangarWindow10OnTouchEndEii` at `0x14c740`;
- original `libgof2hdaa.so` `.rodata` at `0x202844` for the exact social
  reward table;
- `HangarWindow::render` at Android address range around `0x14b000` for the
  visible social-offer indexes and claim-byte routing;
- `HANGAR_ARM32_LAYOUT_TOUCH_MOVE_2026-08-24.md` for the validated class map.
