# Ship-group random segment count (2026-11-05)

The original Android ARMv7 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`)
enters `Globals::getShipGroup(int,int,bool)` at `0xe4e14`.
In the variant-3 branch, instructions `0xe4f2a..0xe4f34` load
`Globals::rnd` through the GOT into `r0`, put bound `4` in `r1`,
and call `AERandom::nextInt(int)` at PLT `0x61848`. The resulting
segment count controls how many pairs of child transforms are added
to the ship group.

The prior ARM32 source instead passed `(0, 4)` to an undefined
`AERandom_nextIntB` helper. The recovered path now calls the typed
method on `Globals::rnd` on both pointer widths. This restores the
original owner pointer and removes the undefined helper name.

Focused ARMv7 compilation passes **209/209** translation units.
`getShipGroup` source-shape rises **83.3% -> 83.6%** and strict fuzzy
**11.3% -> 13.9%**; this 694-instruction original remains non-exact.
The raw unmatched-symbol inventory across the 209 objects falls
**253 -> 252**, removing only `AERandom_nextIntB(int,int)` with no
new name. Windows native and ARM64 static builds pass, as does strict
ARM64 shared linking. Device behavior remains unverified. Full ARMv7
corpus: **4,323/4,524** originals compared in **4,770** pairs,
**96.80%** mean source-shape, **86.80%** mean strict fuzzy,
**2,786 linked-exact**, **1,108 byte-exact**, 201 missing, zero
skipped, and zero wrong-type functions. The full linked-exact and
byte-exact member sets have no losses against the preceding report.
Full ARM32 shared linking remains unverified.
