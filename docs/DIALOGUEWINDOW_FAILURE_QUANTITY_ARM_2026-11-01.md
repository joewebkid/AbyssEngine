# DialogueWindow failure quantities (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. The branch at
`0x16733e`-`0x1673e4` handles non-campaign dialogue whose kind is neither
0 nor 1 and whose mission type is 12. It selects text ID 371, replaces
`#Q1` with `level->killCountB` (offset `0x24`), then replaces `#Q2` with
`level->killCountA` (offset `0x20`). It assigns the resulting text without
an ordinary random text choice. Both `Level` offsets have existing
compile-time assertions in `Level.h`.

The source now follows that path using the existing three-argument
`Status::replaceHash`. It retains text ID 371 for subsequent dialogue
sound selection and avoids overwriting the replaced body text with a
generic line. The other dialogue kinds retain their previous selection.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  36.0% to 45.0%; strict fuzzy improved from 22.3% to 23.2%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.98%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live dialogue rendering and replacement values remain untested while
device work is deferred. Other mission-specific and campaign text paths
in `loadContent()` are still incomplete.
