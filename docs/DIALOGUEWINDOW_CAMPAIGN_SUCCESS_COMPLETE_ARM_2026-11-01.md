# DialogueWindow complete campaign success table (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. Its campaign
success path indexes portrait/text pairs at `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. The 162 mission counts in
the source sum to 2056 words, or 1028 pages. Missions 124–161 contribute
the final 494 words (247 pages). All 494 new words and the complete
2056-word table were checked against the original ELF. All portrait
indices in the table are within the restored 0–62 pointer range. The
new rows contain no further special IDs 1718, 1730, 1833, 2193–2194
or 2782.

The source now uses the complete table directly for every campaign
success page. The temporary zero-filled success table and the range
split at mission 123 were removed. Original ARM control flow reads the
table without that split. Earlier packages restored the corresponding
portrait values and special text paths.

This finishes recovery of the **campaign success row data**. It does
not establish that every `DialogueWindow` behavior or the overall
decompilation is complete.

## Verification

- Data self-check: 494/494 added and 2056/2056 total table words match
  the original ARM ELF; 162 source counts sum to 2056 words; all 1028
  portrait indices are in the 0–62 pointer range.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape changed from
  95.1% to 93.5%; strict fuzzy changed from 36.5% to 36.4%. The score
  decrease follows removal of the incorrect placeholder branch; the
  restored direct table path matches the ARM data and control flow.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Live campaign dialogue and portrait rendering remain untested while
device work is deferred. The next offline priority is a focused review
of the remaining `DialogueWindow::loadContent()` control flow and its
non-campaign branches against ARM, including the `.bss` portrait zero
runtime state.
