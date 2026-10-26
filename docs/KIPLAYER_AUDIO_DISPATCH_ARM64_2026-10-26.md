# KIPlayer engine-sound dispatch on AArch64 (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump shows `KIPlayer::PauseEngineSound` at `0xa617e`,
`PlayEngineSound` at `0xa6192`, `ResumeEngineSound` at `0xa61a8`, and
`StopEngineSound` at `0xa61be`. Each checks its `Player*` and the
engine-sound event before delegating to the corresponding `Player`
method; resume passes false, and play passes the event ID and a null
third argument.

The AArch64 path now calls those existing typed `Player` methods.
The ARMv7 call form stays intact because all four functions were
already linked-exact. A rebuilt ARMv7 `KIPlayer.o` has the same
SHA-256 before and after:
`3d18aa71c0771fa707e96423bff8a48261d59e13e2f9b325009b6bc1eaaa8319`.
The build compiles **209/209** ARMv7 units, and native and AArch64
`gof2` static-library builds pass. The full ARM corpus was not rerun
for this architecture-specific patch; its last checked figures stay
at **4,323/4,524** originals over **4,745** pairs, **94.79%
source-shape / 84.30% strict fuzzy**, **2,607 linked-exact /
1,083 byte-exact**, zero skips.

The strict JNI-initialize AArch64 link diagnostic falls **55 to 51**
unresolved names and the broader JNI-onload diagnostic **265 to 261**,
removing all four `FModSound_*Event` wrappers with no new names.
This changes dispatch, not sound output: the first ARM64 build still
uses the explicit silent adapter. A game `.so`/APK and live audio-state
behavior on Android remain unverified.
