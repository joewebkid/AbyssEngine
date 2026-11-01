# DialogueWindow non-campaign failure 42 and dialogue font (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`.

After `Mission::isCampaignMission()` returns false, original ARM control
flow checks `kind == 2` and `campaignMission == 42` at `0x167290`–
`0x167298`. When true, it enters the campaign failure path at
`0x1673ea`: portrait index 16, name text 1613, and the failure body
built from text IDs 392 and 319. The source previously treated this as
an ordinary mission failure. It now routes this one case through the
campaign failure path. Its `length()` remains one page for this
non-campaign mission.

Before setting the scroll text, the original tests the selected
portrait index at `0x167a38`–`0x167a44`: indices 19 and 56 select
`Globals::fontAlien`; other paths select `Globals::font`. The chosen
font is passed to `ScrollTouchWindow::setText()` at `0x167a4e`.
`ScrollTouchBox::setText()` stores that third argument as the font
handle. The source previously passed zero for every dialogue. It now
tracks the campaign portrait index and forwards the original font
choice. The complete success table uses portrait 56 on five pages.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` source-shape rose from 93.5%
  to 94.9%; strict fuzzy changed from 36.4% to 29.4%. The latter score
  is recorded without reverting source-backed behavior.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped and 201 missing. Average source-shape 95.99%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact. Exact-set
  comparison against the preceding report found zero losses and gains.

Live failure dialogue and font rendering remain untested while device
work is deferred. The next offline priority is the remaining ordinary
mission success and failure control flow in `loadContent()`.
