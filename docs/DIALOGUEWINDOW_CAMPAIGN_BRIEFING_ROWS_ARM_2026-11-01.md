# DialogueWindow campaign briefing rows (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. On the
campaign briefing path, instructions `0x167284`-`0x16731a` select a
portrait/text pair using `briefingOffsets[campaignMission] + page * 2`
from the table at `0x203cc0`. Instructions `0x1673f0`-`0x16741c`
resolve the portrait descriptor, fetch the name at text ID
`0x63d + portraitId`, and mirror only portrait zero.

The source now contains all 120 words (60 pairs) of this table. All
120 values were checked against the original ELF. The 20 missions with
nonzero briefing counts use portrait IDs 0, 1, 2, 5, 6, 7, 16, 20, 24,
26, 31 and 49. Their pointer-table entries at `0x20e29c` were checked;
the 11 file-backed briefing descriptors and the previously restored
descriptor 4 all match their five original words. Portrait zero points
into zero-initialized `.bss`, whose runtime mutation is unverified.

The old source used a zero-filled placeholder for all campaign briefing
text IDs and derived the displayed name from the low byte of that text
ID. It now selects portrait, name, mirror state and body text from the
same original row. The existing special-station portrait 4 remains
mapped after enlarging the pointer table.

## Verification

- Data self-check: 120/120 briefing words, 12/12 file-backed portrait
  descriptors, and all 13 relevant pointer indices agree with the ELF.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  81.0% to 81.7%; strict fuzzy declined from 22.1% to 20.3%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Campaign briefing key-binding replacement and the special narration
for campaign IDs 38, 40 and 41 remain incomplete. The much larger
campaign success table is still a placeholder. Live dialogue and
portrait rendering remain untested while device work is deferred.
