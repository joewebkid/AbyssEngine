# MGame camera call recovery (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original ARM `MGame::switchCamera` at `0x17c4e8`,
`freeCamTouchBegin` at `0x178a28`, and `setCinematicMode` at
`0x17c454` call the corresponding `TargetFollowCamera` methods
directly. In `setCinematicMode`, the look-at state is stored in a byte
at `MGame+356` and passed back to `setLookAtCam`.

Five undefined `TFC_*` call targets in `MGame` are replaced with the
existing `TargetFollowCamera` methods: `setRotationAroundTarget`,
`setLookAtCam`, `enableFirstPersonCam`,
`hideShipForFirstPersonCam`, and `getCamOffset`. The saved look-at flag
and the camera's own look-at field/getter are now `bool`. Both remain
one-byte fields with the original offsets. This preserves the direct
byte argument at the cinematic restore call. A trial that changed the
call without recovering the boolean field types lost the linked-exact
match for `setCinematicMode`; the final typed version retains it.

Native and AArch64 static archives build. Focused ARMv7 verification
compares **48** `MGame` and **42** `TargetFollowCamera` symbol/unit
pairs. `MGame::switchCamera` and `setCinematicMode` remain
**100% source-shape, 100% strict fuzzy, and linked-exact**.
`TargetFollowCamera::isInLookAtMode` and `setLookAtCam` remain both
linked-exact and byte-exact. The focused units lose no exact symbols.

Strict JNI-initialize AArch64 link diagnostics fall **120 → 115**
unresolved names; the broader `JNI_OnLoad` probe falls **338 → 333**.
Both remove the same five `TFC_*` names with no new undefined names.
These probes do not produce a game `.so`; camera switching on a device
remains unverified.

The full ARMv7 corpus compares **4,323/4,524** original functions over
**4,745** pairs with zero skips. Mean source-shape and strict fuzzy
remain **94.79% / 84.29%** after rounding. The complete
**2,607 linked-exact** and **1,083 byte-exact** symbol sets have zero
losses and zero gains against the preceding full report.
