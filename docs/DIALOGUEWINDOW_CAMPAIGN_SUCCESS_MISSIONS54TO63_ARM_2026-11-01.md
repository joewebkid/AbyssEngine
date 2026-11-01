# DialogueWindow campaign success missions 54–63 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The success
path reads portrait/text pairs at `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. Missions 54–63 add 156
words (78 pages). All 156 new words and the 784-word combined prefix
were checked against the original ELF.

The new pages use portrait indices 0, 6, 15, 16, 23, 26, 27 and 28.
Descriptors 27 and 28 were recovered through the original pointer table
at `0x20e29c`: `0x216604` contains `{0, 0, 5, 8, 6}` and `0x216618`
contains `{0, 8, 8, 8, 0}`. The other descriptors were already present.
The source now selects 392 success pages for missions 1–63 from the
original table prefix. Later missions still use a placeholder table.

Mission 60 uses text IDs 2193 and 2194. Original ARM control flow at
`0x1677dc` tests `Mission::getStatusValue() == 1`; when true, it subtracts
two from the text ID and reloads the body text (`0x1677e6`–`0x1677f6`).
The source now chooses IDs 2191 and 2192 in this case. The adjusted ID
continues to the later dialogue sound lookup; other status values retain
2193 and 2194.

## Verification

- Data self-check: 156/156 added and 784/784 combined table words match
  the original ARM ELF; both descriptor values and pointer slots match.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape rose from 93.5%
  to 94.3%; strict fuzzy remains 36.7%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Live dialogue, conditional text and portrait rendering remain untested
while device work is deferred. Next offline block: missions 64–73.
