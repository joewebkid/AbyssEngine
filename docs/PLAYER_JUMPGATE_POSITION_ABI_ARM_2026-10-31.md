# PlayerJumpgate position call ABI (Android ARM)

## Evidence and change

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `PlayerJumpgate::setPosition(float,float,float)` at `0xa51c0`.
REA/IDA assembly converts each coordinate to an integer for the object's
stored position fields, then tail-calls
`AEGeometry::setPosition(float,float,float)` with the original floats.
The previous C++ made a temporary `Vector` and called the vector overload.
Although that overload currently forwards the same coordinates, it changes
the call ABI and adds an unnecessary intermediate object. The source now
calls the three-float overload directly.

## Verification

- Windows native and ARM64 `gof2` static targets: built successfully.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Focused `game/ship/PlayerJumpgate`: 13 comparisons, average source-shape
  98.4%, strict fuzzy 91.3%, 8 linked-exact and 1 byte-exact. The target
  function rose from 29.4% to 100% source-shape and from 20.5% to 100%
  strict fuzzy, gaining linked-exact with zero focused exact losses.
- Full corpus: 4,323/4,524 original functions covered by 4,752 comparison
  rows, zero skipped, 201 missing. Average source-shape rose 95.92% ->
  95.93%; strict fuzzy rose 85.69% -> 85.70%. Linked-exact rose 2,719 ->
  2,720, with only `setPosition` gained and zero old matches lost.
  Byte-exact stayed at 1,091, with zero losses.

This verifies the native call path statically; position updates in a live
game were not exercised. The next priority is a separate low-match
gameplay method with a confirmed behavior or ABI gap.
