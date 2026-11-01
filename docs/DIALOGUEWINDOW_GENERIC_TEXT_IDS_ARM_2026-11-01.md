# DialogueWindow ordinary generic text IDs (Android ARM)

## Evidence and change

Original Android `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
implements `DialogueWindow::loadContent()` at `0x1671f4`. REA/IDA
instructions and pseudocode show three five-ID tables for ordinary
non-German generic dialogue: kind 0 at `0x205f90`, kind 1 at
`0x205fa4`, and kind 2 at `0x205fb8`. Each path calls
`AERandom::nextInt(5)` through `Globals::rnd`. All 15 source values
were checked element by element against the corresponding ARM ELF
bytes.

The source previously used `0x188 + AERandom::gRandom->nextInt(5)`
for all three kinds, selecting IDs 392-396. The ordinary path now
selects 379-383 for kind 0, 373-377 for kind 1, and 384-388 for
kind 2, using the confirmed shared random generator. This also
passes the selected ID to the existing dialogue-sound lookup.

## Verification

- Android ARMv7: 209/209 translation units compiled, zero failures.
- Windows native and Android ARM64 `gof2` static targets: built.
- Focused `game/ui/DialogueWindow`: 29 comparisons, average source-shape
  90.9% -> 91.0%, strict fuzzy 80.3% at one decimal; 13 linked-exact
  and 4 byte-exact unchanged. `loadContent()` source-shape improved
  33.3% -> 33.5%, strict fuzzy 18.2% -> 18.5%.
- Full ARM corpus: 4,323/4,524 original functions covered by 4,752
  comparisons, zero skipped, 201 missing. Average source-shape 95.98%,
  strict fuzzy 85.78%, 2,724 linked-exact and 1,091 byte-exact.
  Exact-set comparison found zero losses and zero gains.

Special mission types and campaign dialogue still have incomplete
branches and tables in `loadContent()`. This packet establishes only
the ordinary non-German five-choice selection. Live dialogue and
sound effects were not exercised while device work is deferred.
