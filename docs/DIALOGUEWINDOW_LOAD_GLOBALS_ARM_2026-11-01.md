# DialogueWindow loadContent resource owners (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. REA/IDA
instructions and pseudocode show calls through `Globals::gameText`,
`Globals::sound`, and `Globals::imageFactory`. The source previously used
three separate null file-local aliases for these resources. The source
now uses the observed shared globals. `Globals::imageFactory` has a
generic pointer type in the current declaration, so its call is cast to
`ImageFactory *`.

The original function contains substantial mission-specific text,
portrait, token replacement, and sound branches that are still absent
from the current source. This packet only restores the confirmed
resource owners.

## Verification

- Android ARMv7 build: 209/209 translation units, zero failures.
- Windows native and Android ARM64 `gof2` static builds: successful.
- Focused `game/ui/DialogueWindow`: 29 comparisons; average source-shape
  86.5% -> 86.6%, strict fuzzy 72.6% -> 72.7%. `loadContent` source-shape
  31.9% -> 33.3%, strict fuzzy 17.1% -> 18.2%. Exact counts remained
  11 linked and 4 byte.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape 95.95%,
  strict fuzzy 85.73%, 2,722 linked-exact and 1,091 byte-exact; zero
  exact-set losses relative to the previous full report.

No phone runtime check was performed. The default portrait description
and the remaining mission-specific branches need separate source-backed
recovery. The original `ImageFactory::loadChar` path is not established
by this packet beyond its resource owner.
