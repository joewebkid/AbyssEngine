# RocketGun math call ABI (2026-10-26)

Reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The original `RocketGun::update` begins at `0x15ecd8`; `seekEnemy`
begins at `0x15f44c`.

In `update`, the calls at `0x15ed5c` and `0x15edc0` target
`AEMath::MatrixRotateVector` and `MatrixGetDir`. The calls at
`0x15ee92`, `0x15eeb6` and `0x15f2b0` target `VectorCross`;
`0x15ee9c`, `0x15eec6`, `0x15f2ba` and `0x15f396` target
`VectorNormalize`. Three indirect calls at `0x15eeac`, `0x15eebe`
and `0x15eed2` use the GOT relocation at `0x20fe98` for
`Vector::operator=`. Four indirect calls at `0x15f2da`, `0x15f2e4`,
`0x15f30e` and `0x15f316` use the GOT relocation at `0x210264` for
`AEMath::operator*(const Vector&, float)`.

In `seekEnemy`, the three indirect calls at `0x15f514`, `0x15f530`
and `0x15f55e` use the GOT relocation at `0x210260` for
`AEMath::VectorNormalize`. The recovered source had called all six
normalization and copy sites through one undefined
`VectorRotateToTarget` declaration. The two methods now use the
original operations at each site, and the other five undefined global
math declarations were removed in favor of the existing `AEMath`
functions and vector assignment.

The first `MatrixSetTranslation` call in `update` had another real
behavior gap. At `0x15ee16–0x15ee3e`, the original obtains the
transform's local matrix and passes it as the mutable input (`r1`),
with the returned matrix written to a stack local (`r0`). The recovered
source instead modified an uninitialized local and then overwrote it
with a copy of the unchanged transform. It now passes the actual
transform local and initializes the returned matrix from that call.
The updated ARMv7 object has the same `r0` output/`r1` transform-input
contract at its `MatrixSetTranslation` relocation.

Focused `update` source-shape/strict fuzzy changes
**91.8%/26.1% → 89.3%/19.7%** and `seekEnemy` changes
**87.8%/47.7% → 84.1%/41.5%**; neither function is exact. The
lower scores are accepted because the original call destinations,
copy-versus-normalize behavior and transform data flow are directly
observed. All **17**
functions in the focused unit compare, with no linked-exact or
byte-exact symbol losses. Native `gof2`, the AArch64 static archive
and **209/209** ARMv7 translation units build. Strict AArch64
whole-archive unresolved symbols fall **574 → 568**, removing six
undefined global math signatures.

The complete ARM corpus compares **4,743** symbol pairs and covers
**4,323/4,524** original functions, with **201** missing and no
skipped comparisons. Mean source-shape is **94.71%**, strict fuzzy
**84.21%**, linked-exact **2,601** and byte-exact **1,083**. Neither
exact symbol set lost or gained a member against the preceding
StarSystem baseline.

Live rocket steering, cluster-wave movement and trail rendering remain
unverified until the Android game starts. The shared library and APK
are still not linkable. Next priority: continue recovering original
call targets in rendering and gameplay code, then exercise these paths
on a device.
