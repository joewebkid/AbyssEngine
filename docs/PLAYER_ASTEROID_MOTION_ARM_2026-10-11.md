# PlayerAsteroid projection, push, and blast force — Android ARM (2026-10-11)

The Android `initPush` routine multiplies `(1 - min(distance / duration, 1))`
by **5000**, not 1000. Its `push` routine divides the stored duration
by 5000 and multiplies the current geometry matrix on the left of the
temporary rotation matrix. Both values and the noncommutative order
were restored. The original `getProjectionVector` also updates the
class-static `pos` before computing `position - value`. This pass
restored the assignment but not the original in-place subtraction into
`pos`; the later
[projection follow-up](PLAYER_ASTEROID_PROJECTION_STATIC_ARM_2026-10-24.md)
corrects that remaining side effect.

For an exploding asteroid with positive bomb force, original ARM
clamps `scaling` to `[0.6, 1.0]`, computes
`float(int(bombForce * 1024 * (1 + clamped / -100)))`, and scales the
hit vector by that result. The recovered formula had used `0.1` and
usually rounded the displacement to zero. ARM also stores the scaled
vector in inherited `targetPos` (`0x90`) and dispatches the asteroid's
virtual `translate`, rather than calling `KIPlayer::translate` and
moving a possible route. Those behaviors are now restored.

Native `gof2` and 206/206 ARM translation units build; 36 verifier
tests pass (one skipped). The four-function focused family moves from
85.6% to 86.8% mean source-shape and 48.9% to 55.0% strict fuzzy.
None of these four functions is exact. The original assembly provides
the constants, call target, field offset, and matrix multiplication
order; game-level physics, projection, and collision remain untested.

The full ARM corpus still compares 4,323/4,524 original functions at
92.11% mean source-shape and 80.34% mean strict fuzzy. It retains
2,338 linked-exact and 1,008 byte-exact with no lost exact pair,
skipped unit, or wrong-type symbol. Three clamp cases were checked
independently (`scaling` 0.2/0.8/2.0 with unit bomb force yields integer
strength 1017/1015/1013). The next separate priority is the null
RNG/sound aliases in `PlayerJunk`.
