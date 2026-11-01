# DialogueWindow constructor and drawing globals (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow(String*,String*,int*)` at `0x166fa0` and
`DialogueWindow::draw()` at `0x168198`. REA/IDA assembly and pseudocode
show the constructor using `Globals::imageFactory`,
`Globals::gameText`, and `Globals::layout`. `draw()` uses
`Globals::Canvas`, `Globals::layout`, `Globals::imageFactory`,
`Globals::is_choice_window_visible`,
`Globals::other_buttons_x/y`, and
`Globals::is_dialogue_window_visible`. The prior source used separate
null or zero file-local aliases for most of these. The recovered code
now uses the confirmed globals.

The original `draw()` copies `agentName` into a temporary `String` and
calls the six-argument `Layout::drawBox` overload. The source now
follows that call shape. Coordinates at indices 2 and 3 retain the
same button mapping as the original instructions.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, average source-shape
  90.3% -> 90.6%, strict fuzzy 78.9% -> 80.0%; 13 linked-exact and
  4 byte-exact unchanged. `draw()` rose from 66.0% to 75.7%
  source-shape and from 38.0% to 63.0% strict fuzzy. The two constructor
  variants rose from 42.7% to 46.1% strict fuzzy; their source-shape
  changed from 80.6% to 80.4%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape
  95.97% -> 95.98%, strict fuzzy 85.77% -> 85.78%, 2,724
  linked-exact and 1,091 byte-exact. Exact-set comparison found
  zero losses and zero gains.

Live rendering remains unverified while device work is deferred.
`DialogueWindow::loadContent()` still lacks several mission-specific
branches and data tables; that is the next offline priority.
