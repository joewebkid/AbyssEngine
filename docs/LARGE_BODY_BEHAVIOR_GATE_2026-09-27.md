# Large Gameplay Bodies: Behavior And ARM Gate

Date: 2026-09-27. Scope: Android HD ARM32 decompilation, not the Godot port.

Generate the six-function table from a **full**, unfiltered verifier
report with `python tools/verify/large_body_gate.py _work/full_arm_hangar_99_final_20260927.json`.
The focused family averages are not completion metrics for these bodies.

The last completed full baseline is `_work/full_arm_hangar_99_final_20260927.json`
(4,626 comparisons, zero wrong-type symbols, 2,072 linked-exact and 942
byte-exact overall). The `HangarWindow::OnTouchEnd` row below comes from the
subsequent focused ARM rebuild, after the row-picker fix. No other C++ body
changed in this pass. A later full `--no-build` sweep was stopped after several
minutes without a report; it is **not** counted as validation.

| Body | Source-shape | Strict | Linked-exact | Byte-exact | ARM instructions (original/local) |
| --- | ---: | ---: | :---: | :---: | ---: |
| `HangarWindow::render` | 90.0% | 9.1% | no | no | 3616/3303 |
| `HangarWindow::OnTouchEnd` | 93.4% | 20.2% | no | no | 2070/2056 |
| `Hud::draw` | 96.9% | 59.8% | no | no | 3142/3056 |
| `Radar::draw` | 96.8% | 47.9% | no | no | 3486/3546 |
| `MGame::OnTouchEnd` | 94.9% | 38.3% | no | no | 2769/2658 |
| `MGame::OnUpdate` | 96.7% | 58.3% | no | no | 3856/3770 |

Source-shape is a structural heuristic; strict is instruction-level fuzzy
similarity. Neither demonstrates behavioral equivalence. Linked-exact and
byte-exact are separate verifier results, and runtime ABI safety needs
executable tests.

## Unconfirmed Branches And Priority

### Hangar Branch Audit

The following is a branch-level map, not a declaration that the complete
functions are recovered. `Android-mapped` means the visible side effect and
condition match the recovered Android body; it does not establish iOS
equivalence, local lifetime, or ARM byte matching.

| Android branch | Local entry | State | Checked behavior / open question |
| --- | --- | --- | --- |
| `render` item/ship rows after `LABEL_46` | `HangarWindow.cpp:380` | Android-mapped | Names, prices, icons and blueprint progress; string cleanup differs. |
| `render` item markers after `LABEL_84` | `HangarWindow.cpp:620` | Android-mapped | Availability, mounted marker, quantity suffix. |
| `render` selected actions `LABEL_90..168` | `HangarWindow.cpp:653` | Android-mapped | Ship, blueprint, mount/unmount and inspect buttons; exact draw order and row-scale screenshots pending. |
| `render` credit-grid tail | `HangarWindow.cpp:750` | Needs confirmation | Product labels/images and social-offer visibility are present; native String return slots and iPad placement need independent check. |
| `OnTouchEnd` auto-complete prompt | `HangarWindow.cpp:1451` | Android-mapped | Price, credits, completion and dialog route; local null guard differs from native valid-state assumption. |
| `OnTouchEnd` paid/free credits | `HangarWindow.cpp:1522` | Android-mapped | NFC/social route and award amounts; provider fulfillment is outside this function. |
| `OnTouchEnd` blueprint purchase | `HangarWindow.cpp:1636` | Needs confirmation | Cost and rollback are present; verify all modal return codes and restored fields. |
| `OnTouchEnd` ship swap `LABEL_185..254` | `HangarWindow.cpp:1696` | Needs confirmation | Cargo/equipment transfer and station mutation are present; object ownership and both no-trade-in branches need runtime tests. |
| `OnTouchEnd` row picker and action switch below `LABEL_127` | `HangarWindow.cpp:1985` | Android-mapped | A negative computed row can clear selection. Only action IDs 0..9 reapply their row index; sell-ship/credits IDs 10..11 do not. |
| `OnTouchEnd` shared dialog tail `LABEL_117` | `HangarWindow.cpp:1901` | Needs confirmation | The second `ChoiceWindow::OnTouchEnd` call is preserved; verify side effects for repeated touch resolution. |

The local `render` has defensive `nullptr` and zero-height guards absent in
the Android pseudocode. Keep them identified as local safety behavior, not
evidence of exact native control flow. Similarly, the ship-swap path checks
template bounds after cargo extraction and credit mutation; its successful
state is source-backed, but malformed data could leave partial changes.

The row-picker fix follows Android `OnTouchEnd` below `LABEL_127`: the native
test is `row < getCurrentLength()`, without a `row >= 0` requirement.
`HangarList::getCurrentItemAt(-1)` returns null, so a negative row clears the
highlight safely. The action switch independently reapplies the row for cases
0..9 only; the local dispatch now follows that scope. The native conditions
are stronger evidence than a fuzzy score. The combined patch passes the
UCRT64 `libgof2.a` link and the focused verifier's 204-unit ARM compile;
`OnTouchEnd` is 93.4% source-shape, 20.2% strict, not linked- or byte-exact.

1. **HangarWindow::render**: align the selected-row actions and dialog tails
   with the Android body at `0x1490d8`, then reconstruct branch-local `String`
   construction and destruction. The current call scan sees 65 original vs
   46 local `String` destructor calls; it is only a diagnostic, not proof of
   specific missing lifetimes. Confirm the phone/iPad row and credit-grid
   branches against screenshots and iOS before claiming pixel equivalence.
2. **HangarWindow::OnTouchEnd**: verify ship-swap ownership and station-list
   mutation, blueprint-purchase rollback, the shared `ChoiceWindow` tail and
   dialog return values against Android `0x14c740`. The source-backed broad
   route is present; exact branch ordering and hidden `String` return slots
   remain open.
3. **Hud::draw**: independently check reticle, dock action, camera-mode,
   auto-turret and menu/overlay joins against Android/iOS. Class-average
   99% source-shape does not close this 3,142-instruction body.
4. **Radar::draw**: reconstruct the enemy-candidate loop, lock-label lifetime
   and ambient/audio join as one control-flow region. The local body is
   longer than the original; synthetic stack locals are not evidence.
5. **MGame::OnTouchEnd**: confirm pause/StarMap branches, HUD-action routing,
   cargo-success transitions and the common return tails. Compare actual
   branch effects, not only the instruction inventory.
6. **MGame::OnUpdate**: confirm jump-drive/radio gating, timer expiry,
   mission/cutscene updates and their one-shot state transitions. The known
   Android byte-field corrections are already in the local body; the large
   remaining CFG and lifetime differences are not resolved by those fixes.

For each body, keep a branch checklist with Android/iOS address, local C++
line, expected side effect, and an explicit confirmed/needs-confirmation
state. Re-run the full report after accepted edits. Do not call a body closed
until the behavior checklist passes and source-shape, strict and exact status
are reported separately.

Evidence: `docs/HANGARWINDOW_ARM_SOURCE_SHAPE_2026-09-25.md`,
`docs/HUD_ARM_SOURCE_SHAPE_99_2026-08-31.md`,
`docs/RADAR_DRAW_SOURCE_SHAPE_2026-09-15.md`, and
`docs/MGAME_INIT_STARMAP_SOURCE_SHAPE_2026-09-21.md`.
