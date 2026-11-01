# DialogueWindow campaign narration (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. For campaign
dialogue kinds other than 0 and 1, the branch at `0x1673ea` chooses
portrait index 16. Its name is text ID `0x63d + 16` (1613). The body
is built at `0x167602`-`0x167654` from text ID 392, a newline, an
optional text ID 527, two newlines, and text ID 319. Instructions
`0x1674fc`-`0x16750e` include text 527 only when the campaign mission ID
is 38, 40 or 41. The branch sets the dialogue sound text ID to `-1` at
`0x16767a`.

The source previously displayed text 1613 as the body, derived the
name from its low byte, used the default portrait, and passed 1613
to sound selection. It now uses the original portrait, constructs the
narration text, and passes `-1` for sound selection. The briefing and
success paths keep their separate behavior.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  81.7% to 89.0%; strict fuzzy improved from 20.3% to 36.9%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

Live text and portrait rendering remain untested while device work is
deferred. Campaign briefing key-binding replacement and the campaign
success table remain incomplete.
