# DialogueWindow type-183 success text (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. On the
non-campaign success path, `0x1678d2`-`0x1678d8` compares the mission type
with `0xb7` (183). For this type, `0x1678dc`-`0x1678f0` always chooses
one of five text IDs with `AERandom::nextInt(5)`; the source table at
`0x205fa4` contains IDs 373, 374, 375, 376 and 377. The existing
generic success table contains the same five entries. This path does
not call the German generic-text selector.

Instructions `0x167908`-`0x167978` append two newlines, text ID 176,
the literal `": "`, and `level->killCountB` (offset `0x24`) to the
chosen text. The newline and separator bytes were checked in the
original ELF at `0x1cb162` and `0x1cafe9`. The source now follows that
sequence and keeps the chosen text ID for dialogue sound selection.
The existing station-108 override still applies after standing is
awarded.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  66.9% to 73.6%; strict fuzzy improved from 22.9% to 25.5%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape rose
  from 95.98% to 95.99%; strict fuzzy is 85.78%, with 2,724 linked-exact
  and 1,091 byte-exact. Exact-set comparison against the previous report
  found zero losses and zero gains.

Live dialogue rendering and the displayed quantity remain untested
while device work is deferred. Campaign text tables and other
mission-specific branches in `loadContent()` remain incomplete.
