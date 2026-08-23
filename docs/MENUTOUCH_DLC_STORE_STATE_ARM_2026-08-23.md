# MenuTouchWindow DLC Store State ARM Pass

Date: 2026-08-23

## Scope

This pass restores Android `MenuTouchWindow::OnTouchEnd(int, int, void *)`
state `15` (`0x0f`): DLC store launch/acknowledgement, purchase and restore
handoff, result/error dismissal, selector updates, and its application-data
protocol. It also separates state `16` (`0x10`) from that body.

Primary evidence:

- Android 2.0.16 IDA body at `0x12aabc`;
- matching Ghidra body at `0x0013aabc`;
- `gof2_recovered_sdk_first_pass/extracted/ui_input_hud/MenuTouchWindow.c`;
- Android `MenuTouchWindow::update` at `0x129598`;
- exported symbols for `Globals::iap_hack_dlc1Bought` through `dlc3Bought`.

## State 15 Body

The DLC launch dialog is distinct from an ordinary message. When
`dlcMessageShowing` (`+0x17a`) receives ChoiceWindow result `0`, native code
sets application data `+0x3c` to `1`, then clears both `dlcMessageShowing` and
`messageShowing`. This is now represented as `dlcMenuAcceptedFlag`.

Result/error dialogs use `+0x17c/+0x17d`. Their dismissal clears both bytes and
`messageShowing` without re-running ChoiceWindow. An ordinary message in this
state calls ChoiceWindow; a result `0` additionally clears application-data
`+0x40` (`dlcStoreReadyFlag`).

The regular store list routes as follows:

| Button pair `(id, secondary)` | Native action |
| --- | --- |
| `(60, 0)` | Writes purchase code `-1` at app-data `+0x48`, sets `+0x3f`, clears the message, and sets `purchaseRestorePending` (`MenuTouchWindow+0x190`). It does **not** directly call `NFC::iap_restore_purchases` in this body. |
| `(52, 0)` | Requests the selected DLC only when its ownership bit is clear, writes selected code `0..4` at `+0x48`, and clears the message. |
| selected selector slot | Requests that DLC immediately, even when already selected; a newly selected slot changes the scroll text to GameText `87 + selection`. |

The Android ownership block contains five adjacent flags. The first three have
exported names and are now used directly:
`Globals::iap_hack_dlc1Bought`, `dlc2Bought`, `dlc3Bought`. The VIP and full
package byte names are not exported, so the C++ source uses two explicitly
local descriptive names. Their *behavior* is source-backed; their original
symbol names remain unknown.

`scrollbarHit` (`+0x1d9`) has a recovered native Image2D width/height probe
against the selected image table at `+0x138`. The recovered body has no state
write after that probe, so this pass keeps the observed query rather than
inventing a scrollbar action.

## Application-Data and Result Flow

`MtwAppData` now has exact asserted offsets for the state-15 protocol:

| Offset | Name | Confirmed use |
| ---: | --- | --- |
| `0x3c` | `dlcMenuAcceptedFlag` | launch-dialog acknowledgement |
| `0x3d` | `dlcMenuRequestFlag` | request from the social/store route |
| `0x3f` | `dlcRestoreRequestFlag` | restore handoff after button `(60, 0)` |
| `0x40` | `dlcStoreReadyFlag` | platform marks DLC list ready; native `update` consumes and clears it |
| `0x41` | `purchaseResultFlag` | successful purchase/restore result |
| `0x42` | `purchaseErrorFlag` | launch/restore error |
| `0x48` | `purchaseCode` | `0..4` DLC code or `-1` restore |

The corresponding Android `update` body confirms the result mapping:

- code `0` sets `iap_hack_dlc1Bought` and system visibility `25`;
- code `1` sets `Status+0x114 = 3` and `iap_hack_dlc2Bought`;
- code `2` sets `iap_hack_dlc3Bought` and system visibility `25`;
- code `3` sets the VIP ownership byte;
- code `4` sets DLC 1, DLC 3, full-package ownership, and system visibility
  `25`.

This replaces the previous unproven use of `Status+0x35/+0x37` and
`OptionsRecord+0x35..0x39` as the DLC ownership block.

## State 16 Boundary

Android `OnTouchBegin` and `OnTouchMove` have explicit state `16` handling,
but both IDA and Ghidra `OnTouchEnd` switches go from case `0x0f` directly to
case `0x11`. State `16` therefore follows the shared Layout tail here rather
than reusing the state-15 purchase/modal body. Drawing remains shared between
states `15` and `16`.

## Verification

- UCRT64 `gof2` links successfully as `cmake-build-ucrt/libgof2.a`.
- ARM object build remains `201/204`; the three failures are the pre-existing
  unrelated `SolarSystem *` versus integer signatures.
- Focused `MenuTouchWindow::OnTouchEnd`: `8.7%`, target/base `3001/2886`, not
  linked- or byte-exact.

This is source-backed behavior recovery, not an ARM byte-match claim. The
remaining `OnTouchEnd` mismatch is still dominated by switch shape, temporary
String lifetime, and stack/VFP frame layout.

## Remaining Work

1. Locate the platform-side consumer/producer implementation for the DLC
   application-data bytes, especially the unexported VIP/full ownership bytes.
2. Audit `MissionsWindow::OnTouchEnd` return ABI before changing state-9 close
   semantics.
3. Recreate exact Cargo String destruction and remaining full-switch local
   lifetime before stack-frame tuning.
