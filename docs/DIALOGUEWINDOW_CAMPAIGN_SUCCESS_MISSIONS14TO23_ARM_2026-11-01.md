# DialogueWindow campaign success missions 14–23 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The success
path indexes portrait/text pairs at `0x203ea0` through
`successOffsets[campaignMission] + page * 2`. Missions 14–23 add 140
words (70 pages); mission 14 has zero pages. All 140 added words and the
332-word combined prefix were checked against the original ELF.

The new pages use portrait indices 0, 1, 5, 6, 12, 13, 16 and 17.
Descriptors 12 and 13 were recovered through the original pointer table
at `0x20e29c`: `0x2164d8` contains `{0, 5, 4, 2, 7}` and `0x2164ec`
contains `{10, 1, 1, 2, 4}`. The other descriptors were already present.
Index zero points into `.bss`, so its initial zero data is not proof of
its runtime contents.

The page with text ID 1833 follows a separate ARM control path. The
instructions at `0x167522`–`0x16754a` stop the current music event, play
event `0x88`, then event `0xA2`. The source now reproduces this order.
The other new text IDs follow the ordinary success path. Mission 23's
last two text IDs are 191 and 192; these values were copied directly
from the original table rather than inferred from adjacent IDs.

The source now selects 166 success pages for missions 1–23 from the
original 332-word prefix, with the original offsets, portraits, names
and mirror state. Later campaign-success missions still use a
placeholder table.

## Verification

- Data self-check: 140/140 added and 332/332 combined table words match
  the original ARM ELF. Source counts total 140 words for missions 14–23.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape rose from 92.7%
  to 93.5%; strict fuzzy changed from 37.8% to 36.7%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Live dialogue, sound and portrait rendering remain untested while
device work is deferred. Next offline block: missions 24–33; their 52
success pages use portrait descriptors already present in the source.
