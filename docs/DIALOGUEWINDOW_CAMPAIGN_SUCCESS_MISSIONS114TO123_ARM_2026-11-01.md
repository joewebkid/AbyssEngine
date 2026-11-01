# DialogueWindow campaign success missions 114–123 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The success
path reads portrait/text pairs at `0x203ea0` using
`successOffsets[campaignMission] + page * 2`. Missions 114–123 add 136
words (68 pages); missions 120 and 123 have none. All 136 new words and
the 1562-word combined prefix were checked against the original ELF.

These pages use portrait indices 0, 4, 6, 20, 38, 40, 43 and 62; all
descriptors were restored in earlier packages. Mission 122 page 15 is
the original pair `(6, 2782)` at word index 1556. Original ARM control
flow checks whether the text ID is 2782 and the language is 1
(`0x1677b0`–`0x1677c2`). In this case it selects the zero portrait and
name text 1597 (`0x167b0e`–`0x167b1c`). The source now does the same;
the body text and sound lookup retain ID 2782, and the previously set
mirror state remains false for portrait 6. `GameText::setLanguage()`
maps language 1 to `de.lang`.

The source now selects 781 success pages for missions 1–123 from the
original table prefix. Later missions still use a placeholder table.

## Verification

- Data self-check: 136/136 added and 1562/1562 combined table words
  match the original ARM ELF; word index 1556 is `(6, 2782)`.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape rose from 94.3%
  to 95.1%; strict fuzzy changed from 36.7% to 36.5%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Live dialogue and localized portrait rendering remain untested while
device work is deferred. The remaining success rows for missions
124–161 contain 247 pages, use already restored portrait descriptors
and contain no further special text IDs; they form the next offline block.
