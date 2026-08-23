# MissionsWindow Touch-End Return ABI ARM Pass

Date: 2026-08-23

## Scope

This pass restores the Android return-value chain for mission-screen touch
handling:

```text
WantedWindow::OnTouchEnd -> MissionsWindow::OnTouchEnd
    -> MenuTouchWindow state 9 / ModStation
```

The affected methods are exported as `int` on the Android ARMv7 binary, even
though the Ghidra high-level output labels both as `void`:

| Symbol | Android IDA address | Android IDA return type |
| --- | ---: | --- |
| `_ZN12WantedWindow10OnTouchEndEii` | `0x0e22ac` | `int` |
| `_ZN14MissionsWindow10OnTouchEndEii` | `0x150d74` | `int` |

The IDA bodies and their callers are the ground truth for this pass. Ghidra
remains useful for control-flow corroboration, but its inferred `void` ABI is
not used here.

## Recovered Contract

`1` has one confirmed meaning in this chain: the shared `Layout` handler
consumed the Back/close action and the owning window may close. Every other
normal touch path returns `0`.

| Method | Return `1` | Return `0` |
| --- | --- | --- |
| `WantedWindow::OnTouchEnd` | `Layout::OnTouchEnd(x, y)` succeeds while the wanted board is visible. | Scroll/list input, selecting a bounty, opening/using its StarMap, help, and all non-closing actions. |
| `MissionsWindow::OnTouchEnd` | Its own `Layout::OnTouchEnd(x, y)` succeeds, or it forwards the nonzero result from the wanted-board mode. | Choice-window results, campaign/freelance map actions, help, StarMap actions, and all non-closing paths. |
| `MenuTouchWindow` state `9` | It writes `menuState = 0` only after the child returns nonzero. | It leaves state `9` active. |

The same nonzero `MissionsWindow` result is checked by the station module
caller, which is why this is an ABI chain rather than a local UI cleanup.

## Source-Backed Repairs

- Changed both public declarations and implementations from `void` to `int`.
- Reinstated the state-9 condition in `MenuTouchWindow::OnTouchEnd`; the
  previous local body closed the mission screen after every touch.
- Reinstated the original `(x, y)` argument pair for
  `MissionsWindow`'s `Layout::OnTouchEnd`. The former `(y, 0)` call was not
  source-backed.
- `WantedWindow` now stays in its StarMap until `StarMap::OnTouchEnd` itself
  returns nonzero, rather than dismissing it on every touch.
- The native positive-index guard around `getWantedAtPosition` is restored.
  It prevents the selected/highlighted sentinel from being treated as a list
  item.
- `ModStation+0x80` is now typed as `MissionsWindow*`, not
  `DialogueWindow*`. Its render, update, draw, touch-begin, touch-move, and
  touch-end consumers call the typed class directly; the prior touch-end shim
  silently discarded the native Y coordinate.
- `ModStation::OnRender3D` now uses the Android
  `PaintCanvas::ClearBuffer(0)` call, instead of passing a truncated Canvas
  pointer as a clear-mask argument.
- Removed three stale `SolarSystem* -> long` casts in the touched
  `WantedWindow` and `ModStation` files; `Status::getSystem()` is already a typed
  `SolarSystem*`.

## Verification

- UCRT64 `gof2` builds successfully as `cmake-build-ucrt/libgof2.a`.
- Focused ARM comparison, matching-toolchain objects:

| Symbol | Similarity | Target/base instructions | Exact |
| --- | ---: | ---: | --- |
| `ModStation::OnTouchEnd` | 6.4% | 2022 / 904 | no |
| `MenuTouchWindow::OnTouchEnd` | 8.7% | 3001 / 2887 | no |
| `MissionsWindow::OnTouchEnd` | 23.6% | 377 / 310 | no |
| `WantedWindow::OnTouchEnd` | 34.9% | 343 / 235 | no |

These are source-shape checkpoints, not byte-match claims. The main missing
work is the ARM block order, temporary/String lifetime, concrete stack frame,
and the wider `ModStation` flag/layout audit. This pass does not claim that
the currently named `subWindowFlags` byte routing is fully recovered.

## Evidence

- Android 2.0.16 IDA bodies at `0x0e22ac` and `0x150d74`;
- Android `MenuTouchWindow::OnTouchEnd` state `9` body at `0x12aabc`;
- Android `ModStation::OnTouchEnd` mission-window caller;
- corresponding Ghidra bodies, used only as corroborating control flow;
- local matching-toolchain ARM validation against the lawfully supplied
  Android binary.
