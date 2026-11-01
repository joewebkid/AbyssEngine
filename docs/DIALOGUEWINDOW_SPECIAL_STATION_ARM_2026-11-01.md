# DialogueWindow special-station pages and portraits (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. ARM instructions
`0x1677fc`-`0x167828` test target station `0x6c` after mission intro
types 12 and 7, then select a page table using
`Globals::status->field_114 == 2`. Instructions `0x167cdc`-`0x167d1c`
read the page's text ID and portrait index, assign body text, load a
portrait pointer from the table at `0x20e29c`, and fetch the agent
name at text ID `0x63d + portraitIndex`. No generic random choice is
made on this path.

The status-2 table at `0x205ed0` contains 18 pairs of portrait index
and text ID. The other table at `0x205f60` contains six pairs. All 48
source words were checked against the original ARM ELF. The only
portrait indices used are 0, 4, and 16. The original pointer table
maps them to `0x2183c0`, `0x216438`, and `0x216528`. The latter two
five-word descriptors were checked against the ELF data. The first
points into zero-initialized `.bss`; REA reported only read-side
references, but runtime mutation of that storage remains unverified.

The source previously consumed a generic random choice, then replaced
the text with fixed ID 458 while leaving the portrait and name from
the earlier page-parity path. It now uses the page pair for text,
portrait and name, and bypasses the random choice. `length()` now
reads the same ARM-confirmed `Globals::status` owner as `loadContent()`.

## Verification

- Table self-check: 36/36 status-2 words, 12/12 default words, and
  both five-word portrait descriptors match the ARM file. The three
  pointer-table entries match their original addresses.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; average source-shape
  91.0% at one decimal, strict fuzzy 80.3% -> 80.4%; 13 linked-exact
  and 4 byte-exact unchanged. `loadContent()` improved from 34.4% to
  36.0% source-shape and from 18.8% to 22.3% strict fuzzy.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape 95.98%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact.
  Exact-set comparison found zero losses and zero gains.

Live dialogue rendering, sound and portrait selection remain untested
while device work is deferred. Campaign text tables and other
mission-specific branches in `loadContent()` are still incomplete.
