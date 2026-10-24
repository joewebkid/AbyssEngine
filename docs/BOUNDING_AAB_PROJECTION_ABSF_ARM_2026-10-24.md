# BoundingAAB projection absolute-value call (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0x98280`, `BoundingAAB::projectCollisionOnSurface` constructs six
face-offset vectors, calls the engine's `AEMath::Absf(float)` for each
candidate distance, selects the smallest distance, and subtracts that
offset from the queried point. The ARM GOT entry at `0x21022c` resolves
to `_ZN11AbyssEngine6AEMath4AbsfEf`; the six indirect calls in this
function use it. The previous C++ instead used `std::fabs`, which Clang
lowered to inline `vabs.f32` instructions. This bypassed the engine
helper's distinct signed-zero and NaN handling at the intermediate
distance step; no observable difference in final projected coordinates
for those edge cases has been established. The six calls now use the
already reconstructed engine helper; face choice and coordinate mapping
are otherwise unchanged.

Focused NDK r18b ARM `-Oz` comparison improves from **71.8% source-shape /
20.2% strict fuzzy** to **95.2% / 58.6%**. The function remains nonexact.
The original-asset station gate passes at host `-O0` and `-O2`, including
its 44 station volumes and direct Level parser checks of 12 station and
26 static shapes. A box with nonzero base center and extents additionally
checks projection onto its nearest face through the production method.
This verifies the finite-input geometry on the host, not live Android
behavior or NaN propagation through the collision chain.

Native `gof2` and all 208 ARM translation units build; 93 verifier tests
pass (one skip). The full ARM corpus compares **4,323/4,524** original
functions across **4,736** pairs, with mean **94.16% source-shape /
83.49% strict fuzzy**, **2,562 linked-exact**, **1,082 byte-exact**,
**2,728 100%-fuzzy**, **201 missing** and no skipped functions.
Pairwise comparison against the preceding sphere-vector baseline finds
no linked- or byte-exact gains or losses. The current missing-symbol
list consists of C++ runtime, crypto, ZIP/zlib and platform-support
symbols, not a missing `BoundingAAB` method; this does not imply that
all game behavior is reconstructed. Next source candidate:
`BoundingVolume::update` (42.9% source-shape), after an ARM behavior
review. The pending device-runtime priority remains first-friend Pan
ship insertion when ADB becomes available.
