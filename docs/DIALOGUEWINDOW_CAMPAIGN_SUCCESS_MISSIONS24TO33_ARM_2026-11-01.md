# DialogueWindow campaign success missions 24–33 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The campaign
success path reads portrait/text pairs from `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. Missions 24–33 add 104
words (52 complete pages); missions 24 and 28 have no success pages.
All 104 new words and the 436-word combined prefix were checked against
the original ELF.

These pages use portrait indices 0, 1, 6, 17 and 20. Their descriptors
were already in the source. None of the new text IDs take the special
success paths for 1718, 1730, 1833, 2193–2194 or 2782. The source now
selects 218 success pages for missions 1–33 from the original table
prefix, with the original offsets, portraits, names and mirror state.
Later campaign-success missions still use a placeholder table.

## Verification

- Data self-check: 104/104 added and 436/436 combined table words match
  the original ARM ELF. Source counts total 104 words for missions 24–33.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` remains at 93.5% source-shape
  and 36.7% strict fuzzy.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Live dialogue and portrait rendering remain untested while device work
is deferred. Next offline block: missions 34–43; their 58 success pages
also use portrait descriptors already present.
