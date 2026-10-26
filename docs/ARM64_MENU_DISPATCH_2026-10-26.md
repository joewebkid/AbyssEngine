# ARM64 menu dispatch to recovered methods (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump shows `MenuTouchWindow::render3D` at `0x12cfe4`
calling `MissionsWindow::render3D` when menu state is 9 and the
missions window exists. It shows `FModSound::tryToStopMusicForBGMusic`
at `0x92da8` returning zero, and `Layout::getPulseValue` at `0xd4bf0`
using `AbyssEngine::AEMath::Sinf`.

The AArch64 path now calls the recovered `MissionsWindow` method,
dispatches the background-music checks through the existing
`FModSound` method, and uses the engine's `Sinf` in `Layout` and
`SpaceLounge`. `MenuTouchWindow` also uses
`ApplicationManager::GetApplicationData` and sends leaderboard results
through `Globals::reportLeaderboards(Globals::globals)`, the call form
shown in its Android `OnTouchEnd` body. The changes are architecture
guarded because these callers were already matched on ARMv7. Rebuilt
ARMv7 objects are
byte-identical to their prior hashes:

| Unit | SHA-256 before and after |
| --- | --- |
| `MenuTouchWindow.o` | `325a996305e687f82581795ea086a933cbe6813ca9c1e62c261b91707936f7e1` |
| `Globals.o` | `dc07e21db114a48a47aec1b189c0174fceaf733682c8d3e8cd8cfaf6d4cd1505` |
| `Layout.o` | `75c1ba88b31801431ebbbda008f7a39590fc12066a81b94f20670af3348606f8` |
| `SpaceLounge.o` | `da519e22ec8eead7e86a47b852ba56c9b54bdc6adc3e5f35b7800a5a998106f5` |

Native and AArch64 static builds pass, and the ARMv7 build compiles
**209/209** translation units. Strict JNI-initialize AArch64 link
diagnostics fall from **39 to 33** unresolved names, with no new
names. The full ARM corpus was not rerun for these architecture-only
changes because the four affected ARMv7 objects are unchanged; the
last full comparison is **4,323/4,524** originals, **94.79%
source-shape / 84.31% strict fuzzy**, **2,607 linked-exact / 1,083
byte-exact**, zero skips and zero exact-set losses.

This closes dispatch references, not live menu or audio behavior.
The silent ARM64 build still has no game `.so` or APK, and screen
rendering on a device is unverified. The typed calls were accepted;
invented standalone implementations of the old wrapper names were
rejected.
