# DialogueWindow mission intro types 12 and 7 (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. In the
non-campaign kind-0 path, ARM instructions `0x1672f2`-`0x167308`
test mission type 12 and immediately load text ID 372. Instructions
`0x167558`-`0x16756e` then test type 7 and load ID 378. Both branches
go directly to `GameText::getText`; neither consumes a random choice
nor checks the special station before selecting the text.

The source previously drew a random generic text first, later
overrode type 12 with ID 372, and had no type-7 text selection.
It now selects IDs 372 and 378 before the generic branch and skips
the special-station override for those two types. This preserves the
observed random-generator state and text priority.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, average source-shape
  91.0% and strict fuzzy 80.3% at one decimal; 13 linked-exact and
  4 byte-exact unchanged. `loadContent()` improved from 33.5% to 34.4%
  source-shape and from 18.5% to 18.8% strict fuzzy.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape 95.98%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact.
  Exact-set comparison found zero losses and zero gains.

The special-station path uses two additional original text/image tables
with 18 and 6 pages; its current source remains incomplete. Other
mission-specific branches and campaign text are also unverified.
Device execution is deferred.
