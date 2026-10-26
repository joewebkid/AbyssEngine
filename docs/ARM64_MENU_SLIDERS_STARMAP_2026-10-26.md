# ARM64 menu slider and map dispatch (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local Android IDA dump shows `MenuTouchWindow::OnTouchMove`
calling `TouchSlider::getValue` for sliders 1–3 and
`FModSound::setVolume` with category IDs **1, 2, 3**. It calls
`FModSound::tryToStopMusicForBGMusic` before category 1. The
`MissionsWindow::OnTouchEnd` map branch delegates to
`StarMap::OnTouchEnd`.

The AArch64 options path now reads the typed `Array<TouchSlider *>`
and calls the recovered slider and FMOD methods with the correct
category IDs. The prior call omitted the category argument, and its
raw pointer arithmetic obscured the array layout on 64-bit targets.
The map branch calls the typed `StarMap` method. ARMv7 call forms are
preserved by architecture guards.

Native and AArch64 static builds pass; the ARMv7 build compiles
**209/209** units. Rebuilt ARMv7 `MenuTouchWindow.o` retains SHA-256
`325a996305e687f82581795ea086a933cbe6813ca9c1e62c261b91707936f7e1`.
The scoped `MissionsWindow` comparison retains all **7 linked-exact**
and **2 byte-exact** functions, with zero exact-set losses. The last
full ARM corpus remains **4,323/4,524** originals over **4,745** pairs,
**94.79% source-shape / 84.31% strict fuzzy**, **2,607 linked-exact /
1,083 byte-exact**, zero skips. Strict JNI-initialize AArch64 link
diagnostics fall **33 → 30** unresolved names, with no new names.

The silent ARM64 mode still produces no audio. Live slider input and
map interaction on Android remain unverified, and a game `.so`/APK
has not yet been built. The recovered typed calls were accepted;
placeholder wrappers with incomplete FMOD parameters were rejected.
