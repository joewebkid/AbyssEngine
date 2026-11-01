# DialogueWindow campaign success missions 8–13 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The campaign
success path indexes portrait/text pairs in the table at `0x203ea0`
using `successOffsets[campaignMission] + page * 2`. Missions 8–13 add
110 words (55 complete pages) after the previously restored 82-word
prefix. All 110 new words and the 192-word combined prefix were
checked against the original ELF.

These pages use portrait indices 0, 2, 3, 4, 16 and 17. Descriptors 3
and 17 were added from the original pointer table at `0x20e29c`:
`0x216424` contains `{0, 9, 9, 8, 9}` and `0x21653c` contains
`{11, 1, 0, 0, 3}`. All six pointer indices and the five file-backed
descriptors were checked; index zero points into `.bss`. No special
success text ID requiring a separate control path occurs in missions
8–13.

The source now uses one 192-word table for all 96 success pages of
campaign missions 1–13, with the original offsets, portrait mapping,
name IDs and mirror state. Later campaign-success missions still use
the placeholder table.

## Verification

- Data self-check: 110/110 new and 192/192 combined words match the ARM
  ELF; six portrait indices and the two new five-word descriptors match.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` remains at 92.7% source-shape
  and 37.8% strict fuzzy; this package restores table contents without
  changing the compiled instruction shape.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live dialogue and portrait rendering remain untested while device work
is deferred. Campaign-success missions 14 onward require separate row
and portrait verification.
