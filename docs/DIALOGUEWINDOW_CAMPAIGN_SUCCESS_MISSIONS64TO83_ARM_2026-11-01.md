# DialogueWindow campaign success missions 64–83 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The success
path reads portrait/text pairs at `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. Missions 64–83 add 304
words (152 pages). Missions 69, 78, 79 and 81 have no success pages.
All 304 new words and the 1088-word combined prefix were checked against
the original ELF.

These pages use portrait indices 0, 6, 20, 26, 31, 32 and 33.
Descriptors 32 and 33 were recovered through the original pointer table
at `0x20e29c`: `0x216668` contains `{7, 0, 0, 2, 1}` and `0x21667c`
contains `{0, 5, 4, 8, 3}`. The other descriptors were already present.
None of the new text IDs take the special success paths for 1718, 1730,
1833, 2193–2194 or 2782. The source now selects 544 success pages for
missions 1–83 from the original table prefix. Later missions still use
a placeholder table.

## Verification

- Data self-check: 304/304 added and 1088/1088 combined table words
  match the original ARM ELF; both descriptor values and pointer slots
  match the original portrait table.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` remains at 94.3% source-shape
  and 36.7% strict fuzzy.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Live dialogue and portrait rendering remain untested while device work
is deferred. The next offline block is missions 84–103 and their
additional portrait descriptors.
