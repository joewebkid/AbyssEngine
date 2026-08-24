# Hangar Modal Close And Station-Credits Return Pass

Date: 2026-08-24

## Scope

This pass resolves the Android `ModStation+0x18` byte used by the Hangar
paid-credit flow and restores the corresponding nonzero
`HangarWindow::OnTouchEnd` return. It corrects an earlier overly narrow return
claim without treating the rest of the large modal state machine as complete.

## Confirmed Field Ownership

`ModStation` is polymorphic, so its ARM object has the vtable pointer at
offset `0x00`. The Android field read as `ApplicationModule + 24` is therefore
`ModStation+0x18`, now represented as:

```cpp
char closeHangarAfterCredits;
```

The 32-bit match build asserts that exact offset.

Direct Android uses establish its lifetime:

1. `ModStation::OnTouchEnd` sets `+0x18 = 1`, opens the Hangar child
   (`+0x66`) and calls `HangarWindow::showCreditsBuyWindow()` when the station
   credit button is selected.
2. `HangarWindow::render` uses that byte to suppress its normal Hangar body
   while the temporary station-credit route is active.
3. When the paid-credit dialog closes, `HangarWindow::OnTouchEnd` clears
   `+0x18` and returns `1` to `ModStation`.
4. The station owner consumes the nonzero return, clears its Hangar child flag
   and resumes the station flow.

The local name `pendingHangarClose` was functional but hid that source-backed
meaning. It is now `closeHangarAfterCredits` everywhere.

## Return Contract Correction

The previous `HANGAR_TOUCH_RETURN_ABI_ARM_2026-08-23.md` correctly established
the `int` ABI and the normal Layout-close route, but its statement that this
was the only `return 1` route was incomplete. Android has two confirmed
nonzero Hangar exits:

| Route | Required state | Result |
| --- | --- | ---: |
| normal Hangar close | top-level `Layout::OnTouchEnd`, no detail/store/blueprint interception, `readyToClose()` | `1` |
| temporary station-credit close | paid-credit dialog accepted/closed and `ModStation+0x18 != 0` | `1` |

All ordinary Hangar purchases, internal credit screens, detail, blueprint and
other modal routes remain return `0` unless they reach one of these confirmed
owner-close routes.

## Source-Backed Code Changes

- Renamed and documented the typed `ModStation+0x18` byte.
- Preserved the byte's initialization and station owner cleanup paths.
- In the paid-credit completion branch, clear the byte and return `1` when
  this is the station-launched flow; a normal Hangar credit flow still returns
  `0`.
- Restored the same close check at the direct common `ChoiceWindow` tail.

The local pointer-null checks are host safety only. Android dereferences the
station module directly on these active paths.

## Verification

UCRT64 `gof2` links successfully.

Focused ARM validation compiles `201/204` translation units; the three known
failures are unrelated `SolarSystem *` type errors.

| Method | Before | After | Target/base instructions after |
| --- | ---: | ---: | ---: |
| `HangarWindow::OnTouchEnd` | 11.2% | 11.2% | 2154 / 1879 |
| `ModStation::OnTouchEnd(int, int, void *)` | 6.4% | 6.4% | 2022 / 906 |

Neither method is linked- or raw-byte-exact. The no-regression score confirms
that the direct source-shaped close branch did not lose the preceding
list/help recovery. Exact `ChoiceWindow` repeated-call lifetime and the wider
modal state machine remain future work.

## Evidence

- Android 2.0.16 `analysis/gof2_libgof2hdaa_full_ida.c`,
  `HangarWindow::OnTouchEnd` at `0x14c740`, especially `LABEL_117/LABEL_124`;
- Android `HangarWindow::render` around `0x149000`, which tests application
  module `5` at `+24`;
- Android `ModStation::OnTouchEnd` at `0x0d7600`, which sets the same byte
  before opening the Hangar credit route and consumes the child's return.
