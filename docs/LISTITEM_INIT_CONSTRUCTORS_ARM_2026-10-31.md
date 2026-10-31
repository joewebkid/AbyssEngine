# ListItem initialization and simple constructors (Android ARM)

## Evidence

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
has an out-of-line `ListItem::init` at `0xa6428`. The short `Agent*` and
integer constructors (`0xa655a`, `0xa6688`) call it, then set their own
fields. The reconstructed compiler had inlined `init`, leaving those
constructors at 25.7–34.9% source-shape and without exact matches.

Original ARM `init` writes zero to `text` and `craftable` together at
offsets `0x44..0x45`, clears the pointer fields at `0x04..0x20`, and
sets the selection/index fields. It does not write the `lines` pointer
at offset `0x00`. The previous C++ cleared `lines` and left `craftable`
uninitialized.

## Change

`init` now remains out of line on the build compilers, matching the
original call boundary. Its field initialization order follows the ARM
stores, adds `craftable = 0`, and omits the non-original `lines = 0`.
The short constructors still call the same helper and set the same
post-initialization fields.

## Verification

- Windows native and ARM64 `gof2` static targets: built successfully.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Focused `game/ui/ListItem`: 55 comparisons, average source-shape
  98.7%, strict fuzzy 93.5%, 37 linked-exact and 15 byte-exact. Eighteen
  constructor aliases gained linked-exact, with zero focused exact losses.
  `init` source-shape rose 82.3% -> 94.3%, strict fuzzy 12.9% -> 28.6%.
- Full corpus: 4,323/4,524 original functions covered by 4,752 comparison
  rows, zero skipped, 201 missing. Average source-shape rose 95.58% ->
  95.91%; strict fuzzy rose 85.20% -> 85.65%. Linked-exact rose 2,689 ->
  2,707, with precisely the 18 constructor aliases gained and zero old
  matches lost. Byte-exact stayed at 1,091, with zero losses.

The original leaves `lines` untouched for constructors that do not
receive an array; runtime use of those instances has not been tested.
The next priority is checking the text/array constructors and their
`getNumLines` call contract against the original before changing them.
