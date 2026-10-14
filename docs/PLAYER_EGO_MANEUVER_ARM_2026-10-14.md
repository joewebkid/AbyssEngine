# PlayerEgo maneuver target (2026-10-14)

Android `PlayerEgo::initManeuver(int)` at `0xa1abc` passes the word at
`0xa1b70` to `AEMath::operator*(float, const Vector&)`. The original
word is `0x461c4000`, exactly `10000.0f`. The previous source used
`0.0f`, making the type-3 strafe target equal to the ship position.
IDA and Ghidra independently confirm the call sequence: obtain player
position, obtain geometry direction, multiply direction by the literal,
subtract from position, assign target, obtain direction again, then
assign facing vector.

The source now uses the existing `AEMath` vector operators and
`Vector::operator=` rather than three declared-only `Vec_*` wrappers.
The production-body host smoke checks the 10,000-unit target, both
direction calls, the one-time maneuver gate and the separate
volatile-goods roll nudge. Native `gof2`, 206/206 ARM translation
units and 37 verifier tests (one skipped) pass. Focused source-shape
and strict fuzzy improve from **93.1%/78.5% → 99.4%/89.4%**; the
function is not linked- or byte-exact.

Full ARM report `_work/player_maneuver_full_20261014.json` compares
**4,323/4,524** original functions, at **93.30%** average
source-shape and **82.21%** strict fuzzy, with **2,482 linked-exact**,
**1,065 byte-exact**, **123 extras** and zero skipped units. Against
`_work/starsystem_flare_full_20261014.json`, exact-pair and extra-symbol
sets have no losses or gains. The full comparison ran with one worker
after an eight-worker attempt failed with Windows page-file error 1455;
the failed attempt produced no report.

Other `PlayerEgo` methods still reference declared-only `Vec_sub` and
`Vec_assign` functions, so this does not establish full executable
linkage. Live maneuver control, geometry scaling and ARM32 runtime
remain unverified. Next: resolve the remaining vector-wrapper call
sites with source-backed operators and exercise a complete maneuver.
