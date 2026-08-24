# Hangar Touch-End List And Help Source-Shape Pass

Date: 2026-08-24

## Scope

This pass continues the recovery of `HangarWindow::OnTouchEnd` at Android
address `0x14c740`. It restores the normal, non-modal list-action continuation
and the five native help-text branches. It deliberately does not reinterpret
the opaque application-module close flag at the common `ChoiceWindow` tail.

## Confirmed Android Control Flow

After a normal list touch, Android computes/records the selected row and walks
all action buttons. The actions have these source-backed outcomes:

| Action slot | Native result |
| ---: | --- |
| `0` | open `ListItemWindow`, set `viewMode = 1`, play sound `0x61`, return |
| `1..7` | select the current item, then continue the action-button walk |
| `8` | `transaction(false)`, sound `0x64`, then continue |
| `9` | `transaction(true)`, sound `0x65`, set pending auto-equip for type `1`, then continue |
| `10` | open GameText `334` sell-ship dialog, then continue |
| `11` | record credit-offer state, save options, open credits dialog, then continue |

The previous host body returned immediately for slots `1..11`. That preserved
the common gameplay result, but skipped the native continuation and therefore
changed the ARM control-flow graph. The recovered body now continues exactly
where the Android decompilation does. Existing null and range checks stay in
place as host safety guards; a valid rendered action row takes the confirmed
native path.

## Help Text Lifetime

The Android body does not index a local help-ID array. It uses an explicit
switch and creates a separate temporary `AbyssEngine::String` for each call to
`Layout::initHelpWindow`:

| View/tab | GameText ID |
| --- | ---: |
| item detail (`viewMode == 1`) | `643` |
| ship tab (`0`) | `623` |
| shop tab (`1`) | `622` |
| cargo tab (`2`) | `625` |
| equipment tab (`3`) | `624` |
| blueprints tab (`4`) | `626` |

`HangarWindow.cpp` now uses that explicit switch and scoped `String` objects,
so their construction/destruction lifetime is represented in C++ rather than
being hidden behind an inferred lookup table.

## Deliberate Boundary

The common dialog tail references a byte inside application module `5` after a
zero `ChoiceWindow::OnTouchEnd` result. Its raw Android offset and semantic
ownership are not yet reconciled with the typed `ModStation` layout. The
already documented nonzero return contract remains unchanged: this class
returns `1` only from the confirmed `Layout -> readyToClose()` route. No modal
close return is claimed in this pass.

## Verification

UCRT64 `gof2` links successfully.

Focused ARM verification compiles `201/204` translation units. The three
failures are the pre-existing unrelated `SolarSystem *` type errors.

| Method | Before | After | Target/base instructions after |
| --- | ---: | ---: | ---: |
| `HangarWindow::OnTouchEnd` | 6.4% | 11.2% | 2154 / 1871 |
| `HangarWindow::OnTouchBegin` | 27.2% | 27.2% | 448 / 390 |
| `HangarWindow::OnTouchMove` | 78.1% | 78.1% | 141 / 138 |
| `HangarWindow::update` | 48.3% | 48.3% | 175 / 173 |

No method is linked- or raw-byte-exact. Remaining work includes the modal
tail's typed module flag, exact row calculation/register shape, wider dialog
String temporary placement and final ARM register allocation.

## Evidence

- Android 2.0.16 `analysis/gof2_libgof2hdaa_full_ida.c`,
  `_ZN12HangarWindow10OnTouchEndEii` at `0x14c740`;
- `HANGAR_TOUCH_RETURN_ABI_ARM_2026-08-23.md` for the confirmed nonzero return
  ownership;
- `HANGAR_TOUCH_END_ECONOMY_SHIP_SWAP_2026-08-24.md` for the preceding modal
  economy and ship-swap recovery.
