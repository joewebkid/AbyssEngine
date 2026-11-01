# DialogueWindow touch-end sound and choice flow (Android ARM)

## Evidence and change

The original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::OnTouchEnd(int,int)` at `0x1683c8`.
REA/IDA assembly uses `Globals::sound` for the mission-15 music change,
voice stop, and previous/next page stops. The additional-choice text
comes from `Globals::gameText`. The prior source used separate null
file-local pointers for all five accesses. Those calls now use the
confirmed shared globals.

The original first separates choice result `1`, then handles result `0`
and other nonzero results. The source follows that order. The original
also retains `choiceWindow` before looking up the additional-choice
text; the source now does the same. These changes preserve the input
conditions and return values while restoring the observed call order.

## Verification

- Windows native and ARM64 `gof2` static targets: built successfully.
- Android ARMv7: 209/209 translation units compiled, zero failures.
- Focused `game/ui/DialogueWindow`: 29 comparisons, average source-shape
  86.5%, strict fuzzy 72.6%, 11 linked-exact and 4 byte-exact.
  `OnTouchEnd` rose from 81.6% to 100% source-shape and from 51.5% to
  100% strict fuzzy, gaining linked-exact with zero focused exact losses.
- Full corpus: 4,323/4,524 original functions covered by 4,752 comparison
  rows, zero skipped, 201 missing. Average source-shape remained 95.95%
  at two decimals; strict fuzzy rose 85.72% -> 85.73%. Linked-exact rose
  2,721 -> 2,722, with only `OnTouchEnd` gained and zero old matches lost.
  Byte-exact stayed at 1,091, with zero losses.

Live touch input and sound playback were not exercised. `loadContent`
still contains separate file-local placeholders for text, sound, and
image factory; the next priority is to identify their original owners
and restore that path in a bounded packet.
