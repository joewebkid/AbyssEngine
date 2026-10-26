# Globals typed constructors on AArch64 (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The recovered `Globals::init` and helper methods still used explicit
32-bit allocation sizes and unimplemented constructor/destructor
wrappers for `Mission`, `Galaxy`, `Achievements`, `FileRead`,
`AERandom`, and `Generator`. Those byte counts cannot be assumed to
fit 64-bit C++ objects.

On AArch64, these sites now use normal typed `new` and `delete`, which
select each class's actual size and invoke its existing constructor or
destructor. The Android ARMv7 call form remains behind architecture
guards. The rebuilt ARMv7 `Globals.o` SHA-256 is unchanged:
`dc07e21db114a48a47aec1b189c0174fceaf733682c8d3e8cd8cfaf6d4cd1505`.

Native and AArch64 static-library builds pass; the ARMv7 build
compiles **209/209** translation units. The strict JNI-initialize
AArch64 link diagnostic falls **30 → 23** unresolved names, removing
the seven constructor/destructor wrapper names with no new names.
The full ARM corpus was not rerun because the only affected ARMv7
object is byte-identical; the last full check remains **4,323/4,524**
originals over **4,745** pairs, **94.79% source-shape / 84.31%
strict fuzzy**, **2,607 linked-exact / 1,083 byte-exact**, zero skips.

The allocations and dispatch are checked by compilation and link
diagnostics, but object initialization and asset loading have not
been exercised in a running ARM64 game. A game `.so`/APK remains
unbuilt. The typed form was accepted; preserving literal ARMv7
allocation sizes on AArch64 was rejected.
