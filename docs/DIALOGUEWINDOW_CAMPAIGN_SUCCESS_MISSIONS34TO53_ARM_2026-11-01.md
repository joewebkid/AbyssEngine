# DialogueWindow campaign success missions 34–53 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The success
path reads portrait/text pairs from `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. Missions 34–53 add 192
words (96 pages). Missions 40, 45 and 49–53 have no success pages.
All 192 new words and the 628-word combined prefix were checked against
the original ELF.

The new pages use portrait indices 0, 1, 6, 7, 15, 23, 25 and 36.
The four newly needed descriptors were recovered through the original
pointer table at `0x20e29c`:

| Index | Pointer | Descriptor |
| --- | --- | --- |
| 15 | `0x216514` | `{11, 1, 0, 0, 1}` |
| 23 | `0x2165b4` | `{0, 2, 10, 9, 2}` |
| 25 | `0x2165dc` | `{1, 3, 1, 5, 0}` |
| 36 | `0x2166b8` | `{0, 2, 10, 9, 2}` |

Indices 23 and 36 have identical data at distinct original addresses.
None of the new text IDs take the special success paths for 1718, 1730,
1833, 2193–2194 or 2782. The source now selects 314 success pages for
missions 1–53 from the original table prefix. Later campaign-success
missions still use a placeholder table.

## Verification

- Data self-check: 192/192 added and 628/628 combined table words match
  the original ARM ELF; the four descriptor values and pointer slots
  match the original portrait table.
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
is deferred. The next offline block is missions 54–63, including the
conditional text change for IDs 2193–2194.
