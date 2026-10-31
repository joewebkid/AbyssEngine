# DialogueWindow automatic page advance (Android ARM)

## Evidence and change

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::update(int)` at `0x167f94`. REA/IDA assembly
reads `byte_2181f6`, which is `Globals::options[0x0e]`, to enable automatic
page advance. It loads `Globals::sound` before both
`FModSound::getPlayingProgress` and `FModSound::isPlaying`.

The previous C++ instead read a file-local zero-initialized sound setting
and a null sound pointer. This disconnected the branch from loaded options
and could dereference null if the placeholder setting were enabled. The
source now uses the saved options byte and the real `Globals::sound`,
loading it at each call as in the original. The timing, page checks, and
timer update are unchanged.

## Verification

- Windows native and ARM64 `gof2` static targets: built successfully.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Focused `game/ui/DialogueWindow`: 29 comparisons, average source-shape
  85.9%, strict fuzzy 71.0%, 10 linked-exact and 4 byte-exact. `update`
  rose from 34.0% to 100% source-shape and from 13.1% to 100% strict
  fuzzy, gaining linked-exact with zero focused exact losses.
- Full corpus: 4,323/4,524 original functions covered by 4,752 comparison
  rows, zero skipped, 201 missing. Average source-shape rose 95.93% ->
  95.95%; strict fuzzy rose 85.70% -> 85.72%. Linked-exact rose 2,720 ->
  2,721, with only `DialogueWindow::update` gained and zero old matches
  lost. Byte-exact stayed at 1,091, with zero losses.

Live voice playback and page timing were not exercised. Other
`DialogueWindow` sound aliases remain separate work. The next priority
is to audit the adjacent voice start/stop paths against original ARM
without expanding this verified update packet.
