# Level procedural route generation (2026-10-24)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`Level::createRoute(int)` at `0xc0ce0`, cross-checked against the IDA
function dump at the same address. The original calls
`AERandom::nextInt(30000)` for **both** the first waypoint's `z` at
`0xc0d7a` and subsequent relative `z` values at `0xc0d60`.
The reconstruction called the unbounded `nextInt()` in those two
branches. That could move procedural mission waypoints far beyond the
original route's intended range. The first `z` is now `50000..79999`;
each later `z` adds `50000..79999` to the previous one. The existing
`x` and `y` draws and draw order are unchanged.

The original exception edge at `0xc0db2..0xc0dbc` deletes the allocated
`Route` if construction throws. The prior manual allocation plus
placement construction omitted that cleanup. Ordinary `new Route(pts,n)`
restores it and uses the 24-byte ARM layout already asserted in `Route.h`.
The coordinate-array ownership and any failure before `Route` allocation
are outside this change.

The two callers are the procedural player-route paths in `Level` mission
cases 4 and 12. Focused NDK r18b ARM `-Oz` verification moves from
**88.3% source-shape / 93.0% strict fuzzy** to **100.0% / 98.8%**.
Both original and built functions have 83 instructions and matching call
count/control flow; the remaining nonexact difference is the order of
two loop increments. Native `gof2` builds, all 208 ARM translation units
build, and 93 verifier tests pass (one skip). This evidence establishes
the bounded draws and cleanup shape, not a live mission route test.

The full verifier run compares 4,323/4,524 originals across 4,736 pairs,
with 201 missing and zero skipped. Mean source-shape/strict fuzzy stays
**94.14%/83.47%** at the displayed precision. The **2,560 linked-exact**
and **1,082 byte-exact** pair sets are unchanged, with no losses. The full
report SHA-256 is
`6c69468e4b6f1ebbc7964684ff2adc9aad7ed98371cb6b15edc04f1e8ef48ed9`.
ADB currently lists no device; procedural mission waypoints remain
unverified in a live Android scene. The first-friend Pan ship-insertion
gate remains the next native runtime priority.
