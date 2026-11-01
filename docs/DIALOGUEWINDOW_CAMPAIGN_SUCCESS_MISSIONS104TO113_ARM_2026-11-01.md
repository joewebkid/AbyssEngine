# DialogueWindow campaign success missions 104–113 and portrait table (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The success
path reads portrait/text pairs at `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. Missions 104–113 add 84
words (42 pages); missions 105–107 have none. All 84 new words and the
1426-word combined prefix were checked against the original ELF.

These pages use portrait indices 0, 1, 6, 20, 37, 41, 42 and 62. The
source's portrait pointer table previously ended at index 49, but the
original table at `0x20e29c` has 63 entries. Index zero points to
`0x2183c0` in `.bss`. Indices 1–62 point to consecutive five-integer
descriptors at `0x2163fc`–`0x2168c0`. The source now contains all 63
pointer slots and all 62 file-backed descriptor values; every slot and
descriptor was checked against the original ELF. This adds 33 previously
missing descriptors, including mission 113's portrait 62
`{0, 10, 2, 9, 4}` at `0x2168c0`.

None of the new text IDs take the special success paths for 1718, 1730,
1833, 2193–2194 or 2782. The source now selects 713 success pages for
missions 1–113 from the original table prefix. Later missions still use
a placeholder table.

## Verification

- Data self-check: 84/84 added and 1426/1426 combined table words match
  the original ARM ELF; 63/63 portrait pointer slots and 62/62
  file-backed descriptors match.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` remains at 94.3% source-shape
  and 36.7% strict fuzzy.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Index zero's initial `.bss` data does not establish its runtime state.
Live dialogue and portrait rendering remain untested while device work
is deferred. Next offline block: missions 114–123 and the special text
2782 path.
