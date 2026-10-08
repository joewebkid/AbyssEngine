# HangarWindow unique ARM source-shape pass — 2026-10-08

Worktree: `C:/Users/Dima/Documents/Galaxy on Fire new version/gof2hd-98-hangar`.
Branch: `codex/hangarwindow-arm-98-20261008`.
Baseline: `473310054460d0eef759db67b1a59064a06f2fb1`.

## Frozen measurement

The unique-body list was frozen before source edits in
`validation/hangar_unique_baseline_2026-10-08.json`: 30 original even ARM
addresses, 10,445 original instructions. C1/C2 and D1/D2 aliases share two
bodies and contribute once each. Weights, representatives and comparison
recipe did not change. All 32 exported family symbols remain compared.

| Measurement | Baseline | Final |
|---|---:|---:|
| Unique-body unweighted source mean | 98.933333333333% | 99.340000000000% |
| Mean weighted by original instruction count | 94.584729535664% | 98.007046433700% |
| render source / strict | 90.0 / 9.1% | 97.1 / 35.8% |
| OnTouchEnd source / strict | 93.4 / 20.2% | 98.0 / 70.3% |
| initialize source / strict | 97.2 / 63.2% | 97.7 / 60.2% |

Both family thresholds pass without rounding the family means. The normal
verifier displays each function's source score to one decimal; that existing
behavior is unchanged. A separate observation of the same arithmetic before
function-score display rounding gives **99.349068318487% unweighted and
98.028317419912% weighted**, also above 98.0%. These are source-shape
measurements, not exact recovery or runtime proof. Strict scores remain separate.

## Evidence and accepted forms

Primary source: Android 2.0.16 `libgof2hdaa.so`, HangarWindow initialize
`0x147e24`, render `0x1490d8`, OnTouchEnd `0x14c740`; readable decompilation:
`A:/Galaxy on Fire new version/gof2_recovered_sdk_first_pass/extracted/ui_input_hud/HangarWindow.c`.
Independent cross-check: `analysis/gof2_ios_ida_full_dump.c`, identified renderer
`sub_EC73C` and input router `sub_F0CE4`. Android governs platform-specific
IDs/costs; the iOS blueprint multiplier differs and was not imported.

- Restore native String construction and cleanup boundaries: plain text uses
  borrowed GameText strings, drawBox/help use direct by-value temporaries,
  credit descriptions/prices end their lifetimes separately, and replacement
  messages are destroyed on the native side of ChoiceWindow calls. Pending
  product and amount suffixes use the confirmed concatenation chains.
- Re-read global owners, layout fields and button slots at native call sites;
  preserve the original Canvas/font/coordinate snapshots across text/price
  queries. Remove unused pointer reads. Move/sell/header rows bypass the
  shared label/actions tail; installed/pending markers bypass the amount suffix.
  Blueprint-progress label Y includes Layout+0x260, shown in both native sources.
- Restore modal convergence, per-case action-row updates and the DLC-offer
  stage. Back from blueprint detail refreshes ingredient availability before
  resetting the selected row. Keep confirmed payload checks and optional module
  checks; initialized arrays/payloads follow the native ownership contract.
- Insufficient-credit messages use **cost minus credits**, proven by Android
  subtraction sites `0x14c804`, `0x14d4d2`, `0x14dc26` and corroborated by iOS.
- Ship transfer uses Status+0x14c (`voidStation`, generated station) for storage
  and Status::getStation (current station, +0x19c) for shop inventory. Restore
  the no-trade removal, slot replacement, old-mod transfer, refresh/init order,
  and item-list release. The current station and generated station are distinct.
- Nine receiver-free NFC JNI wrappers are declared static. The Android callers
  pass arbitrary product/offer indices in r0, and those callees overwrite r0
  from the GOT before using it. This removes invented NFC constructor calls;
  exported mangled names and wrapper bodies remain unchanged. The unrelated
  rateGame/DLC methods were not changed. Callee evidence is in `_work/nfc-abi.asm`.
- initialize now uses the four original String("", false) constructions, ends
  paid-button label lifetimes before setVisible, reads the native owners in order,
  reloads icon storage and seeds **both** social image handles to 0xffffffff.
  The original initialized handles avoid an uninitialized local read on load failure.

Changes are limited to `src/game/ui/HangarWindow.cpp` (render, active OnTouchEnd,
initialize) and the nine NFC declarations in `src/engine/core/NFC.h`.
MGame, Radar, the shared verifier and PROJECT_STATUS are untouched.

## Rejected or superseded trials

- Generic credit-button visibility helpers and cached coordinates/payload tables
  lose native per-slot reads and branch-local lifetimes. Their shorter call/branch
  inventories are not equivalent evidence of the recovered source shape.
- The inlined paid-row lambda with ternary product strings allowed Oz to merge
  the five native rows (render 95.9%). Explicit native rows and live per-row
  spacing were retained instead. A late first-row Y arithmetic snapshot trial
  fell to 96.8%; its sum has the same captured inputs as the retained form and
  introduced no required semantic repair, so only that trial was reverted.
- String/call repairs were retained even when an intermediate source score
  declined: direct by-value lifetimes removed extra copy calls, and static NFC
  calls removed nine fictitious constructions. Ship-transfer recovery subsequently
  restored the genuinely missing calls/branches.
- The native social switch's case 5 is unreachable under its 0..4 loop bound.
  It was not added to inflate instruction/call counts. No asm barriers, padding,
  artificial branches, UB tricks, compiler-flag or verifier changes were introduced.

## Final validation

- Native Windows/MSYS2 CMake `gof2` static-library build passes; all 206 ARM TUs
  pass the unchanged canonical NDK r18b/Oz build (zero failures).
- Full `verify.py --build-dir _work/verify --report _work/final-full.json
  --fail-on-wrong-type` passes. Original name set is unchanged: 4,323/4,524
  originals covered, 201 missing, zero wrong-type and zero skipped units.
- All **2,301 linked-exact and 997 byte-exact (unit, symbol) pairs** survive;
  exact set differences are empty, not merely equal totals. Set hashes are in
  the final ledger. Family linked/raw counts remain 13/6.
- Total comparisons are 4,688 -> 4,689: the restored native ArrayRemove<Ship*>
  call emits another instance of an existing original symbol (100.0% source,
  96.6% strict). All previous comparison pairs remain; original name coverage
  is identical. This helper contributes nothing to the frozen family mean.
  Non-original extra symbols fall from 121 to 116 as invented helpers disappear.
- General verifier tests: 36 run, zero failures, one real-object fixture skipped.
  The real ArrayInstantiations.o alias tests were then run explicitly: two pass,
  zero skipped. No verifier tests or implementation were edited.
- Scoped and full HangarWindow metrics agree exactly. Only render, active
  OnTouchEnd and initialize change in the original full-corpus results.

Focused reports: `_work/baseline-hangar.json`, `_work/final-hangar.json`.
Full reports: `_work/baseline-full.json`, `_work/final-full.json`.
Reproduction uses `GOF2_VERIFY_LOCAL_NDK=1`,
`GOF2_VERIFY_WORK=A:/Galaxy on Fire new version/gof2hd-decomp-master/_work`,
`GOF2_NDK_ROOT=A:/Galaxy on Fire new version/gof2hd-decomp-master/.cache/ndk/android-ndk-r18b`.
All new builds/reports stay on C: inside this worktree; A: supplies read-only
binary/toolchain evidence. The tracked final measurement ledger is
`validation/hangar_unique_final_2026-10-08.json`; it stores report hashes,
all per-body weights/scores and the exact set regression gates.

## Remaining limits

Native `gof2` is a static library; a successful build does not demonstrate an
executable game, JNI purchase flow, device input or runtime ABI safety. Those
runtime paths were not exercised here. Existing options-record bindings and
partially typed ModStation integration remain outside this package. The exact
full-corpus gates below test loss of existing evidence, not runtime behavior.
