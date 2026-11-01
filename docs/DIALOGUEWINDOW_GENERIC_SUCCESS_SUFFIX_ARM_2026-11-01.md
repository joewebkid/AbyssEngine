# DialogueWindow generic success suffix (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. After the
special success cases for mission types 3, 5, 12 and 183, instructions
`0x167c10`-`0x167c50` select a generic success text ID, using the
German selector only when the language is German and an agent exists.
Instructions `0x167c60`-`0x167d4e` then append two newlines and either
text ID 389 with `#S` replaced by the story agent's station name, or
text ID 216 for an ordinary or absent agent. The earlier story-agent
test is at `0x167326`-`0x16733a`. The successful-station override for
station 108 still follows after standing is awarded.

The source previously assigned only the generic text. It now appends
the matching suffix on that generic success path and leaves the four
special mission types on their own paths.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  73.6% to 81.0%; strict fuzzy declined from 25.5% to 22.1%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live dialogue rendering remains untested while device work is
deferred. Campaign text tables and other `loadContent()` behavior
remain incomplete.
