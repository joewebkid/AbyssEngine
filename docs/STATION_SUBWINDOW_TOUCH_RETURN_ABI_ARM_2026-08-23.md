# Station Subwindow Touch-Return ABI ARM Pass

Date: 2026-08-23

## Scope

This pass removes the local `void`/shim ambiguity on the two small station
subwindow paths that feed `ModStation::OnTouchEnd`:

| Method | Android address | Recovered return type |
| --- | ---: | --- |
| `StatusWindow::OnTouchEnd(int, int)` | `0x15a1c4` | `int` |
| `SpaceLounge::OnTouchEnd(int, int)` | `0x171a48` | `int` |
| `ModStation::OnTouchEnd(int, int, void *)` | `0x0d7600` | `void` owner/router |

The Android body decides whether `ModStation` should close only from a child
nonzero return. Treating the child functions as `void` hid that protocol
behind `_ote` wrappers and made the two coordinate arguments harder to audit.

## Confirmed Return Contracts

| Child | Returns `1` | Returns `0` |
| --- | --- | --- |
| `StatusWindow` | `Layout::OnTouchEnd(x, y)` closes the top-level Status window. | Tab selection, scrolling, help and every other touch. |
| `SpaceLounge` | `Layout::OnTouchEnd(x, y)` closes the top-level lounge, only when neither the list nor an agent/detail mode owns the close. | Choice/chat, StarMap, close of the list, close of agent/detail mode, list input, intro/agent selection, scrolling and help. |

`ModStation` now directly invokes `StatusWindow::OnTouchEnd(x, y)` and
`SpaceLounge::OnTouchEnd(x, y)`, clearing the already confirmed Android owner
bytes `+0x68` and `+0x65` only when those calls return nonzero.

## Deliberately Excluded

`HangarWindow::OnTouchEnd` also returns `int` in the Android binary, but it is
not the same small contract. Its large body includes purchase, blueprint,
dialogue and `readyToClose()` paths. This pass does not collapse those paths
to a `Layout` result or change the local Hangar signature without a dedicated
body audit.

## Verification Boundary

The UCRT64 `gof2` static library builds successfully after this change.

Focused ARM validation compiles `201/204` translation units; the three known
failures are unrelated `SolarSystem *` type errors. The checkpoint is:

| Method | Match | Target/base instructions |
| --- | ---: | ---: |
| `SpaceLounge::OnTouchEnd` | 6.0% | 2713 / 333 |
| `StatusWindow::OnTouchEnd` | 19.2% | 306 / 236 |
| `ModStation::OnTouchEnd(int, int, void *)` | 6.4% | 2022 / 906 |

No function is linked- or raw-byte-exact. The source-backed return protocol is
useful independently of those low whole-body scores; the remaining mismatch is
dominated by incomplete child and station bodies, host-side layout differences,
and temporary/control-flow shape.

## Evidence

- Android 2.0.16 `gof2_libgof2hdaa_full_ida.c` bodies at the addresses above;
- the direct `ModStation` Android child-return branches and the owner-byte map
  recorded in `MODSTATION_SUBWINDOW_ROUTING_ARM_2026-08-23.md`;
- Ghidra only as control-flow corroboration where it agrees with the IDA
  return paths.
