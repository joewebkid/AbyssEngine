# DialogueWindow type-12 success text (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. On the
non-campaign success path for mission type 12, `0x167840` selects text ID
370. For a story agent, `0x167850`-`0x16789e` builds a suffix from two
newlines and text ID 389 after replacing `#S` with the agent's station
name. Other agents receive an empty suffix. Instructions
`0x167b4c`-`0x167be2` append that suffix to text 370 and replace `#Q1`
with `level->killCountB` (offset `0x24`), then `#Q2` with
`level->killCountA` (offset `0x20`). The subsequent successful-station
override for station 108 remains in place.

The empty-string data at `0x1ca472` was checked directly against the
original ELF: its first byte is zero.

The source now uses this sequence, bypassing ordinary random text. The
same `Level` offsets have existing compile-time assertions in `Level.h`.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  51.2% to 66.9%; strict fuzzy declined from 23.7% to 22.9%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.98%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live dialogue rendering and replacement values remain untested while
device work is deferred. Other mission-specific and campaign text paths
in `loadContent()` remain incomplete.
