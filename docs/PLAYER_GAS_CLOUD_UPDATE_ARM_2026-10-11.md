# PlayerGasCloud particle update — Android ARM (2026-10-11)

The Android `PlayerGasCloud::update` at `0x1769b4` obtains its canvas,
sound, and item-definition table from `Globals`. Its campaign flag
accesses `0x218279`; the ELF symbol `Globals::hints` begins at
`0x21824c`, proving index `0x2d`. The source's separate campaign
pointer and other translation-unit-local null handles were not live
dependencies and have been removed. A compile-time assertion checks
the hint index against the declared array size.

ARM calls `Transform::Update(dt, true)` in both the model and shard
paths. Per-shard life decreases by `0.08f * dt`; collection needs
distance below `800.0f`, elapsed time at least `2000`, turret mode,
and timer at least `-1499`, then sets the timer to `-1500`. When
uncollected, timer-based scale divides by `1500.0f`; proximity fade
is `(distance - 800.0f) / 2700.0f` below `3500.0f`. The former
zero placeholders made several of these paths inert or undefined.

Successful pickup calls `Item::makeItem(1)` and adds that new object
to cargo. The former code discarded the new object and added the item
prototype. The HUD success call has its final aggregate flag set.
Homing uses normalized `(turretPosition - shardPosition)`, equipment
attribute `49`, and elapsed time to set the new position; the former
code scaled the absolute turret position. Sound `0x8d0` and the hint
flag use the shared globals.

Native `gof2` and all 206 ARM translation units build. The verifier's
36 tests pass (one skipped). Focused `update` source-shape improves
from 80.7% to 93.4%, strict fuzzy from 6.8% to 42.0%. Full ARM
corpus: 4,323/4,524 original functions compared, 92.14% average
source-shape, 80.38% strict fuzzy, 2,343 linked-exact, 1,008
byte-exact, zero skipped units and zero wrong-type symbols. Every
previously exact pair remains exact.

These are binary-shape and build checks, not a running-game test.
In particular, the `GameRecord` load path still uses an absolute
pointer overlay for a block overlapping `Globals::hints`; whether
save/reload preserves the new flag in the native build needs a
separate audit.
