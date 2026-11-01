# DialogueWindow campaign success mission 1 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. On the
campaign success path, `0x167276`-`0x167318` indexes portrait/text
pairs in the success table at `0x203ea0`, using
`successOffsets[campaignMission] + page * 2`. Mission 1 has offset zero
and 38 words: 19 complete pages. All 38 words were checked against the
original ELF. They use portrait indices 0, 2 and 16, whose mappings
were previously checked against the ARM pointer and descriptor data.

The last page selects text ID 1718. The original success path routes
this ID through `Globals::replaceKeyBindingTokens()` at `0x1674be`-
`0x167484`; the Android helper returns a copy of the input string.
No other special success text ID appears in this mission's 19 pages.

The source now selects each mission-1 page's portrait, name and body
text from its original pair, mirrors portrait zero, and makes the
confirmed helper call for text 1718. Other campaign-success missions
still use the existing placeholder table and remain incomplete.

## Verification

- Data self-check: 38/38 source words match the original ARM ELF;
  portrait indices and special text ID were checked separately.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  90.6% to 92.6%; strict fuzzy improved from 36.5% to 37.0%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live dialogue and portrait rendering remain untested while device work
is deferred. The other campaign-success missions require their own
verified rows and portrait mappings.
