# DialogueWindow successful station missions (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. On the
non-campaign success path (`kind == 1`), instructions `0x1676a0`-`0x167704`
select text ID 389 for mission types 5 and 3, then replace `#S` with
`Mission::getTargetStationName()`. The path bypasses ordinary random text.
After applying completed-mission standing at `0x167714`-`0x167720`, the
branch at `0x16772c` selects text ID 458 when the target station is 108,
overriding the earlier body text. The source now follows both branches.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape improved from
  45.0% to 51.2%; strict fuzzy improved from 23.2% to 23.7%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.98%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the previous report found zero losses and zero gains.

The type-12 success path and other mission-specific branches remain
incomplete. Live dialogue rendering remains untested while device work
is deferred.
