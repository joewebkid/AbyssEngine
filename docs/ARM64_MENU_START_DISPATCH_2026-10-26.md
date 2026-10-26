# ARM64 start-game menu dispatch (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump shows `MenuTouchWindow::startGOF2` at `0x12cf84`:
reset game state, copy the menu fade value to `flt_218214`, stop the
current music event, play event `0x8f` with null position/velocity
and zero pitch, then select application module 2. The ELF places
`Globals::options` at `0x2181e8`, making `flt_218214` the option
field at offset `0x2c`.

The AArch64 path now uses `OptionsRecord::fadeValue` rather than the
unrelated `Status::fadeValue` offset; it calls the recovered typed
`FModSound::stop` and `play` methods with the original parameters,
then `ApplicationManager::SetCurrentApplicationModule(2)`. Other
menu sound effects now call typed `FModSound::play` with a null
velocity argument. ARMv7 call forms remain guarded and untouched.

Native and AArch64 static builds pass; ARMv7 compiles **209/209**
translation units. Rebuilt ARMv7 `MenuTouchWindow.o` retains SHA-256
`325a996305e687f82581795ea086a933cbe6813ca9c1e62c261b91707936f7e1`.
Strict JNI-initialize AArch64 unresolved names fall **23 → 21** with
no new names. The full ARM corpus was not rerun because the affected
ARMv7 object is byte-identical; the last full check remains
**4,323/4,524** originals over **4,745** pairs, **94.79%
source-shape / 84.31% strict fuzzy**, **2,607 linked-exact / 1,083
byte-exact**, zero skips.

The silent adapter still produces no sound. The transition to a
running game has not been tested, and no ARM64 game `.so`/APK exists
yet. The typed event sequence was accepted; using the result of
`stop` as a pitch value and routing the fade to `Status` were rejected.
