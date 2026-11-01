# DialogueWindow German generic text selection (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::pickGermanGenericTextBecauseWeSaved100EurosWithThat`
at `0x1680e0`. REA/IDA shows table bases `0x205fcc` for kind 0,
`0x20601c` for kind 2, and `0x20606c` for other nonzero kinds. Each
branch chooses one of two variants through `Globals::rnd`. Male variants
use `base + race * 8 + variant * 4`; female variants use
`base + 72 + variant * 4`.

The source previously read three separate zero-filled arrays and used
`AERandom::gRandom`. It now uses the confirmed shared random generator
and one contiguous table of 62 original 32-bit values. All 62 source
values were checked element by element against the ARM ELF bytes from
`0x205fcc`. The last two values are included because the original
function can read two entries after the third 20-word segment when
the clamped race index is 10. This preserves the observed address
calculation; the game-design intent of that cross-segment read is
not established.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; average source-shape
  90.6% -> 90.9%, strict fuzzy 80.0% -> 80.3%; 13 linked-exact and
  4 byte-exact unchanged. The German picker improved from 75.8% to
  84.8% source-shape and from 48.8% to 56.9% strict fuzzy.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape
  95.98%, strict fuzzy 85.78%, 2,724 linked-exact and 1,091
  byte-exact. Exact-set comparison found zero losses and zero gains.

Live German dialogue was not exercised while device work is deferred.
The ordinary non-German text-ID selection in `loadContent()` remains
incomplete and is the next offline priority.
