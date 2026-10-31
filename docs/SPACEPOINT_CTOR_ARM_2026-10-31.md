# SpacePoint constructor recovery (Android ARM)

## Source evidence

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has `SpacePoint::SpacePoint(int, Vector const&, Vector const&, int)` at
`0x1603ac`. REA/IDA assembly first clears the two 12-byte vector fields,
then calls `Vector::operator=` for the position and direction, and finally
sets the free flag and index. The earlier C++ used direct member copies,
which omitted the two out-of-line assignments and their preceding
initialization.

## Change and verification

The constructor now value-initializes both vectors and assigns them in the
body before setting the free flag and index. This follows the original
observable order, including the calls to `Vector::operator=`.

- Windows native and ARM64 `gof2` static targets: built successfully.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Focused `game/world/SpacePoint`: 8/8 functions at 100% source-shape and
  strict fuzzy; all 8 linked-exact, 6 byte-exact. Both constructor aliases
  improved from 20.6% to 100% source-shape and from 5.4% to 100% strict
  fuzzy; both gained linked-exact. The other six exact functions remain exact.
- Full corpus: 4,323/4,524 original functions covered by 4,752 comparison
  rows, zero skipped, 201 missing. Average source-shape rose 95.55% ->
  95.58%; strict fuzzy rose 85.16% -> 85.20%. Linked-exact rose 2,687 ->
  2,689, with exactly the two constructor aliases gained and zero old
  matches lost. Byte-exact stayed 1,091, with zero losses.

This static comparison does not exercise live gameplay on ARM32. The next
offline priority is the neighboring low-match `ListItem` constructors;
runtime behavior remains a separate device check.
