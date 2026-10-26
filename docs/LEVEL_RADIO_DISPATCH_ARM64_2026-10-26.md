# Level radio message handoff (2026-10-26)

The Android 2.0.16 ARM `Level::createRadioMessage` at `0xc566c` first
checks the `PlayerEgo` pointer and its `Radio*` at `+0x18`. On the
aborted branch it clears the queue and calls `Radio::setMessages` with
null; on the normal branch it passes the newly built message array.
The original target `Radio::setMessages` is at `0x15501c`. It stores
the array and assigns each message's `Radio` back-reference.

Recovered C++ instead tested `PlayerEgo+0x1c`, which is an engine-sound
resource field, and called an undefined `Level::crm_dispatch` with that
integer. The code now tests `radioRef`, calls its typed `setMessages`
method on both branches, and asserts that `radioRef` is at ARMv7 offset
`0x18`. This fixes a real routing error: a valid radio can have a zero
engine-sound field.

Native and Android AArch64 static archives build. The NDK r18b verifier
compiled **209/209** ARMv7 translation units. The focused Level unit
compares **130** functions and retains all **73 linked-exact** and
**34 byte-exact** symbols. `createRadioMessage` remains nonexact:
source-shape **67.1% to 67.0%**, strict fuzzy **21.3% to 21.5%**.
Its remaining long switch and message construction paths need separate
review. Focused report: `_work/level_radio_dispatch_unit_20261026.json`.

The full ARM corpus now compares **4,323/4,524** original functions
over **4,745** pairs, with zero skips. Mean source-shape/strict fuzzy
is **94.75%/84.26%**; exact counts are **2,606 linked-exact** and
**1,083 byte-exact**. Against the preceding full run, no exact symbol
is lost. The two additional linked-exact symbols come from the prior
`StarSystem` target-array recovery. Report:
`_work/level_radio_dispatch_full_20261026.json`.

Strict AArch64 link diagnostics with existing libzip and crypto
archives reduce unresolved names **455 to 454** whole-archive and
**215 to 214** JNI-reachable. The sole removed name is
`Level::crm_dispatch(int, void*)`; no name was added. Logs:
`D:/gof2-arm64-crypto/link-after-radio-final.log` and
`D:/gof2-arm64-crypto/link-jni-gc-after-radio-final.log`.

No live radio playback or message presentation has been exercised on
ARM64. The Android `.so` still fails strict linking, and the sound
backend remains missing. Next priority: source-backed JNI-reachable
bridge/engine calls and a runnable Android host, followed by live
radio-message and audio checks.
