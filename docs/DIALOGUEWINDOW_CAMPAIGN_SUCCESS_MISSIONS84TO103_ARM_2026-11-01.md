# DialogueWindow campaign success missions 84–103 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The success
path reads portrait/text pairs at `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. Missions 84–103 add 254
words (127 pages). Missions 84, 87, 89, 91 and 100 have no success
pages. All 254 new words and the 1342-word combined prefix were checked
against the original ELF.

These pages use portrait indices 0, 1, 2, 6, 15, 17, 20, 22 and 37–40.
The five newly needed descriptors were recovered through the original
pointer table at `0x20e29c`:

| Index | Pointer | Descriptor |
| --- | --- | --- |
| 22 | `0x2165a0` | `{2, 0, 0, 0, 0}` |
| 37 | `0x2166cc` | `{2, 4, 1, 4, 1}` |
| 38 | `0x2166e0` | `{0, 8, 3, 8, 3}` |
| 39 | `0x2166f4` | `{2, 2, 1, 3, 2}` |
| 40 | `0x216708` | `{6, 0, 0, 2, 0}` |

None of the new text IDs take the special success paths for 1718, 1730,
1833, 2193–2194 or 2782. The source now selects 671 success pages for
missions 1–103 from the original table prefix. Later missions still use
a placeholder table.

## Verification

- Data self-check: 254/254 added and 1342/1342 combined table words
  match the original ARM ELF; all five descriptor values and pointer
  slots match the original portrait table.
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
is deferred. Next offline block: missions 104–113, including portrait
index 62. The original pointer table contains 63 entries; the source
currently restores only the first 50.
