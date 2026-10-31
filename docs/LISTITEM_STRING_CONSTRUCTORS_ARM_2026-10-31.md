# ListItem string-copy constructor ABI (Android ARM)

## Evidence

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
calls `AbyssEngine::String::String(String const&, bool)` with `false`
when `ListItem` copies a supplied name. REA/IDA pseudocode shows this
in the one-string, two-string, string-and-flag, indexed-string, and
`ListItem*` copy constructors. The prior source used the separate
one-argument copy-constructor shim.

The original `ListItem::init` leaves the `lines` pointer untouched.
The array constructor assigns `lines` after `init`, while the
string-and-flag constructor writes `text` but does not assign `lines`.
Original `getNumLines` reads `lines` only when `text` is set. IDA shows
no direct cross-references to `getNumLines`; indirect use and runtime
behavior remain unverified.

## Change

All nine `new String(*source)` expressions in `ListItem.cpp` now pass
an explicit `false`, selecting the original two-argument constructor.
The two local implementations currently copy the same character data;
this change restores the original call ABI and source shape.
The field layout, `lines` behavior, and copy-constructor field ordering
were not changed. The remaining copy-constructor mismatch is chiefly
scalar versus vectorized field copying; no unsupported rewrite was made.

## Verification

- Windows native and ARM64 `gof2` static targets: built successfully.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Focused `game/ui/ListItem`: 55 comparisons, average source-shape 99.3%,
  strict fuzzy 96.4%, 49 linked-exact and 15 byte-exact. Twelve constructor
  aliases gained linked-exact, with zero focused exact losses.
- Full corpus: 4,323/4,524 original functions covered by 4,752 comparison
  rows, zero skipped, 201 missing. Average source-shape rose 95.91% ->
  95.92%; strict fuzzy rose 85.65% -> 85.69%. Linked-exact rose 2,707 ->
  2,719, with exactly the 12 text-constructor aliases gained and zero old
  matches lost. Byte-exact stayed at 1,091, with zero losses.

Next priority: inspect the remaining `ListItem*` copy-constructor field
copying and its actual call sites. A running-game check of text rows is
still needed when device testing resumes.
