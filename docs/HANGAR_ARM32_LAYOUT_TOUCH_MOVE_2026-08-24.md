# Hangar ARM32 Layout And Touch-Move Pass

Date: 2026-08-24

## Scope

This pass fixes the `HangarWindow` ARM32 field map before recovering the
source shape of `HangarWindow::OnTouchMove`. It does not invent a new scrolling
model or change a menu policy: the implementation follows the Android HD body
at `0x14be98`.

| Method | Android address | Recovered signature |
| --- | ---: | --- |
| `HangarWindow::OnTouchMove` | `0x14be98` | `int (HangarWindow *, int, int)` |
| `j__ZN12HangarWindow11OnTouchMoveEii` | `0x650dc` | same integer return |

The Android station owner allocates `HangarWindow` with `operator new(0x134)`.
The matching build now locks that size and the known member offsets with
`GOF2_MATCH` `static_assert`s. These asserts describe the 32-bit target only;
the ordinary host UCRT64 build intentionally has a different pointer size.

## Confirmed ARM32 Map

The constructor, `initialize`, `OnTouchMove`, and `update` agree on these
input-related fields:

| Offset | Typed member | Evidence |
| ---: | --- | --- |
| `0x058` | `viewMode` | detail-mode branch in `OnTouchMove` |
| `0x068` | `selectedItem` | cleared when dragging begins |
| `0x06c/0x070` | `holdTime` / `repeatTimer` | reset by drag, consumed by `update` |
| `0x0b4/0x0b8` | `scrollOffset` / `lastTouchY` | scroll recurrence |
| `0x0c0/0x0c4` | `scrollDelta` / `damping` | paired 64-bit write during drag |
| `0x0c8` | `velocity` | inertia recurrence in `update` |
| `0x0cc` | `touchStartY` | six-pixel drag threshold |
| `0x0d0` | `dragging` | drag state in `update` |
| `0x0d2` | `sellConfirmPending` | reset after drag transition |
| `0x0d4/0x0d8` | `currentContentHeight` / `visibleHeight` | scroll clamp in `update` |
| `0x0f8/0x0fc` | `autoEquipPending` / `autoEquipIndex` | existing touch-begin routing |
| `0x100` | `rowLayoutMetrics` | copied Android Layout metrics |
| `0x11c..0x130` | remaining list/platform flags | constructor and render consumers |

Three four-byte words between `viewMode` and `selectedItem`, and small padding
regions, are retained under offset names. Their semantic consumers are not yet
established, so they are not given speculative labels.

## Recovered Touch Flow

1. Forward every move to `Layout`.
2. When a dialog is active, route paid offers `12..16` and `17`, or free offers
   `18..22`, then route the `ChoiceWindow`.
3. When a detail card is active, route directly to `ListItemWindow`.
4. In normal list mode and within the Android content Y bounds, update the
   paired `scrollDelta`/`damping` state, scroll offset and last Y coordinate.
5. Once neither amount button is held and the movement reaches six pixels,
   clear repeat state, forward to every action button, leave sell mode, clear
   pending sale/selected-row state, and reset both amount buttons.
6. Forward tab-button movement only on the ordinary list route, then return
   the Android-confirmed integer zero.

No nullable fallback or host-specific branch was introduced in this method.
The source expects the initialized button, tab and dialog arrays to be valid.

## Verification

UCRT64 `gof2` links successfully.

Focused ARM validation compiles `201/204` translation units; the same three
unrelated `SolarSystem *` type errors remain outside this package.

| Method | Before | After | Target/base instructions after |
| --- | ---: | ---: | ---: |
| `HangarWindow::OnTouchMove` | 11.6% | 78.1% | 141 / 138 |
| `HangarWindow::OnTouchBegin` | 27.2% | 27.2% | 448 / 390 |
| `HangarWindow::update` | 48.3% | 48.3% | 175 / 173 |
| `HangarWindow::OnTouchEnd` | 6.0% | 6.0% | 2154 / 1487 |
| `HangarWindow` constructor | 6.7% | 6.7% | 41 / 48 |

`OnTouchMove` is neither linked- nor raw-byte-exact. Its remaining difference
is small in instruction count, but final register allocation, loop lowering
and local lifetime still require a dedicated pass. The constructor and large
touch-end body remain separate packages.

## Evidence

- Android 2.0.16 `gof2_libgof2hdaa_full_ida.c`, allocation sites around
  `0x0d7614`/`0x0d7a9c`, constructor `0x147d20`, `update` `0x148a7e`, and
  `OnTouchMove` `0x14be98`;
- `HANGAR_LAYOUT_BUTTON_SLOTS_2026-08-16.md` for the 24 action-slot map and
  copied Layout metrics;
- `HANGAR_TOUCH_BEGIN_SOURCE_SHAPE_ARM_2026-08-24.md` for the adjacent
  input-owner and amount-button flow.
