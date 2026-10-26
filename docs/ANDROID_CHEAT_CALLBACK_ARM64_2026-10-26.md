# Android startup cheat callback (2026-10-26)

The Android 2.0.16 `OnCreateApplication` function at ARM address
`0xd0738` registers the no-op `nullsub_3` at `0xd0d14` as the cheat
callback, passing the `MGame` object it just registered as context.
The recovered callback was declared but undefined, and the registration
passed `nullptr` instead of that game object. `OnCheatActivated` now has
the original no-op behavior, and the registration passes its `MGame`
pointer. This keeps the callback non-null while preserving its lack of
effects for the three registered cheat codes.

Native and Android AArch64 static builds pass. The NDK r18b verifier
compiled **209/209** ARMv7 translation units. The focused
`game/core/CheatHandler` unit compares **12** functions and retains
**8 linked-exact / 1 byte-exact**; `platform/android/JniBridge` compares
**69** functions and retains **46 linked-exact / 4 byte-exact**. Neither
unit loses an exact symbol against the last full ARM report.
`OnCreateApplication` remains nonexact: source-shape changes
**96.3% to 96.2%** and strict fuzzy **69.3% to 63.1%**. The lower fuzzy
score is accepted because the registered context and executable callback
now match the ARM behavior. Focused reports are
`_work/cheat_callback_core_unit_20261026.json` and
`_work/cheat_callback_jni_unit_20261026.json`.

Strict AArch64 link probes with the existing libzip and crypto archives
reduce unresolved names **458 to 457** for the whole archive and
**218 to 217** for JNI-reachable sections. In both probes, the sole
removed name is `OnCheatActivated(int, void*)`; no name was added.
Logs are `D:/gof2-arm64-crypto/link-after-cheat.log` and
`D:/gof2-arm64-crypto/link-jni-gc-after-cheat.log`.

The preceding full ARM corpus remains the latest whole-project result:
**4,323/4,524** original functions over **4,743** pairs,
**94.72% source-shape / 84.23% strict fuzzy**, **2,604 linked-exact**,
**1,083 byte-exact**, zero skips. It was not rerun for this focused edit.
The callback has not been exercised inside a running game. Next
priority: resolve source-backed JNI-reachable engine and game helper
symbols; the Android host, audio backend, and live startup gate remain
separate blockers for a game APK.
