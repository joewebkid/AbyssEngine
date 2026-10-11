# Android engine singleton and DLC request handoff (2026-10-11)

Scope: the engine pointer used by the Android JNI bridge, NDK hooks, and
`GetEngine()`. The old source defined `Engine::g_pEngine` as an uninitialized
pointer-to-pointer and dereferenced it during both `ndk23_Init` variants,
rendering, pause/resume, and DLC checks. The Android 2.0.16
`libgof2hdaa.so` instead exports one `Engine *gEngine` object at
`0x00217b24`; the JNI bridge's GOT entry is `0x00210134`. No
`Engine::g_pEngine` counterpart is exported. The source now uses `gEngine`
throughout these direct consumers and removes the unused class member.

ARM anchors:

- `GetEngine()` at `0x00093c88` loads `gEngine`; it is now linked-exact.
- `ndk23_Init` stores the newly constructed engine through the `gEngine`
  entry at `0x00093dd0`; `ndk23_InitWithZip` does the same at `0x00093efa`.
  `ndk23_ndkDone` at `0x00093fec..0x00094013` releases, deletes and clears
  that same global; it is now linked-exact.
- `ndk23_newrender` at `0x0009412c` reloads `gEngine` across callbacks.
  After `OnUpdate`, instructions `0x941d8..0x941f2` read application-data
  byte `+0x3d`, write `1` to `+0x40`, and clear `+0x3d`. The C++ source
  does the same. `MenuTouchWindow::update` later consumes `+0x40` and
  opens store state `0x0f`, providing the native-side handoff.
- `ndk23_sendingPauseSignal` at `0x000943bc` and the DLC ownership check
  `ndk_getDLC_1_BOUGHT` at `0x00094524` test this same global. The resume
  signal becomes linked-exact. An ARM32 assertion confirms `Engine`'s
  `appManager` field at `+0x30`.

Focused `ndk23_newrender` source-shape rises **79.6% to 83.9%**, strict
fuzzy **41.8% to 70.5%**; it is not exact. `GetEngine`, `ndk23_ndkDone`,
and `ndk23_sendingResumeSignal` are the three new linked-exact matches.
The native `gof2` library builds, ARM compiles 206/206 translation units,
and 36 verifier tests pass (one optional skip). The native library defines
`gEngine` and no longer defines `g_pEngine`.

The full report is
`C:/Users/Dima/Documents/Galaxy on Fire new version/_verify_integrated_20261008/integrated_engine_singleton_20261011.json`:
4,323/4,524 original functions compared across 4,689 rows, **91.89%**
mean source-shape, **79.96%** strict fuzzy, **2,317 linked-exact** and
**997 byte-exact**. Relative to the preceding DLC-dialog snapshot, no
comparison row or exact pair is lost; 201 functions are missing, none
wrong-type, no unit skipped, and 112 symbols are extra.

This is a confirmed ARM pointer/ABI repair, not a playable Android port.
The Java billing/store side, the role of application-data byte `+0x4c`,
and a running-game DLC request are unverified. Next audit `GameData`'s
application-data byte layout and ownership on 32-bit ARM versus the native
64-bit host before attempting an integrated request-flow smoke.
