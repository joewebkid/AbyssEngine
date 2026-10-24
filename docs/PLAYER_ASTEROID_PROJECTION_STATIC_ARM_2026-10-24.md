# PlayerAsteroid projection static-vector mutation (2026-10-24)

Primary reference: Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`.
At `0xe3084`, `PlayerAsteroid::getProjectionVector` copies the input
vector to its return storage before querying the geometry. It then
assigns the geometry position to class-static `pos` (`0xe30c2`),
subtracts the copied input **in place** from `pos` (`0xe30c8`),
copies that difference to the return storage (`0xe30d0`), normalizes
it (`0xe30d8`) and copies the normalized result back (`0xe30e0`).

The prior C++ assigned the geometry position to `pos` but subtracted
into a separate local vector. It returned the expected direction for
ordinary inputs while leaving the static at the wrong value. It also
read `value` after mutating `pos`, so passing `pos` itself by reference
would change the input before subtraction. The recovered method keeps
the observed order of copies and operations. A simpler variant that
mutated `pos` but omitted the two result assignments was semantically
correct for these cases but worsened ARM shape and was rejected.

Focused NDK r18b ARM `-Oz` comparison improves source-shape/strict
fuzzy **77.0%/51.2% → 100%/100%**. The 49-instruction function is
linked-exact, not byte-exact; the normalized disassembly is identical.
A source-extracted host smoke at `-O0/-O2` checks the normalized
direction, static `pos` side effect, unchanged ordinary input and an
input that aliases `pos`. Native `gof2`, all **208/208** ARM translation
units and **93** verifier tests pass (one skip). This is not a live
Android collision or mining test.

The full ARM corpus compares **4,323/4,524** original functions over
**4,736** pairs, with mean **94.18% source-shape / 83.52% strict
fuzzy**, **2,563 linked-exact**, **1,082 byte-exact**, **2,729
100%-fuzzy**, **201 missing**, zero wrong-type and zero skipped.
Pairwise comparison with the preceding push-matrix baseline shows
exactly this function gained linked-exact; no linked- or byte-exact
pair was lost. ADB lists no device, so live projection behavior remains
unverified.

The next source candidate is `PlayerAsteroid::getQualityString`: ARM
at `0xe2a28` calls the existing `String(const char*, bool)` constructor,
whereas the current source manually grows a UTF-16 buffer one character
at a time. The quality letters themselves were recovered earlier; the
constructor ABI and ownership still merit a separate review. First-
friend Pan ship insertion remains the device priority when ADB returns.
