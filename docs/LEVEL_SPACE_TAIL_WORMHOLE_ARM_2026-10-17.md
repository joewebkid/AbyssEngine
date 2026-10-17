# Level alien exit, wormhole and entry angle (2026-10-17)

Primary evidence is Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`).
`Level::createSpace` at `0xae5b8` supplies the wormhole visibility condition
and sound 34 at `0xaeb2a`–`0xaeb62`, moves the alien-orbit exit gate and
faces it toward the origin at `0xaefc8`–`0xaf054`, creates slot-3
`PlayerWormHole` if `!Status::gameWon()` at `0xaf05c`–`0xaf104`, and
finishes `field_138` at `0xaf116`–`0xaf138`. The source now implements those
branches. In particular, slot 3 is absent after victory; otherwise the
wormhole uses mesh `0x4262`, player ID 16994 and three RNG offsets, then
receives its owning `Level`.

`PlayerWormHole` constructor at `0xa526a`–`0xa52c8` reads
`Globals::gameText` and `Globals::Canvas`. Its update at
`0xa53a8`–`0xa5754` reads the canvas, status and RNG from `Globals`, and
calls `Level::getPlayer` directly. The file-local null placeholders for
those sources have been removed. The Android JNI path now assigns the
created text object to `Globals::gameText` alongside
`GameText::gGameText`; this alias assignment is an integration repair,
not a claim of exact source shape for the JNI function.

`tools/run_level_space_tail_smoke.py` executes the extracted tail at `-O0`
and `-O2` with controlled RNG and test doubles. Ordinary, attacked-station,
alien, victorious, visible/hidden wormhole and entry-angle paths pass.
The same smoke extracts the wormhole update's random-placement block and
checks that magnitudes and signs consume RNG values in the ARM-observed
order for normal and alien orbits.
The earlier station and jumpgate smokes also pass. Native `gof2`, 208/208
ARM translation units and 40 verifier tests pass (one existing skip).
These tests do not run a complete `Level::init` with game resources.

Focused `Level::createSpace` source-shape/strict fuzzy is
55.0%/11.3% before and 67.2%/32.3% after. Focused
`PlayerWormHole::update` is 62.4%/32.5% before and 95.6%/53.6% after;
constructor C1/C2 is 89.6%/74.7% before and 97.1%/93.6% after.
None of these functions is exact. Reports live under
`A:/Galaxy on Fire new version/_work/`: `level_space_tail_before_20261017.json`,
`level_space_tail_after_20261017.json`, `wormhole_before_20261017.json`,
and `wormhole_after_20261017.json`.

The full ARM report
`A:/Galaxy on Fire new version/_work/level_space_tail_final_full_20261017.json`
compares 4,323/4,524 original functions in 4,730 rows (201 missing,
zero skipped or wrong-type). Mean source-shape rises 93.84% → 93.86%
and strict fuzzy 83.02% → 83.03%. Linked-exact and byte-exact remain
2,528 and 1,076; pairwise comparison against
`level_jumpgate_full_20261017.json` found zero exact-pair losses or gains.

Still open: remaining skybox/resource operations in `createSpace`, the
special stream-out placement in `Level::init`, and a real asset-backed
stage-0/1 runtime test. Real JNI startup and `GameText` resource loading
remain untested; the alias assignment alone does not establish a working
Android startup. These are the next integration priority.
