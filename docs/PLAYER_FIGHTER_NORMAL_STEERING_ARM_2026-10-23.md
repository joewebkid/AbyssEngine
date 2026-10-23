# Fighter state-1/state-7 normal steering (2026-10-23)

Primary evidence: Android 2.0.16 `libgof2hdaa.so` (SHA-256
`6d38fd767ee3b4f8174c371bce371bfc6397cf76e13c1756938b2a94658e3b58`),
`PlayerFighter::update(int)` at `0xded6e–0xdf03e`. The local recovered
IDA `PlayerFighter.c` at lines 2407–2507 independently supports the
control-flow reading, but is not treated as original C++ source.

The common direction prelude retains a normalized world-space target
before writing a ship-local direction to Fighter `+0x170`. On the normal
path (maneuver flag `+0x12d` clear), the original takes the geometry
matrix's current direction, normalizes the difference toward the world
target, scales it by `float(dt * 48) / 65536`, adds it to the current
direction and normalizes the result back into `+0x170`. If the
component-wise absolute error from the world target sums to less than
**0.0625**, it snaps exactly to that target. This update recovers the
direction state, not its later application to the geometry matrix.

The angle comes from the dot product of normalized old and new
directions. The original calls `ACosf` only for a dot strictly between
−1 and +1; otherwise the angle is zero. A right-vector dot/`ACosf`
test against **1.5707964f** selects the sign. The angle is written to
one of five float samples at Fighter `+0x29c–0x2ac`. A filled buffer
averages all five after writing. Before filling, index zero uses the
new angle directly, while later indices average only the *previous*
samples, excluding the just-written slot. Index four wraps to zero and
marks the buffer filled. The result is multiplied by the float values
at `+0x294` (**750.0f**) and `+0x298` (**15.139357f**, ARM bits
`0x41723ACE`), then clamped to ±750 for target roll `+0x20c`.

The two scale fields and five sample fields were previously typed as
integers. They are now floats with unchanged ARM offsets and storage
size. The already-recovered roll invocation, smoothing and timer now
run after either normal steering or maneuver bypass, matching the
shared join at `0xdf042`.

The production-method smoke checks a turn toward a perpendicular
waypoint, negative/positive roll signs, the partial-buffer one-frame
lag, five-sample wrap/fill, and the strict snap branch at `-O0/-O2`.
All 11 Fighter smokes, native UCRT64 `gof2`, 208 NDK r18b ARM
translation units, and 93 verifier tests pass (one existing skip).
Focused comparison of 44 Fighter pairs changes whole-`update`
source-shape/strict fuzzy **84.4%/34.0% → 87.8%/35.3%**; only the
target function's score changes. All **21 linked-exact** and
**10 byte-exact** Fighter pairs are retained.

The full ARM corpus compares **4,323/4,524** original symbols across
**4,735** pairs, with **201** originals still absent and no skipped or
wrong-type symbols. Rounded corpus means remain **94.05%** source-shape
and **83.38%** strict fuzzy. All **2,557 linked-exact** and **1,082
byte-exact** pairs are retained: pairwise comparison against the
preceding checkpoint found no missing pair, exact loss or score change
outside `PlayerFighter::update`.

The shooting branch at `0xdef12`, normal-path construction of the
orthogonal geometry matrix at `0xdefd8–0xdf03e`, and subsequent
matrix application, translation, particles and landmarks remain
unrecovered. `PF_update_body` still marks that boundary. Full
game-frame steering is not validated; completing matrix application
and translation is the next priority.
