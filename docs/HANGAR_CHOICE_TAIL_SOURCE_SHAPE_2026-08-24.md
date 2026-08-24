# Hangar ChoiceWindow Tail Source-Shape Pass

Date: 2026-08-24

## Scope

This pass restores the shared Android `LABEL_117` control-flow tail inside
`HangarWindow::OnTouchEnd` (`0x14c740`). It covers the converging
replace-equipment, insufficient-credit, paid-credit, free-credit and sell-ship
branches. Blueprint, auto-completion and the large ship-swap/DLC state machine
remain independently routed because the Android body does not send all of
them through this tail.

## Confirmed Android Shape

Each of the following branches first evaluates `ChoiceWindow::OnTouchEnd`,
performs its local state work, then reaches `LABEL_117` instead of returning
immediately:

| Branch | Local work before the shared tail |
| --- | --- |
| replace equipment | restore scroll on cancel or demount/mount the chosen items |
| insufficient credits | clear the notice or reopen the paid-credit window |
| paid credits | hide paid controls on close, dispatch one IAP button, and handle the More button |
| free credits | switch back to paid credits when closed; dispatch the five visible social/store actions |
| sell ship | clear the pending state or credit/remove the sold station ship |

At `LABEL_117`, Android reads `buyMode`, calls `ChoiceWindow::OnTouchEnd` a
second time, then processes the shared buy confirmation and the
`ModStation+0x18` owner-close route. The source-backed C++ body now uses a
direct `common_choice_tail` label and `goto` convergence, rather than a lambda
or independent early returns. This preserves the native control-flow shape and
the repeated dialog call.

The Android free-credit decompilation shows an unreachable-looking `case 5`
after a loop bounded to five buttons. This pass retains the confirmed visible
slots `0..4`; it does not invent a sixth action.

## Verification

UCRT64 `gof2` links successfully.

Focused ARM validation compiles `201/204` translation units; the three known
failures are unrelated `SolarSystem *` type errors.

| Method | Before | After | Target/base instructions after |
| --- | ---: | ---: | ---: |
| `HangarWindow::OnTouchEnd` | 11.2% | 11.8% | 2154 / 1968 |

The function is not linked- or raw-byte-exact. Remaining byte-match work is
the exact String/stack allocation around the modal branches, row/register
allocation, and the separate blueprint/ship-swap/DLC branches.

## Evidence

- Android 2.0.16 `analysis/gof2_libgof2hdaa_full_ida.c`,
  `_ZN12HangarWindow10OnTouchEndEii` at `0x14c740`, `LABEL_117` through
  `LABEL_124`;
- `HANGAR_MODAL_CLOSE_CREDITS_ARM_2026-08-24.md` for the typed
  `ModStation+0x18` owner-close byte;
- original ARM verification report
  `_work/hangar_choice_tail_after_20260824.json` (local, intentionally not
  committed).
