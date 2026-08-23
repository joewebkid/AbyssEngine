# ModStation Subwindow Routing ARM Pass

Date: 2026-08-23

## Scope

This pass resolves the Android `ModStation` byte routing shared by station
input, update, and rendering. Earlier local code mixed the source bytes among
several host-side `FlagWord` names, so different methods could send the same
active window to different owners.

The Android ARMv7 IDA bodies are the ground truth:

| Method | Android address |
| --- | ---: |
| `ModStation::OnTouchBegin` | `0x0d72ac` |
| `ModStation::OnTouchMove` | `0x0d7480` |
| `ModStation::OnTouchEnd` | `0x0d7600` |
| `ModStation::OnUpdate` | `0x0d9c78` |
| `ModStation::OnRender2D` | `0x0db810` |
| `ModStation::OnRender3D` | `0x0dbf28` |

## Recovered Android State Bytes

| Android byte | Local view | Owner |
| --- | --- | --- |
| `+0x60` | `m_nStarMapWindowOpen.bytes[0]` | Radio/cutscene flow |
| `+0x61` | `m_nStarMapWindowOpen.bytes[1]` | Station main controls |
| `+0x62` | `m_nStarMapWindowOpen.bytes[2]` | DLC `MenuTouchWindow` |
| `+0x63` | `m_nStarMapWindowOpen.bytes[3]` | Primary `ChoiceWindow` |
| `+0x64` | `subWindowFlags.bytes[0]` | `MissionsWindow` |
| `+0x65` | `subWindowFlags.bytes[1]` | `SpaceLounge` |
| `+0x66` | `subWindowFlags.bytes[2]` | `HangarWindow` |
| `+0x67` | `subWindowFlags.bytes[3]` | `StarMap` |
| `+0x68` | `modalFlags.bytes[0]` | `StatusWindow` |
| `+0x69` | `modalFlags.bytes[1]` | `DialogueWindow` |
| `+0x6a` | `modalFlags.bytes[2]` | Medal `ChoiceWindow` |

`+0x6b` is preserved as a distinct station state; this pass does not assign it
to a UI window.

## Source-Backed Repairs

- Made `OnUpdate` dispatch in Android priority order: radio, dialogue,
  StarMap, hangar, missions, Space Lounge, then status. The normal Choice and
  DLC menu are updated after that state dispatch, as in the binary.
- Unified primary Choice and DLC input/draw/update routing on Android
  `+0x63` and `+0x62`, respectively. They no longer borrow unrelated
  `screenFlags` or medal-dialogue bytes.
- Fixed `OnTouchEnd` so Space Lounge checks `+0x65` and missions checks
  `+0x64`; each now clears its own confirmed byte only after a nonzero child
  return.
- Made the station main-control gate use only `+0x61`. The prior whole-word
  test could interpret an open Choice or DLC screen as the radio/cutscene
  path.
- Restored the native `(x, y)` pair for `DialogueWindow::OnTouchEnd` and the
  direct two-coordinate calls for `ChoiceWindow` and `StarMap`.
- Restored the `DialogueWindow::update(dt)` argument instead of a local zero.
- Recorded the recovered state ownership in `ModStation.h` to stop future
  passes from reintroducing the same cross-routing.

## Verification Boundary

This is a source-backed behavior and ownership repair, not a byte-match
claim. The matching ARM build compiles `201/204` translation units; the three
known failures are unrelated `SolarSystem *` type errors. Focused comparison
produced this checkpoint:

| Method | Match | Target/base instructions |
| --- | ---: | ---: |
| `OnTouchMove` | 1.3% | 141 / 10 |
| `OnUpdate` | 5.6% | 1226 / 623 |
| `OnTouchEnd` | 6.4% | 2022 / 906 |
| `OnRender2D` | 8.3% | 683 / 68 |
| `OnTouchBegin` | 14.1% | 174 / 25 |
| `OnRender3D` | 82.1% | 70 / 64 |

None is linked- or raw-byte-exact. `OnTouchEnd` remains a large
mixed-recovery body with untyped wrapper calls, incomplete temporary lifetime
work, and a non-native host layout. These scores are checkpoints, not proof of
exactness.

## Evidence

- Android 2.0.16 `gof2_libgof2hdaa_full_ida.c` method bodies and direct byte
  reads/writes listed above;
- Android `ModStation` destructor and station-button paths, which confirm the
  child pointers at `+0x74..+0x84` and `MissionsWindow` at `+0x80`;
- Ghidra is retained only as control-flow corroboration where its inferred
  field names agree with the direct IDA byte accesses.
