# PlayerEgo ARM64 dispatch and construction (2026-10-26)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
The recovered `PlayerEgo::stopShooting` selects the single-slot or
slot-and-channel `Player::stopShooting` overload. `PlayerEgo::setShip`
creates a `TractorBeam` from ship geometry and a kind derived from
equipment; its emergency-system path copies a 3-float transform
vector into `emergencyVec`.

On AArch64, these calls now use the corresponding typed `Player`,
`TractorBeam`, and `AEMath::Vector` operations. The ARMv7 call form
remains architecture guarded. Native and AArch64 static builds pass;
the ARMv7 build compiles **209/209** units. Rebuilt ARMv7
`PlayerEgo.o` is byte-identical with SHA-256
`a16742e307bc3b9c1f28ed1ba54f59ef3b08f78223bc7ed2a6280c47a51715b1`.
The scoped `PlayerEgo` comparison continues to have **99
linked-exact** and **61 byte-exact** matches, with zero exact-set
losses. Strict JNI-initialize AArch64 unresolved names fall **18 →
14** with no new names. The last full ARM corpus remains
**4,323/4,524** originals over **4,745** pairs, **94.79%
source-shape / 84.31% strict fuzzy**, **2,607 linked-exact / 1,083
byte-exact**, zero skips.

The typed dispatch and construction were accepted; placeholder
wrappers were rejected. Live firing, tractor-beam and emergency-system
behavior remain unverified. A game `.so`/APK is not yet built.
