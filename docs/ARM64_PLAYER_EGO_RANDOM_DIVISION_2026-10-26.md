# ARM64 random, steering division, and engine sound (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The local IDA dump shows `Globals::getShipGroup` at `0xe4e14`
calling `AERandom::nextInt(Globals::rnd, 4)` for variant 3. The
recovered AArch64 path had passed a literal zero to an unresolved
random wrapper. `PlayerEgo`'s steering calculations divide signed
values by 63 through the ARM EABI integer-division helper, and its
docking transitions invoke its existing `PlayEngineSound` method.

On AArch64, the ship-group branch now calls the typed global random
generator, the signed steering calculation uses native C++ division,
and docking dispatches to `PlayerEgo::PlayEngineSound`. The ARMv7
forms remain architecture guarded.

Native and AArch64 static builds pass; ARMv7 compiles **209/209**
translation units. Rebuilt ARMv7 `Globals.o` remains byte-identical
with SHA-256
`dc07e21db114a48a47aec1b189c0174fceaf733682c8d3e8cd8cfaf6d4cd1505`.
A scoped comparison of **173** `PlayerEgo` functions retains all
**99 linked-exact** and **61 byte-exact** matches with zero exact-set
losses. Strict JNI-initialize AArch64 unresolved names fall **21 →
18** with no new names. The last full ARM corpus remains
**4,323/4,524** originals over **4,745** pairs, **94.79%
source-shape / 84.31% strict fuzzy**, **2,607 linked-exact / 1,083
byte-exact**, zero skips.

The source-backed calls were accepted; a null random receiver and
opaque engine-sound wrapper were rejected. Live ship generation and
docking behavior remain unverified. A game `.so`/APK is not yet built.
