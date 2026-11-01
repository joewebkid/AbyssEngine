# DialogueWindow campaign binding-helper call (Android ARM)

## Evidence and correction

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. On the
campaign briefing path, `0x16759c` checks the text-table word index:
bits `0xA0200A` for indices at most 23, plus index 35. These are
indices 1, 3, 13, 21, 23 and 35, corresponding to text IDs 1677,
1680, 1725, 1750, 1753 and 1878. Instructions `0x167468`-`0x16747e`
pass those six texts through `Globals::replaceKeyBindingTokens()`.

The helper itself was also inspected in the original Android ARM
binary at `0xe8c72`: it constructs and returns a copy of the input
string. `Globals::getKeyActionName()` at `0xe8c68` returns an empty
string. The existing source implementations already have those
behaviors. The earlier project-status claim that the helper was an
unrecovered substitution routine was incorrect. This change restores
the six original call sites; it does not claim visible key-label
substitution in the Android build.

## Verification

- ARM selection self-check: the six indices map to the six text IDs
  above in the restored 120-word briefing table.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  89.0% to 90.6%; strict fuzzy declined from 36.9% to 36.5%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live dialogue remains untested while device work is deferred. The
campaign success table and its special text behavior remain incomplete.
