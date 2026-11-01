# DialogueWindow campaign success missions 2–7 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The campaign
success branch at `0x167276`-`0x167318` uses
`successOffsets[campaignMission] + page * 2` to select portrait/text
pairs from the table at `0x203ea0`. The counts for missions 2–7 span
44 words (22 complete pages) immediately after mission 1's 38 words.
All 44 new words and the combined 82-word prefix were checked against
the original ELF. These pages use only portrait indices 0, 2 and 16,
whose original pointer and descriptor mappings were already verified.

Text ID 1730 is the only special success ID in missions 2–7. The
original path routes it through `Globals::replaceKeyBindingTokens()`;
the Android helper returns a copy of the string. Mission 1's ID 1718
uses the same call. The source now uses one 82-word table with the
original offsets for all 41 pages of missions 1–7, and preserves the
special helper calls for IDs 1718 and 1730.

## Verification

- Data self-check: 44/44 new and 82/82 combined table words match the
  original ELF; all three portrait indices and both special IDs match.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  92.6% to 92.7%; strict fuzzy improved from 37.0% to 37.8%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live dialogue and portrait rendering remain untested while device work
is deferred. Campaign-success missions 8 onward still use a placeholder
table and require separate row and portrait verification.
