# DialogueWindow init global resource owners (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::init()` at `0x166c58`. REA/IDA instructions
show `Globals::layout` for frame and button placement, `Globals::w` and
`Globals::h` for screen centering, and `Globals::gameText` for button
labels. The source previously used separate zero or null file-local
aliases. Those uses now read the confirmed shared globals.

The original function also reads two static mission count tables at
`0x2037b0` and `0x203a38`. The corresponding source arrays are still
zero placeholders and need a separately verified table mapping.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; average source-shape
  86.6% -> 86.7%, strict fuzzy 72.7% -> 72.9%. `init` source-shape
  89.8% -> 95.0%, strict fuzzy 40.4% -> 46.9%. Exact counts stayed at
  11 linked and 4 byte.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape 95.95%,
  strict fuzzy 85.73%, 2,722 linked-exact and 1,091 byte-exact; zero
  exact-set losses compared with the preceding full report.

No phone runtime check was performed. The remaining mission table data
and related dialogue behavior are the next offline priority.
