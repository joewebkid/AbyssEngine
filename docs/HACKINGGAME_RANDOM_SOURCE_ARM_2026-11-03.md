# HackingGame random source (Android ARMv7)

Reference: original Android 2.0.16 `libgof2hdaa.so`, SHA-256
`6d38fd767ee3b4f8174c371bfc6397cf76e13c1756938b2a94658e3b58`.

The original `HackingGame::reInit` at `0x14ed9c` reads `Globals::rnd`
for each `AERandom::nextInt` call while choosing and shuffling the
target tiles and choosing rotations. The reconstruction read
`AERandom::gRandom` instead. The current application initialization
assigns both globals the same generator, but the original call site
uses `Globals::rnd`. All six call sites in this method now use that
global. The focused ARM object has `Globals::rnd` relocations where
it previously had `AERandom::gRandom` relocations.

Focused ARMv7 verification builds 209/209 translation units and
compares 18 `HackingGame` symbols. `reInit` retains 99.4% source-shape,
96.4% strict fuzzy and 166 instructions against 166 original. The
unit retains 13 linked-exact and 10 byte-exact comparisons, with no
focused loss. Focused report:
`_work/hacking_rng_global_focused_20261103.json`.

Windows native and ARM64 static `gof2` builds pass. This verifies the
random source in code; the hacking minigame has not been exercised on
a permitted device. Redmi testing remains stopped.

The full ARM corpus compares 4,323/4,524 original functions in 4,769
pairs, with 201 missing and zero skipped. Mean source-shape remains
96.65%, mean strict fuzzy 86.60%, linked-exact 2,778 and byte-exact
1,107. No comparison pair or exact match was lost. Full report:
`_work/hacking_rng_global_full_20261103.json`.

Next priority: the remaining draw-call and resource-size sequence in
`HackingGame::render2D`; live graphics are still unverified.
